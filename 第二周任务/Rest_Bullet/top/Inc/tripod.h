#include "stm32f4xx_hal.h"
#include "PID.h"
#include "pid.h"

#ifndef __TRIPOD_H
#define __TRIPOD_H

#define PITCH_CANID	0x1FF

typedef struct
{
	float angle_now;
	float angle_offset;
}AngleReference_t;

typedef enum
{
	From_Encoder,
	From_Gyro
}AngleSource_e;//运行状态

typedef enum
{
	Position,
	Speed
}ControlMode_e;//运行状态

typedef struct
{
	ControlMode_e now;
	ControlMode_e last;
}ControlMode_t;//运行状态


typedef struct
{
	AngleSource_e source_last;
	AngleSource_e source_now;
	AngleReference_t encoder;
	AngleReference_t gyro;
}AngleSource_t;//运行状态


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
	float angle_offset;
	Pid_t PID_angle;
	Pid_t PID_speed;
	float angle_max;
	float angle_min;
	uint8_t ready_flag;
	AngleSource_t angle_source;
}Axis_t;

typedef struct
{
	void *Init;
	void *Fun;
	Axis_t Pitch;
	Axis_t Yaw;
	ControlMode_t control_mode;
}Tripod_t;//运行状态

extern Tripod_t Tripod;
extern float coupled_yaw_speed;
extern float yaw_integral;







void Pitch_Init(void);
void Pitch_SetAngle(float pitch);

void Yaw_Init(void);
void Yaw_SetAngle(float pitch);
void GetYawAngle100Hz(void);
void Tripode_SetAngle(float yaw,float pitch);
void Tripode_SetSpeed(float speed_yaw,float speed_pitch);
void Yaw_SetSpeed(float speed);
void Pitch_SetSpeed(float speed);
void Tripode_Init(void);
float GetIntergralYaw(float yaw_speed,uint16_t frequency);
float GetCoupledYawSpeedFromPitch(float pitch, float yaw_speed_raw);
void Tripode_Fun(void);
void mode360(float* angle);
#endif
