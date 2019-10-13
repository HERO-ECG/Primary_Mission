#include "tx2_Protocol.h"
#include "dji_Protocol.h"
#include "m_transmit.h"
#include "t_monitor.h"
#include "chassis.h"

/***底盘开发板给云台开发板发送信息***************
***扩展帧数据***
*/

BToAAToBDataBit_t BtoADataBit;
BToAAToBDataBit_t AtoBDataBit;
float bullet_speed;
void TX_DATA_CHASSIS1_ID(CAN_HandleTypeDef* hcan)//裁判系统数据读取
{
	hcan->pTxMsg->IDE = CAN_ID_EXT;
	hcan->pTxMsg->ExtId = CHASSIS1_ID ;
	hcan->pTxMsg->RTR = CAN_RTR_DATA;
	hcan->pTxMsg->DLC = 0x08;
	

	hcan->pTxMsg->Data [0] = (int16_t)(((int16_t)(DJI_ReadData.ext_power_heat_data.shooter_heat0))>>8);
	hcan->pTxMsg->Data [1] = (int16_t)(((int16_t)(DJI_ReadData.ext_power_heat_data.shooter_heat0)));
	{//弃用:根据裁判系统热量计算发弹
	#define one_ammo_heat 16
	Chassis.heat.heat_server[1] = Chassis.heat.heat_server[0];
	Chassis.heat.heat_server[0] = DJI_ReadData.ext_power_heat_data.shooter_heat0;	
	if((Chassis.heat.heat_server[0]-Chassis.heat.heat_server[1]) > 1)
//		Chassis.heat.fireammo_counter = (Chassis.heat.heat_server[0]-Chassis.heat.heat_server[1])/one_ammo_heat;
//	else
//		Chassis.heat.fireammo_counter = 0;
	Chassis.heat.client_ammo_left --;//= Chassis.heat.fireammo_counter;
	}
	hcan->pTxMsg->Data [2] = 	(((int16_t)(	TX2_Data.yaw_angle*1000))>>8);
	hcan->pTxMsg->Data [3] = 	(((int16_t)(	TX2_Data.yaw_angle*1000)));
	hcan->pTxMsg->Data [4] = 	(((int16_t)(	TX2_Data.pitch_angle*1000))>>8);
	hcan->pTxMsg->Data [5] = 	(((int16_t)(	TX2_Data.pitch_angle*1000)));	
	hcan->pTxMsg->Data [6] = 	(int16_t)(TX2_Data.mode_now);
	hcan->pTxMsg->Data [7] = 	(int16_t)(TX2_Data.velocity);
}


//数据：
void TX_DATA_CHASSIS2_ID(CAN_HandleTypeDef* hcan)
{
	float gyro_data;
	if(RoboData.chassis_ctrl.pitch_speed_gyro<1.5f&&Chassis.speed.spd_yaw_actual==0)
		gyro_data=0;
	else
		gyro_data=10;
	hcan->pTxMsg->IDE = CAN_ID_EXT;
	hcan->pTxMsg->ExtId = CHASSIS2_ID ;
	hcan->pTxMsg->RTR = CAN_RTR_DATA;
	hcan->pTxMsg->DLC = 0x08;
	hcan->pTxMsg->Data [0] = (int16_t)(((int16_t)(gyro_data*100))>>8);
	hcan->pTxMsg->Data [1] = (int16_t)(((int16_t)(gyro_data*100)));
	hcan->pTxMsg->Data [2] = (int16_t)(((int16_t)RoboData.chassis_ctrl.yaw_speed_gyro*100)>>8);
	hcan->pTxMsg->Data [3] = (int16_t)(((int16_t)RoboData.chassis_ctrl.yaw_speed_gyro*100));
	hcan->pTxMsg->Data [4] = DJI_ReadData.ext_game_robot_state.shooter_heat0_cooling_rate;
	hcan->pTxMsg->Data [5] = (int16_t)(((int16_t)(DJI_ReadData.ext_game_robot_state.shooter_heat0_cooling_limit))>>8);
	hcan->pTxMsg->Data [6] = (int16_t)(((int16_t)(DJI_ReadData.ext_game_robot_state.shooter_heat0_cooling_limit)));
	hcan->pTxMsg->Data [7] = DJI_ReadData.ext_game_robot_state.robot_level;
}

void TX_DATA_CHASSIS3_ID(CAN_HandleTypeDef* hcan)
{
	hcan->pTxMsg->IDE = CAN_ID_EXT;
	hcan->pTxMsg->ExtId = CHASSIS3_ID ;
	hcan->pTxMsg->RTR = CAN_RTR_DATA;
	hcan->pTxMsg->DLC = 0x08;
//	hcan->pTxMsg->Data [0] = (int16_t)(((int16_t)(RoboData.chassis_ctrl.pitch_speed_gyro*100))>>8);
//	hcan->pTxMsg->Data [1] = (int16_t)(((int16_t)(RoboData.chassis_ctrl.pitch_speed_gyro*100)));
//	hcan->pTxMsg->Data [2] = (int16_t)(((int16_t)RoboData.chassis_ctrl.yaw_speed_gyro)>>8);
//	hcan->pTxMsg->Data [3] = (int16_t)(((int16_t)RoboData.chassis_ctrl.yaw_speed_gyro));

}


//void TX_DATA_POWER_ID(CAN_HandleTypeDef* hcan)
//{
//	hcan->pTxMsg->IDE = CAN_ID_EXT;
//	hcan->pTxMsg->ExtId = POWER_ID ;
//	hcan->pTxMsg->RTR = CAN_RTR_DATA;
//	hcan->pTxMsg->DLC = 0x08;
//	hcan->pTxMsg->Data [1] = 0x01; 
//	hcan->pTxMsg->Data [0] = (int16_t)(((int16_t)(RoboData.chassis_ctrl.pitch_speed_gyro*100))>>8);
////	hcan->pTxMsg->Data [1] = (int16_t)(((int16_t)(RoboData.chassis_ctrl.pitch_speed_gyro*100)));
////	hcan->pTxMsg->Data [2] = (int16_t)(((int16_t)RoboData.chassis_ctrl.yaw_speed_gyro)>>8);
////	hcan->pTxMsg->Data [3] = (int16_t)(((int16_t)RoboData.chassis_ctrl.yaw_speed_gyro));

//}

#ifdef ChassisPowerControl
/***底盘开发板给超级电容功率控制板发送信息***************
***扩展帧数据***
*/
void TX_DATA_POWER_ID(CAN_HandleTypeDef* hcan,uint8_t toggle, float limit_power)
{
	hcan->pTxMsg->IDE = CAN_ID_EXT;
	hcan->pTxMsg->ExtId = POWER_ID;
	hcan->pTxMsg->RTR = CAN_RTR_DATA;
	hcan->pTxMsg->DLC = 0x03;
	hcan->pTxMsg->Data [0] = toggle;//充电1--40W;0--80W;
	hcan->pTxMsg->Data [1] = (int16_t)limit_power;	
	hcan->pTxMsg->Data [2] = (limit_power-hcan->pTxMsg->Data[1])*100;
}


#else
/***云台开发板给底盘开发板发送信息***************
***扩展帧数据***
*/
void TX_DATA_CLOUDDECK1_ID(CAN_HandleTypeDef* hcan)
{
	//CAN_ID_STD
	//CAN_ID_EXT
	hcan->pTxMsg->IDE = CAN_ID_EXT;
	hcan->pTxMsg->ExtId = CLOUDDECK1_ID ;
	hcan->pTxMsg->RTR = CAN_RTR_DATA;
	hcan->pTxMsg->DLC = 0x08;
	hcan->pTxMsg->Data [0] = (int16_t)(RoboData.chassis_ctrl.spd_forward*100)>>8;
	hcan->pTxMsg->Data [1] = (int16_t)(RoboData.chassis_ctrl.spd_forward*100);
	hcan->pTxMsg->Data [2] = (int16_t)(RoboData.chassis_ctrl.spd_right*100)>>8;
	hcan->pTxMsg->Data [3] = (int16_t)(RoboData.chassis_ctrl.spd_right*100);	
	hcan->pTxMsg->Data [4] = (int16_t)(RoboData.chassis_ctrl.spd_yaw_w*100)>>8;
	hcan->pTxMsg->Data [5] = (int16_t)(RoboData.chassis_ctrl.spd_yaw_w*100);
	#ifdef Yaw6623
		hcan->pTxMsg->Data [6] = (int16_t)(RoboData.chassis_ctrl.angle_yaw_w*100)>>8;
		hcan->pTxMsg->Data [7] = (int16_t)(RoboData.chassis_ctrl.angle_yaw_w*100);
	#endif

}

void TX_DATA_CLOUDDECK2_ID(CAN_HandleTypeDef* hcan)
{
	//CAN_ID_STD
	//CAN_ID_EXT
	//StdId
	hcan->pTxMsg->IDE = CAN_ID_EXT;
	hcan->pTxMsg->ExtId = CLOUDDECK2_ID ;
	hcan->pTxMsg->RTR = CAN_RTR_DATA;
	hcan->pTxMsg->DLC = 0x04;
	hcan->pTxMsg->Data [0] = (int16_t)RoboData.robo_ctrlmode.ctrl_source;
	hcan->pTxMsg->Data [1] = (int16_t)RoboData.robo_ctrlmode.left_mode;
	hcan->pTxMsg->Data [2] = (int16_t)(RC_CtrlData.key.v>>8);
	hcan->pTxMsg->Data [3] = (int16_t)(RC_CtrlData.key.v);
}




#endif



void GetBToADataVal(BToAAToBDataBit_t *BToADataBit)
{
	uint8_t databit=0x00;
	if(BToADataBit->databit.b1==1)
		databit|=0x01;
	if(BToADataBit->databit.b2==1)
		databit|=0x02;
	if(BToADataBit->databit.b3==1)
		databit|=0x04;
	if(BToADataBit->databit.b4==1)
		databit|=0x08;
	if(BToADataBit->databit.b5==1)
		databit|=0x10;
	if(BToADataBit->databit.b6==1)
		databit|=0x20;	
	if(BToADataBit->databit.b7==1)
		databit|=0x40;
	if(BToADataBit->databit.b8==1)
		databit|=0x80;	
	BToADataBit->val=databit;
}

void GetBToADataBit(BToAAToBDataBit_t *BToADataBit)
{
	BToADataBit->databit.b1=(BToADataBit->val&0x0001)==0x0001;
	BToADataBit->databit.b2=(BToADataBit->val&0x0002)==0x0002;
	BToADataBit->databit.b3=(BToADataBit->val&0x0004)==0x0004;
	BToADataBit->databit.b4=(BToADataBit->val&0x0008)==0x0008;
	BToADataBit->databit.b5=(BToADataBit->val&0x0010)==0x0010;
	BToADataBit->databit.b6=(BToADataBit->val&0x0020)==0x0020;
	BToADataBit->databit.b7=(BToADataBit->val&0x0040)==0x0040;
	BToADataBit->databit.b8=(BToADataBit->val&0x0080)==0x0080;

}

