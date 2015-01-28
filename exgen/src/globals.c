#define VERSION_STRING ""
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
#include <openssl/md5.h>

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
