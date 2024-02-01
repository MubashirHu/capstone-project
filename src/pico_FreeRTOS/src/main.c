#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "tasks.h"



int main()
{
    stdio_init_all();
    initTasks();
    vTaskStartScheduler();

    while(1){};
}