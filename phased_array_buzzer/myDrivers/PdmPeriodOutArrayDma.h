#pragma once
#include "mbed.h"

#include "PeriodOutArray.h"
#include "myMath.h"

namespace sky
{
	using namespace std;

	//DMA接口，继承以实现不同的DMA配置
	class Dma
	{
	protected:
		DMA_HandleTypeDef *hdma;

	public:
		Dma() 
		{ 
			hdma = new DMA_HandleTypeDef{ 0 };
		}

		~Dma() 
		{
			HAL_DMA_DeInit(hdma);
			delete hdma; 
		}

		//开始传输
		virtual HAL_StatusTypeDef start(uint32_t *src, uint32_t *dst, uint16_t length) = 0;

		//结束传输
		virtual void stop() = 0;

		HAL_DMA_StateTypeDef getState()
		{
			return hdma->State;
		}
	};

	class DmaTimer :public Dma
	{
	protected:
		TIM_HandleTypeDef* htim;
	public:
		DmaTimer() 
		{
			htim = new TIM_HandleTypeDef{ 0 };
		}

		~DmaTimer() { 
			HAL_TIM_Base_DeInit(htim);
			delete htim; 
		}

		//设置定时器触发的频率
		virtual void setFrq(float frq) = 0;

	};

	class Dma2Timer1 :public DmaTimer
	{
	private:

	protected:
		Dma2Timer1(float frq) 
		{
			/* Peripheral clock enable */
			//MX_DMA_Init
			__HAL_RCC_DMA2_CLK_ENABLE();
			//HAL_TIM_Base_MspInit
			__HAL_RCC_TIM1_CLK_ENABLE();

			HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 0, 0);
			HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);

			//MX_TIM1_Init
			TIM_ClockConfigTypeDef sClockSourceConfig;
			TIM_MasterConfigTypeDef sMasterConfig;

			htim->Instance = TIM1;
			setFrq(frq);
			htim->Init.CounterMode = TIM_COUNTERMODE_UP;
			htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
			htim->Init.RepetitionCounter = 0;
			htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

			if (HAL_TIM_Base_Init(htim) != HAL_OK)
			{
				while (1);
			}

			sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
			if (HAL_TIM_ConfigClockSource(htim, &sClockSourceConfig) != HAL_OK)
			{
				while (1);
			}

			sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
			sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
			sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
			if (HAL_TIMEx_MasterConfigSynchronization(htim, &sMasterConfig) != HAL_OK)
			{
				while (1);
			}


			/* TIM1 DMA Init */
			/* TIM1_UP Init */
			hdma->Instance = DMA2_Stream5;
			hdma->Init.Channel = DMA_CHANNEL_6;
			hdma->Init.Direction = DMA_MEMORY_TO_PERIPH;
			hdma->Init.PeriphInc = DMA_PINC_DISABLE;
			hdma->Init.MemInc = DMA_MINC_ENABLE;
			hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
			hdma->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
			hdma->Init.Mode = DMA_CIRCULAR;
			hdma->Init.Priority = DMA_PRIORITY_HIGH;
			hdma->Init.FIFOMode = DMA_FIFOMODE_DISABLE;
			if (HAL_DMA_Init(hdma) != HAL_OK)
			{
				while (1);
			}

			__HAL_LINKDMA(htim, hdma[TIM_DMA_ID_UPDATE], *hdma);

		}

	public:
		static Dma2Timer1* instance(float frq)
		{
			static Dma2Timer1 *instance = new Dma2Timer1(frq);
			return instance;
		}

		//设置频率
		virtual void setFrq(float frq) override
		{
			//计算Prescaler和Period
			
			uint32_t scaler = SystemCoreClock / frq;
			uint16_t psc = scaler / 65536, period = scaler / (psc + 1) - 1;

			htim->Init.Prescaler = psc;
			htim->Init.Period = period;

			htim->Instance->PSC = psc;
			htim->Instance->ARR = period;
		}

		virtual HAL_StatusTypeDef start(uint32_t *src, uint32_t *dst, uint16_t length) override
		{
			if ((htim->State == HAL_TIM_STATE_BUSY))
			{
				return HAL_BUSY;
			}
			else if ((htim->State == HAL_TIM_STATE_READY))
			{
				if ((src == 0) && (length > 0))
				{
					return HAL_ERROR;
				}
				else
				{
					htim->State = HAL_TIM_STATE_BUSY;
				}
			}

			/* Set the DMA error callback */
			//hdma->XferErrorCallback = completeCallback;

			/* Enable the DMA Stream */
			HAL_DMA_Start(htim->hdma[TIM_DMA_ID_UPDATE], (uint32_t)src, (uint32_t)dst, length);

			/* Enable the TIM Update DMA request */
			__HAL_TIM_ENABLE_DMA(htim, TIM_DMA_UPDATE);

			/* Enable the Peripheral */
			__HAL_TIM_ENABLE(htim);

			/* Return function status */
			return HAL_OK;
		}

		virtual void stop() override
		{
			HAL_TIM_Base_DeInit(htim);
			HAL_DMA_Abort(hdma);
		}

		~Dma2Timer1()
		{
			__HAL_RCC_DMA2_CLK_DISABLE();
			__HAL_RCC_TIM1_CLK_DISABLE();
		}

	};

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

