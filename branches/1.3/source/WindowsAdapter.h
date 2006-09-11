// Windows Adapter interface
// Copyright © 2004-2006, Glenn Fiedler
// http://www.pixeltoaster.com

class WindowsAdapter
{
public:

	virtual bool paint() = 0;				// paint pixels to display
	virtual bool fullscreen() = 0;			// switch to fullscreen output
	virtual bool windowed() = 0;			// switch to windowed output
	virtual bool toggle() = 0;				// toggle fullscreen/windowed output
	virtual void exit() = 0;				// user wants to exit (pressed escape or alt-f4 and there is no listener)
};
