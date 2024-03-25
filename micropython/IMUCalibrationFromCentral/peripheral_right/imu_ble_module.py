import MPU6050
from ble_module import BLEImu
from ble_advertising import advertising_payload
import machine
import bluetooth
import time
import utime

class IMU_BLE:
    def __init__(self):
        
        ###initialization
        
        ### IMU - configure
        # Set up the I2C interface
        self.i2c = machine.I2C(1, sda=machine.Pin(14), scl=machine.Pin(15))
        
        # Set up the MPU6050 class 
        self.mpu = MPU6050.MPU6050(self.i2c)
        
        # Set up the BLE
        self.ble = bluetooth.BLE()
        self.imu_peripheral = BLEImu(self.ble)
        self.calibrated_average = 0
        self.calibrated_average_offset = 0
        self.centering_value = 0
        self.accel_z = 0

    def initialize(self):
        # Wake up the MPU6050 from sleep
        self.mpu.wake()

    def calibrate_sensor(self):
        # Automatically calibrate the IMU sensor
        # Calculate the average raw input from the IMU in neutral position
        self.calibrated_average = self.mpu._auto_calibrate_average(3)
        
        # Calculate the offset needed to make 1g on the IMU
        self.calibrated_average_offset = self.mpu._auto_calibrate_offset(3)
        
        # Use the last read from calibration to determine value to zero the raw data
        self.centering_value = self.mpu.read_accel_data()[2] + self.calibrated_average_offset
        print("Center:", self.centering_value, "\n")
        print("-------------------------------\n")
        time.sleep_ms(2000)
        
    def read_imu(self):
        ### READ IMU
        # Read accelerometer data (acceleration along the Z-axis) and center value around 0
        self.accel_z = self.mpu.read_accel_data()[2] + self.calibrated_average_offset
        self.accel_z -= self.centering_value
    
    def send_imu_data_over_ble(self):
        # sending the value of the IMU to the central
        i = 0
        self.imu_peripheral._send_pothole_event(self.accel_z, notify=i == 0, indicate=True)

    def run(self):
        
        self.read_imu()
        self.send_imu_data_over_ble()
        time.sleep_ms(5)