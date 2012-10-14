
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

