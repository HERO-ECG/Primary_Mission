#include "TW_servo.h"
#include "can.h"
#include "pid.h"


TW_CANopen_RX_t TW_CANopen_RX;
TW_servo_t TW_servo;

void TW_servo_SetSpeedMode(void)
{
	CANopen_SendData(&hcan1,TW_COBID_TX,ONE_BYTE_TXLENGTH,CONTROLMODE_W,0,CONTROLMODE_SPEED);
	HAL_Delay(2);
}

void TW_servo_SetPositionMode(void)
{
	CANopen_SendData(&hcan1,TW_COBID_TX,ONE_BYTE_TXLENGTH,CONTROLMODE_W,0,CONTROLMODE_POSITION);
	HAL_Delay(2);
}

void TW_servo_SetPositionModeMode(u8 mode)
{
	if(mode==	POSITIONMODE_ABSOLUTE)
		CANopen_SendData(&hcan1,TW_COBID_TX,ONE_BYTE_TXLENGTH,CONTROLMODE_W,0,POSITIONMODE_ABSOLUTE);
	else
		CANopen_SendData(&hcan1,TW_COBID_TX,TWO_BYTE_TXLENGTH,CONTROLWORD,0,POSITIONMODE_RELATIVE);
	
	HAL_Delay(2);
}


void TW_servo_QuickStop(void)
{
	CANopen_SendData(&hcan1,TW_COBID_TX,TWO_BYTE_TXLENGTH,CONTROLWORD,0,QUICKSTOP);

}

void TW_servo_SetPosition(s32 move)
{
	CANopen_SendData(&hcan1,TW_COBID_TX, FOUR_BYTE_TXLENGTH,POSITION_W, 0x00,move);
}
	

void TW_servo_GetPosition(void)
{
	CANopen_SendData(&hcan1,TW_COBID_TX,READ,POSITION_R,0x00,0);
}

void TW_servo_GetSpeed(void)
{
	CANopen_SendData(&hcan1,TW_COBID_TX,READ,SPEED_R,0x00,0);
}

void TW_servo_GetCurrent(void)
{
	CANopen_SendData(&hcan1,TW_COBID_TX,READ,CURRENT_R,0x00,0);
}

void TW_servo_SetSpeed(s32 speed)
{
	if(speed==0)
	{
		TW_servo_QuickStop();
	}
	else
	{
		CANopen_SendData(&hcan1,TW_COBID_TX, FOUR_BYTE_TXLENGTH,SPEED_W	, 0x00,speed);
	}
}
	
void CANopen_SendData(CAN_HandleTypeDef* hcan, uint32_t cob_id, u8 code, u16 object_index, u8 subindex,s32 object_val)
{
	hcan->pTxMsg->StdId = cob_id;
	hcan->pTxMsg->IDE = CAN_ID_STD;
	hcan->pTxMsg->RTR = CAN_RTR_DATA;
	hcan->pTxMsg->DLC = 0x08;
	hcan->pTxMsg->Data[0] = code;
	hcan->pTxMsg->Data[1] = object_index;
	hcan->pTxMsg->Data[2] = object_index >> 8;
	hcan->pTxMsg->Data[3] = subindex;
	hcan->pTxMsg->Data[4]=	object_val;
	hcan->pTxMsg->Data[5]=	object_val>>8;
	hcan->pTxMsg->Data[6]=	object_val>>16;
	hcan->pTxMsg->Data[7]=	object_val>>24;
	HAL_CAN_Transmit(hcan, 5);
}

void CANopen_ReadData(CAN_HandleTypeDef* hcan)
{
	TW_CANopen_RX.code=hcan->pRxMsg->Data[0];
	TW_CANopen_RX.object_index=hcan->pRxMsg->Data[1] + (hcan->pRxMsg->Data[2]<<8);
	TW_CANopen_RX.subindex=hcan->pRxMsg->Data[3];
	TW_CANopen_RX.object_val=hcan->pRxMsg->Data[4]+(hcan->pRxMsg->Data[5]<<8)+(hcan->pRxMsg->Data[6]<<16)+(hcan->pRxMsg->Data[7]<<24);
	TW_ProcessData(TW_CANopen_RX);
}

void TW_ProcessData(TW_CANopen_RX_t TW_CANopen_RX)
{
	switch(TW_CANopen_RX.object_index)
	{
		case POSITION_R:	
			if(TW_CANopen_RX.code==TW_FOUR_BYTE_RXLENGTH)
				TW_servo.position = TW_CANopen_RX.object_val;
				TW_Servo_GetEncoderMeasure();
			break;
		case SPEED_R:
			TW_servo.speed=TW_CANopen_RX.object_val;
			break;
		case CURRENT_R:
			TW_servo.current=TW_CANopen_RX.object_val;
			break;
	}
}


void TW_Servo_GetEncoderMeasure(void)
{
	
	if(TW_servo.init_state == 0)
	{
		TW_servo.init_cnt++;
		if(TW_servo.init_cnt>5)
			TW_servo.init_state = 1;
		else{
			TW_servo.offset_angle = TW_servo.position%4096 ; /*得到初始角度值*/
			TW_servo.offset_round = (float)(TW_servo.position%4096)/4096.0f;
		}
	}
	else{
		TW_servo.total_angle=(TW_servo.position-TW_servo.offset_angle);
		TW_servo.angle = ((float)((TW_servo.total_angle%4096+4096)%4096))/4096.0f*360.0f;      /*得到角度值*/
		TW_servo.speed = TW_servo.speed;		/*计算转子速度*/
		TW_servo.round = (float)TW_servo.total_angle/4096.0f -TW_servo.offset_round; 
	}	
}



void TW_servo_SetPositionInSpeedMode(float angle_set)
{
	
	float angle_err;
	while(angle_set<0)
		angle_set+=360;
	while(angle_set>360)
		angle_set-=360;
	TW_servo.angle_set=angle_set;
	if(TW_servo.angle<angle_set)
	{
		if(angle_set-TW_servo.angle<180)
			angle_err=angle_set-TW_servo.angle;
		else
			angle_err=(angle_set-TW_servo.angle)-360;
	}
	else
	{
		if(TW_servo.angle-angle_set<180)
			angle_err=angle_set-TW_servo.angle;
		else
			angle_err=360-(TW_servo.angle-angle_set);
	}
	TW_servo.speed_set=TW_servo.PID_angle.PidCalc(&TW_servo.PID_angle,-angle_err,0);
	TW_servo_SetSpeed(TW_servo.speed_set);
}

void TW_servo_Init(void)
{
	PidParaInit(&TW_servo.PID_angle);
	
	TW_servo.PID_angle.PidSet_all(&TW_servo.PID_angle, POSITION_PID, 5000, 0, 0, 80, 0, 40, 0, 0, 0, 0, 0, 0, 0);
	TW_servo_SetSpeedMode();
}




