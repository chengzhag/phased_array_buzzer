#pragma once
#include "mbed.h"
#include <array>
#include <algorithm>

#include "PeriodicSignal.h"
#include "PeriodOutArray.h"
#include "Frqer.h"

namespace sky 
{
	using namespace std;


	class PwmPeriodOutSoft :public PeriodOutput<float>
	{
	private:
		PwmOut pwm;
	public:
		PwmPeriodOutSoft(PinName pin, size_t samplePoints = 0) :
			pwm(pin),
			PeriodOutput<float>(samplePoints)
		{}

		PwmPeriodOutSoft(const PwmPeriodOutSoft &pwmPeriodOutput) :
			pwm(pwmPeriodOutput.pwm),
			PeriodOutput<float>(pwmPeriodOutput)
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
	class PwmPeriodOutArraySoft :public PeriodOutputArray
	{
	private:
		array<PwmPeriodOutSoft, ArraySize> pwms;
		Ticker ticker;
		size_t index = 0;
		Frqer frqer;
		float actualRate;

		//pwmռ�ձȶ�ʱ�жϺ���
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
		PwmPeriodOutArraySoft(
			const array<PwmPeriodOutSoft, ArraySize> &pwms,
			float pwmFrq = 100e3f, 
			float sampleRate = 16e3f
		) :
			pwms(pwms),
			PeriodOutputArray(sampleRate)
		{
			setPwmFrq(pwmFrq);
			ticker.attach(
				callback(this, &PwmPeriodOutArraySoft<ArraySize>::tickerCallback),
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
				callback(this, &PwmPeriodOutArraySoft<ArraySize>::tickerCallback),
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


