#ifndef __PITCH_H
#define __PITCH_H
#include "pid.h"

#define PITCH_CANID	0x1FF



typedef enum
{
	encounder=1,
	gyro=2
}YawAngleSouce_t;

typedef struct
{
	uint8_t YawAngleSource;
	uint8_t YawAngleSource_last;
	float ChangeSource_pending;
	float angle;
	float round;
	float gyro_offsetangle;
	float encounder_offsetangle;
	
}Yaw_Change_t;

typedef struct
{
	void (*Init)(void);
	void (*Fun)(float);
	//void (*Init)(void);
	float angle;
	float speed;
	float speed_set;
	float angle_set;
	float current_set;
	
	Pid_t PID_angle;
	Pid_t PID_speed;
	float angle_max;
	float angle_min;
}Pitch__t;

extern float last_angle;
extern Pitch__t Pitch;
extern Yaw_Change_t Yaw_Change;
void Pitch_Init(void);
void Pitch_Init2(void);
void Pitch_SetAngle(float pitch);
void Pitch_SetSpeed(float SetSpeed);
void Yaw_Change_Init(void);
void YawAngle_get(void);

#endif



