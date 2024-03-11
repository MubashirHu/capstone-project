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

from micropython import const
WINDOW_SIZE = 10  # Size of the sliding window for averaging
         
def main():
    
    # Initialize Variables
    window_buffer = [9] * WINDOW_SIZE
    
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
    calibratedAverage = mpu._auto_calibrate_average(3)
    
    # Calculate the offset needed to make 1g on the IMU
    calibratedAverageOffset = mpu._auto_calibrate_offset(3)
    
    # Use the last read from calibration to determine value to zero the raw data
    centeringValue = mpu.read_accel_data()[2] + calibratedAverageOffset
    print("center",centeringValue,"\n")
    
    time.sleep_ms(2000)
    
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

if __name__ == "__main__":
    main()
