// Unix Platform
// Copyright © 2004-2007 Glenn Fiedler
// Part of the PixelToaster Framebuffer Library - http://www.pixeltoaster.com

// X11 version implemented by Bram de Greve <bramz@users.sourceforge.net>
// inspired by X11 version of TinyPTC by Alessandro Gatti <a.gatti@tiscali.it>

// unix display implementation

#define XK_LATIN1
#define XK_MISCELLANY

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>

namespace PixelToaster
{
	template <typename T>
	class DirtyVector
	{
	public:
		explicit DirtyVector(size_t size = 0) { data_ = size == 0 ? NULL : (static_cast<T*>(malloc(size * sizeof(T)))); }
		~DirtyVector() { if (data_!=NULL) { free(data_); data_ = NULL; } }
		void reset(size_t size = 0) { DirtyVector<T> temp(size); swap(temp); }
		T& operator[](size_t i) { assert(data_); return data_[i]; }
		T* get() { return data_; }
		bool isEmpty() const { return data_ == NULL; }
		void swap(DirtyVector& other) { T* temp = data_; data_ = other.data_; other.data_ = temp; }
	private:
		T* data_;	
	};

	Format findFormat(int bitsPerPixel, unsigned long redMask, unsigned long greenMask, unsigned long blueMask)
	{
		switch (bitsPerPixel)
		{
		case 16:
			if (redMask == 0x7c00 && greenMask == 0x03e0 && blueMask == 0x001f)
				return Format::XRGB1555;
			if (redMask == 0x001f && greenMask == 0x03e0 && blueMask == 0x7c00)
				return Format::XBGR1555;
			if (redMask == 0xf800 && greenMask == 0x07e0 && blueMask == 0x001f)
				return Format::RGB565;
			if (redMask == 0x001f && greenMask == 0x07e0 && blueMask == 0xf800)
				return Format::BGR565;
			break;
		case 24:
			if (redMask == 0xff0000 && greenMask == 0x00ff00 && blueMask == 0x0000ff)
				return Format::RGB888;
			if (redMask == 0x0000ff && greenMask == 0x00ff00 && blueMask == 0xff0000)
				return Format::BGR888;
			break;
		case 32:
			if (redMask == 0xff0000 && greenMask == 0x00ff00 && blueMask == 0x0000ff)
				return Format::XRGB8888;
			if (redMask == 0x0000ff && greenMask == 0x00ff00 && blueMask == 0xff0000)
				return Format::XBGR8888;
			break;
		}
		return Format::Unknown;
	}

	class UnixDisplay : public DisplayAdapter
	{
	public:
	
		UnixDisplay()
		{
			defaults();
		}

		bool open( const char title[], int width, int height, Output output, Mode mode )
		{
			DisplayAdapter::open( title, width, height, output, mode );

			// let's open a display
		
			display_ = ::XOpenDisplay(0);
			if (!display_)
			{
				close();
				return false;
			}

			const int screen = DefaultScreen(display_);
			::Visual* visual = DefaultVisual(display_, screen);
			if (!visual)
			{
				close();
				return false;
			}

			// It gets messy when talking about color depths.
			//
			// For the image buffer, we either need 8, 16 or 32 bitsPerPixel.  8 bits we'll 
			// never have (hopefully), 16 bits will be used for displayDepth 15 & 16, and 
			// 32 bits must be used for depths 24 and 32.
			//
			// The converters will get this right when talking about displayDepth 15 & 16, but 
			// it will wrongly assume that displayDepth 24 takes _exactly_ 24 bitsPerPixel.  We 
			// solve that by tricking the converter requester by presenting it a 32 bit
			// bufferDepth instead.
			//
			const int displayDepth = DefaultDepth(display_, screen);
			const int bufferDepth = displayDepth == 24 ? 32 : displayDepth;
			const int bytesPerPixel = (bufferDepth + 7) / 8;
			const int bitsPerPixel = 8 * bytesPerPixel;
			if (bitsPerPixel != 16 && bitsPerPixel != 32)
			{
				close();
				return false;
			}

			destFormat_ = findFormat(bufferDepth,
				visual->red_mask, visual->green_mask, visual->blue_mask);
			floatingPointConverter_ = requestConverter(Format::XBGRFFFF, destFormat_);
			trueColorConverter_ = requestConverter(Format::XRGB8888, destFormat_);
			if (!floatingPointConverter_ || !trueColorConverter_)
			{
				close();
				return false;
			}

			// let's create a window
		
			const Window root = DefaultRootWindow(display_);
		
			const int screenWidth = DisplayWidth(display_, screen);
			const int screenHeight = DisplayHeight(display_, screen);
			const int left = (screenWidth - width) / 2;
			const int top = (screenHeight - height) / 2;

			::XSetWindowAttributes attributes;
			attributes.border_pixel = attributes.background_pixel = BlackPixel(display_, screen);
			attributes.backing_store = NotUseful;

			window_ = ::XCreateWindow(display_, root, left, top, width, height, 0,
				displayDepth, InputOutput, visual, 
				CWBackPixel | CWBorderPixel | CWBackingStore, &attributes);

		
			::XStoreName(display_, window_, title);

			wmProtocols_ = XInternAtom(display_, "WM_PROTOCOLS", True);
			wmDeleteWindow_ = XInternAtom(display_, "WM_DELETE_WINDOW", True);
			if (wmProtocols_ == 0 || wmDeleteWindow_ == 0)
			{
				close();
				return false;
			}
			if (::XSetWMProtocols(display_, window_, &wmDeleteWindow_, 1) == 0)
			{
				close();
				return false;
			}

			::XSizeHints sizeHints;
			sizeHints.flags = PPosition | PMinSize | PMaxSize;
			sizeHints.x = sizeHints.y = 0;
			sizeHints.min_width = sizeHints.max_width = width;
			sizeHints.min_height = sizeHints.max_height = height;
			::XSetNormalHints(display_, window_, &sizeHints);
			::XClearWindow(display_, window_);
			::XSelectInput(display_, window_, eventMask_);

			// create (image) buffer

			buffer_.reset(width * height * bytesPerPixel);
			if (buffer_.isEmpty())
			{
				close();
				return false;
			}

			gc_ = DefaultGC(display_, screen);
			image_ = ::XCreateImage(display_, CopyFromParent, displayDepth, ZPixmap, 0, 0,
				width, height, bitsPerPixel, width * bytesPerPixel);
	#if defined(PIXELTOASTER_LITTLE_ENDIAN)
			image_->byte_order = LSBFirst;
	#else
			image_->byte_order = MSBFirst;
	#endif	
			if (!image_)
			{
				close();
				return false;
			}
		
			// we have a winner!

			::XMapRaised(display_, window_);
			::XFlush(display_);

			if ( DisplayAdapter::listener() )
				DisplayAdapter::listener()->onOpen(wrapper() ? *wrapper() : *(DisplayInterface*)this);

			return true;
		}
	
		void close()
		{	
			if (image_)
			{
				XDestroyImage(image_);
				image_ = 0;
			}

			if (display_ && window_)
			{
				XDestroyWindow(display_, window_);
				window_ = 0;
			}
			
			if (display_)
			{
				XCloseDisplay(display_);
				display_ = 0;
			}

			DisplayAdapter::close();			// note: this calls our virtual defaults method
		}

		bool update( const TrueColorPixel * trueColorPixels, const FloatingPointPixel * floatingPointPixels, const Rectangle * dirtyBox )
		{
			if (isShuttingDown_)
			{
				close();
				return false;
			}
		
			if (!display_ || !window_ || !image_)
				return false;

			const int w = width();
			const int h = height();
			const int size = w * h;

			const bool shortcut = trueColorPixels != NULL && destFormat_ == Format::XRGB8888;
		
			if ( !shortcut )
			{
				// extra conversion step: copy pixels to buffer

				if (trueColorPixels)
					trueColorConverter_->convert(trueColorPixels, buffer_.get(), size);
				else if (floatingPointPixels)
					floatingPointConverter_->convert(floatingPointPixels, buffer_.get(), size);
				else
					return false;

				image_->data = buffer_.get();
			}
			else
			{
				// shortcut: avoid extra copy - only works for truecolor pixels
			
				image_->data = (char*) trueColorPixels;			
			}
		
			::XPutImage(display_, window_, gc_, image_, 0, 0, 0, 0, w, h);
			::XFlush(display_);
		
			image_->data = NULL;

			pumpEvents();

			return true;
		}
		
		void title( const char title[] )
		{
			DisplayAdapter::title(title);
			
			if (display_ && window_)
				::XStoreName(display_, window_, title);
		}

	protected:

		void defaults()
		{
			DisplayAdapter::defaults();
		
			display_ = 0;
			window_ = 0;
			gc_ = 0;
			image_ = 0;
			buffer_.reset();
			trueColorConverter_ = 0;
			floatingPointConverter_ = 0;
			isShuttingDown_ = false;
			Format destFormat_ = Format::Unknown;		
		}

	private:

		enum 
		{ 
			eventMask_ = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ButtonMotionMask,
			keyMapSize_ = 256
		};

		typedef DirtyVector<char> TBuffer;
		typedef Key::Code TKeyMap[keyMapSize_];
		typedef bool TKeyFlags[keyMapSize_];

		void pumpEvents()
		{
			::XEvent event;
			while (true)
			{		
				if (::XCheckWindowEvent(display_, window_, -1, &event))
				{
					handleEvent(event);
				}
				else if (::XCheckTypedEvent(display_, ClientMessage, &event))
				{
					handleEvent(event);
				}
				else
				{
					break;
				}
			}
		
			// send key press and up events
		
			for (int i = 0; i < keyMapSize_; ++i)
			{
				if (keyIsReleased_[i] && keyIsPressed_[i])
				{
					if (listener()) listener()->onKeyUp(wrapper() ? *wrapper() : *(DisplayInterface*)this,static_cast<Key::Code>(i));
					keyIsPressed_[i] = false;
					keyIsReleased_[i] = false;
				}
				else if (keyIsPressed_[i])
				{
					if (listener()) listener()->onKeyPressed(wrapper() ? *wrapper() : *(DisplayInterface*)this,static_cast<Key::Code>(i));
				}
			}		
		}

		void handleEvent(const ::XEvent& event)
		{
			switch (event.type)
			{
				case KeyPress:
				case KeyRelease:
				{
					const KeySym keySym = ::XKeycodeToKeysym(display_, event.xkey.keycode, 0);
					const int hiSym = (keySym & 0xff00) >> 8;
					const int loSym = keySym & 0xff;

					Key key = Key::Undefined;
					switch (hiSym)
					{
						case 0x00:
							key = normalKeys_[loSym];
							break;
						case 0xff:
							key = functionKeys_[loSym];
							break;
					}

					if (event.type == KeyPress)
					{
						if (!keyIsPressed_[key])
						{
							bool defaultKeyHandlers = true;

							if (listener())
							{
								listener()->onKeyDown(wrapper() ? *wrapper() : *(DisplayInterface*)this,key);
								defaultKeyHandlers = listener()->defaultKeyHandlers();
							}

							if (defaultKeyHandlers && key == Key::Escape)
							{
								isShuttingDown_ = true;
							}
						
						}
						keyIsPressed_[key] = true;
						keyIsReleased_[key] = false;
					}
					else
					{
						keyIsReleased_[key] = true;
					}
					break;
				}
			
				case ButtonPress:
				case ButtonRelease:
				{
					Mouse mouse;
					mouse.x = static_cast<float>(event.xbutton.x);
					mouse.y = static_cast<float>(event.xbutton.y);
					mouse.buttons.left = event.xbutton.button == Button1;
					mouse.buttons.middle = event.xbutton.button == Button2;
					mouse.buttons.right = event.xbutton.button == Button3;
					if (event.type == ButtonPress)
					{
						if (listener()) listener()->onMouseButtonDown(wrapper() ? *wrapper() : *(DisplayInterface*)this,mouse);
					}
					else
					{
						if (listener()) listener()->onMouseButtonUp(wrapper() ? *wrapper() : *(DisplayInterface*)this,mouse);
					}
					break;
				}
				case MotionNotify:
				{
					Mouse mouse;
					mouse.x = static_cast<float>(event.xmotion.x);
					mouse.y = static_cast<float>(event.xmotion.y);
					mouse.buttons.left = event.xmotion.state & Button1Mask;
					mouse.buttons.middle = event.xmotion.state & Button2Mask;
					mouse.buttons.right = event.xmotion.state & Button3Mask;
					if (listener()) listener()->onMouseMove(wrapper() ? *wrapper() : *(DisplayInterface*)this,mouse);
					break;
				}
				case ClientMessage:
				{
					if (event.xclient.message_type == wmProtocols_ && 
						event.xclient.format == 32 &&
						event.xclient.data.l[0] == (long) wmDeleteWindow_)
					{
						if (listener()) 
						{
							if (listener()->onClose(wrapper() ? *wrapper() : *(DisplayInterface*)this))
								isShuttingDown_ = true;
						}
						else
						{
							isShuttingDown_ = true;
						}
					}
					break;
				}
			}
		}

		static bool initializeKeyMaps()
		{
			for (int i = 0; i < keyMapSize_; ++i)
			{
				normalKeys_[i] = Key::Undefined;
				functionKeys_[i] = Key::Undefined;
				keyIsPressed_[i] = false;
				keyIsReleased_[i] = false;
			}
		
			normalKeys_[XK_space] = Key::Space;
			normalKeys_[XK_comma] = Key::Comma;
			normalKeys_[XK_period] = Key::Period;
			normalKeys_[XK_slash] = Key::Slash;
			normalKeys_[XK_0] = Key::Zero;
			normalKeys_[XK_1] = Key::One;
			normalKeys_[XK_2] = Key::Two;
			normalKeys_[XK_3] = Key::Three;
			normalKeys_[XK_4] = Key::Four;
			normalKeys_[XK_5] = Key::Five;
			normalKeys_[XK_6] = Key::Six;
			normalKeys_[XK_7] = Key::Seven;
			normalKeys_[XK_8] = Key::Eight;
			normalKeys_[XK_9] = Key::Nine;
			normalKeys_[XK_semicolon] = Key::SemiColon;
			normalKeys_[XK_equal] = Key::Equals;
			normalKeys_[XK_a] = Key::A;
			normalKeys_[XK_b] = Key::B;
			normalKeys_[XK_c] = Key::C;
			normalKeys_[XK_d] = Key::D;
			normalKeys_[XK_e] = Key::E;
			normalKeys_[XK_f] = Key::F;
			normalKeys_[XK_g] = Key::G;
			normalKeys_[XK_h] = Key::H;
			normalKeys_[XK_i] = Key::I;
			normalKeys_[XK_j] = Key::J;
			normalKeys_[XK_k] = Key::K;
			normalKeys_[XK_l] = Key::L;
			normalKeys_[XK_m] = Key::M;
			normalKeys_[XK_n] = Key::N;
			normalKeys_[XK_o] = Key::O;
			normalKeys_[XK_p] = Key::P;
			normalKeys_[XK_q] = Key::Q;
			normalKeys_[XK_r] = Key::R;
			normalKeys_[XK_s] = Key::S;
			normalKeys_[XK_t] = Key::T;
			normalKeys_[XK_u] = Key::U;
			normalKeys_[XK_v] = Key::V;
			normalKeys_[XK_w] = Key::W;
			normalKeys_[XK_x] = Key::X;
			normalKeys_[XK_y] = Key::Y;
			normalKeys_[XK_z] = Key::Z;
			normalKeys_[XK_bracketleft] = Key::OpenBracket;
			normalKeys_[XK_backslash] = Key::BackSlash;
			normalKeys_[XK_bracketright] = Key::CloseBracket;
		
			functionKeys_[0xff & XK_BackSpace] = Key::BackSpace;
			functionKeys_[0xff & XK_Tab] = Key::Tab;
			functionKeys_[0xff & XK_Linefeed] = Key::Undefined;
			functionKeys_[0xff & XK_Clear] = Key::Clear;
			functionKeys_[0xff & XK_Return] = Key::Enter;
			functionKeys_[0xff & XK_Pause] = Key::Pause;
			functionKeys_[0xff & XK_Scroll_Lock] = Key::ScrollLock;
			functionKeys_[0xff & XK_Sys_Req] = Key::PrintScreen;
			functionKeys_[0xff & XK_Escape] = Key::Escape;
			functionKeys_[0xff & XK_Delete] = Key::Delete;
			functionKeys_[0xff & XK_Kanji] = Key::Kanji;
			functionKeys_[0xff & XK_Kana_Shift] = Key::Kana;
			functionKeys_[0xff & XK_Home] = Key::Home;
			functionKeys_[0xff & XK_Left] = Key::Left;
			functionKeys_[0xff & XK_Up] = Key::Up;
			functionKeys_[0xff & XK_Right] = Key::Right;
			functionKeys_[0xff & XK_Down] = Key::Down;
			functionKeys_[0xff & XK_Prior] = Key::Undefined;
			functionKeys_[0xff & XK_Page_Up] = Key::PageUp;
			functionKeys_[0xff & XK_Next] = Key::Undefined;
			functionKeys_[0xff & XK_Page_Down] = Key::PageDown;
			functionKeys_[0xff & XK_End] = Key::End;
			functionKeys_[0xff & XK_Begin] = Key::Undefined;
			functionKeys_[0xff & XK_Select] = Key::Undefined;
			functionKeys_[0xff & XK_Print] = Key::Undefined;
			functionKeys_[0xff & XK_Execute] = Key::Undefined;
			functionKeys_[0xff & XK_Insert] = Key::Insert;
			functionKeys_[0xff & XK_Undo] = Key::Undefined;
			functionKeys_[0xff & XK_Redo] = Key::Undefined;
			functionKeys_[0xff & XK_Menu] = Key::Undefined;
			functionKeys_[0xff & XK_Find] = Key::Undefined;
			functionKeys_[0xff & XK_Cancel] = Key::Cancel;
			functionKeys_[0xff & XK_Help] = Key::Help;
			functionKeys_[0xff & XK_Break] = Key::Undefined;
			functionKeys_[0xff & XK_Mode_switch] = Key::ModeChange;
			functionKeys_[0xff & XK_Num_Lock] = Key::NumLock;
			functionKeys_[0xff & XK_KP_Space] = Key::Space;
			functionKeys_[0xff & XK_KP_Tab] = Key::Tab;
			functionKeys_[0xff & XK_KP_Enter] = Key::Enter;
			functionKeys_[0xff & XK_KP_F1] = Key::F1;
			functionKeys_[0xff & XK_KP_F2] = Key::F2;
			functionKeys_[0xff & XK_KP_F3] = Key::F3;
			functionKeys_[0xff & XK_KP_F4] = Key::F4;
			functionKeys_[0xff & XK_KP_Home] = Key::Home;
			functionKeys_[0xff & XK_KP_Left] = Key::Left;
			functionKeys_[0xff & XK_KP_Right] = Key::Right;
			functionKeys_[0xff & XK_KP_Down] = Key::Down;
			functionKeys_[0xff & XK_KP_Prior] = Key::Undefined;
			functionKeys_[0xff & XK_KP_Page_Up] = Key::PageUp;
			functionKeys_[0xff & XK_KP_Next] = Key::Undefined;
			functionKeys_[0xff & XK_KP_Page_Down] = Key::PageDown;
			functionKeys_[0xff & XK_KP_End] = Key::End;
			functionKeys_[0xff & XK_KP_Begin] = Key::Undefined;
			functionKeys_[0xff & XK_KP_Insert] = Key::Insert;
			functionKeys_[0xff & XK_KP_Delete] = Key::Delete;
			functionKeys_[0xff & XK_KP_Equal] = Key::Equals;
			functionKeys_[0xff & XK_KP_Multiply] = Key::Multiply;
			functionKeys_[0xff & XK_KP_Add] = Key::Add;
			functionKeys_[0xff & XK_KP_Separator] = Key::Separator;
			functionKeys_[0xff & XK_KP_Subtract] = Key::Subtract;
			functionKeys_[0xff & XK_KP_Decimal] = Key::Decimal;
			functionKeys_[0xff & XK_KP_Divide] = Key::Divide;
			functionKeys_[0xff & XK_KP_0] = Key::NumPad0;
			functionKeys_[0xff & XK_KP_1] = Key::NumPad1;
			functionKeys_[0xff & XK_KP_2] = Key::NumPad2;
			functionKeys_[0xff & XK_KP_3] = Key::NumPad3;
			functionKeys_[0xff & XK_KP_4] = Key::NumPad4;
			functionKeys_[0xff & XK_KP_5] = Key::NumPad5;
			functionKeys_[0xff & XK_KP_6] = Key::NumPad6;
			functionKeys_[0xff & XK_KP_7] = Key::NumPad7;
			functionKeys_[0xff & XK_KP_8] = Key::NumPad8;
			functionKeys_[0xff & XK_KP_9] = Key::NumPad9;
			functionKeys_[0xff & XK_F1] = Key::F1;
			functionKeys_[0xff & XK_F2] = Key::F2;
			functionKeys_[0xff & XK_F3] = Key::F3;
			functionKeys_[0xff & XK_F4] = Key::F4;
			functionKeys_[0xff & XK_F5] = Key::F5;
			functionKeys_[0xff & XK_F6] = Key::F6;
			functionKeys_[0xff & XK_F7] = Key::F7;
			functionKeys_[0xff & XK_F8] = Key::F8;
			functionKeys_[0xff & XK_F9] = Key::F9;
			functionKeys_[0xff & XK_F10] = Key::F10;
			functionKeys_[0xff & XK_F11] = Key::F11;
			functionKeys_[0xff & XK_F12] = Key::F12;
			functionKeys_[0xff & XK_Shift_L] = Key::Shift;
			functionKeys_[0xff & XK_Shift_R] = Key::Shift;
			functionKeys_[0xff & XK_Control_L] = Key::Control;
			functionKeys_[0xff & XK_Control_R] = Key::Control;
			functionKeys_[0xff & XK_Caps_Lock] = Key::CapsLock;
			functionKeys_[0xff & XK_Shift_Lock] = Key::CapsLock;
			functionKeys_[0xff & XK_Meta_L] = Key::Meta;
			functionKeys_[0xff & XK_Meta_R] = Key::Meta;
			functionKeys_[0xff & XK_Alt_L] = Key::Alt;
			functionKeys_[0xff & XK_Alt_R] = Key::Alt;
			return true;
		}

		::Display* display_;
		::Window window_;
		::GC gc_;
		::XImage* image_;
		TBuffer buffer_;
		Converter* trueColorConverter_;
		Converter* floatingPointConverter_;
		bool isShuttingDown_;
		Format destFormat_;
		Atom wmProtocols_;
		Atom wmDeleteWindow_;
	
		static TKeyMap normalKeys_;
		static TKeyMap functionKeys_;
		static TKeyFlags keyIsPressed_;
		static TKeyFlags keyIsReleased_;
		static bool keyMapsInitialized_;
	};

	UnixDisplay::TKeyMap UnixDisplay::normalKeys_;
	UnixDisplay::TKeyMap UnixDisplay::functionKeys_;
	UnixDisplay::TKeyFlags UnixDisplay::keyIsPressed_;
	UnixDisplay::TKeyFlags UnixDisplay::keyIsReleased_;
	bool UnixDisplay::keyMapsInitialized_ = UnixDisplay::initializeKeyMaps();
}

// unix timer implementation

#ifndef PIXELTOASTER_NO_UNIX_TIMER

#include <time.h>
#include <errno.h>
#include <math.h>

//#define PIXELTOASTER_RDTSC
#ifdef PIXELTOASTER_RDTSC
#	include <stdint.h>
#	include <stdio.h>
#endif

namespace PixelToaster
{
	namespace internal
	{
		void wait( double seconds )
		{
			const double floorSeconds = ::floor(seconds);
			const double fractionalSeconds = seconds - floorSeconds;
		
			timespec timeOut;
			timeOut.tv_sec = static_cast<time_t>(floorSeconds);
			timeOut.tv_nsec = static_cast<long>(fractionalSeconds * 1e9);
		
			// nanosleep may return earlier than expected if there's a signal
			// that should be handled by the calling thread.  If it happens,
			// sleep again. [Bramz]
			//
			timespec timeRemaining;
			while (true)
			{
				const int ret = nanosleep(&timeOut, &timeRemaining);
				if (ret == -1 && errno == EINTR)
				{
					// there was only an sleep interruption, go back to sleep.
					timeOut.tv_sec = timeRemaining.tv_sec;
					timeOut.tv_nsec = timeRemaining.tv_nsec;
				}
				else
				{
					// we're done, or error =)
					return; 
				}
			}
		}
	}

	#ifdef PIXELTOASTER_RDTSC

	class UnixTimer : public TimerInterface
	{
	public:
	
		UnixTimer():
			resolution_(determineResolution())
		{
			reset();
		}
	
		void reset()
		{
			deltaStart_ = start_ = tick();
		}
	
		double time()
		{
			const uint64_t now = tick();
			return resolution_ * (now - start_);
		}
	
		double delta()
		{
			const uint64_t now = tick();
			const double dt = resolution_ * (now - deltaStart_);
			deltaStart_ = now;
			return dt;
		}
	
		double resolution()
		{
			return resolution_;
		}
	
		void wait( double seconds )
		{
			internal::wait(seconds);
		}	
	
	private:

		static inline uint64_t tick()
		{
	#ifdef PIXELTOASTER_64BIT
			uint32_t a, d;
			__asm__ __volatile__("rdtsc": "=a"(a), "=d"(d));
			return (static_cast<uint64_t>(d) << 32) | static_cast<uint64_t>(a);
	#else
			uint64_t val;
			__asm__ __volatile__("rdtsc": "=A"(val));
			return val;
	#endif
		}
	
		static double determineResolution()
		{
			FILE* f = fopen("/proc/cpuinfo", "r");
			if (!f)
			{
				return 0.;
			}
			const int bufferSize = 256;
			char buffer[bufferSize];
			while (fgets(buffer, bufferSize, f))
			{
				float frequency;
				if (sscanf(buffer, "cpu MHz         : %f", &frequency) == 1)
				{
					fclose(f);
					return 1e-6 / static_cast<double>(frequency);
				}
			}
			fclose(f);
			return 0.;
		}

		uint64_t start_;
		uint64_t deltaStart_;
		double resolution_;
	};

	#else

	class UnixTimer : public TimerInterface
	{
	public:
	
		UnixTimer()
		{
			reset();
		}
	
		void reset()
		{
			deltaStart_ = start_ = realTime();
		}
	
		double time()
		{
			return realTime() - start_;
		}
	
		double delta()
		{
			const double now = realTime();
			const double dt = now - deltaStart_;
			deltaStart_ = now;
			return dt;
		}
	
		double resolution()
		{
			timespec res;
			if (clock_getres(CLOCK_REALTIME, &res) != 0)
			{
				return 0.;
			}
			return res.tv_sec + res.tv_nsec * 1e-9;
		}
	
		void wait( double seconds )
		{
			internal::wait(seconds);
		}
	
	private:

		static inline double realTime()
		{
			timespec time;
			if (clock_gettime(CLOCK_REALTIME, &time) != 0)
			{
				return 0.;
			}
			return time.tv_sec + time.tv_nsec * 1e-9;		
		}
	
		double start_;
		double deltaStart_;
	};
}

#endif

#endif
