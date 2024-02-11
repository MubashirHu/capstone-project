#include "FreeRTOS.h"
#include "tasks.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include <string.h>
#include "util.h"



static int chars_rxed = 0;

static void vTaskUart_4g(void * parameters);
void vTaskUart_OBD(void * parameters);
void vTaskI2C_GPS(void * parameters);
static void led_task(void * parameters);

void initTasks(void)
{
	xTaskCreate(vTaskUart_4g, "4G_Task", 256, NULL, 1, NULL);
	//xTaskCreate(vTaskUart_OBD, "OBD2_Task", 256, NULL, 1, NULL);
    //xTaskCreate(vTaskI2C_GPS, "GPS_Task", 256, NULL, 1, NULL);
    //xTaskCreate(led_task, "LED_Task", 256, NULL, 1, NULL);
}

void vTaskUart_4g(void * parameters)
{
    uart_init(UART_ID_4G, BAUD_RATE_UART_4G);
    gpio_set_function(UART_TX_PIN_4G, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN_4G, GPIO_FUNC_UART);
    uart_set_hw_flow(UART_ID_4G, false, false);

    uart_set_format(UART_ID_4G, DATA_BITS, STOP_BITS, PARITY);

    uart_set_fifo_enabled(UART_ID_4G, true);

    uart_init(UART_ID_OBD2, BAUD_RATE_UART_4G);
    gpio_set_function(UART_TX_PIN_OBD2, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN_OBD2, GPIO_FUNC_UART);
    uart_set_hw_flow(UART_ID_OBD2, false, false);
    uart_set_format(UART_ID_OBD2, DATA_BITS, STOP_BITS, PARITY);

    char response[250];
    response[0] = '\0';
    int char_num = 0;
    vTaskDelay(10000);
    // TODO: Send initialization commands to 4g module and wait for response
    while(uart_is_readable(UART_ID_4G))
    {
        uart_getc(UART_ID_4G);
    }
    uart_puts(UART_ID_OBD2, "\r\n");
    uart_send_until_valid(UART_ID_4G, "AT\r\n", response, "AT\r\r\nOK\r\n");
    vTaskDelay(pdMS_TO_TICKS(500));
    uart_send_until_valid(UART_ID_4G, "AT+CPIN?\r\n", response, "AT+CPIN\r\r\n+CPIN: READY\r\n\r\nOK\r\n");
    vTaskDelay(pdMS_TO_TICKS(500));
    uart_send_until_valid(UART_ID_4G, "AT+HTTPINIT\r\n", response, "AT+HTTPINIT\r\r\nOK\r\n");
    vTaskDelay(pdMS_TO_TICKS(500));
    uart_send(UART_ID_4G, "AT+HTTPPARA=\"URL\",\"https://test-f1e70.firebaseio.com/uid.json\"\r\n", response, 0);
    vTaskDelay(pdMS_TO_TICKS(500));
    uart_send(UART_ID_4G, "AT+HTTPPARA=\"CONTENT\",\"application/json\"\r\n", response, 0);
    vTaskDelay(pdMS_TO_TICKS(500));
    uart_send(UART_ID_4G, "AT+HTTPDATA=20,5000\r\n", response, 0);
    vTaskDelay(pdMS_TO_TICKS(500));
    uart_send(UART_ID_4G, "{\"test\":\"test\"}\r\n", response, 0);
    vTaskDelay(pdMS_TO_TICKS(500));
    uart_send(UART_ID_4G, "\n\r\n", response, 1000);
    vTaskDelay(pdMS_TO_TICKS(500));
    uart_send(UART_ID_4G, "AT+HTTPACTION=1\r\n", response, 1000);
    vTaskDelay(pdMS_TO_TICKS(500));
    uart_send(UART_ID_4G, "AT+HTTPREAD=0,250\r\n", response, 1000);
    
    uart_puts(UART_ID_OBD2, "\r\n");
    vTaskDelay(pdMS_TO_TICKS(500));
    // parse response for uid
    uart_send(UART_ID_4G, "AT+HTTPTERM\r\n", response, 0);

    // Set data format

    while (1)
    {
        // int response_num = 0;
        // response_num = uart_send(UART_ID_4G, "AT\r\n", response);

        // uart_puts(UART_ID_OBD2, "\r\n\r\n");
        // for(int i = 0; i < response_num; i++)
        // {
        //     char ascii_code[4];
        //     sprintf(ascii_code, "%02X ", response[i]);
        //     uart_puts(UART_ID_OBD2, ascii_code);
        //     uart_puts(UART_ID_OBD2, " | ");
        // }
        // uart_puts(UART_ID_OBD2, "\r\n");
        vTaskDelay(1000);

    }
}

void vTaskUart_OBD(void * parameters)
{
    uart_init(UART_ID_OBD2, BAUD_RATE_UART_OBD2);
    gpio_set_function(UART_TX_PIN_OBD2, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN_OBD2, GPIO_FUNC_UART);
    uart_set_hw_flow(UART_ID_OBD2, false, false);

    // Set data format
    uart_set_format(UART_ID_OBD2, DATA_BITS, STOP_BITS, PARITY);

    uart_set_fifo_enabled(UART_ID_OBD2, true);

    uart_puts(UART_ID_OBD2, "atz");

    while (1)
        tight_loop_contents();
        
}

void vTaskI2C_GPS(void * parameters)
{
    //Initialize UART0 with a baud rate of 115200
    uart_init(UART_ID_OBD2, BAUD_RATE_UART_4G);
    // Set UART0 TX (transmit) pin to GPIO 0 and RX (receive) pin to GPIO 1
    gpio_set_function(UART_TX_PIN_OBD2, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN_OBD2, GPIO_FUNC_UART);
    uart_set_hw_flow(UART_ID_OBD2, false, false);
    uart_set_format(UART_ID_OBD2, DATA_BITS, STOP_BITS, PARITY);

    uart_set_fifo_enabled(UART_ID_OBD2, true);

    uart_puts(UART_ID_OBD2, "\r\nTest1\r\n");
    i2c_init(I2C_ID_GPS, BAUD_RATE_I2C_GPS);
    gpio_set_function(I2C_SDA_PIN_GPS, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN_GPS, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN_GPS);
    gpio_pull_up(I2C_SCL_PIN_GPS);
    uint8_t num_bytes_high;
    uint8_t num_bytes_low;
    uint16_t num_bytes_available;
    uart_puts(UART_ID_OBD2, "Test2\r\n");
    uint8_t data[256];
    while (1)
    {
        size_t len = i2c_read_blocking(i2c0, 0x42, data, 255, false);
        data[len] = '\0';

            // Process each NMEA sentence
            // For example, you might check if it starts with "$GPGGA" or "$GPRMC" to extract relevant information
        char* token = strtok(data, "\n"); // Tokenize by newline
        uart_puts(UART_ID_OBD2, token);
        uart_puts(UART_ID_OBD2, "\r\n");
        // while (token != NULL) 
        // {
        //     // Check if the token starts with '$' (indicating an NMEA sentence)
        //     if (token[0] == '$') 
        //     {
        //         // Process the NMEA sentence
        //         // Example: Print the NMEA sentence to UART
        //         // uart_puts(UART_ID_OBD2, token);
        //         // uart_puts(UART_ID_OBD2, "\r\n");
        //         if (strncmp(token, "$GNGGA", 6) == 0)
        //         {
        //             // Extract information from GNGGA sentence
        //             char utc_time[11];
        //             char latitude[12];
        //             char latitude_direction;
        //             char longitude[13];
        //             char longitude_direction;
        //             int positioning_status;
        //             int num_satellites;
        //             float hdop;
        //             float altitude;
        //             float geoid_height;
        //             float differential_time;
        //             char reference_base_station[3];

        //             // Use sscanf to extract data from the NMEA sentence
        //             int items_matched = sscanf(token, "$GNGGA,%10[^,],%11[^,],%c,%12[^,],%c,%d,%d,%f,%f,%f,M,%f,M,%f,%3[^*]*",
        //                                     utc_time, latitude, &latitude_direction, longitude, &longitude_direction,
        //                                     &positioning_status, &num_satellites, &hdop, &altitude, &geoid_height,
        //                                     &differential_time, reference_base_station);

        //             //if (items_matched > 0)
        //             {
        //                 // Data successfully extracted, print it
        //                 uart_puts(UART_ID_OBD2, "UTC Time: ");
        //                 uart_puts(UART_ID_OBD2, utc_time);
        //                 uart_puts(UART_ID_OBD2, ", Latitude: ");
        //                 if (strlen(latitude) > 0)
        //                 {
        //                     uart_puts(UART_ID_OBD2, latitude);
        //                     uart_putc(UART_ID_OBD2, latitude_direction);
        //                 }
        //                 else
        //                 {
        //                     uart_puts(UART_ID_OBD2, "N/A");
        //                 }

        //                 uart_puts(UART_ID_OBD2, ", Longitude: ");
        //                 if (strlen(longitude) > 0)
        //                 {
        //                     uart_puts(UART_ID_OBD2, longitude);
        //                     uart_putc(UART_ID_OBD2, longitude_direction);
        //                 }
        //                 else
        //                 {
        //                     uart_puts(UART_ID_OBD2, "N/A");
        //                 }

        //                 // Inside your parsing logic...
        //                 char num_satellites_str[4]; // Assuming maximum 3 digits for the number of satellites
        //                 char hdop_str[10]; // Adjust the size as per your requirements
        //                 char altitude_str[20]; // Adjust the size as per your requirements
        //                 char geoid_height_str[20]; // Adjust the size as per your requirements
        //                 char differential_time_str[20]; // Adjust the size as per your requirements

        //                 // Convert integers and floats to strings
        //                 sprintf(num_satellites_str, "%d", num_satellites);
        //                 sprintf(hdop_str, "%.2f", hdop); // Assuming 2 decimal places for HDOP
        //                 sprintf(altitude_str, "%.2f", altitude); // Assuming 2 decimal places for altitude
        //                 sprintf(geoid_height_str, "%.2f", geoid_height); // Assuming 2 decimal places for geoid height
        //                 sprintf(differential_time_str, "%.2f", differential_time); // Assuming 2 decimal places for differential time



        //                 uart_puts(UART_ID_OBD2, ", Positioning Status: ");
        //                 uart_puts(UART_ID_OBD2, positioning_status);
        //                 uart_puts(UART_ID_OBD2, ", Number of Satellites: ");
        //                 uart_puts(UART_ID_OBD2, num_satellites_str);
        //                 uart_puts(UART_ID_OBD2, ", HDOP: ");
        //                 uart_puts(UART_ID_OBD2, hdop_str);
        //                 uart_puts(UART_ID_OBD2, ", Altitude: ");
        //                 uart_puts(UART_ID_OBD2, altitude_str);
        //                 uart_puts(UART_ID_OBD2, ", Geoid Height: ");
        //                 uart_puts(UART_ID_OBD2, geoid_height_str);
        //                 uart_puts(UART_ID_OBD2, ", Differential Time: ");
        //                 uart_puts(UART_ID_OBD2, differential_time_str);
        //                 uart_puts(UART_ID_OBD2, ", Reference Base Station: ");
        //                 uart_puts(UART_ID_OBD2, reference_base_station);
        //                 uart_puts(UART_ID_OBD2, "\r\n");
        //             }
        //             // else
        //             // {
        //             //     // Print a message indicating that data extraction failed
        //             //     uart_puts(UART_ID_OBD2, "Invalid NMEA sentence\r\n");
        //             // }
        //         }
        //     }
        //     // Move to the next token
        //     token = strtok(NULL, "\n");
        // }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void led_task(void * parameters)
{   
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {
        gpio_put(LED_PIN, 1);
        vTaskDelay(100);
        gpio_put(LED_PIN, 0);
        vTaskDelay(100);
    }
}


