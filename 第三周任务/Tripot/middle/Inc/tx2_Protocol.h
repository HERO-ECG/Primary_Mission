#ifndef __M_TX2_PROTOCOL_H
#define __M_TX2_PROTOCOL_H
#include "mytype.h"
#include "robodata.h"

typedef enum
{
	AUTOAIM_CLOSE = 0,
	WINDMILL_CLKWISE = 1,
	WINDMILL_ANTICLKWISE = 2,
	WINDMILL_STILL = 3,
	OFFLINE = 4,
	COMMU_FAILURE = 5,
	AUTOAIM_LONG = 6,
	WINDMILL_MANUAL = 7
}TXMode_e;

typedef  struct TX2_Ctl_t
{
	void (*TX2DataParaInit)(struct TX2_Ctl_t *TX2_Data);
	float yaw_angle;//右为正
  float pitch_angle;//上为正
	TXMode_e mode_now;
	TXMode_e mode_set;
	uint8_t distance;
	uint8_t detected;
}TX2_Data_t;  //数据帧

extern TX2_Data_t TX2_Data;
extern uint8_t TX2_Data_Send[7];
short TX2_DataProcess(uint8_t *pData, TX2_Data_t *TX2_Data,uint16_t length);
void TX2_SetMode(void);
#endif
