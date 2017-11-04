#pragma once
#include "mbed.h"
#include <array>
#include <algorithm>

#include "PeriodicSignal.h"
#include "Frqer.h"

namespace sky
{
	using namespace std;

	//阵列蜂鸣器，单独控制相位、幅度
	template<size_t ArraySize>
	class SpeakerArray
	{
	protected:
		PeriodOutputArray *outputs;
		array<float, ArraySize> p2ps = { 0 }, phases = { 0 };

		//从p2ps和phases数组刷新pwms的buf
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

		//根据输入的p2ps数组刷新输出信号峰峰值
		void setP2ps(const array<float, ArraySize> &p2ps)
		{
			this->p2ps = p2ps;
			refreshSignal();
		}

		//根据输入的phases数组刷新输出信号的相位
		void setPhases(const array<float, ArraySize> &phases)
		{
			this->phases = phases;
			refreshSignal();
		}

		//根据输入的p2ps和phases数组刷新输出信号的峰峰值和相位
		void setSins(
			const array<float, ArraySize> & p2ps,
			const array<float, ArraySize> & phases
		)
		{
			this->p2ps = p2ps;
			this->phases = phases;
			refreshSignal();
		}

		//根据输入的数值刷新输出信号的峰峰值
		void setP2ps(float p2ps)
		{
			for (auto &p2p : this->p2ps)
				p2p = p2ps;
			refreshSignal();
		}

		//根据输入的数值刷新输出信号的相位
		void setPhases(float phases)
		{
			for (auto &phase : this->phases)
				phase = phases;
			refreshSignal();
		}

		//根据输入的数值刷新输出信号的峰峰值和相位
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

		//设置正弦信号频率
		void setFrq_withoutChangingSampleRate(float frq)
		{
			setPointsAndRefresh(outputs->getSampleRate() / frq);
		}
		void setFrq_byChangingSampleRate(float frq)
		{
			outputs->setSampleRate(outputs->getSamplePoints()*frq);
		}

		//设置采样速率
		void setSampleRate(float rate)
		{
			outputs->setSampleRate(rate);
		}

		//设置样本点数
		void setSamplePoints(size_t samplePoints)
		{
			outputs->setSamplePoints(samplePoints);
		}

		//设置样本点数并刷新正弦样本点
		void setPointsAndRefresh(size_t samplePoints)
		{
			outputs->setSamplePoints(samplePoints);
			refreshSignal();
		}

	};
}