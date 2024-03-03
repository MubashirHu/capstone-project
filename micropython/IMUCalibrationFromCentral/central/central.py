# This example finds and connects to a BLE temperature sensor (e.g. the one in ble_temperature.py).

# This example demonstrates the low-level bluetooth module. For most
# applications, we recommend using the higher-level aioble library which takes
# care of all IRQ handling and connection management. See
# https://github.com/micropython/micropython-lib/tree/master/micropython/bluetooth/aioble
# and in particular the temp_client.py example included with aioble.
import bluetooth
import random
import struct
import time
import micropython
import ble_advertising
import machine
from ble_advertising import decode_services
from ble_module import BLEImuCentral
from micropython import const

POTHOLE_EVENT_PIN = 25 #GP19
BIT_0_PIN = 24 #GP18
BIT_1_PIN = 22 #GP17
BIT_2_PIN = 21 #GP16

pothole_event_pin = machine.Pin(POTHOLE_EVENT_PIN, machine.Pin.OUT)
bit_0 = machine.Pin(BIT_0_PIN, machine.Pin.OUT)
bit_1 = machine.Pin(BIT_1_PIN, machine.Pin.OUT)
bit_2 = machine.Pin(BIT_2_PIN, machine.Pin.OUT)

#set default values for the pin
pothole_event_pin.value(1)
bit_0.value(0)
bit_1.value(0)
bit_2.value(0)

# Define Constants
WINDOW_SIZE = 40  # Size of the sliding window for averaging
GREEN_ZONE = 12
YELLOW_ZONE = 30
AMBER_ZONE = 35
RED_ZONE = 40

# Global variable to track the start time
start_time = None
zone_time = None
WAITING = 0
WAITING_FOR_HIGHEST_VALUE = 1
WAITING_FOR_RETURN_TO_CENTERED_VALUE = 2
highest_value = 0

def _determine_threshold_crossing(average_accel, green_zone):
    
    global start_time, zone_time, highest_value, GREEN_ZONE
    
    # Check if the start time has been initialized
    if start_time is None:
        start_time = time.time()  # Initialize start time on first function call
        
    # Check if 4 seconds have passed since the start time
    if time.time() - start_time < 4:
        return WAITING
    
    ###########################################
        
    #if avg_accel has crossed green_zone and timer has not been started
    if(average_accel > green_zone and zone_time is None):
        
        #highest value is the first read
        highest_value = average_accel
        
        # start zone_time  
        zone_time = time.time()  # Initialize start time on first function call
        return WAITING_FOR_HIGHEST_VALUE
    
    # if the timer has started
    if(zone_time is not None):
        
        #for the next 0.5 seconds find the highest value
        if time.time() - zone_time < 500:
            
            #Update highest value
            if average_accel > highest_value :
                highest_value = average_accel
                print("highest value", highest_value)
                return WAITING_FOR_HIGHEST_VALUE
        
        if average_accel > 1 :
                return WAITING_FOR_RETURN_TO_CENTERED_VALUE
        else:
            return highest_value
    
    highest_value = 0
    return WAITING
    
def _determine_zone(value):
    
    global highest_value
    highest_value = 0
    
    if(value > AMBER_ZONE):
        return RED_ZONE
    if(value > YELLOW_ZONE):
        return AMBER_ZONE
    elif (value > GREEN_ZONE ):
        return YELLOW_ZONE
    elif (value < GREEN_ZONE):
        return GREEN_ZONE
    
def main():
    global highest_value, zone_time
    ble = bluetooth.BLE()
    central = BLEImuCentral(ble)
    window_buffer = [9] * WINDOW_SIZE
        
    not_found = False

    def on_scan(addr_type, addr, name):
        if addr_type is not None:
            print("Found sensor:", addr_type, addr, name)
            central.connect()
        else:
            nonlocal not_found
            not_found = True
            print("No sensor found.")

    while True:
        central.scan(callback=on_scan)

        # Wait for connection...
        while not central.is_connected():
            time.sleep_ms(1000)
            if not_found:
                return  # Exit if no sensor found

        print("Connected")
        
        # Continuously issue read operations
        while central.is_connected():
            print("highest_value", highest_value)
            
            # Read accelerometer data (acceleration along the Z-axis)
            accel_z = central.value()
            
            # Add new data to the pothole window_buffer
            window_buffer.pop(0)
            window_buffer.append(accel_z)
            
            # Calculate the average acceleration value from the window_buffer
            avg_accel_z = (sum(window_buffer) / len(window_buffer))
            
            # square the value to create disparity as well as removing negative values
            avg_accel_z = avg_accel_z**2
            
            thresholding_result = _determine_threshold_crossing(avg_accel_z, GREEN_ZONE)
            
            if(thresholding_result == WAITING):
                pass
            elif(thresholding_result == WAITING_FOR_HIGHEST_VALUE):
                pass
            elif(thresholding_result == WAITING_FOR_RETURN_TO_CENTERED_VALUE):
                pass
            elif(thresholding_result > GREEN_ZONE):
                
                high_value = thresholding_result
                zone_result = _determine_zone(high_value)
                
                print("zone_result", zone_result)
                if zone_result is RED_ZONE:
                    print("RED ZONE")
#                     pothole_event_pin.value(0)
#                     bit_0.value(1)
#                     bit_1.value(1)
#                     bit_2.value(0)
                    #return
                elif zone_result is AMBER_ZONE:
                    print("AMBER ZONE")
#                     pothole_event_pin.value(0)
#                     bit_0.value(0)
#                     bit_1.value(1)
#                     bit_2.value(0)
                    #return
                elif zone_result is YELLOW_ZONE:
                    print("YELLOW ZONE")
#                     pothole_event_pin.value(0)
#                     bit_0.value(1)
#                     bit_1.value(0)
#                     bit_2.value(0)
                    #return        
        
            time.sleep_ms(10)  # Sleep to avoid flooding the connection
           
        print("Disconnected")

if __name__ == "__main__":
    main()
