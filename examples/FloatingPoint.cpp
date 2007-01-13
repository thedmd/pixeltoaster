// Floating Point Color.
// Pixels are float r,g,b,a values automatically clamped in range [0,1]
// Part of the PixelToaster Framebuffer Library - http://www.pixeltoaster.com

#include <PixelToaster.h>

using namespace PixelToaster;

int main()
{
    const int width = 320;
    const int height = 240;

	Display display( "Floating Point Example", width, height );

	vector<Pixel> pixels( width * height );

    while ( display.open() )
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

		display.update( pixels );
    }
}
