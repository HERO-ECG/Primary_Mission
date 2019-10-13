#include "strike.h"
#include "math.h"
#include "stdint.h"
#include "robodata.h"
#include "dji_Protocol.h"
#include "auto_move.h"
#include "m_moto.h"

uint8_t  Preheat17 = 28;
uint8_t  TGcontrol_from_heat = 0;
float speed_from_heat = 0;
uint8_t if_heat_limit=0;



Strike_t Strike;
	float fire;
	float Manual_fire=0;
	float dengdai_shijian;
  float aimstate_last=0;

uint8_t IsOnTarget(float dp,float dy)
{
	float dangle;
	dangle=sqrt(fabs(dp*dp+dy*dy));
	if(tx2_ReadData.longth<5)
	{
	if(dangle<Strike.OnTargetAngleMax)
		return 1;
	else
		return 0;
	}
	else if(tx2_ReadData.longth>10)
	{
	if(dangle<0.5f)
		return 1;
	else
		return 0;
	}
	else
	{
		if(dangle<1.0f)
		return 1;
	else
		return 0;
	}
} 

	 float jam_time;
uint8_t error_time;

void AutoStrike(void)
{

	static uint8_t nowshot;

//	static uint8_t lastshot;


	if( RoboData.robo_ctrlmode.ctrl_source == AUTO_MOVE_MODE2)
	{
		if(tx2_ReadData.control_mode==attack && (IsOnTarget(tx2_ReadData.delta_p_fir,tx2_ReadData.delta_y_fir) == 1))
		{	
			Strike.aimstate=OnTarget;
			nowshot=1;
			
		}
		else
		{	
			Strike.aimstate=OffTarget;
			nowshot=0;
			Strike.Semi_time=first;
		}
		

	}
	
	if(Strike.aimstate==OnTarget && aimstate_last==OffTarget)
		Manual_fire=1;
//	else if(Strike.aimstate==OnTarget && aimstate_last==OnTarget)
//		Manual_fire=0;

	
	
	
	if(Strike.aimstate==OnTarget && Strike.state_2006==normal)
	{
		if(Strike.firemode==Auto_Mode)
		{
			if(nowshot==1 || 	Strike.work_state == real_work)
			{	TG_MOTO[0].set_speed = SHOOTRHYTHM;}
			else
			{	TG_MOTO[0].set_speed = 0;}
		}
		
		
		
		else if(Strike.firemode==Auto_Semi_Mode)
		{
			if(Strike.Semi_time==first)
			{
					TG_MOTO[0].set_speed = SHOOTRHYTHM;
					Strike.Semi_time=second;
			}
			else if(Strike.Semi_time==second)
			{
				dengdai_shijian++;
				if(dengdai_shijian==1500)
				{
					fire=1;
					dengdai_shijian=0;
				}
				if(fire==1)
					{	TG_MOTO[0].set_speed = SHOOTRHYTHM;}
				else
					{	TG_MOTO[0].set_speed = 0;}			
			}
		
		}
		else if(Strike.firemode==Semi_Mode)
		{
			if(Manual_fire==1)
			{
				TG_MOTO[0].set_speed = SHOOTRHYTHM;
			}
			else if(Manual_fire==0)
				TG_MOTO[0].set_speed = 0;
		}
	}

	else if(Strike.state_2006==error)//卡弹处理
	{
		TG_MOTO[0].set_speed = -SHOOTRHYTHM;
		error_time++;
		if(error_time== 150)
		{
			Strike.state_2006=normal;
			error_time=0;
		}
	}
   else
	{TG_MOTO[0].set_speed = 0;}
	
	//判断卡弹
	if(TG_MOTO[0].send_current == -10000  && TG_MOTO[0].set_speed == SHOOTRHYTHM)//&& ABS(TG_MOTO[0].getpara.real_current-TG_MOTO[0].set_speed)>2000
	{
	  
		jam_time++;
		if(jam_time == 1000)
		{
			Strike.state_2006=error;
		  jam_time=0;
		}
	}
	

	aimstate_last=Strike.aimstate;
	 		speed_from_heat=(TGcontrol_from_heat)?(-TG_MOTO[0].set_speed):0;
}

//原来的热量控制，暂时不用
void HeatControl(void)
{
    if (DJI_ReadData.ext_power_heat_data.shooter_heat0 <= (DJI_ReadData.ext_game_robot_state.shooter_heat0_cooling_limit-3*Preheat17))
    {
		TGcontrol_from_heat = 1;
    }
    else  
    {
		TGcontrol_from_heat = 0;
	}
}
//摩擦轮输入捕获测速，暂时不用
period_calc_typedef period_calc_1;
period_calc_typedef period_calc_2;

void mocalun_speed_calc(void)
{
			if(period_calc_1.start_flag==0)
		{
			period_calc_1.cnt=0;
			period_calc_1.period=0;
			
			__HAL_TIM_DISABLE(&htim4);
			__HAL_TIM_SET_COUNTER(&htim4,0);
			TIM_RESET_CAPTUREPOLARITY(&htim4,TIM_CHANNEL_1); //清除原来设置
			TIM_SET_CAPTUREPOLARITY(&htim4,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);
			__HAL_TIM_ENABLE(&htim4);
			period_calc_1.start_flag=1;
		}
		else
		{
			period_calc_1.cnt=HAL_TIM_ReadCapturedValue(&htim4,TIM_CHANNEL_1);
			period_calc_1.result=100000.0f/(period_calc_1.cnt+1+period_calc_1.period*TIM4->ARR);
			period_calc_1.start_flag=0;
			
		}
		
		
}

//服务器热量增加
void Heat_Add(void)
{
	static float shoot_speed;
	static float shoot_speed_last;
	shoot_speed=DJI_ReadData.ext_shoot_data.bullet_speed;
	if(shoot_speed!=shoot_speed_last)
	{
	Strike.Heat_Ctrl.heat_CalcLoacal_fromSever+=DJI_ReadData.ext_shoot_data.bullet_speed;
	}
	shoot_speed_last=shoot_speed;
}
//在裁判系统固定频率的反馈热量中更新热量
void Head_refresh(void)
{
	static float shooter_heat0;
	static float shooter_heat0_last;
	shooter_heat0=DJI_ReadData.ext_power_heat_data.shooter_heat0;
	if(shooter_heat0!=shooter_heat0_last)
		Strike.Heat_Ctrl.refresh_flag=1;
	
	shooter_heat0_last=shooter_heat0;
}

//在本地计算热量,服务器更新
void HeatCtrl(void)
{
	Heat_Add();
		
	Strike.Heat_Ctrl.heat_CalcLoacal_fromSever-=(160/1000);//DJI_ReadData.ext_game_robot_state.shooter_heat0_cooling_rate/1000;
	if(Strike.Heat_Ctrl.heat_CalcLoacal_fromSever<=0)
		Strike.Heat_Ctrl.heat_CalcLoacal_fromSever=0;
	if(Strike.Heat_Ctrl.refresh_flag==1)
	{
		Strike.Heat_Ctrl.heat_CalcLoacal_fromSever=DJI_ReadData.ext_power_heat_data.shooter_heat0;
		Strike.Heat_Ctrl.refresh_flag=0;
	}
	if(Strike.Heat_Ctrl.heat_CalcLoacal_fromSever<DJI_ReadData.ext_game_robot_state.shooter_heat0_cooling_limit-45)
	  TGcontrol_from_heat=1;
	else
		TGcontrol_from_heat=0;
	
}




void strike_init(void)
{
	Strike.firemode=Semi_Mode;
	Strike.OnTargetAngleMax=4.0f;
	Strike.state_2006=normal;
	Strike.work_state=real_work;
	RoboData.robo_ctrlmode.ctrl_source = AUTO_MOVE_MODE;
}


