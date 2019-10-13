/*******************************************************************************************
  * @Include @Headfile
 *******************************************************************************************/
#include "can.h"
#include "para.h"
#include "m_moto.h"
#include "moto_encoder.h"
#include "pid.h"

/*******************************************************************************************
  * @Parameter @Statement
 *******************************************************************************************/
Moto_t Chassis_MOTO[4]; 
Moto_t TG_MOTO[2];
Moto_t Yaw_MOTO;
Moto_t Pitch_MOTO;
Moto_t Fricition_MOTO;
Moto_t Chassis_power;
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
	HAL_CAN_Transmit(hcan, 10);
}

/*-------------------------------超级电容功率控制板信号发送-----------------------------------------*/
void TX_DATA_Power(CAN_HandleTypeDef* hcan, uint32_t extid ,uint8_t toggle, uint8_t limit_power)
{
	hcan->pTxMsg->ExtId = extid;
	hcan->pTxMsg->IDE = CAN_ID_EXT;
	hcan->pTxMsg->RTR = CAN_RTR_DATA;
	hcan->pTxMsg->DLC = 0x03;
	hcan->pTxMsg->Data [0] = toggle;		//1--40W;0--80W;
	hcan->pTxMsg->Data [1] = limit_power;	
	hcan->pTxMsg->Data [2] = 0;
	HAL_CAN_Transmit(hcan, 10);
}


void MotoParaInit(Moto_t *ptr)
{
	ptr->MotoParaInit = &MotoParaInit;
	ptr->getpara.ParaInit = &EncoderParaInit;
	ptr->getpara.ParaInit(&(ptr->getpara));
	
	ptr->pid_angle.PidParaInit = &PidParaInit;
	ptr->pid_angle.PidParaInit(&(ptr->pid_angle));
	
	ptr->pid_speed.PidParaInit = &PidParaInit;
	ptr->pid_speed.PidParaInit(&(ptr->pid_speed));	
	
}


