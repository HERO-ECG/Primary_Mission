#include "strike.h"
#include "control.h"
#include "t_monitor.h"
#include "tripod.h"
#include "math.h"
#include "robodata.h"
Strike_t Strike;



void Strike_SetModeFromControl(void)
{
	static uint8_t openflag;    //  1关摩擦轮、0开摩擦轮
	static RoboControlMode_t switch_last;
	static uint8_t AimMode;
	static uint8_t StrikeMode;
	static uint8_t FireMode=0;//0自动 1三连发 2单发
	if(RoboData.robo_ctrlmode.ctrl_source==FROM_PC)
	{
		

		if(KEY_FIRE_42MM==1)
		{
			Strike.fire_42mm.now=1;
			openflag=1;
		}
		else
		{
			Strike.fire_42mm.now=0;
		}
		
//		if(KEY_AUTOSTRIKE==1)
//			StrikeMode=1;
//		else
//			StrikeMode=0;
		StrikeMode=0;
		
		if(KEY_AUTOAIM==1)
			AimMode=1;
		else
			AimMode=0;
		
//		if(RC_CtrlData.key.key_data.C==1&&key_last.C==0)
//		{
//			if(RC_CtrlData.key.key_data.ctrl==0&&
//					RC_CtrlData.key.key_data.shift==0)
//				FireMode=0;
//			else if(RC_CtrlData.key.key_data.ctrl==0&&
//							RC_CtrlData.key.key_data.shift==1)
//				FireMode=2;
//			else if(RC_CtrlData.key.key_data.ctrl==1&&
//							RC_CtrlData.key.key_data.shift==0)
//				FireMode=1;

	}
	else //遥控器
	{
		
		if((RoboData.robo_ctrlmode.left_mode==LEFT_down)&&(switch_last.left_mode!=LEFT_down))
		{
			openflag=!openflag;
		}

//		if(openflag)
//		{
//			Strike.moto_para.friction_17mm=friction_pulse_17mm;
//			Strike.moto_para.friction_42mm=friction_speed_42mm;
//		}
//		else
//		{
//			Strike.moto_para.friction_17mm=1000;
//			Strike.moto_para.friction_42mm=0;
//		}

		
//		if(RoboData.robo_ctrlmode.ctrl_source==RIGHT_reserve)
//			Strike.fire_17mm.now=1;
//		else
//			Strike.fire_17mm.now=0;
//		
//		if(RoboData.robo_ctrlmode.left_mode==LEFT_down){
//			Strike.fire_42mm.now=1;
//		}else{
//			Strike.fire_42mm.now=0;
//		}
		if(RoboData.robo_ctrlmode.ctrl_source==RIGHT_reserve){
			Strike.fire_42mm.now=1;
		}else{
			Strike.fire_42mm.now=0;
		}
		
		if(RoboData.robo_ctrlmode.left_mode==LEFT_up){
			AimMode=1;
		}else{
			AimMode=0;
		}
	}

			if(TX2_Data.velocity>0)
			{
				if(StrikeMode==1)
				{
					if(AimMode==1)
					{
						Strike.strikemode=AutoAim_AutoStrike;
					}
					else
						Strike.strikemode=ManualAim_AutoStrike;
				}
				else
				{
					if(AimMode==1)
					{
						Strike.strikemode=AutoAim_ManualStrike;
					}
					else
						Strike.strikemode=ManualAim_ManualStrike;
				}
			}
			else
				Strike.strikemode=ManualAim_ManualStrike;
			
		
//		if(openflag)
//		{
//			Strike.moto_para.friction_17mm=friction_pulse_17mm;
//		}
//		else
//			Strike.moto_para.friction_17mm=1000;
		Strike.moto_para.friction_17mm=friction_pulse_17mm;
	
		if(Strike.vision.TX2_no_response==1)
			Strike.strikemode=ManualAim_ManualStrike;
		
		if(FireMode==0)
			Strike.firemode.now=Auto_Mode;
		else if(FireMode==1)
			Strike.firemode.now=Burst_Mode;
		else
			Strike.firemode.now=Semi_Mode;

		switch_last=RoboData.robo_ctrlmode;
}
void FrictionMotorServo(void)
{
	static uint16_t divider;
	static uint16_t speed_now=0;
	static int32_t speed_now2=0;
	
	divider++;
	if(divider==10000)
		divider=0;
	if(divider%3!=0)
		return;

	if(speed_now<Strike.moto_para.friction_17mm)
		speed_now+=1;
	else if(speed_now>Strike.moto_para.friction_17mm)
		speed_now-=1;
	
	if(speed_now<1000)
		TIM2->CCR1=1000;
	else
		TIM2->CCR1=speed_now;
	
	if(ABS(speed_now-Strike.moto_para.friction_17mm)<5)
	{
		if(speed_now2<Strike.moto_para.friction_17mm)
			speed_now2+=1;
		if(speed_now2>Strike.moto_para.friction_17mm)
			speed_now2-=1;
		if(speed_now2<1000)
			TIM2->CCR2=1000;
		else
			TIM2->CCR2=speed_now2;
	}
}
float deangle;
uint8_t IsOnTarget(float dp,float dy)
{
	float dangle;
	dangle=sqrt(fabs(dp*dp+dy*dy));
	deangle=dangle;
	if(dangle<Strike.vision.OnTargetAngleMax)
		return 1;
	else
		return 0;
} 


/**********自瞄设置************/
uint16_t back=0;

float bigBullet_pitch_correct=0;//-2.3f;
float bigBullet_yaw_correct=0;//0.1f;


void AutoStrike(void)
{
	float pitch_before=0,yaw_before=0;
	float pitch_correct=0,yaw_correct=0;
	static uint32_t OnTargetShootCounter;
	static uint8_t OnTargetShootPendingFlag;
	static uint8_t AutoStrike_flag;
	static float AbsPitchSpeed=0,AbsYawSpeed=0;

	{
		pitch_correct = bigBullet_pitch_correct;
		yaw_correct = bigBullet_yaw_correct;
	}


//	AbsPitchSpeed = 0.95f*AbsPitchSpeed+0.05f*Tripod.Pitch.PID_speed.get.now;
//	AbsYawSpeed = 0.95f*AbsYawSpeed+0.05f*Tripod.Yaw.PID_speed.get.now;	
	/*自瞄角度*/
	if(ABS(AbsPitchSpeed)<0.5f)
		Strike.vision.deltaangle_now.delta_pitch = (1.0f*TX2_Data.pitch_angle+pitch_correct);
	if(ABS(AbsYawSpeed)<0.5f)
		Strike.vision.deltaangle_now.delta_yaw = -(1.0f*TX2_Data.yaw_angle+yaw_correct)+1.0f;	

	AngleQueue(&Tripod.Pitch.angle,&Tripod.Yaw.angle,0,SET);
	pitch_before = Tripod.Pitch.angle;
	yaw_before = Tripod.Yaw.angle;
	if(Strike.vision.deltaangle_now.delta_pitch!=Strike.vision.deltaangle_last.delta_pitch||
		Strike.vision.deltaangle_now.delta_yaw!=Strike.vision.deltaangle_last.delta_yaw)
	{
		Strike.vision.autoaim_flag=1;
		AutoStrike_flag=1;
	}
		
	if(IsOnTarget(Strike.vision.deltaangle_now.delta_pitch,Strike.vision.deltaangle_now.delta_yaw)==1&&TX2_Data.velocity>0&&AutoStrike_flag==1)
	{
		OnTargetShootPendingFlag=1;
	}
	else
	{
		OnTargetShootPendingFlag=0;
		OnTargetShootCounter=0;
	}
	if(OnTargetShootPendingFlag==1)
		OnTargetShootCounter++;
	if(OnTargetShootCounter>0)
		Strike.vision.aimstate=OnTarget;
	else
		Strike.vision.aimstate=OffTarget;
	
	if(OnTargetShootCounter>300)
	{
		OnTargetShootPendingFlag=0;
		OnTargetShootCounter=0;
	}
	

	
	if(Strike.strikemode==AutoAim_AutoStrike||Strike.strikemode==AutoAim_ManualStrike)
	{
		Tripod.control_mode.now=Position;
		if(Strike.vision.manual_correct==1)
		{
			Strike.vision.manual_correct_offset.delta_pitch+=-(float)(RC_CtrlData.mouse.y_speed)*PITCHSENSITIVITY;
			Strike.vision.manual_correct_offset.delta_yaw+=-((float)(RC_CtrlData.mouse.x_speed))*YAWSENSITIVITY;
		}
		else
		{
			Strike.vision.manual_correct_offset.delta_pitch=0;
			Strike.vision.manual_correct_offset.delta_yaw=0;
		}
		
		if(Strike.vision.autoaim_flag==1)
		{
			Tripod.Pitch.angle_set=pitch_before-Strike.vision.deltaangle_now.delta_pitch+Strike.vision.manual_correct_offset.delta_pitch;
			Tripod.Yaw.angle_set=yaw_before+Strike.vision.deltaangle_now.delta_yaw+Strike.vision.manual_correct_offset.delta_yaw;
			Strike.vision.autoaim_flag=0;
		}
	}
	else
	{
		Strike.vision.manual_correct_offset.delta_pitch=0;
	}
	
	Strike.vision.deltaangle_last.delta_pitch = Strike.vision.deltaangle_now.delta_pitch;
	Strike.vision.deltaangle_last.delta_yaw = Strike.vision.deltaangle_now.delta_yaw;
}


void AmmoMotoControl(void)
{
	static uint8_t lastshot;
	static uint8_t nowshot;

	uint8_t fire;
	
	
	if(Strike.fire_17mm.now==1)
		nowshot=1;
	else
		nowshot=0;
	if(nowshot==1&&lastshot==0)
		fire = 1;
	else
		fire = 0;
	
	if(Strike.firemode.last!=Burst_Mode&&Strike.firemode.now==Burst_Mode)
		Strike.count_17mm.burst=0;	
	
		if(Strike.firemode.now==Semi_Mode||Strike.firemode.now==Burst_Mode)
		{
			if(fire==1)
				Strike.moto_para.toggle_rpm_17mm = toggle_speed_17mm;
			if(nowshot==0)
				Strike.moto_para.toggle_rpm_17mm = 0;
		}
		else if(Strike.firemode.now==Auto_Mode)
		{
			if(nowshot==1)
				Strike.moto_para.toggle_rpm_17mm = toggle_speed_17mm;
			else
				Strike.moto_para.toggle_rpm_17mm = 0;
		}
		
		if(Strike.heat_17mm.ceasefire==1)
			Strike.moto_para.toggle_rpm_17mm = 0;
		
		LittleAmmo_MOTO.send_current = LittleAmmo_MOTO.pid_speed.PidCalc(&LittleAmmo_MOTO.pid_speed, LittleAmmo_MOTO.getpara.speed_rpm, Strike.moto_para.toggle_rpm_17mm);
		lastshot=nowshot;
}




void AngleQueue(float *pitch,float *yaw,uint16_t index,uint8_t mode)
{
	static uint16_t pointer;
	static uint16_t divider;
	static float pitch_history[10];
	static float yaw_history[10];
	

	if(mode==SET)
	{
		divider++;
		if(divider==10000)
			divider=0;
		pointer++;
		if(pointer==10)
			pointer=0;
		pitch_history[pointer]=*pitch;
		yaw_history[pointer]=*yaw;
	}
	else
	{
		if(index>=10)
		{
			*pitch=0;
			*yaw=0;
		}
		else
		{
			if(index<pointer)
			{
				*pitch=pitch_history[pointer-index];
				*yaw=yaw_history[pointer-index];
			}
			else
			{
				*pitch=pitch_history[10-index+pointer-1];
				*yaw=yaw_history[10-index+pointer-1];
			}
		}
	}
}


void HeatCtrl(void)
{
	static uint16_t heat_17mm_last;
	static uint16_t heat_42mm_last;
	
	Strike.heat_17mm.cooling_rate=DJI_ReadData.ext_game_robot_state.shooter_heat0_cooling_rate;
	Strike.heat_17mm.cooling_limit=DJI_ReadData.ext_game_robot_state.shooter_heat0_cooling_limit;
	Strike.heat_17mm.heat_sever=DJI_ReadData.ext_power_heat_data.shooter_heat0;
	if(Strike.heat_17mm.cooling_rate==0)
		Strike.heat_17mm.cooling_rate=40;
	
//	Strike.heat_42mm.cooling_rate=DJI_ReadData.ext_game_robot_state.shooter_heat1_cooling_rate;
//	Strike.heat_42mm.cooling_limit=DJI_ReadData.ext_game_robot_state.shooter_heat1_cooling_limit;
//	Strike.heat_42mm.heat_sever=DJI_ReadData.ext_power_heat_data.shooter_heat1;
	Strike.heat_42mm.cooling_rate=DJI_ReadData.ext_game_robot_state.shooter_heat1_cooling_rate;
	Strike.heat_42mm.cooling_limit=100;
	
	if(Strike.heat_42mm.cooling_rate==0)
		Strike.heat_42mm.cooling_rate=20;
	
	Strike.heat_17mm.heat_local-=Strike.heat_17mm.cooling_rate/1000.0f;
	
	Strike.heat_42mm.heat_local-=Strike.heat_42mm.cooling_rate/1000.0f;
	
	if(Strike.heat_17mm.heat_local<0)
		Strike.heat_17mm.heat_local=0;
	
	if(Strike.heat_42mm.heat_local<0)
		Strike.heat_42mm.heat_local=0;
		
	
	if(heat_17mm_last!=Strike.heat_17mm.heat_sever)
	{
		Strike.heat_17mm.heat_mingle = Strike.heat_17mm.heat_sever;
		Strike.heat_17mm.heat_local=Strike.heat_17mm.heat_mingle;
	}
	else
		Strike.heat_17mm.heat_mingle=Strike.heat_17mm.heat_local;
	
	if(heat_42mm_last!=Strike.heat_42mm.heat_sever)
	{
		Strike.heat_42mm.heat_mingle = Strike.heat_42mm.heat_sever;
		Strike.heat_42mm.heat_local=Strike.heat_42mm.heat_mingle;
	}
	else
		Strike.heat_42mm.heat_mingle=Strike.heat_42mm.heat_local;
	
	if(Strike.heat_17mm.heat_mingle>Strike.heat_17mm.cooling_limit-Strike.heat_17mm.margin)
		Strike.heat_17mm.ceasefire=1;
	else
		Strike.heat_17mm.ceasefire=0;
	
	if(Strike.heat_42mm.heat_mingle>Strike.heat_42mm.cooling_limit-Strike.heat_42mm.margin)
		Strike.heat_42mm.ceasefire=1;
	else
		Strike.heat_42mm.ceasefire=0;
	
	heat_17mm_last=Strike.heat_17mm.heat_sever;
	heat_42mm_last=Strike.heat_42mm.heat_sever;
}

void StrikeCount(void)
{
	
	if(Strike.count_17mm.in_firecounter<0xffff)
		Strike.count_17mm.in_firecounter++;
	if(Strike.count_17mm.out_firecounter<0xffff)
		Strike.count_17mm.out_firecounter++;
	if(Strike.count_17mm.fire_flag==1)
	{
		Strike.count_17mm.fire_flag=0;
		if(Strike.firemode.now==Semi_Mode)
			Strike.moto_para.toggle_rpm_17mm = 0;
	}
	if(Strike.firemode.now==Burst_Mode)
		{
			if(Strike.count_17mm.burst>2)
			{
				Strike.moto_para.toggle_rpm_17mm = 0;
				Strike.count_17mm.burst=0;
			}
		}
	
}
#define M2006LoadedCurrent 420
uint8_t Shoot_42mm(void)
{
	static uint8_t shoot_order_last=0, TGstart_order=0, TGstop=0, TG_temp_sta=0, prepare_sta=0, restart_sta=0;
	static uint16_t TG_init_time_cnt=0, TG_stop_time_cnt=0, TG_work_time_cnt=0, TG_delay=0, prepare_delay=0,restart_delay=0;
	static int16_t frictionMoto_current=0,Ctrl2006MOTO_current=0;
	float temp_frictionMoto_speed=0;
	uint8_t sta=0;
	uint8_t shoot_order;
	float frictionMoto_speed;
	
	if(Strike.fire_42mm.now==1)
		shoot_order=1;
	else
		shoot_order=0;
	
	frictionMoto_speed=friction_speed_42mm;
	if(TG_init_time_cnt<100){
		/*initiate*/
		TG_init_time_cnt++;
		Ctrl_2006_MOTO.set_speed = -1000;
		Ammunition_MOTO.set_speed = 0;
		temp_frictionMoto_speed = -frictionMoto_speed/3.0f;
	}
	else
	{
		/*initiation have been done*/
		if(TG_init_time_cnt<4500)
			TG_init_time_cnt++;
		
		if(!shoot_order_last&&shoot_order&&Strike.heat_42mm.ceasefire==0){
			TGstart_order=1;
		}
		shoot_order_last=shoot_order;

		temp_frictionMoto_speed = frictionMoto_speed;		
		if(TGstart_order&&!TGstop){
			/*work*/
			Ctrl_2006_MOTO.set_speed = 6000;
			Ammunition_MOTO.set_speed = -1300;
			TG_stop_time_cnt=0;
			if(TG_work_time_cnt<1500)
				TG_work_time_cnt++;
			restart_delay=0;
			restart_sta=0;
			sta=1;
		}else{
				if(restart_sta==0){
					if(restart_delay<2000){
						restart_delay++;
						/*stop*/
						Ctrl_2006_MOTO.set_speed = 0;
						Ammunition_MOTO.set_speed = 0;
						TG_stop_time_cnt++;
						if(TG_stop_time_cnt>50){
							TGstop=0;
							TG_work_time_cnt=0;
						}
					}else{
						/*restart*/
						Ctrl_2006_MOTO.set_speed = -1000;
						Ammunition_MOTO.set_speed = 0;
						if(restart_delay-2050<0){
							restart_delay++;
						}else{
							if(ABS(Ctrl2006MOTO_current)>M2006LoadedCurrent){
								restart_sta=2;
							}else{
								TGstop=0;
								TGstart_order=1;
							}
						}
					}
				}else{
					/*stop*/
					Ctrl_2006_MOTO.set_speed = 0;
					Ammunition_MOTO.set_speed = 0;
					TG_stop_time_cnt++;
					if(TG_stop_time_cnt>300){
						TGstop=0;
						TG_work_time_cnt=0;
					}
				}
				sta=0;
		}
		/*shooting have been done, turning to stop*/
		frictionMoto_current=Friction_Wheel_MOTO[0].getpara.given_current*0.05f+0.95f*frictionMoto_current;
		if(ABS(frictionMoto_current)>2300){
			TG_temp_sta=1;
			TG_delay=0;
		}
		if(TG_temp_sta==1&&TG_delay!=0){
			TGstart_order=0;
			TGstop=1;
			TG_temp_sta=0;
			
			Strike.count_42mm.all++;
			Strike.heat_42mm.heat_local+=100;
		}else{
			TG_delay=1;
		}
		/*preparation have been done, turning to stop*/
		Ctrl2006MOTO_current=Ctrl_2006_MOTO.getpara.given_current*0.05f+0.95f*Ctrl2006MOTO_current;
		if(ABS(Ctrl2006MOTO_current)>M2006LoadedCurrent&&prepare_sta==0&&TG_work_time_cnt>500){
			prepare_sta=1;
			prepare_delay=0;
		}
		if(prepare_sta==1){
			if(prepare_delay<142){
				prepare_delay++;
			}else{
				TGstart_order=0;
				TGstop=1;
				prepare_sta=0;
			}
		}
	}
	ABSLimit(&Ctrl_2006_MOTO.set_speed, 8000);
	Ctrl_2006_MOTO.send_current = PidCalc(&Ctrl_2006_MOTO.pid_speed, Ctrl_2006_MOTO.getpara.speed_rpm, Ctrl_2006_MOTO.set_speed);	//辅助2006	
	Friction_Wheel_MOTO[0].send_current = PidCalc(&Friction_Wheel_MOTO[0].pid_speed, Friction_Wheel_MOTO[0].getpara.speed_rpm, temp_frictionMoto_speed);
	Friction_Wheel_MOTO[1].send_current = PidCalc(&Friction_Wheel_MOTO[1].pid_speed, Friction_Wheel_MOTO[1].getpara.speed_rpm, -temp_frictionMoto_speed);
	Ammunition_MOTO.send_current = PidCalc(&Ammunition_MOTO.pid_speed, Ammunition_MOTO.getpara.speed_rpm, Ammunition_MOTO.set_speed);	  
	
	Strike.fire_42mm.last=Strike.fire_42mm.now;
	
	
	return sta;
}

void Shoot_BigBullet(){
	uint8_t shoot_order;
	static uint8_t shoot_order_last=0, TGstart_order=0;
	static uint16_t TG_init_time_cnt=0;
	float temp_frictionMoto_speed=0;

	if(TG_init_time_cnt<100){
		/*initiate*/
		TG_init_time_cnt++;
		Ctrl_2006_MOTO.set_speed = -1000;
		Ammunition_MOTO.set_speed = 0;
		temp_frictionMoto_speed = -friction_speed_42mm/3.0f;
	//	gpio_exti_0=0;
	}
	else
	{
		temp_frictionMoto_speed = friction_speed_42mm;
		if(Strike.fire_42mm.now==1)
			shoot_order=1;
		else
			shoot_order=0;
		
		if(!shoot_order_last&&shoot_order){//沿边触发
			TGstart_order = 1;
		}
		shoot_order_last=shoot_order;
		
		if(gpio_exti_0==1){
			Ctrl_2006_MOTO.set_speed = 0;
		}else{
			Ctrl_2006_MOTO.set_speed = 6000;
		}
		if(TGstart_order == 1){
			TGstart_order = 0;
			Ctrl_2006_MOTO.set_speed = 6000;
		//	gpio_exti_0=0;
		}
		
	}
	Ctrl_2006_MOTO.send_current = PidCalc(&Ctrl_2006_MOTO.pid_speed, Ctrl_2006_MOTO.getpara.speed_rpm, Ctrl_2006_MOTO.set_speed);	//辅助2006	
	Friction_Wheel_MOTO[0].send_current = PidCalc(&Friction_Wheel_MOTO[0].pid_speed, Friction_Wheel_MOTO[0].getpara.speed_rpm, temp_frictionMoto_speed);
	Friction_Wheel_MOTO[1].send_current = PidCalc(&Friction_Wheel_MOTO[1].pid_speed, Friction_Wheel_MOTO[1].getpara.speed_rpm, -temp_frictionMoto_speed);

}

void Strike_Init(void)
{
	Strike.strikemode=ManualAim_ManualStrike;
	Strike.vision.OnTargetAngleMax=1.5f;
	Strike.firemode.now=Auto_Mode;
	
	Strike.moto_para.toggle_rpm_17mm=6000;
	Strike.heat_17mm.margin=110;
	Strike.heat_42mm.margin=100;
	Strike.moto_para.friction_42mm=6000;
}

void Strike_fun(void)
{
	Strike_SetModeFromControl();	
	AutoStrike();
	//Shoot_42mm();
	Shoot_BigBullet();
	Strike.firemode.last=Strike.firemode.now;
}



