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

#define GREEN_POTHOLE 0
#define YELLOW_POTHOLE 1
#define AMBER_POTHOLE 2
#define RED_POTHOLE 3
#define SLIPPING 4
#define CONGESTION 5

void initTasks(void)
{
    UBaseType_t uxCoreAffinityMask_0;
    uxCoreAffinityMask_0 = ( 1 << 0 );

    UBaseType_t uxCoreAffinityMask_1;
    uxCoreAffinityMask_1 = ( 1 << 1 );

    UBaseType_t uxCoreAffinityMask_both;
    uxCoreAffinityMask_both = ( ( 1 << 0 ) | ( 1 << 1 ) );

	// xTaskCreateAffinitySet(vTaskUart_4g, "4G_Task", 512, NULL, 6, uxCoreAffinityMask_1, NULL);
	// xTaskCreateAffinitySet(vTaskUart_OBD, "OBD2_Task", 512, NULL, 6, uxCoreAffinityMask_1, NULL);
    xTaskCreateAffinitySet(vTaskI2C_GPS, "GPS_Task", 512, NULL, 6, uxCoreAffinityMask_0, NULL);
    // xTaskCreateAffinitySet(vTaskNormal, "Normal_Task", 256, NULL, 6, uxCoreAffinityMask_1, NULL);
    xTaskCreateAffinitySet(led_task, "LED_Task", 256, NULL, 6, uxCoreAffinityMask_1, NULL);

    
}

void vTaskUart_4g(void * parameters)
{
    char uid[21];
    char http_code[4];
    char response[150];
    char response1[150];
    char response2[150];
    int char_num = 0;
    vTaskDelay(pdMS_TO_TICKS(1000));
    uart_puts(UART_TEST, "\r\nStarting 4G Module Task\r\n");
    while(uart_is_readable(UART_ID_4G))
    {
        uart_getc(UART_ID_4G);
    }
    uart_send_until_valid(UART_ID_4G, "AT\r\n", response, "AT\r\r\nOK\r\n");
    uart_puts(UART_TEST, "4G Module booted\r\n");
    vTaskDelay(pdMS_TO_TICKS(500));
    uart_send_until_valid(UART_ID_4G, "AT+CPIN?\r\n", response, "AT+CPIN?\r\r\n+CPIN: READY\r\n\r\nOK\r\n");
    vTaskDelay(pdMS_TO_TICKS(500));
    uart_send(UART_ID_4G, "AT+HTTPTERM\r\n", response, 0);
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
    uart_send(UART_ID_4G, "AT+HTTPACTION=1\r\n", response2, 4000);
    
    strncpy(http_code, response2 + 39, sizeof(http_code) - 1);
    http_code[sizeof(http_code) - 1] = '\0';
    uart_puts(UART_TEST, "\r\nHTTP CODE: ");
    uart_puts(UART_TEST, http_code);
    uart_puts(UART_TEST, "\r\n");

    vTaskDelay(pdMS_TO_TICKS(3000));
    uart_send1(UART_ID_4G, "AT+HTTPREAD=0,250\r\n", response1, 500);
    

    uart_send(UART_ID_4G, "AT+HTTPTERM\r\n", response, 0);
    
    vTaskDelay(pdMS_TO_TICKS(500));

    strncpy(uid, response1 + 55, sizeof(uid) - 1); // -1 to ensure null termination
    uid[sizeof(uid) - 1] = '\0'; // Ensure null termination
    uart_puts(UART_TEST, "\r\nUID: ");
    uart_puts(UART_TEST, uid);
    uart_puts(UART_TEST, "\r\n");

    

    uart_puts(UART_TEST, "Completed 4G init\r\n");

    struct message msg;

    while (1)
    {
        if(message_queue_dequeue(&msg) == 1)
        {
            uart_puts(UART_TEST, "Message Detected\r\n");
            static char json[512]; // Assuming a fixed size for simplicity, adjust as needed
            sprintf(json, "{\"uid\": \"%s\",\"latitude\":%.6lf,\"longitude\":%.6lf,\"speed\":%.2lf,\"message_type\":%d}",
            uid, msg.latitude, msg.longitude, msg.speed, msg.message_type);

            switch (msg.message_type)
            {
            case POTHOLE:
                uart_send_until_valid(UART_ID_4G, "AT+HTTPINIT\r\n", response, "AT+HTTPINIT\r\r\nOK\r\n");
                vTaskDelay(pdMS_TO_TICKS(50));
                uart_send(UART_ID_4G, "AT+HTTPPARA=\"URL\",\"https://test-f1e70.firebaseio.com/pothole.json\"\r\n", response, 0);
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
                uart_puts(UART_TEST, "Message Sent\r\n");
                break;
            case SLIPPING:
                uart_send_until_valid(UART_ID_4G, "AT+HTTPINIT\r\n", response, "AT+HTTPINIT\r\r\nOK\r\n");
                vTaskDelay(pdMS_TO_TICKS(50));
                uart_send(UART_ID_4G, "AT+HTTPPARA=\"URL\",\"https://test-f1e70.firebaseio.com/slip.json\"\r\n", response, 0);
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
                uart_puts(UART_TEST, "Message Sent\r\n");
                break;
            }
        }
        // check speed limit queue and if not empty, send speed limit api request to google for current location
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vTaskNormal(void * parameters)
{
    while(1)
    {
        //get speed limit, check speed, if speed much lower than posted speed, send conjection request
        // vTaskDelay(pdMS_TO_TICKS(5000));
        struct message x;
        struct gps y;
        // if(gps_queue_peek(&y))
        // {
            x.latitude = 50.22324;//y.latitude;
            x.longitude = -25.34223;y.longitude;
            x.message_type = 0;
            x.speed = 0;
            static char json[512];
            sprintf(json, "\r\n\"latitude\":%.6lf,\r\n\"longitude\":%.6lf,\r\n", x.latitude, x.longitude);
            // uart_puts(UART_TEST, json);
            // message_enqueue(x);
        // }
        vTaskDelay(pdMS_TO_TICKS(20000));
    }
}

void vTaskUart_OBD(void * parameters)
{
    char response[250];
    struct obd2_packet packet;
    double wheel_1;
    double wheel_2;
    double wheel_3;
    double wheel_4;
    uint16_t brake_pressure;
    uint16_t vehicle_speed;
    double threshold = 50;
    uart_puts(UART_TEST, "\r\nWheel 1: ");
    uart_send_until_valid(UART_ID_OBD2, "ATZ\r\n", response, "ATZ\r\r\rELM327 v1.4b\r\r>");
    vTaskDelay(pdMS_TO_TICKS(1000));

    while (1)
    {
        char stringValue[6];
        uart_obd2_wheel_speed(UART_ID_OBD2, &packet);
        // // uart_puts(UART_TEST, "\r\nWheel 1: ");
        // snprintf(stringValue, sizeof(stringValue), "%u", wheel_1);
        // uart_puts(UART_TEST, stringValue);

        // uart_puts(UART_TEST, ",");

        // // uart_puts(UART_TEST, "\r\nWheel 2: ");
        // snprintf(stringValue, sizeof(stringValue), "%u", wheel_2);
        // uart_puts(UART_TEST, stringValue);

        // uart_puts(UART_TEST, ",");

        // // uart_puts(UART_TEST, "\r\nWheel 3: ");
        // snprintf(stringValue, sizeof(stringValue), "%u", wheel_3);
        // uart_puts(UART_TEST, stringValue);

        // uart_puts(UART_TEST, ",");

        // // uart_puts(UART_TEST, "\r\nWheel 4: ");
        // snprintf(stringValue, sizeof(stringValue), "%u", wheel_4);
        // uart_puts(UART_TEST, stringValue);

        // uart_puts(UART_TEST, ",");
        
        // // uart_puts(UART_TEST, "\r\nBrake Pressure: ");
        // snprintf(stringValue, sizeof(stringValue), "%u", brake_pressure);
        // uart_puts(UART_TEST, stringValue);

        // uart_puts(UART_TEST, ",");

        // snprintf(stringValue, sizeof(stringValue), "%u", vehicle_speed);
        // uart_puts(UART_TEST, stringValue);

        uart_puts(UART_TEST, "\r\n");
        // uart_puts(UART_TEST, "\r\n===========================================\r\n");

        // if ((wheel_1 ) &&
        //      brake_pressure > 160)
        // {
        //     vTaskDelay(1);
        // }
        //atz receive after sending: "ATZ\r\r\rELM327 v1.4b\r\r>"
        //AT CRA x receive after sending: "AT CRA x\rOK\r\r>"
        //AT MA (for 0b0): "AT MA\r00 00 00 00 11 09 00 00\r" 24 char

        // vTaskDelay(pdMS_TO_TICKS(10));
        //uart_puts(UART_ID_4G, "\r\n");
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
                    uart_puts(UART_TEST, token);
                    uart_puts(UART_TEST, "\r\n");
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
                        uart_puts(UART_TEST, "UTC Time: ");
                        uart_puts(UART_TEST, utc_time);
                        uart_puts(UART_TEST, ", Latitude: ");
                        if (strlen(latitude) > 0)
                        {
                            uart_puts(UART_TEST, latitude);
                            uart_putc(UART_TEST, latitude_direction);
                        }
                        else
                        {
                            uart_puts(UART_TEST, "N/A");
                        }

                        uart_puts(UART_TEST, ", Longitude: ");
                        if (strlen(longitude) > 0)
                        {
                            uart_puts(UART_TEST, longitude);
                            uart_putc(UART_TEST, longitude_direction);
                        }
                        else
                        {
                            uart_puts(UART_TEST, "N/A");
                        }
                        uart_puts(UART_TEST, "\r\n");
                        int lat_degrees = atoi(latitude) / 100;
                        double lat_minutes = atof(latitude + 2);
                        double lat_decimalDegrees;
                        lat_decimalDegrees = lat_degrees + (double)lat_minutes / 60.000;

                        // double long_degrees, long_minutes;
                        double long_decimalDegrees;

                        int long_degrees = atoi(longitude) / 100;
                        double long_minutes = atof(longitude + 3);
                        long_decimalDegrees = long_degrees + (double)long_minutes / 60.000;

                        if(latitude_direction == 'S')
                        {
                            lat_decimalDegrees = -lat_decimalDegrees;
                        }

                        if(longitude_direction == 'W')
                        {
                            long_decimalDegrees = -long_decimalDegrees;
                        }

                        struct gps x;
                        x.latitude = lat_decimalDegrees;
                        x.longitude = long_decimalDegrees;

                        static char json[512];
                        sprintf(json, "\r\n\"time\":%ld,\r\n\"latitude\":%.6lf,\r\n\"longitude\":%.6lf,\r\n",
                                (long)x.time, x.latitude, x.longitude);
                        uart_puts(UART_TEST, json);
                        uart_puts(UART_TEST, "\r\n");
                        
                        if (positioning_status != 0 && num_satellites > 3)
                        {
                            gps_queue_overwrite(x);
                            uart_puts(UART_TEST, "GPS Position enqueued");
                            uart_puts(UART_TEST, "\r\n");
                        }
                    }
                    // else
                    // {
                    //     // Print a message indicating that data extraction failed
                    //     uart_puts(UART_TEST, "Invalid NMEA sentence\r\n");
                    //     break;
                    // }
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


