
/*******************************************************************************************
  * @Include @Headfile
 *******************************************************************************************/
#include "moto_encoder.h"
#include "main.h"
#include "can.h"
#include "m_moto.h"
#include "m_remote.h"
#include "tripod.h"
#include "robodata.h"
#include "t_monitor.h"
#include "dji_Protocol.h"
#include "tx2_Protocol.h"
#include "strike.h"
#include "chassis.h"
/*******************************************************************************************
  * @Parameter @Statement
 *******************************************************************************************/

float correct_offset_angle(uint16_t offset_angle);
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
			case MOTO1_3508_ID:
			case MOTO2_3508_ID:
			case MOTO3_3508_ID:
			case MOTO4_3508_ID:{
				uint32_t can_id = 0;
				can_id = hcan->pRxMsg->StdId - MOTO1_3508_ID;
				Chassis_MOTO[can_id].getpara.GetEncoderMeasure(&Chassis_MOTO[can_id].getpara, hcan);
			}break;
			case LittleAmmo_2006_MOTO_ID://205
				LittleAmmo_MOTO.getpara.GetEncoderMeasure(&LittleAmmo_MOTO.getpara, hcan);
			break;
		
			}
		}
		else if(hcan->pRxMsg->IDE==CAN_ID_EXT){
			switch(hcan->pRxMsg->ExtId){		
				case POWER_ID://底盘开发板接收功率板的信息
					{
						HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_RESET);		//亮
						monitor_can_power.circle_number = 0;
						Chassis.supercapacitor.sup_c_open=1;
						Chassis.supercapacitor.sup_c_ready=1;
						Chassis.supercapacitor.Voltage_realtime=(int16_t)(hcan->pRxMsg->Data[0]<<8 | hcan->pRxMsg->Data[1])/100.0f;
					}break;
				}
			}
		__HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0);//重新使能can中断标志
	}
	else if(hcan == &hcan2){
		if(hcan->pRxMsg->IDE==CAN_ID_STD){		
			switch(hcan->pRxMsg->StdId){
				case YAW_6623_ID://208
					GetEncoderMeasure(&Yaw_MOTO.getpara, hcan);
					if(Yaw_MOTO.getpara.init_sta == 1){
					//	Tripod.Yaw.angle_source.encoder.angle_now=(Yaw_MOTO.getpara.total_angle)/8192.0f*360.0f;//58/46
						Tripod.Yaw.angle_source.encoder.angle_relativeToChassis=(Yaw_MOTO.getpara.total_angle)/8192.0f*360.0f;
					}	
				break;
			
			}
		}
		else if(hcan->pRxMsg->IDE==CAN_ID_EXT){
			switch(hcan->pRxMsg->ExtId){			
				case CLOUDDECK1_ID://底盘开发板接收云台开发板的信息
				{
					monitor_remote.circle_number=0;
					RoboData.chassis_ctrl.spd_forward = (int16_t)(hcan->pRxMsg->Data[0]<<8 | hcan->pRxMsg->Data[1])/100.0f;
					RoboData.chassis_ctrl.spd_right = (int16_t)(hcan->pRxMsg->Data[2]<<8 | hcan->pRxMsg->Data[3])/100.0f;
					RoboData.robo_ctrlmode.left_mode = (hcan->pRxMsg->Data[4]>>6)&0x03;
					RoboData.robo_ctrlmode.ctrl_source= (hcan->pRxMsg->Data[4]>>4)&0x03;
					Strike.count_42mm.out_firecounter = hcan->pRxMsg->Data[4]&0x0F;
					gpio_exti_0 = (hcan->pRxMsg->Data[5]>>1);
					Strike.heat_42mm.unlimited=(hcan->pRxMsg->Data[5])&0x01;
					RC_CtrlData.key.v = (int16_t)(hcan->pRxMsg->Data[6]<<8 | hcan->pRxMsg->Data[7]);
				}break;
				case CLOUDDECK2_ID://底盘开发板接收云台开发板的信息
				{
					Tripod.Pitch.angle=(int16_t)(hcan->pRxMsg->Data[0]<<8 | hcan->pRxMsg->Data[1])/100.0f;
					Tripod.Pitch.angle_source.encoder.angle_relativeToChassis=(int16_t)(hcan->pRxMsg->Data[1]<<8 | hcan->pRxMsg->Data[2])/100.0f;
				}break;
					case POWER_ID://底盘开发板接收功率板的信息
					{
						HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_RESET);		//亮
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








