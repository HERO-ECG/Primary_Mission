#ifndef _M_TRANSMIT_H
#define _M_TRANSMIT_H

#include "main.h"
#include "stm32f1xx_hal.h"


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
extern float BToATest[8];
#ifdef Chassis
extern void TX_DATA_CHASSIS1_ID(CAN_HandleTypeDef* hcan);
extern void TX_DATA_CHASSIS2_ID(CAN_HandleTypeDef* hcan);
extern void TX_DATA_POWER_ID(CAN_HandleTypeDef* hcan, uint8_t toggle, float limit_power);
#else
extern void TX_DATA_CLOUDDECK1_ID(CAN_HandleTypeDef* hcan);
extern void TX_DATA_CLOUDDECK2_ID(CAN_HandleTypeDef* hcan);
#endif

extern float testdata[8];
void GetBToADataVal(BToAAToBDataBit_t *BToADataBit);
void GetBToADataBit(BToAAToBDataBit_t *BToADataBit);
#endif 
