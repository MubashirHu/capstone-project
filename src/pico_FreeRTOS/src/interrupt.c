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

int byte_pico1 = 0;
int byte_pico2 = 0;

void handle_pothole_interrupt(){

    // check the pins to determine which pico has caused the interrupt 
    int check_pin_1_for_change = gpio_get(GPIO_PIN_PH_PICO1);
    int check_pin_2_for_change = gpio_get(GPIO_PIN_PH_PICO2);

    if (check_pin_1_for_change == 0) 
    {
        pico1_interrupt = true;
        // reset value of byte on entry
        byte_pico1 = 0;

        // get the value passed by the pico 
        int BIT_0_PICO1_state = gpio_get(BIT_0_PICO1);
        int BIT_1_PICO1_state = gpio_get(BIT_1_PICO1);
        int BIT_2_PICO1_state = gpio_get(BIT_2_PICO1);

        // Pack the states into a single byte
        byte_pico1 = (BIT_2_PICO1_state << 2) | (BIT_1_PICO1_state << 1) | BIT_0_PICO1_state;
    }

    if (check_pin_2_for_change == 0) 
    {
        pico2_interrupt = true;
        // reset value of byte on entry
        byte_pico2 = 0;
        
        // get the value passed by the pico 
        int BIT_0_PICO1_state = gpio_get(BIT_0_PICO1);
        int BIT_1_PICO1_state = gpio_get(BIT_1_PICO1);
        int BIT_2_PICO1_state = gpio_get(BIT_2_PICO1);

        // Pack the states into a single byte
        byte_pico2 = (BIT_2_PICO1_state << 2) | (BIT_1_PICO1_state << 1) | BIT_0_PICO1_state;
    }
}
