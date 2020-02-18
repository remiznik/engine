#include "fps_lock.h"

#include <chrono>
#include <thread>
#include <time.h>

namespace utils
{
	FpsLock::FpsLock(int fps) : fps_(fps)
	{
		prevTime_ = clock();
	}

	void FpsLock::update()
	{
		const double frameDuration(1.0 / double(fps_));

		double t = frameDuration - ((double)(clock() - prevTime_) / CLOCKS_PER_SEC);

		std::chrono::duration<double, std::milli> sleepDuration(t);
		
		std::this_thread::sleep_for(sleepDuration);
		prevTime_ = clock();
		
	}
}
