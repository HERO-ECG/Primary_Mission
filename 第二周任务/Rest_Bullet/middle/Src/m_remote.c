
#include "m_remote.h"


RC_Ctl_t RC_CtrlData;

/*--------------------------------------------------------遥控器数据解码--------------------------------------------------------------------------------*/
void RCReadKey(RC_Ctl_t *RC_CtrlData)
{
		RC_CtrlData->key.key_data.W = (RC_CtrlData->key.v&0x0001)==0x0001;
		RC_CtrlData->key.key_data.S = (RC_CtrlData->key.v&0x0002)==0x0002;
		RC_CtrlData->key.key_data.A = (RC_CtrlData->key.v&0x0004)==0x0004;
		RC_CtrlData->key.key_data.D = (RC_CtrlData->key.v&0x0008)==0x0008;
		RC_CtrlData->key.key_data.shift = (RC_CtrlData->key.v&0x0010)==0x0010;
		RC_CtrlData->key.key_data.ctrl = (RC_CtrlData->key.v&0x0020)==0x0020;
		RC_CtrlData->key.key_data.Q = (RC_CtrlData->key.v&0x0040)==0x0040;
		RC_CtrlData->key.key_data.E = (RC_CtrlData->key.v&0x0080)==0x0080;
		RC_CtrlData->key.key_data.R = (RC_CtrlData->key.v&0x0100)==0x0100;
		RC_CtrlData->key.key_data.F = (RC_CtrlData->key.v&0x0200)==0x0200;
		RC_CtrlData->key.key_data.G = (RC_CtrlData->key.v&0x0400)==0x0400;
		RC_CtrlData->key.key_data.Z = (RC_CtrlData->key.v&0x0800)==0x0800;
		RC_CtrlData->key.key_data.X = (RC_CtrlData->key.v&0x1000)==0x1000;
		RC_CtrlData->key.key_data.C = (RC_CtrlData->key.v&0x2000)==0x2000;
		RC_CtrlData->key.key_data.V = (RC_CtrlData->key.v&0x4000)==0x4000;
		RC_CtrlData->key.key_data.B = (RC_CtrlData->key.v&0x8000)==0x8000;
}

void RemoteDataProcess(uint8_t *pData, RC_Ctl_t *RC_CtrlData)
{
	
	uint8_t s1 = 0;
	uint8_t s2 = 0;
	uint16_t ch0 = 0;
	uint16_t ch1 = 0;
	uint16_t ch2 = 0;
	uint16_t ch3 = 0;
	
	if(pData==NULL)
	{
	   return;
	}

				s1=((pData[5]>>4)&0x000c)>>2;
				s2=((pData[5]>>4)&0x0003);
				ch0=((int16_t)pData[0]|((int16_t)pData[1]<<8))&0x07FF;
				ch1=(((int16_t)pData[1]>>3)|((int16_t)pData[2]<<5))&0x07FF;
				ch2=(((int16_t)pData[2]>>6)|((int16_t)pData[3]<<2)|((int16_t)pData[4]<<10))&0x07FF;
				ch3=(((int16_t)pData[4]>>1)|((int16_t)pData[5]<<7))&0x07FF;
				
				if(
					((s1 == 1) || (s1 == 2) || (s1 == 3)) && 
					((s2 == 1) || (s2 == 2) || (s2 == 3))&&
					((pData[12] == 0)||(pData[12] == 1))&&
					((pData[13] == 0)||(pData[13] == 1))&&
					(ch0<1700)&&(ch0>350)&&
					(ch1<1700)&&(ch1>350)&&
					(ch2<1700)&&(ch2>350)&&
					(ch3<1700)&&(ch3>350)
					)														
				{
					RC_CtrlData->rc.ch0=ch0;
					RC_CtrlData->rc.ch1=ch1;
					RC_CtrlData->rc.ch2=ch2;
					RC_CtrlData->rc.ch3=ch3;
					RC_CtrlData->rc.s1=s1;
					RC_CtrlData->rc.s2=s2;
					
					RC_CtrlData->mouse.x_speed=((int16_t)pData[6])|((int16_t)pData[7]<<8);
					RC_CtrlData->mouse.y_speed=((int16_t)pData[8])|((int16_t)pData[9]<<8);
					RC_CtrlData->mouse.z_speed=((int16_t)pData[10])|((int16_t)pData[11]<<8);
					RC_CtrlData->mouse.press_1=pData[12];
					RC_CtrlData->mouse.press_2=pData[13];
					RC_CtrlData->key.v=((int16_t)pData[14])|((int16_t)pData[15]<<8);
				} 
	

}




/*--------------------------------------------------------遥控器结构体数据初始化--------------------------------------------------------------------------------*/
void RCDataParaInit(RC_Ctl_t *RC_CtrlData)
{
	RC_CtrlData->RCDataParaInit = &RCDataParaInit;
	RC_CtrlData->rc.ch0 = 1024;
	RC_CtrlData->rc.ch1 = 1024;
	RC_CtrlData->rc.ch2 = 1024;
	RC_CtrlData->rc.ch3 = 1024;
	RC_CtrlData->mouse.x_speed = 0;
	RC_CtrlData->mouse.y_speed = 0;
	RC_CtrlData->mouse.z_speed = 0;
	RC_CtrlData->mouse.press_1 = 0;
	RC_CtrlData->mouse.press_2 = 0;
	RC_CtrlData->key.v = 0x0000;
	RCReadKey(RC_CtrlData);

}

