#include "FreeRTOS.h"
#include "tasks.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "queue.h"

struct message
{
    // char id[15];
    double latitude;
    double longitude;
    double speed;
    int message_type;
};


void initQueues(void)
{
	xMessage_Queue = xQueueCreate(800, sizeof(normal_message*));
}

void message_enqueue(message x)
{
    xQueueSendToBack(xNormal_Queue, &x, NULL);
}


int message_queue_dequeue(message* x)
{
    if(xQueueReceive(xNormal_Queue, &x, 1000))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
