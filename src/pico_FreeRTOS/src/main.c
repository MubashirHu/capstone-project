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

    // uart_init(UART_ID_4G, BAUD_RATE_UART_4G);
    // gpio_set_function(UART_TX_PIN_4G, GPIO_FUNC_UART);
    // gpio_set_function(UART_RX_PIN_4G, GPIO_FUNC_UART);
    // uart_set_hw_flow(UART_ID_4G, false, false);
    // uart_set_format(UART_ID_4G, DATA_BITS, STOP_BITS, PARITY);
    // uart_set_fifo_enabled(UART_ID_4G, true);


    // i2c_init(I2C_ID_GPS, BAUD_RATE_I2C_GPS);
    // gpio_set_function(I2C_SDA_PIN_GPS, GPIO_FUNC_I2C);
    // gpio_set_function(I2C_SCL_PIN_GPS, GPIO_FUNC_I2C);
    // gpio_pull_up(I2C_SDA_PIN_GPS);
    // gpio_pull_up(I2C_SCL_PIN_GPS);

    // uart_init(UART_ID_OBD2, BAUD_RATE_UART_4G);
    // gpio_set_function(UART_TX_PIN_OBD2, GPIO_FUNC_UART);
    // gpio_set_function(UART_RX_PIN_OBD2, GPIO_FUNC_UART);
    // uart_set_hw_flow(UART_ID_OBD2, false, false);
    // uart_set_format(UART_ID_OBD2, DATA_BITS, STOP_BITS, PARITY);


    // enables uart for OBD2 and testing OBD2 using uart0
    uart_init(UART_ID_OBD2, BAUD_RATE_UART_OBD2);
    gpio_set_function(UART_TX_PIN_OBD2, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN_OBD2, GPIO_FUNC_UART);
    uart_set_hw_flow(UART_ID_OBD2, false, false);

    // Set data format
    uart_set_format(UART_ID_OBD2, DATA_BITS, STOP_BITS, PARITY);

    uart_set_fifo_enabled(UART_ID_OBD2, true);

    uart_init(UART_ID_4G, 115200);
    gpio_set_function(UART_TX_PIN_4G, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN_4G, GPIO_FUNC_UART);
    uart_set_hw_flow(UART_ID_4G, false, false);

    uart_set_format(UART_ID_4G, DATA_BITS, STOP_BITS, PARITY);

    uart_set_fifo_enabled(UART_ID_OBD2, true);

    initQueues();
    initTasks();
    vTaskStartScheduler();

    while(1){};
}