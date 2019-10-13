#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "main.h"

	 

extern uint8_t gpio_exti_2,gpio_exti_1,gpio_exti_0;
	 
void MX_GPIO_Init(void);

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

