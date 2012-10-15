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

int call_on_draw(int);