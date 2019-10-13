/*******************************************************************************************
  * @Include @Headfile
 *******************************************************************************************/
#include "can.h"
#include "moto_encoder.h"

/*******************************************************************************************
  * @Parameter @Statement
 *******************************************************************************************/
#define ABS(X) ((X>0)? (X): (-X))

void GetEncoderMeasure(Encoder_t *ptr, CAN_HandleTypeDef* hcan){
	if(ptr->init_sta == 0)
	{
		(ptr->init_cnt)++;
		if(ptr->init_cnt>50)
			ptr->init_sta = 1;
		/*�ϵ��ʼ�Ƕ�*/
		else{
			ptr->offset_angle = (uint16_t)(hcan->pRxMsg->Data[0]<<8 | hcan->pRxMsg->Data[1]); /*�õ���ʼ�Ƕ�ֵ*/
			ptr->offset_round = 0;
		}
		ptr->last_angle = ptr->offset_angle;
	}
	else
	{
		ptr->angle = (uint16_t)(hcan->pRxMsg->Data[0]<<8 | hcan->pRxMsg->Data[1]) ;      	/*�õ��Ƕ�ֵ*/
		ptr->real_current  = (int16_t)(hcan->pRxMsg->Data[2]<<8 | hcan->pRxMsg->Data[3]);	/*����ת���ٶ�*/  //����6623ΪŤ�ص���
		ptr->speed_rpm = ptr->real_current;
		ptr->given_current = (int16_t)(hcan->pRxMsg->Data[4]<<8 | hcan->pRxMsg->Data[5]);	/*����ʵ��ת�ص���*/
		ptr->hall = hcan->pRxMsg->Data[6];                                               	/*�õ��¶�*/
		if(ptr->angle - ptr->last_angle > 4096)
			ptr->round_cnt --;
		else if (ptr->angle - ptr->last_angle < -4096)
			ptr->round_cnt ++;
		ptr->total_angle = ptr->round_cnt*8192 + ptr->angle;
		ptr->total_round = ptr->total_angle/8192.0f; 
		ptr->last_angle = ptr->angle;
		ptr->LPF_current = 0.97f*ptr->LPF_current + 0.03f*ptr->given_current;
		
		/*������Ҫ�ó��ţ��ó˺Ŵ���  0.001220703125f = 20A/16384*/
		ptr->get_power = ABS( ptr->given_current*0.00000394047917046875f*ptr->speed_rpm);	/*���Թ��ƹ���*/
	}
}

void EncoderParaInit(Encoder_t *ptr){
  ptr->ParaInit = &EncoderParaInit;
	ptr->GetEncoderMeasure = &GetEncoderMeasure;
	ptr->init_sta = 0;
	ptr->init_cnt = 0;
	ptr->round_cnt = 0;
}
