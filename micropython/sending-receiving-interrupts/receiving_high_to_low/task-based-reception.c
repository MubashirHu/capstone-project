#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "FreeRTOS.h"
#include "task.h"

// Define GPIO pin
#define GPIO_PIN_PH 19  // GPIO PIN - Pothole
#define GPIO_PIN_RD 18 // GPIO PIN - Road Depression

// Task handles
TaskHandle_t pothole_detection_task_handle;
TaskHandle_t road_depression_task_handle;
TaskHandle_t led_task_handle;

// Pothole detection task
void pothole_detection_task(void *pvParameters) {
    bool previous_state_pothole_detection_pin = true;  // Initialize previous state as high

    while(1) {
        // Read current state of GPIO pin
        bool current_state_pothole_detection_pin = gpio_get(GPIO_PIN_PH);

        // Check for high-to-low transition
        if (previous_state_pothole_detection_pin && !current_state_pothole_detection_pin) {
            // High-to-low transition detected
            printf("High-to-low transition detected on pin %d\n", GPIO_PIN_PH);
            // Bundle the data of : 
            // GPS coordinate
            // pothole event
            // speed from gps queue
        }

        // Update previous state for the next iteration
        previous_state_pothole_detection_pin = current_state_pothole_detection_pin;

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// Road depression detection task
void road_depression_task(void *pvParameters) {
    bool previous_state_road_depression_pin = true;  // Initialize previous state as high

    while(1) {
        // Read current state of GPIO pin
        bool current_state_road_depression_pin = gpio_get(GPIO_PIN_RD);

        // Check for high-to-low transition
        if (previous_state_road_depression_pin && !current_state_road_depression_pin) {
            // High-to-low transition detected
            printf("High-to-low transition detected on pin %d\n", GPIO_PIN_RD);
            // Bundle the data of : 
            // GPS coordinate
            // road depression event
            // speed from gps queue
        }

        // Update previous state for the next iteration
        previous_state_road_depression_pin = current_state_road_depression_pin;

        vTaskDelay(pdMS_TO_TICKS(10));
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
    gpio_init(GPIO_PIN_PH);
    gpio_set_dir(GPIO_PIN_PH, GPIO_IN);
    gpio_pull_up(GPIO_PIN_PH); // high by default

    // Initialize GPIO pin - road depression
    gpio_init(GPIO_PIN_RD);
    gpio_set_dir(GPIO_PIN_RD, GPIO_IN);
    gpio_pull_up(GPIO_PIN_RD); // high by default

    // Create pothole detection task
    xTaskCreate(pothole_detection_task, "Pothole-Detection Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &pothole_detection_task_handle);

    // Create road depression detection task
    xTaskCreate(road_depression_task, "Road-Depression-Detection Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &road_depression_task_handle);

    // Create LED task
    xTaskCreate(led_task, "LED Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &led_task_handle);

    // Start FreeRTOS scheduler
    vTaskStartScheduler();
    return 0;
}
