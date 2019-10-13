#ifndef __M_DJI_PROTOCOL_H
#define __M_DJI_PROTOCOL_H
/**����ϵͳͨ��Э��**/
#include "mytype.h"
#include "robodata.h"
/********************  ��������  ***************************/
//0-3 bits
#define Strike_0   0x00 //0��װ���棨ǰ��
#define Strike_1   0x01 //1��װ���棨��
#define Strike_2   0x02 //2��װ���棨��
#define Strike_3   0x03 //3��װ���棨�ң�
#define Strike_4   0x04 //4��װ���棨��1��
#define Strike_5   0x05 //5��װ���棨��2��
//4-7 bits
#define BC_Strike          0x00 //װ���˺����ܵ������� BloodChanged    BC
#define BC_ModuleOffline   0x01 //ģ�����߿�Ѫ
#define BC_ShootSpeed      0x02 //�ӵ����ٿ�Ѫ
#define BC_ShootFreq       0x03 //�ӵ���Ƶ��Ѫ
#define BC_ShootQLimit     0x04 //ǹ����������
#define BC_CmPLimit        0x05 //���̹��ʳ���

#define BC_CommonFoul      0x06 //��ͨ�����Ѫ
#define BC_Tarmac          0x0a //ͣ��ƺ��Ѫ
#define BC_EngineerOuto    0x0b //���̻������Զ���Ѫ
typedef	struct
{
	  u8 b1;
    u8 b2;
		u8 b3;
		u8 b4;
		u8 b5;
		u8 b6;
}DataBit_t;

typedef	struct
{
	uint8_t val;
	DataBit_t databit;
}CustomDataBit_t;



typedef	struct
{
	CustomDataBit_t databit;
	float fdata[3];
}CustomData_t;

//����������״̬��0x0001��
typedef __packed struct
{
  uint8_t game_type : 4;
  uint8_t game_progress : 4;
  uint16_t stage_remain_time;
} ext_game_state_t;



//�˺����ݣ�0x0002��
typedef __packed struct
{
  uint8_t winner;
} ext_game_result_t;

//ʵʱ�����Ϣ��0x0003��
typedef __packed struct
{
  uint16_t robot_legion;
} ext_game_robot_survivors_t;

//ʵʱ�����������ݣ�0x004��
typedef __packed struct
{
  uint32_t event_type;
} ext_event_data_t;



//���ؽ������ݣ�0x0005��
typedef __packed struct
{
  uint8_t supply_projectile_id; 
  uint8_t supply_robot_id; 
  uint8_t supply_projectile_step; 
uint8_t supply_projectile_num;
} ext_supply_projectile_action_t;

//����ʤ�����ݣ�0x006��
typedef __packed struct
{
  uint8_t supply_projectile_id;
  uint8_t supply_robot_id; 
uint8_t supply_num;
} ext_supply_projectile_booking_t;

//Buff��ȡ���ݣ�0x0007��
typedef __packed struct
{
  uint8_t robot_id;
  uint8_t robot_level;
  uint16_t remain_HP;
  uint16_t max_HP;
  uint16_t shooter_heat0_cooling_rate;
  uint16_t shooter_heat0_cooling_limit;
  uint16_t shooter_heat1_cooling_rate;
  uint16_t shooter_heat1_cooling_limit;
  uint8_t mains_power_gimbal_output : 1;
  uint8_t mains_power_chassis_output : 1;
  uint8_t mains_power_shooter_output : 1;
} ext_game_robot_state_t;

typedef __packed struct
{
  uint16_t chassis_volt; 
  uint16_t chassis_current; 
  float chassis_power; 
  uint16_t chassis_power_buffer; 
  uint16_t shooter_heat0; 
  uint16_t shooter_heat1; 
} ext_power_heat_data_t;

//�������Զ�������
typedef __packed struct
{
	float data1;
	float data2;
	float data3;
	uint8_t mask;
}extShowData_t;
typedef __packed union 
{
	unsigned char datatransmit[4];
	float data;
}TransmitData_t;
typedef __packed struct
{
  uint16_t data_cmd_id;
  uint16_t send_ID;
  uint16_t receiver_ID;
}ext_student_interactive_header_data_t;

typedef __packed struct
{
  float data1;
  float data2;
  float data3;
  uint8_t masks;
} client_custom_data_t;
typedef __packed struct
{
  uint8_t data[113];
} robot_interactive_data_t;
//������λ�ó�����Ϣ
typedef __packed struct
{
 float x;
  float y;
  float z;
  float yaw;
} ext_game_robot_pos_t;

typedef __packed struct
{
  uint8_t power_rune_buff;
}ext_buff_musk_t;

typedef __packed struct
{
  uint8_t armor_id : 4;
  uint8_t hurt_type : 4;
} ext_robot_hurt_t;

typedef __packed struct
{
  uint8_t energy_point;
  uint8_t attack_time;
} aerial_robot_energy_t;

typedef __packed struct
{
  uint8_t bullet_type;
  uint8_t bullet_freq;
  float bullet_speed;
} ext_shoot_data_t;

typedef __packed struct
{
	uint8_t SOF;          //������ʼ�ֽڣ��̶�Ϊ0xA5          
	uint16_t DataLength;  //���ݳ���
	uint8_t Seq;          //�����
	uint8_t CRC8;         //֡ͷCRCУ��
}FrameHeader_t;//֡ͷ

typedef enum //����ǰ�˳������
{
	ext_game_state = 0x0001,     //����״̬����
	ext_game_result,             //�����������
	ext_game_robot_survivors,    //���������˴������       
	ext_event_data=0x101,        //�����¼�����
	ext_supply_projectile_action,//���ز���վ������ʶ���ݣ�
	ext_supply_projectile_booking,//���󲹸�վ��������
	ext_game_robot_state = 0x0201,//������״̬����
	ext_power_heat_data = 0x0202, //ʵʱ�����������ݣ�
    extGameRobotPos = 0x0203,     //������λ������
    ext_buff_musk,            //��������������
	aerial_robot_energy,     //���л���������״̬����
	ext_robot_hurt,          //�˺�״̬����
	ext_shoot_data,        //ʵʱ�������
	extInteractiveData=0x0301   //�����˼佻������
}CmdID_t;
typedef __packed union
	{
	float data;
	unsigned char divideddata[4];
	}floatdata_t;
typedef __packed struct
{
	FrameHeader_t    FrameHeader;
	CmdID_t          CmdID;
	__packed union 
	{
        ext_game_state_t   		    ext_game_state;
		ext_game_result_t			ext_game_result;
		ext_game_robot_survivors_t  ext_game_robot_survivors;
		ext_event_data_t            ext_event_data;
		ext_supply_projectile_action_t    ext_supply_projectile_action;
		ext_supply_projectile_booking_t   ext_supply_projectile_booking;
		ext_game_robot_state_t            ext_game_robot_state;
		ext_power_heat_data_t             ext_power_heat_data;		
		ext_game_robot_pos_t	  extGameRobotPos;
		ext_buff_musk_t           ext_buff_musk;
		aerial_robot_energy_t     aerial_robot_energy;
		ext_robot_hurt_t          ext_robot_hurt;
		ext_shoot_data_t          ext_shoot_data;
		ext_student_interactive_header_data_t  extInteractiveData;
		client_custom_data_t      client_custom_data;
		//TransmitData_t  TransmitData;	
	}Data;
	uint16_t        CRC16;         //֮ǰ��������CRCУ��   ע������ݺ�֮ǰ�����ݿ��ܲ����������Բ�Ҫֱ��ʹ�ã�����Ҫֱ��ʹ�ã������ڴ˸�ֵ
	
}Frame_t;  //����֡

typedef __packed struct
{
		FrameHeader_t    FrameHeader;
		CmdID_t          CmdID;
		ext_game_state_t   		    ext_game_state;
		ext_game_result_t			ext_game_result;
		ext_game_robot_survivors_t  ext_game_robot_survivors;
		ext_event_data_t            ext_event_data;
		ext_supply_projectile_action_t    ext_supply_projectile_action;
		ext_supply_projectile_booking_t   ext_supply_projectile_booking;
		ext_game_robot_state_t            ext_game_robot_state;
		ext_power_heat_data_t             ext_power_heat_data;		
		ext_game_robot_pos_t	  extGameRobotPos;
		ext_buff_musk_t           ext_buff_musk;
		aerial_robot_energy_t     aerial_robot_energy;
		ext_robot_hurt_t          ext_robot_hurt;
		ext_shoot_data_t          ext_shoot_data;
		ext_student_interactive_header_data_t  extInteractiveData;
        client_custom_data_t      client_custom_data;	
}DJI_Data_t; //����֡
extern DJI_Data_t DJI_ReadData;
extern unsigned char Get_CRC8_Check_Sum(unsigned char *pchMessage,unsigned int dwLength,unsigned char ucCRC8);
extern uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC);
extern short dji_DataProcess(uint8_t *pData, DJI_Data_t *DJI_ReadData, RoboData_t *RoboData);
void GetDataBit(CustomData_t *customdata);
void CustomTransmit(CustomData_t CustomData);
extern void  floattest(uint8_t *pData);
void CustomSend(void);



#define RM_IMU_QUAT_ID 0x401
#define RM_IMU_GYRO_ID 0x402
#define RM_IMU_ACCEL_ID 0x403
#define RM_IMU_MAG_ID 0x404
#define RM_IMU_PARAM_ID 0x405
//ת���� m/s^2
#define ACCEL_3G_SEN 0.0008974358974f
#define ACCEL_6G_SEN 0.00179443359375f
#define ACCEL_12G_SEN 0.0035888671875f
#define ACCEL_24G_SEN 0.007177734375f
//ת���� rad/s
#define GYRO_2000_SEN 0.00106526443603169529841533860381f
#define GYRO_1000_SEN 0.00053263221801584764920766930190693f
#define GYRO_500_SEN 0.00026631610900792382460383465095346f
#define GYRO_250_SEN 0.00013315805450396191230191732547673f
#define GYRO_125_SEN 0.000066579027251980956150958662738366f

typedef struct
{
	uint8_t quat_euler:1;
	uint8_t gyro_rangle:3;
	uint8_t accel_rangle:2;
	uint8_t imu_sensor_rotation:5;
	uint8_t ahrs_rotation_sequence:3;
	int16_t quat[4];
	double quat_fp32[4];
	int16_t euler_angle[3];
	double euler_angle_fp32[3];
	int16_t gyro_int16[3];
	int16_t accel_int16[3];
	int16_t mag_int16[3];
	double gyro_fp32[3];
	double accel_fp32[3];
	uint16_t sensor_time;
	uint16_t sensor_temperature;
	int16_t sensor_control_temperature;
	double gyro_sen;
	double accel_sen;
	
	double roll;
	double pitch;
	double yaw;
}rm_imu_data_t;
extern rm_imu_data_t rm_imu_data;

void toEulerAngle(double quat_fp32[4], double *roll, double *pitch, double *yaw);




#endif
