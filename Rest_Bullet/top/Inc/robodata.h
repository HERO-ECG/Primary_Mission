#ifndef __ROBODATA_H
#define __ROBODATA_H
#include "main.h"
#include "mytype.h"
#include "m_remote.h"
#include "m_moto.h"
//#include "dji_Protocol.h"

//#define K_CHASSIS_REMOTE_FORWARD		3000.0F//各控制值比例值 可根据实际情况调整
//#define K_CHASSIS_REMOTE_RIGHT			2000.0F
//#define K_CHASSIS_REMOTE_YAW_W			2000.0F

//#define K_CHASSIS_KEYBOARD_FORWARD		3000.0F//各控制值比例值 可根据实际情况调整
//#define K_CHASSIS_KEYBOARD_RIGHT			3000.0F
//#define K_CHASSIS_KEYBOARD_YAW_W			3000.0F

#define K_mouse_pitch_speed_PC	(1.0f)
#define	K_mouse_yaw_speed_PC		(1.0f)


#define friction17_spd_far 			24
#define friction17_spd_regular 	21
#define friction17_spd_middle 	17
#define friction17_spd_near 		13


#define c_Uplimit_voltage 22
#define c_Downlimit_voltage 21


typedef enum
{
	FROM_PC = 1,
	FROM_REMOTE = 3,
	RIGHT_reserve = 2
}ControlSource_e;

typedef enum
{
	LEFT_up = 1,
	LEFT_mid = 2,
	LEFT_down = 3
}LeftLeverMode_e;

typedef enum
{
	Order_stop = 0,
	Order_start1 = 1,
	Order_startN = 2,
	Order_busy = 3
}Order_e;//指令

typedef enum
{
	WorkingStatus_ready = 0,
	WorkingStatus_busy = 1,
	WorkingStatus_error = 2,
	WorkingStatus_temp = 3
}WorkingStatus_e;//运行状态

typedef struct
{
	LeftLeverMode_e left_mode;
	ControlSource_e ctrl_source;//这几个枚举名称之后可以全工程替换修改
}RoboControlMode_t;//机器人控制模式，由遥控器左右拨杆设置


typedef struct
{
	float angle_yaw_w;//底盘云台偏差角，右旋为正
	float spd_forward;
	float spd_right;
	float spd_yaw_w;//顺时针时针为正向
	float spd_all;
	int16_t yaw_speed_gyro;
	int16_t pitch_speed_gyro;
	//各按键对应的比例系数
	float k_spd_all;
	
	float sup_s_cooling_time;
	//超级电容控制板的返回值
	float sup_s_status;
	
	Order_e chassis_order;
	WorkingStatus_e chassis_working_status;
}ChassisControl_t;//底盘运动控制


typedef struct
{
	float pitch_angle;//向上为正
	float pitch_angle_mix;
	float pitch_speed;	
	float yaw_angle;//顺时针时针为正向(右为正)
	float yaw_angle_mix;
	float yaw_speed;
	Order_e tripod_order;
	WorkingStatus_e tripod_working_status;
}TripodControl_t;//云台运动控制


typedef struct
{
	float Toggler17_angle;
	float Friction17_speed;
	float Toggler42_angle;
	
	uint8_t Fricition_start_flag;
	uint8_t Fricition_time;
	
  uint16_t shoot17_busy_time;
	uint16_t shoot17_error_time;

  uint16_t shoot42_busy_time;
	uint16_t shoot42_error_time;	
	
	Order_e shoot17_order;
	WorkingStatus_e shoot17_working_status;
	Order_e shoot42_order;
	WorkingStatus_e shoot42_working_status;
}AmmunitionControl_t;//发弹控制


typedef struct
{
	
	uint16_t limitHeat17;
	uint16_t limitHeat42;
	float coolingHeat17;
	float coolingHeat42;
	uint16_t coolingWaitTime17;
	uint16_t coolingWaitTime42;
	int16_t nowHeat17;
	int16_t nowHeat42;
	uint16_t addHeat17;
	uint16_t addHeat42;
	float allow17num;
	float allow42num;
}ShootHeatControl_t;//热量控制


typedef struct _RoboData_t
{

	void (*GetRemoteControlData)(struct _RoboData_t *RoboData);//遥控器数据处理函数
	void (*GetDJIControlData)(struct _RoboData_t *RoboData);//裁判系统数据处理函数
	void (*Get17HeatControlData)(struct _RoboData_t *RoboData);//本地17mm枪口热量处理函数
	void (*Get42HeatControlData)(struct _RoboData_t *RoboData);//本地42mm枪口热量处理函数
    void (*GetAmmunition17ControlStatus)(struct _RoboData_t *RoboData, Moto_t *TG_MOTO);//17mm发弹状态处理函数
    void (*GetAmmunition42ControlStatus)(struct _RoboData_t *RoboData, Moto_t *TG_MOTO);//42mm发弹状态处理函数	
	void (*GetChassisControlStatus)(struct _RoboData_t *RoboData);//底盘状态处理函数
  //void (*GetTripodControlStatus)(struct _RoboData_t *RoboData);//云台状态处理函数


	RoboControlMode_t	robo_ctrlmode;//机器人控制模式
	ChassisControl_t chassis_ctrl;//底盘运动控制
	TripodControl_t tripod_ctrl;//云台运动控制	
	AmmunitionControl_t AmmunitionControl;//发弹控制
	ShootHeatControl_t shoot_heat_ctr;//热量控制
	
}RoboData_t;//机器人用户数据总结构体
extern RoboData_t RoboData;//机器人用户数据类
extern void InitRoboData(RoboData_t *RoboData);


#endif
