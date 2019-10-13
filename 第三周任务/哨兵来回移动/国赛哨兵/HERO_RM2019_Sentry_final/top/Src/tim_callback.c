#include "main.h"
#include "control.h"
#include "t_moto.h"
#include "robodata.h"
#include "t_monitor.h"
#include "auto_move.h"
#include "pitch.h"
#include "tx2_Protocol.h"
#include "strike.h"

uint32_t time_piece=0x0000;
void vcan_sendware(u8 *wareaddr, u32 waresize);  
void Turn(void);

double bodan_angle=2711;
double bodan_time=0;
//static float  i = 0;
float tx2_qidong_dengdai=0;
uint8_t tx2_qidong_flag=0;

float yuanlai_angle_buff[90];


uint8_t chassis_power_flag=0;
//static uint8_t chassis_power_dengdai_time=0;

uint32_t FrictionMotorServo(uint32_t setspeed)
{
	static uint32_t speed_now=400;
	if(speed_now<setspeed)
		speed_now++;
	if(speed_now>setspeed)
		speed_now--;
	if(speed_now<1000)
		return 1000;
	return speed_now;
}

float t=2000;
float ttt=0;

extern float time;
uint8_t ring_ready;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{


	if(htim == &htim3)        //100hz
	{ 
		time_piece |= time_piece_0100hz;

		

			mocalun_state_monitor();//2006断电 摩擦轮停
//				qianwanbiezhuangqiang();
				
				tx2_monitor_count++; //tx2监视器变量
		if(tx2_monitor_count>600)
			tx2_monitor_count=600;
		
				
				
				if((overall_mode.OvermodeSta==dodge_level2 ||overall_mode.OvermodeSta == attack)&& yuanlai_xueliang == DJI_ReadData.ext_game_robot_state.remain_HP&&overall_mode.dodge_level2.danfangxiangzou_flag>2&&if_position_to_speed_dengdai_time==0&&Chassis_MOTO[0].pid_angle.err.now<1.0f)
				{
								position_to_speed_dengdai_time++;
							if(position_to_speed_dengdai_time>3000)
								position_to_speed_dengdai_time=3000;
					
				}
				else
					position_to_speed_dengdai_time=0;
				
				if(position_to_speed_dengdai_time==3000)
				{
					if_position_to_speed_dengdai_time=1;
				}				

				if(shoot_flag==1)
				shoot_pending++;
				if(shoot_pending>300)
					shoot_pending=300;
				

	}
	else if(htim == &htim5)//1000hz
	{
		time_piece |= time_piece_1000hz;
		HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);//电机信号接收
		
//				chassis_power_dengdai_time++;
//		if(chassis_power_dengdai_time==1)
//		{chassis_power_flag=1;
//		chassis_power_dengdai_time=0;}
		
		if(overall_mode.OvermodeSta==dodge_level1)
		{time++;}//闪避计时


//底盘电机信号发送
				AllAuto();
				PowerControl();//底盘功率控制
		SetMotoCurrent(&hcan1, STDID_3508, Chassis_MOTO[0].send_current, Chassis_MOTO[1].send_current, Chassis_MOTO[2].send_current, Chassis_MOTO[3].send_current);

		AutoStrike();//击打功能
	//	if(if_heat_limit==1)
		{HeatCtrl();}	//热量控制

 TG_MOTO[0].send_current=PidCalc(&(TG_MOTO[0].pid_speed), TG_MOTO[0].getpara.real_current ,speed_from_heat);

 SetMotoCurrent(&hcan1, STDID_2006_6623, Yaw_MOTO.send_current, Pitch.current_set, TG_MOTO[0].send_current, TG_MOTO[1].send_current);

	}

}











extern void mocalun_speed_calc(void);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim==&htim4)
		mocalun_speed_calc();
}


void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim2)
	{
		
		/*template
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)
		{
			static int i1;
			i1++;
			if(i1>250)
			{
				i1 = 0;
				__HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL_1,1300);
				//htim->Instance->CCR1 = 1300;
				htim->Instance->CCR2 = 1300;
				HAL_TIM_OC_Stop_IT(&htim2,TIM_CHANNEL_1);
				HAL_TIM_OC_Start_IT(&htim2,TIM_CHANNEL_2);
			}
		}
		else if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
		{
			static int i2;
			i2++;
			if(i2>250)
			{
				i2 = 0;
				htim->Instance->CCR1 = 1000;				
				__HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL_2,1000);
				//htim->Instance->CCR2 = 1000;
				HAL_TIM_OC_Start_IT(&htim2,TIM_CHANNEL_1);
				HAL_TIM_OC_Stop_IT(&htim2,TIM_CHANNEL_2);
			}
		}
		***template*/
		
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)
		{
			
		}
		else if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
		{
			
		}
		else if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_3)
		{
			
		}
		else if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_4)
		{
			
		}
	}
	else if(htim == &htim4)
	{
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)
		{
			
		}
		else if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
		{
			
		}
		else if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_3)
		{
			
		}
		else if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_4)
		{
			
		}
	}
	
}


//void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
//void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim);
//void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim);
//void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim);


void vcan_sendware(u8 *wareaddr, u32 waresize)
{
#define CMD_WARE     3
    u8 cmdf[2] = {CMD_WARE, ~CMD_WARE};    //串口调试 使用的前命令
    u8 cmdr[2] = {~CMD_WARE, CMD_WARE};    //串口调试 使用的后命令

		HAL_UART_Transmit(&huart7,cmdf,sizeof(cmdf),1000);
		HAL_UART_Transmit(&huart7,wareaddr,waresize,1000);
		HAL_UART_Transmit(&huart7,cmdr,sizeof(cmdr),1000);
}


