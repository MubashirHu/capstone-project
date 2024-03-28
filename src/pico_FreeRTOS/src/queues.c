#include "FreeRTOS.h"
#include "tasks.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "queue.h"
#include "queues.h"


QueueHandle_t xMessage_Queue;
QueueHandle_t xGPS_Queue;
QueueHandle_t xVehicle_Speed_Queue;

void initQueues(void)
{
	xMessage_Queue = xQueueCreate(400, sizeof(struct message));
    xGPS_Queue = xQueueCreate(1, sizeof(struct gps));
    xVehicle_Speed_Queue = xQueueCreate(1, sizeof(uint8_t));
}

int message_enqueue(struct message x)
{
    if(xQueueSendToBack(xMessage_Queue, &x, NULL) == pdTRUE)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}


int message_queue_dequeue(struct message* x)
{
    if(xQueueReceive(xMessage_Queue, x, 0) == pdTRUE)
    {
        // uart_puts(UART_TEST, "starting dequeue\r\n");
        // static char json[512];
        // sprintf(json, "\r\nQueue Content \"latitude\":%.6lf,\"longitude\":%.6lf, \"message_type\": %d\r\n", x->latitude, x->longitude, x->message_type);
        // uart_puts(UART_TEST, json);
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
    if(xQueuePeek(xGPS_Queue, x, 0) == pdTRUE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void vehicle_speed_queue_overwrite(uint8_t speed)
{
    xQueueOverwrite(xVehicle_Speed_Queue, &speed);
}

int vehicle_speed_queue_peek(uint8_t *speed)
{
    if(xQueuePeek(xVehicle_Speed_Queue, speed, 0) == pdTRUE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
