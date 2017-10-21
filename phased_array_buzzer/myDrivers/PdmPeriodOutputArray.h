#pragma once
#pragma once
#include "mbed.h"
#include <array>
#include <algorithm>

#include "PeriodicSignal.h"
#include "PeriodOutputArray.h"
#include "Frqer.h"

namespace sky
{
	using namespace std;


	template<size_t ArraySize>
	class PdmPeriodOutputArray :public PeriodOutputArray
	{
	protected:
		//TODO：pdm IO并口和buf: vector<bitset<ArraySize>>
		Ticker ticker;
		size_t index = 0;
		Frqer frqer;
		float actualRate;

		//pwm占空比定时中断函数
		void tickerCallback()
		{
			if (index >= PeriodOutputArray::samplePoints)
				index = 0;
			//if (PeriodOutputArray::samplePoints != 0)
			//	for (auto &p : pwms)
					//p.output(index);
					//TODO：输出操作
			index++;
			actualRate = frqer.frq();
		}
	public:
		PdmPeriodOutputArray(
			float sampleRate = 16e3f
		) :
			PeriodOutputArray(sampleRate)
		{
			ticker.attach(
				callback(this, &PdmPeriodOutputArray<ArraySize>::tickerCallback),
				1.f / sampleRate//TODO：sampleRate和中断频率之间的关系
			);
		}

		virtual void setSampleRate(float sampleRate) override
		{
			PeriodOutputArray::setSampleRate(sampleRate);
			ticker.attach(
				callback(this, &PdmPeriodOutputArray<ArraySize>::tickerCallback),
				1.f / sampleRate//TODO：sampleRate和中断频率之间的关系
			);
		}

		//获取实际刷新速率
		auto getActualRate()
		{
			return actualRate;
		}


		virtual void setSamplePoints(size_t samplePoints) override
		{
			//TODO：samplePoints到buf长度的计算
			//PeriodOutputArray::setSamplePoints(samplePoints);
			//for (auto &p : pwms)
			//	p.setSamplePoints(samplePoints);
		}


		virtual void setSignal(function<float(float) > periodFunction, size_t n) override
		{
			//TODO：幅度到buf pdm比特序列的转换
			//pwms[n].setSignal(periodFunction);
		}

		virtual void setSignal(function<float(float)> periodFunction)
		{
			for (size_t i = 0; i < ArraySize; i++)
				setSignal(periodFunction, i);
		}
	};




}


