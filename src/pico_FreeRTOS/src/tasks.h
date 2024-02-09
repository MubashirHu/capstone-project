#ifndef TASKS_H
#define TASKS_H

#define UART_ID_4G uart0
#define UART_ID_OBD2 uart1

#define BAUD_RATE_4G 115200
#define BAUD_RATE_OBD2 9600

#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

#define UART_TX_PIN_4G 0
#define UART_RX_PIN_4G 1

#define UART_TX_PIN_OBD2 4
#define UART_RX_PIN_OBD2 5

void initTasks();

#endif /* TASKS_H */
