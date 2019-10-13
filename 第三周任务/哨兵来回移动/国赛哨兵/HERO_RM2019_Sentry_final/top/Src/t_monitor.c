#include "t_monitor.h"
#include "m_remote.h"
#include "tx2_Protocol.h"
#include "hwt901b.h"
#include "pitch.h"
Monitor_t monitor_remote;
Monitor_t monitor_tx2;
Monitor_t monitor_JY901;

void monitor_remote_process(Monitor_t *Monitor)//100hz
{
	Monitor->circle_number++;
	if(Monitor->circle_number > 3)
	{
		Monitor->status = monitor_err;
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
		Monitor->status = monitor_err;
		tx2_ReadData.TX2DataParaInit(&tx2_ReadData);
	}
	else
		Monitor->status = monitor_regular;
}

//���JY901ͨ��
void monitor_JY901_process(Monitor_t *Monitor)//100hz
{
	Monitor->circle_number++; //�ڴ��ڽ��ջص����������㣬��û�������ۼƼ������ۼ�����˵��ͨ�Ź���
	if(Monitor->circle_number > 50)
	{
		Monitor->status=monitor_err;
		HWT901BDataParaInit();
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
	else if(Monitor==&monitor_JY901)
		Monitor->monitor_process=&monitor_JY901_process;
}
