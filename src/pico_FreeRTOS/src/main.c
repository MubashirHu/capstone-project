#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "tasks.h"
#include "queues.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "interrupt.h"



int main()
{
    stdio_init_all();

    // uart_init(UART_ID_4G, BAUD_RATE_UART_4G);
    // gpio_set_function(UART_TX_PIN_4G, GPIO_FUNC_UART);
    // gpio_set_function(UART_RX_PIN_4G, GPIO_FUNC_UART);
    // uart_set_hw_flow(UART_ID_4G, false, false);
    // uart_set_format(UART_ID_4G, DATA_BITS, STOP_BITS, PARITY);
    // uart_set_fifo_enabled(UART_ID_4G, true);


    i2c_init(I2C_ID_GPS, BAUD_RATE_I2C_GPS);
    gpio_set_function(I2C_SDA_PIN_GPS, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN_GPS, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN_GPS);
    gpio_pull_up(I2C_SCL_PIN_GPS);

    uart_init(UART_ID_OBD2, BAUD_RATE_UART_OBD2);
    gpio_set_function(UART_TX_PIN_OBD2, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN_OBD2, GPIO_FUNC_UART);
    uart_set_hw_flow(UART_ID_OBD2, false, false);
    uart_set_format(UART_ID_OBD2, DATA_BITS, STOP_BITS, PARITY);
    uart_set_fifo_enabled(UART_ID_OBD2, true);

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


        // Initialize GPIO pin - pothole
    gpio_init(GPIO_PIN_PH_1);
    gpio_set_dir(GPIO_PIN_PH_1, GPIO_IN);
    gpio_pull_up(GPIO_PIN_PH_1); // GPIO_PIN_PH by default

    gpio_init(BIT1_0);
    gpio_set_dir(BIT1_0, GPIO_IN);
    gpio_pull_up(BIT1_0); // GPIO_PIN_PH by default

    gpio_init(BIT1_1);
    gpio_set_dir(BIT1_1, GPIO_IN);
    gpio_pull_up(BIT1_1); // GPIO_PIN_PH by default

    gpio_init(BIT1_2);
    gpio_set_dir(BIT1_2, GPIO_IN);
    gpio_pull_up(BIT1_2); // GPIO_PIN_PH by default
    
    // Set up interrupt handler for pothole & road-depression - triggers on HIGH to LOW event on selected GPIO
    gpio_set_irq_enabled_with_callback(GPIO_PIN_PH_1, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &handle_pothole_interrupt);
    gpio_set_irq_enabled(GPIO_PIN_PH_2, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);

    initQueues();
    initTasks();
    vTaskStartScheduler();

    while(1){};
}