
int l_db_open(lua_State * L) {
    sqlite3 *db;
    int retval;
    
    retval = sqlite3_open(lua_tostring(L, 1), &db);
    if (retval){
	sqlite3_close(db);
	return 0;
    } else {
	lua_pushlightuserdata(L, db);
	return 1;
    }
}

int l_db_close(lua_State * L) {
    sqlite3_close(lua_touserdata(L, 1));
    return 0;
}

static int db_callback(lua_State *L, int argc, char **argv, char **az_col_name) {
    int i;
    int tindex = 0;
    int rindex = 0;
    int eindex = 0;
    int rows = 0;
    lua_pushliteral(L, "db_results");
    tindex = lua_gettop(L);
    lua_gettable(L, LUA_GLOBALSINDEX);
    
    lua_pushstring(L, "rows");
    eindex = lua_gettop(L);
    lua_gettable(L, tindex);
    rows = lua_tonumber(L, eindex);

    rows = rows + 1;
    lua_pushstring(L, "rows");
    lua_pushnumber(L, rows);
    lua_settable(L, tindex);
    
    lua_pushnumber(L, rows);
    lua_newtable(L);
    lua_settable(L, tindex);

    lua_pushnumber(L, rows);
    rindex = lua_gettop(L);
    lua_gettable(L, tindex);
    for(i=0; i<argc; i++){
	lua_pushstring(L, az_col_name[i]);
	lua_pushstring(L, argv[i]);
	lua_settable(L, rindex);
    }
    lua_pop(L, 3);
    return 0;
}


int l_db_exec(lua_State * L) {
    char *db_error = 0;
    int retval;
    int tindex = 0;
    sqlite3 *db = lua_touserdata(L, 1);

    lua_pushliteral(L, "db_results");
    lua_newtable(L);
    lua_settable(L, LUA_GLOBALSINDEX);

    lua_pushliteral(L, "db_results");
    tindex = lua_gettop(L);
    lua_gettable(L, LUA_GLOBALSINDEX);

    lua_pushstring(L, "rows");
    lua_pushnumber(L, 0);
    lua_settable(L, tindex);

    retval = sqlite3_exec(db, lua_tostring(L, 2), db_callback, L, &db_error);
    if (retval != SQLITE_OK) {
	fprintf(stderr, "DB Error: %s\n", db_error);
	sqlite3_free(db_error);
	return 0;
    } else {
	return 0;
    }
    
}