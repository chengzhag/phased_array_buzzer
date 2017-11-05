#pragma once
#include <limits>
#include "dsp.h"
#include <algorithm>

namespace sky
{
	using namespace std;

	//д��һ�����ڵ������ź�
	//p2p�����ֵ
	//bias��ֱƫ
	//phase������λ������
	template<typename InputIterator>
	void sinPeriod(
		InputIterator first, 
		InputIterator last, 
		decltype((*first)++) p2p = 1,
		decltype((*first)++) bias = 0,
		decltype((*first)++) phase = 0
	)
	{
		const size_t size = last - first;
		size_t i = size*(phase*0.5 / PI);

		for_each(
			first, last,
			[&i, size, p2p, bias](auto& y) {
			y = p2p * 0.5
				* arm_sin_f32(2 * PI *(i++ % size) / size)
				+ bias;
		});
	}

	//д��һ�����ڵľ�ݲ��ź�
	//min���������
	//max����������
	//phase����λ������
	template<typename InputIterator>
	void sawPeriod(
		InputIterator first,
		InputIterator last,
		decltype((*first)++) min = 0,
		decltype((*first)++) max = 1,
		decltype((*first)++) phase = 0
	)
	{
		const size_t size = last - first;
		size_t i = size*(phase*0.5 / PI);

		for_each(
			first, last,
			[&i, size, min, max](auto& y) {y = (max - min)*(i++ % size) / size + min; }
		);
	}

	//д��һ�����ڵ����ǲ��ź�
	//min����Сֵ
	//max�����ֵ
	//phase����λ������
	template<typename InputIterator>
	void triPeriod(
		InputIterator first,
		InputIterator last,
		decltype((*first)++) min = 0,
		decltype((*first)++) max = 1,
		decltype((*first)++) phase = 0
	)
	{
		const size_t size = last - first;
		size_t i = size*(phase*0.5 / PI);

		for_each(
			first, first + size / 2,
			[&i, size, min, max](auto& y) {y = (max - min)*(i++ % size) * 2 / size; }
		);
		for_each(
			first + size / 2, last,
			[&i, size, min, max](auto& y) {y = (max - min)*(i-- % size) * 2 / size; }
		);
	}


}
