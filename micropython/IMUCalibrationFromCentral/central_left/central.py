# This example finds and connects to a BLE temperature sensor (e.g. the one in ble_temperature.py).

# This example demonstrates the low-level bluetooth module. For most
# applications, we recommend using the higher-level aioble library which takes
# care of all IRQ handling and connection management. See
# https://github.com/micropython/micropython-lib/tree/master/micropython/bluetooth/aioble
# and in particular the temp_client.py example included with aioble.
import bluetooth
import time
import micropython
import ble_advertising
from ble_advertising import decode_services
from ble_module import BLEImuCentral
from thresholding import Threshold
from micropython import const

WINDOW_SIZE = 50 # Size of the sliding window for averaging
    
def main():
    
    ble = bluetooth.BLE()
    central = BLEImuCentral(ble)
    thresholding = Threshold()
    
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
            #print(avg_accel_z)
            
            highest_value_determined = thresholding._determine_highest_value_during_threshold_crossing(avg_accel_z)
            
            if(highest_value_determined > thresholding.green_zone):
                determined_zone = thresholding._determine_zone(highest_value_determined)
                thresholding._transmit_zone(determined_zone)
            else:
                #thresholding._transmit_zone(thresholding.green_zone)
                pass
            time.sleep_ms(10)
           
        print("Disconnected")

if __name__ == "__main__":
    main()
