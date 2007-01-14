// Conversion Unit Test
// Copyright © 2004-2007 Glenn Fiedler
// Part of the PixelToaster Framebuffer Library - http://www.pixeltoaster.com

#include <cstdio>
#include <cstdlib>
#include <Conversion.h>

using namespace PixelToaster;

int main()
{
    printf( "\ntesting conversion routines:\n\n" );

    // ---------------------------------------------------------

    printf( "TrueColor -> TrueColor\n" );
    {
        // verify identity

        printf( "  checking identity...\n" );

        for ( unsigned int i = 0; i <= 0x00FFFFFF; ++i )
        {
            integer32 a = i;
            integer32 b;

            convert_XRGB8888_to_XRGB8888(&a, &b, 1);

            if (a!=b)
            {
                printf("  failed: %d -> %d\n", a, b);
                exit(1);
            }
        }

        printf("  passed.\n\n");
    }

    printf( "FloatingPointColor -> FloatingPointColor\n");
    {
        // verify identity

        printf("  checking identity...\n");

        for (unsigned int i=0; i<=0x00FFFFFF; i++)
        {
            integer32 a = i;
            Pixel b;
            Pixel c;

            convert_XRGB8888_to_XBGRFFFF(&a, &b, 1);
            convert_XBGRFFFF_to_XBGRFFFF(&b, &c, 1);

            if (b.r!=c.r || b.g!=c.g || b.b!=c.b)
            {
                printf("  failed: %d -> (%f,%f,%f) -> (%f,%f,%f)\n", a, b.r, b.g, b.b, c.r, c.g, c.b);
                exit(1);
            }
        }

        printf("  passed.\n\n");
    }

    // ---------------------------------------------------------
       
    printf( "TrueColor -> XBGR1555\n" );
    {
        // verify one-to-one

        printf("  checking one-to-one...\n");

        for (unsigned int i=0; i<=0x00007FFF; i++)
        {
            integer16 a = i;
            integer32 b;
            integer16 c;

            convert_XBGR1555_to_XRGB8888(&a, &b, 1);
            convert_XRGB8888_to_XBGR1555(&b, &c, 1);

            if (a!=c)
            {
                printf("  failed: %d -> %d -> %d\n", a, b, c);
                exit(1);
            }
        }

        // verify black

        printf("  checking black\n");

        integer16 input = 0;
        integer32 output;

        convert_XBGR1555_to_XRGB8888(&input, &output, 1);

        if (output!=0)
        {
            printf("  black test failed: %d\n", output);
            exit(1);
        }

        // verify maximum red

        printf("  checking maximum red\n");

        input = 0x0000001F;

        convert_XBGR1555_to_XRGB8888(&input, &output, 1);

        if (output!=0x00F80000)     // note slightly reduced dynamic range
        {
            printf("  red test failed: %d\n", output);
            exit(1);
        }

        // verify maximum green

        printf("  checking maximum green\n");

        input = 0x000003E0;         

        convert_XBGR1555_to_XRGB8888(&input, &output, 1);

        if (output!=0x0000F800)
        {
            printf("  green test failed: %d\n", output);
            exit(1);
        }

        // verify maximum blue

        printf("  checking maximum blue\n");

        input = 0x7C00;

        convert_XBGR1555_to_XRGB8888(&input, &output, 1);

        if (output!=0x000000F8)     
        {
            printf("  blue test failed: %d\n", output);
            exit(1);
        }

        // verify red channel

        printf("  checking red channel\n");
        {
            const int steps = 32;

            int distinctValues = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<steps; i++)
            {
                input = i;

                convert_XBGR1555_to_XRGB8888(&input, &output, 1);

                if (output&0x0000FFFF)
                {
                    printf("  red channel polluted other channels: %d -> %d\n", input, output);
                    exit(1);
                }

                if (output!=previousDistinctValue)
                {
                    if (output<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  red channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, output);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = output;
                }
            }

            if (distinctValues!=32)
            {
                printf("  red channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify green channel

        printf("  checking green channel\n");
        {
            const int steps = 32;

            int distinctValues = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<steps; i++)
            {
                input = i << 5;

                convert_XBGR1555_to_XRGB8888(&input, &output, 1);

                if (output&0x00FF00FF)
                {
                    printf("  green channel polluted other channels: %d -> %d\n", input, output);
                    exit(1);
                }

                if (output!=previousDistinctValue)
                {
                    if (output<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  green channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, output);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = output;
                }
            }

            if (distinctValues!=32)
            {
                printf("  green channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify blue channel

        printf("  checking blue channel\n");
        {
            const int steps = 32;

            int distinctValues = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<steps; i++)
            {
                input = i << 10;

                convert_XBGR1555_to_XRGB8888(&input, &output, 1);

                if (output&0x00FFFF00)
                {
                    printf("  blue channel polluted other channels: %d -> %d\n", input, output);
                    exit(1);
                }

                if (output!=previousDistinctValue)
                {
                    if (output<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  blue channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, output);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = output;
                }
            }

            if (distinctValues!=32)
            {
                printf("  blue channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        printf("  passed.\n\n");
    }

    // ---------------------------------------------------------

    printf( "TrueColor -> XRGB1555\n" );
    {
        // verify one-to-one

        printf("  checking one-to-one...\n");

        for (unsigned int i=0; i<=0x00007FFF; i++)
        {
            integer16 a = i;
            integer32 b;
            integer16 c;

            convert_XRGB1555_to_XRGB8888(&a, &b, 1);
            convert_XRGB8888_to_XRGB1555(&b, &c, 1);

            if (a!=c)
            {
                printf("  failed: %d -> %d -> %d\n", a, b, c);
                exit(1);
            }
        }

        // verify black

        printf("  checking black\n");

        integer16 input = 0;
        integer32 output;

        convert_XRGB1555_to_XRGB8888(&input, &output, 1);

        if (output!=0)
        {
            printf("  black test failed: %d\n", output);
            exit(1);
        }

        // verify maximum red

        printf("  checking maximum red\n");

        input = 0x7C00;

        convert_XRGB1555_to_XRGB8888(&input, &output, 1);

        if (output!=0x00F80000)     // note slightly reduced dynamic range
        {
            printf("  red test failed: %d\n", output);
            exit(1);
        }

        // verify maximum green

        printf("  checking maximum green\n");

        input = 0x000003E0;         

        convert_XRGB1555_to_XRGB8888(&input, &output, 1);

        if (output!=0x0000F800)
        {
            printf("  green test failed: %d\n", output);
            exit(1);
        }

        // verify maximum blue

        printf("  checking maximum blue\n");

        input = 0x0000001F;

        convert_XRGB1555_to_XRGB8888(&input, &output, 1);

        if (output!=0x000000F8)     
        {
            printf("  blue test failed: %d\n", output);
            exit(1);
        }

        // verify red channel

        printf("  checking red channel\n");
        {
            const int steps = 32;

            int distinctValues = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<steps; i++)
            {
                input = i << 10;

                convert_XRGB1555_to_XRGB8888(&input, &output, 1);

                if (output&0x0000FFFF)
                {
                    printf("  red channel polluted other channels: %d -> %d\n", input, output);
                    exit(1);
                }

                if (output!=previousDistinctValue)
                {
                    if (output<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  red channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, output);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = output;
                }
            }

            if (distinctValues!=32)
            {
                printf("  red channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify green channel

        printf("  checking green channel\n");
        {
            const int steps = 32;

            int distinctValues = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<steps; i++)
            {
                input = i << 5;

                convert_XRGB1555_to_XRGB8888(&input, &output, 1);

                if (output&0x00FF00FF)
                {
                    printf("  green channel polluted other channels: %d -> %d\n", input, output);
                    exit(1);
                }

                if (output!=previousDistinctValue)
                {
                    if (output<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  green channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, output);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = output;
                }
            }

            if (distinctValues!=32)
            {
                printf("  green channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify blue channel

        printf("  checking blue channel\n");
        {
            const int steps = 32;

            int distinctValues = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<steps; i++)
            {
                input = i;

                convert_XRGB1555_to_XRGB8888(&input, &output, 1);

                if (output&0x00FFFF00)
                {
                    printf("  blue channel polluted other channels: %d -> %d\n", input, output);
                    exit(1);
                }

                if (output!=previousDistinctValue)
                {
                    if (output<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  blue channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, output);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = output;
                }
            }

            if (distinctValues!=32)
            {
                printf("  blue channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        printf("  passed.\n\n");
    }

    // ---------------------------------------------------------

    printf( "TrueColor -> BGR565\n" );
    {
        // verify one-to-one

        printf("  checking one-to-one...\n");

        for (unsigned int i=0; i<=0x0000FFFF; i++)
        {
            integer16 a = i;
            integer32 b;
            integer16 c;

            convert_BGR565_to_XRGB8888(&a, &b, 1);
            convert_XRGB8888_to_BGR565(&b, &c, 1);

            if (a!=c)
            {
                printf("  failed: %d -> %d -> %d\n", a, b, c);
                exit(1);
            }
        }

        // verify black

        printf("  checking black\n");

        integer16 input = 0;
        integer32 output;

        convert_BGR565_to_XRGB8888(&input, &output, 1);

        if (output!=0)
        {
            printf("  black test failed: %d\n", output);
            exit(1);
        }

        // verify maximum red

        printf("  checking maximum red\n");

        input = 0x0000001F;

        convert_BGR565_to_XRGB8888(&input, &output, 1);

        if (output!=0x00F80000)     // note slightly reduced dynamic range
        {
            printf("  red test failed: %d\n", output);
            exit(1);
        }

        // verify maximum green

        printf("  checking maximum green\n");

        input = 0x000007E0;         

        convert_BGR565_to_XRGB8888(&input, &output, 1);

        if (output!=0x0000FC00)
        {
            printf("  green test failed: %d\n", output);
            exit(1);
        }

        // verify maximum blue

        printf("  checking maximum blue\n");

        input = 0xF800;

        convert_BGR565_to_XRGB8888(&input, &output, 1);

        if (output!=0x000000F8)    
        {
            printf("  blue test failed: %d\n", output);
            exit(1);
        }

        // verify red channel

        printf("  checking red channel\n");
        {
            const int steps = 32;

            int distinctValues = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<steps; i++)
            {
                input = i;

                convert_BGR565_to_XRGB8888(&input, &output, 1);

                if (output&0x0000FFFF)
                {
                    printf("  red channel polluted other channels: %d -> %d\n", input, output);
                    exit(1);
                }

                if (output!=previousDistinctValue)
                {
                    if (output<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  red channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, output);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = output;
                }
            }

            if (distinctValues!=32)
            {
                printf("  red channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify green channel

        printf("  checking green channel\n");
        {
            const int steps = 64;

            int distinctValues = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<steps; i++)
            {
                input = i << 5;

                convert_BGR565_to_XRGB8888(&input, &output, 1);

                if (output&0x00FF00FF)
                {
                    printf("  green channel polluted other channels: %d -> %d\n", input, output);
                    exit(1);
                }

                if (output!=previousDistinctValue)
                {
                    if (output<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  green channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, output);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = output;
                }
            }

            if (distinctValues!=64)
            {
                printf("  green channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify blue channel

        printf("  checking blue channel\n");
        {
            const int steps = 32;

            int distinctValues = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<steps; i++)
            {
                input = i << 11;

                convert_BGR565_to_XRGB8888(&input, &output, 1);

                if (output&0x00FFFF00)
                {
                    printf("  blue channel polluted other channels: %d -> %d\n", input, output);
                    exit(1);
                }

                if (output!=previousDistinctValue)
                {
                    if (output<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  blue channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, output);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = output;
                }
            }

            if (distinctValues!=32)
            {
                printf("  blue channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        printf("  passed.\n\n");
    }

    // ---------------------------------------------------------

    printf( "TrueColor -> RGB565\n" );
    {
        // verify one-to-one

        printf("  checking one-to-one...\n");

        for (unsigned int i=0; i<=0x0000FFFF; i++)
        {
            integer16 a = i;
            integer32 b;
            integer16 c;

            convert_RGB565_to_XRGB8888(&a, &b, 1);
            convert_XRGB8888_to_RGB565(&b, &c, 1);

            if (a!=c)
            {
                printf("  failed: %d -> %d -> %d\n", a, b, c);
                exit(1);
            }
        }

        // verify black

        printf("  checking black\n");

        integer16 input = 0;
        integer32 output;

        convert_RGB565_to_XRGB8888(&input, &output, 1);

        if (output!=0)
        {
            printf("  black test failed: %d\n", output);
            exit(1);
        }

        // verify maximum red

        printf("  checking maximum red\n");

        input = 0xF800;

        convert_RGB565_to_XRGB8888(&input, &output, 1);

        if (output!=0x00F80000)     // note slightly reduced dynamic range
        {
            printf("  red test failed: %d\n", output);
            exit(1);
        }

        // verify maximum green

        printf("  checking maximum green\n");

        input = 0x000007E0;         

        convert_RGB565_to_XRGB8888(&input, &output, 1);

        if (output!=0x0000FC00)
        {
            printf("  green test failed: %d\n", output);
            exit(1);
        }

        // verify maximum blue

        printf("  checking maximum blue\n");

        input = 0x0000001F;

        convert_RGB565_to_XRGB8888(&input, &output, 1);

        if (output!=0x000000F8)    
        {
            printf("  blue test failed: %d\n", output);
            exit(1);
        }

        // verify red channel

        printf("  checking red channel\n");
        {
            const int steps = 32;

            int distinctValues = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<steps; i++)
            {
                input = i << 11;

                convert_RGB565_to_XRGB8888(&input, &output, 1);

                if (output&0x0000FFFF)
                {
                    printf("  red channel polluted other channels: %d -> %d\n", input, output);
                    exit(1);
                }

                if (output!=previousDistinctValue)
                {
                    if (output<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  red channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, output);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = output;
                }
            }

            if (distinctValues!=32)
            {
                printf("  red channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify green channel

        printf("  checking green channel\n");
        {
            const int steps = 64;

            int distinctValues = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<steps; i++)
            {
                input = i << 5;

                convert_RGB565_to_XRGB8888(&input, &output, 1);

                if (output&0x00FF00FF)
                {
                    printf("  green channel polluted other channels: %d -> %d\n", input, output);
                    exit(1);
                }

                if (output!=previousDistinctValue)
                {
                    if (output<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  green channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, output);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = output;
                }
            }

            if (distinctValues!=64)
            {
                printf("  green channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify blue channel

        printf("  checking blue channel\n");
        {
            const int steps = 32;

            int distinctValues = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<steps; i++)
            {
                input = i;

                convert_RGB565_to_XRGB8888(&input, &output, 1);

                if (output&0x00FFFF00)
                {
                    printf("  blue channel polluted other channels: %d -> %d\n", input, output);
                    exit(1);
                }

                if (output!=previousDistinctValue)
                {
                    if (output<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  blue channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, output);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = output;
                }
            }

            if (distinctValues!=32)
            {
                printf("  blue channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        printf("  passed.\n\n");
    }

    // ---------------------------------------------------------

    printf( "TrueColor -> BGR888\n" );
    {
        // verify one-to-one

        printf("  checking one-to-one...\n");

        for (unsigned int i=0; i<=0x00FFFFFF; i++)
        {
            integer32 a = i;
            integer32 b = 0;
            integer32 c;

            convert_XRGB8888_to_BGR888(&a, (integer8*)&b, 1);
            convert_BGR888_to_XRGB8888((integer8*)&b, &c, 1);

            if (a!=c)
            {
                printf("  failed: %d -> %d -> %d\n", a, b, c);
                exit(1);
            }
        }

        // verify black

        printf("  checking black\n");

        integer32 input = 0;
        integer32 output = 0;

        integer8 *output_b = (integer8*)&output;
        integer8 *output_g = ((integer8*)&output) + 1;
        integer8 *output_r = ((integer8*)&output) + 2;

        convert_XRGB8888_to_BGR888(&input, (integer8*)&output, 1);

        if (output!=0)
        {
            printf("  black test failed: %d\n", output);
            exit(1);
        }

        // verify maximum red

        printf("  checking maximum red\n");

        input = 0x00FF0000;

        convert_XRGB8888_to_BGR888(&input, (integer8*)&output, 1);

        if (*output_r!=0xFF)
        {
            printf("  red test failed: %d (%d,%d,%d)\n", output, *output_r, *output_g, *output_b);
            exit(1);
        }

        // verify maximum green

        printf("  checking maximum green\n");

        input = 0x0000FF00;

        convert_XRGB8888_to_BGR888(&input, (integer8*)&output, 1);

        if (*output_g!=0xFF)
        {
            printf("  red test failed: %d (%d,%d,%d)\n", output, *output_r, *output_g, *output_b);
            exit(1);
        }

        // verify maximum blue

        printf("  checking maximum blue\n");

        input = 0x000000FF;

        convert_XRGB8888_to_BGR888(&input, (integer8*)&output, 1);

        if (*output_b!=0xFF)
        {
            printf("  red test failed: %d (%d,%d,%d)\n", output, *output_r, *output_g, *output_b);
            exit(1);
        }

        // verify red channel

        printf("  checking red channel\n");
        {
            const int steps = 256;

            int distinctValues = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<steps; i++)
            {
                input = i << 16;

                convert_XRGB8888_to_BGR888(&input, (integer8*)&output, 1);

                if (*output_g || *output_b)
                {
                    printf("  red channel polluted other channels: %d -> (%d,%d,%d)\n", input, *output_r, *output_g, *output_b);
                    exit(1);
                }

                if (*output_r!=previousDistinctValue)
                {
                    if (*output_r<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  red channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, *output_r);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = *output_r;
                }
            }

            if (distinctValues!=256)
            {
                printf("  red channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify green channel

        printf("  checking green channel\n");
        {
            const int steps = 256;

            int distinctValues = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<steps; i++)
            {
                input = i << 8;

                convert_XRGB8888_to_BGR888(&input, (integer8*)&output, 1);

                if (*output_r || *output_b)
                {
                    printf("  green channel polluted other channels: %d -> (%d,%d,%d)\n", input, *output_r, *output_g, *output_b);
                    exit(1);
                }

                if (*output_g!=previousDistinctValue)
                {
                    if (*output_g<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  green channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, *output_g);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = *output_g;
                }
            }

            if (distinctValues!=256)
            {
                printf("  green channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify blue channel

        printf("  checking blue channel\n");
        {
            const int steps = 256;

            int distinctValues = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<steps; i++)
            {
                input = i;

                convert_XRGB8888_to_BGR888(&input, (integer8*)&output, 1);

                if (*output_r || *output_g)
                {
                    printf("  blue channel polluted other channels: %d -> (%d,%d,%d)\n", input, *output_r, *output_g, *output_b);
                    exit(1);
                }

                if (*output_b!=previousDistinctValue)
                {
                    if (*output_b<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  blue channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, *output_b);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = *output_b;
                }
            }

            if (distinctValues!=256)
            {
                printf("  blue channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        printf("  passed.\n\n");
    }

    // ---------------------------------------------------------

    printf( "TrueColor -> RGB888\n" );
    {
        // verify one-to-one

        printf("  checking one-to-one...\n");

        for (unsigned int i=0; i<=0x00FFFFFF; i++)
        {
            integer32 a = i;
            integer32 b = 0;
            integer32 c;

            convert_XRGB8888_to_RGB888(&a, (integer8*)&b, 1);
            convert_RGB888_to_XRGB8888((integer8*)&b, &c, 1);

            if (a!=c)
            {
                printf("  failed: %d -> %d -> %d\n", a, b, c);
                exit(1);
            }
        }

        // verify black

        printf("  checking black\n");

        integer32 input = 0;
        integer32 output = 0;

        integer8 *output_r = (integer8*)&output;
        integer8 *output_g = ((integer8*)&output) + 1;
        integer8 *output_b = ((integer8*)&output) + 2;

        convert_XRGB8888_to_RGB888(&input, (integer8*)&output, 1);

        if (output!=0)
        {
            printf("  black test failed: %d\n", output);
            exit(1);
        }

        // verify maximum red

        printf("  checking maximum red\n");

        input = 0x00FF0000;

        convert_XRGB8888_to_RGB888(&input, (integer8*)&output, 1);

        if (*output_r!=0xFF)
        {
            printf("  red test failed: %d (%d,%d,%d)\n", output, *output_r, *output_g, *output_b);
            exit(1);
        }

        // verify maximum green

        printf("  checking maximum green\n");

        input = 0x0000FF00;

        convert_XRGB8888_to_RGB888(&input, (integer8*)&output, 1);

        if (*output_g!=0xFF)
        {
            printf("  red test failed: %d (%d,%d,%d)\n", output, *output_r, *output_g, *output_b);
            exit(1);
        }

        // verify maximum blue

        printf("  checking maximum blue\n");

        input = 0x000000FF;

        convert_XRGB8888_to_RGB888(&input, (integer8*)&output, 1);

        if (*output_b!=0xFF)
        {
            printf("  red test failed: %d (%d,%d,%d)\n", output, *output_r, *output_g, *output_b);
            exit(1);
        }

        // verify red channel

        printf("  checking red channel\n");
        {
            const int steps = 256;

            int distinctValues = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<steps; i++)
            {
                input = i << 16;

                convert_XRGB8888_to_RGB888(&input, (integer8*)&output, 1);

                if (*output_g || *output_b)
                {
                    printf("  red channel polluted other channels: %d -> (%d,%d,%d)\n", input, *output_r, *output_g, *output_b);
                    exit(1);
                }

                if (*output_r!=previousDistinctValue)
                {
                    if (*output_r<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  red channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, *output_r);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = *output_r;
                }
            }

            if (distinctValues!=256)
            {
                printf("  red channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify green channel

        printf("  checking green channel\n");
        {
            const int steps = 256;

            int distinctValues = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<steps; i++)
            {
                input = i << 8;

                convert_XRGB8888_to_RGB888(&input, (integer8*)&output, 1);

                if (*output_r || *output_b)
                {
                    printf("  green channel polluted other channels: %d -> (%d,%d,%d)\n", input, *output_r, *output_g, *output_b);
                    exit(1);
                }

                if (*output_g!=previousDistinctValue)
                {
                    if (*output_g<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  green channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, *output_g);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = *output_g;
                }
            }

            if (distinctValues!=256)
            {
                printf("  green channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify blue channel

        printf("  checking blue channel\n");
        {
            const int steps = 256;

            int distinctValues = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<steps; i++)
            {
                input = i;

                convert_XRGB8888_to_RGB888(&input, (integer8*)&output, 1);

                if (*output_r || *output_g)
                {
                    printf("  blue channel polluted other channels: %d -> (%d,%d,%d)\n", input, *output_r, *output_g, *output_b);
                    exit(1);
                }

                if (*output_b!=previousDistinctValue)
                {
                    if (*output_b<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  blue channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, *output_b);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = *output_b;
                }
            }

            if (distinctValues!=256)
            {
                printf("  blue channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        printf("  passed.\n\n");
    }

    // ---------------------------------------------------------

    printf( "TrueColor -> XBGR8888\n" );
    {
        // verify one-to-one

        printf("  checking one-to-one...\n");

        for (unsigned int i=0; i<=0x00FFFFFF; i++)
        {
            integer32 a = i;
            integer32 b;
            integer32 c;

            convert_XRGB8888_to_XBGR8888(&a, &b, 1);
            convert_XBGR8888_to_XRGB8888(&b, &c, 1);

            if (a!=c)
            {
                printf("  failed: %d -> %d -> %d\n", a, b, c);
                exit(1);
            }
        }

        // verify black

        printf("  checking black\n");

        integer32 input = 0;
        integer32 output;

        convert_XRGB8888_to_XBGR8888(&input, &output, 1);

        if (output!=0)
        {
            printf("  black test failed: %d\n", output);
            exit(1);
        }

        // verify maximum red

        printf("  checking maximum red\n");

        input = 0x00FF0000;

        convert_XRGB8888_to_XBGR8888(&input, &output, 1);

        if (output!=0x000000FF)
        {
            printf("  red test failed: %d\n", output);
            exit(1);
        }

        // verify maximum green

        printf("  checking maximum green\n");

        input = 0x0000FF00;

        convert_XRGB8888_to_XBGR8888(&input, &output, 1);

        if (output!=0x0000FF00)
        {
            printf("  green test failed: %d\n", output);
            exit(1);
        }

        // verify maximum blue

        printf("  checking maximum blue\n");

        input = 0x000000FF;

        convert_XRGB8888_to_XBGR8888(&input, &output, 1);

        if (output!=0x00FF0000)
        {
            printf("  blue test failed: %d\n", output);
            exit(1);
        }

        // verify red channel

        printf("  checking red channel\n");
        {
            const int steps = 256;

            int distinctValues = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<steps; i++)
            {
                input = i << 16;

                convert_XRGB8888_to_XBGR8888(&input, &output, 1);

                if (output&0x00FFFF00)
                {
                    printf("  red channel polluted other channels: %d -> %d\n", input, output);
                    exit(1);
                }

                if (output!=previousDistinctValue)
                {
                    if (output<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  red channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, output);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = output;
                }
            }

            if (distinctValues!=256)
            {
                printf("  red channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify green channel

        printf("  checking green channel\n");
        {
            const int steps = 256;

            int distinctValues = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<steps; i++)
            {
                input = i << 8;

                convert_XRGB8888_to_XBGR8888(&input, &output, 1);

                if (output&0x00FF00FF)
                {
                    printf("  green channel polluted other channels: %d -> %d\n", input, output);
                    exit(1);
                }

                if (output!=previousDistinctValue)
                {
                    if (output<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  green channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, output);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = output;
                }
            }

            if (distinctValues!=256)
            {
                printf("  green channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify blue channel

        printf("  checking blue channel\n");
        {
            const int steps = 256;

            int distinctValues = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<steps; i++)
            {
                input = i;

                convert_XRGB8888_to_XBGR8888(&input, &output, 1);

                if (output&0x0000FFFF)
                {
                    printf("  blue channel polluted other channels: %d -> %d\n", input, output);
                    exit(1);
                }

                if (output!=previousDistinctValue)
                {
                    if (output<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  blue channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, output);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = output;
                }
            }

            if (distinctValues!=256)
            {
                printf("  blue channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        printf("  passed.\n\n");
    }

    // ---------------------------------------------------------

    printf( "FloatingPointColor -> XRGB8888\n" );
    {
        // verify one-to-one mapping    

        printf("  checking one-to-one...\n");

        for (unsigned int i=0; i<=0x00FFFFFF; i++)
        {
            integer32 a = i;
            Pixel b;
            integer32 c;

            convert_XRGB8888_to_XBGRFFFF(&a, &b, 1);
            convert_XBGRFFFF_to_XRGB8888(&b, &c, 1);

            if (a!=c)
            {
                printf("  failed: %d -> (%f,%f,%f) -> %d\n", a, b.r, b.g, b.b, c);
                exit(1);
            }
        }

        // verify black

        printf("  checking black\n");

        Pixel pixel;
        pixel.r = 0;
        pixel.g = 0;
        pixel.b = 0;

        integer32 value;
        convert_XBGRFFFF_to_XRGB8888(&pixel, &value, 1);
        
        if (value!=0)
        {
            printf("  black test failed: %d\n", value);
            exit(1);
        }

        // verify maximum red

        printf("  checking maximum red\n");

        pixel.r = 1.0f;
        pixel.g = 0;
        pixel.b = 0;

        convert_XBGRFFFF_to_XRGB8888(&pixel, &value, 1);

        if (value!=0x00FF0000)
        {
            printf("  red test failed: %d\n", value);
            exit(1);
        }

        // verify maximum green

        printf("  checking maximum green\n");

        pixel.r = 0;
        pixel.g = 1.0f;
        pixel.b = 0;

        convert_XBGRFFFF_to_XRGB8888(&pixel, &value, 1);

        if (value!=0x0000FF00)
        {
            printf("  green test failed: %d\n", value);
            exit(1);
        }

        // verify maximum blue

        printf("  checking maximum blue\n");

        pixel.r = 0;
        pixel.g = 0;
        pixel.b = 1.0f;

        convert_XBGRFFFF_to_XRGB8888(&pixel, &value, 1);

        if (value!=0x000000FF)
        {
            printf("  blue test failed: %d\n", value);
            exit(1);
        }

        // verify red channel

        printf("  checking red channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.r = (float) i / (float) steps * 10 - 5;

                convert_XBGRFFFF_to_XRGB8888(&pixel, &value, 1);

                if (value&0x0000FFFF)
                {
                    printf("  red channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  red channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=256)
            {
                printf("  red channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify green channel

        printf("  checking green channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.g = (float) i / (float) steps * 10 - 5;

                convert_XBGRFFFF_to_XRGB8888(&pixel, &value, 1);

                if (value&0x00FF00FF)
                {
                    printf("  green channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  green channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=256)
            {
                printf("  green channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify blue channel

        printf("  checking blue channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.b = (float) i / (float) steps * 10 - 5;

                convert_XBGRFFFF_to_XRGB8888(&pixel, &value, 1);

                if (value&0x00FFFF00)
                {
                    printf("  blue channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  blue channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=256)
            {
                printf("  blue channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        printf("  passed.\n\n");
    }

    // ---------------------------------------------------------

    printf( "FloatingPointColor -> XBGR8888\n" );
    {
        // verify one-to-one mapping    

        printf("  checking one-to-one...\n");

        for (unsigned int i=0; i<=0x00FFFFFF; i++)
        {
            integer32 a = i;
            Pixel b;
            integer32 c;

            convert_XBGR8888_to_XBGRFFFF(&a, &b, 1);
            convert_XBGRFFFF_to_XBGR8888(&b, &c, 1);

            if (a!=c)
            {
                printf("  failed: %d -> (%f,%f,%f) -> %d\n", a, b.r, b.g, b.b, c);
                exit(1);
            }
        }

        // verify black

        printf("  checking black\n");

        Pixel pixel;
        pixel.r = 0;
        pixel.g = 0;
        pixel.b = 0;

        integer32 value;
        convert_XBGRFFFF_to_XBGR8888(&pixel, &value, 1);

        if (value!=0)
        {
            printf("  black test failed: %d\n", value);
            exit(1);
        }

        // verify maximum red

        printf("  checking maximum red\n");

        pixel.r = 1.0f;
        pixel.g = 0;
        pixel.b = 0;

        convert_XBGRFFFF_to_XBGR8888(&pixel, &value, 1);

        if (value!=0x000000FF)
        {
            printf("  red test failed: %d\n", value);
            exit(1);
        }

        // verify maximum green

        printf("  checking maximum green\n");

        pixel.r = 0;
        pixel.g = 1.0f;
        pixel.b = 0;

        convert_XBGRFFFF_to_XBGR8888(&pixel, &value, 1);

        if (value!=0x0000FF00)
        {
            printf("  green test failed: %d\n", value);
            exit(1);
        }

        // verify maximum blue

        printf("  checking maximum blue\n");

        pixel.r = 0;
        pixel.g = 0;
        pixel.b = 1.0f;

        convert_XBGRFFFF_to_XBGR8888(&pixel, &value, 1);

        if (value!=0x00FF0000)
        {
            printf("  blue test failed: %d\n", value);
            exit(1);
        }

        // verify red channel

        printf("  checking red channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.r = (float) i / (float) steps * 10 - 5;

                convert_XBGRFFFF_to_XBGR8888(&pixel, &value, 1);

                if (value&0x00FFFF00)
                {
                    printf("  red channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  red channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=256)
            {
                printf("  red channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify green channel

        printf("  checking green channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.g = (float) i / (float) steps * 10 - 5;

                convert_XBGRFFFF_to_XBGR8888(&pixel, &value, 1);

                if (value&0x00FF00FF)
                {
                    printf("  green channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  green channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=256)
            {
                printf("  green channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify blue channel

        printf("  checking blue channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.b = (float) i / (float) steps * 10 - 5;

                convert_XBGRFFFF_to_XBGR8888(&pixel, &value, 1);

                if (value&0x0000FFFF)
                {
                    printf("  blue channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  blue channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=256)
            {
                printf("  blue channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        printf("  passed.\n\n");
    }

    // ---------------------------------------------------------

    printf( "FloatingPointColor -> RGB888\n" );
    {
        // verify one-to-one mapping    

        printf("  checking one-to-one...\n");

        for (unsigned int i=0; i<=0x00FFFFFF; i++)
        {
            integer32 r = (i&0x00FF0000)>>16;
            integer32 g = (i&0x0000FF00)>>8;
            integer32 b = (i&0x000000FF);

            integer32 a = 0;

            char *bytes = (char*)&a;

            bytes[0] = r;
            bytes[1] = g;
            bytes[2] = b;

            Pixel pixel;
            integer32 c = 0;

            convert_RGB888_to_XBGRFFFF((integer8*)&a, &pixel, 1);
            convert_XBGRFFFF_to_RGB888(&pixel, (integer8*)&c, 1);

            if (a!=c)
            {
                printf("  failed: %d -> (%f,%f,%f) -> %d\n", a, pixel.r, pixel.g, pixel.b, c);
                exit(1);
            }
        }

        // verify black

        printf("  checking black\n");

        Pixel pixel;
        pixel.r = 0;
        pixel.g = 0;
        pixel.b = 0;

        integer32 value = 0;
        convert_XBGRFFFF_to_RGB888(&pixel, (integer8*)&value, 1);

        if (value!=0)
        {
            printf("  black test failed: %d\n", value);
            exit(1);
        }

        // verify maximum red

        printf("  checking maximum red\n");

        pixel.r = 1.0f;
        pixel.g = 0;
        pixel.b = 0;

        convert_XBGRFFFF_to_RGB888(&pixel, (integer8*)&value, 1);

        integer8 *r = (integer8*)&value;
        integer8 *g = r + 1;
        integer8 *b = g + 1;

        if (*r!=0xFF || *g!=0 || *b!=0)
        {
            printf("  red test failed: %d\n", value);
            exit(1);
        }

        // verify maximum green

        printf("  checking maximum green\n");

        pixel.r = 0;
        pixel.g = 1.0f;
        pixel.b = 0;

        convert_XBGRFFFF_to_RGB888(&pixel, (integer8*)&value, 1);

        if (*g!=0xFF || *r!=0 || *b!=0)
        {
            printf("  green test failed: %d\n", value);
            exit(1);
        }

        // verify maximum blue

        printf("  checking maximum blue\n");

        pixel.r = 0;
        pixel.g = 0;
        pixel.b = 1.0f;

        convert_XBGRFFFF_to_RGB888(&pixel, (integer8*)&value, 1);

        if (*b!=0xFF || *r!=0 || *g!=0)
        {
            printf("  blue test failed: %d\n", value);
            exit(1);
        }

        // verify red channel

        printf("  checking red channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.r = (float) i / (float) steps * 10 - 5;

                value = 0;

                convert_XBGRFFFF_to_RGB888(&pixel, (integer8*)&value, 1);

                if (*g!=0 || *b!=0)
                {
                    printf("  red channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  red channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=256)
            {
                printf("  red channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify green channel

        printf("  checking green channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.g = (float) i / (float) steps * 10 - 5;

                value = 0;

                convert_XBGRFFFF_to_RGB888(&pixel, (integer8*)&value, 1);

                if (*r!=0 || *b!=0)
                {
                    printf("  green channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  green channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=256)
            {
                printf("  green channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify blue channel

        printf("  checking blue channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.b = (float) i / (float) steps * 10 - 5;

                value = 0;

                convert_XBGRFFFF_to_RGB888(&pixel, (integer8*)&value, 1);

                if (*r!=0 || *g!=0)
                {
                    printf("  blue channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  blue channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=256)
            {
                printf("  blue channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        printf("  passed.\n\n");
    }

    // ---------------------------------------------------------

    printf( "FloatingPointColor -> BGR888\n" );
    {
        // verify one-to-one mapping    

        printf("  checking one-to-one...\n");

        for (unsigned int i=0; i<=0x00FFFFFF; i++)
        {
            integer32 r = (i&0x00FF0000)>>16;
            integer32 g = (i&0x0000FF00)>>8;
            integer32 b = (i&0x000000FF);

            integer32 a = 0;

            char *bytes = (char*)&a;

            bytes[0] = b;
            bytes[1] = g;
            bytes[2] = r;

            Pixel pixel;
            integer32 c = 0;

            convert_BGR888_to_XBGRFFFF((integer8*)&a, &pixel, 1);
            convert_XBGRFFFF_to_BGR888(&pixel, (integer8*)&c, 1);

            if (a!=c)
            {
                printf("  failed: %d -> (%f,%f,%f) -> %d\n", a, pixel.r, pixel.g, pixel.b, c);
                exit(1);
            }
        }

        // verify black

        printf("  checking black\n");

        Pixel pixel;
        pixel.r = 0;
        pixel.g = 0;
        pixel.b = 0;

        integer32 value = 0;
        convert_XBGRFFFF_to_BGR888(&pixel, (integer8*)&value, 1);

        if (value!=0)
        {
            printf("  black test failed: %d\n", value);
            exit(1);
        }

        // verify maximum red

        printf("  checking maximum red\n");

        pixel.r = 1.0f;
        pixel.g = 0;
        pixel.b = 0;

        convert_XBGRFFFF_to_BGR888(&pixel, (integer8*)&value, 1);

        integer8 *b = (integer8*)&value;
        integer8 *g = b + 1;
        integer8 *r = g + 1;

        if (*r!=0xFF || *g!=0 || *b!=0)
        {
            printf("  red test failed: %d\n", value);
            exit(1);
        }

        // verify maximum green

        printf("  checking maximum green\n");

        pixel.r = 0;
        pixel.g = 1.0f;
        pixel.b = 0;

        convert_XBGRFFFF_to_BGR888(&pixel, (integer8*)&value, 1);

        if (*g!=0xFF || *r!=0 || *b!=0)
        {
            printf("  green test failed: %d\n", value);
            exit(1);
        }

        // verify maximum blue

        printf("  checking maximum blue\n");

        pixel.r = 0;
        pixel.g = 0;
        pixel.b = 1.0f;

        convert_XBGRFFFF_to_BGR888(&pixel, (integer8*)&value, 1);

        if (*b!=0xFF || *r!=0 || *g!=0)
        {
            printf("  blue test failed: %d\n", value);
            exit(1);
        }

        // verify red channel

        printf("  checking red channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.r = (float) i / (float) steps * 10 - 5;

                value = 0;

                convert_XBGRFFFF_to_BGR888(&pixel, (integer8*)&value, 1);

                if (*g!=0 || *b!=0)
                {
                    printf("  red channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  red channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=256)
            {
                printf("  red channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify green channel

        printf("  checking green channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.g = (float) i / (float) steps * 10 - 5;

                value = 0;

                convert_XBGRFFFF_to_BGR888(&pixel, (integer8*)&value, 1);

                if (*r!=0 || *b!=0)
                {
                    printf("  green channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  green channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=256)
            {
                printf("  green channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify blue channel

        printf("  checking blue channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.b = (float) i / (float) steps * 10 - 5;

                value = 0;

                convert_XBGRFFFF_to_BGR888(&pixel, (integer8*)&value, 1);

                if (*r!=0 || *g!=0)
                {
                    printf("  blue channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  blue channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=256)
            {
                printf("  blue channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        printf("  passed.\n\n");
    }

    // ---------------------------------------------------------

    printf( "FloatingPointColor -> RGB565\n" );
    {
        // verify one-to-one mapping    

        printf("  checking one-to-one...\n");

        for (unsigned int i=0; i<=0x0000FFFF; i++)
        {
            integer16 a = i;
            Pixel b;
            integer16 c;

            convert_RGB565_to_XBGRFFFF(&a, &b, 1);
            convert_XBGRFFFF_to_RGB565(&b, &c, 1);

            if (a!=c)
            {
                printf("  failed: %d -> (%f,%f,%f) -> %d\n", a, b.r, b.g, b.b, c);
                exit(1);
            }
        }

        // verify black

        printf("  checking black\n");

        Pixel pixel;
        pixel.r = 0;
        pixel.g = 0;
        pixel.b = 0;

        integer16 value;
        convert_XBGRFFFF_to_RGB565(&pixel, &value, 1);

        if (value!=0)
        {
            printf("  black test failed: %d\n", value);
            exit(1);
        }

        // verify maximum red

        printf("  checking maximum red\n");

        pixel.r = 1.0f;
        pixel.g = 0;
        pixel.b = 0;

        convert_XBGRFFFF_to_RGB565(&pixel, &value, 1);

        if (value!=0x0000F800)
        {
            printf("  red test failed: %d\n", value);
            exit(1);
        }

        // verify maximum green

        printf("  checking maximum green\n");

        pixel.r = 0;
        pixel.g = 1.0f;
        pixel.b = 0;

        convert_XBGRFFFF_to_RGB565(&pixel, &value, 1);

        if (value!=0x000007E0)
        {
            printf("  green test failed: %d\n", value);
            exit(1);
        }

        // verify maximum blue

        printf("  checking maximum blue\n");

        pixel.r = 0;
        pixel.g = 0;
        pixel.b = 1.0f;

        convert_XBGRFFFF_to_RGB565(&pixel, &value, 1);

        if (value!=0x0000001F)
        {
            printf("  blue test failed: %d\n", value);
            exit(1);
        }

        // verify red channel

        printf("  checking red channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.r = (float) i / (float) steps * 10 - 5;

                convert_XBGRFFFF_to_RGB565(&pixel, &value, 1);

                if (value&~0x0000F800)
                {
                    printf("  red channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  red channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=32)
            {
                printf("  red channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify green channel

        printf("  checking green channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.g = (float) i / (float) steps * 10 - 5;

                convert_XBGRFFFF_to_RGB565(&pixel, &value, 1);

                if (value&~0x000007E0)
                {
                    printf("  green channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  green channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=64)
            {
                printf("  green channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify blue channel

        printf("  checking blue channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.b = (float) i / (float) steps * 10 - 5;

                convert_XBGRFFFF_to_RGB565(&pixel, &value, 1);

                if (value&~0x0000001F)
                {
                    printf("  blue channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  blue channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=32)
            {
                printf("  blue channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        printf("  passed.\n\n");
    }

    // ---------------------------------------------------------

    printf( "FloatingPointColor -> BGR565\n" );
    {
        // verify one-to-one mapping    

        printf("  checking one-to-one...\n");

        for (unsigned int i=0; i<=0x0000FFFF; i++)
        {
            integer16 a = i;
            Pixel b;
            integer16 c;

            convert_BGR565_to_XBGRFFFF(&a, &b, 1);
            convert_XBGRFFFF_to_BGR565(&b, &c, 1);

            if (a!=c)
            {
                printf("  failed: %d -> (%f,%f,%f) -> %d\n", a, b.r, b.g, b.b, c);
                exit(1);
            }
        }

        // verify black

        printf("  checking black\n");

        Pixel pixel;
        pixel.r = 0;
        pixel.g = 0;
        pixel.b = 0;

        integer16 value;
        convert_XBGRFFFF_to_BGR565(&pixel, &value, 1);

        if (value!=0)
        {
            printf("  black test failed: %d\n", value);
            exit(1);
        }

        // verify maximum red

        printf("  checking maximum red\n");

        pixel.r = 1.0f;
        pixel.g = 0;
        pixel.b = 0;

        convert_XBGRFFFF_to_BGR565(&pixel, &value, 1);

        if (value!=0x0000001F)
        {
            printf("  red test failed: %d\n", value);
            exit(1);
        }

        // verify maximum green

        printf("  checking maximum green\n");

        pixel.r = 0;
        pixel.g = 1.0f;
        pixel.b = 0;

        convert_XBGRFFFF_to_BGR565(&pixel, &value, 1);

        if (value!=0x000007E0)
        {
            printf("  green test failed: %d\n", value);
            exit(1);
        }

        // verify maximum blue

        printf("  checking maximum blue\n");

        pixel.r = 0;
        pixel.g = 0;
        pixel.b = 1.0f;

        convert_XBGRFFFF_to_BGR565(&pixel, &value, 1);

        if (value!=0x0000F800)
        {
            printf("  blue test failed: %d\n", value);
            exit(1);
        }

        // verify red channel

        printf("  checking red channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.r = (float) i / (float) steps * 10 - 5;

                convert_XBGRFFFF_to_BGR565(&pixel, &value, 1);

                if (value&~0x0000001F)
                {
                    printf("  red channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  red channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=32)
            {
                printf("  red channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify green channel

        printf("  checking green channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.g = (float) i / (float) steps * 10 - 5;

                convert_XBGRFFFF_to_BGR565(&pixel, &value, 1);

                if (value&~0x000007E0)
                {
                    printf("  green channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  green channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=64)
            {
                printf("  green channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify blue channel

        printf("  checking blue channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.b = (float) i / (float) steps * 10 - 5;

                convert_XBGRFFFF_to_BGR565(&pixel, &value, 1);

                if (value&~0x0000F800)
                {
                    printf("  blue channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  blue channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=32)
            {
                printf("  blue channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        printf("  passed.\n\n");
    }

    // ---------------------------------------------------------

    printf( "FloatingPointColor -> XRGB1555\n" );
    {
        // verify one-to-one mapping    

        printf("  checking one-to-one...\n");

        for (unsigned int i=0; i<=0x00007FFF; i++)
        {
            integer16 a = i;
            Pixel b;
            integer16 c;

            convert_XRGB1555_to_XBGRFFFF(&a, &b, 1);
            convert_XBGRFFFF_to_XRGB1555(&b, &c, 1);

            if (a!=c)
            {
                printf("  failed: %d -> (%f,%f,%f) -> %d\n", a, b.r, b.g, b.b, c);
                exit(1);
            }
        }

        // verify black

        printf("  checking black\n");

        Pixel pixel;
        pixel.r = 0;
        pixel.g = 0;
        pixel.b = 0;

        integer16 value;
        convert_XBGRFFFF_to_XRGB1555(&pixel, &value, 1);

        if (value!=0)
        {
            printf("  black test failed: %d\n", value);
            exit(1);
        }

        // verify maximum red

        printf("  checking maximum red\n");

        pixel.r = 1.0f;
        pixel.g = 0;
        pixel.b = 0;

        convert_XBGRFFFF_to_XRGB1555(&pixel, &value, 1);

        if (value!=0x00007C00)
        {
            printf("  red test failed: %d\n", value);
            exit(1);
        }

        // verify maximum green

        printf("  checking maximum green\n");

        pixel.r = 0;
        pixel.g = 1.0f;
        pixel.b = 0;

        convert_XBGRFFFF_to_XRGB1555(&pixel, &value, 1);

        if (value!=0x000003E0)
        {
            printf("  green test failed: %d\n", value);
            exit(1);
        }

        // verify maximum blue

        printf("  checking maximum blue\n");

        pixel.r = 0;
        pixel.g = 0;
        pixel.b = 1.0f;

        convert_XBGRFFFF_to_XRGB1555(&pixel, &value, 1);

        if (value!=0x0000001F)
        {
            printf("  blue test failed: %d\n", value);
            exit(1);
        }

        // verify red channel

        printf("  checking red channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.r = (float) i / (float) steps * 10 - 5;

                convert_XBGRFFFF_to_XRGB1555(&pixel, &value, 1);

                if (value&~0x00007C00)
                {
                    printf("  red channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  red channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=32)
            {
                printf("  red channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify green channel

        printf("  checking green channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.g = (float) i / (float) steps * 10 - 5;

                convert_XBGRFFFF_to_XRGB1555(&pixel, &value, 1);

                if (value&~0x000003E0)
                {
                    printf("  green channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  green channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=32)
            {
                printf("  green channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify blue channel

        printf("  checking blue channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.b = (float) i / (float) steps * 10 - 5;

                convert_XBGRFFFF_to_XRGB1555(&pixel, &value, 1);

                if (value&~0x0000001F)
                {
                    printf("  blue channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  blue channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=32)
            {
                printf("  blue channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        printf("  passed.\n\n");
    }
    // ---------------------------------------------------------

    printf( "FloatingPointColor -> XBGR1555\n" );
    {
        // verify one-to-one mapping    

        printf("  checking one-to-one...\n");

        for (unsigned int i=0; i<=0x00007FFF; i++)
        {
            integer16 a = i;
            Pixel b;
            integer16 c;

            convert_XBGR1555_to_XBGRFFFF(&a, &b, 1);
            convert_XBGRFFFF_to_XBGR1555(&b, &c, 1);

            if (a!=c)
            {
                printf("  failed: %d -> (%f,%f,%f) -> %d\n", a, b.r, b.g, b.b, c);
                exit(1);
            }
        }

        // verify black

        printf("  checking black\n");

        Pixel pixel;
        pixel.r = 0;
        pixel.g = 0;
        pixel.b = 0;

        integer16 value;
        convert_XBGRFFFF_to_XBGR1555(&pixel, &value, 1);

        if (value!=0)
        {
            printf("  black test failed: %d\n", value);
            exit(1);
        }

        // verify maximum red

        printf("  checking maximum red\n");

        pixel.r = 1.0f;
        pixel.g = 0;
        pixel.b = 0;

        convert_XBGRFFFF_to_XBGR1555(&pixel, &value, 1);

        if (value!=0x0000001F)
        {
            printf("  red test failed: %d\n", value);
            exit(1);
        }

        // verify maximum green

        printf("  checking maximum green\n");

        pixel.r = 0;
        pixel.g = 1.0f;
        pixel.b = 0;

        convert_XBGRFFFF_to_XBGR1555(&pixel, &value, 1);

        if (value!=0x000003E0)
        {
            printf("  green test failed: %d\n", value);
            exit(1);
        }

        // verify maximum blue

        printf("  checking maximum blue\n");

        pixel.r = 0;
        pixel.g = 0;
        pixel.b = 1.0f;

        convert_XBGRFFFF_to_XBGR1555(&pixel, &value, 1);

        if (value!=0x00007C00)
        {
            printf("  blue test failed: %d\n", value);
            exit(1);
        }

        // verify red channel

        printf("  checking red channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.r = (float) i / (float) steps * 10 - 5;

                convert_XBGRFFFF_to_XBGR1555(&pixel, &value, 1);

                if (value&~0x0000001F)
                {
                    printf("  red channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  red channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=32)
            {
                printf("  red channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify green channel

        printf("  checking green channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.g = (float) i / (float) steps * 10 - 5;

                convert_XBGRFFFF_to_XBGR1555(&pixel, &value, 1);

                if (value&~0x000003E0)
                {
                    printf("  green channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  green channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=32)
            {
                printf("  green channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        // verify blue channel

        printf("  checking blue channel\n");
        {
            const int steps = 10000;

            int distinctValues = 0;

            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;

            integer32 previousDistinctValue = 0xFFFFFFFF;

            for (int i=0; i<=steps; i++)
            {
                pixel.b = (float) i / (float) steps * 10 - 5;

                convert_XBGRFFFF_to_XBGR1555(&pixel, &value, 1);

                if (value&~0x00007C00)
                {
                    printf("  blue channel polluted other channels: (%f,%f,%f) -> %d\n", pixel.r, pixel.g, pixel.b, value);
                    exit(1);
                }

                if (value!=previousDistinctValue)
                {
                    if (value<previousDistinctValue && previousDistinctValue!=0xFFFFFFFF)
                    {
                        printf("  blue channel monotonic non-decreasing check failed: %d->%d\n", previousDistinctValue, value);
                        exit(1);
                    }

                    distinctValues++;
                    previousDistinctValue = value;
                }
            }

            if (distinctValues!=32)
            {
                printf("  blue channel distinct value check failed: %d distinct values\n", distinctValues);
                exit(1);
            }
        }

        printf("  passed.\n\n");
    }
}
