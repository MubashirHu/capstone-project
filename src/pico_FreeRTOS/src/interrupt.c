#include "interrupt.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "queues.h"
#include "tasks.h"

bool pico1_interrupt = false;
bool pico2_interrupt = false;

uint8_t byte_pico1 = 0;
uint8_t byte_pico2 = 0;

void handle_pothole_interrupt(){

    // check the pins to determine which pico has caused the interrupt 
    int check_pin_1_for_change = gpio_get(GPIO_PIN_PH_PICO1);
    int check_pin_2_for_change = gpio_get(GPIO_PIN_PH_PICO2);
    // printf("check_pin_1_for_change value:%d\n", check_pin_1_for_change);
    // printf("check_pin_2_for_change value:%d\n", check_pin_2_for_change);
    // uart_puts(UART_TEST, "\r\nTest3\r\n");

    if (check_pin_1_for_change == 0) 
    {
        pico1_interrupt = true;
        // uart_puts(UART_TEST, "Interrupt on PICO1\n");
        // reset value of byte on entry
        __int8_t byte_pico1 = 0;

        // get the value passed by the pico 
        int BIT_0_PICO1_state = gpio_get(BIT_0_PICO1);
        int BIT_1_PICO1_state = gpio_get(BIT_1_PICO1);
        int BIT_2_PICO1_state = gpio_get(BIT_2_PICO1);

        // Pack the states into a single byte
        byte_pico1 = (BIT_2_PICO1_state << 2) | (BIT_1_PICO1_state << 1) | BIT_0_PICO1_state;

        // display the value of the byte
        // uart_puts(UART_TEST, "byte_pico1:");
        // char buffer[20]; // Allocate a buffer to hold the formatted string
        // snprintf(buffer, sizeof(buffer), "%d", byte_pico1);
        // uart_puts(UART_TEST, buffer);
        // uart_puts(UART_TEST, "\r\n");


        switch (byte_pico1)
        {
            case 3:
                // uart_puts(UART_TEST,"RED ZONE\r\n");
                break;
            case 2:
                // uart_puts(UART_TEST,"AMBER_ZONE\r\n");
                break;
            case 1:
                // uart_puts(UART_TEST,"YELLOW_ZONE\r\n");
                break;
            case 0:
                // uart_puts(UART_TEST,"GREEN_ZONE\r\n");
                break;
            default:
                // uart_puts(UART_TEST,"GREEN_ZONE\r\n");
                break;
        }
    }

    if (check_pin_2_for_change == 0) 
    {
        pico2_interrupt = true;
        // uart_puts(UART_TEST,"Interrupt on PICO2\n");
        // reset value of byte on entry
        __int8_t byte_pico1 = 0;
        
        // get the value passed by the pico 
        int BIT_0_PICO1_state = gpio_get(BIT_0_PICO1);
        int BIT_1_PICO1_state = gpio_get(BIT_1_PICO1);
        int BIT_2_PICO1_state = gpio_get(BIT_2_PICO1);

        // Pack the states into a single byte
        byte_pico2 = (BIT_2_PICO1_state << 2) | (BIT_1_PICO1_state << 1) | BIT_0_PICO1_state;

        // display the value of the byte
        // uart_puts(UART_TEST, "byte_pico2:");
        // char buffer[20]; // Allocate a buffer to hold the formatted string
        // snprintf(buffer, sizeof(buffer), "%d", byte_pico2);
        // uart_puts(UART_TEST, buffer);
        // uart_puts(UART_TEST, "\r\n");

        switch (byte_pico2)
        {
            case 3:
                // uart_puts(UART_TEST,"RED ZONE\r\n");
                break;
            case 2:
                // uart_puts(UART_TEST,"AMBER_ZONE\r\n");
                break;
            case 1:
                // uart_puts(UART_TEST,"YELLOW_ZONE\r\n");
                break;
            case 0:
                // uart_puts(UART_TEST,"GREEN_ZONE\r\n");
                break;
            default:
                // uart_puts(UART_TEST,"GREEN_ZONE\r\n");
                break;
        }
    }
}
