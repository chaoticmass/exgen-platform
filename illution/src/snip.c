
int call_on_draw_recursive(int root) {
    int index = 0;
    if (check_surface(root)) {
	if (surfaces[root]->dirty) {
	    //printf("\n\n\nrecursive call_on_draw(%i)\n", root);
	    call_on_draw(root);

    	    for (index = 0; index <= surfacecount; index += 1) {
		if (check_surface(index)) {
		    if (index != root) {
			if (surfaces[index]->parent == root) {
			    call_on_draw_recursive(index);
			}
		    }
		}
	    }
	}
    }
    
    return 0;
}


int call_on_draw(int index) {
    void * L = NULL;
    double start = 0;
    if (check_surface(index)) {
	if (surfaces[index]->on_draw != -1) {
/*	    if (!pthread_equal(pthread_self(), surfaces[index]->thread_id)) {
		//printf("\nnot the same thread for on_draw %i\n", index);
		if (surfaces[index]->parent == rootsurface) {
		    //printf("\nasking thread to draw for me %i\n", index);
		    surfaces[index]->outside_thread_on_draw_request = 1;
		    start = timer();
		    while(surfaces[index]->outside_thread_on_draw_request != 0) {
			pthread_yield();
			if(timer() - start > .1) {
			    break;
			}
			if (!check_surface(index)) {
			    break;
			}
		    }
		    //printf("\nback %i\n", index);
		}
	    } else {*/
	
		//int lockval = pthread_mutex_trylock(surfaces[index]->lua_mutex);
		int lockval = lua_trylock(surfaces[index]->L);
		if (lockval != 0) {
		    if (debug_mode) {
			//printf("locked on_draw, surface = %i\n", index);
		    }
		    //int eid = create_event(index, call_on_draw, "on_draw");
		    if (debug_mode) {
			//printf("exiting draw\n");
		    }
		    return 0;
		}
		    L = surfaces[index]->L;
		    //lua_getref(L, LUA_REGISTRYINDEX, surfaces[index]->on_draw);
		    lua_pushnumber(L, surfaces[index]->on_draw);
		    lua_gettable(L, LUA_REGISTRYINDEX);	    
		    /*
		    if (lua_pcall(L, 0, 0, 0) != 0) {
    		    //	error(L, "error: %s", lua_tostring(L, -1));
			printf("error: %s\n", lua_tostring(L, -1));
		    }
		    //lua_call(L, 0, 0);
		    */
		    safe_call(L, 0);
	    
		//pthread_mutex_unlock(surfaces[index]->lua_mutex);
		lua_unlock(surfaces[index]->L);
	    //}
	}
    }
}

int call_on_got_focus(int index) {
    void * L = NULL;
    if (check_surface(index)) {
	if (surfaces[index]->on_got_focus != -1) {
	//int lockval = pthread_mutex_trylock(surfaces[index]->lua_mutex);
	int lockval = lua_trylock(surfaces[index]->L);
	if (lockval != 0) {
	    if (debug_mode) {
		printf("locked on_got_focus\n");
	    }
	    int eid = create_event(index, call_on_got_focus, "on_got_focus");	    
	    return 0;
	}
	
	    L = surfaces[index]->L;
	    //lua_getref(L, LUA_REGISTRYINDEX, surfaces[index]->on_draw);
	    lua_pushnumber(L, surfaces[index]->on_got_focus);
	    lua_gettable(L, LUA_REGISTRYINDEX);	    
	    /*
	    if (lua_pcall(L, 0, 0, 0) != 0) {
    	    //	error(L, "error: %s", lua_tostring(L, -1));
		printf("error: %s\n", lua_tostring(L, -1));
	    }	    
	    //lua_call(L, 0, 0);
	    */
	    safe_call(L, 0);
//	    printf("returned from on got focus %i\n", index);
	//pthread_mutex_unlock(surfaces[index]->lua_mutex);
	lua_unlock(surfaces[index]->L);
	}
    }
}

int call_on_lost_focus(int index) {
    void * L = NULL;
    if (check_surface(index)) {
	if (surfaces[index]->on_lost_focus != -1) {
	//int lockval = pthread_mutex_trylock(surfaces[index]->lua_mutex);    
	int lockval = lua_trylock(surfaces[index]->L);
	if (lockval != 0) {
	    if (debug_mode) {
		printf("locked on_lost_focus\n");
	    }
	    int eid = create_event(index, call_on_lost_focus, "on_lost_focus");
	    return 0;
	}
	
	    L = surfaces[index]->L;
	    //lua_getref(L, LUA_REGISTRYINDEX, surfaces[index]->on_draw);
	    lua_pushnumber(L, surfaces[index]->on_lost_focus);
	    lua_gettable(L, LUA_REGISTRYINDEX);	    
	    /*
	    if (lua_pcall(L, 0, 0, 0) != 0) {
    	    //	error(L, "error: %s", lua_tostring(L, -1));
		printf("error: %s\n", lua_tostring(L, -1));
	    }
	    //lua_call(L, 0, 0);
	    */
	    safe_call(L, 0);
	//pthread_mutex_unlock(surfaces[index]->lua_mutex);    
	lua_unlock(surfaces[index]->L);
	}
    }
}




int call_on_keypress(int index, int inkey) {
    int keycode = inkey >> 8;
    int keyval = inkey & 0xff;
    void * L = NULL;
    if (check_surface(index)) {
	if (surfaces[index]->on_keypress != -1) {
	    L = surfaces[index]->L;
	    if (L != NULL) {
	    //int lockval = pthread_mutex_trylock(surfaces[index]->lua_mutex);
	    int lockval = lua_trylock(surfaces[index]->L);
	if (lockval != 0) {
	    if (debug_mode) {
		printf("locked on_keypress\n");
	    }
	    int eid = create_event(index, call_on_keypress, "on_keypress");
	
	    return 0;
	}
	    
		lua_pushnumber(L, surfaces[index]->on_keypress);
		lua_gettable(L, LUA_REGISTRYINDEX);
	    
		lua_pushnumber(L, keycode);
		lua_pushnumber(L, keyval);
		/*
		if (lua_pcall(L, 2, 0, 0) != 0) {
		    printf("error: %s\n", lua_tostring(L, -1));
    		//    error(L, "error: %s", lua_tostring(L, -1));
		}
		//lua_call(L, 2, 0);
		*/
		safe_call(L, 2);
		if (debug_mode) {
		    printf("Keycode %i, Keyscan %i\n", keycode, keyval);
		}
	    //pthread_mutex_unlock(surfaces[index]->lua_mutex);
	    
	    if (debug_mode) {
		printf("Returned from on_keypress, checking to see if surface %i still exists...", index);
	    }
	    if (check_surface(index)) {	    
		if (debug_mode) {
		    printf("It does. Unlocking surface.\n");
		}
		lua_unlock(surfaces[index]->L);
	    } else {
		if (debug_mode) {
		    printf("It does NOT.\n");
		}	    
	    }
	    
	    }
	}
    }
}

int call_on_mouse_down(int index, int button) {
    int ox = 0;
    int oy = 0;
    int screen_x = 0;
    int screen_y = 0;
    void * L = NULL;
    if (check_surface(index)) {
	if (surfaces[index]->on_mouse_down != -1) {
	    L = surfaces[index]->L;
	    if (L != NULL) {
	    //int lockval = pthread_mutex_trylock(surfaces[index]->lua_mutex);
	    int lockval = lua_trylock(surfaces[index]->L);
	    if (lockval != 0) {
		if (debug_mode) {
		    printf("locked on_mouse_down\n");
		}
		//int eid = create_event(index, call_on_mouse_down, "on_mouse_down");
		return 0;
	    }
	    
	    
		lua_pushnumber(L, surfaces[index]->on_mouse_down);
		lua_gettable(L, LUA_REGISTRYINDEX);
	    
		lua_pushnumber(L, button);
		if (surfaces[index]->parent != -1) {
		    screen_x = get_surface_screen_x(surfaces[index]->parent);
		    screen_y = get_surface_screen_y(surfaces[index]->parent);
		} else {
		    screen_x = get_surface_screen_x(surfaces[index]);
		    screen_y = get_surface_screen_y(surfaces[index]);		
		}
		ox = mouse_x - screen_x;
		oy = mouse_y - screen_y;
		
		lua_pushnumber(L, ox);
		lua_pushnumber(L, oy);

		/*
		if (lua_pcall(L, 3, 0, 0) != 0) {
    		//    error(L, "error: %s", lua_tostring(L, -1));
		    printf("error: %s\n", lua_tostring(L, -1));
		}
		//lua_call(L, 3, 0);
		*/
		safe_call(L, 3);
		//printf("Mouse Down: %i\n", button);
	    //pthread_mutex_unlock(surfaces[index]->lua_mutex);
	    lua_unlock(surfaces[index]->L);
	    }
	}
    }
}

int call_on_mouse_up(int index, int button) {
    int ox = 0;
    int oy = 0;
    int screen_x = 0;
    int screen_y = 0;

    void * L = NULL;
    if (check_surface(index)) {
	if (!pthread_equal(pthread_self(), surfaces[index]->thread_id)) {
	    if (debug_mode) {
		printf("not the same thread for mouse up %p, %p\n", pthread_self(), surfaces[index]->thread_id);
	    }
	    int eid = create_event(index, call_on_mouse_up, "on_mouse_up");

	    return 0;
	    
	    //return -1;
	}
	if (debug_mode) {
	    printf("IT IS same thread for mouse up\n");
	}
	if (surfaces[index]->on_mouse_up != -1) {
	    L = surfaces[index]->L;
	    if (L != NULL) {
	    //int lockval = pthread_mutex_trylock(surfaces[index]->lua_mutex);
	    int lockval = lua_trylock(surfaces[index]->L);
	if (lockval != 0) {
	    if (debug_mode) {
		printf("locked on_mouse_up\n");
	    }
	    int eid = create_event(index, call_on_mouse_up, "on_mouse_up");

	    return 0;
	}
	    
	    
		lua_pushnumber(L, surfaces[index]->on_mouse_up);
		lua_gettable(L, LUA_REGISTRYINDEX);
	    
		lua_pushnumber(L, button);
		if (surfaces[index]->parent != -1) {
		    screen_x = get_surface_screen_x(surfaces[index]->parent);
		    screen_y = get_surface_screen_y(surfaces[index]->parent);
		} else {
		    screen_x = get_surface_screen_x(surfaces[index]);
		    screen_y = get_surface_screen_y(surfaces[index]);		
		}
		ox = mouse_x - screen_x;
		oy = mouse_y - screen_y;
		
		lua_pushnumber(L, ox);
		lua_pushnumber(L, oy);

		/*
		if (lua_pcall(L, 3, 0, 0) != 0) {
		    //error(L, "error: %s", lua_tostring(L, -1));
		    printf("error: %s\n", lua_tostring(L, -1));
		}
		//lua_call(L, 3, 0);
		*/
		safe_call(L, 3);
		//printf("back\n");
		if (check_surface(index)) {
		    //pthread_mutex_unlock(surfaces[index]->lua_mutex);
		    lua_unlock(surfaces[index]->L);
		}
	    }
	}
    }
}

int call_on_mouse_click(int index, int button) {
    int ox = 0;
    int oy = 0;
    int screen_x = 0;
    int screen_y = 0;

    void * L = NULL;
    if (check_surface(index)) {
	if (surfaces[index]->on_mouse_click != -1) {
	    L = surfaces[index]->L;
	    if (L != NULL) {
	    //int lockval = pthread_mutex_trylock(surfaces[index]->lua_mutex);
	    int lockval = lua_trylock(surfaces[index]->L);
	if (lockval != 0) {
	    if (debug_mode) {
		printf("locked on_mouse_click\n");
	    }
	    int eid = create_event(index, call_on_mouse_click, "on_mouse_click");
	
	    return 0;
	}
	    
		lua_pushnumber(L, surfaces[index]->on_mouse_click);
		lua_gettable(L, LUA_REGISTRYINDEX);
	    
		lua_pushnumber(L, button);
		if (surfaces[index]->parent != -1) {
		    screen_x = get_surface_screen_x(surfaces[index]->parent);
		    screen_y = get_surface_screen_y(surfaces[index]->parent);
		} else {
		    screen_x = get_surface_screen_x(surfaces[index]);
		    screen_y = get_surface_screen_y(surfaces[index]);		
		}
		ox = mouse_x - screen_x;
		oy = mouse_y - screen_y;
		
		lua_pushnumber(L, ox);
		lua_pushnumber(L, oy);

		/*
		if (lua_pcall(L, 3, 0, 0) != 0) {
    		    //error(L, "error: %s", lua_tostring(L, -1));
		    printf("error: %s\n", lua_tostring(L, -1));
		}
		//lua_call(L, 3, 0);
		*/
		safe_call(L, 3);
		if (debug_mode) {
		    printf("Mouse Click: %i\n", button);
		}
	    //pthread_mutex_unlock(surfaces[index]->lua_mutex);
	    
	    lua_unlock(L);
	    }
	}
    }
}


