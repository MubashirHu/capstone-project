#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "FreeRTOS.h"
#include "task.h"

// Define GPIO pins for Pico 1
#define GPIO_PIN_PH_PICO1 16  // GPIO PIN - Pothole GP19
#define BIT_0_PICO1 18
#define BIT_1_PICO1 17
#define BIT_2_PICO1 16

#define GPIO_PIN_PH_PICO2 17  // GPIO PIN - Pothole GP19
#define BIT_0_PICO2 18
#define BIT_1_PICO2 17
#define BIT_2_PICO2 16

// Flags to indicate interrupts on each Pico
volatile bool pico1_interrupt = false;
volatile bool pico2_interrupt = false;

// Task handles
TaskHandle_t led_task_handle;

void handle_pothole_interrupt(){

    // check the pins to determine which pico has caused the interrupt 
    int pico_1_interrupted = gpio_get(GPIO_PIN_PH_PICO1);
    int pico_2_interrupted = gpio_get(GPIO_PIN_PH_PICO2);
    printf("pico_1_interrupted value:%d\n", pico_1_interrupted);
    printf("pico_2_interrupted value:%d\n", pico_2_interrupted);

    if (pico_1_interrupted == 0) {
        printf("Interrupt on PICO1\n");
        // reset value of byte on entry
        __int8_t byte_pico1 = 0;
        
        // get the value passed by the pico 
        int BIT_0_PICO1_state = gpio_get(BIT_0_PICO1);
        int BIT_1_PICO1_state = gpio_get(BIT_1_PICO1);
        int BIT_2_PICO1_state = gpio_get(BIT_2_PICO1);

        // Pack the states into a single byte
        byte_pico1 = (BIT_2_PICO1_state << 2) | (BIT_1_PICO1_state << 1) | BIT_0_PICO1_state;

        // display the value of the byte
        printf("byte_pico1:%d\r\n", byte_pico1);

        switch (byte_pico1)
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
    }
    else if (pico_2_interrupted == 0) {
        printf("Interrupt on PICO2\n");
        // reset value of byte on entry
        __int8_t byte_pico1 = 0;
        
        // get the value passed by the pico 
        int BIT_0_PICO1_state = gpio_get(BIT_0_PICO1);
        int BIT_1_PICO1_state = gpio_get(BIT_1_PICO1);
        int BIT_2_PICO1_state = gpio_get(BIT_2_PICO1);

        // Pack the states into a single byte
        byte_pico1 = (BIT_2_PICO1_state << 2) | (BIT_1_PICO1_state << 1) | BIT_0_PICO1_state;

        // display the value of the byte
        printf("byte_pico2:%d\r\n", byte_pico1);

        switch (byte_pico1)
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
    }
}

// LED task
void led_task(void *pvParameters) {
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (true) {
        gpio_put(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_put(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
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
    gpio_init(GPIO_PIN_PH_PICO1);
    gpio_set_dir(GPIO_PIN_PH_PICO1, GPIO_IN);
    gpio_pull_up(GPIO_PIN_PH_PICO1); // GPIO_PIN_PH_PICO1 by default

    gpio_init(BIT_0_PICO1);
    gpio_set_dir(BIT_0_PICO1, GPIO_IN);
    gpio_pull_up(BIT_0_PICO1); // GPIO_PIN_PH_PICO1 by default

    gpio_init(BIT_1_PICO1);
    gpio_set_dir(BIT_1_PICO1, GPIO_IN);
    gpio_pull_up(BIT_1_PICO1); // GPIO_PIN_PH_PICO1 by default

    gpio_init(BIT_2_PICO1);
    gpio_set_dir(BIT_2_PICO1, GPIO_IN);
    gpio_pull_up(BIT_2_PICO1); // GPIO_PIN_PH_PICO1 by default
    
    // Set up interrupt handler for pothole & road-depression - triggers on HIGH to LOW event on selected GPIO
    gpio_set_irq_enabled_with_callback(GPIO_PIN_PH_PICO1, GPIO_IRQ_EDGE_FALL, true, &handle_pothole_interrupt);
    gpio_set_irq_enabled_with_callback(GPIO_PIN_PH_PICO2, GPIO_IRQ_EDGE_FALL, true, &handle_pothole_interrupt);
    
    while(1)
    {

        // Create LED task
        xTaskCreate(led_task, "LED Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &led_task_handle);

        // Start FreeRTOS scheduler
        vTaskStartScheduler();
        return 0;

    }

}
