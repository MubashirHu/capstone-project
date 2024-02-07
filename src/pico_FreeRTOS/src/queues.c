#include "FreeRTOS.h"
#include "tasks.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "queue.h"

struct message
{
    double latitude;
    double longitude;
    double speed;
    int message_type;
};

QueueHandle_t xMessage_Queue;


void initQueues(void)
{
	xMessage_Queue = xQueueCreate(800, sizeof(struct message));
}

void message_enqueue(struct message x)
{
    xQueueSendToBack(xMessage_Queue, &x, NULL);
}


int message_queue_dequeue(struct message* x)
{
    if(xQueueReceive(xMessage_Queue, &x, 1000))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
