#pragma once
#include <limits>
#include "dsp.h"
#include <algorithm>

namespace sky
{
	using namespace std;

	//写入一个周期的正弦信号
	//p2p：峰峰值
	//bias：直偏
	//phase：初相位，弧度
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

	//写入一个周期的锯齿波信号
	//min：上升起点
	//max：上升顶点
	//phase：相位，弧度
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

	//写入一个周期的三角波信号
	//min：最小值
	//max：最大值
	//phase：相位，弧度
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
