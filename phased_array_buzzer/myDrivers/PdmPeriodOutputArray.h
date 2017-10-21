#pragma once
#pragma once
#include "mbed.h"
#include <array>
#include <bitset>
#include <algorithm>

#include "PeriodicSignal.h"
#include "PeriodOutputArray.h"
#include "Frqer.h"

namespace sky
{
	using namespace std;

	//用bitset写入的并口
	template<size_t N>
	class ParallelOut
	{
	protected:
		array<DigitalOut, N> outputs;
	public:
		ParallelOut(
			const array<DigitalOut, N> &outputs
		) :
			outputs(outputs)
		{

		}

		void write(const bitset<N> &bits)
		{
			for (int i = 0; i < N; i++)
			{
				outputs[i].write(bits[i]);
			}
		}
	};

	template<size_t ArraySize>
	class PdmPeriodOutputArray :public PeriodOutputArray
	{
	protected:
		//PortOut port;//若性能较差可考虑用PortOut
		vector<bitset<ArraySize>> signal;
		ParallelOut<ArraySize> pout;

		Ticker ticker;
		size_t index = 0;
		Frqer frqer;
		float actualRate;

		//pwm占空比定时中断函数
		void tickerCallback()
		{
			if (index >= PeriodOutputArray::samplePoints)
				index = 0;
			if (PeriodOutputArray::samplePoints != 0)
			{
				pout.write(signal[index]);
				index++;
			}
			actualRate = frqer.frq();
		}
	public:
		PdmPeriodOutputArray(
			const array<DigitalOut, ArraySize> &pout,
			float sampleRate = 50e3f
		) :
			pout(pout),
			PeriodOutputArray(sampleRate)
		{
			ticker.attach(
				callback(this, &PdmPeriodOutputArray<ArraySize>::tickerCallback),
				1.f / sampleRate
			);
		}

		virtual void setSampleRate(float sampleRate) override
		{
			PeriodOutputArray::setSampleRate(sampleRate);
			ticker.attach(
				callback(this, &PdmPeriodOutputArray<ArraySize>::tickerCallback),
				1.f / sampleRate
			);
		}

		//获取实际刷新速率
		auto getActualRate()
		{
			return actualRate;
		}


		virtual void setSamplePoints(size_t samplePoints) override
		{
			PeriodOutputArray::setSamplePoints(samplePoints);
			signal.resize(samplePoints);
		}


		virtual void setSignal(function<float(float) > periodFunction, size_t n) override
		{
			float accumulator = 0;
			size_t signalSize = signal.size();
			for (size_t index = 0; index < signalSize; index++)
			{
				accumulator += periodFunction((float)index / signalSize);
				if (accumulator>1.f)
				{
					accumulator -= 1.f;
					signal[index][n] = 1;
				}
				else
				{
					signal[index][n] = 0;
				}
			}
		}

		virtual void setSignal(function<float(float)> periodFunction)
		{
			for (size_t i = 0; i < ArraySize; i++)
				setSignal(periodFunction, i);
		}
	};




}


