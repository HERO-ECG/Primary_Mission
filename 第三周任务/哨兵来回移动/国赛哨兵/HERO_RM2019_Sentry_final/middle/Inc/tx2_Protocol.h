#ifndef __M_TX2_PROTOCOL_H
#define __M_TX2_PROTOCOL_H
#include "mytype.h"
#include "robodata.h"
typedef enum
{
	tx2_normal,
	tx2_error
}TX2_STATE;

typedef __packed struct TX2_Ctl_t
{
	void (*TX2DataParaInit)(struct TX2_Ctl_t *tx2_ReadData);
	float delta_y_fir;
	float delta_p_fir;
	float longth;
	uint8_t tx2_speed;
	uint8_t control_mode;
	uint8_t check_sum;
	uint8_t sta;
}TX2_Data_t;  //สýพÝึก
extern TX2_Data_t tx2_ReadData;
extern uint8_t TX2_Data_Send[5];
extern void tx2_send(void);
extern float tx2_monitor_count;
extern uint8_t tx2_state;
extern float usart_monitor;
extern uint8_t usart3_state;
extern uint8_t usart_monitor_begin_flag;
extern uint8_t wushibie_flag;
extern float wushibie_pending;
extern short TX2_DataProcess(uint8_t *pData, TX2_Data_t *tx2_ReadData);
extern short TX2_DataProcess_Send(uint8_t *pData);
extern void TX2DataParaInit(TX2_Data_t *tx2_ReadData);
extern void TX2_monitor(void);


#endif
