#ifndef TASKS_H
#define TASKS_H

#define UART_ID_4G uart0
#define UART_ID_OBD2 uart1
#define I2C_ID_GPS i2c0

#define BAUD_RATE_UART_4G 115200
#define BAUD_RATE_UART_OBD2 9600
#define BAUD_RATE_I2C_GPS 400 * 1000

#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

#define UART_TX_PIN_4G 0
#define UART_RX_PIN_4G 1

#define UART_TX_PIN_OBD2 4
#define UART_RX_PIN_OBD2 5

#define I2C_SDA_PIN_GPS 8
#define I2C_SCL_PIN_GPS 9

void initTasks();

#endif /* TASKS_H */
