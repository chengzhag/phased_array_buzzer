#pragma once
#include "mbed.h"

#include "PeriodOutArray.h"
#include "myMath.h"
#include "myDma.h"

namespace sky
{
	using namespace std;

	//从一个port输出16路pdm信号
	class PdmPeriodOutArrayDma :public PeriodOutputArray
	{
	private:
		PortOut pout;
		GPIO_TypeDef *gpio;
		vector<uint16_t> signal;
		Dma2Timer1* dma;

		GPIO_TypeDef * getGpio(PortName port)
		{
			uint32_t gpio_add = 0;
			switch (port) {
			case PortA:
				gpio_add = GPIOA_BASE;
				break;
			case PortB:
				gpio_add = GPIOB_BASE;
				break;
#if defined(GPIOC_BASE)
			case PortC:
				gpio_add = GPIOC_BASE;
				break;
#endif
#if defined GPIOD_BASE
			case PortD:
				gpio_add = GPIOD_BASE;
				break;
#endif
#if defined GPIOE_BASE
			case PortE:
				gpio_add = GPIOE_BASE;
				break;
#endif
#if defined GPIOF_BASE
			case PortF:
				gpio_add = GPIOF_BASE;
				break;
#endif
#if defined GPIOG_BASE
			case PortG:
				gpio_add = GPIOG_BASE;
				break;
#endif
#if defined GPIOH_BASE
			case PortH:
				gpio_add = GPIOH_BASE;
				break;
#endif
#if defined GPIOI_BASE
			case PortI:
				gpio_add = GPIOI_BASE;
				break;
#endif
#if defined GPIOJ_BASE
			case PortJ:
				gpio_add = GPIOJ_BASE;
				break;
#endif
#if defined GPIOK_BASE
			case PortK:
				gpio_add = GPIOK_BASE;
				break;
#endif
			default:
				error("Pinmap error: wrong port number.");
				break;
			}
			return (GPIO_TypeDef *)gpio_add;
		}


		void _setSignal(function<float(float) > periodFunction, size_t n)
		{
			float accumulator = 0.5, increase = 0;
			size_t signalSize = signal.size();
			float step = 1 / (float)signalSize;
			for (size_t index = 0; index < signalSize; index++)
			{
				increase = periodFunction((float)index / signalSize);
				limit<float>(increase, 0.f, 1.f);
				accumulator += increase;
				if (accumulator > 1.f)
				{
					accumulator -= 1.f;
					signal[index] |= 1 << n;
				}
				else
				{
					signal[index] &= ~(1 << n);
				}
			}
		}

		void start()
		{
			dma->start((uint32_t *)signal.data(), (uint32_t *)&gpio->ODR, getSamplePoints());
		}

		void stop()
		{
			dma->stop();
		}

	public:
		PdmPeriodOutArrayDma(
			PortName port,
			float sampleRate = 50e3f
		) :
			pout(port),
			PeriodOutputArray(sampleRate),
			dma(Dma2Timer1::instance(sampleRate)),
			gpio(getGpio(port))
		{
		}

		virtual void setSampleRate(float sampleRate) override
		{
			PeriodOutputArray::setSampleRate(sampleRate);
			dma->setFrq(sampleRate);
		}

		//设置了采样点数后必须再调用setSignal以设置输出
		virtual void setSamplePoints(size_t samplePoints) override
		{
			PeriodOutputArray::setSamplePoints(samplePoints);
			stop();
			signal.resize(samplePoints);
			start();
		}

		virtual void setSignal(function<float(float, size_t) > periodFunction) override
		{
			stop();
			for (size_t i = 0; i < 16; i++)
				_setSignal([i, periodFunction](float x) {return periodFunction(x, i); }, i);
			start();
		}

		virtual void setSignal(function<float(float) > periodFunction, size_t n) override
		{
			stop();
			_setSignal(periodFunction, n);
			start();
		}

		virtual void setSignal(function<float(float)> periodFunction)
		{
			stop();
			for (size_t i = 0; i < 16; i++)
				_setSignal(periodFunction, i);
			start();
		}
	};

}

