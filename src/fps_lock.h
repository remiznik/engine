#pragma once
#include <chrono>
#include <time.h>

namespace utils
{
	class FpsLock
	{
	public:		
		FpsLock(int fps);
		void update();
	private:
		int fps_;
		clock_t prevTime_;
	};
}