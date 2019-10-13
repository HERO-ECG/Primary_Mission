#ifndef __STRIKE_H
#define __STRIKE_H

#include "main.h"
#include "stm32f1xx_hal.h"
#include "mytype.h"
#include "m_moto.h"
#include "robodata.h"

#define	KEY_FIREMODE	RC_CtrlData.key.key_data.Q
#define	KEY_AMMOCOVERSTATE	RC_CtrlData.key.key_data.R
#define	KEY_AUTOAIM		RC_CtrlData.mouse.press_2
#define	KEY_FIRE			RC_CtrlData.mouse.press_1

#ifdef INFANTRY3

#define friction_of_long_range_left 1285
#define friction_of_long_range_right 1285
#define friction_of_close_range_left 1200
#define friction_of_close_range_right 1200
#endif


#ifdef INFANTRY4

#define friction_of_long_range_left 1265
#define friction_of_long_range_right 1265
#define friction_of_close_range_left 1200
#define friction_of_close_range_right 1200

#endif

#ifdef INFANTRY5

#define friction_of_long_range_left 1265
#define friction_of_long_range_right 1265
#define friction_of_close_range_left 1190
#define friction_of_close_range_right 1190

#endif


#define SET 0
#define GET 1
#define CLR 2

#define MID_UP	0
#define LEFT_UP 1
#define LEFT_DOWN	2
#define RIGHT_UP	3
#define RIGHT_DOWN	4

typedef enum
{
	ManualAim_ManualStrike,
	ManualAim_AutoStrike,
	AutoAim_ManualStrike,
	AutoAim_AutoStrike,
	WindMill_Still,
	WindMill_Clkwise,
	WindMill_Anticlkwise,
	WindMill_Manual
}StrikeMode_e;//运行状态

typedef enum
{
	OnTarget,
	OffTarget
}AimState_e;//运行状态

typedef	struct
{
	float	delta_pitch;
	float	delta_yaw;
}DeltaAngle_t;

typedef struct
{
	float angular_velocity_tripod;
	float angular_velocity_vision;
	float angular_velocity_absolute;
	float variance;
	float variance_valid_threshold;
	float angular_velocity_absolute_filtered;
	float angle_predicted_ahead;
}PredictionAngle_t;

typedef struct
{
	PredictionAngle_t pitch;
	PredictionAngle_t yaw;
	float distance;
}Prediction_t;

typedef struct
{
	DeltaAngle_t deltaangle_now;	
	DeltaAngle_t deltaangle_last;
	DeltaAngle_t manual_correct_offset;
	Prediction_t	prediction;
	float OnTargetAngleMax;
	uint8_t	autoaim_flag;
	uint8_t	manual_correct;
	uint8_t manual_correct_clear;
	AimState_e aimstate;
	uint8_t	TX2_no_response;
}Vision_t;



typedef enum
{
	Semi_Mode,
	Burst_Mode,
	Auto_Mode
}FireMode_e;

typedef enum
{
	Close_Range,
	Long_Range
}ShootingRange_e;

typedef struct
{
	uint8_t IsOn;
	ShootingRange_e shooting_range;
	uint16_t friction_moto_pwm_pulse_left;
	uint16_t friction_moto_pwm_pulse_right;
}FrictionCtrl_t;

typedef struct
{
	FireMode_e now;
	FireMode_e last;
}FireMode_t;

typedef struct
{
	uint8_t now;
	uint8_t last;
	uint8_t cease_fire_code;
}Fire_t;

typedef struct
{
	uint16_t all;
	uint16_t burst;
	uint16_t num_to_shot;
	uint16_t period;//决定射频，两发子弹之间的毫秒数
	uint64_t in_firecounter;
	uint8_t	in_flag;
	uint64_t out_firecounter;
	uint8_t fire_flag;
}Counter_t;



typedef struct
{
	float heat_local;
	float heat_server;
	float heat_mingle;
	float margin;
	float cooling_rate_expected;
	float cooling_rate_server;
	int16_t cooling_limit_expected;
	int16_t cooling_limit_server;
	uint16_t sever_lag;
	uint8_t ceasefire;
	
	uint16_t robot_level;
	uint8_t unlimited;
	uint32_t unlimited_counter;
	
	uint8_t set_level;
}Heat_t;

typedef struct
{
	float angle;
	float speed;
	float speed_set;
	float angle_set;
	float current_set;
	float angle_offset;
	Pid_t PID_angle;
	Pid_t PID_speed;
}Toggle_Motor_t;


typedef struct
{
	DeltaAngle_t base_angle;
	uint8_t  IsCalibrated;
	uint8_t aim_num;
}WindMillManual_t;




typedef struct
{
	Fire_t fire;
	Counter_t count;
	Vision_t	vision;
	uint16_t	shootspd;
	FireMode_t	firemode;
	StrikeMode_e	strikemode;
	WindMillManual_t windmill_manual;
	Heat_t heat;
	FrictionCtrl_t friction_ctrl;
	Toggle_Motor_t	toggle_motor;
	uint8_t cover_open;//0是关
}Strike_t;


extern Strike_t Strike;

extern uint8_t AmmoKey;


void FrictionMotorServo(void);
void Strike_Init(void);
void Strike_fun(void);
void AutoStrike(void);
void ToggleMotoControl(void);
void FrictionMotoCtrl(void);
void Strike_IntFun_in(void);
void Strike_IntFun_out(void);
void HeatControl(void);
void LastHeatControl(void);
void CoverControl(void);
void AngleQueue(float *pitch,float *yaw,uint16_t index,uint8_t mode);
float HeatQueue(float heat,uint16_t index,uint8_t mode);
void HeatCtrl(void);
void StrikeCount(void);
void WindMill_ManualStrike(void);
#endif

