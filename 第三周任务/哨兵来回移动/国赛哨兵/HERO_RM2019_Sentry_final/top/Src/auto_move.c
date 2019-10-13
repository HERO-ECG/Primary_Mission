#include "auto_move.h"
#include "dji_Protocol.h"
#include "can.h"
#include "robodata.h"


extern float pitch_up_plus ;
extern float pitch_down_plus ;
/*-------------------------------Para--------------------------------------------*/
//自动瞄参数
OverallMode_t overall_mode={&Init_OverallMode};
float bodan_zhuansu = 0;
float err_auto_sum=0;
float chassis_positon = 0;

float chassiss_square_position=0;//用于到场边位置清零和位置累积

uint16_t rest_bullet=500;
float time=0;
#define PowerLimit 18.0f			
float RealPower_Total=0;
float SpeedReduceFactor=1;//转速削弱因数

float guidao_changdu_qidian=0;
float guidao_changdu_zhongdian=0;
float guidao_changdu_zongde=0; //为了确定轨道长度及自身位置
uint8_t jilu_weizhi1=0;
uint8_t jilu_weizhi2=0;//记录起点终点的标志位
uint16_t yuanlai_xueliang=0;  //判断血量改变位置 
uint16_t jiluxiuliang;
uint8_t position_duobi_flag=0;//在躲避模式2下 是否开启位置躲避
float position_to_speed_dengdai_time;//在躲避模式2下，是否位置躲避改为速度躲避
uint8_t if_position_to_speed_dengdai_time;//在躲避模式2下，是否位置躲避改为速度躲避的标志位
float position_set_round;//为了记录设置的位置 在定时器中实现在固定位置的延时等待
uint8_t position_duobi_fanwei;//等于1是无人机侧,等于2是中间侧,等于3是靠近桥的一侧


float Turn_Pending_Left;
float Turn_Pending_Right;
extern uint8_t ring_ready;
extern uint8_t chassissmode;


#define LONGTH 400

/*-------------------------------Chassis--------------------------------------------*/
float RealPower_Total,power_now,buff,speed_decreasement,speed_decreasement_k,setdespeed;

void setspeed(Moto_t *MOTO,float speed)
{
	RealPower_Total = RealPower_Total*0.9f+0.1f*(Chassis_MOTO[0].getpara.get_power);
	power_now=RealPower_Total*0.75f;
	
	
	buff=DJI_ReadData.ext_power_heat_data.chassis_power_buffer;
//	if(buff<75){
	speed_decreasement=PidCalc(&Chassis_power.pid_speed,power_now,22);
	if(speed_decreasement>0)
		speed_decreasement=0;
	setdespeed=fabs(speed);
	if(setdespeed>0)
	{
		if((1.0f+speed_decreasement/setdespeed)<0)
			speed_decreasement_k=0;
		else if((1.0f+speed_decreasement/setdespeed)>1)
			speed_decreasement_k=1;
		else
			speed_decreasement_k=1.0f+speed_decreasement/setdespeed;
	}
	else
		speed_decreasement_k=1;
//}
//	else
//		speed_decreasement_k=1;
	MOTO->set_speed=speed_decreasement_k*speed;
}

//void setspeed(Moto_t *MOTO,float speed)
//{
//	overall_mode.chassiss_control_mode = SPEED;
////	RealPower_Total = 0.9f*RealPower_Total +  0.1f*(Chassis_MOTO[0].getpara.get_power);
//	if(DJI_ReadData.ext_power_heat_data.chassis_power>12.0f)
//	{		
//////			if(chassis_power_flag==1)
////		//PID_PowerControl.PidCalc(&PID_PowerControl, RealPower_Total, PowerLimit);	 //限制在80w
////		{SpeedReduceFactor -=0.2f;
//////			chassis_power_flag=0;
////		}
////		if(SpeedReduceFactor<0.3f) 
////		{
////			SpeedReduceFactor=0.3f;
//		SpeedReduceFactor=0.1f;
////		}
//	}
//	else
//	{
//		SpeedReduceFactor +=0.0001f;
//		if(SpeedReduceFactor>1) 
//		{
//			SpeedReduceFactor=1.0f;
//		
//	}

//	MOTO->set_speed = SpeedReduceFactor*speed;	
//}
//}

int direction = 0;

void ChassisMotoAutoMode(Moto_t *MOTO)
{
						Turn();
	overall_mode.yu_shu = overall_mode.Chassis_derection % 2;
			switch(overall_mode.yu_shu)
			{
				case  1:
					direction = 1;
				break;
				case  0:
					direction = -1;
				break;
			}
			
			
//	if(overall_mode.OvermodeSta == attack)
//	  {
//			overall_mode.dodge_level2.danfangxiangzou_flag=0;
//			position_duobi_flag=0;
//			
//			
//				setspeed(&Chassis_MOTO[0],0);
//			
//		}


		if(DJI_ReadData.ext_game_robot_state.remain_HP<350)
				overall_mode.OvermodeSta= dodge_level1;
		else 
				overall_mode.OvermodeSta= dodge_level2;
//		if(rest_bullet<=0 ) //防止子弹打完站着挨打
//				overall_mode.OvermodeSta= dodge_level1;		
		
		 if(overall_mode.OvermodeSta==dodge_level1)
			{
//				overall_mode.dodge_level2.danfangxiangzou_flag=0;
//				position_duobi_flag=0;
			


				
				if(time<2000) 
				{setspeed(&Chassis_MOTO[0],direction*7000);}
				else if(time < 4000)
        {setspeed(&Chassis_MOTO[0],direction*8000);}
//				else if(time == 4000&&(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_0))==1&& 	(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_1))==1)
//        {overall_mode.Chassis_derection++;}				
				else if(time < 4500) 	
        {setspeed(&Chassis_MOTO[0],direction*9000);}
//				else if(time == 5000&&(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_0))==1&& 	(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_1))==1) 	
//        {overall_mode.Chassis_derection++;}
				else if(time < 8000) 	
        {setspeed(&Chassis_MOTO[0],direction*9000);}
//				else if(time == 6100&&(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_0))==1&& 	(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_1))==1)
//				{overall_mode.Chassis_derection++;}	
				else if(time>8000) 	
        {time=0;}
			}		
			
		else if(overall_mode.OvermodeSta==dodge_level2||overall_mode.OvermodeSta == attack)
		{
//			setspeed(&Chassis_MOTO[0],direction*7000);
			if(overall_mode.dodge_level2.danfangxiangzou_flag	== 0)
			{
				jilu_weizhi1=0;//防止轨道起点位置不停改变
				jilu_weizhi2=0;//防止轨道终点位置不停改变

			  setspeed(&Chassis_MOTO[0],direction*5000);
		//	  dodge_level2_zuichu_remain_HP=DJI_ReadData.ext_game_robot_state.remain_HP;
			}

			else if(overall_mode.dodge_level2.danfangxiangzou_flag	== 1)
			{
				setspeed(&Chassis_MOTO[0],direction*5000);
				if(jilu_weizhi1==0)
				{
				guidao_changdu_qidian = Chassis_MOTO[0].getpara.xiangdui_round;
					jilu_weizhi1=1;
				}
			}
//			else if(overall_mode.dodge_level2.danfangxiangzou_flag	== 2)
//			{
//				setspeed(&Chassis_MOTO[0],direction*6000);
//			}
//			
			else if(overall_mode.dodge_level2.danfangxiangzou_flag	>= 2)

			{	
				

				if(position_duobi_flag == 0)
					setspeed(&Chassis_MOTO[0],direction*5000);
				
				if(jilu_weizhi2==0)
				{
					guidao_changdu_zhongdian = Chassis_MOTO[0].getpara.xiangdui_round;
					guidao_changdu_zongde = fabs(guidao_changdu_zhongdian - guidao_changdu_qidian); //统计轨道总长度
					jilu_weizhi2=1;
				}			
				
				if(((DJI_ReadData.ext_game_robot_state.remain_HP<yuanlai_xueliang)&&(position_duobi_flag==0))||((position_duobi_flag==1)&&(DJI_ReadData.ext_game_robot_state.remain_HP<(jiluxiuliang-30))))
				{	
					if_position_to_speed_dengdai_time=0; //延迟等待巡逻			
					position_duobi_flag=1;
					jiluxiuliang=DJI_ReadData.ext_game_robot_state.remain_HP;
									if(nearby_plane)//无人机侧被打，在中间巡逻
											{position_duobi_fanwei=2;}
									else if(nearby_middle)//中间被打，在桥侧巡逻
											{position_duobi_fanwei=1;}
									else
											{position_duobi_fanwei=1;}//在桥侧被打，在无人机侧巡逻
				}
				
				if(position_duobi_flag==1)//进入位置躲避模式
				{
					switch(position_duobi_fanwei)
					{
						case 1:
							if(now_potision>guidao_changdu_zongde*0.1f)//&&zuobian_check!=0&&youbian_check!=0)//大于无人机侧范围
								overall_mode.Chassis_derection=1;
								setspeed(&Chassis_MOTO[0],direction*5000);
							break;
						case 2:
							if(now_potision>guidao_changdu_zongde*0.7f)//&&zuobian_check!=0&&youbian_check!=0)//靠近桥侧
								overall_mode.Chassis_derection=1;
							else if(now_potision<guidao_changdu_zongde*0.3f&&zuobian_check!=0&&youbian_check!=0)
								overall_mode.Chassis_derection=2;
								setspeed(&Chassis_MOTO[0],direction*5000);			
							break;
						case 3:
							if(now_potision<guidao_changdu_zongde*0.8f)//&&zuobian_check!=0&&youbian_check!=0)
								overall_mode.Chassis_derection=2;
								setspeed(&Chassis_MOTO[0],direction*5000);		
					}
				}
				
				if(if_position_to_speed_dengdai_time==1)
					position_duobi_flag  = 0;

			}
			
			yuanlai_xueliang=DJI_ReadData.ext_game_robot_state.remain_HP;
		}
		
		
}







/*-------------------------------Yaw--------------------------------------------*/
float yyy=0;

void YawMotoAutoMode(Moto_t *MOTO)
{

		if(overall_mode.tx2_gimble == attack)
	  {
			overall_mode.Overall_mode_gimbal = 1;
			overall_mode.yaw.yaw_auto_angle_set = yaw_dangshi_angle;
		}
		else
		{
			overall_mode.Overall_mode_gimbal = 0;
		}
		if(overall_mode.Overall_mode_gimbal == 0 )
		{
			if(Yaw_MOTO.getpara.xiangdui_round>40)
				overall_mode.yaw_derection = 2;
			else if(Yaw_MOTO.getpara.xiangdui_round<(-40))
				overall_mode.yaw_derection = 1;
			if(overall_mode.yaw_derection  == 1)
			{
			static int  y1 = 0;
					y1++;
					if(y1 == 1)
					{
						overall_mode.yaw.yaw_auto_angle_set += 7;
						y1 = 0;

					}
				}
			else 
			{
					static int  y2 = 0;
					y2++;
					if(y2 == 1)
					{
						overall_mode.yaw.yaw_auto_angle_set -= 7;
						y2 = 0;
					}
				}
		}
		YawMotoCtrl(MOTO,1000, overall_mode.yaw.yaw_auto_angle_set);
	
}


/*-------------------------------Pitch--------------------------------------------*/
float ppp=5700;
float qwe=0;
float a_new=8192.0f;
float pitch_nowangle=(-200);
uint8_t Pitch_AngleFlag=0;

float  qqqq;
void PitchMotoAutoMode(Moto_t *MOTO)
{
	if(overall_mode.tx2_gimble == attack)
	{

			overall_mode.Overall_mode_gimbal = 1;
		overall_mode.pitch.pit_auto_angle_set = pitch_dangshi_angle;
		pitch_nowangle=overall_mode.pitch.pit_auto_angle_set;
	
	Pitch_SetAngle(overall_mode.pitch.pit_auto_angle_set);

	}
	else
	{
			overall_mode.Overall_mode_gimbal = 0;
	}
	if(overall_mode.Overall_mode_gimbal == 0)
	{
					if(Pitch.angle < (-510) )
				overall_mode.pitch_derection = (-1);
			else if(Pitch.angle > 130)
				overall_mode.pitch_derection = (1);
			if(overall_mode.pitch_derection == (-1))
			Pitch_SetSpeed(overall_mode.pitch_derection*240);
			else if(overall_mode.pitch_derection == (1))
			Pitch_SetSpeed(overall_mode.pitch_derection*300);
	}

}





/*-------------------------------Function--------------------------------------------*/
//最初的发弹判断
void Audo_bodan(void)
{
	err_auto_sum=Pitch.PID_angle.err.now+Yaw_MOTO.pid_angle.err.now;
}

//统计剩余弹量
void restbullet(void)
{
	static float last_shootspeed=0;
	static float count_bullet=0;


	if(DJI_ReadData.ext_shoot_data.bullet_speed!=last_shootspeed && DJI_ReadData.ext_shoot_data.bullet_speed !=0)
	{count_bullet++;}
	if(DJI_ReadData.ext_shoot_data.bullet_speed !=0)
	{last_shootspeed = DJI_ReadData.ext_shoot_data.bullet_speed;}
	
	rest_bullet=(uint16_t)(total_bullet-count_bullet);
}

	float zhuangqiang_time1=0;
	float zhuangqiang_time2=0;
//uint8_t  qwweqweow,ucuhvsidh;
//防止撞墙
void qianwanbiezhuangqiang(void)
{
//qwweqweow=HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_0);
//	ucuhvsidh=HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_1);
	if(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_0)==0)
	{zhuangqiang_time1++;}
	else
	{zhuangqiang_time1 = 0;}
	
	if(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_1)==0)
	{zhuangqiang_time2++;}
	else
	{zhuangqiang_time2 = 0;}
	
	if(zhuangqiang_time1>=250)
	{overall_mode.Chassis_derection++;
		zhuangqiang_time1=0;
	}
	if(zhuangqiang_time2>=250)
	{overall_mode.Chassis_derection++;
		zhuangqiang_time2=0;
	}
}
int16_t length_temp=0;
/*来回摆动模式*/
void Repeated_move(void)
{
	/*1为向右走 -1向左走*/
	static int8_t s_direction = -1;
	/*1000的倍数*/
	static uint16_t s_speed = 0;
	

	length_temp=GetLightlength(s_direction)+Chassis_MOTO[0].getpara.xiangdui_round;
	/*针对不同方向，对于处于不同位置设定不同的速度*/
	if(s_direction==-1)
	{
		if(length_temp > -150)
		{
			if(s_speed<8000){s_speed+=500;}
			else{s_speed=8000;}
		}
		else if(length_temp > -240)
		{
			if(s_speed>5000){s_speed-=500;}
			else{s_speed=5000;}
		}
		else if(length_temp > -303)
		{
			if(s_speed<8000){s_speed+=500;}
			else{s_speed=8000;}
		}
		else
		{
			if(s_speed>500){s_speed-=500;}
			else{s_speed=0;}
			if(abs((int)Chassis_MOTO[0].getpara.speed_rpm)<5){s_direction *= -1;}
		}
	}
	else
	{
		if(length_temp < -245)
		{
			if(s_speed<8000){s_speed+=500;}
			else{s_speed=8000;}
		}
		else if(length_temp < -160)
		{
			if(s_speed>5000){s_speed-=500;}
			else{s_speed=5000;}
		}
		else if(length_temp < -31)
		{
			if(s_speed<8000){s_speed+=500;}
			else{s_speed=8000;}	
		}
		else
		{
			if(s_speed>500){s_speed-=500;}
			else{s_speed=0;}
			if(abs((int)Chassis_MOTO[0].getpara.speed_rpm)<5){s_direction *= -1;}
		}
	}
	setspeed(&Chassis_MOTO[0],s_direction*s_speed);
}
int16_t s_move_length = 0;
/*用变量记录检测到柱子的编码器距离*/

int16_t GetLightlength(int8_t direction)
{
	/*记录激光状态*/
	static uint8_t s_status_right = 0,s_status_left = 1;
	static int16_t offset=0;
 uint8_t temp_right=HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_0);
 uint8_t temp_left=HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_1);
	
	if(temp_right!=s_status_right)
	{
		offset=(direction>0?(-55):(-114))-Chassis_MOTO[0].getpara.xiangdui_round;
		s_move_length=Chassis_MOTO[0].getpara.xiangdui_round;
	}
	if(temp_left!=s_status_left)
	{
		offset=(direction>0?(-235):(-275))-Chassis_MOTO[0].getpara.xiangdui_round;
		s_move_length=Chassis_MOTO[0].getpara.xiangdui_round;
	}
	s_status_right=temp_right;s_status_left=temp_left;
	return offset;
}

float mocalun_ccr = 1000;
uint8_t shoot_flag;
float shoot_pending;
void mocalun_state_monitor(void)
{
		overall_mode.mocalun_state.Mocalun_Connect_TO_2006_count++;
	if(overall_mode.mocalun_state.Mocalun_Connect_TO_2006_count>20)
		overall_mode.mocalun_state.Mocalun_Connect_TO_2006=1;
	else
		overall_mode.mocalun_state.Mocalun_Connect_TO_2006=0;
	
	if(overall_mode.mocalun_state.Mocalun_Connect_TO_2006==1)
	{
			TIM2->CCR1 =1000;
			TIM2->CCR2 =1000;
			shoot_flag=0;
	}
	else
	{
		shoot_flag=1;
		if(RoboData.robo_ctrlmode.ctrl_source== AUTO_MOVE_MODE2)
		{
			if(shoot_pending>250)
			{
			TIM2->CCR1 =1250;//FrictionMotorServo(mocalun_ccr);
			TIM2->CCR2 = 1250;//FrictionMotorServo(mocalun_ccr);
				shoot_pending=0;
			}
		}
		else 
		{
			TIM2->CCR1 =FrictionMotorServo(mocalun_ccr);
			TIM2->CCR2 = FrictionMotorServo(mocalun_ccr);
		}
	}
	
					if(RC_CtrlData.rc.s1 !=  RC_CtrlData.rc.s1_last && RoboData.robo_ctrlmode.left_mode == LEFT_1)
				{
					if(mocalun_ccr==1250)//1142
						mocalun_ccr=1000;
					else if(mocalun_ccr==1000)
						mocalun_ccr=1250;
						
				}
	
}



void Turn(void)
{
	if(overall_mode.Chassis_derection ==1)
	{
		if(HAL_GPIO_ReadPin(Right_GPIO,Right_Pin)==0)
			Turn_Pending_Right++;
		else		Turn_Pending_Right=0;
	}
	else if(overall_mode.Chassis_derection ==2) 
	{
		if(HAL_GPIO_ReadPin(Left_GPIO,Left_Pin)==0)
			Turn_Pending_Left++;
		else 
			Turn_Pending_Left=0;
	}
	if(Turn_Pending_Left>0)
	{

		overall_mode.Chassis_derection = 1;
		
//			if(((overall_mode.OvermodeSta==dodge_level2||overall_mode.OvermodeSta == attack)&&overall_mode.dodge_level2.danfangxiangzou_flag==1)&&(chassissmode==2))
//				overall_mode.dodge_level2.danfangxiangzou_flag=2;//保证位置控制终点一定
//			if(((overall_mode.OvermodeSta==dodge_level2||overall_mode.OvermodeSta == attack)&&overall_mode.dodge_level2.danfangxiangzou_flag==2)&&(chassissmode==2))
//				overall_mode.dodge_level2.danfangxiangzou_flag=3;//记录位置之后，进入位置躲避计算
	Turn_Pending_Left=0;
	}
	else if(Turn_Pending_Right>0)
	{
		overall_mode.Chassis_derection = 2;		
//			if(((overall_mode.OvermodeSta==dodge_level2||overall_mode.OvermodeSta == attack)&&overall_mode.dodge_level2.danfangxiangzou_flag==0)&&(chassissmode==2))
//				overall_mode.dodge_level2.danfangxiangzou_flag=1;//保证位置控制起点一定
//			if(((overall_mode.OvermodeSta==dodge_level2||overall_mode.OvermodeSta == attack)&&overall_mode.dodge_level2.danfangxiangzou_flag==2)&&(chassissmode==2))
//				overall_mode.dodge_level2.danfangxiangzou_flag=3;//记录位置之后，进入位置躲避计算	
	Turn_Pending_Right=0;
	}
	if(overall_mode.dodge_level2.danfangxiangzou_flag>0)
	{
		if(Chassis_MOTO[0].getpara.xiangdui_round<guidao_changdu_qidian-LONGTH)
			overall_mode.Chassis_derection = 1;	
		else if(Chassis_MOTO[0].getpara.xiangdui_round>guidao_changdu_qidian-1.5f)
			overall_mode.Chassis_derection = 2;
		
}
	
	if(overall_mode.dodge_level2.danfangxiangzou_flag==3)
	{
		if(Chassis_MOTO[0].getpara.xiangdui_round>guidao_changdu_qidian)
			overall_mode.Chassis_derection = 2;
		else if(Chassis_MOTO[0].getpara.xiangdui_round<guidao_changdu_zhongdian)
			overall_mode.Chassis_derection = 1;			
			
	}
}

/*-------------------------------Init--------------------------------------------*/


//初始化自动功能结构体
void Init_OverallMode(OverallMode_t *overall_mode)
{
	overall_mode->yaw.yaw_auto_angle_set = 0;	
	overall_mode->ChassisMotoAutoMode = &ChassisMotoAutoMode;
	overall_mode->yaw.YawMotoAutoMode = &YawMotoAutoMode;
	overall_mode->pitch.pit_auto_angle_set = -10;
	overall_mode->pitch.PitchMotoAutoMode = &PitchMotoAutoMode;
	overall_mode->OvermodeSta=dodge_level2;
	overall_mode->chassiss_control_mode = SPEED;
	overall_mode->Overall_mode_gimbal = 0;
	overall_mode->yaw_derection = 1;
	overall_mode->pitch_derection = 1;
	overall_mode->Chassis_derection = 1;
	overall_mode->yu_shu = 1;
}


void AllAuto(void)
{
			YawAngle_get();

	RoboData.GetRemoteControlData(&RoboData);//遥控器数据处理

	
//	if(chassissmode==2)
//		overall_mode.ChassisMotoAutoMode(Chassis_MOTO);
//	else if(chassissmode==1)
//		setspeed(&Chassis_MOTO[0],direction*0);
	if(chassissmode==1)
	{
		Repeated_move();
	}
	else if(chassissmode==2){Chassis_MOTO[0].getpara.xiangdui_round=0;}
	
//		static uint8_t ring;
//	static uint8_t ring_flag=GPIO_PIN_SET;

//if(ring_ready)
//{	
//		ring++;
//		if(ring==5)
//		{
//		if(ring_flag==GPIO_PIN_SET)
//			ring_flag=GPIO_PIN_RESET;
//		else
//			ring_flag=GPIO_PIN_SET;
//		ring=0;
//		}
//}
//		HAL_GPIO_WritePin(GPIOH,GPIO_PIN_6,ring_flag);//灭	
				restbullet();
}

