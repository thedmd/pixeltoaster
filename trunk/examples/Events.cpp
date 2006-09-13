// Events Example
// How to get keyboard, mouse and system events from a display.
// Copyright © Glenn Fiedler, 2004-2006. http://www.pixeltoaster.com

#include <cstdio>
#include <PixelToaster.h>

using namespace PixelToaster;

class Application : public Listener
{
public:

    int run()
    {
		// open display

        const int width = 320;
        const int height = 240;

        display.listener( this );

        if ( !display.open( "Events Example", width, height ) )
			return 1;

		// keep updating the display while it is open

        vector<Pixel> pixels( width * height );

        while ( display.open() )
        {
			unsigned int index = 0;

			for ( int y = 0; y < height; ++y )
			{
				for ( int x = 0; x < width; ++x )
				{
					pixels[index].r = 0.3f + (x + y) * 0.00125f;
					pixels[index].g = 0.7f + (x + y) * 0.001f;
					pixels[index].b = 0.5f + (x + y) * 0.0008f;

					++index;
				}
			}

            display.update( pixels );
        }

		return 0;
    }

protected:

	// respond to events sent back via the listener interface

    void onKeyDown( DisplayInterface & display, Key key )
    {
        printf( "onKeyDown: key=%s\n", getKeyString(key) );
    }

    void onKeyPressed( DisplayInterface & display, Key key )
    {
        printf( "onKeyPressed: key=%s\n", getKeyString(key) );
    }

    void onKeyUp( DisplayInterface & display, Key key )
    {
        printf( "onKeyUp: key=%s\n", getKeyString(key) );
    }

    void onMouseButtonDown( DisplayInterface & display, Mouse mouse )
    {
        printf( "onMouseButtonDown: buttons=%d,%d,%d x=%f, y=%f\n", 
                mouse.buttons.left, 
                mouse.buttons.middle, 
                mouse.buttons.right, 
                mouse.x, 
                mouse.y );
    }

    void onMouseButtonUp( DisplayInterface & display, Mouse mouse )
    {
        printf( "onMouseButtonUp: buttons=%d,%d,%d x=%f, y=%f\n", 
                mouse.buttons.left, 
                mouse.buttons.middle, 
                mouse.buttons.right, 
                mouse.x, 
                mouse.y );
    }

    void onMouseMove( DisplayInterface & display, Mouse mouse )
    {
        printf( "onMouseMove: buttons=%d,%d,%d x=%f, y=%f\n", 
                mouse.buttons.left, 
                mouse.buttons.middle, 
                mouse.buttons.right, 
                mouse.x, 
                mouse.y );
    }

    void onActivate( DisplayInterface & display, bool active )
    {
        printf( "onActivate: active=%d\n", active );
    }

    void onOpen( DisplayInterface & display )
	{
		printf( "onOpen: \"%s\", %d x %d ", display.title(), display.width(), display.height() );
		switch ( display.mode() )
		{
			case Mode::TrueColor: printf( "truecolor" ); break;
			case Mode::FloatingPoint: printf( "floating point" ); break;
		}
		switch ( display.output() )
		{
			case Output::Windowed: printf( " (windowed)\n" ); break;
			case Output::Fullscreen: printf( " (fullscreen)\n" ); break;
			default: break;
		}
    }

    bool onClose( DisplayInterface & display )
    {
		printf( "onClose" );
        return true;
    }

    const char * getKeyString( Key key )
    {
        switch (key)
        {
            case Key::Enter: return "Enter";
            case Key::BackSpace: return "BackSpace";
            case Key::Tab: return "Tab";
            case Key::Cancel: return "Cancel";
            case Key::Clear: return "Clear";
            case Key::Shift: return "Shift";
            case Key::Control: return "Control";
            case Key::Alt: return "Alt";
            case Key::Pause: return "Pause";
            case Key::CapsLock: return "CapsLock";
            case Key::Escape: return "Escape";
            case Key::Space: return "Space";
            case Key::PageUp: return "PageUp";
            case Key::PageDown: return "PageDown";
            case Key::End: return "End";
            case Key::Home: return "Home";
            case Key::Left: return "Left";
            case Key::Up: return "Up";
            case Key::Right: return "Right";
            case Key::Down: return "Down";
            case Key::Comma: return "Comma";
            case Key::Period: return "Period";
            case Key::Slash: return "Slash";
            case Key::Zero: return "Zero";
            case Key::One: return "One";
            case Key::Two: return "Two";
            case Key::Three: return "Three";
            case Key::Four: return "Four";
            case Key::Five: return "Five";
            case Key::Six: return "Six";
            case Key::Seven: return "Seven";
            case Key::Eight: return "Eight";
            case Key::Nine: return "Nine";
            case Key::SemiColon: return "SemiColon";
            case Key::Equals: return "Equals";
            case Key::A: return "A";
            case Key::B: return "B";
            case Key::C: return "C";
            case Key::D: return "D";
            case Key::E: return "E";
            case Key::F: return "F";
            case Key::G: return "G";
            case Key::H: return "H";
            case Key::I: return "I";
            case Key::J: return "J";
            case Key::K: return "K";
            case Key::L: return "L";
            case Key::M: return "M";
            case Key::N: return "N";
            case Key::O: return "O";
            case Key::P: return "P";
            case Key::Q: return "Q";
            case Key::R: return "R";
            case Key::S: return "S";
            case Key::T: return "T";
            case Key::U: return "U";
            case Key::V: return "V";
            case Key::W: return "W";
            case Key::X: return "X";
            case Key::Y: return "Y";
            case Key::Z: return "Z";
            case Key::OpenBracket: return "OpenBracket";
            case Key::BackSlash: return "BackSlash";
            case Key::CloseBracket: return "CloseBracket";
            case Key::NumPad0: return "NumPad0";
            case Key::NumPad1: return "NumPad1";
            case Key::NumPad2: return "NumPad2";
            case Key::NumPad3: return "NumPad3";
            case Key::NumPad4: return "NumPad4";
            case Key::NumPad5: return "NumPad5";
            case Key::NumPad6: return "NumPad6";
            case Key::NumPad7: return "NumPad7";
            case Key::NumPad8: return "NumPad8";
            case Key::NumPad9: return "NumPad9";
            case Key::Multiply: return "Multiply";
            case Key::Add: return "Add";
            case Key::Separator: return "Separator";
            case Key::Subtract: return "Subtract";
            case Key::Decimal: return "Decimal";
            case Key::Divide: return "Divide";
            case Key::F1: return "F1";
            case Key::F2: return "F2";
            case Key::F3: return "F3";
            case Key::F4: return "F4";
            case Key::F5: return "F5";
            case Key::F6: return "F6";
            case Key::F7: return "F7";
            case Key::F8: return "F8";
            case Key::F9: return "F9";
            case Key::F10: return "F10";
            case Key::F11: return "F11";
            case Key::F12: return "F12";
            case Key::Delete: return "Delete";
            case Key::NumLock: return "NumLock";
            case Key::ScrollLock: return "ScrollLock";
            case Key::PrintScreen: return "PrintScreen";
            case Key::Insert: return "Insert";
            case Key::Help: return "Help";
            case Key::Meta: return "Meta";
            case Key::BackQuote: return "BackQuote";
            case Key::Quote: return "Quote";
            case Key::Final: return "Final";
            case Key::Convert: return "Convert";
            case Key::NonConvert: return "NonConvert";
            case Key::Accept: return "Accept";
            case Key::ModeChange: return "ModeChange";
            case Key::Kana: return "Kana";
            case Key::Kanji: return "Kanji";
            default: return "Undefined";
        }
    }

private:

	Display display;
};


int main()
{
    Application application;
    application.run();
}
