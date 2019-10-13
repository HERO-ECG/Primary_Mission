#ifndef __MAIN_H
#define __MAIN_H


//#define SINGLE

#define RED

//#define INFANTRY3
//#define INFANTRY4
#define INFANTRY5

#ifdef INFANTRY3
	
	#ifdef RED
	
		#ifdef SINGLE
			#define ROBOT_TX_ID	0x0004//红方
			#define ROBOT_RX_ID	0x0104
		#else
			#define ROBOT_TX_ID	0x0003 //红方
			#define ROBOT_RX_ID	0x0103
		#endif
	
	#else
	
	
		#ifdef SINGLE
			#define ROBOT_TX_ID	0x000E
			#define ROBOT_RX_ID	0x0114 //蓝方
		#else
			#define ROBOT_TX_ID	0x000D
			#define ROBOT_RX_ID	0x0113 //蓝方
		#endif
		
	

	#endif

#define CHASSIS_FILLOW_OFFSET 60.0f
#define PITCH_OFFSET	273.5f
#endif


#ifdef INFANTRY4

	#ifdef RED
	#define ROBOT_TX_ID	0x0004 //红方
	#define ROBOT_RX_ID	0x0104
	#else	
	#define ROBOT_TX_ID	0x00E
	#define ROBOT_RX_ID	0x0114 //蓝方
	#endif
#define PITCH_OFFSET	273.5f
#define CHASSIS_FILLOW_OFFSET 0.7f
#endif


#ifdef INFANTRY5

	#ifdef RED
	#define ROBOT_TX_ID	0x0005 //红方
	#define ROBOT_RX_ID	0x0105
	#else
	#define ROBOT_TX_ID	0x00F
	#define ROBOT_RX_ID	0x0115 //蓝方
	#endif
#define PITCH_OFFSET	273.5f	
#define CHASSIS_FILLOW_OFFSET 60.7f
#endif


#define IST_INT_Pin 				GPIO_PIN_3
#define IST_INT_GPIO_Port 	GPIOE
#define IST_RST_Pin 				GPIO_PIN_2
#define IST_RST_GPIO_Port 	GPIOE
#define LASER_Pin						GPIO_PIN_13
#define LASER_GPIO_Port			GPIOG
#define KEY_Pin 						GPIO_PIN_2
#define KEY_GPIO_Port     	GPIOB
#define LED_GREEN_Pin       GPIO_PIN_14
#define LED_GREEN_GPIO_Port GPIOF
#define LED_RED_Pin         GPIO_PIN_11
#define LED_RED_GPIO_Port   GPIOE
#define Power_Pin           GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5
#define Power_Port          GPIOH
#define LED_ON							GPIO_PIN_RESET
#define LED_OFF							GPIO_PIN_SET
#define BEEP_ON							1
#define BEEP_OFF						0

#endif /* __MAIN_H */

