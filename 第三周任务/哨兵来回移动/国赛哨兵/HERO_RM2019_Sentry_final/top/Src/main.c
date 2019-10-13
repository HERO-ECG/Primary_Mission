#include "main.h"
#include "stm32f4xx_hal.h"
#include "mytype.h"
#include "dma.h"
#include "usb_device.h"
#include "control.h"
#include "t_moto.h"
#include "robodata.h"
#include "t_monitor.h"
#include "auto_move.h"
#include "pitch.h"
#include "strike.h"


void SystemClock_Config(void);
void Error_Handler(void);
void all_bsp_init(void);
void all_pid_init(void);

float var[8];
  
uint8_t TX2_Data_Send2[3]={1,2,3};

int main(void)
{
	all_bsp_init();
	all_pid_init();
//	 	HAL_TIM_IC_Start_IT (&htim4,TIM_CHANNEL_1);输入捕获中断开启
//	usart_monitor_begin_flag=0;

	while (1)
	{	
		//部分时间片
		if((time_piece&time_piece_0100hz) == time_piece_0100hz){
			time_piece &= ~time_piece_0100hz;
			monitor_remote.monitor_process(&monitor_remote);//遥控器监视器
	//		TX2_monitor();//TX2监视器
//			usart_monitor_begin_flag=1;
//			if(usart_monitor_begin_flag==1)
//			{UsartMonitor();}
//			TX2_DataProcess_Send(TX2_Data_Send);

//			HAL_UART_Transmit(&huart3,TX2_Data_Send2,sizeof(TX2_Data_Send2),20);
//				tx2_send();
		}
		else if((time_piece&time_piece_1000hz) == time_piece_1000hz){
			time_piece &= ~time_piece_1000hz;
		}
		else
			{		

			}
				
	}
}	

void all_bsp_init(void)
{
	HAL_Init();//4位抢占优先级、0位响应优先级
	SystemClock_Config();

	MX_GPIO_Init();
	HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_RESET);//亮
 	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin,GPIO_PIN_RESET);//亮
	HAL_GPIO_WritePin(Power_Port,Power_Pin,GPIO_PIN_SET);  //power
	
	/*------------初始化机构体&设置指针指向&机器人控制相关数据----------------*/
	InitRoboData(&RoboData);
	Init_OverallMode(&overall_mode);
	Pitch_Init();
	Pitch_Init2();
	Yaw_Change_Init();
//	MX_SPI5_Init();
//	MPU6500_Init();
	
	MX_CAN1_Init();//3508电机can通信初始化，中断抢占优先级为1
	CAN_Filter_Init_Recv_ALL(&hcan1);//3508电机can通信过滤器初始化
	HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);//3508电机can通信接收中断配置
  //CAN的接收中断函数在m_moto.c中

	MX_USART1_UART_Init();//遥控器   波特率：100000
	MYDMA_Config(DMA2_Stream2,DMA_CHANNEL_4,&hdma_usart1_rx);//初始化USART1到DMA，USART1到DMA接收中断抢占优先级为0,中断服务函数在dma中
	while(HAL_UART_Receive_DMA(&huart1,uart1_rx_buff,36u)!=HAL_OK);//DMA while(1)之前启动一下DMA接收
//	
//	MX_USART2_UART_Init();//接收姿态角   波特率：115200
//	MYDMA_Config(DMA1_Stream5,DMA_CHANNEL_4,&hdma_usart2_rx);//初始化DMA
//	while(HAL_UART_Receive_DMA(&huart2,uart2_rx_buff,22u)!=HAL_OK);//DMA方式
	
	MX_USART3_UART_Init();//tx2数据   波特率：115200
	MYDMA_Config(DMA1_Stream1,DMA_CHANNEL_4,&hdma_usart3_rx);	
	HAL_UART_Receive_DMA(&huart3,uart3_rx_buff,11u); //缓启动串口接受 防止串口出现问题
		//		MYDMA_Config(DMA1_Stream3,DMA_CHANNEL_4,&hdma_usart3_tx);



	MX_USART6_UART_Init();//陀螺仪   波特率115200
	MYDMA_Config(DMA2_Stream1,DMA_CHANNEL_5,&hdma_usart6_rx);
	while(HAL_UART_Receive_DMA(&huart6,uart6_rx_buff,28)!=HAL_OK);
	//MYDMA_Config(DMA2_Stream7,DMA_CHANNEL_5,&hdma_usart6_tx);	
	
	
	MX_USART8_UART_Init();
	MYDMA_Config(DMA1_Stream6,DMA_CHANNEL_5,&hdma_usart8_rx);	
	while(HAL_UART_Receive_DMA(&huart8,uart8_rx_buff,len_uart8_rx_buff)!=HAL_OK);
	
	MX_USART7_UART_Init();

	TIM2_Init(20000-1,84-1);//pwm
	
	TIM3_Init(1000-1,840-1);//100hz		
//	TIM4_Init(1000-1,840-1);//100hz	
	TIM5_Init(100-1,840-1);//1000hz                               
				
	HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_RESET);//灭
	
	strike_init();
}

void all_pid_init(void)//PID参数初始化
{
  power_pid[0].PidSet_all(&power_pid[0], POSITION_PID, 1, 0, -1, 1, 0.01, 0.0001, 0, 20, 10, 5, 0, 0);
  power_pid[1].PidSet_all(&power_pid[1], POSITION_PID, 1, 0, -1, 1, 0.01, 0.0001, 0, 20, 10, 5, 0, 0);

	Chassis_MOTO[0].pid_speed.PidSet_all(&Chassis_MOTO[0].pid_angle, POSITION_PID, 5000, 500, -500, 130, 5, 0, 0, 0, 0, 0, 0, 500);
	Chassis_MOTO[0].pid_speed.PidSet_all(&Chassis_MOTO[0].pid_speed, POSITION_PID, 7500, 500, -500, 20, 0.1, 10, 0, 0, 0, 0, 0, 500);	
	
	Chassis_MOTO[1].pid_speed.PidSet_all(&Chassis_MOTO[1].pid_angle, POSITION_PID, 2500, 500, -500, 3, 0.1, 0, 0, 0, 0, 0, 0, 500);
	Chassis_MOTO[1].pid_speed.PidSet_all(&Chassis_MOTO[1].pid_speed, POSITION_PID, 2500, 500, -500, 3, 0.1, 0, 0, 0, 0, 0, 0, 500);
	
	
	Chassis_MOTO[2].pid_speed.PidSet_all(&Chassis_MOTO[2].pid_speed, POSITION_PID, 2500, 500, -500, 5, 0.1, 0, 0, 0, 0, 0, 0, 500);
	Chassis_MOTO[3].pid_speed.PidSet_all(&Chassis_MOTO[3].pid_speed, POSITION_PID, 2500, 500, -500, 5, 0.1, 0, 0, 0, 0, 0, 0, 500);
	
	Chassis_power.pid_speed.PidSet_all(&Chassis_power.pid_speed, POSITION_PID, 10000, 0.1, -20000, 5, 2, 0, 0, 0, 50000, 100, 0, 500);
	
	Yaw_MOTO.pid_angle.PidSet_all(&Yaw_MOTO.pid_angle, POSITION_PID, 1000, 100, -100, 1 , 0.0001, 5, 0, 0,  30, 10, 0, 0);
	Yaw_MOTO.pid_speed.PidSet_all(&Yaw_MOTO.pid_speed, POSITION_PID, 25000, 200, -200, 40, 0, 0, 0, 0, 40, 10, 0, 500);
	

	//Pitch_MOTO.pid_angle.PidSet_all(&Pitch_MOTO.pid_angle, POSITION_PID, 500, 20, -10, 25.0, 0.10, 0, 0, 0, 0, 0, 0, 0);
	//Pitch_MOTO.pid_speed.PidSet_all(&Pitch_MOTO.pid_speed, POSITION_PID, 5000, 130, -130, 4.0, 0.10, 0, 0, 0.5, 0, 0, 0, 500);
	
	TG_MOTO[0].pid_angle.PidSet_all(&TG_MOTO[0].pid_angle, POSITION_PID, 5800, 500, -500, 0.1, 0.001, 0, 0, 0, 0, 0, 0, 0);
	TG_MOTO[0].pid_speed.PidSet_all(&TG_MOTO[0].pid_speed, POSITION_PID, 10000, 2000, -2000, 10, 0.51, 0, 0, 0, 0, 0, 0, 0);
	
	TG_MOTO[1].pid_angle.PidSet_all(&TG_MOTO[1].pid_angle, POSITION_PID, 2500, 500, -500, 0.1, 0.001, 0, 0, 0, 0, 0, 0, 0);
	TG_MOTO[1].pid_speed.PidSet_all(&TG_MOTO[1].pid_speed, POSITION_PID, 2500, 500, -500, 0.1, 0.001, 0, 0, 0, 0, 0, 0, 0);	

	Fricition_MOTO.pid_speed.PidSet_all(&Fricition_MOTO.pid_speed, POSITION_PID, 2000, 500, -500, 5, 0.01, 0, 0, 0, 0, 0, 0, 0);
}

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

