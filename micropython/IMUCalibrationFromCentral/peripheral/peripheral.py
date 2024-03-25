# This example demonstrates a simple temperature sensor peripheral.
#
# The sensor's local value updates every second, and it will notify
# any connected central every 10 seconds.

# This example demonstrates the low-level bluetooth module. For most
# applications, we recommend using the higher-level aioble library which takes
# care of all IRQ handling and connection management. See
# https://github.com/micropython/micropython-lib/tree/master/micropython/bluetooth/aioble
# and in particular the temp_sensor.py example included with aioble.
import bluetooth
import struct
import time
import utime
import sys
from micropython import const
from imu_ble_module import IMU_BLE

def main():
    
    while True:
        imu_ble_peripheral = IMU_BLE()
        imu_ble_peripheral.initialize()
        imu_ble_peripheral.calibrate_sensor()
        
        try:
            while True:
                imu_ble_peripheral.run()
                #raise OSError(22, "Invalid argument (EINVAL). Please check your inputs.")
                
        except OSError as e:
            if e.args[0] == 22:  # Check if the errno is EINVAL (22)
                print("Caught OSError: Invalid argument (EINVAL). Please check your inputs.")
                #time.sleep_ms(2000)
                pass
            elif e.args[0] == 5:
                print("IMU diconnected")
                time.sleep_ms(4000)
                
                pass
            elif e.args[1] == 110:
                print("IMU GND is disconnected")
                sys.exit(1)
                
            else:
                print("Caught OSError:", e)

if __name__ == "__main__":
    main()
