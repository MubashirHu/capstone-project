#ifndef UTIL_H
#define UTIL_H
struct obd2_packet
{
    uint8_t slipping;
    uint8_t vehicle_speed;
};


int uart_send_until_valid(uart_inst_t *uart, char *command, char *response, char *expected_response);
int uart_send(uart_inst_t *uart, char *command, char *response, int wait);
int uart_send1(uart_inst_t *uart, char *command, char *response, int wait);
void uart_obd2_wheel_speed(uart_inst_t *uart, struct obd2_packet *packet);
int send_message(__int8_t message_type);

#endif /* UTIL_H */