#include "t_monitor.h"
#include "t_moto.h"
#include "m_remote.h"
#include "tx2_Protocol.h"
#include "chassis.h"

Monitor_t monitor_remote;
Monitor_t monitor_tx2;
Monitor_t monitor_can_power;

void monitor_remote_process(Monitor_t *Monitor)//100hz
{
	Monitor->circle_number++;
	if(Monitor->circle_number > 20)
	{
		Monitor->status = monitor_error;
		RC_CtrlData.RCDataParaInit(&RC_CtrlData);
	}
	else
		Monitor->status = monitor_regular;
}

void monitor_tx2_process(Monitor_t *Monitor)//100hz
{
	Monitor->circle_number++;
	if(Monitor->circle_number > 7)
	{
		Monitor->status = monitor_error;
//			RoboData.shoot_heat_ctr.addHeat17 = Friction17_spd_regular*1;
//			RoboData.AmmunitionControl.Toggler17_angle = Toggler17_angle_one;
//			RoboData.AmmunitionControl.Toggler42_angle = Toggler42_angle_one;
//			RoboData.AmmunitionControl.Friction17_speed = Friction17_spd_regular;
	}
	else
		Monitor->status = monitor_regular;
}

void monitor_can_power_process(Monitor_t *Monitor)//100hz
{
	Monitor->circle_number++;
	if(Monitor->circle_number > 100)
	{
		Monitor->status = monitor_error;
		Chassis.supercapacitor.sup_c_open=0;
		RoboData.chassis_ctrl.sup_s_status = 0;
		RoboData.chassis_ctrl.chassis_order = Order_stop;
		RoboData.chassis_ctrl.chassis_working_status = WorkingStatus_error;	
		HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_SET);		//ÁÁ
	}
	else
		Monitor->status = monitor_regular;
}

void MonitorParaInit(Monitor_t *Monitor)
{
	Monitor->MonitorParaInit = &MonitorParaInit;
	Monitor->circle_number = 100;
	/*--ÅÐ¶Ï¼àÊÓ¶ÔÏó--*/
	if(Monitor == &monitor_remote)
		Monitor->monitor_process = &monitor_remote_process;
	else if(Monitor == &monitor_tx2)
		Monitor->monitor_process = &monitor_tx2_process;
	else if(Monitor == &monitor_can_power)
		Monitor->monitor_process = &monitor_can_power_process;
}
