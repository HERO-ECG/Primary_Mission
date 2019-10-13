#include "main.h"
#include "control.h"
#include "t_moto.h"
#include "robodata.h"
#include "t_monitor.h"
#include "chassis.h"

uint32_t time_piece=0x0000;//ʱ��Ƭ��������while(1)��ʵ����ʵʱ
uint32_t divider;


int k;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint32_t divider100hz;
    if((time_piece&time_piece_start) == time_piece_start) {
        if(htim == &htim3) //100Hz��ʱ��������
        {
            time_piece |= time_piece_0100hz;
            divider100hz++;
            if(divider100hz%20==0)//��ʮ��Ƶ��5Hz
            {
                CustomSend();//����Կͻ��˷����Զ�������
            }

            Monitor_LED_Display();//�ڰ����ϵ�LED&��������ʾ�쳣�ͱ���
            TX2_SetMode();//���Ӿ�С���Է�������������Ϣ
            Chassis_Fun();//�������˶���������

            Monitor_Motor();//���������
            monitor_tx2_process(&monitor_tx2);//�Ӿ�С����ͨ�����ݼ����
            monitor_tx2_valid_process(&monitor_tx2_valid);//�Ӿ�С����ͨ����Ч������
            monitor_can_power_process(&monitor_can_power);//��������ͨ�ż�����
            monitor_dji_process(&monitor_dji);//����ϵͳ������


            if(divider100hz==100000)
                divider100hz=0;
        }
        else if(htim == &htim5)//1000hz��ʱ��������
        {
            time_piece |= time_piece_1000hz;

            IMU_Get_Data();//��ȡ����������
            RoboData.chassis_ctrl.pitch_speed_gyro=imu_data.gy;//�����������ݸ�ֵ���������ݽṹ��
            RoboData.chassis_ctrl.yaw_speed_gyro=imu_data.gz;

            //���̵���źŷ���
            SetMotoCurrent(&hcan2, STDID_3508, Chassis_MOTO[0].send_current, Chassis_MOTO[1].send_current, Chassis_MOTO[2].send_current, Chassis_MOTO[3].send_current);
        }
    }
}

/*******************���¶�û��*************************************/

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim == &htim2)
    {



        if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)
        {

        }
        else if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
        {

        }
        else if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_3)
        {

        }
        else if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_4)
        {

        }
    }
    else if(htim == &htim4)
    {
        if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)
        {

        }
        else if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
        {

        }
        else if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_3)
        {

        }
        else if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_4)
        {

        }
    }

}





