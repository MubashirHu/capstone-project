import machine
import time

# Define GPIO pin
GPIO_PIN = 19  # Example GPIO pin
GPIO_PIN2 = 18  # Example GPIO pin

# Initialize GPIO pin
gpio_ph = machine.Pin(GPIO_PIN, machine.Pin.OUT)
gpio_rd = machine.Pin(GPIO_PIN2, machine.Pin.OUT)

# set to high initially
gpio_ph.value(1)
gpio_rd.value(1)
    
time.sleep(5)

# Main loop
while True:    
    print("Interrupt on pin 19")
    gpio_ph.value(0)
    
    time.sleep(5)
    print("Returned pin 19 to HIGH")
    gpio_ph.value(1)
    
    time.sleep(5)
    print("Interrupt on pin 18")
    gpio_rd.value(0)
    
    time.sleep(5)
    print("Returned pin 18 to HIGH")
    gpio_rd.value(1)
    
    time.sleep(5)