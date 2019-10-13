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
	all_bsp_init();//�ײ��ʼ��
	all_pid_init();
	time_piece_start = 1;
	while (1)
	{	
		//����ʱ��Ƭ������ӦҪ��͵ķ�����
		if((time_piece&time_piece_0100hz) == time_piece_0100hz)
		{			
			time_piece &= ~time_piece_0100hz;
			monitor_remote_process(&monitor_remote);//ң���������
			monitor_tx2.monitor_process(&monitor_tx2);//tx2�����
			monitor_can_power_process(&monitor_can_power);		
		}
		else if((time_piece&time_piece_1000hz) == time_piece_1000hz)
		{
			time_piece &= ~time_piece_1000hz;
			RoboData.GetRemoteControlData(&RoboData);//ң�������ݴ���			
		}
	}
}	

void all_bsp_init(void){
	HAL_Init();//4λ��ռ���ȼ���0λ��Ӧ���ȼ�
	SystemClock_Config();

	MX_GPIO_Init();
	HAL_GPIO_WritePin(LASER_GPIO_Port,LASER_Pin,GPIO_PIN_SET);		//��
	HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_RESET);		//��
 	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin,GPIO_PIN_RESET);//��
	HAL_GPIO_WritePin(Power_Port,Power_Pin,GPIO_PIN_SET);
	
	/*------------��ʼ��������&����ָ��ָ��&�����˿����������----------------*/
	InitRoboData(&RoboData);
	MX_SPI5_Init();
	MPU6500_Init();
	Chassis_Init();
	Tripod_Init();
	Strike_Init();
	
	MX_CAN1_Init();
	MX_CAN2_Init();//3508���canͨ�ų�ʼ�����ж���ռ���ȼ�Ϊ1
	CAN_Filter_Init_Recv_All();//3508���canͨ�Ź�������ʼ��
	HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);//3508���canͨ�Ž����ж�����
	HAL_CAN_Receive_IT(&hcan2, CAN_FIFO0);
	
	MX_USART1_UART_Init();//ң����   �����ʣ�100000
	MYDMA_Config(DMA2_Stream2,DMA_CHANNEL_4,&hdma_usart1_rx);//��ʼ��USART1��DMA��USART1��DMA�����ж���ռ���ȼ�Ϊ0,�жϷ�������dma��
	while(HAL_UART_Receive_DMA(&huart1,uart1_rx_buff,len_uart1_rx_buff)!=HAL_OK);//DMA while(1)֮ǰ����һ��DMA����
	
//	MX_USART2_UART_Init();//
//	MYDMA_Config(DMA1_Stream5,DMA_CHANNEL_4,&hdma_usart2_rx);//��ʼ��DMA
//	while(HAL_UART_Receive_DMA(&huart2,uart2_rx_buff,22u)!=HAL_OK);//DMA��ʽ

	MX_USART3_UART_Init();//
	MYDMA_Config(DMA1_Stream1,DMA_CHANNEL_4,&hdma_usart3_rx);	
	while(HAL_UART_Receive_DMA(&huart3,uart3_rx_buff,len_uart3_rx_buff)!=HAL_OK);

//	MX_USART6_UART_Init();//
//	MYDMA_Config(DMA2_Stream1,DMA_CHANNEL_5,&hdma_usart6_rx);
//	while(HAL_UART_Receive_DMA(&huart6,uart6_rx_buff,len_uart6_rx_buff)!=HAL_OK);

	MX_USART7_UART_Init();//����ϵͳ
	MYDMA_Config(DMA1_Stream3,DMA_CHANNEL_5,&hdma_usart7_rx);
	{while(HAL_UART_Receive_DMA(&huart7,uart7_rx_buff,len_uart7_rx_buff)!=HAL_OK);}

  MX_TIM1_Init();
 	
	TIM3_Init(1000-1,840-1);//100hz		
	
	TIM5_Init(100-1,840-1);//1000hz                               
				
	HAL_Delay(1500);
	
	
	Strike.count_42mm.out_firecounter=3;
	
	/*���ܽṹ���ʼ��*/
}

void all_pid_init(void){
	/*���̵����ʼ��*/
	PidSet_all(&Chassis_MOTO[0].pid_speed, POSITION_PID, 16000, 5000, -5000, 6, 0.2, 0, 0, 0, 5000, 2000, 0, 500);
	PidSet_all(&Chassis_MOTO[1].pid_speed, POSITION_PID, 16000, 5000, -5000, 6, 0.2, 0, 0, 0, 5000, 2000, 0, 500);
	PidSet_all(&Chassis_MOTO[2].pid_speed, POSITION_PID, 16000, 5000, -5000, 6, 0.2, 0, 0, 0, 5000, 2000, 0, 500);
	PidSet_all(&Chassis_MOTO[3].pid_speed, POSITION_PID, 16000, 5000, -5000, 6, 0.2, 0, 0, 0, 5000, 2000, 0, 500);

	
	
	/*���������ʼ��*/
	PidSet_all(&Ammunition_MOTO.pid_angle, POSITION_PID, 2, 0, -0, 0.2, 0, 0, 0, 0, 0, 0, 0, 0);	
	PidSet_all(&Ammunition_MOTO.pid_speed, POSITION_PID, 12000, 2000, -2000, 1, 0.01, 0, 0, 0, 10000, 3000, 0, 0);	
	PidSet_all(&LittleAmmo_MOTO.pid_speed, POSITION_PID, 8000, 4000, -4000, 10, 0.0001, 0,  0, 0, 0, 0, 0, 0);
	PidSet_all(&Ctrl_2006_MOTO.pid_angle, POSITION_PID, 4000, 500, -500, 7, 0, 0, 0, 0, 0, 0, 0, 0);
	PidSet_all(&Ctrl_2006_MOTO.pid_speed, POSITION_PID, 6000, 500, -500, 9, 0.1, 2, 0, 0, 0, 0, 0, 0);
	
	PidSet_all(&Friction_Wheel_MOTO[0].pid_speed, POSITION_PID, 16000, 500, -500, 20, 0.1, 0, 0, 0, 0, 0, 0, 0);
	PidSet_all(&Friction_Wheel_MOTO[1].pid_speed, POSITION_PID, 16000, 500, -500, 20, 0.1, 0, 0, 0, 0, 0, 0, 0);
	
	
	
	HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_SET);//��
}//PID������ʼ��

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

