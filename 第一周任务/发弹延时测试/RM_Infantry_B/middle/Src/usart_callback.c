#include "main.h"
#include "usart.h"
#include "m_remote.h"
#include "dji_Protocol.h"
#include "tx2_Protocol.h"
#include "t_monitor.h"
#include "hwt901b.h"
#include "tripod.h"


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart==&huart4)
	{
		monitor_remote.circle_number=0;
		RemoteDataProcess(uart4_rx_buff, &RC_CtrlData);/*rm遥控器数据*/
		HAL_UART_Receive_DMA(&huart4,uart4_rx_buff,len_uart4_rx_buff); 

	}
	else if(huart==&huart3)
	{
		//Tripod.monitor.circle_number=0;
		
//		HWT901DataProcess(uart3_rx_buff,len_uart3_rx_buff);/*hwt901b陀螺仪数据*/
//		Tripod.Pitch.angle_source.gyro.angle_now=-((180-ANGLE.Angle[0])>180?(-180-ANGLE.Angle[0]):(180-ANGLE.Angle[0]));
//		Tripod.Pitch.speed=ANGLE.Angular_Velocity[0];
//		Tripod.Yaw.speed=-ANGLE.Angular_Velocity[2];
		HAL_UART_Receive_DMA(&huart3,uart3_rx_buff,len_uart3_rx_buff);
	}
}

