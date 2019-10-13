#include "main.h"
#include "control.h"
#include "strike.h"
#include "robodata.h"
#include "t_monitor.h"
#include "math.h"
#include "tripod.h"
#include "hwt901b.h"
#include "strike.h"

uint32_t time_piece=0x0000;
uint16_t pwm_pulse=1000;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if((time_piece&time_piece_start)==time_piece_start)
	{
		if(htim==&htim3)        //100hz
		{ 
			time_piece |=time_piece_0100hz;
			
			

		}
		else if(htim==&htim5)//1000hz
		{		
			time_piece |=time_piece_1000hz;
			Strike_fun();//打击服务函数
			Tripode_Fun();//云台服务函数
			
			
			BToATest[0]=Strike.vision.prediction.yaw.angular_velocity_vision;
			BToATest[1]=Strike.vision.prediction.yaw.angular_velocity_tripod;
			BToATest[2]=Strike.vision.prediction.yaw.angular_velocity_absolute_filtered;
			
			
			
			HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);//6623&2006&3508电机信号接收
			

			
			SetMotoCurrent(&hcan1, STDID_2006_6623, 0,Tripod.Yaw.current_set , 0, Tripod.Pitch.current_set);
		
			SetMotoCurrent(&hcan2, STDID_2006_6623, Strike.toggle_motor.current_set, Strike.toggle_motor.current_set,Strike.toggle_motor.current_set,Strike.toggle_motor.current_set);
		}
		else if(htim==&htim4)
		{
				StrikeCount();
		}
	}
}





