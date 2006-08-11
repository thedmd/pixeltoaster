// Apple Display
// Copyright © 2004-2006, Glenn Fiedler
// http://www.pixeltoaster.com

class AppleDisplay : public DisplayAdapter
{
public:
	
	AppleDisplay()
	{
		defaults();

		// ...
	}
	
	~AppleDisplay()
	{
		// ...
	}

	bool open( const char title[], int width, int height, Output output, Mode mode )
	{
		DisplayAdapter::open( title, width, height, output, mode );
		
		// ...

		return true;
	}
	
	void close()
	{
		// ...

		DisplayAdapter::close();
	}

	bool update( const TrueColorPixel * integerPixels, const FloatingPointPixel * floatingPointPixels )
	{
		// ...

		return true;
	}

protected:

	void defaults()
	{
		// ...
	}

private:

	// ...
};
