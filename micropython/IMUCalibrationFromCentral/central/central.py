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
from thresholding import ThresholdCrossing
from thresholding import WAITING, WAITING_FOR_HIGHEST_VALUE, WAITING_FOR_RETURN_TO_CENTERED_VALUE
from micropython import const

WINDOW_SIZE = 25 # Size of the sliding window for averaging
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
    
def main():
    
    ble = bluetooth.BLE()
    central = BLEImuCentral(ble)
    threshold_crossing = ThresholdCrossing()
    
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
            
            # Read accelerometer data (acceleration along the Z-axis)
            accel_z = central.value()
            
            # Add new data to the pothole window_buffer
            window_buffer.pop(0)
            window_buffer.append(accel_z)
            
            # Calculate the average acceleration value from the window_buffer
            avg_accel_z = (sum(window_buffer) / len(window_buffer))
            
            # square the value to create disparity as well as removing negative values
            avg_accel_z = avg_accel_z**2
            
            thresholding_result = threshold_crossing._determine_threshold_crossing(avg_accel_z)
            #print("threshold_crossing.highest_value", threshold_crossing.highest_value)
            #print("thresholding_result", threshold_crossing.highest_value)
            
            if(thresholding_result > threshold_crossing.green_zone):
                threshold_crossing._determine_zone(thresholding_result)
             
            time.sleep_ms(20)
           
        print("Disconnected")

if __name__ == "__main__":
    main()
    
#             if(thresholding_result == WAITING):
#                 pass
#             elif(thresholding_result == WAITING_FOR_HIGHEST_VALUE):
#                 pass
#             elif(thresholding_result == WAITING_FOR_RETURN_TO_CENTERED_VALUE):
#                 pass
#             elif(thresholding_result > GREEN_ZONE):
#                 #print("thresholding_result", thresholding_result)
#                 zone_result = _determine_zone(thresholding_result)
#                 
#                 if zone_result is RED_ZONE:
#                     print("RED ZONE")
# #                     pothole_event_pin.value(0)
# #                     bit_0.value(1)
# #                     bit_1.value(1)
# #                     bit_2.value(0)
#                     #return
#                 elif zone_result is AMBER_ZONE:
#                     print("AMBER ZONE")
# #                     pothole_event_pin.value(0)
# #                     bit_0.value(0)
# #                     bit_1.value(1)
# #                     bit_2.value(0)
#                     #return
#                 elif zone_result is YELLOW_ZONE:
#                     print("YELLOW ZONE")
# #                     pothole_event_pin.value(0)
# #                     bit_0.value(1)
# #                     bit_1.value(0)
# #                     bit_2.value(0)
#                     #return
