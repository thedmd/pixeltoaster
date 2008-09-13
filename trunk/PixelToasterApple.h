// Apple MacOS X Platform
// Copyright © 2004-2007 Glenn Fiedler
// Part of the PixelToaster Framebuffer Library - http://www.pixeltoaster.com

// native Cocoa output implemented by Thorsten Schaaps <bitpull@aixplosive.de>

#ifndef PIXELTOASTER_APPLE_USE_X11
	#define PIXELTOASTER_APPLE_USE_X11	0
#endif

#include "CoreServices/CoreServices.h"

#if PIXELTOASTER_APPLE_USE_X11
	#define PIXELTOASTER_NO_UNIX_TIMER
	#include "PixelToasterUnix.h"
#endif

// display implementation
namespace PixelToaster
{
	#if !PIXELTOASTER_APPLE_USE_X11		
		class AppleDisplay : public DisplayAdapter
		{
			class AppleDisplayPrivate;
			
		public:
			AppleDisplay();
			
			virtual ~AppleDisplay();
			
			virtual bool open( const char title[],
												 int width, int height,
												 Output output,
												 Mode mode );
			
			virtual void close();
			
			virtual bool update( const TrueColorPixel *			trueColorPixels,
													 const FloatingPointPixel * floatingPointPixels,
													 const Rectangle *					dirtyBox );
			
			virtual void title( const char title[] );
			
			virtual bool windowed();
			
			virtual bool fullscreen();

			virtual void listener( Listener * listener );
			
			void setShouldClose()  { _shouldClose = true; }

			void setShouldToggle() { _shouldToggle = true; }

			void shutdown();

		protected:
			
			virtual void defaults();

		private:

			AppleDisplayPrivate*	_private;
			bool									_shouldClose;
			bool									_shouldToggle;
		}; 
	#else
		class AppleDisplay : public UnixDisplay 
		{
			// ...
		}; 
	#endif

	// timer implementation
	class AppleTimer : public TimerInterface
	{
	public:
	
		AppleTimer()
		{
			reset();
		}
	
		void reset()
		{
			Microseconds( (UnsignedWide*) &_timeCounter );
			_deltaCounter = _timeCounter;
			_time = 0;
		}
	
		double time()
		{
			UInt64 counter;
			Microseconds( (UnsignedWide*) &counter );
			UInt64 delta = counter - _timeCounter;
			_timeCounter = counter;
			_time += delta / 1000000.0;
			return _time;
		}
	
		double delta()
		{
			UInt64 counter;
			Microseconds( (UnsignedWide*) &counter );
			UInt64 delta = counter - _deltaCounter;
			_deltaCounter = counter;
			return delta / 1000000.0;
		}
	
		double resolution()
		{
			return 1.0 / 1000000.0;		// microseconds
		}
	
		void wait( double seconds )
		{
			UInt64 counter;
			Microseconds( (UnsignedWide*) &counter );
			UInt64 finish = counter + UInt64( seconds*1000000 );
			while  ( counter < finish )
				Microseconds( (UnsignedWide*) &counter );
		}
	
	private:
	
		double _time;               ///< current time in seconds
		UInt64 _timeCounter;        ///< time counter in microseconds
		UInt64 _deltaCounter;		///< delta counter in microseconds
	};
}
