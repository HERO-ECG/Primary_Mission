#include "dji_Protocol.h"
#include "tx2_Protocol.h"
#include "robodata.h"
#include "m_remote.h"
#include "strike.h"
#include "t_monitor.h"
#include "tripod.h"

uint8_t windcar_key = 0;
RoboData_t RoboData={&InitRoboData};//�������û�������


/*function�����ÿ���ģʽ*******************************
*parameter��Remote_t�࣬RoboControlMode_t��
*/
void SetRoboControlMode(Remote_t rc, RoboControlMode_t *rcmode)
{
	//��
	switch(rc.s1){
		case 1:
			rcmode->left_mode=LEFT_up;break;
		case 2:
			rcmode->left_mode=LEFT_down;break;
		case 3:
			rcmode->left_mode=LEFT_mid;break;
		default: 
			rcmode->left_mode=LEFT_mid;break;
	}
	//��
	switch(rc.s2){
		case 1:
			rcmode->ctrl_source=FROM_PC;break;
		case 2:
			rcmode->ctrl_source=RIGHT_reserve;break;
		case 3:
			rcmode->ctrl_source=FROM_REMOTE;break;
		default: 
			rcmode->ctrl_source=FROM_REMOTE;break;
	}
	
}

/*function�����õ��跢�����*******************************
*parameter��RC_Ctl_t�࣬RoboControlMode_t�࣬AmmunitionControl_t��
*/
void GenerallySetShootControl(RC_Ctl_t rc_data, RoboControlMode_t robo_ctrlmode, AmmunitionControl_t *AmmunitionControl)
{
	if(robo_ctrlmode.ctrl_source==FROM_PC)
	{
//		if(rc_data.mouse.press_1==1) {	AmmunitionControl->shoot17_order=Order_start1; }
//		else { AmmunitionControl->shoot17_order=Order_stop; }
	}
	else //if(RoboData.robo_ctrlmode.ctrl_source==FROM_REMOTE)
	{
//		if(robo_ctrlmode.ctrl_source==RIGHT_reserve) { AmmunitionControl->shoot17_order=Order_start1; }
//		else {	AmmunitionControl->shoot17_order=Order_stop; }
	}

	
}

/*function�����õ����˶�����*******************************
*parameter��RC_Ctl_t�࣬RoboControlMode_t�࣬ChassisControl_t��

*/
void GenerallySetChassisSpeed(RC_Ctl_t rc_data, RoboControlMode_t robo_ctrlmode, ChassisControl_t *chassis_ctrl)
{
	if(robo_ctrlmode.ctrl_source==FROM_PC)
	{
		chassis_ctrl->spd_forward=(rc_data.key.key_data.W - rc_data.key.key_data.S);
		chassis_ctrl->spd_right=(rc_data.key.key_data.D - rc_data.key.key_data.A);
//		chassis_ctrl->spd_yaw_w=(spd_q - spd_e);

	}
	else //if(RoboData.robo_ctrlmode.ctrl_source==FROM_REMOTE)
	{
		if(rc_data.rc.ch3 !=0) { chassis_ctrl->spd_forward=(rc_data.rc.ch3-1024.0)/660.0; }
		else { chassis_ctrl->spd_forward=0; }

		if(rc_data.rc.ch2 !=0) { chassis_ctrl->spd_right=(rc_data.rc.ch2-1024.0)/660.0; }
		else { chassis_ctrl->spd_right=0; }	
	}
	
	
}

/*function������״̬������*******************************
*parameter��RoboData_t��
*/
void GetChassisControlStatus(RoboData_t *RoboData)
{
	if(monitor_can_power.status==monitor_regular)//�������ݹ��ʰ淴������
	{
		if(RoboData->robo_ctrlmode.ctrl_source==FROM_PC)
		{
			/*�������Ƴ������ݿ���*/
			if(RC_CtrlData.key.key_data.V==1)
				RoboData->chassis_ctrl.chassis_order=Order_startN;//׼����ʱ�俪����������
			else
				RoboData->chassis_ctrl.chassis_order=Order_stop;
		}
		else if(RoboData->robo_ctrlmode.ctrl_source==RIGHT_reserve)
		{
			/*���˿��Ƴ������ݿ���*/
			if(RoboData->robo_ctrlmode.left_mode==LEFT_up )
						RoboData->chassis_ctrl.chassis_order=Order_startN;//׼����ʱ�俪����������
			else
				RoboData->chassis_ctrl.chassis_order=Order_stop;
		}
		else //if(RoboData->robo_ctrlmode.ctrl_source==FROM_REMOTE)	
		{	
			/*��ң����ģʽ��������������*/
			RoboData->chassis_ctrl.chassis_order=Order_stop;
		}
	}
	else
	{ //�������ݹ��ʰ淴��������
		RoboData->chassis_ctrl.chassis_order=Order_stop;
		RoboData->chassis_ctrl.chassis_working_status=WorkingStatus_error;
	}
}


/*function��������̨�˶�����*******************************
*parameter��RC_Ctl_t�࣬RoboControlMode_t�࣬TripodControl_t��
*/
void GenerallySetTripodControl(RC_Ctl_t rc_data, RoboControlMode_t robo_ctrlmode, TripodControl_t *tripod_ctrl)	//��λΪ��
{	
//	if(robo_ctrlmo.ctrl_source==FROM_REMOTE)
//	{

			if(rc_data.rc.ch1 !=0) { tripod_ctrl->pitch_angle=(rc_data.rc.ch1-1024.0)/660.0f*25; }
			else { tripod_ctrl->pitch_angle=0; }	
			if(rc_data.rc.ch0 !=0) { tripod_ctrl->yaw_angle=-(rc_data.rc.ch0-1024.0)/660.0f*60; }
			else { tripod_ctrl->yaw_angle=0; }
			tripod_ctrl->pitch_angle_mix=tripod_ctrl->pitch_angle;
			tripod_ctrl->yaw_angle_mix=tripod_ctrl->yaw_angle;

}



/*function��ң�������ݴ�����*******************************
*parameter��RoboData_t��
*/
void GetRemoteControlData(RoboData_t *RoboData)
{
	RCReadKey(&RC_CtrlData);//��ȡ��������
	SetRoboControlMode(RC_CtrlData.rc, &RoboData->robo_ctrlmode);//���ÿ���ģʽ
	GenerallySetChassisSpeed(RC_CtrlData, RoboData->robo_ctrlmode, &RoboData->chassis_ctrl);//���õ����˶�����
	GenerallySetTripodControl(RC_CtrlData, RoboData->robo_ctrlmode, &RoboData->tripod_ctrl);//������̨�˶�����
	GenerallySetShootControl(RC_CtrlData, RoboData->robo_ctrlmode, &RoboData->AmmunitionControl);//���õ��跢�����
}




/*function������ϵͳ���ݴ�����*******************************
*parameter��RoboData_t��
*/
void GetDJIControlData(RoboData_t *RoboData)//100hz
{
	
//	switch(DJI_ReadData.extGameRobotState.robotLevel)
//	{
//		case 1:
//		{	RoboData->shoot_heat_ctr.limitHeat17=240;
//			RoboData->shoot_heat_ctr.coolingHeat17=0.4;
//			RoboData->shoot_heat_ctr.limitHeat42=150;
//			RoboData->shoot_heat_ctr.coolingHeat42=0.2;
//		} break;
//		case 2:
//		{	RoboData->shoot_heat_ctr.limitHeat17=360;
//			RoboData->shoot_heat_ctr.coolingHeat17=0.6;
//			RoboData->shoot_heat_ctr.limitHeat42=250;
//			RoboData->shoot_heat_ctr.coolingHeat42=0.4;
//		}	break;
//		case 3:
//		{	RoboData->shoot_heat_ctr.limitHeat17=480;
//			RoboData->shoot_heat_ctr.coolingHeat17=0.8;
//			RoboData->shoot_heat_ctr.limitHeat42=400;
//			RoboData->shoot_heat_ctr.coolingHeat42=0.6;
//		}	break;
//		default://���ز���
//		{	RoboData->shoot_heat_ctr.limitHeat17=240;
//			RoboData->shoot_heat_ctr.coolingHeat17=0.4;
//		  RoboData->shoot_heat_ctr.limitHeat42=80;
//			RoboData->shoot_heat_ctr.coolingHeat42=0.2;
//		}	break;
//	}
//	
//	switch((int)(TX2_Data.delta_distance))
//	{
//		case 1://regular
//			RoboData->shoot_heat_ctr.addHeat17=friction17_spd_regular*2;
//			RoboData->AmmunitionControl.Toggler17_angle=Toggler17_angle_two;
//			RoboData->AmmunitionControl.Friction17_speed=friction17_spd_regular;
//			break;
//		case 2://middle
//			RoboData->shoot_heat_ctr.addHeat17=friction17_spd_middle*3;
//			RoboData->AmmunitionControl.Toggler17_angle=Toggler17_angle_three;
//			RoboData->AmmunitionControl.Friction17_speed=friction17_spd_middle;
//			break;
//		case 3://near
//			RoboData->shoot_heat_ctr.addHeat17=friction17_spd_near*4;
//			RoboData->AmmunitionControl.Toggler17_angle=Toggler17_angle_four;
//			RoboData->AmmunitionControl.Friction17_speed=friction17_spd_near;
//			break;
//		case 4://far
//			RoboData->shoot_heat_ctr.addHeat17=friction17_spd_far*1;
//			RoboData->AmmunitionControl.Toggler17_angle=Toggler17_angle_one;
//			RoboData->AmmunitionControl.Friction17_speed=friction17_spd_far;
//			break;
//		default:
//			RoboData->shoot_heat_ctr.addHeat17=friction17_spd_regular*1;
//			RoboData->AmmunitionControl.Toggler17_angle=Toggler17_angle_one;
//			RoboData->AmmunitionControl.Friction17_speed=friction17_spd_regular;
//			break;
//	}
//	if(RoboData->shoot_heat_ctr.allow17num < 4)//��������ʱ����
//	{
//		RoboData->shoot_heat_ctr.addHeat17=RoboData->AmmunitionControl.Friction17_speed*1;
////		RoboData->AmmunitionControl.Toggler17_angle=Toggler17_angle_one;
//	}
	
//	
//	RoboData->shoot_heat_ctr.addHeat42=100;
////	RoboData->AmmunitionControl.Toggler42_angle=Toggler42_angle_one;
//	if(DJI_ReadData.extGameRobotState.remainHP*5 < DJI_ReadData.extGameRobotState.maxHP)
//		RoboData->shoot_heat_ctr.coolingHeat17 *=2;
	
	/*����buff�ɱ����������½��*/
}


/*function������17mmǹ������������*******************************
*parameter��RoboData_t��
*/
void Get17HeatControlData(RoboData_t *RoboData)//100hz
{
	RoboData->shoot_heat_ctr.coolingWaitTime17++;
	if(RoboData->shoot_heat_ctr.coolingWaitTime17 > 9)//ms;10hz heatCooling
	{
		RoboData->shoot_heat_ctr.coolingWaitTime17=0;
		if(RoboData->shoot_heat_ctr.nowHeat17>0)
			RoboData->shoot_heat_ctr.nowHeat17 -=RoboData->shoot_heat_ctr.coolingHeat17*10;
		else
			RoboData->shoot_heat_ctr.nowHeat17=0;
	}
//	
//	if(RoboData->shoot_heat_ctr.nowHeat17 > RoboData->shoot_heat_ctr.limitHeat17)
//		RoboData->shoot_heat_ctr.allow17num=0;
//	else
//		RoboData->shoot_heat_ctr.allow17num=(RoboData->shoot_heat_ctr.limitHeat17 - RoboData->shoot_heat_ctr.nowHeat17)/RoboData->AmmunitionControl.Friction17_speed;
}


/*function������42mmǹ������������*******************************
*parameter��RoboData_t��
*/
void Get42HeatControlData(RoboData_t *RoboData)//100hz
{
	RoboData->shoot_heat_ctr.coolingWaitTime42++;
	if(RoboData->shoot_heat_ctr.coolingWaitTime42 > 9)//ms;100hz heatCooling
	{
		RoboData->shoot_heat_ctr.coolingWaitTime42=0;
		if(RoboData->shoot_heat_ctr.nowHeat42>0)
			RoboData->shoot_heat_ctr.nowHeat42 -=RoboData->shoot_heat_ctr.coolingHeat42*10;
		else
			RoboData->shoot_heat_ctr.nowHeat42=0;
	}

	if(RoboData->shoot_heat_ctr.nowHeat42 > RoboData->shoot_heat_ctr.limitHeat42)
		RoboData->shoot_heat_ctr.allow42num=0;
	else
		RoboData->shoot_heat_ctr.allow42num=(RoboData->shoot_heat_ctr.limitHeat42 - RoboData->shoot_heat_ctr.nowHeat42)/RoboData->shoot_heat_ctr.addHeat42;
}


/*function��17mm����״̬������*******************************
*parameter��RoboData_t�࣬Moto_t��
*/
//void GetAmmunition17ControlStatus(RoboData_t *RoboData, Moto_t *TG_MOTO)//1000hz
//{
//	/*---------����æµ״̬---------*/
//	if(RoboData->AmmunitionControl.shoot17_working_status==WorkingStatus_busy)
//	{		
//		RoboData->AmmunitionControl.shoot17_busy_time ++;
//		RoboData->AmmunitionControl.shoot17_error_time=0;
//		if(RoboData->AmmunitionControl.shoot17_busy_time < 500)//ms
//		{
//			if((ABS(TG_MOTO->pid_angle.err.now) < 10)&&(ABS(TG_MOTO->pid_speed.err.now) < 10))
//			{
//				//RoboData->shoot_heat_ctr.nowHeat17 +=RoboData->shoot_heat_ctr.addHeat17;//17mm��������
//				//RoboData->AmmunitionControl.shoot17_order=Order_stop;
//				RoboData->AmmunitionControl.shoot17_working_status=WorkingStatus_temp;
//			}
//			else
//				RoboData->AmmunitionControl.shoot17_working_status=WorkingStatus_busy;
//		}
//		else 
//		{
//			RoboData->AmmunitionControl.shoot17_busy_time=0;
//			RoboData->AmmunitionControl.shoot17_working_status=WorkingStatus_error;
//		}
//	}
//	/*---------�����ȴ�״̬---------*/
//	else if(RoboData->AmmunitionControl.shoot17_working_status==WorkingStatus_ready)
//	{
//		RoboData->AmmunitionControl.shoot17_busy_time=0;
//		RoboData->AmmunitionControl.shoot17_error_time=0;
//		if((RoboData->AmmunitionControl.shoot17_order==Order_start1)
//			&&((RoboData->shoot_heat_ctr.nowHeat17 + RoboData->shoot_heat_ctr.addHeat17) < RoboData->shoot_heat_ctr.limitHeat17))
//		{			
//			RoboData->shoot_heat_ctr.nowHeat17 +=RoboData->shoot_heat_ctr.addHeat17;//17mm����Ԥ����
//			TG_MOTO->getpara.round_cnt -=(RoboData->AmmunitionControl.Toggler17_angle/360.0f); //17mm�����Ƕ�����
//			TG_MOTO->getpara.total_round=TG_MOTO->getpara.round_cnt + (float)(TG_MOTO->getpara.angle)/8192.0f;
//			//RoboData->AmmunitionControl.shoot17_order=Order_busy;
//			RoboData->AmmunitionControl.shoot17_working_status=WorkingStatus_busy;
//		}
//		else
//		{//regular
//			RoboData->shoot_heat_ctr.addHeat17=friction17_spd_regular*2;
//			RoboData->AmmunitionControl.Toggler17_angle=Toggler17_angle_two;
//			RoboData->AmmunitionControl.Friction17_speed=friction17_spd_regular;
//		}
//	}
//	/*---------��������״̬---------*/
//	else if(RoboData->AmmunitionControl.shoot17_working_status==WorkingStatus_error)
//	{
//		RoboData->AmmunitionControl.shoot17_busy_time=0;
//		RoboData->AmmunitionControl.shoot17_error_time ++;
//		if(RoboData->AmmunitionControl.shoot17_error_time < 50)//ms
//		{
//			//���������
//		}
//		else
//		{
//			RoboData->AmmunitionControl.shoot17_error_time=0;
//			RoboData->AmmunitionControl.shoot17_working_status=WorkingStatus_temp;			
//		}
//	}
//	else if(RoboData->AmmunitionControl.shoot17_working_status==WorkingStatus_temp)
//	{
//		RoboData->AmmunitionControl.shoot17_busy_time=0;
//		RoboData->AmmunitionControl.shoot17_error_time=0;
//		if(RoboData->AmmunitionControl.shoot17_order==Order_start1)
//		{//near
//			RoboData->shoot_heat_ctr.addHeat17=friction17_spd_near*4;
//			RoboData->AmmunitionControl.Toggler17_angle=Toggler17_angle_four;
//			RoboData->AmmunitionControl.Friction17_speed=friction17_spd_near;
//		}
//		else
//		{//regular
//			RoboData->shoot_heat_ctr.addHeat17=friction17_spd_regular*2;
//			RoboData->AmmunitionControl.Toggler17_angle=Toggler17_angle_two;
//			RoboData->AmmunitionControl.Friction17_speed=friction17_spd_regular;
//		}
//		RoboData->AmmunitionControl.shoot17_working_status=WorkingStatus_ready;			

//	}
//}


/*function��42mm����״̬������*******************************
*parameter��RoboData_t�࣬Moto_t��
*/
void GetAmmunition42ControlStatus(RoboData_t *RoboData, Moto_t *TG_MOTO)//1000hz
{
	/*---------����æµ״̬---------*/
	if(RoboData->AmmunitionControl.shoot42_working_status==WorkingStatus_busy)
	{		
		RoboData->AmmunitionControl.shoot42_busy_time ++;
		RoboData->AmmunitionControl.shoot42_error_time=0;
		if(RoboData->AmmunitionControl.shoot42_busy_time < 500)//ms
		{
			if((ABS(TG_MOTO->pid_angle.err.now) < 10)&&(ABS(TG_MOTO->pid_speed.err.now) < 10))
			{
				//RoboData->shoot_heat_ctr.nowHeat42 +=RoboData->shoot_heat_ctr.addHeat42;//42mm��������
				//RoboData->AmmunitionControl.shoot42_order=Order_stop;
				RoboData->AmmunitionControl.shoot42_working_status=WorkingStatus_ready;
			}
			else
				RoboData->AmmunitionControl.shoot42_working_status=WorkingStatus_busy;
		}
		else 
		{
			RoboData->AmmunitionControl.shoot42_busy_time=0;
			RoboData->AmmunitionControl.shoot42_working_status=WorkingStatus_error;
		}
	}
	/*---------�����ȴ�״̬---------*/
	else if(RoboData->AmmunitionControl.shoot42_working_status==WorkingStatus_ready)
	{
		RoboData->AmmunitionControl.shoot42_busy_time=0;
		RoboData->AmmunitionControl.shoot42_error_time=0;
		if((RoboData->AmmunitionControl.shoot42_order==Order_start1)
			&&((RoboData->shoot_heat_ctr.nowHeat42 + RoboData->shoot_heat_ctr.addHeat42) < RoboData->shoot_heat_ctr.limitHeat42))
		{	
			RoboData->shoot_heat_ctr.nowHeat42 +=RoboData->shoot_heat_ctr.addHeat42;//42mm����Ԥ����
			TG_MOTO->getpara.round_cnt -=(RoboData->AmmunitionControl.Toggler42_angle/360.0f); //42mm�����Ƕ�����
			TG_MOTO->getpara.total_round=TG_MOTO->getpara.round_cnt + (float)(TG_MOTO->getpara.angle)/8192.0f;
			//RoboData->AmmunitionControl.shoot42_order=Order_busy;
			RoboData->AmmunitionControl.shoot42_working_status=WorkingStatus_busy;
		}
	}
	/*---------��������״̬---------*/
	else if(RoboData->AmmunitionControl.shoot42_working_status==WorkingStatus_error)
	{
		RoboData->AmmunitionControl.shoot42_busy_time=0;
		RoboData->AmmunitionControl.shoot42_error_time ++;
		if(RoboData->AmmunitionControl.shoot42_error_time < 250)//ms
		{
			//���������
		}
		else
		{
			RoboData->AmmunitionControl.shoot42_error_time=0;
			RoboData->AmmunitionControl.shoot42_working_status=WorkingStatus_ready;			
		}
	}
	
}

/*function����ʼ��������������RoboData�ڵ�����*******************************
*parameter��RoboData_t��
*/
void InitRoboData(RoboData_t *RoboData)
{

	RC_CtrlData.RCDataParaInit=&RCDataParaInit;
		RC_CtrlData.RCDataParaInit(&RC_CtrlData);

	
	monitor_remote.MonitorParaInit=&MonitorParaInit;
		monitor_remote.MonitorParaInit(&monitor_remote);
	monitor_tx2.MonitorParaInit=&MonitorParaInit;
		monitor_tx2.MonitorParaInit(&monitor_tx2);
	monitor_can_power.MonitorParaInit=&MonitorParaInit;
		monitor_can_power.MonitorParaInit(&monitor_can_power);
	

	
	
	RoboData->Get17HeatControlData=&Get17HeatControlData;
	RoboData->Get42HeatControlData=&Get42HeatControlData;
//	RoboData->GetAmmunition17ControlStatus=&GetAmmunition17ControlStatus;
	RoboData->GetAmmunition42ControlStatus=&GetAmmunition42ControlStatus;
	RoboData->GetDJIControlData=&GetDJIControlData;
	RoboData->GetRemoteControlData=&GetRemoteControlData;
	RoboData->GetChassisControlStatus=&GetChassisControlStatus;

	RoboData->robo_ctrlmode.left_mode=LEFT_mid;//�󲦸�ģʽ
	RoboData->robo_ctrlmode.ctrl_source=FROM_REMOTE;//������ԴģʽĬ��Ϊ����ң����
	RoboData->AmmunitionControl.shoot17_order=Order_stop;//���跢������Ĭ��Ϊ������
	RoboData->AmmunitionControl.shoot42_order=Order_stop;//���跢������Ĭ��Ϊ������
	RoboData->AmmunitionControl.shoot17_working_status=WorkingStatus_ready;
	RoboData->AmmunitionControl.shoot42_working_status=WorkingStatus_ready;

	RoboData->chassis_ctrl.chassis_order=Order_stop;
	RoboData->chassis_ctrl.chassis_working_status=WorkingStatus_temp;//׼���رճ�������


}

