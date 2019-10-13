/*******************************************************************************
* �ļ����ƣ�mavlink_main.c
*
* ժ    Ҫ��mavlink�Զ����ļ�
*
* ��ǰ�汾��
* ��    �ߣ�
* ��    �ڣ�2018/05/30
* ���뻷����keil5
*
* ��ʷ��Ϣ��
*******************************************************************************/

#include "mavlink.h"
#include "usart.h"
#include "mytype.h"
#include "stdio.h"

#define  MAVLINK_READ_BUFFER_SIZE 128

/*������������*/
//ringbuffer�������
RingBuffer  m_Mavlink_RX_RingBuffMgr;
/*MAVLINK�������ݻ���������*/
uint8_t   m_Mavlink_RX_Buff[MAVLINK_READ_BUFFER_SIZE*10];
uint8_t   mavlink_byte=0;

/*******************************��������****************************************
* ��������: void Mavlink_RB_Init(void)
* �������:
* ���ز���:
* ��    ��: ��ʼ��һ��ѭ�����У�����������յ��Ĵ������ݡ���ʵ����һ�����ݻ�����
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
void Mavlink_RB_Init(void)
{
	//��m_Mavlink_RX_Buffm_Mavlink_RX_RingBuffMgr�����н��й�������
	rbInit(&m_Mavlink_RX_RingBuffMgr, m_Mavlink_RX_Buff, sizeof(m_Mavlink_RX_Buff));
}

/*******************************��������****************************************
* ��������: void Mavlink_RB_Clear(void)
* �������:
* ���ز���:
* ��    ��: ����ringbuffer��������á�
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
void Mavlink_RB_Clear(void)
{
	rbClear(&m_Mavlink_RX_RingBuffMgr);
}

/*******************************��������****************************************
* ��������: uint8_t  Mavlink_RB_IsOverFlow(void)
* �������:
* ���ز���:  ���Ϊ1����֮Ϊ0
* ��    ��: �жϻ������Ƿ����
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
uint8_t  Mavlink_RB_IsOverFlow(void)
{
	return m_Mavlink_RX_RingBuffMgr.flagOverflow;
}

/*******************************��������****************************************
* ��������: void Mavlink_RB_Push(uint8_t data)
* �������: data����ѹ�������
* ���ز���:
* ��    ��: �����յ�����ѹ�뻺����
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
void Mavlink_RB_Push(uint8_t data)
{
	rbPush(&m_Mavlink_RX_RingBuffMgr, (uint8_t)(data & (uint8_t)0xFFU));
}

/*******************************��������****************************************
* ��������: uint8_t Mavlink_RB_Pop(void)
* �������:
* ���ز���: uint8_t ����������
* ��    ��: �ӻ�������������
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
uint8_t Mavlink_RB_Pop(void)
{
	return rbPop(&m_Mavlink_RX_RingBuffMgr);
}

/*******************************��������****************************************
* ��������: uint8_t Mavlink_RB_HasNew(void)
* �������:
* ���ز���:
* ��    ��: �ж��Ƿ����µ�����
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
uint8_t Mavlink_RB_HasNew(void)
{
	return !rbIsEmpty(&m_Mavlink_RX_RingBuffMgr);
}

/*******************************��������****************************************
* ��������: uint16_t Mavlink_RB_Count(void)
* �������:
* ���ز���:
* ��    ��: �ж��ж��ٸ�������
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
uint16_t Mavlink_RB_Count(void)
{
	return rbGetCount(&m_Mavlink_RX_RingBuffMgr);
}

/*******************************��������****************************************
* ��������: void Mavlink_Rece_Enable(void)
* �������:
* ���ز���:
* ��    ��: ʹ��DMA����
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
void Mavlink_Rece_Enable(void)
{
	while(	HAL_UART_Receive_DMA(&huart3, &mavlink_byte, 1u)!=HAL_OK);
	//HAL_UART_Receive_IT(&huart3, &mavlink_byte, 1u);
}

/*******************************��������****************************************
* ��������: void Mavlink_Init(void)
* �������:
* ���ز���:
* ��    ��: ��ʼ��MAVLINK��ʹ�ܽ��գ�ringbuffer����
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
void Mavlink_Init(void)
{
	Mavlink_Rece_Enable();
	Mavlink_RB_Init();
}
 
/*******************************��������****************************************
* ��������: void Mavlin_RX_InterruptHandler(void)
* �������:
* ���ز���:
* ��    ��: �����жϵĴ���������Ҫ�ǽ�����ѹ��ringbuffer������
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
void Mavlin_RX_InterruptHandler(void)
{
	//����ѹ��
	rbPush(&m_Mavlink_RX_RingBuffMgr,mavlink_byte);
//rbPop(&m_Mavlink_RX_RingBuffMgr,mavlink_byte);//ѹ������
}
/*�ڡ�mavlink_helpers.h����Ҫʹ�á�*/
mavlink_system_t mavlink_system =
{
	1,
	1
}; // System ID, 1-255, Component/Subsystem ID, 1-255

/*******************************��������****************************************
* ��������: void mavlink_send_uart_bytes(mavlink_channel_t chan, const uint8_t *ch, int length)
* �������:
* ���ز���:
* ��    ��: ���¶���mavlink�ķ��ͺ�������ײ�Ӳ���ӿڹ�������
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
void mavlink_send_uart_bytes(mavlink_channel_t chan, const uint8_t *ch, int length)
{
	HAL_UART_Transmit(&huart3, (uint8_t *)ch, length,20);
}

/*******************************��������****************************************
* ��������: void Mavlink_Msg_Handle(void)
* �������:
* ���ز���:
* ��    ��: �����QGC��λ����������������Ϣ
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
static mavlink_message_t  msg;
mavlink_heartbeat_t  heartbeat;
mavlink_gimbal_control_t  gimbal_control;
mavlink_chassis_control_t chassis_control;
void Mavlink_Msg_Handle(mavlink_message_t msg)
{	
	switch(msg.msgid)
	{
	case MAVLINK_MSG_ID_HEARTBEAT:
	     mavlink_msg_heartbeat_decode(&msg, &heartbeat);
       break;	
	case MAVLINK_MSG_ID_GIMBAL_CONTROL:
       mavlink_msg_gimbal_control_decode(&msg, &gimbal_control);
	     break;
	case MAVLINK_MSG_ID_CHASSIS_CONTROL:	
       mavlink_msg_chassis_control_decode(&msg,&chassis_control);
	     break;
	default:
	   	 break;
	}
}


/*******************************��������****************************************
* ��������: Loop_Mavlink_Parse(void)
* �������:
* ���ز���:
* ��    ��: ��main�����в���ϵ��ô˺���
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
mavlink_message_t msg;
mavlink_status_t status;

void Loop_Mavlink_Parse(void)
{
	if(Mavlink_RB_IsOverFlow()) //�жϻ������Ƿ����
	{ //���Ϊ1����֮Ϊ0
		Mavlink_RB_Clear(); //����ringbuffer���������
	}

	while(Mavlink_RB_HasNew()) //�ж��Ƿ����µ�����
//		while(0) 
	{
		uint8_t read = Mavlink_RB_Pop(); //�ӻ�������������
		if(mavlink_parse_char(MAVLINK_COMM_0, read, &msg, &status))
		 //����������� 
		 //ÿ�ν��յ�һ���ֽ����ݵ�ʱ�����mavlink_parse_char����������Ϣ
		 //ֱ��һ�����������ݰ����������
		{
			//�źŴ����� �����QGC��λ����������������Ϣ
			//���
			Mavlink_Msg_Handle(msg);
//			printf("Received message with ID %d, sequence: %d from component %d of system %d", msg.msgid, msg.seq, msg.compid, msg.sysid);
		}
	}
}

/**********************������**************************/
static void mavlink_msg_heartbeat(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
	mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
	if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_HEARTBEAT >= 256)
	{
		return;
	}
#endif
	mavlink_heartbeat_t packet_in =
	{
		963497464,17,84,151,218,3
	};
	mavlink_heartbeat_t packet1, packet2;
	memset(&packet1, 0, sizeof(packet1));
	packet1.custom_mode = packet_in.custom_mode;
	packet1.type = packet_in.type;
	packet1.autopilot = packet_in.autopilot;
	packet1.base_mode = packet_in.base_mode;
	packet1.system_status = packet_in.system_status;
	packet1.mavlink_version = packet_in.mavlink_version;


#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
	if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1)
	{
		// cope with extensions
		memset(MAVLINK_MSG_ID_HEARTBEAT_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_HEARTBEAT_MIN_LEN);
	}
#endif
	memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_heartbeat_send(MAVLINK_COMM_0 , packet1.type , packet1.autopilot , packet1.base_mode , packet1.custom_mode , packet1.system_status );
	MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
}



/**********************��̨�Ƕ�**************************/
extern uint8_t TX2Mode;
static void mavlink_msg_gimbal_control(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
	mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
	if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_GIMBAL_CONTROL >= 256)
	{
		return;
	}
#endif
	mavlink_gimbal_control_t packet1;
	memset(&packet1, 0, sizeof(packet1));
	//�ṹ�帳ֵ
//	packet1.yaw_angle = (float)99;
//	packet1.pitch_angle = (float)66;
	  packet1.control_mode = (int8_t)TX2Mode;

#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
	if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1)
	{
		// cope with extensions
		memset(MAVLINK_MSG_ID_GIMBAL_CONTROL_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_GIMBAL_CONTROL_MIN_LEN);
	}
#endif
	
	mavlink_msg_gimbal_control_send(MAVLINK_COMM_0 , packet1.yaw_angle, packet1.pitch_angle,packet1.control_mode);
}



/**********************���̿���**************************/
static void mavlink_msg_chassis_control(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
	mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
	if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_CHASSIS_CONTROL >= 256)
	{
		return;
	}
#endif

	mavlink_chassis_control_t packet1;
	memset(&packet1, 0, sizeof(packet1));//���� ����ַ�����ݣ����ȣ�

  packet1.forward_velocity = (float)99; 
  packet1.right_velocity = (float)100;

#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
	if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1)
	{
		// cope with extensions
		memset(MAVLINK_MSG_ID_CHASSIS_CONTROL_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_CHASSIS_CONTROL_MIN_LEN);
	}
#endif	
	mavlink_msg_chassis_control_send(MAVLINK_COMM_0 , packet1.forward_velocity, packet1.right_velocity);
}


void mavlink_test(void)
{
	static uint16_t test_count=0;
	mavlink_message_t lastmsg;
	test_count++;
	//5hz
	if((test_count%100)==0)
	{
//	mavlink_msg_heartbeat(1,1,&lastmsg);
	}
	if((test_count%50)==0)
	{
		mavlink_msg_gimbal_control(1,1,&lastmsg);
//  mavlink_msg_chassis_control(1,1,&lastmsg);
	}
	
}



/**
  * @brief  rbInitialize��ʼ�����ã�����������Ϣ����ṹ��
  * @param  pRingBuff:ringbuffer�ṹ��
	*         buff�����ݻ�����
	*         length����������С
  * @note   
  * @retval void
  * @author Acorss������	
  */	
void rbInit(RingBuffer* pRingBuff, uint8_t* buff, uint16_t length)
{
	pRingBuff->pBuff = buff;
	pRingBuff->pEnd  = buff + length;
	pRingBuff->wp = buff;
	pRingBuff->rp = buff;
	pRingBuff->length = length;
	pRingBuff->flagOverflow = 0;
}


/**
  * @brief  ���ringbuffer�ṹ��������Ϣ
  * @param  pRingBuff���������ringbuffer
  * @note   
  * @retval void
  * @author Acorss������	
  */	
  void rbClear(RingBuffer* pRingBuff)
{
 	pRingBuff->wp = pRingBuff->pBuff;
	pRingBuff->rp = pRingBuff->pBuff;
	pRingBuff->flagOverflow = 0;
}

/**
  * @brief  ѹ�뵥�ֽڵ�������
  * @param  pRingBuff���������ringbuffer
  *         value��ѹ�������
  * @note   
  * @retval void
  * @author Acorss������	
  */	
  void rbPush(RingBuffer* pRingBuff, uint8_t value)
{
	uint8_t* wp_next = pRingBuff->wp + 1;
	if( wp_next == pRingBuff->pEnd ) {
		wp_next -= pRingBuff->length; // Rewind pointer when exceeds bound
	}
	if( wp_next != pRingBuff->rp ) {
		*pRingBuff->wp = value;
		pRingBuff->wp = wp_next;
	} else {
		pRingBuff->flagOverflow = 1;
	}
}

/**
  * @brief  ѹ�����ֽڵ�������
  * @param  pRingBuff���������ringbuffer   
  * @note   
  * @retval ѹ��������
  * @author Acorss������	
  */	

  uint8_t rbPop(RingBuffer* pRingBuff)
{
	if( pRingBuff->rp == pRingBuff->wp ) 
		return 0; // empty
  
	uint8_t ret = *(pRingBuff->rp++);
	if( pRingBuff->rp == pRingBuff->pEnd ) {
		pRingBuff->rp -= pRingBuff->length; // Rewind pointer when exceeds bound
	}
	return ret;

}

/**
  * @brief  ��ȡ��������δ������ֽ���
  * @param  pRingBuff���������ringbuffer   
  * @note   
  * @retval ��������ֽ���
  * @author Acorss������	
  */
  uint16_t rbGetCount(const RingBuffer* pRingBuff)
{
	return (pRingBuff->wp - pRingBuff->rp + pRingBuff->length) % pRingBuff->length;
}

/**
  * @brief  �жϻ������Ƿ�Ϊ��
  * @param  pRingBuff���������ringbuffer   
  * @note   
  * @retval ��Ϊ1������Ϊ0
  * @author Acorss������	
  */
  int8_t rbIsEmpty(const RingBuffer* pRingBuff)
{
	return pRingBuff->wp == pRingBuff->rp; 
}

/**
  * @brief  �жϻ������Ƿ��
  * @param  pRingBuff���������ringbuffer   
  * @note   
  * @retval ��Ϊ1������Ϊ0
  * @author Acorss������	
  */
  int8_t rbIsFull(const RingBuffer* pRingBuff)
{
 	return (pRingBuff->rp - pRingBuff->wp + pRingBuff->length - 1) % pRingBuff->length == 0;
}
