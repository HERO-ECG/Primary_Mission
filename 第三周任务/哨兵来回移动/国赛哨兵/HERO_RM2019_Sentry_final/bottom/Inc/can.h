#ifndef __can_H
#define __can_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "mytype.h"
	 
#define CAN1_RX0_INT_ENABLE	1		////CAN1����RX0�ж�ʹ��.0,��ʹ��;1,ʹ��.

extern CAN_HandleTypeDef hcan1;

extern void Error_Handler(void);

void MX_CAN1_Init(void);
void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle);
void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle);
extern void CAN_Filter_Init_Recv_ALL(CAN_HandleTypeDef* _hcan);
extern void CAN_Filter_Init_Recv_SP(CAN_HandleTypeDef* _hcan, uint32_t pass_id);
	 
#ifdef __cplusplus
}
#endif
#endif /*__ can_H */


