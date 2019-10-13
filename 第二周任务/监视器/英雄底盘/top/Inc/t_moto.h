#ifndef __T_MOTO_H
#define __T_MOTO_H

#include "mytype.h"
#include "m_moto.h"

#define cm_max_send_current 3000 
#define cm_min_send_current 0
#define mid_power 70

//#define Toggler17_speed_one		(5800)		//rpm
//#define Toggler17_angle_one 	(1620)		//��һ�����ĽǶ�
//#define Toggler17_angle_two 	(3200)		//���������ĽǶ�
//#define Toggler17_angle_three 	(4700)	//���������ĽǶ�
//#define Toggler17_angle_four  	(6500)	//���ĸ����ĽǶ�

//#define Toggler42_speed_one		(500)			//rpm
//#define Toggler42_angle_one 	(675)			//��һ�����ĽǶ�

extern int __sgn(float x);

extern void SetChassisSpeed(void);

void PitchMotoCtrl(Moto_t *MOTO, float up_plus, float down_plus, float set_speed, float set_angle);
void YawMoto6623Ctrl(Moto_t *MOTO, float the_set_speed, float the_set_angle);

#endif
