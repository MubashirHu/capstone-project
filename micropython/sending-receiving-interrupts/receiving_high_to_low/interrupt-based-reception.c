#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"

// Define GPIO pin
#define GPIO_PIN_PH 19  // GPIO PIN - Pothole GP19
#define BIT_0 18
#define BIT_1 17
#define BIT_2 16

void handle_pothole_interrupt(){
    printf("Pothole Interrupt on pin %d\r\n", GPIO_PIN_PH);
    __int8_t byte = 0;
    
    int bit_0_state = gpio_get(BIT_0);
    int bit_1_state = gpio_get(BIT_1);
    int bit_2_state = gpio_get(BIT_2);

    // Pack the states into a single byte
    byte = (bit_2_state << 2) | (bit_1_state << 1) | bit_0_state;

    printf("byte:%d\r\n", byte);

    switch (byte)
    {
    case 3:
        printf("RED ZONE\r\n");
        break;
    case 2:
        printf("AMBER_ZONE\r\n");
        break;
    case 1:
        printf("YELLOW_ZONE\r\n");
        break;
    case 0:
        printf("GREEN_ZONE\r\n");
        break;
    default:
        printf("GREEN_ZONE\r\n");
        break;
    }

    //get GPS data
    //determine potlevel
    //send speed 
}

int main() {
    stdio_init_all();

    // Initialize UART0
    uart_init(uart0, 115200);
    gpio_set_function(0, GPIO_FUNC_UART); // Set UART0 TX Pin
    gpio_set_function(1, GPIO_FUNC_UART); // Set UART0 RX Pin
    uart_set_hw_flow(uart0, false, false);
    uart_set_format(uart0, 8, 1, UART_PARITY_NONE);

    // Initialize GPIO pin - pothole
    gpio_init(GPIO_PIN_PH);
    gpio_set_dir(GPIO_PIN_PH, GPIO_IN);
    gpio_pull_up(GPIO_PIN_PH); // GPIO_PIN_PH by default

    gpio_init(BIT_0);
    gpio_set_dir(BIT_0, GPIO_IN);
    gpio_pull_up(BIT_0); // GPIO_PIN_PH by default

    gpio_init(BIT_1);
    gpio_set_dir(BIT_1, GPIO_IN);
    gpio_pull_up(BIT_1); // GPIO_PIN_PH by default

    gpio_init(BIT_2);
    gpio_set_dir(BIT_2, GPIO_IN);
    gpio_pull_up(BIT_2); // GPIO_PIN_PH by default
    
    // Set up interrupt handler for pothole & road-depression - triggers on HIGH to LOW event on selected GPIO
    gpio_set_irq_enabled_with_callback(GPIO_PIN_PH, GPIO_IRQ_EDGE_FALL, true, &handle_pothole_interrupt);

    while(1)
    {

    }

}
