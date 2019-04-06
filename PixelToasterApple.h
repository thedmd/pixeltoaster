// Apple MacOS X Platform
// Copyright © 2004-2007 Glenn Fiedler
// Part of the PixelToaster Framebuffer Library - http://www.pixeltoaster.com

// native Cocoa output implemented by Thorsten Schaaps <bitpull@aixplosive.de>

#ifndef PIXELTOASTER_APPLE_USE_X11
#    define PIXELTOASTER_APPLE_USE_X11 0
#endif

#include "CoreServices/CoreServices.h"
#include <mach/mach_time.h>

#if PIXELTOASTER_APPLE_USE_X11
#    define PIXELTOASTER_NO_UNIX_TIMER
#    include "PixelToasterUnix.h"
#endif

// display implementation
namespace PixelToaster {
#if !PIXELTOASTER_APPLE_USE_X11
class AppleDisplay : public DisplayAdapter
{
    class AppleDisplayPrivate;

public:
    AppleDisplay();

    virtual ~AppleDisplay();

    bool open(const char title[],
              int width, int height,
              Output output,
              Mode   mode) override;

    void close() override;

    bool update(const TrueColorPixel*     trueColorPixels,
                const FloatingPointPixel* floatingPointPixels,
                const Rectangle*          dirtyBox) override;

    void title(const char title[]) override;

    bool windowed() override;

    bool fullscreen() override;

    void listener(Listener* listener) override;

    void setShouldClose() { _shouldClose = true; }

    void setShouldToggle() { _shouldToggle = true; }

    void shutdown();

protected:
    void defaults() override;

private:
    AppleDisplayPrivate* _private;
    bool                 _shouldClose;
    bool                 _shouldToggle;
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
        mach_timebase_info_data_t timebase;
        mach_timebase_info(&timebase);
        _resolution = (double)timebase.numer / ((double)timebase.denom * 1.0e9);

        reset();
    }

    void reset()
    {
        _deltaCounter = mach_absolute_time();
        _time         = 0;
    }

    double time()
    {
        uint64_t counter = mach_absolute_time();
        uint64_t delta   = counter - _timeCounter;
        _timeCounter     = counter;
        _time += delta * _resolution;
        return _time;
    }

    double delta()
    {
        uint64_t counter = mach_absolute_time();
        uint64_t delta   = counter - _deltaCounter;
        _deltaCounter    = counter;
        return delta * _resolution;
    }

    double resolution()
    {
        return _resolution;
    }

    void wait(double seconds)
    {
        uint64_t counter = mach_absolute_time();
        uint64_t finish  = counter + uint64_t(seconds / _resolution);
        while (counter < finish)
            counter = mach_absolute_time();
    }

private:
    double   _time;         ///< current time in seconds
    uint64_t _timeCounter;  ///< time counter in timebase
    uint64_t _deltaCounter; ///< delta counter in timebase
    double   _resolution;
};
} // namespace PixelToaster
