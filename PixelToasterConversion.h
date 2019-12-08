// Pixel Format Conversion Routines
// Copyright © 2004-2007 Glenn Fiedler
// Part of the PixelToaster Framebuffer Library - http://www.pixeltoaster.com

#ifndef PIXELTOASTER_CONVERSION_H
#define PIXELTOASTER_CONVERSION_H

#include "PixelToaster.h"

#ifndef PIXELTOASTER_NO_CRT
#    include <memory.h>
#endif

#ifdef PIXELTOASTER_USE_SSE2
#    include <emmintrin.h>
#endif

namespace PixelToaster {
// floating point tricks!

union FloatInteger
{
    float f;
    int   i;
};

inline integer32 clamp_positive(integer32 value)
{
    return (value - (value & (((int)value) >> 31)));
}

#ifdef PIXELTOASTER_USE_SSE2
inline __m128i clamp_positive(__m128i value)
{
    auto x = _mm_srai_epi32(value, 31);
    return _mm_andnot_si128(x, value);
}
#endif

inline integer32 clamped_fraction_8(float input)
{
    FloatInteger value;

    value.f = input;
    value.i = clamp_positive(value.i);

    if (value.i >= 0x3F7FFFFF)
        return 0x07F8000;

    value.f += 1.0f;

    return value.i & 0x07F8000;
}

#ifdef PIXELTOASTER_USE_SSE2
inline __m128i clamped_fraction_8(__m128 input)
{
    auto maskRet = _mm_set1_epi32(0x07F8000);
    auto onei    = _mm_set1_epi32(0x3F7FFFFF);
    auto onef    = _mm_castsi128_ps(onei);

    auto xi   = clamp_positive(_mm_castps_si128(input));
    auto xf   = _mm_castsi128_ps(xi);
    auto mask = _mm_cmpgt_epi32(xi, onei);

    // Compute either cases simultaneously and merge them later
    auto y0 = _mm_and_si128(mask, onei);
    auto y1 = _mm_andnot_si128(mask, _mm_castps_si128(_mm_add_ps(xf, onef)));
    return _mm_and_si128(_mm_or_si128(y0, y1), maskRet);
}
#endif

inline integer32 clamped_fraction_6(float input)
{
    FloatInteger value;

    value.f = input;
    value.i = clamp_positive(value.i);

    if (value.i >= 0x3F7FFFFF)
        return 0x07E0000;

    value.f += 1.0f;

    return value.i & 0x07E0000;
}

inline integer32 clamped_fraction_5(float input)
{
    FloatInteger value;

    value.f = input;
    value.i = clamp_positive(value.i);

    if (value.i >= 0x3F7FFFFF)
        return 0x07C0000;

    value.f += 1.0f;

    return value.i & 0x07C0000;
}

inline float uint8ToFloat(integer8 input)
{
    FloatInteger value;

    value.i = input | (142L << 23);
    value.f -= 32768.0f;

    return value.f;
}

// floating point conversion routines

inline void convert_XBGRFFFF_to_XRGB8888(const Pixel source[], integer32 destination[], unsigned int count)
{
    unsigned int offset = 0;

#ifdef PIXELTOASTER_USE_SSE2
    integer32          output[16];
    const unsigned int numBlocks = count / 4;

    for (unsigned int i = 0; i < numBlocks; ++i)
    {
        __m128 p0 = _mm_load_ps(reinterpret_cast<const float*>(&source[4 * i + 0]));
        __m128 p1 = _mm_load_ps(reinterpret_cast<const float*>(&source[4 * i + 1]));
        __m128 p2 = _mm_load_ps(reinterpret_cast<const float*>(&source[4 * i + 2]));
        __m128 p3 = _mm_load_ps(reinterpret_cast<const float*>(&source[4 * i + 3]));

        __m128i f0 = clamped_fraction_8(p0);
        __m128i f1 = clamped_fraction_8(p1);
        __m128i f2 = clamped_fraction_8(p2);
        __m128i f3 = clamped_fraction_8(p3);

        _mm_store_si128(reinterpret_cast<__m128i*>(&output[0]), f0);
        _mm_store_si128(reinterpret_cast<__m128i*>(&output[4]), f1);
        _mm_store_si128(reinterpret_cast<__m128i*>(&output[8]), f2);
        _mm_store_si128(reinterpret_cast<__m128i*>(&output[12]), f3);

        const integer32 r0 = output[0] << 1;
        const integer32 g0 = output[1] >> 7;
        const integer32 b0 = output[2] >> 15;

        const integer32 r1 = output[4] << 1;
        const integer32 g1 = output[5] >> 7;
        const integer32 b1 = output[6] >> 15;

        const integer32 r2 = output[8] << 1;
        const integer32 g2 = output[9] >> 7;
        const integer32 b2 = output[10] >> 15;

        const integer32 r3 = output[12] << 1;
        const integer32 g3 = output[13] >> 7;
        const integer32 b3 = output[14] >> 15;

        destination[4 * i + 0] = r0 | g0 | b0;
        destination[4 * i + 1] = r1 | g1 | b1;
        destination[4 * i + 2] = r2 | g2 | b2;
        destination[4 * i + 3] = r3 | g3 | b3;
    }

    offset += 4 * count;
#endif

    for (unsigned int i = offset; i < count; ++i)
    {
        const integer32 r = clamped_fraction_8(source[i].r) << 1;
        const integer32 g = clamped_fraction_8(source[i].g) >> 7;
        const integer32 b = clamped_fraction_8(source[i].b) >> 15;

        destination[i] = r | g | b;
    }
}

inline void convert_XRGB8888_to_XBGRFFFF(const integer32 source[], Pixel destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        destination[i].r = uint8ToFloat(source[i] >> 16);
        destination[i].g = uint8ToFloat(source[i] >> 8);
        destination[i].b = uint8ToFloat(source[i]);
    }
}

inline void convert_XBGRFFFF_to_XBGR8888(const Pixel source[], integer32 destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        const integer32 r = clamped_fraction_8(source[i].r) >> 15;
        const integer32 g = clamped_fraction_8(source[i].g) >> 7;
        const integer32 b = clamped_fraction_8(source[i].b) << 1;

        destination[i] = r | g | b;
    }
}

inline void convert_XBGR8888_to_XBGRFFFF(const integer32 source[], Pixel destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        destination[i].r = uint8ToFloat(source[i]);
        destination[i].g = uint8ToFloat(source[i] >> 8);
        destination[i].b = uint8ToFloat(source[i] >> 16);
    }
}

inline void convert_XBGRFFFF_to_RGB888(const Pixel source[], integer8 destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        const integer32 r = clamped_fraction_8(source[i].r) >> 15;
        const integer32 g = clamped_fraction_8(source[i].g) >> 15;
        const integer32 b = clamped_fraction_8(source[i].b) >> 15;

        destination[0] = (integer8)r;
        destination[1] = (integer8)g;
        destination[2] = (integer8)b;

        destination += 3;
    }
}

inline void convert_RGB888_to_XBGRFFFF(const integer8 source[], Pixel destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        destination[i].r = uint8ToFloat(source[0]);
        destination[i].g = uint8ToFloat(source[1]);
        destination[i].b = uint8ToFloat(source[2]);

        source += 3;
    }
}

inline void convert_XBGRFFFF_to_BGR888(const Pixel source[], integer8 destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        const integer32 r = clamped_fraction_8(source[i].r) >> 15;
        const integer32 g = clamped_fraction_8(source[i].g) >> 15;
        const integer32 b = clamped_fraction_8(source[i].b) >> 15;

        destination[0] = (integer8)b;
        destination[1] = (integer8)g;
        destination[2] = (integer8)r;

        destination += 3;
    }
}

inline void convert_BGR888_to_XBGRFFFF(const integer8 source[], Pixel destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        destination[i].r = uint8ToFloat(source[2]);
        destination[i].g = uint8ToFloat(source[1]);
        destination[i].b = uint8ToFloat(source[0]);

        source += 3;
    }
}

inline void convert_XBGRFFFF_to_RGB565(const Pixel source[], integer16 destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        const integer32 r = clamped_fraction_5(source[i].r) >> 7;
        const integer32 g = clamped_fraction_6(source[i].g) >> 12;
        const integer32 b = clamped_fraction_5(source[i].b) >> 18;

        destination[i] = (integer16)(r | g | b);
    }
}

inline void convert_RGB565_to_XBGRFFFF(const integer16 source[], Pixel destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        const auto color = (integer32)source[i];

        const integer8 r = (color & 0x0000F800) >> 8;
        const integer8 g = (color & 0x000007E0) >> 3;
        const integer8 b = (color & 0x0000001F) << 3;

        destination[i].r = uint8ToFloat(r);
        destination[i].g = uint8ToFloat(g);
        destination[i].b = uint8ToFloat(b);
    }
}

inline void convert_XBGRFFFF_to_BGR565(const Pixel source[], integer16 destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        const integer32 r = clamped_fraction_5(source[i].r) >> 18;
        const integer32 g = clamped_fraction_6(source[i].g) >> 12;
        const integer32 b = clamped_fraction_5(source[i].b) >> 7;

        destination[i] = (integer16)(r | g | b);
    }
}

inline void convert_BGR565_to_XBGRFFFF(const integer16 source[], Pixel destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        const auto color = (integer32)source[i];

        const integer8 b = (color & 0x0000F800) >> 8;
        const integer8 g = (color & 0x000007E0) >> 3;
        const integer8 r = (color & 0x0000001F) << 3;

        destination[i].r = uint8ToFloat(r);
        destination[i].g = uint8ToFloat(g);
        destination[i].b = uint8ToFloat(b);
    }
}

inline void convert_XBGRFFFF_to_XRGB1555(const Pixel source[], integer16 destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        const integer32 r = clamped_fraction_5(source[i].r) >> 8;
        const integer32 g = clamped_fraction_5(source[i].g) >> 13;
        const integer32 b = clamped_fraction_5(source[i].b) >> 18;

        destination[i] = (integer16)(r | g | b);
    }
}

inline void convert_XRGB1555_to_XBGRFFFF(const integer16 source[], Pixel destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        const auto color = (integer32)source[i];

        const integer8 r = (color & 0x00007C00) >> 7;
        const integer8 g = (color & 0x000003E0) >> 2;
        const integer8 b = (color & 0x0000001F) << 3;

        destination[i].r = uint8ToFloat(r);
        destination[i].g = uint8ToFloat(g);
        destination[i].b = uint8ToFloat(b);
    }
}

inline void convert_XBGRFFFF_to_XBGR1555(const Pixel source[], integer16 destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        const integer32 r = clamped_fraction_5(source[i].r) >> 18;
        const integer32 g = clamped_fraction_5(source[i].g) >> 13;
        const integer32 b = clamped_fraction_5(source[i].b) >> 8;

        destination[i] = (integer16)(r | g | b);
    }
}

inline void convert_XBGR1555_to_XBGRFFFF(const integer16 source[], Pixel destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        const auto color = (integer32)source[i];

        const integer8 b = (color & 0x00007C00) >> 7;
        const integer8 g = (color & 0x000003E0) >> 2;
        const integer8 r = (color & 0x0000001F) << 3;

        destination[i].r = uint8ToFloat(r);
        destination[i].g = uint8ToFloat(g);
        destination[i].b = uint8ToFloat(b);
    }
}

// integer to integer converters

inline void convert_XRGB8888_to_XBGR8888(const integer32 source[], integer32 destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        integer32 r    = (source[i] & 0x00FF0000) >> 16;
        integer32 g    = (source[i] & 0x0000FF00);
        integer32 b    = (source[i] & 0x000000FF) << 16;
        destination[i] = r | g | b;
    }
}

inline void convert_XBGR8888_to_XRGB8888(const integer32 source[], integer32 destination[], unsigned int count)
{
    convert_XRGB8888_to_XBGR8888(source, destination, count);
}

inline void convert_XRGB8888_to_RGB888(const integer32 source[], integer8 destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        destination[0] = (integer8)((source[i] & 0x00FF0000) >> 16);
        destination[1] = (integer8)((source[i] & 0x0000FF00) >> 8);
        destination[2] = (integer8)((source[i] & 0x000000FF));
        destination += 3;
    }
}

inline void convert_RGB888_to_XRGB8888(const integer8 source[], integer32 destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        integer32 r = source[0];
        integer32 g = source[1];
        integer32 b = source[2];

        destination[i] = (r << 16) | (g << 8) | b;

        source += 3;
    }
}

inline void convert_XRGB8888_to_BGR888(const integer32 source[], integer8 destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        destination[0] = (integer8)((source[i] & 0x000000FF));
        destination[1] = (integer8)((source[i] & 0x0000FF00) >> 8);
        destination[2] = (integer8)((source[i] & 0x00FF0000) >> 16);
        destination += 3;
    }
}

inline void convert_BGR888_to_XRGB8888(const integer8 source[], integer32 destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        integer32 r = source[0];
        integer32 g = source[1];
        integer32 b = source[2];

        destination[i] = (b << 16) | (g << 8) | r;

        source += 3;
    }
}

inline void convert_XRGB8888_to_RGB565(const integer32 source[], integer16 destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        integer32 r = (source[i] & 0x00F80000) >> 8;
        integer32 g = (source[i] & 0x0000FC00) >> 5;
        integer32 b = (source[i] & 0x000000F8) >> 3;

        destination[i] = (integer16)(r | g | b);
    }
}

inline void convert_RGB565_to_XRGB8888(const integer16 source[], integer32 destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        auto color = (integer32)source[i];

        integer32 r = (color & 0x0000F800) << 8;
        integer32 g = (color & 0x000007E0) << 5;
        integer32 b = (color & 0x0000001F) << 3;

        destination[i] = r | g | b;
    }
}

inline void convert_XRGB8888_to_BGR565(const integer32 source[], integer16 destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        integer32 r = (source[i] & 0x00F80000) >> 19;
        integer32 g = (source[i] & 0x0000FC00) >> 5;
        integer32 b = (source[i] & 0x000000F8) << 8;

        destination[i] = (integer16)(r | g | b);
    }
}

inline void convert_BGR565_to_XRGB8888(const integer16 source[], integer32 destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        auto color = (integer32)source[i];

        integer32 r = ((color & 0x0000F800) << 8) >> 16;
        integer32 g = ((color & 0x000007E0) << 5);
        integer32 b = ((color & 0x0000001F) << 3) << 16;

        destination[i] = r | g | b;
    }
}

inline void convert_XRGB8888_to_XRGB1555(const integer32 source[], integer16 destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        integer32 r = (source[i] & 0x00F80000) >> 9;
        integer32 g = (source[i] & 0x0000F800) >> 6;
        integer32 b = (source[i] & 0x000000F8) >> 3;

        destination[i] = (integer16)(r | g | b);
    }
}

inline void convert_XRGB1555_to_XRGB8888(const integer16 source[], integer32 destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        auto color = (integer32)source[i];

        integer32 r = (color & 0x00007C00) << 9;
        integer32 g = (color & 0x000003E0) << 6;
        integer32 b = (color & 0x0000001F) << 3;

        destination[i] = r | g | b;
    }
}

inline void convert_XRGB8888_to_XBGR1555(const integer32 source[], integer16 destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        integer32 r = (source[i] & 0x00F80000) >> 19;
        integer32 g = (source[i] & 0x0000F800) >> 6;
        integer32 b = (source[i] & 0x000000F8) << 7;

        destination[i] = (integer16)(r | g | b);
    }
}

inline void convert_XBGR1555_to_XRGB8888(const integer16 source[], integer32 destination[], unsigned int count)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        auto color = (integer32)source[i];

        integer32 r = (color & 0x00007C00) >> 7;
        integer32 g = (color & 0x000003E0) << 6;
        integer32 b = (color & 0x0000001F) << 19;

        destination[i] = r | g | b;
    }
}

// copy converters

inline void convert_XRGB8888_to_XRGB8888(const integer32 source[], integer32 destination[], unsigned int count)
{
#ifndef PIXELTOASTER_NO_CRT
    memcpy(destination, source, count * 4);
#else
    for (unsigned int i = 0; i < count; ++i)
        destination[i] = source[i];
#endif
}

inline void convert_XBGRFFFF_to_XBGRFFFF(const Pixel source[], Pixel destination[], unsigned int count)
{
#ifndef PIXELTOASTER_NO_CRT
    memcpy(destination, source, count * 16);
#else
    for (unsigned int i = 0; i < count; ++i)
        destination[i] = source[i];
#endif
}

// declare set of converter classes

class ConverterAdapter : public Converter
{
    virtual void begin() override {}
    virtual void end() override {}
};

#define PIXELTOASTER_CONVERTER(type, source_type, destination_type)                             \
                                                                                                \
    class Converter_##type : public ConverterAdapter                                            \
    {                                                                                           \
        void convert(const void* source, void* destination, int pixels)                         \
        {                                                                                       \
            convert_##type((const source_type*)source, (destination_type*)destination, pixels); \
        }                                                                                       \
    };

PIXELTOASTER_CONVERTER(XBGRFFFF_to_XBGRFFFF, Pixel, Pixel);
PIXELTOASTER_CONVERTER(XBGRFFFF_to_XRGB8888, Pixel, integer32);
PIXELTOASTER_CONVERTER(XBGRFFFF_to_XBGR8888, Pixel, integer32);
PIXELTOASTER_CONVERTER(XBGRFFFF_to_RGB888, Pixel, integer8);
PIXELTOASTER_CONVERTER(XBGRFFFF_to_BGR888, Pixel, integer8);
PIXELTOASTER_CONVERTER(XBGRFFFF_to_RGB565, Pixel, integer16);
PIXELTOASTER_CONVERTER(XBGRFFFF_to_BGR565, Pixel, integer16);
PIXELTOASTER_CONVERTER(XBGRFFFF_to_XRGB1555, Pixel, integer16);
PIXELTOASTER_CONVERTER(XBGRFFFF_to_XBGR1555, Pixel, integer16);

PIXELTOASTER_CONVERTER(XRGB8888_to_XBGRFFFF, integer32, Pixel);
PIXELTOASTER_CONVERTER(XRGB8888_to_XRGB8888, integer32, integer32);
PIXELTOASTER_CONVERTER(XRGB8888_to_XBGR8888, integer32, integer32);
PIXELTOASTER_CONVERTER(XRGB8888_to_RGB888, integer32, integer8);
PIXELTOASTER_CONVERTER(XRGB8888_to_BGR888, integer32, integer8);
PIXELTOASTER_CONVERTER(XRGB8888_to_RGB565, integer32, integer16);
PIXELTOASTER_CONVERTER(XRGB8888_to_BGR565, integer32, integer16);
PIXELTOASTER_CONVERTER(XRGB8888_to_XRGB1555, integer32, integer16);
PIXELTOASTER_CONVERTER(XRGB8888_to_XBGR1555, integer32, integer16);

#undef PIXELTOASTER_CONVERTER
} // namespace PixelToaster

#endif
