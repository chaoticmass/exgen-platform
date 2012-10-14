
int l_calibrate_scale(void * L) {
    char *scale_file = lua_tostring(L, 1);
    lua_pushnumber(L, calibrate_scale(scale_file));
    return 1;
}

int l_read_scale(void * L) {
    char *scale_file = lua_tostring(L, 1);
    lua_pushnumber(L, read_scale(scale_file, lua_tonumber(L, 2)));
    return 1;
}

void usbscale_lua_register_call(void * L) {
    lua_register(L, "calibrate_scale", l_calibrate_scale);
    lua_register(L, "read_scale", l_read_scale);
}

int _init_thread(char * mod_name) {
    mod_lua_register(mod_name, usbscale_lua_register_call);
    SetVar(mod_name, "version", VERSION_STRING);
    SetVar(mod_name, "arch", ARCH_STRING);

    return 0;
}