#pragma once
#include <limits>
#include <arm_math.h>

namespace sky
{
	using namespace std;

	//д��һ�����ڵ������ź�
	//p2p�����ֵ
	//bias��ֱƫ
	//phase����λ������
	template<typename InputIterator>
	void sinPeriod(
		InputIterator first, 
		InputIterator last, 
		double p2p = 1,
		double bias = 0,
		double phase = 0
	)
	{
		const size_t size = last - first;
		size_t i = size*(phase*0.5 / PI);
		for (; first != last; first++)
		{
			*first = p2p * 0.5
				* sin(2 * PI *i++ / size)
				+ bias;
		}
	}


}
