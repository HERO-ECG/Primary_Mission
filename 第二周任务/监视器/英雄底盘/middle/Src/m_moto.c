/*******************************************************************************************
  * @Include @Headfile
 *******************************************************************************************/
#include "can.h"
#include "m_moto.h"
#include "moto_encoder.h"
#include "pid.h"

/*******************************************************************************************
  * @Parameter @Statement
 *******************************************************************************************/
Moto_t Chassis_MOTO[4]; 
Moto_t Ammunition_MOTO;
Moto_t Pitch_MOTO[2];
Moto_t Yaw_MOTO;
Moto_t Friction_Wheel_MOTO[2];
Moto_t Ctrl_2006_MOTO;
Moto_t LittleAmmo_MOTO;

/*******************************************************************************************
  * @Func			SetMotoCurrent
  * @Brief    通过CAN发送数据包，控制电机电流
  * @Param		CAN_HandleTypeDef* hcan, s16 iq1, s16 iq2, s16 iq3, s16 iq4
  * @Retval		None
  * @Date     2018.10.10
  *	@Author		SZC			LZK添加函数标识符形参
 *******************************************************************************************/
void SetMotoCurrent(CAN_HandleTypeDef* hcan, uint32_t stdid, s16 iq1, s16 iq2, s16 iq3, s16 iq4){
	hcan->pTxMsg->StdId = stdid;
	hcan->pTxMsg->IDE = CAN_ID_STD;
	hcan->pTxMsg->RTR = CAN_RTR_DATA;
	hcan->pTxMsg->DLC = 0x08;
	hcan->pTxMsg->Data[0] = iq1 >> 8;
	hcan->pTxMsg->Data[1] = iq1;
	hcan->pTxMsg->Data[2] = iq2 >> 8;
	hcan->pTxMsg->Data[3] = iq2;
	hcan->pTxMsg->Data[4] = iq3 >> 8;
	hcan->pTxMsg->Data[5] = iq3;
	hcan->pTxMsg->Data[6] = iq4 >> 8;
	hcan->pTxMsg->Data[7] = iq4;
	HAL_CAN_Transmit(hcan, 3);
}

void MotoParaInit(Moto_t *ptr){
	ptr->MotoParaInit = &MotoParaInit;
	ptr->getpara.ParaInit = &EncoderParaInit;
	ptr->getpara.ParaInit(&(ptr->getpara));
	
	ptr->pid_angle.PidParaInit = &PidParaInit;
	ptr->pid_angle.PidParaInit(&(ptr->pid_angle));
	
	ptr->pid_speed.PidParaInit = &PidParaInit;
	ptr->pid_speed.PidParaInit(&(ptr->pid_speed));	
}
