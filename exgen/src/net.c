
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

