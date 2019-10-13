#include "stm32f4xx_hal.h"
#include "pid.h"
#include "t_monitor.h"
#ifndef __TRIPOD_H
#define __TRIPOD_H

#define CHASSIS_FILLOW_OFFSET 7150
#define	PITCH6020_OFFSET	300.0f
#define PITCH_CANID	0x1FF

typedef struct
{
	float angle_now;
	float angle_offset;
	float angle_relativeToChassis;//相对于底盘的角度 正前方大装甲板中央为零点，逆时针为正
}AngleReference_t;

typedef enum
{
	From_Encoder,
	From_Gyro
}AngleSource_e;//运行状态

typedef enum
{
	Position=1,
	Speed
}ControlMode_e;//运行状态

typedef struct
{
	ControlMode_e now;
	ControlMode_e last;
}ControlMode_t;//运行状态


typedef struct
{
	AngleSource_e last;
	AngleSource_e now;
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
	Axis_t Pitch;
	Axis_t Yaw;
	ControlMode_t control_mode;
	Monitor_t monitor;
	uint8_t GyroOffline;
}Tripod_t;//运行状态

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
void Tripod_Init(void);
float GetIntergralYaw(float yaw_speed,uint16_t frequency);
float GetIntergralPitch(float Pitch_speed,uint16_t frequency);
float GetCoupledYawSpeedFromPitch(float pitch, float yaw_speed_raw);
void Tripod_Fun(void);
float mode360(float angle);
uint8_t GyroMonitor(void);
#endif
