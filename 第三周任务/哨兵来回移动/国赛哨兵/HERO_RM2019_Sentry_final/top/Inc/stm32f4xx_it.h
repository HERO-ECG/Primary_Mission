
#ifndef __STM32F4xx_IT_H
#define __STM32F4xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 


void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void CAN1_TX_IRQHandler(void);
void CAN1_RX0_IRQHandler(void);
void OTG_FS_IRQHandler(void);
void USART3_IRQHandler(void);
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI15_10_IRQHandler(void);
void EXTI9_5_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_IT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
