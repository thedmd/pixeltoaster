// Snow Crash
// Respect to Neil Stephenson
// Copyright © Glenn Fiedler
// http://www.pixeltoaster.com

#include <PixelToaster.h>

using namespace PixelToaster;

int main()
{
    const int width = 320;
    const int height = 240;

    Display display( "Snow Crash", width, height, Output::Fullscreen, Mode::TrueColor );

    vector<TrueColorPixel> pixels( width * height );

	int seed = 0x12345;

    while ( display.open() )
    {
        for ( unsigned int i = 0; i < pixels.size(); ++i )
        {
            int noise = seed;
            noise >>= 3;
            noise ^= seed;
            int carry = noise & 1;
            noise >>= 1;
            seed >>= 1;
            seed |= (carry << 30);
            noise &= 0xFF;
            
			pixels[i].r = noise;
			pixels[i].g = noise;
			pixels[i].b = noise;
        }

        display.update( pixels );
    }
}
