#pragma once
#include "mbed.h"
#include <vector>
#include <functional>
#include <algorithm>

#include "PeriodicSignal.h"
#include "Frqer.h"

namespace sky
{
	using namespace std;

	//�ӿ� PeriodOutputArray
	class PeriodOutputArray
	{
	private:
		float sampleRate = 0;
		size_t samplePoints = 0;
	public:
		PeriodOutputArray(float sampleRate = 0):
			sampleRate(sampleRate)
		{};

		//���ò�����
		virtual void setSampleRate(float sampleRate)
		{
			this->sampleRate = sampleRate;
		}

		//��ȡ������
		virtual float getSampleRate()
		{
			return sampleRate;
		}

		//������������
		virtual void setSamplePoints(size_t samplePoints)
		{
			this->samplePoints = samplePoints;
		}

		//��ȡ��������
		virtual size_t getSamplePoints()
		{
			return samplePoints;
		}

		//���������ź�
		//���룺
		//setSignal(
		//	[](float t)->float
		//{
		//	float y;
		//	...
		//		return y;
		//},
		//n
		//);
		virtual void setSignal(function<float(float)> periodFunction, size_t n) = 0;

	};

	//�ӿ� PeriodOutput
	//�������������������ϵ��������
	template<typename BufT>
	class PeriodOutput
	{
	protected:
		vector<BufT> signal;
	public:
		PeriodOutput(vector<BufT> signalInit = {}) :
			signal(signalInit)
		{}

		PeriodOutput(size_t samplePoints) :
			signal(samplePoints, 0)
		{}

		PeriodOutput(const PeriodOutput &periodOutput) :
			signal(periodOutput.signal)
		{}

		//������������
		virtual void setSamplePoints(size_t samplePoints)
		{
			signal.resize(samplePoints);
		}

		//���������ź�
		//���룺
		//setSignal(
		//	[](float t)->float
		//{
		//	float y;
		//	...
		//		return y;
		//}
		//);
		virtual void setSignal(function<float(float)> periodFunction)
		{
			size_t index = 0;
			for (auto &y : signal)
				y = periodFunction((float)index++/signal.size());
		}

		//�����һ���ź�
		//Ĭ���޲��������������������Բ���ʵ��
		virtual void output(size_t index) {};
	};

}