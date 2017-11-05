#pragma once
#include "mbed.h"
#include <arm_math.h>

namespace sky
{
	using namespace std;

	template<typename T>
	inline void limit(T &x, T min, T max)
	{
		if (x < min)
		{
			x = min;
		}
		else if (x > max)
		{
			x = max;
		}
	}

	template<typename T>
	inline void limitLow(T &x, T min)
	{
		if (x < min)
		{
			x = min;
		}
	}

	template<typename T>
	inline void limitHigh(T &x, T max)
	{
		if (x > max)
		{
			x = max;
		}
	}
}
