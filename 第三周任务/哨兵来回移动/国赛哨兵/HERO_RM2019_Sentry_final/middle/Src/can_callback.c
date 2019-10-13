/*******************************************************************************************
  * @Include @Headfile
 *******************************************************************************************/
#include "main.h"
#include "can.h"
#include "m_moto.h"
#include "moto_encoder.h"
#include "m_remote.h"
#include "robodata.h"
#include "t_monitor.h"
#include "dji_Protocol.h"
#include "tx2_Protocol.h"
#include "pitch.h"
#include "auto_move.h"
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
		switch(hcan->pRxMsg->StdId){

			case MOTO1_3508_ID:
			case MOTO2_3508_ID:
			case MOTO3_3508_ID:
			case MOTO4_3508_ID:{
				uint32_t can_id = 0;
				can_id = hcan->pRxMsg->StdId - MOTO1_3508_ID;
				Chassis_MOTO[can_id].getpara.GetEncoderMeasure(&Chassis_MOTO[can_id].getpara, hcan);
			}break;
		
			case MOTO1_6620Yaw_ID:
				Yaw_MOTO.getpara.GetEncoderMeasure(&Yaw_MOTO.getpara, hcan);
				
			break;
			case MOTO2_6623Pitch_ID:
				Pitch_MOTO.getpara.GetEncoderMeasure(&Pitch_MOTO.getpara, hcan);
							if(Pitch_MOTO.getpara.total_angle>4096.0f)Pitch_MOTO.getpara.total_angle=Pitch_MOTO.getpara.total_angle-8192.0f;
			  //last_angle=Pitch.angle;
			  //Pitch.angle=0.8f*Pitch_MOTO.getpara.total_angle+0.2f*last_angle;
			Pitch.angle=Pitch_MOTO.getpara.total_angle;

			break;
			case MOTO1_2006_ID:
				TG_MOTO[0].getpara.GetEncoderMeasure(&TG_MOTO[0].getpara, hcan);
				overall_mode.mocalun_state.Mocalun_Connect_TO_2006_count=0;
			break;
			case MOTO2_2006_ID:
				TG_MOTO[1].getpara.GetEncoderMeasure(&TG_MOTO[1].getpara, hcan);
			break;
	
		}
		switch(hcan->pRxMsg->ExtId){
			#ifdef Chassis
				case CLOUDDECK1_ID://底盘开发板接收云台开发板的信息
					monitor_remote.circle_number = 0;
					RoboData.chassis_ctrl.spd_forward = (int16_t)(hcan->pRxMsg->Data[0]<<8 | hcan->pRxMsg->Data[1])/100.0f;
					RoboData.chassis_ctrl.spd_right = (int16_t)(hcan->pRxMsg->Data[2]<<8 | hcan->pRxMsg->Data[3])/100.0f;
					RoboData.chassis_ctrl.spd_yaw_w = (int16_t)(hcan->pRxMsg->Data[4]<<8 | hcan->pRxMsg->Data[5])/100.0f;
					RoboData.chassis_ctrl.angle_yaw_w = (int16_t)(hcan->pRxMsg->Data[6]<<8 | hcan->pRxMsg->Data[7])/100.0f;
				break;
				case CLOUDDECK2_ID://底盘开发板接收云台开发板的信息
					monitor_remote.circle_number = 0;
					RoboData.robo_ctrlmode.ctrl_source = hcan->pRxMsg->Data[0];
					RoboData.robo_ctrlmode.left_mode = hcan->pRxMsg->Data[1];
				break;
				#ifdef ChassisPowerControl
					case POWER_ID://底盘开发板接收功率板的信息
						
					break;
				#endif
			#else
				case CHASSIS1_ID://云台开发板接收底盘开发板的信息
					DJ_ReadData.Data.extGameRobotState.remainHP = (uint16_t)(hcan->pRxMsg->Data[0]<<8 | hcan->pRxMsg->Data[1]);
					DJ_ReadData.Data.extGameRobotState.maxHP = (uint16_t)(hcan->pRxMsg->Data[2]<<8 | hcan->pRxMsg->Data[3]);
				//云台开发板接不到裁判系统的实际热量则全靠本地的基本热量控制
					RoboData.shoot_heat_ctr.nowHeat17 = (uint16_t)(hcan->pRxMsg->Data[4]<<8 | hcan->pRxMsg->Data[5]);
					RoboData.shoot_heat_ctr.nowHeat42 = (uint16_t)(hcan->pRxMsg->Data[6]<<8 | hcan->pRxMsg->Data[7]);
				break;
				case CHASSIS2_ID://云台开发板接收底盘开发板的信息
//					monitor_tx2.circle_number = 0;
//					tx2_ReadData.delta_x = (int16_t)(hcan->pRxMsg->Data[0]<<8 | hcan->pRxMsg->Data[1])/100.0f;
//					tx2_ReadData.delta_y = (int16_t)(hcan->pRxMsg->Data[2]<<8 | hcan->pRxMsg->Data[3])/100.0f;
//					tx2_ReadData.delta_z = (int16_t)(hcan->pRxMsg->Data[4]<<8 | hcan->pRxMsg->Data[5])/100.0f;
//					tx2_ReadData.sta = (hcan->pRxMsg->Data[6]);
//				//位置不够啦。。。
//					DJ_ReadData.Data.extGameRobotState.robotLevel = (hcan->pRxMsg->Data[7]);
				break;
				
				/*
				//遥控器接收端放底盘时
				case CHASSIS3_ID://云台开发板接收底盘开发板的信息
					monitor_remote.circle_number = 0;
					RC_CtrlData.rc.s1 = ((hcan->pRxMsg->Data[5]>>4)&0x000c)>>2;
					RC_CtrlData.rc.s2 = ((hcan->pRxMsg->Data[5]>>4)&0x0003);
					RC_CtrlData.rc.ch0 = ((int16_t)hcan->pRxMsg->Data[0]|((int16_t)hcan->pRxMsg->Data[1]<<8))&0x07FF;
					RC_CtrlData.rc.ch1 = (((int16_t)hcan->pRxMsg->Data[1]>>3)|((int16_t)hcan->pRxMsg->Data[2]<<5))&0x07FF;
					RC_CtrlData.rc.ch2 = (((int16_t)hcan->pRxMsg->Data[2]>>6)|((int16_t)hcan->pRxMsg->Data[3]<<2)|((int16_t)hcan->pRxMsg->Data[4]<<10))&0x07FF;
					RC_CtrlData.rc.ch3 = (((int16_t)hcan->pRxMsg->Data[4]>>1)|((int16_t)hcan->pRxMsg->Data[5]<<7))&0x07FF; 
					RC_CtrlData.key.v = ((int16_t)hcan->pRxMsg->Data[6])|((int16_t)hcan->pRxMsg->Data[7]<<8); 
					RCReadKey(&RC_CtrlData);
				break;
				case CHASSIS4_ID://云台开发板接收底盘开发板的信息
					RC_CtrlData.mouse.x_speed = ((int16_t)hcan->pRxMsg->Data[0])|((int16_t)hcan->pRxMsg->Data[1]<<8);
					RC_CtrlData.mouse.y_speed = ((int16_t)hcan->pRxMsg->Data[2])|((int16_t)hcan->pRxMsg->Data[3]<<8);
					RC_CtrlData.mouse.z_speed = ((int16_t)hcan->pRxMsg->Data[4])|((int16_t)hcan->pRxMsg->Data[5]<<8);
					RC_CtrlData.mouse.press_1 = hcan->pRxMsg->Data[6];
					RC_CtrlData.mouse.press_2 = hcan->pRxMsg->Data[7];
				break;
				*/
				
			#endif
		}
	}
	__HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0);//重新使能can中断标志
}
