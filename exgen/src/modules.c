

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
