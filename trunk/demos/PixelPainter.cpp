// PixelPainter, a pixel painting demo of PixelToaster
// Copyright © Bram de Greve <bramz@users.sourceforge.net>
//
// PixelPainter is a very minimalistic paint program to demonstrate
// the use of Listener.  The setup of PixelPainter is very simple:
// it has two modes: a paint mode and a color picking mode.
// You can switch between both modes by pressing Space.
// You can leave PixelPainter by pressing Escape.
//
// Paint mode: in this mode you can paint on the canvas by pressing
// a mouse button and dragging it all around.  There are three mouse
// buttons, so there are three "brushes" you can paint with.
//
// In the paint mode you can also set the brush thickness (this is
// set for all three brushes at once).  To do that, you type the
// desired thickness using the number keys on the main keyboard
// (so not the ones on the numpad).  Each number you type is a digit
// of the thickness, so you can enter numbers larger than 9.  This is
// stopped at the moment you cause any other 'event' (clicking or
// moving mouse, or pressing any other key).
//
// Color picking mode: in this mode you can select the colors of
// each brush.  On the top half of the screen you see three color
// channels red, green and blue.  On the bottom half you see the
// selected colors of all brushes: left, middle and right.  You
// can change a brush color by clicking with the corresponding
// mouse button in any of the channels.  It will set the channel
// of that brush to the selected value.

#include <PixelToaster.h>
#include <iostream>
#include "PixelPainterGui.h"

using namespace PixelToaster;

class ColorDialog: public Dialog
{
public:
	ColorDialog():
		Dialog(Rectangle(10, 10, 220, 90)),
		redSlider_(Rectangle(30, 10, 100, 10)),
		greenSlider_(Rectangle(30, 30, 100, 10)),
		blueSlider_(Rectangle(30, 50, 100, 10)),
		alphaSlider_(Rectangle(30, 70, 100, 10)),
		red_(Rectangle(10, 10, 10, 10), Pixel(1.f, 0.f, 0.f, 1.f)),
		green_(Rectangle(10, 30, 10, 10), Pixel(0.f, 1.f, 0.f, 1.f)),
		blue_(Rectangle(10, 50, 10, 10), Pixel(0.f, 0.f, 1.f, 1.f)),
		alpha_(Rectangle(10, 70, 10, 10), Pixel(0.f, 0.f, 0.f, 0.f)),
		color_(Rectangle(140, 10, 70, 70), Pixel(1.f, 1.f, 1.f, 1.f))
	{
		addChild(&redSlider_);
		addChild(&greenSlider_);
		addChild(&blueSlider_);
		addChild(&alphaSlider_);
		addChild(&red_);
		addChild(&green_);
		addChild(&blue_);
		addChild(&alpha_);
		addChild(&color_);
		redSlider_.setOnChangeValue(makeCallback(&color_, &ColorMixer::setRed));
		greenSlider_.setOnChangeValue(makeCallback(&color_, &ColorMixer::setGreen));
		blueSlider_.setOnChangeValue(makeCallback(&color_, &ColorMixer::setBlue));
		alphaSlider_.setOnChangeValue(makeCallback(&color_, &ColorMixer::setAlpha));
	}
	const Pixel color() const { return color_.color(); }
private:
	Slider redSlider_;
	Slider greenSlider_;
	Slider blueSlider_;
	Slider alphaSlider_;
	ColorMixer red_;
	ColorMixer green_;
	ColorMixer blue_;
	ColorMixer alpha_;
	ColorMixer color_;
};

class PixelPainter: public Listener
{
public:

	PixelPainter(int width = 320, int height = 240):
		colorDialog_(),
		brushColor_(1.f, 1.f, 1.f, 0.5f),
		canvas_(width * height, Pixel(0.f, 0.f, 0.f, 1.f)),
		colorPicker_(width * height),
		width_(width),
		height_(height),
		prevX_(0),
		prevY_(0),
		thickness_(4),
		isQuiting_(false),
		isEnteringThickness_(false),
		isPickingColors_(false)
	{
	}

	void run()
	{
		isQuiting_ = false;

		if (!display_.open( "PixelPainter!", width_, height_))
		{
			std::cerr << "Could not open display\n";
			return;
		}

		display_.listener(this);

		while (!isQuiting_)
		{
			if (isPickingColors_)
			{
				colorPicker_ = canvas_;
				Surface surface(&colorPicker_[0], width_, height_);
				colorDialog_.draw(surface);
				display_.update(colorPicker_);
			}
			else
			{
				display_.update(canvas_);
			}
		}
	}

private:

	typedef FloatingPointPixel TPixel;
	typedef std::vector<TPixel> TBuffer;

	void onKeyDown(DisplayInterface & display, Key key)
	{
		const bool isAlreadyEnteringThickness = isEnteringThickness_;
		isEnteringThickness_ = false;

		switch (key)
		{
		case Key::Escape:
			isQuiting_ = true;
			break;
		case Key::Space:
			isPickingColors_ = !isPickingColors_;
			if (isPickingColors_)
			{
				colorPicker_ = canvas_;
			}
			else
			{
				brushColor_ = colorDialog_.color();
			}
			break;
		default:
			const int num = number(key);
			if (num != -1 && !isPickingColors_)
			{
				isEnteringThickness_ = true;
				if (isAlreadyEnteringThickness)
				{
					thickness_ = std::min(10000, 10 * thickness_ + num);
				}
				else
				{
					thickness_ = num;
				}
			}
		}
	}

	void onMouseButtonDown(DisplayInterface & display, Mouse mouse)
	{
		isEnteringThickness_ = false;
		const int x = static_cast<int>(mouse.x);
		const int y = static_cast<int>(mouse.y);
		const int but = button(mouse);

		if (isPickingColors_)
		{
			colorDialog_.mouseDown(mouse);
		}
		else
		{
			if (but == 0)
			{
				drawBrush(canvas_, x, y, brushColor_);
			}
			prevX_ = x;
			prevY_ = y;
		}
	}

	void onMouseMove(DisplayInterface & display, Mouse mouse)
	{
		isEnteringThickness_ = false;
		const int x = static_cast<int>(mouse.x);
		const int y = static_cast<int>(mouse.y);

		if (isPickingColors_)
		{
			colorDialog_.mouseMove(mouse);
		}
		else
		{
			const int but = button(mouse);
			if (but == 0)
			{
				drawLine(canvas_, prevX_, prevY_, x, y, brushColor_);
			}
			prevX_ = x;
			prevY_ = y;
		}
	}

	void onClose(DisplayInterface & display)
	{
		isQuiting_ = true;
	}

	inline int button(const Mouse& mouse) const
	{
		if (mouse.buttons.left) return 0;
		if (mouse.buttons.middle) return 1;
		if (mouse.buttons.right) return 2;
		return -1;
	}

	inline int number(Key key) const
	{
		const int k = static_cast<int>(key);
		if (k >= static_cast<int>(Key::Zero) && key <= static_cast<int>(Key::Nine))
		{
			return k - static_cast<int>(Key::Zero);
		}
		return -1;
	}

	void drawBox(TBuffer& buffer, int xbegin, int ybegin, int xend, int yend, const TPixel& color)
	{
		xbegin = std::max(0, xbegin);
		ybegin = std::max(0, ybegin);
		xend = std::min(width_, xend);
		yend = std::min(height_, yend);
		for (int j = ybegin; j < yend; ++j)
		{
			int i0 = j * width_;
			for (int i = xbegin; i < xend; ++i)
			{
				Pixel& p = buffer[i0 + i];
				p = over(color, p);
			}
		}
	}

	inline void drawBrush(TBuffer& buffer, int x, int y, const TPixel& color)
	{
		const int halfThickness = thickness_ / 2;
		const int xbegin = x - halfThickness;
		const int ybegin = y - halfThickness;
		drawBox(buffer, xbegin, ybegin, xbegin + thickness_, ybegin + thickness_, color);
	}

	void drawLine(TBuffer& buffer, int x1, int y1, int x2, int y2, const TPixel& color)
	{
		int dx = abs(x2 - x1);
		int dy = abs(y2 - y1);
		int sx = x2 >= x1 ? 1 : -1;
		int sy = y2 >= y1 ? 1 : -1;
		if (dx >= dy)
		{
			int y = y1;
			int h = 0;
			for (int x = x1; x != x2; x += sx)
			{
				drawBrush(buffer, x, y, color);
				h += dy;
				if (h >= dx)
				{
					h -= dx;
					y += sy;
				}
			}
		}
		else
		{
			int x = x1;
			int h = 0;
			for (int y = y1; y != y2; y += sy)
			{
				drawBrush(buffer, x, y, color);
				h += dx;
				if (h >= dy)
				{
					h -= dy;
					x += sx;
				}
			}
		}
		drawBrush(buffer, x2, y2, color);
	}

	ColorDialog colorDialog_;
	Display display_;
	TPixel brushColor_;
	TBuffer canvas_;
	TBuffer colorPicker_;
	int width_;
	int height_;
	int prevX_;
	int prevY_;
	int thickness_;
	bool isQuiting_;
	bool isEnteringThickness_;
	bool isPickingColors_;
};


int main()
{
	PixelPainter application;
	application.run();
}
