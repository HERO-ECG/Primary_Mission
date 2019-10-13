/*******************************************************************************************
  * @Include @Headfile
 *******************************************************************************************/
#include "can.h"
#include "moto_encoder.h"

/*******************************************************************************************
  * @Parameter @Statement
 *******************************************************************************************/
//Encoder_t CMEncoder[4]={&EncoderParaInit};
//Encoder_t TGEncoder[2]={&EncoderParaInit};


void GetEncoderMeasure(Encoder_t *ptr, CAN_HandleTypeDef* hcan){
	if(ptr->init_sta==0)
	{
		(ptr->init_cnt)++;
		if(ptr->init_cnt>50)
			ptr->init_sta=1;
		else{
			ptr->offset_angle=(uint16_t)(hcan->pRxMsg->Data[0]<<8 | hcan->pRxMsg->Data[1]) ; /*�õ���ʼ�Ƕ�ֵ*/
			ptr->offset_round=(float)(ptr->offset_angle)/8192.0f;
		}
		ptr->last_angle=ptr->offset_angle;
	}
	else{
		ptr->angle=(uint16_t)(hcan->pRxMsg->Data[0]<<8 | hcan->pRxMsg->Data[1]) ;      /*�õ��Ƕ�ֵ*/
		ptr->real_current=(int16_t)(hcan->pRxMsg->Data[2]<<8 | hcan->pRxMsg->Data[3]);			/*����ת���ٶ�*/  //����6623ΪŤ�ص���
		ptr->speed_rpm=ptr->real_current;
		ptr->given_current=(int16_t)(hcan->pRxMsg->Data[4]<<8 | hcan->pRxMsg->Data[5]);/*����ʵ��ת�ص���*/
		ptr->hall=hcan->pRxMsg->Data[6];                                               /*�õ��¶�*/
		if(ptr->angle - ptr->last_angle > 4096)
			ptr->round_cnt --;
		else if (ptr->angle - ptr->last_angle < -4096)
			ptr->round_cnt ++;
		ptr->total_round=ptr->round_cnt + (float)(ptr->angle)/8192.0f; 
		ptr->total_angle=ptr->round_cnt*8192 + ptr->angle;
		ptr->last_angle=ptr->angle;
	}
	

}


void EncoderParaInit(Encoder_t *ptr){
  ptr->ParaInit=&EncoderParaInit;
	ptr->GetEncoderMeasure=&GetEncoderMeasure;
	ptr->init_sta=0;
	ptr->init_cnt=0;
	ptr->round_cnt=0;
}


