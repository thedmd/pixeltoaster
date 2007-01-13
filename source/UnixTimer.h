// Unix Timer
// Copyright © 2006-2007 Bram de Greve
// Part of the PixelToaster Framebuffer Library - http://www.pixeltoaster.com

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

#endif
