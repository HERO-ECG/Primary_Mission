#ifndef _M_TRANSMIT_H
#define _M_TRANSMIT_H

#include "main.h"
#include "stm32f4xx_hal.h"

typedef	struct
{
	  uint8_t b1;
    uint8_t b2;
		uint8_t b3;
		uint8_t b4;
		uint8_t b5;
		uint8_t b6;
		uint8_t b7;
		uint8_t b8;
}AToBDataBit_t;

typedef	struct
{
	uint8_t val;
	AToBDataBit_t databit;
}BToAAToBDataBit_t;

extern BToAAToBDataBit_t BtoADataBit;

extern void TX_DATA_CHASSIS1_ID(CAN_HandleTypeDef* hcan);
extern void TX_DATA_CHASSIS2_ID(CAN_HandleTypeDef* hcan);
void TX_DATA_CHASSIS3_ID(CAN_HandleTypeDef* hcan);
void TX_DATA_POWER_ID(CAN_HandleTypeDef* hcan);

void GetBToADataVal(BToAAToBDataBit_t *BToADataBit);
void GetBToADataBit(BToAAToBDataBit_t *BToADataBit);
extern float testdata[8];

#endif 
