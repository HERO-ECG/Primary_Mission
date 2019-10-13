#include "usart.h"
#include "tx2_Protocol.h"
#include "m_moto.h"
#include "dji_Protocol.h"
#include "pitch.h"
#include "auto_move.h"

	uint8_t checksum = 0;
	uint16_t tx2_dengdai_querenzhuangtai =0;
	float tx2_monitor_count=0;
	uint8_t tx2_state=0;
	float usart_monitor=0;//tx2串口的监视器 如果断了停掉串口，过一会重新开始
	uint8_t usart3_state=0;
	uint8_t usart_monitor_begin_flag;
	
	float fangzhi_dangzhu_flag=0;
	extern uint8_t IsOnTarget(float dp,float dy);
	
	uint8_t wushibie_flag;
	float wushibie_pending;
	
TX2_Data_t tx2_ReadData;
short TX2_DataProcess(uint8_t *pData, TX2_Data_t *tx2_ReadData)
{
	static uint8_t last_OvermodeSta=0;
	static float last_delta_y_fir;
	static float last_delta_p_fir;
	static float total_delta_y_fir;
	static float total_delta_p_fir;
	 uint8_t i;
	 uint8_t* data_ptr;

	 for(i=0;i<28;i++)
	 {
				 if((pData[i]==0x55)&&(pData[i+1]==0x56)&&(pData[i+10]==0xA5))
				 { 
							data_ptr = (uint8_t*)(pData+i+2);	
							tx2_ReadData->delta_y_fir =  (short)(data_ptr[0] | data_ptr[1]<<8)/100.0;	
							tx2_ReadData->delta_p_fir =	(short)(data_ptr[2] | data_ptr[3]<<8)/100.0;
							tx2_ReadData->longth=(short)(data_ptr[4] | data_ptr[5]<<8)/100.0;
//							tx2_ReadData->tx2_speed   =  data_ptr[4];
							tx2_ReadData->control_mode =  data_ptr[6];
							tx2_ReadData->check_sum = data_ptr[7];
							

							checksum =(uint8_t)(tx2_ReadData->delta_y_fir*100+tx2_ReadData->delta_p_fir*100+tx2_ReadData->control_mode);	
					 
							if(tx2_ReadData->control_mode!=0)//checksum == tx2_ReadData->check_sum)//
							{
								
								if(tx2_ReadData->longth>10||(tx2_ReadData->control_mode==attack&&tx2_ReadData->longth<3.5f&&Pitch.angle<-440))
								{
							tx2_ReadData->delta_y_fir = 0;
							tx2_ReadData->delta_p_fir =	0;
							tx2_ReadData->control_mode = dodge_level2 ;
								}
								

								
								tx2_monitor_count=0;
								last_OvermodeSta = overall_mode.tx2_gimble;
								
								
								
									
									
								total_delta_y_fir=(float)tx2_ReadData->delta_y_fir*1.0f+last_delta_y_fir*0.0f;
								total_delta_p_fir=(float)tx2_ReadData->delta_p_fir*1.0f+last_delta_p_fir*0.0f;
								
//								if(Chassis_MOTO[0].getpara.speed_rpm<0)//3000&&Chassis_MOTO[0].getpara.speed_rpm<5300)
//									total_delta_y_fir+=5.0f;
								
								
//								else if((Chassis_MOTO[0].getpara.speed_rpm>5400 )&&(Chassis_MOTO[0].getpara.speed_rpm<10000))
//									total_delta_y_fir+=3.0f;
								
								pitch_dangshi_angle=Pitch_MOTO.getpara.total_angle-(float)(total_delta_p_fir*8192.0f)/360.0f*1.0f;
								yaw_dangshi_angle=Yaw_Change.angle-(float)((total_delta_y_fir*8192.0f)/360.0f)*1.0f;
								
								
		if(pitch_dangshi_angle<-512)
		{

							tx2_ReadData->control_mode = dodge_level2 ;
		}		
	
			
							
								if(tx2_ReadData->control_mode == attack)
									fangzhi_dangzhu_flag++;
								else
									fangzhi_dangzhu_flag=0;
								
								
								
								
								static float History_Angle[25];
								static int i=0;
								static int flag;
								static float average_angle;
								if(i>24)
								{
									for(i=0;i<24;i++)
									History_Angle[i]=History_Angle[i+1];
									i=24;
									flag=1;
								}
								else 
								flag=0;
								
								
								History_Angle[i]=Yaw_Change.angle;
								i++;
								if(flag==1)
									average_angle=1.0f*(History_Angle[24]-History_Angle[0])/25.0f;
								
								
								yaw_dangshi_angle=yaw_dangshi_angle+average_angle;
								
								
								if(tx2_ReadData->control_mode != attack && last_OvermodeSta== attack)//减少丢目标造成的抖动
								{

									tx2_dengdai_querenzhuangtai++;
									if(tx2_dengdai_querenzhuangtai>=40&&fangzhi_dangzhu_flag<100)
									{
									//	tx2_ReadData->sta = 0;
										overall_mode.tx2_gimble = tx2_ReadData->control_mode;
										tx2_dengdai_querenzhuangtai=0;
										fangzhi_dangzhu_flag=0;
									}
									else if(tx2_dengdai_querenzhuangtai>=200)
									{
										overall_mode.tx2_gimble = tx2_ReadData->control_mode;
										tx2_dengdai_querenzhuangtai=0;
										fangzhi_dangzhu_flag=0;
									}
									
										for(i=0;i<25;i++)
									History_Angle[i]=0;
									i=0;
									flag=0;
									average_angle=0;
								}
								else
								{overall_mode.tx2_gimble = tx2_ReadData->control_mode;}//除了攻击模式切换到其他的模式之外 其他模式之间迅速切换
								


								
								last_delta_p_fir=total_delta_p_fir;
								last_delta_y_fir=total_delta_y_fir;
									return 1;
							}
					}
	}
overall_mode.OvermodeSta=dodge_level2;	 
	return 0;
}	


uint8_t TX2_Data_Send[5] = {0,0,0,0,0};
short  TX2_DataProcess_Send(uint8_t *pData)
{
//		uint8_t i;
//		short* data_ptr;
		pData[0] = 0x55;
		pData[1] = 0x56;
		//data_ptr = (short*)(pData+i+2);	
	pData[2] = (short)((Pitch.angle-(-544))*439.45f)>>8;
	pData[3] = (short)((Pitch.angle-(-544))*439.45f);
//	pData[4] = rest_bullet>>8;
//	pData[5] = rest_bullet;
//		data_ptr[0] = 0;	
//		data_ptr[1] = 0;
//	pData[6] = (uint8_t)(DJI_ReadData.ext_game_robot_state.remain_HP+rest_bullet);	
		pData[4] = 0xA5;
	
		return 1;
}

void tx2_send(void)
{
			TX2_DataProcess_Send(TX2_Data_Send);
		HAL_UART_Transmit(&huart3,TX2_Data_Send,sizeof(TX2_Data_Send),5);
}


void TX2DataParaInit(TX2_Data_t *tx2_ReadData)
{
	 tx2_ReadData->TX2DataParaInit = &TX2DataParaInit;
	 tx2_ReadData->delta_y_fir =  0;	
	 tx2_ReadData->sta = 0;
}


void TX2_monitor(void)
{
	if(tx2_monitor_count >= 500)
		tx2_state=tx2_error;
	else
		tx2_state=tx2_normal;
		
	if(tx2_state == tx2_error)
	{
		overall_mode.OvermodeSta=dodge_level1;
	}
	
}


