#define VERSION_STRING	"1.00A"
#define ARCH_STRING	"linux_x86"

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include "lua.h"

int l_initialize_video(void * L) {
    printf("Initialize Video\n");
    
    return 0;
}

void illution_lua_register_call(void * L) {
    lua_register(L, "initialize_video", l_initialize_video);
}

int _init_thread(char * mod_name) {
    mod_lua_register(mod_name, illution_lua_register_call);
    SetVar(mod_name, "version", VERSION_STRING);
    SetVar(mod_name, "arch", ARCH_STRING);
    
    return 0;
}