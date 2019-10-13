#include "can.h"
#include "gpio.h"

CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;

/* CAN1 init function */
void MX_CAN1_Init(void){
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 3;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SJW = CAN_SJW_1TQ;
  hcan1.Init.BS1 = CAN_BS1_9TQ;
  hcan1.Init.BS2 = CAN_BS2_4TQ;
  hcan1.Init.TTCM = DISABLE;
  hcan1.Init.ABOM = DISABLE;
  hcan1.Init.AWUM = DISABLE;
  hcan1.Init.NART = DISABLE;
  hcan1.Init.RFLM = DISABLE;
  hcan1.Init.TXFP = ENABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK){
    Error_Handler();
  }
}

/* CAN2 init function */
void MX_CAN2_Init(void){
  hcan2.Instance = CAN2;
  hcan2.Init.Prescaler = 3;
  hcan2.Init.Mode = CAN_MODE_NORMAL;
  hcan2.Init.SJW = CAN_SJW_1TQ;
  hcan2.Init.BS1 = CAN_BS1_9TQ;
  hcan2.Init.BS2 = CAN_BS2_4TQ;
  hcan2.Init.TTCM = DISABLE;
  hcan2.Init.ABOM = DISABLE;
  hcan2.Init.AWUM = DISABLE;
  hcan2.Init.NART = DISABLE;
  hcan2.Init.RFLM = DISABLE;
  hcan2.Init.TXFP = ENABLE;
  if (HAL_CAN_Init(&hcan2) != HAL_OK)
  {
    Error_Handler();
  }
}

static uint32_t HAL_RCC_CAN1_CLK_ENABLED=0;

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* Peripheral clock enable */
    HAL_RCC_CAN1_CLK_ENABLED++;
    if(HAL_RCC_CAN1_CLK_ENABLED==1){
      __HAL_RCC_CAN1_CLK_ENABLE();
    }
  
    /**CAN1 GPIO Configuration    
    PD0     ------> CAN1_RX
    PD1     ------> CAN1_TX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(CAN1_TX_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
  else if(canHandle->Instance==CAN2)
  {
  /* USER CODE BEGIN CAN2_MspInit 0 */

  /* USER CODE END CAN2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_CAN2_CLK_ENABLE();
    HAL_RCC_CAN1_CLK_ENABLED++;
    if(HAL_RCC_CAN1_CLK_ENABLED==1){
      __HAL_RCC_CAN1_CLK_ENABLE();
    }
  
    /**CAN2 GPIO Configuration    
    PB12     ------> CAN2_RX
    PB13     ------> CAN2_TX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(CAN2_TX_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(CAN2_TX_IRQn);
    HAL_NVIC_SetPriority(CAN2_RX0_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
  /* USER CODE BEGIN CAN2_MspInit 1 */

  /* USER CODE END CAN2_MspInit 1 */
  }
}


void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    HAL_RCC_CAN1_CLK_ENABLED--;
    if(HAL_RCC_CAN1_CLK_ENABLED==0){
      __HAL_RCC_CAN1_CLK_DISABLE();
    }
  
    /**CAN1 GPIO Configuration    
    PD0     ------> CAN1_RX
    PD1     ------> CAN1_TX 
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0|GPIO_PIN_1);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);

    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);

  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
  else if(canHandle->Instance==CAN2)
  {
  /* USER CODE BEGIN CAN2_MspDeInit 0 */

  /* USER CODE END CAN2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN2_CLK_DISABLE();
    HAL_RCC_CAN1_CLK_ENABLED--;
    if(HAL_RCC_CAN1_CLK_ENABLED==0){
      __HAL_RCC_CAN1_CLK_DISABLE();
    }
  
    /**CAN2 GPIO Configuration    
    PB12     ------> CAN2_RX
    PB13     ------> CAN2_TX 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12|GPIO_PIN_13);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(CAN2_TX_IRQn);

    HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);

  /* USER CODE BEGIN CAN2_MspDeInit 1 */

  /* USER CODE END CAN2_MspDeInit 1 */
  }
} 


void CAN_Filter_Init_Recv_All(void){
	//can1 &can2 use same filter config
	CAN_FilterConfTypeDef		CAN_FilterConfigStructure;
	static CanTxMsgTypeDef		Tx1Message;
	static CanRxMsgTypeDef 		Rx1Message;
	static CanTxMsgTypeDef		Tx2Message;
	static CanRxMsgTypeDef 		Rx2Message;

	CAN_FilterConfigStructure.FilterNumber = 0;
	CAN_FilterConfigStructure.FilterMode = CAN_FILTERMODE_IDMASK;
	CAN_FilterConfigStructure.FilterScale = CAN_FILTERSCALE_32BIT;
	CAN_FilterConfigStructure.FilterIdHigh = 0x0000;
	CAN_FilterConfigStructure.FilterIdLow = 0x0000;
	CAN_FilterConfigStructure.FilterMaskIdHigh = 0x0000;
	CAN_FilterConfigStructure.FilterMaskIdLow = 0x0000;
	CAN_FilterConfigStructure.FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterConfigStructure.BankNumber = 14;//can1(0-13)和can2(14-27)分别得到一半的filter
	CAN_FilterConfigStructure.FilterActivation = ENABLE;

	if(HAL_CAN_ConfigFilter(&hcan1, &CAN_FilterConfigStructure) != HAL_OK)
	{
		//错误处理
	}

	//filter config for can2 
	//can1(0-13)和can2(14-27)分别得到一半的filter
	CAN_FilterConfigStructure.FilterNumber = 14;
	if(HAL_CAN_ConfigFilter(&hcan2, &CAN_FilterConfigStructure) != HAL_OK)
	{
	//err_deadloop();
	}

	hcan1.pTxMsg = &Tx1Message;
	hcan1.pRxMsg = &Rx1Message;
	
	hcan2.pTxMsg = &Tx2Message;
	hcan2.pRxMsg = &Rx2Message;
}
