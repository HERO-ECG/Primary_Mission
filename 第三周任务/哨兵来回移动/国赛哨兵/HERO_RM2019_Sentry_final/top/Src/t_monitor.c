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

//监测JY901通信
void monitor_JY901_process(Monitor_t *Monitor)//100hz
{
	Monitor->circle_number++; //在串口接收回调函数中清零，若没清零则累计技术。累计三次说名通信故障
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
	/*--判断监视对象--*/
	if(Monitor==&monitor_remote)
		Monitor->monitor_process=&monitor_remote_process;
	else if(Monitor==&monitor_tx2)
		Monitor->monitor_process=&monitor_tx2_process;
	else if(Monitor==&monitor_JY901)
		Monitor->monitor_process=&monitor_JY901_process;
}
