#include "pitch.h"
#include "pid.h"
#include "t_moto.h"
#include "math.h"

Pitch__t Pitch={&Pitch_Init,&Pitch_SetAngle};
Pitch__t Pitch2={&Pitch_Init,&Pitch_SetSpeed};
Yaw_Change_t Yaw_Change;

void Pitch_SetAngle(float pitch)
{
	Pitch.angle_set = pitch;
	
	if(Pitch.angle_set>Pitch.angle_max)
		Pitch.angle_set=Pitch.angle_max;
	if(Pitch.angle_set<Pitch.angle_min)
		Pitch.angle_set=Pitch.angle_min;
	
	Pitch.speed_set=PidCalc(&Pitch.PID_angle,Pitch.angle,Pitch.angle_set);
	
	Pitch.current_set=PidCalc(&Pitch.PID_speed,-Pitch.speed,Pitch.speed_set);
	
	//Pitch.current_set+=-21*Pitch.angle-280;
}
void Pitch_SetSpeed(float SetSpeed)
{
	Pitch2.speed_set=SetSpeed;
	Pitch2.speed=Pitch.speed;
	Pitch.current_set=-PidCalc(&Pitch2.PID_speed,Pitch2.speed,Pitch2.speed_set);
}

void Pitch_Init(void)
{
	PidParaInit(&Pitch.PID_angle);
	PidParaInit(&Pitch.PID_speed);
	
	Pitch.PID_angle.PidSet_all(&Pitch.PID_angle, POSITION_PID, 1000, 200, -200, 1, 0.0005,0, 0, 0,0, 0, 0, 0);
	Pitch.PID_speed.PidSet_all(&Pitch.PID_speed, POSITION_PID, 25000, 200, -200, 70, 0.0005, 0, 0, 0, 0, 0, 0, 0);
	
//	Pitch.PID_angle.PidSet_all(&Pitch.PID_angle, POSITION_PID, 1000, 50, -50, 4.5, 0.01,0, 0, 0,0, 0, 0, 0);
//	Pitch.PID_speed.PidSet_all(&Pitch.PID_speed, POSITION_PID, 30000, 3000, -3000, 75, 0.01, 0, 0, 0, 0, 0, 0, 0);
	
	/*struct __Pid_t *pid, uint32_t mode, 
		              float max_out, float integral_uplimit, float integral_downlimit,
									float p, float i, float d, 
								  float f, float max_err, float mid_err, float min_err, float deadband, float f_out_limit);
	*/
	Pitch.angle_max=130;																																														//8+0.6
	Pitch.angle_min=-512;
}

void Pitch_Init2(void)
{
	PidParaInit(&Pitch2.PID_angle);
	PidParaInit(&Pitch2.PID_speed);
	
	Pitch2.PID_angle.PidSet_all(&Pitch2.PID_angle, POSITION_PID, 1000, 200, -200, 1, 0.0005,15, 0, 0,0, 0, 0, 0);
	Pitch2.PID_speed.PidSet_all(&Pitch2.PID_speed, POSITION_PID, 30000, 10000, -200, 50, 30, 0, 0, 8000, 0, 0, 0, 14000);
}

void YawAngle_get(void)
{
	if(fabs(Yaw_MOTO.getpara.speed_rpm)<1.0f&&fabs(yaw_speed)>4)
		Yaw_Change.ChangeSource_pending++;
	else 
		Yaw_Change.ChangeSource_pending=0;
	
	if(Yaw_Change.ChangeSource_pending!=0)
		Yaw_Change.YawAngleSource=gyro;
	else 
		Yaw_Change.YawAngleSource=encounder;
	
	
	if(Yaw_Change.YawAngleSource==gyro)
	{
			if(Yaw_Change.YawAngleSource_last==encounder)
			{	
				Yaw_Change.gyro_offsetangle=tuoluo_total_angle;
				Yaw_Change.encounder_offsetangle=Yaw_Change.angle;
			}
			Yaw_Change.angle=Yaw_Change.encounder_offsetangle-(tuoluo_total_angle-Yaw_Change.gyro_offsetangle)*8192.0f/360.0f;
	}
	else
		Yaw_Change.angle=Yaw_MOTO.getpara.xiangdui_angle;
	
	Yaw_Change.YawAngleSource_last=Yaw_Change.YawAngleSource;
	
}

void Yaw_Change_Init(void)
{
	Yaw_Change.YawAngleSource=encounder;
	Yaw_Change.YawAngleSource_last=encounder;
}




