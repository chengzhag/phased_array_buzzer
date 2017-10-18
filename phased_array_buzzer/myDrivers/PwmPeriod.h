#pragma once
#include "mbed.h"
#include <array>

#include "PeriodicSignal.h"
#include "Frqer.h"

namespace sky 
{
	//ͨ������ֱ�����ڸı�pwmռ�ձ�
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

		//����pwmƵ��
		void setFrq(float frq)
		{
			pwm.period(1 / frq);
		}
	};

	//���ڿ���pwmռ�ձ�����
	template<size_t ArraySize, size_t BufferSize>
	class PwmPeriodArray
	{
	protected:
		array<PwmPeriod<BufferSize>, ArraySize> pwms;

		//���ø������ʿ�ʼ���
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

		//����pwmƵ��
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

		//�󶨶�ʱ����ʼ���
		void begin(float rate)
		{
			ticker.attach(
				callback(this, &PwmPeriodDirectArray<ArraySize, BufferSize>::refresh),
				1.f / rate
			);
		}

		//pwmռ�ձȶ�ʱ�жϺ���
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

		//��ȡʵ��ˢ������
		float getRate()
		{
			return rate;
		}
	};

	//���з�����������������λ������
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


