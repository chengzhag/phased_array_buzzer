#pragma once

#include "mbed.h"

namespace sky
{
	using namespace std;

	class Frqer
	{
	protected:
		Timer timer;
		int timeOld = 0, timeNew = 0;
		
	public:
		Frqer() 
		{
			timer.start();
		}

		float frq()
		{
			timeNew = timer.read_us();
			float frq = 1000000.0f / (timeNew - timeOld);
			timeOld = timeNew;
			return frq;
		}
	};

}
