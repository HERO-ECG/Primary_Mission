#include "usart.h"
#include "tx2_Protocol.h"
#include "m_moto.h"


TX2_Data_t TX2_Data;


void TX2_SetMode(void)
{
uint8_t modedata[5];
	modedata[0]=0x55;
	modedata[3]=0xA5;
	modedata[4]=0x0D;

	
	if(TX2_Data.mode_set==AUTOAIM_CLOSE)
	{
		modedata[1]=0x50;
		modedata[2]=0x00;
	}
	else	if(TX2_Data.mode_set==WINDMILL_CLKWISE)
	{
		modedata[1]=0x51;
		modedata[2]=0x01;
	}
	else if(TX2_Data.mode_set==WINDMILL_ANTICLKWISE)
	{
		modedata[1]=0x52;
		modedata[2]=0x02;
	}
	else if(TX2_Data.mode_set==AUTOAIM_LONG)
	{
		modedata[1]=0x54;
		modedata[2]=0x04;
	}

	HAL_UART_Transmit(&huart3,modedata,4,1000);
}





