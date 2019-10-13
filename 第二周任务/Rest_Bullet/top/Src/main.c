#include "main.h"
#include "stm32f4xx_hal.h"
#include "mytype.h"
#include "dma.h"
#include "usb_device.h"
#include "control.h"
#include "t_moto.h"
#include "robodata.h"
#include "t_monitor.h"
#include "idle_interrupt.h"
#include "chassis.h"


void SystemClock_Config(void);
void Error_Handler(void);
void all_bsp_init(void);
void all_pid_init(void);

float var[8];
void vcan_sendware(u8 *wareaddr, u32 waresize);

int main(void)
{
    all_bsp_init();
    all_pid_init();
    time_piece |= time_piece_start;

    while (1)
    {
        //部分时间片
        if((time_piece&time_piece_0100hz) == time_piece_0100hz) {

            time_piece &= ~time_piece_0100hz;//100hz

//			mavlink_test(); // 发送

            RoboData.GetDJIControlData(&RoboData);//获取裁判系统数据

//	  RoboData.GetRemoteControlData(&RoboData);//遥控器数据处理

            monitor_remote.monitor_process(&monitor_remote);//遥控器监视器
            //monitor_can_power.monitor_process(&monitor_can_power);//超级电容控制板can通信监控器

            //MYDMA_USART_Transmit(&huart6,uart6_tx_buff,len_uart6_tx_buff);
//		HAL_UART_Transmit_DMA(&huart3,TX2_Data_Send,sizeof(TX2_Data_Send));

#ifdef ChassisPowerControl
            RoboData.GetChassisControlStatus(&RoboData);
            switch_chassis_speed_power(70);//超级电容控制&(底盘pid参数)上坡与平地的切换
#endif

        }
        else if((time_piece&time_piece_1000hz) == time_piece_1000hz) {
            static int transmit_count = 0;
            transmit_count++;
            time_piece &= ~time_piece_1000hz;//1000hz


            switch(transmit_count)//底盘控制板can发送数据
            {
            case 1:
                TX_DATA_CHASSIS1_ID(&hcan1);
                HAL_CAN_Transmit(&hcan1, 5);
                break;
            case 2:
                TX_DATA_CHASSIS2_ID(&hcan1);
                HAL_CAN_Transmit(&hcan1, 5);
                break;
            case 3:
                TX_DATA_CHASSIS3_ID(&hcan1);
                HAL_CAN_Transmit(&hcan1, 5);
                transmit_count = 0;
                break;
            }

        }
        else {
            //底盘速度设置,底盘正方向与云台正方向差角为theta°，向右为正



//				var[0] = 0;

//				vcan_sendware((u8*) var, (u32)sizeof(var));

        }
//	SetMotoCurrent(&hcan1, STDID_3508, Chassis_MOTO[0].send_current, Chassis_MOTO[1].send_current, Chassis_MOTO[2].send_current, Chassis_MOTO[3].send_current);
//	SetMotoCurrent(&hcan2, STDID_2006_6623, Yaw_MOTO6020.send_current, 0, 0, 0);
    }
    /*end*/
}

void all_bsp_init(void)
{
    HAL_Init();//4位抢占优先级、0位响应优先级
    SystemClock_Config();

    MX_GPIO_Init();
    //HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_RESET);//亮
    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin,GPIO_PIN_SET);//灭
    HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_SET);//灭
    HAL_GPIO_WritePin(Power_Port,Power_Pin,GPIO_PIN_SET);  //power

    /*------------初始化机构体&设置指针指向&机器人控制相关数据----------------*/
    InitRoboData(&RoboData);
    MX_SPI5_Init();
    MPU6500_Init();
    MX_CAN1_Init();//3508、6623
    MX_CAN2_Init();
    CAN_Filter_Init_Recv_All();//3508，6623

//  CAN的接收中断函数在m_moto.c中

    Chassis_Init();

    MX_USART1_UART_Init();//遥控器   波特率：100000
    MYDMA_Config(DMA2_Stream2,DMA_CHANNEL_4,&hdma_usart1_rx);//初始化USART1到DMA，USART1到DMA接收中断抢占优先级为0,中断服务函数在dma中
//	while(HAL_UART_Receive_DMA(&huart1,uart1_rx_buff,36u)!=HAL_OK);//DMA while(1)之前启动一下DMA接收

    MX_USART2_UART_Init();//接收姿态角   波特率：115200
    MYDMA_Config(DMA1_Stream5,DMA_CHANNEL_4,&hdma_usart2_rx);//初始化DMA
//
    MX_USART3_UART_Init();//数传电台//现也用作PID调参    波特率：9600
    MYDMA_Config(DMA1_Stream1,DMA_CHANNEL_4,&hdma_usart3_rx);
    MYDMA_Config(DMA1_Stream3,DMA_CHANNEL_4,&hdma_usart3_tx);

    MX_USART6_UART_Init();//
    MYDMA_Config(DMA2_Stream1,DMA_CHANNEL_5,&hdma_usart6_rx);
    MYDMA_Config(DMA2_Stream7,DMA_CHANNEL_5,&hdma_usart6_tx);

    MX_USART7_UART_Init();//
    MYDMA_Config(DMA1_Stream3,DMA_CHANNEL_5,&hdma_usart7_rx);

    MX_USART8_UART_Init();//

    //MYDMA_Config(DMA1_Stream6,DMA_CHANNEL_5,&hdma_usart8_rx);
    //MYDMA_Config(DMA1_Stream0,DMA_CHANNEL_5,&hdma_usart8_tx);
//		Mavlink_Init(); //初始化MAVLINK：使能接收，ringbuffer关联
    //TIM2_Init(20000-1,90-1);

    //TIM12_Init(10-1,840-1);//100hz

    TIM3_Init(1000-1,900-1);//100hz

    TIM5_Init(100-1,900-1);//1000hz
    TIM12_Init();
    HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1);

    HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);//3508电机can通信接收中断配置
    HAL_CAN_Receive_IT(&hcan2, CAN_FIFO0);//重新使能can中断标志

    while(HAL_UART_Receive_DMA(&huart2,uart2_rx_buff,len_uart2_rx_buff)!=HAL_OK);//DMA方式
    while(HAL_UART_Receive_DMA(&huart3,uart3_rx_buff,len_uart3_rx_buff)!=HAL_OK);
    while(HAL_UART_Receive_DMA(&huart6,uart6_rx_buff,len_uart6_rx_buff)!=HAL_OK);
    while(HAL_UART_Receive_DMA(&huart7,uart7_rx_buff,len_uart7_rx_buff)!=HAL_OK);
    //while(	HAL_UART_Receive_IT(&huart8, &mavlink_byte, 1u)!=HAL_OK);
// while(HAL_UART_Receive_DMA(&huart8,uart8_rx_buff,len_uart8_rx_buff)!=HAL_OK);
    //dbus_uart_init();//遥控器uart1的空闲中断
}

void all_pid_init(void)//PID参数初始化
{
    /*void PidSet_all(Pid_t *pid, uint32_t mode,
    							 float max_out, float integral_uplimit, float integral_downlimit,
                   float p, float i, float d,
    							 float deadband, float f, float max_err, float mid_err, float min_err, float f_out_limit, float f_divider)*/

    PidSet_all(&Chassis_MOTO[0].pid_speed, POSITION_PID, 16000, 5000, -5000, 6, 0.2, 0, 0, 0, 5000, 2000, 0, 500, 0);
    PidSet_all(&Chassis_MOTO[1].pid_speed, POSITION_PID, 16000, 5000, -5000, 6, 0.2, 0, 0, 0, 5000, 2000, 0, 500, 0);
    PidSet_all(&Chassis_MOTO[2].pid_speed, POSITION_PID, 16000, 5000, -5000, 6, 0.2, 0, 0, 0, 5000, 2000, 0, 500, 0);
    PidSet_all(&Chassis_MOTO[3].pid_speed, POSITION_PID, 16000, 5000, -5000, 6, 0.2, 0, 0, 0, 5000, 2000, 0, 500, 0);


    TG_MOTO[0].pid_angle.PidSet_all(&TG_MOTO[0].pid_angle, POSITION_PID, 5800, 500, -500, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    TG_MOTO[0].pid_speed.PidSet_all(&TG_MOTO[0].pid_speed, POSITION_PID, 6000, 500, -500, 10, 0.1, 2, 0, 0, 0, 0, 0, 0, 0);

    TG_MOTO[1].pid_angle.PidSet_all(&TG_MOTO[1].pid_angle, POSITION_PID, 500, 50, -50, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    TG_MOTO[1].pid_speed.PidSet_all(&TG_MOTO[1].pid_speed, POSITION_PID, 3000, 500, -500, 5, 0.15, 0, 0, 8, 0, 0, 0, 2000, 0);

    Fricition_MOTO[0].pid_speed.PidSet_all(&Fricition_MOTO[0].pid_speed, POSITION_PID, 2000, 500, -500, 5, 0.01, 0, 0, 0, 0, 0, 0, 0, 0);
    Fricition_MOTO[1].pid_speed.PidSet_all(&Fricition_MOTO[1].pid_speed, POSITION_PID, 2000, 500, -500, 5, 0.01, 0, 0, 0, 0, 0, 0, 0, 0);

    //TW_servo.PID_angle.PidSet_all(&TW_servo.PID_angle, POSITION_PID, 5000, 0, 0, 80, 0, 40, 0, 0, 0, 0, 0, 0, 0);
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

\
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
      ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}

