#pragma once
#include <limits>
#include <arm_math.h>

namespace sky
{
	using namespace std;


	template<typename InputIterator>
	void sinPeriod(
		InputIterator first, 
		InputIterator last, 
		double p2p,
		double bias
		)
	{
		size_t i = 0;
		const size_t size = last - first;
		for (; first != last; first++)
		{
			*first = p2p * 0.5
				* sin(2 * PI *i++ / size)
				+ bias;
		}
	}


}
