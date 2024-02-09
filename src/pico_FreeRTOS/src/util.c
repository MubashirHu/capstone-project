#include "FreeRTOS.h"
#include <stdio.h>
#include "pico/stdlib.h"


int uart_validate_response(uart_inst_t *uart, char *response, char* expected_response)
{

}

void uart_get_response(uart_inst_t *uart, char *response)
{
    int i = 0;
    while(uart_is_readable() && i < 250)
    {
        response[i] = uart_getc(uart);
        i++;
    }
}
