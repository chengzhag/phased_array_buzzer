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

	class PwmPeriodOutput :public PeriodOutput
	{
	protected:
		PwmOut pwm;
	public:
		PwmPeriodOutput(PinName pin, size_t samplePoints = 0) :
			pwm(pin),
			PeriodOutput(samplePoints)
		{}

		PwmPeriodOutput(const PwmPeriodOutput &pwmPeriodOutput) :
			pwm(pwmPeriodOutput.pwm),
			PeriodOutput(pwmPeriodOutput)
		{}

		//����pwmƵ��
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
	protected:
		array<PwmPeriodOutput, ArraySize> pwms;
		Ticker ticker;
		size_t index = 0;
		Frqer frqer;
		float actualRate;

		//pwmռ�ձȶ�ʱ�жϺ���
		void tickerCallback()
		{
			if (index >= PeriodOutputArray::samplePoints)
				index = 0;
			if (PeriodOutputArray::samplePoints != 0)
				for (auto &p : pwms)
					p.output(index);
			index++;
			actualRate = frqer.frq();
		}
	public:
		PwmPeriodOutputArray(
			const array<PwmPeriodOutput, ArraySize>& pwms, 
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

		//��ȡʵ��ˢ������
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

