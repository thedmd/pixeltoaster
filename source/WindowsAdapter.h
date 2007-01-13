// Windows Adapter interface
// Copyright © 2006-2007 Glenn Fiedler
// Part of the PixelToaster Framebuffer Library - http://www.pixeltoaster.com

class WindowsAdapter
{
public:

	virtual bool paint() = 0;				// paint pixels to display
	virtual bool fullscreen() = 0;			// switch to fullscreen output
	virtual bool windowed() = 0;			// switch to windowed output
	virtual void toggle() = 0;				// toggle fullscreen/windowed output
	virtual void exit() = 0;				// user wants to exit (pressed escape or alt-f4 and there is no listener)
};
