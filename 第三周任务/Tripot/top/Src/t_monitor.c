#include "t_monitor.h"
#include "strike.h"
#include "m_remote.h"
#include "tx2_Protocol.h"
Monitor_t monitor_remote;
Monitor_t monitor_tx2;
Monitor_t monitor_can_power;
Monitor_t monitor_JY901;

//���ң��ͨ���Ƿ�����
void monitor_remote_process(Monitor_t *Monitor)//100hz
{
	Monitor->circle_number++; //�ڴ��ڽ��ջص����������㣬��û�������ۼƼ������ۼ�����˵��ͨ�Ź���
	if(Monitor->circle_number > 3)
	{
		Monitor->status=monitor_error;
		RC_CtrlData.RCDataParaInit(&RC_CtrlData);
	}
	else
		Monitor->status=monitor_regular;
}
//���TX2������֮���ͨ���Ƿ�����
void monitor_tx2_process(Monitor_t *Monitor)//100hz
{
	Monitor->circle_number++; //�ڴ��ڽ��ջص����������㣬��û�������ۼƼ������ۼ�7��˵��ͨ�Ź���
	if(Monitor->circle_number > 7)
	{
		Monitor->status=monitor_error;
			RoboData.shoot_heat_ctr.addHeat17=friction17_spd_regular*1;//???
//			RoboData.AmmunitionControl.Toggler17_angle=Toggler17_angle_one;
//			RoboData.AmmunitionControl.Toggler42_angle=Toggler42_angle_one;
			RoboData.AmmunitionControl.Friction17_speed=friction17_spd_regular; //FrictionĦ����
		//TX2_Data.TX2DataParaInit(&TX2_Data);
	}
	else
		Monitor->status=monitor_regular;
}
//������ϵͳ�ģ��������������㣿������������
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

//���JY901ͨ��
void monitor_JY901_process(Monitor_t *Monitor)//100hz
{
	Monitor->circle_number++; //�ڴ��ڽ��ջص����������㣬��û�������ۼƼ������ۼ�����˵��ͨ�Ź���
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
	/*--�жϼ��Ӷ���--*/
	if(Monitor==&monitor_remote)
		Monitor->monitor_process=&monitor_remote_process;
	else if(Monitor==&monitor_tx2)
		Monitor->monitor_process=&monitor_tx2_process;
	else if(Monitor==&monitor_can_power)
		Monitor->monitor_process=&monitor_can_power_process;
	else if(Monitor==&monitor_JY901)
		Monitor->monitor_process=&monitor_JY901_process;
}

