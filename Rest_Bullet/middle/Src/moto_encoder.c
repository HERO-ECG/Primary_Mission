/*******************************************************************************************
  * @Include @Headfile
 *******************************************************************************************/
#include "can.h"
#include "moto_encoder.h"

/*******************************************************************************************
  * @Parameter @Statement
 *******************************************************************************************/
//Encoder_t CMEncoder[4] = {&EncoderParaInit};
//Encoder_t TGEncoder[2] = {&EncoderParaInit};
#define ABS(X) ((X>0)? (X): (-X))

/*电机回传信息处理*/
void GetEncoderMeasure(Encoder_t *ptr, CAN_HandleTypeDef* hcan){
	if(ptr->init_sta == 0)
	{
		(ptr->init_cnt)++;
		if(ptr->init_cnt>50)
			ptr->init_sta = 1;
		else{
			ptr->offset_angle = (uint16_t)(hcan->pRxMsg->Data[0]<<8 | hcan->pRxMsg->Data[1]) ; /*得到初始角度值*/
			ptr->offset_round = (float)(ptr->offset_angle)/8192.0f;
		}
		ptr->last_angle = ptr->offset_angle;
		ptr->total_round = 0;
	}
	else{
		ptr->angle = (uint16_t)(hcan->pRxMsg->Data[0]<<8 | hcan->pRxMsg->Data[1]) ;      /*得到角度值*/
		ptr->real_current  = (int16_t)(hcan->pRxMsg->Data[2]<<8 | hcan->pRxMsg->Data[3]);			/*计算转子速度*/  //对于6623为扭矩电流
		ptr->speed_rpm = ptr->real_current;
		ptr->given_current = (int16_t)(hcan->pRxMsg->Data[4]<<8 | hcan->pRxMsg->Data[5]);/*计算实际转矩电流*/
		ptr->hall = hcan->pRxMsg->Data[6];                                               /*得到温度*/
		if(ptr->angle - ptr->last_angle > 4096)
			ptr->round_cnt --;
		else if (ptr->angle - ptr->last_angle < -4096)
			ptr->round_cnt ++;
		ptr->total_round = ptr->round_cnt + (float)(ptr->angle)/8192.0f - ptr->offset_round; 
		ptr->total_angle = ptr->round_cnt*8192 + ptr->angle;
		ptr->last_angle = ptr->angle;
  //	ptr->get_power = ABS(ptr->speed_rpm/500.0f * ptr->given_current/16384.0f);	/*粗略估计功率*/

		//ptr->get_power = ABS( ptr->given_current*0.00000394047917046875*ptr->speed_rpm);	/*粗略估计功率*/
		
		ptr->get_power = ptr->given_current*0.00000394047917046875f*ptr->speed_rpm;	/*粗略估计功率*/
		if(ptr->get_power<0)
			ptr->get_power=0;
	}

}


void EncoderParaInit(Encoder_t *ptr){
  ptr->ParaInit = &EncoderParaInit;
	ptr->GetEncoderMeasure = &GetEncoderMeasure;
	ptr->init_sta = 0;
	ptr->init_cnt = 0;
	ptr->round_cnt = 0;
}



