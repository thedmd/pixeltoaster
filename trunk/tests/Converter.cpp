// Converter Unit Test
// Copyright © 2004-2007 Glenn Fiedler
// Part of the PixelToaster Framebuffer Library - http://www.pixeltoaster.com

#include <cstdio>
#include <cstdlib>
#include <Conversion.h>

using namespace PixelToaster;

bool equal(float a, float b)
{
    const float epsilon = 0.00001f;
    const float difference = a - b;
    return difference > epsilon || difference < epsilon;
}

int main()
{
	printf( "\ntesting converter objects:\n\n" );

	printf( "TrueColor -> FloatingPointColor\n" );
    {
        Converter * converter = requestConverter( Format::XRGB8888, Format::XBGRFFFF );

        if ( !converter )
        {
            printf( "invalid converter\n" );
            exit(1);
        }

        converter->begin();

        for ( unsigned int i = 0; i <= 0x00FFFFFF; ++i )
        {
            integer32 a = i;
            Pixel b;
            Pixel c;

            converter->convert( &a, &b, 1 );
            
			convert_XRGB8888_to_XBGRFFFF( &a, &c, 1 );

            if ( !equal( b.r, c.r ) || !equal( b.g, c.g ) || !equal( b.b, c.b ) )
            {
                printf( "failed: (%f,%f,%f) vs. (%f,%f,%f)\n", b.r, b.g, b.b, c.r, c.g, c.b );
                exit(1);
            }
        }

        converter->end();
    }

    printf( "TrueColor -> TrueColor \n" );
    {
        Converter * converter = requestConverter( Format::XRGB8888, Format::XRGB8888  );

        if ( !converter )
        {
            printf( "invalid converter\n" );
            exit(1);
        }

        converter->begin();

        for ( unsigned int i = 0; i <= 0x00FFFFFF; ++i )
        {
            integer32 a = i;
            integer32 b;
            integer32 c;

            converter->convert( &a, &b, 1 );
            
			convert_XRGB8888_to_XRGB8888( &a, &c, 1 );
                                    
            if ( b != c )
            {
                printf( "failed: %d vs. %d\n", b, c );
                exit(1);
            }
        }

        converter->end();
    }

    printf( "TrueColor -> XBGR8888\n");
    {
        Converter * converter = requestConverter( Format::XRGB8888, Format::XBGR8888 );

        if ( !converter )
        {
            printf( "invalid converter\n" );
            exit(1);
        }

        converter->begin();

        for ( unsigned int i = 0; i <= 0x00FFFFFF; ++i )
        {
            integer32 a = i;
            integer32 b;
            integer32 c;

            converter->convert( &a, &b, 1 );
            
			convert_XRGB8888_to_XBGR8888( &a, &c, 1 );

            if ( b != c )
            {
                printf( "failed: %d vs. %d\n", b, c );
                exit(1);
            }
        }

        converter->end();
    }

    printf( "TrueColor -> RGB888\n" );
    {
        Converter * converter = requestConverter( Format::XRGB8888, Format::RGB888 );

        if ( !converter )
        {
            printf( "invalid converter\n" );
            exit(1);
        }

        converter->begin();

        for ( unsigned int i = 0; i <= 0x00FFFFFF; ++i )
        {
            integer32 a = i;
            integer8 b[3];
            integer8 c[3];

            converter->convert( &a, b, 1 );

            convert_XRGB8888_to_RGB888( &a, c, 1 );

            if ( b[0] != c[0] )
            {
                printf( "failed: (%d,%d,%d) vs. (%d,%d,%d)\n", b[0], b[1], b[2], c[0], c[1], c[2] );
                exit(1);
            }
        }

        converter->end();
    }

    printf( "TrueColor -> BGR888\n" );
    {
        Converter * converter = requestConverter( Format::XRGB8888, Format::BGR888 );

        if ( !converter )
        {
            printf( "invalid converter\n" );
            exit(1);
        }

        converter->begin();

        for ( unsigned int i = 0; i <= 0x00FFFFFF; ++i )
        {
            integer32 a = i;
            integer8 b[3];
            integer8 c[3];

            converter->convert( &a, b, 1 );

            convert_XRGB8888_to_BGR888( &a, c, 1 );

            if ( b[0] != c[0] )
            {
                printf( "failed: (%d,%d,%d) vs. (%d,%d,%d)\n", b[0], b[1], b[2], c[0], c[1], c[2] );
                exit(1);
            }
        }

        converter->end();
    }

    printf( "TrueColor -> RGB565\n" );
    {
        Converter * converter = requestConverter( Format::XRGB8888, Format::RGB565 );

        if ( !converter )
        {
            printf( "invalid converter\n" );
            exit(1);
        }

        converter->begin();

        for ( unsigned int i = 0; i <= 0x00FFFFFF; ++i )
        {
            integer32 a = i;
            integer16 b;
            integer16 c;

            converter->convert( &a, &b, 1 );

            convert_XRGB8888_to_RGB565( &a, (integer16*)&c, 1 );

            if ( b != c )
            {
                printf( "failed: %d vs. %d\n", b, c );
                exit(1);
            }
        }

        converter->end();
    }

    printf( "TrueColor -> BGR565\n");
    {
        Converter * converter = requestConverter( Format::XRGB8888, Format::BGR565 );

        if ( !converter )
        {
            printf( "invalid converter\n" );
            exit(1);
        }

        converter->begin();

        for ( unsigned int i = 0; i <= 0x00FFFFFF; ++i )
        {
            integer32 a = i;
            integer16 b;
            integer16 c;

            converter->convert( &a, &b, 1 );

            convert_XRGB8888_to_BGR565( &a, (integer16*)&c, 1 );

            if ( b != c )
            {
                printf( "failed: %d vs. %d\n", b, c );
                exit(1);
            }
        }

        converter->end();
    }

	printf( "TrueColor -> XRGB1555\n");
    {
        Converter * converter = requestConverter( Format::XRGB8888, Format::XRGB1555 );

        if ( !converter )
        {
            printf( "invalid converter\n" );
            exit(1);
        }

        converter->begin();

        for ( unsigned int i = 0; i <= 0x00FFFFFF; ++i )
        {
            integer32 a = i;
            integer16 b;
            integer16 c;

            converter->convert( &a, &b, 1 );

            convert_XRGB8888_to_XRGB1555( &a, (integer16*)&c, 1 );

            if ( b != c )
            {
                printf( "failed: %d vs. %d\n", b, c );
                exit(1);
            }
        }

        converter->end();
    }

    printf( "TrueColor -> XBGR1555\n" );
    {
        Converter * converter = requestConverter( Format::XRGB8888, Format::XBGR1555 );

        if ( !converter )
        {
            printf( "invalid converter\n" );
            exit(1);
        }

        converter->begin();

        for ( unsigned int i = 0; i <= 0x00FFFFFF; ++i )
        {
            integer32 a = i;
            integer16 b;
            integer16 c;

            converter->convert( &a, &b, 1 );

            convert_XRGB8888_to_XBGR1555( &a, (integer16*)&c, 1 );

            if ( b != c )
            {
                printf( "failed: %d vs. %d\n", b, c );
                exit(1);
            }
        }

        converter->end();
    }

	printf( "\n" );

    printf( "FloatingPointColor -> FloatingPointColor\n");
    {
        Converter * converter = requestConverter( Format::XBGRFFFF, Format::XBGRFFFF );

        if ( !converter )
        {
            printf( "invalid converter\n" );
            exit(1);
        }

        converter->begin();

        for ( unsigned int i = 0; i <= 0x00FFFFFF; ++i )
        {
            Pixel a;
            Pixel b;
            Pixel c;

            convert_XRGB8888_to_XBGRFFFF( &i, &a, 1 );

            converter->convert(&a, &b, 1);

            convert_XBGRFFFF_to_XBGRFFFF( &a, &c, 1 );

            if ( b.r != c.r || b.g != c.g || b.b != c.b)
            {
                printf( "failed: (%f,%f,%f) vs. (%f,%f,%f)\n", b.r, b.g, b.b, c.r, c.g, c.b );
                exit(1);
            }
        }

        converter->end();
    }

    printf( "FloatingPointColor -> TrueColor\n");
    {
        Converter * converter = requestConverter( Format::XBGRFFFF, Format::XRGB8888);

        if ( !converter )
        {
            printf( "invalid converter\n" );
            exit(1);
        }

        converter->begin();

        for ( unsigned int i = 0; i <= 0x00FFFFFF; ++i )
        {
            Pixel a;
            integer32 b;
            integer32 c;

            convert_XRGB8888_to_XBGRFFFF( &i, &a, 1 );

            converter->convert( &a, &b, 1 );

            convert_XBGRFFFF_to_XRGB8888( &a, &c, 1 );

            if ( b != c )
            {
                printf( "failed: %d vs. %d\n", b, c );
                exit(1);
            }
        }

        converter->end();
    }

    printf( "FloatingPointColor -> XBGR8888\n" );
    {
        Converter * converter = requestConverter( Format::XBGRFFFF, Format::XBGR8888 );

        if ( !converter )
        {
            printf( "invalid converter\n" );
            exit(1);
        }

        converter->begin();

        for ( unsigned int i = 0; i <= 0x00FFFFFF; ++i )
        {
            Pixel a;
            integer32 b;
            integer32 c;

            convert_XRGB8888_to_XBGRFFFF( &i, &a, 1 );

            converter->convert(&a, &b, 1);

            convert_XBGRFFFF_to_XBGR8888( &a, &c, 1 );

            if (b!=c)
            {
                printf( "failed: %d vs. %d\n", b, c );
                exit(1);
            }
        }

        converter->end();
    }

    printf( "FloatingPointColor -> RGB888\n");
    {
        Converter * converter = requestConverter( Format::XBGRFFFF, Format::RGB888 );

        if ( !converter )
        {
            printf( "invalid converter\n" );
            exit(1);
        }

        converter->begin();

        for ( unsigned int i = 0; i <= 0x00FFFFFF; ++i )
        {
            Pixel a;
            integer8 b[3];
            integer8 c[3];

            converter->convert( &a, b, 1 );

            convert_XBGRFFFF_to_RGB888( &a, c, 1 );

            if ( b[0] != c[0] )
            {
                printf( "failed: (%d,%d,%d) vs. (%d,%d,%d)\n", b[0], b[1], b[2], c[0], c[1], c[2] );
                exit(1);
            }
        }

        converter->end();
    }

    printf( "FloatingPointColor -> BGR888\n" );
    {
        Converter * converter = requestConverter( Format::XBGRFFFF, Format::BGR888 );

        if ( !converter )
        {
            printf( "invalid converter\n" );
            exit(1);
        }

        converter->begin();

        for ( unsigned int i = 0; i <= 0x00FFFFFF; ++i )
        {
            Pixel a;
            integer8 b[3];
            integer8 c[3];

            converter->convert( &a, b, 1 );

            convert_XBGRFFFF_to_BGR888( &a, c, 1 );

            if ( b[0] != c[0] )
            {
                printf( "failed: (%d,%d,%d) vs. (%d,%d,%d)\n", b[0], b[1], b[2], c[0], c[1], c[2] );
                exit(1);
            }
        }
        
        converter->end();
    }

    printf( "FloatingPointColor -> RGB565\n" );
    {
        Converter * converter = requestConverter( Format::XBGRFFFF, Format::RGB565 );

        if ( !converter )
        {
            printf( "invalid converter\n" );
            exit(1);
        }

        converter->begin();

        for ( unsigned int i = 0; i <= 0x00FFFFFF; ++i )
        {
            Pixel a;
            integer16 b;
            integer16 c;

            convert_XRGB8888_to_XBGRFFFF( &i, &a, 1 );

            converter->convert( &a, (integer16*)&b, 1 );

            convert_XBGRFFFF_to_RGB565( &a, (integer16*)&c, 1 );

            if ( b != c )
            {
                printf( "failed: %d vs. %d\n", b, c );
                exit(1);
            }
        }

        converter->end();
    }

    printf( "FloatingPointColor -> BGR565\n" );
    {
        Converter * converter = requestConverter( Format::XBGRFFFF, Format::BGR565);

        if ( !converter )
        {
            printf( "invalid converter\n" );
            exit(1);
        }

        converter->begin();

        for ( unsigned int i = 0; i <= 0x00FFFFFF; ++i )
        {
            Pixel a;
            integer16 b;
            integer16 c;

            convert_XRGB8888_to_XBGRFFFF( &i, &a, 1 );

            converter->convert( &a, (integer16*)&b, 1 );

            convert_XBGRFFFF_to_BGR565( &a, (integer16*)&c, 1 );

            if ( b != c )
            {
                printf( "failed: %d vs. %d\n", b, c );
                exit(1);
            }
        }

        converter->end();
    }

    printf( "FloatingPointColor -> XRGB1555\n" );
    {
        Converter * converter = requestConverter( Format::XBGRFFFF, Format::XRGB1555 );

        if ( !converter )
        {
            printf( "invalid converter\n" );
            exit(1);
        }

        converter->begin();

        for ( unsigned int i = 0; i <= 0x00FFFFFF; ++i )
        {
            Pixel a;
            integer16 b;
            integer16 c;

            convert_XRGB8888_to_XBGRFFFF( &i, &a, 1 );

            converter->convert( &a, (integer16*)&b, 1 );

            convert_XBGRFFFF_to_XRGB1555( &a, (integer16*)&c, 1 );

            if ( b != c )
            {
                printf( "failed: %d vs. %d\n", b, c );
                exit(1);
            }
        }

        converter->end();
    }

    printf( "FloatingPointColor -> XBGR1555\n" );
    {
        Converter * converter = requestConverter( Format::XBGRFFFF, Format::XBGR1555 );

        if ( !converter )
        {
            printf( "invalid converter\n" );
            exit(1);
        }

        converter->begin();

        for ( unsigned int i = 0; i <= 0x00FFFFFF; ++i )
        {
            Pixel a;
            integer16 b;
            integer16 c;

            convert_XRGB8888_to_XBGRFFFF( &i, &a, 1 );

            converter->convert( &a, (integer16*)&b, 1 );
            convert_XBGRFFFF_to_XBGR1555( &a, (integer16*)&c, 1 );

            if ( b != c )
            {
                printf( "failed: %d vs. %d\n", b, c );
                exit(1);
            }
        }

        converter->end();
    }

    printf( "\npassed.\n\n" );
}
