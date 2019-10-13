//#include "strike.h"
//#include "m_imu.h"
//#include "control.h"
//#include "t_monitor.h"


//uint32_t FrictionMotorServo(uint32_t setspeed)
//{
//	static uint32_t speed_now=400;
//	if(speed_now<setspeed)
//		speed_now++;
//	if(speed_now>setspeed)
//		speed_now--;
//	if(speed_now<1000)
//		return 1000;
//	return speed_now;
//}

//void AmmoMotoControl(Moto_t *TG_MOTO, RoboData_t *RoboData, float the_set_speed)
//{
//	if(AmmoKey == 0){
//		TG_MOTO[0].set_speed = 0;
//		RoboData->shoot_heat_ctr.nowHeat17 += RoboData->shoot_heat_ctr.addHeat17;
//		AmmoKey = 1;
//	}
//	else if(AmmoKey == 1){
//		if(RoboData->shoot_heat_ctr.nowHeat17 + RoboData->shoot_heat_ctr.addHeat17 < RoboData->shoot_heat_ctr.limitHeat17){
//			if(RoboData->AmmunitionControl.shoot17_order == Order_start1) 
//				TG_MOTO[0].set_speed = the_set_speed;
//			else TG_MOTO[0].set_speed = 0;
//		}
//		else 
//			TG_MOTO[0].set_speed = 0;
//	}
//}

///*-------------------------------AmmunitionToggle--------------------------------------------*/
//void AmmunitionToggle2006(Moto_t *MOTO, float the_get_speed, float the_set_speed, float the_set_angle)		//1000hz
//{
//	MOTO->getpara.other_speed = the_get_speed*0.7f + 0.3f*MOTO->getpara.other_speed;//rpm,传感器在中频时准确

//	//电机角度信号赋值
//	MOTO->set_angle = the_set_speed;
//	//电机限速
//	MOTO->pid_angle.PidSet_limit(&(MOTO->pid_angle), ABS(the_set_speed), MOTO->pid_angle.integral_uplimit, MOTO->pid_angle.integral_downlimit, 0);
//	//角度环相关参数赋值_&_角度环_&_电机速度信号赋值
//	MOTO->pid_angle.PidSet_errband(&(MOTO->pid_angle), 5, 3, 0, 0,0);
//	MOTO->set_speed = MOTO->pid_angle.PidCalc(&(MOTO->pid_angle), (MOTO->getpara.total_round)*360, the_set_angle);	
//	//速度环相关参数赋值_&_速度环_&_电机输出信号赋值
//	MOTO->pid_speed.PidSet_errband(&(MOTO->pid_speed), 0.6f*the_set_speed, 0.2f*the_set_speed, 0, 0,0);
//	MOTO->send_current = MOTO->pid_speed.PidCalc(&(MOTO->pid_speed), MOTO->getpara.other_speed, MOTO->set_speed);		

//	MOTO->set_speed_last = MOTO->set_speed;
//	MOTO->set_angle_last = MOTO->set_angle;
//}




