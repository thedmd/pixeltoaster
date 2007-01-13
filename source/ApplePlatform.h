// Apple Platform
// Copyright © 2004-2007 Glenn Fiedler
// Part of the PixelToaster Framebuffer Library - http://www.pixeltoaster.com

#include "CoreServices/CoreServices.h"

#define XK_LATIN1
#define XK_MISCELLANY

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>

namespace PixelToaster
{
	#include "AppleTimer.h"
	#include "UnixDisplay.h"
	//#include "AppleDisplay.h"
}

#define TimerClass AppleTimer
#define DisplayClass UnixDisplay		// note: use the X11 display until a native apple display is implemented
//#define DisplayClass AppleDisplay
