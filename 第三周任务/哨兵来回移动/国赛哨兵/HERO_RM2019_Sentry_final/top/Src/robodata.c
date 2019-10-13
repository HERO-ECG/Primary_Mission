#include "dji_Protocol.h"
#include "tx2_Protocol.h"
#include "robodata.h"
#include "m_remote.h"
#include "t_moto.h"
#include "t_monitor.h"
#include "auto_move.h"
#include "strike.h"


RoboData_t RoboData={&InitRoboData};//机器人用户数据类


/*function：设置控制模式*******************************
*parameter：Remote_t类，RoboControlMode_t类
*/
void SetRoboControlMode(Remote_t rc, RoboControlMode_t *rcmode)
{
	//左
	switch(rc.s1){
		case 1:
			rcmode->left_mode = LEFT_1;break;
		case 2:
			rcmode->left_mode = LEFT_2;break;
		case 3:
			rcmode->left_mode = LEFT_3;break;
		default: 
			rcmode->left_mode = LEFT_1;break;
	}
	//右
	switch(rc.s2){
		case 1:
			rcmode->ctrl_source = AUTO_MOVE_MODE;break;
		case 2:
			rcmode->ctrl_source = AUTO_MOVE_MODE2;break;
		case 3:
			rcmode->ctrl_source = FROM_REMOTE;break;
		default: 
			rcmode->ctrl_source = FROM_REMOTE;break;
	}
	
}

/*function：设置弹丸发射控制*******************************
*parameter：RC_Ctl_t类，RoboControlMode_t类，AmmunitionControl_t类
*/
void GenerallySetShootControl(RC_Ctl_t rc_data, RoboControlMode_t robo_ctrlmode, AmmunitionControl_t *AmmunitionControl)
{
//	if(robo_ctrlmode.ctrl_source == FROM_REMOTE)
//	{
		set_cm_speed();//底盘速度设置,底盘正方向与与云台正方向差角为theta° 
		Strike.firemode=Auto_Mode;
		if(robo_ctrlmode.left_mode == LEFT_2) 
		{ 
			Strike.aimstate=OnTarget;
			Strike.work_state=real_work;
			if_heat_limit=1;
		}
		else 
		{	Strike.aimstate=OffTarget;
			if_heat_limit=1;
		}

			
//	}
//	else if(robo_ctrlmode.ctrl_source == AUTO_MOVE_MODE)
//	{
//			TG_MOTO[0].set_speed = 0;
//			if_heat_limit=1;
//	}
//	else if(robo_ctrlmode.ctrl_source == AUTO_MOVE_MODE2)
//	{
//			Strike.firemode=Auto_Mode;
//			if_heat_limit=1;
//	}
	
}

/*function：设置底盘运动控制*******************************
*parameter：RC_Ctl_t类，RoboControlMode_t类，ChassisControl_t类
*/
uint8_t chassissmode;
void GenerallySetChassisSpeed(RC_Ctl_t rc_data, RoboControlMode_t robo_ctrlmode, ChassisControl_t *chassis_ctrl)
{
	if(robo_ctrlmode.ctrl_source == FROM_REMOTE)
	{
		chassissmode=0;
		overall_mode.chassiss_control_mode = SPEED;
//				Chassis_MOTO[0].set_speed = (rc_data.rc.ch2-1024.0)/660.0*K_CHASSIS_REMOTE_RIGHT; 
		if(rc_data.rc.ch3 != 0) { chassis_ctrl->spd_forward = (rc_data.rc.ch3-1024.0)/660.0*K_CHASSIS_REMOTE_FORWARD; }
		else { chassis_ctrl->spd_forward = 0; }

		if(rc_data.rc.ch2 != 0) { chassis_ctrl->spd_right = (rc_data.rc.ch2-1024.0)/660.0*K_CHASSIS_REMOTE_RIGHT; }
		else { chassis_ctrl->spd_forward = 0; }	

	}
	else if(robo_ctrlmode.ctrl_source == AUTO_MOVE_MODE)
	{
		chassissmode=1;
					//overall_mode.ChassisMotoAutoMode(Chassis_MOTO);
	}
	else if(robo_ctrlmode.ctrl_source == AUTO_MOVE_MODE2)
	{
		chassissmode=2;
				//	overall_mode.ChassisMotoAutoMode(Chassis_MOTO);

	}
}

/*function：设置云台运动控制*******************************
*parameter：RC_Ctl_t类，RoboControlMode_t类，TripodControl_t类
*/

void GenerallySetTripodControl(RC_Ctl_t rc_data, RoboControlMode_t robo_ctrlmode, TripodControl_t *tripod_ctrl)	//单位为度
{	
//	if(robo_ctrlmode.ctrl_source == FROM_REMOTE)
//	{
		if(rc_data.rc.ch1 != 0) { tripod_ctrl->pitch_angle = -(rc_data.rc.ch1-1024.0)/660.0 *70; }
		else { tripod_ctrl->pitch_angle = 0; }	
				if(rc_data.rc.ch0 != 0) { tripod_ctrl->yaw_angle = (rc_data.rc.ch0-1024.0)/660.0	*150; }
		else { tripod_ctrl->yaw_angle = 0; }
		
		overall_mode.pitch.pit_auto_angle_set+=RoboData.tripod_ctrl.pitch_angle/30.0f;
		overall_mode.yaw.yaw_auto_angle_set-=RoboData.tripod_ctrl.yaw_angle/40.0f;
		if(overall_mode.pitch.pit_auto_angle_set < (-510))
			overall_mode.pitch.pit_auto_angle_set = (-510);
		else if(overall_mode.pitch.pit_auto_angle_set > 130)
			overall_mode.pitch.pit_auto_angle_set = 130;
			Pitch_SetAngle(overall_mode.pitch.pit_auto_angle_set);
		  YawMotoCtrl(&Yaw_MOTO,1000, overall_mode.yaw.yaw_auto_angle_set);
		
//	}
//	else if(robo_ctrlmode.ctrl_source == AUTO_MOVE_MODE)
//	{
//			overall_mode.pitch.PitchMotoAutoMode(&Pitch_MOTO);
//			overall_mode.yaw.YawMotoAutoMode(&Yaw_MOTO);

//	}
//	else if(robo_ctrlmode.ctrl_source == AUTO_MOVE_MODE2)
//	{
//			overall_mode.pitch.PitchMotoAutoMode(&Pitch_MOTO);
//			overall_mode.yaw.YawMotoAutoMode(&Yaw_MOTO);
//	}
	
}



/*function：遥控器数据处理函数*******************************
*parameter：RoboData_t类
*/
void GetRemoteControlData(RoboData_t *RoboData)
{
	SetRoboControlMode(RC_CtrlData.rc, &RoboData->robo_ctrlmode);//设置控制模式
	GenerallySetChassisSpeed(RC_CtrlData, RoboData->robo_ctrlmode, &RoboData->chassis_ctrl);//设置底盘运动控制
	GenerallySetTripodControl(RC_CtrlData, RoboData->robo_ctrlmode, &RoboData->tripod_ctrl);//设置云台运动控制
	GenerallySetShootControl(RC_CtrlData, RoboData->robo_ctrlmode, &RoboData->AmmunitionControl);//设置弹丸发射控制
}






/*function：初始化机器人数据类RoboData内的数据*******************************
*parameter：RoboData_t类
*/
void InitRoboData(RoboData_t *RoboData)
{
	Chassis_MOTO[0].MotoParaInit = &MotoParaInit;
	Chassis_MOTO[1].MotoParaInit = &MotoParaInit;
	Chassis_MOTO[2].MotoParaInit = &MotoParaInit;
	Chassis_MOTO[3].MotoParaInit = &MotoParaInit;
	Chassis_power.MotoParaInit = &MotoParaInit;
	TG_MOTO[0].MotoParaInit = &MotoParaInit;
	TG_MOTO[1].MotoParaInit = &MotoParaInit;
	Yaw_MOTO.MotoParaInit = &MotoParaInit;
	Pitch_MOTO.MotoParaInit = &MotoParaInit;
	Fricition_MOTO.MotoParaInit = &MotoParaInit;
	power_pid[0].PidParaInit = &PidParaInit;
	power_pid[1].PidParaInit = &PidParaInit;
	monitor_remote.MonitorParaInit = &MonitorParaInit;
	monitor_tx2.MonitorParaInit = &MonitorParaInit;
	RC_CtrlData.RCDataParaInit = &RCDataParaInit;
	tx2_ReadData.TX2DataParaInit = &TX2DataParaInit;
	
	Chassis_MOTO[0].MotoParaInit(&Chassis_MOTO[0]);
	Chassis_MOTO[1].MotoParaInit(&Chassis_MOTO[1]);
	Chassis_MOTO[2].MotoParaInit(&Chassis_MOTO[2]);
	Chassis_MOTO[3].MotoParaInit(&Chassis_MOTO[3]);
	Chassis_power.MotoParaInit(&Chassis_power);
	TG_MOTO[0].MotoParaInit(&TG_MOTO[0]);
	TG_MOTO[1].MotoParaInit(&TG_MOTO[1]);
	Yaw_MOTO.MotoParaInit(&Yaw_MOTO);
	Pitch_MOTO.MotoParaInit(&Pitch_MOTO);
	Fricition_MOTO.MotoParaInit(&Fricition_MOTO);
	power_pid[0].PidParaInit(&power_pid[0]);
	power_pid[1].PidParaInit(&power_pid[1]);
	monitor_remote.MonitorParaInit(&monitor_remote);
	monitor_tx2.MonitorParaInit(&monitor_tx2);
	RC_CtrlData.RCDataParaInit(&RC_CtrlData);
	tx2_ReadData.TX2DataParaInit(&tx2_ReadData);
	
	
	
	
	
//	RoboData->Get17HeatControlData = &Get17HeatControlData;
//	RoboData->Get42HeatControlData = &Get42HeatControlData;
//	RoboData->GetAmmunition17ControlStatus = &GetAmmunition17ControlStatus;
//	RoboData->GetAmmunition42ControlStatus = &GetAmmunition42ControlStatus;
//	RoboData->GetDJIControlData = &GetDJIControlData;
	RoboData->GetRemoteControlData = &GetRemoteControlData;
	
	
	
	RoboData->robo_ctrlmode.left_mode = LEFT_1;//左拨杆模式
	RoboData->robo_ctrlmode.ctrl_source = FROM_REMOTE;//控制来源模式默认为来自遥控器
	RoboData->AmmunitionControl.shoot17_order = Order_stop;//弹丸发射设置默认为不发射
	RoboData->AmmunitionControl.shoot42_order = Order_stop;//弹丸发射设置默认为不发射
	RoboData->AmmunitionControl.shoot17_working_status = WorkingStatus_ready;
	RoboData->AmmunitionControl.shoot42_working_status = WorkingStatus_ready;


}

