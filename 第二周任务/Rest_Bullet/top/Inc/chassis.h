#ifndef __CHASSIS_H
#define	__CHASSIS_H

#include "robodata.h"
#include "pid.h"



typedef enum
{
	FRONT_FOLLOW=1,//�������
	BACK_FOLLOW,//�������
	FRONT_NOT_FOLLOW,//������
	BACK_NOT_FOLLOW,//������
	SIDE_FOLLOW_LEFT,
	SIDE_FOLLOW_RIGHT,
	SPIN,//С����
	SWAY//ҡ��
}ChassisFollowMode_e;

typedef struct
{
	ChassisFollowMode_e now;
	ChassisFollowMode_e last;
}ChassisFollowMode_t;


typedef struct
{
	ChassisFollowMode_t mode;
	float angle;//��̨����ڵ��̽Ƕȣ���ʱ��
	float deadband;//��������
	float total_angle;//��ֹҡ�ڷ��
		Pid_t pid;
}ChassisFollow_t;

typedef struct
{
	float now[4];//��������ֵ
	float set[4];//��������ֵ
	float spd_forward;//���ñ���ֵ
	float spd_right;//���ñ���ֵ
	float spd_yaw_actual;//���ñ���ֵ
	float spd_spinspeed_actual;//���ñ���ֵ
	float base;//��ֵ
	uint8_t spin_fast;
}ChassisSpeed_t;

typedef struct
{
	float Voltage_realtime;
	float Voltage_filtered;
	uint8_t sup_c_ready;
	uint8_t V_status;
	uint8_t sup_c_open;
	float voltage_min;
	float voltage_max;
	float power_percentage;
	float utility_percentage;
	
	uint8_t manual_disabled;
}SuperCapacitor_t;

typedef enum
{
	FAST = 0,
	NORMAL = 1,
	SLOW = 2,
}PowerMode_e;


typedef struct
{
	PowerMode_e mode;
	float power_now;
	float power_set;
	float speed_decreasement;
	float speed_decreasement_k;
	float deadband;
	float buff;
	Pid_t pid;
	uint8_t unlimited;
}Power_t;

typedef struct
{
	float heat_server[2];
	int fireammo_counter;
	float client_ammo_left;
	
}Heat_t;

typedef struct
{
	ChassisSpeed_t speed;
	ChassisFollow_t follow;	
	Power_t	power;
	SuperCapacitor_t supercapacitor;

	Heat_t heat;
}Chassis_t;

extern Chassis_t Chassis;
extern 	float RealPower_Total ;
void Chassis_Init(void);
void Chassis_Fun(void);
void Chassis_PowerControl(void);
#endif

