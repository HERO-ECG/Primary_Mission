#include "t_monitor.h"
#include "strike.h"
#include "m_remote.h"
#include "tx2_Protocol.h"
Monitor_t monitor_remote;
Monitor_t monitor_tx2;
Monitor_t monitor_can_power;
Monitor_t monitor_JY901;

//监测遥控通信是否正常
void monitor_remote_process(Monitor_t *Monitor)//100hz
{
	Monitor->circle_number++; //在串口接收回调函数中清零，若没清零则累计技术。累计三次说名通信故障
	if(Monitor->circle_number > 3)
	{
		Monitor->status=monitor_error;
		RC_CtrlData.RCDataParaInit(&RC_CtrlData);
	}
	else
		Monitor->status=monitor_regular;
}
//监测TX2和主控之间的通信是否正常
void monitor_tx2_process(Monitor_t *Monitor)//100hz
{
	Monitor->circle_number++; //在串口接收回调函数中清零，若没清零则累计技术。累计7次说名通信故障
	if(Monitor->circle_number > 7)
	{
		Monitor->status=monitor_error;
			RoboData.shoot_heat_ctr.addHeat17=friction17_spd_regular*1;//???
//			RoboData.AmmunitionControl.Toggler17_angle=Toggler17_angle_one;
//			RoboData.AmmunitionControl.Toggler42_angle=Toggler42_angle_one;
			RoboData.AmmunitionControl.Friction17_speed=friction17_spd_regular; //Friction摩擦轮
		//TX2_Data.TX2DataParaInit(&TX2_Data);
	}
	else
		Monitor->status=monitor_regular;
}
//监测裁判系统的，计数在哪里清零？？？？？？？
void monitor_can_power_process(Monitor_t *Monitor)//100hz
{
	Monitor->circle_number++;
	if(Monitor->circle_number > 5)
	{
		Monitor->status=monitor_error;
			RoboData.chassis_ctrl.sup_s_status=0;
			RoboData.chassis_ctrl.chassis_order=Order_stop;
			RoboData.chassis_ctrl.chassis_working_status=WorkingStatus_error;
	
	}
	else
		Monitor->status=monitor_regular;
}

//监测JY901通信
void monitor_JY901_process(Monitor_t *Monitor)//100hz
{
	Monitor->circle_number++; //在串口接收回调函数中清零，若没清零则累计技术。累计三次说名通信故障
	if(Monitor->circle_number > 50)
	{
		Monitor->status=monitor_error;
		RC_CtrlData.RCDataParaInit(&RC_CtrlData);
		time_piece=0;
	}
	else
		Monitor->status=monitor_regular;
}

void MonitorParaInit(Monitor_t *Monitor)
{
	Monitor->MonitorParaInit=&MonitorParaInit;
	/*--判断监视对象--*/
	if(Monitor==&monitor_remote)
		Monitor->monitor_process=&monitor_remote_process;
	else if(Monitor==&monitor_tx2)
		Monitor->monitor_process=&monitor_tx2_process;
	else if(Monitor==&monitor_can_power)
		Monitor->monitor_process=&monitor_can_power_process;
	else if(Monitor==&monitor_JY901)
		Monitor->monitor_process=&monitor_JY901_process;
}

