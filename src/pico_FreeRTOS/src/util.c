#include "FreeRTOS.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "task.h"
#include <string.h>

int uart_get_response(uart_inst_t *uart, char *response)
{
    int i = 0;
    while(uart_is_readable(uart) && i < 250)
    {
        response[i] = uart_getc(uart);
        i++;
    }
    return i;
}

int uart_validate_response(uart_inst_t *uart, char *response, char *expected_response)
{
    for (int i = 0; i < sizeof(expected_response); i++)
    {
        if(response[i] != expected_response[i])
        {
            return 1;
        }
    }
    return 0;
}

int uart_send(uart_inst_t *uart, char *command, char *response, int wait)
{
    int i = 0;
    for(i = 0; i < strlen(command); i++)
    {
        uart_putc(uart, command[i]);
        response[i] = uart_getc(uart);

    }
    vTaskDelay(wait + 1);
    while(uart_is_readable(uart) && i < 250)
    {
        response[i] = uart_getc(uart);
        i++;
        vTaskDelay(10);
    }
    
    for(int j = 0; j < i; j++)
    {
        uart_putc(uart1, response[j]);
        vTaskDelay(1);
    }
    return i;
}

int uart_send_until_valid(uart_inst_t *uart, char *command, char *response, char* expected_response)
{
    do
    {
        while(uart_is_readable(uart))
        {
            uart_getc(uart);
        }
        uart_send(uart, command, response, 50);
        vTaskDelay(100);

    } while (strncmp(response, expected_response, strlen(expected_response)) == 1);

    uart_puts(uart1, "end_of_command\r\n");
    return 0;
}

int uart_read_chars(uart_inst_t *uart, char *command, int num_char_to_read, char *response)
{
    while(uart_is_readable(uart))
    {
        uart_getc(uart);
    }
    uart_puts(uart, command);
    for (int i = 0; i < num_char_to_read; i++)
    {
        response[i] = uart_getc(uart);
    }
    uart_puts(uart, "\r\n");
}

void uart_obd2_wheel_speed(uart_inst_t *uart, uint16_t wheel_1, uint16_t wheel_2, uint16_t wheel_3, uint16_t wheel_4)
{
    char response[20];
    uart_read_chars(uart, "at cra 0b0\r\n", 13, "");

}

