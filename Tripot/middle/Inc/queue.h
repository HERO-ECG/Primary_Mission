#ifndef __QUEUE_H
#define __QUEUE_H

#include "math.h"
#include "stm32f1xx_hal.h"

#define MAX_LENGTH	100
typedef struct 
{
	uint16_t length;
	uint16_t pointer;
	float data[MAX_LENGTH];
}Queue_t;

#endif
