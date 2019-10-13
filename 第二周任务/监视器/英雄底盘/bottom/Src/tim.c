#include "tim.h"
#include "mytype.h"


TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim8;

/*-----------------TIM1--------------*/
/* TIM1 init function */
void MX_TIM1_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 167;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 20000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
	HAL_TIM_Base_Init(&htim1);
  //HAL_TIM_MspPostInit(&htim1);
	
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* tim_pwmHandle)
{

  if(tim_pwmHandle->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspInit 0 */

  /* USER CODE END TIM1_MspInit 0 */
    /* TIM1 clock enable */
    __HAL_RCC_TIM1_CLK_ENABLE();
  /* USER CODE BEGIN TIM1_MspInit 1 */

  /* USER CODE END TIM1_MspInit 1 */
  }
}
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(timHandle->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspPostInit 0 */

  /* USER CODE END TIM1_MspPostInit 0 */
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**TIM1 GPIO Configuration    
    PA9     ------> TIM1_CH2
    PA8     ------> TIM1_CH1
    PE13     ------> TIM1_CH3
    PE14     ------> TIM1_CH4 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM1_MspPostInit 1 */

  /* USER CODE END TIM1_MspPostInit 1 */
  }

}

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* tim_pwmHandle)
{

  if(tim_pwmHandle->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspDeInit 0 */

  /* USER CODE END TIM1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM1_CLK_DISABLE();
  /* USER CODE BEGIN TIM1_MspDeInit 1 */

  /* USER CODE END TIM1_MspDeInit 1 */
  }
} 


/*-----------------TIM2--------------*/
void TIM2_Init(u16 arr,u16 psc)
{  
    htim2.Instance=TIM2;                          		//通用定时器2
    htim2.Init.Prescaler=psc;                     		//分频系数
    htim2.Init.CounterMode=TIM_COUNTERMODE_UP;    		//向上计数器
    htim2.Init.Period=arr;                        		//自动装载值
    htim2.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;	//时钟分频因子
	  htim2.Init.RepetitionCounter=0x00;                //计数重复系数
    HAL_TIM_Base_Init(&htim2);												//使能定时器2

}
void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim2);
}
/***TIM2***/





/*-----------------TIM3--------------*/
void TIM3_Init(u16 arr,u16 psc)
{  
    htim3.Instance=TIM3;                          		//通用定时器3
    htim3.Init.Prescaler=psc;                     		//分频系数
    htim3.Init.CounterMode=TIM_COUNTERMODE_UP;    		//向上计数器
    htim3.Init.Period=arr;                        		//自动装载值
    htim3.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;	//时钟分频因子
		htim3.Init.RepetitionCounter=0x00;                //计数重复系数
    HAL_TIM_Base_Init(&htim3);												//使能定时器3

}
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim3);
}
/***TIM3***/



/*-----------------TIM4--------------*/
void TIM4_Init(u16 arr,u16 psc)
{  
    htim4.Instance=TIM4;                          		//通用定时器4
    htim4.Init.Prescaler=psc;                     		//分频系数
    htim4.Init.CounterMode=TIM_COUNTERMODE_UP;    		//向上计数器
    htim4.Init.Period=arr;                        		//自动装载值
    htim4.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;	//时钟分频因子
		htim4.Init.RepetitionCounter=0x00;                //计数重复系数
    HAL_TIM_Base_Init(&htim4);												//使能定时器4

}
void TIM4_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim4);
}
/***TIM4***/




/*-----------------TIM5--------------*/
void TIM5_Init(u16 arr,u16 psc)
{  
    htim5.Instance=TIM5;                          		//通用定时器5
    htim5.Init.Prescaler=psc;                     		//分频系数
    htim5.Init.CounterMode=TIM_COUNTERMODE_UP;    		//向上计数器
    htim5.Init.Period=arr;                        		//自动装载值
    htim5.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;	//时钟分频因子
		htim5.Init.RepetitionCounter=0x00;                //计数重复系数
    HAL_TIM_Base_Init(&htim5);												//使能定时器5

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
	  __HAL_RCC_TIM2_CLK_ENABLE();            //使能TIM2时钟	
		
		ClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
		HAL_TIM_ConfigClockSource(tim_baseHandle, &ClockSourceConfig);
		
		MasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		MasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		HAL_TIMEx_MasterConfigSynchronization(tim_baseHandle, &MasterConfig);
		
		ConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		ConfigOC.OCMode = TIM_OCMODE_PWM1;
		ConfigOC.Pulse = 1000;
		ConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;		
		HAL_TIM_OC_ConfigChannel(tim_baseHandle, &ConfigOC, TIM_CHANNEL_3);
		HAL_TIM_OC_ConfigChannel(tim_baseHandle, &ConfigOC, TIM_CHANNEL_4);

    /**TIM2 GPIO Configuration    
    PA1     ------> TIM2_CH2
    PA0/WKUP     ------> TIM2_CH1
    PA2     ------> TIM2_CH3
    PA3     ------> TIM2_CH4 
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		HAL_NVIC_SetPriority(TIM2_IRQn,3,0);    //设置抢占优先级与子优先级
		HAL_NVIC_EnableIRQ(TIM2_IRQn);          //开启TIM2中断   
		HAL_TIM_OC_Start(&htim2,TIM_CHANNEL_3);
		HAL_TIM_OC_Start(&htim2,TIM_CHANNEL_4);
		//HAL_TIM_OC_Start_IT(&htim2,TIM_CHANNEL_1); 	//定时器2通道1 OC_DELAY中断   

  }
	
	else if(tim_baseHandle->Instance==TIM3)
  {
    /* Peripheral clock enable */
	  __HAL_RCC_TIM3_CLK_ENABLE();            //使能TIM3时钟
    
		/**TIM3 GPIO Configuration    
    PB4     ------> TIM3_CH1 
    */	
		
		HAL_NVIC_SetPriority(TIM3_IRQn,2,0);    //设置抢占优先级与子优先级
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //开启TIM3中断   
		HAL_TIM_Base_Start_IT(&htim3); 					//定时器3更新中断：TIM_IT_UPDATE   

  }
  else if(tim_baseHandle->Instance==TIM4)
  {
    /* Peripheral clock enable */
	  __HAL_RCC_TIM4_CLK_ENABLE();            //使能TIM4时钟		
  
    /**TIM4 GPIO Configuration    
    PD15     ------> TIM4_CH4
    PD14     ------> TIM4_CH3
    PD13     ------> TIM4_CH2
    PD12     ------> TIM4_CH1 
    */
		
		HAL_NVIC_SetPriority(TIM4_IRQn,5,0);    //设置抢占优先级与子优先级
		HAL_NVIC_EnableIRQ(TIM4_IRQn);          //开启TIM4中断   
		HAL_TIM_Base_Start_IT(&htim4); 					//定时器4更新中断：TIM_IT_UPDATE   

  }

  else if(tim_baseHandle->Instance==TIM5)
  {
    /* Peripheral clock enable */
	  __HAL_RCC_TIM5_CLK_ENABLE();						//使能TIM5时钟
  
    /**TIM5 GPIO Configuration    
    PI0     ------> TIM5_CH4
    PH12     ------> TIM5_CH3
    PH11     ------> TIM5_CH2
    PH10     ------> TIM5_CH1 
    */
		
		HAL_NVIC_SetPriority(TIM5_IRQn,1,0);    //设置抢占优先级与子优先级
		HAL_NVIC_EnableIRQ(TIM5_IRQn);          //开启TIM5中断   
		HAL_TIM_Base_Start_IT(&htim5); 					//定时器5更新中断：TIM_IT_UPDATE  

  }
	else if(tim_baseHandle->Instance==TIM8) 
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





