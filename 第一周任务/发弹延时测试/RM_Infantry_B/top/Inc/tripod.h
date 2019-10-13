#include "stm32f1xx_hal.h"
#include "pid.h"
#include "t_monitor.h"
#ifndef __TRIPOD_H
#define __TRIPOD_H

#define PITCH_CANID	0x1FF

extern float gyro[4];

typedef struct
{
	float angle_offset;
	float angle_relativeToChassis;//����ڵ��̵ĽǶ� ��ǰ����װ�װ�����Ϊ��㣬��ʱ��Ϊ��
}AngleReference_t;

typedef enum
{
	From_Encoder,
	From_Gyro
}AngleSource_e;//����״̬

typedef enum
{
	Position=1,
	Speed
}ControlMode_e;//����״̬

typedef struct
{
	ControlMode_e now;
	ControlMode_e last;
}ControlMode_t;//����״̬


typedef struct
{
	AngleSource_e last;
	AngleSource_e now;
	AngleReference_t encoder;
	AngleReference_t gyro;
}AngleSource_t;//����״̬


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
	Pid_t PID1_angle;
	Pid_t PID_speed;
	Pid_t PID2_angle;
	float angle_max;
	float angle_min;
	uint8_t ready_flag;
	AngleSource_t angle_source;
}Axis_t;

typedef struct
{
	uint8_t now;
	uint8_t last;
}Pid_Mode_t;

typedef struct
{
	Axis_t Pitch;
	Axis_t Yaw;
	ControlMode_t control_mode;
	Monitor_t monitor;
	Pid_Mode_t Pid_Mode;//����PIDģʽ
	uint8_t GyroOffline;//�����Ǽ��״��
}Tripod_t;

extern Tripod_t Tripod;
extern float coupled_yaw_speed;








void Pitch_Init(void);
void Pitch_SetAngle(float pitch);

void Yaw_Init(void);
void Yaw_SetAngle(float pitch);
void GetYawAngle1000Hz(void);
void GetPitchAngle1000Hz(void);
void Tripode_SetAngle(float yaw,float pitch);
void Tripode_SetSpeed(float speed_yaw,float speed_pitch);
void Yaw_SetSpeed(float speed);
void Pitch_SetSpeed(float speed);
void Tripode_Init(void);
float GetIntergralYaw(float yaw_speed,uint16_t frequency);
float GetIntergralPitch(float Pitch_speed,uint16_t frequency);
float GetCoupledYawSpeedFromPitch(float pitch, float yaw_speed_raw);
void Tripode_Fun(void);
float mode360(float angle);
uint8_t GyroMonitor(void);
#endif
