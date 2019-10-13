#ifndef __HWT_901B_H
#define __HWT_901B_H
#include "stm32f4xx_hal.h"
#include "mytype.h"

//HWT-901 的角度输出
struct SAngle   //角度
{
	float Angle[3];
	float Angular_Velocity[3];
	float Acc[3];
	uint8_t SUM;
};  

extern struct SAngle ANGLE;

void HWT901DataProcess(uint8_t* pData,uint8_t length);

#endif
