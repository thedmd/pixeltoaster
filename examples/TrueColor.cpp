// TrueColor Example
// How to open a display in truecolor mode and work with 32 bit integer pixels.
// Copyright © Glenn Fiedler, 2004-2006. http://www.pixeltoaster.com

#include <PixelToaster.h>

using namespace PixelToaster;

int main()
{
    const int width = 320;
    const int height = 240;

	Display display( "TrueColor Example", width, height, Output::Default, Mode::TrueColor );

	vector<TrueColorPixel> pixels( width * height );

    while ( display.open() )
    {
        unsigned int index = 0;

        for ( int y = 0; y < height; ++y )
        {
            for ( int x = 0; x < width; ++x )
            {
				pixels[index].r = x < 255 ? x : 255;
				pixels[index].g = y < 255 ? y : 255;
				pixels[index].b = x+y < 255 ? x+y : 255;

				++index;
            }
        }

		display.update( pixels );
    }
}
