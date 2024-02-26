#include "FreeRTOS.h"
#include "tasks.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include <string.h>
#include "util.h"
#include "queues.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


static void vTaskUart_4g(void * parameters);
static void vTaskNormal(void * parameters);
void vTaskUart_OBD(void * parameters);
void vTaskI2C_GPS(void * parameters);
static void led_task(void * parameters);

void initTasks(void)
{
	// xTaskCreate(vTaskUart_4g, "4G_Task", 512, NULL, 6, NULL);
	// xTaskCreate(vTaskUart_OBD, "OBD2_Task", 512, NULL, 6, NULL);
    xTaskCreate(vTaskI2C_GPS, "GPS_Task", 512, NULL, 6, NULL);
    // xTaskCreate(vTaskNormal, "Normal_Task", 256, NULL, 6, NULL);
    // xTaskCreate(led_task, "LED_Task", 256, NULL, 6, NULL);
}

void vTaskUart_4g(void * parameters)
{
    char uid[20];
    char response[150];
    int char_num = 0;
    vTaskDelay(pdMS_TO_TICKS(1000));
    uart_puts(UART_ID_OBD2, "start_init_task\r\n");
    while(uart_is_readable(UART_ID_4G))
    {
        uart_getc(UART_ID_4G);
    }
    uart_puts(UART_ID_OBD2, "start_init\r\n");
    uart_send_until_valid(UART_ID_4G, "AT\r\n", response, "AT\r\r\nOK\r\n");
    vTaskDelay(pdMS_TO_TICKS(500));
    uart_send_until_valid(UART_ID_4G, "AT+CPIN?\r\n", response, "AT+CPIN?\r\r\n+CPIN: READY\r\n\r\nOK\r\n");
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
    uart_send(UART_ID_4G, "AT+HTTPACTION=1\r\n", response, 500);
    vTaskDelay(pdMS_TO_TICKS(500));
    uart_send1(UART_ID_4G, "AT+HTTPREAD=0,250\r\n", response, 0);
    
    vTaskDelay(pdMS_TO_TICKS(500));

    strncpy(uid, response + 53, sizeof(uid) - 1); // -1 to ensure null termination
    uid[sizeof(uid) - 1] = '\0'; // Ensure null termination

    
    // parse response for uid 91
    uart_send(UART_ID_4G, "AT+HTTPTERM\r\n", response, 0);

    uart_puts(UART_ID_OBD2, "completed_init\r\n");

    struct message msg;

    while (1)
    {

        if(message_queue_dequeue(&msg) == 1)
        {
            static char json[512]; // Assuming a fixed size for simplicity, adjust as needed
            sprintf(json, "{\"uid\": \"%s\", \"time\":%ld,\"latitude\":%.6lf,\"longitude\":%.6lf,\"speed\":%.2lf,\"message_type\":%d}",
            uid, (long)msg.time, msg.latitude, msg.longitude, msg.speed, msg.message_type);

            uart_send_until_valid(UART_ID_4G, "AT+HTTPINIT\r\n", response, "AT+HTTPINIT\r\r\nOK\r\n");
            vTaskDelay(pdMS_TO_TICKS(50));
            uart_send(UART_ID_4G, "AT+HTTPPARA=\"URL\",\"https://test-f1e70.firebaseio.com/test.json\"\r\n", response, 0);
            vTaskDelay(pdMS_TO_TICKS(50));
            uart_send(UART_ID_4G, "AT+HTTPPARA=\"CONTENT\",\"application/json\"\r\n", response, 0);
            vTaskDelay(pdMS_TO_TICKS(50));
            uart_send(UART_ID_4G, "AT+HTTPDATA=20,5000\r\n", response, 0);
            vTaskDelay(pdMS_TO_TICKS(50));
            // use string to create json string with message structure
            uart_send(UART_ID_4G, json, response, 0);
            vTaskDelay(pdMS_TO_TICKS(50));
            uart_send(UART_ID_4G, "\n\r\n", response, 1000);
            vTaskDelay(pdMS_TO_TICKS(50));
            uart_send(UART_ID_4G, "AT+HTTPACTION=1\r\n", response, 1000);
            // verify http response of 200 if failed, then repeat until it doesn't for x amount of times
            vTaskDelay(pdMS_TO_TICKS(50));
            uart_send(UART_ID_4G, "AT+HTTPTERM\r\n", response, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
        uart_puts(UART_ID_OBD2, "completed_init\r\n");

    }
}

void vTaskNormal(void * parameters)
{
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(5000));
        struct message x;
        struct gps y;
        if(gps_queue_peek(&y))
        {
            x.latitude = y.latitude;
            x.longitude = y.longitude;
            x.message_type = 0;
            x.time = y.time;
            x.speed = 0;
            message_enqueue(x);
        }
        vTaskDelay(2000);
    }
}

void vTaskUart_OBD(void * parameters)
{
    char response[250];
    uint16_t wheel_1;
    uint16_t wheel_2;
    uint16_t wheel_3;
    uint16_t wheel_4;
    uint16_t brake_pressure;
    //uart_puts(UART_ID_OBD2, "atz");
    uart_send_until_valid(UART_ID_OBD2, "ATZ\r\n", response, "ATZ\r\r\rELM327 v1.4b\r\r>");
    vTaskDelay(pdMS_TO_TICKS(1000));

    while (1)
    {
        char stringValue[6];
        uart_obd2_wheel_speed(UART_ID_OBD2, &wheel_1, &wheel_2, &wheel_3, &wheel_4, &brake_pressure);
        uart_puts(UART_ID_4G, "\r\nWheel 1: ");
        snprintf(stringValue, sizeof(stringValue), "%u", wheel_1);
        uart_puts(UART_ID_4G, stringValue);

        uart_puts(UART_ID_4G, "\r\nWheel 2: ");
        snprintf(stringValue, sizeof(stringValue), "%u", wheel_2);
        uart_puts(UART_ID_4G, stringValue);

        uart_puts(UART_ID_4G, "\r\nWheel 3: ");
        snprintf(stringValue, sizeof(stringValue), "%u", wheel_3);
        uart_puts(UART_ID_4G, stringValue);

        uart_puts(UART_ID_4G, "\r\nWheel 4: ");
        snprintf(stringValue, sizeof(stringValue), "%u", wheel_4);
        uart_puts(UART_ID_4G, stringValue);
        
        uart_puts(UART_ID_4G, "\r\nBrake Pressure: ");
        snprintf(stringValue, sizeof(stringValue), "%u", brake_pressure);
        uart_puts(UART_ID_4G, stringValue);
        uart_puts(UART_ID_4G, "\r\n===========================================\r\n");
        //atz receive after sending: "ATZ\r\r\rELM327 v1.4b\r\r>"
        //AT CRA x receive after sending: "AT CRA x\rOK\r\r>"
        //AT MA (for 0b0): "AT MA\r00 00 00 00 11 09\r" 24 char
        

        //uart_obd2_wheel_speed(UART_ID_OBD2, wheel_1, wheel_2, wheel_3, wheel_4);
        //int response_num = 0;
        //response_num = uart_send(UART_ID_OBD2, "AT MA\r\n", response, 100);
        //uart_puts(UART_ID_4G, "\r\n");
        // for(int i = 0; i < response_num; i++)
        // {
        //     char ascii_code[4];
        //     sprintf(ascii_code, "%02X ", response[i]);
        //     uart_puts(UART_ID_4G, ascii_code);
        //     uart_puts(UART_ID_4G, " | ");
        // }

        vTaskDelay(pdMS_TO_TICKS(10));
        //uart_puts(UART_ID_4G, "\r\n");

        //send at cra 0b2 (wheel 4 and 3) each wheel on bytes 1&2 (back left) and 3&4 (back right)
        //send at ma and read x bytes of data and then send enter to stop command
        // send at cra ob0 (wheel 2 and 1) each wheel on bytes 1&2 (front left) and 3&4 (front right) 
        // repeat at ma
        // send at cra 224 (brake pedal)
        // repeat at ma
        // send at cra 610 for vehicle speed (byte 3)
        // if wheel speeds significantly different and car is braking, and speed is above x,
        // get gps data and enqueue a message
    }
        
}

void vTaskI2C_GPS(void * parameters)
{
    uint8_t num_bytes_high;
    uint8_t num_bytes_low;
    uint16_t num_bytes_available;
    uint8_t data[255];
    while (1)
    {
        size_t len = i2c_read_blocking(i2c0, 0x42, data, 254, false);
        char* token = strtok(data, "\n"); // Tokenize by newline

        while (token != NULL) 
        {
            // Check if the token starts with '$' (indicating an NMEA sentence)
            if (token[0] == '$') 
            {
                
                if (strncmp(token, "$GNGGA", 6) == 0)
                {
                    // uart_puts(UART_ID_OBD2, token);
                    // uart_puts(UART_ID_OBD2, "\r\n");
                    // Extract information from GNGGA sentence
                    char utc_time[11];
                    char latitude[12];
                    char latitude_direction;
                    char longitude[13];
                    char longitude_direction;
                    int positioning_status;
                    int num_satellites;
                    float hdop;
                    float altitude;
                    float geoid_height;
                    float differential_time;
                    char reference_base_station[3];

                    // Use sscanf to extract data from the NMEA sentence
                    int items_matched = sscanf(token, "$GNGGA,%10[^,],%11[^,],%c,%12[^,],%c,%d,%d,%f,%f,%f,M,%f,M,%f,%3[^*]*",
                                            utc_time, latitude, &latitude_direction, longitude, &longitude_direction,
                                            &positioning_status, &num_satellites, &hdop, &altitude, &geoid_height,
                                            &differential_time, reference_base_station);
                    if (items_matched > 4)
                    {
                        // uart_puts(uart0, token);
                        // uart_puts(uart0, "\r\n");
                        // // Data successfully extracted, print it

                        uart_puts(uart0, "UTC Time: ");
                        uart_puts(uart0, utc_time);
                        uart_puts(uart0, ", Latitude: ");
                        if (strlen(latitude) > 0)
                        {
                            uart_puts(uart0, latitude);
                            uart_putc(uart0, latitude_direction);
                        }
                        else
                        {
                            uart_puts(uart0, "N/A");
                        }

                        uart_puts(uart0, ", Longitude: ");
                        if (strlen(longitude) > 0)
                        {
                            uart_puts(uart0, longitude);
                            uart_putc(uart0, longitude_direction);
                        }
                        else
                        {
                            uart_puts(uart0, "N/A");
                        }

                        int lat_degrees = atoi(latitude) / 100;
                        double lat_minutes = atof(latitude + 2);
                        double lat_decimalDegrees;
                        //sscanf(latitude, "%lf%lf", &lat_degrees, &lat_minutes);
                        lat_decimalDegrees = lat_degrees + (double)lat_minutes / 60.000;

                        // double long_degrees, long_minutes;
                        double long_decimalDegrees;

                        int long_degrees = atoi(longitude) / 100;
                        double long_minutes = atof(longitude + 3);
                        // sscanf(longitude, "%lf%lf", &long_degrees, &long_minutes);
                        long_decimalDegrees = long_degrees + (double)long_minutes / 60.000;

                        int hours = (utc_time[0] - '0') * 10 + (utc_time[1] - '0');
                        int minutes = (utc_time[2] - '0') * 10 + (utc_time[3] - '0');
                        int seconds = (utc_time[4] - '0') * 10 + (utc_time[5] - '0');
                        int milliseconds = (utc_time[7] - '0') * 100 + (utc_time[8] - '0') * 10 + (utc_time[9] - '0');

                        time_t unixTime = hours * 3600 + minutes * 60 + seconds;
                        unixTime += milliseconds / 1000;

                        struct gps x;
                        x.time = unixTime;
                        x.latitude = lat_decimalDegrees;
                        x.longitude = long_decimalDegrees;

                        static char json[512]; // Assuming a fixed size for simplicity, adjust as needed
                        sprintf(json, "\r\n\"time\":%ld,\r\n\"latitude\":%.6lf,\r\n\"longitude\":%.6lf,\r\n",
                        (long)x.time, x.latitude, x.longitude);
                        uart_puts(uart0, json);
                        if (positioning_status != 0)
                        {
                            gps_queue_overwrite(x);
                        }

                        // char num_satellites_str[4]; // Assuming maximum 3 digits for the number of satellites
                        // char hdop_str[10]; // Adjust the size as per your requirements
                        // char altitude_str[20]; // Adjust the size as per your requirements
                        // char geoid_height_str[20]; // Adjust the size as per your requirements
                        // char differential_time_str[20]; // Adjust the size as per your requirements

                        // // Convert integers and floats to strings
                        // sprintf(num_satellites_str, "%d", num_satellites);
                        // sprintf(hdop_str, "%.2f", hdop); // Assuming 2 decimal places for HDOP
                        // sprintf(altitude_str, "%.2f", altitude); // Assuming 2 decimal places for altitude
                        // sprintf(geoid_height_str, "%.2f", geoid_height); // Assuming 2 decimal places for geoid height
                        // sprintf(differential_time_str, "%.2f", differential_time); // Assuming 2 decimal places for differential time



                        // uart_puts(UART_ID_OBD2, ", Positioning Status: ");
                        // uart_puts(UART_ID_OBD2, positioning_status);
                        // uart_puts(UART_ID_OBD2, ", Number of Satellites: ");
                        // uart_puts(UART_ID_OBD2, num_satellites_str);
                        // uart_puts(UART_ID_OBD2, ", HDOP: ");
                        // uart_puts(UART_ID_OBD2, hdop_str);
                        // uart_puts(UART_ID_OBD2, ", Altitude: ");
                        // uart_puts(UART_ID_OBD2, altitude_str);
                        // uart_puts(UART_ID_OBD2, ", Geoid Height: ");
                        // uart_puts(UART_ID_OBD2, geoid_height_str);
                        // uart_puts(UART_ID_OBD2, ", Differential Time: ");
                        // uart_puts(UART_ID_OBD2, differential_time_str);
                        // uart_puts(UART_ID_OBD2, ", Reference Base Station: ");
                        // uart_puts(UART_ID_OBD2, reference_base_station);
                        // uart_puts(UART_ID_OBD2, "\r\n");
                    }
                    else
                    {
                        // Print a message indicating that data extraction failed
                        uart_puts(uart0, "Invalid NMEA sentence\r\n");
                        break;
                    }
                }
            }
            // Move to the next token
            token = strtok(NULL, "\n");
        }
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
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_put(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}


