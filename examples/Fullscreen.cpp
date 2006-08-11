// Fullscreen Example.
// Opens a display for fullscreen output in floating point color mode.
// Copyright © Glenn Fiedler, 2004-2005. http://www.pixeltoaster.com

#include <PixelToaster.h>

using namespace PixelToaster;

int main()
{
    const int width = 320;
    const int height = 240;

	Display display( "Fullscreen Example", width, height, Output::Fullscreen );

	vector<Pixel> pixels( width * height );

    while ( display.open() )
    {
        unsigned int index = 0;

        for ( int y = 0; y < height; ++y )
        {
            for ( int x = 0; x < width; ++x )
            {
                pixels[index].r = 0.8f + y * 0.0015f;
                pixels[index].g = 0.2f + y * 0.00075f;
                pixels[index].b = 0.1f + y * 0.0005f;

				++index;
            }
        }

		display.update( pixels );
    }
}
