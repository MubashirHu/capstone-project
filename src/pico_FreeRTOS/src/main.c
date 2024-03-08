#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "tasks.h"
#include "queues.h"




int main()
{
    stdio_init_all();

    uart_init(UART_ID_4G, BAUD_RATE_UART_4G);
    gpio_set_function(UART_TX_PIN_4G, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN_4G, GPIO_FUNC_UART);
    uart_set_hw_flow(UART_ID_4G, false, false);
    uart_set_format(UART_ID_4G, DATA_BITS, STOP_BITS, PARITY);
    uart_set_fifo_enabled(UART_ID_4G, true);


    i2c_init(I2C_ID_GPS, BAUD_RATE_I2C_GPS);
    gpio_set_function(I2C_SDA_PIN_GPS, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN_GPS, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN_GPS);
    gpio_pull_up(I2C_SCL_PIN_GPS);

    // uart_init(UART_ID_OBD2, BAUD_RATE_UART_OBD2);
    // gpio_set_function(UART_TX_PIN_OBD2, GPIO_FUNC_UART);
    // gpio_set_function(UART_RX_PIN_OBD2, GPIO_FUNC_UART);
    // uart_set_hw_flow(UART_ID_OBD2, false, false);
    // uart_set_format(UART_ID_OBD2, DATA_BITS, STOP_BITS, PARITY);
    // uart_set_fifo_enabled(UART_ID_OBD2, true);

    uart_init(UART_TEST, 115200);
    if(UART_TEST == uart0)
    {
        gpio_set_function(0, GPIO_FUNC_UART);
        gpio_set_function(1, GPIO_FUNC_UART);
    }
    else
    {
        gpio_set_function(4, GPIO_FUNC_UART);
        gpio_set_function(5, GPIO_FUNC_UART);
    }
    uart_set_hw_flow(UART_TEST, false, false);

    uart_set_format(UART_TEST, DATA_BITS, STOP_BITS, PARITY);

    initQueues();
    initTasks();
    vTaskStartScheduler();

    while(1){};
}