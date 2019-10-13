#include "t_moto.h"
#include "m_imu.h"
#include "control.h"
#include "m_moto.h"
#include "auto_move.h"


extern float pitch_up_plus ;
extern float pitch_down_plus ;




/*---------------------------------------------------------------------------*/


int __sgn(float x)
{
	if(x > 1) return 1;
	else if (x < -1) return -1;
	else return 0;
}


/*-------------------------------Chassis--------------------------------------------*/
void set_cm_speed(void) 
{
	if((RC_CtrlData.rc.ch3 != 0)&&(RC_CtrlData.rc.ch2 != 0)&&(RC_CtrlData.rc.ch0 != 0))
	{	

		Chassis_MOTO[0].set_speed =  RoboData.chassis_ctrl.spd_right; //+ RoboData.chassis_ctrl.spd_yaw_w;
		Chassis_MOTO[1].set_speed =  RoboData.chassis_ctrl.spd_right; //+ RoboData.chassis_ctrl.spd_yaw_w;
	}
	else{
		Chassis_MOTO[0].set_speed=0;
		Chassis_MOTO[1].set_speed=0;
		Chassis_MOTO[2].set_speed=0;
		Chassis_MOTO[3].set_speed=0;
	}
}


/*-------------------------------PowerControl--------------------------------------------*/
float  a_bool;

void PowerControl(void)
{
	if(overall_mode.chassiss_control_mode == SPEED)
	{
		int i=0;
		for(i=0; i<2; i++)
		{
			if(Chassis_MOTO[i].getpara.speed_rpm*Chassis_MOTO[i].set_speed<0)
			Chassis_MOTO[i].pid_speed.max_output = 16000;
			else
				Chassis_MOTO[i].pid_speed.max_output = 2500;
			/*速度环_&_电机输出信号赋值*/
			Chassis_MOTO[i].send_current = Chassis_MOTO[i].pid_speed.PidCalc(&Chassis_MOTO[i].pid_speed, Chassis_MOTO[i].getpara.speed_rpm, Chassis_MOTO[i].set_speed);	  
		}
	}
	else if(overall_mode.chassiss_control_mode == POSITION)
	{
			if(Chassis_MOTO[0].getpara.speed_rpm*Chassis_MOTO[0].set_speed<0)
				Chassis_MOTO[0].pid_speed.max_output = 16000;
			else
				Chassis_MOTO[0].pid_speed.max_output = 2500;
		
		a_bool = PidCalc(&Chassis_MOTO[0].pid_angle,Chassis_MOTO[0].getpara.xiangdui_round,chassis_positon);
		if(a_bool>=0) a_bool=1;
		else a_bool=(-1);
		
		Chassis_MOTO[0].send_current = PidCalc(&Chassis_MOTO[0].pid_speed,Chassis_MOTO[0].getpara.speed_rpm,a_bool*6000);
		
	}

}












/*-------------------------------Yaw--------------------------------------------*/




void YawCloudDeck6623_ctr(Moto_t *MOTO, float the_get_speed, float the_set_speed, float the_set_angle)		//1000hz
{
	MOTO->getpara.other_speed = the_get_speed*0.2f + 0.8f*MOTO->getpara.other_speed;	//度每秒
	MOTO->set_angle = the_set_angle;
	//电机限速
	MOTO->pid_angle.PidSet_limit(&(MOTO->pid_angle), ABS(the_set_speed), MOTO->pid_angle.integral_uplimit, MOTO->pid_angle.integral_downlimit, 0);
	//角度环相关参数赋值_&_角度环_&_电机速度信号赋值
	MOTO->pid_angle.PidSet_errband(&(MOTO->pid_angle), 30, 3, 0, 0);			//8192/360 = 22.756 
	MOTO->set_speed = MOTO->pid_angle.PidCalc(&(MOTO->pid_angle),Yaw_Change.angle, the_set_angle);	
	//速度环相关参数赋值_&_速度环_&_电机输出信号赋值
	MOTO->pid_speed.PidSet_errband(&(MOTO->pid_speed), 15,3, 0, 0);
	MOTO->pid_speed.PidCalc(&(MOTO->pid_speed), -yaw_speed, MOTO->set_speed);		
	MOTO->send_current = MOTO->pid_speed.output.pos_out;
	ABSLimit(&(MOTO->send_current), 25000);
	MOTO->set_speed_last = MOTO->set_speed;
	MOTO->set_angle_last = MOTO->set_angle;
}

void YawMotoCtrl(Moto_t *MOTO, float the_set_speed, float the_set_angle)
{
//	if(the_set_angle != 0){
//		if(the_set_angle < yaw6623_RightLimit+5)
//			the_set_angle = yaw6623_RightLimit;
//		else if(the_set_angle > yaw6623_LeftLimit-5)
//			the_set_angle = yaw6623_LeftLimit;
//	}
//	else the_set_angle = yaw6623_MiddleAngle;
	//获取云台板载IMU数据
//	IMU_Get_Data();
	YawCloudDeck6623_ctr(MOTO, yaw_speed, the_set_speed, the_set_angle);
}




/*-------------------------------AmmunitionToggle--------------------------------------------*/
void AmmunitionToggle2006(Moto_t *MOTO, float the_get_speed, float the_set_speed, float the_set_angle)		//1000hz
{
	MOTO->getpara.other_speed = the_get_speed*0.7f + 0.3f*MOTO->getpara.other_speed;//rpm,传感器在中频时准确

	//电机角度信号赋值
	MOTO->set_angle = the_set_angle;
	//电机限速
	MOTO->pid_angle.PidSet_limit(&(MOTO->pid_angle), ABS(the_set_speed), MOTO->pid_angle.integral_uplimit, MOTO->pid_angle.integral_downlimit, 0);
	//角度环相关参数赋值_&_角度环_&_电机速度信号赋值
	MOTO->pid_angle.PidSet_errband(&(MOTO->pid_angle), 5, 3, 0, 0);
	MOTO->set_speed = MOTO->pid_angle.PidCalc(&(MOTO->pid_angle), MOTO->getpara.total_angle, the_set_angle);	
	//速度环相关参数赋值_&_速度环_&_电机输出信号赋值
	MOTO->pid_speed.PidSet_errband(&(MOTO->pid_speed), 0.6f*the_set_speed, 0.2f*the_set_speed, 0, 0);
	MOTO->send_current = MOTO->pid_speed.PidCalc(&(MOTO->pid_speed), MOTO->getpara.real_current,the_set_speed);		

	MOTO->set_speed_last = MOTO->set_speed;
	MOTO->set_angle_last = MOTO->set_angle;
}





