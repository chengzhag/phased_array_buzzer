#pragma once
#include "mbed.h"
#include <array>

#include "PeriodicSignal.h"
#include "Frqer.h"

namespace sky 
{
	//通过函数直接周期改变pwm占空比
	template<size_t N>
	class PwmPeriod
	{
	protected:
	public:
		PwmOut pwm;
		array<float, N> buf;

		PwmPeriod(PinName pin, float frq = 500e3f, array<float, N> buf = { 0 }) :
			pwm(pin),
			buf(buf)
		{
			pwm.write(0);
			setFrq(frq);
		}

		//设置pwm频率
		void setFrq(float frq)
		{
			pwm.period(1 / frq);
		}
	};

	//周期控制pwm占空比类组
	template<size_t ArraySize, size_t BufferSize>
	class PwmPeriodArray
	{
	protected:
		array<PwmPeriod<BufferSize>, ArraySize> pwms;

		//设置更新速率开始输出
		virtual void begin(float rate) = 0;

	public:
		PwmPeriodArray(
			array<PwmPeriod<BufferSize>, ArraySize> pwmPeriods,
			float frq = 500e3f
		) :
			pwms(pwmPeriods)
		{
			setFrq(frq);
		}

		//设置pwm频率
		void setFrq(float frq)
		{
			for (auto p : pwms)
			{
				p.setFrq(frq);
			}
		}

		
	};

	template<size_t ArraySize, size_t BufferSize>
	class PwmPeriodDirectArray :public PwmPeriodArray<ArraySize, BufferSize>
	{
	protected:
		Ticker ticker;
		Frqer frqer;
		float rate;
		size_t i = 0;

		//绑定定时器开始输出
		void begin(float rate)
		{
			ticker.attach(
				callback(this, &PwmPeriodDirectArray<ArraySize, BufferSize>::refresh),
				1.f / rate
			);
		}

		//pwm占空比定时中断函数
		void refresh()
		{
			if (i == BufferSize)
				i = 0;
			for (auto &p : PwmPeriodArray<ArraySize, BufferSize>::pwms)
				p.pwm.write(p.buf[i]);
			i++;
			rate = frqer.frq();
		}

	public:
		PwmPeriodDirectArray(
			array<PwmPeriod<BufferSize>, ArraySize> pwmPeriods,
			float frq = 500e3f,
			float rate = 20e3f
		) :
			PwmPeriodArray<ArraySize, BufferSize>(
				pwmPeriods,
				frq
				)
		{
			begin(rate);
		}

		//获取实际刷新速率
		float getRate()
		{
			return rate;
		}
	};

	//阵列蜂鸣器，单独控制相位、幅度
	template<size_t ArraySize, size_t BufferSize>
	class BuzzerArray
	{
	protected:
	public:
		BuzzerArray(

		) 
		{
		
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


