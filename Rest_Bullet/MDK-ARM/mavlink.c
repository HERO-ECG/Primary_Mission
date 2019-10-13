/*******************************************************************************
* 文件名称：mavlink_main.c
*
* 摘    要：mavlink自定义文件
*
* 当前版本：
* 作    者：
* 日    期：2018/05/30
* 编译环境：keil5
*
* 历史信息：
*******************************************************************************/

#include "mavlink.h"
#include "usart.h"
#include "mytype.h"
#include "stdio.h"

#define  MAVLINK_READ_BUFFER_SIZE 128

/*缓冲区管理器*/
//ringbuffer管理变量
RingBuffer  m_Mavlink_RX_RingBuffMgr;
/*MAVLINK接收数据缓冲区数组*/
uint8_t   m_Mavlink_RX_Buff[MAVLINK_READ_BUFFER_SIZE*10];
uint8_t   mavlink_byte=0;

/*******************************函数声明****************************************
* 函数名称: void Mavlink_RB_Init(void)
* 输入参数:
* 返回参数:
* 功    能: 初始化一个循环队列，用来管理接收到的串口数据。其实就是一个数据缓冲区
* 作    者: by Across
* 日    期: 2018/06/02
*******************************************************************************/
void Mavlink_RB_Init(void)
{
	//将m_Mavlink_RX_Buffm_Mavlink_RX_RingBuffMgr环队列进行关联管理。
	rbInit(&m_Mavlink_RX_RingBuffMgr, m_Mavlink_RX_Buff, sizeof(m_Mavlink_RX_Buff));
}

/*******************************函数声明****************************************
* 函数名称: void Mavlink_RB_Clear(void)
* 输入参数:
* 返回参数:
* 功    能: 归零ringbuffer里面的设置。
* 作    者: by Across
* 日    期: 2018/06/02
*******************************************************************************/
void Mavlink_RB_Clear(void)
{
	rbClear(&m_Mavlink_RX_RingBuffMgr);
}

/*******************************函数声明****************************************
* 函数名称: uint8_t  Mavlink_RB_IsOverFlow(void)
* 输入参数:
* 返回参数:  溢出为1，反之为0
* 功    能: 判断缓冲器是否溢出
* 作    者: by Across
* 日    期: 2018/06/02
*******************************************************************************/
uint8_t  Mavlink_RB_IsOverFlow(void)
{
	return m_Mavlink_RX_RingBuffMgr.flagOverflow;
}

/*******************************函数声明****************************************
* 函数名称: void Mavlink_RB_Push(uint8_t data)
* 输入参数: data：待压入的数据
* 返回参数:
* 功    能: 将接收的数据压入缓冲区
* 作    者: by Across
* 日    期: 2018/06/02
*******************************************************************************/
void Mavlink_RB_Push(uint8_t data)
{
	rbPush(&m_Mavlink_RX_RingBuffMgr, (uint8_t)(data & (uint8_t)0xFFU));
}

/*******************************函数声明****************************************
* 函数名称: uint8_t Mavlink_RB_Pop(void)
* 输入参数:
* 返回参数: uint8_t 读出的数据
* 功    能: 从缓冲器读出数据
* 作    者: by Across
* 日    期: 2018/06/02
*******************************************************************************/
uint8_t Mavlink_RB_Pop(void)
{
	return rbPop(&m_Mavlink_RX_RingBuffMgr);
}

/*******************************函数声明****************************************
* 函数名称: uint8_t Mavlink_RB_HasNew(void)
* 输入参数:
* 返回参数:
* 功    能: 判断是否有新的数据
* 作    者: by Across
* 日    期: 2018/06/02
*******************************************************************************/
uint8_t Mavlink_RB_HasNew(void)
{
	return !rbIsEmpty(&m_Mavlink_RX_RingBuffMgr);
}

/*******************************函数声明****************************************
* 函数名称: uint16_t Mavlink_RB_Count(void)
* 输入参数:
* 返回参数:
* 功    能: 判断有多少个新数据
* 作    者: by Across
* 日    期: 2018/06/02
*******************************************************************************/
uint16_t Mavlink_RB_Count(void)
{
	return rbGetCount(&m_Mavlink_RX_RingBuffMgr);
}

/*******************************函数声明****************************************
* 函数名称: void Mavlink_Rece_Enable(void)
* 输入参数:
* 返回参数:
* 功    能: 使能DMA接收
* 作    者: by Across
* 日    期: 2018/06/02
*******************************************************************************/
void Mavlink_Rece_Enable(void)
{
	while(	HAL_UART_Receive_DMA(&huart3, &mavlink_byte, 1u)!=HAL_OK);
	//HAL_UART_Receive_IT(&huart3, &mavlink_byte, 1u);
}

/*******************************函数声明****************************************
* 函数名称: void Mavlink_Init(void)
* 输入参数:
* 返回参数:
* 功    能: 初始化MAVLINK：使能接收，ringbuffer关联
* 作    者: by Across
* 日    期: 2018/06/02
*******************************************************************************/
void Mavlink_Init(void)
{
	Mavlink_Rece_Enable();
	Mavlink_RB_Init();
}
 
/*******************************函数声明****************************************
* 函数名称: void Mavlin_RX_InterruptHandler(void)
* 输入参数:
* 返回参数:
* 功    能: 串口中断的处理函数，主要是将数据压入ringbuffer管理器
* 作    者: by Across
* 日    期: 2018/06/02
*******************************************************************************/
void Mavlin_RX_InterruptHandler(void)
{
	//数据压入
	rbPush(&m_Mavlink_RX_RingBuffMgr,mavlink_byte);
//rbPop(&m_Mavlink_RX_RingBuffMgr,mavlink_byte);//压出数据
}
/*在“mavlink_helpers.h中需要使用”*/
mavlink_system_t mavlink_system =
{
	1,
	1
}; // System ID, 1-255, Component/Subsystem ID, 1-255

/*******************************函数声明****************************************
* 函数名称: void mavlink_send_uart_bytes(mavlink_channel_t chan, const uint8_t *ch, int length)
* 输入参数:
* 返回参数:
* 功    能: 重新定义mavlink的发送函数，与底层硬件接口关联起来
* 作    者: by Across
* 日    期: 2018/06/02
*******************************************************************************/
void mavlink_send_uart_bytes(mavlink_channel_t chan, const uint8_t *ch, int length)
{
	HAL_UART_Transmit(&huart3, (uint8_t *)ch, length,20);
}

/*******************************函数声明****************************************
* 函数名称: void Mavlink_Msg_Handle(void)
* 输入参数:
* 返回参数:
* 功    能: 处理从QGC上位机传过来的数据信息
* 作    者: by Across
* 日    期: 2018/06/02
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


/*******************************函数声明****************************************
* 函数名称: Loop_Mavlink_Parse(void)
* 输入参数:
* 返回参数:
* 功    能: 在main函数中不间断调用此函数
* 作    者: by Across
* 日    期: 2018/06/02
*******************************************************************************/
mavlink_message_t msg;
mavlink_status_t status;

void Loop_Mavlink_Parse(void)
{
	if(Mavlink_RB_IsOverFlow()) //判断缓冲器是否溢出
	{ //溢出为1，反之为0
		Mavlink_RB_Clear(); //归零ringbuffer里面的设置
	}

	while(Mavlink_RB_HasNew()) //判断是否有新的数据
//		while(0) 
	{
		uint8_t read = Mavlink_RB_Pop(); //从缓冲器读出数据
		if(mavlink_parse_char(MAVLINK_COMM_0, read, &msg, &status))
		 //处理接收数据 
		 //每次接收到一个字节数据的时候调用mavlink_parse_char来解析的信息
		 //直到一个完整的数据包被解析完成
		{
			//信号处理函数 处理从QGC上位机传过来的数据信息
			//解包
			Mavlink_Msg_Handle(msg);
//			printf("Received message with ID %d, sequence: %d from component %d of system %d", msg.msgid, msg.seq, msg.compid, msg.sysid);
		}
	}
}

/**********************心跳包**************************/
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



/**********************云台角度**************************/
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
	//结构体赋值
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



/**********************底盘控制**************************/
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
	memset(&packet1, 0, sizeof(packet1));//设置 （地址，内容，长度）

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
  * @brief  rbInitialize初始化配置，将缓冲区信息填入结构体
  * @param  pRingBuff:ringbuffer结构体
	*         buff：数据缓冲区
	*         length：缓冲区大小
  * @note   
  * @retval void
  * @author Acorss工作室	
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
  * @brief  清除ringbuffer结构体的相关信息
  * @param  pRingBuff：待处理的ringbuffer
  * @note   
  * @retval void
  * @author Acorss工作室	
  */	
  void rbClear(RingBuffer* pRingBuff)
{
 	pRingBuff->wp = pRingBuff->pBuff;
	pRingBuff->rp = pRingBuff->pBuff;
	pRingBuff->flagOverflow = 0;
}

/**
  * @brief  压入单字节到缓冲区
  * @param  pRingBuff：待处理的ringbuffer
  *         value：压入的数据
  * @note   
  * @retval void
  * @author Acorss工作室	
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
  * @brief  压出单字节到缓冲区
  * @param  pRingBuff：待处理的ringbuffer   
  * @note   
  * @retval 压出的数据
  * @author Acorss工作室	
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
  * @brief  获取缓冲区尚未处理的字节数
  * @param  pRingBuff：待处理的ringbuffer   
  * @note   
  * @retval 待处理的字节数
  * @author Acorss工作室	
  */
  uint16_t rbGetCount(const RingBuffer* pRingBuff)
{
	return (pRingBuff->wp - pRingBuff->rp + pRingBuff->length) % pRingBuff->length;
}

/**
  * @brief  判断缓冲区是否为空
  * @param  pRingBuff：待处理的ringbuffer   
  * @note   
  * @retval 空为1；否则为0
  * @author Acorss工作室	
  */
  int8_t rbIsEmpty(const RingBuffer* pRingBuff)
{
	return pRingBuff->wp == pRingBuff->rp; 
}

/**
  * @brief  判断缓冲区是否空
  * @param  pRingBuff：待处理的ringbuffer   
  * @note   
  * @retval 满为1；否则为0
  * @author Acorss工作室	
  */
  int8_t rbIsFull(const RingBuffer* pRingBuff)
{
 	return (pRingBuff->rp - pRingBuff->wp + pRingBuff->length - 1) % pRingBuff->length == 0;
}
