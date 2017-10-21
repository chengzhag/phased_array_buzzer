#pragma once
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


	template<size_t ArraySize>
	class PdmPeriodOutputArray :public PeriodOutputArray
	{
	protected:
		//TODO��pdm IO���ں�buf: vector<bitset<ArraySize>>
		Ticker ticker;
		size_t index = 0;
		Frqer frqer;
		float actualRate;

		//pwmռ�ձȶ�ʱ�жϺ���
		void tickerCallback()
		{
			if (index >= PeriodOutputArray::samplePoints)
				index = 0;
			//if (PeriodOutputArray::samplePoints != 0)
			//	for (auto &p : pwms)
					//p.output(index);
					//TODO���������
			index++;
			actualRate = frqer.frq();
		}
	public:
		PdmPeriodOutputArray(
			float sampleRate = 16e3f
		) :
			PeriodOutputArray(sampleRate)
		{
			ticker.attach(
				callback(this, &PdmPeriodOutputArray<ArraySize>::tickerCallback),
				1.f / sampleRate//TODO��sampleRate���ж�Ƶ��֮��Ĺ�ϵ
			);
		}

		virtual void setSampleRate(float sampleRate) override
		{
			PeriodOutputArray::setSampleRate(sampleRate);
			ticker.attach(
				callback(this, &PdmPeriodOutputArray<ArraySize>::tickerCallback),
				1.f / sampleRate//TODO��sampleRate���ж�Ƶ��֮��Ĺ�ϵ
			);
		}

		//��ȡʵ��ˢ������
		auto getActualRate()
		{
			return actualRate;
		}


		virtual void setSamplePoints(size_t samplePoints) override
		{
			//TODO��samplePoints��buf���ȵļ���
			//PeriodOutputArray::setSamplePoints(samplePoints);
			//for (auto &p : pwms)
			//	p.setSamplePoints(samplePoints);
		}


		virtual void setSignal(function<float(float) > periodFunction, size_t n) override
		{
			//TODO�����ȵ�buf pdm�������е�ת��
			//pwms[n].setSignal(periodFunction);
		}

		virtual void setSignal(function<float(float)> periodFunction)
		{
			for (size_t i = 0; i < ArraySize; i++)
				setSignal(periodFunction, i);
		}
	};




}


