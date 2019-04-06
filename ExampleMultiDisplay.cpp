// Multiple Displays Example.
// Demonstrates how to open and update multiple displays.
// Part of the PixelToaster Framebuffer Library - http://www.pixeltoaster.com

#include "PixelToaster.h"
#include <stdio.h>

#ifdef PIXELTOASTER_NO_STL
#    include <vector>
using std::vector;
#endif

using namespace PixelToaster;

class MultiDisplayListener : public Listener
{
    void onOpen(DisplayInterface& display) override
    {
        printf("open: %s\n", display.title());
    }

    bool onClose(DisplayInterface& display) override
    {
        printf("close: %s\n", display.title());
        return true;
    }

    void onMouseMove(DisplayInterface& display, Mouse mouse) override
    {
        printf("%s: mouse move (%f,%f)\n",
               display.title(),
               mouse.x,
               mouse.y);
    }
};

MultiDisplayListener listener;

int main()
{
    const int width  = 320;
    const int height = 240;

    Display a, b, c;

    a.listener(&listener);
    b.listener(&listener);
    c.listener(&listener);

    a.open("Display A", width, height);
    b.open("Display B", width, height);
    c.open("Display C", width, height);

    vector<Pixel> pixels(width * height);

    while (a.open() || b.open() || c.open())
    {
        unsigned int index = 0;

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                pixels[index].r = 0.1f + (x + y) * 0.0015f;
                pixels[index].g = 0.5f + (x + y) * 0.001f;
                pixels[index].b = 0.7f + (x + y) * 0.0005f;

                ++index;
            }
        }

        if (a.open())
        {
#ifdef PIXELTOASTER_NO_STL
            a.update(pixels.data());
#else
            a.update(pixels);
#endif
        }

        if (b.open())
        {
#ifdef PIXELTOASTER_NO_STL
            b.update(pixels.data());
#else
            b.update(pixels);
#endif
        }

        if (c.open())
        {
#ifdef PIXELTOASTER_NO_STL
            c.update(pixels.data());
#else
            c.update(pixels);
#endif
        }
    }
}
