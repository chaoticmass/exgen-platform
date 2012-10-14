// dummy module functions. Used with exgen-core only.

// Module functions
// mod_ prefix
// Functions related to loading/unloading and managing Modules

/*
create_module makes a new blank entry in the module registry and returns
an index to the entry
*/
int create_module(void) {
    return 0;
}

/*
delete_module removes an entry from the module registry
*/
void delete_module(int index) {
}

void * mod_load(char * file) {
}

/* returns the registry index of the module. -1 means error */
int * mod_loader(char * file, int flags) {
}

void * mod_unload(char * name) {
}

void * mod_lua_register(char *name, double (*callback)(void * L)) {
}


int mod_fail(void) {
}

int mod_proc(char * name, char * proc) {
}

int mod_isloaded(char * name) {
}


int mod_loadthread(char * file) {
}
