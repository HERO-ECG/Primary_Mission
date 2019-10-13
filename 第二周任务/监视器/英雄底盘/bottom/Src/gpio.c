#include "gpio.h"

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/

void MX_GPIO_Init(void){
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();

  /*Configure GPIO pins : PEPin PEPin */
  GPIO_InitStruct.Pin = IST_INT_Pin|IST_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = LASER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LASER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PF6 PFPin */
  GPIO_InitStruct.Pin = GPIO_PIN_6|LED_GREEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	
//	GPIO_InitStruct.Pin = GPIO_PIN_2;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_PULLUP;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = KEY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = LED_RED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_RED_GPIO_Port, &GPIO_InitStruct);
	
	/*Configure GPIO pin : PowerPin */
	GPIO_InitStruct.Pin = Power_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Power_Port, &GPIO_InitStruct);
	

		/*大拨弹限位开关检测IO口*/
		/*Configure GPIO pin : S-PA0 */
		GPIO_InitStruct.Pin = GPIO_PIN_0;	
		GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//		/*小拨弹限位开关检测IO口*/
//		/*Configure GPIO pin : A-PI0 */
//		GPIO_InitStruct.Pin = GPIO_PIN_0;	
//		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
//		GPIO_InitStruct.Pull = GPIO_PULLUP;
//		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//		HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);


//	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 2);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
//	HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 2);
//	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
//	HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 2);
//	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
}
