#include "can.h"
#include "gpio.h"

CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;

/* CAN1 init function */
void MX_CAN1_Init(void)
{
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
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
}


void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(canHandle->Instance==CAN1)
  {
    /* Peripheral clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();
 
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
    HAL_NVIC_SetPriority(CAN1_TX_IRQn, 2, 1);
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();
  
    /**CAN1 GPIO Configuration    
    PD0     ------> CAN1_RX
    PD1     ------> CAN1_TX 
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0|GPIO_PIN_1);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);

    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);

  }
} 


static CanTxMsgTypeDef		Tx1Message;
static CanRxMsgTypeDef 		Rx1Message;
void CAN_Filter_Init_Recv_ALL(CAN_HandleTypeDef* _hcan)
{
	//can1 &can2 use same filter config
	CAN_FilterConfTypeDef		CAN_FilterConfigStructure;

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

	HAL_CAN_ConfigFilter(_hcan, &CAN_FilterConfigStructure);

	if(_hcan == &hcan1){
		_hcan->pTxMsg = &Tx1Message;
		_hcan->pRxMsg = &Rx1Message;
	}
}

void CAN_Filter_Init_Recv_SP(CAN_HandleTypeDef* _hcan, uint32_t pass_id)
{
	//can1 &can2 use same filter config
	CAN_FilterConfTypeDef		CAN_FilterConfigStructure;

	if(_hcan == &hcan1){
		_hcan->pTxMsg = &Tx1Message;
		_hcan->pRxMsg = &Rx1Message;
	}
	CAN_FilterConfigStructure.FilterNumber = 0;
	CAN_FilterConfigStructure.FilterMode = CAN_FILTERMODE_IDMASK;
	CAN_FilterConfigStructure.FilterScale = CAN_FILTERSCALE_32BIT;
	CAN_FilterConfigStructure.FilterIdHigh = ((pass_id<<3|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16;
	CAN_FilterConfigStructure.FilterIdLow = ((pass_id<<3|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF);
	CAN_FilterConfigStructure.FilterMaskIdHigh = 0xFFFF;
	CAN_FilterConfigStructure.FilterMaskIdLow = 0xFFFF;
	CAN_FilterConfigStructure.FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterConfigStructure.BankNumber = 14;//can1(0-13)和can2(14-27)分别得到一半的filter
	CAN_FilterConfigStructure.FilterActivation = ENABLE;

	HAL_CAN_ConfigFilter(_hcan,&CAN_FilterConfigStructure);
	
}

