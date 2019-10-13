/*******************************************************************************************
  * @Include @Headfile
 *******************************************************************************************/
#include "main.h"
#include "can.h"
#include "strike.h"
#include "m_remote.h"
#include "robodata.h"
#include "t_monitor.h"
#include "dji_Protocol.h"
#include "tx2_Protocol.h"

#include "tripod.h"

float gyro[4];

/*******************************************************************************************
  * @Parameter @Statement
 *******************************************************************************************/
/*******************************************************************************************
  * @Func			HAL_CAN_RxCpltCallback
  * @Brief    CAN接收回调函数
  * @Param		CAN_HandleTypeDef* hcan
  * @Retval		None
  * @Date     2018.10.10
  *	@Author		SZC		LZK添加CAN入口判断
 *******************************************************************************************/

float last_heat;
uint16_t counter;
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan){
	if(hcan==&hcan1){	
		if(hcan->pRxMsg->IDE==CAN_ID_STD){
			switch(hcan->pRxMsg->StdId){
				case Pitch6020_MOTO_ID:
					GetEncoderMeasure(&Pitch_MOTO6020.getpara, hcan);
				if(Pitch_MOTO6020.getpara.init_sta==1)				
					Tripod.Pitch.angle_source.encoder.angle_relativeToChassis=mode360((Pitch_MOTO6020.getpara.angle)/8192.0f*360.0f - PITCH_OFFSET);
				break;
				case Yaw6020_MOTO_ID:
					GetEncoderMeasure(&Yaw_MOTO6020.getpara, hcan);
				if(Yaw_MOTO6020.getpara.init_sta==1)
						Tripod.Yaw.angle_source.encoder.angle_relativeToChassis=mode360((Yaw_MOTO6020.getpara.angle)/8192.0f*360.0f-YAW_OFFSET);
				break;
			}
		}
		else if(hcan->pRxMsg->IDE==CAN_ID_EXT){
			switch(hcan->pRxMsg->ExtId){
				
					case CHASSIS1_ID://云台开发板接收底盘开发板的信息
					{
						Strike.heat.heat_server=(int16_t)(hcan->pRxMsg->Data[0]<<8 | hcan->pRxMsg->Data[1])/1.0f	;		
						TX2_Data.yaw_angle=(int16_t)(hcan->pRxMsg->Data[2]<<8 | hcan->pRxMsg->Data[3])/1000.0f;			
						TX2_Data.pitch_angle=(int16_t)(hcan->pRxMsg->Data[4]<<8 | hcan->pRxMsg->Data[5])/1000.0f;	
						TX2_Data.mode_now=(TXMode_e)(hcan->pRxMsg->Data[6]);
						TX2_Data.distance=(hcan->pRxMsg->Data[7]);
					}break;
					case CHASSIS2_ID://云台开发板接收底盘开发板的信息
					{
						monitor_tx2.circle_number=0;
						RoboData.chassis_ctrl.pitch_speed_gyro=(float)((int16_t)(hcan->pRxMsg->Data[0]<<8 | hcan->pRxMsg->Data[1]))/1638;
						RoboData.chassis_ctrl.yaw_speed_gyro=(float)((int16_t)(hcan->pRxMsg->Data[2]<<8 | hcan->pRxMsg->Data[3]))/1638;
						Strike.heat.cooling_rate_server=hcan->pRxMsg->Data[4];
						Strike.heat.cooling_limit_server=(int16_t)(hcan->pRxMsg->Data[5]<<8 | hcan->pRxMsg->Data[6]);
						Strike.heat.robot_level=hcan->pRxMsg->Data[7];
					}break;
					case CHASSIS3_ID://云台开发板接收底盘开发板的信息
					{

					}break;
			}
		}
			__HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0);//重新使能can中断标志
	}
	else if(hcan==&hcan2)
	{
		if(hcan->pRxMsg->IDE==CAN_ID_STD)
			{
			switch(hcan->pRxMsg->StdId)
				{
					case 	MOTO1_2006_ID :
						GetEncoderMeasure(&TG_MOTO[0].getpara, hcan);
					break;
				}
			}
			else if(hcan->pRxMsg->IDE==CAN_ID_EXT)
				{
					switch(hcan->pRxMsg->ExtId)
					{
							case 0x1315:
							{
								Tripod.monitor.circle_number=0;
								HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_RESET);
								gyro[0]=(int16_t)(hcan->pRxMsg->Data[0]<<8 | hcan->pRxMsg->Data[1])/16.384f;	
								gyro[1]=(int16_t)(hcan->pRxMsg->Data[2]<<8 | hcan->pRxMsg->Data[3])/16.384f;
								gyro[2]=(int16_t)(hcan->pRxMsg->Data[4]<<8 | hcan->pRxMsg->Data[5])/16.384f;
								gyro[3]=(int16_t)(hcan->pRxMsg->Data[6]<<8 | hcan->pRxMsg->Data[7])/100.0f;
							}break;
					}
				}
		__HAL_CAN_ENABLE_IT(&hcan2, CAN_IT_FMP0);
	}

	
}
