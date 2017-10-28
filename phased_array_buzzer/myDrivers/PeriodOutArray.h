#pragma once
#include "mbed.h"
#include <vector>
#include <functional>
#include <algorithm>

#include "PeriodicSignal.h"
#include "Frqer.h"

namespace sky
{
	using namespace std;

	//接口 PeriodOutputArray
	class PeriodOutputArray
	{
	private:
		float sampleRate = 0;
		size_t samplePoints = 0;
	public:
		PeriodOutputArray(float sampleRate = 0):
			sampleRate(sampleRate)
		{};

		//设置采样率
		virtual void setSampleRate(float sampleRate)
		{
			this->sampleRate = sampleRate;
		}

		//获取采样率
		virtual float getSampleRate()
		{
			return sampleRate;
		}

		//设置样本点数
		virtual void setSamplePoints(size_t samplePoints)
		{
			this->samplePoints = samplePoints;
		}

		//获取样本点数
		virtual size_t getSamplePoints()
		{
			return samplePoints;
		}

		//设置周期信号
		//代码：
		//setSignal(
		//	[](float t)->float
		//{
		//	float y;
		//	...
		//		return y;
		//},
		//n
		//);
		virtual void setSignal(function<float(float)> periodFunction, size_t n) = 0;

	};

	//接口 PeriodOutput
	//用于输出独立，可以组合的周期输出
	template<typename BufT>
	class PeriodOutput
	{
	protected:
		vector<BufT> signal;
	public:
		PeriodOutput(vector<BufT> signalInit = {}) :
			signal(signalInit)
		{}

		PeriodOutput(size_t samplePoints) :
			signal(samplePoints, 0)
		{}

		PeriodOutput(const PeriodOutput &periodOutput) :
			signal(periodOutput.signal)
		{}

		//设置样本点数
		virtual void setSamplePoints(size_t samplePoints)
		{
			signal.resize(samplePoints);
		}

		//设置周期信号
		//代码：
		//setSignal(
		//	[](float t)->float
		//{
		//	float y;
		//	...
		//		return y;
		//}
		//);
		virtual void setSignal(function<float(float)> periodFunction)
		{
			size_t index = 0;
			for (auto &y : signal)
				y = periodFunction((float)index++/signal.size());
		}

		//输出下一个信号
		//默认无操作，非软件控制输出可以不用实现
		virtual void output(size_t index) {};
	};

}