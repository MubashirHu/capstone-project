#include "FreeRTOS.h"
#include <stdio.h>
#include "pico/stdlib.h"
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
        vTaskDelay(1);
    }
    vTaskDelay(wait + 1);
    while(uart_is_readable(uart) && i < 250)
    {
        response[i] = uart_getc(uart);
        i++;
        vTaskDelay(100);
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
        uart_send(uart, command, response, 0);
        vTaskDelay(10);

    } while (strncmp(response, expected_response, strlen(expected_response)) == 1);

    uart_puts(uart1, "\r\n");
    return 0;
}


