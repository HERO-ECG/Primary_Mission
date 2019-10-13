#ifndef __ENCODER_H
#define __ENCODER_H

#include "mytype.h"
/*******************************************************************************************
  * @define
 *******************************************************************************************/
#ifdef INFANTRY3
enum{	
	STDID_3508= 0x200,
	MOTO1_3508_ID= 0x201,
	MOTO2_3508_ID= 0x202,
	MOTO3_3508_ID= 0x203,
	MOTO4_3508_ID= 0x204,
	STDID_2006_6623= 0x1FF,
	Yaw6020_MOTO_ID=0x206,
	Pitch6020_MOTO_ID=0x208,
	MOTO1_2006_ID = 0x207,
	MOTO5_3508_ID= 0x208
	
};
#endif

#ifdef INFANTRY4
enum{	
	STDID_3508= 0x200,
	MOTO1_3508_ID= 0x201,
	MOTO2_3508_ID= 0x202,
	MOTO3_3508_ID= 0x203,
	MOTO4_3508_ID= 0x204,
	STDID_2006_6623= 0x1FF,
	Yaw6020_MOTO_ID=0x206,
	Pitch6020_MOTO_ID=0x208,
	MOTO1_2006_ID = 0x207,
	MOTO5_3508_ID= 0x208
	
};
#endif

#ifdef INFANTRY5
enum{	
	STDID_3508= 0x200,
	MOTO1_3508_ID= 0x201,
	MOTO2_3508_ID= 0x202,
	MOTO3_3508_ID= 0x203,
	MOTO4_3508_ID= 0x204,
	STDID_2006_6623= 0x1FF,
	Yaw6020_MOTO_ID=0x206,
	Pitch6020_MOTO_ID=0x208,
	MOTO1_2006_ID = 0x207,
	MOTO5_3508_ID= 0x208
	
};
#endif
enum{
	POWER_ID = 0x1314,
	CLOUDDECK1_ID = 0x1315,
	CLOUDDECK2_ID = 0x1316,
	CHASSIS1_ID = 0x1317,
	CHASSIS2_ID = 0x1318,
	CHASSIS3_ID = 0x1319,
	CHASSIS4_ID = 0x1320
};
/*******************************************************************************************
  * @Parameter
 *******************************************************************************************/
typedef struct __Encoder_t{
	void (*ParaInit)(struct __Encoder_t *ptr);
	void (*GetEncoderMeasure)(struct __Encoder_t *ptr, CAN_HandleTypeDef* hcan);
	float  	speed_rpm;
	float   real_current;
	float  	given_current;
	uint8_t  	hall;
	uint16_t 	angle;				/*abs angle range:[0,8191]*/
	uint16_t 	last_angle;				/*abs angle range:[0,8191]*/
	
	float   other_angle;
	float   other_speed;
	
	uint8_t init_sta;	
	uint8_t init_cnt;
	uint16_t	offset_angle;
	float		round_cnt;
	int32_t total_angle;
	float		total_round;
	float   offset_round;
	float		get_power;
	float		set_power;
}Encoder_t;

//extern  Encoder_t CMEncoder[4];
//extern  Encoder_t TGEncoder[1];

/*******************************************************************************************
  * @Function 
 *******************************************************************************************/
extern void EncoderParaInit(Encoder_t *ptr);

void GetEncoderMeasure(Encoder_t *ptr, CAN_HandleTypeDef* hcan);
#endif
