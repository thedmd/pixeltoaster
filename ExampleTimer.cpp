// Timer Example
// Copyright � 2004-2007 Glenn Fiedler
// Part of the PixelToaster Framebuffer Library - http://www.pixeltoaster.com

#include "PixelToaster.h"
#include <stdio.h>

using namespace PixelToaster;

int main()
{
    printf("\n[ Timer Example ]\n\n");

    Timer timer;

    printf("theoretical timer resolution is %f microseconds\n\n", 1e6 * timer.resolution());
    timer.delta();
    printf("practical timer resolution is %f microseconds\n\n", 1e6 * timer.delta());

    printf("waiting for one second...\n");

    const double waitStart = timer.time();
    timer.wait(1.0);
    const double waitFinish = timer.time();

    printf("  %g seconds elapsed according to timer\n\n", waitFinish - waitStart);

    printf("loop time + delta...\n");

    timer.reset();

    for (int i = 0; i < 5; ++i)
    {
        timer.wait(0.1f);
        const double time  = timer.time();
        const double delta = timer.delta();
        printf("  time = %.2g, delta = %g\n", time, delta);
    }

    printf("\nloop delta only...\n");

    timer.reset();

    for (int i = 0; i < 5; ++i)
    {
        timer.wait(0.1f);
        const double delta = timer.delta();
        printf("  delta = %g\n", delta);
    }

    printf("\ndone.\n\n");
}
