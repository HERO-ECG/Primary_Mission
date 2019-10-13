#ifndef __monitor_H
#define __monitor_H


/* Includes ------------------------------------------------------------------*/
#include "mytype.h"
#include "control.h"

typedef enum
{
	monitor_error=0,
	monitor_regular=1, //regular�ϸ�ģ��й��ɵ�
}monitor_status_e;


typedef struct __monitor{
	uint32_t circle_number;
	monitor_status_e status;
	void (*toggle)(void);
	void (*monitor_process)(struct __monitor *Monitor);
	void (*MonitorParaInit)(struct __monitor *Monitor);
}Monitor_t;
	

extern void MonitorParaInit(Monitor_t *Monitor);
extern Monitor_t monitor_remote;
extern Monitor_t monitor_tx2;
extern Monitor_t monitor_can_power;
extern Monitor_t monitor_JY901;
#endif



