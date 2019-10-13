#include "main.h"
#include "control.h"
#include "robodata.h"
#include "t_moto.h"
#include "tripod.h"
#include "strike.h"
#include "chassis.h"
#include "t_monitor.h"
#include "m_remote.h"


uint8_t time_piece_start=0;
uint32_t time_piece=0x0000;

/*Robodata�еľɲ����������õ�*/
float friction_set_speed_debug = 7000.0f;
float ctrl_spd_test = 0.0f;
float ammo_spd_test = 0.0f;
uint8_t TG_init_sta=0;


void vcan_sendware(u8 *wareaddr, u32 waresize)
{
#define CMD_WARE     3
    u8 cmdf[2] = {CMD_WARE, ~CMD_WARE};    //���ڵ��� ʹ�õ�ǰ����
    u8 cmdr[2] = {~CMD_WARE, CMD_WARE};    //���ڵ��� ʹ�õĺ�����

		HAL_UART_Transmit(&huart3,cmdf,sizeof(cmdf),1000);
		HAL_UART_Transmit(&huart3,wareaddr,waresize,1000);
		HAL_UART_Transmit(&huart3,cmdr,sizeof(cmdr),1000);
}



float var[8];

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint16_t divider;
	if(time_piece_start == 1){
		if(htim == &htim3){         //100hz
			time_piece |= time_piece_0100hz;
			/*������ƺ�����������ͺ�������*/
			if(divider<10000)
				divider++;
			else
				divider=0;
			if(divider%5==0)
				CustomSend();//�ͻ������ݷ��ͺ���
			
			if(imu_data.init_sta)//�ȴ������ǳ�ʼ�����
				Chassis_Fun();//�����˶�������
	//		vcan_sendware((uint8_t*) var, (uint32_t)sizeof(var));

			
		}
		else if(htim == &htim5){		//1000hz
			static uint8_t tim5_cnt=0;
			float tempYawSpeed=0;
			time_piece |= time_piece_1000hz;
			IMU_Get_Data();//��ð�������������
			
			tempYawSpeed = imu_data.gz*GYRO_2000_SEN*5;//�ʵ��Ŵ�5��
			ABSLimit(&tempYawSpeed,250);
			RoboData.chassis_ctrl.yaw_speed_gyro = tempYawSpeed*0.05+RoboData.chassis_ctrl.yaw_speed_gyro*0.95;

			
			//���������
			if(Strike.count_42mm.out_firecounter<2){
				static uint16_t wait_time=0,wait_flag=0;
				if(ABS(LittleAmmo_MOTO.getpara.LPF_current)>5000){
					wait_flag = 1;//���������ȸ��ص����������յ��
				}
				if(wait_flag){
					wait_time ++;
					LittleAmmo_MOTO.set_speed = -2000;//��ת
					if(wait_time>1000)
						wait_flag=0;
				}else{
					LittleAmmo_MOTO.set_speed = 2000;
					wait_time = 0;
				}
			}else
				LittleAmmo_MOTO.set_speed = 0;
			LittleAmmo_MOTO.send_current = LittleAmmo_MOTO.pid_speed.PidCalc(&LittleAmmo_MOTO.pid_speed, LittleAmmo_MOTO.getpara.speed_rpm, LittleAmmo_MOTO.set_speed);
			
			
			/*can�źŷ���*/
			tim5_cnt++;
			if(tim5_cnt==1)
			{				
				SetMotoCurrent(&hcan1, STDID_LOW, Chassis_MOTO[0].send_current, Chassis_MOTO[1].send_current, Chassis_MOTO[2].send_current, Chassis_MOTO[3].send_current);
				SetMotoCurrent(&hcan1, STDID_HIGH, LittleAmmo_MOTO.send_current, 0, 0, 0);

//				SetMotoCurrent(&hcan2, STDID_HIGH,0,0,0,temp_current);
			}
			else if(tim5_cnt>1)
			{	
				TX_DATA_CHASSIS1_ID(&hcan2);
				tim5_cnt=0;
			}
			/*can�źŷ��䣬�������������can�źŴ����Լ���������*/
		}
	}
}
