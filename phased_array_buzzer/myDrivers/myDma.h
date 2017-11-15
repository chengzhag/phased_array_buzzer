#pragma once
#include "mbed.h"

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
		static void _Error_Handler(char * file, int line) { while (1) {} }
		static void _Error_Handler() { _Error_Handler(__FILE__, __LINE__); }

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
				_Error_Handler(__FILE__, __LINE__);
			}

			sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
			if (HAL_TIM_ConfigClockSource(htim, &sClockSourceConfig) != HAL_OK)
			{
				_Error_Handler(__FILE__, __LINE__);
			}

			sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
			sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
			sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
			if (HAL_TIMEx_MasterConfigSynchronization(htim, &sMasterConfig) != HAL_OK)
			{
				_Error_Handler(__FILE__, __LINE__);
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
				_Error_Handler(__FILE__, __LINE__);
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
}