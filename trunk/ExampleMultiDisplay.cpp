// Multiple Displays Example.
// Demonstrates how to open and update multiple displays.
// Part of the PixelToaster Framebuffer Library - http://www.pixeltoaster.com

#include "PixelToaster.h"
#include <stdio.h>

using namespace PixelToaster;

class MultiDisplayListener : public Listener
{
	void onOpen( DisplayInterface & display )
	{
		printf( "open: %s\n", display.title() );
	}

	bool onClose( DisplayInterface & display )
	{
		printf( "close: %s\n", display.title() );
		return true;
	}

    void onMouseMove( DisplayInterface & display, Mouse mouse )
    {
		printf( "%s: mouse move (%f,%f)\n", 
				display.title(),
                mouse.x, 
                mouse.y );
    }
};

MultiDisplayListener listener;

int main()
{
    const int width = 320;
    const int height = 240;

	Display a,b,c;
	
	a.listener( &listener );
	b.listener( &listener );
	c.listener( &listener );

	a.open( "Display A", width, height );
	b.open( "Display B", width, height );
	c.open( "Display C", width, height );

	vector<Pixel> pixels( width * height );

    while ( a.open() || b.open() || c.open() )
    {
        unsigned int index = 0;

        for ( int y = 0; y < height; ++y )
        {
            for ( int x = 0; x < width; ++x )
            {
                pixels[index].r = 0.1f + (x + y) * 0.0015f;
                pixels[index].g = 0.5f + (x + y) * 0.001f;
                pixels[index].b = 0.7f + (x + y) * 0.0005f;

				++index;
            }
        }

		if ( a.open() )
			a.update( pixels );

		if ( b.open() )
			b.update( pixels );

		if ( c.open() )
			c.update( pixels );
	}
}
