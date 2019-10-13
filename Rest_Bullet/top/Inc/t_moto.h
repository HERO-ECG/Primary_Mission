#ifndef _T_MOTO_H
#define _T_MOTO_H
#include "mytype.h"
#include "m_moto.h"

#define cm_max_send_current 3000 
#define cm_min_send_current 0
#define mid_power 70


//角度单位为°
#define Toggler17_speed_one		(5800)//rpm
#define Toggler17_angle_one 	(1620)//发一个弹的角度
#define Toggler17_angle_two 	(3200)//发两个弹的角度
#define Toggler17_angle_three 	(4700)//发三个弹的角度
#define Toggler17_angle_four  	(6500)//发四个弹的角度


#define Toggler42_speed_one		(500)//rpm
#define Toggler42_angle_one 	(675)//发一个弹的角度

#ifdef Yaw6025
#define yaw6025_MiddleAngle (0/2048.0f*360.0f)
#define yaw_middle_angle	yaw6025_MiddleAngle
#endif
#ifdef Yaw6623
#define yaw6623_RightLimit 	(5010/8192.0f*360.0f)		
#define yaw6623_MiddleAngle (7080/8192.0f*360.0f)		
#define yaw6623_LeftLimit 	(9152/8192.0f*360.0f)			
#define yaw_middle_angle 	yaw6623_MiddleAngle
#endif
#ifndef yaw_middle_angle 
#define yaw_middle_angle 0
#endif

#define pitch6623_UpLimit 		(6280/8192.0f*360.0f)		//290
#define pitch6623_MiddleAngle (7280/8192.0f*360.0f)		//320
#define pitch6623_DownLimit 	(7796/8192.0f*360.0f)		//340

extern Pid_t PID_ChassissFollow;

extern int __sgn(float x);
extern void switch_chassis_speed_power(float limit_power);
extern void set_cm_speed(float theta);
extern void AmmunitionToggle2006(Moto_t *MOTO, float the_get_speed, float the_set_speed, float the_set_angle);		//1000hz
extern void AmmunitionToggle3508(Moto_t *MOTO, float the_get_speed, float the_set_speed, float the_set_angle);		//1000hz
extern void PitchMotoCtrl(Moto_t *MOTO, float up_plus, float down_plus, float set_speed, float set_angle);		//1000hz	
extern void YawMoto6623Ctrl(Moto_t *MOTO, float set_speed, float set_angle);		//1000hz	
extern void chassis_control(void);
float ChassissFollow(float angle_now,float angle_set);
#endif
