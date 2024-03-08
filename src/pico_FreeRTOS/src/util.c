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
    uart_send_until_valid(uart, "AT CRA 0B0\r\n", response1, "AT CRA 0B0\rOK\r\r>");
    do
    {
        uart_read_chars(uart, "AT MA\r\n", 25, response1);
    } while (strncmp(response1, "AT MA\r", 6) != 0 && response1[23] == '\r');

    uart_send_until_valid(uart, "AT CRA 0B2\r\n", response2, "AT CRA 0B2\rOK\r\r>");
    do
    {
        uart_read_chars(uart, "AT MA\r\n", 25, response2);
    } while (strncmp(response2, "AT MA\r", 6) != 0 && response2[23] == '\r');

    uart_send_until_valid(uart, "AT CRA 224\r\n", response3, "AT CRA 224\rOK\r\r>");
    do
    {
        uart_read_chars(uart, "AT MA\r\n", 31, response3);
    } while (strncmp(response3, "AT MA\r", 6) != 0 && response3[29] == '\r');

    uart_send_until_valid(uart, "AT CRA 2C4\r\n", response3, "AT CRA 2C4\rOK\r\r>");
    do
    {
        uart_read_chars(uart, "AT MA\r\n", 31, response4);
    } while (strncmp(response4, "AT MA\r", 6) != 0 && response4[29] == '\r');

    uart_send_until_valid(uart, "AT CRA 3B7\r\n", response5, "AT CRA 3B7\rOK\r\r>");
    do
    {
        uart_read_chars(uart, "AT MA\r\n", 31, response5);
    } while (strncmp(response5, "AT MA\r", 6) != 0 && response5[29] == '\r');


    do
    {
        uart_read_chars(uart, "010D\r\n", 14, response6);
    } while (strncmp(response6, "010D\r", 5) != 0);

    sscanf(response1 + 6, "%2hx %2hx", &high_nibble, &low_nibble);
    packet->wheel_1 = ((high_nibble << 8) | low_nibble) * 0.01;
    sscanf(response1 + 12, "%2hx %2hx", &high_nibble, &low_nibble);
    packet->wheel_2 = ((high_nibble << 8) | low_nibble) * 0.01;
    
    sscanf(response2 + 6, "%2hx %2hx", &high_nibble, &low_nibble);
    packet->wheel_3 = ((high_nibble << 8) | low_nibble) * 0.01;
    sscanf(response2 + 12, "%2hx %2hx", &high_nibble, &low_nibble);
    packet->wheel_4 = ((high_nibble << 8) | low_nibble) * 0.01;

    sscanf(response3 + 18, "%2hx %2hx", &high_nibble, &low_nibble);
    packet->brake_pressure = (high_nibble << 8) | low_nibble;

    sscanf(response4 + 6, "%2hx %2hx", &high_nibble, &low_nibble);
    packet->rpm = (high_nibble << 8) | low_nibble;

    uart_puts(UART_TEST, response5);

    sscanf(response6 + 11, "%2hx", &high_nibble);
    packet->vehicle_speed = high_nibble;
    //for vehicle speed, send 010D, expect response 010D\r41 0D xx\r


}

