#include "tx2_Protocol.h"
#include "tripod.h"
#include "usart.h"
/*英雄使用的是NUC不是TX2*/
TX2_Data_t TX2_Data;
short TX2_DataProcess(uint8_t *pData, TX2_Data_t *TX2_Data,uint16_t length)
{
	uint8_t i;
	uint8_t* data_ptr;
	uint8_t success=0;
	for(i=0;i<length-7;i++)
	{
		 if((pData[i]==0x55)&&(pData[i+1]==0x50)&&(pData[i+8]==0xA5))
		 { 
			 float y=0,p=0,s=0;
					data_ptr = (uint8_t*)(pData+i+2);	
					y = (short)(data_ptr[0] | data_ptr[1]<<8)/1000.0f;	
					p =	(short)(data_ptr[2] | data_ptr[3]<<8)/1000.0f;
					s =  data_ptr[4];
					if(ABS(y)<30&&ABS(p)<30)
						{
						TX2_Data->yaw_angle = y;
						TX2_Data->pitch_angle = p;
						TX2_Data->distance=s;
						success=1;
					}
		 }
	}			 
	return success;
}	
uint8_t TX2_sendDataBuff[4];
void TX2_sendData(uint8_t mode){
	TX2_sendDataBuff[0]=0x55;
	TX2_sendDataBuff[1]=0x50|mode;
	TX2_sendDataBuff[2]=0x00|mode;
	TX2_sendDataBuff[3]=0xA5;
	HAL_UART_Transmit(&huart3,TX2_sendDataBuff,sizeof(TX2_sendDataBuff),100);
}

