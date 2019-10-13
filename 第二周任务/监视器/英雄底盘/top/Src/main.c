#include "main.h"
#include "mytype.h"
#include "stm32f4xx_hal.h"
#include "dma.h"
#include "can.h"
#include "pwm.h"
#include "usb_device.h"
#include "robodata.h"
#include "chassis.h"
#include "tripod.h"
#include "strike.h"
#include "ammo.h"
#include "t_monitor.h"
#include "t_moto.h"
#include "control.h"



void SystemClock_Config(void);
void Error_Handler(void);
void all_bsp_init(void);
void all_pid_init(void);

extern float friction_set_speed_debug;
extern float ctrl_spd_test;
extern float ammo_spd_test;


void vcan_sendware(u8 *wareaddr, u32 waresize); 
float a=2000;
int main(void){
	all_bsp_init();//底层初始化
	all_pid_init();
	time_piece_start = 1;
	while (1)
	{	
		//空闲时间片，对响应要求低的放其中
		if((time_piece&time_piece_0100hz) == time_piece_0100hz)
		{			
			time_piece &= ~time_piece_0100hz;
			monitor_remote_process(&monitor_remote);//遥控器监控器
			monitor_tx2.monitor_process(&monitor_tx2);//tx2监控器
			monitor_can_power_process(&monitor_can_power);		
		}
		else if((time_piece&time_piece_1000hz) == time_piece_1000hz)
		{
			time_piece &= ~time_piece_1000hz;
			RoboData.GetRemoteControlData(&RoboData);//遥控器数据处理			
		}
	}
}	

void all_bsp_init(void){
	HAL_Init();//4位抢占优先级、0位响应优先级
	SystemClock_Config();

	MX_GPIO_Init();
	HAL_GPIO_WritePin(LASER_GPIO_Port,LASER_Pin,GPIO_PIN_SET);		//亮
	HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_RESET);		//亮
 	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin,GPIO_PIN_RESET);//亮
	HAL_GPIO_WritePin(Power_Port,Power_Pin,GPIO_PIN_SET);
	
	/*------------初始化机构体&设置指针指向&机器人控制相关数据----------------*/
	InitRoboData(&RoboData);
	MX_SPI5_Init();
	MPU6500_Init();
	Chassis_Init();
	Tripod_Init();
	Strike_Init();
	
	MX_CAN1_Init();
	MX_CAN2_Init();//3508电机can通信初始化，中断抢占优先级为1
	CAN_Filter_Init_Recv_All();//3508电机can通信过滤器初始化
	HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);//3508电机can通信接收中断配置
	HAL_CAN_Receive_IT(&hcan2, CAN_FIFO0);
	
	MX_USART1_UART_Init();//遥控器   波特率：100000
	MYDMA_Config(DMA2_Stream2,DMA_CHANNEL_4,&hdma_usart1_rx);//初始化USART1到DMA，USART1到DMA接收中断抢占优先级为0,中断服务函数在dma中
	while(HAL_UART_Receive_DMA(&huart1,uart1_rx_buff,len_uart1_rx_buff)!=HAL_OK);//DMA while(1)之前启动一下DMA接收
	
//	MX_USART2_UART_Init();//
//	MYDMA_Config(DMA1_Stream5,DMA_CHANNEL_4,&hdma_usart2_rx);//初始化DMA
//	while(HAL_UART_Receive_DMA(&huart2,uart2_rx_buff,22u)!=HAL_OK);//DMA方式

	MX_USART3_UART_Init();//
	MYDMA_Config(DMA1_Stream1,DMA_CHANNEL_4,&hdma_usart3_rx);	
	while(HAL_UART_Receive_DMA(&huart3,uart3_rx_buff,len_uart3_rx_buff)!=HAL_OK);

//	MX_USART6_UART_Init();//
//	MYDMA_Config(DMA2_Stream1,DMA_CHANNEL_5,&hdma_usart6_rx);
//	while(HAL_UART_Receive_DMA(&huart6,uart6_rx_buff,len_uart6_rx_buff)!=HAL_OK);

	MX_USART7_UART_Init();//裁判系统
	MYDMA_Config(DMA1_Stream3,DMA_CHANNEL_5,&hdma_usart7_rx);
	{while(HAL_UART_Receive_DMA(&huart7,uart7_rx_buff,len_uart7_rx_buff)!=HAL_OK);}

  MX_TIM1_Init();
 	
	TIM3_Init(1000-1,840-1);//100hz		
	
	TIM5_Init(100-1,840-1);//1000hz                               
				
	HAL_Delay(1500);
	
	
	Strike.count_42mm.out_firecounter=3;
	
	/*功能结构体初始化*/
}

void all_pid_init(void){
	/*底盘电机初始化*/
	PidSet_all(&Chassis_MOTO[0].pid_speed, POSITION_PID, 16000, 5000, -5000, 6, 0.2, 0, 0, 0, 5000, 2000, 0, 500);
	PidSet_all(&Chassis_MOTO[1].pid_speed, POSITION_PID, 16000, 5000, -5000, 6, 0.2, 0, 0, 0, 5000, 2000, 0, 500);
	PidSet_all(&Chassis_MOTO[2].pid_speed, POSITION_PID, 16000, 5000, -5000, 6, 0.2, 0, 0, 0, 5000, 2000, 0, 500);
	PidSet_all(&Chassis_MOTO[3].pid_speed, POSITION_PID, 16000, 5000, -5000, 6, 0.2, 0, 0, 0, 5000, 2000, 0, 500);

	
	
	/*拨弹电机初始化*/
	PidSet_all(&Ammunition_MOTO.pid_angle, POSITION_PID, 2, 0, -0, 0.2, 0, 0, 0, 0, 0, 0, 0, 0);	
	PidSet_all(&Ammunition_MOTO.pid_speed, POSITION_PID, 12000, 2000, -2000, 1, 0.01, 0, 0, 0, 10000, 3000, 0, 0);	
	PidSet_all(&LittleAmmo_MOTO.pid_speed, POSITION_PID, 8000, 4000, -4000, 10, 0.0001, 0,  0, 0, 0, 0, 0, 0);
	PidSet_all(&Ctrl_2006_MOTO.pid_angle, POSITION_PID, 4000, 500, -500, 7, 0, 0, 0, 0, 0, 0, 0, 0);
	PidSet_all(&Ctrl_2006_MOTO.pid_speed, POSITION_PID, 6000, 500, -500, 9, 0.1, 2, 0, 0, 0, 0, 0, 0);
	
	PidSet_all(&Friction_Wheel_MOTO[0].pid_speed, POSITION_PID, 16000, 500, -500, 20, 0.1, 0, 0, 0, 0, 0, 0, 0);
	PidSet_all(&Friction_Wheel_MOTO[1].pid_speed, POSITION_PID, 16000, 500, -500, 20, 0.1, 0, 0, 0, 0, 0, 0, 0);
	
	
	
	HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_SET);//灭
}//PID参数初始化

/** System Clock Configuration*/
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	/**Configure the main internal regulator output voltage */
	__HAL_RCC_PWR_CLK_ENABLE();

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/**Initializes the CPU, AHB and APB busses clocks */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 12;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
	Error_Handler();
	}

	/**Initializes the CPU, AHB and APB busses clocks */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
	Error_Handler();
	}

	/**Configure the Systick interrupt time */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/**Configure the Systick */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
}

void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}

