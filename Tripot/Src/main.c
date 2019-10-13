
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "can.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "mytype.h"
#include "control.h"
#include "strike.h"
#include "robodata.h"
#include "t_monitor.h"
#include "tripod.h"
#include "strike.h"



//#define Debug_tripod
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void all_bsp_init(void);
void all_pid_init(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
//unsigned char data_to_send[16];
float var[8] = {0};
int var_time_count = 0;
void Data_Send(u8 *pst)
{
  unsigned char _cnt=0;	unsigned char sum = 0;
	unsigned char data_to_send[23];         //发送缓存
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x02;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=(unsigned char)(pst[0]>>8);  //高8位
	data_to_send[_cnt++]=(unsigned char)pst[0];  //低8位
	data_to_send[_cnt++]=(unsigned char)(pst[1]>>8);
	data_to_send[_cnt++]=(unsigned char)pst[1];
	data_to_send[_cnt++]=(unsigned char)(pst[2]>>8);
	data_to_send[_cnt++]=(unsigned char)pst[2];
	data_to_send[_cnt++]=(unsigned char)(pst[3]>>8);
	data_to_send[_cnt++]=(unsigned char)pst[3];
	data_to_send[_cnt++]=(unsigned char)(pst[4]>>8);
	data_to_send[_cnt++]=(unsigned char)pst[4];
	data_to_send[_cnt++]=(unsigned char)(pst[5]>>8);
	data_to_send[_cnt++]=(unsigned char)pst[5];
	data_to_send[_cnt++]=(unsigned char)(pst[6]>>8);
	data_to_send[_cnt++]=(unsigned char)pst[6];
	data_to_send[_cnt++]=(unsigned char)(pst[7]>>8);
	data_to_send[_cnt++]=(unsigned char)pst[7];
	data_to_send[_cnt++]=(unsigned char)(pst[8]>>8);
	data_to_send[_cnt++]=(unsigned char)pst[8];
	
	data_to_send[3] = _cnt-4;
	
	sum = 0;
        
	for(unsigned char i=0;i<_cnt;i++)
		sum += data_to_send[i];
  
	data_to_send[_cnt++] = sum;
	
	HAL_UART_Transmit(&huart3,data_to_send,sizeof(data_to_send),1000);
	
}

void vcan_sendware(u8 *wareaddr, u32 waresize)
{
#define CMD_WARE     3
    u8 cmdf[2]={CMD_WARE, ~CMD_WARE};    //串口调试 使用的前命令
    u8 cmdr[2]={~CMD_WARE, CMD_WARE};    //串口调试 使用的后命令

		HAL_UART_Transmit(&huart3,cmdf,sizeof(cmdf),1000);
		HAL_UART_Transmit(&huart3,wareaddr,waresize,1000);
		HAL_UART_Transmit(&huart3,cmdr,sizeof(cmdr),1000);
}


/* USER CODE END 0 */


/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  HAL_Init();

  /* USER CODE BEGIN Init */
	__HAL_AFIO_REMAP_SWJ_ENABLE();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();


	all_bsp_init();
  /* USER CODE BEGIN SysInit */
  all_pid_init();
  /* USER CODE END SysInit */
  /* Initialize all configured peripherals */
	
  /* USER CODE BEGIN 2 */
	//	HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);//3508电机can通信接收中断配置

	while(HAL_UART_Receive_DMA(&huart3,uart3_rx_buff,len_uart3_rx_buff)!=HAL_OK);
	while(HAL_UART_Receive_DMA(&huart4,uart4_rx_buff,len_uart4_rx_buff)!=HAL_OK);//DMA while(1)之前启动一下DMA接收

  /* USER CODE END 2 */
	time_piece |=time_piece_start;
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
	{	
		//部分时间片
		if((time_piece&time_piece_0100hz)==time_piece_0100hz)
		{
			time_piece &=~time_piece_0100hz;//100hz

			RoboData.GetDJIControlData(&RoboData);//获取裁判系统数据	
			RoboData.GetRemoteControlData(&RoboData);//遥控器数据处理
			monitor_remote.monitor_process(&monitor_remote);//遥控器监视器
			//monitor_tx2.monitor_process(&monitor_tx2);//tx2监控器
			monitor_can_power.monitor_process(&monitor_can_power);//超级电容控制板can通信监控器
			//monitor_JY901.monitor_process(&monitor_JY901);
			
		}
		else if((time_piece&time_piece_1000hz)==time_piece_1000hz)
		{
			static int transmit_count=0;
			time_piece &=~time_piece_1000hz;//1000hz
			transmit_count++;
			var_time_count++;
			if(var_time_count == 10)
				var_time_count = 0;
			

		//	RoboData.GetAmmunition17ControlStatus(&RoboData, &TG_MOTO[0]);//17mm发弹状态处理		

				switch(transmit_count)//云台控制板can发送数据
				{
					case 1:
						TX_DATA_CLOUDDECK1_ID(&hcan1);
						HAL_CAN_Transmit(&hcan1, 0);
					  break;
					case 2:
						TX_DATA_CLOUDDECK2_ID(&hcan1);
						HAL_CAN_Transmit(&hcan1, 0);
						break;
					case 3:
						HAL_UART_Receive_DMA(&huart4,uart4_rx_buff,len_uart4_rx_buff);
						break;
					default:			
						transmit_count=0;
						break;
						
				}

		}
		else
		{		
		//通信调试
			if(1) 
			{
				//var[0]=-angle_err;
				var[0]=Tripod.Yaw.angle_set;
				var[1]=Tripod.Yaw.angle;
				var[2]=Tripod.Yaw.speed_set;
				var[3]=Tripod.Yaw.speed;
				var[4]=Tripod.Yaw.current_set;
//				var[1]=Yaw_MOTO6020.getpara.total_round*360.0f;
//				var[3]=Yaw_MOTO6020.getpara.other_speed;
//				var[4]=Yaw_MOTO6020.pid_angle.output.pos_out;
//				var[5]=-Yaw_MOTO6020.send_current;
//				var[6]=kkkkkk;
				var[7]=var_time_count;
				vcan_sendware((u8*) var, (u32)sizeof(var));
				//Data_Send((u8*) var);//山外通信协议打包及发送
			}
		}
		
	}
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType=RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState=RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue=16;
  RCC_OscInitStruct.Prediv1Source=RCC_PREDIV1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLState=RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource=RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL=RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL2.PLL2State=RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) !=HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType=RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider=RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider=RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider=RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) !=HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /**Configure the Systick interrupt time 
    */
  __HAL_RCC_PLLI2S_ENABLE();

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}


/* USER CODE BEGIN 4 */
void all_bsp_init(void)
{
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();

	/*------------初始化机构体&设置指针指向&机器人控制相关数据----------------*/
	InitRoboData(&RoboData);
	/*摩擦轮初始化*/
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
	
	TIM1->CCR1=1000;
	TIM1->CCR2=1000;

	MX_USART3_UART_Init();/*陀螺仪		波特率：115200*/		
	MX_UART4_Init();//MX_USART1_UART_Init();/*遥控器   波特率：100000*/
	//HAL_UART_Receive_IT(&huart4,uart4_rx_buff,len_uart4_rx_buff);
	
	MX_CAN1_Init();
	MX_CAN2_Init();//3508电机can通信初始化，中断抢占优先级为1
	CAN_Filter_Init_Recv_ALL(&hcan1);
	CAN_Filter_Init_Recv_ALL(&hcan2);
	__HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0);
	__HAL_CAN_ENABLE_IT(&hcan2, CAN_IT_FMP0);

	Tripode_Init();//yaw6025&pitch6623初始化
	Strike_Init();
	MX_TIM3_Init();//100hz		
	MX_TIM5_Init();//1000hz   
	MX_TIM4_Init();
	
	

	
	HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_SET);//灭
	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin,GPIO_PIN_RESET);//灭
	HAL_GPIO_WritePin(LASER_GPIO_Port,LASER_Pin,GPIO_PIN_SET);//亮
}

void all_pid_init(void)//PID参数初始化
{


	PidSet_all(&Chassis_MOTO[0].pid_speed, POSITION_PID, 3000, 500, -500, 5, 0.1, 0, 0, 0, 0, 0, 0, 500,0);
	PidSet_all(&Chassis_MOTO[1].pid_speed, POSITION_PID, 3000, 500, -500, 5, 0.1, 0, 0, 0, 0, 0, 0, 500,0);
	PidSet_all(&Chassis_MOTO[2].pid_speed, POSITION_PID, 3000, 500, -500, 5, 0.1, 0, 0, 0, 0, 0, 0, 500,0);
	PidSet_all(&Chassis_MOTO[3].pid_speed, POSITION_PID, 3000, 500, -500, 5, 0.1, 0, 0, 0, 0, 0, 0, 500,0);

	PidSet_all(&TG_MOTO[0].pid_angle, POSITION_PID, 5800, 500, -500, 5, 0, 0, 0, 0, 0, 0, 0, 0,0);
	PidSet_all(&TG_MOTO[0].pid_speed, POSITION_PID, 6000, 2000, -2000, 7, 0.051, 0, 0, 0, 10000, 5000, 0, 0,0);
	
	PidSet_all(&TG_MOTO[1].pid_angle, POSITION_PID, 2500, 500, -500, 0.1, 0.001, 0, 0, 0, 0, 0, 0, 0,0);
	PidSet_all(&TG_MOTO[1].pid_speed, POSITION_PID, 2500, 500, -500, 0.1, 0.001, 0, 0, 0, 0, 0, 0, 0,0);	

	PidSet_all(&Fricition_MOTO[0].pid_speed, POSITION_PID, 2000, 500, -500, 5, 0.01, 0, 0, 0, 0, 0, 0, 0,0);
	PidSet_all(&Fricition_MOTO[1].pid_speed, POSITION_PID, 2000, 500, -500, 5, 0.01, 0, 0, 0, 0, 0, 0, 0,0);

}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
