#include "t_moto.h"
#include "m_imu.h"
#include "control.h"
#include "t_monitor.h"
#include "robodata.h"

extern float pitchL_angle_test;
extern float pitchR_angle_test;
extern float yaw_angle_test;

int __sgn(float x){
	if(x > 1) return 1;
	else if (x < -1) return -1;
	else return 0;
}

/*-------------------------------Chassis--------------------------------------------*/
void SetChassisSpeed(void){
	if(monitor_remote.status == monitor_regular){
		float x_axis = 0, y_axis = 0, calc_set_speed[5] = {0, 0, 0, 0, 8000};	//[6]与PID输出限幅对应
		x_axis = -RoboData.chassis_ctrl.spd_right;
		y_axis = RoboData.chassis_ctrl.spd_forward;
		calc_set_speed[1] = -(-y_axis + x_axis - RoboData.chassis_ctrl.spd_yaw_w);
		calc_set_speed[0] = -(-y_axis - x_axis - RoboData.chassis_ctrl.spd_yaw_w);
		calc_set_speed[3] = -(y_axis + x_axis - RoboData.chassis_ctrl.spd_yaw_w);
		calc_set_speed[2] = -(y_axis - x_axis - RoboData.chassis_ctrl.spd_yaw_w);
		int	i = 0;
		float k = 0;
		for(i = 0; i < 4; i++){
			calc_set_speed[4] = calc_set_speed[4] > calc_set_speed[i] ? calc_set_speed[4] : calc_set_speed[i];
			k = calc_set_speed[4]/6000.0f;
		}
		for(i = 0; i < 4; i++){
			Chassis_MOTO[i].set_speed = calc_set_speed[i] / k; 
		}
	}
	else{
		int i = 0;
		for(i = 0; i < 4; i++){
			Chassis_MOTO[i].set_speed = 0;
		}
	}
}

void PitchCloudDeck6623_ctr(Moto_t *MOTO, float up_plus, float down_plus, float the_get_speed, float the_set_speed, float the_set_angle)		//1000hz
{
//	float pitch_current_k = 0;
	if(MOTO == &Pitch_MOTO[0])
	{
//	MOTO->getpara.other_speed = the_get_speed*0.8f + 0.2f*MOTO->getpara.other_speed;	//低通滤波//度每秒
	//电机角度信号赋值
	MOTO->set_angle = the_set_angle;
	//电机限速
//	MOTO->pid_angle.PidSet_limit(&(MOTO->pid_angle), MOTO->pid_angle.max_output, MOTO->pid_angle.integral_uplimit, MOTO->pid_angle.integral_downlimit, 0);
	//角度环相关参数赋值_&_角度环_&_电机速度信号赋值
//	MOTO->pid_angle.PidSet_errband(&(MOTO->pid_angle), MOTO->pid_angle.max_err, MOTO->pid_angle.mid_err, MOTO->pid_angle.min_err, 0);			//8192/360 = 22.756 
	MOTO->set_speed = MOTO->pid_angle.PidCalc(&(MOTO->pid_angle), MOTO->getpara.total_angle, the_set_angle);	
	//速度环相关参数赋值_&_速度环
//	MOTO->pid_speed.PidSet_errband(&(MOTO->pid_speed), 0.6f*ABS(MOTO->set_speed)+1, 0.2f*ABS(MOTO->set_speed)+0.5f, 0.05f*ABS(MOTO->set_speed)+0.1f, 0);		//动态积分分离带，可以尝试
	MOTO->pid_speed.PidCalc(&(MOTO->pid_speed), the_get_speed, MOTO->set_speed);		
	//重力补偿系数
//	if(MOTO->pid_speed.output.pos_out < 0){
//		pitch_current_k = (pitch6020L_DownLimit - MOTO->getpara.total_round*360.0f +48)/(pitch6020L_DownLimit - pitch6020L_UpLimit) * down_plus;
//	}
//	else{
//		pitch_current_k = (MOTO->getpara.total_round*360.0f - pitch6020L_UpLimit +10)/(pitch6020L_DownLimit - pitch6020L_UpLimit) * up_plus;
//	}
	//电机输出信号赋值
//	MOTO->send_current = (pitch_current_k + 1) * MOTO->pid_speed.output.pos_out;
		MOTO->send_current = MOTO->pid_speed.output.pos_out;
	ABSLimit(&(MOTO->send_current),MOTO->pid_speed.max_output);
	MOTO->set_speed_last = MOTO->set_speed;
	MOTO->set_angle_last = MOTO->set_angle;
	}
	else if(MOTO == &Pitch_MOTO[1])
	{
//		MOTO->getpara.other_speed = the_get_speed*0.8f + 0.2f*MOTO->getpara.other_speed;	//低通滤波//度每秒
	//电机角度信号赋值
		MOTO->set_angle = the_set_angle;
		//电机限速
		MOTO->pid_angle.PidSet_limit(&(MOTO->pid_angle), MOTO->pid_angle.max_output, MOTO->pid_angle.integral_uplimit, MOTO->pid_angle.integral_downlimit, 0);
		//角度环相关参数赋值_&_角度环_&_电机速度信号赋值
		MOTO->pid_angle.PidSet_errband(&(MOTO->pid_angle), 6, 4, 0, 0);			//8192/360 = 22.756 
		MOTO->set_speed = MOTO->pid_angle.PidCalc(&(MOTO->pid_angle), MOTO->getpara.angle/8192.0f*360.0f, the_set_angle);	
		//速度环相关参数赋值_&_速度环
	//	MOTO->pid_speed.PidSet_errband(&(MOTO->pid_speed), 0.6f*ABS(MOTO->set_speed)+1, 0.2f*ABS(MOTO->set_speed)+0.5f, 0.05f*ABS(MOTO->set_speed)+0.1f, 0);
		MOTO->pid_speed.PidCalc(&(MOTO->pid_speed), MOTO->getpara.speed_rpm*6.0f, MOTO->set_speed);		
		//重力补偿系数
//		if(MOTO->pid_speed.output.pos_out < 0){
//			pitch_current_k = (pitch6020R_DownLimit - MOTO->getpara.total_round*360.0f +48)/(pitch6020R_DownLimit - pitch6020R_UpLimit) * down_plus;
//		}
//		else{
//			pitch_current_k = (MOTO->getpara.total_round*360.0f - pitch6020R_UpLimit +10)/(pitch6020R_DownLimit - pitch6020R_UpLimit) * up_plus;
//		}
		//电机输出信号赋值
//		MOTO->send_current = (pitch_current_k + 1) * MOTO->pid_speed.output.pos_out;
		MOTO->send_current = (0 + 1) * MOTO->pid_speed.output.pos_out;

		ABSLimit(&(MOTO->send_current), 25000);
		MOTO->set_speed_last = MOTO->set_speed;
		MOTO->set_angle_last = MOTO->set_angle;
	}
}
void PitchMotoCtrl(Moto_t *MOTO, float up_plus, float down_plus, float set_speed, float set_angle)
{
	if(MOTO == &Pitch_MOTO[0])
	{
	//设定角度限幅
//	if(set_angle != 0)
//	{
//		if(set_angle<pitch6020L_UpLimit-1)
//			set_angle = pitch6020L_UpLimit-1;//pitchL_angle_test;
//		else if(set_angle>pitch6020L_DownLimit+1)
//			set_angle = pitch6020L_DownLimit+1;
//	}
//	else set_angle = pitch6020L_MiddleAngle;
	PitchCloudDeck6623_ctr(MOTO, up_plus, down_plus, MOTO->getpara.speed_rpm, set_speed, set_angle);		
	//6020有速度反馈，不用Pitch.speed,此处getpara.speed_rpm仅供参考，需要查看电机手册进行单位换算为度/秒，注意减速比！
	}
	else if(MOTO == &Pitch_MOTO[1])
	{
	//设定角度限幅
//	if(set_angle != 0)
//	{
//		if(set_angle>pitch6020R_UpLimit+1)
//			set_angle = pitch6020R_UpLimit+1;
//		else if(set_angle<pitch6020R_DownLimit-1)
//			set_angle = pitch6020R_DownLimit-1;
//	}
//	else set_angle = pitch6020R_MiddleAngle;

	PitchCloudDeck6623_ctr(MOTO, up_plus, down_plus, MOTO->getpara.speed_rpm, set_speed, set_angle);
	}
}

/*-------------------------------Yaw6623--------------------------------------------*/
void YawCloudDeck6623_ctr(Moto_t *MOTO, float the_get_speed, float the_set_speed, float the_set_angle)		//1000hz
{
	MOTO->getpara.other_speed = the_get_speed*0.3f + 0.7f*MOTO->getpara.other_speed;	//度每秒
	MOTO->set_angle = the_set_angle;
	//电机限速
	MOTO->pid_angle.PidSet_limit(&(MOTO->pid_angle), ABS(the_set_speed), MOTO->pid_angle.integral_uplimit, MOTO->pid_angle.integral_downlimit, 0);
	//角度环相关参数赋值_&_角度环_&_电机速度信号赋值
	MOTO->pid_angle.PidSet_errband(&(MOTO->pid_angle), MOTO->pid_angle.max_err, MOTO->pid_angle.mid_err, MOTO->pid_angle.min_err, 0);			//8192/360 = 22.756 
	MOTO->set_speed = MOTO->pid_angle.PidCalc(&(MOTO->pid_angle), MOTO->getpara.total_round*360.0f, the_set_angle);			//遥控器控制问题可能出在total_round*360.f上，如果不行就改成getpara.angle/8192*360
	//速度环相关参数赋值_&_速度环_&_电机输出信号赋值
	MOTO->pid_speed.PidSet_errband(&(MOTO->pid_speed), MOTO->pid_speed.max_err, MOTO->pid_speed.mid_err, MOTO->pid_speed.min_err, 0);
	MOTO->pid_speed.PidCalc(&(MOTO->pid_speed), MOTO->getpara.other_speed, MOTO->set_speed);		
	MOTO->send_current = -MOTO->pid_speed.output.pos_out;
	ABSLimit(&(MOTO->send_current), MOTO->pid_speed.max_output);
	MOTO->set_speed_last = MOTO->set_speed;
	MOTO->set_angle_last = MOTO->set_angle;
}

void YawMoto6623Ctrl(Moto_t *MOTO, float the_set_speed, float the_set_angle)
{
	if(the_set_angle != 0){
//		if(the_set_angle < yaw6623_RightLimit+2)
//			the_set_angle = yaw6623_RightLimit+2;
//		else if(the_set_angle > yaw6623_LeftLimit-2)
//			the_set_angle = yaw6623_LeftLimit-2;
	}
//	else the_set_angle = yaw6623_MiddleAngle;
	YawCloudDeck6623_ctr(MOTO, Yaw_MOTO.getpara.speed_rpm, the_set_speed, the_set_angle);	//Yaw.speed是陀螺仪反馈，串口6回调函数中，注意单位是否为度/秒
//	YawCloudDeck6623_ctr(MOTO, imu_data.gz, the_set_speed, the_set_angle);
}

