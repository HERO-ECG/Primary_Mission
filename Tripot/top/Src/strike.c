#include "strike.h"
#include "control.h"
#include "t_monitor.h"
#include "tripod.h"
#include "math.h"
#include "robodata.h"

Strike_t Strike;




void Strike_SetModeFromControl(void)
{
	static Key_Data_t key_last;
	static RoboControlMode_t switch_last;
	static uint8_t AimMode;//自动瞄准为1
	static uint8_t StrikeMode;//自动打击为1
	static uint8_t FireMode;//0自动 1三连发 2单发
	static uint16_t correct_angle_clear_delay;//用于判定双击清零手动自瞄修正角
	static uint8_t WindMillMode;//1顺时针 2逆时针 3静止 0不用 4手打静止
	static uint8_t setlevel;
	static uint8_t ShootingRange;//0近距离，1远距离
	
	Strike.fire.cease_fire_code=0;//初始禁止发弹标志位为0
	if(RoboData.robo_ctrlmode.ctrl_source==FROM_PC)
	{
		if(KEY_FIRE==1)
		{
			Strike.fire.now=1;
			Strike.friction_ctrl.IsOn=1;
		}
		else
		{
			Strike.fire.now=0;
		}

		if(RC_CtrlData.key.key_data.V==1&&Strike.strikemode!=WindMill_Manual)
			StrikeMode=1;
		else
			StrikeMode=0;
		
		if(KEY_AUTOAIM==1)
			AimMode=1;
		else
			AimMode=0;

		
		if(RC_CtrlData.key.key_data.C==1&&key_last.C==0)
		{
			if(RC_CtrlData.key.key_data.ctrl==0&&
					RC_CtrlData.key.key_data.shift==0)
				FireMode=0;//射击模式设置 全自动
			else if(RC_CtrlData.key.key_data.ctrl==0&&
							RC_CtrlData.key.key_data.shift==1)
				FireMode=2;//射击模式设置 单发
			else if(RC_CtrlData.key.key_data.ctrl==1&&
							RC_CtrlData.key.key_data.shift==0)
				FireMode=1;//射击模式设置 三连发
			else	if(RC_CtrlData.key.key_data.shift==1
							&&RC_CtrlData.key.key_data.ctrl==1)
				{
					Strike.heat.unlimited=1;//无线热量开启
					Strike.heat.unlimited_counter=0;//无线热量有限计时开始
				}
		}
		
		if(correct_angle_clear_delay<0xffff)
			correct_angle_clear_delay++;
		

		if(RC_CtrlData.key.key_data.Q==1)//与一键回头绑定
			Strike.cover_open=0;//0是关
		
		if(RC_CtrlData.key.key_data.E==1&&key_last.E==0)//开关弹仓盖
			{
				if(RC_CtrlData.key.key_data.ctrl==0)
					Strike.cover_open=0; 
				else
					Strike.cover_open=1;
			}
			
		if(RC_CtrlData.key.key_data.G==1&&key_last.G==0)
			{
				if(RC_CtrlData.key.key_data.shift==0&&RC_CtrlData.key.key_data.ctrl==0)
				{
					ShootingRange=0;//按下G近距离
					FireMode=0;
					
				}
				else if(RC_CtrlData.key.key_data.shift==1&&RC_CtrlData.key.key_data.ctrl==1)
				{
					Strike.heat.unlimited=0;
					setlevel++;//control+G 设置等级
				}
				else if(RC_CtrlData.key.key_data.shift==1&&RC_CtrlData.key.key_data.ctrl==0)
					ShootingRange=1;//shift+G远距离
			}
			if(setlevel>0)
			{
				if(RC_CtrlData.key.key_data.ctrl==0)//如果松开ctrl，则等级设置完成
				{
					Strike.heat.set_level=setlevel;
					setlevel=0;
				}
			}
			
			if(RC_CtrlData.key.key_data.B==1&&key_last.B ==0)//按下修正角按键
			{
				if(RC_CtrlData.key.key_data.shift==1&&RC_CtrlData.key.key_data.ctrl==0)
				{
					Strike.vision.manual_correct_offset.delta_yaw+=1.5f;//左修正修正角
				} 
			else if(RC_CtrlData.key.key_data.shift==0&&RC_CtrlData.key.key_data.ctrl==1)
				Strike.vision.manual_correct_offset.delta_yaw-=1.5f;//右修正修正角
			}
			
			if(RC_CtrlData.key.key_data.B==1&&key_last.B==0&&(!((RC_CtrlData.key.key_data.ctrl==1)&&(RC_CtrlData.key.key_data.shift==1))))
			{		
				if(correct_angle_clear_delay<250&&(RC_CtrlData.key.key_data.shift==0&&RC_CtrlData.key.key_data.ctrl==0))//防止和设置冲突
				{
					Strike.vision.manual_correct=0;
					Strike.vision.manual_correct_clear=1;
				}
				else
				{
					Strike.vision.manual_correct=1;
					Strike.vision.manual_correct_clear=0;
				}
				correct_angle_clear_delay=0;
			}
		
		if(RC_CtrlData.key.key_data.B==0)//松开修正按键，则停止修正
			Strike.vision.manual_correct=0;

			
		if(WindMillMode>0)
		{
			if(RC_CtrlData.key.key_data.W||RC_CtrlData.key.key_data.S||RC_CtrlData.key.key_data.A||RC_CtrlData.key.key_data.D||RC_CtrlData.key.key_data.Q)//松开就取消
				WindMillMode=0;
		}
		
		
		if(RC_CtrlData.key.key_data.Z==1&&key_last.Z==0)
		{
			if(RC_CtrlData.key.key_data.ctrl==1&&RC_CtrlData.key.key_data.shift==0)
				WindMillMode=2;
			else if(RC_CtrlData.key.key_data.ctrl==0&&RC_CtrlData.key.key_data.shift==1)
				WindMillMode=4;
//			else if(RC_CtrlData.key.key_data.ctrl==1&&RC_CtrlData.key.key_data.shift==1)
//				WindMillMode=3;
		}
		

		if(WindMillMode==4)
		{
			if(RC_CtrlData.key.key_data.X==1&&key_last.X==0)
			{
				if(RC_CtrlData.key.key_data.shift==0&&RC_CtrlData.key.key_data.ctrl==0)
					Strike.windmill_manual.aim_num=LEFT_UP;
				else if(RC_CtrlData.key.key_data.shift==0&&RC_CtrlData.key.key_data.ctrl==1)
					Strike.windmill_manual.aim_num=LEFT_DOWN;
			}
			if(RC_CtrlData.key.key_data.C==1&&key_last.C==0)
			{
				if(RC_CtrlData.key.key_data.shift==0&&RC_CtrlData.key.key_data.ctrl==0)
					Strike.windmill_manual.aim_num=RIGHT_UP;
				else if(RC_CtrlData.key.key_data.shift==0&&RC_CtrlData.key.key_data.ctrl==1)
					Strike.windmill_manual.aim_num=RIGHT_DOWN;
			}
			if(RC_CtrlData.key.key_data.Z==1&&key_last.Z==0)
			{
				if(RC_CtrlData.key.key_data.shift==0&&RC_CtrlData.key.key_data.ctrl==0)
					Strike.windmill_manual.aim_num=MID_UP;
			}
			
			if(RC_CtrlData.key.key_data.V==1&&key_last.V==0)
			{
				if(RC_CtrlData.key.key_data.shift==0&&RC_CtrlData.key.key_data.ctrl==0)
					Strike.windmill_manual.IsCalibrated=1;
				else if(RC_CtrlData.key.key_data.shift==0&&RC_CtrlData.key.key_data.ctrl==1)
					Strike.windmill_manual.IsCalibrated=0;
			}
		}
		
		
		
		}
		else //遥控器
		{
			if((RoboData.robo_ctrlmode.left_mode==LEFT_down)&&(switch_last.left_mode!=LEFT_down))
			{
				Strike.friction_ctrl.IsOn=!Strike.friction_ctrl.IsOn;
			}
			
			if((RoboData.robo_ctrlmode.left_mode==LEFT_up)&&(switch_last.left_mode!=LEFT_up))
			{
				Strike.cover_open=!Strike.cover_open;
			}
			
		if(RoboData.robo_ctrlmode.ctrl_source==RIGHT_reserve)
				Strike.fire.now=1;
			else
				Strike.fire.now=0;
		}
		
		if(ShootingRange==0)
			Strike.friction_ctrl.shooting_range=Close_Range;//射程->摩擦轮转速
		else
			Strike.friction_ctrl.shooting_range=Long_Range;

	if(TX2_Data.mode_now!=TX2_Data.mode_set)//TX2无响应指示
		Strike.vision.TX2_no_response=1;
	else	
		Strike.vision.TX2_no_response=0;
		
		
	
		if(WindMillMode==1)
			TX2_Data.mode_set=WINDMILL_CLKWISE;
		else if(WindMillMode==2)
			TX2_Data.mode_set=WINDMILL_ANTICLKWISE;
		else if(WindMillMode==3)
			TX2_Data.mode_set=WINDMILL_STILL;
		else if(WindMillMode==4)
		{
			TX2_Data.mode_set=WINDMILL_MANUAL;
			
		}
		else
		{
			if(Strike.friction_ctrl.shooting_range==Close_Range)
				TX2_Data.mode_set=AUTOAIM_CLOSE;
			else
				TX2_Data.mode_set=AUTOAIM_LONG;			
		}
	if(TX2_Data.distance>0&&Strike.vision.TX2_no_response==0)//若有相应
		{
			if(TX2_Data.mode_now==AUTOAIM_CLOSE||TX2_Data.mode_now==AUTOAIM_LONG)
			{

					if(StrikeMode==1)
					{
						if(AimMode==1)
						{
							Strike.strikemode=AutoAim_AutoStrike;//设置相应自瞄相关模式
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
			else if(TX2_Data.mode_now==WINDMILL_ANTICLKWISE)
			{
				Strike.strikemode=WindMill_Anticlkwise;
				FireMode=2;
			}
			else if(TX2_Data.mode_now==WINDMILL_CLKWISE)
			{
				Strike.strikemode=WindMill_Clkwise;
				FireMode=2;
			}
			else if(TX2_Data.mode_now==WINDMILL_STILL)
			{
				Strike.strikemode=WindMill_Still;
				FireMode=2;
			}
		}
		else
			Strike.strikemode=ManualAim_ManualStrike;


		if(Strike.vision.TX2_no_response==1)//无响应则手动打击&瞄准
			Strike.strikemode=ManualAim_ManualStrike;
		
		if(WindMillMode==4)
		{
			Strike.strikemode=WindMill_Manual;
			FireMode=2;
		}
		
		
		if(FireMode==0)//设置开火模式
		{
			Strike.firemode.now=Auto_Mode;
		}
		else if(FireMode==1)
		{
			Strike.firemode.now=Burst_Mode;
		}
		else if(FireMode==2)
		{
			Strike.firemode.now=Semi_Mode;
		}
		
		if(Strike.firemode.now!=Auto_Mode)
		{
			Strike.friction_ctrl.shooting_range=Long_Range;
			ShootingRange=1;
			
		}

		
		key_last=RC_CtrlData.key.key_data;
		switch_last=RoboData.robo_ctrlmode;
		
}

void FrictionMotorServo(void)
{
	static uint16_t divider;
	static int32_t speed_now=0;
	static int32_t speed_now2=0;
	static uint16_t init_counter;
	
	divider++;
	if(divider==10000)
		divider=0;
	

	if(init_counter<3000)
	{
		init_counter++;
		TIM1->CCR3=1000;
		TIM1->CCR2=1000;
		return;
	}
	

	if(speed_now<Strike.friction_ctrl.friction_moto_pwm_pulse_left)
		speed_now+=1;
	if(speed_now>Strike.friction_ctrl.friction_moto_pwm_pulse_left)
		speed_now-=1;
	if(speed_now<1000)
		TIM1->CCR3=1000;
	else
	TIM1->CCR3=speed_now;
	
	if(ABS(speed_now-Strike.friction_ctrl.friction_moto_pwm_pulse_left)<5)
	{
		if(speed_now2<Strike.friction_ctrl.friction_moto_pwm_pulse_right)
			speed_now2+=1;
		if(speed_now2>Strike.friction_ctrl.friction_moto_pwm_pulse_right)
			speed_now2-=1;
		if(speed_now2<1000)
			TIM1->CCR2=1000;
		else
			TIM1->CCR2=speed_now2;
	}
	
}
uint8_t IsOnTarget(float dp,float dy)
{
	float dangle;
	dangle=sqrt(fabs(dp*dp+dy*dy));
	if(dangle<Strike.vision.OnTargetAngleMax)
		return 1;
	else
		return 0;
} 

float angular_velocity_history[51];//最后一位表示是不是空的。没时间整了。
uint16_t Angular_length=35;
float AngularVelocityQueue(float angular_velocity,uint16_t index,uint8_t mode)
{
	static uint16_t pointer;
	static uint16_t divider;
	uint16_t i;
	
	if(mode==SET)
	{
		if(divider<10000)
			divider++;
		else
			divider=0;
//		if(divider%5!=0)
//			return 0;
		
		pointer++;
		if(pointer==Angular_length)
		{
			pointer=0;
			angular_velocity_history[50]=1;
		}
		angular_velocity_history[pointer]=angular_velocity;
		return 0;
	}
	else if(mode==GET)
	{
		if(index>=Angular_length)
		{
			return 0;
		}
		else
		{
			if(index<pointer)
			{
				return angular_velocity_history[pointer-index];
			}
			else
			{
				return angular_velocity_history[Angular_length-index+pointer-1];
			}
		}
	}
	else
	{
		for(i=0;i<Angular_length;i++)
			angular_velocity_history[i]=0;
		angular_velocity_history[50]=0;
		pointer=0;
		return 0;
	}
}

float GetVariance(float *data,uint16_t length)
{
	float average;
	float sum=0;
	float variance;
	uint16_t i;
	
	for(i=0;i<length;i++)
		sum+=data[i];
	average=sum/length;
	for(i=0;i<length;i++)
		variance+=(data[i]-average)*(data[i]-average)/length;
	return variance;
}

float GetAverage(float *data,uint16_t length)
{
	float average;
	float sum=0;
	uint16_t i;
	
	for(i=0;i<length;i++)
		sum+=data[i];
	average=sum/length;
	return average;
}

void AutoStrike(void)
{
	if(Strike.strikemode==WindMill_Clkwise||Strike.strikemode==WindMill_Anticlkwise||Strike.strikemode==WindMill_Still)
	{
		Strike.vision.deltaangle_now.delta_pitch=TX2_Data.pitch_angle+TX2_WINDMILL_OFFSET_PITCH;
		Strike.vision.deltaangle_now.delta_yaw=-TX2_Data.yaw_angle+TX2_WINDMILL_OFFSET_YAW;
	}		
	else
	{
		Strike.vision.deltaangle_now.delta_pitch=TX2_Data.pitch_angle+TX2_OFFSET_PITCH;
		Strike.vision.deltaangle_now.delta_yaw=-TX2_Data.yaw_angle+TX2_OFFSET_YAW;
	}
	
	float pitch_before;
	float yaw_before;
	static uint32_t OnTargetShootCounter;
	static uint8_t OnTargetShootPendingFlag;
	static uint32_t angular_velocity_counter;//目标角速度计算时基
	static float yaw_last;//用于角速度计算
	float bullet_speed;
	
	AngleQueue(&Tripod.Pitch.angle,&Tripod.Yaw.angle,0,SET);
	
	if(TX2_Data.distance>0)
		Strike.vision.OnTargetAngleMax=7.5f/Strike.vision.prediction.distance;
	if(Strike.friction_ctrl.shooting_range==Close_Range)
		bullet_speed=20;
	else
		bullet_speed=27;
	
	if(TX2_Data.mode_now==OFFLINE)//TX2都掉线了，还玩个球的
	{
		Strike.vision.TX2_no_response=1;
		return;
	}
	
	if(angular_velocity_counter<10000)
		angular_velocity_counter++;
	else
		angular_velocity_counter=0;

		if(Strike.vision.deltaangle_now.delta_pitch!=Strike.vision.deltaangle_last.delta_pitch||//自瞄角度更新了
		Strike.vision.deltaangle_now.delta_yaw!=Strike.vision.deltaangle_last.delta_yaw)
	{
		Strike.vision.autoaim_flag=1;//更新后可以转动
		if(angular_velocity_counter<400)//时间太久则数据弃用
		{
			Strike.vision.prediction.distance=TX2_Data.distance*0.02f;
			Strike.vision.prediction.yaw.angular_velocity_vision=(Strike.vision.deltaangle_now.delta_yaw-Strike.vision.deltaangle_last.delta_yaw)/((float)angular_velocity_counter)*1000.0f;
			
			
			if(yaw_last<Tripod.Yaw.angle)
			{
				if(Tripod.Yaw.angle-yaw_last<180)
					Strike.vision.prediction.yaw.angular_velocity_tripod=(Tripod.Yaw.angle-yaw_last)/((float)angular_velocity_counter)*1000.0f;
				else
					Strike.vision.prediction.yaw.angular_velocity_tripod=((Tripod.Yaw.angle-yaw_last)-360)/((float)angular_velocity_counter)*1000.0f;
			}
			else
			{
				if(yaw_last-Tripod.Yaw.angle<180)
					Strike.vision.prediction.yaw.angular_velocity_tripod=(Tripod.Yaw.angle-yaw_last)/((float)angular_velocity_counter)*1000.0f;
				else
					Strike.vision.prediction.yaw.angular_velocity_tripod=(360-(yaw_last-Tripod.Yaw.angle))/((float)angular_velocity_counter)*1000.0f;
			}
			
			Strike.vision.prediction.yaw.angular_velocity_tripod=Tripod.Yaw.speed;
			
			
			Strike.vision.prediction.yaw.angular_velocity_absolute=Strike.vision.prediction.yaw.angular_velocity_vision+Strike.vision.prediction.yaw.angular_velocity_tripod;
			if(fabs(Strike.vision.prediction.yaw.angular_velocity_absolute)<100)//去除异常值
				AngularVelocityQueue(Strike.vision.prediction.yaw.angular_velocity_absolute,0,SET);
			Strike.vision.prediction.yaw.variance=GetVariance(angular_velocity_history,Angular_length);
			if(angular_velocity_history[50]==1)//如果队列已满
			{
				if(Strike.vision.prediction.yaw.variance<Strike.vision.prediction.yaw.variance_valid_threshold)
					Strike.vision.prediction.yaw.angular_velocity_absolute_filtered=GetAverage(angular_velocity_history,Angular_length);
				else
				{
					Strike.vision.prediction.yaw.angular_velocity_absolute_filtered=0;
					AngularVelocityQueue(0,0,CLR);
					Strike.vision.prediction.yaw.angular_velocity_vision=0;
					Strike.vision.prediction.yaw.angular_velocity_tripod=0;
					Strike.vision.prediction.yaw.angular_velocity_absolute=0;
				}
				Strike.vision.prediction.yaw.angle_predicted_ahead=(Strike.vision.prediction.distance/bullet_speed+0.05f)*Strike.vision.prediction.yaw.angular_velocity_absolute_filtered;
			}
			else
				Strike.vision.prediction.yaw.angle_predicted_ahead=0;
		
			if(Strike.vision.prediction.yaw.angle_predicted_ahead>10)
				Strike.vision.prediction.yaw.angle_predicted_ahead=10;
			if(Strike.vision.prediction.yaw.angle_predicted_ahead<-10)
				Strike.vision.prediction.yaw.angle_predicted_ahead=-10;
			yaw_last=Tripod.Yaw.angle;
		}
		else
		{
			AngularVelocityQueue(0,0,CLR);
			Strike.vision.prediction.yaw.angular_velocity_vision=0;
			Strike.vision.prediction.yaw.angular_velocity_tripod=0;
			Strike.vision.prediction.yaw.angular_velocity_absolute=0;
		}
		angular_velocity_counter=0;	
	}
	
	if(TX2_Data.distance>0)
	{
		if(IsOnTarget(Strike.vision.deltaangle_now.delta_pitch,Strike.vision.deltaangle_now.delta_yaw)||
			IsOnTarget(Strike.vision.deltaangle_now.delta_pitch,Strike.vision.deltaangle_now.delta_yaw+Strike.vision.prediction.yaw.angle_predicted_ahead+Strike.vision.manual_correct_offset.delta_yaw))
		{
			OnTargetShootPendingFlag=1;
			OnTargetShootCounter=0;
		}
		else
		{
			OnTargetShootCounter=0xffff;
			OnTargetShootPendingFlag=0;
		}
	}
	if(OnTargetShootPendingFlag==1&&OnTargetShootCounter<0xffff)
		OnTargetShootCounter++;
	if(OnTargetShootCounter<300)//如果在击发时间范围内
		{
			Strike.vision.aimstate=OnTarget;
		}
	else
		{
			Strike.vision.aimstate=OffTarget;
			OnTargetShootPendingFlag=0;
		}

	if(Strike.vision.manual_correct==1)//如果是修正模式，则开始记录修正角
		{
			Strike.vision.manual_correct_offset.delta_pitch+=-(float)(RC_CtrlData.mouse.y_speed)*PITCHSENSITIVITY;
			Strike.vision.manual_correct_offset.delta_yaw+=-((float)(RC_CtrlData.mouse.x_speed))*YAWSENSITIVITY;
		}
		if(Strike.vision.manual_correct_clear==1)//修正角清空
		{
			Strike.vision.manual_correct_offset.delta_pitch=0;
			Strike.vision.manual_correct_offset.delta_yaw=0;
		}
		
	if(Strike.strikemode==AutoAim_AutoStrike||Strike.strikemode==AutoAim_ManualStrike)//如果是自动瞄准
	{
		Tripod.control_mode.now=Position;//进入位置模式
		if(Strike.vision.autoaim_flag==1)
		{
			AngleQueue(&pitch_before,&yaw_before,0,GET);//读取之前的坐标
			Tripod.Pitch.angle_set=pitch_before+Strike.vision.deltaangle_now.delta_pitch+Strike.vision.manual_correct_offset.delta_pitch;
			Tripod.Yaw.angle_set=yaw_before+Strike.vision.deltaangle_now.delta_yaw+Strike.vision.prediction.yaw.angle_predicted_ahead+Strike.vision.manual_correct_offset.delta_yaw;
			Strike.vision.autoaim_flag=0;//赋值并清空标志位，在下一次数据更新后再进行自瞄
			
		}
	}
	else if(Strike.strikemode==WindMill_Clkwise||Strike.strikemode==WindMill_Anticlkwise||Strike.strikemode==WindMill_Still)
	{
		Tripod.Pitch.angle_set=Strike.vision.deltaangle_now.delta_pitch;//非增量式
		Tripod.Yaw.angle_set=Strike.vision.deltaangle_now.delta_yaw;
		Strike.vision.autoaim_flag=0;//赋值并清空标志位，在下一次数据更新后再进行自瞄
	}
	
	if(Strike.strikemode==ManualAim_AutoStrike||Strike.strikemode==AutoAim_AutoStrike)
	{
		if(Strike.vision.aimstate==OnTarget)
			Strike.fire.now=1;
		else
			Strike.fire.now=0;
	}
	

	Strike.vision.deltaangle_last=Strike.vision.deltaangle_now;
}




void ToggleMotoControl(void)
{
	static uint8_t lastshot;
	static uint8_t nowshot;
	static uint64_t counter;
	static uint16_t shootnum_last;
	uint8_t fire_once;
	
	Strike.toggle_motor.angle=(TG_MOTO[0].getpara.total_round)*10.0f;
	Strike.toggle_motor.speed=TG_MOTO[0].getpara.speed_rpm/6.0f;
	
//	if(Strike.heat.cooling_limit_expected-Strike.heat.heat_mingle<80)
//	{
//		Strike.count.period=80+200.0f*(((float)Strike.heat.cooling_limit_expected-(float)Strike.heat.heat_mingle)/80.0f);
//	}
//	else
//		Strike.count.period=80;

	if(Strike.fire.now==1)
		nowshot=1;
	else
		nowshot=0;
	if(nowshot==1&&lastshot==0)
		fire_once = 1;
	else
		fire_once = 0;
		

		if(Strike.firemode.now==Auto_Mode)
		{
			if(nowshot==1)
				Strike.count.num_to_shot=2;//自动则一直有弹。设置为2而不是1是为了避开下面首次瞬时击发，使连发射速可控
			else
				Strike.count.num_to_shot = 0;//松开就不打
		}
		else
		{
			if(fire_once==1)
			{
				if(Strike.firemode.now==Burst_Mode)
					Strike.count.num_to_shot+=3;
				else if(Strike.firemode.now==Semi_Mode)
					Strike.count.num_to_shot+=1;
			}
		}
		
		if(Strike.fire.cease_fire_code>0)//禁止发弹保护
		{
			Strike.count.num_to_shot = 0;
	//		Strike.toggle_motor.angle_set=Strike.toggle_motor.angle;//拨弹叉立刻停止转动
		}
		
		
		if(Strike.count.num_to_shot>3)//防止卡弹击发过多。
			Strike.count.num_to_shot=3;
		
		if(counter<0xffffffff)
			counter++;
		else
			counter=0;
		
		
		
		if(Strike.count.num_to_shot==0&&fabs(Strike.toggle_motor.PID_angle.err.now)<2.0f)
		{
			if(Strike.toggle_motor.angle>54000.0f)
			{
				
				Strike.toggle_motor.angle=mode360(Strike.toggle_motor.angle);
				Strike.toggle_motor.angle_set=mode360(Strike.toggle_motor.angle_set);
				
				TG_MOTO[0].getpara.round_cnt=Strike.toggle_motor.angle*0.1f;
				
			}
		}
		if(Strike.count.num_to_shot>0&&shootnum_last==0)
			counter=0;
		if(counter%(Strike.count.period+1)==0)
		{
			if(Strike.count.num_to_shot>0&&fabs(Strike.toggle_motor.PID_angle.err.now)<200.0f)
			{
				Strike.count.num_to_shot--;
				Strike.toggle_motor.angle_set+=65;
				Strike.count.all++;
				if(Strike.friction_ctrl.shooting_range==Long_Range)
					Strike.heat.heat_local+=30;
				else
					Strike.heat.heat_local+=22;
			}
		}
		Strike.toggle_motor.speed_set=PidCalc(&Strike.toggle_motor.PID_angle,Strike.toggle_motor.angle,Strike.toggle_motor.angle_set);
		Strike.toggle_motor.current_set=PidCalc(&Strike.toggle_motor.PID_speed,Strike.toggle_motor.speed,Strike.toggle_motor.speed_set);
		
		lastshot=nowshot;
		shootnum_last=Strike.count.num_to_shot;
		
		
}

void Strike_IntFun_out(void)
{
//	Strike.count.out_firecounter=0;
//	Strike.count.fire_flag=1;
}



void Strike_IntFun_in(void)
{
//	if(Strike.count.in_firecounter>IN_FIRE_COUNTER )
//	{
//		Strike.count.in_firecounter=0;
//		Strike.count.all++;
//		if(Strike.friction_ctrl.shooting_range==Long_Range)
//			Strike.heat.heat_local+=30;
//		else
//			Strike.heat.heat_local+=22;
//		if(Strike.firemode.now==Burst_Mode)
//		{
//			Strike.count.burst++;
//		}
//		Strike.count.fire_flag=1;
//	}
}





void CoverControl(void)
{
	if(Strike.cover_open==0)
		TIM1->CCR4=2280;
	else
		TIM1->CCR4=1100;
}

void AngleQueue(float *pitch,float *yaw,uint16_t index,uint8_t mode)
{
	static uint16_t pointer;
	static uint16_t divider;
	static float pitch_history[50];
	static float yaw_history[50];
	

	if(mode==SET)
	{
		divider++;
		if(divider==10000)
			divider=0;
		pointer++;
		if(pointer==50)
			pointer=0;
		pitch_history[pointer]=*pitch;
		yaw_history[pointer]=*yaw;
	}
	else
	{
		if(index>=50)
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
				*pitch=pitch_history[50-index+pointer-1];
				*yaw=yaw_history[50-index+pointer-1];
			}
		}
	}
}


float HeatQueue(float heat,uint16_t index,uint8_t mode)
{
	static uint16_t pointer;
	static float heat_history[50];

	if(mode==SET)
	{
		pointer++;
		if(pointer==50)
			pointer=0;
		heat_history[pointer]=heat;
		return 0;
	}
	else
	{
		if(index>=50)
		{
			return 0;
		}
		else
		{
			if(index<pointer)
			{
				return heat_history[pointer-index];
			}
			else
			{
				return heat_history[50-index+pointer-1];
			}
		}
	}
}



void HeatCtrl(void)
{

	static float heat_sever_last;
	
	if(Strike.friction_ctrl.shooting_range==Long_Range)
		Strike.heat.margin=60;
	else
		Strike.heat.margin=50;
	
	if(Strike.heat.heat_local>1.5f*Strike.heat.cooling_limit_expected)
	{
		Strike.heat.heat_local=1.5f*Strike.heat.cooling_limit_expected;
	}
	
	if(Strike.heat.set_level>0)
	{
		if(Strike.heat.set_level<=3)
			Strike.heat.robot_level=Strike.heat.set_level;
		else
			Strike.heat.robot_level=3;
	}
 if(Strike.heat.robot_level<=1)
	{
		if(Strike.heat.cooling_limit_server>240)
			Strike.heat.cooling_limit_expected=Strike.heat.cooling_limit_server;
		else
			Strike.heat.cooling_limit_expected=240;
		if(Strike.heat.cooling_rate_expected>40)
			Strike.heat.cooling_rate_expected=Strike.heat.cooling_limit_server;
		else
			Strike.heat.cooling_rate_expected=40;
	}
	else if(Strike.heat.robot_level==2)
	{
		if(Strike.heat.cooling_limit_server>360)
			Strike.heat.cooling_limit_expected=Strike.heat.cooling_limit_server;
		else
			Strike.heat.cooling_limit_expected=360;
		if(Strike.heat.cooling_rate_expected>60)
			Strike.heat.cooling_rate_expected=Strike.heat.cooling_limit_server;
		Strike.heat.cooling_rate_expected=60;
	}
	else if(Strike.heat.robot_level==3)
	{
		if(Strike.heat.cooling_limit_server>480)
			Strike.heat.cooling_limit_expected=Strike.heat.cooling_limit_server;
		else
			Strike.heat.cooling_limit_expected=480;
				if(Strike.heat.cooling_rate_expected>80)
			Strike.heat.cooling_rate_expected=Strike.heat.cooling_limit_server;
		Strike.heat.cooling_rate_expected=80;
	}

			
	Strike.heat.heat_local-=Strike.heat.cooling_rate_expected/1000.0f;
	if(Strike.heat.heat_local<0)
		Strike.heat.heat_local=0;
	

		if(heat_sever_last!=Strike.heat.heat_server)
		{
			Strike.heat.heat_local = Strike.heat.heat_server;
			Strike.heat.heat_mingle = Strike.heat.heat_server;
		}
		else
		{
			Strike.heat.heat_mingle = Strike.heat.heat_local;
		}


	if(Strike.heat.unlimited==1)
	{
		if(Strike.heat.unlimited_counter<20000)
			Strike.heat.unlimited_counter++;
		else
		{
			Strike.heat.unlimited=0;
			Strike.heat.unlimited_counter=0;
		}
	}
	else
			Strike.heat.unlimited_counter=0;
	
	if(Strike.heat.heat_mingle>Strike.heat.cooling_limit_expected-Strike.heat.margin&&Strike.heat.unlimited==0)
	{
		Strike.heat.ceasefire=1;
		Strike.fire.cease_fire_code=1;
	}
	else
		Strike.heat.ceasefire=0;
	
	heat_sever_last = Strike.heat.heat_server;
}



void StrikeCount(void)
{
	
	if(Strike.count.in_firecounter<0xffff)
		Strike.count.in_firecounter++;
	if(Strike.count.in_flag==1)
	{
		if(Strike.count.out_firecounter<0xffff)
			Strike.count.out_firecounter++;
	}
	else
	{
		Strike.count.out_firecounter=0;
	}
	if(Strike.count.fire_flag==1)
	{
		Strike.count.fire_flag=0;
	}
	if(Strike.firemode.now==Burst_Mode)
		{
			if(Strike.count.burst>2)
			{
				Strike.count.burst=0;
			}
		}
	
}

void FrictionMotoCtrl(void)
{
	static uint16_t friction_on_counter;

	
	
	if(Strike.friction_ctrl.IsOn==1&&friction_on_counter<0xffff)
		friction_on_counter++;
	else
		friction_on_counter=0;

	
	if(friction_on_counter<2000)
		Strike.fire.cease_fire_code=2;
	
	if(Strike.friction_ctrl.IsOn==1)
	{
		if(Strike.friction_ctrl.shooting_range==Close_Range)
		{
			Strike.friction_ctrl.friction_moto_pwm_pulse_left=friction_of_close_range_left;
			Strike.friction_ctrl.friction_moto_pwm_pulse_right=friction_of_close_range_right;
		}
		else if(Strike.friction_ctrl.shooting_range==Long_Range)
		{
			Strike.friction_ctrl.friction_moto_pwm_pulse_left=friction_of_long_range_left;
			Strike.friction_ctrl.friction_moto_pwm_pulse_right=friction_of_long_range_right;
		}
	}
	else
	{
		Strike.friction_ctrl.friction_moto_pwm_pulse_left=1000;
		Strike.friction_ctrl.friction_moto_pwm_pulse_right=1000;		
	}
		FrictionMotorServo();
}

//float down1=4.16f;
//float down2=10.915f;
//float left1=5.737f;
//float left2=3.553f;

//float down1=3.56f;
//float down2=9.415f;
//float left1=5.0f;
//float left2=3.353f;


float down1=2.5f;
float down2=8;
float left1=5.0f;
float left2=3.353f;


void WindMill_ManualStrike(void)
{
	static uint8_t last_fire;
	static DeltaAngle_t last_fire_coordinate;
	static uint8_t last_IsCalibrated;
	
	if(Strike.strikemode!=WindMill_Manual)
	{
		Strike.windmill_manual.aim_num=0;
		Strike.windmill_manual.IsCalibrated=0;
		Strike.windmill_manual.base_angle.delta_pitch=0;
		Strike.windmill_manual.base_angle.delta_yaw=0;
	}
	else
	{
		if(Strike.fire.now==1&&last_fire==0)
		{
			last_fire_coordinate.delta_pitch=Tripod.Pitch.angle;
			last_fire_coordinate.delta_yaw=Tripod.Yaw.angle;
		}
		
		if(Strike.windmill_manual.IsCalibrated==1&&last_IsCalibrated==0)
		{
			Strike.windmill_manual.base_angle=last_fire_coordinate;
		}
		
		if(Strike.windmill_manual.IsCalibrated==1)
		{
			if(Strike.windmill_manual.aim_num==MID_UP)
			{
				Tripod.Pitch.angle_set=Strike.windmill_manual.base_angle.delta_pitch;
				Tripod.Yaw.angle_set=Strike.windmill_manual.base_angle.delta_yaw;
			}
			else if(Strike.windmill_manual.aim_num==LEFT_UP)
			{
				Tripod.Pitch.angle_set=Strike.windmill_manual.base_angle.delta_pitch-down1;
				Tripod.Yaw.angle_set=Strike.windmill_manual.base_angle.delta_yaw+left1;
			}
			else if(Strike.windmill_manual.aim_num==RIGHT_UP)
			{
				Tripod.Pitch.angle_set=Strike.windmill_manual.base_angle.delta_pitch-down1;
				Tripod.Yaw.angle_set=Strike.windmill_manual.base_angle.delta_yaw-left1;
			}
			else if(Strike.windmill_manual.aim_num==LEFT_DOWN)
			{
				Tripod.Pitch.angle_set=Strike.windmill_manual.base_angle.delta_pitch-down2;
				Tripod.Yaw.angle_set=Strike.windmill_manual.base_angle.delta_yaw+left2;
			}
			else if(Strike.windmill_manual.aim_num==RIGHT_DOWN)
			{
				Tripod.Pitch.angle_set=Strike.windmill_manual.base_angle.delta_pitch-down2;
				Tripod.Yaw.angle_set=Strike.windmill_manual.base_angle.delta_yaw-left2;
			}
		}
	}
	
	last_IsCalibrated=Strike.windmill_manual.IsCalibrated;
	last_fire=Strike.fire.now;
}
void Strike_Init(void)
{
	Strike.strikemode=ManualAim_ManualStrike;
	Strike.vision.OnTargetAngleMax=3.5f;
	Strike.firemode.now=Auto_Mode;
	Strike.shootspd=6000;
	Strike.friction_ctrl.IsOn=0;
	Strike.friction_ctrl.shooting_range=Close_Range;

	Strike.heat.sever_lag=0;
	Strike.heat.unlimited=0;
	
	Strike.count.period=80;
	
	Strike.vision.prediction.yaw.variance_valid_threshold = 1000;
	
	Strike.toggle_motor.PID_angle.pid_mode=POSITION_PID;
	Strike.toggle_motor.PID_angle.max_output=3000;
	Strike.toggle_motor.PID_angle.integral_uplimit=50;
	Strike.toggle_motor.PID_angle.integral_downlimit=-50;
	Strike.toggle_motor.PID_angle.p=60;
	Strike.toggle_motor.PID_angle.i=0;
	Strike.toggle_motor.PID_angle.d=0;
	Strike.toggle_motor.PID_angle.deadband=0;
	Strike.toggle_motor.PID_angle.f=0;
	Strike.toggle_motor.PID_angle.max_err=20;//5;
	Strike.toggle_motor.PID_angle.mid_err=5;//2;
	Strike.toggle_motor.PID_angle.min_err=0;
	Strike.toggle_motor.PID_angle.f_out_limit=0;
	Strike.toggle_motor.PID_angle.f_divider=0;
	
	Strike.toggle_motor.PID_speed.pid_mode=POSITION_PID;
	Strike.toggle_motor.PID_speed.max_output=10000;
	Strike.toggle_motor.PID_speed.integral_uplimit=6000;
	Strike.toggle_motor.PID_speed.integral_downlimit=-6000;
	Strike.toggle_motor.PID_speed.p=80;
	Strike.toggle_motor.PID_speed.i=0.5;
	Strike.toggle_motor.PID_speed.d=0;
	Strike.toggle_motor.PID_speed.deadband=0;
	Strike.toggle_motor.PID_speed.f=0;
	Strike.toggle_motor.PID_speed.max_err=60;
	Strike.toggle_motor.PID_speed.mid_err=30;
	Strike.toggle_motor.PID_speed.min_err=0;
	Strike.toggle_motor.PID_speed.f_out_limit=500;
	Strike.toggle_motor.PID_speed.f_divider=500;
	
	
}

void Strike_fun(void)
{
	Strike_SetModeFromControl();	
	HeatCtrl();
	AutoStrike();
	WindMill_ManualStrike();
	FrictionMotoCtrl();
	ToggleMotoControl();	
	CoverControl();
	Strike.fire.last=Strike.fire.now;
	Strike.firemode.last=Strike.firemode.now;
}


