#pragma once
#include "mbed.h"
#include <array>
#include <algorithm>

#include "PeriodicSignal.h"
#include "Frqer.h"

namespace sky
{
	using namespace std;

	//���з�����������������λ������
	template<size_t ArraySize>
	class SpeakerArray
	{
	protected:
		PeriodOutputArray *outputs;
		array<float, ArraySize> p2ps = { 0 }, phases = { 0 };

		//��p2ps��phases����ˢ��pwms��buf
		void refreshSignal()
		{
			outputs->setSignal(
				[this](float x, size_t i) {return float(this->p2ps[i] * 0.5* sin(2 * PI *x + this->phases[i]) + 0.5); }
			);
		}
	public:
		SpeakerArray(
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
		void setPhases(const array<float, ArraySize> &phases)
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

		//���������ź�Ƶ��
		void setFrq_withoutChangingSampleRate(float frq)
		{
			setPointsAndRefresh(outputs->getSampleRate() / frq);
		}
		void setFrq_byChangingSampleRate(float frq)
		{
			outputs->setSampleRate(outputs->getSamplePoints()*frq);
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
		}

		//��������������ˢ������������
		void setPointsAndRefresh(size_t samplePoints)
		{
			outputs->setSamplePoints(samplePoints);
			refreshSignal();
		}

	};
}