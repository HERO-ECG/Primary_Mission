#include "tx2_Protocol.h"
#include "dji_Protocol.h"
#include "m_transmit.h"
#include "tripod.h"
#include "strike.h"

extern uint8_t windcar_key;

float BToATest[8];
BToAAToBDataBit_t BtoADataBit;

void TX_DATA_CLOUDDECK1_ID(CAN_HandleTypeDef* hcan)
{
	//CAN_ID_STD
	//CAN_ID_EXT
	hcan->pTxMsg->IDE=CAN_ID_EXT;
	hcan->pTxMsg->ExtId=CLOUDDECK1_ID ;
	hcan->pTxMsg->RTR=CAN_RTR_DATA;
	hcan->pTxMsg->DLC=0x08;
	hcan->pTxMsg->Data [0]=(int16_t)((RoboData.chassis_ctrl.spd_forward)*100)>>8;  //B����յ�ң������ԭʼ����ֱ��ת��������A��
	hcan->pTxMsg->Data [1]=(int16_t)((RoboData.chassis_ctrl.spd_forward)*100);
	hcan->pTxMsg->Data [2]=(int16_t)((RoboData.chassis_ctrl.spd_right)*100)>>8;
	hcan->pTxMsg->Data [3]=(int16_t)((RoboData.chassis_ctrl.spd_right)*100);	
	hcan->pTxMsg->Data [4]=(int16_t)RoboData.robo_ctrlmode.ctrl_source;//���������������ң��������PC������
	hcan->pTxMsg->Data [5]=(int16_t)RoboData.robo_ctrlmode.left_mode;//�����˶�ģʽ��С���ݣ�������ģʽ����̨ת�����̷��򲻱䣩������ģʽ������ҡ��ģʽ
	hcan->pTxMsg->Data [6]=(int16_t)(RC_CtrlData.key.v>>8);//��������еİ���ֵ
	hcan->pTxMsg->Data [7]=(int16_t)(RC_CtrlData.key.v);

}

void TX_DATA_CLOUDDECK2_ID(CAN_HandleTypeDef* hcan)
{

	if(Strike.firemode.now==Auto_Mode)
	{
		BtoADataBit.databit.b1=1;
		BtoADataBit.databit.b2=0;
		BtoADataBit.databit.b3=0;
	}
	else if(Strike.firemode.now==Burst_Mode)
	{
		BtoADataBit.databit.b1=0;
		BtoADataBit.databit.b2=1;
		BtoADataBit.databit.b3=0;
	}
	else 
	{
		BtoADataBit.databit.b1=0;
		BtoADataBit.databit.b2=0;
		BtoADataBit.databit.b3=1;
	}
	if(Strike.cover_open==0)
		BtoADataBit.databit.b4=1;
	else
		BtoADataBit.databit.b4=0;
	if(Strike.friction_ctrl.shooting_range==Long_Range)
		BtoADataBit.databit.b5=1;
	else
		BtoADataBit.databit.b5=0;
	GetBToADataVal(&BtoADataBit);
	

	
	
	hcan->pTxMsg->IDE=CAN_ID_EXT;
	hcan->pTxMsg->ExtId=CLOUDDECK2_ID ;
	hcan->pTxMsg->RTR=CAN_RTR_DATA;
	hcan->pTxMsg->DLC=0x08;
	hcan->pTxMsg->Data [0]=TX2_Data.mode_set;
	hcan->pTxMsg->Data [1]=(int16_t)(BToATest[0]*10)>>8;
	hcan->pTxMsg->Data [2]=(int16_t)(BToATest[0]*10);
	hcan->pTxMsg->Data [3]=(int16_t)(BToATest[1]*10)>>8;
	hcan->pTxMsg->Data [4]=(int16_t)(BToATest[1]*10);
	hcan->pTxMsg->Data [6]=(int16_t)(BToATest[2]*10)>>8;
	hcan->pTxMsg->Data [7]=(int16_t)(BToATest[2]*10);
	hcan->pTxMsg->Data [5]=BtoADataBit.val;
}

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
