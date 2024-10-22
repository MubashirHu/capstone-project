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
import machine
led = machine.Pin("LED", machine.Pin.OUT)
led.off()

WINDOW_SIZE = 50 # Size of the sliding window for averaging
    
def main():
    while True:
        
        print("Waiting 3 seconds")
        time.sleep(3)
        
        print("Trying to connect to a BLE peripheral")
        ble = bluetooth.BLE()
        central = BLEImuCentral(ble)
        thresholding = Threshold()
        
        window_buffer = [9] * WINDOW_SIZE
            
        not_found = False

        def on_scan(addr_type, addr, name):
            if addr_type is not None:
                print("Found sensor:", addr_type, addr, name)
                x = central.connect()
                print("x", x)
                print("Connected")
                led.on()
            else:
                nonlocal not_found
                not_found = True
                print("No sensor found.")
                led.off()

        while True:
            central.scan(callback=on_scan)

            # Wait for connection...
            while not central.is_connected():
                time.sleep_ms(1000)
                if not_found:
                    break  # Exit if no sensor found

            # Continuously issue read operations
            while central.is_connected():
                
                # Read accelerometer data (acceleration along the Z-axis)
                accel_z = central.value()
                
                # Check if accel_z is None
                if accel_z is not None:
                    led.on()
                    # Add new data to the pothole window_buffer
                    window_buffer.pop(0)
                    window_buffer.append(accel_z)
                    
                    # Print window_buffer and accel_z for debugging
                    print("Window Buffer:", window_buffer)
                    print("Current Accel Z:", accel_z)
                    
                    # Calculate the average acceleration value from the window_buffer
                    avg_accel_z = (sum(window_buffer) / len(window_buffer))
                    
                    # square the value to create disparity as well as removing negative values
                    avg_accel_z = avg_accel_z**2
                    print("Average Accel Z:", avg_accel_z)
                    
                    highest_value_determined = thresholding._determine_highest_value_during_threshold_crossing(avg_accel_z)
                    
                    if(highest_value_determined > thresholding.green_zone):
                        determined_zone = thresholding._determine_zone(highest_value_determined)
                        thresholding._transmit_zone(determined_zone)
                    else:
                        thresholding._transmit_zone(thresholding.green_zone)
                        pass
                else:
                    print("Warning: accel_z is None because data is not being sent, due to peripheral calibration")
                    led.toggle()
                    time.sleep_ms(100)
                time.sleep_ms(10)
               
            break

if __name__ == "__main__":
    main()




