#include "dji_Protocol.h"
#include "tx2_Protocol.h"
#include "robodata.h"
#include "m_remote.h"
#include "t_moto.h"
#include "t_monitor.h"


RoboData_t RoboData= {&InitRoboData}; //机器人用户数据类
//extern DJI_Data_t DJI_ReadData;

/*function：设置控制模式*******************************
*parameter：Remote_t类，RoboControlMode_t类
*/
void SetRoboControlMode(Remote_t rc, RoboControlMode_t *rcmode)
{
    //左
    switch(rc.s1) {
    case 1:
        rcmode->left_mode = LEFT_up;
        break;
    case 2:
        rcmode->left_mode = LEFT_down;
        break;
    case 3:
        rcmode->left_mode = LEFT_mid;
        break;
    default:
        rcmode->left_mode = LEFT_mid;
        break;
    }
    //右
    switch(rc.s2) {
    case 1:
        rcmode->ctrl_source = FROM_PC;
        break;
    case 2:
        rcmode->ctrl_source = RIGHT_reserve;
        break;
    case 3:
        rcmode->ctrl_source = FROM_REMOTE;
        break;
    default:
        rcmode->ctrl_source = FROM_REMOTE;
        break;
    }

}

/*function：设置弹丸发射控制*******************************
*parameter：RC_Ctl_t类，RoboControlMode_t类，AmmunitionControl_t类
*/
void GenerallySetShootControl(RC_Ctl_t rc_data, RoboControlMode_t robo_ctrlmode, AmmunitionControl_t *AmmunitionControl)
{
    if(robo_ctrlmode.ctrl_source == FROM_PC)
    {
        if(rc_data.mouse.press_1 == 1) {
            AmmunitionControl->shoot17_order = Order_start1;
            AmmunitionControl->shoot42_order = Order_start1;
        }
        else {
            AmmunitionControl->shoot17_order = Order_stop;
            AmmunitionControl->shoot42_order = Order_stop;
        }
    }
    else //if(RoboData.robo_ctrlmode.ctrl_source == FROM_REMOTE)
    {
        if(robo_ctrlmode.left_mode == LEFT_down) {
            AmmunitionControl->shoot17_order = Order_start1;
            AmmunitionControl->shoot42_order = Order_start1;
        }
        else {
            AmmunitionControl->shoot17_order = Order_stop;
            AmmunitionControl->shoot42_order = Order_stop;
        }
    }


}

///*function：设置底盘运动控制*******************************
//*parameter：RC_Ctl_t类，RoboControlMode_t类，ChassisControl_t类
//*/
//void GenerallySetChassisSpeed(RC_Ctl_t rc_data, RoboControlMode_t robo_ctrlmode, ChassisControl_t *chassis_ctrl)
//{
//	if(robo_ctrlmode.ctrl_source == FROM_PC)
//	{
//		static float spd_w =0,spd_s=0,spd_a=0,spd_d=0,spd_q=0,spd_e=0;
//		//相位滞后，使曲线平滑
//		spd_w = rc_data.key.key_data.W*0.5f + 0.5f*spd_w;
//		spd_s = rc_data.key.key_data.S*0.5f + 0.5f*spd_s;
//		spd_a = rc_data.key.key_data.A*0.5f + 0.5f*spd_a;
//		spd_d = rc_data.key.key_data.D*0.5f + 0.5f*spd_d;
//		spd_q = rc_data.key.key_data.Q*0.5f + 0.5f*spd_q;
//		spd_e = rc_data.key.key_data.E*0.5f + 0.5f*spd_e;
//
//		chassis_ctrl->spd_forward = (spd_w - spd_s);
//		chassis_ctrl->spd_right = (spd_d - spd_a);
////		chassis_ctrl->spd_yaw_w = (spd_q - spd_e);

//	}
//	else //if(RoboData.robo_ctrlmode.ctrl_source == FROM_REMOTE)
//	{
//		if(rc_data.rc.ch3 != 0) { chassis_ctrl->spd_forward = (rc_data.rc.ch3-1024.0)/660.0; }
//		else { chassis_ctrl->spd_forward = 0; }

//		if(rc_data.rc.ch2 != 0) { chassis_ctrl->spd_right = (rc_data.rc.ch2-1024.0)/660.0; }
//		else { chassis_ctrl->spd_right = 0; }
//
////		if(rc_data.rc.ch0 != 0) { chassis_ctrl->angle_yaw_w = (rc_data.rc.ch0-1024.0)/660.0*180; }
////		else { chassis_ctrl->angle_yaw_w = 0; }
//
////		if(rc_data.rc.ch1 != 0) { chassis_ctrl->angle_yaw_w = (rc_data.rc.ch1-1024.0)/660.0*180; }
////		else { chassis_ctrl->angle_yaw_w = 0; }
//
////		if(rc_data.rc.ch0 != 0) { chassis_ctrl->spd_yaw_w = (rc_data.rc.ch0-1024.0)/660.0;}
////		else { chassis_ctrl->spd_yaw_w = 0; }
//	}
//
//
//}

/*function：底盘状态处理函数*******************************
*parameter：RoboData_t类
*/
void GetChassisControlStatus(RoboData_t *RoboData)
{
    if(monitor_can_power.status == monitor_regular)//超级电容功率版反馈正常
    {
        if(RoboData->robo_ctrlmode.ctrl_source == FROM_PC)
        {
            /*按键控制超级电容开启*/
            if(RC_CtrlData.key.key_data.V == 1)
                RoboData->chassis_ctrl.chassis_order = Order_startN;//准备长时间开启超级电容
            else
                RoboData->chassis_ctrl.chassis_order=Order_stop;
        }
        else if(RoboData->robo_ctrlmode.ctrl_source == RIGHT_reserve)
        {
            /*拨杆控制超级电容开关*/
            if(RoboData->robo_ctrlmode.left_mode == LEFT_up )
                RoboData->chassis_ctrl.chassis_order = Order_startN;//准备长时间开启超级电容
            else
                RoboData->chassis_ctrl.chassis_order=Order_stop;
        }
        else //if(RoboData->robo_ctrlmode.ctrl_source == FROM_REMOTE)
        {
            /*纯遥控器模式不启动超级电容*/
            RoboData->chassis_ctrl.chassis_order=Order_stop;
        }
    }
    else
    {   //超级电容功率版反馈不正常
        RoboData->chassis_ctrl.chassis_order=Order_stop;
        RoboData->chassis_ctrl.chassis_working_status = WorkingStatus_error;
    }
}


///*function：设置云台运动控制*******************************
//*parameter：RC_Ctl_t类，RoboControlMode_t类，TripodControl_t类
//*/
//void GenerallySetTripodControl(RC_Ctl_t rc_data, RoboControlMode_t robo_ctrlmode, TripodControl_t *tripod_ctrl)	//单位为度
//{
//	if(robo_ctrlmode.ctrl_source == FROM_REMOTE)
//	{
//		if(rc_data.rc.ch1 != 0) { tripod_ctrl->pitch_angle = -(rc_data.rc.ch1-1024.0)/660.0f*25; }
//		else { tripod_ctrl->pitch_angle = 0; }

//		if(rc_data.rc.ch0 != 0) { tripod_ctrl->yaw_angle = (rc_data.rc.ch0-1024.0)/660.0f*60; }
//		else { tripod_ctrl->yaw_angle = 0; }
//
//		tripod_ctrl->pitch_angle_mix = tripod_ctrl->pitch_angle + pitch6623_MiddleAngle;
//	  tripod_ctrl->yaw_angle_mix = tripod_ctrl->yaw_angle + yaw_middle_angle;
//	}
//	else if(robo_ctrlmode.ctrl_source == FROM_PC)
//	{
//		static float auto_pitch=0,auto_yaw=0;
//		tripod_ctrl->pitch_speed = rc_data.mouse.y_speed *0.000031;
//		tripod_ctrl->yaw_speed = rc_data.mouse.x_speed *0.000031;
//		//是否开启自瞄
//		if(rc_data.mouse.press_2 == 1) {	tripod_ctrl->tripod_order = Order_startN; }
//		else { tripod_ctrl->tripod_order = Order_stop; }
//		//自瞄
//		if((tripod_ctrl->tripod_order == Order_startN)&&(TX2_Data.sta == 1))
//		{
//			//判断pitch自动瞄准数据更新情况
//			if(ABS(auto_pitch - TX2_Data.delta_pitch)> 0.09F){
//				tripod_ctrl->pitch_angle_mix = Pitch_MOTO6623.getpara.total_round*360 - TX2_Data.delta_pitch;
//				tripod_ctrl->pitch_angle = tripod_ctrl->pitch_angle_mix - pitch6623_MiddleAngle;
//			}
//			else{
//				tripod_ctrl->pitch_angle += K_mouse_pitch_speed_PC * tripod_ctrl->pitch_speed*0.5f;//引入鼠标修正
//				tripod_ctrl->pitch_angle_mix = tripod_ctrl->pitch_angle + pitch6623_MiddleAngle;
//			}
//			//判断yaw自动瞄准数据更新情况
//			if(ABS(auto_yaw - TX2_Data.delta_yaw)>0.09F){
//				tripod_ctrl->yaw_angle_mix = Yaw_MOTO6020.getpara.total_round*360 - TX2_Data.delta_yaw;
//				tripod_ctrl->yaw_angle = tripod_ctrl->yaw_angle_mix - yaw_middle_angle;
//			}
//			else{
//				tripod_ctrl->yaw_angle -= K_mouse_yaw_speed_PC * tripod_ctrl->yaw_speed*0.5f;//引入鼠标修正
//				tripod_ctrl->yaw_angle_mix = tripod_ctrl->yaw_angle + yaw_middle_angle;
//			}
//		}
//		//非自瞄
//		else
//		{
//			tripod_ctrl->pitch_angle += K_mouse_pitch_speed_PC * tripod_ctrl->pitch_speed;
//			tripod_ctrl->yaw_angle -= K_mouse_yaw_speed_PC * tripod_ctrl->yaw_speed;
//			tripod_ctrl->pitch_angle_mix = tripod_ctrl->pitch_angle + pitch6623_MiddleAngle;
//			tripod_ctrl->yaw_angle_mix = tripod_ctrl->yaw_angle + yaw_middle_angle;
//		}
//		auto_pitch = TX2_Data.delta_pitch;
//		auto_yaw = TX2_Data.delta_yaw;
//	}

//
//}



/*function：遥控器数据处理函数*******************************
*parameter：RoboData_t类
*/
void GetRemoteControlData(RoboData_t *RoboData)
{
    RCReadKey(&RC_CtrlData);//获取按键数据
//	SetRoboControlMode(RC_CtrlData.rc, &RoboData->robo_ctrlmode);//设置控制模式
    //GenerallySetChassisSpeed(RC_CtrlData, RoboData->robo_ctrlmode, &RoboData->chassis_ctrl);//设置底盘运动控制
    //GenerallySetTripodControl(RC_CtrlData, RoboData->robo_ctrlmode, &RoboData->tripod_ctrl);//设置云台运动控制
    //GenerallySetShootControl(RC_CtrlData, RoboData->robo_ctrlmode, &RoboData->AmmunitionControl);//设置弹丸发射控制
}




/*function：裁判系统数据处理函数*******************************
*parameter：RoboData_t类
*/
void GetDJIControlData(RoboData_t *RoboData)//时间片频率100hz，裁判系统更新10hz
{
    switch(DJI_ReadData.ext_game_robot_state.robot_level)
    {
    case 1:
    {   RoboData->shoot_heat_ctr.limitHeat17=240;
        RoboData->shoot_heat_ctr.coolingHeat17=0.4;
        RoboData->shoot_heat_ctr.limitHeat42=150;
        RoboData->shoot_heat_ctr.coolingHeat42=0.2;
    }
    break;
    case 2:
    {   RoboData->shoot_heat_ctr.limitHeat17=360;
        RoboData->shoot_heat_ctr.coolingHeat17=0.6;
        RoboData->shoot_heat_ctr.limitHeat42=250;
        RoboData->shoot_heat_ctr.coolingHeat42=0.4;
    }
    break;
    case 3:
    {   RoboData->shoot_heat_ctr.limitHeat17=480;
        RoboData->shoot_heat_ctr.coolingHeat17=0.8;
        RoboData->shoot_heat_ctr.limitHeat42=400;
        RoboData->shoot_heat_ctr.coolingHeat42=0.6;
    }
    break;
    default://本地测试
    {   RoboData->shoot_heat_ctr.limitHeat17=240;
        RoboData->shoot_heat_ctr.coolingHeat17=0.4;
        RoboData->shoot_heat_ctr.limitHeat42=80;
        RoboData->shoot_heat_ctr.coolingHeat42=0.2;
    }
    break;
    }

//  if(RoboData->shoot_heat_ctr.allow17num < 4)//热量不足时单发
//	{
//		RoboData->shoot_heat_ctr.addHeat17=RoboData->AmmunitionControl.Friction17_speed*1;
////		RoboData->AmmunitionControl.Toggler17_angle=Toggler17_angle_one;
//	}

//	RoboData->shoot_heat_ctr.addHeat42 = 100;
//	RoboData->AmmunitionControl.Toggler42_angle = Toggler42_angle_one;
    if(DJI_ReadData.ext_game_robot_state.remain_HP*5 < DJI_ReadData.ext_game_robot_state.max_HP)
        RoboData->shoot_heat_ctr.coolingHeat17 *= 2;

    /*场地buff由本地热量更新解决*/
}


/*function：本地17mm枪口热量处理函数*******************************
*parameter：RoboData_t类
*/
void Get17HeatControlData(RoboData_t *RoboData)//100hz
{
    RoboData->shoot_heat_ctr.coolingWaitTime17++;
    if(RoboData->shoot_heat_ctr.coolingWaitTime17 > 9)//ms;10hz heatCooling
    {
        RoboData->shoot_heat_ctr.coolingWaitTime17 = 0;
        if(RoboData->shoot_heat_ctr.nowHeat17>0)
            RoboData->shoot_heat_ctr.nowHeat17 -= RoboData->shoot_heat_ctr.coolingHeat17*10;
        else
            RoboData->shoot_heat_ctr.nowHeat17 = 0;
    }

    if(RoboData->shoot_heat_ctr.nowHeat17 > RoboData->shoot_heat_ctr.limitHeat17)
        RoboData->shoot_heat_ctr.allow17num = 0;
    else
        RoboData->shoot_heat_ctr.allow17num = (RoboData->shoot_heat_ctr.limitHeat17 - RoboData->shoot_heat_ctr.nowHeat17)/RoboData->AmmunitionControl.Friction17_speed;
}


/*function：本地42mm枪口热量处理函数*******************************
*parameter：RoboData_t类
*/
void Get42HeatControlData(RoboData_t *RoboData)//100hz
{
    RoboData->shoot_heat_ctr.coolingWaitTime42++;
    if(RoboData->shoot_heat_ctr.coolingWaitTime42 > 9)//ms;100hz heatCooling
    {
        RoboData->shoot_heat_ctr.coolingWaitTime42 = 0;
        if(RoboData->shoot_heat_ctr.nowHeat42>0)
            RoboData->shoot_heat_ctr.nowHeat42 -= RoboData->shoot_heat_ctr.coolingHeat42*10;
        else
            RoboData->shoot_heat_ctr.nowHeat42 = 0;
    }

    if(RoboData->shoot_heat_ctr.nowHeat42 > RoboData->shoot_heat_ctr.limitHeat42)
        RoboData->shoot_heat_ctr.allow42num = 0;
    else
        RoboData->shoot_heat_ctr.allow42num = (RoboData->shoot_heat_ctr.limitHeat42 - RoboData->shoot_heat_ctr.nowHeat42)/RoboData->shoot_heat_ctr.addHeat42;
}


/*function：17mm发弹状态处理函数*******************************
*parameter：RoboData_t类，Moto_t类
*/
void GetAmmunition17ControlStatus(RoboData_t *RoboData, Moto_t *TG_MOTO)//1000hz
{
    /*---------发弹忙碌状态---------*/
    if(RoboData->AmmunitionControl.shoot17_working_status == WorkingStatus_busy)
    {
        RoboData->AmmunitionControl.shoot17_busy_time ++;
        RoboData->AmmunitionControl.shoot17_error_time = 0;
        if(RoboData->AmmunitionControl.shoot17_busy_time < 500)//ms
        {
            if((ABS(TG_MOTO->pid_angle.err.now) < 10)&&(ABS(TG_MOTO->pid_speed.err.now) < 10))
            {
                //RoboData->shoot_heat_ctr.nowHeat17 += RoboData->shoot_heat_ctr.addHeat17;//17mm热量增加
                //RoboData->AmmunitionControl.shoot17_order = Order_stop;
                RoboData->AmmunitionControl.shoot17_working_status = WorkingStatus_ready;
            }
            else
                RoboData->AmmunitionControl.shoot17_working_status = WorkingStatus_busy;
        }
        else
        {
            RoboData->AmmunitionControl.shoot17_busy_time = 0;
            RoboData->AmmunitionControl.shoot17_working_status = WorkingStatus_error;
        }
    }
    /*---------发弹等待状态---------*/
    else if(RoboData->AmmunitionControl.shoot17_working_status == WorkingStatus_ready)
    {
        RoboData->AmmunitionControl.shoot17_busy_time = 0;
        RoboData->AmmunitionControl.shoot17_error_time = 0;
        if((RoboData->AmmunitionControl.shoot17_order == Order_start1)
                &&((RoboData->shoot_heat_ctr.nowHeat17 + RoboData->shoot_heat_ctr.addHeat17) < RoboData->shoot_heat_ctr.limitHeat17))
        {
            RoboData->shoot_heat_ctr.nowHeat17 += RoboData->shoot_heat_ctr.addHeat17;//17mm热量预增加
            TG_MOTO->getpara.round_cnt -= (RoboData->AmmunitionControl.Toggler17_angle/360.0f); //17mm拨弹角度设置
            TG_MOTO->getpara.total_round = TG_MOTO->getpara.round_cnt + (float)(TG_MOTO->getpara.angle)/8192.0f;
            //RoboData->AmmunitionControl.shoot17_order = Order_busy;
            RoboData->AmmunitionControl.shoot17_working_status = WorkingStatus_busy;
        }
    }
    /*---------发弹错误状态---------*/
    else if(RoboData->AmmunitionControl.shoot17_working_status == WorkingStatus_error)
    {
        RoboData->AmmunitionControl.shoot17_busy_time = 0;
        RoboData->AmmunitionControl.shoot17_error_time ++;
        if(RoboData->AmmunitionControl.shoot17_error_time < 50)//ms
        {
            //错误处理待定
        }
        else
        {
            RoboData->AmmunitionControl.shoot17_error_time = 0;
            RoboData->AmmunitionControl.shoot17_working_status = WorkingStatus_ready;
        }
    }

}


/*function：42mm发弹状态处理函数*******************************
*parameter：RoboData_t类，Moto_t类
*/
void GetAmmunition42ControlStatus(RoboData_t *RoboData, Moto_t *TG_MOTO)//1000hz
{
    /*---------发弹忙碌状态---------*/
    if(RoboData->AmmunitionControl.shoot42_working_status == WorkingStatus_busy)
    {
        RoboData->AmmunitionControl.shoot42_busy_time ++;
        RoboData->AmmunitionControl.shoot42_error_time = 0;
        if(RoboData->AmmunitionControl.shoot42_busy_time < 500)//ms
        {
            if((ABS(TG_MOTO->pid_angle.err.now) < 10)&&(ABS(TG_MOTO->pid_speed.err.now) < 10))
            {
                //RoboData->shoot_heat_ctr.nowHeat42 += RoboData->shoot_heat_ctr.addHeat42;//42mm热量增加
                //RoboData->AmmunitionControl.shoot42_order = Order_stop;
                RoboData->AmmunitionControl.shoot42_working_status = WorkingStatus_ready;
            }
            else
                RoboData->AmmunitionControl.shoot42_working_status = WorkingStatus_busy;
        }
        else
        {
            RoboData->AmmunitionControl.shoot42_busy_time = 0;
            RoboData->AmmunitionControl.shoot42_working_status = WorkingStatus_error;
        }
    }
    /*---------发弹等待状态---------*/
    else if(RoboData->AmmunitionControl.shoot42_working_status == WorkingStatus_ready)
    {
        RoboData->AmmunitionControl.shoot42_busy_time = 0;
        RoboData->AmmunitionControl.shoot42_error_time = 0;
        if((RoboData->AmmunitionControl.shoot42_order == Order_start1)
                &&((RoboData->shoot_heat_ctr.nowHeat42 + RoboData->shoot_heat_ctr.addHeat42) < RoboData->shoot_heat_ctr.limitHeat42))
        {
            RoboData->shoot_heat_ctr.nowHeat42 += RoboData->shoot_heat_ctr.addHeat42;//42mm热量预增加
            TG_MOTO->getpara.round_cnt -= (RoboData->AmmunitionControl.Toggler42_angle/360.0f); //42mm拨弹角度设置
            TG_MOTO->getpara.total_round = TG_MOTO->getpara.round_cnt + (float)(TG_MOTO->getpara.angle)/8192.0f;
            //RoboData->AmmunitionControl.shoot42_order = Order_busy;
            RoboData->AmmunitionControl.shoot42_working_status = WorkingStatus_busy;
        }
    }
    /*---------发弹错误状态---------*/
    else if(RoboData->AmmunitionControl.shoot42_working_status == WorkingStatus_error)
    {
        RoboData->AmmunitionControl.shoot42_busy_time = 0;
        RoboData->AmmunitionControl.shoot42_error_time ++;
        if(RoboData->AmmunitionControl.shoot42_error_time < 250)//ms
        {
            //错误处理待定
        }
        else
        {
            RoboData->AmmunitionControl.shoot42_error_time = 0;
            RoboData->AmmunitionControl.shoot42_working_status = WorkingStatus_ready;
        }
    }

}

/*function：初始化机器人数据类RoboData内的数据*******************************
*parameter：RoboData_t类
*/
void InitRoboData(RoboData_t *RoboData)
{
    Chassis_MOTO[0].MotoParaInit = &MotoParaInit;
    Chassis_MOTO[0].MotoParaInit(&Chassis_MOTO[0]);

    Chassis_MOTO[1].MotoParaInit = &MotoParaInit;
    Chassis_MOTO[1].MotoParaInit(&Chassis_MOTO[1]);

    Chassis_MOTO[2].MotoParaInit = &MotoParaInit;
    Chassis_MOTO[2].MotoParaInit(&Chassis_MOTO[2]);

    Chassis_MOTO[3].MotoParaInit = &MotoParaInit;
    Chassis_MOTO[3].MotoParaInit(&Chassis_MOTO[3]);

    TG_MOTO[0].MotoParaInit = &MotoParaInit;
    TG_MOTO[0].MotoParaInit(&TG_MOTO[0]);
    TG_MOTO[1].MotoParaInit = &MotoParaInit;
    TG_MOTO[1].MotoParaInit(&TG_MOTO[1]);


    Fricition_MOTO[0].MotoParaInit = &MotoParaInit;
    Fricition_MOTO[0].MotoParaInit(&Fricition_MOTO[0]);
    Fricition_MOTO[1].MotoParaInit = &MotoParaInit;
    Fricition_MOTO[1].MotoParaInit(&Fricition_MOTO[1]);

    power_pid[0].CommonPidSet = &CommonPidSet;
    power_pid[1].CommonPidSet = &CommonPidSet;
    power_pid[0].CommonPidCalc = &CommonPidCalc;
    power_pid[1].CommonPidCalc = &CommonPidCalc;

    RC_CtrlData.RCDataParaInit = &RCDataParaInit;
    RC_CtrlData.RCDataParaInit(&RC_CtrlData);


    monitor_remote.MonitorParaInit = &MonitorParaInit;
    monitor_remote.MonitorParaInit(&monitor_remote);
    monitor_tx2.MonitorParaInit = &MonitorParaInit;
    monitor_tx2.MonitorParaInit(&monitor_tx2);
    monitor_can_power.MonitorParaInit = &MonitorParaInit;
    monitor_can_power.MonitorParaInit(&monitor_can_power);




    RoboData->Get17HeatControlData = &Get17HeatControlData;
    RoboData->Get42HeatControlData = &Get42HeatControlData;
    RoboData->GetAmmunition17ControlStatus = &GetAmmunition17ControlStatus;
    RoboData->GetAmmunition42ControlStatus = &GetAmmunition42ControlStatus;
    RoboData->GetDJIControlData = &GetDJIControlData;
    RoboData->GetRemoteControlData = &GetRemoteControlData;
    RoboData->GetChassisControlStatus = &GetChassisControlStatus;

    RoboData->robo_ctrlmode.left_mode = LEFT_mid;//左拨杆模式
    RoboData->robo_ctrlmode.ctrl_source = FROM_REMOTE;//控制来源模式默认为来自遥控器
    RoboData->AmmunitionControl.shoot17_order = Order_stop;//弹丸发射设置默认为不发射
    RoboData->AmmunitionControl.shoot42_order = Order_stop;//弹丸发射设置默认为不发射
    RoboData->AmmunitionControl.shoot17_working_status = WorkingStatus_ready;
    RoboData->AmmunitionControl.shoot42_working_status = WorkingStatus_ready;
    RoboData->tripod_ctrl.pitch_angle_mix = pitch6623_MiddleAngle;
    RoboData->tripod_ctrl.yaw_angle_mix = yaw_middle_angle;
    RoboData->chassis_ctrl.chassis_order = Order_stop;
    RoboData->chassis_ctrl.chassis_working_status = WorkingStatus_temp;//准备关闭超级电容


}

