#ifndef __TW_SERVO_H
#define __TW_SERVO_H

#include "mytype.h"
#include "pid.h"

#define TW_COBID_TX						0x601
#define ONE_BYTE_TXLENGTH 		0x2f
#define TWO_BYTE_TXLENGTH 		0x2b
#define THREE_BYTE_TXLENGTH 	0x27
#define FOUR_BYTE_TXLENGTH 		0x23
#define	READ									0x40
#define	CONTROLWORD						0x6040
#define QUICKSTOP							0x44
#define	POSITIONMODE_BIT			6
#define	POSITIONMODE_ABSOLUTE	0x01
#define	POSITIONMODE_RELATIVE	0x40

#define STATUSEWORD						0x6041

#define CONTROLMODE_W					0x6060
#define CONTROLMODE_R					0x6061
#define CONTROLMODE_POSITION	1
#define CONTROLMODE_SPEED			3

#define POSITION_W						0x607a
#define	SPEEDLIMIT						0x6081

#define	SPEED_W								0x60ff
#define	POSITION_R						0x6064
#define	SPEED_R								0x606c
#define	CURRENT_R							0x6078

#define TW_COBID_RX 						0x581
#define TW_ONE_BYTE_RXLENGTH 		0x4f
#define TW_TWO_BYTE_RXLENGTH 		0x4b
#define TW_THREE_BYTE_RXLENGTH 	0x47
#define TW_FOUR_BYTE_RXLENGTH 	0x43

typedef struct
{
	u8 code;
	u16 object_index;
	u8 subindex;
	u32 object_val;
}TW_CANopen_RX_t;

typedef struct
{
	s32 position;
	s32 speed;
	s16 current;
	
	u8 init_state;
	u8 init_cnt;
	
	s32 offset_angle;
	float offset_round;
	s32	total_angle;
	
	
	float round;
	float angle;
	float integral_angle;
	s32 speed_set;
	s32 angle_set;
	
	Pid_t PID_angle;
	
}TW_servo_t;


extern TW_CANopen_RX_t TW_CANopen_RX;
extern TW_servo_t TW_servo;

extern void CANopen_SendData(CAN_HandleTypeDef* hcan, uint32_t cob_id, u8 code, u16 object_index, u8 subindex,s32 object_val);
extern void TW_servo_SetSpeedMode(void);
extern void TW_servo_QuickStop(void);
extern void TW_servo_SetSpeed(s32 speed);
extern void TW_servo_SetPositionMode(void);
extern void TW_servo_SetPosition(s32 move);
extern void TW_servo_SetPositionModeMode(u8 mode);

extern void CANopen_ReadData(CAN_HandleTypeDef* hcan);
extern void TW_servo_GetPosition(void);
extern void TW_servo_GetSpeed(void);
extern void TW_servo_GetCurrent(void);
extern void TW_ProcessData(TW_CANopen_RX_t TW_CANopen_RX);
void TW_Servo_GetEncoderMeasure(void);
void TW_servo_Init(void);
void TW_servo_SetPositionInSpeedMode(float angle_set,float angle_now);
#endif

