#include "usart.h"
#include "tx2_Protocol.h"
#include "m_moto.h"
#include "math.h"

TX2_Data_t TX2_Data;

short TX2_DataProcess(uint8_t *pData, TX2_Data_t *TX2_Data,uint16_t length)
{
	 uint8_t i;
	uint8_t j;
	uint8_t checksum=0;
	uint8_t success=0;
	 for(i=0;i<length-10;i++)
	 {
		 if((pData[i]==0x55)&&(pData[i+8]==0xA5))
		 { 
			 checksum=0;
			 for(j=0;j<5;j++)
				checksum+=pData[j+2];
			//if(checksum==pData[i+7])
			 {
				 switch(pData[i+1])
				 {
					 case 0x50:TX2_Data->mode_now=AUTOAIM_CLOSE;break;
					 case 0x51:TX2_Data->mode_now=WINDMILL_CLKWISE;break;
					 case 0x52:TX2_Data->mode_now=WINDMILL_ANTICLKWISE;break;
					 case 0x53:TX2_Data->mode_now=WINDMILL_STILL;break;
					 case 0x54:TX2_Data->mode_now=AUTOAIM_LONG;break;
				 }
					TX2_Data->yaw_angle = (int16_t)((int16_t)(pData[i+2]|(int16_t)(pData[i+3]<<8)))/1000.0f;
					TX2_Data->pitch_angle = (int16_t)((int16_t)(pData[i+4]|(int16_t)(pData[i+5]<<8)))/1000.0f;

				 
					TX2_Data->velocity=(int16_t)(pData[i+6]);
				 if((fabs(TX2_Data->yaw_angle) >60.0f)|| (fabs(TX2_Data->pitch_angle)>60.0f))
				 {
					 
					 TX2_Data->yaw_angle=0;
					 TX2_Data->pitch_angle=0;
					 TX2_Data->velocity=0;
					 
				 }
				 success=1;

			}
			}
	}			 
	return success;
}	 
/*A板向TX2发数据*/
void TX2_SetMode(void)
{
	uint8_t modedata[6];
	modedata[0]=0x55;
	modedata[5]=0xA5;

	
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
	else if(TX2_Data.mode_set==WINDMILL_ANTICLKWISE)//神符
	{
		modedata[1]=0x52;
		modedata[2]=0x02;
	}
	else if(TX2_Data.mode_set==AUTOAIM_LONG)
	{
		modedata[1]=0x54;
		modedata[2]=0x04;
	}
	
	modedata[3] = (int16_t)(((int16_t)(TX2_Data.now_pitch_tx*100))>>8);
	modedata[4] = (int16_t)(((int16_t)(TX2_Data.now_pitch_tx*100)));

	HAL_UART_Transmit(&huart3,modedata,6,100);
}





