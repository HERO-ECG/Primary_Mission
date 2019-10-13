#ifndef __M_TX2_PROTOCOL_H
#define __M_TX2_PROTOCOL_H
#include "mytype.h"
#include "robodata.h"


#define RED

#ifdef RED
#define ROBOT_TX_ID	0x0001 //�췽
#define ROBOT_RX_ID	0x0101

#else
#define ROBOT_TX_ID	0x000B
#define ROBOT_RX_ID	0x0111 //����

#endif


typedef  struct TX2_Ctl_t
{
	void (*TX2DataParaInit)(struct TX2_Ctl_t *TX2_Data);
	float yaw_angle;//��Ϊ��
  float pitch_angle;//��Ϊ��
	uint8_t velocity;
	uint8_t distance;
	uint8_t sendMode;
}TX2_Data_t;  //����֡

extern TX2_Data_t TX2_Data;
extern uint8_t TX2_sendDataBuff[];
void TX2_sendData(uint8_t mode);
short TX2_DataProcess(uint8_t *pData, TX2_Data_t *TX2_Data,uint16_t length);
#endif
