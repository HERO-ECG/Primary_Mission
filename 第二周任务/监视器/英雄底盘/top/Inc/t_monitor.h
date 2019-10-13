#ifndef __monitor_H
#define __monitor_H

#include "mytype.h"
#include "control.h"

typedef enum{
	monitor_error = 0,
	monitor_regular = 1,
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
void monitor_remote_process(Monitor_t *Monitor);
void monitor_can_power_process(Monitor_t *Monitor);
#endif
