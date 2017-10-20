#pragma once
#include "mbed.h"
#include <array>
#include <algorithm>

#include "PeriodicSignal.h"
#include "Frqer.h"

namespace sky 
{
	//通过函数直接周期改变pwm占空比
	template<size_t N>
	class PwmPeriod :public array<float, N>
	{
	protected:
		PwmOut pwm;
	public:
		PwmPeriod(PinName pin, float frq = 500e3f, array<float, N> buf = { 0 }) :
			pwm(pin),
			array<float, N>(buf)
		{
			pwm.write(0);
			setFrq(frq);
		}

		//设置pwm频率
		void setFrq(float frq)
		{
			pwm.period(1 / frq);
		}

		//设置pwm占空比
		void setDuty(float duty)
		{
			pwm.write(duty);
		}
	};

	//周期控制pwm占空比类组
	template<size_t ArraySize, size_t BufferSize>
	class PwmPeriodArray:public array<PwmPeriod<BufferSize>, ArraySize>
	{
	protected:
		//设置更新速率开始输出
		virtual void init(float rate) = 0;

	public:
		PwmPeriodArray(
			array<PwmPeriod<BufferSize>, ArraySize> pwmPeriods,
			float frq = 500e3f
		) :
			array<PwmPeriod<BufferSize>, ArraySize>(pwmPeriods)
		{
			setFrq(frq);
		}

		//设置pwm频率
		void setFrq(float frq)
		{
			for_each(
				array<PwmPeriod<BufferSize>, ArraySize>::begin(),
				array<PwmPeriod<BufferSize>, ArraySize>::end(),
				[frq](auto &p) {p.setFrq(frq); }
			);
		}

		//设置刷新速率
		virtual void setRate(float rate) = 0;
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
		void init(float rate)
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
			for_each(
				array<PwmPeriod<BufferSize>, ArraySize>::begin(),
				array<PwmPeriod<BufferSize>, ArraySize>::end(),
				[this](auto &p) {p.setDuty(p[this->i]); }
			);
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
			init(rate);
		}

		//设置刷新速率
		void setRate(float rate)
		{
			ticker.detach();
			init(rate);
		}

		//获取实际刷新速率
		auto getRate()
		{
			return rate;
		}
	};

	//阵列蜂鸣器，单独控制相位、幅度
	template<size_t ArraySize, size_t BufferSize>
	class BuzzerArray
	{
	protected:
		PwmPeriodArray< ArraySize, BufferSize> *pwms;
		array<float, ArraySize> p2ps = { 0 }, phases = { 0 };

		//从p2ps和phases数组刷新pwms的buf
		void refreshBufs()
		{
			size_t i = 0;
			for (auto &p : *pwms)
			{
				sinPeriod(p.begin(), p.end(), p2ps[i], 0.5, phases[i]);
				i++;
			}
		}
	public:
		BuzzerArray(
			PwmPeriodArray< ArraySize, BufferSize> &pwms
		) :
			pwms(&pwms)
		{
			
		}

		//根据输入的p2ps数组刷新pwms的峰峰值
		void setP2ps(array<float, ArraySize> p2ps)
		{
			this->p2ps = p2ps;
			refreshBufs();
		}

		//根据输入的phases数组刷新pwms的相位
		void setPhases(array<float, ArraySize> phases)
		{
			this->phases = phases;
			refreshBufs();
		}

		//根据输入的p2ps和phases数组刷新pwms的峰峰值和相位
		void setSins(
			array<float, ArraySize> p2ps,
			array<float, ArraySize> phases
		)
		{
			this->p2ps = p2ps,
			this->phases = phases;
			refreshBufs();
		}

		//根据输入的数值刷新pwms的峰峰值
		void setP2ps(float p2ps)
		{
			for (auto &p2p : this->p2ps)
				p2p = p2ps;
			refreshBufs();
		}

		//根据输入的数值刷新pwms的相位
		void setPhases(float phases)
		{
			for (auto &phase : this->phases)
				phase = phases;
			refreshBufs();
		}

		//根据输入的数值刷新pwms的峰峰值和相位
		void setSins(
			float p2ps,
			float phases
		)
		{
			for (auto &p2p : this->p2ps)
				p2p = p2ps;
			for (auto &phase : this->phases)
				phase = phases;
			refreshBufs();
		}

		//设置pwm频率
		void setFrq(float frq)
		{
			pwms->setFrq(frq);
		}

		//设置刷新速率
		void setRate(float rate)
		{
			pwms->setRate(rate);
		}
	};


	//template<size_t N>
	//class PwmPeriodDma
	//{
	//protected:
	//	PwmOut pwm;
	//public:
	//	array<uint16_t, N> buf;

	//	PwmPeriodDma(PinName pin, array<uint16_t, N> buf) :
	//		pwm(pin),
	//		buf(buf)
	//	{
	//	}

	//	~PwmPeriodDma()
	//	{
	//	}
	//};

}


