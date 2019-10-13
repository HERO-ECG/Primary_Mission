/*
 author:GTR
 data:2018.12.16
 adapted from 无人车电控V2.4\jy901
 */
#include "hwt901b.h"
#include "usart.h"
 /*调用外部接口*/


//定义HWT-901角度输出的结构体的接收数组
struct SAngle ANGLE;

uint8_t test=0;

void HWT901DataProcess(uint8_t* pData,uint8_t length)
{
	
	uint8_t  i=0;
	uint8_t  j=2;
	short start_flag=1;
	ANGLE.SUM=0;
	
	
	if(pData==NULL)
	{
		return;
	}
	else
	{
		start_flag=start_flag;
		for(i=0;i<length-10;i++)
		{
			if(pData[i]==0x55 )//判断是否遇到帧头，数据是否只使用一次
			{
				for(j=i;j<i+10;j++)
				{
					ANGLE.SUM=ANGLE.SUM+pData[j];//计算验校和
				}
				if(ANGLE.SUM==pData[i+10])//判断验校和
				{
					switch(pData[i+1])
					{
						case 0x53:
								ANGLE.Angle[0]=(int16_t)((int16_t)(pData[i+2]|(int16_t)(pData[i+3]<<8)))/32768.0f*180.0f - 180;
								ANGLE.Angle[1]=(int16_t)((int16_t)(pData[i+4]|(int16_t)(pData[i+5]<<8)))/32768.0f*180.0f - 180;
								ANGLE.Angle[2]=(-1)*((int16_t)((int16_t)(pData[i+6]|(int16_t)(pData[i+7]<<8)))/32768.0f*180.0f - 180);
						break;
						case 0x52:
								ANGLE.Angular_Velocity[0]=-(int16_t)(pData[i+2]|(pData[i+3]<<8))/32768.0f*2000.0f;
								ANGLE.Angular_Velocity[1]=(int16_t)(pData[i+4]|(pData[i+5]<<8))/32768.0f*2000.0f;
								ANGLE.Angular_Velocity[2]=(int16_t)(pData[i+6]|(pData[i+7]<<8))/32768.0f*2000.0f;
								break;
						case 0x51:
								ANGLE.Acc[0]=(int16_t)(pData[i+2]|(pData[i+3]<<8))/32768.0f*16*9.8f;
								ANGLE.Acc[1]=(int16_t)(pData[i+4]|(pData[i+5]<<8))/32768.0f*16*9.8f;
								ANGLE.Acc[2]=(int16_t)(pData[i+6]|(pData[i+7]<<8))/32768.0f*16*9.8f;
								break;
						default:break;
					}
				}
				ANGLE.SUM=0;//每次计算验校和后必要的清零
				start_flag=0;//数据使用标志位
			}
		}
  }
}

