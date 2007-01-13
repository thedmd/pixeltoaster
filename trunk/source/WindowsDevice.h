// Windows Device
// Copyright © 2006-2007 Glenn Fiedler
// Part of the PixelToaster Framebuffer Library - http://www.pixeltoaster.com

class WindowsDevice
{
public:

	WindowsDevice( LPDIRECT3D9 direct3d, HWND window, int width, int height, Mode mode, bool windowed )
	{
		assert( direct3d );
		assert( window );

		this->direct3d = direct3d;
		this->window = window;

		// store parameters

		this->width = width;
		this->height = height;
		this->window = window;
		this->mode = mode;
		this->windowed = windowed;

		// defaults

		surface = NULL;
		device = NULL;

		// setup

		createDeviceAndSurface();
	}

	/// destructor

	~WindowsDevice()
	{
		destroyDeviceAndSurface();
	}

	/// check if device is valid
	/// @returns true if the device is valid

	bool valid() const
	{
		if ( device == NULL && surface == NULL )
			return false;

		return !FAILED( device->TestCooperativeLevel() );		// not valid if device is lost
	}

	/// update the device pixels.
	/// @returns true if the update succeeded, false otherwise.

	bool update( const TrueColorPixel * trueColorPixels, const FloatingPointPixel * floatingPointPixels )
	{
		// handle device loss

		HRESULT result = device->TestCooperativeLevel();

		if ( FAILED( result ) )
		{
			if ( result == D3DERR_DEVICENOTRESET )
				device->Reset( &presentation );
		}

		if ( !valid() )
			return false;

		// copy pixels to surface

		if ( !surface )
			return false;

		D3DLOCKED_RECT lock;

		if ( FAILED( surface->LockRect( &lock, NULL, D3DLOCK_DISCARD ) ) )
			return false;

		unsigned char * data = (unsigned char*) lock.pBits;
		const int pitch = lock.Pitch;

		unsigned char * line = data;

		if ( floatingPointPixels )
		{
			Converter * converter = requestConverter( Format::XBGRFFFF, format );

			if ( converter )
			{
				converter->begin();

				const Pixel * source = floatingPointPixels;

				for ( int y = 0; y < height; ++y )
				{
					converter->convert( source, line, width );
					source += width;
					line += pitch; 
				}

				converter->end();
			}
		}
		else if ( trueColorPixels )
		{
			Converter * converter = requestConverter( Format::XRGB8888, format );

			if (converter)
			{
				converter->begin();

				const TrueColorPixel * source = trueColorPixels;

				for ( int y = 0; y < height; y++ )
				{
					converter->convert( source, line, width );
					source += width;
					line += pitch; 
				}

				converter->end();
			}
		}

		surface->UnlockRect();

		// paint display

		return paint();
	}
	
	/// paint pixels to device
	/// @returns true if paint succeeded, false if it failed.

	bool paint()
	{
		// check if valid

		if ( !valid() )
			return false;

		// copy surface to back buffer

		LPDIRECT3DSURFACE9 backBuffer;

		device->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer );

		if ( !backBuffer )
			return false;

		HRESULT result = device->UpdateSurface( surface, 0, backBuffer, NULL );

		backBuffer->Release();

		if ( FAILED( result ) )
			return false;

		// present back buffer to display

		if ( FAILED( device->Present( NULL, NULL, NULL, NULL ) ) )
			return false;

		// tell windows that we dont need to repaint anything

		ValidateRect( window, NULL );

		return true;
	}

protected:

	Format convertFormat( D3DFORMAT format )
	{
		switch ( format )
		{
			case D3DFMT_A32B32G32R32F: return Format::XBGRFFFF;
			case D3DFMT_X8R8G8B8: return Format::XRGB8888;
			case D3DFMT_X8B8G8R8: return Format::XBGR8888;
			case D3DFMT_R8G8B8: return Format::RGB888;
			case D3DFMT_R5G6B5: return Format::RGB565;
			case D3DFMT_X1R5G5B5: return Format::XRGB1555;
			default: return Format::Unknown;
		}
	}

	D3DFORMAT convertFormat( Format format )
	{
		switch ( format )
		{
			case Format::XBGRFFFF: return D3DFMT_A32B32G32R32F;
			case Format::XRGB8888: return D3DFMT_X8R8G8B8;
			case Format::XBGR8888: return D3DFMT_X8B8G8R8;
			case Format::RGB888: return D3DFMT_R8G8B8;
			case Format::RGB565: return D3DFMT_R5G6B5;
			case Format::XRGB1555: return D3DFMT_X1R5G5B5;
			default: return D3DFMT_UNKNOWN;
		}
	}

protected:

	void zeroMemory( char * memory, int size )
	{
		while ( size-- )
			*(memory++) = 0; 
	}

	bool createDevice( LPDIRECT3D9 direct3d, int width, int height, Format format, bool windowed )
	{
		this->format = format;

		// triple buffered device

		zeroMemory( (char*) &presentation, sizeof(presentation) );

		presentation.BackBufferWidth = width;
		presentation.BackBufferHeight = height;
		presentation.Windowed = windowed;
		presentation.SwapEffect = D3DSWAPEFFECT_DISCARD;
		presentation.BackBufferFormat = convertFormat(format);
		presentation.hDeviceWindow = window;
		presentation.BackBufferCount = 2;

		if ( FAILED( direct3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &presentation, &device ) ) )
			if ( FAILED( direct3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_MIXED_VERTEXPROCESSING, &presentation, &device ) ) )
				if ( FAILED( direct3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &presentation, &device ) ) )
					device = NULL;

		// double buffered fallback

		if ( !device )
		{
			presentation.BackBufferCount = 1;

			if ( FAILED( direct3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &presentation, &device ) ) )
				if ( FAILED( direct3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_MIXED_VERTEXPROCESSING, &presentation, &device ) ) )
					if ( FAILED( direct3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &presentation, &device ) ) )
						device = NULL;
		}

		// check for failure

		if ( !device )
			return false;

		return true;
	}

protected:

	void createDeviceAndSurface()
	{
		// create device

		if ( mode == Mode::FloatingPoint )
		{
			if ( !createDevice( direct3d, width, height, Format::XBGRFFFF, windowed ) )
				if ( !createDevice( direct3d, width, height, Format::XRGB8888, windowed ) )
					if ( !createDevice( direct3d, width, height, Format::XBGR8888, windowed ) )
						if ( !createDevice( direct3d, width, height, Format::RGB888, windowed ) )
							if ( !createDevice( direct3d, width, height, Format::RGB565, windowed ) )
								if ( !createDevice(direct3d, width, height, Format::XRGB1555, windowed ) )
									return;
		}
		else
		{
			if ( !createDevice(direct3d, width, height, Format::XRGB8888, windowed ) )
				if ( !createDevice(direct3d, width, height, Format::XBGR8888, windowed ) )
					if ( !createDevice(direct3d, width, height, Format::RGB888, windowed ) )
						if ( !createDevice(direct3d, width, height, Format::RGB565, windowed ) )
							if ( !createDevice(direct3d, width, height, Format::XRGB1555, windowed ) )
								return;
		}

		// create surface

		device->CreateOffscreenPlainSurface( width, height, convertFormat(format), D3DPOOL_SYSTEMMEM, &surface, NULL );
	}

	void destroyDeviceAndSurface()
	{
		if ( surface )
		{
			surface->Release();
			surface = NULL;
		}

		if ( device )
		{
			device->Release();
			device = NULL;
		}
	}

private:

	LPDIRECT3D9 direct3d;
	HWND window;

	D3DPRESENT_PARAMETERS presentation;
	LPDIRECT3DDEVICE9 device;
	LPDIRECT3DSURFACE9 surface;

	int width;
	int height;
	Format format;
	Mode mode;
	bool windowed;
	bool lost;
};
