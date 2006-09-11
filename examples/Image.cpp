// Image example
// Demonstrates how to load and display a TGA image
// Copyright © Glenn Fiedler, 2004-2006. http://www.pixeltoaster.com

#include <cstdio>
#include <PixelToaster.h>

using namespace PixelToaster;


bool load( const char filename[], int & width, int & height, vector<Pixel> & pixels );


int main()
{
	// load image and show it on the screen

	vector<Pixel> pixels;

    int width = 0;
	int height = 0;

	load( "image.tga", width, height, pixels );

	Display display( "Image Example", width, height );

    while ( display.open() )
		display.update( pixels );
}


bool load( const char filename[], int & width, int & height, vector<Pixel> & pixels )
{
	unsigned int index = 0;

	vector<unsigned char> buffer;

	// open file for binary reading

	FILE * file = fopen( filename, "rb" );

	if ( !file )
	{
		printf( "failed to open file\n" );
		return false;
	}

	// read 18 byte TGA header
	
	unsigned char header[18];

	if ( !fread( header, 18, 1, file ) )
	{
		printf( "failed to read header\n" );
		goto failure;
	}

	// fail if not uncompressed rgb format

	if ( header[2] != 2 )
	{
		printf( "tga must be uncompressed rgb format\n" );
		goto failure;
	}

	// fail if not 24 bits per pixel

	if ( header[16] != 24 )
	{
		printf( "tga must be 24 bits per pixel\n" );
		goto failure;
	}

	// read image pixels

	width = ( header[13] << 8 ) | header[12];
	height = ( header[15] << 8 ) | header[14];

	buffer.resize( width * height * 3, 0 );

	if ( !fread( &buffer[0], buffer.size(), 1, file ) )
	{
		printf( "failed to read image pixel data\n" );
		goto failure;
	}

	fclose( file );
	
	// convert 24 bit image pixels to floating point color

	pixels.resize( width * height );

	for ( int y = 0; y < height; ++y )
	{
		for ( int x = 0; x < width; ++x )
		{
			Pixel & pixel = pixels[index];

			pixel.b = buffer[index*3+0] * 1.0f / 255.0f;
			pixel.g = buffer[index*3+1] * 1.0f / 255.0f;
			pixel.r = buffer[index*3+2] * 1.0f / 255.0f;

			++index;
		}
	}

	return true;

failure:

	// somebody call the whaaaambulance...

	width = 0;
	height = 0;
	fclose( file );
	
	return false;
}
