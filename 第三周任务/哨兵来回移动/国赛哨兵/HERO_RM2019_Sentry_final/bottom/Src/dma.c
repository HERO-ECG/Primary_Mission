#include "dma.h"

DMA_HandleTypeDef hdma_usart1_rx;

DMA_HandleTypeDef hdma_usart2_rx;

DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;

DMA_HandleTypeDef hdma_usart6_rx;
DMA_HandleTypeDef hdma_usart6_tx;

DMA_HandleTypeDef hdma_usart8_tx;
DMA_HandleTypeDef hdma_usart8_rx;
//DMAx�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMAͨ��ѡ��,@ref DMA_channel DMA_CHANNEL_0~DMA_CHANNEL_7
void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx,DMA_HandleTypeDef *hdma)
{ 
	if((u32)DMA_Streamx>(u32)DMA2)//�õ���ǰstream������DMA2����DMA1
	{
        __HAL_RCC_DMA2_CLK_ENABLE();//DMA2ʱ��ʹ��	
	}
	else 
	{
        __HAL_RCC_DMA1_CLK_ENABLE();//DMA1ʱ��ʹ�� 
	}
	 //Rx DMA����   �����ȼ��ǵ͵����ȼ��������е����ȼ������������óɺ�����ԭ����̳��������ͬ��ģʽ��
    hdma->Instance = DMA_Streamx;                            //������ѡ�� DMA2_Stream2   ����2��DMA1_Stream5
    hdma->Init.Channel = chx;                                //ͨ��ѡ�� DMA_CHANNEL_4    ����2��DMA_CHANNEL_4
	  if(hdma == &hdma_usart6_tx) 
		{
    hdma->Init.Direction = DMA_MEMORY_TO_PERIPH;             //�洢��������			
		}
		else
		{
    hdma->Init.Direction = DMA_PERIPH_TO_MEMORY;             //���赽�洢��		
		}
    hdma->Init.PeriphInc=DMA_PINC_DISABLE;                 //���������ģʽ
    hdma->Init.MemInc=DMA_MINC_ENABLE;                     //�洢������ģʽ
    hdma->Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;    //�������ݳ���:8λ
    hdma->Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;       //�洢�����ݳ���:8λ
    hdma->Init.Mode=DMA_CIRCULAR;                          //ѭ��ģʽ
  	hdma->Init.FIFOMode=DMA_FIFOMODE_DISABLE;              
    hdma->Init.FIFOThreshold=DMA_FIFO_THRESHOLD_1QUARTERFULL;     
    hdma->Init.MemBurst=DMA_MBURST_SINGLE;                 //�洢��ͻ�����δ���
    hdma->Init.PeriphBurst=DMA_PBURST_SINGLE;              //����ͻ�����δ���	
	  if(hdma == &hdma_usart1_rx)
	  { 
		  __HAL_LINKDMA(&huart1,hdmarx,hdma_usart1_rx);     //��DMA��USART1��ϵ����(����DMA)
		  hdma->Init.Priority=DMA_PRIORITY_VERY_HIGH;          //�ǳ��ߵ����ȼ�
		 
		  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 1, 0);
      HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
	  }
	  else if(hdma == &hdma_usart3_rx)
	  {
		  __HAL_LINKDMA(&huart3,hdmarx,hdma_usart3_rx);    
	 	  hdma->Init.Priority=DMA_PRIORITY_VERY_HIGH;             //�ǳ��ߵ����ȼ�
		 
	 		HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
      HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
	  }	
	  else if(hdma == &hdma_usart6_rx)
	  {
		  __HAL_LINKDMA(&huart6,hdmarx,hdma_usart6_rx);    
	 	  hdma->Init.Priority=DMA_PRIORITY_VERY_HIGH;              //�����ȼ�
		 
	 		HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 2, 0);
      HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
	  }	
	  else if(hdma == &hdma_usart2_rx)
	  {
		  __HAL_LINKDMA(&huart2,hdmarx,hdma_usart2_rx);    
	 	  hdma->Init.Priority=DMA_PRIORITY_LOW;             //�����ȼ�
		 
	 		HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 3, 0);
      HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
	  }	
	  else if(hdma == &hdma_usart6_tx)
	  {
		  __HAL_LINKDMA(&huart6,hdmatx,hdma_usart6_tx);    
	 	  hdma->Init.Priority=DMA_PRIORITY_MEDIUM;               //��ͨ���ȼ�
	  }	
		else if(hdma == &hdma_usart8_rx)
	  {
		  __HAL_LINKDMA(&huart8,hdmarx,hdma_usart8_rx);    
	 	  hdma->Init.Priority=DMA_PRIORITY_MEDIUM;             //�����ȼ�
		 
	 		HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 3, 0);
      HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
	  }		
	  
	  
	  if(HAL_DMA_Init(hdma) != HAL_OK)
    {
      Error_Handler();
    }	 
    HAL_DMA_DeInit(hdma);   
    HAL_DMA_Init(hdma);	
} 

//����һ��DMA����
//huart:���ھ��
//pData�����������ָ��
//Size:�����������
void MYDMA_USART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    HAL_DMA_Start(huart->hdmatx, (u32)pData, (uint32_t)&huart->Instance->DR, Size);//����DMA����
    
    huart->Instance->CR3 |= USART_CR3_DMAT;//ʹ�ܴ���DMA����
}	  

void DMA2_Stream2_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart1_rx);	
} 

void DMA1_Stream5_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart2_rx);	
} 

void DMA1_Stream1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart3_rx);	
} 

void DMA1_Stream4_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart3_rx);	
} 

void DMA2_Stream1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart6_rx);	
} 

void DMA1_Stream6_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart8_rx);	
} 

 


