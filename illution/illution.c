#define VERSION_STRING 	"0.20A"
#define ARCH_STRING	"linux_x86"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include "allegro.h"
#include <sys/stat.h>
#include "lua.h"
#include <pthread.h>
#include <ode/ode.h>
#include "algif/algif.h"
#include <unistd.h>

#define BITMAP_COLOR_DEPTH "32"

#define ESTIMATED_MAX_JPG_SIZE(bmp)	(((bmp)->w * (bmp)->h * 3) + 1024)



//BITMAP *bscreen2;
PALETTE palette;
COLOR_MAP global_trans_table;

// fonts
BITMAP *b_lucida;
FONT *f_lucida;
BITMAP *b_tahoma;
FONT *f_tahoma;
BITMAP *b_modern;
FONT *f_modern;


int ticker = 0;
double tick = 0;
double last_render_time = 0;
double gtimer = 0;

int illution_resx = 1024;
int illution_resy = 768;

int render_mode = 0;
int render_only_if_dirty = 0;

int debug_mode = 0;
int debug_surfaces = 1;

int dirty = 0;

int focus = 0;
struct surface {
    int index;
    float x; // location
    float y;
    float y_offset;
    float x_offset;
    int position_absolute;
    int z;
    int z2;    
    int dirty;
    int parent;
    int forecolor;
    int backcolor;
    int transcolor;
    int transparent;
    int visible;
    float alpha;
    int rotation;
    int always_on_top;
    int has_focus;
    int can_have_focus;
    int destroy;
    FONT *font;
    BITMAP *bmap;
    
    
    // animated gif
    BITMAP **frames;
    int **durations;
    int frame_count;
    double frame_timer;
    int current_frame;

    int noevents;
    int tabindex;
    
    int on_draw;
    int on_got_focus;
    int on_lost_focus;
    int on_keypress;    
    int on_mouse_down;
    int on_mouse_up;
    int on_mouse_click;
    int on_blit;
    
    int outside_thread_on_draw_request;
    
    pthread_t thread_id;
    void * L;	// lua script
    pthread_mutex_t * lua_mutex;	// mutex for lua script
					// so we don't clobber the stack
					// when calling lua functions in a 
					// threaded environment
};

struct event {
    int index;
    char name [32];
    pthread_t thread_id;
    double (*event_proc)(int);
};
struct event **events = NULL;
int event_count = 0;
pthread_mutex_t *event_mutex;

struct thread_safe_call_data {
    lua_State * L;
    int args;
};

int bail = 0;

struct surface **surfaces = NULL;
pthread_mutex_t surface_mutex = PTHREAD_MUTEX_INITIALIZER;

int surfacecount = 0;
int *zorder = NULL;

void * rootscript = NULL;
int rootsurface = 0;

int last_mouse_b;
int last_mouse_x;
int last_mouse_y;
int last_mouse_surface_b1 = -1;
int last_mouse_surface_b2 = -1;






// Constants
const float PI = 3.1415926;
const float Deg2Rad = 0.01745329222222222;
const float Rad2Deg = 57.2957795;
const float All2Rad = 0.0245436926;
const float Rad2All = 40.7436655;
const float deg180_rad = 3.14159265;

float fov = 60;
float aspect = 1.33f;

// polygons

struct poly_type {
    V3D_f vx[4];
    V3D_f *vp[4];
    int *next_poly;
};


V3D_f vertex[4];
V3D_f *vertptr[4];
V3D_f pvector[1];
V3D_f pvertex;
V3D_f tvout[4];


struct camera_type {
    MATRIX_f camera;
    MATRIX_f roller;
    float x;
    float y;
    float z;
    float pitch;
    float heading;
    float r_x;
    float r_y;
    float r_z;
    float fov;
    BITMAP *bmp;
    ZBUFFER *zbuf;    
};


struct trans_type {
    MATRIX_f matrix;
    QUAT rotation;
    float x;
    float y;
    float z;
    float r_x;
    float r_y;
    float r_z;
    float scale;
};


// models
struct model_type {
    int index;
    struct mquad **faces;
    int facecount;
    float rx;
    float ry;
    float rz;
    float ox;
    float oy;
    float oz;
};

struct mquad {
    V3D_f v[4];
    V3D_f m;
};

// ode

struct worldspace_type {
    dWorldID world;
    dSpaceID space;
    dJointGroupID contactgroup;
    double lasttime;
};


struct body_type {
    dBodyID body;
    dMass mass;
    dGeomID geom;
    int heightfield_data_id;
    int type_num;
    struct worldspace_type * world;
};


int ode_initialized = 0;

struct model_type **models = NULL;
int modelcount = 0;

int call_on_draw(int);int illution_exit(int mode) {
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
}int run_queue_thread(pthread_t);
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


/*
int list_menu(char **caption_list, int ox, int oy) {
    int i = 0;
    int index = 0;
    char *foo = NULL;
    int m_width = 0;
    int m_height = 0;
    int item_count = 0;
    int fgcolor = makecol(64, 64, 255);
    int bgcolor = makecol(0, 0, 0);
    int txcolor = makecol(255, 255, 255);
    int inkey = 0;
    int keyval = 0;
    int keycode = 0;
    int selected = 0;
    int start = 0;
    BITMAP *bmenu = NULL;
    char **lame = NULL; // this is lame
    
    while(caption_list[i] != NULL) {
	i += 1;
	item_count += 1;
    }
	for(i = 0; i < item_count; i += 1) {
	    if (text_length(f_modern, caption_list[i]) > m_width) {
		m_width = text_length(f_modern, caption_list[i]);
	    }
	}
	m_width += 8;
	m_height = text_height(f_modern) * item_count;
	if (oy + m_height > bmenu->h) {
	    m_height = bmenu->h - oy;
	}
	if (ox == -1) {
	    ox = 320 - (m_width * .5);
	    oy = 240 - (m_height * .5);
	}
	while (1 == 1) {
	    draw_list(bmenu, caption_list, selected, &start, ox, oy, ox + m_width, oy + m_height, 0); //ox, oy, ox + m_width, oy + m_height, 0);
	    pthread_yield();
	    inkey = readkey();
	    keycode = inkey >> 8;
	    keyval = inkey & 0xff;
	    //printf("Selected: %i, You pressed %i\n", selected, keycode);
	    
	    //
	    if (keycode == 85) { // arrow down
		selected += 1;
	    }
	    if (keycode == 84) { // arrow up
		selected -= 1;
	    }
	    if (keycode == 59) { // arrow up
		selected = -1;
		break;
	    }

	    if (keycode == 67) { // enter
		break;
	    }
	    if (selected >= item_count) {
		selected = 0;
	    }
	    if (selected < 0) {
		selected = item_count - 1;
	    }	    
	}
//	clear_to_color(bmenu, trans_color);
	return selected;
}

int draw_list(BITMAP *bmp, char **caption_list, int selected, int *start, int x1, int y1, int x2, int y2, int no_scrollbar) {
    int i = 0;
    int index = 0;
    char *foo = NULL;
    int m_width = x2 - x1;
    int m_height = y2 - y1;
    int item_count = 0;
    int fgcolor = makecol(64, 64, 255);
    int bgcolor = makecol(0, 0, 0);
    int txcolor = makecol(255, 255, 255);
    int inkey = 0;
    int keyval = 0;
    int keycode = 0;
    int max_items = 0;
    int total_height = 0;
    int scroll_height = 0;
    int scroll_pos = 0;
    
    i = 0;
    while (caption_list[i] != NULL) {
	item_count += 1;
	i += 1;
    }

    total_height = text_height(f_modern) * item_count;
    if (total_height != m_height) {
	scroll_height = (float)m_height * (float)((float)(1 / (float)total_height) * (float)m_height);
    } else {
	no_scrollbar = 1;
    }
    if (item_count == 0) {
	no_scrollbar = 1;
    }

    max_items = (m_height / text_height(f_modern));
    if (selected - *start >= max_items - 1) {
	*start = selected - (max_items - 1);
    }
    if (selected - *start < 0) {
	*start = selected;
    }
    if (max_items == item_count) {
	no_scrollbar = 1;
    }
    if (no_scrollbar == 0) {
	scroll_pos = (float)((m_height - scroll_height) / (item_count - max_items)) * *start;
    }
    draw_gradient(bmp, x1, y1, x2, y2, 128, 128, 128, 64, 64, 64);
    draw_gradient(bmp, x1, y1 + (text_height(f_modern) * (selected - *start)), x2, y1 + (text_height(f_modern) * ((selected - *start) + 1)), 128, 128, 190, 64, 64, 128);

    if (no_scrollbar == 0) {    
	draw_gradient(bmp, x2 - 8, y1, x2, y2, 64, 64, 64, 32, 32, 32);
	draw_gradient(bmp, x2 - 8, y1 + scroll_pos, x2, y1 + scroll_pos + scroll_height, 64, 64, 128, 32, 32, 128);
    }
    
    i = *start;
    
    while ((caption_list[i] != NULL) && (i < max_items + *start)) {	
	textprintf_ex(bmp, f_modern, x1, y1 + (text_height(f_modern) * (i - *start)), txcolor, -1, "%s", caption_list[i]);
	i += 1;
    }
}


int draw_message(BITMAP *bmp, char *title, char *text, int waitforkey) {
    int i = 0;
    int x1 = 0;
    int x2 = 0;
    int y1 = 0;
    int y2 = 0;
    int th = text_height(f_modern);
    int m_width = 0;
    int m_height = 0;
    
    m_width = text_length(f_modern, title);
    if (text_length(f_modern, text) > m_width) {
	m_width = text_length(f_modern, text);
    }
    m_width += 16;
    m_height = th * 3 + 16;
    
    x1 = (bmp->w * .5) - (m_width * .5);
    y1 = (bmp->h * .5) - (m_height * .5);
    x2 = x1 + m_width;
    y2 = y1 + m_height;
    
    draw_gradient(bmp, x1, y1, x2, y2, 128, 128, 128, 64, 64, 64);
    draw_gradient(bmp, x1, y1, x2, y1 + th + 8, 128, 128, 190, 64, 64, 128);

    textprintf_ex(bmp, f_modern, x1, y1 + 4, 0, -1, "%s", title);
    textprintf_ex(bmp, f_modern, x1, y1 + 8 + (th * 1.5), 0, -1, "%s", text);
    
    if (waitforkey) {
	readkey();
    }
    return 0;
}

*/
int draw_gradient(BITMAP *bmp, int x1, int y1, int x2, int y2, int r1, int g1, int b1, int r2, int g2, int b2, int border) {
    int x = 0;
    int y = 0;
    float dist = y2 - y1;
    float rs = (r2 - r1) / dist;
    float gs = (g2 - g1) / dist;
    float bs = (b2 - b1) / dist;
    float r = r1;
    float g = g1;
    float b = b1;
    
    int color;
    for (y = y1; y <= y2; y += 1) {
	color = makecol(r, g, b);
	fastline(bmp, x1, y, x2, y, color);
	r += rs;
	g += gs;
	b += bs;
    }
    if (border == 1) {
	rect(bmp, x1, y1, x2, y2, makecol(0, 0, 0));
	fastline(bmp, x1 + 1, y1 + 1, x2 - 1, y1 + 1, makecol(255, 255, 255));
	fastline(bmp, x1 + 1, y1 + 1, x1 + 1, y2 - 1, makecol(255, 255, 255));    
	fastline(bmp, x1 + 1, y2 - 1, x2 - 1, y2 - 1, 0);
	fastline(bmp, x2 - 1, y1 + 1, x2 - 1, y2 - 1, 0);    
    }
    return 0;
}


long filelength(int f) {
    struct stat buf;
    fstat(f, &buf);
    return(buf.st_size);
}

int load3ds (char *p_filename, struct model_type * model)
{
    int i; //Index variable
	
    FILE *l_file; //File pointer
    float *ptmp;
    float tmpval = 0;
    ptmp = &tmpval;
    unsigned short l_chunk_id; //Chunk identifier
    unsigned int l_chunk_lenght; //Chunk lenght
    unsigned char l_char; //Char variable
    unsigned short l_qty; //Number of elements in each chunk
    unsigned short l_face_flags; //Flag that stores some face information
    unsigned short tmpf;
    unsigned short vcount;
    float fcount = 0;
    int index = 0;
    V3D_f *v = NULL;
    int *map = NULL;
//    float conv = 1 / 512;

    if ((l_file=fopen (p_filename, "rb"))== NULL) return 0; //Open the file
    while (ftell(l_file) < filelength(fileno(l_file))) {
	fread (&l_chunk_id, 2, 1, l_file); //Read the chunk header
	////printf("ChunkID: %x\n",l_chunk_id); 
	
	fread (&l_chunk_lenght, 4, 1, l_file); //Read the lenght of the chunk
	
	switch (l_chunk_id) {
			//----------------- MAIN3DS -----------------
			// Description: Main chunk, contains all the other chunks
			// Chunk ID: 4d4d 
			// Chunk Lenght: 0 + sub chunks
			//-------------------------------------------
	case 0x4d4d: break;    
			//----------------- EDIT3DS -----------------
			// Description: 3D Editor chunk, objects layout info 
			// Chunk ID: 3d3d (hex)
			// Chunk Lenght: 0 + sub chunks
			//-------------------------------------------
	case 0x3d3d: break;			
			//--------------- EDIT_OBJECT ---------------
			// Description: Object block, info for each object
			// Chunk ID: 4000 (hex)
			// Chunk Lenght: len(object name) + sub chunks
			//-------------------------------------------
	case 0x4000: 
	    i=0;
	    do {
		fread (&l_char, 1, 1, l_file);
                //p_object->name[i]=l_char;
		i++;
            } while(l_char != '\0' && i<20);
    	break;
			//--------------- OBJ_TRIMESH ---------------
			// Description: Triangular mesh, contains chunks for 3d mesh info
			// Chunk ID: 4100 (hex)
			// Chunk Lenght: 0 + sub chunks
			//-------------------------------------------
	case 0x4100: break;			
			//--------------- TRI_VERTEXL ---------------
			// Description: Vertices list
			// Chunk ID: 4110 (hex)
			// Chunk Lenght: 1 x unsigned short (number of vertices) 
			//             + 3 x float (vertex coordinates) x (number of vertices)
			//             + sub chunks
			//-------------------------------------------
	case 0x4110: 
	    //printf("VERTICES : %x\n",l_chunk_id); 
    	    fread (&l_qty, sizeof (unsigned short), 1, l_file);
            //vertices_qty = l_qty;
	    vcount = l_qty;
	    if (v != NULL) {
		free(v);
		free(map);
	    }
	    v = calloc(l_qty, sizeof(V3D_f));
	    map = calloc(l_qty, sizeof(int));
            //printf("Number of vertices: %d\n",l_qty);
            for (i=0; i<l_qty; i++) {
		fread (&v[i].x, sizeof(float), 1, l_file);
    		fread (&v[i].z, sizeof(float), 1, l_file);
		fread (&v[i].y, sizeof(float), 1, l_file);
		//v[i].y = 0 - v[i].y;
	    }
	break;

			//--------------- TRI_FACEL1 ----------------
			// Description: Polygons (faces) list
			// Chunk ID: 4120 (hex)
			// Chunk Lenght: 1 x unsigned short (number of polygons) 
			//             + 3 x unsigned short (polygon points) x (number of polygons)
			//             + sub chunks
			//-------------------------------------------
	case 0x4120:
	    //printf("POLYGONS: %x\n",l_chunk_id); 
	    fread (&l_qty, sizeof (unsigned short), 1, l_file);
            //polygons_qty = l_qty;
            //printf("Number of polygons: %d\n",l_qty); 
            for (i=0; i<l_qty; i++) {
		addface(model);
		index = fcount + i;
		fread (&tmpf, sizeof (unsigned short), 1, l_file);
		model->faces[index]->v[0].x = v[tmpf].x;
		model->faces[index]->v[0].y = v[tmpf].y;
		model->faces[index]->v[0].z = v[tmpf].z;
		model->faces[index]->v[0].u = v[tmpf].u;
		model->faces[index]->v[0].v = v[tmpf].v;
		fread (&tmpf, sizeof (unsigned short), 1, l_file);
		model->faces[index]->v[1].x = v[tmpf].x;
		model->faces[index]->v[1].y = v[tmpf].y;
		model->faces[index]->v[1].z = v[tmpf].z;
		model->faces[index]->v[1].u = v[tmpf].u;
		model->faces[index]->v[1].v = v[tmpf].v;
		fread (&tmpf, sizeof (unsigned short), 1, l_file);
		model->faces[index]->v[2].x = v[tmpf].x;
		model->faces[index]->v[2].y = v[tmpf].y;
		model->faces[index]->v[2].z = v[tmpf].z;
		model->faces[index]->v[2].u = v[tmpf].u;
		model->faces[index]->v[2].v = v[tmpf].v;
		fread (&tmpf, sizeof (unsigned short), 1, l_file);
		
	    }
	    fcount += l_qty;
    	    break;

			//------------- TRI_MAPPINGCOORS ------------
			// Description: Vertices list
			// Chunk ID: 4140 (hex)
			// Chunk Lenght: 1 x unsigned short (number of mapping points) 
			//             + 2 x float (mapping coordinates) x (number of mapping points)
			//             + sub chunks
			//-------------------------------------------
	case 0x4140:
	    //printf("MAPPING: %x\n",l_chunk_id); 
	    //printf("Number of mapping: %d\n",l_qty);
	    fread (&l_qty, sizeof (unsigned short), 1, l_file);
	    for (i=0; i<l_qty; i++) {		
		fread (&tmpval, sizeof (float), 1, l_file);
		tmpval *= 256;
		v[i].v = tmpval;
		//allegro_message("%f\n", tmpval);
                fread (&tmpval, sizeof (float), 1, l_file);
		tmpval *= 256;
		v[i].u = tmpval;
		//allegro_message("%f\n", tmpval);
	    }
        break;

			//----------- Skip unknow chunks ------------
			//We need to skip all the chunks that currently we don't use
			//We use the chunk lenght information to set the file pointer
			//to the same level next chunk
			//-------------------------------------------
	default:
	    fseek(l_file, l_chunk_lenght-6, SEEK_CUR);
        } 
    }
//    allegro_message("vcount: %i\n", vcount);
    fclose(l_file); // Closes the file stream
    return 1; // Returns ok
}


/*
int renderhudentity(int entityindex) {
    int i = 0;
    int index = 0;
    int f = 0;
    int e = 0;
    float tmp;
    int facecount = 0;
    int selected = 0;
    float ax = 0;
    float ay = 0;
    float az = 0;
    int area = 0;
    MATRIX_f matrix;
    
    area = entities[playerentity]->area;
    
    V3D_f *v[4];
    V3D_f vout[4];
    index = entityindex;
	i = entities[index]->model;
	//entities[index]->scale
	get_transformation_matrix_f(&matrix, 1,
	    entities[index]->rx + models[i]->rx, entities[index]->ry + models[i]->ry, entities[index]->rz + models[i]->rz,
	    //entities[index]->x, entities[index]->y, entities[index]->z);
	    0, 0, 400);
	
	facecount = models[i]->facecount;    
	//if (facecount > 1) {
	//    struct mquad **faces;
	//    faces = models[i]->faces;
	//    //qsort(faces, facecount, sizeof(struct mquad **), model_qsort_cmp);
	//}
	for (f = 0; f < facecount; f += 1) {
	    for (e = 0; e < 3; e += 1) {
		vout[e].x = models[i]->faces[f]->v[e].x;
		vout[e].y = models[i]->faces[f]->v[e].y;
		vout[e].z = models[i]->faces[f]->v[e].z;
		vout[e].u = models[i]->faces[f]->v[e].u;
		vout[e].v = models[i]->faces[f]->v[e].v;
		apply_matrix_f(&matrix, vout[e].x, vout[e].y, vout[e].z, &vout[e].x, &vout[e].y, &vout[e].z);
	    }
	    drawquad4(&hudcamera, &vout[0], &vout[1], &vout[2], entities[index]->texture);
	}
}

*/
/*
int createmodel(void) {
    modelcount += 1;
//    struct model ** tmpmodels = calloc(modelcount, sizeof(struct model));
    struct model ** tmpmodels = calloc(modelcount, sizeof(struct model *));
    int i = 0;
    for (i = 0; i < modelcount - 1; i += 1) {
	tmpmodels[i] = models[i];
    }
    if (models != NULL) {
	free(models);
    }
    models = calloc(modelcount, sizeof(struct model *));
    //models[modelcount - 1] = malloc(sizeof(struct model));
    models[modelcount - 1] = calloc(1, sizeof(struct model));
    models[modelcount - 1]->facecount = 0;
    for (i = 0; i < modelcount - 1; i += 1) {
	models[i] = tmpmodels[i];
    }    
    free(tmpmodels);
}
*/

int addface(struct model_type * model) {
    int facecount = 0;
    facecount = model->facecount;
    facecount += 1;
//    struct mquad ** tmpfaces = calloc(facecount, sizeof(struct mquad ));  // why does this work?
    struct mquad ** tmpfaces = calloc(facecount, sizeof(struct mquad *));
    int i = 0;
    for (i = 0; i < facecount - 1; i += 1) {
	tmpfaces[i] = model->faces[i];
    }
    if (facecount - 1 > 0) {
	free(model->faces);
    }
    model->faces = calloc(facecount, sizeof(struct mquad *));
//    model->faces[facecount - 1] = malloc(sizeof(struct mquad));
    model->faces[facecount - 1] = calloc(1, sizeof(struct mquad));
    model->faces[facecount - 1]->v[0].x = 0;
    model->faces[facecount - 1]->v[0].y = 64;
    model->faces[facecount - 1]->v[0].z = 0;
    model->faces[facecount - 1]->v[0].c = makecol(128, 0, 0);

    model->faces[facecount - 1]->v[1].x = 64;
    model->faces[facecount - 1]->v[1].y = 64;
    model->faces[facecount - 1]->v[1].z = 0;
    model->faces[facecount - 1]->v[1].c = makecol(0, 128, 0);

    model->faces[facecount - 1]->v[2].x = 64;
    model->faces[facecount - 1]->v[2].y = 64;
    model->faces[facecount - 1]->v[2].z = 64;
    model->faces[facecount - 1]->v[2].c = makecol(0, 0, 128);

    model->faces[facecount - 1]->v[3].x = 0;
    model->faces[facecount - 1]->v[3].y = 64;
    model->faces[facecount - 1]->v[3].z = 64;
    model->faces[facecount - 1]->v[3].c = makecol(0, 0, 0);

    for (i = 0; i < facecount - 1; i += 1) {
	model->faces[i] = tmpfaces[i];
    }    
    model->facecount = facecount;
}



int draw_polygon(struct camera_type *camera, V3D_f *v1, V3D_f *v2, V3D_f *v3, BITMAP *texture_bmp, BITMAP *target) {
    MATRIX_f *cam = &camera->camera;
    V3D_f _vout[8], _vtmp[8];
    V3D_f *v[4], *vout[8], *vtmp[8];
    int flags[4], out[8];
    int c, vc;

    v[0] = v1;
    v[1] = v2;
    v[2] = v3;
    for (c = 0; c < 8; c += 1) {
	vout[c] = &_vout[c];
	vtmp[c] = &_vtmp[c];
    }

    for (c = 0; c < 3; c += 1) {
	apply_matrix_f(cam, v[c]->x, v[c]->y, v[c]->z, 
			    &v[c]->x, &v[c]->y, &v[c]->z);
	flags[c] = 0;
	if (v[c]->x < -v[c]->z) {
	    flags[c] |= 1;
	} else if (v[c]->x > v[c]->z) {
	    flags[c] |= 2;
	} 
	
	if (v[c]->y < -v[c]->z) {
	    flags[c] |= 4;
	} else if (v[c]->y > v[c]->z) {
	    flags[c] |= 8;
	}
	if (v[c]->z < 0.1) {
	    flags[c] |= 16;
	}
	
    }
    if (flags[0] & flags[1] & flags[2]) {
	//printf("A exited early\n");
	return 0;
    }

    if (flags[0] | flags[1] | flags[2]) {
	vc = clip3d_f(POLYTYPE_PTEX , 0.1, 0.1, 3, (AL_CONST V3D_f **)v, vout, vtmp, out);
	if (vc <= 0) {
	    //printf("B exited early\n");
	    return 1;
	}	
    } else {
	vout[0] = v[0];
	vout[1] = v[1];	
	vout[2] = v[2];
	
	vc = 3;
    }

    for (c = 0; c < vc; c += 1) {
	persp_project_f(vout[c]->x, vout[c]->y, vout[c]->z,
			&vout[c]->x, &vout[c]->y);
	
    }
    polygon3d_f(target, POLYTYPE_PTEX | POLYTYPE_ZBUF, texture_bmp, vc, vout);
    return 0;
}

int draw_polygon_gouraud(struct camera_type *camera, V3D_f *v1, V3D_f *v2, V3D_f *v3, BITMAP *target) {
    MATRIX_f *cam = &camera->camera;
    V3D_f _vout[8], _vtmp[8];
    V3D_f *v[4], *vout[8], *vtmp[8];
    int flags[4], out[8];
    int c, vc;

    v[0] = v1;
    v[1] = v2;
    v[2] = v3;
    for (c = 0; c < 8; c += 1) {
	vout[c] = &_vout[c];
	vtmp[c] = &_vtmp[c];
    }

    for (c = 0; c < 3; c += 1) {
	apply_matrix_f(cam, v[c]->x, v[c]->y, v[c]->z, 
			    &v[c]->x, &v[c]->y, &v[c]->z);
	flags[c] = 0;
	if (v[c]->x < -v[c]->z) {
	    flags[c] |= 1;
	} else if (v[c]->x > v[c]->z) {
	    flags[c] |= 2;
	} 
	
	if (v[c]->y < -v[c]->z) {
	    flags[c] |= 4;
	} else if (v[c]->y > v[c]->z) {
	    flags[c] |= 8;
	}
	if (v[c]->z < 0.1) {
	    flags[c] |= 16;
	}
	
    }
    if (flags[0] & flags[1] & flags[2]) {
	//printf("A exited early\n");
	return 0;
    }

    if (flags[0] | flags[1] | flags[2]) {
	vc = clip3d_f(POLYTYPE_GCOL , 0.1, 0.1, 3, (AL_CONST V3D_f **)v, vout, vtmp, out);
	if (vc <= 0) {
	    //printf("B exited early\n");
	    return 1;
	}	
    } else {
	vout[0] = v[0];
	vout[1] = v[1];	
	vout[2] = v[2];
	
	vc = 3;
    }

    for (c = 0; c < vc; c += 1) {
	persp_project_f(vout[c]->x, vout[c]->y, vout[c]->z,
			&vout[c]->x, &vout[c]->y);
	
    }
    polygon3d_f(target, POLYTYPE_GCOL | POLYTYPE_ZBUF, NULL, vc, vout);
    return 0;
}


int draw_polygon_wireframe(struct camera_type *camera, V3D_f *v1, V3D_f *v2, V3D_f *v3, BITMAP *target) {
    MATRIX_f *cam = &camera->camera;
    V3D_f _vout[8], _vtmp[8];
    V3D_f *v[4], *vout[8], *vtmp[8];
    int flags[4], out[8];
    int c, vc;

    v[0] = v1;
    v[1] = v2;
    v[2] = v3;
    for (c = 0; c < 8; c += 1) {
	vout[c] = &_vout[c];
	vtmp[c] = &_vtmp[c];
    }

    for (c = 0; c < 3; c += 1) {
	apply_matrix_f(cam, v[c]->x, v[c]->y, v[c]->z, 
			    &v[c]->x, &v[c]->y, &v[c]->z);
	flags[c] = 0;
	if (v[c]->x < -v[c]->z) {
	    flags[c] |= 1;
	} else if (v[c]->x > v[c]->z) {
	    flags[c] |= 2;
	} 
	
	if (v[c]->y < -v[c]->z) {
	    flags[c] |= 4;
	} else if (v[c]->y > v[c]->z) {
	    flags[c] |= 8;
	}
	if (v[c]->z < 0.1) {
	    flags[c] |= 16;
	}
	
    }
    if (flags[0] & flags[1] & flags[2]) {
	//printf("A exited early\n");
	return 0;
    }

    if (flags[0] | flags[1] | flags[2]) {
	vc = clip3d_f(POLYTYPE_GCOL , 0.1, 0.1, 3, (AL_CONST V3D_f **)v, vout, vtmp, out);
	if (vc <= 0) {
	    //printf("B exited early\n");
	    return 1;
	}	
    } else {
	vout[0] = v[0];
	vout[1] = v[1];	
	vout[2] = v[2];
	
	vc = 3;
    }

    for (c = 0; c < vc; c += 1) {
	persp_project_f(vout[c]->x, vout[c]->y, vout[c]->z,
			&vout[c]->x, &vout[c]->y);
	
	if (c > 0) {
	    fastline(target, vout[c-1]->x, vout[c-1]->y, vout[c]->x, vout[c]->y, vout[c]->c);
	}
    }
    //polygon3d_f(target, POLYTYPE_GCOL | POLYTYPE_ZBUF, NULL, vc, vout);
    return 0;
}

int plot_polygon(struct camera_type *camera, V3D_f *v1, V3D_f *v2, V3D_f *v3, BITMAP *target) {
    MATRIX_f *cam = &camera->camera;
    V3D_f _vout[8], _vtmp[8];
    V3D_f *v[4], *vout[8], *vtmp[8];
    int flags[4], out[8];
    int c, vc;

    v[0] = v1;
    v[1] = v2;
    v[2] = v3;
    for (c = 0; c < 8; c += 1) {
	vout[c] = &_vout[c];
	vtmp[c] = &_vtmp[c];
    }

    for (c = 0; c < 3; c += 1) {
	apply_matrix_f(cam, v[c]->x, v[c]->y, v[c]->z, 
			    &v[c]->x, &v[c]->y, &v[c]->z);
	flags[c] = 0;
	if (v[c]->x < -v[c]->z) {
	    flags[c] |= 1;
	} else if (v[c]->x > v[c]->z) {
	    flags[c] |= 2;
	} 
	
	if (v[c]->y < -v[c]->z) {
	    flags[c] |= 4;
	} else if (v[c]->y > v[c]->z) {
	    flags[c] |= 8;
	}
	if (v[c]->z < 0.1) {
	    flags[c] |= 16;
	}
	
    }
    if (flags[0] & flags[1] & flags[2]) {
	//printf("A exited early\n");
	return 0;
    }

    if (flags[0] | flags[1] | flags[2]) {
	vc = clip3d_f(POLYTYPE_GCOL , 0.1, 0.1, 3, (AL_CONST V3D_f **)v, vout, vtmp, out);
	if (vc <= 0) {
	    //printf("B exited early\n");
	    return 1;
	}	
    } else {
	vout[0] = v[0];
	vout[1] = v[1];	
	vout[2] = v[2];
	
	vc = 3;
    }

    for (c = 0; c < vc; c += 1) {
	persp_project_f(vout[c]->x, vout[c]->y, vout[c]->z,
			&vout[c]->x, &vout[c]->y);
	
	printf("X: %f, Y: %f, Z: %f\n", vout[c]->x, vout[c]->y, vout[c]->z);
    }
    //polygon3d_f(target, POLYTYPE_GCOL | POLYTYPE_ZBUF, NULL, vc, vout);
    return 0;
}

/*
static dWorldID world;
static dSpaceID space;
static dBodyID body;
static dGeomID geom;
static dMass m;
static dJointGroupID contactgroup;

*/


// dummy __cxa_pure_virtual
void __cxa_pure_virtual(void) {
}


int l_create_worldspace(void * L) {
    struct worldspace_type *ws = calloc(1, sizeof(struct worldspace_type));
    int n = lua_gettop(L);
    int gravity = 0;
    
    
    if (ode_initialized == 0) {
	//dInitODE();
	ode_initialized = 1;
    }
    ws->world = dWorldCreate();
    ws->space = dHashSpaceCreate(0);
    ws->contactgroup = dJointGroupCreate(0);
    ws->lasttime = timer();
    
    //set sane defaults
    if (n >= 1) {
	gravity = lua_tonumber(L, 1);
    } else {
	gravity = -9.81;
    }

    dWorldSetGravity(ws->world, 0, 0, gravity);
    dWorldSetCFM(ws->world, 1e-5);
    
    lua_pushlightuserdata(L, ws);
    return 1;
}

int l_destroy_worldspace(void * L) {
    struct worldspace_type *ws = lua_touserdata(L, 1);

    dSpaceDestroy(ws->space);
    dWorldDestroy(ws->world);
    free(ws);
    return 0;
}

int l_create_plane(void * L) {
    struct worldspace_type *ws = lua_touserdata(L, 1);
    
    lua_pushlightuserdata(L, dCreatePlane(ws->space, 0,0,1,0));
    return 1;
}

int l_plane_set_geom(void * L) {
    dGeomID gid = lua_touserdata(L, 1);
    
    dGeomPlaneSetParams(gid, lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5));
    return 0;
}




/*dReal heightfield_callback(void *user_data, int x, int z) {
    return 0;
}*/

int l_create_body(void * L) {
    struct worldspace_type *ws = lua_touserdata(L, 1);
    int n = lua_gettop(L);
    char *type = NULL;
    int type_num = -1;
    dReal radius = 0.5; 
    dReal mass = 1;
    dReal lx = 0;
    dReal ly = 0;
    dReal lz = 0;
    
    
    if (n >= 2) {
	type = lua_tostring(L, 2);
	if (strcmp(type, "sphere") == 0) {
	    type_num = 0;
	    radius = lua_tonumber(L, 3);
	    mass = lua_tonumber(L, 4);	    
	} else if (strcmp(type, "box") == 0) {
	    type_num = 1;
	    
	    // side lengths
	    lx = lua_tonumber(L, 3);
	    ly = lua_tonumber(L, 4);
	    lz = lua_tonumber(L, 5);
	    
	    mass = lua_tonumber(L, 6);
	} else if (strcmp(type, "trimesh") == 0) {
	    type_num = 2;
	} else if (strcmp(type, "heightfield") == 0) {
	//    type_num = 3;
	}
    }
    
    struct body_type *b = calloc(1, sizeof(struct body_type));
    
    b->body = dBodyCreate(ws->world);
    b->type_num = type_num;
    b->world = &ws;

    if (type_num == 0) {
	b->geom = dCreateSphere(ws->space, radius);
	dMassSetSphereTotal(&b->mass, mass, radius);
	dBodySetMass(b->body, &b->mass);
	dGeomSetBody(b->geom, b->body);
	dBodySetPosition(b->body, 0,0,3);

    } else if (type_num == 1) {
	b->geom = dCreateBox(ws->space, lx, ly, lz);
	dMassSetBoxTotal(&b->mass, mass, lx, ly, lz);
	dBodySetMass(b->body, &b->mass);
	dGeomSetBody(b->geom, b->body);
	dBodySetPosition(b->body, 0,0,3);

    } else if (type_num == 2) {
    } else if (type_num == 3) {
/*    
	float *height_data = calloc(10*10*4, sizeof(float));
	dReal width = 10;
	dReal depth = 10;
	int width_samples = 2;
	int depth_samples = 2;
	dReal scale = 1;
	dReal offset = 0;
	dReal thickness = 1;
	int wrap = 0;
	b->heightfield_data_id = dGeomHeightfieldDataCreate();
	printf("Heightfield Data ID: %i\n", b->heightfield_data_id);
	/dGeomHeightfieldDataBuildSingle (b->heightfield_data_id,
                                      height_data,
                                      1,
                                      width, depth,
                                      width_samples, depth_samples,
                                      scale, offset, thickness, wrap);	/
	dGeomHeightfieldDataBuildCallback(b->heightfield_data_id, NULL, 
	heightfield_callback, width, depth, width_samples, depth_samples, 
	1, 0, 0, 0);
	dGeomHeightfieldDataSetBounds(b->heightfield_data_id, -4, +6);
	
	
        b->geom = dCreateHeightfield(ws->space, b->heightfield_data_id, 1);
        
/        dVector3 pos;
        pos[0] = 0;
        pos[1] = 0;
        pos[2] = 0;
        
        dMatrix3 R;
        dRSetIdentity(R);
        dRFromAxisAndAngle(R, 1, 0, 0, Deg2Rad*90);
        
        dGeomSetRotation(b->geom, R);
        dGeomSetPosition(b->geom, pos[0], pos[1], pos[2]);/
        
        
        free(height_data);*/
    }
    	
    
    lua_pushlightuserdata(L, b);
    return 1;
}

int l_destroy_body(void * L) {
    int n = lua_gettop(L);
    struct body_type *b = lua_touserdata(L, 1);
    
    if (b->type_num == 2) {
    } else if (b->type_num == 3) {
	//dGeomHeightfieldDataDestroy(b->heightfield_data_id);
    }
    
    dBodyDestroy(b->body);
    dGeomDestroy(b->geom);
//    free(b->body);
//    free(b->geom);
    free(b);
    
    return 0;
}

int l_body_set_mass(void * L) {
    struct body_type *b = lua_touserdata(L, 1);
    
    b->mass.mass = lua_tonumber(L, 2);
    dBodySetMass(b->body, &b->mass);
    return 0;
}

int l_body_get_mass(void * L) {
    struct body_type *b = lua_touserdata(L, 1);
    
    dBodyGetMass(b->body, &b->mass);
    lua_pushnumber(L, (float)b->mass.mass);
    return 1;
}

int l_body_get_position(void * L) {
    struct body_type *b = lua_touserdata(L, 1);
    
    dReal * retval = dBodyGetPosition(b->body);
    lua_pushnumber(L, retval[0]);
    lua_pushnumber(L, retval[1]);    
    lua_pushnumber(L, retval[2]);    
    return 3;
}


int l_body_set_position(void * L) {
    struct body_type *b = lua_touserdata(L, 1);
    
    dBodySetPosition(b->body, lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    return 0;
}


int l_body_set_rotation(void * L) {
    struct body_type *b = lua_touserdata(L, 1);
    
//    dBodySetPosition(b->body, lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
//    dMatrix3 * rotation = dBodyGetRotation(b->body);
//    rotation[
    MATRIX_f m;
    QUAT q;
    dQuaternion q2;
//    get_rotation_matrix_f(&m, lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    get_rotation_matrix_f(&m, lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    matrix_to_quat(&m, &q);
    q2[0] = q.w;
    q2[1] = q.x;
    q2[2] = q.y;
    q2[3] = q.z;
    dBodySetQuaternion(b->body, q2);
    /*printf("Rotation: r0: %f, r1: %f, r2: %f \n", m.v[0][0], m.v[0][1], m.v[0][2]);
    printf("Rotation: r1: %f, r1: %f, r2: %f \n", m.v[1][0], m.v[1][1], m.v[1][2]);    
    printf("Rotation: r2: %f, r1: %f, r2: %f \n", m.v[2][0], m.v[2][1], m.v[2][2]);*/
    
//    dBodySetRotation(b->body, m);
    
    return 0;
}

// get_rotation_quaternion
int l_body_get_rotation(void * L) {
    struct body_type *b = lua_touserdata(L, 1);
    
    dReal * retval = dBodyGetQuaternion(b->body);
//    dMatrix3 * rotation = dBodyGetRotation(b->body);
/*    printf("Rotation: r0: %f, r1: %f, r2: %f \n", rotation[0], rotation[1], rotation[2]);
    printf("Rotation: r3: %f, r4: %f, r5: %f \n", rotation[3], rotation[4], rotation[5]);
    printf("Rotation: r6: %f, r7: %f, r8: %f \n", rotation[6], rotation[7], rotation[8]);*/
    //printf("Rotation: w: %f, x: %f, y: %f, z: %f\n", retval[0], retval[1], retval[2],  retval[3]);
    
    lua_pushnumber(L, retval[1]);
    lua_pushnumber(L, retval[0]);    
    lua_pushnumber(L, retval[2]);    
    lua_pushnumber(L, retval[3]);    
    return 4;
}

// old callback
void oldnearCallback(struct worldspace_type *ws, dGeomID o1, dGeomID o2) {
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    
    dContact contact;
    contact.surface.mode = dContactBounce | dContactSoftCFM;
    
    contact.surface.mu = dInfinity;
    
    contact.surface.bounce = 0.9;
    
    contact.surface.bounce_vel = 0.1;
    
    contact.surface.soft_cfm = .001;
    int numc = 0;
    if (numc = dCollide(o1,o2,1,&contact.geom, sizeof(dContact))) {
	dJointID c = dJointCreateContact(ws->world, ws->contactgroup, &contact);
	dJointAttach(c, b1, b2);
    }
}

// new callback
void nearCallback(struct worldspace_type *ws, dGeomID o1, dGeomID o2) {
    int i;
    int max_contacts = 3;
    
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    if (b1 && b2 && dAreConnectedExcluding (b1,b2,dJointTypeContact)) return;
    
    dContact contact[max_contacts];
    for (i = 0; i < max_contacts; i++) {
	contact[i].surface.mode = dContactBounce | dContactSoftCFM;
	contact[i].surface.mu = dInfinity;
	contact[i].surface.mu2 = 0;
	contact[i].surface.bounce = 0.09;
	contact[i].surface.bounce_vel = 0.01;
	contact[i].surface.soft_cfm = 0.001;
    }
    int numc = 0;
    if (numc = dCollide(o1,o2, max_contacts, &contact[0].geom, sizeof(dContact))) {
	dMatrix3 RI;
	dRSetIdentity (RI);
	const dReal ss[3] = {0.02, 0.02, 0.02};
	for(i = 0; i<numc; i++) {
	    dJointID c = dJointCreateContact(ws->world, ws->contactgroup, contact+i);
	    dJointAttach(c, b1, b2);
	}
    }
}
int l_simulate(void * L) {
    int n = lua_gettop(L);
    struct worldspace_type *ws = lua_touserdata(L, 1);
    dSpaceCollide(ws->space, ws, &nearCallback);
    if (n == 2) {
	dWorldQuickStep(ws->world, lua_tonumber(L, 2));
	//dWorldStep(ws->world, lua_tonumber(L, 2));
    } else {
	dWorldQuickStep(ws->world, timer() - ws->lasttime);
//	printf("%f \n", timer() - ws->lasttime);
	ws->lasttime = timer();
    }
    
    dJointGroupEmpty(ws->contactgroup);

    return 0;
}

int l_body_set_linear_velocity(void * L) {
    int n = lua_gettop(L);
    struct body_type *b = lua_touserdata(L, 1);
    dReal fx = lua_tonumber(L, 2);
    dReal fy = lua_tonumber(L, 3);
    dReal fz = lua_tonumber(L, 4);
    
    dBodySetLinearVel(b->body, fx, fy, fz);
    
    return 0;
}

int l_body_set_angular_velocity(void * L) {
    int n = lua_gettop(L);
    struct body_type *b = lua_touserdata(L, 1);
    dReal fx = lua_tonumber(L, 2);
    dReal fy = lua_tonumber(L, 3);
    dReal fz = lua_tonumber(L, 4);
    
    dBodySetAngularVel(b->body, fx, fy, fz);
    
    return 0;
}


int l_body_get_linear_velocity(void * L) {
    int n = lua_gettop(L);
    struct body_type *b = lua_touserdata(L, 1);
    dReal * retval = NULL;
    
    retval = dBodyGetLinearVel(b->body);
    lua_pushnumber(L, retval[0]);
    lua_pushnumber(L, retval[1]);
    lua_pushnumber(L, retval[2]);
    
    return 3;
}

int l_body_get_angular_velocity(void * L) {
    int n = lua_gettop(L);
    struct body_type *b = lua_touserdata(L, 1);
    dReal * retval = NULL;
    
    retval = dBodyGetAngularVel(b->body);
    lua_pushnumber(L, retval[0]);
    lua_pushnumber(L, retval[1]);
    lua_pushnumber(L, retval[2]);
    
    return 3;
}


int l_body_set_force(void * L) {
    int n = lua_gettop(L);
    struct body_type *b = lua_touserdata(L, 1);
    dReal fx = lua_tonumber(L, 2);
    dReal fy = lua_tonumber(L, 3);
    dReal fz = lua_tonumber(L, 4);
    
    dBodySetForce(b->body, fx, fy, fz);
    
    return 0;
}

int l_body_add_force(void * L) {
    int n = lua_gettop(L);
    struct body_type *b = lua_touserdata(L, 1);
    dReal fx = lua_tonumber(L, 2);
    dReal fy = lua_tonumber(L, 3);
    dReal fz = lua_tonumber(L, 4);
    
    dBodyAddForce(b->body, fx, fy, fz);
    
    return 0;
}

int l_body_add_torque(void * L) {
    int n = lua_gettop(L);
    struct body_type *b = lua_touserdata(L, 1);
    dReal fx = lua_tonumber(L, 2);
    dReal fy = lua_tonumber(L, 3);
    dReal fz = lua_tonumber(L, 4);
    
    dBodyAddTorque(b->body, fx, fy, fz);
    
    return 0;
}


int l_body_add_relative_torque(void * L) {
    int n = lua_gettop(L);
    struct body_type *b = lua_touserdata(L, 1);
    dReal fx = lua_tonumber(L, 2);
    dReal fy = lua_tonumber(L, 3);
    dReal fz = lua_tonumber(L, 4);
    
    dBodyAddTorque(b->body, fx, fy, fz);
    
    return 0;
}

int l_body_add_relative_force(void * L) {
    int n = lua_gettop(L);
    struct body_type *b = lua_touserdata(L, 1);
    dReal fx = lua_tonumber(L, 2);
    dReal fy = lua_tonumber(L, 3);
    dReal fz = lua_tonumber(L, 4);
    
    dBodyAddRelForce(b->body, fx, fy, fz);
    
    return 0;
}

int l_body_is_connected(void * L) {
    struct body_type *b1 = lua_touserdata(L, 1);
    struct body_type *b2 = lua_touserdata(L, 2);
    
    lua_pushnumber(L, dAreConnected(b1->body, b2->body));
    
    return 1;
}

int l_get_plane_depth(void * L) {
    dReal retval = 0;
    dGeomID plane = lua_touserdata(L, 1);
    dReal x = lua_tonumber(L, 2);
    dReal y = lua_tonumber(L, 3);
    dReal z = lua_tonumber(L, 4);
    
    retval = dGeomPlanePointDepth(plane, x, y, z);
    
    lua_pushnumber(L, retval);
    return 1;
}

void raycast_callback(void * data, dGeomID o1, dGeomID o2) {
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    dContact contact;
    printf("Inside Raycast Callback.\n");
//    if (numc = dCollide(o1,o2, max_contacts, &contact[0].geom, sizeof(dContact))) {
    if (dCollide(o1, o2, 1, &contact.geom, sizeof(dContact))) {
	printf("Raycast hit something!\n");
    }
}

int l_raycast(void * L) {
    dGeomID ray = NULL;
    struct worldspace_type *ws = lua_touserdata(L, 1);
    dReal length = lua_tonumber(L, 2);
    dReal x1 = lua_tonumber(L, 3);
    dReal y1 = lua_tonumber(L, 4);
    dReal z1 = lua_tonumber(L, 5);
    dReal x2 = lua_tonumber(L, 6);
    dReal y2 = lua_tonumber(L, 7);
    dReal z2 = lua_tonumber(L, 8);

    printf("\nRaycasting...\n");
    ray = dCreateRay(ws->space, length); 
    dGeomRaySet(ray, x1, y1, z1, x2, y2, z2); 
    dSpaceCollide(ws->space, ws, &raycast_callback); 
    dGeomDestroy(ray);
    printf("Raycast done\n");
    return 0;
}
int resort_zorder(void) {
    int i = 0;
    int i2 = 0;
    int index = 0;
    int largest = 0;
    int largestindex = 0;
    int found = 0;
    int z = 0;

    //printf("resort z order\n\n");
//pthread_mutex_lock(&surface_mutex);
    
    for(i = 0; i < surfacecount; i += 1) {
	zorder[i] = -1;
    }
//    zorder[0] = 0;
    for(i = 0; i < surfacecount; i += 1) {
	largest = -1;
	// cycle through surfaces
	for(index = 0; index < surfacecount; index += 1) {
	    // find the largest
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
	//printf("%i index, z %i\n", largestindex, largest);
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
//--pthread_mutex_unlock(&surface_mutex);
}

int check_surface(int index) {
    if (index >= 0) {
	if (index < surfacecount) {
	    if (surfaces[index] != NULL) {
		//if (pthread_equal(pthread_self(), surfaces[index]->thread_id)) {
		    return -1;
		//}
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
    int parent = 0;
    float x_offset = 0;
    float y_offset = 0;
    
    for(index = 0; index < surfacecount; index += 1) {
	if (check_surface(index)) {	    
	//if (pthread_equal(pthread_self(), surfaces[index]->thread_id)) {
	    //printf("%p touching %i\n", pthread_self(), index);
	    if (check_surface(surfaces[index]->parent)) {
		parent = surfaces[index]->parent;
		if(!surfaces[index]->position_absolute) {
		    y_offset = surfaces[parent]->y_offset;
		    x_offset = surfaces[parent]->x_offset;
		}
	    }
	    screen_x = get_surface_screen_x(index) + x_offset;
	    screen_y = get_surface_screen_y(index) + y_offset;
	    screen_z = surfaces[index]->z; //get_surface_screen_z(index);
	    if (screen_x <= x) {
		if (surfaces[index]->bmap->w + screen_x >= x) {
		    if (screen_y <= y) {
			if (surfaces[index]->bmap->h + screen_y >= y) {
			    if (screen_z > max_z) {
				if (!surfaces[index]->always_on_top) {
				    if (!surfaces[index]->noevents) {
					if (surfaces[index]->visible) {
					    if (check_surface(surfaces[index]->parent)) {
						if (surfaces[surfaces[index]->parent]->visible) {
						    max_z = screen_z;
						    target = index;
						}
					    } else {
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
	    }
	//}
	}
    }
//    if (surfacecount > 9) {
//	printf("zindex %i, target %i target z %i\n", surfaces[9]->z, target, max_z);
//    }
    
    return target;
}

int l_get_thread_count(void * L) {
    lua_pushnumber(L, get_thread_count());
    return 1;
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
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->visible);
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
    //--pthread_mutex_unlock(&surface_mutex);
}

int l_set_surface_visible(void * L) {
    int index = lua_tonumber(L, 1);
    int visible = lua_tonumber(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->visible = visible;
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_get_surface_noevents(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->noevents);
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_noevents(void * L) {
    int index = lua_tonumber(L, 1);
    int transparent = lua_tonumber(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->noevents = transparent;
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}


int l_get_surface_can_have_focus(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->can_have_focus);
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_can_have_focus(void * L) {
    int index = lua_tonumber(L, 1);
    int can_have_focus = lua_tonumber(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->can_have_focus = can_have_focus;
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}


int l_get_surface_tabindex(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->tabindex);
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_tabindex(void * L) {
    int index = lua_tonumber(L, 1);
    int transparent = lua_tonumber(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->tabindex = transparent;
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}


int l_get_surface_transparent(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->transparent);
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_transparent(void * L) {
    int index = lua_tonumber(L, 1);
    int transparent = lua_tonumber(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->transparent = transparent;
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_get_surface_position_absolute(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->position_absolute);
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_position_absolute(void * L) {
    int index = lua_tonumber(L, 1);
    int position_absolute = lua_tonumber(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->position_absolute = position_absolute;
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}


int l_get_surface_y_offset(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->y_offset);
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_y_offset(void * L) {
    int index = lua_tonumber(L, 1);
    int y_offset = lua_tonumber(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->y_offset = y_offset;
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}



int l_get_surface_x_offset(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->x_offset);
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_x_offset(void * L) {
    int index = lua_tonumber(L, 1);
    int x_offset = lua_tonumber(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->x_offset = x_offset;
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_get_surface_alpha(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->alpha);
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_alpha(void * L) {
    int index = lua_tonumber(L, 1);
    int alpha = lua_tonumber(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->alpha = alpha;
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_get_surface_rotation(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->rotation);
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_rotation(void * L) {
    int index = lua_tonumber(L, 1);
    int rotation = lua_tonumber(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->rotation = rotation;
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_get_surface_backcolor(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->backcolor);
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_backcolor(void * L) {
    int index = lua_tonumber(L, 1);
    int backcolor = lua_tonumber(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->backcolor = backcolor;
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_get_surface_forecolor(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->forecolor);
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_forecolor(void * L) {
    int index = lua_tonumber(L, 1);
    int forecolor = lua_tonumber(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->forecolor = forecolor;
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_get_surface_transcolor(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->transcolor);
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_transcolor(void * L) {
    int index = lua_tonumber(L, 1);
    int transcolor = lua_tonumber(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->transcolor = transcolor;
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}


int l_get_surface_x(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->x);
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_x(void * L) {
    int index = lua_tonumber(L, 1);
    float x = lua_tonumber(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->x = x;
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_get_surface_y(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->y);
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_y(void * L) {
    int index = lua_tonumber(L, 1);
    float y = lua_tonumber(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->y = y;
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}


int l_get_surface_z(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->z);
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_z(void * L) {
    int index = lua_tonumber(L, 1);
    int z = lua_tonumber(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->z = z;
	//--pthread_mutex_unlock(&surface_mutex);
	resort_zorder();
	//--pthread_mutex_lock(&surface_mutex);
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}


int l_surface_get_text_length(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, text_length(surfaces[index]->font, lua_tostring(L, 2)));
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}


int l_surface_get_text_height(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, text_height(surfaces[index]->font));
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}



int l_get_surface_parent(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->parent);
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_parent(void * L) {
    int index = lua_tonumber(L, 1);
    int parent = lua_tonumber(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	if (check_surface(parent)) {
	    surfaces[index]->parent = parent;
	}
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_set_surface_font(void * L) {
    int index = lua_tonumber(L, 1);
    int font_index = lua_tonumber(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	if (check_surface(font_index)) {
	    surfaces[index]->font = grab_font_from_bitmap(surfaces[font_index]->bmap);
	}
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}


int l_get_surface_bitmap(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
        lua_pushlightuserdata(L, surfaces[index]->bmap);
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }    
}

int l_set_surface_bitmap(void * L) {
    int index = lua_tonumber(L, 1);
    BITMAP* bmp = lua_touserdata(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->bmap = bmp;
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}


int set_surface_dirty(int root) {
    int index = 0;
    if (check_surface(root)) {
	surfaces[root]->dirty = 1;
	//printf("setting %i, child of %i, dirty\n", root, surfaces[root]->parent);
        for (index = 0; index <= surfacecount; index += 1) {
	    if (check_surface(index)) {
		if (index != root) {
		    if (surfaces[index]->parent == root) {
			set_surface_dirty(index);
		    }
		}
	    }
	}
    }
    return 0;
}

int l_get_surface_dirty(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->dirty);
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_dirty(void * L) {
    int index = lua_tonumber(L, 1);
    int n = lua_gettop(L);
    int value = 1;
    if (n > 1) {
	value = lua_tonumber(L, 2);
    }
    if (check_surface(index)) {
	// you are now dirty
	//printf("%i is dirty\n", index);
	//surfaces[index]->dirty = 1;
	if (value) {
	    set_surface_dirty(index);
	} else {
	    surfaces[index]->dirty = 0;
	}
    }
    return 0;
    /*
    ////--pthread_mutex_lock(&surface_mutex);
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
    ////--pthread_mutex_unlock(&surface_mutex);
    

    return 0;
    */
}




int l_get_surface_screen_x(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, get_surface_screen_x(index));
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}


int l_get_surface_screen_y(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, get_surface_screen_y(index));
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}


int l_get_surface_width(void * L) {
    int index = lua_tonumber(L, 1);
    
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->bmap->w);
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_width(void * L) {
    int index = lua_tonumber(L, 1);
    int width = lua_tonumber(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	BITMAP * tmp = create_bitmap(width, surfaces[index]->bmap->h);
	blit(surfaces[index]->bmap, tmp, 0, 0, 0, 0, surfaces[index]->bmap->w, surfaces[index]->bmap->h);
	free(surfaces[index]->bmap);
	surfaces[index]->bmap = tmp;
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_get_surface_height(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->bmap->h);
	//--pthread_mutex_unlock(&surface_mutex);
	return 1;
    } else {
	//--pthread_mutex_unlock(&surface_mutex);
	return 0;
    }
}

int l_set_surface_height(void * L) {
    int index = lua_tonumber(L, 1);
    int height = lua_tonumber(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	BITMAP * tmp = create_bitmap(surfaces[index]->bmap->w, height);
	blit(surfaces[index]->bmap, tmp, 0, 0, 0, 0, surfaces[index]->bmap->w, surfaces[index]->bmap->h);
	free(surfaces[index]->bmap);
	surfaces[index]->bmap = tmp;
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_draw_gradient(void * L) {
    int n = lua_gettop(L);
    int index = lua_tonumber(L, 1);
    int x1 = lua_tonumber(L, 2);
    int y1 = lua_tonumber(L, 3);
    int x2 = lua_tonumber(L, 4);
    int y2 = lua_tonumber(L, 5);

    int color1 = lua_tonumber(L, 6);
    int color2 = lua_tonumber(L, 7);
    int border = 1;
    if (n == 8) {
	border = lua_tonumber(L, 8);
    }

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
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
//      draw_gradient(surfaces[index]->bmap, 0, 0, surfaces[index]->bmap->w, surfaces[index]->bmap->h, r1, g1, b1, r2, g2, b2);	
        draw_gradient(surfaces[index]->bmap, x1, y1, x2, y2, r1, g1, b1, r2, g2, b2, border);
    }
    //--pthread_mutex_unlock(&surface_mutex);
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


int l_set_thread_priority(void * L) {
    float priority = lua_tonumber(L, 1);
    
    lua_pushnumber(L, pthread_self());  /* push address */
    lua_pushnumber(L, priority);  /* push value */
    lua_settable(L, LUA_REGISTRYINDEX);    

    return 0;
}

int l_get_thread_priority(void * L) {
    
    lua_pushnumber(L, pthread_self());  /* push address */
    lua_gettable(L, LUA_REGISTRYINDEX);    
    lua_pushnumber(L, lua_tonumber(L, -1));

    return 1;
}


int l_set_surface_on_draw(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->on_draw = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_set_surface_on_got_focus(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->on_got_focus = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_set_surface_on_lost_focus(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->on_lost_focus = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}


int l_set_surface_always_on_top(void * L) {
    int index = lua_tonumber(L, 1);
    int value = lua_tonumber(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->always_on_top = value;
	if (value) {
	    surfaces[index]->z = 9999;
	    //--pthread_mutex_unlock(&surface_mutex);
	    resort_zorder();
	    //--pthread_mutex_lock(&surface_mutex);
	}
	
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}


int l_set_surface_on_mouse_down(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->on_mouse_down = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_set_surface_on_mouse_up(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->on_mouse_up = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}


int l_set_surface_on_mouse_click(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->on_mouse_click = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}




int l_set_surface_on_keypress(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->on_keypress = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}


int l_set_global_on_keypress(void * L) {
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(rootsurface)) {
	surfaces[rootsurface]->on_keypress = luaL_ref(L, LUA_REGISTRYINDEX);
	surfaces[rootsurface]->L = L;
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}


int l_set_surface_lua(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	surfaces[index]->L = L;
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;
}

int l_surface_clear(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	clear_to_color(surfaces[index]->bmap, surfaces[index]->backcolor);
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;    
}

int l_surface_blit(void * L) {
    int src = lua_tonumber(L, 1);
    int dest = lua_tonumber(L, 2);
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;
    int width = 0;
    int height = 0;

    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(src)) {
	if (check_surface(dest)) {
	    if (lua_gettop(L) > 2) {
		x1 = lua_tonumber(L, 3);
		y1 = lua_tonumber(L, 4);	
		x2 = lua_tonumber(L, 5);	
		y2 = lua_tonumber(L, 6);	
		width = lua_tonumber(L, 7);	
    		height = lua_tonumber(L, 8);	
	    } else {
		x1 = 0;
		y1 = 0;
		x2 = 0;
		y2 = 0;
		width = surfaces[src]->bmap->w;
		height = surfaces[src]->bmap->h;
	    }	
	    blit(surfaces[src]->bmap, surfaces[dest]->bmap, x1, y1, x2, y2, width, height);
	}
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;    
}

int l_surface_stretch_blit(void * L) {
    int src = lua_tonumber(L, 1);
    int dest = lua_tonumber(L, 2);
    int source_x = 0;
    int source_y = 0;
    int source_width = 0;
    int source_height = 0;
    int dest_x = 0;
    int dest_y = 0;
    int dest_width = 0;
    int dest_height = 0;


    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(src)) {
	if (check_surface(dest)) {
	    if (lua_gettop(L) > 2) {
		source_x = lua_tonumber(L, 3);
		source_y = lua_tonumber(L, 4);	
		source_width = lua_tonumber(L, 5);	
		source_height = lua_tonumber(L, 6);	
		dest_x = lua_tonumber(L, 7);
		dest_y = lua_tonumber(L, 8);	
		dest_width = lua_tonumber(L, 9);	
		dest_height = lua_tonumber(L, 10);	

	    } else {
		source_x = 0;
		source_y = 0;
		source_width = surfaces[src]->bmap->w;	
		source_height = surfaces[src]->bmap->h;		
		dest_x = 0;
		dest_y = 0;
		dest_width = surfaces[dest]->bmap->w;	
		dest_height = surfaces[dest]->bmap->h;	

	    }	
	    stretch_blit(surfaces[src]->bmap, surfaces[dest]->bmap, source_x, source_y, source_width, source_height, dest_x, dest_y, dest_width, dest_height);
	}
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;    
}


int l_surface_transparent_blit(void * L) {
    int src = lua_tonumber(L, 1);
    int dest = lua_tonumber(L, 2);
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;
    int width = 0;
    int height = 0;

    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(src)) {
	if (check_surface(dest)) {
	    if (lua_gettop(L) > 2) {
		x1 = lua_tonumber(L, 3);
		y1 = lua_tonumber(L, 4);	
		/*x2 = lua_tonumber(L, 5);	
		y2 = lua_tonumber(L, 6);	
		width = lua_tonumber(L, 7);	
    		height = lua_tonumber(L, 8);	*/
	    } else {
		x1 = 0;
		y1 = 0;
		x2 = 0;
		y2 = 0;
		width = surfaces[src]->bmap->w;
		height = surfaces[src]->bmap->h;
	    }	
	    draw_sprite(surfaces[dest]->bmap, surfaces[src]->bmap, x1, y1);
	}
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;    
}


int l_surface_print(void * L) {
    int index = lua_tonumber(L, 1);
    int x = lua_tonumber(L, 2);    
    int y = lua_tonumber(L, 3);    
    char *text = lua_tostring(L, 4);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	textprintf_ex(surfaces[index]->bmap, surfaces[index]->font, x, y, surfaces[index]->forecolor, surfaces[index]->backcolor, "%s", text);
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;        
}

int l_surface_print_wordwrap(void * L) {
    int bw = 0;
    int bh = 0;
    int index;
    char *text;
    int start; 

    int n = lua_gettop(L);
    int x = lua_tonumber(L, 1);
    int y = lua_tonumber(L, 2);
    if (n == 7) {
	bw = lua_tonumber(L, 3);
	bh = lua_tonumber(L, 4);
    
	index = lua_tonumber(L, 5);
	text = lua_tostring(L, 6);
	start = lua_tonumber(L, 7);    	
	
    } else {
	index = lua_tonumber(L, 3);
	text = lua_tostring(L, 4);
	start = lua_tonumber(L, 5);    	
    
	if (check_surface(index)) {
	    bw = surfaces[index]->bmap->w;
	    bh = surfaces[index]->bmap->h;
	}
    }
    int th = text_height(surfaces[index]->font);
    int len = strlen(text);
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
    
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	
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
	    tw = text_length(surfaces[index]->font, buffer);
	    if (tw > bw) {
		for (w = 0; w < word_count; w += 1) {
		    b2[0] = buffer[w];
		    b2[1] = 0;
		    tw = text_length(surfaces[index]->font, b2);
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
		    textprintf_ex(surfaces[index]->bmap, surfaces[index]->font, x + lw, y, surfaces[index]->forecolor, -1, "%s", b2);
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
		textprintf_ex(surfaces[index]->bmap, surfaces[index]->font, x + lw, y, surfaces[index]->forecolor, -1, "%s", buffer);
		lw += tw;
	    }
	    
	    if (y > bh) {
		break;
	    }	    
	}
    
	//textprintf_ex(surfaces[index]->bmap, font, x, y, surfaces[index]->forecolor, -1, "%s", text);
    }
    lua_pushnumber(L, nextline);
    //--pthread_mutex_unlock(&surface_mutex);
    return 1;
}


int l_wordwrap(void * L) {
    int index = lua_tonumber(L, 1);
    int x = 0;
    int y = 0;
    char *text = lua_tostring(L, 2);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	textprintf_ex(surfaces[index]->bmap, font, x, y, surfaces[index]->forecolor, -1, "%s", text);
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;            
}

int l_surface_has_focus(void * L) {
    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	lua_pushnumber(L, surfaces[index]->has_focus);
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 1;            

}

int l_next_focus(void * L) {
    int parent = 0;
    int index = 0;
    int i = 0;
    int found = 0;
    int tabindex = -1;
    ////--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	if (debug_mode) { printf("\n"); }
	if (debug_mode) { printf("Next Focus: Check Surface %i \n", focus); }
	if (check_surface(focus)) {
	    if (debug_mode) { printf("%i is valid\n", focus); }
	    surfaces[focus]->has_focus = 0;
	    if (debug_mode) { printf("Removed focus from %i \n", focus); }
	    if (debug_mode) { printf("Calling on_lost_focus for %i \n", focus); }
	    call_on_lost_focus(focus);
	    if (debug_mode) { printf("Returned from on_lost_focus \n"); }
	}
	parent = surfaces[focus]->parent;
	if (debug_mode) { printf("%i's parent is %i \n", focus, parent); }

	if (check_surface(parent)) {
	    if (debug_mode) { printf("Parent is valid\n"); }
	    for(i = 0; i <= surfacecount; i += 1) {
		index = i;// = zorder[surfacecount - i];
		if (debug_mode) { printf("Checking %i for next_focus candidate\n", index); }
		if (check_surface(index)) {
		    if (surfaces[index]->parent == parent) {
			if (debug_mode) { printf("Checking %i for can_have_focus = 1: %i \n", index, surfaces[index]->can_have_focus); }
			if (surfaces[index]->can_have_focus) {
			    if (debug_mode) { printf("Tab Index = %i, must be greater than %i \n", surfaces[index]->tabindex, surfaces[focus]->tabindex); }
			    if (surfaces[index]->tabindex > -1) {
				if (surfaces[index]->tabindex > surfaces[focus]->tabindex) {
				    if (debug_mode) { printf("Found a candidate, %i \n", index); }
				    focus = index;
				    found = 1;
			    	    break;
				}
			    }
			}
		    }
		}
	    }
	    if (found == 0) {
		//printf("not found\n");
		if (debug_mode) { printf("No candidate found, searching for surface with tab index >= 0\n"); }
		for(i = 0; i <= surfacecount; i += 1) {
		    index = i;
		    if (check_surface(index)) {
			if (surfaces[index]->parent == parent) {
			    if (debug_mode) { printf("Checking %i for can_have_focus = 1: %i \n", index, surfaces[focus]->can_have_focus); }
			    if (surfaces[index]->can_have_focus) {			
				if (surfaces[index]->tabindex >= 0) {
				    if (debug_mode) { printf("Found candidate, %i \n", index); }
				    focus = index;
				    found = 1;
				    break;
				}
			    }
			}
		    }
		}
		
	    }
	    if (found == 0) {
		index = focus;
	    }
	    //printf("found %i\n", index);
	    if (debug_mode) { printf("Found %i for new focus \n", index); }
	    surfaces[index]->has_focus = 1;
	    if (debug_mode) { printf("Calling on_got_focus for %i \n", index); }
	    call_on_got_focus(index);
	    if (debug_mode) {
		printf("DING DING DING Focus: %i\n", index);
	    }
	    lua_pushnumber(L, index);
	    return 1;
	}
    }
    ////--pthread_mutex_unlock(&surface_mutex);
    
    return 0;
}


int l_set_focus(void * L) {
    int index = lua_tonumber(L, 1);
    ////--pthread_mutex_lock(&surface_mutex);
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
    ////--pthread_mutex_unlock(&surface_mutex);
    return 0;            
}

int l_surface_get_pixel(void * L) {
    int index = lua_tonumber(L, 1);
    int x1 = lua_tonumber(L, 2);
    int y1 = lua_tonumber(L, 3);    
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	//printf("Get Pixel: %i\n", getpixel(surfaces[index]->bmap, x1, y1));
	lua_pushnumber(L, getpixel(surfaces[index]->bmap, x1, y1));
	return 1;
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;            
}

int l_get_rgb(void * L) {
    int c = lua_tonumber(L, 1);
    lua_pushnumber(L, getr(c));
    lua_pushnumber(L, getg(c));
    lua_pushnumber(L, getb(c));
    return 3;
}


int l_surface_line(void * L) {
    int index = lua_tonumber(L, 1);
    int x1 = lua_tonumber(L, 2);
    int y1 = lua_tonumber(L, 3);    
    int x2 = lua_tonumber(L, 4);    
    int y2 = lua_tonumber(L, 5);    
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	line(surfaces[index]->bmap, x1, y1, x2, y2, surfaces[index]->forecolor);
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;            
}


int l_surface_thick_line(void * L) {
    int index = lua_tonumber(L, 1);
    int x1 = lua_tonumber(L, 2);
    int y1 = lua_tonumber(L, 3);    
    int x2 = lua_tonumber(L, 4);    
    int y2 = lua_tonumber(L, 5);    
    float thickness = lua_tonumber(L, 6);    
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {

	float dx = x1 - x2;
	float dy = y1 - y2;
	float d = sqrtf(dx * dx + dy * dy);
	if (!d) {
	    return 0;
	}
 
	int v[4 * 2];
        /* left up */
        v[0] = x1 - thickness * dy / d;
	v[1] = y1 + thickness * dx / d;
        /* right up */
        v[2] = x1 + thickness * dy / d;
        v[3] = y1 - thickness * dx / d;
        /* right down */
        v[4] = x2 + thickness * dy / d;
        v[5] = y2 - thickness * dx / d;
        /* left down */
        v[6] = x2 - thickness * dy / d;
        v[7] = y2 + thickness * dx / d;
        polygon(surfaces[index]->bmap, 4, v, surfaces[index]->forecolor);
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;            
}

int l_surface_rect(void * L) {
    int index = lua_tonumber(L, 1);
    int x1 = lua_tonumber(L, 2);
    int y1 = lua_tonumber(L, 3);    
    int x2 = lua_tonumber(L, 4);    
    int y2 = lua_tonumber(L, 5);    
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	rect(surfaces[index]->bmap, x1, y1, x2, y2, surfaces[index]->forecolor);
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;            
}

int l_surface_rect_fill(void * L) {
    int index = lua_tonumber(L, 1);
    int x1 = lua_tonumber(L, 2);
    int y1 = lua_tonumber(L, 3);    
    int x2 = lua_tonumber(L, 4);    
    int y2 = lua_tonumber(L, 5);    
    //--pthread_mutex_lock(&surface_mutex);
    if (check_surface(index)) {
	rectfill(surfaces[index]->bmap, x1, y1, x2, y2, surfaces[index]->forecolor);
    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;            
}

int l_render_loop(void * L) {
    int i = 0;
    int n = lua_gettop(L);
    int index = 0;
    bail = 0;
    if (debug_mode) {
	printf("Entering Render Loop %p :\n\n\n\n", pthread_self());
    }
    if (n == 0) {
	if (debug_mode) {
	    printf("Render_loop() -- No arguments given.\n");
	}
	render_loop();
    } else {	
	if (debug_mode) {
	    printf("Render_loop() -- %i arguments given.\n", n);
	}
	int *surface_list = calloc(sizeof(int), n);
	for(i = 0; i < n; i += 1) {
	    surface_list[i] = lua_tonumber(L, i + 1);
	    if (debug_mode) {
		printf("Adding to the render loop: %i\n", surface_list[i]);
	    }
	}
	index = surface_list[0];
	if (debug_mode) {
	    printf("render_loop(%i)\n", index);
	}
	
	// This is one method
	/*
	create_event(index, call_on_draw, "on_draw");
	while (check_surface(index)) {
	    int retval = render_surfaces(index);
	    printf("render_surfaces(%i) = %i\n", index, retval);
	    if (!retval) {
		pthread_yield();
	    }
	
	    run_queue();
	}*/
	
	// This is another method
	while (check_surface(index)) {	    
	    process_events();
	    run_queue();
	    pthread_yield();
	    render_surfaces(index);
	}
	while (1 == 0) {
	    for(i = 0; i < n; i += 1) {
		index = surface_list[i];
	    
		// we have to re-check after process_events because the 
		// surface may not exist any more
		if (check_surface(index)) {
		    if (surfaces[index]->outside_thread_on_draw_request == 1) {
			if (surfaces[index]->dirty) {
			    call_on_draw(index);
			}
				    
			printf("finished outside draw for %i, dirty=%i\n", index, surfaces[index]->dirty); 
			if (check_surface(index)) {
			    if (surfaces[index]->dirty == 0) {
				// redraw all children
				set_surface_dirty(index);
				surfaces[index]->dirty = 0;			
    				render_surfaces(index);			
			    }
			    surfaces[index]->outside_thread_on_draw_request = 0;
			}
		    
		    }
		
		    if (rootsurface == index) {
			if (check_surface(index)) {
			
			    for(i = 0; i <= surfacecount; i += 1) {
				if (check_surface(i)) {
				    //printf("surfaces[%i]->parent = %i\n", i, surfaces[i]->parent);
				    if (surfaces[i]->parent == index) {
					//printf("super rendering %i\n", i);
					render_surfaces(i);
					//render_surface(index, i);
				    }
				}
			    }
			    render_desktop(index);			
			}
		    } else {
			if (check_surface(index)) {
			    if (!render_surfaces(index)) {
	    			pthread_yield();
			    }
			}
		    }
	
		}	
	    }
	} // while

	//--pthread_mutex_unlock(&surface_mutex);
    }
    
    // never returns anyway
    return 0;            
    
}

/*
int l_render(void * L) {
    render_once();
    return 0;
}
*/

int l_render(void * L) {
//    int index = lua_tonumber(L, 1);
    //--pthread_mutex_lock(&surface_mutex);
//    if (check_surface(index)) {
	//render_desktop(rootsurface);
	//if (!render_surfaces(index)) {
//	    printf("render yield\n");
//	    pthread_yield();
//	}
	render_once();
	
//    }
    //--pthread_mutex_unlock(&surface_mutex);
    return 0;            
}

int l_stop_render(void * L) {
    bail = 1;
    return 0;
}

int l_set_render_mode(void * L) {
    int new_render_mode = lua_tonumber(L, 1);
    
    render_mode = new_render_mode;
    return 0;
}

int l_get_render_mode(void * L) {
    lua_pushnumber(L, render_mode);
    return 1;
}

int l_initialize_video(void * L) {
    int n = lua_gettop(L);
    int x = 640;
    int y = 480;
    int video_mode = GFX_AUTODETECT_WINDOWED;
    if (debug_mode) {
	printf("Graphics modes supported:\n");
	printf("GFX_AUTODETECT = %i\n", GFX_AUTODETECT);
	printf("GFX_AUTODETECT_FULLSCREEN = %i\n", GFX_AUTODETECT_FULLSCREEN);
	printf("GFX_AUTODETECT_WINDOWED = %i (default)\n", GFX_AUTODETECT_WINDOWED);
	printf("\n");
    }
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
    int new_rootsurface = lua_tonumber(L, 1);
    if(check_surface(new_rootsurface)) {
	rootsurface = new_rootsurface;
	surfaces[rootsurface]->parent = -1;
	if (debug_mode) {
	    printf("new root surface = %i\n", rootsurface);
	}
    }
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
	//--pthread_mutex_lock(&surface_mutex);
	if (check_surface(index)) {
	    BITMAP * tmp = load_bmp(file, NULL);
	    free(surfaces[index]->bmap);
	    surfaces[index]->bmap = tmp;	    
	}
	//--pthread_mutex_unlock(&surface_mutex);
    }
    return 0;                
}

int l_surface_save_bmp(void * L) {
    int index = lua_tonumber(L, 1);
    char *file = lua_tostring(L, 2);
    if (check_surface(index)) {
	save_bmp(file, surfaces[index]->bmap, palette);
    }
    return 0;                
}

int l_surface_load_jpg(void * L) {
    int index = lua_tonumber(L, 1);
    char *file = lua_tostring(L, 2);
    if (findfile(&file)) {
	//--pthread_mutex_lock(&surface_mutex);
	if (check_surface(index)) {
	    BITMAP * tmp = load_jpg(file, NULL);
	    free(surfaces[index]->bmap);
	    surfaces[index]->bmap = tmp;	    
	}
	//--pthread_mutex_unlock(&surface_mutex);
    }
    return 0;                
}

int l_surface_save_jpg(void * L) {
    int n = lua_gettop(L);
    int index = lua_tonumber(L, 1);
    if (n == 1) {
	if (check_surface(index)) {
	    int size = (surfaces[index]->bmap->w * surfaces[index]->bmap->h * 3) + 1024;
	    int *buffer = malloc(size);
	    printf("Saving JPG to memory buffer of size: %i \n", size);
	    save_memory_jpg(buffer, &size, surfaces[index]->bmap, palette);
	    lua_pushlstring(L, buffer, size);
	    printf("JPG Saved, size: %i\n", size);
	    return 1;
	}
    } else if (n == 2) {
	char *file = lua_tostring(L, 2);
	if (check_surface(index)) {
	    save_jpg(file, surfaces[index]->bmap, palette);
	}
	return 0;
    }
    
}



int l_surface_load_gif(void * L) {
    int index = lua_tonumber(L, 1);
    char *file = lua_tostring(L, 2);
    if (findfile(&file)) {
	//--pthread_mutex_lock(&surface_mutex);
	if (check_surface(index)) {
	    BITMAP * tmp = load_bitmap(file, NULL);
	    free(surfaces[index]->bmap);
	    surfaces[index]->bmap = tmp;	    
	}
	//--pthread_mutex_unlock(&surface_mutex);
    }
    return 0;                
}

int l_surface_load_animated_gif(void * L) {
    int index = lua_tonumber(L, 1);
    char *file = lua_tostring(L, 2);
    int i = 0;
    if (findfile(&file)) {
	if (check_surface(index)) {
	    if (surfaces[index]->frame_count) {
		for(i = 0; i < surfaces[index]->frame_count; i++) {
		    destroy_bitmap(surfaces[index]->frames[i]);
		}
		free(surfaces[index]->frames);
		free(surfaces[index]->durations);
		surfaces[index]->frame_count = 0;
	    }
	    surfaces[index]->frame_count = algif_load_animation(file, &surfaces[index]->frames, &surfaces[index]->durations);
	    if (debug_mode) {
		printf("Loaded animated gif with %i frames onto surface %i.\n", surfaces[index]->frame_count, index);
	    }
	    
	    // resize the bmap to match the dimensions of the animated gif
	    BITMAP * tmp = create_bitmap(surfaces[index]->frames[1]->w, surfaces[index]->frames[1]->h);
	    blit(surfaces[index]->bmap, tmp, 0, 0, 0, 0, surfaces[index]->bmap->w, surfaces[index]->bmap->h);
	    free(surfaces[index]->bmap);
	    surfaces[index]->bmap = tmp;

	    
	    surfaces[index]->frame_timer = timer();
	    surfaces[index]->current_frame = 1;
	}
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
    camera->r_x = 0;
    camera->r_y = 0;
    camera->r_z = 0;
    camera->fov = 60;
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

int l_set_camera_r_x(void * L) {
    struct camera_type *camera = lua_touserdata(L, 1);
    float r_x = lua_tonumber(L, 2);
    camera->r_x = r_x;
    return 0;
}

int l_get_camera_r_x(void * L) {
    struct camera_type *camera = lua_touserdata(L, 1);
    lua_pushnumber(L, camera->r_x);
    return 1;
}


int l_set_camera_r_y(void * L) {
    struct camera_type *camera = lua_touserdata(L, 1);
    float r_y = lua_tonumber(L, 2);
    camera->r_y = r_y;
    return 0;
}

int l_get_camera_r_y(void * L) {
    struct camera_type *camera = lua_touserdata(L, 1);
    lua_pushnumber(L, camera->r_y);
    return 1;
}

int l_set_camera_r_z(void * L) {
    struct camera_type *camera = lua_touserdata(L, 1);
    float r_z = lua_tonumber(L, 2);
    camera->r_z = r_z;
    return 0;
}

int l_get_camera_r_z(void * L) {
    struct camera_type *camera = lua_touserdata(L, 1);
    lua_pushnumber(L, camera->r_z);
    return 1;
}

int l_set_camera_fov(void * L) {
    struct camera_type *camera = lua_touserdata(L, 1);
    float cfov = lua_tonumber(L, 2);
    camera->fov = cfov;
    return 0;
}

int l_get_camera_fov(void * L) {
    struct camera_type *camera = lua_touserdata(L, 1);
    lua_pushnumber(L, camera->fov);
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
    roll = 0;

//    heading = ((camera->pitch / -1) + 90) * Deg2Rad;
//    pitch = (camera->heading / -1) * Deg2Rad; 	// heading/pitch swapped due to 
						// rolling the camera to match 
						// ODE's cordinate system

    pitch = ((camera->pitch / -1) + 0) * Deg2Rad;
    heading = ((camera->heading / -1)+0) * Deg2Rad; 

    
//    xfront = camera->r_x; //sin(heading) * cos(pitch);
//    yfront = camera->r_y; //sin(pitch);
//    zfront = camera->r_z; //cos(heading) * cos(pitch);

    xfront = sin(heading) * cos(pitch);
    zfront = sin(pitch);
    yfront = cos(heading) * cos(pitch);
    
    //get_vector_rotation_matrix_f(&camera->roller, xfront, yfront, zfront, roll);
    //apply_matrix_f(&camera->roller, 0, 0, 1, &xup, &yup, &zup);
    
    get_camera_matrix_f(&camera->camera,
			xpos, ypos, zpos,
			xfront, yfront, zfront,
			//xup, yup, zup,
			0, 0, 1,
			camera->fov,
			aspect);
//    printf("Cam POS, X: %f, Y: %f, Z: %f, Front, X: %f, Y: %f, Z: %f, Up, X: %f, Y: %f, Z: %f\n", xpos, ypos, zpos, xfront, yfront, zfront, xup, yup, zup);
//    printf("Pitch: %f Heading: %f\n", pitch, heading);
			
    clear_zbuffer(camera->zbuf, 0.);
    return 0;
}

int l_normalize_vector(void * L) {
    int n = lua_gettop(L);
    float x = 0;
    float y = 0;
    float z = 0;
    if (n == 3) {
	x = lua_tonumber(L, 1);
	y = lua_tonumber(L, 2);
	z = lua_tonumber(L, 3);
	normalize_vector_f(&x, &y, &z);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pushnumber(L, z);
	return 3;
    } else {
	return 0;
    }
}

int l_create_model(void * L) {
    struct model_type * model;
    model = calloc(1, sizeof(struct model_type ));
    model->facecount = 0;
    model->rx = 0;
    model->ry = 0;
    model->rz = 0;
    model->ox = 0;
    model->oy = 0;
    model->oz = 0;
        
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

int l_set_model_offset(void * L) {
    struct model_type * model = lua_touserdata(L, 1);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);
    float z = lua_tonumber(L, 4);
    
    model->ox = x;
    model->oy = y;
    model->oz = z;
    
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
	    vout[e].x = model->faces[f]->v[e].x + model->ox;
	    vout[e].y = model->faces[f]->v[e].y + model->oy;
	    vout[e].z = model->faces[f]->v[e].z + model->oz;
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

int l_render_model_gouraud(void * L) {
    struct model_type * model = lua_touserdata(L, 1);
    struct trans_type * matrix = lua_touserdata(L, 2);
    struct camera_type * camera = lua_touserdata(L, 3);
    BITMAP * target_bmp = lua_touserdata(L, 4);    

    int f = 0;
    int e = 0;
    int facecount = model->facecount;
    
    V3D_f *v[4];
    V3D_f vout[4];
    
    for (f = 0; f < facecount; f += 1) {
	for (e = 0; e < 3; e += 1) {
	    vout[e].x = model->faces[f]->v[e].x + model->ox;
	    vout[e].y = model->faces[f]->v[e].y + model->oy;
	    vout[e].z = model->faces[f]->v[e].z + model->oz;

//	    vout[e].x = model->faces[f]->v[e].x;
//	    vout[e].y = model->faces[f]->v[e].y;
//	    vout[e].z = model->faces[f]->v[e].z;
	    vout[e].u = model->faces[f]->v[e].u;
	    vout[e].v = model->faces[f]->v[e].v;
	    vout[e].c = model->faces[f]->v[e].c;
	    
	    apply_quat(&matrix->rotation, vout[e].x, vout[e].y, vout[e].z, 
		&vout[e].x, &vout[e].y, &vout[e].z);	    
	    apply_matrix_f(&matrix->matrix, vout[e].x, vout[e].y, vout[e].z, 
		&vout[e].x, &vout[e].y, &vout[e].z);

	}
	draw_polygon_gouraud(camera, &vout[0], &vout[1], &vout[2], target_bmp);
    }
    return 0;
}

int l_render_model_wireframe(void * L) {
    struct model_type * model = lua_touserdata(L, 1);
    struct trans_type * matrix = lua_touserdata(L, 2);
    struct camera_type * camera = lua_touserdata(L, 3);
    BITMAP * target_bmp = lua_touserdata(L, 4);    

    int f = 0;
    int e = 0;
    int facecount = model->facecount;
    
    V3D_f *v[4];
    V3D_f vout[4];
    
    for (f = 0; f < facecount; f += 1) {
	for (e = 0; e < 3; e += 1) {
	    vout[e].x = model->faces[f]->v[e].x + model->ox;
	    vout[e].y = model->faces[f]->v[e].y + model->oy;
	    vout[e].z = model->faces[f]->v[e].z + model->oz;
	    vout[e].u = model->faces[f]->v[e].u;
	    vout[e].v = model->faces[f]->v[e].v;
	    vout[e].c = model->faces[f]->v[e].c;
	    
	    apply_quat(&matrix->rotation, vout[e].x, vout[e].y, vout[e].z, 
		&vout[e].x, &vout[e].y, &vout[e].z);	    
	    apply_matrix_f(&matrix->matrix, vout[e].x, vout[e].y, vout[e].z, 
		&vout[e].x, &vout[e].y, &vout[e].z);
	    
	}
	draw_polygon_wireframe(camera, &vout[0], &vout[1], &vout[2], target_bmp);
    }
    return 0;
}

int l_render_perspective(void * L) {
// This would be nice to have.
//    persp_project_f(x, y, z, perspective_x, perspective_y);
}

int l_plot_model(void * L) {
    struct model_type * model = lua_touserdata(L, 1);
    struct trans_type * matrix = lua_touserdata(L, 2);
    struct camera_type * camera = lua_touserdata(L, 3);
    BITMAP * target_bmp = lua_touserdata(L, 4);    

    int f = 0;
    int e = 0;
    int facecount = model->facecount;
    
    V3D_f *v[4];
    V3D_f vout[4];
    
    for (f = 0; f < facecount; f += 1) {
	for (e = 0; e < 3; e += 1) {
	    vout[e].x = model->faces[f]->v[e].x + model->ox;
	    vout[e].y = model->faces[f]->v[e].y + model->oy;
	    vout[e].z = model->faces[f]->v[e].z + model->oz;
	    vout[e].u = model->faces[f]->v[e].u;
	    vout[e].v = model->faces[f]->v[e].v;
	    vout[e].c = model->faces[f]->v[e].c;
	    
	    apply_quat(&matrix->rotation, vout[e].x, vout[e].y, vout[e].z, 
		&vout[e].x, &vout[e].y, &vout[e].z);	    
	    apply_matrix_f(&matrix->matrix, vout[e].x, vout[e].y, vout[e].z, 
		&vout[e].x, &vout[e].y, &vout[e].z);
	    
	}
	plot_polygon(camera, &vout[0], &vout[1], &vout[2], target_bmp);
    }
    return 0;
}

int l_plot_point(void * L) {
    float x = lua_tonumber(L, 1);
    float y = lua_tonumber(L, 2);
    float z = lua_tonumber(L, 3);
    
    float dx = 0;
    float dy = 0;
    float dz = 0;
    float d = 0;
    
    struct trans_type * matrix = lua_touserdata(L, 4);
    struct camera_type * camera = lua_touserdata(L, 5);
    MATRIX_f *cam = &camera->camera;
    //BITMAP * target_bmp = lua_touserdata(L, 6);

    V3D_f *v[4];
    V3D_f vout[4];
    
    apply_quat(&matrix->rotation, x, y, z, 
        &x, &y, &z);	    
    apply_matrix_f(&matrix->matrix, x, y, z, 
        &x, &y, &z);

    apply_matrix_f(cam, x, y, z, &x, &y, &z);
    
    dx = camera->x - x;
    dy = camera->y - y;
    dz = camera->z - z;
    d = ((dx * dx) + (dy*dy) + (dz*dz))*.5;
    persp_project_f(x, y, z, &x, &y);
    
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    lua_pushnumber(L, d);
    return 3;
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

int l_set_matrix_rotation(void * L) {
    struct trans_type * matrix = lua_touserdata(L, 1);
    
    //get_rotation_quat(&matrix->rotation, lua_tonumber(L, 2), lua_tonumber(L, 3),lua_tonumber(L, 4) );    
    matrix->rotation.w = lua_tonumber(L, 2);
    matrix->rotation.x = lua_tonumber(L, 3);
    matrix->rotation.y = lua_tonumber(L, 4);
    matrix->rotation.z = lua_tonumber(L, 5);
    return 0;
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
	matrix->r_x, matrix->r_y, matrix->r_z,
//	0, 0, 0,
	matrix->x, matrix->y, matrix->z);
	
    //get_rotation_quat(&matrix->rotation, matrix->r_x, matrix->r_y, matrix->r_z);
    

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
    get_rotation_quat(&matrix->rotation, matrix->r_x, matrix->r_y, matrix->r_z);    
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
    get_rotation_quat(&matrix->rotation, matrix->r_x, matrix->r_y, matrix->r_z);    
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
    get_rotation_quat(&matrix->rotation, matrix->r_x, matrix->r_y, matrix->r_z);    
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


int l_unlock(void * L) {
    lua_unlock(L);
    return 0;
}

int l_set_window_title(void * L) {
    set_window_title(lua_tostring(L, 1));
    return 0;
}

void illution_lua_register_call(void * L) {
//    printf("Registering NEW FEATURE!\n");
//    lua_register(L, "new_feature", l_new_feature);

    //lua_register(L, "surface_load_bmp", l_surface_load_bmp);
    // ode
    lua_register(L, "set_window_title", l_set_window_title);
    lua_register(L, "unlock", l_unlock);    
    lua_register(L, "set_debug_mode", l_set_debug_mode);

    // libode
    lua_register(L, "create_worldspace", l_create_worldspace);
    lua_register(L, "destroy_worldspace", l_destroy_worldspace);
//    lua_register(L, "", );
    lua_register(L, "create_plane", l_create_plane);
    lua_register(L, "plane_set_geom", l_plane_set_geom);
    lua_register(L, "create_body", l_create_body);
    lua_register(L, "destroy_body", l_destroy_body);


    lua_register(L, "body_set_mass", l_body_set_mass);
    lua_register(L, "body_get_mass", l_body_get_mass);
    lua_register(L, "body_set_position", l_body_set_position);
    lua_register(L, "body_get_position", l_body_get_position);

    lua_register(L, "body_set_rotation", l_body_set_rotation);
    lua_register(L, "body_get_rotation", l_body_get_rotation);

    lua_register(L, "body_set_linear_velocity", l_body_set_linear_velocity);
    lua_register(L, "body_set_angular_velocity", l_body_set_angular_velocity);
    lua_register(L, "body_get_linear_velocity", l_body_get_linear_velocity);
    lua_register(L, "body_get_angular_velocity", l_body_get_angular_velocity);
    
    lua_register(L, "body_set_force", l_body_set_force);
    lua_register(L, "body_add_force", l_body_add_force);
    lua_register(L, "body_add_relative_force", l_body_add_relative_force);
    lua_register(L, "body_add_torque", l_body_add_torque);
    lua_register(L, "body_add_relative_torque", l_body_add_relative_torque);
    lua_register(L, "body_is_connected", l_body_is_connected);
    lua_register(L, "get_plane_depth", l_get_plane_depth);
    lua_register(L, "raycast", l_raycast);
    
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
    lua_register(L, "set_camera_r_x", l_set_camera_r_x);
    lua_register(L, "get_camera_r_x", l_get_camera_r_x);
    lua_register(L, "set_camera_r_y", l_set_camera_r_y);
    lua_register(L, "get_camera_r_y", l_get_camera_r_y);
    lua_register(L, "set_camera_r_z", l_set_camera_r_z);
    lua_register(L, "get_camera_r_z", l_get_camera_r_z);
    lua_register(L, "set_camera_fov", l_set_camera_fov);
    lua_register(L, "get_camera_fov", l_get_camera_fov);

    lua_register(L, "set_camera_heading", l_set_camera_heading);
    lua_register(L, "get_camera_heading", l_get_camera_heading);
    lua_register(L, "set_camera_pitch", l_set_camera_pitch);
    lua_register(L, "get_camera_pitch", l_get_camera_pitch);
    lua_register(L, "initialize_camera", l_initialize_camera);
    lua_register(L, "update_camera", l_update_camera);
    lua_register(L, "normalize_vector", l_normalize_vector);
    lua_register(L, "create_model", l_create_model);
    lua_register(L, "model_add_face", l_model_add_face);
    lua_register(L, "get_model_face_count", l_get_model_face_count);
    lua_register(L, "set_model_offset", l_set_model_offset);
    lua_register(L, "set_model_face_data", l_set_model_face_data);
    lua_register(L, "get_model_face_data", l_get_model_face_data);
    lua_register(L, "load_3ds", l_load_3ds);
    lua_register(L, "render_model", l_render_model);
    lua_register(L, "render_model_gouraud", l_render_model_gouraud);    
    lua_register(L, "plot_model", l_plot_model);
    lua_register(L, "plot_point", l_plot_point);
    lua_register(L, "render_model_wireframe", l_render_model_wireframe);    
    lua_register(L, "create_matrix", l_create_matrix);
    lua_register(L, "update_matrix", l_update_matrix);
    lua_register(L, "set_matrix_rotation", l_set_matrix_rotation);
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
    lua_register(L, "next_focus", l_next_focus);    
    lua_register(L, "surface_has_focus", l_surface_has_focus);
    lua_register(L, "surface_print_wordwrap", l_surface_print_wordwrap);
    lua_register(L, "surface_print", l_surface_print);
    lua_register(L, "surface_clear", l_surface_clear);
    lua_register(L, "surface_blit", l_surface_blit);
    lua_register(L, "surface_stretch_blit", l_surface_stretch_blit);    
    lua_register(L, "surface_transparent_blit", l_surface_transparent_blit);    
    lua_register(L, "surface_get_pixel", l_surface_get_pixel);
    lua_register(L, "get_rgb", l_get_rgb);    
    lua_register(L, "surface_thick_line", l_surface_thick_line);
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

    lua_register(L, "get_surface_position_absolute", l_get_surface_position_absolute);    
    lua_register(L, "set_surface_position_absolute", l_set_surface_position_absolute);        


    lua_register(L, "set_surface_x_offset", l_set_surface_x_offset);    
    lua_register(L, "set_surface_y_offset", l_set_surface_y_offset);        

    lua_register(L, "get_surface_x_offset", l_get_surface_x_offset);    
    lua_register(L, "get_surface_y_offset", l_get_surface_y_offset);        

    lua_register(L, "get_surface_z", l_get_surface_z);        
    lua_register(L, "set_surface_x", l_set_surface_x);    
    lua_register(L, "set_surface_y", l_set_surface_y);        
    lua_register(L, "set_surface_z", l_set_surface_z);        

    lua_register(L, "set_surface_tab_index", l_set_surface_tabindex);
    lua_register(L, "get_surface_tab_index", l_get_surface_tabindex);    

    lua_register(L, "set_surface_noevents", l_set_surface_noevents);
    lua_register(L, "get_surface_noevents", l_get_surface_noevents);
    lua_register(L, "set_surface_can_have_focus", l_set_surface_can_have_focus);
    lua_register(L, "get_surface_can_have_focus", l_get_surface_can_have_focus);


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
    lua_register(L, "get_surface_dirty", l_get_surface_dirty);    

    lua_register(L, "set_surface_on_draw", l_set_surface_on_draw);
    lua_register(L, "set_surface_on_got_focus", l_set_surface_on_got_focus);    
    lua_register(L, "set_surface_on_lost_focus", l_set_surface_on_lost_focus);        
    lua_register(L, "set_surface_on_keypress", l_set_surface_on_keypress);    
    lua_register(L, "set_global_on_keypress", l_set_global_on_keypress);    
    lua_register(L, "set_surface_on_mouse_down", l_set_surface_on_mouse_down);
    lua_register(L, "set_surface_on_mouse_up", l_set_surface_on_mouse_up);
    lua_register(L, "set_surface_on_mouse_click", l_set_surface_on_mouse_click);
    
    lua_register(L, "set_surface_bitmap", l_set_surface_bitmap);
    lua_register(L, "get_surface_bitmap", l_get_surface_bitmap);    
    
    lua_register(L, "set_root_surface", l_set_root_surface);
    lua_register(L, "get_root_surface", l_get_root_surface);

    lua_register(L, "gradient", l_draw_gradient);
    lua_register(L, "makecol", l_makecol);
    lua_register(L, "render_loop", l_render_loop);
    lua_register(L, "initialize_video", l_initialize_video);
    lua_register(L, "set_render_mode", l_set_render_mode);
    lua_register(L, "get_render_mode", l_get_render_mode);
    
    
    lua_register(L, "get_surface_by_xy", l_get_surface_by_xy);
    lua_register(L, "get_surface_screen_x", l_get_surface_screen_x);
    lua_register(L, "get_surface_screen_y", l_get_surface_screen_y);
    
    lua_register(L, "set_surface_always_on_top", l_set_surface_always_on_top);
    lua_register(L, "stop_render", l_stop_render);
    lua_register(L, "surface_load_bmp", l_surface_load_bmp);
    lua_register(L, "surface_save_bmp", l_surface_save_bmp);
    lua_register(L, "surface_load_jpg", l_surface_load_jpg);    
    lua_register(L, "surface_save_jpg", l_surface_save_jpg);    
    lua_register(L, "surface_load_gif", l_surface_load_gif);    
    lua_register(L, "surface_load_animated_gif", l_surface_load_animated_gif);
    
    lua_register(L, "surface_get_text_length", l_surface_get_text_length);
    lua_register(L, "surface_get_text_height", l_surface_get_text_height);
    
    lua_register(L, "get_thread_count", l_get_thread_count);
    lua_register(L, "get_thread_priority", l_get_thread_priority);
    lua_register(L, "set_thread_priority", l_set_thread_priority);
    

    lua_sethook(L, watchdog, LUA_MASKCOUNT, 1000);
    //lua_runscript_reuse(L, "surface.lua");
}

int _init_thread(char * mod_name) {
    mod_lua_register(mod_name, illution_lua_register_call);
    SetVar("illution", "illution_version", VERSION_STRING);
    SetVar("illution", "illution_arch", ARCH_STRING);
    
    if (debug_mode) {
	printf("illution %s %s loaded.\n", ARCH_STRING, VERSION_STRING);
    }
//    printf("illution Starting Up...\n");
//    printf("Press Enter for Command Prompt.\n");
//    interactive_console_thread();
//    render_loop();
    return 0;
}


/* This will load a gif animation into an array of BITMAP pointers. Each
 * bitmap will have the size of the complete animation. The bitmaps will all
 * use Allegro's current color depth. Returns the number of stored frames,
 * 0 on error. You are responsible for freeing all the bitmaps as well as
 * the arrays yourself.
 *
 * E.g.
 * BITMAP **frames = NULL;
 * int **durations = NULL;
 * int n = algif_load_animation ("my.gif", &frames, &durations);
 * if (n)
 * {
 *     ...
 *     for (i = 0; i < n; i++)
 *         destroy_bitmap (frames[i]);
 *     free (frames);
 *     free (durations);
 * }
 */
int
algif_load_animation (const char *filename, BITMAP ***frames, int **durations)
{
    GIF_ANIMATION *gif = algif_load_raw_animation (filename);
    int i, n;
    BITMAP *prev = NULL;

    if (!gif)
        return 0;

    n = gif->frames_count;
    *frames = calloc (n, sizeof **frames);
    if (durations)
        *durations = calloc (n, sizeof **durations);
    for (i = 0; i < n; i++)
    {
        BITMAP *b = (*frames)[i] = create_bitmap (gif->width, gif->height);
        if (prev)
            blit (prev, b, 0, 0, 0, 0, b->w, b->h);
        algif_render_frame (gif, b, i, 0, 0);
        if (durations)
            (*durations)[i] = gif->frames[i].duration;
        prev = b;
    }
    algif_destroy_raw_animation (gif);
    return n;
}

/* Allegrified version. Puts all frames into a single bitmap,
 * with the current color depth. */
BITMAP *
load_gif (AL_CONST char *filename, RGB *pal)
{
    int i;
    GIF_ANIMATION *gif = algif_load_raw_animation (filename);
    BITMAP *bmp = NULL;
    GIF_PALETTE gifpal;
    PALETTE tmppal;

    if (!gif || gif->frames_count == 0)
        return NULL;

    /* Either use the global palette, or the palette of the first frame. */
    gifpal = gif->palette;
    if (gifpal.colors_count == 0)
    {
        gifpal = gif->frames[0].palette;
    }

    if (!pal)
        pal = tmppal;

    for (i = 0; i < gifpal.colors_count; i++)
    {
        pal[i].r = gifpal.colors[i].r / 4;
        pal[i].g = gifpal.colors[i].g / 4;
        pal[i].b = gifpal.colors[i].b / 4;
    }

    for ( ; i < PAL_SIZE; i++) {
        pal[i].r = 0;
        pal[i].g = 0;
        pal[i].b = 0;
    }

    if (gif)
    {
        bmp = create_bitmap (gif->width, gif->height);

        select_palette(pal);

        for (i = 0; i < gif->frames_count; i++)
        {
            algif_render_frame (gif, bmp, i, 0, 0);
        }

        unselect_palette();

        algif_destroy_raw_animation (gif);
    }

    return bmp;
}

/* Allegrified version. Saves only a single bitmap. */
int
save_gif (AL_CONST char *filename, BITMAP *bmp, AL_CONST PALETTE pal)
{
    GIF_ANIMATION gif;
    GIF_FRAME frame;
    int ret, i;
    PALETTE cp;

    gif.width = bmp->w;
    gif.height = bmp->h;
    gif.frames_count = 1;
    gif.background_index = 0;
    gif.loop = -1;
    gif.palette.colors_count = 0;
    
    gif.frames = &frame;
    frame.bitmap_8_bit = create_bitmap_ex (8, bmp->w, bmp->h);
    frame.palette.colors_count = 0;
    frame.xoff = 0;
    frame.yoff = 0;
    frame.duration = 0;
    frame.disposal_method = 0;
    frame.transparent_index = -1;

    blit (bmp, frame.bitmap_8_bit, 0, 0, 0, 0, bmp->w, bmp->h);

    if (!pal)
    {
        get_palette (cp);
        pal = cp;
    }

    frame.palette.colors_count = 256;
    for (i = 0; i < 256; i++)
    {
        frame.palette.colors[i].r = _rgb_scale_6[pal[i].r];
        frame.palette.colors[i].g = _rgb_scale_6[pal[i].g];
        frame.palette.colors[i].b = _rgb_scale_6[pal[i].b];
    }

    ret = algif_save_raw_animation (filename, &gif);
    destroy_bitmap (frame.bitmap_8_bit);
    return ret;
}


GIF_ANIMATION *
algif_create_raw_animation (int frames_count)
{
    GIF_ANIMATION *gif = calloc (1, sizeof *gif);
    /* Create frames. */
    gif->frames_count = frames_count;
    gif->frames = calloc (gif->frames_count, sizeof *gif->frames);
    return gif;
}

/* Destroy a complete gif, including all frames. */
void
algif_destroy_raw_animation (GIF_ANIMATION *gif)
{
    int i;

    for (i = 0; i < gif->frames_count; i++)
    {
        GIF_FRAME *frame = gif->frames + i;

        if (frame->bitmap_8_bit)
            destroy_bitmap (frame->bitmap_8_bit);
    }
    if (gif->store)
        destroy_bitmap (gif->store);
    free (gif->frames);
    free (gif);
}

static void
write_palette (PACKFILE *file, GIF_PALETTE *palette, int bits)
{
    int i;
    for (i = 0; i < (1 << bits); i++)
    {
        pack_putc (palette->colors[i].r, file);
        pack_putc (palette->colors[i].g, file);
        pack_putc (palette->colors[i].b, file);
    }
}

static void
read_palette (PACKFILE * file, GIF_PALETTE *palette)
{
    int i;

    for (i = 0; i < palette->colors_count; i++)
    {
        palette->colors[i].r = pack_getc (file);
        palette->colors[i].g = pack_getc (file);
        palette->colors[i].b = pack_getc (file);
    }
}

/*
 * Compresses all the frames in the animation and writes to a gif file.
 * Nothing else like extensions or comments is written.
 *
 * Returns 0 on success.
 *
 * Note: All bitmaps must have a color depth of 8.
 */
int
algif_save_raw_animation (const char *filename, GIF_ANIMATION *gif)
{
    int frame;
    int i, j;
    PACKFILE *file;

    file = pack_fopen (filename, "w");
    if (!file)
        return -1;

    pack_fwrite ("GIF89a", 6, file);
    pack_iputw (gif->width, file);
    pack_iputw (gif->height, file);
    /* 7 global palette
     * 456 color richness
     * 3 sorted
     * 012 palette bits
     */
    for (i = 1, j = 0; i < gif->palette.colors_count; i *= 2, j++);
    pack_putc ((j ? 128 : 0) + 64 + 32 + 16 + (j ? j - 1 : 0), file);
    pack_putc (gif->background_index, file);
    pack_putc (0, file);        /* No aspect ratio. */

    if (j)
        write_palette (file, &gif->palette, j);

    if (gif->loop != -1)
    /* Loop count extension. */
    {
        pack_putc (0x21, file); /* Extension Introducer. */
        pack_putc (0xff, file); /* Application Extension. */
        pack_putc (11, file);    /* Size. */
        pack_fwrite ("NETSCAPE2.0", 11, file);
        pack_putc (3, file); /* Size. */
        pack_putc (1, file);
        pack_iputw (gif->loop, file);
        pack_putc (0, file);
    }

    for (frame = 0; frame < gif->frames_count; frame++)
    {
        int w = gif->frames[frame].bitmap_8_bit->w;
        int h = gif->frames[frame].bitmap_8_bit->h;

        pack_putc (0x21, file); /* Extension Introducer. */
        pack_putc (0xf9, file); /* Graphic Control Extension. */
        pack_putc (4, file);    /* Size. */
        /* Disposal method, and enable transparency. */
        i = gif->frames[frame].disposal_method << 2;
        if (gif->frames[frame].transparent_index != -1)
            i |= 1;
        pack_putc (i, file);
        pack_iputw (gif->frames[frame].duration, file); /* In 1/100th seconds. */
        if (gif->frames[frame].transparent_index != -1)
            pack_putc (gif->frames[frame].transparent_index, file);       /* Transparent color index. */
        else
             pack_putc (0, file);
        pack_putc (0x00, file); /* Terminator. */

        pack_putc (0x2c, file); /* Image Descriptor. */
        pack_iputw (gif->frames[frame].xoff, file);
        pack_iputw (gif->frames[frame].yoff, file);
        pack_iputw (w, file);
        pack_iputw (h, file);

         /* 7: local palette
         * 6: interlaced
         * 5: sorted
         * 012: palette bits
         */

        for (i = 1, j = 0; i < gif->frames[frame].palette.colors_count; i *= 2, j++);
        pack_putc ((j ? 128 : 0) + (j ? j - 1 : 0), file);

        if (j)
            write_palette (file, &gif->frames[frame].palette, j);

        LZW_encode (file, gif->frames[frame].bitmap_8_bit);

        pack_putc (0x00, file); /* Terminator. */
    }

    pack_putc (0x3b, file);     /* Trailer. */

    pack_fclose (file);
    return 0;
}

static void
deinterlace (BITMAP *bmp)
{
    BITMAP *n = create_bitmap_ex (BITMAP_COLOR_DEPTH, bmp->w, bmp->h);
    int y, i = 0;
    for (y = 0; y < n->h; y += 8)
    {
        blit (bmp, n, 0, i++, 0, y, n->w, 1);
    }
    for (y = 4; y < n->h; y += 8)
    {
        blit (bmp, n, 0, i++, 0, y, n->w, 1);
    }
    for (y = 2; y < n->h; y += 4)
    {
        blit (bmp, n, 0, i++, 0, y, n->w, 1);
    }
    for (y = 1; y < n->h; y += 2)
    {
        blit (bmp, n, 0, i++, 0, y, n->w, 1);
    }
    blit (n, bmp, 0, 0, 0, 0, n->w, n->h);
    destroy_bitmap (n);
}

static GIF_ANIMATION *
load_object (PACKFILE * file, long size)
{
    int version;
    BITMAP *bmp = NULL;
    int i, j;
    GIF_ANIMATION *gif = calloc (1, sizeof *gif);
    GIF_FRAME frame;
    int have_global_palette = 0;

    (void) size;

    gif->frames_count = 0;

    /* is it really a GIF? */
    if (pack_getc (file) != 'G')
        goto error;
    if (pack_getc (file) != 'I')
        goto error;
    if (pack_getc (file) != 'F')
        goto error;
    if (pack_getc (file) != '8')
        goto error;
    /* '7' or '9', for 87a or 89a. */
    version = pack_getc (file);
    if (version != '7' && version != '9')
        goto error;
    if (pack_getc (file) != 'a')
        goto error;

    gif->width = pack_igetw (file);
    gif->height = pack_igetw (file);
    i = pack_getc (file);
    /* Global color table? */
    if (i & 128)
        gif->palette.colors_count = 1 << ((i & 7) + 1);
    else
        gif->palette.colors_count = 0;
    /* Background color is only valid with a global palette. */
    gif->background_index = pack_getc (file);

    /* Skip aspect ratio. */
    pack_fseek (file, 1);

    if (gif->palette.colors_count)
    {
        read_palette (file, &gif->palette);
        have_global_palette = 1;
    }

    memset(&frame, 0, sizeof frame); /* For first frame. */
    frame.transparent_index = -1;

    do
    {
        i = pack_getc (file);

        switch (i)
        {
            case 0x2c:         /* Image Descriptor */
            {
                int w, h;
                int interlaced = 0;

                frame.xoff = pack_igetw (file);
                frame.yoff = pack_igetw (file);
                w = pack_igetw (file);
                h = pack_igetw (file);
                bmp = create_bitmap_ex (8, w, h);
                if (!bmp)
                    goto error;
                i = pack_getc (file);

                /* Local palette. */
                if (i & 128)
                {
                    frame.palette.colors_count = 1 << ((i & 7) + 1);
                    read_palette (file, &frame.palette);
                }
                else
                {
                    frame.palette.colors_count = 0;
                }

                if (i & 64)
                    interlaced = 1;

                if (LZW_decode (file, bmp))
                    goto error;

                if (interlaced)
                    deinterlace (bmp);

                frame.bitmap_8_bit = bmp;
                bmp = NULL;

                gif->frames_count++;
                gif->frames =
                    realloc (gif->frames,
                             gif->frames_count * sizeof *gif->frames);
                gif->frames[gif->frames_count - 1] = frame;

                memset(&frame, 0, sizeof frame); /* For next frame. */
                frame.transparent_index = -1;

                break;
            }
            case 0x21: /* Extension Introducer. */
                j = pack_getc (file); /* Extension Type. */
                i = pack_getc (file); /* Size. */
                if (j == 0xf9) /* Graphic Control Extension. */
                {
                    /* size must be 4 */
                    if (i != 4)
                        goto error;
                    i = pack_getc (file);
                    frame.disposal_method = (i >> 2) & 7;
                    frame.duration = pack_igetw (file);
                    if (i & 1)  /* Transparency? */
                    {
                        frame.transparent_index = pack_getc (file);
                    }
                    else
                    {
                        pack_fseek (file, 1); 
                        frame.transparent_index = -1;
                    }
                    i = pack_getc (file); /* Size. */
                }
                /* Application Extension. */
                else if (j == 0xff)
                {
                    if (i == 11)
                    {
                        char name[12];
                        pack_fread (name, 11, file);
                        i = pack_getc (file); /* Size. */
                        name[11] = '\0';
                        if (!strcmp (name, "NETSCAPE2.0"))
                        {
                            if (i == 3)
                            {
                                j = pack_getc (file);
                                gif->loop = pack_igetw (file);
                                if (j != 1)
                                    gif->loop = 0;
                                i = pack_getc (file); /* Size. */
                            }
                        }
                    }
                }

                /* Possibly more blocks until terminator block (0). */
                while (i)
                {
                    pack_fseek (file, i);
                    i = pack_getc (file);
                }
                break;
            case 0x3b:
                /* GIF Trailer. */
                pack_fclose (file);
                return gif;
        }
    }
    while (TRUE);
  error:
    if (file)
        pack_fclose (file);
    if (gif)
        algif_destroy_raw_animation (gif);
    if (bmp)
        destroy_bitmap (bmp);
    return NULL;
}

/*
 * Allocates and reads a GIF_ANIMATION structure, filling in all the
 * frames found in the file. On error, nothing is allocated, and NULL is
 * returned. No extensions or comments are read in. If the gif contains
 * a transparency index, and it is no 0, it is swapped with 0 - so index
 * 0 will be the transparent color. There is no way to know when a file
 * contained no transparency originally. Frame duration is specified in
 * 1/100th seconds.
 *
 * All bitmaps will have a color depth of 8.
 */
GIF_ANIMATION *
algif_load_raw_animation (const char *filename)
{
    PACKFILE *file;
    GIF_ANIMATION *gif = NULL;

    file = pack_fopen (filename, "r");
    if (file)
        gif = load_object (file, 0);
    return gif;
}

static int
get_rgbcolor (RGB *rgb)
{
    return makecol (rgb->r, rgb->g, rgb->b);
}

/* Renders the next frame in a GIF animation to the specified bitmap and
 * the given position. You need to call this in order on the same
 * destination for frames [0..gif->frames_count - 1] to properly render all
 * the frames in the GIF.
 */
void
algif_render_frame (GIF_ANIMATION *gif, BITMAP *bitmap, int frame, int xpos,
                    int ypos)
{
    int x, y, w, h;
    int mask_color = makecol(255, 0, 255);
    GIF_FRAME *f = &gif->frames[frame];
    GIF_PALETTE *pal;
    if (frame == 0)
        rectfill (bitmap, xpos, ypos, xpos + gif->width - 1,
              ypos + gif->height - 1, mask_color);
    else
    {
        GIF_FRAME *p = &gif->frames[frame - 1];
        if (p->disposal_method == 2)
            rectfill (bitmap, xpos + p->xoff, ypos + p->yoff,
                xpos + p->xoff + p->bitmap_8_bit->w - 1,
                ypos + p->yoff + p->bitmap_8_bit->h - 1,
                bitmap_mask_color (bitmap));
        else if (p->disposal_method == 3 && gif->store)
        {
            blit (gif->store, bitmap, 0, 0, xpos + p->xoff, ypos + p->yoff,
                gif->store->w, gif->store->h);
            destroy_bitmap (gif->store);
            gif->store = NULL;
        }
    }
    w = f->bitmap_8_bit->w;
    h = f->bitmap_8_bit->h;
    if (f->disposal_method == 3)
    {
        if (gif->store)
            destroy_bitmap (gif->store);
        gif->store = create_bitmap_ex (BITMAP_COLOR_DEPTH, w, h);
        blit (bitmap, gif->store, xpos + f->xoff, ypos + f->yoff, 0, 0, w, h);
    }
    pal = &gif->frames[frame].palette;
    if (pal->colors_count == 0)
        pal = &gif->palette;

    //int i;
    //for (i = 0; i < pal->colors_count; i++)
    //    printf("%d: %d %d %d\n", i, pal->colors[i].r, pal->colors[i].g, pal->colors[i].b);

    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            int c = getpixel (f->bitmap_8_bit, x, y);
            if (c != f->transparent_index)
            {
                putpixel (bitmap, xpos + f->xoff + x, ypos + f->yoff + y,
                          get_rgbcolor (&pal->colors[c]));
            }
        }
    }
}

/* Registers gif as bitmap and datafile type. */
void
algif_init (void)
{
    register_bitmap_file_type ("gif", load_gif, save_gif);
    register_datafile_object (DAT_GIF,
                              (void *(*)(PACKFILE *, long)) load_object,
                              (void (*)(void *))
                              algif_destroy_raw_animation);
}

static int
read_code (PACKFILE * file, char *buf, int *bit_pos, int bit_size)
{
    int i;
    int code = 0;
    int pos = 1;

    for (i = 0; i < bit_size; i++)
    {
        int byte_pos = (*bit_pos >> 3) & 255;

        if (byte_pos == 0)
        {
            int data_len = pack_getc (file);

            if (data_len == 0)
            {
                //printf ("Fatal. Errorneous GIF stream.\n");
                //abort ();
                return -1;
            }
            pack_fread (buf + 256 - data_len, data_len, file);
            byte_pos = 256 - data_len;
            *bit_pos = byte_pos << 3;
        }
        if (buf[byte_pos] & (1 << (*bit_pos & 7)))
            code += pos;
        pos += pos;
        (*bit_pos)++;
    }
    return code;
}

static void
write_code (PACKFILE * file, char *buf, int *bit_pos, int bit_size, int code)
{
    int i;
    int pos = 1;

    for (i = 0; i < bit_size; i++)
    {
        int byte_pos = *bit_pos >> 3;

        if (code & pos)
            buf[byte_pos] |= (1 << (*bit_pos & 7));
        else
            buf[byte_pos] &= ~(1 << (*bit_pos & 7));
        (*bit_pos)++;
        if (*bit_pos == 2040)
        {
            pack_putc (byte_pos + 1, file);
            pack_fwrite (buf, byte_pos + 1, file);
            *bit_pos = 0;
        }
        pos += pos;
    }
}

static int
read_pixel (BITMAP *bmp, int pos)
{
    return getpixel (bmp, pos % bmp->w, pos / bmp->w);
}

static void
write_pixel (BITMAP *bmp, int pos, int code)
{
    putpixel (bmp, pos % bmp->w, pos / bmp->w, code);
}

int
LZW_decode (PACKFILE * file, BITMAP *bmp)
{
    int orig_bit_size;
    char buf[256];
    int bit_size;
    int bit_pos;
    int clear_marker;
    int end_marker;
    struct
    {
        int prefix;
        int c;
        int len;
    }
    codes[4096];                /* Maximum bit size is 12. */
    int n;
    int i, prev, code, c;
    int out_pos = 0;

    orig_bit_size = pack_getc (file);
    n = 2 + (1 << orig_bit_size);

    for (i = 0; i < n; i++)
    {
        codes[i].c = i;
        codes[i].len = 0;
    }

    clear_marker = n - 2;
    end_marker = n - 1;

    bit_size = orig_bit_size + 1;

    bit_pos = 0;

    /* Expect to read clear code as first code here. */
    prev = read_code (file, buf, &bit_pos, bit_size);
    if (prev == -1)
        return -1;
    do
    {
        code = read_code (file, buf, &bit_pos, bit_size);
        if (code == -1)
            return -1;
        if (code == clear_marker)
        {
            bit_size = orig_bit_size;
            n = 1 << bit_size;
            n += 2;
            bit_size++;
            prev = code;
            continue;
        }

        if (code == end_marker)
            break;

        /* Known code: ok. Else: must be doubled char. */
        if (code < n)
            c = code;
        else
            c = prev;

        /* Output the code. */
        out_pos += codes[c].len;
        i = 0;
        do
        {
            write_pixel (bmp, out_pos - i, codes[c].c);
            if (codes[c].len)
                c = codes[c].prefix;
            else
                break;
            i++;
        }
        while (1);

        out_pos++;

        /* Unknown code -> must be double char. */
        if (code >= n)
        {
            write_pixel (bmp, out_pos, codes[c].c);
            out_pos++;
        }

        /* Except after clear marker, build new code. */
        if (prev != clear_marker)
        {
            codes[n].prefix = prev;
            codes[n].len = codes[prev].len + 1;
            codes[n].c = codes[c].c;
            n++;
        }

        /* Out of bits? Increase. */
        if (n == (1 << bit_size))
        {
            if (bit_size < 12)
                bit_size++;
        }

        prev = code;
    }
    while (1);
    return 0;
}

static int
get_minimum_bitsize (BITMAP *bmp)
{
    int x, y, max = 0, b = 2;
    for (y = 0; y < bmp->h; y++)
    {
        for (x = 0; x < bmp->w; x++)
        {
            int c = getpixel (bmp, x, y);
            if (c > max)
                max = c;
        }
    }
    while ((1 << b) <= max)
    {
        b++;
    }
    return b;
}

void
LZW_encode (PACKFILE * file, BITMAP *bmp)
{
    int orig_bit_size;
    int bit_size;
    char buf[256];
    int bit_pos;
    int clear_marker;
    int end_marker;
    struct
    {
        int prefix;
        int c;
        int len;
    }
    codes[4096];                /* Maximum bit size is 12. */
    int code, prev;
    int in_pos;
    int n, i;

    orig_bit_size = get_minimum_bitsize (bmp);

    n = 2 + (1 << orig_bit_size);

    for (i = 0; i < n; i++)
    {
        codes[i].c = i;
        codes[i].len = 0;
    }

    clear_marker = n - 2;
    end_marker = n - 1;

    pack_putc (orig_bit_size, file);

    bit_size = orig_bit_size + 1;

    bit_pos = 0;

    /* Play fair and put a clear marker at the start. */
    write_code (file, buf, &bit_pos, bit_size, clear_marker);

    prev = read_pixel (bmp, 0);

    for (in_pos = 1; in_pos < bmp->w * bmp->h; in_pos++)
    {
        code = read_pixel (bmp, in_pos);

        if (prev != clear_marker)
        {
            /* Search for the code. */
            for (i = end_marker + 1; i < n; i++)
            {
                if (codes[i].prefix == prev && codes[i].c == code)
                {
                    code = i;
                    break;
                }
            }

            /* If not found, add it, and write previous code. */
            if (i == n)
            {
                codes[n].prefix = prev;
                codes[n].len = codes[prev].len + 1;
                codes[n].c = code;
                n++;

                write_code (file, buf, &bit_pos, bit_size, prev);
            }
        }

        /* Out of bits? Increase. */
        if (n == 1 + (1 << bit_size))
        {
            if (bit_size < 12)
                bit_size++;
        }

        /* Too big table? Clear and start over. */
        if (n == 4096)
        {
            write_code (file, buf, &bit_pos, bit_size, clear_marker);
            bit_size = orig_bit_size + 1;
            n = end_marker + 1;
        }

        prev = code;
    }
    write_code (file, buf, &bit_pos, bit_size, prev);
    write_code (file, buf, &bit_pos, bit_size, end_marker);
    if (bit_pos)
    {
        int byte_pos = (bit_pos + 7) / 8;

        pack_putc (byte_pos, file);
        pack_fwrite (buf, byte_pos, file);
    }
}
