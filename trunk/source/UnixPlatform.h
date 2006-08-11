// Unix Platform
// Copyright © Glenn Fiedler
// http://www.pixeltoaster.com

#define XK_LATIN1
#define XK_MISCELLANY

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>

// for timer
#include <stdint.h>
#include <stdio.h>

// for timer sleep
#include <time.h>
#include <errno.h>
#include <math.h>

namespace PixelToaster
{
	#include "UnixDisplay.h"
	#include "UnixTimer.h"
}

#define TimerClass UnixTimer
#define DisplayClass UnixDisplay
