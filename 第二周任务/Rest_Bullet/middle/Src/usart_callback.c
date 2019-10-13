#include "main.h"
#include "chassis.h"
#include "usart.h"
#include "m_remote.h"
#include "dji_Protocol.h"
#include "tx2_Protocol.h"
#include "t_monitor.h"
#include "idle_interrupt.h"

 float bulletspeed = 0.0f;
	uint8_t shootflag;
	uint32_t counter;


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if(huart == &huart1)
	{
			monitor_remote.circle_number = 0;
			RemoteDataProcess(uart1_rx_buff, &RC_CtrlData);
	
		HAL_UART_Receive_DMA(&huart1,uart1_rx_buff,len_uart1_rx_buff); 
	}
	else if(huart == &huart2)
	{
		HAL_UART_Receive_DMA(&huart2,uart2_rx_buff,len_uart2_rx_buff); 
	}
	else if(huart == &huart3)
	{ 
		 	HAL_UART_Receive_IT(&huart3, uart3_rx_buff, len_uart3_rx_buff);
		
			if(TX2_DataProcess(uart3_rx_buff, &TX2_Data,len_uart3_rx_buff))
			{
				monitor_tx2_valid.circle_number = 0;
				
			}
			monitor_tx2.circle_number = 0;
		  HAL_UART_Receive_DMA(&huart3, uart3_rx_buff, len_uart3_rx_buff);
			
	}
	else if(huart == &huart6)
	{
		monitor_dji.circle_number=0;
		dji_DataProcess(uart6_rx_buff, &DJI_ReadData, &RoboData);
		if(DJI_ReadData.ext_shoot_data.bullet_speed!=bulletspeed&&DJI_ReadData.ext_shoot_data.bullet_speed>0)
		{
			counter++;
			//Chassis.heat.client_ammo_left -= 1;
		}
		
		bulletspeed = DJI_ReadData.ext_shoot_data.bullet_speed;
		HAL_UART_Receive_DMA(&huart6,uart6_rx_buff,len_uart6_rx_buff);
	}
		else if(huart == &huart7)
	{

//			monitor_tx2.circle_number = 0;
//			TX2_DataProcess(uart7_rx_buff, &TX2_Data,len_uart7_rx_buff);
//		
//		HAL_UART_Receive_DMA(&huart7,uart7_rx_buff,len_uart7_rx_buff);
		
		   	HAL_UART_Receive_IT(&huart7, uart7_rx_buff, len_uart7_rx_buff);
		
			if(TX2_DataProcess(uart7_rx_buff, &TX2_Data,len_uart7_rx_buff))
			{
				monitor_tx2_valid.circle_number = 0;
				
			}
			monitor_tx2.circle_number = 0;
		  HAL_UART_Receive_DMA(&huart7, uart7_rx_buff, len_uart7_rx_buff);
			
			
		
	}
	else if(huart == &huart8)
	{

		
		if(TX2_DataProcess(uart8_rx_buff, &TX2_Data,len_uart8_rx_buff))
			monitor_tx2.circle_number = 0;
	
	HAL_UART_Receive_DMA(&huart8,uart8_rx_buff,len_uart8_rx_buff);
	
	}
}

void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart3)
		huart->gState=HAL_UART_STATE_READY;
}

void uart_rx_idle_callback(UART_HandleTypeDef* huart)
{
  
  if (huart == &huart1)
  {
    /* handle dbus data dbus_buf from DMA */
    if ((DBUS_MAX_LEN - (uint16_t)(huart->hdmarx->Instance->NDTR)) == DBUS_BUFLEN)
    {
			monitor_remote.circle_number = 0;
			RemoteDataProcess(dbus_buf, &RC_CtrlData);

    }

  }
}
