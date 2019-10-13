#ifndef __MOTO_H
#define __MOTO_H


#include "pid.h"
#include "moto_encoder.h"
#include "mytype.h"
/*******************************************************************************************
  * @define
 *******************************************************************************************/




/*******************************************************************************************
  * @Parameter
 *******************************************************************************************/
//typedef struct __Set_t{
//	float 	beforelast;
//	float 	last;
//	float 	now;
//}Set_t;

typedef struct __Moto_t{
	void (*MotoParaInit)(struct __Moto_t *ptr);
	Encoder_t 	getpara;
	
	Pid_t 	pid_angle; 
	Pid_t 	pid_speed;  
	
	float   set_angle;
	float   set_angle_last;
  float   set_speed;
	float   set_speed_last;
	float   send_current;
}Moto_t;

extern Moto_t Chassis_MOTO[4];
extern Moto_t TG_MOTO[2];
extern Moto_t Yaw_MOTO;
extern Moto_t Pitch_MOTO;
extern Moto_t Fricition_MOTO;
extern Moto_t Chassis_power;
/*******************************************************************************************
  * @Function 
 *******************************************************************************************/
extern void SetMotoCurrent(CAN_HandleTypeDef* hcan, uint32_t stdid, s16 iq1, s16 iq2, s16 iq3, s16 iq4); 
extern void TX_DATA_Power(CAN_HandleTypeDef* hcan, uint32_t extid ,uint8_t toggle, uint8_t limit_power);
extern void MotoParaInit(Moto_t *ptr);
extern void Audo_bodan(void);
extern float yaw_speed;
extern float yaw_angle;
extern float tuoluo_offset_angle;
extern float tuoluo_total_angle ;
extern int tuoluo_cnt;
extern float pitch_dangshi_angle;
extern float yaw_dangshi_angle;
extern float bodan_zhuansu ;
extern float err_auto_sum;
#endif
