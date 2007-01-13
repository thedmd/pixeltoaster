// Windows Platform
// Copyright © 2006-2007 Glenn Fiedler
// Part of the PixelToaster Framebuffer Library - http://www.pixeltoaster.com

#define VC_EXTRALEAN 
#define WIN32_LEAN_AND_MEAN 

#include <windows.h>
#include <windowsx.h>

#include <d3d9.h>

#ifndef __MINGW32__ 
#include <d3dx9.h> 
#endif

#ifdef _MSC_VER
#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "kernel32.lib" )
#pragma comment( lib, "gdi32.lib" )
#pragma comment( lib, "shell32.lib" )
#pragma comment( lib, "user32.lib" )
#endif

namespace PixelToaster
{
	#include "WindowsAdapter.h"
	#include "WindowsWindow.h"
	#include "WindowsDevice.h"
	#include "WindowsDisplay.h"
	#include "WindowsTimer.h"
}

#define TimerClass WindowsTimer
#define DisplayClass WindowsDisplay
