
[ Introduction ]

PixelToaster is a library for C++ programmers who want to write their own
software rendering routines, instead of using hardware accelerated 
rendering with OpenGL or Direct3D.

To use PixelToaster, all you need to do is ‘open’ a display at the desired 
resolution, then each frame, render into an array of pixels and ‘update’ 
your pixels to the display.

Take a look at the example programs to see just how easy it is!

[ Installation ]

 * Windows:

 * UNIX:

	The supported display on these platforms


[ How to Use PixelToaster ]

...


[ Pixel Format ]

You have the choice of working in truecolor on floating point color.

Truecolor has 32bit integer pixels with byte size rgb color components,
while floating point color has 128bits per pixel, with floating point values 
for red, green, blue and alpha!

	struct Pixel
	{
		float b,g,r,a;
	};

All things being equal, floating point is obviously slower than truecolor, 
if for no reason other than requiring 4X the memory bandwidth! However, the 
increased dynamic range and high precision of floating point make it a very 
interesting and fun pixel format to work in.

No matter which format you choose, PixelToaster will automatically convert 
from your pixel format to the native pixel format of the display so you dont 
have to worry about machines that only support 16bit color, or dont support 
floating point color natively. 

Everything just works!


[ Working in TrueColor ]

In truecolor, pixels are packed into 32bit integer values, like this:

	[x][r][g][b]

r,g,b are the red, green and blue color components making up the color.

the x component is padding so the pixel takes up 32 bits.

Each component is in the range [0,255] with 0 being lowest intensity, 
and 255 being maximum intensity.

therefore: 

	black is (0,0,0) -> 0x00000000
	bright red is (255,0,0) -> 0x00FF0000
	bright green is (0,255,0) -> 0x0000FF00
	bright blue = (0,0,255) -> 0x000000FF

You can unpack and manually using masking and shift operations:

	integer8 r = ( pixel & 0x00FF0000 ) >> 16;
	integer8 g = ( pixel & 0x0000FF00 ) >> 8;
	integer8 b = ( pixel & 0x000000FF );
	
And repack them again with shifts:
	
	integer32 repacked = ( r << 16 ) | ( g << 8 ) | b;

Alternatively, you can treat your array of pixels as an array of
TrueColorPixel structs and use its union to access pixels:

	struct TrueColorPixel
	{
		union
		{
			integer32 integer;
			struct { integer8 a,r,g,b; };
		};
	};

    TrueColorPixel pixel;

	pixel.r = 128;
	pixel.g = 10;
	pixel.b = 192;
	
This is much easier! Just remember that each r,g,b value can only
in the range [0,255] when working with it - if you go outside this range
the color will wrap around from light to dark and vice versa.


[ Working in Floating Point Color ]

Floating point pixels made up of four floating point values:

	struct FloatingPointPixel
	{
		
	};

Floating point color is much easier to work with. I guess thats what 4X
the memory bandwidth buys you:

	FloatingPointPixel pixel;
	
	pixel.r = 0.5f;
	


[ Memory Layout for Pixels ]

Note that when working in an array of pixels in PixelToaster, you
calculate 

	int index = 



[ Licence ]

	PixelToaster Framebuffer Library.

	Copyright © 2004-2007 Glenn Fiedler

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.

	3. This notice may not be removed or altered from any source distribution.

	Glenn Fiedler
	gaffer@gaffer.org
