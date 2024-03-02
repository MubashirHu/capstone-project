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

from ble_advertising import decode_services, decode_name

from micropython import const

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
WAITING_FOR_VALUE = 1
highest_value = 0

_IRQ_CENTRAL_CONNECT = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)
_IRQ_GATTS_WRITE = const(3)
_IRQ_GATTS_READ_REQUEST = const(4)
_IRQ_SCAN_RESULT = const(5)
_IRQ_SCAN_DONE = const(6)
_IRQ_PERIPHERAL_CONNECT = const(7)
_IRQ_PERIPHERAL_DISCONNECT = const(8)
_IRQ_GATTC_SERVICE_RESULT = const(9)
_IRQ_GATTC_SERVICE_DONE = const(10)
_IRQ_GATTC_CHARACTERISTIC_RESULT = const(11)
_IRQ_GATTC_CHARACTERISTIC_DONE = const(12)
_IRQ_GATTC_DESCRIPTOR_RESULT = const(13)
_IRQ_GATTC_DESCRIPTOR_DONE = const(14)
_IRQ_GATTC_READ_RESULT = const(15)
_IRQ_GATTC_READ_DONE = const(16)
_IRQ_GATTC_WRITE_DONE = const(17)
_IRQ_GATTC_NOTIFY = const(18)
_IRQ_GATTC_INDICATE = const(19)

_ADV_IND = const(0x00)
_ADV_DIRECT_IND = const(0x01)
_ADV_SCAN_IND = const(0x02)
_ADV_NONCONN_IND = const(0x03)

# org.bluetooth.service.environmental_sensing
_ENV_SENSE_UUID = bluetooth.UUID("c5e15ad3-bcf5-4cc1-a40a-899931a69a3b")
# org.bluetooth.characteristic.temperature
_TEMP_UUID = bluetooth.UUID(0x2A6E)

# org.bluetooth.service.sensor1
_SENS1_UUID = bluetooth.UUID("0f5b161f-5f75-49bd-b435-13f88527952d")
# org.bluetooth.characteristic.message
_VAL1_UUID = bluetooth.UUID(0x2A6F)

class BLEImuCentral:
    def __init__(self, ble):
        self._ble = ble
        self._ble.active(True)
        self._ble.irq(self._irq)

        self._reset()

    def _reset(self):
        # Cached name and address from a successful scan.
        self._name = None
        self._addr_type = None
        self._addr = None

        # Cached value (if we have one)
        self._value = None

        # Callbacks for completion of various operations.
        # These reset back to None after being invoked.
        self._scan_callback = None
        self._conn_callback = None
        self._read_callback = None

        # Persistent callback for when new data is notified from the device.
        self._notify_callback = None

        # Connected device.
        self._conn_handle = None
        self._start_handle = None
        self._end_handle = None
        self._value_handle = None

    def _irq(self, event, data):
        if event == _IRQ_SCAN_RESULT:
            addr_type, addr, adv_type, rssi, adv_data = data
            if adv_type in (_ADV_IND, _ADV_DIRECT_IND) and _ENV_SENSE_UUID in decode_services(
                adv_data
            ):
                # Found a potential device, remember it and stop scanning.
                self._addr_type = addr_type
                self._addr = bytes(
                    addr
                )  # Note: addr buffer is owned by caller so need to copy it.
                self._name = decode_name(adv_data) or "?"
                self._ble.gap_scan(None)
            if adv_type in (_ADV_IND, _ADV_DIRECT_IND) and _SENS1_UUID in decode_services(
                adv_data
            ):
                # Found a potential device, remember it and stop scanning.
                self._addr_type = addr_type
                self._addr = bytes(
                    addr
                )  # Note: addr buffer is owned by caller so need to copy it.
                self._name = decode_name(adv_data) or "?"
                self._ble.gap_scan(None)

        elif event == _IRQ_SCAN_DONE:
            if self._scan_callback:
                if self._addr:
                    # Found a device during the scan (and the scan was explicitly stopped).
                    self._scan_callback(self._addr_type, self._addr, self._name)
                    self._scan_callback = None
                else:
                    # Scan timed out.
                    self._scan_callback(None, None, None)

        elif event == _IRQ_PERIPHERAL_CONNECT:
            # Connect successful.
            conn_handle, addr_type, addr = data
            if addr_type == self._addr_type and addr == self._addr:
                self._conn_handle = conn_handle
                self._ble.gattc_discover_services(self._conn_handle)

        elif event == _IRQ_PERIPHERAL_DISCONNECT:
            # Disconnect (either initiated by us or the remote end).
            conn_handle, _, _ = data
            if conn_handle == self._conn_handle:
                # If it was initiated by us, it'll already be reset.
                self._reset()

        elif event == _IRQ_GATTC_SERVICE_RESULT:
            # Connected device returned a service.
            conn_handle, start_handle, end_handle, uuid = data
            if conn_handle == self._conn_handle and uuid == _ENV_SENSE_UUID:
                self._start_handle, self._end_handle = start_handle, end_handle
            if conn_handle == self._conn_handle and uuid == _SENS1_UUID:
                self._start_handle, self._end_handle = start_handle, end_handle

        elif event == _IRQ_GATTC_SERVICE_DONE:
            # Service query complete.
            if self._start_handle and self._end_handle:
                self._ble.gattc_discover_characteristics(
                    self._conn_handle, self._start_handle, self._end_handle
                )
            else:
                print("Failed to find environmental sensing service.")

        elif event == _IRQ_GATTC_CHARACTERISTIC_RESULT:
            # Connected device returned a characteristic.
            conn_handle, def_handle, value_handle, properties, uuid = data
            if conn_handle == self._conn_handle and uuid == _TEMP_UUID:
                self._value_handle = value_handle
            if conn_handle == self._conn_handle and uuid == _VAL1_UUID:
                self._value_handle = value_handle

        elif event == _IRQ_GATTC_CHARACTERISTIC_DONE:
            # Characteristic query complete.
            if self._value_handle:
                # We've finished connecting and discovering device, fire the connect callback.
                if self._conn_callback:
                    self._conn_callback()
            else:
                print("Failed to find temperature characteristic.")

        elif event == _IRQ_GATTC_READ_RESULT:
            # A read completed successfully.
            conn_handle, value_handle, char_data = data
            if conn_handle == self._conn_handle and value_handle == self._value_handle:
                self._update_value(char_data)
                if self._read_callback:
                    self._read_callback(self._value)
                    self._read_callback = None

        elif event == _IRQ_GATTC_READ_DONE:
            # Read completed (no-op).
            conn_handle, value_handle, status = data

        elif event == _IRQ_GATTC_NOTIFY:
            # The ble_temperature.py demo periodically notifies its value.
            conn_handle, value_handle, notify_data = data
            if conn_handle == self._conn_handle and value_handle == self._value_handle:
                self._update_value(notify_data)
                if self._notify_callback:
                    self._notify_callback(self._value)            

    # Returns true if we've successfully connected and discovered characteristics.
    def is_connected(self):
        return self._conn_handle is not None and self._value_handle is not None

    # Find a device advertising the environmental sensor service.
    def scan(self, callback=None):
        self._addr_type = None
        self._addr = None
        self._scan_callback = callback
        self._ble.gap_scan(2000, 30000, 30000)

    # Connect to the specified device (otherwise use cached address from a scan).
    def connect(self, addr_type=None, addr=None, callback=None):
        self._addr_type = addr_type or self._addr_type
        self._addr = addr or self._addr
        self._conn_callback = callback
        if self._addr_type is None or self._addr is None:
            return False
        self._ble.gap_connect(self._addr_type, self._addr)
        return True

    # Disconnect from current device.
    def disconnect(self):
        if self._conn_handle is None:
            return
        self._ble.gap_disconnect(self._conn_handle)
        self._reset()

    # Issues an (asynchronous) read, will invoke callback with data.
    def read(self, callback):
        if not self.is_connected():
            return
        self._read_callback = callback
        self._ble.gattc_read(self._conn_handle, self._value_handle)

    # Sets a callback to be invoked when the device notifies us.
    def on_notify(self, callback):
        self._notify_callback = callback

    def _update_value(self, data):
        # Data is sint16 in degrees Celsius with a resolution of 0.01 degrees Celsius.
        self._value = struct.unpack("<f", data)[0]
        return self._value

    def value(self):
        return self._value

def _determine_threshold_crossing(average_accel, green_zone):
    
    global start_time, zone_time, highest_value
    
    # Check if the start time has been initialized
    if start_time is None:
        start_time = time.time()  # Initialize start time on first function call
        
    # Check if 5 seconds have passed since the start time
    if time.time() - start_time < 5:
        return WAITING
    
    ###########################################
    
    #if avg_accel has crossed green_zone and timer has not been started
    if(average_accel > green_zone and zone_time is None):
        
        #highest value is the first read
        highest_value = average_accel
        
        # start zone_time  
        zone_time = time.time()  # Initialize start time on first function call
        return WAITING_FOR_VALUE
    
    # if the timer has started
    if(zone_time is not None):
        
        #for the next 3 seconds find the highest value
        if time.time() - zone_time < 300:
            
            #Update highest value
            if average_accel > highest_value :
                highest_value = average_accel
                print("highest value", highest_value)
                return WAITING_FOR_VALUE
        
        #At this point, 3 seconds have passed, So Reset timer and return value
        print("Timer has ended")
        zone_time = None #reset the timer
        return highest_value #return the highest value
    
    highest_value = 0
    return WAITING
    
def _determine_zone(value):
    if(value > AMBER_ZONE):
        return RED_ZONE
    if(value > YELLOW_ZONE):
        return AMBER_ZONE
    elif (value > GREEN_ZONE ):
        return YELLOW_ZONE
    elif (value < GREEN_ZONE):
        return GREEN_ZONE
    
def main():
    ble = bluetooth.BLE()
    central = BLEImuCentral(ble)
    window_buffer = [9] * WINDOW_SIZE
    
    pothole_detected = False
    road_depression_detected = False
    
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
            
            thresholding_result = _determine_threshold_crossing(avg_accel_z, GREEN_ZONE)
            
            if(thresholding_result == WAITING):
                print(avg_accel_z)
#                 print("WAITING")
#                 print("highest value:", highest_value)
#                 print("average z:", avg_accel_z)
                pass
            
            elif(thresholding_result == WAITING_FOR_VALUE):
#                 print("WAITING_FOR_VALUE")
#                 print("highest value:", highest_value)
#                 print("average z:", avg_accel_z)
                pass

            elif(thresholding_result > GREEN_ZONE):
                high_value = thresholding_result
#                 print("thresholding_result", thresholding_result)
#                 print("high_value", high_value)
                pass
                
                zone_result = _determine_zone(high_value)
                
                if zone_result is RED_ZONE:
                    print("RED ZONE")
                    #return
                elif zone_result is YELLOW_ZONE:
                    print("YELLOW ZONE")
                    #return
                else:
                    print("GREEN ZONE")
                    
            time.sleep_ms(10)  # Sleep to avoid flooding the connection
            
        print("Disconnected")

if __name__ == "__main__":
    main()
