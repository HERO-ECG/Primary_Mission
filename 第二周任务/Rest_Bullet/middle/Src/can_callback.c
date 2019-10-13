/*******************************************************************************************
  * @Include @Headfile
 *******************************************************************************************/
#include "main.h"
#include "can.h"
#include "t_moto.h"
#include "m_remote.h"
#include "robodata.h"
#include "t_monitor.h"
#include "dji_Protocol.h"
#include "tx2_Protocol.h"
#include "chassis.h"
//#include "TW_servo.h"
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

float testdata[8];
 
 
uint16_t allow17num = 0;
uint8_t TX2Mode;
uint32_t powernum;
float pitch;
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan){
	if(hcan == &hcan1){	
		if(hcan->pRxMsg->IDE == CAN_ID_STD){
			switch(hcan->pRxMsg->StdId){
					case MOTO1_3508_ID:
					case MOTO2_3508_ID:
					case MOTO3_3508_ID:
					case MOTO4_3508_ID:{
						uint32_t can_id = 0;
						can_id = hcan->pRxMsg->StdId - MOTO1_3508_ID;
						Chassis_MOTO[can_id].getpara.GetEncoderMeasure(&Chassis_MOTO[can_id].getpara, hcan);
					}break;
			case Pitch6020_MOTO_ID:
					GetEncoderMeasure(&Pitch_MOTO6020.getpara, hcan);	
					TX2_Data.now_pitch_tx=Pitch_MOTO6020.getpara.angle/8192.0f*360.0f - PITCH_OFFSET;
				break;
					case Yaw6020_MOTO_ID:
						GetEncoderMeasure(&Yaw_MOTO6020.getpara, hcan);
					break;
					
			}
		}
		else	if(hcan->pRxMsg->IDE == CAN_ID_EXT){
			switch(hcan->pRxMsg->ExtId){
					

					case CLOUDDECK1_ID://底盘开发板接收云台开发板的信息
					{
						static Key_Data_t key_last;
						
						monitor_remote.circle_number = 0;
						
						if(fabs((int16_t)(hcan->pRxMsg->Data[0]<<8 | hcan->pRxMsg->Data[1])/100.0f)<=1)
							Chassis.speed.spd_forward = (int16_t)(hcan->pRxMsg->Data[0]<<8 | hcan->pRxMsg->Data[1])/100.0f;
						else
							break;
						if(fabs((int16_t)(hcan->pRxMsg->Data[2]<<8 | hcan->pRxMsg->Data[3])/100.0f)<=1)
							Chassis.speed.spd_right = (int16_t)(hcan->pRxMsg->Data[2]<<8 | hcan->pRxMsg->Data[3])/100.0f;
						else
							break;
						
						RoboData.robo_ctrlmode.ctrl_source = (hcan->pRxMsg->Data[4]);
						RoboData.robo_ctrlmode.left_mode = (hcan->pRxMsg->Data[5]);
						RC_CtrlData.key.v = (uint16_t)(hcan->pRxMsg->Data[6]<<8 | hcan->pRxMsg->Data[7]);
						RCReadKey(&RC_CtrlData);//获取按键数据
						if(RC_CtrlData.key.key_data.Z==1&&key_last.Z==0)
						{
							if(RC_CtrlData.key.key_data.shift==1)
								Chassis.heat.client_ammo_left -= 50;
							else
								Chassis.heat.client_ammo_left += 50;
						}
						if(RC_CtrlData.key.key_data.X==1&&key_last.X==0)
						{
							if(RC_CtrlData.key.key_data.shift==1)
								Chassis.heat.client_ammo_left -= 20;
							else
								Chassis.heat.client_ammo_left += 20;
						}
							
							
						key_last=RC_CtrlData.key.key_data;
					}break;
					case CLOUDDECK2_ID://底盘开发板接收云台开发板的信息
					{	
						monitor_remote.circle_number = 0;
						TX2_Data.mode_set=hcan->pRxMsg->Data[0];
						testdata[0]=(int16_t)(hcan->pRxMsg->Data[1]<<8 | hcan->pRxMsg->Data[2])/10.0f;
						testdata[1]=(int16_t)(hcan->pRxMsg->Data[3]<<8 | hcan->pRxMsg->Data[4])/10.0f;
						testdata[2]=(int16_t)(hcan->pRxMsg->Data[6]<<8 | hcan->pRxMsg->Data[7])/10.0f;
						
						//testdata[3]=(int16_t)(hcan->pRxMsg->Data[8]<<8 | hcan->pRxMsg->Data[9])/10.0f;
						
						BtoADataBit.val=hcan->pRxMsg->Data[5];
						GetBToADataBit(&BtoADataBit);
						
					}break;
					case POWER_ID://底盘开发板接收功率板的信息
					{
						monitor_can_power.circle_number = 0;
						Chassis.supercapacitor.sup_c_open=1;
						Chassis.supercapacitor.sup_c_ready=1;
						Chassis.supercapacitor.Voltage_realtime=(int16_t)(hcan->pRxMsg->Data[0]<<8 | hcan->pRxMsg->Data[1])/100.0f;
					}break;
			}
		}
			__HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0);//重新使能can中断标志
	}
	else 
		if(hcan == &hcan2)
	{
		if(hcan->pRxMsg->IDE == CAN_ID_STD){
			uint32_t can_id = 0;
			switch(hcan->pRxMsg->StdId){
					case MOTO1_3508_ID:
						can_id = hcan->pRxMsg->StdId - MOTO1_3508_ID;
						Chassis_MOTO[can_id].getpara.GetEncoderMeasure(&Chassis_MOTO[can_id].getpara, hcan);
						monitor_motor[0].circle_number=0;
						break;			
					case MOTO2_3508_ID:
						can_id = hcan->pRxMsg->StdId - MOTO1_3508_ID;
						Chassis_MOTO[can_id].getpara.GetEncoderMeasure(&Chassis_MOTO[can_id].getpara, hcan);
						monitor_motor[1].circle_number=0;
						break;
					case MOTO3_3508_ID:
						can_id = hcan->pRxMsg->StdId - MOTO1_3508_ID;
						Chassis_MOTO[can_id].getpara.GetEncoderMeasure(&Chassis_MOTO[can_id].getpara, hcan);
						monitor_motor[2].circle_number=0;
						break;			
					case MOTO4_3508_ID:				
						can_id = hcan->pRxMsg->StdId - MOTO1_3508_ID;
						Chassis_MOTO[can_id].getpara.GetEncoderMeasure(&Chassis_MOTO[can_id].getpara, hcan);
						monitor_motor[3].circle_number=0;
						break;
			}
		}
		else	if(hcan->pRxMsg->IDE == CAN_ID_EXT){
			switch(hcan->pRxMsg->ExtId){
					case POWER_ID://底盘开发板接收功率板的信息
					{
						monitor_can_power.circle_number = 0;
						Chassis.supercapacitor.sup_c_open=1;
						Chassis.supercapacitor.sup_c_ready=1;
						Chassis.supercapacitor.Voltage_realtime=(int16_t)(hcan->pRxMsg->Data[0]<<8 | hcan->pRxMsg->Data[1])/100.0f;
					}break;
			}
		}
			__HAL_CAN_ENABLE_IT(&hcan2, CAN_IT_FMP0);//重新使能can中断标志
	}

}


