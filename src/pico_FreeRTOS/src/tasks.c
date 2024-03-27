#include "FreeRTOS.h"
#include "tasks.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "interrupt.h"
#include <string.h>
#include "util.h"
#include "queues.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "pico/binary_info.h"

static void vTaskUart_4g(void * parameters);
static void vTaskNormal(void * parameters);
void vTaskUart_OBD(void * parameters);
void vTaskI2C_GPS(void * parameters);
static void led_task(void * parameters);
void bundle_task(void *pvParameters);

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

	xTaskCreateAffinitySet(vTaskUart_4g, "4G_Task", 512, NULL, 6, uxCoreAffinityMask_1, NULL);
	// xTaskCreateAffinitySet(vTaskUart_OBD, "OBD2_Task", 512, NULL, 6, uxCoreAffinityMask_0, NULL);
    // xTaskCreateAffinitySet(vTaskI2C_GPS, "GPS_Task", 512, NULL, 6, uxCoreAffinityMask_1, NULL);
    // xTaskCreateAffinitySet(vTaskNormal, "Normal_Task", 256, NULL, 6, uxCoreAffinityMask_1, NULL);
    xTaskCreateAffinitySet(led_task, "LED_Task", 256, NULL, 6, uxCoreAffinityMask_1, NULL);
    xTaskCreateAffinitySet(bundle_task, "BUNDLE_Task", 512, NULL, 6, uxCoreAffinityMask_1, NULL);    
}

void vTaskUart_4g(void * parameters)
{
    char uid[21] = "-Ntr9d1PiQmfRuqryj-c";
    char http_code[4];
    char response[150];
    char response1[150];
    char response2[150];
    int char_num = 0;
    vTaskDelay(pdMS_TO_TICKS(1000));
    // uart_puts(UART_TEST, "\r\nStarting 4G Module Task\r\n");
    while(uart_is_readable(UART_ID_4G))
    {
        uart_getc(UART_ID_4G);
    }
    uart_send_until_valid(UART_ID_4G, "AT\r\n", response, "AT\r\r\nOK\r\n");
    // uart_puts(UART_TEST, "4G Module booted\r\n");
    vTaskDelay(pdMS_TO_TICKS(500));
    uart_send_until_valid(UART_ID_4G, "AT+CPIN?\r\n", response, "AT+CPIN?\r\r\n+CPIN: READY\r\n\r\nOK\r\n");
    vTaskDelay(pdMS_TO_TICKS(500));
    uart_send_until_valid(UART_ID_4G, "AT+HTTPTERM\r\n", response, "AT+HTTPTERM\r\r\nERROR\r\n");
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
    // if(strncmp(http_code, "200", 3) != 0)
    // {
    //     uart_puts(UART_TEST, "\r\nHTTP CODE: ");
    //     uart_puts(UART_TEST, http_code);
    //     uart_puts(UART_TEST, "\r\n");
    // }    

    vTaskDelay(pdMS_TO_TICKS(3000));
    uart_send1(UART_ID_4G, "AT+HTTPREAD=0,250\r\n", response1, 500);
    

    uart_send_until_valid(UART_ID_4G, "AT+HTTPTERM\r\n", response, "AT+HTTPTERM\r\r\nERROR\r\n");
    
    vTaskDelay(pdMS_TO_TICKS(500));

    strncpy(uid, response1 + 55, sizeof(uid) - 1); // -1 to ensure null termination
    uid[sizeof(uid) - 1] = '\0'; // Ensure null termination
    // uart_puts(UART_TEST, "\r\nUID: ");
    // uart_puts(UART_TEST, uid);
    // uart_puts(UART_TEST, "\r\n");

    // uart_puts(UART_TEST, "Completed Task init\r\n");

    struct message msg;

    while (1)
    {
        if(message_queue_dequeue(&msg) == 1)
        {
            uart_puts(UART_TEST, "Message Detected\r\n");
            static char json[512]; // Assuming a fixed size for simplicity, adjust as needed
            sprintf(json, "{\"uid\": \"%s\",\"latitude\":%.6lf,\"longitude\":%.6lf,\"speed\":%.2lf,\"message_type\":%d}",
            uid, msg.latitude, msg.longitude, msg.speed, msg.message_type);
            uart_puts(UART_TEST, "\r\n");
            uart_puts(UART_TEST, json);
            uart_puts(UART_TEST, "\r\n");
            uart_send_until_valid(UART_ID_4G, "AT+HTTPTERM\r\n", response, "AT+HTTPTERM\r\r\nERROR\r\n");
            vTaskDelay(pdMS_TO_TICKS(50));
            uart_send_until_valid(UART_ID_4G, "AT+HTTPINIT\r\n", response, "AT+HTTPINIT\r\r\nOK\r\n");
            vTaskDelay(pdMS_TO_TICKS(50));
            uart_send(UART_ID_4G, "AT+HTTPPARA=\"URL\",\"https://us-central1-test-f1e70.cloudfunctions.net/rms_data\"\r\n", response, 0);
            vTaskDelay(pdMS_TO_TICKS(50));
            uart_send(UART_ID_4G, "AT+HTTPPARA=\"CONTENT\",\"application/json\"\r\n", response, 0);
            vTaskDelay(pdMS_TO_TICKS(50));
            uart_send(UART_ID_4G, "AT+HTTPDATA=107,5000\r\n", response, 0);
            vTaskDelay(pdMS_TO_TICKS(50));
            // use response string to create json string with message structure
            uart_send(UART_ID_4G, json, response, 0);
            vTaskDelay(pdMS_TO_TICKS(50));
            uart_send(UART_ID_4G, "\n\r\n", response, 500);
            vTaskDelay(pdMS_TO_TICKS(50));
            uart_send(UART_ID_4G, "AT+HTTPACTION=1\r\n", response2, 500);
            // verify http response of 200 if failed, then repeat until it doesn't for x amount of times
            strncpy(http_code, response2 + 39, sizeof(http_code) - 1);
            http_code[sizeof(http_code) - 1] = '\0';

            uart_puts(UART_TEST, "\r\nHTTP CODE: ");
            uart_puts(UART_TEST, http_code);
            uart_puts(UART_TEST, "\r\n");

            vTaskDelay(pdMS_TO_TICKS(50));
            uart_send_until_valid(UART_ID_4G, "AT+HTTPTERM\r\n", response, "AT+HTTPTERM\r\r\nERROR\r\n");
            uart_puts(UART_TEST, "Message Sent\r\n");
        }
        // check speed limit queue and if not empty, send speed limit api request to google for current location
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void vTaskNormal(void * parameters)
{
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(10000));
        //get speed limit, check speed, if speed much lower than posted speed, send conjection request
        // vTaskDelay(pdMS_TO_TICKS(5000));
        struct message x;
        // if(gps_queue_peek(&y))
        // {
            x.latitude = 50.416721;//gps.latitude;
            x.longitude = -104.589579;//gps.longitude;
            x.message_type = 0;
            x.speed = 20;
            // static char json[512];
            // sprintf(json, "\r\n\"latitude\":%.6lf,\r\n\"longitude\":%.6lf,\r\n", x.latitude, x.longitude);
            // uart_puts(UART_TEST, json);
            message_enqueue(x);
            // uart_puts(UART_TEST, "message enqueued");
        // }
        
    }
}

void vTaskUart_OBD(void * parameters)
{
    struct obd2_packet packet;
    struct gps gps;
    char response[150];
    uart_send_until_valid(UART_ID_OBD2, "ATZ\r\n", response, "ATZ\r\r\rELM327 v1.4b\r\r>");
    vTaskDelay(pdMS_TO_TICKS(1000));

    while (1)
    {
        char stringValue[6];
        uart_obd2_wheel_speed(UART_ID_OBD2, &packet);
        // sprintf(response, "\r\n\"Slipping\":%d,\r\n\"Speed\":%d,\r\n", packet.slipping, packet.vehicle_speed);
        // uart_puts(UART_TEST, response);
        vehicle_speed_queue_overwrite(packet.vehicle_speed);
        if (packet.slipping == 16)
        {
            if(gps_queue_peek(&gps))
            {
                struct message message;
                message.latitude = gps.latitude;
                message.longitude = gps.longitude;
                message.message_type = SLIPPING;
                message.speed = packet.vehicle_speed;
                message_enqueue(message);
            }
        }
        uart_puts(UART_TEST, "\r\n");
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
                    // uart_puts(UART_TEST, token);
                    // uart_puts(UART_TEST, "\r\n");
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
                        // uart_puts(UART_TEST, "UTC Time: ");
                        // uart_puts(UART_TEST, utc_time);
                        // uart_puts(UART_TEST, ", Latitude: ");
                        // if (strlen(latitude) > 0)
                        // {
                        //     uart_puts(UART_TEST, latitude);
                        //     uart_putc(UART_TEST, latitude_direction);
                        // }
                        // else
                        // {
                        //     uart_puts(UART_TEST, "N/A");
                        // }

                        // uart_puts(UART_TEST, ", Longitude: ");
                        // if (strlen(longitude) > 0)
                        // {
                        //     uart_puts(UART_TEST, longitude);
                        //     uart_putc(UART_TEST, longitude_direction);
                        // }
                        // else
                        // {
                        //     uart_puts(UART_TEST, "N/A");
                        // }
                        // uart_puts(UART_TEST, "\r\n");
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
                        sprintf(json, "\r\n\"latitude\":%.6lf,\r\n\"longitude\":%.6lf,\r\n", x.latitude, x.longitude);
                        uart_puts(UART_TEST, json);
                        uart_puts(UART_TEST, "\r\n");
                        
                        if (positioning_status != 0 && num_satellites > 3)
                        {
                            gps_queue_overwrite(x);
                            // uart_puts(UART_TEST, "GPS Position enqueued");
                            // uart_puts(UART_TEST, "\r\n");
                        }
                    }
                }
            }
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

// bundleTask
void bundle_task(void *pvParameters) {
    uart_puts(UART_TEST, "\r\nStarting IMU Test\r\n");
    struct gps gps;
    
    TickType_t xLastWakeTime;
    const TickType_t xDelay = pdMS_TO_TICKS(100);
    int byte_level = 0;

    // Initialize the last wake time
    xLastWakeTime = xTaskGetTickCount();

    while (true) {

    //check if either of the global variables for pico2 and pico2 become true
    if(pico1_interrupt)
    {
        // uart_puts(UART_TEST, "\r\nTest2\r\n");
        //start a non blocking timer for the next 500ms 
        TickType_t xStartTime = xTaskGetTickCount();
        byte_level = byte_pico1;

        // within the next 500ms 
        while (xTaskGetTickCount() - xStartTime < xDelay) 
        {
            if(pico2_interrupt)
            {
                // uart_puts(UART_TEST, "bundle test \r\n");
                if (byte_pico1 >= byte_pico2) 
                {
                    uart_puts(UART_TEST, "Bundle data from PICO1 pico1_interrupt \r\n");
                    byte_level = byte_pico1;
                    // display the value of the byte
                    // uart_puts(UART_TEST, "byte_pico1:");
                    // char buffer[20]; // Allocate a buffer to hold the formatted string
                    // snprintf(buffer, sizeof(buffer), "%d", byte_pico1);
                    // uart_puts(UART_TEST, buffer);
                    // uart_puts(UART_TEST, "\r\n");
                    send_message(byte_level);

                    // Bundle data from PICO1
                } 
                else 
                {
                    uart_puts(UART_TEST, "Bundle data from PICO2 pico1_interrupt\r\n");
                    // Bundle data from PICO2
                    byte_level = byte_pico2;
                    // display the value of the byte
                    // uart_puts(UART_TEST, "byte_pico2:");
                    // char buffer[20]; // Allocate a buffer to hold the formatted string
                    // snprintf(buffer, sizeof(buffer), "%d", byte_pico2);
                    // uart_puts(UART_TEST, buffer);
                    // uart_puts(UART_TEST, "\r\n");
                    send_message(byte_level);
                }

                break;
            }
        }

        // If the other interrupt did not occur within 500ms, bundle the byte level from pico1
        if (pico2_interrupt == false) 
        {
            // send the byte level from pico1_interrupt
            byte_level = byte_pico1;
                // display the value of the byte
            // uart_puts(UART_TEST, "byte_pico1:");
            // char buffer[20]; // Allocate a buffer to hold the formatted string
            // snprintf(buffer, sizeof(buffer), "%d", byte_pico1);
            // uart_puts(UART_TEST, buffer);
            // uart_puts(UART_TEST, "\r\n");
            send_message(byte_level);
            
        }
    }
    else if(pico2_interrupt)
    {
        //start a non blocking timer for the next 500ms 
        TickType_t xStartTime = xTaskGetTickCount();
        byte_level = byte_pico2;

        // within the next 500ms 
        while (xTaskGetTickCount() - xStartTime < xDelay) 
        {
            if(pico1_interrupt)
            {
                if (byte_pico1 >= byte_pico2) 
                {
                    uart_puts(UART_TEST,"Bundle data from PICO1 pico2_interrupt\r\n");
                    byte_level = byte_pico1;
                    send_message(byte_level);

                } 
                else 
                {
                    uart_puts(UART_TEST,"Bundle data from PICO2 pico2_interrupt\r\n");
                    byte_level = byte_pico2;
                    send_message(byte_level);
                }

                break;
            }
        }

        // If the other interrupt did not occur within 500ms, bundle the byte level from pico2
        if (pico1_interrupt == false) 
        {
            // send the byte level from pico2_interrupt
            byte_level = byte_pico2;
            send_message(byte_level);
            
        }
    }

    //reset the values 
    pico1_interrupt = false;
    pico2_interrupt = false;
    byte_pico1 = 0;
    byte_pico2 = 0;

    // Delay to avoid continuous looping
        vTaskDelay(pdMS_TO_TICKS(100));

    }
}

