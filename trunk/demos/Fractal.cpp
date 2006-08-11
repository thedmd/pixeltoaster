// Fractal Demo
// Copyright © Glenn Fiedler
// http://www.pixeltoaster.com

#include <cmath>
#include <PixelToaster.h>

using namespace PixelToaster;


#define HIGH_QUALITY


struct Complex
{
	float x,y;
};


inline void colorize( Pixel & pixel, float value )
{
	if ( value < 33.33f )
	{
		pixel.r = value * 0.015f;
		pixel.g = 0.0f;
	}
	else
	{
		const float base = value - 33.33f;
		pixel.r = 0.5f + base * 0.0075f;
		pixel.g = base * 0.002625f;
	}
}


#ifdef HIGH_QUALITY

inline float mandelbrot(float x, float y)
{
	Complex z,c,s;
	
	z.x = 0;
	z.y = 0;

	c.x = x;
	c.y = y;

	s.x = z.x * z.x;
	s.y = z.y * z.y;

	float norm = s.x + s.y;

	float value = 0.0f;

	int iterations = 0;

	while (true)
	{
		if ( norm > 4 )
			return value;
		
		if ( iterations > 1000 )
			return 0.0f;

		z.y = z.x * z.y * 2;
		z.y += c.y;

		z.x = s.x - s.y;
		z.x += c.x;

		s.x = z.x * z.x;
		s.y = z.y * z.y;

		norm = s.x + s.y;

		value += exp( -sqrt( norm ) );

		iterations++;
	}
}

#else

inline float mandelbrot(float x, float y)
{
	Complex z,s,c;
	
	c.x = x;
	c.y = y;

	z.x = c.x;
	z.y = c.y;

	s.x = z.x * z.x;
	s.y = z.y * z.y;

	unsigned int iterations = 0;

	while ( true )
	{
		z.y = z.x * z.y;
		z.y += z.y;
		z.y += c.y;

		z.x = s.x - s.y;
		z.x += c.x;

		s.x = z.x * z.x;
		s.y = z.y * z.y;

		if ( s.x + s.y > 4 )
			return (float)iterations;

		if ( ++iterations > 1000 )
			return 0.0f;
	}
}

#endif


inline float mandelbrot(float x1, float y1, float dx, float dy)
{
	float value = 0;

	float y = y1;

	for (int i=0; i<5; i++)
	{
		float x = x1;

		for (int j=0; j<5; j++)
		{
			value += mandelbrot(x, y);
			x += dx;
		}
		
		y += dy;
	}

	value *= 1.0f/25.0f;

	return value;
}


int main()
{
    const int width = 512;
	const int height = 512;

    Display display( "Fractal", width, height );

    std::vector<Pixel> pixels(width * height);

	const float x1 = 0.278587f;
	const float y1 = -0.00744f;
	const float x2 = 0.283707f;
	const float y2 = -0.012560f;

	int iy = 0;
	int index = 0;

	const float dx = (x2 - x1) / width;
	const float dy = (y2 - y1) / height;

	float x = x1 - dx * 0.5f;
	float y = y1 - dy * 0.5f;

	// main loop

	Timer timer;

	while ( display.open() )
    {
		float start = (float) timer.time();

		while ((float)timer.time()<start+0.01f)
		{
			if (iy<height)
			{
				for (int ix=0; ix<width; ix++)
				{
					const float value = mandelbrot(x, y, dx/5.0f, dy/5.0f);

					colorize( pixels[index++], value );

					x += dx;
				}
			}
			else
				index += width;

			iy ++;
			y += dy;
			x = x1;
		}

        display.update( pixels );
    }
}
