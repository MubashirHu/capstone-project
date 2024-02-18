#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"

// Define GPIO pin
#define GPIO_PIN_PH 19  // GPIO PIN - Pothole
#define GPIO_PIN_RD 18 // GPIO PIN - Road Depression

void handle_pothole_interrupt(){
    printf("Pothole Event happened on pin %d/n", GPIO_PIN_PH);
}

// // Define interrupt handler function
// void handle_road_depression_interrupt() {
//     printf("Interrupt occurred on pin %d\n", GPIO_PIN_RD);
// }

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
    gpio_pull_up(GPIO_PIN_PH); // high by default

    // // Initialize GPIO pin - road depression
    // gpio_init(GPIO_PIN_RD);
    // gpio_set_dir(GPIO_PIN_RD, GPIO_IN);
    // gpio_pull_up(GPIO_PIN_RD); // high by default

     // Set up interrupt handler for pothole & road-depression
    gpio_set_irq_enabled_with_callback(GPIO_PIN_PH, GPIO_IRQ_EDGE_FALL, true, &handle_pothole_interrupt);
    //gpio_set_irq_enabled_with_callback(GPIO_PIN_RD, GPIO_IRQ_EDGE_FALL, true, &handle_road_depression_interrupt);

    bool previous_state_pothole_detection_pin = true;  // Initialize previous state as high
    //bool previous_state_road_depression_pin = true;  // Initialize previous state as high

    while(1)
    {
        // Read current state of GPIO pin
        bool current_state_pothole_detection_pin = gpio_get(GPIO_PIN_PH);
        //bool current_state_road_depression_pin = gpio_get(GPIO_PIN_RD);

        // Check for high-to-low transition
        if (previous_state_pothole_detection_pin && !current_state_pothole_detection_pin) {
            // High-to-low transition detected
            printf("High-to-low transition detected on pin %d\n", GPIO_PIN_PH);
        } 
        // else if(previous_state_road_depression_pin && !current_state_road_depression_pin)
        // {
        //     printf("High-to-low transition detected on pin %d\n", GPIO_PIN_RD);
        // }

        // Update previous state for the next iteration
        previous_state_pothole_detection_pin = current_state_pothole_detection_pin;
        //previous_state_road_depression_pin = current_state_road_depression_pin;

        // Add a delay to control the loop frequency
        sleep_ms(10);  // Sleep for 10 milliseconds (adjust as needed)
    }

}

// Bundle the data of : 
                // GPS coordinate
                // pothole or road depression event
                // speed from gps queue
