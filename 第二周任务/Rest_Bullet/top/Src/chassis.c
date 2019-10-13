#include "chassis.h"
#include "math.h"
#include "t_monitor.h"

Chassis_t Chassis;


#define ABS(X) ((X>0)? (X): (-X))

float ChassissFollow(float angle_now,float angle_set,float deadband)
{
    float angle_err;
    float follow_speed_actual;
    while(angle_set<0)
        angle_set+=360;
    while(angle_set>360)
        angle_set-=360;
    if(angle_now<angle_set)
    {
        if(angle_set-angle_now<180)
            angle_err=angle_set-angle_now;
        else
            angle_err=(angle_set-angle_now)-360;
    }
    else
    {
        if(angle_now-angle_set<180)
            angle_err=angle_set-angle_now;
        else
            angle_err=360-(angle_now-angle_set);
    }

    follow_speed_actual=PidCalc(&Chassis.follow.pid,-angle_err,0);

    if(ABS(angle_err)<deadband)
        follow_speed_actual=0;
    else
    {
        if(follow_speed_actual>0)
            follow_speed_actual-=deadband*Chassis.follow.pid.p;
        else
            follow_speed_actual+=deadband*Chassis.follow.pid.p;
    }
    return follow_speed_actual;

}

/*底盘运动分解*/
void ChassisSetSpeed(float theta)
{
    float increasement;
    float speed_abs_all;

    uint8_t i;
    float ksin=0,kcos=0,W_sin=0,W_cos=0,D_sin=0,D_cos=0,cm_set_spd[5]= {0,0,0,0,0};
    static float spd_forward=0;
    static float spd_right=0;

    if(Chassis.power.mode==SLOW)
        increasement=0.1;
    else
        increasement=(0.02*(1.6-Chassis.power.speed_decreasement_k));

    if(Chassis.speed.spd_forward==0)
    {
        if(fabs(spd_forward-Chassis.speed.spd_forward)<1.5f*increasement)
            spd_forward=Chassis.speed.spd_forward;
        if(fabs(spd_forward-Chassis.speed.spd_forward)>0.4)
        {
            if(spd_forward>0)
                spd_forward=0.4;
            else
                spd_forward=-0.4;
        }
    }
    else
    {
        while(fabs(spd_forward-Chassis.speed.spd_forward)>1)
        {
            if(spd_forward>Chassis.speed.spd_forward)
                spd_forward=Chassis.speed.spd_forward+0.5;
            else
                spd_forward=Chassis.speed.spd_forward-0.5;
        }
        if(fabs(spd_forward-Chassis.speed.spd_forward)<0.1*fabs(Chassis.speed.spd_forward))
            spd_forward=Chassis.speed.spd_forward;
    }
    if(spd_forward<Chassis.speed.spd_forward)
        spd_forward+=increasement;
    else if(spd_forward>Chassis.speed.spd_forward)
        spd_forward-=increasement;

    if(Chassis.speed.spd_right==0)
    {
        if(fabs(spd_right-Chassis.speed.spd_right)<2.0f*increasement)
            spd_right=Chassis.speed.spd_right;
        if(fabs(spd_right-Chassis.speed.spd_right)>0.3)
        {
            if(spd_right>0)
                spd_right=0.3;
            else
                spd_right=-0.3;
        }
    }
    else
    {
        while(fabs(spd_right-Chassis.speed.spd_right)>1)
        {
            if(spd_right>Chassis.speed.spd_right)
                spd_right-=0.8;
            else
                spd_right+=0.8;
        }
        if(fabs(spd_right-Chassis.speed.spd_right)<0.2*fabs(Chassis.speed.spd_right))
            spd_right=Chassis.speed.spd_right;
    }
    if(spd_right<Chassis.speed.spd_right)
        spd_right+=1.5f*increasement;
    else if(spd_right>Chassis.speed.spd_right)
        spd_right-=1.5f*increasement;
    //theta>0为右转
    if(monitor_remote.status == monitor_regular)
    {
        ksin = (float)sin(theta*3.1416f/180.0f);
        kcos = (float)cos(theta*3.1416f/180.0f);
        W_sin = spd_forward*ksin;
        W_cos = spd_forward*kcos;
        D_sin = spd_right*ksin;
        D_cos = spd_right*kcos;
        //速度分配矩阵
        cm_set_spd[0] = ( (W_cos-D_sin) + (W_sin+D_cos) );
        cm_set_spd[1] = (-(W_cos-D_sin) + (W_sin+D_cos) );
        cm_set_spd[2] = ( (W_cos-D_sin) - (W_sin+D_cos) );
        cm_set_spd[3] = (-(W_cos-D_sin) - (W_sin+D_cos)	);
        for(i=0; i<4; i++)
        {
            Chassis.speed.set[i]= Chassis.speed.base * cm_set_spd[i] ;
        }
    }
    else
    {
        for(i=0; i<4; i++)
        {
            Chassis.speed.set[i]= 0;
        }

    }


    speed_abs_all=fabs(Chassis.speed.set[0])+fabs(Chassis.speed.set[1])+fabs(Chassis.speed.set[2])+fabs(Chassis.speed.set[3]);
    if(speed_abs_all>0)
    {
        if((1.0f+Chassis.power.speed_decreasement/speed_abs_all)<0)
            Chassis.power.speed_decreasement_k=0;
        else if((1.0f+Chassis.power.speed_decreasement/speed_abs_all)>1)
            Chassis.power.speed_decreasement_k=1;
        else
            Chassis.power.speed_decreasement_k=1.0f+Chassis.power.speed_decreasement/speed_abs_all;
    }
    else
        Chassis.power.speed_decreasement_k=1;
//	if(Chassis.speed.spd_yaw_actual<-Chassis.power.speed_decreasement)
//	{
//		Chassis.speed.spd_yaw_actual=Chassis.power.speed_decreasement;
//		Chassis.power.speed_decreasement_k=0;
//	}
    for(i=0; i<4; i++)
    {
        Chassis.speed.set[i]=Chassis.power.speed_decreasement_k*Chassis.speed.set[i]+ Chassis.speed.spd_yaw_actual;
        Chassis_MOTO[i].set_speed=Chassis.speed.set[i];
        Chassis.speed.now[i]=Chassis_MOTO[i].getpara.speed_rpm;
        /*速度环PID计算_&_电机输出信号赋值*/
        Chassis_MOTO[i].send_current = Chassis_MOTO[i].pid_speed.PidCalc(&Chassis_MOTO[i].pid_speed, Chassis_MOTO[i].getpara.speed_rpm, Chassis_MOTO[i].set_speed);
    }

}



void Chassis_SetModeFromControl(void)
{

    static Key_Data_t key_last;
    static uint8_t Spin_flag;
    static uint8_t Follow_flag;
    static uint8_t FollowDir;//0为正跟随 1为反跟随  2 3 为不跟随暂态 4为侧面左跟随 5为侧面右跟随
    static uint64_t FollowDir_counter;//反面跟随计时恢复
    static uint16_t Follow_counter;
    static uint16_t F_long_press_counter;
    static uint16_t turn_delay_counter;
    static uint8_t turn_pendingflag;
    static uint16_t supercap_disable_counter;
    static uint16_t supercap_enable_counter;
    static uint32_t unlimited_power_counter;

    if(RoboData.robo_ctrlmode.ctrl_source==FROM_REMOTE)
    {
        if(RoboData.robo_ctrlmode.left_mode==LEFT_down)
            Chassis.follow.mode.now=FRONT_FOLLOW;
        if(RoboData.robo_ctrlmode.left_mode==LEFT_mid)
            Chassis.follow.mode.now=FRONT_NOT_FOLLOW;

//		if(RoboData.robo_ctrlmode.left_mode==LEFT_down)
//			Chassis.follow.mode.now=SPIN;
//		if(RoboData.robo_ctrlmode.left_mode==LEFT_mid)
//		Chassis.follow.mode.now=FRONT_FOLLOW;
//
        Chassis.power.mode=NORMAL;



    }
    else if(RoboData.robo_ctrlmode.ctrl_source==FROM_PC)
    {
        if(RC_CtrlData.key.key_data.R==1&&key_last.R==0)
        {
            Spin_flag=1;
            if(RC_CtrlData.key.key_data.shift==0&&RC_CtrlData.key.key_data.ctrl==1)
                Chassis.speed.spin_fast=0;
            else
                Chassis.speed.spin_fast=1;
        }

        if(RC_CtrlData.key.key_data.Q==1&&key_last.Q==0)//一键回头被按下
        {
            if(Spin_flag==0&&Follow_flag==1)//如果不是小陀螺并且处于跟随
            {
                turn_pendingflag=1;//处理回头标志位置位
                turn_delay_counter=0;//回头及时清零
                if(FollowDir==0||FollowDir==3)//如果是正面跟随
                {
                    FollowDir=2;//2 3 进入两种0.5秒的暂态 分别为正、反跟随暂态
                    Chassis.follow.mode.now=BACK_NOT_FOLLOW;
                }
                else if(FollowDir==1||FollowDir==2)//如果是反面跟随
                {
                    FollowDir=3;//2 3 进入两种0.8秒的暂态 分别为正、反跟随暂态
                    Chassis.follow.mode.now=FRONT_NOT_FOLLOW;
                }
            }
        }
        if(turn_pendingflag==1)//如果处于回头挂起状态
        {
            turn_delay_counter++;//回头挂起计时
            if(turn_delay_counter>50)//如果计时超过了50，则暂态消除
            {
                if(FollowDir==2)//如果是反面不跟随暂态
                {
                    Chassis.follow.mode.now=BACK_FOLLOW;
                    FollowDir=1;//进入反面跟随
                }
                else if(FollowDir==3)//同上，回到正面跟随
                {
                    Chassis.follow.mode.now=FRONT_FOLLOW;
                    FollowDir=0;
                }
                turn_pendingflag=0;//回头标志位清零
                turn_delay_counter=0;//回头计时清零
            }
        }

        if(Follow_counter<0xffff)//两个计时越界处理
            Follow_counter++;

        if(F_long_press_counter<0xffff)
            F_long_press_counter++;

        if(RC_CtrlData.key.key_data.F==1&&key_last.F==0)//按下F
        {
            //if(FollowDir)//去除暂态为正向跟随
            //FollowDir=0;

            if(Spin_flag==1)//如果小陀螺
            {
                FollowDir=0;//正跟随
                Follow_flag=1;//开跟随
            }
            else
            {
                Follow_flag=0;//按下F期间不跟随
            }
            Spin_flag=0;//按下F就关小陀螺
            F_long_press_counter=0;//计时清零
        }
        else if(RC_CtrlData.key.key_data.F==0&&key_last.F==1)//松开F
        {
            if(F_long_press_counter>20)//如果按下的时间超过20，则不立刻跟随（长按F也是云台探视野功能。如果立刻恢复跟随，则云台还没回来，底盘就跟随过去）
                Follow_counter=0;//恢复跟随计时开始
            else
            {
                Follow_flag=1;//如果按下时间较短，则打开跟随
                FollowDir=0;//正向跟随
            }
        }

        if(RC_CtrlData.key.key_data.B==1&&key_last.B==0)
        {
            if(RC_CtrlData.key.key_data.shift==1)
                unlimited_power_counter=1;
        }
        if(unlimited_power_counter>0)
        {
            Chassis.power.unlimited=1;
            if(unlimited_power_counter<500)
                unlimited_power_counter++;
            else
                unlimited_power_counter=0;
        }
        else
            Chassis.power.unlimited=0;

        if(Follow_counter>60&&RC_CtrlData.key.key_data.F==0)//恢复跟随计数达到60，且F键未被按下，则恢复跟随。此时云台应该已经回来了。
            Follow_flag=1;

        if((RC_CtrlData.key.key_data.W==1||
                RC_CtrlData.key.key_data.S==1||
                RC_CtrlData.key.key_data.A==1||
                RC_CtrlData.key.key_data.D==1))
        {
            if(RC_CtrlData.key.key_data.ctrl==0&&
                    RC_CtrlData.key.key_data.shift==0)
                Chassis.power.mode=NORMAL;
            else if(RC_CtrlData.key.key_data.ctrl==0&&
                    RC_CtrlData.key.key_data.shift==1)
                Chassis.power.mode=FAST;
            else if(RC_CtrlData.key.key_data.ctrl==1&&
                    RC_CtrlData.key.key_data.shift==0)
                Chassis.power.mode=SLOW;
        }

        if(RC_CtrlData.key.key_data.X==1&&key_last.X==0)//超级电容取消功能
        {
            if(RC_CtrlData.key.key_data.ctrl==1&&
                    RC_CtrlData.key.key_data.shift==1)
                supercap_disable_counter++;
            else
                supercap_disable_counter=0;
        }
        if(supercap_disable_counter>0)
        {
            if(RC_CtrlData.key.key_data.ctrl==0||
                    RC_CtrlData.key.key_data.shift==0)
                supercap_disable_counter=0;
        }


        if(RC_CtrlData.key.key_data.Z==1&&key_last.Z==0)
        {
            if(RC_CtrlData.key.key_data.ctrl==1&&
                    RC_CtrlData.key.key_data.shift==1)
                supercap_enable_counter++;
            else
                supercap_enable_counter=0;
        }
        if(supercap_enable_counter>0)
        {
            if(RC_CtrlData.key.key_data.ctrl==0||
                    RC_CtrlData.key.key_data.shift==0)
                supercap_enable_counter=0;
        }



        if(Spin_flag==1)
        {
            Chassis.follow.mode.now=SPIN; //优先小陀螺
        }
        else if(Follow_flag==1) //若开跟随
        {
            if(FollowDir==0)
                Chassis.follow.mode.now=FRONT_FOLLOW;//前向跟随
            else if(FollowDir==1)//后向跟随
            {
                FollowDir_counter++;
                Chassis.follow.mode.now=BACK_FOLLOW;
                if(FollowDir_counter>150000)//应该为150，此处取消
                {
                    FollowDir_counter=0;
                    FollowDir=0;
                }
            }
            else if(FollowDir==4)//两种不用的侧面跟随
                Chassis.follow.mode.now=SIDE_FOLLOW_LEFT;
            else if(FollowDir==5)
                Chassis.follow.mode.now=SIDE_FOLLOW_RIGHT;
        }
        else //关跟随
        {
            if(FollowDir==0)
                Chassis.follow.mode.now=FRONT_NOT_FOLLOW;
            else
                Chassis.follow.mode.now=BACK_NOT_FOLLOW;
        }

        if(supercap_disable_counter>5)
        {
            Chassis.supercapacitor.manual_disabled=1;
        }

        if(supercap_enable_counter>5)
        {
            Chassis.supercapacitor.manual_disabled=0;
        }
    }
    key_last=RC_CtrlData.key.key_data;
}


void Chassis_PowerControl(void)
{
    static float RealPower_Total;
    static uint32_t still_counter;
    Chassis.supercapacitor.Voltage_filtered=0.9f*Chassis.supercapacitor.Voltage_filtered+0.1f*Chassis.supercapacitor.Voltage_realtime;
    Chassis.supercapacitor.power_percentage=(Chassis.supercapacitor.Voltage_filtered*Chassis.supercapacitor.Voltage_filtered
                                            -Chassis.supercapacitor.voltage_min*Chassis.supercapacitor.voltage_min)
                                            /(Chassis.supercapacitor.voltage_max*Chassis.supercapacitor.voltage_max
                                                    -Chassis.supercapacitor.voltage_min*Chassis.supercapacitor.voltage_min);

    if(Chassis.supercapacitor.power_percentage<0)
        Chassis.supercapacitor.power_percentage=0;
    else if(Chassis.supercapacitor.power_percentage>1)
        Chassis.supercapacitor.power_percentage=1;

    if(Chassis.supercapacitor.power_percentage>0.3)
        Chassis.supercapacitor.utility_percentage=1;
    else
        Chassis.supercapacitor.utility_percentage=(Chassis.supercapacitor.power_percentage)/0.3f;

    if(Chassis.power.unlimited==1)
    {
        Chassis.supercapacitor.utility_percentage=1;
    }

    if(Chassis.power.mode==SLOW)
    {
        Chassis.speed.base=1000;
        Chassis.power.power_set=70;
        Chassis.follow.pid.max_output=3500;

        if(fabs(Chassis.speed.spd_forward)+fabs(Chassis.speed.spd_right)!=0&&still_counter<0xffff)
            still_counter++;
        else
            still_counter=0;
        if(still_counter<40)
            Chassis.speed.spd_spinspeed_actual=3000.0f*(Chassis.supercapacitor.utility_percentage+1.0f);
        else
            Chassis.speed.spd_spinspeed_actual=2000.0f*(Chassis.supercapacitor.utility_percentage+1.0f);



    }
    else if(Chassis.supercapacitor.sup_c_open==1)
    {
        if(fabs(Chassis.speed.spd_forward)+fabs(Chassis.speed.spd_right)!=0&&still_counter<0xffff)
            still_counter++;
        else
            still_counter=0;


#ifdef SINGLE
        if(still_counter<1)
        {
            if(Chassis.speed.spin_fast==1)
                Chassis.speed.spd_spinspeed_actual=3500.0f*(Chassis.supercapacitor.utility_percentage+1.0f);
            else
                Chassis.speed.spd_spinspeed_actual=2500.0f*(Chassis.supercapacitor.utility_percentage+1.0f);
        }
        else
        {
            if(Chassis.speed.spin_fast==1)
                Chassis.speed.spd_spinspeed_actual=2500.0f*(Chassis.supercapacitor.utility_percentage+1.0f);
            else
                Chassis.speed.spd_spinspeed_actual=1500.0f*(Chassis.supercapacitor.utility_percentage+1.0f);
        }
        if(Chassis.power.mode==FAST)
        {
            Chassis.speed.base = 9000*(Chassis.supercapacitor.utility_percentage+0.5);
            Chassis.follow.pid.max_output=3000*(Chassis.supercapacitor.utility_percentage+0.5);
            Chassis.power.power_set=(6.3f*Chassis.supercapacitor.utility_percentage+2)*30;
        }
        else //if(Chassis.power.mode==NORMAL)
        {
            Chassis.speed.base = 4000*(Chassis.supercapacitor.utility_percentage+1);
            Chassis.follow.pid.max_output=3500*(Chassis.supercapacitor.utility_percentage+0.5);

            Chassis.power.power_set=(2.0f*Chassis.supercapacitor.utility_percentage+2+fabs(Chassis.speed.spd_yaw_actual)*0.0001f)*30;
            Chassis.power.pid.output.iout-=fabs(Chassis.speed.spd_yaw_actual)*0.0004f;
        }
#else

        if(still_counter<1)
        {
            if(Chassis.speed.spin_fast==1)
                Chassis.speed.spd_spinspeed_actual=3500.0f*(Chassis.supercapacitor.utility_percentage+1.0f);
            else
                Chassis.speed.spd_spinspeed_actual=1800.0f*(Chassis.supercapacitor.utility_percentage+1.0f);
        }
        else
        {
            if(Chassis.speed.spin_fast==1)
                Chassis.speed.spd_spinspeed_actual=1800.0f*(Chassis.supercapacitor.utility_percentage+1.0f);
            else
                Chassis.speed.spd_spinspeed_actual=1000.0f*(Chassis.supercapacitor.utility_percentage+1.0f);
        }
        if(Chassis.power.mode==FAST)
        {
            Chassis.speed.base = 9000*(Chassis.supercapacitor.utility_percentage+0.5);
            Chassis.follow.pid.max_output=3000*(Chassis.supercapacitor.utility_percentage+0.5);
            Chassis.power.power_set=(6.3f*Chassis.supercapacitor.utility_percentage+2)*30;
        }
        else //if(Chassis.power.mode==NORMAL)
        {
            Chassis.speed.base = 4000*(Chassis.supercapacitor.utility_percentage+1);
            Chassis.follow.pid.max_output=3500*(Chassis.supercapacitor.utility_percentage+0.5);

            Chassis.power.power_set=(1.5f*Chassis.supercapacitor.utility_percentage+2+fabs(Chassis.speed.spd_yaw_actual)*0.0001f)*23;
            Chassis.power.pid.output.iout-=fabs(Chassis.speed.spd_yaw_actual)*0.0004f;
        }




#endif
    }
    else
    {
        Chassis.speed.base = 4000;
        Chassis.power.power_set=70;
        Chassis.speed.spd_spinspeed_actual=2500;
        Chassis.follow.pid.max_output=3500;
    }

    RealPower_Total = RealPower_Total*0.9f+0.1f*(Chassis_MOTO[0].getpara.get_power+ Chassis_MOTO[1].getpara.get_power+ Chassis_MOTO[2].getpara.get_power+ Chassis_MOTO[3].getpara.get_power + 3);
    Chassis.power.power_now=RealPower_Total*0.75f;

    if(Chassis.power.unlimited==1)
        Chassis.power.power_set=360;

    Chassis.power.buff=DJI_ReadData.ext_power_heat_data.chassis_power_buffer;
    Chassis.power.speed_decreasement=PidCalc(&Chassis.power.pid,Chassis.power.power_now,Chassis.power.power_set);
    if(Chassis.power.speed_decreasement>0)
        Chassis.power.speed_decreasement=0;

#ifdef INFANTRY3
    Chassis.speed.spd_spinspeed_actual=-Chassis.speed.spd_spinspeed_actual;
#endif
}
/*底盘模式*/
void Chassis_Fun(void)
{
    static uint16_t init_delay_counter;
    if(init_delay_counter<70)
    {
        init_delay_counter++;
        return;
    }

    Chassis_SetModeFromControl();
    Chassis_PowerControl();

    Chassis.follow.angle=-((Yaw_MOTO6020.getpara.angle)/8192.0f*360.0f)+CHASSIS_FILLOW_OFFSET;

    if((TX2_Data.mode_set==WINDMILL_ANTICLKWISE&&TX2_Data.mode_now==WINDMILL_ANTICLKWISE)||TX2_Data.mode_set==WINDMILL_MANUAL)
    {
        Chassis.follow.mode.now=FRONT_NOT_FOLLOW;
    }

    if(Chassis.power.mode==FAST)
        Chassis.follow.deadband=1;
    else
        Chassis.follow.deadband=5;

    if(Chassis.follow.mode.now==FRONT_NOT_FOLLOW)
    {
        Chassis.speed.spd_yaw_actual=0;
        ChassisSetSpeed(0);
    }
    else if(Chassis.follow.mode.now==BACK_NOT_FOLLOW)
    {
        Chassis.speed.spd_yaw_actual=0;
        ChassisSetSpeed(180);
    }
    else if(Chassis.follow.mode.now==FRONT_FOLLOW)//前部跟随模式
    {
        Chassis.speed.spd_yaw_actual=ChassissFollow(-Chassis.follow.angle,0,Chassis.follow.deadband);
        ChassisSetSpeed(Chassis.follow.angle);
    }
    else if(Chassis.follow.mode.now==BACK_FOLLOW) //后部跟随模式
    {
        Chassis.speed.spd_yaw_actual=ChassissFollow(-Chassis.follow.angle,180,Chassis.follow.deadband);
        ChassisSetSpeed(Chassis.follow.angle);
    }

    else if(Chassis.follow.mode.now==SPIN)//小陀螺模式
    {
        Chassis.speed.spd_yaw_actual=Chassis.speed.spd_spinspeed_actual;
        ChassisSetSpeed(Chassis.follow.angle);
    }
    else if(Chassis.follow.mode.now==SIDE_FOLLOW_LEFT)//侧面跟随模式
    {
        Chassis.speed.spd_yaw_actual=ChassissFollow(-Chassis.follow.angle,90,Chassis.follow.deadband);
        ChassisSetSpeed(Chassis.follow.angle);
    }
    else if(Chassis.follow.mode.now==SIDE_FOLLOW_RIGHT)//侧面跟随模式
    {
        Chassis.speed.spd_yaw_actual=ChassissFollow(-Chassis.follow.angle,-90,Chassis.follow.deadband);
        ChassisSetSpeed(Chassis.follow.angle);
    }

    Chassis.follow.mode.last=Chassis.follow.mode.now;
}

void Chassis_Init(void)
{
    Chassis.speed.base = 8000;

    Chassis.follow.deadband=5;
    Chassis.follow.mode.now=FRONT_FOLLOW;
    Chassis.follow.mode.last=FRONT_FOLLOW;

    Chassis.power.deadband=5;
    Chassis.power.power_set=75;
    Chassis.power.mode=NORMAL;

    Chassis.power.pid.pid_mode=POSITION_PID;
    Chassis.power.pid.max_output=60000;
    Chassis.power.pid.integral_uplimit=0.1;
    Chassis.power.pid.integral_downlimit=-40000;
    Chassis.power.pid.p=15;
    Chassis.power.pid.i=12;
    Chassis.power.pid.d=0;
    Chassis.power.pid.deadband=5;
    Chassis.power.pid.f=0;
    Chassis.power.pid.max_err=50000;
    Chassis.power.pid.mid_err=100;
    Chassis.power.pid.min_err=0;
    Chassis.power.pid.f_out_limit=0;
    Chassis.power.pid.f_divider=0;

    Chassis.follow.pid.pid_mode=POSITION_PID;
    Chassis.follow.pid.max_output=6000;
    Chassis.follow.pid.integral_uplimit=2000;
    Chassis.follow.pid.integral_downlimit=-2000;
    Chassis.follow.pid.p=130;
    Chassis.follow.pid.i=0;
    Chassis.follow.pid.d=0;
    Chassis.follow.pid.deadband=0;
    Chassis.follow.pid.f=0;
    Chassis.follow.pid.max_err=40;
    Chassis.follow.pid.mid_err=15;
    Chassis.follow.pid.min_err=0;
    Chassis.follow.pid.f_out_limit=0;
    Chassis.follow.pid.f_divider=0;

    Chassis.supercapacitor.voltage_max=53;
    Chassis.supercapacitor.voltage_min=20;
    Chassis.supercapacitor.manual_disabled=0;
}
