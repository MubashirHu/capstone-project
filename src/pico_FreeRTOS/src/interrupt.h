#include <stdint.h>
#include <stdbool.h>
#ifndef INTERRUPT_H
#define INTERRUPT_H

#define GPIO_PIN_PH_PICO1 16  // GPIO PIN - Pothole GP19
#define BIT_0_PICO1 18
#define BIT_1_PICO1 17
#define BIT_2_PICO1 19

#define GPIO_PIN_PH_PICO2 12  // GPIO PIN - Pothole GP19
#define BIT_0_PICO2 10
#define BIT_1_PICO2 13
#define BIT_2_PICO2 11

// Flags to indicate interrupts on each Pico
extern bool pico1_interrupt;
extern bool pico2_interrupt;

extern uint8_t byte_pico1;
extern uint8_t byte_pico2;

void handle_pothole_interrupt();

#endif
