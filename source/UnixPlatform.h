// Unix Platform
// Copyright © 2004-2007 Glenn Fiedler
// Part of the PixelToaster Framebuffer Library - http://www.pixeltoaster.com

#define XK_LATIN1
#define XK_MISCELLANY

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>

// for timer
#include <time.h>
#include <errno.h>
#include <math.h>

//#define PIXELTOASTER_RDTSC
#ifdef PIXELTOASTER_RDTSC
#	include <stdint.h>
#	include <stdio.h>
#endif

namespace PixelToaster
{
	#include "UnixDisplay.h"
	#include "UnixTimer.h"
}

#define TimerClass UnixTimer
#define DisplayClass UnixDisplay
