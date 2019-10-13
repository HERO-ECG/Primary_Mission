#include "t_monitor.h"
#include "t_moto.h"
#include "m_remote.h"
#include "chassis.h"
#include "tx2_Protocol.h"

Monitor_t monitor_remote;
Monitor_t monitor_tx2;
Monitor_t monitor_tx2_valid;
Monitor_t monitor_dji;
Monitor_t monitor_can_power;
Monitor_t monitor_motor[4];

uint8_t beep_off;


void LED_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOH_CLK_ENABLE();
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_8|GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5
                      |GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1, GPIO_PIN_SET);

    /*Configure GPIO pins : PG8 PG7 PG6 PG5
                             PG4 PG3 PG2 PG1 */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5
                          |GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}

void KEY_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOH_CLK_ENABLE();
    /*Configure GPIO pin : PB2 */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);
}

void TIM12_Init(void)
{
    TIM_MasterConfigTypeDef sMasterConfig;
    TIM_OC_InitTypeDef sConfigOC;
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_TIM12_CLK_ENABLE();

    htim12.Instance = TIM12;
    htim12.Init.Prescaler = TIM_PSC_APB1;
    htim12.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim12.Init.Period = PWM_RESOLUTION-1;
    htim12.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim12.Init.RepetitionCounter = 0;
    if (HAL_TIM_PWM_Init(&htim12) != HAL_OK)
    {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim12, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = PWM_DEFAULT_DUTY;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(&htim12, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_TIM_PWM_ConfigChannel(&htim12, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_TIM_PWM_ConfigChannel(&htim12, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_TIM_PWM_ConfigChannel(&htim12, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
    {
        Error_Handler();
    }

    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(&htim12, &sBreakDeadTimeConfig) != HAL_OK)
    {
        Error_Handler();
    }

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF9_TIM12;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

}

void LED_CTRL( uint8_t LED_number, uint8_t PinState)
{
    //PinState为1表示亮
    uint16_t GPIO_Pin = 0;
    uint8_t select = 1;
    switch (LED_number)
    {
    case 1 :
        GPIO_Pin = GPIO_PIN_1;
        break;
    case 2 :
        GPIO_Pin = GPIO_PIN_2;
        break;
    case 3 :
        GPIO_Pin = GPIO_PIN_3;
        break;
    case 4 :
        GPIO_Pin = GPIO_PIN_4;
        break;
    case 5 :
        GPIO_Pin = GPIO_PIN_5;
        break;
    case 6 :
        GPIO_Pin = GPIO_PIN_6;
        break;
    case 7 :
        GPIO_Pin = GPIO_PIN_7;
        break;
    case 8 :
        GPIO_Pin = GPIO_PIN_8;
        break;
    default :
        select = 0;
        break;
    }
    if(select==1)
    {
        if(PinState==1)
            HAL_GPIO_WritePin(GPIOG, GPIO_Pin, GPIO_PIN_RESET);
        else
            HAL_GPIO_WritePin(GPIOG, GPIO_Pin, GPIO_PIN_SET);
    }
    else {}
}

void BEEP_Set(uint8_t beep_state,uint32_t frequency)
{
    if(beep_state==1)
    {
        //TIM12挂载在APB1上，频率84M，若prescaler = 84，period = 1000，则为1000Hz
        htim12.Instance->PSC = 84;
        htim12.Instance->ARR = 1000000/frequency-1;
        htim12.Instance->CCR1 = (htim12.Instance->ARR*0.5) - 1;
    }
    else
    {
        htim12.Instance->CCR1 = 0;
    }
}

void monitor_remote_process(Monitor_t *Monitor)//100hz
{
    if(Monitor->circle_number<0xffff)
        Monitor->circle_number++;
    if(Monitor->circle_number > 30)
    {
        Monitor->status = monitor_error;
        RC_CtrlData.RCDataParaInit(&RC_CtrlData);
    }
    else
        Monitor->status = monitor_regular;
}

void monitor_dji_process(Monitor_t *Monitor)//100hz
{
    if(Monitor->circle_number<0xffff)
        Monitor->circle_number++;
    if(Monitor->circle_number > 30)
    {
        Monitor->status = monitor_error;
    }
    else
        Monitor->status = monitor_regular;
}


void monitor_tx2_process(Monitor_t *Monitor)//100hz
{
    if(Monitor->circle_number<0xffff)
        Monitor->circle_number++;
    if(Monitor->circle_number > 100)
    {
        Monitor->status = monitor_error;
        TX2_Data.velocity=0;
        TX2_Data.pitch_angle=0;
        TX2_Data.yaw_angle=0;
        TX2_Data.mode_now=OFFLINE;
    }
    else
        Monitor->status = monitor_regular;
}

void monitor_tx2_valid_process(Monitor_t *Monitor)//100hz
{
    if(TX2_Data.mode_now==OFFLINE)
        return;
    if(Monitor->circle_number<0xffff)
        Monitor->circle_number++;
    if(Monitor->circle_number > 100)
    {
        Monitor->status = monitor_error;
        TX2_Data.velocity=0;
        TX2_Data.pitch_angle=0;
        TX2_Data.yaw_angle=0;
        TX2_Data.mode_now=COMMU_FAILURE;
    }
    else
        Monitor->status = monitor_regular;
}

void monitor_can_power_process(Monitor_t *Monitor)//100hz
{
    Monitor->circle_number++;
    if(Monitor->circle_number > 200)
    {
        Monitor->status = monitor_error;
        Chassis.supercapacitor.sup_c_open=0;
        RoboData.chassis_ctrl.sup_s_status = 0;
        RoboData.chassis_ctrl.chassis_order = Order_stop;
        RoboData.chassis_ctrl.chassis_working_status = WorkingStatus_error;
    }
    else
        Monitor->status = monitor_regular;
}

void MonitorParaInit(Monitor_t *Monitor)
{
    Monitor->MonitorParaInit = &MonitorParaInit;
    /*--判断监视对象--*/
    if(Monitor == &monitor_remote)
        Monitor->monitor_process = &monitor_remote_process;
    else if(Monitor == &monitor_tx2)
        Monitor->monitor_process = &monitor_tx2_process;
    else if(Monitor == &monitor_can_power)
        Monitor->monitor_process = &monitor_can_power_process;

}

void Monitor_Motor(void)
{
    uint8_t i;
    for(i=0; i<4; i++)
    {
        monitor_motor[i].circle_number++;
        if(monitor_motor[i].circle_number > 100)
        {
            monitor_motor[i].status = monitor_error;
        }
        else
            monitor_motor[i].status = monitor_regular;
    }
}

void Monitor_LED_Display(void)
{
    static uint32_t counter;
    uint8_t green_code=0;
    uint8_t red_code=0;
    static uint8_t fatal_error;
    uint8_t blink1=0;
    uint8_t i;

    if(TX2_Data.mode_now==OFFLINE)
        green_code=2;
    else if(TX2_Data.mode_now==COMMU_FAILURE)
        green_code=1;

    if(monitor_dji.status==monitor_error)
        blink1=1;



    if(Chassis.supercapacitor.sup_c_open==0)
        red_code=2;

    if(monitor_remote.status==monitor_error)
        red_code=1;

    if(counter<10000)
        counter++;
    else
        counter=0;

    if(green_code==0)
        HAL_GPIO_WritePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin,GPIO_PIN_RESET);
    else if(green_code==1)
    {
        if(counter%100<10)
            HAL_GPIO_WritePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin,GPIO_PIN_RESET);
        else
            HAL_GPIO_WritePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin,GPIO_PIN_SET);
    }
    else if(green_code==2)
    {
        if((counter%100)<10||((counter%100)<30&&(counter%100)>20))
        {
            HAL_GPIO_WritePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin,GPIO_PIN_RESET);
            if((counter%100)>20)
                BEEP_Set(1,1000);
            else
                BEEP_Set(1,1300);
        }
        else if((counter%100)<20||(counter%100)>30)
        {
            HAL_GPIO_WritePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin,GPIO_PIN_SET);
            BEEP_Set(0,1500);
        }

    }

    if(green_code+red_code+fatal_error==0)
        BEEP_Set(0,1000);



    if(red_code==0)
        HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_RESET);
    else if(red_code==1)
    {
        if(counter%100<10)
        {
            HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_RESET);
            BEEP_Set(1,1500);
        }
        else
        {
            HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_SET);
            BEEP_Set(0,1000);
        }
    }
    else if(red_code==2)
    {
        if(((counter%100)<60&&counter%100>50)||(((counter%100)<80)&&counter%100>70))
        {
            HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_RESET);
            if((counter%100)>70)
                BEEP_Set(1,1200);
            else
                BEEP_Set(1,1000);
        }
        else if(((counter%100)>60&&(counter%100)<70)||(counter%100)>80)
        {
            HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_SET);
            BEEP_Set(0,1000);
        }
    }

    fatal_error=0;
    for(i=0; i<4; i++)
    {
        if(monitor_motor[i].status==monitor_error)
        {
            LED_CTRL(i+1,1);
            fatal_error=1;
        }
        else
            LED_CTRL(i+1,0);
    }

    if(fatal_error==1)
    {
        if(counter%20<5)
        {
            HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_RESET);
            BEEP_Set(1,1000);
        }
        else
        {
            HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_SET);
            BEEP_Set(0,1000);
        }
    }

    if(blink1==1)
    {
        if(((counter%100)<60&&counter%100>50)||(((counter%100)<80)&&counter%100>70))
        {
            LED_CTRL(8,1);
            if((counter%100)>50)
                BEEP_Set(1,1200);
            else
                BEEP_Set(1,1000);
        }
        else if(((counter%100)>60&&(counter%100)<70)||(counter%100)>80)
        {
            LED_CTRL(8,0);
            BEEP_Set(0,1000);
        }
    }
    else
        LED_CTRL(8,0);


    if(HAL_GPIO_ReadPin(KEY_GPIO_Port,KEY_Pin)==1)
        beep_off=1;

    if(beep_off==1)
        BEEP_Set(0,1000);


}

