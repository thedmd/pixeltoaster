// Profile conversion routines
// Copyright © 2004-2007 Glenn Fiedler
// Part of the PixelToaster Framebuffer Library - http://www.pixeltoaster.com

#include "PixelToaster.h"

using namespace PixelToaster;

char * getFormatString( Format format )
{
    switch ( format )
    {
        case Format::XRGB8888: 	return "truecolor";
        case Format::XBGR8888: 	return "xbgr8888";
        case Format::RGB888: 	return "rgb888";
        case Format::BGR888: 	return "bgr888";
        case Format::RGB565: 	return "rgb565";
        case Format::BGR565: 	return "bgr565";
        case Format::XRGB1555: 	return "xrgb1555";
        case Format::XBGR1555: 	return "xbgr1555";
        case Format::XBGRFFFF: 	return "floating point";
        default: 				return "???";
    }
}

const float duration = 1.0f;

Timer timer;

void profilePixelConverter( Format format, const Pixel * source, void * destination, int count )
{
    printf( "   floating point -> %s", getFormatString(format) );

    Converter * converter = requestConverter( Format::XBGRFFFF, format );

    if ( !converter )
    {
        printf( "\n     failed: null converter\n" );
        exit(1);
    }

    double startTime = timer.time();

    double time = 0.0;

    int iterations = 0;

    while ( time < duration )
    {
        converter->convert( source, destination, count );
        time = timer.time() - startTime;
        iterations ++;
    }

    printf( " = %f ms\n", (double) time / iterations * 1000 );
}

void profileIntegerConverter( Format format, const integer32 *source, void *destination, int count )
{
    printf( "   truecolor -> %s", getFormatString(format) );

    Converter * converter = requestConverter( Format::XRGB8888, format );

    if ( !converter )
    {
        printf( "\n     failed: null converter\n" );
        exit(1);
    }

    double startTime = timer.time();

    double time = 0.0;

    int iterations = 0;

    while ( time < duration )
    {
        converter->convert( source, destination, count );
        time = timer.time() - startTime;
        iterations ++;
    }

    printf( " = %f ms\n", (double) time / iterations * 1000 );
}


int main()
{
    const int width = 256;
    const int height = 256;

	vector<Pixel> pixelSource( width * height );

    for ( unsigned int i = 0; i < pixelSource.size(); ++i )
    {
        pixelSource[i].r = 1.5;
        pixelSource[i].g = 0.5;
        pixelSource[i].b = 0.25;
        pixelSource[i].a = 0;
    }

    vector<integer32> integerSource( width * height );

    for ( unsigned int i = 0; i < integerSource.size(); ++i )
        integerSource[i] = 0x00FF6677;

    integer8 * destination = new integer8[width*height*16];

	printf( "\n[ PixelToaster Profiling Suite ]\n\n" );

	printf( "floating point color conversion routines:\n\n" );

    profilePixelConverter( Format::XBGRFFFF, &pixelSource[0], destination, (int) pixelSource.size() );
    profilePixelConverter( Format::XRGB8888, &pixelSource[0], destination, (int) pixelSource.size() );
    profilePixelConverter( Format::XBGR8888, &pixelSource[0], destination, (int) pixelSource.size() );
    profilePixelConverter( Format::RGB888, &pixelSource[0], destination, (int) pixelSource.size() );
    profilePixelConverter( Format::BGR888, &pixelSource[0], destination, (int) pixelSource.size() );
    profilePixelConverter( Format::RGB565, &pixelSource[0], destination, (int) pixelSource.size() );
    profilePixelConverter( Format::BGR565, &pixelSource[0], destination, (int) pixelSource.size() );
    profilePixelConverter( Format::XRGB1555, &pixelSource[0], destination, (int) pixelSource.size() );
    profilePixelConverter( Format::XBGR1555, &pixelSource[0], destination, (int) pixelSource.size() );

	printf( "\ntruecolor conversion routines:\n\n" );

    profileIntegerConverter( Format::XBGRFFFF, &integerSource[0], destination, (int) integerSource.size() );
    profileIntegerConverter( Format::XRGB8888, &integerSource[0], destination, (int) integerSource.size() );
    profileIntegerConverter( Format::XBGR8888, &integerSource[0], destination, (int) integerSource.size() );
    profileIntegerConverter( Format::RGB888, &integerSource[0], destination, (int) integerSource.size() );
    profileIntegerConverter( Format::BGR888, &integerSource[0], destination, (int) integerSource.size() );
    profileIntegerConverter( Format::RGB565, &integerSource[0], destination, (int) integerSource.size() );
    profileIntegerConverter( Format::BGR565, &integerSource[0], destination, (int) integerSource.size() );
    profileIntegerConverter( Format::XRGB1555, &integerSource[0], destination, (int) integerSource.size() );
    profileIntegerConverter( Format::XBGR1555, &integerSource[0], destination, (int) integerSource.size() );

	printf( "\n" );
}
