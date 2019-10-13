#ifndef __ROBODATA_H
#define __ROBODATA_H
#include "main.h"
#include "mytype.h"
#include "m_remote.h"
#include "m_moto.h"

#define K_CHASSIS_REMOTE_FORWARD		3000.0F//������ֵ����ֵ �ɸ���ʵ���������
#define K_CHASSIS_REMOTE_RIGHT			12000.0F
#define K_CHASSIS_REMOTE_YAW_W			2000.0F

#define K_CHASSIS_KEYBOARD_FORWARD		3000.0F//������ֵ����ֵ �ɸ���ʵ���������
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
}Order_e;//ָ��

typedef enum
{
	WorkingStatus_ready = 0,
	WorkingStatus_busy = 1,
	WorkingStatus_error = 2
}WorkingStatus_e;//����״̬

typedef struct
{
	uint8_t left_mode;
	uint8_t ctrl_source;//�⼸��ö������֮�����ȫ�����滻�޸�
}RoboControlMode_t;//�����˿���ģʽ����ң�������Ҳ�������

typedef struct
{
	float spd_forward;
	float spd_right;
	float spd_yaw_w;//˳ʱ��ʱ��Ϊ����
	float angle_yaw_w;
	Order_e chassis_order;
	WorkingStatus_e chassis_working_status;
}ChassisControl_t;//�����˶�����

typedef struct
{
	float pitch_angle;//����Ϊ��
	float pitch_angle_mix;
	float pitch_speed;	
	float yaw_angle;//˳ʱ��ʱ��Ϊ����(��Ϊ��)
	float yaw_angle_mix;
	float yaw_speed;
	Order_e tripod_order;
	WorkingStatus_e tripod_working_status;
}TripodControl_t;//��̨�˶�����

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
}AmmunitionControl_t;//��������


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
}ShootHeatControl_t;//��������


typedef struct _RoboData_t
{

	void (*GetRemoteControlData)(struct _RoboData_t *RoboData);//ң�������ݴ�����
//	void (*GetDJIControlData)(struct _RoboData_t *RoboData);//����ϵͳ���ݴ�����
	//void (*Get17HeatControlData)(struct _RoboData_t *RoboData);//����17mmǹ������������
//	void (*Get42HeatControlData)(struct _RoboData_t *RoboData);//����42mmǹ������������
  //void (*GetAmmunition17ControlStatus)(struct _RoboData_t *RoboData, Moto_t *TG_MOTO);//17mm����״̬������
 // void (*GetAmmunition42ControlStatus)(struct _RoboData_t *RoboData, Moto_t *TG_MOTO);//42mm����״̬������	
  //void (*GetChassisControlStatus)(struct _RoboData_t *RoboData, Moto_t *Chassis_MOTO[4],Pid_t *power_pid);//����״̬������
  //void (*GetTripodControlStatus)(struct _RoboData_t *RoboData);//��̨״̬������


	RoboControlMode_t	robo_ctrlmode;//�����˿���ģʽ
	ChassisControl_t chassis_ctrl;//�����˶�����
	TripodControl_t tripod_ctrl;//��̨�˶�����	
	AmmunitionControl_t AmmunitionControl;//��������
	ShootHeatControl_t shoot_heat_ctr;//��������
	
}RoboData_t;//�������û������ܽṹ��
extern RoboData_t RoboData;//�������û�������


//extern RoboControlMode_t	robo_ctrlmode;//�����˿���ģʽ
//extern ChassisControl_t chassis_ctrl;//�����˶�����
//extern TripodControl_t tripod_ctrl;//��̨�˶�����	
//extern AmmunitionControl_t AmmunitionControl;//��������
//extern ShootHeatControl_t shoot_heat_ctr;//��������

extern void InitRoboData(RoboData_t *RoboData);
//extern void GetRemoteControlData(void);//ң�������ݴ�����
//extern void GetDJIControlData(void);//����ϵͳ���ݴ�����
//extern void Get17HeatControlData(void);//����17mmǹ������������
//extern void Get42HeatControlData(void);//����42mmǹ������������
//extern void GetAmmunition17ControlStatus(Moto_t *TG_MOTO);//17mm����״̬������
//extern void GetAmmunition42ControlStatus(Moto_t *TG_MOTO);//42mm����״̬������	
//extern void GetChassisControlStatus(Moto_t *Chassis_MOTO[4]);//����״̬������
////extern void GetTripodControlStatus(void);//��̨״̬������
#endif
