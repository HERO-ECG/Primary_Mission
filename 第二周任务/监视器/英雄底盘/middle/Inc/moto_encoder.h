#ifndef __ENCODER_H
#define __ENCODER_H

#include "mytype.h"
/*******************************************************************************************
  * @define
 *******************************************************************************************/
//enum{	
//	STDID_LOW = 0x200,
//	MOTO1_3508_ID = 0x201,//Friction_Wheel_MOTO
//	MOTO2_3508_ID = 0x202,//Friction_Wheel_MOTO
//	MOTO3_3508_ID = 0x203,
//	MOTO4_3508_2006_ID = 0x204,
//	
//	STDID_HIGH = 0x1FF,
//	YAW_6623_ID = 0x208,
//	PITCHL_6020_ID = 0x206,
//	PITCHR_6020_ID = 0x205,
//	AMMO_3508_ID = 0x205//随便给的
//};

//enum{
//	POWER_ID = 0x1314,
//	CLOUDDECK1_ID = 0x1315,
//	CLOUDDECK2_ID = 0x1316,
//	CHASSIS1_ID = 0x1317,
//	CHASSIS2_ID = 0x1318,
//	CHASSIS3_ID = 0x1319,
//	CHASSIS4_ID = 0x1320
//};

enum{	

	STDID_LOW = 0x200,
	MOTO1_3508_ID = 0x201,//Friction_Wheel_MOTO
	MOTO2_3508_ID = 0x202,//Friction_Wheel_MOTO
	MOTO3_3508_ID = 0x203,
	MOTO4_3508_ID = 0x204,

	STDID_HIGH = 0x1FF,
	YAW_6623_ID = 0x208,
	PITCHL_6020_ID = 0x206,
	PITCHR_6020_ID = 0x205,
	
	LittleAmmo_2006_MOTO_ID = 0x205,
	Ctrl_2006_MOTO_ID = 0x203,
	AMMO_3508_ID = 0x206,

};

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
	float LPF_current;
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
	float		get_power;	//通过电调读取的功率
	float		set_power;	//通过DJI电源管理模块读取的功率
}Encoder_t;

/*******************************************************************************************
  * @Function 
 *******************************************************************************************/
extern void EncoderParaInit(Encoder_t *ptr);
void GetEncoderMeasure(Encoder_t *ptr, CAN_HandleTypeDef* hcan);
#endif
