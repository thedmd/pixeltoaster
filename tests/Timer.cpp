// Timer Example
// Copyright © 2004-2007 Glenn Fiedler
// Part of the PixelToaster Framebuffer Library - http://www.pixeltoaster.com

#include <PixelToaster.h>

using namespace PixelToaster;

int main()
{
    printf( "Timer Example\n\n" );

    Timer timer;

    printf( "theoretical timer resolution is %f microseconds\n\n", 1000000 * timer.resolution() );

    timer.delta(); // warm up    
    timer.delta();
    const double practicalResolution = timer.delta();
    printf( "practical timer resolution is %f microseconds\n\n", 1e6 * practicalResolution );

    const int updates = 500;
    const int width = 320;
    const int height = 240;

	// time truecolor updates

    double trueColorStart, trueColorFinish;

    printf( "timing %d updates in truecolor %dx%d...\n", updates, width, height );
    {
        Display display( "TrueColor Timing", width, height, Output::Windowed, Mode::TrueColor );

        vector<TrueColorPixel> pixels( width * height );

		trueColorStart = timer.time();

        for ( int i = 0; i < updates; ++i )
        {
            memset( &pixels[0], 0, sizeof(integer32) * pixels.size() );
            
			display.update( pixels );
        }

		trueColorFinish = timer.time();
    }

    const double trueColorTime = trueColorFinish - trueColorStart;

    printf( "  %g seconds, %.2f fps average.\n\n", trueColorTime, updates/trueColorTime );

	// time floating color updates

    double floatingPointColorStart, floatingPointColorFinish;

    printf( "timing %d updates in floating point color %dx%d...\n", updates, width, height );
    {
        Display display( "Floating Point Color Timing", width, height );

        vector<FloatingPointPixel> pixels( width * height );

		floatingPointColorStart = timer.time();

        for (int i=0; i<updates; i++)
        {
            memset( &pixels[0], 0, sizeof(Pixel) * pixels.size() );
            
			display.update( pixels );
        }

		floatingPointColorFinish = timer.time();
    }

    const double floatingPointColorTime = floatingPointColorFinish - floatingPointColorStart;

    printf("  %g seconds, %.2f fps average.\n\n", floatingPointColorTime, updates/floatingPointColorTime);

    if (floatingPointColorTime>trueColorTime)
        printf("floating point color is %.2g times slower than truecolor.\n\n", floatingPointColorTime/trueColorTime);
    else
        printf("hell freezes over! floating point color is %.2g times faster than true color.\n\n", trueColorTime/floatingPointColorTime);

	// put the timer class through its paces

    printf( "waiting for one second...\n" );

    const double waitStart = timer.time();

    timer.wait(1.0);

    const double waitFinish = timer.time();

    printf( "  %g seconds elapsed.\n\n", waitFinish-waitStart );

    printf( "resetting timer...\n" );
    printf( "  time before reset: %g seconds\n", timer.time() );
    timer.reset();
    printf( "  time after reset: %g seconds\n\n", timer.time() );

    printf( "time + delta...\n" );
    
    timer.reset();

    for ( int i = 0; i < 10; ++i )
    {
        timer.wait(0.1);
        
        const double time = timer.time();
        const double delta = timer.delta();

        printf( "  time=%.2g, delta=%g\n", time, delta );
    }

    printf("\ndelta only...\n");

    timer.reset();

    for ( int i = 0; i < 10; ++i )
    {
        timer.wait(0.2);

        const double delta = timer.delta();

        printf( "  delta=%g\n", delta );
    }

    printf(" \ndone.\n\n ");
}
