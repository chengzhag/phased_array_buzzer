#pragma once
#include "mbed.h"
#include <array>

#include "PeriodicSignal.h"
#include "Frqer.h"

namespace sky 
{
	template<size_t N>
	class PwmPeriodTicker
	{
	protected:
		PwmOut pwm;
		size_t i;
	public:
		array<float, N> buf;

		PwmPeriodTicker(PinName pin, array<float, N> buf) :
			pwm(pin),
			buf(buf),
			i(0)
		{

		}

		void refresh()
		{
			if (i == N)
			{
				i = 0;
			}
			pwm.write(buf[i]);
			i++;
		}
	};

	template<size_t ArraySize, size_t BufferSize>
	class PwmPeriodArray
	{
	protected:
		array<PwmPeriodTicker<BufferSize>, ArraySize> pwms;
		Ticker ticker;
		Frqer frqer;
		float _rate;

		void init(float rate)
		{
			for (auto &p : pwms)
			{
				sinPeriod(
					p.buf.begin(),
					p.buf.end(),
					1,
					0.5
				);
			}
			ticker.attach(
				callback(
					this,
					&PwmPeriodArray<ArraySize,  BufferSize>::refresh
					),
				1.f / rate
			);
		}

		void refresh()
		{
			for (auto &p : pwms)
			{
				p.refresh();
			}
			_rate = frqer.frq();
		}

	public:
		PwmPeriodArray(
			array<PwmPeriodTicker<BufferSize>, ArraySize> pwmPeriodCpus,
			float rate
		) :
			pwms(pwmPeriodCpus)
		{
			init(rate);
		}

		float rate()
		{
			return _rate;
		}
	};



	template<size_t N>
	class PwmPeriodDma
	{
	protected:
		PwmOut pwm;
	public:
		array<uint16_t, N> buf;

		PwmPeriodDma(PinName pin, array<uint16_t, N> buf) :
			pwm(pin),
			buf(buf)
		{
		}

		~PwmPeriodDma()
		{
		}
	};

}


