#ifndef _AUTO_MOVE_H
#define _AUTO_MOVE_H
#include "mytype.h"
#include "m_moto.h"
#include "t_moto.h"
#include "m_imu.h"
#include "control.h"
#include "pitch.h"
#include "m_moto.h"

#define Left_GPIO        GPIOF     //��ߵĹ��Թ�
#define Left_Pin         GPIO_PIN_1
#define Right_GPIO       GPIOF     //�ұߵĹ��Թ�
#define Right_Pin        GPIO_PIN_0
#define total_bullet			500

/*-------------------------------Para--------------------------------------------*/
//���ּӾ���ֵ����Ϊ�����������λ�ÿ����и�������������ܳ���ҲΪ���������㱻����λ�ø����Ƚϻ����
#define nearby_plane  (fabs(Chassis_MOTO[0].getpara.xiangdui_round-guidao_changdu_qidian)<guidao_changdu_zongde*0.2f)//���˻��෶Χ
#define nearby_middle ((fabs(Chassis_MOTO[0].getpara.xiangdui_round-guidao_changdu_qidian)<guidao_changdu_zongde*0.8f)&&(fabs(Chassis_MOTO[0].getpara.xiangdui_round-guidao_changdu_qidian)>guidao_changdu_zongde*0.2f))//�м䷶Χ
#define nearby_bridge	(fabs(Chassis_MOTO[0].getpara.xiangdui_round-guidao_changdu_qidian)>guidao_changdu_zongde*0.8f)//�Ų෶Χ
#define now_potision fabs(Chassis_MOTO[0].getpara.xiangdui_round-guidao_changdu_qidian)//���ڱ����λ��
#define zuobian_check HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_0)
#define youbian_check HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_1)

typedef struct __mocalun_state_t{
	uint8_t Mocalun_Connect_TO_2006;
	float Mocalun_Connect_TO_2006_count;
}mocalun_state_t;

//�����˶�ģʽ
typedef enum {
	attack=1,
	dodge_level1=2,
	dodge_level2=3//λ�û�

}OvermodeSta_t;

//���̿��Ʒ�ʽ
typedef enum __chassiss
{
	POSITION,
	SPEED
}chassiss_control_mode_t;


//yaw���˶�
typedef struct __Yaw_t{
	float yaw_auto_angle_set;
	void (*YawMotoAutoMode)(struct __Moto_t *MOTO);
}Yaw_t;

//pitch���˶�
typedef struct __Pitch_t{
	float pit_auto_angle_set;    //�Ƕ�ֵ
	void (*PitchMotoAutoMode)(struct __Moto_t *MOTO);
}Pitch_t;


typedef struct __dodge_level2_t{
	uint8_t danfangxiangzou_flag;
}dodge_level2_t;

//�ڱ��˶��ṹ��
typedef struct __OverallMode_t{	
	void (*ChassisMotoAutoMode)(struct __Moto_t *MOTO);
	
	mocalun_state_t mocalun_state;
	Yaw_t yaw;
	Pitch_t pitch;
	chassiss_control_mode_t chassiss_control_mode;
	dodge_level2_t dodge_level2;
	uint8_t OvermodeSta;
	uint8_t tx2_gimble;
	uint8_t if_attrack;
	uint8_t	Overall_mode_gimbal;  //Ѳ�߻��߶�����
	uint8_t yaw_derection;     //�������� ż������
	float pitch_derection;
	uint8_t Chassis_derection;
	uint8_t yu_shu;
}OverallMode_t;

/*-------------------------------Para--------------------------------------------*/
extern float chassis_positon;
extern OverallMode_t overall_mode;
extern uint16_t rest_bullet;
extern uint8_t chassis_power_flag;
extern uint16_t yuanlai_xueliang; 
extern float position_to_speed_dengdai_time;
extern uint8_t if_position_to_speed_dengdai_time;
extern uint8_t shoot_flag;
extern float shoot_pending;

extern void PitchMotoAutoMode(Moto_t *MOTO);
extern void YawMotoAutoMode(Moto_t *MOTO);
extern void ChassisMotoAutoMode(Moto_t *MOTO);
extern void Init_OverallMode(OverallMode_t *overall_mode);
extern void restbullet(void);
extern void qianwanbiezhuangqiang(void);

extern void Repeated_move(void);
extern int16_t GetLightlength(int8_t direction);

extern void UsartMonitor(void);
extern void mocalun_state_monitor(void);
extern void Turn(void);
extern void AllAuto(void);
#endif

