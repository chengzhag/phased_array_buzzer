#pragma once
#include "mbed.h"
#include <array>
#include <bitset>
#include <algorithm>

#include "PeriodicSignal.h"
#include "PeriodOutArray.h"
#include "Frqer.h"

namespace sky
{
	using namespace std;

	//用bitset写入的并口
	template<size_t N>
	class ParallelOut
	{
	private:
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
	class PdmPeriodOutArrayBitsetSoft :public PeriodOutputArray
	{
	private:
		//PortOut port;//若性能较差可考虑用PortOut
		vector<bitset<ArraySize>> signal;
		ParallelOut<ArraySize> pout;

		Ticker ticker;
		size_t index = 0;
		Frqer frqer;
		float actualRate;

		//pdm定时中断函数
		void tickerCallback()
		{
			if (getSamplePoints() != 0)
			{
				if (index >= getSamplePoints())
					index = 0;
				pout.write(signal[index]);
				index++;
			}
			actualRate = frqer.frq();
		}
	public:
		PdmPeriodOutArrayBitsetSoft(
			const array<DigitalOut, ArraySize> &pout,
			float sampleRate = 50e3f
		) :
			pout(pout),
			PeriodOutputArray(sampleRate)
		{
			ticker.attach(
				callback(this, &PdmPeriodOutArrayBitsetSoft<ArraySize>::tickerCallback),
				1.f / sampleRate
			);
		}

		virtual void setSampleRate(float sampleRate) override
		{
			PeriodOutputArray::setSampleRate(sampleRate);
			ticker.attach(
				callback(this, &PdmPeriodOutArrayBitsetSoft<ArraySize>::tickerCallback),
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

	class PdmPeriodOutUint8Soft :public PeriodOutput<uint8_t>
	{
	private:
		DigitalOut pdm;
	public:
		PdmPeriodOutUint8Soft(PinName pin, size_t samplePoints = 0) :
			pdm(pin),
			PeriodOutput<uint8_t>(samplePoints)
		{}

		PdmPeriodOutUint8Soft(const PdmPeriodOutUint8Soft &pdmPeriodOutput) :
			pdm(pdmPeriodOutput.pdm),
			PeriodOutput<uint8_t>(pdmPeriodOutput)
		{}

		virtual void output(size_t index) override
		{
			pdm.write(signal[index]);
		}

		virtual void setSignal(function<float(float)> periodFunction) override
		{
			float accumulator = 0;
			size_t signalSize = signal.size();
			for (size_t index = 0; index < signalSize; index++)
			{
				accumulator += periodFunction((float)index / signalSize);
				if (accumulator > 1.f)
				{
					accumulator -= 1.f;
					signal[index] = 1;
				}
				else
				{
					signal[index] = 0;
				}
			}
		}
	};


	template<size_t ArraySize>
	class PdmPeriodOutArrayUint8Soft :public PeriodOutputArray
	{
	private:
		//若性能较差可考虑用PortOut
		array<PdmPeriodOutUint8Soft, ArraySize> pdms;
		Ticker ticker;
		size_t index = 0;
		Frqer frqer;
		float actualRate;

		//pdm定时中断函数
		void tickerCallback()
		{
			if (getSamplePoints() != 0)
			{
				if (index >= getSamplePoints())
					index = 0;
				for (auto &p : pdms)
					p.output(index);
				index++;
			}
			actualRate = frqer.frq();
		}
	public:
		PdmPeriodOutArrayUint8Soft(
			const array<PdmPeriodOutUint8Soft, ArraySize> &pdms,
			float sampleRate = 50e3f
		) :
			pdms(pdms),
			PeriodOutputArray(sampleRate)
		{
			ticker.attach(
				callback(this, &PdmPeriodOutArrayUint8Soft<ArraySize>::tickerCallback),
				1.f / sampleRate
			);
		}

		virtual void setSampleRate(float sampleRate) override
		{
			PeriodOutputArray::setSampleRate(sampleRate);
			ticker.attach(
				callback(this, &PdmPeriodOutArrayUint8Soft<ArraySize>::tickerCallback),
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
			for (auto &p : pdms)
				p.setSamplePoints(samplePoints);
		}


		virtual void setSignal(function<float(float) > periodFunction, size_t n) override
		{
			pdms[n].setSignal(periodFunction);
		}

		virtual void setSignal(function<float(float)> periodFunction)
		{
			for (size_t i = 0; i < ArraySize; i++)
				setSignal(periodFunction, i);
		}
	};


}


