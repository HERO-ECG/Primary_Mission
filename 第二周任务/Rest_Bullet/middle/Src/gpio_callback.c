#include "main.h"
#include "gpio.h"
#include "can.h"
#include "m_moto.h"
#include "control.h"
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_1)//This paramater has been changed by LiJu.
	{
		/*
		#ifdef ChassisPowerControl
			static int ModeSwitch = 0;
			if(ModeSwitch == 1)
			{
				TX_DATA_Power(&hcan1, POWER_ID, 1, 80);			//40W
				if(HAL_CAN_Transmit(&hcan1, 10) == HAL_OK)
				{
					HAL_GPIO_WritePin(GPIOE,GPIO_PIN_11, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOF,GPIO_PIN_14, GPIO_PIN_RESET);
				}
				ModeSwitch = 0;
			}
			else
			{
				TX_DATA_Power(&hcan1, POWER_ID, 0, 80);			//80W
				if(HAL_CAN_Transmit(&hcan1, 10)==HAL_OK)
				{
					HAL_GPIO_WritePin(GPIOE,GPIO_PIN_11, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOF,GPIO_PIN_14, GPIO_PIN_SET);
				}
				ModeSwitch = 1;
			}
		#endif
		*/
	}
	else if(GPIO_Pin == GPIO_PIN_2)
	{
		#ifdef Fricition
			if(!HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_2))
			{
				++(Fricition_MOTO[0].getpara.round_cnt);
			}
		#endif
	}
}


