#ifndef __STRIKE_H
#define __STRIKE_H

#include "main.h"
#include "stm32f4xx_hal.h"
#include "mytype.h"
#include "m_moto.h"
#include "robodata.h"
#include "tx2_Protocol.h"

#define SHOOTRHYTHM 10000

typedef struct{
	uint8_t start_flag;
	uint16_t cnt;
	uint16_t period;
	float result;
	
}period_calc_typedef;


typedef enum
{
	OnTarget,
	OffTarget
}AimState_e;//运行状态



typedef enum
{
	Semi_Mode,
	Burst_Mode,
	Auto_Mode,
	Auto_Semi_Mode
}FireMode_e;


typedef enum
{
	first,
	second
}Semi_time_e;

typedef enum
{
	normal,
	error
}state_2006_e;

typedef enum
{
	qingkong,
	real_work
}work_state_e;

typedef struct
{
	uint8_t refresh_flag;//判断裁判系统热量更新
	float heat_CalcLoacal_fromSever;//服务器反馈射速计算热量,本地计算冷却,停止发弹之后同步
	//后三个目前没用到
	uint16_t heat_cooling_Calctime;//冷切计算时间
	uint8_t heat_source;
	uint16_t heat_now;//实际使用的热量
}Heat_state;

typedef struct
{
	uint16_t	shootspd;
	FireMode_e	firemode;
	AimState_e aimstate;
	Semi_time_e  Semi_time;
	state_2006_e state_2006;
	Heat_state  Heat_Ctrl;
	float OnTargetAngleMax;
	float work_state;
}Strike_t;

extern Strike_t Strike;
extern 	float fire;
extern float Manual_fire;
extern uint8_t  TGcontrol_from_heat;
extern float speed_from_heat;
extern uint8_t if_heat_limit;

extern void AutoStrike(void);
extern void strike_init(void);
extern void HeatControl(void);
extern void Head_refresh(void);//回调
extern void HeatCtrl(void);

#endif


