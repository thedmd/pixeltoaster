#ifndef PIXELTOASTER_GUI_H
#define PIXELTOASTER_GUI_H

#include <PixelToaster.h>
#include "Callback.h"

using namespace PixelToaster;
 
class Rectangle
{
public:
	Rectangle(int left, int top, int width, int height): 
		left_(left), top_(top), width_(width), height_(height) {}
	const int left() const { return left_; }
	const int top() const { return top_; }
	const int width() const { return width_; }
	const int height() const { return height_; }
	const int right() const { return left_ + width_; }
	const int bottom() const { return top_ + height_; }
	void setLeft(int left) { left_ = left; }
	void setTop(int top) { top_ = top; }
	void setWidth(int width) { width_ = width; }
	void setHeight(int height) { height_ = height; }
	void setRight(int right) { width_ = right - left_; }
	void setBottom(int bottom) { height_ = bottom - top_; }
	bool contains(int x, int y) const
	{ 	
		return x >= left_ && y >= top_ && x < left_ + width_ && y < top_ + height_; 
	}
private:
	int left_;
	int top_;
	int width_;
	int height_;
};



class Surface
{
public:
	Surface(Pixel* buffer, int width, int height):
		clipping_(0, 0, width, height),
		buffer_(buffer),
		pitch_(width)
	{
	}
	Pixel* line(int y) { return buffer_ + (clipping_.top() + y) * pitch_ + clipping_.left(); }
	Pixel& operator()(int y, int x)
	{
		return buffer_[(clipping_.top() + y) * pitch_ + clipping_.left() + x];
	}
	const Rectangle& clipping() const { return clipping_; }
	Surface view(const Rectangle& rect) const
	{
		Surface temp(*this);
		temp.clipping_.setLeft(
			clipping_.left() + std::max(0, std::min(clipping_.width(), rect.left())));
		temp.clipping_.setTop(
			clipping_.top() + std::max(0, std::min(clipping_.height(), rect.top())));
		temp.clipping_.setWidth(std::min(clipping_.width() - rect.left(), rect.width()));
		temp.clipping_.setHeight(std::min(clipping_.height() - rect.top(), rect.height()));
		return temp;
	}
private:
	Rectangle clipping_;
	Pixel* buffer_;
	int pitch_;
};



inline Pixel over(const Pixel& a, const Pixel& b)
{	    
	// C_o = C_a + C_b * (1 - \alpha_a)
	// \alpha_o = \alpha_a + \alpha_b * (1 - \alpha_a) 
	const float w = 1.f - a.a;
	return Pixel(a.r + b.r * w, a.g + b.g * w, a.b + b.b * w, a.a + b.a * w);
}



inline void drawRectangle(Surface& surface, const Rectangle& rectangle, 
	const Pixel& colour, bool fillRectangle = false)
{
	const int x1 = std::max(rectangle.left(), 0);
	const int y1 = std::max(rectangle.top(), 0);
	const int x2 = std::min(rectangle.right(), surface.clipping().width());
	const int y2 = std::min(rectangle.bottom(), surface.clipping().height());
	if (fillRectangle)
	{
		for (int y = y1; y < y2; ++y)
		{
			PixelToaster::Pixel* line = surface.line(y);
			for (int x = x1; x < x2; ++x)
			{
				line[x] = over(colour, line[x]);
			}
		}
	}
	else
	{
		if (rectangle.top() >= 0)
		{
			PixelToaster::Pixel* line = surface.line(rectangle.top());
			for (int x = x1; x < x2; ++x) line[x] = over(colour, line[x]);
		}
		if (rectangle.bottom() <= surface.clipping().height())
		{
			PixelToaster::Pixel* line = surface.line(rectangle.bottom() - 1);
			for (int x = x1; x < x2; ++x) line[x] = over(colour, line[x]);
		}
		if (rectangle.left() >= 0)
		{
			for (int y = y1; y < y2; ++y) 
			{
				Pixel& p = surface(y, rectangle.left());
				p = over(colour, p);
			}
		}
		if (rectangle.right() <= surface.clipping().width())
		{
			const int x = rectangle.right() - 1;
			for (int y = y1; y < y2; ++y) 
			{
				Pixel& p = surface(y, x);
				p = over(colour, p);
			}
		}
	}	
}



class Widget
{
public:
	virtual ~Widget() {}
	const Rectangle& position() const { return position_; }
	void draw(Surface& surface) const { doDraw(surface); }
	void mouseDown(const PixelToaster::Mouse& mouse) { doMouseDown(mouse); }
	void mouseMove(const PixelToaster::Mouse& mouse) { doMouseMove(mouse); }
	void mouseUp(const PixelToaster::Mouse& mouse) { doMouseUp(mouse); }
protected:
	Widget(const Rectangle& position): position_(position) {}
private:
	virtual void doDraw(Surface& surface) const = 0;
	virtual void doMouseDown(const PixelToaster::Mouse& mouse) {};
	virtual void doMouseMove(const PixelToaster::Mouse& mouse) {};
	virtual void doMouseUp(const PixelToaster::Mouse& mouse) {};
	Rectangle position_;
};



class Dialog: public Widget
{
public:
	Dialog(const Rectangle& position): 
		Widget(position), fillColor_(0.4f, 0.4f, 0.4f, 0.8f), borderColor_(1.f, 1.f, 1.f, 1.f),
		focusChild_(0) {}
	void addChild(Widget* child) { children_.push_back(child); }
private:
	typedef std::vector<Widget*> Children;
	void doDraw(Surface& surface) const
	{
		drawRectangle(surface, position(), fillColor_, true);
		drawRectangle(surface, position(), borderColor_, false);
		Surface clipped = surface.view(position());
		for (Children::const_iterator i = children_.begin(); i != children_.end(); ++i)
		{
			(*i)->draw(clipped);
		}
	}
	void doMouseDown(const PixelToaster::Mouse& mouse)
	{
		focusChild_ = 0;
		for (Children::const_iterator i = children_.begin(); i != children_.end(); ++i)
		{
			PixelToaster::Mouse relative = mouse;
			relative.x -= position().left();
			relative.y -= position().top();		
			if ((*i)->position().contains(
				static_cast<int>(relative.x), static_cast<int>(relative.y)))
			{
				focusChild_ = *i;
				focusChild_->mouseDown(relative);
				return;
			}
		}
	}		
	void doMouseMove(const PixelToaster::Mouse& mouse)
	{
		if (focusChild_)
		{
			PixelToaster::Mouse relative = mouse;
			relative.x -= position().left();
			relative.y -= position().top();		
			focusChild_->mouseMove(relative);
		}
	}		
	void doMouseUp(const PixelToaster::Mouse& mouse)
	{
		if (focusChild_)
		{
			PixelToaster::Mouse relative = mouse;
			relative.x -= position().left();
			relative.y -= position().top();		
			focusChild_->mouseUp(relative);
		}
		focusChild_ = 0;
	}		
	Pixel fillColor_;
	Pixel borderColor_;
	Children children_;
	Widget* focusChild_;
};



class Slider: public Widget
{
public:
	Slider(const Rectangle& position, float min = 0.f, float max = 1.f, float value = 0.f):
		Widget(position), fillColor_(1.f, 1.f, 1.f, 1.f), borderColor_(1.f, 1.f, 1.f, 1.f),
		min_(min), max_(max)
	{
		setValue(value);
	}
	const float value() const { return value_; }
	void setValue(float value) 
	{ 
		value_ = std::max(min_, std::min(max_, value)); 
		onChangeValue_(value_);
	}
	void setOnChangeValue(const Callback1<float>& callback) { onChangeValue_ = callback; }
private:
	void doDraw(Surface& surface) const
	{
		Rectangle fill = position();
		fill.setWidth(static_cast<int>((value_ - min_) * fill.width() / (max_ - min_)));
		drawRectangle(surface, fill, fillColor_, true);
		drawRectangle(surface, position(), borderColor_, false);
	}
	void doMouseDown(const PixelToaster::Mouse& mouse)
	{
		doMouseStuff(mouse);
	}
	void doMouseMove(const PixelToaster::Mouse& mouse)
	{
		doMouseStuff(mouse);
	}
	void doMouseStuff(const PixelToaster::Mouse& mouse)
	{
		if (mouse.buttons.left)
		{
			float u = static_cast<float>(mouse.x - position().left()) / position().width();
			setValue(min_ + u * (max_ - min_));
		}
	}
	Pixel fillColor_;
	Pixel borderColor_;
	Callback1<float> onChangeValue_;
	float min_;
	float max_;
	float value_;
};

class ColorMixer: public Widget
{
public:
	ColorMixer(const Rectangle& position, const Pixel& color): Widget(position), color_(color) {}
	const Pixel color() const { return premultiply(color_); }
	void setRed(float r) { color_.r = r; }
	void setGreen(float g) { color_.g = g; }
	void setBlue(float b) { color_.b = b; }
	void setAlpha(float a) { color_.a = a; }
private:
	void doDraw(Surface& surface) const
	{
		const Pixel premultiplied = premultiply(color_);
		const Pixel white = over(premultiplied, Pixel(1.f, 1.f, 1.f, 1.f));
		const Pixel black = over(premultiplied, Pixel(.0f, .0f, .0f, 1.f));
		const int halfX = position().left() + position().width() / 2;
		const int halfY = position().top() + position().height() / 2;
		Rectangle temp = position(); temp.setRight(halfX); temp.setBottom(halfY);
		drawRectangle(surface, temp, white, true);	
		temp.setTop(halfY); temp.setBottom(position().bottom());
		drawRectangle(surface, temp, black, true);	
		temp = position(); 
		temp.setLeft(halfX); temp.setRight(position().right()); temp.setBottom(halfY);
		drawRectangle(surface, temp, black, true);	
		temp.setTop(halfY); temp.setBottom(position().bottom()); 
		drawRectangle(surface, temp, white, true);		
	}
	Pixel premultiply(const Pixel& c) const { return Pixel(c.r * c.a, c.g * c.a, c.b * c.a, c.a); }
	Pixel color_;
};


#endif
