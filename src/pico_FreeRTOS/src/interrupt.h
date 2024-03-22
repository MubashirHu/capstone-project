#ifndef INTERRUPT_H
#define INTERRUPT_H



#define GPIO_PIN_PH_1 19  // GPIO PIN - Pothole GP19
#define BIT1_0 18
#define BIT1_1 17
#define BIT1_2 16


#define GPIO_PIN_PH_2 13  // GPIO PIN - Pothole GP19
#define BIT2_0 12
#define BIT2_1 11
#define BIT2_2 10
#include <stdio.h>
void handle_pothole_interrupt(uint gpio, uint32_t events);

#endif
