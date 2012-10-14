#define VERSION_STRING "1.0B"
#define ARCH_STRING "linux_x86"

#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <asm/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/hiddev.h>
#include "lua.h"

#define EV_NUM 5

int calibrate_scale (char *scale_file) {
    int fd = -1;
    int last_status = 0;
    struct hiddev_event ev[EV_NUM];
    char name[100];
    int weight_oz;
    int weight_pd;
    int tare;
    int temp;
    int settle = 0;
    int conversion = 76.3;


    if ((fd = open(scale_file, O_RDONLY)) < 0) {
        perror("hiddev open");
        exit(1);
    }

    ioctl(fd, HIDIOCGNAME(100), name);

    printf("%s ready...\n", name);
  
    printf("Reading values .. \n");
    
    while (1) {
        read(fd, ev, sizeof(struct hiddev_event) * EV_NUM);
        printf("EV: PARAM %x : %d %d %d %d\n", ev[0].hid, ev[0].value, ev[1].value, ev[2].value, ev[3].value);
	if (ev[0].value == 24) {
		temp = ev[2].value;
		if (temp < 0) {
			// flip it
			temp = temp / -1;
			// invert it
			temp = 128 - temp;
			// add it
			temp = 128 + temp;
		}
		temp = temp + (ev[1].value * 255);
		if (settle < 10) {
		    settle += 1;
		    tare = temp;
		    printf("Calibrating... %d\n", tare);
		} else {		
		    break;
		}
	}
    }

    close(fd);

    return tare;
} 


int read_scale (char *scale_file, int tare) {
    int fd = -1;
    int last_status = 0;
    struct hiddev_event ev[EV_NUM];
    char name[100];
    int weight_oz;
    int weight_pd;
    int temp;
    int settle = 0;
    int conversion = 76.3;


    if ((fd = open(scale_file, O_RDONLY)) < 0) {
        perror("hiddev open");
        exit(1);
    }

    ioctl(fd, HIDIOCGNAME(100), name);

    printf("%s ready...\n", name);
  
    printf("Reading values .. \n");
    
    while (1) {
        read(fd, ev, sizeof(struct hiddev_event) * EV_NUM);
        printf("EV: PARAM %x : %d %d %d %d\n", ev[0].hid, ev[0].value, ev[1].value, ev[2].value, ev[3].value);
	if (ev[0].value == 24) {
		temp = ev[2].value;
		if (temp < 0) {
			// flip it
			temp = temp / -1;
			// invert it
			temp = 128 - temp;
			// add it
			temp = 128 + temp;
		}
		temp = temp + (ev[1].value * 255);
		weight_oz = (temp - tare) / conversion;
		//weight_pd = weight_oz / 16;
		//weight_oz = weight_oz - 16 * weight_pd;
		//printf("%d lbs, %d oz\n", weight_pd, weight_oz);
	}
    }

    close(fd);

    return weight_oz;
} 

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