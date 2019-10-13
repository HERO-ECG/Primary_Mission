#include "tripod.h"
#include "can.h"
#include "math.h"
#include "robodata.h"
#include "hwt901b.h"
#include "robodata.h"
#include "strike.h"
#include "usart.h"

Tripod_t Tripod;


static void ABSLimit(float *a, float abs_max)
{
	if(*a > abs_max)
		*a=abs_max;
	if(*a < -abs_max)
		*a=-abs_max;
}

//将角度限制在360度范围内
float mode360(float angle)
{
	static float angle1;
	angle1=angle;
	while(angle1<0.0f)
	{
		angle1=angle1+360.0f;
	}
	while(angle1>=360.0f)
	{
		angle1=angle1-360.0f;
	}
	return angle1;
}

//获取解耦的YAW速度，应该是yaw在水平面的旋转速度，用于通过积分计算水平旋转的角度
float GetCoupledYawSpeedFromPitch(float pitch, float yaw_speed_raw)
{
	float coupled_yaw_speed;
	//coupled_yaw_speed=cos(Tripod.Pitch.angle/180.0*3.1415926)*yaw_speed_raw-sin(Tripod.Pitch.angle/180.0*3.1415926)*cos(Tripod.Pitch.angle/180.0*3.1415926)*Tripod.Pitch.speed;
	coupled_yaw_speed=cos(Tripod.Pitch.angle_source.encoder.angle_relativeToChassis/180.0*3.1415926)*yaw_speed_raw;
	 //coupled_yaw_speed=yaw_speed_raw;
	return coupled_yaw_speed;
}

//获取YAW速度的积分，获得角度
float GetIntergralYaw(float yaw_speed,uint16_t frequency)
{
	static float yaw_integral;
	yaw_integral+=(yaw_speed)/(float)frequency;
	yaw_integral=mode360(yaw_integral);
	return yaw_integral;
}

//获取Pitch速度的积分，获得角度
float GetIntergralPitch(float Pitch_speed,uint16_t frequency)
{
	static float Pitch_integral;
	Pitch_integral+=(Pitch_speed)/(float)frequency;
	Pitch_integral=mode360(Pitch_integral);
	return Pitch_integral;
}


void Pitch_SetAngle(float angle_set)
{
		float angle_err;
		float angle_relativeToChassis;//-180 - +180
		float dangle;

	
	angle_set=mode360(angle_set);
	if(Tripod.Pitch.angle_source.encoder.angle_relativeToChassis>180)
		angle_relativeToChassis=Tripod.Pitch.angle_source.encoder.angle_relativeToChassis-360;
	else
		angle_relativeToChassis=Tripod.Pitch.angle_source.encoder.angle_relativeToChassis;
	
	dangle=angle_set-Tripod.Pitch.angle;
	if(dangle>180)
		dangle=dangle-360;
	else if(dangle<-180)
		dangle=360+dangle;


	Tripod.Pitch.angle=mode360(Tripod.Pitch.angle);
	
	Tripod.Pitch.angle_set=angle_set;
	if(dangle+angle_relativeToChassis>Tripod.Pitch.angle_max)
		Tripod.Pitch.angle_set=Tripod.Pitch.angle_max+Tripod.Pitch.angle-angle_relativeToChassis;
	else if(dangle+angle_relativeToChassis<Tripod.Pitch.angle_min)
		Tripod.Pitch.angle_set=Tripod.Pitch.angle_min+Tripod.Pitch.angle-angle_relativeToChassis;

	
	Tripod.Pitch.angle_set=mode360(Tripod.Pitch.angle_set);

//	if(Tripod.Pitch.angle_set>180)
//		Tripod.Pitch.angle_set=Tripod.Pitch.angle_set-360;
	
	if(Tripod.Pitch.angle<Tripod.Pitch.angle_set)
	{
		if(Tripod.Pitch.angle_set-Tripod.Pitch.angle<180)
			angle_err=Tripod.Pitch.angle_set-Tripod.Pitch.angle;
		else
			angle_err=(Tripod.Pitch.angle_set-Tripod.Pitch.angle)-360;
	}
	else
	{
		if(Tripod.Pitch.angle-Tripod.Pitch.angle_set<180)
			angle_err=Tripod.Pitch.angle_set-Tripod.Pitch.angle;
		else
			angle_err=360-(Tripod.Pitch.angle-Tripod.Pitch.angle_set);
	}
	

	if(Tripod.Pid_Mode.now==1)
	{
		if(Tripod.Pid_Mode.last==2)
			PidPITransfer(&Tripod.Pitch.PID2_angle,&Tripod.Pitch.PID1_angle);
		Tripod.Pitch.speed_set=PidCalc(&Tripod.Pitch.PID1_angle,-angle_err,0);
		Tripod.Pitch.current_set=PidCalc(&Tripod.Pitch.PID_speed,Tripod.Pitch.speed,Tripod.Pitch.speed_set);
	}
	else
	{
		if(Tripod.Pid_Mode.last==1)
			PidPITransfer(&Tripod.Pitch.PID1_angle,&Tripod.Pitch.PID2_angle);
		Tripod.Pitch.speed_set=PidCalc(&Tripod.Pitch.PID2_angle,-angle_err,0);
		Tripod.Pitch.current_set=PidCalc(&Tripod.Pitch.PID_speed,Tripod.Pitch.speed,Tripod.Pitch.speed_set);
	}
	

}

//YAW轴控制
void Yaw_SetAngle(float angle_set)
{
	float angle_err;

	Tripod.Yaw.angle_set=mode360(angle_set);
	if(Tripod.Yaw.angle<Tripod.Yaw.angle_set)
	{
		if(Tripod.Yaw.angle_set-Tripod.Yaw.angle<180)
			angle_err=Tripod.Yaw.angle_set-Tripod.Yaw.angle;
		else
			angle_err=(Tripod.Yaw.angle_set-Tripod.Yaw.angle)-360;
	}
	else
	{
		if(Tripod.Yaw.angle-Tripod.Yaw.angle_set<180)
			angle_err=Tripod.Yaw.angle_set-Tripod.Yaw.angle;
		else
			angle_err=360-(Tripod.Yaw.angle-Tripod.Yaw.angle_set);
	}
	
	
//	Tripod.Yaw.speed_set=PidCalc(&Tripod.Yaw.PID1_angle,-angle_err,0);
//	
//	//Tripod.Yaw.current_set=PidCalc(&Tripod.Yaw.PID_speed,Tripod.Yaw.speed-RoboData.chassis_ctrl.yaw_speed_gyro,Tripod.Yaw.speed_set);	Tripod.Yaw.current_set=PidCalc(&Tripod.Yaw.PID_speed,Tripod.Yaw.speed,Tripod.Yaw.speed_set);
//	Tripod.Yaw.current_set=PidCalc(&Tripod.Yaw.PID_speed,Tripod.Yaw.speed,Tripod.Yaw.speed_set);
//	//Tripod.Yaw.current_set=PidCalc(&Tripod.Yaw.PID_speed,Tripod.Yaw.speed,Tripod.Yaw.speed_set);
//	
//	
		if(Tripod.Pid_Mode.now==1)
	{
		if(Tripod.Pid_Mode.last==2)
			PidPITransfer(&Tripod.Yaw.PID2_angle,&Tripod.Yaw.PID1_angle);
		Tripod.Yaw.speed_set=PidCalc(&Tripod.Yaw.PID1_angle,-angle_err,0);
		Tripod.Yaw.current_set=PidCalc(&Tripod.Yaw.PID_speed,Tripod.Yaw.speed,Tripod.Yaw.speed_set);
	}
	else
	{
		if(Tripod.Pid_Mode.last==1)
			PidPITransfer(&Tripod.Yaw.PID1_angle,&Tripod.Yaw.PID2_angle);
		Tripod.Yaw.speed_set=PidCalc(&Tripod.Yaw.PID2_angle,-angle_err,0);
		Tripod.Yaw.current_set=PidCalc(&Tripod.Yaw.PID_speed,Tripod.Yaw.speed,Tripod.Yaw.speed_set);
	}
	
	

}

//Yaw轴速度控制
void Yaw_SetSpeed(float speed)
{
	ABSLimit(&Tripod.Yaw.speed_set,Tripod.Yaw.PID1_angle.max_output);
	Tripod.Yaw.current_set=PidCalc(&Tripod.Yaw.PID_speed,Tripod.Yaw.speed,Tripod.Yaw.speed_set);
}

void Pitch_SetSpeed(float speed)
{
		
	ABSLimit(&Tripod.Pitch.speed_set,Tripod.Pitch.PID1_angle.max_output);
		

	Tripod.Pitch.current_set=PidCalc(&Tripod.Pitch.PID_speed,Tripod.Pitch.speed,Tripod.Pitch.speed_set);
}

//获得YAW的角度，通过陀螺仪积分计算	
void GetPitchAngle1000Hz(void)
{
	static uint32_t delaytimer;
	uint8_t encoder_windmill=0;
	Tripod.Pitch.angle_source.gyro.angle_relativeToChassis=GetIntergralPitch(Tripod.Pitch.speed,1000);	
	Tripod.Pitch.angle_source.gyro.angle_relativeToChassis=mode360(Tripod.Pitch.angle_source.gyro.angle_relativeToChassis);
	Tripod.Pitch.angle_source.encoder.angle_relativeToChassis=mode360(Tripod.Pitch.angle_source.encoder.angle_relativeToChassis);

	Tripod.Pitch.angle_source.last=Tripod.Pitch.angle_source.now;//
	
	//底盘旋转速度为0，
	if(fabs(RoboData.chassis_ctrl.pitch_speed_gyro)<3)
	{
		if(delaytimer<0xffff)
			delaytimer++;
	}
	else 
		delaytimer=0;
	
	if(Strike.strikemode==WindMill_Clkwise||Strike.strikemode==WindMill_Anticlkwise||Strike.strikemode==WindMill_Still||Strike.strikemode==WindMill_Manual)
	{
		encoder_windmill=1;
		
	}
	if(delaytimer>400||encoder_windmill) //编码器
	{
		Tripod.Pitch.angle_source.now=From_Encoder;
		if(Tripod.Pitch.angle_source.last==From_Gyro)
		{
			Tripod.Pitch.angle_source.encoder.angle_offset=Tripod.Pitch.angle-Tripod.Pitch.angle_source.encoder.angle_relativeToChassis;
			Tripod.Pitch.angle_source.encoder.angle_offset=mode360(Tripod.Pitch.angle_source.encoder.angle_offset);
		}
		if(encoder_windmill)
			Tripod.Pitch.angle_source.encoder.angle_offset=0;
		
		Tripod.Pitch.angle=Tripod.Pitch.angle_source.encoder.angle_relativeToChassis+Tripod.Pitch.angle_source.encoder.angle_offset;
	}
	else //陀螺仪
	{
		Tripod.Pitch.angle_source.now=From_Gyro;
		if(Tripod.Pitch.angle_source.last==From_Encoder)
		{
			Tripod.Pitch.angle_source.gyro.angle_offset=Tripod.Pitch.angle-Tripod.Pitch.angle_source.gyro.angle_relativeToChassis;
			Tripod.Pitch.angle_source.gyro.angle_offset=mode360(Tripod.Pitch.angle_source.gyro.angle_offset);
		}
		Tripod.Pitch.angle=Tripod.Pitch.angle_source.gyro.angle_relativeToChassis+Tripod.Pitch.angle_source.gyro.angle_offset;
	

	}
	Tripod.Pitch.angle=mode360(Tripod.Pitch.angle);
	
}

//获得YAW的角度，通过陀螺仪积分计算	
void GetYawAngle1000Hz(void)
{
	static uint32_t delaytimer;
	static uint8_t encoder_windmill_last;
	float coupled_yaw_speed;
	uint8_t encoder_windmill=0;
	
//	coupled_yaw_speed=GetCoupledYawSpeedFromPitch(Tripod.Pitch.angle,Tripod.Yaw.speed);//yaw角积分运算
	coupled_yaw_speed=Tripod.Yaw.speed;
	
	Tripod.Yaw.angle_source.gyro.angle_relativeToChassis=GetIntergralYaw(coupled_yaw_speed,1000);	
	Tripod.Yaw.angle_source.gyro.angle_relativeToChassis=mode360(Tripod.Yaw.angle_source.gyro.angle_relativeToChassis);
	Tripod.Yaw.angle_source.encoder.angle_relativeToChassis=mode360(Tripod.Yaw.angle_source.encoder.angle_relativeToChassis);
	
	Tripod.Yaw.angle_source.last=Tripod.Yaw.angle_source.now;//
	
	//底盘旋转速度为0，并且旋转控制速度为0，
	if(fabs(RoboData.chassis_ctrl.yaw_speed_gyro)<1.5f && RoboData.chassis_ctrl.spd_yaw_w==0)
	{
		if(delaytimer<0xffff)
			delaytimer++;
	}
	else 
		delaytimer=0;
	
if((TX2_Data.mode_now==TX2_Data.mode_set&&TX2_Data.mode_now==WINDMILL_ANTICLKWISE)||(Strike.strikemode==WindMill_Manual))
	{
		encoder_windmill=1;
	}
		if(delaytimer>400||encoder_windmill) //编码器
	{
		Tripod.Yaw.angle_source.now=From_Encoder;
		if(Tripod.Yaw.angle_source.last==From_Gyro)
		{
			Tripod.Yaw.angle_source.encoder.angle_offset=Tripod.Yaw.angle-Tripod.Yaw.angle_source.encoder.angle_relativeToChassis;
			Tripod.Yaw.angle_source.encoder.angle_offset=mode360(Tripod.Yaw.angle_source.encoder.angle_offset);
		}
		
	if(encoder_windmill)
		{
			if(encoder_windmill_last==0)
			{
				Tripod.Yaw.angle_set=Tripod.Yaw.angle_source.encoder.angle_relativeToChassis;
			}
			Tripod.Yaw.angle_source.encoder.angle_offset=0;
			Tripod.Yaw.angle=Tripod.Yaw.angle_source.encoder.angle_relativeToChassis+Tripod.Yaw.angle_source.encoder.angle_offset;
			if(Tripod.Yaw.angle>180)
				Tripod.Yaw.angle=Tripod.Yaw.angle-360;
		}
		else
			Tripod.Yaw.angle=Tripod.Yaw.angle_source.encoder.angle_relativeToChassis+Tripod.Yaw.angle_source.encoder.angle_offset;
	}
	else //陀螺仪
	{
			Tripod.Yaw.angle_source.now=From_Gyro;
			if(Tripod.Yaw.angle_source.last==From_Encoder)
			{
				Tripod.Yaw.angle_source.gyro.angle_offset=Tripod.Yaw.angle-Tripod.Yaw.angle_source.gyro.angle_relativeToChassis;
				Tripod.Yaw.angle_source.gyro.angle_offset=mode360(Tripod.Yaw.angle_source.gyro.angle_offset);
			}
			Tripod.Yaw.angle=Tripod.Yaw.angle_source.gyro.angle_relativeToChassis+Tripod.Yaw.angle_source.gyro.angle_offset;
		

	}

	Tripod.Yaw.angle=mode360(Tripod.Yaw.angle);
	encoder_windmill_last=encoder_windmill;
}


void Yaw_Init(void)
{
	Tripod.Yaw.PID1_angle.pid_mode=POSITION_PID;
	Tripod.Yaw.PID1_angle.max_output=700;
	Tripod.Yaw.PID1_angle.integral_uplimit=50;
	Tripod.Yaw.PID1_angle.integral_downlimit=-50;
	Tripod.Yaw.PID1_angle.p=15;
	Tripod.Yaw.PID1_angle.i=0;
	Tripod.Yaw.PID1_angle.d=0;
	Tripod.Yaw.PID1_angle.deadband=0;
	Tripod.Yaw.PID1_angle.f=0;
	Tripod.Yaw.PID1_angle.max_err=20;//5;
	Tripod.Yaw.PID1_angle.mid_err=5;//2;
	Tripod.Yaw.PID1_angle.min_err=0;
	Tripod.Yaw.PID1_angle.f_out_limit=0;
	Tripod.Yaw.PID1_angle.f_divider=0;
	
	Tripod.Yaw.PID_speed.pid_mode=POSITION_PID;
	Tripod.Yaw.PID_speed.max_output=30000;
	Tripod.Yaw.PID_speed.integral_uplimit=3000;
	Tripod.Yaw.PID_speed.integral_downlimit=-3000;
	Tripod.Yaw.PID_speed.p=200;//120;
	Tripod.Yaw.PID_speed.i=1;//2;
	Tripod.Yaw.PID_speed.d=0;
	Tripod.Yaw.PID_speed.deadband=0;
	Tripod.Yaw.PID_speed.f=0;
	Tripod.Yaw.PID_speed.max_err=60;
	Tripod.Yaw.PID_speed.mid_err=30;
	Tripod.Yaw.PID_speed.min_err=0;
	Tripod.Yaw.PID_speed.f_out_limit=500;
	Tripod.Yaw.PID_speed.f_divider=500;
	
	Tripod.Yaw.PID2_angle.pid_mode=POSITION_PID;
	Tripod.Yaw.PID2_angle.max_output=700;
	Tripod.Yaw.PID2_angle.integral_uplimit=50;
	Tripod.Yaw.PID2_angle.integral_downlimit=-50;
	Tripod.Yaw.PID2_angle.p=15;
	Tripod.Yaw.PID2_angle.i=0.01;
	Tripod.Yaw.PID2_angle.d=0;
	Tripod.Yaw.PID2_angle.deadband=0;
	Tripod.Yaw.PID2_angle.f=0;
	Tripod.Yaw.PID2_angle.max_err=2;//5;
	Tripod.Yaw.PID2_angle.mid_err=1;//2;
	Tripod.Yaw.PID2_angle.min_err=0;
	Tripod.Yaw.PID2_angle.f_out_limit=0;
	Tripod.Yaw.PID2_angle.f_divider=0;
}

void Pitch_Init(void)
{

	Tripod.Pitch.PID1_angle.pid_mode=POSITION_PID;
	Tripod.Pitch.PID1_angle.max_output=600;
	Tripod.Pitch.PID1_angle.integral_uplimit=20;
	Tripod.Pitch.PID1_angle.integral_downlimit=-20;
	Tripod.Pitch.PID1_angle.p=15;
	Tripod.Pitch.PID1_angle.i=0;
	Tripod.Pitch.PID1_angle.d=0;
	Tripod.Pitch.PID1_angle.deadband=0;
	Tripod.Pitch.PID1_angle.f=0;
	Tripod.Pitch.PID1_angle.max_err=3;
	Tripod.Pitch.PID1_angle.mid_err=1;
	Tripod.Pitch.PID1_angle.min_err=0;
	Tripod.Pitch.PID1_angle.f_out_limit=0;
	Tripod.Pitch.PID1_angle.f_divider=0;
	
	Tripod.Pitch.PID_speed.pid_mode=POSITION_PID;
	Tripod.Pitch.PID_speed.max_output=30000;
	Tripod.Pitch.PID_speed.integral_uplimit=6000;
	Tripod.Pitch.PID_speed.integral_downlimit=-6000;
	Tripod.Pitch.PID_speed.p=200;//95;
	Tripod.Pitch.PID_speed.i=1;
	Tripod.Pitch.PID_speed.d=0;
	Tripod.Pitch.PID_speed.deadband=0;
	Tripod.Pitch.PID_speed.f=0;
	Tripod.Pitch.PID_speed.max_err=60;
	Tripod.Pitch.PID_speed.mid_err=30;
	Tripod.Pitch.PID_speed.min_err=0;
	Tripod.Pitch.PID_speed.f_out_limit=500;
	Tripod.Pitch.PID_speed.f_divider=500;
	
	
	
	Tripod.Pitch.PID2_angle.pid_mode=POSITION_PID;
	Tripod.Pitch.PID2_angle.max_output=600;
	Tripod.Pitch.PID2_angle.integral_uplimit=200;
	Tripod.Pitch.PID2_angle.integral_downlimit=-200;
	Tripod.Pitch.PID2_angle.p=15;
	Tripod.Pitch.PID2_angle.i=0.01;
	Tripod.Pitch.PID2_angle.d=0;
	Tripod.Pitch.PID2_angle.deadband=0;
	Tripod.Pitch.PID2_angle.f=0;
	Tripod.Pitch.PID2_angle.max_err=3;//4;
	Tripod.Pitch.PID2_angle.mid_err=1.5;
	Tripod.Pitch.PID2_angle.min_err=0;
	Tripod.Pitch.PID2_angle.f_out_limit=0;
	Tripod.Pitch.PID2_angle.f_divider=0;

	Tripod.Pitch.angle_max=34;
	Tripod.Pitch.angle_min=-16;
}


void Tripode_Init(void)
{
	Tripod.control_mode.now=Position;
	Tripod.Yaw.angle_source.now=From_Encoder;
	Tripod.Yaw.angle_source.last=From_Encoder;
	Tripod.Pid_Mode.now=1;
	Pitch_Init();
	Yaw_Init();
}


uint8_t GyroMonitor(void)
{
	static uint32_t startdelay;
	static uint32_t onlinecounter;

	if(startdelay<2000)
	{
		startdelay++;
		return 0;
	}
	Tripod.monitor.circle_number++;
	if(Tripod.monitor.circle_number>100)
	{
		Tripod.GyroOffline=1;
		onlinecounter=0;
	}
	else
	{
		if(onlinecounter<0xffff)
			onlinecounter++;
		if(onlinecounter>50)
			Tripod.GyroOffline=0;
	}
	
	if(Tripod.GyroOffline)
	{
		Tripod.Pitch.current_set=0;
		Tripod.Yaw.current_set=0;
		return 1;
	}
	else
		return 0;
}


void Tripode_Fun(void)
{
	float yaw_setspeed;
	float pitch_setspeed;
	
	static uint32_t startdelay;
	static uint8_t Pirch_middle_init_flag;
	static Key_Data_t key_last;
	static float lastyaw;
	static float lastpitch;

	
	Tripod.Pitch.speed=-gyro[0];
	Tripod.Yaw.speed=-gyro[2];	
	

	
	if(Strike.strikemode==AutoAim_AutoStrike||Strike.strikemode==AutoAim_ManualStrike)//两种PID的切换判断
	//if(TX2_Data.mode_now==AUTOAIM_CLOSE)
		Tripod.Pid_Mode.now=2;
	else
		Tripod.Pid_Mode.now=1;
	
	if(startdelay<2000)
	{
		startdelay++;
		return;
	}
	
	GetYawAngle1000Hz();
	GetPitchAngle1000Hz();
	
	if(Pitch_MOTO6020.getpara.init_sta==1)
		Tripod.Pitch.ready_flag=1;
	if(Yaw_MOTO6020.getpara.init_sta==1)
		Tripod.Yaw.ready_flag=1;
	


	
	if(Pirch_middle_init_flag==0&&Tripod.Pitch.ready_flag)
	{
		Tripod.Pitch.angle_set=Tripod.Pitch.angle-Tripod.Pitch.angle_source.encoder.angle_relativeToChassis;
		Pirch_middle_init_flag=1;
	}
	
	if(GyroMonitor())
		return;


	if (RoboData.robo_ctrlmode.ctrl_source==FROM_REMOTE)
	{
		Tripod.control_mode.now=Position;
		Tripod.Pitch.angle_set+=RoboData.tripod_ctrl.pitch_angle/200.0f;
		Tripod.Yaw.angle_set+=RoboData.tripod_ctrl.yaw_angle/200.0f;
	}
	
	
	else if(RoboData.robo_ctrlmode.ctrl_source==FROM_PC)
	{
		if(Strike.cover_open==1)
			Tripod.Pitch.angle_set=Tripod.Pitch.angle-Tripod.Pitch.angle_source.encoder.angle_relativeToChassis;
		if(RC_CtrlData.key.key_data.Q==1&&key_last.Q==0)
			{
				Tripod.Yaw.angle_set=Tripod.Yaw.angle+180;
			}
		if(RC_CtrlData.key.key_data.F==1&&key_last.F==0)
		{
			lastyaw=Tripod.Yaw.angle;
			lastpitch=Tripod.Pitch.angle;
			
		}
		if(RC_CtrlData.key.key_data.F==0&&key_last.F==1)
		{
			Tripod.Yaw.angle_set=lastyaw;
			Tripod.Pitch.angle_set=lastpitch;
		}
		
		if(Strike.strikemode==ManualAim_AutoStrike||Strike.strikemode==ManualAim_ManualStrike||Strike.strikemode==WindMill_Manual)
		{
			Tripod.control_mode.now=Position;
			
			//Tripod_PC_Ctrl(-(float)(RC_CtrlData.mouse.y_speed)*0.0008,-((float)(RC_CtrlData.mouse.x_speed))*0.001);
			Tripod.Pitch.angle_set+=-(float)(RC_CtrlData.mouse.y_speed)*PITCHSENSITIVITY;
			Tripod.Yaw.angle_set+=-((float)(RC_CtrlData.mouse.x_speed))*YAWSENSITIVITY;
		}
	else if(Strike.strikemode==AutoAim_AutoStrike||Strike.strikemode==AutoAim_ManualStrike)
	{
		Tripod.control_mode.now=Position;
	}
	
	
	
}
	
	if(Tripod.control_mode.last==Speed&&Tripod.control_mode.now==Position)
	{
		Tripod.Yaw.angle_set=Tripod.Yaw.angle;
		Tripod.Pitch.angle_set=Tripod.Pitch.angle;
	}
	
	


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
	key_last=RC_CtrlData.key.key_data;
	Tripod.Pid_Mode.last=Tripod.Pid_Mode.now;
}







