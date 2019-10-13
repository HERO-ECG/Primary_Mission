#include "tripod.h"
#include "can.h"
#include "math.h"
#include "robodata.h"
#include "hwt901b.h"
#include "robodata.h"
#include "strike.h"
#include "chassis.h"

Tripod_t Tripod;

float yaw_test;
void Tripod_SetModeFromControl(void)
{
	static uint8_t rotateSta=0;
	static float rotateAim_angle=0;
	if(RC_CtrlData.key.key_data.G==1){
		rotateAim_angle=mode360(350+Tripod.Yaw.angle);
		rotateSta=1;
	}else{
		rotateSta=0;
	}
	if(rotateSta){
		if(fabs(Tripod.Yaw.angle-rotateAim_angle)>40.0f)
			Tripod.Yaw.angle_set += 0.2f;
		else if(fabs(Tripod.Yaw.angle-rotateAim_angle)>10.0f)
			Tripod.Yaw.angle_set += 0.04f;
		else if(fabs(Tripod.Yaw.angle-rotateAim_angle)>2.0f)
			Tripod.Yaw.angle_set += 0.02f;
		else
			rotateSta=1;
	}
	
}

static void ABSLimit(float *a, float abs_max){
	if(*a > abs_max)
		*a=abs_max;
	if(*a < -abs_max)
		*a=-abs_max;
}

float mode360(float angle)
{
	while(angle<0)
		angle+=360.0f;
	while(angle>360)
		angle-=360.0f;
	return angle;
}


float GetCoupledYawSpeedFromPitch(float pitch, float yaw_speed_raw)
{
	float coupled_yaw_speed;
	//coupled_yaw_speed=cos(Tripod.Pitch.angle/180.0*3.1415926)*yaw_speed_raw-sin(Tripod.Pitch.angle/180.0*3.1415926)*cos(Tripod.Pitch.angle/180.0*3.1415926)*Tripod.Pitch.speed;
	coupled_yaw_speed=yaw_speed_raw;
	return coupled_yaw_speed;
}


float GetIntergralYaw(float yaw_speed,uint16_t frequency)
{
	static float yaw_integral;
	yaw_integral+=(yaw_speed)/(float)frequency;
	yaw_integral=mode360(yaw_integral);
	return yaw_integral;
}





void Pitch_SetAngle(float pitch)
{
	Tripod.Pitch.angle_set=pitch;
	
	if(Tripod.Pitch.angle_set-Tripod.Pitch.angle+Tripod.Pitch.angle_source.encoder.angle_now>Tripod.Pitch.angle_max)
		Tripod.Pitch.angle_set=Tripod.Pitch.angle_max+Tripod.Pitch.angle-Tripod.Pitch.angle_source.encoder.angle_now;
	if(Tripod.Pitch.angle_set-Tripod.Pitch.angle+Tripod.Pitch.angle_source.encoder.angle_now<Tripod.Pitch.angle_min)
		Tripod.Pitch.angle_set=Tripod.Pitch.angle_min+Tripod.Pitch.angle-Tripod.Pitch.angle_source.encoder.angle_now;
	
	Tripod.Pitch.speed_set=PidCalc(&Tripod.Pitch.PID_angle,Tripod.Pitch.angle,Tripod.Pitch.angle_set);
	
	Tripod.Pitch.current_set=PidCalc(&Tripod.Pitch.PID_speed,Tripod.Pitch.speed,Tripod.Pitch.speed_set);
}



		
void Yaw_SetAngle(float angle_set)
{
	float angle_err;
//	float dangle;
//	float angle_relativeToChassis;//-180 - +180
//	float tempAngle=0;
	float temp_yaw_angle_set=0;
		 
	angle_set=mode360(angle_set);
	
//	tempAngle=mode360(Tripod.Yaw.angle_source.encoder.angle_relativeToChassis);
	
//	if(tempAngle>180)
//		angle_relativeToChassis=tempAngle-360;
//	else
//		angle_relativeToChassis=tempAngle;
	
//	dangle=Tripod.Yaw.angle_set-Tripod.Yaw.angle;
//	if(dangle>180)
//		dangle=-360+dangle;
//	else if(dangle<-180)
//		dangle=360+dangle;
	
//	Tripod.Yaw.angle_set=angle_set;
//	if(dangle+angle_relativeToChassis>Tripod.Yaw.angle_max)
//		Tripod.Yaw.angle_set=Tripod.Yaw.angle_max+Tripod.Yaw.angle-angle_relativeToChassis;
//	else if(dangle+angle_relativeToChassis<Tripod.Yaw.angle_min)
//		Tripod.Yaw.angle_set=Tripod.Yaw.angle_min+Tripod.Yaw.angle-angle_relativeToChassis;
//	Tripod.Yaw.angle_set=mode360(Tripod.Yaw.angle_set);

//  temp_yaw_angle_set=	Tripod.Yaw.angle_set;
//	if(dangle+angle_relativeToChassis>35)
//		temp_yaw_angle_set= 35+Tripod.Yaw.angle-angle_relativeToChassis;
//	else if(dangle+angle_relativeToChassis<-35)
//		temp_yaw_angle_set= -35+Tripod.Yaw.angle-angle_relativeToChassis;
//	temp_yaw_angle_set=mode360(temp_yaw_angle_set);

	temp_yaw_angle_set=	mode360(Tripod.Yaw.angle_set);
	
	if(Tripod.Yaw.angle<temp_yaw_angle_set)
	{
		if(temp_yaw_angle_set-Tripod.Yaw.angle<180)
			angle_err=temp_yaw_angle_set-Tripod.Yaw.angle;
		else
			angle_err=(temp_yaw_angle_set-Tripod.Yaw.angle)-360;
	}
	else
	{
		if(Tripod.Yaw.angle-temp_yaw_angle_set<180)
			angle_err=temp_yaw_angle_set-Tripod.Yaw.angle;
		else
			angle_err=360-(Tripod.Yaw.angle-temp_yaw_angle_set);
	}
		
	Tripod.Yaw.speed_set=PidCalc(&Tripod.Yaw.PID_angle,-angle_err,0);
	Tripod.Yaw.current_set=PidCalc(&Tripod.Yaw.PID_speed,Tripod.Yaw.speed,Tripod.Yaw.speed_set);
}


void Yaw_SetSpeed(float speed)
{
	ABSLimit(&Tripod.Yaw.speed_set,Tripod.Yaw.PID_angle.max_output);
	Tripod.Yaw.current_set=PidCalc(&Tripod.Yaw.PID_speed,Tripod.Yaw.speed,Tripod.Yaw.speed_set);
}

void Pitch_SetSpeed(float speed)
{
	ABSLimit(&Tripod.Pitch.speed_set,Tripod.Pitch.PID_angle.max_output);
	Tripod.Pitch.current_set=PidCalc(&Tripod.Pitch.PID_speed,Tripod.Pitch.speed,Tripod.Pitch.speed_set);
}

	
void GetYawAngle(void)
{
	static uint32_t delaytimer;
	//float coupled_yaw_speed;
	//coupled_yaw_speed=GetCoupledYawSpeedFromPitch(Tripod.Pitch.angle,Tripod.Yaw.speed);//yaw角积分运算
	//Tripod.Yaw.angle_source.gyro.angle_now=GetIntergralYaw(coupled_yaw_speed,1000);	

//	toEulerAngle(rm_imu_data.quat_fp32,&rm_imu_data.roll,&rm_imu_data.pitch,&rm_imu_data.yaw);//四元数转欧拉角
//	Tripod.Yaw.speed = rm_imu_data.gyro_int16[2];//左正
//	Tripod.Yaw.angle_source.gyro.angle_now = rm_imu_data.yaw;//左正
//	Tripod.Pitch.speed = rm_imu_data.gyro_int16[1];//下正
//	Tripod.Pitch.angle_source.gyro.angle_now = rm_imu_data.pitch;//下正
//	Tripod.Pitch.angle = (Tripod.Pitch.angle_source.gyro.angle_now);

	Tripod.Yaw.speed = imu_data.gz*GYRO_2000_SEN*5;//左正
	Tripod.Yaw.angle_source.gyro.angle_now=GetIntergralYaw(Tripod.Yaw.speed,10000);	//左正
	Tripod.Pitch.speed = -imu_data.gx*GYRO_2000_SEN*5;//-下负
	Tripod.Pitch.angle_source.gyro.angle_now = -imu_data.angley*0.05f + 0.95f*Tripod.Pitch.angle_source.gyro.angle_now;//-下负
	
	Tripod.Pitch.angle = (Tripod.Pitch.angle_source.encoder.angle_now);//下正
	
//	{//有关底盘陀螺仪的角度解算1000hz
//		RoboData.chassis_ctrl.pitch_speed_gyro = RoboData.chassis_ctrl.pitch_speed_gyro*0.95f+imu_data.gy*0.05f;
//		RoboData.chassis_ctrl.yaw_speed_gyro = RoboData.chassis_ctrl.yaw_speed_gyro*0.95f+imu_data.gz*0.05f;
//		RoboData.chassis_ctrl.roll_speed_gyro = RoboData.chassis_ctrl.roll_speed_gyro*0.95f+imu_data.gx*0.05f;
//		
//		RoboData.chassis_ctrl.pitch_angle_gyro += RoboData.chassis_ctrl.pitch_speed_gyro*0.001f;
//	}
	
	if(fabs(RoboData.chassis_ctrl.yaw_speed_gyro)<10 && fabs(Chassis.speed.spd_yaw_actual)<0.1f)
	{
		if(delaytimer<0xffff)
			delaytimer++;
	}
	else 
		delaytimer=0;
	if(delaytimer>500) //编码器
	{
		Tripod.Yaw.angle_source.now=From_Encoder;
		if(Tripod.Yaw.angle_source.last==From_Gyro)
		{
			Tripod.Yaw.angle_source.encoder.angle_offset=mode360(Tripod.Yaw.angle_source.encoder.angle_now-Tripod.Yaw.angle);
		}
		Tripod.Yaw.angle=Tripod.Yaw.angle_source.encoder.angle_now-Tripod.Yaw.angle_source.encoder.angle_offset;
	}
	else //陀螺仪
	{
		{
			Tripod.Yaw.angle_source.now=From_Gyro;
			if(Tripod.Yaw.angle_source.last==From_Encoder)
			{
				Tripod.Yaw.angle_source.gyro.angle_offset=mode360(Tripod.Yaw.angle_source.gyro.angle_now-Tripod.Yaw.angle);
			}
			Tripod.Yaw.angle=Tripod.Yaw.angle_source.gyro.angle_now-Tripod.Yaw.angle_source.gyro.angle_offset;
		}
	}
	Tripod.Yaw.angle=mode360(Tripod.Yaw.angle);
	Tripod.Yaw.angle_source.last=Tripod.Yaw.angle_source.now;
}


void Tripod_Fun(void)
{
	float yaw_setspeed;
	float pitch_setspeed;
	static uint32_t InitCounter;
	static Key_Data_t key_last;

	
	if(Tripod.Pitch.ready_flag==0&&Tripod.Pitch.angle!=0)
		Tripod.Pitch.ready_flag=1;
	if(Yaw_MOTO.getpara.init_sta==1)
		Tripod.Yaw.ready_flag=1;
	

	GetYawAngle();
	
	if(InitCounter==799)
	{
		float angle_relativeToChassis,tempAngle=0;//-180 - +180
		tempAngle=mode360(Tripod.Yaw.angle_source.encoder.angle_relativeToChassis);
		if(tempAngle>180)
			angle_relativeToChassis=tempAngle-360;
		else
			angle_relativeToChassis=tempAngle;
		Tripod.Yaw.angle_set=Tripod.Yaw.angle-angle_relativeToChassis;
	}
	if(InitCounter<800)
	{
		InitCounter++;
		return;
	}
	

	if(RoboData.robo_ctrlmode.ctrl_source==FROM_REMOTE||RoboData.robo_ctrlmode.ctrl_source==RIGHT_reserve)
	{
		Tripod.control_mode.now=Position;
		Tripod.Pitch.angle_set-=RoboData.tripod_ctrl.pitch_angle/400.0f;
		Tripod.Yaw.angle_set+=RoboData.tripod_ctrl.yaw_angle/400.0f;
	}
	else if(RoboData.robo_ctrlmode.ctrl_source==FROM_PC)
	{
		float ky=0.005f;//pitch
		float kx=0.01f;//yaw
		if(RC_CtrlData.key.key_data.Q==1&&key_last.Q==0)
		{
			Tripod.Yaw.angle_set=Tripod.Yaw.angle+180;
		}
		if(RC_CtrlData.key.key_data.F==1&&key_last.F==0)
		{
			Tripod.Yaw.angle_set=Tripod.Yaw.angle;
		}
		
		Tripod.control_mode.now=Position;
		Tripod.Pitch.angle_set += RC_CtrlData.mouse.y_speed*ky;
		Tripod.Yaw.angle_set += -RC_CtrlData.mouse.x_speed*kx;
		

	}
	
//	else if(RoboData.robo_ctrlmode.ctrl_source==FROM_PC&&
//		(Strike.strikemode==ManualAim_AutoStrike||Strike.strikemode==ManualAim_ManualStrike))
//	{
//		Tripod.control_mode.now=Position;
//		
//		Tripod_PC_Ctrl(-(float)(RC_CtrlData.mouse.y_speed)*0.008f,-((float)(RC_CtrlData.mouse.x_speed))*0.01f);
//	}
//	else if(RoboData.robo_ctrlmode.ctrl_source==FROM_PC&&
//		(Strike.strikemode==AutoAim_AutoStrike||Strike.strikemode==AutoAim_ManualStrike))
//	{
//		Tripod.control_mode.now=Position;
//		Tripod_PC_Ctrl(-(float)(RC_CtrlData.mouse.y_speed)*0.008f,-((float)(RC_CtrlData.mouse.x_speed))*0.01f);
//	}
	
	if(Tripod.control_mode.last==Speed&&Tripod.control_mode.now==Position)
	{
		Tripod.Yaw.angle_set=Tripod.Yaw.angle;
		Tripod.Pitch.angle_set=Tripod.Pitch.angle;
	}
//	Tripod_SetModeFromControl();
	


	if(Tripod.control_mode.now==Position)
	{	
		if(Tripod.Pitch.ready_flag==1)
			Pitch_SetAngle(Tripod.Pitch.angle_set);
		if(Tripod.Yaw.ready_flag==1)	
			Yaw_SetAngle(Tripod.Yaw.angle_set);
	}
	else if(Tripod.control_mode.now==Speed)
	{
		if(Tripod.Pitch.ready_flag==1)
			Pitch_SetSpeed(pitch_setspeed);
		if(Tripod.Yaw.ready_flag==1)	
			Yaw_SetSpeed(yaw_setspeed);	
	}

	Tripod.control_mode.last=Tripod.control_mode.now;
	
}


void Yaw_Init(void)
{
	Tripod.Yaw.PID_angle.pid_mode=POSITION_PID;
	Tripod.Yaw.PID_angle.max_output=400;
	Tripod.Yaw.PID_angle.integral_uplimit=50;
	Tripod.Yaw.PID_angle.integral_downlimit=-50;
	Tripod.Yaw.PID_angle.p=3;
	Tripod.Yaw.PID_angle.i=0;
	Tripod.Yaw.PID_angle.d=0;
	Tripod.Yaw.PID_angle.deadband=0;
	Tripod.Yaw.PID_angle.f=0;
	Tripod.Yaw.PID_angle.max_err=5;
	Tripod.Yaw.PID_angle.mid_err=2;
	Tripod.Yaw.PID_angle.min_err=0;
	Tripod.Yaw.PID_angle.f_out_limit=0;
	
	Tripod.Yaw.PID_speed.pid_mode=POSITION_PID;
	Tripod.Yaw.PID_speed.max_output=30000;
	Tripod.Yaw.PID_speed.integral_uplimit=5000;
	Tripod.Yaw.PID_speed.integral_downlimit=-5000;
	Tripod.Yaw.PID_speed.p=800;
	Tripod.Yaw.PID_speed.i=0.051;
	Tripod.Yaw.PID_speed.d=2500;
	Tripod.Yaw.PID_speed.deadband=0;
	Tripod.Yaw.PID_speed.f=0;
	Tripod.Yaw.PID_speed.max_err=1000;
	Tripod.Yaw.PID_speed.mid_err=100;
	Tripod.Yaw.PID_speed.min_err=0;
	Tripod.Yaw.PID_speed.f_out_limit=10000;
}

void Pitch_Init(void)
{

	Tripod.Pitch.PID_angle.pid_mode=POSITION_PID;
	Tripod.Pitch.PID_angle.max_output=1000;
	Tripod.Pitch.PID_angle.integral_uplimit=50;
	Tripod.Pitch.PID_angle.integral_downlimit=-50;
	Tripod.Pitch.PID_angle.p=3;
	Tripod.Pitch.PID_angle.i=0.05;
	Tripod.Pitch.PID_angle.d=0;
	Tripod.Pitch.PID_angle.deadband=0;
	Tripod.Pitch.PID_angle.f=0;
	Tripod.Pitch.PID_angle.max_err=5;
	Tripod.Pitch.PID_angle.mid_err=3;
	Tripod.Pitch.PID_angle.min_err=0;
	Tripod.Pitch.PID_angle.f_out_limit=0;
	
	Tripod.Pitch.PID_speed.pid_mode=POSITION_PID;
	Tripod.Pitch.PID_speed.max_output=30000;
	Tripod.Pitch.PID_speed.integral_uplimit=10000;
	Tripod.Pitch.PID_speed.integral_downlimit=-10000;
	Tripod.Pitch.PID_speed.p=400;
	Tripod.Pitch.PID_speed.i=0;
	Tripod.Pitch.PID_speed.d=200;
	Tripod.Pitch.PID_speed.deadband=0;
	Tripod.Pitch.PID_speed.f=0;
	Tripod.Pitch.PID_speed.max_err=500;
	Tripod.Pitch.PID_speed.mid_err=100;
	Tripod.Pitch.PID_speed.min_err=0;
	Tripod.Pitch.PID_speed.f_out_limit=5000;

	Tripod.Pitch.angle_max=15;
	Tripod.Pitch.angle_min=-35;
}


void Tripod_Init(void)
{
	Tripod.control_mode.now=Position;
	Tripod.Yaw.angle_source.now=From_Encoder;
	Tripod.Yaw.angle_source.last=From_Encoder;
	Pitch_Init();
	Yaw_Init();
}











