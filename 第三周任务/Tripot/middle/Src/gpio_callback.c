#include "main.h"
#include "gpio.h"
#include "can.h"
#include "m_moto.h"
#include "control.h"
#include "strike.h"

uint32_t delay1;
uint32_t bulletcount;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_0)
	{
		#ifdef Fricition
			if(!HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0))//摩擦轮反馈
			{
				++(Fricition_MOTO[0].getpara.round_cnt);
			}
		#endif
	}
	else if(GPIO_Pin == GPIO_PIN_1)
	{
		#ifdef Fricition
			if(!HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_1))//摩擦轮反馈
			{
				++(Fricition_MOTO[1].getpara.round_cnt);
			}
		#endif
	}
	else if (GPIO_Pin == GPIO_PIN_2)
	{
		
		//HAL_NVIC_DisableIRQ(EXTI2_IRQn);

		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_2)==GPIO_PIN_RESET)//低电平 进入
		{
//			if(TIM4->CNT>delay1)
//			{
//				bulletcount++;
//				TIM4->CNT=0;
//			}
			
	//		Strike_IntFun_in();	

			
		}
		else//高电平 离开
		{
			


		//	Strike_IntFun_out();	
		}

		//HAL_NVIC_EnableIRQ(EXTI2_IRQn);
		

	}
	
}


