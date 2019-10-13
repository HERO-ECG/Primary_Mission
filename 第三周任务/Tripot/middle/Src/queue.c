#include "queue.h"


void EnQueue(Queue_t* queue,float data)
{
	queue->pointer++;
	if(queue->pointer==queue->length)
		queue->pointer=0;
	queue->data[queue->pointer]=data;
}

float GetValFromQueue(Queue_t* queue,uint16_t index)
{
		if(index>=queue->length)
		{
			return 0;
		}
		else
		{
			if(index<queue->pointer)
			{
				return queue->data[queue->pointer-index];
			}
			else
			{
				return queue->data[queue->length-index+queue->pointer-1];
			}
		}
}

void ClearQueue(Queue_t* queue)
{
	uint16_t i;
	for(i=0;i<MAX_LENGTH;i++)
			queue->data[i]=0;
	queue->pointer=0;
	queue->length=0;
}
