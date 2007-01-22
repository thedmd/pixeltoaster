
[ Introduction ]

PixelToaster is a library for C++ programmers who want to write their own
software rendering routines, instead of using hardware accelerated 
rendering with OpenGL or Direct3D.

To use PixelToaster, all you need to do is ‘open’ a display at the desired 
resolution, then each frame, render into an array of pixels and ‘update’ 
your pixels to the display.


[ Installation ]

 * Windows:

    PixelToaster for Windows requires DirectX 9.0.
    
    First you must make sure you have the latest DirectX Runtime installed.
    
    Visual C++ Users:
    
        - Install the latest DirectX SDK so you have its headers and libs
    
        - If you are using Visual C++ Express, you need to install the 
          Platform SDK if you havent already. Follow the instructions on
          the Visual C++ Express site.
    
        - If you plan on building PixelToaster from the command line (nmake)
          make sure you add the appropriate DirectX directories to your LIB 
          and INCLUDE environment variables
        
        - To use the solution file, just open PixelToaster.sln select the
          example project you want to build (right click, make active project), 
          and press F5 to build & run. For speed, switch to "Release"
        
        - The solution file only supports the latest Visual Studio 2005,
          so if you have an earlier version of Visual C++, you'll need to use
          nmake or create your own solution/project files.

        - To use nmake, open "Visual Studio XXXX Command Prompt" to get a cmd
          line with the path and environment fully setup, then type:
        
              nmake -f makefile.visualc
                        
    MinGW Users:
    
        - You *DO NOT* need to install the DirectX SDK
        
        - Make sure your MinGW installation includes the Windows and DirectX
          headers. The easiest way to do this is to simply install the entire
          MinGW package instead of the smaller pieces. The full package
          contains everything you need to build PixelToaster.
        
        - Go to the command line and type:
        
              mingw32-make -f makefile.mingw
         
        - All the example programs will be built for you
        
        - You can use PixelToaster with Dev-C++ or any other IDE 
          on top of MinGW, just as long as you setup your own project files.
    
 * UNIX: FreeBSD, Linux and MacOSX

    The supported display on these platforms is XWindows.
    
    This includes MacOS X because there is no native Cocoa display yet :(
    
    If you would like to help out this project, the best thing you can do
    is implement a native MacOS X display for us!

    Until then, MacOS X users, make sure you have both the X11 SDK,
    and XWindows binaries installed. You can install them from the
    development tools CD that came with MacOS X.

    To compile the example programs, just use make:
    
        make -f makefile.linux
        make -f makefile.bsd
        make -f makefile.apple

    Pick the correct line based on your platform obviously,
    I like to speed things up a bit my using a symbolic link:
    
    For example on MacOSX, i just go:
    
        ln -s makefile.apple makefile
        make
        
    So everything is easy from this point on:
    
        make test
        make profile
        make all
        make docs
        make clean

    In order to make docs, you'll need to have doxygen installed:
    
        http://www.doxygen.org
    
    Alternatively, you can browse the docs for the latest release online:
    
        http://www.pixeltoaster.com
        
    Have fun!


[ How to Use PixelToaster ]

The API of PixelToaster is so easy to use, just look at the example programs
and you'll get the idea quickly.

However, what isnt obvious at first, is exactly what to do with the pixels!

Obviously, I cant teach software rendering in a few pages, but here is a quick 
tutorial to get you started...


[ Pixels in Memory ]

Pixels are laid out in memory sequentially from top to bottom, left to right.

You can visualize this by rearranging each line of the image into a single chain,
each line bolted on at the end of the one above it, in one long line.

Lets say we have an image of dimensions 320x240, that is:

    width = 320
    height = 240
    
    Pixel pixels[320*240];
    
Coordinate (0,0) is the top-left of the screen, and (319,239) is the bottom right. 

Your x coordinate needs to be in range [0,319] and your y coordinate needs to be 
in range [0,239]

We can find the index of a pixel given its (x,y) coordinates as follows:

    int index = x + y * width;
    
So we can set get this pixel as follows:

    pixel[index] = Pixel( 0, 0, 1 );		// set to blue
    
If you think about this closely, you are starting from the top-left of the screen,
advancing past all the whole lines above the line you want (y*width). This gets
you to the start of the line you want, so all if you have to do now is add "x"
and you have the index of the pixel at (x,y).


[ Pixel Format ]

You have the choice of working in truecolor or floating point color.

Truecolor has 32bit integer pixels with byte size rgb color components,
while floating point color has 128bits per pixel, with floating point values 
for red, green, blue and alpha!

All things being equal, floating point is obviously slower than truecolor, 
if for no reason other than requiring 4X the memory bandwidth! However, the 
increased dynamic range and high precision of floating point make it a very 
interesting format to work in.


[ Working in Floating Point Color ]

Floating point pixels made up of four floating point values:

    struct FloatingPointPixel
    {
        float r,g,b,a;
    };

You set the color of a pixel by setting its components.

The alpha value is for padding to 128bits. You can ignore it, or use it for 
any purpose you like. It makes a nice z-buffer for a software renderer, or
alpha channel for compositing.

Each component is in range [0.0, 1.0f]

0.0f is minimum intensity (dark), while 1.0f is maximum intensity.

At the moment, values outside this range are automatically clamped.

In the future, these values will be used to indicate "high dynamic range"
to high contrast displays!

Here are some examples:

    black = (0,0,0)
    white = (1,1,1)
    red = (1,0,0)
    green = (0,1,0)
    blue = (0,0,1)
    
    really bright white = (1000000,1000000,1000000) ... !!!

    
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

    TrueColorPixel pixel;

    pixel.r = 128;
    pixel.g = 10;
    pixel.b = 192;
    
This is much easier! Just remember that each r,g,b value can only
in the range [0,255] when working with it - if you go outside this range
the color will wrap around from light to dark and vice versa.


[ Example Programs ]

    * ExampleFloatingPoint
        - how to open a display in floating point color and get pixels on the screen

    * ExampleTrueColor
        - the same thing, but in truecolor

    * ExampleFullscreen
        - how to open a fullscreen display
        - fullscreen output is currently only supported in windows (DirectX 9.0)

    * ExampleKeyboardAndMouse
        - how to receive keyboard and mouse events from a display
        
    * ExampleTimer
        - how to use the high resolution timer
    
    * ExampleImage
        - how to load a TGA image and display it
        
    * ExampleMultiDisplay
        - how to work with multiple displays (windowed only)


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
