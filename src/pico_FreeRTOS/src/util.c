#include "FreeRTOS.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "task.h"
#include "tasks.h"
#include "util.h"
#include <string.h>

int uart_send(uart_inst_t *uart, char *command, char *response, int wait)
{
    int i = 0;
    for(i = 0; i < strlen(command); i++)
    {
        uart_putc(uart, command[i]);
        response[i] = uart_getc(uart);
        uart_putc(UART_TEST, response[i]);

    }
    vTaskDelay(wait + 1);
    while(uart_is_readable(uart) && i < 150)
    {
        response[i] = uart_getc(uart);
        uart_putc(UART_TEST, response[i]);
        i++;
        // vTaskDelay(15);
    }
    
    // uart_puts(UART_TEST, "\r\n");
    // for(int j = 0; j < i; j++)
    // {
    //     char ascii_code[4];
    //     sprintf(ascii_code, "%02X ", response[j]);
    //     uart_puts(UART_TEST, ascii_code);
    // }
    // uart_puts(UART_TEST, "\r\n");
    return i;
}

int uart_send1(uart_inst_t *uart, char *command, char *response, int wait)
{
    while(uart_is_readable(uart))
    {
        uart_getc(uart);
    }
    // uart_puts(UART_TEST, "\r\nSTART: \r\n");
    uart_puts(uart, command);
    uart_read_blocking(uart, response, 75);
    // for(int j = 0; j < 101; j++)
    // {
    //     //char ascii_code[4];
    //     //sprintf(ascii_code, "%02X ", response[j]);
    //     //uart_puts(UART_TEST, ascii_code);
    //     uart_is_readable_within_us(uart, 2000);
    //     response[j] = uart_getc(uart);
    // }
    // uart_puts(UART_TEST, response);
    // uart_puts(UART_TEST, "\r\n END \r\n");
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
        vTaskDelay(100);

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

void uart_obd2_wheel_speed(uart_inst_t *uart, struct obd2_packet *packet)
{
    uint16_t high_nibble, low_nibble;

    high_nibble = low_nibble = 0;

    char response1[26];
    char response2[26];
    char response3[32];
    char response4[32];
    char response5[32];
    char response6[15];

    uart_send_until_valid(uart, "AT CRA 3B7\r\n", response5, "AT CRA 3B7\rOK\r\r>");
    do
    {
        uart_read_chars(uart, "AT MA\r\n", 31, response5);
    } while (strncmp(response5, "AT MA\r", 6) != 0 && response5[29] == '\r');


    do
    {
        uart_read_chars(uart, "010D\r\n", 14, response6);
    } while (strncmp(response6, "010D\r", 5) != 0);

    uart_puts(UART_TEST, response5);

    sscanf(response5 + 9, "%2hx", &high_nibble);
    packet->slipping = high_nibble;

    sscanf(response6 + 11, "%2hx", &high_nibble);
    packet->vehicle_speed = high_nibble;
    //for vehicle speed, send 010D, expect response 010D\r41 0D xx\r


}
