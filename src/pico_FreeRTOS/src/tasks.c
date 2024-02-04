#include "FreeRTOS.h"
#include "tasks.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"



static int chars_rxed = 0;

static void vTaskUart_4g(void * parameters);
static void led_task(void * parameters);

void initTasks(void)
{
	xTaskCreate(vTaskUart_4g, "4G", 256, NULL, 1, NULL);
	xTaskCreate(led_task, "LED_Task", 256, NULL, 1, NULL);
}

void vTaskUart_4g(void * parameters)
{
    uart_init(UART_ID_4G, BAUD_RATE_4G);
    gpio_set_function(UART_TX_PIN_4G, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN_4G, GPIO_FUNC_UART);
    uart_set_hw_flow(UART_ID_4G, false, false);

    uart_set_format(UART_ID_4G, DATA_BITS, STOP_BITS, PARITY);

    uart_set_fifo_enabled(UART_ID_4G, true);


    // TODO: Send initialization commands to 4g module and wait for response  
    uart_puts(UART_ID_4G, "\nHello, uart interrupts\n");

    while (1)
        // TODO: Check relavent queues and if certain ones are not empty, send http request else block
        tight_loop_contents();
}

void vTaskUart_OBD(void * parameters)
{
    uart_init(UART_ID_OBD2, BAUD_RATE_OBD2);
    gpio_set_function(UART_TX_PIN_OBD2, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN_OBD2, GPIO_FUNC_UART);
    uart_set_hw_flow(UART_ID_OBD2, false, false);

    // Set data format
    uart_set_format(UART_ID_OBD2, DATA_BITS, STOP_BITS, PARITY);

    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled(UART_ID_OBD2, true);

    uart_puts(UART_ID_OBD2, "\nHello, uart interrupts\n");

    while (1)
        tight_loop_contents();
}

void led_task(void * parameters)
{   
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {
        gpio_put(LED_PIN, 1);
        vTaskDelay(100);
        gpio_put(LED_PIN, 0);
        vTaskDelay(100);
    }
}


