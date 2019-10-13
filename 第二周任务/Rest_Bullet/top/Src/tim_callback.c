#include "main.h"
#include "control.h"
#include "t_moto.h"
#include "robodata.h"
#include "t_monitor.h"
#include "chassis.h"

uint32_t time_piece=0x0000;//时间片，用于在while(1)中实现软实时
uint32_t divider;


int k;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint32_t divider100hz;
    if((time_piece&time_piece_start) == time_piece_start) {
        if(htim == &htim3) //100Hz定时器服务函数
        {
            time_piece |= time_piece_0100hz;
            divider100hz++;
            if(divider100hz%20==0)//二十分频，5Hz
            {
                CustomSend();//向电脑客户端发送自定义数据
            }

            Monitor_LED_Display();//在板子上的LED&蜂鸣器显示异常和报警
            TX2_SetMode();//向视觉小电脑发送自瞄设置信息
            Chassis_Fun();//【底盘运动服务函数】

            Monitor_Motor();//电机监视器
            monitor_tx2_process(&monitor_tx2);//视觉小电脑通信数据监控器
            monitor_tx2_valid_process(&monitor_tx2_valid);//视觉小电脑通信有效监视器
            monitor_can_power_process(&monitor_can_power);//超级电容通信监视器
            monitor_dji_process(&monitor_dji);//裁判系统监视器


            if(divider100hz==100000)
                divider100hz=0;
        }
        else if(htim == &htim5)//1000hz定时器服务函数
        {
            time_piece |= time_piece_1000hz;

            IMU_Get_Data();//读取陀螺仪数据
            RoboData.chassis_ctrl.pitch_speed_gyro=imu_data.gy;//把陀螺仪数据赋值给底盘数据结构体
            RoboData.chassis_ctrl.yaw_speed_gyro=imu_data.gz;

            //底盘电机信号发送
            SetMotoCurrent(&hcan2, STDID_3508, Chassis_MOTO[0].send_current, Chassis_MOTO[1].send_current, Chassis_MOTO[2].send_current, Chassis_MOTO[3].send_current);
        }
    }
}

/*******************往下都没用*************************************/

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





