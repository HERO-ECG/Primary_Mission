#include "main.h"
#include "usart.h"
#include "m_remote.h"
#include "dji_Protocol.h"
#include "tx2_Protocol.h"
#include "t_monitor.h"
#include "hwt901b.h"
#include "pitch.h"
#include "pid.h"
#include "m_moto.h"
#include "auto_move.h"
#include "strike.h"
float yaw_speed = 0;
int tuoluo_angle_sta=0;
int tuoluo_cnt = 0;
float yaw_angle = 0;
float last_angle=0;
float tuoluo_last_angle=0;
float tuoluo_offset_angle = 0;
float tuoluo_offset_round = 0;
float tuoluo_total_angle = 0;
float tuoluo_round_total = 0;
float tuoluo_round=0;
float pitch_dangshi_angle=0;
float yaw_dangshi_angle=0;

void Yaw_AngleGet(void);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart1)
	{
				
		
			monitor_remote.circle_number = 0;
			RemoteDataProcess(uart1_rx_buff, &RC_CtrlData);
	
		HAL_UART_Receive_DMA(&huart1,uart1_rx_buff,len_uart1_rx_buff); 
	}
//	else if(huart == &huart2)
//	{
//		#ifdef Chassis
//		#else
//		#endif
//		HAL_UART_Receive_DMA(&huart2,uart2_rx_buff,len_uart2_rx_buff); 
//	}
	else if(huart == &huart3)
	{

			monitor_tx2.circle_number = 0;
		usart_monitor=0;
			TX2_DataProcess(uart3_rx_buff, &tx2_ReadData);
	
		HAL_UART_Receive_DMA(&huart3,uart3_rx_buff,len_uart3_rx_buff);
	}
	else if(huart == &huart6)
	{
			monitor_JY901.circle_number=0;	
			
			HWT901DataProcess(uart6_rx_buff,len_uart6_rx_buff);/*hwt901bÍÓÂÝÒÇÊý¾Ý*/
			Pitch.speed=ANGLE.Angular_Velocity[0];

			Yaw_AngleGet();
			yaw_speed=ANGLE.Angular_Velocity[2];
		HAL_UART_Receive_DMA(&huart6,uart6_rx_buff,len_uart6_rx_buff);
//		#ifdef Chassis
//			dji_DataProcess(uart6_rx_buff, &DJ_ReadData, &RoboData);
//		#else
//		#endif
//		HAL_UART_Receive_DMA(&huart6,uart6_rx_buff,len_uart6_rx_buff);
	}
	else if(huart == &huart8)
	{
		dji_DataProcess(uart8_rx_buff, &DJI_ReadData, &RoboData);
		restbullet();
		Head_refresh();
		
		HAL_UART_Receive_DMA(&huart8,uart8_rx_buff,len_uart8_rx_buff);
	}		
}



void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart3)
		huart->gState=HAL_UART_STATE_READY;

}


void Yaw_AngleGet(void)
{
	if(tuoluo_angle_sta == 0)
	{
		tuoluo_cnt++;
		if(tuoluo_cnt>10)
		{	tuoluo_angle_sta=1;}
		else
		{
		tuoluo_offset_angle=ANGLE.Angle[2];
		tuoluo_offset_round=(float)(ANGLE.Angle[2])/360.0f;
		}
		tuoluo_last_angle=tuoluo_offset_angle;
	}
	else
	{
		yaw_angle=ANGLE.Angle[2];
		if(yaw_angle-tuoluo_last_angle>180)
		{tuoluo_round--;}
		else if(yaw_angle-tuoluo_last_angle<-180)
		{tuoluo_round++;}
		tuoluo_total_angle=tuoluo_round*360+yaw_angle-tuoluo_offset_angle;
		tuoluo_round_total=tuoluo_round+(float)(yaw_angle)/360.0f-tuoluo_offset_round;
		
		tuoluo_last_angle=yaw_angle;
	}
			
}
