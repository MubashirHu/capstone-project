#include "FreeRTOS.h"
#include "tasks.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"



static int chars_rxed = 0;

static void vTaskUart_4g(void * parameters);
void vTaskUart_OBD(void * parameters);
void vTaskI2C_GPS(void * parameters);
static void led_task(void * parameters);

void initTasks(void)
{
	//xTaskCreate(vTaskUart_4g, "4G_Task", 256, NULL, 1, NULL);
	//xTaskCreate(vTaskUart_OBD, "OBD2_Task", 256, NULL, 1, NULL);
    xTaskCreate(vTaskI2C_GPS, "GPS_Task", 256, NULL, 1, NULL);
    //xTaskCreate(led_task, "LED_Task", 256, NULL, 1, NULL);
}

void vTaskUart_4g(void * parameters)
{
    uart_init(UART_ID_4G, BAUD_RATE_UART_4G);
    gpio_set_function(UART_TX_PIN_4G, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN_4G, GPIO_FUNC_UART);
    uart_set_hw_flow(UART_ID_4G, false, false);

    uart_set_format(UART_ID_4G, DATA_BITS, STOP_BITS, PARITY);

    uart_set_fifo_enabled(UART_ID_4G, true);


    // TODO: Send initialization commands to 4g module and wait for response  
    uart_puts(UART_ID_4G, "AT+HTTPINIT\n");
    // get response
    uart_puts(UART_ID_4G, "AT+HTTPPARA=\"URL\",\"https://test-f1e70.firebaseio.com/pothole.json\"\n");
    // get response
    uart_puts(UART_ID_4G, "AT+HTTPACTION=0\n");
    // get response
    uart_puts(UART_ID_4G, "AT+HTTPREAD=0,290\n");
    // get response
    uart_puts(UART_ID_4G, "AT+HTTPTERM\n");
    // get response

    while (1)
        // TODO: Check relavent queues and if certain ones are not empty, send http request else block
        tight_loop_contents();
}

void vTaskUart_OBD(void * parameters)
{
    uart_init(UART_ID_OBD2, BAUD_RATE_UART_OBD2);
    gpio_set_function(UART_TX_PIN_OBD2, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN_OBD2, GPIO_FUNC_UART);
    uart_set_hw_flow(UART_ID_OBD2, false, false);

    // Set data format
    uart_set_format(UART_ID_OBD2, DATA_BITS, STOP_BITS, PARITY);

    uart_set_fifo_enabled(UART_ID_OBD2, true);

    uart_puts(UART_ID_OBD2, "atz");

    while (1)
        tight_loop_contents();
        
}

void vTaskI2C_GPS(void * parameters)
{
    // Initialize UART0 with a baud rate of 115200
    uart_init(UART_TX_PIN_4G, BAUD_RATE_UART_4G);
    // Set UART0 TX (transmit) pin to GPIO 0 and RX (receive) pin to GPIO 1
    gpio_set_function(UART_TX_PIN_4G, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN_4G, GPIO_FUNC_UART);

    uart_puts(UART_ID_4G, "Test1");
    // i2c_init(I2C_ID_GPS, BAUD_RATE_I2C_GPS);
    // gpio_set_function(I2C_SDA_PIN_GPS, GPIO_FUNC_I2C);
    // gpio_set_function(I2C_SCL_PIN_GPS, GPIO_FUNC_I2C);
    // gpio_pull_up(I2C_SDA_PIN_GPS);
    // gpio_pull_up(I2C_SCL_PIN_GPS);
    //uint8_t data[32];
    uart_puts(UART_ID_4G, "Test2");
    while (1)
        // i2c_write_blocking(I2C_ID_GPS, 0x42, "\xFD\xFE", 2, false);
        // vTaskDelay(pdMS_TO_TICKS(100));
        // i2c_read_blocking(I2C_ID_GPS, 0x42, data, 2, false);

        uart_puts(UART_ID_4G, "GPS Data Bytes: ");
        // for (int i = 0; i < 2; i++)
        // {
        //     uart_puts(UART_ID_4G, (char *)data[i]);
        // }
        //uart_puts(UART_ID_4G, "\n\r");
        uart_puts(UART_ID_4G, "Test2");
        printf("Test2");

        vTaskDelay(pdMS_TO_TICKS(10));
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


