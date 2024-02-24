#ifndef UTIL_H
#define UTIL_H

int uart_send_until_valid(uart_inst_t *uart, char *command, char *response, char *expected_response);
int uart_send(uart_inst_t *uart, char *command, char *response, int wait);
int uart_send1(uart_inst_t *uart, char *command, char *response, int wait);
void uart_obd2_wheel_speed(uart_inst_t *uart, uint16_t *wheel_1, uint16_t *wheel_2, uint16_t *wheel_3, uint16_t *wheel_4, uint16_t *brake_pressure);

#endif /* UTIL_H */