#pragma once

#include "mbed.h"

namespace sky
{
	using namespace std;

	class TicToc
	{
		Timer timer;
		int ticTime;
	public:
		TicToc()
		{
			timer.start();
		}

		void tic()
		{
			ticTime = timer.read_us();
		}

		int toc_us()
		{
			return timer.read_us() - ticTime;
		}

		int toc_ms()
		{
			return (timer.read_us() - ticTime) / 1000;
		}

		float toc()
		{
			return float(timer.read_us() - ticTime) / 1000000.0f;
		}
	};

}
