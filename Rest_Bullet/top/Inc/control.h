#ifndef __CONTROL_H
#define __CONTROL_H

#include "stm32f4xx_HAL.h"
#include "mytype.h"
#include <math.h>
#include "gpio.h"
#include "spi.h"
#include "m_imu.h"
#include "tim.h"
#include "usart.h"
#include "can.h"
#include "m_remote.h"
#include "m_moto.h"
#include "robodata.h"
#include "dji_Protocol.h"
#include "tx2_Protocol.h"
#include "pid.h"
#include "m_transmit.h"
extern float var[8];
extern void vcan_sendware(u8 *wareaddr, u32 waresize); 
extern uint32_t time_piece;

#define time_piece_start	0x0001
#define time_piece_1000hz	0x1000
#define time_piece_0100hz 0x0100

#endif

