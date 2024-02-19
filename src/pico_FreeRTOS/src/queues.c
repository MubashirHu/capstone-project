#include "FreeRTOS.h"
#include "tasks.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "queue.h"
#include "queues.h"


QueueHandle_t xMessage_Queue;
QueueHandle_t xGPS_Queue;


void initQueues(void)
{
	xMessage_Queue = xQueueCreate(800, sizeof(struct message));
    xGPS_Queue = xQueueCreate(1, sizeof(struct gps));
}

void message_enqueue(struct message x)
{
    xQueueSendToBack(xMessage_Queue, &x, NULL);
}


int message_queue_dequeue(struct message* x)
{
    if(xQueueReceive(xMessage_Queue, &x, 10) == pdTRUE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void gps_queue_overwrite(struct gps x)
{
    xQueueOverwrite(xGPS_Queue, &x);
}

int gps_queue_peek(struct gps* x)
{
    if(xQueuePeek(xGPS_Queue, &x, 0) == pdTRUE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
