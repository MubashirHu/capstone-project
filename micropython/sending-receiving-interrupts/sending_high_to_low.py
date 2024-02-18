import machine
import time

# Define GPIO pin
GPIO_PIN = 19  # Example GPIO pin
GPIO_PIN2 = 18

# Initialize GPIO pin
gpio_ph = machine.Pin(GPIO_PIN, machine.Pin.OUT)
gpio_rd = machine.Pin(GPIO_PIN2, machine.Pin.OUT)

# Main loop
while True:
    # Toggle GPIO pin
    print("Setting HIGH NOW")
    gpio_ph.value(1)
    gpio_rd.value(0)
    
    time.sleep(5)
    
    print("This should show an interrupt now")
    gpio_ph.value(0)
    
    # Wait for 2 seconds
    time.sleep(15)
