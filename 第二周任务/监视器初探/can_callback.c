
/*******************************************************************************************
  * @Include @Headfile
 *******************************************************************************************/
#include "moto_encoder.h"
#include "main.h"
#include "can.h"
#include "m_moto.h"
#include "m_remote.h"
#include "tripod.h"
#include "chassis.h"
#include "robodata.h"
#include "t_monitor.h"
#include "dji_Protocol.h"
#include "tx2_Protocol.h"
#include "strike.h"
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
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan){
	if(hcan == &hcan1){	
		if(hcan->pRxMsg->IDE==CAN_ID_STD){
			switch(hcan->pRxMsg->StdId){
				case MOTO1_3508_ID://201
					Friction_Wheel_MOTO[0].getpara.GetEncoderMeasure(&Friction_Wheel_MOTO[0].getpara, hcan);
				    monitor_motor[1].circle_number=0;
				break;
				case MOTO2_3508_ID://202
					Friction_Wheel_MOTO[1].getpara.GetEncoderMeasure(&Friction_Wheel_MOTO[1].getpara, hcan);
				    monitor_motor[2].circle_number=0;
				break;
				
				/*双电机主从模式，左电机为主控制电机，右电机为从动电机*/
				case PITCHR_6020_ID://205
					GetEncoderMeasure(&Pitch_MOTO[0].getpara, hcan);
					if(Pitch_MOTO[0].getpara.init_sta == 1){				
						Tripod.Pitch.angle_source.encoder.angle_now=-(Pitch_MOTO[0].getpara.angle)/8192.0f*360.0f+PITCH6020_OFFSET;
					}
				break;
				case PITCHL_6020_ID://206
					Pitch_MOTO[1].getpara.GetEncoderMeasure(&Pitch_MOTO[1].getpara, hcan);
				break;


			}
		}
		__HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0);//重新使能can中断标志
	}
	else if(hcan == &hcan2){	
		if(hcan->pRxMsg->IDE==CAN_ID_STD){
			switch(hcan->pRxMsg->StdId){
//				case MOTO1_3508_ID:
//				case MOTO2_3508_ID:
//				case MOTO3_3508_ID:
//				case MOTO4_3508_ID:{
//					uint32_t can_id = 0;
//					can_id = hcan->pRxMsg->StdId - MOTO1_3508_ID;
//					Chassis_MOTO[can_id].getpara.GetEncoderMeasure(&Chassis_MOTO[can_id].getpara, hcan);
//				}break;
//				case LittleAmmo_2006_MOTO_ID://205
//					LittleAmmo_MOTO.getpara.GetEncoderMeasure(&LittleAmmo_MOTO.getpara, hcan);
//				break;

				
				case Ctrl_2006_MOTO_ID://203
					Ctrl_2006_MOTO.getpara.GetEncoderMeasure(&Ctrl_2006_MOTO.getpara, hcan);
				    break;
				case AMMO_2006_MOTO_ID://206
					Ammunition_MOTO.getpara.GetEncoderMeasure(&Ammunition_MOTO.getpara, hcan);
				    monitor_motor[0].circle_number=0;
				    break;				
				case YAW_6623_ID://208
					GetEncoderMeasure(&Yaw_MOTO.getpara, hcan);
					if(Yaw_MOTO.getpara.init_sta == 1){
						Tripod.Yaw.angle_source.encoder.angle_now=((Yaw_MOTO.getpara.angle)/8192.0f*360.0f)-336.0f;
						Tripod.Yaw.angle_source.encoder.angle_relativeToChassis=(Yaw_MOTO.getpara.total_angle-Yaw_MOTO.getpara.offset_angle)/8192.0f*360.0f;
					}
					
				break;
		
				
			}
		}
		else if(hcan->pRxMsg->IDE==CAN_ID_EXT){
			switch(hcan->pRxMsg->ExtId){			
					case CHASSIS1_ID://云台开发板接收底盘开发板的信息
					{
						monitor_tx2.circle_number=0;
						TX2_Data.distance=((float)hcan->pRxMsg->Data[0]);
						Strike.heat_42mm.margin=(int16_t)(hcan->pRxMsg->Data[1]<<8 | hcan->pRxMsg->Data[6])/10.0f;		
						TX2_Data.yaw_angle=(int16_t)(hcan->pRxMsg->Data[2]<<8 | hcan->pRxMsg->Data[3])/1000.0f;			
						TX2_Data.pitch_angle=(int16_t)(hcan->pRxMsg->Data[4]<<8 | hcan->pRxMsg->Data[5])/1000.0f;	
						Chassis.follow.mode.now = (hcan->pRxMsg->Data[6])>>4;
						RoboData.chassis_ctrl.yaw_speed_gyro = (int8_t)(hcan->pRxMsg->Data[7]);
						
					}break;

			}
		}
		__HAL_CAN_ENABLE_IT(&hcan2, CAN_IT_FMP0);//重新使能can中断标志
	}
}





