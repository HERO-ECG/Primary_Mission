#ifndef __monitor_H
#define __monitor_H


/* Includes ------------------------------------------------------------------*/
#include "mytype.h"
#include "control.h"

#define BEEP_ON							1
#define BEEP_OFF						0

typedef enum
{
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
	
extern Monitor_t monitor_remote;
extern Monitor_t monitor_tx2;
extern Monitor_t monitor_can_power;
extern Monitor_t monitor_dji;
extern Monitor_t monitor_tx2_valid;
extern Monitor_t monitor_motor[4];

extern void MonitorParaInit(Monitor_t *Monitor);
void monitor_dji_process(Monitor_t *Monitor);

void Monitor_LED_Display(void);
void monitor_tx2_process(Monitor_t *Monitor);
void monitor_tx2_valid_process(Monitor_t *Monitor);
void monitor_can_power_process(Monitor_t *Monitor);
void Monitor_Motor(void);
void LED_GPIO_Init(void);
void KEY_GPIO_Init(void);
void TIM12_Init(void);
void LED_CTRL( uint8_t LED_number, uint8_t PinState);
void BEEP_Set(uint8_t beep_state,uint32_t period);
#endif



