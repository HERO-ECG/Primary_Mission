/**
  *@file test_imu.h
  *@date 2016-12-12
  *@author Albert.D
  *@brief 
  */
  
#ifndef _TEST__IMU_H
#define _TEST__IMU_H

#include "stm32f4xx_hal.h"
#include "main.h"

#define MPU6500_NSS_Low() HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, GPIO_PIN_RESET)
#define MPU6500_NSS_High() HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, GPIO_PIN_SET)

typedef struct
{
  float ax;
  float ay;
  float az;
  
  int16_t temp;
  
  float gx;//
  float gy;//
  float gz;//
  
  int16_t mx;
  int16_t my;
  int16_t mz;
	
	float anglex;
	float angley;
	float anglez;
	
	uint8_t init_sta;
}IMUDataTypedef;

extern uint8_t MPU_id;
extern IMUDataTypedef imu_data;
extern IMUDataTypedef imu_data_offest;
extern uint8_t IST8310_Init(void);
extern uint8_t MPU6500_Init(void);
extern void IMU_Get_Data(void);

#endif
