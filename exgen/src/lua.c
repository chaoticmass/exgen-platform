
// Lua functions
// l_ prefix
// Functions called by lua scripts

int l_md5_hash(void * L) {
    char *str = lua_tostring(L, 1);
    int length = lua_strlen(L, 1);
    int n;
    MD5_CTX c;
    unsigned char digest[16];
    char *out = (char*)malloc(33);

    MD5_Init(&c);

    while (length > 0) {
        if (length > 512) {
            MD5_Update(&c, str, 512);
        } else {
            MD5_Update(&c, str, length);
        }
        length -= 512;
        str += 512;
    }

    MD5_Final(digest, &c);

    for (n = 0; n < 16; ++n) {
        snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)digest[n]);
    }

    lua_pushstring(L, out);
    return 1;
}

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
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
	printf("Error setting socket option: %d\n", errno);
    }
    
    if (listen(sockfd, 25)) {
	printf("Error listening to socket: %d\n", errno);
    }
    
    
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
    lua_pushnumber(L, close(sockfd));
    return 1;
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
//    printf("NUMBYTES: %i\n", numbytes);
    if (numbytes > 0) {
	buf[numbytes] = 0;
	lua_pushstring(L, buf);
	return 1;
    } else {
	//lua_pushnumber(L, numbytes);
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
    
    bytesleft = lua_strlen(L, 2);
    start = bytesleft;
    
    while (total < start) {
	n = send(sockfd, send_string+total, bytesleft, 0);
	if (n == -1) {
	    break;
	}
	total += n;
	bytesleft -= n;
    }
    lua_pushnumber(L, n==-1?-1:0); // -1 on failure, 0 on success
    return 1;
}

int l_send(lua_State * L) { // sockfd, string
    int sockfd;
    char *send_string = NULL;
    size_t string_length = 0;
    
    sockfd = lua_tonumber(L, 1);
    send_string = lua_tostring(L, 2);
    string_length = lua_strlen(L, 2);
//    printf("About to send Sockfd: %i\n", sockfd);
    lua_pushnumber(L, send(sockfd, send_string, string_length, MSG_NOSIGNAL));
//    printf("done %i\n", sockfd);
    return 1;
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


int l_filemtime(lua_State * L) {
    int n = lua_gettop(L);
    char *string = lua_tostring(L, 1);
    
    lua_pushnumber(L, filemtime(string));
    return 1;
}

int l_cout(lua_State * L) {
    int n = lua_gettop(L);
    
    char *string = lua_tostring(L, 1);
    //cout(string);
    //printf("%p: %s", pthread_self(), string);
    printf("%s", string);
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
//    printf("Listing directory: %s\n", path);
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
    char *m = lua_tostring(L, 2);
    
    chmod(file, strtol(m, 0, 8));
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
    int length = lua_strlen(L, 2);
    //printf("Saving %s!\n", file);
    FILE *h = fopen(file, "w");
    if (h == NULL) { 
	//printf("FAILED TO SAVE FILE!~!!!!\n");
    } else {
	//fputs(string, h);
	fwrite(string, length, 1, h);
	fclose(h);
    }
    return 0;
}


int l_openfile(lua_State * L) {
    int n = lua_gettop(L);
    
    char *name = lua_tostring(L, 1);
    if (findfile(&name)) {
//	printf("Name: %s\n", name);

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
//	printf("FAIL\n");
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
//    printf("\nNew Lua State...\n");
    nL = lua_open();

    lua_baselibopen(nL);
    luaopen_table(nL);
    luaopen_io(nL);
    luaopen_string(nL);
    luaopen_math(nL);
    lua_init(nL);
//    printf("done\n");    
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

int l_thread_id(lua_State * L) {
    lua_pushnumber(L, pthread_self());
    return 1;
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

//    lua_pushliteral(L, "db_results");
//    lua_newtable(L);
//    lua_settable(L, LUA_GLOBALSINDEX);
    
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
    lua_register(L, "md5_hash", l_md5_hash);
    lua_register(L, "thread_id", l_thread_id);
    
    lua_register(L, "db_open", l_db_open);
    lua_register(L, "db_close", l_db_close);
    lua_register(L, "db_exec", l_db_exec);
    
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
