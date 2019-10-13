/*******************************************************************************************
  * @Include @Headfile
 *******************************************************************************************/

#include "pid.h"
/*******************************************************************************************
  * @Parameter @Statement
 *******************************************************************************************/

Common_Pid_t power_pid[2];
/*******************************************************************************************
  * @Func		ABSLimit
  * @Brief 	��Χ����
  * @Param	float *a, float abs_max
  * @Retval	None
  * @Date		2018.10.9
  *	@Author	LZK
 *******************************************************************************************/
void ABSLimit(float *a, float abs_max){
	if(*a > abs_max)
		*a=abs_max;
	if(*a < -abs_max)
		*a=-abs_max;
}


void CommonPidSet(Common_Pid_t *pid, uint32_t pid_mode,
										float max_output, float integral_uplimit, float integral_downlimit,
										float p, float i, float d)
{
	pid->pid_mode=pid_mode;
	pid->max_output=ABS(max_output);
	pid->integral_uplimit=ABS(integral_uplimit);
	pid->integral_downlimit=ABS(integral_downlimit);
	pid->p=p;
	pid->i=i;
	pid->d=d;
}
 

void PidSet_all(Pid_t *pid, uint32_t mode, 
								 float max_out, float integral_uplimit, float integral_downlimit, 
	               float p, float i, float d, 
								 float deadband, float f, float max_err, float mid_err, float min_err, float f_out_limit)
{
	pid->pid_mode=mode;
	pid->max_output=ABS(max_out);
	pid->integral_uplimit=ABS(integral_uplimit);
	pid->integral_downlimit=-ABS(integral_downlimit);									 
	pid->p=p;
	pid->i=i;
	pid->d=d;
	pid->deadband=ABS(deadband);
	pid->f=f;
  pid->max_err=ABS(max_err);
	pid->mid_err=ABS(mid_err);
	pid->min_err=ABS(min_err);
	pid->f_out_limit=ABS(f_out_limit);
}

void PidSet_pidf(Pid_t *pid, float p, float i, float d, float f)
{							 
	pid->p=p;
	pid->i=i;
	pid->d=d;
	pid->f=f;
}

void PidSet_errband(Pid_t *pid, float max_err, float mid_err, float min_err, float deadband)
{
  pid->max_err=ABS(max_err);
	pid->mid_err=ABS(mid_err);
	pid->min_err=ABS(min_err);
	pid->deadband=ABS(deadband);

}

void PidSet_limit(Pid_t *pid, float max_out, float integral_uplimit, float integral_downlimit, float f_out_limit)
{
	pid->max_output=ABS(max_out);
	pid->integral_uplimit=ABS(integral_uplimit);
	pid->integral_downlimit=-ABS(integral_downlimit);			
	pid->f_out_limit=ABS(f_out_limit);	
}

/*******************************************************************************************
  * @Func		PIDCalc
  * @Brief 	PID��������
  * @Param	Pid_t* pid, float get, float set
  * @Retval	pid_output.pos_out
 *******************************************************************************************/
float CommonPidCalc(Common_Pid_t *pid, float get, float set)
{
	pid->get=get;
	pid->set=set;
	pid->err[0]=set-get;
	
	if(pid->pid_mode==POSITION_PID)  /*��ɢ��λ��ʽPID*/
	{
		pid->pout=pid->p * pid->err[0];
		pid->iout +=pid->i * pid->err[0];
		pid->dout=pid->d * (pid->err[0] - pid->err[1]);
		
		/*���ֿ�����*/
		if(pid->integral_uplimit>0 && pid->integral_downlimit<0)
		{
			if(pid->iout > pid->integral_uplimit)
				pid->iout=pid->integral_uplimit;
			else if(pid->iout < pid->integral_downlimit)
				pid->iout=pid->integral_downlimit;
		}
		else pid->iout=0;
		
		pid->pos_out=pid->pout + pid->iout + pid->dout;
		ABSLimit(&(pid->pos_out), pid->max_output);
	}
	pid->err[2]=pid->err[1];
	pid->err[1]=pid->err[0];	
	
	return pid->pos_out;
}






float PidCalc(Pid_t* pid, float get, float set)
{
	float i_max_err,i_mid_err,i_min_err,abs_err;
	i_max_err=ABS(pid->max_err);
	i_mid_err=ABS(pid->mid_err);
	i_min_err=ABS(pid->min_err);
	abs_err=ABS(pid->err.now);
	
	pid->set=set;
	pid->get.now=get;
	pid->err.now=set - get;											/*set - measure*/  
	if(abs_err < pid->deadband)		
		return pid->pid_mode==POSITION_PID ? pid->output.pos_out : pid->output.delta_out;	  //pid����
	
	if(pid->pid_mode==POSITION_PID)  /*��ɢ��λ��ʽPID*/
	{ 			

		
		/*pout*/
		pid->output.pout=pid->p * pid->err.now;
		/*���ַ���*/
		if(pid->max_err > pid->mid_err && pid->mid_err > pid->min_err)
		{				
			if(abs_err > i_max_err)  {  pid->output.iout +=0; }
			else if(abs_err > i_mid_err)  { pid->output.iout +=((i_max_err-abs_err)/(i_max_err-i_mid_err)+1.0f)/2.0f * pid->i * pid->err.now; }
			else if(abs_err > i_min_err)  { pid->output.iout +=pid->i * pid->err.now; }
			//else  pid->output.iout *=1; /*��������*/			
		}					
		else 	
			pid->output.iout +=pid->i * pid->err.now;
		if(pid->i==0)
			pid->output.iout=0;//��ѧ�ӣ�������������������ù�
		/*���ֿ�����*/
		if(pid->integral_uplimit>0 && pid->integral_downlimit<0)
		{
			if(pid->output.iout > pid->integral_uplimit)
				pid->output.iout=pid->integral_uplimit;
			else if(pid->output.iout < pid->integral_downlimit)
				pid->output.iout=pid->integral_downlimit;
		}
		else pid->output.iout=0;
		/*dout*/
		pid->output.dout=0.2*pid->output.dout+0.8*pid->d * (pid->err.now - pid->err.last); 
			
		pid->output.pos_out=pid->output.fout + pid->output.pout + pid->output.iout + pid->output.dout;
			ABSLimit(&(pid->output.pos_out), pid->max_output);	
			pid->output.last_pos_out=pid->output.pos_out;	
	}
	else if(pid->pid_mode==DELTA_PID)  /*����ʽPID*/
	{				
		pid->output.pout=pid->p * (pid->err.now - pid->err.last);
		pid->output.iout=pid->i * pid->err.now;
		pid->output.dout=pid->d * (pid->err.now - 2*pid->err.last + pid->err.beforelast);
		/*���ֿ�����*/
		if(pid->integral_uplimit > pid->integral_downlimit)
		{
			if(pid->output.iout > pid->integral_uplimit)
				pid->output.iout=pid->integral_uplimit;
			else if(pid->output.iout < pid->integral_downlimit)
				pid->output.iout=pid->integral_downlimit;
		}
		else pid->output.iout=0;
		pid->output.delta_u=pid->output.pout + pid->output.iout + pid->output.dout;
		pid->output.delta_out=pid->output.last_delta_out + pid->output.delta_u;
		ABSLimit(&(pid->output.delta_out), pid->max_output);
		pid->output.last_delta_out=pid->output.delta_out;
	}
	pid->err.beforelast=pid->err.last;
	pid->err.last=pid->err.now;
	pid->get.beforelast=pid->get.last;
	pid->get.last=pid->get.now;
	if(pid->i==0)
		pid->output.iout=0;
	return pid->pid_mode==POSITION_PID ? pid->output.pos_out : pid->output.delta_out;	
}



void PidParaInit(Pid_t *pid)
{
	pid->PidParaInit=&PidParaInit;
	pid->PidSet_all=&PidSet_all;
	pid->PidSet_errband=&PidSet_errband;
	pid->PidSet_limit=&PidSet_limit;
	pid->PidSet_pidf=&PidSet_pidf;
	pid->PidCalc=&PidCalc; 
	pid->pid_mode=POSITION_PID;

}


