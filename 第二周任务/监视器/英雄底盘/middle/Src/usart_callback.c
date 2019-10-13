#include "main.h"
#include "usart.h"
#include "tripod.h"
#include "dji_Protocol.h"
#include "tx2_Protocol.h"
#include "t_monitor.h"
#include "hwt901b.h"
#include "m_remote.h"
#include "m_moto.h"
#include "t_moto.h"

extern float	pitchL_angle_test;
extern float	pitchR_angle_test;
extern float yaw_angle_test;
float debug_pitch_up = 0.04;
float debug_pitch_down = 0.04;
float debug_yaw_left = 0.05;
float debug_yaw_down = 0.05;

uint32_t tx2cnt;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart == &huart1){		
		monitor_remote.circle_number = 0;
		RemoteDataProcess(uart1_rx_buff, &RC_CtrlData);	
		HAL_UART_Receive_DMA(&huart1,uart1_rx_buff,len_uart1_rx_buff); 
	}
	else if(huart == &huart2){
		HAL_UART_Receive_DMA(&huart2,uart2_rx_buff,len_uart2_rx_buff); 
	}
	else if(huart == &huart3){
		monitor_tx2.circle_number = 0;
		TX2_DataProcess(uart3_rx_buff, &TX2_Data,len_uart3_rx_buff);
		HAL_UART_Receive_DMA(&huart3,uart3_rx_buff,len_uart3_rx_buff);
	}
	else if(huart == &huart6){
//		HWT901DataProcess(uart6_rx_buff,len_uart6_rx_buff);
//		Tripod.Pitch.angle=ANGLE.Angle[0]+180.0f;
//		Tripod.Pitch.speed=-ANGLE.Angular_Velocity[0];
//			//Tripod.Yaw.speed=-ANGLE.Angular_Velocity[2]-RoboData.chassis_ctrl.yaw_speed_gyro;
//		Tripod.Yaw.speed=ANGLE.Angular_Velocity[2];
		HAL_UART_Receive_DMA(&huart6,uart6_rx_buff,len_uart6_rx_buff);
	}
	else if(huart == &huart7){
		dji_DataProcess(uart7_rx_buff, &DJI_ReadData, &RoboData);
		HAL_UART_Receive_DMA(&huart7,uart7_rx_buff,len_uart7_rx_buff);

	}
}

