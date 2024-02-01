import machine
import time
import MPU6050

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

# Main Loop
while 1:
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
    
    # Check if the average acceleration value falls below THRESHOLD_LOW
    if avg_accel_z < THRESHOLD_LOW:
        if not pothole_detected:
            pothole_detected = True
            pothole_duration = 0
        else:
            pothole_duration += 1
    else:
        # If the average acceleration value rises above THRESHOLD_HIGH
        # Not fully functional yet
        if avg_accel_z > THRESHOLD_HIGH:
            if not pothole_detected:
                pothole_detected = True
                pothole_duration = 0
            else:
                pothole_duration += 1
                
        if pothole_detected and pothole_duration >= MIN_POTHOLE_DURATION:
            print("Pothole Detected!")
            print("Pothole duration:",pothole_duration)
        pothole_detected = False
        pothole_duration = 0
            
    # Delay for a short interval
    time.sleep(0.1)
