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
import random
import struct
import time
from ble_advertising import advertising_payload
import MPU6050


from micropython import const

# Set up the I2C interface
i2c = machine.I2C(1, sda=machine.Pin(14), scl=machine.Pin(15))

# Set up the MPU6050 class 
mpu = MPU6050.MPU6050(i2c)

# Wake up the MPU6050 from sleep
mpu.wake()

# Define Constants
THRESHOLD_LOW = 9  # Lower threshold value for detecting potholes (in m/s^2)
THRESHOLD_HIGH = 10  # Upper threshold value for detecting potholes (in m/s^2)
WINDOW_SIZE = 10  # Size of the sliding window for averaging
MIN_POTHOLE_DURATION = 10  # Minimum duration for a pothole event (in milliseconds)
CALIBRATED_VALUE = 0.48

# Initialize Variables
window_buffer = [0] * WINDOW_SIZE
pothole_detected = False
pothole_duration = 0

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

    def _send_pothole_event(self, temp_deg_c, notify=False, indicate=False):
        # Data is sint16 in degrees Celsius with a resolution of 0.01 degrees Celsius.
        # Write the local value, ready for a central to read.
        self._ble.gatts_write(self._handle, struct.pack("<h", int(temp_deg_c * 100)))
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


def main():
    ble = bluetooth.BLE()
    imu_peripheral = BLEImu(ble)

    t = 25
    i = 0

    imu_peripheral._send_pothole_event(t, notify=i == 0, indicate=False)
    
    while True:
        ### READ IMU
        # Read accelerometer data (acceleration along the Z-axis)
        accel_z = mpu.read_accel_data()[2]
        
        # Add new data to the window_buffer
        window_buffer.pop(0)
        window_buffer.append(accel_z)
        
        # Calculate the average acceleration value from the window_buffer
        avg_accel_z = (sum(window_buffer) / len(window_buffer)) - CALIBRATED_VALUE
        print("Avg Acc: z axis:", avg_accel_z)
        #print(len(window_buffer))
        for i in range(10):
            print(window_buffer[i])
            
        ## DETERMINE POTHOLE
        
        
        # Write every second, notify every 10 seconds.
        i = 0
        #imu_peripheral._send_pothole_event(t, notify=i == 0, indicate=True)
        imu_peripheral._send_pothole_event(pothole_detected, notify=i == 0, indicate=True)
        
        time.sleep_ms(1000)

if __name__ == "__main__":
    main()

