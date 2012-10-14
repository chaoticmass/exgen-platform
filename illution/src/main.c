int illution_exit(int mode) {
    int i = 0;

    if (mode > 0) {
	for(i = 1; i <= surfacecount; i += 1) {
	    if (check_surface(i)) {
		destroy_surface(i);
	    }
	}
    }
//    allegro_exit();
    exit(0);    
    return 0;
}

double timer(void) {
    struct timeval time;

    gettimeofday(&time, NULL);
    return time.tv_sec + time.tv_usec*1e-6;
}

int initialize_video(int video_mode, int l_resx, int l_resy) {
    float FPS = 0;
    int frames;
	
    double lastTime, currentTime, fps;
    struct timeval time;

    gettimeofday(&time, NULL);
    lastTime = time.tv_sec + time.tv_usec*1e-6;

    allegro_init();
    install_timer();

    
    set_color_depth(32);
    illution_resx = l_resx;
    illution_resy = l_resy;
    int retval = set_gfx_mode(video_mode, illution_resx, illution_resy, 0, 0);	
    //int retval = set_gfx_mode(GFX_FBCON, illution_resx, illution_resy, 0, 0);	
    if (retval) {
	printf("Unable to initialize graphics mode.\n");
	return 1;
	//exit(retval);
    }
//    set_gfx_mode(GFX_AUTODETECT, illution_resx, illution_resy, 0, 0);	


// color map init
    create_trans_table(&global_trans_table, palette, 128, 128, 128, NULL);

	 

    install_keyboard();
    install_mouse();
    
    algif_init();
    jpgalleg_init();

//    bscreen = create_bitmap(illution_resx, illution_resy);	
    
    create_surface(illution_resx, illution_resy);
    surfaces[0]->parent = -1;
    
//    create_surface(illution_resx, illution_resy);
//    draw_gradient(surfaces[1]->bmap, 0, 0, surfaces[1]->bmap->w, surfaces[1]->bmap->h, 128, 128, 255, 64, 64, 128);

//    rootscript = lua_runscript_reuse(NULL, "exterm.lua");
//    rootscript = lua_runscript_reuse(NULL, "combo.lua");
    return 0;
}

int render_loop(void) {
    float FPS = 0;
    int frames;
    double last_time = 0;
    double sleep_time = 0;
	
    double lastTime, currentTime, fps;
    struct timeval time;
    
    int i = 0;
    int dirty_count = 0;
    int index = 0;

    gettimeofday(&time, NULL);
    lastTime = time.tv_sec + time.tv_usec*1e-6;
    last_time = timer();
    while (bail == 0) {
	if ((timer() - last_time) > sleep_time) {
	if (check_surface(rootsurface)) {
	    if (pthread_equal(pthread_self(), surfaces[rootsurface]->thread_id)) {
		if (debug_mode) {
		    
		}
		
		
		gtimer = timer();
	    	render_once();
		pthread_yield();
		last_time = timer();
		

/*		dirty_count = 0;
		
	        for (i = 0; i <= surfacecount; i += 1) {
		    index = zorder[surfacecount - i];
		    if (check_surface(index)) {
			if (index != rootsurface) {
			    if (surfaces[index]->parent == rootsurface) {
				if (surfaces[index]->dirty) {
				    dirty_count = dirty_count + 1;
				}
			    }
			}
		    }
		}
		printf("%f render_once(), %i \n", timer(), dirty_count);


		if (dirty_count == 0) {
		    sleep_time = .5;
		} else {
		    sleep_time = 0;
		}*/
		//printf("root surface is owned by main thread\n");
	    } else {
		//pthread_yield();
		//printf("root surface is !NOT! owned by main thread\n");
	    }
	}
	} else {
	    sleep(sleep_time);
	}
    }	
    return 0;
}

int render_once(void) {
    float FPS = 0;
    int frames;
    float min_time = .1;
    int retval = 0;
    int i = 0;
    int index = 0;
	
    double lastTime, currentTime, fps;
    struct timeval time;

    gettimeofday(&time, NULL);
    //lastTime = time.tv_sec + time.tv_usec*1e-6;
    
    
    
    
    
    
//    if ((timer() - last_render_time) > .5) {
	//render_surfaces(rootsurface);
	process_events();
	for (i = 0; i <= surfacecount; i += 1) {
	    index = zorder[surfacecount - i];
	    index = zorder[i];
	    if (check_surface(index)) {
		//if (surfaces[index]->parent != rootsurface) {
		    if (surfaces[index]->visible) {
			if (surfaces[index]->dirty) {
			    call_on_draw(index);
			    //surfaces[index]->dirty = 0;
			}
		    }
		//}
	    }
	}
	
	/*if (debug_mode) {
	for (i = 0; i <= surfacecount; i += 1) {
	    printf("Surface: %i ", i);
	    if (check_surface(i)) {
		if (i == rootsurface) {
		    printf("(Root Surface) W:%i H:%i\n", surfaces[i]->bmap->w, surfaces[i]->bmap->h);
		} else {
		    printf("(Child Surface) W:%i H:%i P: %i\n", surfaces[i]->bmap->w, surfaces[i]->bmap->h, surfaces[i]->parent);
		}
	    } else {
		printf("(nil)\n");
	    }
	}
	printf("\n\n\n");
	}*/
	retval = new_render_surfaces(-1, 0);
	

	last_render_time = timer();
	
//    }
    
    gettimeofday(&time, NULL);
    currentTime = time.tv_sec + time.tv_usec*1e-6;
    tick = currentTime - lastTime;
    
    FPS = 1/(currentTime - lastTime);
    ticker += 1;
    if (ticker > 1000) {
        ticker = 0;
    }	
    lastTime = currentTime;		

    return 0;
}


int new_render_surfaces(int root, int indent) {
    int indent_count = 0;
    int index = 0;
    int retval = 0;
    int i = 0;
    
    /*if (debug_mode) {
    for (indent_count = 0; indent_count < indent; indent_count += 1) {
        printf("-");
    }
    printf("> %i\n", root);
    }*/
    indent = indent + 1;
    
    for (i = 0; i <= surfacecount; i += 1) {
	index = zorder[surfacecount - i];
	//index = zorder[i];
	if (check_surface(index)) {
	    if (index != root) {
		if (surfaces[index]->parent == root) {
		    retval = new_render_surfaces(index, indent);
		    //rect(screen, surfaces[index]->x-1, surfaces[index]->y-1, 1+surfaces[index]->x + surfaces[index]->bmap->w, 1+surfaces[index]->y + surfaces[index]->bmap->h, makecol(255, 255, 255));
		}
	    }
	}
    }
    
    if (check_surface(root)) {
	render_surface(root, surfaces[root]->parent);
    }
    return retval;
}



int render_surface(int root, int parent) {
    int i = 0;
    int p = parent;
    int index = 0;
    int x = 0;
    int y = 0;
    float elapsed_time = 0;
    int current_frame = 0;

    // If parent surface is the screen, then
    if (p == root) {
	return -1;
    }
    if (root == -1) {
	return -1;
    }
    if (p == -1) {
	blit(surfaces[root]->bmap, screen, 0, 0, x, y, surfaces[root]->bmap->w, surfaces[root]->bmap->h);
    } else {
	if (check_surface(p)) {
	    if (check_surface(root)) {
		if (surfaces[root]->bmap != NULL) {
		    //if (surfaces[root]->dirty) {
			if (surfaces[root]->visible) {
			    if (surfaces[root]->frame_count) {
				elapsed_time = timer() - surfaces[root]->frame_timer;
				//printf("I can't even get this basic thing right. %f - %f = %f\n", timer(), surfaces[root]->frame_timer, (timer() - surfaces[root]->frame_timer));
				current_frame = surfaces[root]->current_frame;
				//printf("Duration of frame %i is %i. Elapsed time is %f.\n", current_frame, surfaces[root]->durations[current_frame], elapsed_time);
				if (elapsed_time > (int)surfaces[root]->durations[current_frame] * .01) {
				    //printf("Rendering frame %i\n", current_frame);
				    current_frame += 1;
				    if (current_frame >= surfaces[root]->frame_count) {
					current_frame = 1;
				    }
				    blit(surfaces[root]->frames[current_frame], surfaces[root]->bmap, 0, 0, 0, 0, surfaces[root]->frames[current_frame]->w, surfaces[root]->frames[current_frame]->h);
				    surfaces[root]->frame_timer = timer();
				    surfaces[root]->current_frame = current_frame;
				}
			    }
			    
			    if (surfaces[root]->position_absolute) {
				x = surfaces[root]->x;
				y = surfaces[root]->y;
			    } else {
				x = surfaces[root]->x + surfaces[p]->x_offset;
				y = surfaces[root]->y + surfaces[p]->y_offset;
			    }
			    
			    if (surfaces[root]->transparent) {
				draw_sprite(surfaces[p]->bmap, surfaces[root]->bmap, x, y);
			    } else if (surfaces[root]->alpha) {
				set_trans_blender(128, 128, 128, 255 - surfaces[root]->alpha);
				draw_trans_sprite(surfaces[p]->bmap, surfaces[root]->bmap, x, y);
			    } else if (surfaces[root]->rotation) {
				rotate_sprite(surfaces[p]->bmap, surfaces[root]->bmap, x, y, itofix(surfaces[root]->rotation));
			    } else {
				blit(surfaces[root]->bmap, surfaces[p]->bmap, 0, 0, x, y, surfaces[root]->bmap->w, surfaces[root]->bmap->h);
			    }
			    
			}
		    //} // if dirty
		}
	    }
	}
    }
}


int render_surfaces(int root) {
    int i = 0;
    int index = 0;
    int x = 0;
    int y = 0;
    int p = 0;
    int retval = 0;
    float elapsed_time = 0;
    int current_frame = 0;
    
    
    pthread_attr_t tattr;
    pthread_t tid;

    if (surfaces[root]->parent == rootsurface) {    
//	call_on_draw(root);
    }
//    pthread_mutex_lock(&surface_mutex);

    printf("Render %i\n", root);
    for (i = 0; i <= surfacecount; i += 1) {
	index = zorder[surfacecount - i];
	//index = zorder[i];
	if (check_surface(index)) {
	    if (index != root) {
		if (surfaces[index]->parent == root) {
		    retval = render_surfaces(index);
		}
	    }
	}
    }

    p = surfaces[root]->parent;
if (p == rootsurface) {
    //blit(surfaces[root]->bmap, screen, 0, 0, x, y, surfaces[root]->bmap->w, surfaces[root]->bmap->h);
    //render_surface(p, root);


/* before you think about trying something like THIS again, just accept that
   it won't work this way. You call_on_draw all of an items children, blitting
   the children onto the parent. If you call_on_draw the parent after this (not
   before) then you over-write all the children you just blitted 
   
   also this is a RECURSIVE function. So you can't just call_on_draw during the
   beginning of this function either.

   
    if (check_surface(root)) {
//	if (surfaces[root]->dirty) {
//	    surfaces[root]->dirty = 0;
	    if (root == 6) {
		if (pthread_equal(surfaces[root]->thread_id, pthread_self())) {
		    printf("call_on_draw(%i), render_surfaces\n", root);
		    call_on_draw(root);
		} else {
		    printf("threads not match (%i), render_surfaces\n", root);
		}
	    }
	    
//	}
    }
*/
} else {
//if (p == p) {

    if (check_surface(p)) {
	if (check_surface(root)) {
	    if (surfaces[root]->bmap != NULL) {
		//if (surfaces[root]->dirty) {
		    if (surfaces[root]->visible) {
			//surfaces[p]->dirty = 1;
			
			//x = surfaces[root]->x;
			//y = surfaces[root]->y;
			//printf("render_surface - blit %i onto %i, with y_o %f \n", root, p, surfaces[p]->y_offset);
			if (debug_mode) {
			    if (root == 1) {
				//printf("render_surfaceS - blit %i onto %i\n", root, p);
			    }
			}
			    if (surfaces[root]->frame_count) {
				elapsed_time = timer() - surfaces[root]->frame_timer;
				//printf("I can't even get this basic thing right. %f - %f = %f\n", timer(), surfaces[root]->frame_timer, (timer() - surfaces[root]->frame_timer));
				current_frame = surfaces[root]->current_frame;
				//printf("Duration of frame %i is %i. Elapsed time is %f.\n", current_frame, surfaces[root]->durations[current_frame], elapsed_time);
				if (elapsed_time > (int)surfaces[root]->durations[current_frame] * .01) {
				    //printf("Rendering frame %i\n", current_frame);
				    current_frame += 1;
				    if (current_frame >= surfaces[root]->frame_count) {
					current_frame = 1;
				    }
				    blit(surfaces[root]->frames[current_frame], surfaces[root]->bmap, 0, 0, 0, 0, surfaces[root]->frames[current_frame]->w, surfaces[root]->frames[current_frame]->h);
				    surfaces[root]->frame_timer = timer();
				    surfaces[root]->current_frame = current_frame;
				}
			    }


			if (surfaces[root]->position_absolute) {
			    x = surfaces[root]->x;
			    y = surfaces[root]->y;
			} else {
			    x = surfaces[root]->x + surfaces[p]->x_offset;
			    y = surfaces[root]->y + surfaces[p]->y_offset;
			}
			
			//if (pthread_equal(surfaces[root]->thread_id, pthread_self())) {
			    if (surfaces[root]->dirty) {
				
			    //}
			    //if (1 == 1) {  // keep brackets nice (wtf)
				if (surfaces[root]->transparent) {
				    draw_sprite(surfaces[p]->bmap, surfaces[root]->bmap, x, y);
				} else if (surfaces[root]->alpha) {
				    set_trans_blender(128, 128, 128, 255 - surfaces[root]->alpha);
				    draw_trans_sprite(surfaces[p]->bmap, surfaces[root]->bmap, x, y);
				} else if (surfaces[root]->rotation) {
				    rotate_sprite(surfaces[p]->bmap, surfaces[root]->bmap, x, y, itofix(surfaces[root]->rotation));
				} else {
				    blit(surfaces[root]->bmap, surfaces[p]->bmap, 0, 0, x, y, surfaces[root]->bmap->w, surfaces[root]->bmap->h);
				}
				
				//textprintf_ex(surfaces[root]->bmap, font, 0, 0, surfaces[root]->forecolor, surfaces[root]->backcolor, "S: %i, Z: %i", root, surfaces[root]->z);
				retval = 1;
				
			    }
			//} //pthread_equal
		    }
		//} // if dirty
	    }
	}
    }

//    render_surface(p, root);
}
//    pthread_mutex_unlock(&surface_mutex);
    return retval;
}

int old_render_surfaces() {
    int i = 0;
    int index = 0;
    int x = 0;
    int y = 0;
    int p = 0;

    
    for (i = 0; i < surfacecount; i += 1) {
	index = zorder[i];
	if (check_surface(index)) {
	    if (surfaces[index]->bmap != NULL) {
		if (surfaces[index]->visible) {
		    x = surfaces[index]->x;
		    y = surfaces[index]->y;
		    if (check_surface(surfaces[index]->parent)) {
			x += surfaces[surfaces[index]->parent]->x;
			y += surfaces[surfaces[index]->parent]->y;		    
		    }
		    if (surfaces[index]->transparent) {
			draw_sprite(screen, surfaces[index]->bmap, x, y);
		    } else if (surfaces[index]->alpha) {
		    } else if (surfaces[index]->rotation) {
			rotate_sprite(screen, surfaces[index]->bmap, x, y, itofix(surfaces[index]->rotation));
		    } else {
			blit(surfaces[index]->bmap, screen, 0, 0, x, y, surfaces[index]->bmap->w, surfaces[index]->bmap->h);
		    }
		}
	    }
	}
    }

    return 0;
}


int illution_flip(void) {
//    blit(bscreen, screen, 0, 0,0,0, bscreen->w, bscreen->h);     
//    clear_bitmap(bscreen); 
    return 0;
}

int create_surface(int width, int height) {
    int i = 0;
    int index = -1;
//--pthread_mutex_lock(&surface_mutex);    
    for (i = 0; i < surfacecount; i += 1) {
	if (surfaces[i] == NULL) {
	    index = i;
	    break;
	}
    }
    if (index == -1) {
	surfacecount += 1;
	struct surface ** tmp = calloc(surfacecount, sizeof(struct surface *));
	for (i = 0; i < surfacecount - 1; i += 1) {
	    tmp[i] = surfaces[i];
	}
	if (surfaces != NULL) {
	    free(surfaces);
	}
	index = surfacecount - 1;
	surfaces = calloc(surfacecount, sizeof(struct surface *));
	for (i = 0; i < surfacecount - 1; i += 1) {
	    surfaces[i] = tmp[i];
	}
	if (zorder != NULL) {
	    free(zorder);
	}
	zorder = calloc(surfacecount, sizeof(int));

	free(tmp);
    }
    surfaces[index] = calloc(1, sizeof(struct surface));
    surfaces[index]->index = index;
    surfaces[index]->x = 0;
    surfaces[index]->y = 0;
    surfaces[index]->x_offset = 0;
    surfaces[index]->y_offset = 0;
    surfaces[index]->position_absolute = 0;

    surfaces[index]->z = 1;
    surfaces[index]->z2 = 1;
    surfaces[index]->dirty = 0;
    surfaces[index]->parent = rootsurface;
    surfaces[index]->noevents = 0;
    surfaces[index]->tabindex = -1;
    
    surfaces[index]->has_focus = 0;
    surfaces[index]->can_have_focus = 0;

    surfaces[index]->backcolor = 0;
    surfaces[index]->forecolor = makecol(255, 255, 255);
    surfaces[index]->transcolor = makecol(255, 0, 255);
    surfaces[index]->font = font;
    surfaces[index]->transparent = 0;
    surfaces[index]->alpha = 0;
    surfaces[index]->rotation = 0;
    surfaces[index]->visible = 1;
    surfaces[index]->dirty = 1;    
    surfaces[index]->destroy = 0;
    surfaces[index]->always_on_top = 0;
    
    surfaces[index]->outside_thread_on_draw_request = 0;
    
    surfaces[index]->on_draw = -1;
    surfaces[index]->on_blit = -1;    
    surfaces[index]->on_got_focus = -1;
    surfaces[index]->on_lost_focus = -1;
    surfaces[index]->on_keypress = -1;    
    surfaces[index]->on_mouse_down = -1;    
    surfaces[index]->on_mouse_up = -1;    
    surfaces[index]->on_mouse_click = -1;    
    
    
    surfaces[index]->bmap = create_bitmap(width, height); 
    
    surfaces[index]->frames = NULL;
    surfaces[index]->durations = NULL;
    surfaces[index]->frame_count = 0;
    surfaces[index]->frame_timer = 0;
    surfaces[index]->current_frame = 0;
    
    surfaces[index]->L = NULL;    
    surfaces[index]->thread_id = pthread_self();
    surfaces[index]->lua_mutex = (pthread_mutex_t*) malloc (sizeof(pthread_mutex_t));
    pthread_mutex_init(surfaces[index]->lua_mutex, NULL);
    
//--pthread_mutex_unlock(&surface_mutex);        
    resort_zorder();    

    return index;
}

int destroy_surface(int index) {
    int i = 0;

    if (debug_mode) { 
	printf("destroy surface...%i\n", index);
    }
//--pthread_mutex_lock(&surface_mutex);    
    if (check_surface(index)) {
	if (focus == index) {
	    focus = surfaces[index]->parent;
	    if (check_surface(focus)) {
	    } else {
		focus = 0;
	    }
	}
    
	for(i = 0; i < surfacecount; i += 1) {
	    if (check_surface(i)) {
		if (surfaces[i]->parent == index) {
		    //--pthread_mutex_unlock(&surface_mutex);        
		    destroy_surface(i);
		    //--pthread_mutex_lock(&surface_mutex);        
		}
	    }
	}
	destroy_bitmap(surfaces[index]->bmap);
	
	if (surfaces[index]->frame_count) {
	    for (i = 0; i < surfaces[index]->frame_count; i++) {
		destroy_bitmap(surfaces[index]->frames[i]);
	    }
	    free(surfaces[index]->frames);
	    free(surfaces[index]->durations);
	    surfaces[index]->frame_count = 0;
	}
	
	pthread_mutex_unlock(surfaces[index]->lua_mutex);

	pthread_mutex_destroy(surfaces[index]->lua_mutex);
	free(surfaces[index]);
	surfaces[index] = NULL;
	if (rootsurface == index) {
	    rootsurface = -1;
	}
    }
//--pthread_mutex_unlock(&surface_mutex);        
    if (debug_mode) {
	printf("done\n");
    }
    return 0;
    /*
    for (i = 0; i < surfacecount; i += 1) {
	if (surfaces[i] == NULL) {
	    index = i;
	    break;
	}
    }
    if (index == -1) {
	surfacecount += 1;
	struct surface ** tmp = calloc(surfacecount, sizeof(struct surface *));
	for (i = 0; i < surfacecount - 1; i += 1) {
	    tmp[i] = surfaces[i];
	}
	if (surfaces != NULL) {
	    free(surfaces);
	}
	index = surfacecount - 1;
	surfaces = calloc(surfacecount, sizeof(struct surface *));
	for (i = 0; i < surfacecount - 1; i += 1) {
	    surfaces[i] = tmp[i];
	}
	if (zorder != NULL) {
	    free(zorder);
	}
	zorder = calloc(surfacecount, sizeof(int));

	free(tmp);
    }
    surfaces[index] = calloc(1, sizeof(struct surface));
    surfaces[index]->index = index;
    surfaces[index]->x = 0;
    surfaces[index]->y = 0;
    surfaces[index]->z = 1;
    surfaces[index]->dirty = 0;
    surfaces[index]->parent = 0;

    surfaces[index]->backcolor = 0;
    surfaces[index]->forecolor = makecol(255, 255, 255);
    surfaces[index]->transcolor = makecol(255, 0, 255);
    surfaces[index]->font = font;
    surfaces[index]->transparent = 0;
    surfaces[index]->alpha = 0;
    surfaces[index]->rotation = 0;
    surfaces[index]->visible = 1;
    surfaces[index]->dirty = 1;    
    
    surfaces[index]->on_draw = -1;
    surfaces[index]->on_keypress = -1;    
    
    surfaces[index]->bmap = create_bitmap(width, height); 
    surfaces[index]->L = NULL;    
//--pthread_mutex_unlock(&surface_mutex);        
    resort_zorder();    
//    printf("...create surface done...\n");    
    return index;
    */
}

int get_thread_count(void) {	// luxurious 600 thread count egyptian fibers
    int index = 0;
    int i = 0;
    int found = 0;
    pthread_t threads[surfacecount];	// each surface will have at most
					// one associated thread, so the
					// count will not exceed surfacecount
    pthread_t thread_id;
    int thread_count = 0;
    
    for(i = 0; i <= surfacecount; i += 1) {
	threads[i] = 0;
    }
    
    for(index = 0; index <= surfacecount; index += 1) {
	if (check_surface(index)) {
	    if (index != rootsurface) {
		if (surfaces[index]->parent == rootsurface) {
		    thread_id = surfaces[index]->thread_id;
		    found = 0;
		    for(i = 0; i <= surfacecount; i += 1) {
			if (pthread_equal(thread_id, threads[i])) {
			    found = 1;
			    //printf("found \n");
			    break;
			}
		    }
		    if (!found) {
			for(i = 0; i <= surfacecount; i += 1) {
			    if (threads[i] == 0) {
				threads[i] = thread_id;
				thread_count += 1;
				break;
			    }
			}			
		    }
		}
	    }
	}
    }
    
    /*
    for (i = 0; i <= surfacecount; i += 1) {
	if (threads[i] != 0) {
	    printf("%p thread\n", threads[i]);
	}
    }
    */
    
    return thread_count;
}