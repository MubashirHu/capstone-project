#include "FreeRTOS.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "task.h"
#include <string.h>

int uart_send(uart_inst_t *uart, char *command, char *response, int wait)
{
    int i = 0;
    for(i = 0; i < strlen(command); i++)
    {
        uart_putc(uart, command[i]);
        response[i] = uart_getc(uart);

    }
    vTaskDelay(wait + 1);
    while(uart_is_readable(uart) && i < 150)
    {
        response[i] = uart_getc(uart);
        i++;
        // vTaskDelay(15);
    }
    
    // for(int j = 0; j < i; j++)
    // {
    //     //char ascii_code[4];
    //     //sprintf(ascii_code, "%02X ", response[j]);
    //     //uart_puts(UART_TEST, ascii_code);
    //     uart_putc(UART_TEST, response[j]);
    // }
    return i;
}

int uart_send1(uart_inst_t *uart, char *command, char *response, int wait)
{
    uart_puts(uart, command);
    uart_read_blocking(uart, response, 91);
    // for(int j = 0; j < 91; j++)
    // {
    //     //char ascii_code[4];
    //     //sprintf(ascii_code, "%02X ", response[j]);
    //     //uart_puts(UART_TEST, ascii_code);
    //     // uart_putc(UART_TEST, response[j]);
    // }
    return 91;
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
        // vTaskDelay(100);

   } while (strncmp(response, expected_response, strlen(expected_response)) != 0);

    // uart_puts(UART_TEST, "end_of_command\r\n");
    return 0;
}

int uart_read_chars(uart_inst_t *uart, char *command, int num_char_to_read, char *response)
{
    while(uart_is_readable(uart))
    {
        uart_getc(uart);
    }
    uart_puts(uart, command);
    uart_read_blocking(uart, response, num_char_to_read);
    // for (int i = 0; i < num_char_to_read; i++)
    // {
    //     response[i] = uart_getc(uart);
    //     //uart_putc(UART_TEST, response[i]);
    // }
    uart_puts(uart, "\r");
    char check;
    do
    {
        check = uart_getc(uart);
    }while(check != '>');
}

void uart_obd2_wheel_speed(uart_inst_t *uart, uint16_t *wheel_1, uint16_t *wheel_2, uint16_t *wheel_3, uint16_t *wheel_4, uint16_t *brake_pressure, uint8_t *vechicle_speed)
{
    uint16_t high_nibble, low_nibble;

    char response[30];
    uart_send_until_valid(uart, "AT CRA 0B0\r\n", response, "AT CRA 0B0\rOK\r\r>");
    do
    {
        uart_read_chars(uart, "AT MA\r\n", 25, response);
    } while (strncmp(response, "AT MA\r", 6) != 0 && response[24] != '<');
    // also check for error frames
    
    sscanf(response + 6, "%2hx %2hx", &high_nibble, &low_nibble);
    *wheel_1 = (high_nibble << 8) | low_nibble;
    sscanf(response + 12, "%2hx %2hx", &high_nibble, &low_nibble);
    *wheel_2 = (high_nibble << 8) | low_nibble;
    uart_send_until_valid(uart, "AT CRA 0B2\r\n", response, "AT CRA 0B2\rOK\r\r>");
    do
    {
        uart_read_chars(uart, "AT MA\r\n", 24, response);
    } while (strncmp(response, "AT MA\r", 6) != 0);
    sscanf(response + 6, "%2hx %2hx", &high_nibble, &low_nibble);
    *wheel_3 = (high_nibble << 8) | low_nibble;
    sscanf(response + 12, "%2hx %2hx", &high_nibble, &low_nibble);
    *wheel_4 = (high_nibble << 8) | low_nibble;

    uart_send_until_valid(uart, "AT CRA 224\r\n", response, "AT CRA 224\rOK\r\r>");
    do
    {
        uart_read_chars(uart, "AT MA\r\n", 29, response);
    } while (strncmp(response, "AT MA\r", 6) != 0);
    sscanf(response + 18, "%2hx %2hx", &high_nibble, &low_nibble);
    *brake_pressure = (high_nibble << 8) | low_nibble;

    //for vehicle speed, send 010D, expect response 41 0D xx\r


}

