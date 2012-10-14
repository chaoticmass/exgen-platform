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
