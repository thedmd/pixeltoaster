// Apple MacOS X Platform
// Copyright © 2004-2007 Glenn Fiedler
// Part of the PixelToaster Framebuffer Library - http://www.pixeltoaster.com

// native Cocoa output implemented by Thorsten Schaaps <bitpull@aixplosive.de>

#include "PixelToaster.h"
#include "PixelToasterCommon.h"
#include "PixelToasterConversion.h"
#include "PixelToasterApple.h"

#if !PIXELTOASTER_APPLE_USE_X11

	#import <Cocoa/Cocoa.h>
	#import <QuartzCore/CIContext.h>
	#import <OpenGL/OpenGL.h>
	#include <OpenGL/glu.h>

	using namespace PixelToaster;

	#ifndef PIXELTOASTER_APPLE_ALLOW_ZOOM
		// adds Zoom1x, 2x, 3x, 4x to "Window" menu and allows
		// resizing the window by dragging the lower right corner
		#define PIXELTOASTER_APPLE_ALLOW_ZOOM				1
	#endif

	// allows PixelToaster to change the screen resolution
	// when switching to fullscreen mode (not yet implemented)
	#define PIXELTOASTER_APPLE_ALLOW_MODESWITCH	0

	#ifndef PIXELTOASTER_APPLE_DEBUG
		// enables some debug output, disables fades and
		// capturing the mouse cursor
		#define PIXELTOASTER_APPLE_DEBUG						0
	#endif

	#ifndef PIXELTOASTER_APPLE_VBSYNC
		// switches sync to vertical blank on/off (0=off, 1=on)
		#define PIXELTOASTER_APPLE_VBSYNC						1
	#endif

	// fade time in seconds
	static const double FADE_TIME = 0.5;

	// number of steps per fade
	static const int		FADE_STEPS = 30;

	#if PIXELTOASTER_APPLE_DEBUG
		#define GL_CHECK_ERROR( cmd ) \
				cmd; \
				{ GLenum error = glGetError(); \
					if( GL_NO_ERROR != error ) \
					fprintf( stderr, "%s:%d:: ’%s’ failed with error: '%s'\n", \
									__FILE__, __LINE__, #cmd, gluErrorString( error ) ); \
				}
	#else
		#define GL_CHECK_ERROR( cmd ) cmd;
	#endif

	// to get rid of the unused parameter warning in ObjC methods
	#define PARAMETER_UNUSED(x) (void) (x);

	#pragma mark -
	#pragma mark Objective-C classes
	#pragma mark -

	@interface PTApplication : NSApplication
		{
			@private
			BOOL	messageLoopIsRunning;
		}
		- (id)init;

		// since we never actually call [NSApp run] we need to overwrite -isRunning
		- (BOOL)isRunning;
		- (void)setIsRunning:(BOOL)isRunning;

		- (void)performAbout:(id)sender;
		- (void)setupApplicationMenu;
	@end

	@implementation PTApplication

		- (id)init
		{
			self = [ super init ];
			if( self != nil )
			{
				messageLoopIsRunning = NO;
				[ self setDelegate:self ];
			}
			return self;
		}

		- (BOOL)isRunning
		{
			return messageLoopIsRunning;
		}

		- (void)setIsRunning:(BOOL)isRunning
		{
			messageLoopIsRunning = isRunning;
		}

		// Called on Cmd-Q / Quit menu item
		- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
		{
			PARAMETER_UNUSED(sender)

			// try to close all windows..
			NSArray*			windows			= [ self windows ];
			NSEnumerator* enumerator	= [ windows objectEnumerator ];
			NSWindow*			window;
			while( (window = [ enumerator nextObject ]) != nil )
			{
				[ window performClose:nil ];
			}
	
			// if no windows are left open, we allow the application to terminate
			if( [ [ self windows ] count ] == 0 )
			{
				[ self setIsRunning: NO ];
				return NSTerminateNow;
			}
			else
				return NSTerminateCancel;
		}

		// a small custom about box
		- (void)performAbout:(id)sender
		{
			PARAMETER_UNUSED(sender)
			NSString*						versionString		= [ NSString stringWithFormat:@"using PixelToaster v%.1f", PIXELTOASTER_VERSION ];
			NSAttributedString* creditString		= [ [ [ NSAttributedString alloc ] initWithString:versionString ] autorelease ];
			NSString*						copyRightString = [ NSString stringWithCString: "Copyright \xc2\xa9 2004-2007 Glenn Fiedler\n"
																																					"Mac OS X implementation by Thorsten Schaaps"
																																encoding: NSUTF8StringEncoding ];
			NSDictionary *dict = [ NSDictionary dictionaryWithObjectsAndKeys: creditString,		 @"Credits",
																																				copyRightString, @"Copyright",
																																				nil ];
			[ NSApp orderFrontStandardAboutPanelWithOptions: dict ];
		}

		// Set up a menu bar, so the PixelToaster application looks a bit more "native"
		// Besides the well known apple menu there is only a "Window" menu added.
		// This only gets called when we create the NSApp instance, so if PixelToaster
		// is used from a true Cocoa context, this should not interfere...
		- (void)setupApplicationMenu
		{
			NSString* applicationName = [ [ NSProcessInfo processInfo ] processName ];

			NSMenu*		mainMenu				= [ [ NSMenu alloc] initWithTitle: @"MainMenu" ];

			// Setup Application menu.. this needs a hack (see below)
			{
				// the title of the menu and item don't matter, they will be replaced by the processname anyway
				NSMenu*		appMenu				= [ [ NSMenu alloc ] initWithTitle: @"AppleMenu" ];

				// "About "applicationName
				[ appMenu addItemWithTitle: [ @"About " stringByAppendingString: applicationName ]
														action: @selector(performAbout:)
										 keyEquivalent: @"" ];


				// ---
				[ appMenu addItem: [ NSMenuItem separatorItem ] ];

				// Preferences TODO: nothing implemented yet, we could let the user choose a code path
				// or an initial screen here...
				[ appMenu addItemWithTitle: @"Preferences"
														action: @selector(showPreferences:)
										 keyEquivalent: @","];
		
				//  ---
				[ appMenu addItem: [ NSMenuItem separatorItem ] ];

				// Services
				NSMenuItem* servicesItem = [ appMenu addItemWithTitle: @"Services"
																												 action: nil
																									keyEquivalent: @"" ];
				NSMenu*			servicesMenu = [ [ NSMenu alloc ] initWithTitle: @"" ];

				[ servicesItem	setSubmenu: servicesMenu ];
				[ NSApp	setServicesMenu: servicesMenu ];
				[ servicesMenu release ];

				// ---
				[ appMenu addItem: [ NSMenuItem separatorItem ] ];

				// Hide Application
				[ appMenu addItemWithTitle: [ @"Hide " stringByAppendingString: applicationName ]
														action: @selector( hide: )
										 keyEquivalent: @"h" ];

				// Hide Others
				NSMenuItem* hideOthersItem = [ appMenu addItemWithTitle: @"Hide Others"
																												 action: @selector( hideOtherApplications: )
																									keyEquivalent: @"h" ];
				[ hideOthersItem setKeyEquivalentModifierMask: ( NSAlternateKeyMask | NSCommandKeyMask ) ];

				// Show All
				[ appMenu addItemWithTitle: @"Show All"
														action: @selector( unhideAllApplications: )
										 keyEquivalent: @"" ];

				// ---
				[ appMenu addItem: [ NSMenuItem separatorItem ] ];
		
				// Quit <applicationName>
				[ appMenu addItemWithTitle:[ @"Quit " stringByAppendingString: applicationName ]
																															 action: @selector(terminate:)
																												keyEquivalent: @"q" ];

				// setAppleMenu is private since Mac OS X 10.4 Tiger, but it still seems to be available
				// (and does work) in 10.5 Leopard.
				if( [ NSApp respondsToSelector: @selector( setAppleMenu: ) ] )
				{
					[ NSApp performSelector: @selector( setAppleMenu: ) withObject: appMenu ];
				}

				NSMenuItem*	appItem	= [ [ NSMenuItem alloc] initWithTitle: @"AppMenuItem"
																													 action: nil
																										keyEquivalent: @"" ];
				[ appItem	setSubmenu: appMenu ];
				[ mainMenu addItem: appItem ];

				[ appItem release ];
				[ appMenu release ];
			}

			// Setup "Window" menu
			{
				NSMenu*	windowMenu = [ [ NSMenu alloc] initWithTitle: @"Window" ];
		
				// Close Window
				[ windowMenu addItemWithTitle: @"Close Window"
															 action: @selector( performClose: )
												keyEquivalent: @"w" ];

				// ---
				[ windowMenu addItem: [ NSMenuItem separatorItem ] ];

				// Minimize
				[ windowMenu addItemWithTitle: @"Minimize"
															 action: @selector( performMiniaturize: )
												keyEquivalent: @"m" ];
		
				#if PIXELTOASTER_APPLE_ALLOW_ZOOM
				// Zoom
					[ windowMenu addItemWithTitle: @"Zoom"
																 action: @selector( performZoom: )
													keyEquivalent: @"" ];
				#endif
		
				// ---
				[ windowMenu addItem: [ NSMenuItem separatorItem ] ];

				// Toggle Fullscreen
				[ windowMenu addItemWithTitle: @"Toggle Fullscreen"
															 action: @selector( menuToggleFullscreen: )
												keyEquivalent: @"F" ];				

				#if PIXELTOASTER_APPLE_ALLOW_ZOOM
					// ---
					[ windowMenu addItem: [ NSMenuItem separatorItem ] ];

					// 1 x
					NSMenuItem* zoomItem;
					zoomItem = [ windowMenu addItemWithTitle: @"1 x"
																						action: @selector( zoomX: )
																		 keyEquivalent: @"1" ];
					[ zoomItem setTag:1 ];

					// 2 x
					zoomItem = [ windowMenu addItemWithTitle: @"2 x"
																						action: @selector( zoomX: )
																		 keyEquivalent: @"2" ];
					[ zoomItem setTag:2 ];

					// 3 x
					zoomItem = [ windowMenu addItemWithTitle: @"3 x"
																						action: @selector( zoomX: )
																		 keyEquivalent: @"3" ];
					[ zoomItem setTag:3 ];

					// 4 x
					zoomItem = [ windowMenu addItemWithTitle: @"4 x"
																						action: @selector( zoomX: )
																		 keyEquivalent: @"4" ];
					[ zoomItem setTag:4 ];
				#endif

				// ---
				[ windowMenu addItem: [ NSMenuItem separatorItem ] ];

				// Bring All to Front
				[ windowMenu addItemWithTitle: @"Bring All to Front"
															 action: @selector( arrangeInFront: )
												keyEquivalent: @"" ];
		
				// ---
				[ windowMenu addItem: [ NSMenuItem separatorItem ] ];

				NSMenuItem*	windowItem = [ [ NSMenuItem alloc] initWithTitle: @""
																														 action: nil
																											keyEquivalent: @"" ];
				[ mainMenu addItem: windowItem ];
				[ windowItem setSubmenu: windowMenu ];
				[ NSApp setWindowsMenu: windowMenu ];
			}

			[ NSApp setMainMenu: mainMenu ];

			[ mainMenu release ];
		}

	@end

	#pragma mark -

	@interface PixelToasterView : NSView
		{
			@private
			NSOpenGLContext*						_openGLContext;
			NSOpenGLPixelFormat*				_pixelFormat;

			float												glVersion;
			GLint												glMaxRectTextureSize;
			GLint												glMaxTextureUnits;
			GLint												glMaxTextureSize;
			GLboolean										glHasHWFloats;
			GLboolean										glHasTextureRange;
			GLboolean										glHasClientStorage;
			GLboolean										glHasTextureRectangle;
			bool												glHasFastFloats;
			bool												glIsIntelIntegratedGraphics;
			bool												glUseSharedHint;
	
			GLuint											glTexture;
	
			// Gamma values used for fade operations
			CGGammaValue								redMin, redMax, redGamma;
			CGGammaValue								greenMin, greenMax, greenGamma;
			CGGammaValue								blueMin, blueMax, blueGamma;

			// buffer used to convert floats into before uploading to graphics card
			unsigned char*							XRGB8888_buffer;

			// Listener that should receive any events
			Listener*										displayListener;
	
			// Display represented by this view
			AppleDisplay*								display;

			// rectangle the pixel buffers are copied/stretched to,
			// usually the whole window content, but may have offsets in
			// fullscreen mode (e.g. for letterbox or square formats)
			NSRect											fillRect;
	
			BOOL												isFullscreen;

			// screen ID that is used for output
			CGDirectDisplayID						displayID;
		}

		+ (NSOpenGLPixelFormat*)defaultPixelFormatForFullscreen:(BOOL)fullscreen displayID:(CGDirectDisplayID)displayID;
		+ (NSOpenGLPixelFormat*)defaultPixelFormat;

		- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat*)format display:(AppleDisplay*)theDisplay;
		- (void)setOpenGLContext:(NSOpenGLContext*)context;
		- (NSOpenGLContext*)openGLContext;
		- (void)clearGLContext;
		- (void)updateGLInfo;
		- (void)releaseTexture;
		- (void)prepareTexture:(const GLvoid*)textureData;
		- (void)prepareOpenGL;
		- (void)update;
		- (void)reshape;
		- (void)setPixelFormat:(NSOpenGLPixelFormat*)	pixelFormat;
		- (NSOpenGLPixelFormat*)pixelFormat;

		- (void)setListener:(Listener*)listener;
		- (Listener*)listener;

		// no setter for "display", as this is set once in -init
		- (AppleDisplay*)display;

		- (CGDirectDisplayID)displayID;

		- (void)setFillRect:(NSRect)newFillRect;
		- (NSRect)fillRect;

		-(void)hideCursor;
		-(void)unhideCursor;

		-(void)fadeInDisplay;
		-(void)fadeOutDisplay;

		- (void)clear;
		- (void)copyTrueColorPixelsUsingOpenGL:(const TrueColorPixel*)trueColorPixels
											 floatingPointPixels:(const FloatingPointPixel*)floatingPointPixels
																 dirtyRect:(const Rectangle*)dirtyBox;

		- (BOOL)setFullscreen:(BOOL)fullscreen;

		- (void)menuToggleFullscreen:(id)sender;
		- (void)zoomX:(id)sender;
	@end

	@implementation PixelToasterView

		+ (NSOpenGLPixelFormat*)defaultPixelFormat
		{
			return [ self defaultPixelFormatForFullscreen:NO displayID:CGMainDisplayID()];
		}

		+ (NSOpenGLPixelFormat*)defaultPixelFormatForFullscreen:(BOOL)fullscreen displayID:(CGDirectDisplayID)displayID
		{
			NSOpenGLPixelFormat*					pixelFormat = nil;
			NSOpenGLPixelFormatAttribute	attributes[20];
			int														at_index		= 0;

			attributes[ at_index++ ] = NSOpenGLPFADoubleBuffer;
			attributes[ at_index++ ] = NSOpenGLPFAAccelerated;
			//attributes[ at_index++ ] = NSOpenGLPFAColorFloat;
			attributes[ at_index++ ] = NSOpenGLPFAColorSize;
			attributes[ at_index++ ] = (NSOpenGLPixelFormatAttribute)24;
			attributes[ at_index++ ] = NSOpenGLPFAScreenMask;
			attributes[ at_index++ ] = (NSOpenGLPixelFormatAttribute)CGDisplayIDToOpenGLDisplayMask( displayID );

			if( ! fullscreen )
			{
				attributes[ at_index++ ] = NSOpenGLPFABackingStore;
				attributes[ at_index++ ] = NSOpenGLPFANoRecovery;
			}
			else
			{	// Fullscreen		
				attributes[ at_index++ ] = NSOpenGLPFAFullScreen;
			}
			attributes[ at_index ] = (NSOpenGLPixelFormatAttribute)0;

			pixelFormat = [ [ NSOpenGLPixelFormat alloc ] initWithAttributes:attributes ];

			return [ pixelFormat autorelease ];
		}

		- (void) surfaceNeedsUpdate:(NSNotification*)notification
		{
			PARAMETER_UNUSED(notification)
			[ self update ];
		}

		- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat*)format display:(AppleDisplay*)theDisplay
		{
			self = [super initWithFrame:frameRect];
	
			display = theDisplay;
			displayID = CGMainDisplayID();
			glTexture = 0;
	
			if (self != nil)
			{
				_pixelFormat = [ format retain ];
				[ [ NSNotificationCenter defaultCenter] addObserver: self
																									 selector: @selector( surfaceNeedsUpdate: )
																											 name: NSViewGlobalFrameDidChangeNotification
																										 object: self ];
			}
			return self;
		}

		- (void)releaseTexture
		{
			if( XRGB8888_buffer != 0 )
			{
				free( XRGB8888_buffer );
				XRGB8888_buffer = 0;
			}
	
			if( glTexture > 0 )
			{
				glDeleteTextures( 1, &glTexture );
				glTexture = 0;
			}
		}

		- (void)dealloc
		{
			[ [ NSNotificationCenter defaultCenter ] removeObserver: self
																												 name: NSViewGlobalFrameDidChangeNotification
																											 object: self ];
			[ self releaseTexture ];
			[ self clearGLContext ];
			[ _pixelFormat release ];
			[ super dealloc ];
		}

		- (void)setPixelFormat:(NSOpenGLPixelFormat*)pixelFormat
		{
		    [ _pixelFormat release ];
		    _pixelFormat = [pixelFormat retain];
		}

		- (NSOpenGLPixelFormat*)pixelFormat
		{
		    return _pixelFormat;
		}

		- (void)setOpenGLContext:(NSOpenGLContext*)context
		{
			[ self clearGLContext ];
			_openGLContext = [ context retain ];
		}

		- (NSOpenGLContext*)openGLContext
		{
			// create a context if none exists
			if( _openGLContext == nil )
			{
				[ self updateGLInfo ];
		
				if( _pixelFormat == nil )
				{
					[ self setPixelFormat: [ [ self class ] defaultPixelFormatForFullscreen:isFullscreen displayID:displayID ] ];
				}
				_openGLContext = [ [ NSOpenGLContext alloc ] initWithFormat:_pixelFormat shareContext:nil ];
				[ _openGLContext makeCurrentContext ];
				[ self prepareOpenGL ];
			}
	
			if( ( _openGLContext == nil ) || ( _pixelFormat == nil ) )
			{
				return nil;
			}
	
			return _openGLContext;
		}

		- (void)clearGLContext
		{
			if( _openGLContext != nil )
			{
				[ self releaseTexture ];
				if( [ _openGLContext view ] == self )
				{
						[ _openGLContext clearDrawable ];
				}
				[ _openGLContext release ];
				_openGLContext = nil;
			}

			// release pixel format too
			[ self setPixelFormat:nil ];
		}

		-(void)updateGLInfo
		{
			const GLubyte *glVersionString, *glExtensions, *glRendererString;
	
			if( displayID == 0 )
				displayID = CGMainDisplayID();
	
			CGOpenGLDisplayMask displayMask = CGDisplayIDToOpenGLDisplayMask( displayID );

			// Check capabilities of display represented by display mask
			CGLPixelFormatAttribute attribs[] = { kCGLPFADisplayMask, (CGLPixelFormatAttribute)displayMask,
																						(CGLPixelFormatAttribute)0 };
			CGLPixelFormatObj pixelFormat = NULL;
			GLint numPixelFormats = 0;
			CGLContextObj cglContext = 0;
			CGLContextObj curr_ctx = CGLGetCurrentContext();
	
			CGLChoosePixelFormat( attribs, &pixelFormat, &numPixelFormats );
	
			if( pixelFormat )
			{
				CGLCreateContext( pixelFormat, NULL, &cglContext );
				CGLDestroyPixelFormat( pixelFormat );
				CGLSetCurrentContext( cglContext );
				if( cglContext )
				{
					// Check for capabilities and functionality
					glVersionString		= glGetString( GL_VERSION );
					glExtensions			= glGetString( GL_EXTENSIONS );
					glRendererString	= glGetString( GL_RENDERER );
			
					sscanf( (char*)glVersionString, "%f", &glVersion );
			
					glGetIntegerv( GL_MAX_TEXTURE_UNITS, &glMaxTextureUnits );
					glGetIntegerv( GL_MAX_TEXTURE_SIZE,  &glMaxTextureSize );

					glHasHWFloats					= gluCheckExtension( (const GLubyte*)"GL_APPLE_float_pixels", glExtensions );
					glHasTextureRange			=	gluCheckExtension( (const GLubyte*)"GL_APPLE_texture_range", glExtensions );
					glHasClientStorage		=	gluCheckExtension( (const GLubyte*)"GL_APPLE_client_storage", glExtensions );
					glHasTextureRectangle = gluCheckExtension( (const GLubyte*)"GL_EXT_texture_rectangle", glExtensions );

					// on my iMac handing floats directly to the GPU can keep up with
					// PixelToaster's conversion, but on all other tested machines it
					// absolutely destroys performance. We will need this later for
					// PixelToaster's ToneMapping extension, but for now we leave it disabled
					glHasFastFloats				= false; // = glHasHWFloats
			
					// the integrated graphics chipsets I could test on were slower with
					// client storage, so we set a flag for this code path...
					glIsIntelIntegratedGraphics = ( NULL != strstr((const char*)glRendererString, "Intel") );
			
					// generally GL_STORAGE_CACHED_APPLE seems to get the higher performance,
					// but at least on the new iMacs (ATI HD 2600 Pro in my case)
					// GL_STORAGE_SHARED_APPLE is a lot faster for TrueColor
					glUseSharedHint = ( NULL != strstr((const char*)glRendererString, "Radeon HD 2600" ) );
			
					if( glHasTextureRectangle )
						glGetIntegerv (GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT, &glMaxRectTextureSize);
					else
						glMaxRectTextureSize = 0;
			
					#if PIXELTOASTER_APPLE_DEBUG
						const GLubyte* glVendorString		= glGetString( GL_VENDOR );
						printf( "OpenGL Version:   %s\n", glVersionString );
						printf( "Vendor:           %s\n", glVendorString );
						printf( "Renderer:         %s\n", glRendererString );
						printf( "Max Texture Size: %d\n", (int)glMaxTextureSize );
						printf( "Max Texture Size: %d\n", (int)glMaxTextureSize );
						printf( "Max Rectangle Texture Size: %d\n", (int)glMaxRectTextureSize );
						printf( "Texture Rectangle: %savailable\n", glHasTextureRectangle ? "": "not " );
						printf( "Float Buffers....: %savailable\n", glHasHWFloats ? "" : "not " );
						printf( "Texture Range....: %savailable\n", glHasTextureRange ? "": "not " );
						printf( "Client Storage...: %savailable\n", glHasClientStorage ? "" : "not " );
					#endif
				}
			}
			CGLDestroyContext( cglContext );
			CGLSetCurrentContext( curr_ctx );
		}

		- (void)prepareTexture:(const GLvoid*)textureData
		{
			if( glTexture == 0 )
			{
				const	int		width    = display->width();
				const int		height	 = display->height();

				bool useFloat = ( display->mode() == Mode::FloatingPoint );

				GL_CHECK_ERROR( glGenTextures( 1, &glTexture ) );
				GL_CHECK_ERROR( glBindTexture( GL_TEXTURE_RECTANGLE_ARB, glTexture ) );

				if( ! glIsIntelIntegratedGraphics )
				{
					GL_CHECK_ERROR( glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_TRUE) );
					GL_CHECK_ERROR( glTexParameterf( GL_TEXTURE_RECTANGLE_ARB ,GL_TEXTURE_PRIORITY, 0.0f) );
					GL_CHECK_ERROR( glTexParameteri( GL_TEXTURE_RECTANGLE_ARB,
																					 GL_TEXTURE_STORAGE_HINT_APPLE,
																					 glUseSharedHint ? GL_STORAGE_SHARED_APPLE
																													 : GL_STORAGE_CACHED_APPLE ) );
				}
		
				GL_CHECK_ERROR( glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR ) );
				GL_CHECK_ERROR( glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR ) );
				GL_CHECK_ERROR( glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE) );
				GL_CHECK_ERROR( glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE) );
				GL_CHECK_ERROR( glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL) );

				GL_CHECK_ERROR( glPixelStorei( GL_UNPACK_ROW_LENGTH, width ) );
				GL_CHECK_ERROR( glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ) );

				if( ! glHasFastFloats )
				{
					const int buffer_size = width * height * 4;
					if(!XRGB8888_buffer)
						XRGB8888_buffer = (unsigned char*)malloc( buffer_size );
			
					if( useFloat )
					{
						textureData = XRGB8888_buffer;
						useFloat = false;
					}
				}

				if( glHasTextureRange )
				{
					GL_CHECK_ERROR( glTextureRangeAPPLE(GL_TEXTURE_RECTANGLE_ARB,
																							width * height * (useFloat ? 16 : 4),
																							textureData) );
				}
				GL_CHECK_ERROR( glTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0,
																			useFloat ? GL_RGB_FLOAT32_APPLE : GL_RGB8,
																				// HERE: using floats directly is faster on my iMac, but slower on my MBP
																				//       and slower on both compared to PixelToasters software conversion :-P
																			width, height, 0,
																			useFloat ? GL_RGBA	: GL_BGRA_EXT,
																			useFloat ? GL_FLOAT : GL_UNSIGNED_INT_8_8_8_8_REV,
																			textureData ) );
			}
		}

		- (void)prepareOpenGL
		{
			// prepare OpenGL context (state, texture, etc.)

			if( isFullscreen )
			{
				[ _openGLContext setFullScreen ];
			}

			glDisable( GL_TEXTURE_2D );
			glDisable( GL_BLEND );
			glDisable( GL_ALPHA_TEST );
			glDisable( GL_DEPTH_TEST );
			glDisable( GL_LIGHTING );
			GL_CHECK_ERROR( glEnable( GL_TEXTURE_RECTANGLE_ARB ) );
			
			NSOpenGLContext* context = _openGLContext;

			// Clear the view, so we get a black background
			[ self clear ];
			[ context flushBuffer ];
			[ self clear ];
			[ context flushBuffer ];

			// enable/disable sync to VB
			GLint swapInterval = PIXELTOASTER_APPLE_VBSYNC;
			[ context setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval ];
		}

		- (void)updateDisplayID
		{
			NSScreen* screen = [ [ self window ] screen ];
	
			if( nil != screen )
			{
				NSDictionary* screenInfo		= [ screen deviceDescription ];
				NSNumber*			screenID			= [ screenInfo objectForKey:@"NSScreenNumber" ];
				displayID = (CGDirectDisplayID)[ screenID longValue ];
			}
		}

		- (void)update
		{
			if ( [ _openGLContext view ] == self )
			{
				[ _openGLContext update ];
				[ self updateDisplayID ];
			}
		}

		// In NSOpenGLView this is called automatically, for normal NSViews it is not,
		// which is why the sample code does not implement it, we just call it
		// manually when necessary
		- (void)reshape
		{	
			NSOpenGLContext* context = [ self openGLContext ];
		
			[ context makeCurrentContext ];
			[ context update ];

			NSRect drawableRect = [self convertRect:[self bounds] toView:nil];
							
			// calculate fillRect
			double drawableRatio  = NSWidth(drawableRect) / NSHeight(drawableRect);
			float displayRatio = (float)display->width() / (float)display->height();
	
			if( drawableRatio <= displayRatio )
			{
				// display is wider than screen, so we leave black borders at top and bottom (letterbox)
				double usedHeight = ceilf( NSWidth( drawableRect ) / displayRatio );
				[ self setFillRect: NSMakeRect( 0.0f, (NSHeight( drawableRect ) - usedHeight) / 2.0f,
																				NSWidth( drawableRect ), usedHeight ) ];
			}
			else
			{
				// screen is wider than display, leave black borders to the left and right
				double usedWidth = ceilf( NSHeight( drawableRect ) * displayRatio );
				[ self setFillRect: NSMakeRect( ( NSWidth( drawableRect ) - usedWidth ) / 2.0f, 0.0f,
																				usedWidth, NSHeight( drawableRect ) ) ];
			}

			glMatrixMode(GL_PROJECTION);	
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);	
			glLoadIdentity();

			glViewport( (GLint)fillRect.origin.x, (GLint)fillRect.origin.y,
									(GLsizei)fillRect.size.width, (GLsizei)fillRect.size.height);

			[ NSOpenGLContext clearCurrentContext ];
		}

		- (BOOL)setFullscreen:(BOOL)fullscreen
		{
			if( isFullscreen != fullscreen )
			{
				[ self clearGLContext ];
		
				if( fullscreen == YES )
				{
					CGRect rect = CGDisplayBounds( displayID );
					[ self setFrame: *(NSRect*)&rect ];
			
					#if !PIXELTOASTER_APPLE_DEBUG
						// Capture the display
						CGDisplayErr err = CGDisplayCapture( displayID );
						if( err != CGDisplayNoErr )
						{
							fprintf( stderr, "%s:%d:: -setFullscreen: could not capture display!\n", __FILE__, __LINE__);
							return NO;
						}

						#if PIXELTOASTER_APPLE_ALLOW_MODESWITCH
							//boolean_t exactMatch;
							CFDictionaryRef bestMode = CGDisplayBestModeForParameters( displayID, 32, width(), height(), 0/*&exactMatch*/ );
							if( bestMode )
								CGDisplaySwitchToMode ( displayID, bestMode );
						#endif
					#endif
				}
				else
				{
					// Release any captured display
					CGReleaseAllDisplays();
				}
		
				isFullscreen = fullscreen;
				[ self reshape ];
		
				if( nil == [ self openGLContext ] )
				{
					fprintf( stderr, "%s:%d:: -setFullscreen: could not get new context!\n", __FILE__, __LINE__ );
					return NO;
				}
			}

			return YES;
		}

		- (BOOL)fullscreen
		{
			return isFullscreen;
		}

		- (CGDirectDisplayID)displayID
		{
			return displayID;
		}

		- (void)lockFocus
		{
			NSOpenGLContext* context = [ self openGLContext ];
			[ super lockFocus ];
			if( [ context view ] != self)
			{
				[ context setView:self ];
			}
			[ context makeCurrentContext ];
		}

		-(void)viewDidMoveToWindow
		{
			[ super viewDidMoveToWindow ];
			if( [ self window ] == nil )
			{
				[ _openGLContext clearDrawable ];
			}
			else
			{
				[ self updateDisplayID ];
			}
		}

		- (void)setListener:(Listener*)listener
		{
			displayListener = listener;
		}

		- (Listener*)listener
		{
			return displayListener;
		}

		- (AppleDisplay*)display
		{
			return display;
		}

		- (void)setFillRect:(NSRect)newFillRect
		{
			fillRect = newFillRect;
		}

		- (NSRect)fillRect
		{
			return fillRect;
		}

		- (void)menuToggleFullscreen:(id)sender
		{
			PARAMETER_UNUSED(sender)
			if( display )
				display->setShouldToggle();
		}

		// resize window using a factor taken from the tag of the
		// menu item sending this message
		- (void)zoomX:(id)sender
		{	
			NSWindow *window	= [ self window ];
			int	zoomFactor		= (int)[ sender tag ];

			if( ( display != nil ) && ( window != nil ) )
			{
				NSRect contentRect = [ window contentRectForFrameRect:[ window frame ]];
				NSRect newContentRect = contentRect;

				newContentRect.size.width  = display->width()  * zoomFactor;
				newContentRect.size.height = display->height() * zoomFactor;
		
				// fix the y origin (else the window will keep it's *lower* right corner)
				newContentRect.origin.y -= (newContentRect.size.height - contentRect.size.height);
		
				[ window setFrame:[ window frameRectForContentRect: newContentRect ] display:YES];
				[ self reshape ];
			}
		}

		-(void)hideCursor
		{
			// the parameter to CGDisplayHideCursor is unused,
			// so we just pass main display
			CGDisplayHideCursor( kCGDirectMainDisplay );
			CGAssociateMouseAndMouseCursorPosition( false );
	
			// position the mouse cursor in the middle of the screen
			size_t	displayWidth	= CGDisplayPixelsWide( displayID );
			size_t	displayHeight = CGDisplayPixelsHigh( displayID );
			CGWarpMouseCursorPosition( CGPointMake( displayWidth / 2, displayHeight / 2 ) );
		}

		-(void)unhideCursor
		{
			// the parameter to CGDisplayShowCursor is unused,
			// so we just pass main display
			CGDisplayShowCursor( kCGDirectMainDisplay );
			CGAssociateMouseAndMouseCursorPosition( true );
		}

		-(void)fadeOutDisplay
		{
			#if !PIXELTOASTER_APPLE_DEBUG
				const float				FADE_INTERVAL = ( 1.0f / (float) FADE_STEPS );
				const useconds_t	SLEEP_TIME		= (useconds_t)( 1000000 * ( FADE_TIME / (double)FADE_STEPS ) );
		
				float	fade;
				CGGetDisplayTransferByFormula( displayID,
																			 &redMin,		&redMax,		&redGamma,
																			 &greenMin, &greenMax,	&greenGamma,
																			 &blueMin,	&blueMax,		&blueGamma );
				for( int step = 0; step < FADE_STEPS; ++step )
				{
					fade = 1.0f - ( step * FADE_INTERVAL );
					CGSetDisplayTransferByFormula( displayID,
																				 redMin,		fade * redMax,		redGamma,
																				 greenMin,	fade * greenMax,	greenGamma,
																				 blueMin,		fade * blueMax,		blueGamma );
					usleep( SLEEP_TIME );
				}
			#endif
		}

		-(void)fadeInDisplay
		{
			#if !PIXELTOASTER_APPLE_DEBUG
				const float				FADE_INTERVAL = ( 1.0f / (float) FADE_STEPS );
				const useconds_t	SLEEP_TIME		= (useconds_t)( 1000000 * ( FADE_TIME / (double)FADE_STEPS ) );
		
				float	fade;
				for( int step = 0; step < FADE_STEPS; ++step )
				{
					fade = step * FADE_INTERVAL;
					CGSetDisplayTransferByFormula( displayID,
																				 redMin,		fade * redMax,		redGamma,
																				 greenMin,	fade * greenMax,	greenGamma,
																				 blueMin,		fade * blueMax,		blueGamma);
					usleep( SLEEP_TIME );
				}
			#endif
			CGDisplayRestoreColorSyncSettings();
		}

		-(void)clear
		{
			glClearColor(0, 0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT);
			glFlush();
		}

		-(void)blitOpenGLTexture
		{
			if( glTexture == 0 )
				return;

			const	int	 width		= display->width();
			const int	 height		= display->height();
		
			[ [ self openGLContext ] makeCurrentContext ];

			glBegin( GL_QUADS ); 
				glTexCoord2i( 0,		 0			); glVertex2f( -1.0f,  1.0f ); 
				glTexCoord2i( width, 0			); glVertex2f(  1.0f,  1.0f ); 
				glTexCoord2i( width, height ); glVertex2f(  1.0f, -1.0f ); 
				glTexCoord2i( 0,		 height ); glVertex2f( -1.0f, -1.0f ); 
			glEnd();

			[ [ self openGLContext ] flushBuffer ];
	
			[ NSOpenGLContext clearCurrentContext ];
		}
		
		-(void)copyTrueColorPixelsUsingOpenGL:(const TrueColorPixel*)trueColorPixels
											floatingPointPixels:(const FloatingPointPixel*)floatingPointPixels
																dirtyRect:(const Rectangle*) dirtyBox
		{
			const	int	 width		= display->width();
			const int	 height		= display->height();

			bool useFloat = (floatingPointPixels != 0);

			GLint			x = 0;
			GLint			y = 0;
			GLsizei		w = width;
			GLsizei		h = height;
		
			const GLvoid*		bufferStart		= useFloat ? (GLvoid *)floatingPointPixels : (GLvoid *)trueColorPixels;
			unsigned char*	buffer				= (unsigned char*)bufferStart;
			const int				bytesPerPixel = useFloat ? 16 : 4;

			if( dirtyBox != 0 )
			{
				x = dirtyBox->xBegin;
				y = dirtyBox->yBegin;
				w = dirtyBox->xEnd - dirtyBox->xBegin;
				h = dirtyBox->yEnd - dirtyBox->yBegin;
				buffer += ( width * y + x ) * bytesPerPixel;
			}

			[ [ self openGLContext ] makeCurrentContext ];

			if( glTexture == 0 )
				[ self prepareTexture: bufferStart ];

			if( useFloat && !glHasFastFloats )
			{
				Converter*	converter;
				converter = requestConverter( Format::XBGRFFFF, Format::XRGB8888 );
				
				if( converter )
				{
					converter->begin();

					const Rectangle box = dirtyBox ? *dirtyBox : Rectangle( 0, width, 0, height );
					const int boxWidth = box.xEnd - box.xBegin;
			
					// already convert to coordinates (0, 0) of XRGB8888_buffer
					unsigned char *dest = XRGB8888_buffer;
					for ( int conv_y = box.yBegin; conv_y < box.yEnd; ++conv_y )
					{
						converter->convert( buffer, dest, boxWidth );
						buffer += ( bytesPerPixel * width );
						dest += ( width * 4 );
					}

					converter->end();
				}

				useFloat = false;
				buffer = XRGB8888_buffer;
			}

			GL_CHECK_ERROR( glTexSubImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, x, y, w, h,
																			 useFloat ? GL_RGBA	 : GL_BGRA_EXT,
																			 useFloat ? GL_FLOAT : GL_UNSIGNED_INT_8_8_8_8_REV,
																			 (GLvoid*)buffer) );
	
			glBegin( GL_QUADS ); 
				glTexCoord2i( 0,		 0			); glVertex2f( -1.0f,  1.0f ); 
				glTexCoord2i( width, 0			); glVertex2f(  1.0f,  1.0f ); 
				glTexCoord2i( width, height ); glVertex2f(  1.0f, -1.0f ); 
				glTexCoord2i( 0,		 height ); glVertex2f( -1.0f, -1.0f ); 
			glEnd();

			[ [ self openGLContext ] flushBuffer ];
	
			[ NSOpenGLContext clearCurrentContext ];
		}

		- (BOOL)isOpaque
		{
			return YES;
		}

		// dummy function to prevent beeps due to unhandled keyDown events
		- (void)keyDown:(NSEvent*)theEvent
		{
			PARAMETER_UNUSED(theEvent)
		}

		- (BOOL)acceptsFirstResponder
		{
			return YES;
		}

		- (void)windowDidResignKey:(NSNotification*)notification
		{
			PARAMETER_UNUSED(notification)
			if( displayListener && display )
				displayListener->onActivate( display->wrapper() ? *display->wrapper() : *display, false );

			// if the display is fullscreen, but the window lost the focus
			// switch to windowed mode (happens on Cmd-Shift-Q / Log out user..)
			if( display->output() == Output::Fullscreen )
				display->setShouldToggle();
		}

		- (void)windowDidBecomeKey:(NSNotification*)notification
		{
			PARAMETER_UNUSED(notification)
			if(displayListener && display)
				displayListener->onActivate( display->wrapper() ? *display->wrapper() : *display, true );
		}

		-(void)windowWillResize:(NSNotification *)notification
		{
			PARAMETER_UNUSED(notification)
			[ self reshape ];
			[ self blitOpenGLTexture ];
		}

		-(void)windowDidResize:(NSNotification *)notification
		{
			PARAMETER_UNUSED(notification)
			[ self reshape ];
			[ self blitOpenGLTexture ];
		}

		- (BOOL)windowShouldClose:(NSNotification *)notification
		{
			PARAMETER_UNUSED(notification)
			if( displayListener )
				return ( true == displayListener->onClose( display->wrapper() ? *display->wrapper() : *display ) );
			else
				return YES;
		}

		- (void)windowWillClose:(NSNotification *)notification
		{
			PARAMETER_UNUSED(notification)
			// if we are in fullscreen mode, release display, etc.
			if( isFullscreen )
				display->windowed();

			if( display )
			{
				// this needs to go first, as defaults() is called, which resets _shouldClose
				display->shutdown();
		
				display->setShouldClose();
			}
		}
	@end

	#pragma mark -
	#pragma mark C++ part
	#pragma mark -

	namespace PixelToaster
	{
		namespace internal
		{

			// Small helper class so we do not need to keep track of the NSAutoreleasePools
			class AutoreleasePoolAutoPtr
			{
			public:
				AutoreleasePoolAutoPtr()	{ pool = [ [ NSAutoreleasePool alloc] init ]; }				
				~AutoreleasePoolAutoPtr()	{	[ pool release ];	}
			private:
				NSAutoreleasePool *pool;
			};
		
			#pragma mark -

			// If NSApp is not set (which will be the case for most PixelToaster apps),
			// we need to create one and make it the foreground application.
			static void setupApplication()
			{
				if( NSApp == NULL )
				{
					AutoreleasePoolAutoPtr pool;

					// initialize application (also sets NSApp)
					PTApplication *app = (PTApplication *)[PTApplication sharedApplication];

					// make our process a foreground application, so we get a dock icon and a menu bar
					ProcessSerialNumber psn = { 0, kCurrentProcess };
					TransformProcessType(&psn, kProcessTransformToForegroundApplication);

					[ app setupApplicationMenu ];
				
					[ NSApp activateIgnoringOtherApps:YES ];
				}
			}
		} // namespace internal
		

		using namespace PixelToaster::internal;
	
		#pragma mark -
		class AppleDisplay::AppleDisplayPrivate
		{			
		public:
			AppleDisplayPrivate( AppleDisplay* theDisplay )
					: window(0), view(0),
						oldWindowFrame(NSZeroRect),
						display( theDisplay )
			{
			}
		
			void			setListener( Listener* newListener )
			{
				listener = newListener;
			}
		
			Listener*	getListener()	const
			{
				return listener;
			}
		
			void closeWindow()
			{
				if( window && ( display->output() == Output::Fullscreen ) )
					display->windowed();
		
				// close old window if it exists.. this will also release it
				// since the contentView is retained, it will not be deallocated
				[ window close ];
				window = nil;
			}
		
			#pragma mark -
			// -------------------------
			#pragma mark Mouse Event Handling
			// -------------------------

			void updateMouseCoordsFromEvent( NSEvent* theEvent )
			{
				NSRect fillRect = [ view fillRect ];
				const float widthRatio = display->width() / (float)NSWidth( fillRect );
				const float heightRatio = display->height() / (float)NSHeight( fillRect );

				if( display->output() == Output::Fullscreen )
				{
					CGMouseDelta deltaX, deltaY;
					CGGetLastMouseDelta(&deltaX, &deltaY);
				
					fullscreenMouseX += deltaX;
					fullscreenMouseY += deltaY;
				
					// basically PixelToaster allows exceeding screen coordinates, but
					// a) the first few CGGetLastMouseDelta() calls seem to deliver quite
					//    extreme values and
					// b) the Windows version clamps the values in fullscreen mode too
					if( fullscreenMouseX < 0 )
						fullscreenMouseX = 0;
					if( fullscreenMouseX >= (int)NSWidth( fillRect ) )
						fullscreenMouseX = (int)NSWidth( fillRect ) - 1;
					if( fullscreenMouseY < 0 )
						fullscreenMouseY = 0;
					if( fullscreenMouseY >= (int)NSHeight( fillRect ) )
						fullscreenMouseY = (int)NSHeight( fillRect ) - 1;
					
					// transform coordinates to native display
					mouse.x = fullscreenMouseX * widthRatio;
					mouse.y = fullscreenMouseY * heightRatio;
				}
				else
				{
					NSRect	frame					 = [ view frame ];
					NSPoint event_location = [ theEvent locationInWindow ];

					// flip y value, as Mac OS X counts from bottom to top,
					// while PixelToaster counts from top to bottom
					event_location.y = NSHeight(frame) - event_location.y;

					// transform coordinates to native display
					mouse.x = (float)( event_location.x - fillRect.origin.x ) * widthRatio;
					mouse.y = (float)( event_location.y - fillRect.origin.y ) * heightRatio;
				}
			}

			void handleMouseDown( NSEvent *theEvent, long button )
			{
				switch(button)
				{
					case 1:	ctrlWasPressed = ( 0 != ( [ theEvent modifierFlags ] & NSControlKeyMask ) );
									if( ctrlWasPressed )
										mouse.buttons.right = true;
									else
										mouse.buttons.left = true;
									break;
					case 2:	mouse.buttons.right = true;
									break;
					case 3:	
					default:
									mouse.buttons.middle = true;
									break;
				}
				updateMouseCoordsFromEvent( theEvent );
			
				if( listener )
					listener->onMouseButtonDown( display->wrapper() ? *display->wrapper() : *display, mouse );
			}

			void handleMouseUp( NSEvent* theEvent, long button )
			{
				switch( button )
				{
					case 1:	if( ctrlWasPressed )
										mouse.buttons.right = false;
									else
										mouse.buttons.left = false;
									break;
					case 2:	mouse.buttons.right = false;
									break;
					case 3:
					default:
									mouse.buttons.middle = false;
									break;
				}

				updateMouseCoordsFromEvent( theEvent );

				if( listener )
					listener->onMouseButtonUp( display->wrapper() ? *display->wrapper() : *display, mouse );
			}

			void handleMouseMoved( NSEvent* theEvent )
			{
				updateMouseCoordsFromEvent( theEvent );
				if( listener )
					listener->onMouseMove( display->wrapper() ? *display->wrapper() : *display, mouse );
			}

			#pragma mark -
			// -----------------------------
			#pragma mark Keyboard Event Handling
			// -----------------------------

			// Translates key event to PixelToaster::Key value.
			// I decided to map alphabetical keys using their "string", which should help
			// with international keymaps, but map special PixelToaster::Key values to key codes.
			// The X11 implementation uses the X11 keymap, which unfortunately leads to lots of
			// unmapped/dead keys and unreachable codes.
			static Key keyFromNSEvent( NSEvent* theEvent )
			{
				// NOTE: charactersIgnoringModifiers does not ignore "shift"
				NSString*				keyString = [theEvent charactersIgnoringModifiers];
				unsigned short	keyCode		= [theEvent keyCode];
				Key key;

				#if 0	// DEBUG
					if( [ keyString length ] > 0 )
					{
						unichar	ch2;
						[keyString getCharacters:&ch2 range:NSMakeRange(0, 1)];
						NSLog(@"key down = %d / 0x%x / %c\n", keyCode, keyCode, keyCode);
						NSLog(@"ignoringModifiers = '%@' char bytes = 0x%x\n", keyString, ch2);
					}
				#endif
			
				// translate some fixed keys
				switch( keyCode )
				{
					case 0x0A:	return Key::BackQuote;		// first key, second row (^ and ° on german keyboard)
					case 0x18:	return Key::Equals;				// key next to backspace
					case 0x1B:	return Key::Separator;		// key next to "0" (ß/? on german keyboard)
					case 0x21:	return Key::OpenBracket;	// key next to "P" (ü on german keyboard)
					case 0x1e:	return Key::CloseBracket;	// key next to OpenBracket ("+" on german keyboard)
					case 0x27:	return Key::Quote;				// key next to Enter (next to ö on german keyboard -> ä)
					case 0x29:	return Key::SemiColon;		// key next to "L" (ö on german keyboard)
					case 0x2A:	return Key::BackSlash;		// key next to CloseBracket (# on germany keyboard, one row below)

					case 0x1D:	return Key::Zero;
					case 0x12:	return Key::One;
					case 0x13:	return Key::Two;
					case 0x14:	return Key::Three;
					case 0x15:	return Key::Four;
					case 0x17:	return Key::Five;
					case 0x16:	return Key::Six;
					case 0x1A:	return Key::Seven;
					case 0x1C:	return Key::Eight;
					case 0x19:	return Key::Nine;

					case 0x2B:	return Key::Comma;				// last three keys above space, next to right shift
					case 0x2F:	return Key::Period;
					case 0x2C:	return Key::Slash;

					case 0x24:	return Key::Enter;
					case 0x30:	return Key::Tab;
					case 0x31:	return Key::Space;
					case 0x33:	return Key::BackSpace;
					case 0x35:	return Key::Escape;
				
					case 0x41:	return Key::Decimal;			// NumPad "."
					case 0x43:	return Key::Multiply;			// NumPad "*"
					case 0x45:	return Key::Add;					// NumPad "+"
					case 0x4B:	return Key::Divide;				// NumPad "/"
					case 0x4C:	return Key::Enter;				// NumPad Enter
					case 0x4E:	return Key::Subtract;			// NumPad "-"
				
					case 0x51:	return Key::Equals;				// NumPad "="
					case 0x52:	return Key::NumPad0;
					case 0x53:	return Key::NumPad1;
					case 0x54:	return Key::NumPad2;
					case 0x55:	return Key::NumPad3;
					case 0x56:	return Key::NumPad4;
					case 0x57:	return Key::NumPad5;
					case 0x58:	return Key::NumPad6;
					case 0x59:	return Key::NumPad7;
					case 0x5B:	return Key::NumPad8;
					case 0x5C:	return Key::NumPad9;

			#if 0
					TODO: these codes cannot be mapped?
						Clear 	 clear key
						Insert 	 insert key
						Cancel 	 cancel key
						Final 	 final key
						Convert 	 convert key
						NonConvert 	 non convert key
						Accept 	 accept key
						ModeChange mode change key						NSModeSwitchFunctionKey ?
					Undefined on german keyboards:
						"<" / ">" .. first key, last row, next to left shift
					Undefined on Mac Keyboards:
						NumPad "="	(aliased for now to Equals.. which matches for US keyboards, but not for international ones)
						fn					(NSFunctionKeyMask?)
			#endif

					default:		break;
				}
			
				// If we cannot get the unicode character either, bail so we do not get an exception on -getCharacters
				if( [ keyString length ] == 0 )
					return Key::Undefined;
			
				unichar	ch;
				[ keyString getCharacters: &ch range: NSMakeRange( 0, 1 ) ];

				if( ch <= 0x00FF )
				{
					// Normal keymap
					unsigned char ascii = ch;

					// catch non-capital letters
					if( ( ascii >= 'a' ) && ( ascii <= 'z' ) )
						return (Key::Code)( ascii - 0x20 );

					if( ( ascii >= 'A' ) && ( ascii <= 'Z' ) )
						return (Key::Code)ascii;
				}
				else
				{
					// Function keys
					switch (ch)
					{
						case NSF1FunctionKey:					return Key::F1;
						case NSF2FunctionKey:					return Key::F2;
						case NSF3FunctionKey:					return Key::F3;
						case NSF4FunctionKey:					return Key::F4;
						case NSF5FunctionKey:					return Key::F5;
						case NSF6FunctionKey:					return Key::F6;
						case NSF7FunctionKey:					return Key::F7;
						case NSF8FunctionKey:					return Key::F8;
						case NSF9FunctionKey:					return Key::F9;
						case NSF10FunctionKey:				return Key::F10;
						case NSF11FunctionKey:				return Key::F11;
						case NSF12FunctionKey:				return Key::F12;
					
						case NSDeleteFunctionKey:			return Key::Delete;
						case NSPageUpFunctionKey:			return Key::PageUp;
						case NSPageDownFunctionKey:		return Key::PageDown;
						case NSEndFunctionKey:				return Key::End;
						case NSHomeFunctionKey:				return Key::Home;
					
						// NumLock key is "clear line" on Mac
						case NSClearLineFunctionKey:	return Key::NumLock;
						case NSScrollLockFunctionKey:	return Key::ScrollLock;
						case NSPauseFunctionKey:			return Key::Pause;

						case NSPrintScreenFunctionKey:return Key::PrintScreen;
						case NSUpArrowFunctionKey:		return Key::Up;
						case NSDownArrowFunctionKey:	return Key::Down;
						case NSLeftArrowFunctionKey:	return Key::Left;
						case NSRightArrowFunctionKey:	return Key::Right;
					
						case NSHelpFunctionKey:				return Key::Help;

						default:											break;
					}
				}

				#if 0 // DEBUG
					NSLog(@"undefined key:\n");
					NSLog(@"key down = %d / 0x%x / %c\n", keyCode, keyCode, keyCode);
					NSLog(@"ignoringModifiers = '%@' char bytes = 0x%x\n", keyString, ch);
				#endif
			
				return Key::Undefined;
			}

			void handleKeyDown( NSEvent *theEvent )
			{
				bool defaultKeyHandlers = true;
				Key key = keyFromNSEvent( theEvent );
							
				if( listener )
				{
					if( ! [ theEvent isARepeat ] )
						listener->onKeyDown( display->wrapper() ? *display->wrapper() : *display, key );
						listener->onKeyPressed( display->wrapper() ? *display->wrapper() : *display, key );
					defaultKeyHandlers = listener->defaultKeyHandlers();
				}
			
				if( defaultKeyHandlers )
				{
					[ [ NSApp mainMenu ] performKeyEquivalent: theEvent ];

					if( ( key == Key::Escape ) && ( modifiers == 0 ) )
						[ window performClose:nil ];

					// TODO: instead of just leaving fullscreen: minimize and set a flag.. on app_activate, reenter fullscreen
					if( ( key == Key::Tab ) && ( modifiers == NSCommandKeyMask ) )
						display->setShouldToggle();
				}				
			}
		
			void handleKeyUp( NSEvent* theEvent )
			{
				Key key = keyFromNSEvent( theEvent );
				if( listener )
					listener->onKeyUp( display->wrapper() ? *display->wrapper() : *display, key );
			}

			// modifier status changed
			void handleFlagsChanged( NSEvent* theEvent )
			{
				unsigned long newModifiers			= ( [theEvent modifierFlags] & NSDeviceIndependentModifierFlagsMask );
				unsigned long changedModifiers	= newModifiers ^ modifiers;
				modifiers = newModifiers;

				if( listener == 0 )
					return;
			
				if( ( changedModifiers & NSAlphaShiftKeyMask ) != 0 )
				{
					if( ( newModifiers & NSAlphaShiftKeyMask ) != 0 )
						listener->onKeyDown( display->wrapper() ? *display->wrapper() : *display, Key::CapsLock );
					else
						listener->onKeyUp( display->wrapper() ? *display->wrapper() : *display, Key::CapsLock );
				}
			
				if( ( changedModifiers & NSCommandKeyMask ) != 0 )
				{
					if( ( newModifiers & NSCommandKeyMask ) != 0 )
						listener->onKeyDown( display->wrapper() ? *display->wrapper() : *display, Key::Meta );
					else
						listener->onKeyUp( display->wrapper() ? *display->wrapper() : *display, Key::Meta );
				}
			
				if( ( changedModifiers & NSShiftKeyMask ) != 0 )
				{
					if( ( newModifiers & NSShiftKeyMask ) != 0 )
						listener->onKeyDown( display->wrapper() ? *display->wrapper() : *display, Key::Shift );
					else
						listener->onKeyUp( display->wrapper() ? *display->wrapper() : *display, Key::Shift );

				}
			
				if( ( changedModifiers & NSControlKeyMask ) != 0 )
				{
					if( ( newModifiers & NSControlKeyMask ) != 0 )
						listener->onKeyDown( display->wrapper() ? *display->wrapper() : *display, Key::Control );
					else
						listener->onKeyUp( display->wrapper() ? *display->wrapper() : *display, Key::Control );
				}
			
				if( ( changedModifiers & NSAlternateKeyMask ) != 0 )
				{
					if( ( newModifiers & NSAlternateKeyMask ) != 0 )
						listener->onKeyDown( display->wrapper() ? *display->wrapper() : *display, Key::Alt );
					else
						listener->onKeyUp( display->wrapper() ? *display->wrapper() : *display, Key::Alt );
				}
			
				// TODO: NSFunctionKeyMask
			}

			void handleEvent( NSEvent *event )
			{
				NSEventType eventType		= [ event type ];
				NSWindow*		eventWindow	= [ event window ];
			
				AppleDisplayPrivate* receiver = this;
			
				// if this event was aimed at a window, try to find the
				// corresponding receiver
				if( 0 != eventWindow )
				{
					// the contentView may be nil in fullscreen mode,
					// but the delegate is set even in this case
					PixelToasterView* eventDelegate = [ eventWindow delegate ];
					if(eventDelegate && [ eventDelegate isKindOfClass: [ PixelToasterView class ] ] )
					{
						AppleDisplay* eventDisplay = [ eventDelegate display ];
						receiver = eventDisplay->_private;
					}
				}

				switch( eventType )
				{
					case NSLeftMouseDown:			receiver->handleMouseDown(event, 1);
																		// delegate event to app so activation events get handled
																		[ NSApp sendEvent: event ];
																		break;
					case NSLeftMouseUp:				receiver->handleMouseUp(event, 1);
																		[ NSApp sendEvent: event ];
																		break;
					case NSRightMouseDown:		receiver->handleMouseDown(event, 2); break;
					case NSRightMouseUp:			receiver->handleMouseUp(event, 2); break;
					case NSOtherMouseDown:		receiver->handleMouseDown(event, [event buttonNumber]); break;
					case NSOtherMouseUp:			receiver->handleMouseUp(event, [event buttonNumber]); break;

					case NSLeftMouseDragged:
					case NSRightMouseDragged:
					case NSOtherMouseDragged:
					case NSMouseMoved:				receiver->handleMouseMoved(event); break;

					case NSKeyDown:						receiver->handleKeyDown( event );	break;
					case NSKeyUp:							receiver->handleKeyUp( event ); break;
					case NSFlagsChanged:			receiver->handleFlagsChanged( event ); break;
					case NSScrollWheel:				break;	// TODO: if PixelToaster adds scrollwheel support..
					default:									[ NSApp sendEvent: event ];
																		break;
				}
			}

			// handle the event message queue
			void pumpMessages()
			{
				AutoreleasePoolAutoPtr pool;

				NSEvent* event;
				while( nil != (event = [ NSApp nextEventMatchingMask: NSAnyEventMask
																									 untilDate: [ NSDate distantPast ]	// do not block
																											inMode: NSDefaultRunLoopMode
																										 dequeue: YES ]) )
				{
					handleEvent( event );
				}
			};

			// current mouse status
			Mouse									mouse;
			int										fullscreenMouseX, fullscreenMouseY;

			// on mouseDown CTRL was pressed, so we sent a onRightMouse and need to
			// reset rightButton on mouseUp, no matter what the current modifiers say
			bool									ctrlWasPressed;
		
			unsigned long					modifiers;

			// output window
			NSWindow*							window;
			// custom content view
			PixelToasterView*			view;
		
			// position and size of normal window before switching to fullscreen
			NSRect								oldWindowFrame;
		
			Listener*							listener;
			AppleDisplay*					display;
		};	// class AppleDisplayPrivate

		#pragma mark -

		AppleDisplay::AppleDisplay()
		{
			AutoreleasePoolAutoPtr pool;
	
			defaults();
			
			_private = new AppleDisplayPrivate( this );
			
			if( NSApp == nil )
			{
				internal::setupApplication();
			}

			// create content view, it is resized to the correct values later
			NSRect viewRect = NSMakeRect(0.0f, 0.0f, 50.0f, 50.0f);
			NSOpenGLPixelFormat* format = [ PixelToasterView defaultPixelFormatForFullscreen: NO
																											 displayID: CGMainDisplayID() ];
			_private->view = [ [ PixelToasterView alloc ] initWithFrame: viewRect
																											pixelFormat: format
																													display: this ];
			[ _private->view setAutoresizingMask: NSViewWidthSizable | NSViewHeightSizable ];
		}

		AppleDisplay::~AppleDisplay()
		{
			AutoreleasePoolAutoPtr pool;
			close();

			if( _private )
			{
				[ _private->view release ];

				delete _private;
				_private = 0;
			}
		}

		static const float WINDOW_START_X = 50.0f;
		static const float WINDOW_START_Y = 50.0f;
		static const float WINDOW_OFFSET_Y = 25.0f;
		static const float WINDOW_OFFSET_X = 30.0f;
	
		#if PIXELTOASTER_APPLE_ALLOW_ZOOM
			static const unsigned int PIXELTOASTER_WINDOW_STYLE = NSTitledWindowMask | NSClosableWindowMask |
																														NSMiniaturizableWindowMask | NSResizableWindowMask;
		#else
			static const unsigned int PIXELTOASTER_WINDOW_STYLE = NSTitledWindowMask | NSClosableWindowMask |
			 																											NSMiniaturizableWindowMask;
		#endif
		
		bool AppleDisplay::open( const char title[], int width, int height, Output output, Mode mode )
		{
			// not using "output" in the next call to suppress unwanted fade operations
			DisplayAdapter::open( title, width, height, Output::Default, mode );
			bool result;

			static float winX = WINDOW_START_X, winY = -1.0f;
		
			AutoreleasePoolAutoPtr pool;

			NSScreen*	mainScreen = [ NSScreen mainScreen ];
			NSRect		screenRect = [ mainScreen frame ];
		
			// clamp cascading windows to screen size
			if( winX + width > NSWidth( screenRect ) )
			{
				winX = WINDOW_START_X;
				winY -= WINDOW_OFFSET_Y;
			}
			if( winY - height < 0 )
			{
				winY = NSHeight( screenRect ) - WINDOW_START_Y;
			}

			_private->oldWindowFrame = NSMakeRect( winX, winY - height, width, height );
			result = windowed();

			winX += WINDOW_OFFSET_X;
			winY -= WINDOW_OFFSET_Y;
		
			if (output == Output::Fullscreen)
			{
				result = fullscreen();
			}
		
			if ( result && ( 0L != DisplayAdapter::listener() ) )
			{
				DisplayAdapter::listener()->onOpen( wrapper() ? *wrapper() : *(DisplayInterface*)this );
				[ _private->view setListener:(DisplayAdapter::listener()) ];
			}

			return result;
		}

		// called when the window is closed by user interaction
		void AppleDisplay::shutdown()
		{
			[ _private->view clearGLContext ];

			// window and view will be autoreleased when closed, just clear pointers
			_private->window = nil;

			close();
		}

		void AppleDisplay::close()
		{
			AutoreleasePoolAutoPtr pool;
			[ _private->view clearGLContext ];
			_private->closeWindow();

			[ _private->view unhideCursor ];
			CGDisplayRestoreColorSyncSettings();

			DisplayAdapter::close();
		}

		bool AppleDisplay::update( const TrueColorPixel * trueColorPixels, const FloatingPointPixel * floatingPointPixels, const Rectangle * dirtyBox )
		{
			AutoreleasePoolAutoPtr pool;

			if( _shouldClose || ( nil == [ _private->view openGLContext ] ) )
			{
				close();
				return false;
			}

			if( _shouldToggle )
			{
				if( output() == Output::Fullscreen )
					windowed();
				else
					fullscreen();
			}
		
			if( ! [ NSApp isRunning ] )
			{
				[ NSApp finishLaunching ];
				[ (PTApplication*)NSApp setIsRunning: YES ];
				[ NSApp setWindowsNeedUpdate: YES ];
			}
		
			[ _private->view copyTrueColorPixelsUsingOpenGL: trueColorPixels
																	floatingPointPixels: floatingPointPixels
																						dirtyRect: _shouldToggle ? 0 : dirtyBox ];
						
			_shouldToggle = false;
		
			// If the window is still hidden, now is a good time to make it visible
			if( ! [ _private->window isVisible ] )
				[ _private->window makeKeyAndOrderFront: nil ];
						
			_private->pumpMessages();
				
			return true;
		}

		void AppleDisplay::title( const char title[] )
		{
			DisplayAdapter::title( title );

			AutoreleasePoolAutoPtr pool;

			[ _private->window setTitle:[ NSString stringWithCString: title encoding: NSUTF8StringEncoding ] ];
		}

		bool AppleDisplay::windowed()
		{
			if(! DisplayAdapter::open() )
				return false;

			const bool windowExists				 = ( 0 != _private->window );
			const bool outputWasFullscreen = ( output() == Output::Fullscreen );

			CGDirectDisplayID displayID = 0;
		
			if( windowExists && !outputWasFullscreen )
				return true;

			if ( outputWasFullscreen )
			{
				displayID = [ _private->view displayID ];
				[ _private->view fadeOutDisplay ];
				[ _private->window orderOut: nil ];
			}

			[ _private->view clearGLContext ];

			NSWindow*	window	= _private->window;
 
			if( ( NSWidth( _private->oldWindowFrame ) == 0 ) ||
					( NSHeight( _private->oldWindowFrame ) == 0 ) )
			{
				// if the program started in fullscreen mode, we do not yet
				// have a window frame, just make one up..
				_private->oldWindowFrame = NSMakeRect( WINDOW_START_X, WINDOW_START_Y,
																							 width(), height() );
			}
		
			if( window == nil )
			{
				window = [ [ NSWindow alloc ] initWithContentRect: _private->oldWindowFrame
																								styleMask: PIXELTOASTER_WINDOW_STYLE
																									backing: NSBackingStoreBuffered
																										defer: NO ];

				NSString* title = [ NSString stringWithCString: DisplayAdapter::title()
																							encoding: NSUTF8StringEncoding ];

				[ window setIgnoresMouseEvents: NO ];
				[ window setAcceptsMouseMovedEvents: YES ];
				[ window setInitialFirstResponder: _private->view ];
				[ window setTitle: title ];
				[ window setReleasedWhenClosed: YES ];
				[ window setOneShot: NO ];
				[ window setCanHide: YES ];
				[ window setContentAspectRatio: _private->oldWindowFrame.size ];
				[ window setDelegate: _private->view ];
				_private->window	= window;
			}
			else
			{
				NSRect frameRect = [ window frameRectForContentRect: _private->oldWindowFrame ];
				[ window setFrame: frameRect display: NO ];
			}
			[ window setContentView: _private->view ];
			
			DisplayAdapter::windowed();
		
			if( [ _private->view setFullscreen:NO ] == NO )
			{
				close();
				return false;
			}

			NSOpenGLContext *context = [ _private->view openGLContext ];

			[ context setView: _private->view ];

			// fade in display, note that the window is still hidden
			// it will be made visible after the first update() call
			if ( outputWasFullscreen )
				[ _private->view fadeInDisplay ];

			[ _private->view unhideCursor ];

			return true;
		}
		
		bool AppleDisplay::fullscreen()
		{
			if(! DisplayAdapter::open() )
				return false;
		
			if( output() == Output::Fullscreen )
				return true;
			
			[ _private->view hideCursor ];
			[ _private->view fadeOutDisplay ];
		
			// if a window already exists, save frame to restore later
			if( _private->window != nil )
			{
				_private->oldWindowFrame = [ _private->window contentRectForFrameRect: [ _private->window frame ] ];
				[ _private->window setContentView: nil ];
				// make window smaller so it does not reach into still visible screens
				[ _private->window setFrame: NSMakeRect( 0.0f, 0.0f, 50.0f, 50.0f )
														display: NO ];
			}
			else
			{
				_private->oldWindowFrame = NSZeroRect;
			}

			// set state to fullscreen
			DisplayAdapter::fullscreen();

			if( NO == [ _private->view setFullscreen: YES ] )
			{
				close();
				return false;
			}

			// for now: instead of fade in, just reset the gamma settings (we have a black screen anyway)
			// TODO: correct solution for later would be to do the fade in asynchronously
			CGDisplayRestoreColorSyncSettings();

			return true;
		}
	
		void AppleDisplay::listener( Listener * aListener )
		{
			DisplayAdapter::listener( aListener );
			if( _private )
			{
				_private->setListener( aListener );
				[ _private->view setListener:aListener ];
			}
		}
	
		void AppleDisplay::defaults()
		{
			DisplayAdapter::defaults();
			_shouldClose = false;
			_shouldToggle = false;
		}

	} // namespace PixelToaster

#endif // !PIXELTOASTER_APPLE_USE_X11