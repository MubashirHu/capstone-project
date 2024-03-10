#include "interrupt.h"

void handle_pothole_interrupt(uint gpio, uint32_t events)
{
    __uint8_t byte = 0;

    switch(gpio) 
    {
        case GPIO_PIN_PH_1:
            byte = (gpio_get(BIT1_2) << 2) | (gpio_get(BIT1_1) << 1) | gpio_get(BIT1_0);
            break;
    
        case GPIO_PIN_PH_2:
            byte = (gpio_get(BIT2_2) << 2) | (gpio_get(BIT2_1) << 1) | gpio_get(BIT2_0);
            break;
    }
    

    switch (byte)
    {
    case 3:
        uart_puts(UART_TEST, "RED ZONE\r\n");
        break;
    case 2:
        uart_puts(UART_TEST, "AMBER_ZONE\r\n");
        break;
    case 1:
        uart_puts(UART_TEST, "YELLOW_ZONE\r\n");
        break;
    case 0:
        uart_puts(UART_TEST, "GREEN_ZONE\r\n");
        break;
    default:
        uart_puts(UART_TEST, "GREEN_ZONE\r\n");
        break;
    }

    //get GPS data
    // struct gps x;
    // gps_queue_peek(x);
    //determine potlevel
    //send speed 
}
