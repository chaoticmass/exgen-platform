int run_queue_thread(pthread_t);
pthread_t ded_thred(pthread_t);



int create_event(int surface_index, double(*event_proc)(int), char* name) {
    int i = 0;
    int index = -1;
    
    if (debug_mode) {
	printf("adding event \"%s\"\n", name);
    }
    pthread_mutex_lock(&event_mutex);
    for (i = 0; i < event_count; i += 1) {
	if (events[i] != NULL) {
	    if (events[i]->index == surface_index) {
	        if (events[i]->event_proc == event_proc) {
		    if (events[i]->thread_id == surfaces[surface_index]->thread_id) {
//			printf("Not adding event %s, it already exists in queue\n", events[i]->name);
			pthread_mutex_unlock(&event_mutex);
			return i;
		    }
		}
	    }
	}
    }
    for (i = 0; i < event_count; i += 1) {
	if (events[i] == NULL) {
	    index = i;
	    break;
	}
    }
    if (index == -1) {
	event_count += 1;
	struct event ** tmp = calloc(event_count, sizeof(struct event *));
	for (i = 0; i < event_count - 1; i += 1) {
	    tmp[i] = events[i];
	}
	if (events != NULL) {
	    free(events);
	}
	index = event_count - 1;
	events = calloc(event_count, sizeof(struct event *));
	for (i = 0; i < event_count - 1; i += 1) {
	    events[i] = tmp[i];
	}
	free(tmp);
    }
    events[index] = calloc(1, sizeof(struct event));
    events[index]->index = surface_index;
    events[index]->event_proc = event_proc;
    events[index]->thread_id = surfaces[surface_index]->thread_id;
    strncpy(events[index]->name, name, 31);
    
    pthread_mutex_unlock(&event_mutex);
    return index;
}

int destroy_event(int index) {
    int i = 0;

    pthread_mutex_lock(&event_mutex);
    if (debug_mode) { 
	printf("destroy event...%s\n", events[index]->name);
    }
    if (index < event_count) {
	free(events[index]);
	events[index] = NULL;
    }
    if (debug_mode) {
	printf("done\n");
    }
    pthread_mutex_unlock(&event_mutex);
    return 0;
}


int check_event(int index) {
    if (index >= 0) {
	if (index < event_count) {
	    if (events[index] != NULL) {
		
		return -1;
	    }
	}
    }
    return 0;
}

// spawns a new thread and calls run_queue_thread
pthread_t ded_thred(pthread_t thread_id) {
    int ret = 0;
    int status = 0;
    int retval = 0;
    pthread_attr_t tattr;
    pthread_t tid;
    
//    pthread_t * thread_id_pass = calloc(1, sizeof(pthread_t));
//    thread_id_pass = thread_id;
    if (debug_mode) {
	printf("inside ded_thred Parent: %p, DedThred: %p\n", pthread_self(), thread_id);
    }
    pthread_attr_init(&tattr);
    retval = pthread_attr_setschedpolicy(&tattr, SCHED_OTHER);
    ret = pthread_create(&tid, &tattr, run_queue_thread, thread_id);
    pthread_attr_destroy(&tattr);
    pthread_detach(tid);
    
    return tid;
}

int run_queue_thread(pthread_t thread_id) {
    if (debug_mode) {
	printf("run_queue_thread. This thread: %p, Old thread: %p\n", pthread_self(), thread_id);
    }
    run_queue();
    if (debug_mode) {
	printf("run_queue_thread done %p\n", pthread_self());
    }
    return 0;
}

int run_queue(void) {
    int inkey = 0;
    int keycode = 0;// inkey >> 8;
    int keyval = 0; // inkey & 0xff;
    int surface = 0;
    int i = 0;
    int retval = 0;
    int index = 0;
    int surface_index = 0;
    int ran_event = 0;
    double (*myproc)(int);
    int v = 0;
    
    if (!pthread_equal(pthread_self(), surfaces[0]->thread_id)) {
	v = 1;
    }

    if (v) {
	if (debug_mode) {
	    printf("process_events %p\n", pthread_self());
	}
    }

    // run event queue
    pthread_mutex_lock(&event_mutex);
    for (i = 0; i <= event_count; i += 1) {
	index = i;
	if (check_event(index)) {
	    surface_index = events[index]->index;
	    //printf("	Event %i, %p, %s\n", i, events[
	    retval = pthread_setschedparam(surfaces[surface_index]->thread_id, 0, NULL);
	    if (retval == 3) {
		// this thred is ded
		if (v) {
		    if (debug_mode) {
			printf("%p, I would run this event, but the thred is ded: %p\n", pthread_self(), surfaces[index]->thread_id);
		    }
		}
		if (!pthread_equal(pthread_self(), surfaces[surface_index]->thread_id)) {
		    lua_force_unlock(surfaces[surface_index]->L);
		    ded_thred(surfaces[surface_index]->thread_id);
		} else {
		    printf("Not calling ded_thred on self\n");
		}
	    }
	    if (pthread_equal(pthread_self(), events[index]->thread_id)) {
		//if (v) { printf("%p 1\n", pthread_self()); }
		//if (lua_trylock(surfaces[surface_index]->L == 0)) {
		    //if (v) { printf("lua is unlocked %p\n", pthread_self()); }
		    *(void **) (&myproc) = events[index]->event_proc;
		    if (debug_mode) {
			printf("running event %s\n", events[index]->name);
		    }
		    ran_event = 1;		    
		    pthread_mutex_unlock(&event_mutex);
		    
		    (*myproc)(events[index]->index);
		    destroy_event(index);
		    
		    pthread_mutex_lock(&event_mutex);
		    if (debug_mode) {
			printf("done\n");
		    }
		//}
	    }
	}
    }
    pthread_mutex_unlock(&event_mutex);

    if (ran_event == 0) {
	pthread_yield();
    }
    return ran_event;
}



int process_events(void) {
    int inkey = 0;
    int keycode = 0;// inkey >> 8;
    int keyval = 0; // inkey & 0xff;
    int surface = 0;
    int i = 0;
    int retval = 0;
    int index = 0;
    double (*myproc)(int);

    //printf("process_events %p\n", pthread_self());

    // run event queue
    run_queue();

    // remove zombie surfaces
    // these are surfaces where the parent thread has ended
    for (i = 0; i <= surfacecount; i += 1) {
	index = i;
	if (check_surface(index)) {
	    retval = pthread_setschedparam(surfaces[index]->thread_id, 0, NULL);
	    if (retval == 3) {
		if (check_surface(index)) {
		    if (pthread_equal(pthread_self(), surfaces[index]->thread_id)) {
			if (debug_mode) {
			    printf("Destroying Zombie Surface[%i]\n", index);
			}
			destroy_surface(index);
		    }
		}
	    }
	}
    }
    

    
    if (keypressed()) {
//	if (pthread_equal(pthread_self(), surfaces[focus]->thread_id)) {
	    inkey = readkey();
	    keycode = inkey >> 8;
	    keyval = inkey & 0xff;

	    if (key[KEY_F12]) {
		if (check_surface(rootsurface)) {
		    if (pthread_equal(pthread_self(), surfaces[rootsurface]->thread_id)) {
			//if (debug_mode) {
			    illution_exit(2);
			//}
		    }
		}
	    }

	    //printf("Calling global_on_keypress...%i\n", rootsurface);
	    call_on_keypress(rootsurface, inkey);
	    call_on_keypress(0, inkey);
	
	    if (debug_mode) { 
		printf("Calling on_keypress...%i\n", focus);
	    }
	    call_on_keypress(focus, inkey);
	    if (debug_mode) { 
		printf("Returned from keypress...%i\n", focus);
	    }
    
//	}
    }
    int surface_b1 = 0;
    int surface_b2 = 0;
    
    surface = get_surface_by_xy(mouse_x, mouse_y);
    //surface = 0;
    //printf("woo %i\n", surface);
    int enabled = 1;
    if (check_surface(surface)) {
	if (!surfaces[surface]->visible) {
	    enabled = 0;
	}
	if (check_surface(surfaces[surface]->parent)) {
	    if (!surfaces[surfaces[surface]->parent]->visible) {
		enabled = 0;
	    }
	}
    if (enabled) {
//    if (pthread_equal(pthread_self(), surfaces[surface]->thread_id)) {
	
	if (last_mouse_surface_b1 == -1) {
	    surface_b1 = surface;
	} else {
	    surface_b1 = last_mouse_surface_b1;
	}
	if (mouse_b & 1) {
    	    call_on_mouse_down(surface_b1, 1);
	    last_mouse_surface_b1 = surface_b1;
	}

	if (last_mouse_surface_b2 == -1) {
	    surface_b2 = surface;
	} else {
	    surface_b2 = last_mouse_surface_b2;
	}
	if (mouse_b & 2) {
    	    call_on_mouse_down(surface_b2, 1);
	    last_mouse_surface_b2 = surface_b2;
	}

	if (mouse_b != last_mouse_b) {
	
	    if (mouse_b & 1) {
		call_on_mouse_down(surface, 1);
	    } else if (last_mouse_b & 1) {
		call_on_mouse_up(last_mouse_surface_b1, 1);
		call_on_mouse_click(surface, 1);
		last_mouse_surface_b1 = -1;
	    }
	    if (mouse_b & 2) {
		call_on_mouse_down(surface, 2);
	    } else if (last_mouse_b & 2) {
		call_on_mouse_up(last_mouse_surface_b2, 2);
		call_on_mouse_click(surface, 2);
		last_mouse_surface_b2 = -1;	
	    }	
	}
	last_mouse_b = mouse_b;	
    }
    }
}
int lua_trylock(void * L) {
    int i = 0;
    int lockval = 0;
    int locked = 0;
    if (debug_mode) {
//	printf("trylock %p\n", pthread_self());
    }
    
    for(i = 0; i < surfacecount; i += 1) {
        if (check_surface(i)) {
	    if (surfaces[i]->L == L) {
		lockval = pthread_mutex_trylock(surfaces[i]->lua_mutex);
		if (lockval != 0) {
		    locked = 1;
		}	    
    	    }
	}
    }
    return locked;
}

int lua_unlock(void * L) {
    int i = 0;
    int locked = 0;
    int lockval = 0;
    if (debug_mode) {
//	printf("unlock %p\n", pthread_self());
    }
    
    for(i = 0; i < surfacecount; i += 1) {
        if (check_surface(i)) {
	    if (surfaces[i]->L == L) {
		lockval = pthread_mutex_trylock(surfaces[i]->lua_mutex);		
		if (lockval != 0) {
		    pthread_mutex_unlock(surfaces[i]->lua_mutex);
		}	    
    	    }
	}
    }
    return 0;
}

int lua_force_unlock(void * L) {
    int i = 0;
    int locked = 0;
    int lockval = 0;
    if (debug_mode) {
//	printf("unlock %p\n", pthread_self());
    }
    
    for(i = 0; i < surfacecount; i += 1) {
        if (check_surface(i)) {
	    if (surfaces[i]->L == L) {
		//lockval = pthread_mutex_trylock(surfaces[i]->lua_mutex);		
		//if (lockval != 0) {
		    pthread_mutex_unlock(surfaces[i]->lua_mutex);
		//}	    
    	    }
	}
    }
    return 0;
}


void watchdog(lua_State *L, lua_Debug *ar) {
    double time_elapsed = timer() - last_render_time;
    float priority = 0;

    if (debug_mode) {
//	printf("watchdog %p\n", pthread_self());
    }
//    printf("watchdog %p, %i Events in Queue. \n", pthread_self(), event_count);
    
/*    lua_pushnumber(L, pthread_self());  
    lua_gettable(L, LUA_REGISTRYINDEX);    
    priority = lua_tonumber(L, -1);
    if (priority == 0) {	
	priority = atof(GetVar("configuration", "default-thread-priority"));
	lua_pushnumber(L, pthread_self()); 
	lua_pushnumber(L, priority);  
	lua_settable(L, LUA_REGISTRYINDEX);    // store data	
    }
*/
/*    if (time_elapsed > priority * 0.001) {
	pthread_yield();
    }*/
}

void wrapper_thread_safe_call(struct thread_safe_call_data * call_data) {
    if (debug_mode) {
//	printf("wrapper thread safe call %p\n", pthread_self());
    }

    safe_call(call_data->L, call_data->args);
    free(call_data);
}

int safe_call(void * L, int args) {
    if (debug_mode) {
//	printf("safe call %p\n", pthread_self());
    }

    int i = 0;
    if (lua_pcall(L, args, 0, 0) != 0) {
	printf("error: %s\n", lua_tostring(L, -1));
	for(i = 0; i < surfacecount; i += 1) {
	    if (check_surface(i)) {
		if (surfaces[i]->L == L) {
		    destroy_surface(i);
		}
	    }
	}
	lua_close(L);
	return 1;
    } else {
	return 0;
    }
}


int thread_safe_call(void * L, int args) {
    if (debug_mode) {
	printf("thread safe call %p\n", pthread_self());
    }

    pthread_attr_t tattr;
    pthread_t tid;
    
    //printf("thread_safe_call\n");
    struct thread_safe_call_data * data = calloc(1, sizeof(struct thread_safe_call_data));
    data->L = L;
    data->args = args;
    
    pthread_create(&tid, NULL, wrapper_thread_safe_call, data);
    pthread_detach(tid);
}


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
    int lockval = 0;
    if (index > 1) {
	if (debug_mode) {
	    if (pthread_equal(pthread_self(), surfaces[index]->thread_id)) {
    		//printf("%p=%p: surface[%i].call_on_draw\n", pthread_self(), surfaces[index]->thread_id, index);
	    } else {
		//printf("!!%p=%p: surface[%i].call_on_draw\n", pthread_self(), surfaces[index]->thread_id, index);
	    }
	}
    }
    
    if (check_surface(index)) {
	if (surfaces[index]->on_draw != -1) {
	    if (surfaces[index]->visible == 0) {
		return 0;
	    }
	    if (!pthread_equal(pthread_self(), surfaces[index]->thread_id)) {
		if (debug_mode) {
		    printf("outside thread on_draw, surface = %i\n", index);
		}		
		int eid = create_event(index, call_on_draw, "on_draw");
/*		// this method, so called outside drawing, is actually
		// less smooth than if you remove this entirely and let	
		printf("\nnot the same thread for on_draw %i\n", index);
		if (surfaces[index]->parent == rootsurface) {
		    printf("\nasking thread to draw for me %i\n", index);
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
		    printf("\nback %i\n", index);
		}
*/		
	    } else {
		
		//int lockval = pthread_mutex_trylock(surfaces[index]->lua_mutex);
		lockval = lua_trylock(surfaces[index]->L);
		if (lockval != 0) {
		    if (debug_mode) {
			printf("locked on_draw, surface = %i\n", index);
		    }
		    //int eid = create_event(index, call_on_draw, "on_draw");
		    if (debug_mode) {
			//printf("exiting draw\n");
		    }
		    return 0;
		}
		    surfaces[index]->dirty = 0;
		    L = surfaces[index]->L;
		    lua_pushnumber(L, surfaces[index]->on_draw);
		    lua_gettable(L, LUA_REGISTRYINDEX);	    
		    safe_call(L, 0);
		    
	    
		//pthread_mutex_unlock(surfaces[index]->lua_mutex);
		lua_unlock(surfaces[index]->L);
	    }
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
    //printf("Keypress: %i, %i\n", index, surfaces[index]->on_keypress);
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
    int i = 0;
    if (check_surface(index)) {
	if (surfaces[index]->on_mouse_down != -1) {
	    if (!pthread_equal(pthread_self(), surfaces[index]->thread_id)) {
		if (debug_mode) {
		    printf("outside mouse down, adding event\n");
		}
	    
		int eid = create_event(index, call_on_mouse_down, "on_mouse_down");
	    } else {
	    
		int lockval = lua_trylock(surfaces[index]->L);
		if (lockval != 0) {
		    if (debug_mode) {
			printf("locked on_mouse_down\n");
		    }
		    //int eid = create_event(index, call_on_mouse_down, "on_mouse_down");
		    return 0;
		}
		L = surfaces[index]->L;
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
	    
		safe_call(L, 3);
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


