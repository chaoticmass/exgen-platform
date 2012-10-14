#define VERSION_STRING	"0.298A"
#define ARCH_STRING	"linux_x86"

#include "rote/rote.h"
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include "pthread.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <sqlite3.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/wait.h>

char *self_name = NULL; // despite what the name may indicate, this contains
			// the entire path and program name as returned by
			// argv[0]

char *self_nameonly = NULL; // contains just the filename part of self_name

// name of extracted embedded data file
char em_tmpname[64];
char em_tmpdir[64];
int em_embedded = 0; // equals 1 if data exists and was extracted

void * mod_load(char *);
int * mod_loader(char *, int);
void * mod_unload(char *);
//void * mod_unregister(char *);
char * zopenfile(char *);
char * zextractfile(char *, char *);
void *lua_runscript_reuse (void *, char *);
void *lua_runstring_reuse (void *, char *);
void *lua_runscript_thread_reuse (void *, char *);
void *lua_runstring_thread_reuse (void *, char *);
char * openfile(char *);
char * findfile(char ** );
char * GetVar(char *, char *);

lua_State * DB;

pthread_mutex_t db_mutex = PTHREAD_MUTEX_INITIALIZER;
int db_locked = 0;

int modulecount = 0;
struct moduledata {
    char *name;
    char *file;
    char *version;
    void *handle;
    int unload;
    double (*lua_register_call)(void * L);
};
struct moduledata **modules = NULL;

struct luathread_data {
    lua_State * L;
    char *file;
};


int mod_failed = 0;
char * coremsg = NULL;
int cmsg_len = 0;

char * path;

// sock functions
// s_ prefix
// functions pertaining to network/sockets

char * s_resolv(char * hostname) {
    char * ip = NULL;
    struct hostent *h;
    h = gethostbyname(hostname);
    ip = inet_ntoa(*((struct in_addr *)h->h_addr));
    return ip;
}

int s_connect(int port, char * ip) { // PORT, IP
    int sockfd;
    struct sockaddr_in dest_addr;
    
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    //sockfd = socket(PF_INET, SOCK_SEQPACKET, 0);
    
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr.s_addr = inet_addr(ip);
    memset(dest_addr.sin_zero, "\0", sizeof(dest_addr.sin_zero));
    
    connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    return sockfd;
}

int s_listen(int port, char * ip) { // PORT, IP
    int sockfd;
    struct sockaddr_in my_addr;
    //char *ip = NULL;
    int ipi = 0;
    
    if (strcmp(ip, "") == 0) {
	ipi = INADDR_ANY;
    } else {
	ipi = inet_addr(ip);
    }
    //sockfd = socket(PF_INET, SOCK_SEQPACKET, 0);
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = ipi;
    memset(my_addr.sin_zero, "\0", sizeof(my_addr.sin_zero));
    
    bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
    
    listen(sockfd, 5);
    
    return sockfd;
}


char * s_mrecv(int sockfd) { // sockfd
    char buf[512] = "";
    char len[512] = "";    
    char signal[512] = "";    
    int datasize = 511;
    int numbytes = 0;
    int messagelength;
    char *data = NULL;

    // get signal
    //printf("file: %i\n", sockfd);
    numbytes = recv(sockfd, len, datasize, 0);
    if (numbytes <= 0) {
	perror("recv");
	return NULL;
    }
    //len[numbytes] = 0;
    //printf("numbytes: %i, rec'd: %s\n", numbytes, len);
    // send size
    send(sockfd, len, strlen(len), 0);
    
/*    while (strcmp(signal, len) != 0) {
	// get signal
	printf("get size\n");
	recv(sockfd, signal, datasize, 0);
    
	// send size
	printf("send size\n");
	send(sockfd, len, strlen(len), 0);
    
	// get size
	printf("get size\n");
	recv(sockfd, len, datasize, 0);
    } */

    messagelength = atoi(len);

    data = calloc(messagelength + 1, sizeof(char));
    do  {	
	numbytes = recv(sockfd, buf, datasize, 0);
	buf[numbytes] = 0;
	data = strcat(data, buf);
    } while (messagelength > strlen(data));
    
    // send size of data received
    sprintf(len, "%i", strlen(data));
    send(sockfd, len, strlen(len), 0);    
    
    return data;
}

int s_msend(int sockfd, char * send_string) { // sockfd, string
    int total = 0;
    int bytesleft = 0;
    int start = 0;
    int n;
    char ack[512] = "";
    int datasize = 511;
    char len[512];    
    
    sprintf(len, "%i", strlen(send_string));
    
    do  {
	// send length of message	
	send(sockfd, len, strlen(len), 0);
	
	// recv back length of message
	recv(sockfd, ack, datasize, 0);
    } while (strcmp(ack, len) != 0);
    
    // send data
    bytesleft = strlen(send_string);
    start = bytesleft;
    
    while (total < start) {
	n = send(sockfd, send_string+total, bytesleft, 0);
	total += n;
	bytesleft -= n;
    }

    // wait for signal to continue
    recv(sockfd, ack, datasize, 0);
    
    if (strcmp(ack, len) == 0) {
	return 0;
    } else {
	return -1;
    }
}

int s_downloadfile(char *hostname, char *url, char *output_file) {
    char buf[16384] = "";
    char len[16384] = "";    
    char signal[16384] = "";    
    int datasize = 16 * 1024;
    int numbytes = 1;
    int totalbytes = 0;
    int messagelength;
    char *data = NULL;
    char *head = NULL;
    int headlen = 0;
    char *tmpdata = NULL;
    int sockfd = 0;
    char *ip = NULL;
    int retval = 0;
    char *token = NULL;
    char *contentlength = NULL;
    int clength = 0;
    int i = 0;
    char *content = NULL;

    
    asprintf(&ip, "%s", s_resolv(hostname));
    sockfd = s_connect(80, ip);
    if (sockfd != -1) {
	//snprintf(signal, 512, "HEAD %s HTTP/1.0\n\n", url);
	snprintf(signal, 512, "HEAD %s HTTP/1.0\nHost: %s\n\n", url, hostname);
	//printf("%s \n", signal);
	retval = send(sockfd, signal, strlen(signal), 0);
	if (retval != -1) {
	    //printf("connected\n");
	    // get the HEAD
	    do  {		    
		numbytes = recv(sockfd, buf, datasize, 0);
		buf[numbytes] = 0;
		tmpdata = data;
		asprintf(&data, "%s%s", data, buf);
		free(tmpdata); tmpdata = NULL;	    
		totalbytes += numbytes;
	    } while (numbytes > 0);
	    asprintf(&head, "%s", data);
	    free(data); data = NULL;
	    headlen = strlen(head) - 5;
	    totalbytes = 0;
	    
	    // find content length
	    token = strstr(head, "Content-Length: ");
	    for(i = 0; i < strlen(token); i += 1) {
		if (token[i] == 13) {
		    token[i] = 0;
		    break;
		}
	    }
	    for(i = 0; i < strlen(token); i += 1) {
		if (token[i] == 32) {
		    contentlength = &token[i + 1];
		    break;
		}
	    }	    
	    clength = atoi(contentlength);
	    close(sockfd);
	    
	    //printf("clength = %i\n", clength);
	    
	    sockfd = s_connect(80, ip);
	    //snprintf(signal, 512, "GET %s HTTP/1.0\n\n", url);
	    snprintf(signal, 512, "GET %s HTTP/1.0\nHost: %s\n\n", url, hostname);
	    retval = send(sockfd, signal, strlen(signal), 0);
	    numbytes = 1;
	    content = malloc(clength + headlen + 6);
	    FILE *h = fopen(output_file, "w");	
	    if (h == NULL) {
		return -1;
	    }
	    // discard the header
	    do {
		numbytes = read(sockfd, buf, 1);
		//buf[numbytes] = 0;
		//printf("%s", buf);
		totalbytes += numbytes;
	    } while (totalbytes < headlen);
	    
	    // get the actual file we want
	    totalbytes = 0;
	    do {
		numbytes = read(sockfd, buf, datasize);
		fwrite(buf, numbytes, 1, h);
		totalbytes += numbytes;
		printf("got %i, total %i, need: %i\n", numbytes, totalbytes, clength);
	    } while (numbytes != 0);
	    //} while (totalbytes < clength);
	    fclose(h);	    	    
	    
	    return 0;
	} else {
	}
    } else {
    }
    return -1;
}

int s_downloadfile_post(char *hostname, char *url,  char *post_string, char *output_file) {
    char buf[16384] = "";
    char len[16384] = "";    
    char *signal = NULL;
    int datasize = 16 * 1024;
    int numbytes = 1;
    int totalbytes = 0;
    int messagelength;
    char *data = NULL;
    char *head = NULL;
    int headlen = 0;
    char *tmpdata = NULL;
    int sockfd = 0;
    char *ip = NULL;
    int retval = 0;
    char *token = NULL;
    char *contentlength = NULL;
    int clength = 0;
    int i = 0;
    char *content = NULL;

    
    asprintf(&ip, "%s", s_resolv(hostname));
    sockfd = s_connect(80, ip);
    if (sockfd != -1) {
	//snprintf(signal, 512, "HEAD %s HTTP/1.0\n\n", url);
	//printf("%s \n", signal);
	asprintf(&signal, "POST %s HTTP/1.0\nHost: %s\nUser-Agent: exgen-%s/%s\nContent-Length: %i\nContent-Type: application/x-www-form-urlencoded\n\n%s", 
	    url, 
	    hostname, 
	    GetVar("configuration", "arch"), 
	    GetVar("configuration", "version"),
	    strlen(post_string),
	    post_string
	);
	//printf("%s\n", signal);
	retval = send(sockfd, signal, strlen(signal), 0);
	if (retval != -1) {
	    
	    retval = send(sockfd, signal, strlen(signal), 0);
	    numbytes = 1;
	    FILE *h = fopen(output_file, "w");	
	    if (h == NULL) {
		return -1;
	    }
	    // discard the header
	    // find two consecutive crlf's
	    // I know this looks like a brain-dead way to do this
	    // but it works just fine, works simply and quickly
	    do {
		numbytes = read(sockfd, buf, 1); buf[numbytes] = 0; totalbytes += numbytes;
		if (buf[0] == 13) {
		    numbytes = read(sockfd, buf, 1); buf[numbytes] = 0; totalbytes += numbytes;
		    if (buf[0] == 10) {
			numbytes = read(sockfd, buf, 1); buf[numbytes] = 0; totalbytes += numbytes;
			if (buf[0] == 13) {
			    numbytes = read(sockfd, buf, 1); buf[numbytes] = 0; totalbytes += numbytes;
			    break;
			    
			    // it doesn't look like this part will ever run.
			    // should be safe to remove it
			    /*if (buf[0] == 10) {
				numbytes = read(sockfd, buf, 1); buf[numbytes] = 0; totalbytes += numbytes;
				break;
			    }*/
			}
		    }
		}
	    } while (1 == 1);
	    
	    // get the actual file we want
	    totalbytes = 0;
	    do {
		numbytes = read(sockfd, buf, datasize);
		fwrite(buf, numbytes, 1, h);
		totalbytes += numbytes;
		//printf("got %i, total %i, need: %i\n", numbytes, totalbytes, clength);
	    } while (numbytes != 0);
	    //} while (totalbytes < clength);
	    fclose(h);	    	    
	    
	    return 0;
	} else {
	}
    } else {
    }
    return -1;
}


int s_old_downloadstring(char *hostname, char *url, char *output_string) {
    char buf[16384] = "";
    char len[16384] = "";    
    char signal[16384] = "";    
    int datasize = 16 * 1024;
    int numbytes = 1;
    int totalbytes = 0;
    int messagelength;
    char *data = NULL;
    char *head = NULL;
    int headlen = 0;
    char *tmpdata = NULL;
    int sockfd = 0;
    char *ip = NULL;
    int retval = 0;
    char *token = NULL;
    char *contentlength = NULL;
    int clength = 0;
    int i = 0;
    char *content = NULL;
    int magic_num = 6; // magic
    
    printf("fuckshit\n");
    asprintf(&ip, "%s", s_resolv(hostname));
    printf("downloading url %s from %s\n", url, ip);
    sockfd = s_connect(80, ip);
    if (sockfd != -1) {	
	snprintf(signal, 512, "HEAD %s HTTP/1.0\nHost: %s\n\n", url, hostname);
	//printf("%s \n", signal);
	retval = send(sockfd, signal, strlen(signal), 0);
	if (retval != -1) {
	    //printf("connected\n");
	    // get the HEAD
	    do  {		    
		numbytes = recv(sockfd, buf, datasize, 0);
		buf[numbytes] = 0;
		tmpdata = data;
		asprintf(&data, "%s%s", data, buf);
		free(tmpdata); tmpdata = NULL;	    
	    } while (numbytes > 0);
	    asprintf(&head, "%s", data);
	    free(data); data = NULL;
	    //headlen = strlen(head) - magic_num; //  why the magic number?
	    headlen = strlen(head);
	    printf("head: %s\n", head);
	    printf("headlen = %i\n", headlen);
	    
	    // find content length
	    token = strstr(head, "Content-Length: ");
	    for(i = 0; i < strlen(token); i += 1) {
		if (token[i] == 13) {
		    token[i] = 0;
		    break;
		}
	    }
	    for(i = 0; i < strlen(token); i += 1) {
		if (token[i] == 32) {
		    contentlength = &token[i + 1];
		    break;
		}
	    }	    
	    clength = atoi(contentlength);
	    close(sockfd);
	    
	    printf("clength = %i\n", clength);
	    
	    sockfd = s_connect(80, ip);
	    //snprintf(signal, 512, "GET %s HTTP/1.0\n\n", url);
	    snprintf(signal, 512, "GET %s HTTP/1.0\nHost: %s\n\n", url, hostname);
	    retval = send(sockfd, signal, strlen(signal), 0);
	    numbytes = 1;
	    content = malloc(clength + headlen + magic_num);

	    //printf("get the contents\n");
	    // get the actual file we want

	    do  {		    
		numbytes = recv(sockfd, buf, datasize, 0);
		totalbytes += numbytes;
		buf[numbytes] = 0;
		tmpdata = data;
		asprintf(&data, "%s%s", data, buf);
		free(tmpdata); tmpdata = NULL;	    
	    } while (numbytes > 0);
	        
	    tmpdata = &data[strlen(data) - clength];
	    strcpy(output_string, tmpdata);
	    free(data); data = NULL;
	    //printf("recd  \"%s\"\n", output_string);
	    return 0;
	} else {
	}
    } else {
    }
    return -1;
}

int s_downloadstring(char *hostname, char *url, char *output_string) {
    char buf[16384] = "";
    char len[16384] = "";    
    char signal[16384] = "";    
    int datasize = 16 * 1024;
    int numbytes = 1;
    int totalbytes = 0;
    int messagelength;
    char *data = NULL;
    char *head = NULL;
    int headlen = 0;
    char *tmpdata = NULL;
    int sockfd = 0;
    char *ip = NULL;
    int retval = 0;
    char *token = NULL;
    char *contentlength = NULL;
    int clength = 0;
    int i = 0;
    int l = 0;
    char *content = NULL;
    int magic_num = 6; // magic
    
    asprintf(&ip, "%s", s_resolv(hostname));
    //printf("downloading url %s from %s\n", url, ip);
    sockfd = s_connect(80, ip);
    if (sockfd != -1) {	
	snprintf(signal, 512, "GET %s HTTP/1.0\nHost: %s\n\n", url, hostname);
	//printf("%s \n", signal);
	retval = send(sockfd, signal, strlen(signal), 0);
	if (retval != -1) {
	    //printf("connected\n");
	    // get the HEAD
	    do  {		    
		numbytes = recv(sockfd, buf, datasize, 0);
		buf[numbytes] = 0;
		tmpdata = data;
		asprintf(&data, "%s%s", data, buf);
		free(tmpdata); tmpdata = NULL;	    
	    } while (numbytes > 0);
	    //printf("%s\n", data);
	    
	    for(i = 0; i < strlen(data) - 4; i += 1) {
		if (data[i] == 13) {
		    if (data[i + 1] == 10) {
			if (data[i + 2] == 13) {
			    if (data[i + 3] == 10) {
				l = i + 4;
				break;
			    }
			}
		    }
		}
	    }
	    
	    asprintf(output_string, "%s", &data[l]);
	    free(data); data = NULL;
	    //printf("head: %s\n", output_string);
	    
	    	        
	    //strcpy(output_string, head);
	    return 0;
	} else {
	}
    } else {
    }
    return -1;
}

int s_downloadstring_post(char *hostname, char *url, char *post_string, char *output_string) {
    char buf[16384] = "";
    char *signal = NULL;
    int datasize = 16 * 1024;
    int numbytes = 1;
    char *data = NULL;
    char *tmpdata = NULL;
    int sockfd = 0;
    char *ip = NULL;
    int retval = 0;
    int i = 0;
    int l = 0;
    
    asprintf(&ip, "%s", s_resolv(hostname));
    //printf("downloading url %s from %s\n", url, ip);
    sockfd = s_connect(80, ip);
    if (sockfd != -1) {	
	asprintf(&signal, "POST %s HTTP/1.0\nHost: %s\nUser-Agent: exgen-%s/%s\nContent-Length: %i\nContent-Type: application/x-www-form-urlencoded\n\n%s", 
	    url, 
	    hostname, 
	    GetVar("configuration", "arch"), 
	    GetVar("configuration", "version"),
	    strlen(post_string),
	    post_string
	);
	//printf("%s \n", signal);
	retval = send(sockfd, signal, strlen(signal), 0);
	if (retval != -1) {
	    //printf("connected\n");
	    do  {		    
		numbytes = recv(sockfd, buf, datasize, 0);
		buf[numbytes] = 0;
		tmpdata = data;
		asprintf(&data, "%s%s", data, buf);
		free(tmpdata); tmpdata = NULL;	    
	    } while (numbytes > 0);
	    //printf("%s\n", data);
	    
	    // disregard the head
	    for(i = 0; i < strlen(data) - 4; i += 1) {
		if (data[i] == 13) {
		    if (data[i + 1] == 10) {
			if (data[i + 2] == 13) {
			    if (data[i + 3] == 10) {
				l = i + 4;
				break;
			    }
			}
		    }
		}
	    }
	    
	    asprintf(output_string, "%s", &data[l]);
	    free(data); data = NULL;
	    return 0;
	} else {
	}
    } else {
    }
    return -1;
}



// Module functions
// mod_ prefix
// Functions related to loading/unloading and managing Modules

/*
create_module makes a new blank entry in the module registry and returns
an index to the entry
*/
int create_module(void) {
    int i = 0;
    int index = -1;
    
    // find empty spot
    for (i = 0; i < modulecount; i += 1) {
	if (modules[i] == NULL) {
	    index = i;
	    break;
	}
    }
    
    if (index == -1) {
	// resize array
	modulecount += 1;
	struct moduledata ** tmpmodules = calloc(modulecount, sizeof(struct moduledata *));
	for (i = 0; i < modulecount - 1; i += 1) {
	    tmpmodules[i] = modules[i];
	}
	if (modules != NULL) {
	    free(modules);
	}
	index = modulecount -1;
	modules = calloc(modulecount, sizeof(struct moduledata *));
	for (i = 0; i < modulecount - 1; i += 1) {
	    modules[i] = tmpmodules[i];
	}
	free(tmpmodules);
    }
    modules[index] = calloc(1, sizeof(struct moduledata));
    modules[index]->name = NULL;
    modules[index]->file = NULL;
    modules[index]->version = NULL;
    modules[index]->handle = NULL;
    modules[index]->unload = 0;
    modules[index]->lua_register_call = NULL;
    
    return index;
}

/*
delete_module removes an entry from the module registry
*/
void delete_module(int index) {
    if (index >= 0) {
	if (index < modulecount) {
	    if (modules[index] != NULL) {
		if (modules[index]->name != NULL) {
		    free(modules[index]->name);
		}
		if (modules[index]->file != NULL) {
		    free(modules[index]->file);
		}
		if (modules[index]->version != NULL) {
		    free(modules[index]->version);
		}
		free(modules[index]);
		modules[index] = NULL;
	    }
	}
    }
}

void * mod_load(char * file) {
    double (*mod_proc)(char *);
    int retval = mod_loader(file, RTLD_GLOBAL|RTLD_NOW|RTLD_DEEPBIND);

    if (retval != -1) {
	*(void **) (&mod_proc) = dlsym(modules[retval]->handle, "_init_thread");
	if (mod_proc != NULL) {
	    if (atoi(GetVar("configuration", "verbose")) != 0) {
	        printf("Info: Module %s initialization procedure is %p.\n", modules[retval]->name, mod_proc);
	    }

	    (*mod_proc)(modules[retval]->name);	
	} else {
    	    if (atoi(GetVar("configuration", "verbose")) != 0) {
	        printf("Warning: Module %s has no initialization procedure.\n", modules[retval]->name);
	    }
	}
    }
}

/* returns the registry index of the module. -1 means error */
int * mod_loader(char * file, int flags) {
    int i = 0;
    int c = 0;
    int index = -1;
    char *name;
    void *handle;

    char *tmpfile = NULL;
    findfile(&file);
    name = &file[i];

    for (i = strlen(file);i > 0; i--) {
	c = file[i];
	if (c == 47) {
	    if (i < strlen(file)) {
		name = &file[i + 1];
		break;
	    }
	}
    }
    if (mod_isloaded(name) != -1) {
        if (atoi(GetVar("configuration", "verbose")) != 0) {
	    printf("Module: %s... already loaded.", name);
	}

	return 0;
    }
    
    if (atoi(GetVar("configuration", "verbose")) != 0) {
	printf("Loading Module: %s...", name);
    }
    
    /*cout("Loading Module: ");
    cout(file);
    cout("\n");*/

    mod_failed = 0;
    handle = dlopen(file, flags);
    if (!handle) {
	cout(dlerror());
	cout("\n");
    } else {
	if (mod_failed == 0) {
	    index = create_module();
	    asprintf(&modules[index]->name, "%s", name);
	    asprintf(&modules[index]->file, "%s", file);
	    modules[index]->handle = handle;
	    if (atoi(GetVar("configuration", "verbose")) != 0) {
		cout("done\n");
	    }
	} else {
	    if (atoi(GetVar("configuration", "verbose")) != 0) {
		cout("failed.\n");
	    }
	}
    }
    return index;
}

void * mod_unload(char * name) {
    int i = 0;
    int index = -1;
    int found = 0;
    void *handle;
    for (i = 0; i < modulecount; i += 1) {
	if (modules[i] != NULL) {
	    if (strcmp(name, modules[i]->name) == 0) {
		handle = modules[i]->handle;
		found = 1;
		index = i;
		break;
	    }
	}
    }
    if (atoi(GetVar("configuration", "verbose")) != 0) {
        printf("Unloading Module: ", i);
        cout(name);
        cout("...");
    }
    if (found == 0) {
    	if (atoi(GetVar("configuration", "verbose")) != 0) {
	    printf("Module not loaded or already unloaded...");
	}
    }
    if (found == 1) {
	//mod_unregister(name);
	delete_module(index);
	dlclose(handle);
    }
    if (atoi(GetVar("configuration", "verbose")) != 0) {
	cout("done.\n");    
    }
    //printf("%i\n", dlclose(handle));    
    //printf("aloha\n");

}

void * mod_lua_register(char *name, double (*callback)(void * L)) {
    int i = 0;
    int found = 0;
    void *handle;

    //printf("%s registered callback %p\n", name, callback);
    if (name != NULL) {
        for (i = 0; i < modulecount; i += 1) {
	    if (modules[i] != NULL) {
    		if (strcmp(name, modules[i]->name) == 0) {
		    modules[i]->lua_register_call = callback;
		    break;
		}
	    }
	}
//    } else {
//	modules[modulecount + 1]->lua_register_call = callback;
    }
}

/*
void * mod_unregister(char * name) {
    struct moduledata ** tmpmodules = calloc(modulecount, sizeof(struct moduledata));
    int i = 0;
    int c = 0;
    void *handle;

    i = 0;
    int index = 0;
    int found = 0;
    for (i = 0; i < modulecount; i += 1) {
	if (found == 0) {
	    if (strcmp(name, modules[i]->name) == 0) {
		//printf("removing %s\n", modules[i]->name);
		handle = modules[i]->handle;
		found = 1;
	    } else {
		tmpmodules[index] = modules[i];
		index += 1;
	    }
	} else {
	    tmpmodules[index] = modules[i];
	    index += 1;
	}
    }
    
    if (modules != NULL) {
	free(modules);
    }
    modulecount = index;
    if (modulecount > 0) {
	modules = calloc(modulecount, sizeof(struct moduledata *));
	for (i = 0; i < modulecount; i += 1) {
	    modules[i] = tmpmodules[i];
	}
    }

    //printf("dclose %i\n", dlclose(handle));
    return 0;
}
*/

int mod_fail(void) {
    mod_failed = 1;
    return 0;
}

int mod_proc(char * name, char * proc) {
    void *handle;
    char *error;
    int i = 0;
    
    for (i = 0; i < modulecount; i += 1) {
	if (modules[i] != NULL) {
	    if (strcmp(name, modules[i]->name) == 0) {
		handle = modules[i]->handle;
		break;
	    }
	}
    }

    double (*mod_proc)(void);
    *(void **) (&mod_proc) = dlsym(handle, proc);
    if ((error = dlerror()) != NULL) {
	//fprintf(stderr, "%p:%s\n", handle, error);
	//exit(1);
	cout(error);
	cout("\n");
	
    }
    (*mod_proc)();
}

int mod_isloaded(char * name) {
    int i = 0;
    if (modulecount > 0) {
	for (i = 0; i < modulecount; i += 1) {
	    if (modules[i] != NULL) {
		if (strcmp(name, modules[i]->name) == 0) {
		    return i;
		    break;
		}
	    }
	}
    }
    return -1;
}


int mod_loadthread(char * file) {
    pthread_attr_t tattr;
    pthread_t tid;
    int ret;
    int status;
    char *name;
    void *handle;
    char *error;
    int i = 0;
    
    handle = mod_loader(file, RTLD_NOW|RTLD_GLOBAL);
    if (handle == -1) {
	return 0;
    }
    double (*mod_proc)(char *);
    *(void **) (&mod_proc) = dlsym(handle, "_init_thread");
    if ((error = dlerror()) != NULL) {
	//fprintf(stderr, "%p:%s\n", handle, error);
	cout(error);
	cout("\n");
	//exit(1);
    }
    //(*mod_proc)();

    int c = 0;
    name = &file[i];
    for (i = strlen(file);i > 0; i--) {
	c = file[i];
	if (c == 47) {
	    if (i < strlen(file)) {
		name = &file[i + 1];
		break;
	    }
	}
    }
    

    ret = pthread_create(&tid, NULL, mod_proc, name);
    pthread_detach(tid);
}

// Lua functions
// l_ prefix
// Functions called by lua scripts

int l_download_string(void * L) {
    int n = lua_gettop(L);
    char *hostname = lua_tostring(L, 1);
    char *url = lua_tostring(L, 2);
    char *post_string = NULL;
    char *retval = NULL;


    
    if (n == 2) {
	s_downloadstring(hostname, url, &retval);
    } else if (n == 3) {
	post_string = lua_tostring(L, 3);
	s_downloadstring_post(hostname, url, post_string, &retval);
    }
//    printf("%s\n", retval);
    lua_pushstring(L, retval);
    return 1;
}


int l_download_file(void * L) {
    int n = lua_gettop(L);
    char *hostname = lua_tostring(L, 1);
    char *url = lua_tostring(L, 2);
    char *post_string = NULL;
    char *file_name = NULL;
    char *retval = NULL;


    
    if (n == 4) {
	post_string = lua_tostring(L, 3);
	file_name = lua_tostring(L, 4);
	s_downloadfile_post(hostname, url, post_string, file_name);
    } else if (n == 3) {
	file_name = lua_tostring(L, 3);
	s_downloadfile(hostname, url, file_name);        
    }
//    printf("%s\n", retval);
    lua_pushstring(L, retval);
    return 1;
}

/*
// finish this at a later date, or never
int * l_bind(lua_State * L) {
    double (*mod_proc)(char *);
    int i = 0;
    //int retval = mod_loader(file, RTLD_GLOBAL|RTLD_NOW);
    //int retval = mod_loader(file, RTLD_NOW);

    char *name = lua_tostring(L, 1);
    char *proc_name = lua_tostring(L, 2);
    
    
    
    if (name != NULL) { // the NULL check might be retarded here
	for (i = 0; i < modulecount; i += 1) {
	    if (strcmp(name, modules[i]->name) == 0) {
	    	*(void **) (&mod_proc) = dlsym(modules[i]->handle, "_init_thread");
		lua_register(L, proc_name, mod_proc);// lua_register_help(L, "echo", "STRING text", "Print text to the screen.", "", "core");
	    }
	}
    }
}
*/

int l_fork(lua_State * L) {
    pid_t child_pid;
    child_pid = fork();
    lua_pushnumber(L, child_pid);
    return 1;
}

int l_inkey(lua_State * L) {
    lua_pushnumber(L, getchar_unlocked());
    return 1;
}

int l_vt_create(lua_State * L) {
    int trows = lua_tonumber(L, 1);
    int tcols = lua_tonumber(L, 2);
    lua_pushlightuserdata(L, rote_vt_create(trows, tcols));
    return 1;
}

int l_vt_forkpty(lua_State * L) {
    RoteTerm *rt = lua_touserdata(L, 1);
    char *command = lua_tostring(L, 2);
    lua_pushnumber(L, rote_vt_forkpty(rt, command));
    return 1;
}

int l_vt_child_alive(lua_State * L) {
    pid_t pid = lua_tonumber(L, 1);
    int status = 0;
    pid_t retval = waitpid(pid, status, WNOHANG);
    lua_pushnumber(L, retval);
    return 1;
}


int l_vt_forsake_child(lua_State * L) {
    RoteTerm *rt = lua_touserdata(L, 1);
    rote_vt_forsake_child(rt);
    return 0;
}


int l_vt_update(lua_State * L) {
    RoteTerm *rt = lua_touserdata(L, 1);
    rote_vt_update(rt);
    return 0;
}

int l_vt_dirty(lua_State * L) {
    RoteTerm *rt = lua_touserdata(L, 1);
    if (rt->curpos_dirty) {
	lua_pushnumber(L, 1);
    } else {
	lua_pushnil(L);
    }
    return 1;
}

int l_vt_clean(lua_State * L) {
    RoteTerm *rt = lua_touserdata(L, 1);
    rt->curpos_dirty = FALSE;
    return 0;
}

int l_vt_char(lua_State * L) {
    RoteTerm *rt = lua_touserdata(L, 1);
    int row = lua_tonumber(L, 2);
    int col = lua_tonumber(L, 3);
    char ch[1] = "";
    ch[0] = rt->cells[row][col].ch;
    ch[1] = 0;
    lua_pushstring(L, ch);
    return 1;
}

int l_vt_cursor_row(lua_State * L) {
    RoteTerm *rt = lua_touserdata(L, 1);
    lua_pushnumber(L, rt->crow);
    return 1;
}

int l_vt_cursor_col(lua_State * L) {
    RoteTerm *rt = lua_touserdata(L, 1);
    lua_pushnumber(L, rt->ccol);
    return 1;
}

int l_vt_fgcolor(lua_State * L) {
    RoteTerm *rt = lua_touserdata(L, 1);
    int row = lua_tonumber(L, 2);
    int col = lua_tonumber(L, 3);
    lua_pushnumber(L, ROTE_ATTR_FG(rt->cells[row][col].attr));
    return 1;
}

int l_vt_bgcolor(lua_State * L) {
    RoteTerm *rt = lua_touserdata(L, 1);
    int row = lua_tonumber(L, 2);
    int col = lua_tonumber(L, 3);
    lua_pushnumber(L, ROTE_ATTR_BG(rt->cells[row][col].attr));
    return 1;
}

int l_vt_write(lua_State * L) {
    RoteTerm *rt = lua_touserdata(L, 1);
    int length = 0;
    char *string = lua_tostring(L, 2);
    length = strlen(string);
    rote_vt_write(rt, string, length);
    return 0;    
}

int l_vt_inject(lua_State * L) {
    RoteTerm *rt = lua_touserdata(L, 1);
    int length = 0;
    char *string = lua_tostring(L, 2);
    length = strlen(string);
    rote_vt_inject(rt, string, length);
    return 0;
}

int l_vt_keypress(lua_State * L) {
    RoteTerm *rt = lua_touserdata(L, 1);
    int key = lua_tonumber(L, 2);
    rote_vt_keypress(rt, key);
    return 0;
}

int l_vt_convscan(lua_State * L) {
    int scan = lua_tonumber(L, 1);
    int retval;
    retval = 0;
    switch (scan) {
	case 84 : retval = 259; break;
	case 85 : retval = 258; break;	
	case 82 : retval = 260; break;	
	case 83 : retval = 261; break;		
	case 47 : retval = 265; break;		
	case 48 : retval = 266; break;	
	case 49 : retval = 267; break;		
	case 50 : retval = 268; break;		
	case 51 : retval = 269; break;	
	case 52 : retval = 270; break;		
	case 53 : retval = 271; break;		
	case 54 : retval = 272; break;	
	case 55 : retval = 273; break;		
	case 56 : retval = 274; break;		
	case 57 : retval = 275; break;	
	case 58 : retval = 276; break;		
	case 80 : retval = 339; break;		
	case 81 : retval = 338; break;	
	case 78 : retval = 262; break;		
	case 79 : retval = 360; break;		
	case 76 : retval = 331; break;		
	case 77 : retval = 330; break;
    }
    lua_pushnumber(L, retval);
    return 1;
}


int l_resolv(lua_State * L) { // hostname
    struct hostent *h;
    h = gethostbyname(lua_tostring(L, 1));
    lua_pushstring(L, inet_ntoa(*((struct in_addr *)h->h_addr)));    
    return 1;
}

int l_connect(lua_State * L) { // PORT, IP
    int sockfd;
    struct sockaddr_in dest_addr;
    
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(lua_tonumber(L, 1));
    dest_addr.sin_addr.s_addr = inet_addr(lua_tostring(L, 2));
    memset(dest_addr.sin_zero, "\0", sizeof(dest_addr.sin_zero));
    
    connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    
    lua_pushnumber(L, sockfd);
    return 1;
}

int l_listen(lua_State * L) { // PORT, IP
    int sockfd;
    struct sockaddr_in my_addr;
    char *ip = NULL;
    int ipi = 0;
    
    ip = lua_tostring(L, 2);
    if (strcmp(ip, "") == 0) {
	ipi = INADDR_ANY;
    } else {
	ipi = inet_addr(ip);
    }
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(lua_tonumber(L, 1));
    my_addr.sin_addr.s_addr = ipi;
    memset(my_addr.sin_zero, "\0", sizeof(my_addr.sin_zero));
    
    bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
    
    int yes = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    
    listen(sockfd, 5);
    
    lua_pushnumber(L, sockfd);
    
    return 1;
}

int l_accept(lua_State * L) { // sockfd
    int sockfd;
    int newfd;
    struct sockaddr_in their_addr;
    int sin_size;
    
    sockfd = lua_tonumber(L, 1);
    
    sin_size = sizeof(their_addr);
    newfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    
    lua_pushnumber(L, newfd);
    return 1;
}

int l_close(lua_State * L) { // sockfd
    int sockfd;
    
    sockfd = lua_tonumber(L, 1);
    close(sockfd);
    return 0;
}

int l_msend(lua_State * L) { // sockfd, string
    int sockfd;
    char *send_string = NULL;
    int retval = 0;
    
    sockfd = lua_tonumber(L, 1);
    send_string = lua_tostring(L, 2);
    
    retval = s_msend(sockfd, send_string);
    if (retval == 0) {
	lua_pushnumber(L, 0);
    } else {
	lua_pushnil(L);
    }
    
    return 1;
}

int l_mrecv(lua_State * L) { // sockfd
    int sockfd;
    char *retval = NULL;
    
    sockfd = lua_tonumber(L, 1);
    retval = s_mrecv(sockfd);
    
//    printf("done\n");
//    printf("rec'd: %s\n", retval);
    lua_pushstring(L, retval);
//    free(retval); 
    
    return 1;
}

int l_recv(lua_State * L) { // sockfd
    int sockfd;
    char buf[512] = "";
    int datasize = 511;
    int numbytes = 0;

    sockfd = lua_tonumber(L, 1);
    
    numbytes = recv(sockfd, buf, datasize, MSG_DONTWAIT);
    printf("NUMBYTES: %i\n", numbytes);
    if (numbytes > 0) {
	buf[numbytes] = 0;
	lua_pushstring(L, buf);
	return 1;
    } else if (numbytes == 0) {
	lua_pushnil(L);
	return 1;
    } else if (numbytes == -1) {
	printf("No bytes chief, it's -1\n");
	lua_pushnil(L);
	return 1;
    }
}

int l_sendall(lua_State * L) { // sockfd, string
    int sockfd;
    char *send_string = NULL;
    int total = 0;
    int bytesleft = 0;
    int start = 0;
    int n;
    
    sockfd = lua_tonumber(L, 1);
    send_string = lua_tostring(L, 2);
    
    bytesleft = strlen(send_string);
    start = bytesleft;
    
    while (total < start) {
	n = send(sockfd, send_string+total, bytesleft, 0);
	total += n;
	bytesleft -= n;
    }
    return n==-1?-1:0; // -1 on failure, 0 on success
}

int l_send(lua_State * L) { // sockfd, string
    int sockfd;
    char *send_string = NULL;
    size_t string_length = 0;
    
    sockfd = lua_tonumber(L, 1);
    send_string = lua_tostring(L, 2);
    string_length = lua_strlen(L, 2);
    printf("About to send Sockfd: %i\n", sockfd);
    send(sockfd, send_string, string_length, MSG_NOSIGNAL);
    printf("done\n");
    return 0;
}

int l_getvar(lua_State * L) {
    int n = lua_gettop(L);
    char *table = lua_tostring(L, 1);
    char *key = lua_tostring(L, 2);
    
    lua_pushstring(L, GetVar(table, key));
    return 1;
}

int l_setvar(lua_State * L) {
    int n = lua_gettop(L);

    char *table = lua_tostring(L, 1);
    char *key = lua_tostring(L, 2);    
    char *value = lua_tostring(L, 3);
    
    SetVar(table, key, value);
    return 0;
}

int l_sqlite_open(lua_State * L) {
    int n = lua_gettop(L);
    char *filename = lua_tostring(L, 1);
//    char *
}

int l_filemtime(lua_State * L) {
    int n = lua_gettop(L);
    char *string = lua_tostring(L, 1);
    
    lua_pushnumber(L, filemtime(string));
    return 1;
}

int l_cout(lua_State * L) {
    int n = lua_gettop(L);
    
    char *string = lua_tostring(L, 1);
    cout(string);
    //cout("\n");

    //pthread_yield();    
    return 0;
}

int l_mod_isloaded(lua_State * L) {
    int n = lua_gettop(L);
    
    char *string = lua_tostring(L, 1);
    int ret = mod_isloaded(string);
    lua_pushnumber(L, ret);
    
    return 1;
}


int l_shutdown(lua_State * L) {
    exgen_shutdown();
    return 0;
}

int l_modload(lua_State * L) {
    int n = lua_gettop(L);
    
    char *file = lua_tostring(L, 1);
    
    mod_load(file);
    lua_init(L);
    return 0;
}

int l_mod_unload(lua_State * L) {
    int n = lua_gettop(L);
    
    char *file = lua_tostring(L, 1);

    mod_unload(file);
    return 0;
}

int l_modloadthread(lua_State * L) {
    int n = lua_gettop(L);
    
    char *file = lua_tostring(L, 1);
    mod_loadthread(file);

    return 0;
}

int l_luaload(lua_State * L) {
    int n = lua_gettop(L);
    
    char *file = lua_tostring(L, 1);
    //printf("run script: %s\n", file);
    lua_runscript(file);
    
    return 0;
}

int l_runstring(lua_State * L) {
    char *script = lua_tostring(L, 1);
    
    lua_runstring(script);
    return 0;
}

// aka branch_string
int l_runstringthread(lua_State * L) {
    int n = lua_gettop(L);
    void * uL = NULL;
    char *file = NULL;
    
    if (n == 2) {
	 file = lua_tostring(L, 1);    
	 uL = lua_touserdata(L, 2);
    } else if (n == 1) {
	 file = lua_tostring(L, 1);
    }

    //printf("run script thread: %s\n", file);
    lua_pushlightuserdata(L, lua_runstring_thread_reuse(uL, file));
    
    return 1;
}


// #working2
int l_replace(lua_State * L) {
    int i = 0;
    int i2 = 0;
    char *text = lua_tostring(L, 3);
    char *find_string = lua_tostring(L, 1);
    char *repl_string = lua_tostring(L, 2);
    char *tmp_text = NULL;
    char *new_text = NULL;

    printf("find_string length: %d\ntext length: %d\n", strlen(find_string), strlen(text));
    for(i = 0; i < strlen(text); i += 1) {
	// is text[i] a match for find_string?
	
	for(i2 = 0; i2 < strlen(find_string); i2 += 1) {
	    if (i+i2 < strlen(text)) {
		if (text[i+i2] == find_string[i2]) {
		    if (i2 == strlen(find_string) - 1) {
			printf("match\n");
			
			tmp_text = new_text;
			asprintf(&new_text, "%s%s", new_text, repl_string);
			free(tmp_text); tmp_text = NULL;
		    }
		}
	    }
	}
    }
    lua_pushstring(L, text);
    return 0;
}

int l_list_directories(lua_State * L) {
    int n = lua_gettop(L);
    int i = 0;
    char *path = lua_tostring(L, 1);

    DIR *dp;
    struct dirent *ep;

    lua_newtable(L);

    dp = opendir(path);
    printf("Listing directory: %s\n", path);
    if (dp != NULL) {
	while (ep = readdir(dp)) {
	    if (strcmp(".", ep->d_name) == 0) {
	    } else if (strcmp("..", ep->d_name) == 0) {
	    } else {
		printf("%i == %i, %s\n", ep->d_type, 10, ep->d_name);
		if ((ep->d_type == 10) || (ep->d_type == 4)) {
		    printf("%s\n", ep->d_name);
		    i += 1;
		    lua_pushnumber(L, i);
		    lua_pushstring(L, ep->d_name);
		    lua_rawset(L, -3);
		}
	    }
	}
	closedir(dp);
    } else {
	
    }
    return 1;
}

int l_list_files(lua_State * L) {
    int n = lua_gettop(L);
    int i = 0;
    char *path = lua_tostring(L, 1);

    DIR *dp;
    struct dirent *ep;

    lua_newtable(L);

    dp = opendir(path);
    if (dp != NULL) {
	while (ep = readdir(dp)) {
	    if (strcmp(".", ep->d_name) == 0) {
	    } else if (strcmp("..", ep->d_name) == 0) {
	    } else {
		//printf("%i == %i, %s\n", ep->d_type, 10, ep->d_name);
		if ((ep->d_type == 10) || (ep->d_type == 4)) {
		} else {
		    i += 1;
		    lua_pushnumber(L, i);
		    lua_pushstring(L, ep->d_name);
		    lua_rawset(L, -3);
		}
	    }
	}
	closedir(dp);
    } else {
	
    }
    return 1;
}


int l_file_exists(lua_State * L) {
    int n = lua_gettop(L);
    char *file = lua_tostring(L, 1);
    int ret = file_exists(file);
    if (ret) {
	lua_pushnumber(L, ret);
    } else {
	lua_pushnil(L);
    }
    return 1;
}

int l_exec(lua_State * L) {
    int n = lua_gettop(L);
    
    char *file = lua_tostring(L, 1);
    char *args = lua_tostring(L, 2);
    
    pid_t pid = fork();
    if (pid == 0) {
	execlp(file, file, args, NULL);
	cout(strerror(errno));
    } else {
	waitpid(pid, NULL, 0);
    }
    return 0;
}

int l_chmod(lua_State * L) {
    int n = lua_gettop(L);
    
    char *file = lua_tostring(L, 1);
    int m = lua_tonumber(L, 2);
    
    chmod(file, m);
    return 0;
}


int l_memory_usage(lua_State * L) {
    int n = lua_gettop(L);
    
    char buf[30];
    int size; //       total program size
    int resident;//   resident set size
    int share;//      shared pages
    int text;//       text (code)
    int lib;//        library
    int data;//       data/stack
    int dt;//         dirty pages (unused in Linux 2.6)

    snprintf(buf, 30, "/proc/%u/statm", (unsigned)getpid());
    FILE* pf = fopen(buf, "r");
    if (pf) {
        fscanf(pf, "%i" /* %u %u %u %u %u"*/, &size/*, &resident, &share, &text, &lib, &data*/);        
    }
    fclose(pf);    
    
    lua_pushnumber(L, (size/1024));
    
    return 1;
}

int l_savefile(lua_State * L) {
    int n = lua_gettop(L);
    
    char *file = lua_tostring(L, 1);
    char *string = lua_tostring(L, 2);
    printf("Saving %s!\n", file);
    FILE *h = fopen(file, "w");
    if (h == NULL) { 
	printf("FAILED TO SAVE FILE!~!!!!\n");
    } else {
	fputs(string, h);
	fclose(h);
    }
    return 0;
}


int l_openfile(lua_State * L) {
    int n = lua_gettop(L);
    
    char *name = lua_tostring(L, 1);
    printf("Name: %s\n", name);
    if (findfile(&name)) {
	printf("Name: %s\n", name);

        FILE *file;
	char *buffer;
        unsigned long fileLen;

        //Open file
	file = fopen(name, "rb");
        if (!file) {
	    fprintf(stderr, "Unable to open file %s", name);
	    return 0;
	}
	//Get file length
	fseek(file, 0, SEEK_END);
	fileLen=ftell(file);
        fseek(file, 0, SEEK_SET);

	//Allocate memory
        buffer=(char *)malloc(fileLen+1);
        if (!buffer) {
	    fprintf(stderr, "Memory error!");
	    fclose(file);
	    return 0;
        }

        //Read file contents into buffer
        fread(buffer, fileLen, 1, file);
        fclose(file);

        //Do what ever with buffer
        lua_pushlstring(L, buffer, fileLen);
        return 1;
    } else {
	printf("FAIL\n");
	return 0;
    }
}

int l_old_openfile(lua_State * L) {
    int n = lua_gettop(L);
    
    char *file = lua_tostring(L, 1);
    FILE *h = fopen(file, "r");
    char *buffer[1024];
    char *contents = NULL;
    char *tmpcontents = NULL;
    if (h) {
	while (feof(h) == 0) {
	    fgets(buffer, 1024, h);
	    if (contents == NULL) {
		asprintf(&contents, "%s", buffer);
	    } else {
		asprintf(&tmpcontents, "%s%s", contents, buffer);
		free(contents);
		contents = tmpcontents;
	    }
	}
        fclose(h);
	lua_pushstring(L, contents);
	return 1;
    }
    return 0;
}

int l_include (lua_State * L) {
    int n = lua_gettop(L);
    
    char *file = lua_tostring(L, 1);

    if (findfile(&file)) {
	lua_newdofile(L, file);
    }
    return 0;
}

int l_include_string (lua_State * L) {
    int n = lua_gettop(L);
    
    if (n == 1) {
	char *string = lua_tostring(L, 1);
        //lua_dostring(L, string);
	lua_runstring_reuse(L, string);
    }
    return 0;
}



int l_lualoadthread(lua_State * L) {
    int n = lua_gettop(L);
    
    char *file = lua_tostring(L, 1);
    //printf("run script thread: %s\n", file);
    lua_runscriptthread(file);
    
    return 0;
}

int l_luaload_reuse(lua_State * L) {
    int n = lua_gettop(L);
    void * uL = NULL;
    char *file = NULL;
    
    if (n == 2) {
	 file = lua_tostring(L, 1);    
	 uL = lua_touserdata(L, 2);
    } else if (n == 1) {
	 file = lua_tostring(L, 1);
    }

    //printf("run script thread: %s\n", file);
    lua_pushlightuserdata(L, lua_runscript_reuse(uL, file));
    
    return 1;
}

// aka branch
int l_luaload_thread_reuse(lua_State * L) {
    int n = lua_gettop(L);
    void * uL = NULL;
    char *file = NULL;
    
    if (n == 2) {
	 file = lua_tostring(L, 1);    
	 uL = lua_touserdata(L, 2);
    } else if (n == 1) {
	 file = lua_tostring(L, 1);
    }

    //printf("run script thread: %s\n", file);
    lua_pushlightuserdata(L, lua_runscript_thread_reuse(uL, file));
    
    return 1;
}

int l_new_state(lua_State * L) {
    void * nL = NULL;
    printf("\nNew Lua State...\n");
    nL = lua_open();

    lua_baselibopen(nL);
    luaopen_table(nL);
    luaopen_io(nL);
    luaopen_string(nL);
    luaopen_math(nL);
    lua_init(nL);
    printf("done\n");    
    lua_pushlightuserdata(L, nL);
    return 1;
}


int l_state(lua_State * L) {
    lua_pushlightuserdata(L, L);
    return 1;
}

int l_lua_close(lua_State * L) {
    void * uL = NULL;
    uL = lua_touserdata(L, 1);
    lua_close(uL);
    free(uL);
    return 0;
}

int l_sleep(lua_State * L) {
    int sleep_time = lua_tonumber(L, 1);
    usleep(sleep_time);
    return 0;
}

int l_help(lua_State * L) {

    int n = lua_gettop(L);
    int tindex = 0;
    int rindex = 0;
    int eindex = 0;
    char *arg = lua_tostring(L, 1);
    char *desc;
    char *args;
    char *longdesc;
    char *modulename;

    if (n == 1) {
	lua_pushliteral(L, "reference");	// the table we want
	tindex = lua_gettop(L);		// index of the reference table    
	lua_gettable(L, LUA_GLOBALSINDEX);	// reference table is now at the top of the stack
    
	lua_pushstring(L, arg);
	rindex = lua_gettop(L);
	lua_gettable(L, tindex);		// got reference["arg"]
    
	lua_pushliteral(L, "description");
	eindex = lua_gettop(L);
	lua_gettable(L, rindex);		// got reference["arg"]["desc"] 
        desc = lua_tostring(L, eindex);

        lua_pushliteral(L, "arguments");
	eindex = lua_gettop(L);
        lua_gettable(L, rindex);		// got reference["arg"]["desc"] 
	args = lua_tostring(L, eindex);

        lua_pushliteral(L, "longdescription");
	eindex = lua_gettop(L);
        lua_gettable(L, rindex);		// got reference["arg"]["desc"] 
	longdesc = lua_tostring(L, eindex);

        lua_pushliteral(L, "module");
	eindex = lua_gettop(L);
        lua_gettable(L, rindex);		// got reference["arg"]["desc"] 
	modulename = lua_tostring(L, eindex);

	printf("%s\n", arg);
	printf("	Provided by %s module.\n\n", modulename);
	printf("Syntax:\n	%s(%s)\n\n", arg, args);
        printf("Description:\n	%s\n", desc);    
        printf("\n	%s\n", longdesc);    	

    } else {
	printf("Enter help(\"command\") for information on that command.\n\nCommand reference:\n");
	lua_pushliteral(L, "reference");	// the table we want
	tindex = lua_gettop(L);		// index of the reference table    
	lua_gettable(L, LUA_GLOBALSINDEX);	// reference table is now at the top of the stack
	
	lua_pushnil(L);
	while(lua_next(L, tindex) != 0) {
	    printf("	%s\n", lua_tostring(L, -2));
				
	    lua_pop(L, 1);
	}
    }
    return 0;
}

int l_yield(lua_State * L) {
    pthread_yield();
    return 0;
}


int l_reinit(lua_State * L) {
    lua_init(L);
        
    return 0;
}


int l_mod_update(lua_State * L) {
    char *ip = NULL;
    char url[1024] = "";
    char version[1024] = "";
    char valid[512] = "";
    char *module_name = NULL;
    int n = lua_gettop(L);
    int interactive = 0;
    int key = 0;
    
    if (n > 0) {
	module_name = lua_tostring(L, 1);
	interactive = lua_tonumber(L, 2);
    } else {
	lua_pushnumber(L, -1); return 1;	    
    }
    if (mod_isloaded(module_name) == -1) {
	mod_load(module_name);
	if (mod_isloaded(module_name) == -1) {
	    //lua_pushnumber(L, -1); return 1;
	}
	mod_unload(module_name);
    } else {
	mod_unload(module_name);
    }

    asprintf(&ip, "%s", s_resolv(GetVar("configuration", "update-server")));
    if (interactive) {
	printf("Checking update server for updates %s (%s)...\n", GetVar("configuration", "update-server"), ip);
    }

    if (s_downloadstring(GetVar("configuration", "update-server"), "/exgen/valid", &valid) == 0) {
	//printf("validation string = \"%s\"\n", valid);
	if (strcmp("exgen", valid) == 0) {
	} else {
	    if (interactive) {
		printf("Update server %s is not valid.\n", GetVar("configuration", "update-server"));
	    }
	    lua_pushnumber(L, -1); return 1;	    
	}
    } else {
	if (interactive) {
	    printf("Could not access update server.\n");
	}
	lua_pushnumber(L, -1); return 1;
    }

    snprintf(url, 1024, "/exgen/modules/%s/latest", module_name);    
    if (s_downloadstring(GetVar("configuration", "update-server"), url, &version) == 0) {
	version[strlen(version) - 1] = 0;
    } else {
	if (interactive) {
	    printf("Could not access update server.\n");
	}
	lua_pushnumber(L, -1); return 1;
    }
    
    if (strcmp(GetVar(module_name, "version"), version) == 0) {
	if (interactive) {
	    printf("No newer versions available.\n");
	    //return -1;
	    lua_pushnumber(L, -1); return 1;
	}
    } else {
	if (interactive) {
	    printf("A newer version is available %s (%s)\n", module_name, version);
	    printf("Would you like to install the new version? [Y/N]\n");
	    key = getchar_unlocked();
	    if((key == 121) || (key == 89)) {
	    } else {
		printf("Update aborted.\n");
		//return -1;
		lua_pushnumber(L, -1); return 1;
	    }
	}
	// versions are different
	
	if (interactive) {
	    printf("Downloading %s %s...\n", module_name, version);
	}
	snprintf(url, 1024, "/exgen/modules/%s/%s/%s/%s", module_name, version, GetVar(module_name, "arch"), module_name);
	if (s_downloadfile(GetVar("configuration", "update-server"), url, "/tmp/tmpmod") == 0) {
	    if (interactive) {
	        printf("done.\n");
	    }

	    if (interactive) {
		printf("Installing %s %s.\n", module_name, version);
	    }
	    snprintf(url, 1024, "%smodules/%s", GetVar("configuration", "resource-path"), module_name);
	    if (filecopy("/tmp/tmpmod", url) == 0) {
		if (interactive) {
		    printf("Installation success.\n\n");	    
		}
	
	    } else {
		if (interactive) {
		    printf("Could not copy file.\nError: %s\n", strerror(errno));	    
		    lua_pushnumber(L, -1); return 1;
		}
		// failed
	    }
	    
	} else {
	    if (interactive) {
		printf("failed.\n Error downloading file.\nError: %s\n", strerror(errno));
		
	    }
	    lua_pushnumber(L, -1); return 1;
	    //return -1;
	    // error downloading file
	}
    }
    lua_pushnumber(L, 0); return 1;
}


int l_exgen_update(lua_State * L) {
    char *ip = NULL;
    char url[1024] = "";
    char version[1024] = "";
    char valid[512] = "";
    int n = lua_gettop(L);
    int interactive = 0;
    int no_version_check = 0;
    int key = 0;
    
    if (n > 0) {
	interactive = lua_tonumber(L, 1);
    }
    if (n == 2) {
	if (lua_tonumber(L, 2) != 0) {
	    no_version_check = 1;
	}
    }

    asprintf(&ip, "%s", s_resolv(GetVar("configuration", "update-server")));
    if (interactive) {
	printf("Checking update server for updates %s (%s)...\n", GetVar("configuration", "update-server"), ip);
    }

    if (s_downloadstring(GetVar("configuration", "update-server"), "/exgen/valid", &valid) == 0) {
	if (strcmp("exgen", valid) == 0) {
	} else {
	    if (interactive) {
		printf("Update server %s is not valid.\n", GetVar("configuration", "update-server"));
	    }
	    lua_pushnumber(L, -1); return 1;	    
	}
    } else {
	if (interactive) {
	    printf("Could not access update server.\n");
	}
	lua_pushnumber(L, -1); return 1;
    }
    
    if (s_downloadstring(GetVar("configuration", "update-server"), "/exgen/latest", &version) == 0) {
	version[strlen(version) - 1] = 0;
    } else {
	if (interactive) {
	    printf("Could not access update server.\n");
	}
	lua_pushnumber(L, -1); return 1;
    }
    
    if (no_version_check == 0) {
	if (strcmp(GetVar("configuration", "version"), version) == 0) {
	    if (interactive) {
		printf("No newer versions available.\n");
		//return -1;
		lua_pushnumber(L, -1);
	    }
	    return 1;
	}
    
	if (interactive) {
	    printf("A newer version is available (%s)\n", version);
	    printf("Would you like to install the new version? [Y/N]\n");
	    key = getchar_unlocked();
	    if((key == 121) || (key == 89)) {
	    } else {
		printf("Update aborted.\n");
		//return -1;
		lua_pushnumber(L, -1); return 1;
	    }
	}
	// versions are different
    }
    if (interactive) {
	printf("Downloading exgen %s...\n", version);
    }
    snprintf(url, 1024, "/exgen/%s/%s/exgen", version, GetVar("configuration", "arch"));
    //printf("%s\n", url);
    if (s_downloadfile(GetVar("configuration", "update-server"), url, "/tmp/exgen") == 0) {
        if (interactive) {
            printf("done.");
        }
        if (chmod("/tmp/exgen", S_IRWXU | S_IXGRP | S_IXOTH) == 0) {
	    execlp("/tmp/exgen", "/tmp/exgen", "-install", NULL);
	} else {
	    if (interactive) {
	        printf("Error setting execute permission. Update failed.\n");
	    }
	    //return -1;
	    lua_pushnumber(L, -1); return 1;
	    // error setting exec permission
	}
    } else {
	if (interactive) {
	    printf("failed.\n Error downloading file.\nError: %s\n", strerror(errno));
	}
	lua_pushnumber(L, -1); return 1;
	//return -1;
	// error downloading file
    }
    lua_pushnumber(L, 0); return 1;
}

int l_exgen_install(lua_State * L) {
    int n = lua_gettop(L);
    int interactive = 0;
    int key = 0;
    char *tmp_path = NULL;

    if (n > 0) {
	interactive = lua_tonumber(L, 1);
    }

    if (interactive) {
	printf("Installing exgen %s.\n", GetVar("configuration", "version"));
    }
    asprintf(&tmp_path, "%sexgen", GetVar("configuration", "executable-path"));
    if (filecopy(self_name, tmp_path) == 0) {
	free(tmp_path);
	asprintf(&tmp_path, "%s", GetVar("configuration", "resource-path"));
	mkdir(tmp_path, S_IRWXU | S_IXGRP | S_IXOTH);
	
	free(tmp_path);
	asprintf(&tmp_path, "%s/modules", GetVar("configuration", "resource-path"));
	mkdir(tmp_path, S_IRWXU | S_IXGRP | S_IXOTH);
	
	asprintf(&tmp_path, "%sexgen", GetVar("configuration", "executable-path"));
	if (chmod(tmp_path, S_IRWXU | S_IXGRP | S_IXOTH) == 0) {
	} else {
	    free(tmp_path);
	    if (interactive) {
	        printf("Error setting execute permission. Update failed.\n");
	    }
	    //return -1;
	    lua_pushnumber(L, -1); return 1;
	    // error setting exec permission
	}
	free(tmp_path);
	
	if (interactive) {
	    printf("Installation success.\n");	    
	}
	
    } else {
	//  free tmp_path from up above before the if
	free(tmp_path);
	if (interactive) {
	    printf("Could not copy file.\nError: %s\n", strerror(errno));	    
	    lua_pushnumber(L, -1); return 1;
	}
	// failed
    }
    lua_pushnumber(L, 0); return 1;
}

// lua related functions
// lua_ prefix
// functions related to Lua used by this host program

int lua_register_help(lua_State * L, char * name, char * args, char * desc, char * longdesc, char * modulename) {
    int tindex = 0;
    int rindex = 0;
    lua_pushliteral(L, "reference");
    tindex = lua_gettop(L);
    lua_gettable(L, LUA_GLOBALSINDEX); // push, pop
    
    lua_pushstring(L, name);
    lua_newtable(L);
    lua_settable(L, tindex); // push push, pop pop
    
    lua_pushstring(L, name);
    rindex = lua_gettop(L);
    lua_gettable(L, tindex); // push, pop
    
    lua_pushliteral(L, "arguments");
    lua_pushstring(L, args);
    lua_settable(L, rindex); // push push, pop pop

    lua_pushliteral(L, "description");
    lua_pushstring(L, desc);
    lua_settable(L, rindex); // push push, pop pop

    lua_pushliteral(L, "longdescription");
    lua_pushstring(L, longdesc);
    lua_settable(L, rindex); // push push, pop pop

    lua_pushliteral(L, "module");
    lua_pushstring(L, modulename);
    lua_settable(L, rindex); // push push, pop pop
    
    lua_pop(L, 2);
}


int lua_init(lua_State * L) {
    int tindex = 0;
    //char init_script[512] = "";
    char *init_script = NULL;
    
    lua_pushliteral(L, "reference");
    lua_newtable(L);
    lua_settable(L, LUA_GLOBALSINDEX);
    
    lua_pushliteral(L, "reference");
    tindex = lua_gettop(L);
    lua_gettable(L, LUA_GLOBALSINDEX);

    /*lua_pushliteral(L, "description");
    lua_pushliteral(L, "woohoo");
    lua_settable(L, tindex);*/

    //lua_register_help(L, "", "", "", "", "");
    lua_register(L, "exgen_update", l_exgen_update); lua_register_help(L, "exgen_update", "", "Check for a newer version of exgen then download and install newer version if it exists.", "", "core");
    lua_register(L, "exgen_install", l_exgen_install); lua_register_help(L, "exgen_install", "", "Attempt to install self.", "", "core");    
    lua_register(L, "mod_update", l_mod_update); lua_register_help(L, "mod_update", "", "", "", "core");
//    lua_register(L, "download_mod", l_download_mod); lua_register_help(L, "download_mod", "", "", "", "core");
    lua_register(L, "download_string", l_download_string);
    lua_register(L, "download_file", l_download_file);    

    lua_register(L, "sleep", l_sleep);
    lua_register(L, "echo", l_cout); lua_register_help(L, "echo", "STRING text", "Print text to the screen.", "", "core");
    lua_register(L, "help", l_help); lua_register_help(L, "help", "STRING help_topic", "Display help.", "", "core");
    lua_register(L, "mod_load", l_modload); lua_register_help(L, "mod_load", "STRING module_name", "Loads a binary module into the current thread.", "", "core");
    lua_register(L, "mod_loadthread", l_modloadthread); lua_register_help(L, "mod_loadthread", "STRING module_name", "Loads a binary module into a new thread.", "Loading a module into a new thread allows it to perform long running tasks asynchronously.", "core");
    lua_register(L, "mod_isloaded", l_mod_isloaded); lua_register_help(L, "mod_isloaded", "STRING module_name", "Tests to see if a module is loaded. Returns 1 if the given module is loaded, 0 if it is not.", "", "core");
    lua_register(L, "mod_unload", l_mod_unload); lua_register_help(L, "mod_unload", "STRING module_name", "Unloads the previously loaded module given by module_name.", "The use of this is discouraged as it can lead to program instability.", "core");    
    lua_register(L, "run_scriptthread", l_lualoadthread); lua_register_help(L, "run_scriptthread", "STRING script_file", "Loads and executes a Lua script given by script_file in a new thread.", "Loading a script into a new thread allows it to perform long running tasks asynchronously, allowing the calling thread to continue before the child thread exits.", "core");
    lua_register(L, "run_script", l_luaload); lua_register_help(L, "run_script", "STRING script_file", "Loads and executes a Lua script given by script_file in the current thread.", "Running a script in the current thread causes the calling script to halt until the child script exits.", "core");
    lua_register(L, "run_script_reuse", l_luaload_reuse); //lua_register_help(L, "run_script", "STRING script_file", "Loads and executes a Lua script given by script_file in the current thread.", "Running a script in the current thread causes the calling script to halt until the child script exits.", "core");    
    lua_register(L, "run_script_thread_reuse", l_luaload_thread_reuse); //lua_register_help(L, "run_script", "STRING script_file", "Loads and executes a Lua script given by script_file in the current thread.", "Running a script in the current thread causes the calling script to halt until the child script exits.", "core");        
    lua_register(L, "branch", l_luaload_thread_reuse); //lua_register_help(L, "run_script", "STRING script_file", "Loads and executes a Lua script given by script_file in the current thread.", "Running a script in the current thread causes the calling script to halt until the child script exits.", "core");            
    lua_register(L, "state", l_state);
    lua_register(L, "new_state", l_new_state);
    
    lua_register(L, "close_script", l_lua_close);
    lua_register(L, "include", l_include); lua_register_help(L, "include", "STRING script_file", "Loads a Lua script into the current script.", "Useful for defining configuration files that can be loaded at run-time, for splitting a single script into multiple files, or for implementing your own Lua script libraries.", "core");
    lua_register(L, "include_string", l_include_string); lua_register_help(L, "include_string", "STRING script", "Loads a Lua script into the current script.", "Useful for defining configuration files that can be loaded at run-time, for splitting a single script into multiple files, or for implementing your own Lua script libraries.", "core");    
    lua_register(L, "yield", l_yield); lua_register_help(L, "yield", "", "Explicitly force the current thread to yield its processor timeslice to the next thread.", "", "core");
    lua_register(L, "shutdown", l_shutdown); lua_register_help(L, "shutdown", "", "Unload all modules, terminate all running scripts, threads, and end the application.", "", "core");
    lua_register(L, "exit", l_shutdown); lua_register_help(L, "exit", "", "Alias for shutdown.", "", "core");
    lua_register(L, "quit", l_shutdown); lua_register_help(L, "quit", "", "Alias for shutdown.", "", "core");
    lua_register(L, "reinit", l_reinit); lua_register_help(L, "reinit", "", "Reinitialize the current Lua script.", "When a module is loaded, all subsequently started scripts will be able to use an extended functionality offered by the function. Scripts started before the module was loaded however will not be able to use the functionality unless the script is reinitialized. Use reinit after loading a module to be able to use any new functions the module provides.", "core");
    lua_register(L, "filemtime", l_filemtime); lua_register_help(L, "filemtime", "STRING filename", "Returns the modified time of the file given by filename. Time is formatted as a UNIX timestamp.", "", "core");
    lua_register(L, "exec", l_exec); lua_register_help(L, "exec", "STRING command", "Execute a system command.", "", "core");
    lua_register(L, "openfile", l_openfile); lua_register_help(L, "openfile", "STRING filename", "Returns the contents of the file given by filename.", "", "core");
    lua_register(L, "savefile", l_savefile); lua_register_help(L, "savefile", "STRING filename, STRING contents", "Writes the value given by contents to the file given by filename.", "", "core");

    // #working1
    lua_register(L, "replace", l_replace); lua_register_help(L, "replace", "STRING find_text, STRING replace_text, STRING text", "Finds each occurance of find_text in text and replaces it with replace_text", "", "core");
    lua_register(L, "list_directories", l_list_directories); lua_register_help(L, "list_directories", "STRING path", "Returns an array containing all the directories within the given path.", "", "core");
    lua_register(L, "list_files", l_list_files); lua_register_help(L, "list_files", "STRING path", "Returns an array containing all the files within the given path.", "", "core");

    lua_register(L, "file_exists", l_file_exists); lua_register_help(L, "file_exists", "STRING filename", "Returns 1 if the file given by filename exists. Returns 0 if it does not.", "", "core");
    lua_register(L, "chmod", l_chmod); lua_register_help(L, "chmod", "STRING filename, NUMBER mode", "Changes the mode of the file given by filename to the mode given by mode.", "", "core");
    lua_register(L, "setvar", l_setvar); lua_register_help(L, "setvar", "STRING table, STRING key, STRING value", "Stores the key/value pair given by key and value respectively in the table given by table.", "Modifies data in the global data store, which is accessable by all scripts, threads, and modules.", "core");
    lua_register(L, "getvar", l_getvar); lua_register_help(L, "getvar", "STRING table, STRING key", "Returns the value of a key given by key from the table given by table.", "Retrieves data from the global data store, which is accessable by all scripts, threads, and modules.", "core");    

    lua_register(L, "fork", l_fork); lua_register_help(L, "fork", "", "See Unix fork()", "", "core");
    lua_register(L, "inkey", l_inkey); lua_register_help(L, "inkey", "", "Returns the value of last key pressed by the user.", "", "core");    
    
    lua_register(L, "resolv", l_resolv); lua_register_help(L, "resolv", "STRING hostname", "Returns the IP address of the given hostname.", "", "core");
    lua_register(L, "connect", l_connect);  lua_register_help(L, "connect", "NUMBER port, STRING ip", "Opens and returns a handle to a network connection.", "", "core");
    lua_register(L, "close", l_close); lua_register_help(L, "close", "STRING handle", "Closes the network connection given by handle.", "", "core");
    lua_register(L, "listen", l_listen); lua_register_help(L, "listen", "NUMBER port, STRING ip", "Listens for incoming network connections.", "Must provide a port to listen on. May provide an IP to listen on but this is optional. IP may be left blank to listen on all IPs. Returns a handle.", "core");
    lua_register(L, "accept", l_accept); lua_register_help(L, "accept", "STRING handle", "Accepts a connection given by handle.", "", "core");
    lua_register(L, "recv", l_recv); 
    lua_register(L, "send", l_send);
    lua_register(L, "mrecv", l_mrecv);
    lua_register(L, "msend", l_msend);

    lua_register(L, "sendall", l_sendall);
    lua_register(L, "run_string", l_runstring);    
    lua_register(L, "branch_string", l_runstringthread);    

																    
    //lua_register(L, "", l_); lua_register_help(L, "", "", "", "", "core");    
    lua_register(L, "vt_create", l_vt_create); 		lua_register_help(L, "vt_create", 	"", "Returns a pointer to a new virtual terminal.", "", "core");
    lua_register(L, "vt_forkpty", l_vt_forkpty); 	lua_register_help(L, "vt_forkpty", 	"VT *vt, char *cmd", "Forks *cmd into the virtual terminal *vt.", "", "core");
    lua_register(L, "vt_child_alive", l_vt_child_alive);
    lua_register(L, "vt_update", l_vt_update); 		lua_register_help(L, "vt_update", 	"VT *vt", "Updates the state of the virtual terminal.", "", "core");
    lua_register(L, "vt_forsake_child", l_vt_forsake_child); lua_register_help(L, "vt_forsake_child", 	"VT *vt", "see man page", "", "core");    
    lua_register(L, "vt_dirty", l_vt_dirty); 		lua_register_help(L, "vt_dirty", 	"VT *vt", "Returns TRUE if the virtual terminal has changed. Use this to tell if the virtual terminal needs to be rendered again.", "", "core");
    lua_register(L, "vt_clean", l_vt_clean); 		lua_register_help(L, "vt_clean", 	"VT *vt", "Resets the value of vt_dirty to FALSE. Use this after rendering the virtual terminal.", "", "core");
    lua_register(L, "vt_char", l_vt_char); 		lua_register_help(L, "vt_char", 	"VT *vt, int row, int col", "Returns the character at the specified row and column of the virtual terminal.", "", "core");
    lua_register(L, "vt_convscan", l_vt_convscan);	lua_register_help(L, "vt_convscan", 	"VT *vt, int scancode", "Converts scancode to terminal code.", "Explantion/Example: Pressing the UP key is scancode 84. To send the UP key to the virtual terminal you use convscan to converty 84 to 256, which the virtual terminal recognizes as UP.", "core");
    lua_register(L, "vt_fgcolor", l_vt_fgcolor); 	lua_register_help(L, "vt_fgcolor", 	"VT *vt, int row, int col", "Returns the foreground color attribute at the specified row and column of the virtual terminal.", "", "core");
    lua_register(L, "vt_bgcolor", l_vt_bgcolor); 	lua_register_help(L, "vt_bgcolor", 	"VT *vt, int row, int col", "Returns the background color attribute at the specified row and column of the virtual terminal.", "", "core");    
    lua_register(L, "vt_write", l_vt_write); 		lua_register_help(L, "vt_write", 	"VT *vt, char *string", "Sends string to the forked process running in the virtual terminal.", "", "core");
    lua_register(L, "vt_inject", l_vt_inject); 		lua_register_help(L, "vt_inject", 	"VT *vt, char *string", "Sends string to the virtual terminal, bypassing any process that may be running in the virtual terminal.", "", "core");
    lua_register(L, "vt_keypress", l_vt_keypress); 	lua_register_help(L, "vt_keypress", 	"VT *vt, int key", "Sends keypress to the process running in the virtual terminal.", "", "core");
    lua_register(L, "vt_cursor_row", l_vt_cursor_row); 	lua_register_help(L, "vt_cursor_row", 	"VT *vt", "Returns the current row of the cursor in the virtual terminal.", "", "core");
    lua_register(L, "vt_cursor_col", l_vt_cursor_col); 	lua_register_help(L, "vt_cursor_col", 	"VT *vt", "Returns the current column of the cursor in the virtual terminal.", "", "core");

    lua_register(L, "memory_usage", l_memory_usage);
    
    
    int i = 0;
    for (i = 0; i < modulecount; i += 1) {	
	if (modules[i] != NULL) {
	    if (modules[i]->lua_register_call != NULL) {
		//printf("Info: Module %s registered callback %p.\n", modules[i]->name, modules[i]->lua_register_call);
		(*modules[i]->lua_register_call)(L);
	    }
	}
    }

    //lua_dostring(L, GetVar("configuration", "lua-init-script"));
    asprintf(&init_script, "thread = 'id%i';\n\n%s\n\n", &L, GetVar("configuration", "lua-init-script"));
//    printf("%s\n", init_script);
    lua_dostring(L, init_script);
    free(init_script);

}

// catch signal
void catch_signal(int signo) {
    printf("Caught Signal!!\n");
    exit(0);
}

void lua_panic(lua_State * L) {
    printf("Lua Panic\n");
    return 0;
}

int lua_newdofile(lua_State * L, char * file) {
    
//    printf("file: %s\n", file);
    luaL_loadfile(L, file);
    lua_atpanic(L, lua_panic);
//    printf("pcall \n");
    if (lua_pcall(L, 0, 0, 0) != 0) {
	printf("%s error: %s\n", file, lua_tostring(L, -1));
	lua_pop(L, 1);
    	
    }
//    printf("dead\n");

/*
    printf("file:\n%s\n", openfile(file));
    printf("lua state: %p\n", L);
    char * dostring = NULL;
    asprintf(&dostring, "dofile(\"%s\")", file);
    printf("to run: %s\n", dostring);
    lua_dostring(L, dostring);
*/
    return 0;
}

int lua_newdostring(lua_State * L, char * file) {
    int err = 0;
    err = luaL_loadbuffer(L, file, strlen(file), file);
    if (err) {
	cout(lua_tostring(L, -1));
	cout("\n");
	lua_pop(L, 1);
    }
    lua_atpanic(L, lua_panic);
    if (lua_pcall(L, 0, 0, 0) != 0) {
	printf("%s error: %s\n", file, lua_tostring(L, -1));
	lua_pop(L, 1);
    }
    return 0;
}

// unlink tmp file at exit
/*void unlink_tmp(void) {
    unlink(em_tmpname);
    printf("unlinked tmp\n");
}

// function to extract any embedded data if present
int extract_embedded(void) {
    char em_matchtext[18];
    em_matchtext[0] = 60; em_matchtext[1] = 32; em_matchtext[2] = 101;
    em_matchtext[3] = 109; em_matchtext[4] = 98; em_matchtext[5] = 101;
    em_matchtext[6] = 100; em_matchtext[7] = 100; em_matchtext[8] = 101;
    em_matchtext[9] = 100; em_matchtext[10] = 32; em_matchtext[11] = 100;
    em_matchtext[12] = 97; em_matchtext[13] = 116; em_matchtext[14] = 97;	    
    em_matchtext[15] = 32; em_matchtext[16] = 62; em_matchtext[17] = 10; em_matchtext[18] = 0;
	    
    int em_char = 0;
    int em_matchindex = 0;
    int em_matchlen = strlen(em_matchtext);
    int em_embeddedloc = 0;
    int em_embeddedfound = 0;
    char *em_contents = NULL;
    char *em_tmpcontents = NULL;
    char em_buffer[1025];
    int em_fsize = filesize(self_name);
    int em_embeddedsize = 0;
    
    FILE *h = fopen(self_name, "r");
    if (h) {		
	while (feof(h) == 0) {
	    em_char = fgetc(h);	
	    if (em_char == em_matchtext[em_matchindex]) {
		em_matchindex += 1;
		if (em_matchindex == em_matchlen) {			    
		    em_embeddedfound = 1;
		    break;
		}
	    } else {
		em_matchindex = 0;
	    }
	    em_embeddedloc += 1;
	}
		
    }

    if (em_embeddedfound == 1) {    
	sprintf(em_tmpname, "/tmp/exgen_%i", (uint32_t)getpid());	
	em_embeddedsize = em_fsize - em_embeddedloc;
	em_contents = malloc(em_embeddedsize);
	int em_bytesread = fread(em_contents, 1, em_embeddedsize, h);
	fclose(h);
	h = fopen(em_tmpname, "w");
	if (h) {
	    fwrite(em_contents, 1, em_bytesread, h);
	    fclose(h);
	    free(em_contents);
	}
	em_embedded = 1;
	atexit(unlink_tmp);
	return 1;	
    } else {
	if (h) {
	    fclose(h);
	}
	return 0;
    }

}


// general functions
// no prefix
// just general stuff
*/

/*
int unzip(char *zipfile, char *destdir) {

    ZZIP_DIR *pdir;    
    ZZIP_DIRENT *pent;
    char *newdir = NULL;
    char *tmpfile = NULL;
    char *contents = NULL;
    
    mkdir(destdir, 0);
    pdir = zzip_dir_open(zipfile, NULL);
    if (!pdir) {
	return 0;
    } 
    
    pent = zzip_readdir(pdir);
    while (pent != NULL) {
	if (pent->d_name[sizeof(pent->d_name)] == 47) {
	} else {
	    asprintf(&tmpfile, "%s%s", destdir, pent->d_name);	    
	    asprintf(&newdir, "%s/%s", zipfile, pent->d_name);	    
	    
	    zextractfile(newdir, tmpfile);
    
    	    free(tmpfile);
	    free(newdir);
	}
	pent = zzip_readdir(pdir);
    }
}
*/
int filesize(char *filename) {
    int x = 0;
    int s = 0;
    FILE *h = fopen(filename, "r");
    if (h) {		
	while (feof(h) == 0) {
	    x = fgetc(h);	
	    s += 1;
        }
	fclose(h);
    }
    return s;
}


int filecopy(char *src, char *dest) {
    FILE *from;
    FILE *to;
    int ch = 0;

    from = fopen(src, "rb");
    to = fopen(dest, "wb");
    if (to == NULL) {
	return -1;
    }
    while(!feof(from)) {
	ch = fgetc(from);
	if (!feof(from)) {
	    fputc(ch, to);
	}
    }
    fclose(from);
    fclose(to);
    return 0;
}

char * openfile(char *file) {
    FILE *h = fopen(file, "r");
    char *buffer[1024];
    char *contents = NULL;
    char *tmpcontents = NULL;
    if (h) {
	while (feof(h) == 0) {
	    fgets(buffer, 1024, h);
	    if (contents == NULL) {
		asprintf(&contents, "%s", buffer);
	    } else {
		asprintf(&tmpcontents, "%s%s", contents, buffer);
		free(contents);
		contents = tmpcontents;
	    }
	}
        fclose(h);
	return contents;
    }
}
/*
char * zextractfile(char *filename, char *dest) {
    char array[chunksize + 1];
    int bytecount = chunksize;
    int fsize = 0;

    
    // what file endings to accept... "" means that you can use /test.zip/
    // in the file name - has to end with a zero
    static zzip_strings_t ext[] = { ".zip", ".exp", "", 0 };
    
    // these functions are exactly the same as the normal posix-IO ones
    ZZIP_FILE * file;
    file = zzip_open_ext_io (filename, O_RDONLY, 0, ext, NULL);
    if (file == NULL)
    {
        return NULL;
    }
    while (bytecount == chunksize) {
	bytecount = zzip_fread(array , sizeof(char) , chunksize , file);
	fsize += bytecount;
    }
    zzip_close (file);

    file = zzip_open_ext_io (filename, O_RDONLY, 0, ext, NULL);
    if (file == NULL)
    {
        return NULL;
    }
    
    bytecount = chunksize;
    char *contents = malloc(fsize+1);
    zzip_fread(contents , 1, fsize , file);
    zzip_close (file);
    
    FILE *h = fopen(dest, "w");
    if (h) {
	fwrite(contents, 1, fsize, h);
	fclose(h);
	free(contents);	
    }
    
    
}


char * zopenfile(char *filename) {
    char array[chunksize + 1];
    int bytecount = chunksize;
    int fsize = 0;

    
    // what file endings to accept... "" means that you can use /test.zip/
    // in the file name - has to end with a zero
    static zzip_strings_t ext[] = { ".zip", ".exp", "", 0 };
    
    // these functions are exactly the same as the normal posix-IO ones
    ZZIP_FILE * file;
    file = zzip_open_ext_io (filename, O_RDONLY, 0, ext, NULL);
    if (file == NULL)
    {
        return NULL;
    }
    while (bytecount == chunksize) {
	bytecount = zzip_fread(array , sizeof(char) , chunksize , file);
	fsize += bytecount;
    }
    zzip_close (file);

    file = zzip_open_ext_io (filename, O_RDONLY, 0, ext, NULL);
    if (file == NULL)
    {
        return NULL;
    }
    
    bytecount = chunksize;
    char *contents = malloc(fsize+1);
    while (bytecount == chunksize) {
	bytecount = zzip_fread(array , sizeof(char) , chunksize , file);
	array[bytecount] = 0;
	contents = strcat(contents, array);
    }
    zzip_close (file);
    return contents;
}
*/
char * GetVar(char *table, char *key) {    
    int tindex = 0;
    int rindex = 0;
    int sindex = 0;
//    printf("GetVar %s\n", key);
    pthread_mutex_lock(&db_mutex);
    /*while (db_locked != 0) {
	pthread_yield();
    }*/
    db_locked = 1;
    
    tindex = lua_gettop(DB);  
    sindex = tindex;
    lua_pushstring(DB, table);    
    lua_gettable(DB, LUA_GLOBALSINDEX);
    if (lua_istable(DB, tindex) == 1) {

	lua_pushstring(DB, key);	
	lua_gettable(DB, tindex);   
	rindex = lua_gettop(DB);
	char *tmp = NULL;
	tmp = lua_tostring(DB, rindex); 
	lua_pop(DB, 2);
	db_locked = 0;
	pthread_mutex_unlock(&db_mutex);
	return tmp;
    } else {
	char *tmp = NULL;
	asprintf(&tmp, "");
	lua_pop(DB, 1);
	tindex = lua_gettop(DB);  
	//printf("tf %i - %i\n", sindex, tindex);
	//printf("returning empty string\n");
	db_locked = 0;
	pthread_mutex_unlock(&db_mutex);
	return tmp;
    }
}

lua_Number GetVarN(char *table, char *key) {
    int tindex = 0;
    int rindex = 0;
    lua_pushstring(DB, table);
    tindex = lua_gettop(DB);
    lua_gettable(DB, LUA_GLOBALSINDEX);
    if (lua_istable(DB, tindex) == 1) {

	lua_pushstring(DB, key);
	rindex = lua_gettop(DB);
	lua_gettable(DB, tindex);   
	return lua_tonumber(DB, rindex); 
    } else {
	return 0;
    }
}


void SetVar(char *table, char *key, char *value) {
    int tindex = 0;
    int sindex = 0;
//    printf("SetVar %s = %s\n", key, value);
    pthread_mutex_lock(&db_mutex);
    /*while (db_locked != 0) {
	pthread_yield();
    }*/
    db_locked = 1;
    tindex = lua_gettop(DB);
    sindex = tindex;
    lua_pushstring(DB, table);    
    lua_gettable(DB, LUA_GLOBALSINDEX);
    if (lua_istable(DB, tindex) == 0) {
	lua_pushstring(DB, table);
	lua_newtable(DB);
	lua_settable(DB, LUA_GLOBALSINDEX);
	
	lua_pushstring(DB, table);
	tindex = lua_gettop(DB);
	lua_gettable(DB, LUA_GLOBALSINDEX);
    }
    lua_pushstring(DB, key);
    lua_pushstring(DB, value);
    
    lua_settable(DB, tindex);    

    lua_pop(DB, 1);
    db_locked = 0;
    pthread_mutex_unlock(&db_mutex);
}

void SetVarN(char *table, char *key, lua_Number value) {
    int tindex = 0;
    lua_pushstring(DB, table);
    tindex = lua_gettop(DB);
    lua_gettable(DB, LUA_GLOBALSINDEX);
    if (lua_istable(DB, tindex) == 0) {
	lua_pushstring(DB, table);
	lua_newtable(DB);
	lua_settable(DB, LUA_GLOBALSINDEX);
	
	lua_pushstring(DB, table);
	tindex = lua_gettop(DB);
	lua_gettable(DB, LUA_GLOBALSINDEX);
    }
    lua_pushstring(DB, key);
    lua_pushnumber(DB, value);
    lua_settable(DB, tindex);    
}


void config_defaults(void) {
    SetVar("configuration", "default", "1");
    SetVar("configuration", "verbose", "0");
    SetVar("configuration", "verbose", "0");    
    SetVar("configuration", "master-port", "2600");    
    SetVar("configuration", "master-address", "");
    SetVar("configuration", "slave-port", "2601");    
    SetVar("configuration", "slave-ip", "");        
    SetVar("configuration", "slave", "0");    
    SetVar("configuration", "update-server", "exgen.illution.net");
    SetVar("configuration", "version", VERSION_STRING);    
    SetVar("configuration", "arch", ARCH_STRING);        
    SetVar("configuration", "prompt", "# ");
    SetVar("configuration", "lua-init-script", "");
    SetVar("configuration", "resource-path", "/usr/local/share/exgen/");
    SetVar("configuration", "executable-path", "/bin/");
}

int file_exists(const char * filename)
{
    FILE * file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return -1;
    }
    return 0;
}

// pass file by reference, this func will find the file
// and set file arg to the location of the file
// eg "myscript.lua" becomes "bin/myscript.lua"
// returns 1 if file was found
// returns 0 if not found
// free's file arg before writing to it
// so file arg must be dynamically allocated
char * findfile(char ** file) {
    char * tmpfile = NULL;
    char cwd[1024] = "";
    int size = 1023;
    char * retval = NULL;
    int i = 0;

    //printf("findfile %s\n", *file);
    
    
    // check current working directory, method 1
    asprintf(&tmpfile, "./%s", *file);
    //printf("test: %s\n", tmpfile);
    if (file_exists(tmpfile)) {
        //free(*file);
        *file = tmpfile;
        return 1;
    }
    free(tmpfile);

    // check as is
    if (file_exists(*file)) {
	return 1;
    }
    
    // check current working directory, method 2
    retval = getcwd(cwd, size);
    if (retval != NULL) {
	asprintf(&tmpfile, "%s/%s", cwd, *file);
	//printf("test: %s\n", tmpfile);
	if (file_exists(tmpfile)) {
	    //free(*file);
	    *file = tmpfile;
	    return 1;
	}
	free(tmpfile);	
    }
    
    
    // check executable path
    asprintf(&tmpfile, "%s%s", path, *file);
    if (file_exists(tmpfile)) {
	//free(*file);
	*file = tmpfile;
	return 1;
    }
    free(tmpfile);

    // check installed path
    asprintf(&tmpfile, "%smodules/%s", GetVar("configuration", "resource-path"),  *file);
    //printf("Checking %s...", tmpfile);
    if (file_exists(tmpfile)) {
	//free(*file);
	*file = tmpfile;
	//printf("found.\n", tmpfile);
	return 1;
    }
    //printf("NOT found.\n", tmpfile);
    free(tmpfile);

    // check module support paths
    for(i = 0; i < modulecount; i += 1) {
	if (modules[i] != NULL) {
	    asprintf(&tmpfile, "%smodules/%s_support/%s", GetVar("configuration", "resource-path"),  modules[i]->name, *file);
	    //printf("Checking %s...", tmpfile);
	    if (file_exists(tmpfile)) {
		//free(*file);
		*file = tmpfile;
		//printf("found.\n", tmpfile);
		return 1;
	    }
	    //printf("NOT found.\n", tmpfile);
	    free(tmpfile);
	}
    }

    // check /etc
    asprintf(&tmpfile, "/etc/%s", *file);
    if (file_exists(tmpfile)) {
	//free(*file);
	*file = tmpfile;
	return 1;
    }
    free(tmpfile);
    
    
    // check /bin/exgen_modules/	// legacy support
    asprintf(&tmpfile, "/bin/exgen_modules/%s", *file);
    if (file_exists(tmpfile)) {
	//free(*file);
	*file = tmpfile;
	return 1;
    }
    free(tmpfile);
    
    return 0;
}

char * getpath(void) {
    return path;
}

int cout(char * text) {
/*    char * tmpmsg = NULL;
    if (coremsg != NULL) {
	asprintf(&tmpmsg, "%s%s", coremsg, text);
	free(coremsg);
	coremsg = tmpmsg;
	printf("%s", text);
    } else {
	asprintf(&coremsg, "%s", text);
	printf("%s", text);    
    }
    cmsg_len = strlen(coremsg);*/
    printf("%s", text);
    return 0;
}

int filemtime(char *fname) {
    struct stat stb;
    if (findfile(&fname)) {
	if (stat(fname, &stb) < 0) {
	    fprintf(stderr, "Error - stat of %s failed!\n", fname);
	    return -1;
	}
	return (stb.st_mtime);
    } else {
	return -1;
    }
}

int exgen_shutdown(void) {
    int i = 0;
    int retval = 0;
    //cout("Unloading modules...\n");
    if (modulecount > 0) {
	for (i = modulecount - 1; i >= 0; i -= 1) {
	    if (modules[i] != NULL) {
		mod_unload(modules[i]->name);
	    }
	}
    }

    exit(0);
}


int lua_runscript (char *file ) {
    char *tmpfile = NULL;
    char buff[256];
    int err = 0;
    
    //printf("lua_runscript: %s\n", file);
    //asprintf(&tmpfile, "%s%s", path, file);
    //free(file);
    //file = tmpfile;

    
    lua_State* L;
    /* initialize Lua */
    L = lua_open();
    

    /* load various Lua libraries */
    lua_baselibopen(L);
    luaopen_table(L);
    luaopen_io(L);
    luaopen_string(L);
    luaopen_math(L);
    lua_init(L);    
    
    lua_newdofile(L, file);
    /*FILE *fileh = fopen(file, "r");
    while (fgets(buff, sizeof(buff), fileh) != NULL) {
	cout("code: ");
	cout(buff);
	cout("\n");
	err = luaL_loadbuffer(L, buff, strlen(buff), file) ||
                lua_pcall(L, 0, 0, 0);
	if (err) {
	    cout(lua_tostring(L, -1));
	    cout("\n");
	    lua_pop(L, 1);
	}
    }
    fclose(fileh);*/
    /* cleanup Lua */
    lua_close(L);

    return 0;
}

void *lua_runscript_reuse (void * L, char *file ) {
    char *tmpfile = NULL;
    char *string = NULL;
    char buff[256];
    int err = 0;

    if (L == NULL) {
        L = lua_open();
	

	/* load various Lua libraries */
	lua_baselibopen(L);
        luaopen_table(L);
	luaopen_io(L);
        luaopen_string(L);
	luaopen_math(L);
	
	lua_init(L);
    }


    //tmpfile = findfile(file);
    /*string = openfile(tmpfile);
    err = luaL_loadbuffer(L, string, strlen(string), string) ||
	    lua_pcall(L, 0, 0, 0);
    
    if (err) {
	cout(lua_tostring(L, -1));
	cout("\n");
	lua_pop(L, 1);
    }*/
    
    lua_newdofile(L, file);

    if (tmpfile != NULL) {
	free(tmpfile);
	tmpfile = NULL;
    }
    return L;
}

void lua_lua_dofile(struct luathread_data * lua_data) {
//    printf("thread is born %p\n", pthread_self());
    lua_newdofile(lua_data->L, lua_data->file);
//    printf("thred is ded %p\n", pthread_self());
    lua_close(lua_data->L);
    free(lua_data);
}

void lua_lua_dofile_reuse(struct luathread_data * lua_data) {
//    printf("thread is born %p\n", pthread_self());
    lua_newdofile(lua_data->L, lua_data->file);
//    printf("thred is ded %p\n", pthread_self());
//    lua_close(lua_data->L);
//    free(lua_data);
}

void lua_lua_dostring_reuse(struct luathread_data * lua_data) {
//    printf("thread is born %p\n", pthread_self());
    lua_newdostring(lua_data->L, lua_data->file);
//    printf("thred is ded %p\n", pthread_self());
//    lua_close(lua_data->L);
//    free(lua_data);
}


void *lua_runscript_thread_reuse (void * L, char *file ) {
    
    char *tmpfile = NULL;
    char buff[256];
    int err = 0;
    pthread_attr_t tattr;
    pthread_t tid;
    int ret;
    int status;
    
    findfile(&file);

    if (L == NULL) {
        L = lua_open();
	/* load various Lua libraries */
	lua_baselibopen(L);
        luaopen_table(L);
	luaopen_io(L);
        luaopen_string(L);
	luaopen_math(L);
	
	lua_init(L);
    }

    struct luathread_data *lua_data = calloc(1, sizeof(struct luathread_data));
    lua_data->L = L;
    lua_data->file = file;

    int retval = 0;
    
    pthread_attr_init(&tattr);
    
    retval = pthread_attr_setschedpolicy(&tattr, SCHED_OTHER);
    
//    printf("Spawning thread\n");
    ret = pthread_create(&tid, &tattr, lua_lua_dofile_reuse, lua_data);
    pthread_attr_destroy(&tattr);
    
    pthread_detach(tid);
    
    

    return L;
}


void *lua_runstring_thread_reuse (void * L, char *file ) {
    
    char *tmpfile = NULL;
    char buff[256];
    int err = 0;
    pthread_attr_t tattr;
    pthread_t tid;
    int ret;
    int status;
    
    if (L == NULL) {
        L = lua_open();
	/* load various Lua libraries */
	lua_baselibopen(L);
        luaopen_table(L);
	luaopen_io(L);
        luaopen_string(L);
	luaopen_math(L);
	
	lua_init(L);
    }

    struct luathread_data *lua_data = calloc(1, sizeof(struct luathread_data));
    lua_data->L = L;
    lua_data->file = file;

    int retval = 0;
    
    pthread_attr_init(&tattr);
    
    retval = pthread_attr_setschedpolicy(&tattr, SCHED_OTHER);
    
    printf("Spawning thread\n");
    ret = pthread_create(&tid, &tattr, lua_lua_dostring_reuse, lua_data);
    pthread_attr_destroy(&tattr);
    
    pthread_detach(tid);
    
    

    return L;
}


int lua_runscriptthread(char * file) {
    pthread_attr_t tattr;
    pthread_t tid;
    int ret;
    int status;

/*    
struct sched_param param;
int priority;
sched_param.sched_priority = priority;

policy = SCHED_OTHER;
ret = pthread_setschedparam(tid, policy, &param);
*/    
    char *newfile = malloc(sizeof(file));
    newfile = strcpy(newfile, file);
    //printf(">>%s\n", newfile);
    ret = pthread_create(&tid, NULL, lua_runscript, newfile);
    pthread_detach(tid);
    //lua_runscript(file);
}



/*int lua_dostring (lua_State *L, char *string) {
    int err = 0;
    err = luaL_loadbuffer(L, string, strlen(string), string) ||
    lua_pcall(L, 0, 0, 0);
    return 0;
}*/

int lua_runstring (char *string ) {
    lua_State* L;
    int err = 0;
    /* initialize Lua */
    L = lua_open();
    

    /* load various Lua libraries */
    lua_baselibopen(L);
    luaopen_table(L);
    luaopen_io(L);
    luaopen_string(L);
    luaopen_math(L);
    lua_init(L);
    
    //err = lua_dostring(L, string);
    err = luaL_loadbuffer(L, string, strlen(string), string) ||
	    lua_pcall(L, 0, 0, 0);
    
    if (err) {
	cout(lua_tostring(L, -1));
	cout("\n");
	lua_pop(L, 1);
    }
    
    /* cleanup Lua */
    lua_close(L);

    return 0;
}

void *lua_runstring_reuse (void * L, char *string ) {
    int err = 0;
    if (L == NULL) {
        L = lua_open();
	

	/* load various Lua libraries */
	lua_baselibopen(L);
        luaopen_table(L);
	luaopen_io(L);
        luaopen_string(L);
	luaopen_math(L);
	lua_init(L);
    }
    
    err = luaL_loadbuffer(L, string, strlen(string), string) ||
	    lua_pcall(L, 0, 0, 0);
    
    if (err) {
	cout(lua_tostring(L, -1));
	cout("\n");
	lua_pop(L, 1);
    }
    
    return L;
}


int lua_runstringthread(char * file) {
    pthread_attr_t tattr;
    pthread_t tid;
    int ret;
    int status;

    //char *newfile = malloc(sizeof(file));
    //newfile = strcpy(newfile, file);
    //printf(">>%s\n", newfile);
    ret = pthread_create(&tid, NULL, lua_runstring, file);
    pthread_detach(tid);
    //lua_runscript(file);
}


// lua wrapper functions
// lw_ prefix
// Modules might need to call Lua functions. Instead of including lua.h they can call these functions instead
void lw_register(lua_State * L, char * string,  double (*callback)(void)) {
    lua_register(L, string, callback);
}

int lw_toboolean(lua_State * L, int index) {
    return lua_toboolean(L, index);
}


char * lw_tostring(lua_State * L, int index) {
    return lua_tostring(L, index);
}

float lw_tonumber(lua_State * L, int index) {
    return (float)lua_tonumber(L, index);
}

void * lw_topointer(lua_State * L, int index) {
    return lua_topointer(L, index);
}

void * lw_tothread(lua_State * L, int index) {
    return lua_tothread(L, index);
}

void lw_pushboolean(lua_State * L, int value) {
    lua_pushboolean(L, value);
}


void lw_pushlstring(lua_State * L, char *s, size_t len) {
    lua_pushlstring(L, s, len);
}

void lw_pushnumber(lua_State * L, float value) {
    lua_pushnumber(L, value);
}
void lw_pushstring(lua_State * L, char * value) {
    lua_pushstring(L, value);
}

void slave(void) {
    char slave_script[] =
"l_sock = listen(getvar(\"configuration\", \"slave-port\"), getvar(\"configuration\", \"slave-address\"));\n\
while 1 == 1 do\n\
    sock = accept(l_sock);\n\
    --echo('got connection\\n');\n\
    data = nil;\n\
    fulldata = '';\n\
    while data ~= '' do\n\
	data = recv(sock);\n\
	if (data ~= nil) then\n\
	    fulldata = fulldata..data;\n\
	end\n\
    end\n\    
    --echo(fulldata);\n\
    run_string(fulldata);\n\
    --send(sock, getvar(\"app\", \"result\"));\n\
    sendall(sock, \"foo\");\n\
    close(sock);\n\
end\n";
    //printf("%s\n", slave_script);
    //lua_runstring(slave_script);

    int sin_size;
    struct sockaddr_in their_addr;
    
    int sock = 0;
    //atoi(GetVar("configuration", "slave-port"))
    // GetVar("configuration", "slave-ip")
    int l_sock = s_listen(atoi(GetVar("configuration", "slave-port")), GetVar("configuration", "slave-ip"));
    //printf("Listening for commands on port %s, address %s\n", GetVar("configuration", "slave-port"), GetVar("configuration", "slave-ip"));
    char * cmd = NULL;
    for (;;) {
	sin_size = sizeof(their_addr);
	sock = accept(l_sock, (struct sockaddr *)&their_addr, &sin_size);
	printf("sock: %i\n", sock);
	perror("accept");
	cmd = s_mrecv(sock);
	if (cmd != NULL) {
	    if (strlen(cmd) > 0) {
		lua_runstring(cmd);
	    }
	    free(cmd);
	    //GetVar("app", "result")
	    //printf("%s\n", GetVar("app", "result"));
	    s_msend(sock, GetVar("app", "result"));
	    close(sock);
	}
    }
    close(l_sock);
}

void interactive_console(void) {
    char *input;
    char buffer[256];
    int bail = 0;

    do {
        printf(GetVar("configuration", "prompt"));
        input = fgets(buffer, 256, stdin);
        if (input != NULL) {
    	    lua_runstring(input);
        }
    } while (!bail);
}

void interactive_console_thread(void) {
    pthread_attr_t tattr;
    pthread_t tid;
    int ret;
    int status;

    ret = pthread_create(&tid, NULL, interactive_console, NULL);
    pthread_detach(tid);
}

void handler(int sig)
{
    pid_t pid;
    pid = wait(NULL);

//  printf("Pid %d exited.\n", pid);
}

// main

int main (int argc, char *argv[]) {
    void *handle;
    void *handle2;
    char *file = NULL;
    char *input;
    char buffer[256];
    self_name = malloc(strlen(argv[0]));
    int bail = 0;
    int interactive = 0;
    
    char *left_tuple = NULL;
    char *right_tuple = NULL;
    int e = 0;
    int fsize = 0;

    // catch SIGINT and exit cleanly
    struct sigaction act;
    act.sa_handler = catch_signal;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);

    // catch child process exits
    signal(SIGCHLD, handler);


    self_name = strcpy(self_name, argv[0]);
    //printf("%s\n", self_name);
    path = argv[0];

    int c = 0;
    int i = 0;
    for (i = strlen(path);i > 0; i--) {
	c = path[i];
	if (c == 47) {
	    if (i < strlen(path)) {
		i = i + 1;
		char *tmp_nameonly = NULL;
		tmp_nameonly = &path[i];
		self_nameonly = malloc(sizeof(tmp_nameonly));
		self_nameonly = strcpy(self_nameonly, tmp_nameonly);
		path[i] = 0;		
		i = i - 1;
		break;
	    }
	}
    }

    // extract any embedded data if present
/*    if (extract_embedded()) {    
	printf("embedded data extracted\n");
	char *em_scriptname = malloc(sizeof(self_nameonly) + 4 + sizeof(em_tmpname));
	sprintf(em_scriptname, "%s/%s.lua", em_tmpname, self_nameonly);
	
	printf("Lua Script: %s\n", em_scriptname);	
	char *em_scriptdata = zopenfile(em_scriptname);
	//printf("embedded script data: %s\n", em_scriptdata);
	unzip(em_tmpname, "unzip/");
    }
*/
    // global DB, load config file    
    //	checks for .exgenrc first
    //  then exgenrc
    DB = lua_open();    
    config_defaults();  // important to call this before lua_init or findfile            

    asprintf(&file, ".exgenrc");
    int config_found = findfile(&file);
    if (config_found) {    
	lua_runscript(file);
	free(file);
    } else {
	free(file);
	asprintf(&file, "exgenrc");
	config_found = findfile(&file);
	if (config_found) {    
	    lua_runscript(file);
	    free(file);
	}
    }
    

    // process arguments    
    if (argc == 1) {
	if (em_embedded == 0) {
	    interactive = 1;
	}
    }
    for(i = 1; i <= argc - 1; i += 1) {
	//printf(">%s\n", argv[i]);
	//printf("%s = -c: %i\n", argv[i], strcmp("-c", argv[i]));	
	if (strcmp("-l", argv[i]) == 0) {
	    if (i + 1 <= argc - 1) { 
		i += 1;
		lua_runstring(argv[i]);
	    }
	} else if (strcmp("-i", argv[i]) == 0) {
	    interactive = 1;
	} else if (strcmp("-r", argv[i]) == 0) {
	    lua_runstring("help()");
	    interactive = 1;
	} else if (strcmp("-v", argv[i]) == 0) {
	    printf("%s\n", VERSION_STRING);
	    interactive = 0;
	} else if (strcmp("-update", argv[i]) == 0) {
	    lua_runstring("exgen_update(1)");
	} else if (strcmp("-install", argv[i]) == 0) {
	    lua_runstring("exgen_install(1)");	    
	} else if ((strcmp("-h", argv[i]) == 0) || (strcmp("--help", argv[i]) == 0)) {
	    printf("\nexgen platform version %s, an extensible application platform and lua script \ninterpreter.\n\nUsage: exgen [OPTION...] [MODULE...] [SCRIPT...] \n\n    -i			Run interactively\n    -l <code>		Run lua <code>\n    -h			Display this help\n    -v			Version number\n    -verbose=1		Enable verbose output\n", VERSION_STRING);
	    printf("    -r			Output command reference\n");
	    printf("    -update		Download & Install latest version\n");	    
	    printf("    -install		Perform installation routine\n");	    
	    printf("\n");
	    
	} else if (argv[i][0] == 45) {
	    left_tuple = &argv[i][1];
	    right_tuple = "\0";
	    for (e = 0; e <= strlen(left_tuple); e += 1) {
		//printf("%i\n", e);
		if (left_tuple[e] == 61) {
		    if (e + 1 < strlen(left_tuple)) {
			left_tuple[e] = 0;
			e += 1;
			right_tuple = &left_tuple[e];
		    }
		    break;
		}
	    }
	    //printf("left tuple: %s\n", left_tuple);
	    //printf("right tuple: %s\n", right_tuple);	    
	    if (strcmp(right_tuple, "") == 0) {
		SetVar("configuration", left_tuple, "1");
	    } else {
		SetVar("configuration", left_tuple, right_tuple);
	    }
	}
    }
    for(i = 1; i <= argc - 1; i += 1) {    
	if (argv[i][0] == 45) {    
	} else {
	    asprintf(&file, "%s", argv[i]);
	    if (findfile(&file)) {
		if (strcasestr(file, ".lua") == NULL) {
		    if (strcasestr(file, ".exgen") == NULL) {
			mod_load(file);
		    } else {
			lua_runscript(file);
		    }	    
		} else {
		    lua_runscript(file);
		}	    


	    }
	    free(file);
	}	
    }
    if (strcmp("1", GetVar("configuration", "slave")) == 0) {
	slave();
    }
    // run interactive mode
    if (interactive == 1) {
	printf("exgen platform version %s\nRunning in interactive mode.\nEnter help() for command reference.\n", VERSION_STRING);
	interactive_console();
    }
    return 0;
    
    lua_close(DB);
    pthread_exit(0);
}

