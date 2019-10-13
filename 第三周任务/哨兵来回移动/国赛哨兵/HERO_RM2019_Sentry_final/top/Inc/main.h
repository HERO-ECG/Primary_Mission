#ifndef __MAIN_H
#define __MAIN_H

#define Debug_2MCU
 //#define Debug_tripod
#define Chassis
#define ChassisPowerControl
#ifndef Chassis
	#define Pitch6623
	#define Yaw6623	 //#define YawTop
	#define AmmunitionToggle
	#define Fricition
#endif

#define IST_INT_Pin 				GPIO_PIN_3
#define IST_INT_GPIO_Port 	GPIOE
#define IST_RST_Pin 				GPIO_PIN_2
#define IST_RST_GPIO_Port 	GPIOE
#define LASER_Pin						GPIO_PIN_13
#define LASER_GPIO_Port			GPIOG
#define KEY_Pin 						GPIO_PIN_10
#define KEY_GPIO_Port     	GPIOD
#define LED_GREEN_Pin       GPIO_PIN_14
#define LED_GREEN_GPIO_Port GPIOF
#define LED_RED_Pin         GPIO_PIN_11
#define LED_RED_GPIO_Port   GPIOE
#define Power_Pin           GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5
#define Power_Port          GPIOH

#endif /* __MAIN_H */

