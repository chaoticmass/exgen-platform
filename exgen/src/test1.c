#include "lua.h"
#include <stdlib.h>

int l_test(void * L) {
    printf("test 1.0\n");
    return 0;
}

void test_lua_register_call(void * L) {
    lua_register(L, "test", l_test);
}

int _init_thread(char * mod_name) {
    mod_lua_register(mod_name, test_lua_register_call);
    return 0;
}
