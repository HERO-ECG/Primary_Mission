#include "main.h"
#include "gpio.h"
#include "strike.h"

uint8_t gpio_exti_2=0,gpio_exti_1=0,gpio_exti_0=0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == GPIO_PIN_0){
		//if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==GPIO_PIN_RESET)//�͵�ƽ ����
		//	gpio_exti_0=1;
	}
	else if(GPIO_Pin == GPIO_PIN_1)	{
		gpio_exti_1=1;
	}
	else if(GPIO_Pin == GPIO_PIN_2)	
	{
		
//		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_2)==GPIO_PIN_RESET)//�͵�ƽ ����
//		{
//			
//			Strike_IntFun_in();	

//			
//		}
//		else//�ߵ�ƽ �뿪
//		{
//			


//			Strike_IntFun_out();	
//		}
	}
}
