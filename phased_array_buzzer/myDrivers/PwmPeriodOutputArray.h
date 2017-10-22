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


	class PwmPeriodOutput :public PeriodOutput<float>
	{
	private:
		PwmOut pwm;
	public:
		PwmPeriodOutput(PinName pin, size_t samplePoints = 0) :
			pwm(pin),
			PeriodOutput<float>(samplePoints)
		{}

		PwmPeriodOutput(const PwmPeriodOutput &pwmPeriodOutput) :
			pwm(pwmPeriodOutput.pwm),
			PeriodOutput<float>(pwmPeriodOutput)
		{}

		//设置pwm频率
		void setPwmFrq(float frq)
		{
			pwm.period(1 / frq);
		}

		virtual void output(size_t index) override
		{
			pwm.write(signal[index]);
		}

	};

	template<size_t ArraySize>
	class PwmPeriodOutputArray :public PeriodOutputArray
	{
	private:
		array<PwmPeriodOutput, ArraySize> pwms;
		Ticker ticker;
		size_t index = 0;
		Frqer frqer;
		float actualRate;

		//pwm占空比定时中断函数
		void tickerCallback()
		{
			if (getSamplePoints() != 0)
			{
				if (index >= getSamplePoints())
					index = 0;
				for (auto &p : pwms)
					p.output(index);
				index++;
			}
			actualRate = frqer.frq();
		}
	public:
		PwmPeriodOutputArray(
			const array<PwmPeriodOutput, ArraySize> &pwms,
			float pwmFrq = 100e3f, 
			float sampleRate = 16e3f
		) :
			pwms(pwms),
			PeriodOutputArray(sampleRate)
		{
			setPwmFrq(pwmFrq);
			ticker.attach(
				callback(this, &PwmPeriodOutputArray<ArraySize>::tickerCallback),
				1.f / sampleRate
			);
		}

		void setPwmFrq(float pwmFrq)
		{
			for (auto &p : pwms)
				p.setPwmFrq(pwmFrq);
		}

		virtual void setSampleRate(float sampleRate) override
		{
			PeriodOutputArray::setSampleRate(sampleRate);
			ticker.attach(
				callback(this, &PwmPeriodOutputArray<ArraySize>::tickerCallback),
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
			for (auto &p : pwms)
				p.setSamplePoints(samplePoints);
		}


		virtual void setSignal(function<float(float) > periodFunction, size_t n) override
		{
			pwms[n].setSignal(periodFunction);
		}

		virtual void setSignal(function<float(float)> periodFunction)
		{
			for (size_t i = 0; i < ArraySize; i++)
				setSignal(periodFunction, i);
		}
	};




}


