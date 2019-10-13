#ifndef _T_MOTO_H
#define _T_MOTO_H
#include "mytype.h"
#include "m_moto.h"


#define cm_max_send_current 3000 
#define cm_min_send_current 0
#define mid_power 70


//角度单位为°

#define Toggler17_angle_one 	(1620)//发一个弹的角度
#define Toggler17_speed_one		(5800)//rpm
#define Toggler17_angle_N 		(4000)//因未知误差改90°为80°
#define Toggler17_speed_N			(5800)//rpm


#define Toggler42_angle_one 	(80.0f*819.2f)//因未知误差改90°为80°
#define Toggler42_speed_one		(700)//rpm
#define Toggler42_angle_N 		(3*80.0f*819.2f)//因未知误差改90°为80°
#define Toggler42_speed_N			(700)//rpm




#define yaw6623_RightLimit (2400/8192.0f*360.0f)		//144.36
#define yaw6623_MiddleAngle (4320/8192.0f*360.0f)		//227.72
#define yaw6623_LeftLimit (6370/8192.0f*360.0f)			//319.92


#define pitch6623_UpLimit (800/8192.0f*360.0f)			//54.4
#define pitch6623_MiddleAngle (480/8192.0f*360.0f)	//320
#define pitch6623_DownLimit (67/8192.0f*360.0f)		//2.9

extern float yuanlai_angle_buff[90];


extern void PitchMotoAutoMode(Moto_t *MOTO);
extern void YawMotoAutoMode(Moto_t *MOTO);
extern void ChassisMotoAutoMode(Moto_t *MOTO);
extern uint32_t FrictionMotorServo(uint32_t setspeed);

extern int __sgn(float x);
extern void switch_cm_speed_pid(void);
extern void cm_power_speed_ctr(void); 
extern void set_cm_speed(void);
extern void AmmunitionToggle2006(Moto_t *MOTO, float get_speed, float set_speed, float set_angle);		//1000hz
extern void PitchMotoCtrl(Moto_t *MOTO, float up_plus, float down_plus, float set_speed, float set_angle);		//1000hz	
extern void YawMotoCtrl(Moto_t *MOTO, float set_speed, float set_angle);		//1000hz	
extern void PowerControl(void);
#endif
