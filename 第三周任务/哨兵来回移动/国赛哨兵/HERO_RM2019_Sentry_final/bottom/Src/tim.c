#include "tim.h"
#include "mytype.h"

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim8;




/*-----------------TIM2--------------*/
void TIM2_Init(u16 arr,u16 psc)
{  
    htim2.Instance=TIM2;                          		//ͨ�ö�ʱ��2
    htim2.Init.Prescaler=psc;                     		//��Ƶϵ��
    htim2.Init.CounterMode=TIM_COUNTERMODE_UP;    		//���ϼ�����
    htim2.Init.Period=arr;                        		//�Զ�װ��ֵ
    htim2.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;	//ʱ�ӷ�Ƶ����
	  htim2.Init.RepetitionCounter=0x00;                //�����ظ�ϵ��
    HAL_TIM_Base_Init(&htim2);												//ʹ�ܶ�ʱ��2

}
void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim2);
}
/***TIM2***/





/*-----------------TIM3--------------*/
void TIM3_Init(u16 arr,u16 psc)
{  
    htim3.Instance=TIM3;                          		//ͨ�ö�ʱ��3
    htim3.Init.Prescaler=psc;                     		//��Ƶϵ��
    htim3.Init.CounterMode=TIM_COUNTERMODE_UP;    		//���ϼ�����
    htim3.Init.Period=arr;                        		//�Զ�װ��ֵ
    htim3.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;	//ʱ�ӷ�Ƶ����
		htim3.Init.RepetitionCounter=0x00;                //�����ظ�ϵ��
    HAL_TIM_Base_Init(&htim3);												//ʹ�ܶ�ʱ��3

}
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim3);
}
/***TIM3***/



/*-----------------TIM4--------------*/
void TIM4_Init(u16 arr,u16 psc)
{  
    htim4.Instance=TIM4;                          		//ͨ�ö�ʱ��4
    htim4.Init.Prescaler=psc;                     		//��Ƶϵ��
    htim4.Init.CounterMode=TIM_COUNTERMODE_UP;    		//���ϼ�����
    htim4.Init.Period=arr;                        		//�Զ�װ��ֵ
    htim4.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;	//ʱ�ӷ�Ƶ����
		htim4.Init.RepetitionCounter=0x00;                //�����ظ�ϵ��
    HAL_TIM_Base_Init(&htim4);												//ʹ�ܶ�ʱ��4
	
	
	
	
		TIM_ClockConfigTypeDef sClockSourceConfig = {0};
		TIM_MasterConfigTypeDef sMasterConfig = {0};
		TIM_IC_InitTypeDef sConfigIC = {0};
		
		sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
		if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
		{
			Error_Handler();
		}
		if (HAL_TIM_IC_Init(&htim4) != HAL_OK)
		{
			Error_Handler();
		}
		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
		{
			Error_Handler();
		}
		sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
		sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
		sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
		sConfigIC.ICFilter = 0;
		if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
		{
			Error_Handler();
		}
		if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
		{
			Error_Handler();
		}
		

}
void TIM4_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim4);
}
/***TIM4***/




/*-----------------TIM5--------------*/
void TIM5_Init(u16 arr,u16 psc)
{  
    htim5.Instance=TIM5;                          		//ͨ�ö�ʱ��5
    htim5.Init.Prescaler=psc;                     		//��Ƶϵ��
    htim5.Init.CounterMode=TIM_COUNTERMODE_UP;    		//���ϼ�����
    htim5.Init.Period=arr;                        		//�Զ�װ��ֵ
    htim5.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;	//ʱ�ӷ�Ƶ����
		htim5.Init.RepetitionCounter=0x00;                //�����ظ�ϵ��
    HAL_TIM_Base_Init(&htim5);												//ʹ�ܶ�ʱ��5
	


}
void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim5);
}
/***TIM5***/






/*---TIM_Base---------------------*/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
  if(tim_baseHandle->Instance==TIM2)
  {
		TIM_ClockConfigTypeDef ClockSourceConfig;
    TIM_MasterConfigTypeDef MasterConfig;
		TIM_OC_InitTypeDef ConfigOC;
		GPIO_InitTypeDef GPIO_InitStruct;
		
	  /* Peripheral clock enable */
	  __HAL_RCC_TIM2_CLK_ENABLE();            //ʹ��TIM2ʱ��	
		
		ClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
		HAL_TIM_ConfigClockSource(tim_baseHandle, &ClockSourceConfig);
		
		MasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		MasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		HAL_TIMEx_MasterConfigSynchronization(tim_baseHandle, &MasterConfig);
		
		ConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		ConfigOC.OCMode = TIM_OCMODE_PWM1;
		ConfigOC.Pulse = 1000;
		ConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;		
		HAL_TIM_OC_ConfigChannel(tim_baseHandle, &ConfigOC, TIM_CHANNEL_1);
		HAL_TIM_OC_ConfigChannel(tim_baseHandle, &ConfigOC, TIM_CHANNEL_2);
		HAL_TIM_OC_ConfigChannel(tim_baseHandle, &ConfigOC, TIM_CHANNEL_3);
		HAL_TIM_OC_ConfigChannel(tim_baseHandle, &ConfigOC, TIM_CHANNEL_4);

    /**TIM2 GPIO Configuration    
    PA1     ------> TIM2_CH2
    PA0/WKUP     ------> TIM2_CH1
    PA2     ------> TIM2_CH3
    PA3     ------> TIM2_CH4 
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
	
		HAL_NVIC_SetPriority(TIM2_IRQn,1,0);    //������ռ���ȼ��������ȼ�
		HAL_NVIC_EnableIRQ(TIM2_IRQn);          //����TIM2�ж�   
		HAL_TIM_OC_Start(&htim2,TIM_CHANNEL_1);
		HAL_TIM_OC_Start(&htim2,TIM_CHANNEL_2);
		HAL_TIM_OC_Start(&htim2,TIM_CHANNEL_3);
		HAL_TIM_OC_Start(&htim2,TIM_CHANNEL_4);
		//HAL_TIM_OC_Start_IT(&htim2,TIM_CHANNEL_1); 	//��ʱ��2ͨ��1 OC_DELAY�ж�   

  }
	
	else if(tim_baseHandle->Instance==TIM3)
  {
    /* Peripheral clock enable */
	  __HAL_RCC_TIM3_CLK_ENABLE();            //ʹ��TIM3ʱ��
    
		/**TIM3 GPIO Configuration    
    PB4     ------> TIM3_CH1 
    */	
		
		HAL_NVIC_SetPriority(TIM3_IRQn,2,0);    //������ռ���ȼ��������ȼ�
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //����TIM3�ж�   
		HAL_TIM_Base_Start_IT(&htim3); 					//��ʱ��3�����жϣ�TIM_IT_UPDATE   

  }
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(tim_baseHandle->Instance==TIM4)
  {
    /* Peripheral clock enable */
	  __HAL_RCC_TIM4_CLK_ENABLE();            //ʹ��TIM4ʱ��		
  
    /**TIM4 GPIO Configuration    
    PD15     ------> TIM4_CH4
    PD14     ------> TIM4_CH3
    PD13     ------> TIM4_CH2
    PD12     ------> TIM4_CH1 
    */
		
		GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
		
		HAL_NVIC_SetPriority(TIM4_IRQn,5,0);    //������ռ���ȼ��������ȼ�
		HAL_NVIC_EnableIRQ(TIM4_IRQn);          //����TIM4�ж�   
		HAL_TIM_Base_Start_IT(&htim4); 					//��ʱ��4�����жϣ�TIM_IT_UPDATE   

  }
	

 if(tim_baseHandle->Instance==TIM5)
  {
		

    /* Peripheral clock enable */
	  __HAL_RCC_TIM5_CLK_ENABLE();						//ʹ��TIM5ʱ��
  
    /**TIM5 GPIO Configuration    
    PI0     ------> TIM5_CH4
    PH12     ------> TIM5_CH3
    PH11     ------> TIM5_CH2
    PH10     ------> TIM5_CH1 
    */
		

		
		HAL_NVIC_SetPriority(TIM5_IRQn,0,2);    //������ռ���ȼ��������ȼ�
		HAL_NVIC_EnableIRQ(TIM5_IRQn);          //����TIM5�ж�   
		HAL_TIM_Base_Start_IT(&htim5); 					//��ʱ��5�����жϣ�TIM_IT_UPDATE  

  }
if(tim_baseHandle->Instance==TIM8) 
	{
    /**TIM8 GPIO Configuration    
    PI7     ------> TIM8_CH3
    PI6     ------> TIM8_CH2
    PI5     ------> TIM8_CH1
    PI2     ------> TIM8_CH4 
    */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{
	if(tim_baseHandle->Instance==TIM2)
	{
		/* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();
	}
  else if(tim_baseHandle->Instance==TIM3)
  {
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();
  }
  else if(tim_baseHandle->Instance==TIM4)
  {
    /* Peripheral clock disable */
    __HAL_RCC_TIM4_CLK_DISABLE();
  }
  else if(tim_baseHandle->Instance==TIM5)
  {
    /* Peripheral clock disable */
    __HAL_RCC_TIM5_CLK_DISABLE();
  }
	else if(tim_baseHandle->Instance==TIM6)
  {
		 /* Peripheral clock disable */
    __HAL_RCC_TIM6_CLK_DISABLE();
	}
} 





