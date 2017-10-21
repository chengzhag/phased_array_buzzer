#pragma once
#include "mbed.h"
#include <array>
#include <algorithm>

#include "PeriodicSignal.h"
#include "PwmPeriodOutputArray.h"
#include "Frqer.h"

namespace sky
{
	using namespace std;

	//���з�����������������λ������
	template<size_t ArraySize>
	class BuzzerArray
	{
	protected:
		PeriodOutputArray *outputs;
		array<float, ArraySize> p2ps = { 0 }, phases = { 0 };

		//��p2ps��phases����ˢ��pwms��buf
		void refreshSignal()
		{
			for (size_t i = 0; i < ArraySize; i++)
				outputs->setSignal(
					[this, i](float x) {return float(this->p2ps[i] * 0.5* sin(2 * PI *x + this->phases[i]) + 0.5); },
					i
				);
		}
	public:
		BuzzerArray(
			PeriodOutputArray &outputs
		) :
			outputs(&outputs)
		{

		}

		//���������p2ps����ˢ������źŷ��ֵ
		void setP2ps(const array<float, ArraySize> &p2ps)
		{
			this->p2ps = p2ps;
			refreshSignal();
		}

		//���������phases����ˢ������źŵ���λ
		void setPhases(const array<float, ArraySize> & phases)
		{
			this->phases = phases;
			refreshSignal();
		}

		//���������p2ps��phases����ˢ������źŵķ��ֵ����λ
		void setSins(
			const array<float, ArraySize> & p2ps,
			const array<float, ArraySize> & phases
		)
		{
			this->p2ps = p2ps;
			this->phases = phases;
			refreshSignal();
		}

		//�����������ֵˢ������źŵķ��ֵ
		void setP2ps(float p2ps)
		{
			for (auto &p2p : this->p2ps)
				p2p = p2ps;
			refreshSignal();
		}

		//�����������ֵˢ������źŵ���λ
		void setPhases(float phases)
		{
			for (auto &phase : this->phases)
				phase = phases;
			refreshSignal();
		}

		//�����������ֵˢ������źŵķ��ֵ����λ
		void setSins(
			float p2ps,
			float phases
		)
		{
			for (auto &p2p : this->p2ps)
				p2p = p2ps;
			for (auto &phase : this->phases)
				phase = phases;
			refreshSignal();
		}

		//���ò�������
		void setSampleRate(float rate)
		{
			outputs->setSampleRate(rate);
		}

		//������������
		void setSamplePoints(size_t samplePoints)
		{
			outputs->setSamplePoints(samplePoints);
			refreshSignal();
		}
	};
}