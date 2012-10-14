int lua_trylock(void * L) {
    int i = 0;
    int lockval = 0;
    int locked = 0;
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

void watchdog(lua_State *L, lua_Debug *ar) {
    double time_elapsed = timer() - last_render_time;
    if (time_elapsed > .05) {
	//render_once();
    }
}

void wrapper_thread_safe_call(struct thread_safe_call_data * call_data) {
    safe_call(call_data->L, call_data->args);
    free(call_data);
}

int safe_call(void * L, int args) {
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
    pthread_attr_t tattr;
    pthread_t tid;
    
    printf("thread_safe_call\n");
    struct thread_safe_call_data * data = calloc(1, sizeof(struct thread_safe_call_data));
    data->L = L;
    data->args = args;
    
    pthread_create(&tid, NULL, wrapper_thread_safe_call, data);
    pthread_detach(tid);
}


int resort_zorder(void) {
    int i = 0;
    int i2 = 0;
    int index = 0;
    int largest = 0;
    int largestindex = 0;
    int found = 0;
    int z = 0;

pthread_mutex_lock(&surface_mutex);
    
    for(i = 0; i < surfacecount; i += 1) {
	zorder[i] = -1;
    }
//    zorder[0] = 0;
    for(i = 0; i < surfacecount; i += 1) {
	largest = -1;
	// cycle through surfaces
	for(index = 0; index < surfacecount; index += 1) {
	    // find the largest z-index
	    if (check_surface(index)) {
		//z = get_surface_screen_z(index); //surfaces[index]->z;
		z = surfaces[index]->z;
		if (check_surface(surfaces[index]->parent)) {
		    z += surfaces[surfaces[index]->parent]->z;
		}
		if (surfaces[index]->always_on_top) {
		    z += 99999;
		}
		
		//printf("	Check Surface[%i]->z = %i\n", index, z);
		if (z >= largest) {
		    
		    found = 0;
		    // check if we already found this one
		    for(i2 = 0; i2 < surfacecount; i2 += 1) {
			if (zorder[i2] == index) {
			    found = -1;
			}
		    }
		    if (found == 0) {
			largest = z;
			largestindex = index;
			//printf("surfaces[%i]->z (%i) >= %i\n", index, z, largest);
			//readkey();
		    }		    
		}
	    }
	}
	zorder[i] = largestindex;
    }
    for(i = 0; i < surfacecount; i += 1) {	
	if (check_surface(zorder[i])) {
	    surfaces[zorder[i]]->z = surfacecount - i;
	    //printf("surface[%i] = %i (%i)\n", zorder[i], surfacecount - i, surfaces[zorder[i]]->z);
	    //textprintf_ex(surfaces[zorder[i]]->bmap, font, 0, 0, surfaces[zorder[i]]->forecolor, surfaces[zorder[i]]->backcolor, "%i", surfaces[zorder[i]]->z);
	}
    }   
    //printf("\n\n");
pthread_mutex_unlock(&surface_mutex);
}

int check_surface(int index) {
    if (index >= 0) {
	if (index < surfacecount) {
	    if (surfaces[index] != NULL) {
		return -1;
	    }
	}
    }
    return 0;
}

int get_surface_screen_x(int index) {
    int x = 0;
    if (check_surface(index)) {
	x = surfaces[index]->x;
	if (surfaces[index]->parent != -1) {
	    x += get_surface_screen_x(surfaces[index]->parent);
	}
    }
    return x;
}

int get_surface_screen_y(int index) {
    int y = 0;
    if (check_surface(index)) {
	y = surfaces[index]->y;
	if (surfaces[index]->parent != -1) {
	    y += get_surface_screen_y(surfaces[index]->parent);
	}
    }
    return y;
}

int get_surface_screen_z(int index) {
    int z = 0;
    if (check_surface(index)) {
	if (debug_mode) {
	    printf("Surface %i, z = 0\n", index);
	}
	z = surfaces[index]->z;
	if (debug_mode) {
	    printf("Surface %i, z = %i\n", index, z);
	}
	if (surfaces[index]->parent != -1) {
	    if (surfaces[index]->parent != index) {
		z += get_surface_screen_z(surfaces[index]->parent);
		if (debug_mode) {
		    printf("Surface %i, z = %i\n", index, z);
		}
	    }
	}
	if (debug_mode) {
	    printf("Surface %i done\n\n", index);
	}
	int retval = readkey();
    }
    return z;
}


int get_surface_by_xy(int x, int y) {
    int index = 0;
    int max_z = 0;
    int target = -1;
    int screen_x = 0;
    int screen_y = 0;
    int screen_z = 0;
    
    for(index = 0; index < surfacecount; index += 1) {
	if (check_surface(index)) {
	    screen_x = get_surface_screen_x(index);
	    screen_y = get_surface_screen_y(index);
	    screen_z = surfaces[index]->z; //get_surface_screen_z(index);
	    //screen_z = get_surface_screen_z(index);
//	    printf("Mouse X: %i, Mouse Y: %i, [%i] X: %i, [%i] Y: %i\n", mouse_x, mouse_y, index, screen_x, index, screen_y);
//	    printf("[%i] Z: %i, Max Z: %i, Mouse Cursor: %i\n", index, screen_z, max_z, surfaces[index]->mouse_cursor);
	    if (screen_x <= x) {
		if (surfaces[index]->bmap->w + screen_x >= x) {
		    if (screen_y <= y) {
			if (surfaces[index]->bmap->h + screen_y >= y) {
			    if (screen_z > max_z) {
				if (!surfaces[index]->always_on_top) {
				    max_z = screen_z;
				    target = index;
				}
			    }
			}
		    }		    
		}
	    }
	}
    }
    return target;
}



int l_get_surface_by_xy(void * L) {
    int x = lua_tonumber(L, 1);
    int y = lua_tonumber(L, 2);
    lua_pushnumber(L, get_surface_by_xy(x, y));
    return 1;
}

int l_mouse_xy(void * L) {
    lua_pushnumber(L, mouse_x);
    lua_pushnumber(L, mouse_y);
    
    return 2;
}

int l_mouse_lr(void * L) {
    int b1 = 0;
    int b2 = 0;
    
    if (mouse_b & 1) {
	b1 = 1;
    }
    if (mouse_b & 2) {
	b2 = 1;
    }
    lua_pushnumber(L, b1);
    lua_pushnumber(L, b2);
    return 2;
}

int l_readkey(void * L) {

    int inkey = readkey();
    int keycode = inkey >> 8;
    int keyval = inkey & 0xff;
    
    lua_pushnumber(L, keycode);
    lua_pushnumber(L, keyval);
    return 2;
}

int l_create_surface(void * L) {
    int index = 0;
    int width = 0;
    int height = 0;
    
    width = lua_tonumber(L, 1);
    height = lua_tonumber(L, 2);
    index = create_surface(width, height);
    lua_pushnumber(L, index);
    return 1;
}

int l_destroy_surface(void * L) {
    int index = 0;
    
    index = lua_tonumber(L, 1);
    destroy_surface(index);
    //surfaces[index]->destroy = 1;
    return 0;
}

int l_get_surface_visible(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->visible);
	pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
    pthread_mutex_unlock(&surface_mutex);
}

int l_set_surface_visible(void * L) {
    int index = lua_tonumber(L, 1);
    int visible = lua_tonumber(L, 2);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->visible = visible;
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_get_surface_transparent(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->transparent);
	pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_transparent(void * L) {
    int index = lua_tonumber(L, 1);
    int transparent = lua_tonumber(L, 2);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->transparent = transparent;
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_get_surface_alpha(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->alpha);
	pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_alpha(void * L) {
    int index = lua_tonumber(L, 1);
    int alpha = lua_tonumber(L, 2);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->alpha = alpha;
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_get_surface_rotation(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->rotation);
	pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_rotation(void * L) {
    int index = lua_tonumber(L, 1);
    int rotation = lua_tonumber(L, 2);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->rotation = rotation;
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_get_surface_backcolor(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->backcolor);
	pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_backcolor(void * L) {
    int index = lua_tonumber(L, 1);
    int backcolor = lua_tonumber(L, 2);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->backcolor = backcolor;
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_get_surface_forecolor(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->forecolor);
	pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_forecolor(void * L) {
    int index = lua_tonumber(L, 1);
    int forecolor = lua_tonumber(L, 2);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->forecolor = forecolor;
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_get_surface_transcolor(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->transcolor);
	pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_transcolor(void * L) {
    int index = lua_tonumber(L, 1);
    int transcolor = lua_tonumber(L, 2);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->transcolor = transcolor;
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
}


int l_get_surface_x(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->x);
	pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_x(void * L) {
    int index = lua_tonumber(L, 1);
    float x = lua_tonumber(L, 2);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->x = x;
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_get_surface_y(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->y);
	pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_y(void * L) {
    int index = lua_tonumber(L, 1);
    float y = lua_tonumber(L, 2);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->y = y;
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
}


int l_get_surface_z(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->z);
	pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_z(void * L) {
    int index = lua_tonumber(L, 1);
    int z = lua_tonumber(L, 2);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->z = z;
	pthread_mutex_unlock(&surface_mutex);
	resort_zorder();
	pthread_mutex_lock(&surface_mutex);
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
}


int l_surface_get_text_length(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, text_length(surfaces[index]->font, lua_tostring(L, 2)));
	pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}


int l_surface_get_text_height(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, text_height(surfaces[index]->font));
	pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}



int l_get_surface_parent(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->parent);
	pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_parent(void * L) {
    int index = lua_tonumber(L, 1);
    int parent = lua_tonumber(L, 2);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	if (check_surface(parent)) {
	    surfaces[index]->parent = parent;
	}
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_set_surface_font(void * L) {
    int index = lua_tonumber(L, 1);
    int font_index = lua_tonumber(L, 2);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	if (check_surface(font_index)) {
	    surfaces[index]->font = grab_font_from_bitmap(surfaces[font_index]->bmap);
	}
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
}


int l_get_surface_bitmap(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
        lua_pushlightuserdata(L, surfaces[index]->bmap);
	pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	pthread_mutex_unlock(&surface_mutex);
	return 0;
    }    
}

int l_set_surface_bitmap(void * L) {
    int index = lua_tonumber(L, 1);
    BITMAP* bmp = lua_touserdata(L, 2);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->bmap = bmp;
	pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}


int l_get_surface_dirty(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->dirty);
	pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_dirty(void * L) {
    int index = lua_tonumber(L, 1);

    if (check_surface(index)) {
	// you are now dirty
	//printf("%i is dirty\n", index);
	surfaces[index]->dirty = 1;
    }
    return 0;
    /*
    //pthread_mutex_lock(&surface_mutex);
    //printf("%i is dirty\n", index);
    if (check_surface(index)) {
	// you are now dirty
	surfaces[index]->dirty = 1;
	int i = 0;
	// all your children are now dirty too
	for(i = 0; i < surfacecount; i += 1) {
	    if (check_surface(i)) {
		if (surfaces[i]->parent == index) {
		    surfaces[i]->dirty = 1;
		}
	    }
	}
	// and all of your parent's children (your siblings) are also dirty
	int p = surfaces[index]->parent;
	for(i = 0; i < surfacecount; i += 1) {
	    if (check_surface(i)) {
		if (surfaces[i]->parent == p) {
		    surfaces[i]->dirty = 1;
		}
	    }
	}
	
	// and your parent is dirty
	if (check_surface(p)) {
	    surfaces[p]->dirty = 1;
	}
    }
    // you dirty bastard
    //pthread_mutex_unlock(&surface_mutex);
    

    return 0;
    */
}




int l_get_surface_screen_x(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, get_surface_screen_x(index));
	pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}


int l_get_surface_screen_y(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, get_surface_screen_y(index));
	pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}


int l_get_surface_width(void * L) {
    int index = lua_tonumber(L, 1);
    
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->bmap->w);
	pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_width(void * L) {
    int index = lua_tonumber(L, 1);
    int width = lua_tonumber(L, 2);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	BITMAP * tmp = create_bitmap(width, surfaces[index]->bmap->h);
	blit(surfaces[index]->bmap, tmp, 0, 0, 0, 0, surfaces[index]->bmap->w, surfaces[index]->bmap->h);
	free(surfaces[index]->bmap);
	surfaces[index]->bmap = tmp;
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_get_surface_height(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->bmap->h);
	pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_height(void * L) {
    int index = lua_tonumber(L, 1);
    int height = lua_tonumber(L, 2);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	BITMAP * tmp = create_bitmap(surfaces[index]->bmap->w, height);
	blit(surfaces[index]->bmap, tmp, 0, 0, 0, 0, surfaces[index]->bmap->w, surfaces[index]->bmap->h);
	free(surfaces[index]->bmap);
	surfaces[index]->bmap = tmp;
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_draw_gradient(void * L) {
    int index = lua_tonumber(L, 1);
    int x1 = lua_tonumber(L, 2);
    int y1 = lua_tonumber(L, 3);
    int x2 = lua_tonumber(L, 4);
    int y2 = lua_tonumber(L, 5);

    int color1 = lua_tonumber(L, 6);
    int color2 = lua_tonumber(L, 7);

    int r1 = 0;
    int g1 = 0;
    int b1 = 0;
    int r2 = 0;
    int g2 = 0;
    int b2 = 0;
    
    r1 = (color1 >> 16) & 0xFF;
    g1 = (color1 >> 8) & 0xFF;
    b1 = color1 & 0xFF;

    r2 = (color2 >> 16) & 0xFF;
    g2 = (color2 >> 8) & 0xFF;
    b2 = color2 & 0xFF;
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
//      draw_gradient(surfaces[index]->bmap, 0, 0, surfaces[index]->bmap->w, surfaces[index]->bmap->h, r1, g1, b1, r2, g2, b2);	
        draw_gradient(surfaces[index]->bmap, x1, y1, x2, y2, r1, g1, b1, r2, g2, b2);		
    }
    pthread_mutex_unlock(&surface_mutex);
}

int l_makecol(void * L) {
    int r = lua_tonumber(L, 1);
    int g = lua_tonumber(L, 2);
    int b = lua_tonumber(L, 3);
    lua_pushnumber(L, makecol(r, g, b));
    return 1;
}

int l_screen_width(void * L) {
    lua_pushnumber(L, illution_resx);
    return 1;
}

int l_screen_height(void * L) {
    lua_pushnumber(L, illution_resy);
    return 1;
}

int l_timer(void * L) {
    lua_pushnumber(L, gtimer);
    return 1;
}

int l_tick(void * L) {
    lua_pushnumber(L, tick);
    return 1;
}

int l_vsync(void * L) {
    double lastticker = ticker;
    double start = gtimer;
    
    while(ticker == lastticker) {
	pthread_yield();
    }
    lua_pushnumber(L, gtimer - start);
    return 1;
}


int l_set_surface_on_draw(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->on_draw = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_set_surface_on_got_focus(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->on_got_focus = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_set_surface_on_lost_focus(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->on_lost_focus = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
}


int l_set_surface_always_on_top(void * L) {
    int index = lua_tonumber(L, 1);
    int value = lua_tonumber(L, 2);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->always_on_top = value;
	if (value) {
	    surfaces[index]->z = 9999;
	    pthread_mutex_unlock(&surface_mutex);
	    resort_zorder();
	    pthread_mutex_lock(&surface_mutex);
	}
	
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
}


int l_set_surface_on_mouse_down(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->on_mouse_down = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_set_surface_on_mouse_up(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->on_mouse_up = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
}


int l_set_surface_on_mouse_click(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->on_mouse_click = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
}


int call_on_draw(int index) {
    void * L = NULL;
    if (check_surface(index)) {
	if (surfaces[index]->on_draw != -1) {
	
	//int lockval = pthread_mutex_trylock(surfaces[index]->lua_mutex);
	int lockval = lua_trylock(surfaces[index]->L);
	if (lockval != 0) {
	    printf("locked\n");
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
	    printf("makin the call for %i\n", index);
	    safe_call(L, 0);
	    printf("made it back home safe for %i\n", index);
	    
	//pthread_mutex_unlock(surfaces[index]->lua_mutex);
	lua_unlock(surfaces[index]->L);
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
	    printf("returned from on draw %i\n", index);
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


int l_set_surface_on_keypress(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->on_keypress = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
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
	    lua_unlock(surfaces[index]->L);
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
	if (surfaces[index]->on_mouse_up != -1) {
	    L = surfaces[index]->L;
	    if (L != NULL) {
	    //int lockval = pthread_mutex_trylock(surfaces[index]->lua_mutex);
	    int lockval = lua_trylock(surfaces[index]->L);
	if (lockval != 0) {
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
		printf("back\n");
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
	    lua_unlock(surfaces[index]->L);
	    }
	}
    }
}


int l_set_surface_lua(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->L = L;
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_surface_clear(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	clear_to_color(surfaces[index]->bmap, surfaces[index]->backcolor);
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;    
}

int l_surface_print(void * L) {
    int index = lua_tonumber(L, 1);
    int x = lua_tonumber(L, 2);    
    int y = lua_tonumber(L, 3);    
    char *text = lua_tostring(L, 4);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	textprintf_ex(surfaces[index]->bmap, surfaces[index]->font, x, y, surfaces[index]->forecolor, surfaces[index]->backcolor, "%s", text);
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;        
}

int l_surface_print_wordwrap(void * L) {
    int index = lua_tonumber(L, 1);
    char *text = lua_tostring(L, 2);
    int start = lua_tonumber(L, 3);    
    int x = 0;
    int y = 0;
    int th = text_height(font);
    int len = strlen(text);
    int bw = 0;
    int tw = 0;
    int lw = 0;
    int i = 0;

    int w = 0;
    char whitespace[] = " ";
    char nonprintable[] = "\n";
    int word_start = 0;
    int word_end = 0;
    char buffer[1024] = "";
    char b2[2] = "";
    int word_count = 0;
    int cr = 0;
    int nextline = 0;
    int actual_word_count = 0; // trailing whitespace count
    
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	bw = surfaces[index]->bmap->w;
	for (i = start; i < len; i += 1) {	    
	    while (text[i] == 10) {
		lw = 0;
		y += th;
		i += 1;
		if (nextline == 0) {
		    nextline = i;
		    if (debug_mode) {
			printf("\n carriage return\nnextline %i\n", nextline);
		    }
		}
	    }
	    word_start = i;
	    word_end = word_start;
	    for (w = word_start; w < len; w += 1) {
//		printf("w: %i\n", text[w]);
		if (text[w] == 10) {
		    word_end = w;
		    break;		    
		}
		if (strchr(whitespace, text[w]) != NULL) {
		    word_end = w;
		    break;
		}
	    }
	    if (w == len) {
		word_end = len;
	    }
	    actual_word_count = word_end - word_start;
	    for (w = word_end; w < len; w += 1) {
//		printf("s: %i\n", text[w]);
//		if (text[w] == 10) {
//		    word_end = w;
//		    break;		    
//		}		
		if (strchr(whitespace, text[w]) == NULL) {
		    word_end = w;
		    break;
		}
	    }
	    if (word_end == word_start) {
//		printf("Panic at the disco\n");
		break;
	    }

	    word_count = word_end - word_start;
	    if (word_count >= 1024) {
		word_count = 1023;
	    }
	    for (w = 0; w < word_count; w += 1) {
		//if (strchr(nonprintable, text[word_start + w]) == NULL) {
		    buffer[w] = text[word_start + w];
		//} else {
		//    buffer[w] = 32;
		//}
	    }
	    buffer[word_count] = 0;
	    i += word_count - 1;
//	    printf("Word: %s\n", buffer);
	    tw = text_length(font, buffer);
	    if (tw > bw) {
		for (w = 0; w < word_count; w += 1) {
		    b2[0] = buffer[w];
		    b2[1] = 0;
		    tw = text_length(font, b2);
		    if (tw + lw > bw) {
			y += th;
			lw = 0;
			if (nextline == 0) {
			    //nextline = i;
			    nextline = i - (word_count - w) + 1;
			    if (debug_mode) {
				printf("\n Large word\n");
			    }
			    
			}
		    }
		    textprintf_ex(surfaces[index]->bmap, font, lw, y, surfaces[index]->forecolor, -1, "%s", b2);
		    lw += tw;
		}
	    } else {
		if (tw + lw > bw) {
		    y += th;
		    lw = 0;
		    if (nextline == 0) {
			if (word_count == actual_word_count) {
			    nextline = i - actual_word_count + 1;
			} else {
			    nextline = i - actual_word_count;
			}

			if (debug_mode) {
			    printf("\n(A) next line: \"%s\"\n", buffer);
			    printf("word_count: %i, actual_word_count %i\n", word_count, actual_word_count);			
			}
		    }		    
		}
		textprintf_ex(surfaces[index]->bmap, font, lw, y, surfaces[index]->forecolor, -1, "%s", buffer);
		lw += tw;
	    }
	    
	    if (y > surfaces[index]->bmap->h) {
		break;
	    }	    
	}
    
	//textprintf_ex(surfaces[index]->bmap, font, x, y, surfaces[index]->forecolor, -1, "%s", text);
    }
    lua_pushnumber(L, nextline);
    pthread_mutex_unlock(&surface_mutex);
    return 1;
}


int l_wordwrap(void * L) {
    int index = lua_tonumber(L, 1);
    int x = 0;
    int y = 0;
    char *text = lua_tostring(L, 2);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	textprintf_ex(surfaces[index]->bmap, font, x, y, surfaces[index]->forecolor, -1, "%s", text);
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;            
}

int l_surface_has_focus(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->has_focus);
    }
    pthread_mutex_unlock(&surface_mutex);
    return 1;            

}

int l_set_focus(void * L) {
    int index = lua_tonumber(L, 1);
    //pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	if (check_surface(focus)) {
	    surfaces[focus]->has_focus = 0;
	    call_on_lost_focus(focus);
	}
	focus = index;
	surfaces[index]->has_focus = 1;
	call_on_got_focus(index);
	if (debug_mode) {
	    printf("Focus: %i\n", index);
	}
    }
    //pthread_mutex_unlock(&surface_mutex);
    return 0;            
}

int l_surface_line(void * L) {
    int index = lua_tonumber(L, 1);
    int x1 = lua_tonumber(L, 2);
    int y1 = lua_tonumber(L, 3);    
    int x2 = lua_tonumber(L, 4);    
    int y2 = lua_tonumber(L, 5);    
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	line(surfaces[index]->bmap, x1, y1, x2, y2, surfaces[index]->forecolor);
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;            
}

int l_surface_rect(void * L) {
    int index = lua_tonumber(L, 1);
    int x1 = lua_tonumber(L, 2);
    int y1 = lua_tonumber(L, 3);    
    int x2 = lua_tonumber(L, 4);    
    int y2 = lua_tonumber(L, 5);    
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	rect(surfaces[index]->bmap, x1, y1, x2, y2, surfaces[index]->forecolor);
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;            
}

int l_surface_rect_fill(void * L) {
    int index = lua_tonumber(L, 1);
    int x1 = lua_tonumber(L, 2);
    int y1 = lua_tonumber(L, 3);    
    int x2 = lua_tonumber(L, 4);    
    int y2 = lua_tonumber(L, 5);    
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	rectfill(surfaces[index]->bmap, x1, y1, x2, y2, surfaces[index]->forecolor);
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;            
}

int l_render_loop(void * L) {
/*    pthread_attr_t tattr;
    pthread_t tid;
    
    pthread_create(&tid, NULL, render_loop, NULL);
    pthread_detach(tid); */
    render_loop();
    return 0;
}

/*
int l_render(void * L) {
    render_once();
    return 0;
}
*/

int l_render(void * L) {
    int index = lua_tonumber(L, 1);
    pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	render_surfaces(index);
    }
    pthread_mutex_unlock(&surface_mutex);
    return 0;            
}

int l_stop_render(void * L) {
    bail = 1;
    return 0;
}

int l_initialize_video(void * L) {
    int n = lua_gettop(L);
    int x = 640;
    int y = 480;
    int video_mode = GFX_SAFE;
    int retval = 0;
    if (n == 0) {
	retval = initialize_video(video_mode, x, y);
    } else if (n == 2) {
	x = lua_tonumber(L, 1);
	y = lua_tonumber(L, 2);
	retval = initialize_video(video_mode, x, y);
    } else if (n == 3) {
	video_mode = lua_tonumber(L, 1);
	x = lua_tonumber(L, 2);
	y = lua_tonumber(L, 3);
	retval = initialize_video(video_mode, x, y);
    }
    lua_pushnumber(L, retval);
    
    return 1;
}

int l_set_root_surface(void * L) {
    rootsurface = lua_tonumber(L, 1);
    return 0;
}

int l_get_root_surface(void * L) {
    lua_pushnumber(L, rootsurface);
    return 1;
}

int l_surface_load_bmp(void * L) {
    int index = lua_tonumber(L, 1);
    char *file = lua_tostring(L, 2);
    if (findfile(&file)) {
	pthread_mutex_lock(&surface_mutex);
	if (check_surface(index)) {
	    BITMAP * tmp = load_bmp(file, NULL);
	    free(surfaces[index]->bmap);
	    surfaces[index]->bmap = tmp;	    
	}
	pthread_mutex_unlock(&surface_mutex);
    }
    return 0;                
}

//int l_new_feature(void * L) {
//    printf("I am the all-singing, all-dancing, new feature crap of the world.\n");
//    return 0;
//}

int l_create_camera(void * L) {
    struct camera_type *camera = calloc(1, sizeof(struct camera_type));
    camera->bmp = NULL;
    camera->x = 0;
    camera->y = 0;
    camera->z = 0;
    camera->pitch = 0;
    camera->heading = 0;
    camera->zbuf = NULL;
        
    lua_pushlightuserdata(L, camera);
    return 1;
}

int l_set_camera_bitmap(void * L) {
    struct camera_type *camera = lua_touserdata(L, 1);
    BITMAP *bmp = lua_touserdata(L, 2);

    camera->bmp = bmp;
    return 0;
}

int l_get_camera_bitmap(void * L) {
    struct camera_type *camera = lua_touserdata(L, 1);
    lua_pushlightuserdata(L, camera->bmp);
    return 1;
}

int l_set_camera_x(void * L) {
    struct camera_type *camera = lua_touserdata(L, 1);
    float x = lua_tonumber(L, 2);
    camera->x = x;
    return 0;
}

int l_get_camera_x(void * L) {
    struct camera_type *camera = lua_touserdata(L, 1);
    lua_pushnumber(L, camera->x);
    return 1;
}


int l_set_camera_y(void * L) {
    struct camera_type *camera = lua_touserdata(L, 1);
    float y = lua_tonumber(L, 2);
    camera->y = y;
    return 0;
}

int l_get_camera_y(void * L) {
    struct camera_type *camera = lua_touserdata(L, 1);
    lua_pushnumber(L, camera->y);
    return 1;
}

int l_set_camera_z(void * L) {
    struct camera_type *camera = lua_touserdata(L, 1);
    float z = lua_tonumber(L, 2);
    camera->z = z;
    return 0;
}

int l_get_camera_z(void * L) {
    struct camera_type *camera = lua_touserdata(L, 1);
    lua_pushnumber(L, camera->z);
    return 1;
}

int l_set_camera_heading(void * L) {
    struct camera_type *camera = lua_touserdata(L, 1);
    float heading = lua_tonumber(L, 2);
    camera->heading = heading;
    return 0;
}

int l_get_camera_heading(void * L) {
    struct camera_type *camera = lua_touserdata(L, 1);
    lua_pushnumber(L, camera->heading);
    return 1;
}

int l_set_camera_pitch(void * L) {
    struct camera_type *camera = lua_touserdata(L, 1);
    float pitch = lua_tonumber(L, 2);
    camera->pitch = pitch;
    return 0;
}

int l_get_camera_pitch(void * L) {
    struct camera_type *camera = lua_touserdata(L, 1);
    lua_pushnumber(L, camera->pitch);
    return 1;
}

int l_initialize_camera(void * L) {
    struct camera_type *camera = lua_touserdata(L, 1);

    set_projection_viewport(0, 0, camera->bmp->w, camera->bmp->h);
    camera->zbuf = create_zbuffer(camera->bmp);
    set_zbuffer(camera->zbuf);
    
    return 0;
}

int l_update_camera(void * L) {
    int x, y, w, h;
    float xfront, yfront, zfront;
    float xup, yup, zup;
    float xpos, ypos, zpos;
    float heading;
    float pitch;
    float roll;
    
    struct camera_type *camera = lua_touserdata(L, 1);
    
    x = 0;
    y = 0;
    w = camera->bmp->w;
    h = camera->bmp->h;

    xpos = camera->x;
    ypos = camera->y;
    zpos = camera->z;
    roll = 64;

    heading = ((camera->pitch / -1) + 90) * Deg2Rad;
    pitch = (camera->heading / -1) * Deg2Rad; 	// heading/pitch swapped due to 
						// rolling the camera to match 
						// ODE's cordinate system
    
    xfront = sin(heading) * cos(pitch);
    yfront = sin(pitch);
    zfront = cos(heading) * cos(pitch);

    get_vector_rotation_matrix_f(&camera->roller, xfront, yfront, zfront, roll);
    apply_matrix_f(&camera->roller, 0, -1, 0, &xup, &yup, &zup);
    
    get_camera_matrix_f(&camera->camera,
			xpos, ypos, zpos,
			xfront, yfront, zfront,
			xup, yup, zup,
			fov,
			aspect);
			
    clear_zbuffer(camera->zbuf, 0.);
    return 0;
}

int l_create_model(void * L) {
    struct model_type * model;
    model = calloc(1, sizeof(struct model_type ));
    model->facecount = 0;
    lua_pushlightuserdata(L, model);
    return 1;
}

int l_model_add_face(void * L) {
    struct model_type * model = lua_touserdata(L, 1);
    addface(model);
    return 0;
}

int l_get_model_face_count(void * L) {
    struct model_type * model = lua_touserdata(L, 1);
    
    lua_pushnumber(L, model->facecount);
    return 1;
}

int l_set_model_face_data(void * L) {
    struct model_type * model = lua_touserdata(L, 1);
    int face = lua_tonumber(L, 2);
    int vertex = lua_tonumber(L, 3);
    float x = lua_tonumber(L, 4);
    float y = lua_tonumber(L, 5);
    float z = lua_tonumber(L, 6);
    float u = lua_tonumber(L, 7);    
    float v = lua_tonumber(L, 8);    
    float c = lua_tonumber(L, 9);
    
    model->faces[face]->v[vertex].x = x;
    model->faces[face]->v[vertex].y = y;
    model->faces[face]->v[vertex].z = z;
    model->faces[face]->v[vertex].u = u;    
    model->faces[face]->v[vertex].v = v;    
    model->faces[face]->v[vertex].c = c;
    
    return 0;
}

int l_get_model_face_data(void * L) {
    struct model_type * model = lua_touserdata(L, 1);
    int face = lua_tonumber(L, 2);
    int vertex = lua_tonumber(L, 3);
    
    lua_pushnumber(L, model->faces[face]->v[vertex].x);
    lua_pushnumber(L, model->faces[face]->v[vertex].y);
    lua_pushnumber(L, model->faces[face]->v[vertex].z);
    lua_pushnumber(L, model->faces[face]->v[vertex].u);    
    lua_pushnumber(L, model->faces[face]->v[vertex].v);    
    lua_pushnumber(L, model->faces[face]->v[vertex].c);
    
    return 6;
}


int l_load_3ds(void * L) {
    struct model_type * model;
    char * file;
    model = lua_touserdata(L, 1);
    file = lua_tostring(L, 2);
    
    lua_pushnumber(L, load3ds(file, model));
    return 1;
}

int l_render_model(void * L) {
    struct model_type * model = lua_touserdata(L, 1);
    struct trans_type * matrix = lua_touserdata(L, 2);
    struct camera_type * camera = lua_touserdata(L, 3);
    BITMAP * texture_bmp = lua_touserdata(L, 4);
    BITMAP * target_bmp = lua_touserdata(L, 5);    

    int f = 0;
    int e = 0;
    int facecount = model->facecount;
    
    V3D_f *v[4];
    V3D_f vout[4];
    
    for (f = 0; f < facecount; f += 1) {
	for (e = 0; e < 3; e += 1) {
	    vout[e].x = model->faces[f]->v[e].x;
	    vout[e].y = model->faces[f]->v[e].y;
	    vout[e].z = model->faces[f]->v[e].z;
	    vout[e].u = model->faces[f]->v[e].u;
	    vout[e].v = model->faces[f]->v[e].v;
	    apply_quat(&matrix->rotation, vout[e].x, vout[e].y, vout[e].z, 
		&vout[e].x, &vout[e].y, &vout[e].z);	    
	    apply_matrix_f(&matrix->matrix, vout[e].x, vout[e].y, vout[e].z, 
		&vout[e].x, &vout[e].y, &vout[e].z);

	}
	draw_polygon(camera, &vout[0], &vout[1], &vout[2], texture_bmp, target_bmp);
    }
    return 0;
}

int l_create_matrix(void * L) {
    struct trans_type * matrix = calloc(1, sizeof(struct trans_type));
    matrix->x = 0;
    matrix->y = 0;    
    matrix->z = 0;    
    matrix->r_x = 0;
    matrix->r_y = 0;    
    matrix->r_z = 0;    
    matrix->scale = 1;
    lua_pushlightuserdata(L, matrix);
    return 1;
}

int l_update_matrix(void * L) {
    int n = lua_gettop(L);
    struct trans_type * matrix = lua_touserdata(L, 1);
    struct model_type * model = NULL;
    struct model_type * collision_model = NULL;
    
    
    if (n == 3) {
	model = lua_touserdata(L, 2);
	collision_model = lua_touserdata(L, 3);
    }
    
    get_transformation_matrix_f(&matrix->matrix, matrix->scale,
//	matrix->r_x, matrix->r_y, matrix->r_z,
	0, 0, 0,
	matrix->x, matrix->y, matrix->z);
	
    get_rotation_quat(&matrix->rotation, matrix->r_x, matrix->r_y, matrix->r_z);
    

    if (n == 3) {
	int i = 0;
	int f = 0;
	int e = 0;
	if (collision_model->facecount < model->facecount) {
	    for(i = 0; i < model->facecount; i += 1) {
		addface(collision_model);
	    }
	}
	for (f = 0; f < model->facecount; f += 1) {
	    for (e = 0; e < 3; e += 1) {
		apply_quat(&matrix->rotation,
				model->faces[f]->v[e].x, 
				model->faces[f]->v[e].y, 
				model->faces[f]->v[e].z,
				&collision_model->faces[f]->v[e].x, 
				&collision_model->faces[f]->v[e].y, 
				&collision_model->faces[f]->v[e].z
			    );
	    
		apply_matrix_f(&matrix->matrix, 
				collision_model->faces[f]->v[e].x, 
				collision_model->faces[f]->v[e].y, 
				collision_model->faces[f]->v[e].z,
				&collision_model->faces[f]->v[e].x, 
				&collision_model->faces[f]->v[e].y, 
				&collision_model->faces[f]->v[e].z
			    );
	    }
	}
    }
    return 0;
}

int l_set_matrix_x(void * L) {
    struct trans_type *matrix = lua_touserdata(L, 1);
    float x = lua_tonumber(L, 2);
    matrix->x = x;
    return 0;
}

int l_get_matrix_x(void * L) {
    struct trans_type *matrix = lua_touserdata(L, 1);
    lua_pushnumber(L, matrix->x);
    return 1;
}


int l_set_matrix_y(void * L) {
    struct trans_type *matrix = lua_touserdata(L, 1);
    float y = lua_tonumber(L, 2);
    matrix->y = y;
    return 0;
}

int l_get_matrix_y(void * L) {
    struct trans_type *matrix = lua_touserdata(L, 1);
    lua_pushnumber(L, matrix->y);
    return 1;
}

int l_set_matrix_z(void * L) {
    struct trans_type *matrix = lua_touserdata(L, 1);
    float z = lua_tonumber(L, 2);
    matrix->z = z;
    return 0;
}

int l_get_matrix_z(void * L) {
    struct trans_type *matrix = lua_touserdata(L, 1);
    lua_pushnumber(L, matrix->z);
    return 1;
}

int l_set_matrix_r_x(void * L) {
    struct trans_type *matrix = lua_touserdata(L, 1);
    float r_x = lua_tonumber(L, 2);
    matrix->r_x = r_x;
    return 0;
}

int l_get_matrix_r_x(void * L) {
    struct trans_type *matrix = lua_touserdata(L, 1);
    lua_pushnumber(L, matrix->r_x);
    return 1;
}


int l_set_matrix_r_y(void * L) {
    struct trans_type *matrix = lua_touserdata(L, 1);
    float r_y = lua_tonumber(L, 2);
    matrix->r_y = r_y;
    return 0;
}

int l_get_matrix_r_y(void * L) {
    struct trans_type *matrix = lua_touserdata(L, 1);
    lua_pushnumber(L, matrix->r_y);
    return 1;
}

int l_set_matrix_r_z(void * L) {
    struct trans_type *matrix = lua_touserdata(L, 1);
    float r_z = lua_tonumber(L, 2);
    matrix->r_z = r_z;
    return 0;
}

int l_get_matrix_r_z(void * L) {
    struct trans_type *matrix = lua_touserdata(L, 1);
    lua_pushnumber(L, matrix->r_z);
    return 1;
}

int l_set_matrix_scale(void * L) {
    struct trans_type *matrix = lua_touserdata(L, 1);
    float scale = lua_tonumber(L, 2);
    matrix->scale = scale;
    return 0;
}

int l_get_matrix_scale(void * L) {
    struct trans_type *matrix = lua_touserdata(L, 1);
    lua_pushnumber(L, matrix->scale);
    return 1;
}

int l_set_debug_mode(void * L) {
    debug_mode = lua_tonumber(L, 1);
    return 0;
}

void illution_lua_register_call(void * L) {
    
//    printf("Registering NEW FEATURE!\n");
//    lua_register(L, "new_feature", l_new_feature);

    //lua_register(L, "surface_load_bmp", l_surface_load_bmp);
    // ode
    lua_register(L, "set_debug_mode", l_set_debug_mode);
    lua_register(L, "create_worldspace", l_create_worldspace);
//    lua_register(L, "", );
    lua_register(L, "create_plane", l_create_plane);
    lua_register(L, "plane_set_geom", l_plane_set_geom);
    lua_register(L, "create_body", l_create_body);
    lua_register(L, "body_set_position", l_body_set_position);
    lua_register(L, "body_get_position", l_body_get_position);
    lua_register(L, "simulate", l_simulate);
    lua_register(L, "create_camera", l_create_camera);
    lua_register(L, "set_camera_bitmap", l_set_camera_bitmap);
    lua_register(L, "get_camera_bitmap", l_get_camera_bitmap);
    lua_register(L, "set_camera_x", l_set_camera_x);
    lua_register(L, "get_camera_x", l_get_camera_x);
    lua_register(L, "set_camera_y", l_set_camera_y);
    lua_register(L, "get_camera_y", l_get_camera_y);
    lua_register(L, "set_camera_z", l_set_camera_z);
    lua_register(L, "get_camera_z", l_get_camera_z);
    lua_register(L, "set_camera_heading", l_set_camera_heading);
    lua_register(L, "get_camera_heading", l_get_camera_heading);
    lua_register(L, "set_camera_pitch", l_set_camera_pitch);
    lua_register(L, "get_camera_pitch", l_get_camera_pitch);
    lua_register(L, "initialize_camera", l_initialize_camera);
    lua_register(L, "update_camera", l_update_camera);
    lua_register(L, "create_model", l_create_model);
    lua_register(L, "model_add_face", l_model_add_face);
    lua_register(L, "get_model_face_count", l_get_model_face_count);
    lua_register(L, "set_model_face_data", l_set_model_face_data);
    lua_register(L, "get_model_face_data", l_get_model_face_data);
    lua_register(L, "load_3ds", l_load_3ds);
    lua_register(L, "render_model", l_render_model);
    lua_register(L, "create_matrix", l_create_matrix);
    lua_register(L, "update_matrix", l_update_matrix);
    lua_register(L, "set_matrix_x", l_set_matrix_x);
    lua_register(L, "get_matrix_x", l_get_matrix_x);
    lua_register(L, "set_matrix_y", l_set_matrix_y);
    lua_register(L, "get_matrix_y", l_get_matrix_y);
    lua_register(L, "set_matrix_z", l_set_matrix_z);
    lua_register(L, "get_matrix_z", l_get_matrix_z);
    lua_register(L, "set_matrix_r_x", l_set_matrix_r_x);
    lua_register(L, "get_matrix_r_x", l_get_matrix_r_x);
    lua_register(L, "set_matrix_r_y", l_set_matrix_r_y);
    lua_register(L, "get_matrix_r_y", l_get_matrix_r_y);
    lua_register(L, "set_matrix_r_z", l_set_matrix_r_z);
    lua_register(L, "get_matrix_r_z", l_get_matrix_r_z);
    lua_register(L, "set_matrix_scale", l_set_matrix_scale);
    lua_register(L, "get_matrix_scale", l_get_matrix_scale);
    
    
    lua_register(L, "set_focus", l_set_focus);
    lua_register(L, "surface_has_focus", l_surface_has_focus);
    lua_register(L, "surface_print_wordwrap", l_surface_print_wordwrap);
    lua_register(L, "surface_print", l_surface_print);
    lua_register(L, "surface_clear", l_surface_clear);
    lua_register(L, "surface_line", l_surface_line);
    lua_register(L, "surface_rect", l_surface_rect);    
    lua_register(L, "surface_rect_fill", l_surface_rect_fill);
    lua_register(L, "set_surface_lua", l_set_surface_lua);
    lua_register(L, "timer", l_timer);
    lua_register(L, "render", l_render);
    lua_register(L, "readkey", l_readkey);
    lua_register(L, "mouse_xy", l_mouse_xy);
    lua_register(L, "mouse_lr", l_mouse_lr);
    lua_register(L, "tick", l_tick);
    lua_register(L, "vsync", l_vsync);    
    lua_register(L, "screen_width", l_screen_width);
    lua_register(L, "screen_height", l_screen_height);

    lua_register(L, "create_surface", l_create_surface);
    lua_register(L, "destroy_surface", l_destroy_surface);    
    lua_register(L, "get_surface_height", l_get_surface_height);
    lua_register(L, "set_surface_height", l_set_surface_height);    
    lua_register(L, "get_surface_width", l_get_surface_width);
    lua_register(L, "set_surface_width", l_set_surface_width);    
    lua_register(L, "get_surface_x", l_get_surface_x);    
    lua_register(L, "get_surface_y", l_get_surface_y);        
    lua_register(L, "get_surface_z", l_get_surface_z);        
    lua_register(L, "set_surface_x", l_set_surface_x);    
    lua_register(L, "set_surface_y", l_set_surface_y);        
    lua_register(L, "set_surface_z", l_set_surface_z);        

    lua_register(L, "get_surface_parent", l_get_surface_parent);
    lua_register(L, "set_surface_parent", l_set_surface_parent);
    lua_register(L, "set_surface_font", l_set_surface_font);    
    
    lua_register(L, "set_surface_backcolor", l_set_surface_backcolor);    
    lua_register(L, "set_surface_forecolor", l_set_surface_forecolor);        
    lua_register(L, "set_surface_transcolor", l_set_surface_transcolor);        
    lua_register(L, "get_surface_backcolor", l_get_surface_backcolor);    
    lua_register(L, "get_surface_forecolor", l_get_surface_forecolor);        
    lua_register(L, "get_surface_transcolor", l_get_surface_transcolor);        

    lua_register(L, "get_surface_visible", l_get_surface_visible);
    lua_register(L, "get_surface_rotation", l_get_surface_rotation);
    lua_register(L, "get_surface_alpha", l_get_surface_alpha);
    lua_register(L, "get_surface_transparent", l_get_surface_transparent);
    lua_register(L, "set_surface_visible", l_set_surface_visible);
    lua_register(L, "set_surface_rotation", l_set_surface_rotation);
    lua_register(L, "set_surface_alpha", l_set_surface_alpha);
    lua_register(L, "set_surface_transparent", l_set_surface_transparent);

    lua_register(L, "set_surface_dirty", l_set_surface_dirty);    

    lua_register(L, "set_surface_on_draw", l_set_surface_on_draw);
    lua_register(L, "set_surface_on_got_focus", l_set_surface_on_got_focus);    
    lua_register(L, "set_surface_on_lost_focus", l_set_surface_on_lost_focus);        
    lua_register(L, "set_surface_on_keypress", l_set_surface_on_keypress);    
    lua_register(L, "set_surface_on_mouse_down", l_set_surface_on_mouse_down);
    lua_register(L, "set_surface_on_mouse_up", l_set_surface_on_mouse_up);
    lua_register(L, "set_surface_on_mouse_click", l_set_surface_on_mouse_click);
    
    lua_register(L, "set_surface_bitmap", l_set_surface_bitmap);
    lua_register(L, "get_surface_bitmap", l_get_surface_bitmap);    
    
    lua_register(L, "set_root_surface", l_set_root_surface);
    lua_register(L, "get_root_surface", l_set_root_surface);

    lua_register(L, "gradient", l_draw_gradient);
    lua_register(L, "makecol", l_makecol);
    lua_register(L, "render_loop", l_render_loop);
    lua_register(L, "initialize_video", l_initialize_video);
    
    lua_register(L, "get_surface_by_xy", l_get_surface_by_xy);
    lua_register(L, "get_surface_screen_x", l_get_surface_screen_x);
    lua_register(L, "get_surface_screen_y", l_get_surface_screen_y);
    
    lua_register(L, "set_surface_always_on_top", l_set_surface_always_on_top);
    lua_register(L, "stop_render", l_stop_render);
    lua_register(L, "surface_load_bmp", l_surface_load_bmp);
    
    lua_register(L, "surface_get_text_length", l_surface_get_text_length);
    lua_register(L, "surface_get_text_height", l_surface_get_text_height);

    lua_sethook(L, watchdog, LUA_MASKCOUNT, 1000);
    //lua_runscript_reuse(L, "surface.lua");
}

int _init_thread(char * mod_name) {
    mod_lua_register(mod_name, illution_lua_register_call);
    SetVar(mod_name, "version", VERSION_STRING);
    SetVar(mod_name, "arch", ARCH_STRING);
//    printf("illution Starting Up...\n");
//    printf("Press Enter for Command Prompt.\n");
//    interactive_console_thread();
//    render_loop();
    return 0;
}

