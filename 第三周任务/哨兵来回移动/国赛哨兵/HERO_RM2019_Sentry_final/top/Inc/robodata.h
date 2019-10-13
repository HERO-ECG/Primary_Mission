#ifndef __ROBODATA_H
#define __ROBODATA_H
#include "main.h"
#include "mytype.h"
#include "m_remote.h"
#include "m_moto.h"

#define K_CHASSIS_REMOTE_FORWARD		3000.0F//各控制值比例值 可根据实际情况调整
#define K_CHASSIS_REMOTE_RIGHT			12000.0F
#define K_CHASSIS_REMOTE_YAW_W			2000.0F

#define K_CHASSIS_KEYBOARD_FORWARD		3000.0F//各控制值比例值 可根据实际情况调整
#define K_CHASSIS_KEYBOARD_RIGHT			3000.0F
#define K_CHASSIS_KEYBOARD_YAW_W			3000.0F

#define K_mouse_pitch_speed_PC	0.2f
#define	K_mouse_yaw_speed_PC	0.4f

typedef enum
{
	AUTO_MOVE_MODE = 1,
	FROM_REMOTE = 2,
	RIGHT_3 = 3,
	AUTO_MOVE_MODE2 = 4
}ControlSource_e;

typedef enum
{
	LEFT_1 = 1,
	LEFT_2 = 2,
	LEFT_3 = 3
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
	WorkingStatus_error = 2
}WorkingStatus_e;//运行状态

typedef struct
{
	uint8_t left_mode;
	uint8_t ctrl_source;//这几个枚举名称之后可以全工程替换修改
}RoboControlMode_t;//机器人控制模式，由遥控器左右拨杆设置

typedef struct
{
	float spd_forward;
	float spd_right;
	float spd_yaw_w;//顺时针时针为正向
	float angle_yaw_w;
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
	
	float limitHeat17;
	float limitHeat42;
	float coolingHeat17;
	float coolingHeat42;
	uint16_t coolingWaitTime17;
	uint16_t coolingWaitTime42;
	float nowHeat17;
	float nowHeat42;
	float addHeat17;
	float addHeat42;
	uint16_t allow17num;
	uint16_t allow42num;
}ShootHeatControl_t;//热量控制


typedef struct _RoboData_t
{

	void (*GetRemoteControlData)(struct _RoboData_t *RoboData);//遥控器数据处理函数
//	void (*GetDJIControlData)(struct _RoboData_t *RoboData);//裁判系统数据处理函数
	//void (*Get17HeatControlData)(struct _RoboData_t *RoboData);//本地17mm枪口热量处理函数
//	void (*Get42HeatControlData)(struct _RoboData_t *RoboData);//本地42mm枪口热量处理函数
  //void (*GetAmmunition17ControlStatus)(struct _RoboData_t *RoboData, Moto_t *TG_MOTO);//17mm发弹状态处理函数
 // void (*GetAmmunition42ControlStatus)(struct _RoboData_t *RoboData, Moto_t *TG_MOTO);//42mm发弹状态处理函数	
  //void (*GetChassisControlStatus)(struct _RoboData_t *RoboData, Moto_t *Chassis_MOTO[4],Pid_t *power_pid);//底盘状态处理函数
  //void (*GetTripodControlStatus)(struct _RoboData_t *RoboData);//云台状态处理函数


	RoboControlMode_t	robo_ctrlmode;//机器人控制模式
	ChassisControl_t chassis_ctrl;//底盘运动控制
	TripodControl_t tripod_ctrl;//云台运动控制	
	AmmunitionControl_t AmmunitionControl;//发弹控制
	ShootHeatControl_t shoot_heat_ctr;//热量控制
	
}RoboData_t;//机器人用户数据总结构体
extern RoboData_t RoboData;//机器人用户数据类


//extern RoboControlMode_t	robo_ctrlmode;//机器人控制模式
//extern ChassisControl_t chassis_ctrl;//底盘运动控制
//extern TripodControl_t tripod_ctrl;//云台运动控制	
//extern AmmunitionControl_t AmmunitionControl;//发弹控制
//extern ShootHeatControl_t shoot_heat_ctr;//热量控制

extern void InitRoboData(RoboData_t *RoboData);
//extern void GetRemoteControlData(void);//遥控器数据处理函数
//extern void GetDJIControlData(void);//裁判系统数据处理函数
//extern void Get17HeatControlData(void);//本地17mm枪口热量处理函数
//extern void Get42HeatControlData(void);//本地42mm枪口热量处理函数
//extern void GetAmmunition17ControlStatus(Moto_t *TG_MOTO);//17mm发弹状态处理函数
//extern void GetAmmunition42ControlStatus(Moto_t *TG_MOTO);//42mm发弹状态处理函数	
//extern void GetChassisControlStatus(Moto_t *Chassis_MOTO[4]);//底盘状态处理函数
////extern void GetTripodControlStatus(void);//云台状态处理函数
#endif
