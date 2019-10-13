//#include "t_moto.h"
//#include "m_imu.h"
//#include "control.h"
//#include "t_monitor.h"

////TW_servo.PID_angle.PidSet_all(&TW_servo.PID_angle, POSITION_PID, 5000, 0, 0, 80, 0, 40, 0, 0, 0, 0, 0, 0, 0);

////Pid_t PID_ChassissFollow;
//#define ABS(X) ((X>0)? (X): (-X))

//#define deadband	6
//float ChassissFollow(float angle_now,float angle_set)
//{
//	float angle_err;
//	while(angle_set<0)
//		angle_set+=360;
//	while(angle_set>360)
//		angle_set-=360;
//	if(angle_now<angle_set)
//	{
//		if(angle_set-angle_now<180)
//			angle_err=angle_set-angle_now;
//		else
//			angle_err=(angle_set-angle_now)-360;
//	}
//	else
//	{
//		if(angle_now-angle_set<180)
//			angle_err=angle_set-angle_now;
//		else
//			angle_err=360-(angle_now-angle_set);
//	}
//	if(ABS(angle_err)<deadband)
//		angle_err=0;
//	else if(angle_err>deadband)
//		angle_err-=deadband;
//	else if(angle_err<-deadband)
//		angle_err+=deadband;
//
//	angle_err/=120.0f;
//	if(angle_err>1)
//		angle_err=1;
//	if(angle_err<-1)
//		angle_err=-1;
//	return angle_err;

//}


//int __sgn(float x)
//{
//	if(x > 1) return 1;
//	else if (x < -1) return -1;
//	else return 0;
//}


///*-------------------------------Chassis--------------------------------------------*/
//void chassis_regular(void)//平地常速
//{
//	//		RoboData.chassis_ctrl.k_spd_all = 6000;
//	for(int i=0; i<4; i++)   //平地常速对应的参数
//	{
//		//Chassis_MOTO[i].pid_speed.PidSet_errband(&Chassis_MOTO[i].pid_speed, 0, 0, 0, 0);
//		Chassis_MOTO[i].pid_speed.PidSet_limit(&Chassis_MOTO[i].pid_speed, 2200+DJI_ReadData.extPowerHeatData.chassisPowerBuffer*15, 500, -500, 0);
//		Chassis_MOTO[i].pid_speed.PidSet_pidf(&Chassis_MOTO[i].pid_speed, 5, 0.1, 0, 0);
//	}
//}

//void set_cm_speed(float theta)
//{
//	#ifndef ChassisPowerControl
//	RoboData.chassis_ctrl.k_spd_all = 8000;
//	#endif
//	//theta>0为右转
//	if(monitor_remote.status == monitor_regular)
//	{
//		float ksin=0,kcos=0,W_sin=0,W_cos=0,D_sin=0,D_cos=0,cm_set_spd[5]={0,0,0,0,0};
//		ksin = (float)sin(theta*3.1416f/180.0f);
//		kcos = (float)cos(theta*3.1416f/180.0f);
//		W_sin = RoboData.chassis_ctrl.spd_forward*ksin;
//		W_cos = RoboData.chassis_ctrl.spd_forward*kcos;
//		D_sin = RoboData.chassis_ctrl.spd_right*ksin;
//		D_cos = RoboData.chassis_ctrl.spd_right*kcos;
//		//速度分配矩阵
//		cm_set_spd[0] = ( (W_cos-D_sin) + (W_sin+D_cos) + RoboData.chassis_ctrl.spd_yaw_w);
//		cm_set_spd[1] = (-(W_cos-D_sin) + (W_sin+D_cos) + RoboData.chassis_ctrl.spd_yaw_w);
//		cm_set_spd[2] = ( (W_cos-D_sin) - (W_sin+D_cos) + RoboData.chassis_ctrl.spd_yaw_w);
//		cm_set_spd[3] = (-(W_cos-D_sin) - (W_sin+D_cos) + RoboData.chassis_ctrl.spd_yaw_w);
//		//底盘各电机期望速度设定
//		for(int i=0;i<4;i++)		cm_set_spd[4] += ABS(cm_set_spd[i]);
//		cm_set_spd[4] = cm_set_spd[4]>1 ? cm_set_spd[4] : 1;
//		for(int i=0;i<4;i++) Chassis_MOTO[i].set_speed = RoboData.chassis_ctrl.k_spd_all * cm_set_spd[i] / cm_set_spd[4] * 4;
//	}
//	else{
//		Chassis_MOTO[0].set_speed=0;
//		Chassis_MOTO[1].set_speed=0;
//		Chassis_MOTO[2].set_speed=0;
//		Chassis_MOTO[3].set_speed=0;
//	}
//}




//#ifdef ChassisPowerControl
//void chassis_special_speedup(void)//平地超级电容加速
//{
//			RoboData.chassis_ctrl.k_spd_all = RoboData.chassis_ctrl.k_spd_all*0.5f + 0.5f*3500;
//	for(int i=0; i<4; i++)   //超级电容加速对应的参数
//	{
//		//Chassis_MOTO[i].pid_speed.PidSet_errband(&Chassis_MOTO[i].pid_speed, 0, 0, 0, 0);
//		Chassis_MOTO[i].pid_speed.PidSet_limit(&Chassis_MOTO[i].pid_speed, 5000+DJ_ReadData.Data.extPowerHeatData.chassisPowerBuffer*15, 1000, -1000, 0);
//		Chassis_MOTO[i].pid_speed.PidSet_pidf(&Chassis_MOTO[i].pid_speed, 5, 0.12, 0, 0);
//	}
//}

//void chassis_climb(void)//爬坡
//{
//			RoboData.chassis_ctrl.k_spd_all = RoboData.chassis_ctrl.k_spd_all*0.5f + 0.5f*2000;
//	for(int i=0; i<4; i++)   //无超级电容时爬坡对应的参数
//	{
//		//Chassis_MOTO[i].pid_speed.PidSet_errband(&Chassis_MOTO[i].pid_speed, 0, 0, 0, 0);
//		Chassis_MOTO[i].pid_speed.PidSet_limit(&Chassis_MOTO[i].pid_speed, 5000+DJ_ReadData.Data.extPowerHeatData.chassisPowerBuffer*15, 1000, -1000, 0);
//		Chassis_MOTO[i].pid_speed.PidSet_pidf(&Chassis_MOTO[i].pid_speed, 5, 0.12, 0, 0);
//	}
//}

///*-------------------------------PowerControl--------------------------------------------*/
//void switch_chassis_speed_power(float limit_power)//100hz
//{
//	if(RoboData.chassis_ctrl.sup_s_status == 1)//超级电容已启动
//	{
//		chassis_special_speedup();//平地超级电容加速
//		if(RoboData.chassis_ctrl.chassis_order == Order_stop)
//		{
//			chassis_regular();//平地常速
//			TX_DATA_POWER_ID(&hcan1,1,limit_power);//使超级电容关闭
//			HAL_CAN_Transmit(&hcan1,5);
//		}
//		RoboData.chassis_ctrl.sup_s_cooling_time = 0;
//	}
//	else if(RoboData.chassis_ctrl.sup_s_status == 0)//超级电容已关闭
//	{
//		chassis_regular();//平地常速
//		if(RoboData.chassis_ctrl.chassis_order != Order_stop)
//		{
//			chassis_climb();//爬坡
//			if(RoboData.chassis_ctrl.sup_s_cooling_time++>20)
//			{
//				TX_DATA_POWER_ID(&hcan1,0,limit_power);//使超级电容开启
//				HAL_CAN_Transmit(&hcan1,5);
//			}
//		}
//		else	RoboData.chassis_ctrl.sup_s_cooling_time = 0;
//	}

//}


//void chassis_control(void)
//{
//	float power_restrict_k=1;
//	power_pid[0].CommonPidCalc(&power_pid[0], DJ_ReadData.Data.extPowerHeatData.chassisPower, mid_power);
//	//power_pid[1].CommonPidCalc(&power_pid[1], DJ_ReadData.Data.extPowerHeatData.chassisPower, mid_power);
//	power_restrict_k = 1 + (power_pid[0].pos_out>0 ? power_pid[0].pos_out : 0);
//
//	for(int i=0; i<4; i++)
//	{
//		Chassis_MOTO[i].pid_speed.PidSet_limit(&Chassis_MOTO[i].pid_speed, 2400+10*DJ_ReadData.Data.extPowerHeatData.chassisPowerBuffer, 500, -500, 0);
//		/*速度环_&_电机输出信号赋值*/
//		Chassis_MOTO[i].send_current = Chassis_MOTO[i].pid_speed.PidCalc(&Chassis_MOTO[i].pid_speed, Chassis_MOTO[i].getpara.speed_rpm, Chassis_MOTO[i].set_speed/power_restrict_k);
//	}


//}

//void cm_power_speed_ctr(void)
//{
//	static int power_restrict_sta=0;
//	static int power_restrict_k=1;
//	/*功率环*/
//	power_pid[0].CommonPidCalc(&power_pid[0], DJ_ReadData.Data.extPowerHeatData.chassisPower, mid_power);
//	power_pid[1].CommonPidCalc(&power_pid[1], DJ_ReadData.Data.extPowerHeatData.chassisPower, mid_power);

//  /*功率限制条件判断*/
//	if(DJ_ReadData.Data.extPowerHeatData.chassisPowerBuffer < (DJ_ReadData.Data.extPowerHeatData.chassisPower-mid_power)){
//	  power_restrict_sta = 1;
//	}
//	else{
//		power_restrict_sta = 0;
//	}
//
//	for(int i=0; i<4; i++)
//	{
//		/*功率限制条件判断*/
//		if((power_restrict_sta == 1) && (ABS(Chassis_MOTO[i].set_speed) > 2000))
//			power_restrict_k = 1;
//    else
//			power_restrict_k = 0;
//		/*速度环_&_电机输出信号赋值*/
//		Chassis_MOTO[i].send_current = Chassis_MOTO[i].pid_speed.PidCalc(&Chassis_MOTO[i].pid_speed, Chassis_MOTO[i].getpara.speed_rpm,
//														Chassis_MOTO[i].set_speed + power_restrict_k * __sgn(Chassis_MOTO[i].set_speed) * power_pid[0].pos_out);
//
//	}

//}

////void PowerControl(void)
////{
////	int i;
////	float power_restrict_k[3];
////	power_pid[0].PidSet_limit(&power_pid[0], 1, 0, 0, 0);
////	power_pid[1].PidSet_limit(&power_pid[1], 1, 0, 0, 0);

////	//功率环输出分离参数计算
////	if(DJ_ReadData.Data.extPowerHeatData.chassisPower < 40)
////		power_restrict_k[2] = 0;
////	else if(DJ_ReadData.Data.extPowerHeatData.chassisPower < mid_power)
////		power_restrict_k[2] = (DJ_ReadData.Data.extPowerHeatData.chassisPower-40)/(mid_power - 40);
////	else
////		power_restrict_k[2] = 1;
////	//速度环输出限幅参数计算（0,2）
////	power_restrict_k[0] = 1+power_restrict_k[2] *power_pid[0].PidCalc(&power_pid[0], DJ_ReadData.Data.extPowerHeatData.chassisPower, mid_power);
////	power_restrict_k[1] = 1+power_restrict_k[2] *power_pid[1].PidCalc(&power_pid[1], DJ_ReadData.Data.extPowerHeatData.chassisPower, mid_power);
////
////	for(i=0; i<4; i++)
////	{
////		Chassis_MOTO[i].pid_speed.PidSet_limit(&Chassis_MOTO[i].pid_speed, power_restrict_k[0]*3000+1000, 500, -500, 0);
////		/*速度环_&_电机输出信号赋值*/
////		Chassis_MOTO[i].send_current = power_restrict_k[1] * Chassis_MOTO[i].pid_speed.PidCalc(&Chassis_MOTO[i].pid_speed, Chassis_MOTO[i].getpara.speed_rpm, Chassis_MOTO[i].set_speed);
////	}

////}

//#endif






