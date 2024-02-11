#ifndef UTIL_H
#define UTIL_H

int uart_send_until_valid(uart_inst_t *uart, char *command, char *response, char *expected_response);
int uart_send(uart_inst_t *uart, char *command, char *response, int wait);
#endif /* UTIL_H */