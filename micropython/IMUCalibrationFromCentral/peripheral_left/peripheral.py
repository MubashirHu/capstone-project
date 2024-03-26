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
from ble_module import BLEImu
from ble_advertising import advertising_payload
import MPU6050
import machine
import sys

from micropython import const

window_buffer = [9] * 20
window_buffer2 = [9] * 10
def main():
    while True:
        try:
            # Initialize Variables            
            ### IMU - configure
            # Set up the I2C interface
            i2c = machine.I2C(1, sda=machine.Pin(14), scl=machine.Pin(15))
            # Set up the MPU6050 class 
            mpu = MPU6050.MPU6050(i2c)
            # Wake up the MPU6050 from sleep
            mpu.wake()
            
            ### BLE - configure
            ble = bluetooth.BLE()
            imu_peripheral = BLEImu(ble)
            
            # Automatically calibrate the IMU sensor
            # Calculate the average raw input from the IMU in neutral position
            calibratedAverage = mpu._auto_calibrate_average(3, window_buffer)
            
            # Calculate the offset needed to make 1g on the IMU
            calibratedAverageOffset = mpu._auto_calibrate_offset(3, window_buffer2)
            
            # Use the last read from calibration to determine value to zero the raw data
            centeringValue = mpu.read_accel_data()[2] + calibratedAverageOffset
            print("center",centeringValue,"\n")
            
            while True:
                ### READ IMU
                # Read accelerometer data (acceleration along the Z-axis) and center value around 0
                accel_z = mpu.read_accel_data()[2] + calibratedAverageOffset
                #print("current read", accel_z)
                #print("centeringValue", centeringValue)
                
                accel_z = accel_z - centeringValue
                #print("final", accel_z)
                #sending the value of the IMU to the central
                i = 0
                imu_peripheral._send_pothole_event(accel_z, notify=i == 0, indicate=True)
                         
                time.sleep_ms(5)
                #raise OSError(5, "Invalid argument (EINVAL)")
                
        except OSError as e:
            if e.args[0] == 22:  # Check if the errno is EINVAL (22)
                print("Caught OSError: Invalid argument (EINVAL). Please check your inputs.")
                time.sleep_ms(2000)
                pass
            elif e.args[0] == 5:
                print("IMU diconnected")
                time.sleep_ms(2000)
                pass
            elif e.args[1] == 110:
                print("IMU GND is disconnected")
                time.sleep_ms(2000)
                pass
            else:
                print("Caught OSError:", e)
                pass
                # Additional error handling code if needed
        except Exception as e :
            print("Last Exception", e)
            break

if __name__ == "__main__":
    main()

