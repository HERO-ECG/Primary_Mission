#ifndef __STRIKE_H
#define __STRIKE_H

#include "main.h"
#include "stm32f4xx_hal.h"
#include "mytype.h"
#include "m_moto.h"
#include "robodata.h"

#define	KEY_AUTOAIM					(RC_CtrlData.mouse.press_2)

#define	KEY_FIRE_42MM			(RC_CtrlData.mouse.press_1)

//#define friction_pulse_17mm 1060
#define friction_pulse_17mm 1160
#define friction_speed_42mm 5100

#define toggle_speed_17mm 6000
#define toggle_speed_42mm 6000

#define SET 0
#define GET 1
#define CLR	2


typedef enum
{
	ManualAim_ManualStrike,
	ManualAim_AutoStrike,
	AutoAim_ManualStrike,
	AutoAim_AutoStrike,
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
	AimState_e aimstate;
	uint8_t	TX2_no_response;
}Vision_t;

typedef enum
{
	Semi_Mode,
	Burst_Mode,
	Auto_Mode
}FireMode_17mm_e;

typedef struct
{
	FireMode_17mm_e now;
	FireMode_17mm_e last;
}FireMode_t;

typedef struct
{
	uint8_t now;
	uint8_t last;
}Fire_t;

typedef struct
{
	uint16_t all;
	uint16_t burst;
	uint64_t in_firecounter;
	uint64_t out_firecounter;
	uint8_t fire_flag;
}Counter_t;



typedef struct
{
	float heat_local;
	uint8_t heat_sever;
	float heat_mingle;
	float margin;
	float cooling_rate;
	uint8_t cooling_limit;
	uint8_t ceasefire;
	uint8_t unlimited;
}Heat_t;

typedef struct
{
	uint16_t friction_17mm;
	float friction_42mm;
	float	toggle_rpm_17mm;
	float	toggle_rpm_42mm;
}StrikeMotorPara_t;





typedef struct
{
	Fire_t fire_17mm;
	Fire_t fire_42mm;
	Counter_t count_17mm;
	Counter_t count_42mm;
	Vision_t	vision;
	FireMode_t	firemode;
	StrikeMode_e	strikemode;
	Heat_t heat_17mm;
	Heat_t heat_42mm;
	StrikeMotorPara_t moto_para;
	uint8_t cover_open;//0是关
}Strike_t;



extern Strike_t Strike;

extern uint8_t AmmoKey;



void FrictionMotorServo(void);
void Strike_Init(void);
void Strike_fun(void);
void AutoStrike(void);
void AmmoMotoControl(void);
void Strike_IntFun_in(void);
void Strike_IntFun_out(void);
void HeatControl(void);
void LastHeatControl(void);
void CoverControl(void);
void AngleQueue(float *pitch,float *yaw,uint16_t index,uint8_t mode);
float HeatQueue(float heat,uint16_t index,uint8_t mode);
void HeatCtrl(void);
void StrikeCount(void);
uint8_t Shoot_42mm(void);
#endif

