#pragma once
#include "mbed.h"
#include <array>
#include <algorithm>

#include "PeriodicSignal.h"
#include "Frqer.h"

namespace sky 
{
	//ͨ������ֱ�����ڸı�pwmռ�ձ�
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

		//����pwmƵ��
		void setFrq(float frq)
		{
			pwm.period(1 / frq);
		}

		//����pwmռ�ձ�
		void setDuty(float duty)
		{
			pwm.write(duty);
		}
	};

	//���ڿ���pwmռ�ձ�����
	template<size_t ArraySize, size_t BufferSize>
	class PwmPeriodArray:public array<PwmPeriod<BufferSize>, ArraySize>
	{
	protected:
		//���ø������ʿ�ʼ���
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

		//����pwmƵ��
		void setFrq(float frq)
		{
			for_each(
				array<PwmPeriod<BufferSize>, ArraySize>::begin(),
				array<PwmPeriod<BufferSize>, ArraySize>::end(),
				[frq](auto &p) {p.setFrq(frq); }
			);
		}

		//����ˢ������
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

		//�󶨶�ʱ����ʼ���
		void init(float rate)
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

		//����ˢ������
		void setRate(float rate)
		{
			ticker.detach();
			init(rate);
		}

		//��ȡʵ��ˢ������
		auto getRate()
		{
			return rate;
		}
	};

	//���з�����������������λ������
	template<size_t ArraySize, size_t BufferSize>
	class BuzzerArray
	{
	protected:
		PwmPeriodArray< ArraySize, BufferSize> *pwms;
		array<float, ArraySize> p2ps = { 0 }, phases = { 0 };

		//��p2ps��phases����ˢ��pwms��buf
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

		//���������p2ps����ˢ��pwms�ķ��ֵ
		void setP2ps(array<float, ArraySize> p2ps)
		{
			this->p2ps = p2ps;
			refreshBufs();
		}

		//���������phases����ˢ��pwms����λ
		void setPhases(array<float, ArraySize> phases)
		{
			this->phases = phases;
			refreshBufs();
		}

		//���������p2ps��phases����ˢ��pwms�ķ��ֵ����λ
		void setSins(
			array<float, ArraySize> p2ps,
			array<float, ArraySize> phases
		)
		{
			this->p2ps = p2ps,
			this->phases = phases;
			refreshBufs();
		}

		//�����������ֵˢ��pwms�ķ��ֵ
		void setP2ps(float p2ps)
		{
			for (auto &p2p : this->p2ps)
				p2p = p2ps;
			refreshBufs();
		}

		//�����������ֵˢ��pwms����λ
		void setPhases(float phases)
		{
			for (auto &phase : this->phases)
				phase = phases;
			refreshBufs();
		}

		//�����������ֵˢ��pwms�ķ��ֵ����λ
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

		//����pwmƵ��
		void setFrq(float frq)
		{
			pwms->setFrq(frq);
		}

		//����ˢ������
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


