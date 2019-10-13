#ifndef __PID_H
#define __PID_H

#include "mytype.h"
/*******************************************************************************************
  * @define
 *******************************************************************************************/
#define ABS(X) ((X>0)? (X): (-X))

enum{
	POSITION_PID = 1,
	DELTA_PID = 2
};

/*******************************************************************************************
  * @Parameter
 *******************************************************************************************/
typedef struct __Fliter_t{
	int 	i;
	float 	value[6];
	float 	sum;
}Fliter_t;

typedef struct __Get_t{
	float 	beforelast;
	float 	last;
	float 	now;
}Get_t;

typedef struct __Err_t{
	float 	beforelast;
	float 	last;
	float 	now;
}Err_t;

typedef struct __Output_t{
	float 	fout;           /*ǰ�����(�ٶȻ�����)*/
	float 	pout;						/*P���*/	
	float 	iout;						/*I���*/
	float 	dout;						/*D���*/	
	float 	pos_out;				/*����λ��ʽ���*/
	float 	last_pos_out;		/*�ϴ�λ��ʽ���*/
	float 	delta_u;				/*��������ֵ*/
	float 	delta_out;			/*��������ʽ��� = last_delta_out + delta_u*/
	float 	last_delta_out;	/*�ϴ�����ʽ���*/
}Output_t;

typedef struct __Pid_t{
	void (*PidParaInit)(struct __Pid_t *pid);
	/*��Ҫ�Զ���*/
	uint32_t 	 pid_mode;
	float 	 max_output;				/*����޷�*/
	float 	 integral_uplimit;			/*�������򿹱���*/
	float 	 integral_downlimit;		/*���ָ��򿹱���*/
	float   f_out_limit;
	float 	p;
	float 	i;
	float 	d;
	float 	f;        		/*(�ٶȻ�����)ǰ��ϵ��*/
	float 	max_err;
	float 	mid_err;
	float 	min_err;
	float 	deadband;     	/*err < deadband return*/
	
	void (*PidSet_all)(struct __Pid_t *pid, uint32_t mode, 
		              float max_out, float integral_uplimit, float integral_downlimit,
									float p, float i, float d, 
								  float f, float max_err, float mid_err, float min_err, float deadband, float f_out_limit);
	void (*PidSet_pidf)(struct __Pid_t *pid, float p, float i, float d, float f);
  void (*PidSet_errband)(struct __Pid_t *pid, float max_err, float mid_err, float min_err, float deadband);
	void (*PidSet_limit)(struct __Pid_t *pid, float max_out, float integral_uplimit, float integral_downlimit, float f_out_limit);
	float (*PidCalc)(struct __Pid_t *pid, float get, float set);
	float up_k;
	float down_k;
	/*�����Զ���*/							
	float 	set;					/*�趨ֵ*/
	Get_t 	get;					/*����ֵ*/
	Err_t 	err;					/*���ֵ*/
	Output_t 	output;			/*���ֵ*/
	
}Pid_t;

//extern Pid_t TG2006_pid_spd;
//extern Pid_t TG2006_pid_ang;
//extern Pid_t CM3508_pid_spd[4];
extern Pid_t power_pid[2];
/*******************************************************************************************
  * @Function 
 *******************************************************************************************/
extern void ABSLimit(float *a, float abs_max);
extern void PidParaInit(Pid_t *pid);
extern float PidCalc(Pid_t *pid, float get, float set);

#endif
