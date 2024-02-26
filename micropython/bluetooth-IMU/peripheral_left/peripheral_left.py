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
from ble_advertising import advertising_payload
import MPU6050
import machine

from micropython import const

EVENT_NOTIFYING_TIMEOUT = 100
last_pothole_time = 0
last_depression_time = 0

# Define Constants
THRESHOLD_HIGH_RD = 9 # Higher threshold value for detecting road depression
THRESHOLD_LOW_RD = 8 # Lower threshold value for detecting road depression
THRESHOLD_LOW_PH = 9  # Lower threshold value for detecting potholes (in m/s^2)
THRESHOLD_HIGH_PH = 10  # Upper threshold value for detecting potholes (in m/s^2)
WINDOW_SIZE = 10  # Size of the sliding window for averaging
MIN_ROAD_DEPRESSION_DURATION = 10  # Minimum duration for a pothole event (in milliseconds)

# POTHOLE EVENTS
POTHOLE_EVENT = 1
ROAD_DEPRESSION_EVENT = 2

value_needs_to_be_reset = False
calibratedValue = 0.0

###BT

_IRQ_CENTRAL_CONNECT = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)
_IRQ_GATTS_INDICATE_DONE = const(20)

_FLAG_READ = const(0x0002)
_FLAG_NOTIFY = const(0x0010)
_FLAG_INDICATE = const(0x0020)

# org.bluetooth.service.environmental_sensing
_ENV_SENSE_UUID = bluetooth.UUID("c5e15ad3-bcf5-4cc1-a40a-899931a69a3b")
# org.bluetooth.characteristic.temperature
_TEMP_CHAR = (
    bluetooth.UUID(0x2A6E),
    _FLAG_READ | _FLAG_NOTIFY | _FLAG_INDICATE,
)
_ENV_SENSE_SERVICE = (
    _ENV_SENSE_UUID,
    (_TEMP_CHAR,),
)

# org.bluetooth.characteristic.gap.appearance.xml
_ADV_APPEARANCE_GENERIC_THERMOMETER = const(768)


class BLEImu:
    def __init__(self, ble, name="leftIMU"):
        self._ble = ble
        self._ble.active(True)
        self._ble.irq(self._irq)
        ((self._handle,),) = self._ble.gatts_register_services((_ENV_SENSE_SERVICE,))
        self._connections = set()
        self._payload = advertising_payload(
            name=name, services=[_ENV_SENSE_UUID])
        self._advertise()

    def _irq(self, event, data):
        # Track connections so we can send notifications.
        if event == _IRQ_CENTRAL_CONNECT:
            conn_handle, _, _ = data
            self._connections.add(conn_handle)
        elif event == _IRQ_CENTRAL_DISCONNECT:
            conn_handle, _, _ = data
            self._connections.remove(conn_handle)
            # Start advertising again to allow a new connection.
            self._advertise()
        elif event == _IRQ_GATTS_INDICATE_DONE:
            conn_handle, value_handle, status = data

    def _send_pothole_event(self, event:float, notify=False, indicate=False):
        # Write the local value, ready for a central to read.
        self._ble.gatts_write(self._handle, struct.pack("<f", event))
        if notify or indicate:
            for conn_handle in self._connections:
                if notify:
                    # Notify connected centrals.
                    self._ble.gatts_notify(conn_handle, self._handle)
                if indicate:
                     # Indicate connected centrals.
                    self._ble.gatts_indicate(conn_handle, self._handle)

    def _advertise(self, interval_us=500000):
        self._ble.gap_advertise(interval_us, adv_data=self._payload)
        
# Non-blocking timer checking if 3 seconds seconds have passed since the last pothole/road event
def is_time_to_send():
    global last_pothole_time, last_depression_time
    current_time = utime.ticks_ms()
    if current_time - last_pothole_time >= 3000 and current_time - last_depression_time >= 3000:
        last_pothole_time = current_time
        last_depression_time = current_time
        return True
    else:
        return False
        
def main():
    
    # Initialize Variables
    window_buffer = [9] * WINDOW_SIZE
    pothole_detected = False
    road_depression_detected = False
    pothole_duration = 0
    road_depression_counter = 0
    
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
    
    ## Automatically calibrate the IMU sensor
    calibratedValue = mpu._auto_calibrate(2)
        
    while True:
        ### READ IMU
        # Read accelerometer data (acceleration along the Z-axis)
        accel_z = mpu.read_accel_data()[2]
        
        # Add new data to the window_buffer
        window_buffer.pop(0)
        window_buffer.append(accel_z)
        
        # Calculate the average acceleration value from the window_buffer
        avg_accel_z = (sum(window_buffer) / len(window_buffer)) + calibratedValue
        print("Avg Acc: z axis:", avg_accel_z)      
            
        ### DETERMINE WHETHER THRESHOLDS HAVE BEEN CROSSED
        pothole_detected = mpu._determine_threshold_crossing(avg_accel_z, THRESHOLD_LOW_PH, THRESHOLD_HIGH_PH)
        road_depression = mpu._determine_threshold_crossing(avg_accel_z, THRESHOLD_LOW_RD, THRESHOLD_HIGH_RD)
        
        #Get access to global variable
        global value_needs_to_be_reset
        
        #sending the value of the IMU to the central
        i = 0
        imu_peripheral._send_pothole_event(avg_accel_z, notify=i == 0, indicate=True)
        
        if value_needs_to_be_reset:
            imu_peripheral._send_pothole_event(0, notify=i == 0, indicate=True)
            print("here")
            value_needs_to_be_reset = False
            
        if(pothole_detected and is_time_to_send()):
            print("ph")
            print(len(window_buffer))
            i = 0
            imu_peripheral._send_pothole_event(POTHOLE_EVENT, notify=i == 0, indicate=True)
            pothole_detected = False
            value_needs_to_be_reset = True
            
        if(road_depression_detected and is_time_to_send()):
            print("rd")
            road_depression_counter = road_depression_counter + 1
            
            if(road_depression_counter > MIN_ROAD_DEPRESSION_DURATION):
                i = 0
                imu_peripheral._send_pothole_event(ROAD_DEPRESSION_EVENT, notify=i == 0, indicate=True)
                road_depression_counter = 0
                road_depression_detected = False
                value_needs_to_be_reset = False
                                
        time.sleep_ms(100)

if __name__ == "__main__":
    main()
