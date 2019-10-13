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
Moto_t TG_MOTO[2];
Moto_t Yaw_MOTO6020;
Moto_t Pitch_MOTO6020;
Moto_t Fricition_MOTO[2];
/*******************************************************************************************
  * @Func			SetMotoCurrent
  * @Brief    ͨ��CAN�������ݰ������Ƶ������
  * @Param		CAN_HandleTypeDef* hcan, s16 iq1, s16 iq2, s16 iq3, s16 iq4
  * @Retval		None
  * @Date     2018.10.10
  *	@Author		SZC			LZK��Ӻ�����ʶ���β�
 *******************************************************************************************/
void SetMotoCurrent(CAN_HandleTypeDef* hcan, uint32_t stdid, s16 iq1, s16 iq2, s16 iq3, s16 iq4){
	hcan->pTxMsg->StdId=stdid;
	hcan->pTxMsg->IDE=CAN_ID_STD;
	hcan->pTxMsg->RTR=CAN_RTR_DATA;
	hcan->pTxMsg->DLC=0x08;
	hcan->pTxMsg->Data[0]=iq1 >> 8;
	hcan->pTxMsg->Data[1]=iq1;
	hcan->pTxMsg->Data[2]=iq2 >> 8;
	hcan->pTxMsg->Data[3]=iq2;
	hcan->pTxMsg->Data[4]=iq3 >> 8;
	hcan->pTxMsg->Data[5]=iq3;
	hcan->pTxMsg->Data[6]=iq4 >> 8;
	hcan->pTxMsg->Data[7]=iq4;
	HAL_CAN_Transmit(hcan, 0);
}



