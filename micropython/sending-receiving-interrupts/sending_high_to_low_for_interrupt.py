import machine
import utime

# GPIO pins
GPIO_PIN_1 = 16
GPIO_PIN_2 = 17
LED_PIN = 25

# Initialize GPIO pins
pin_1 = machine.Pin(GPIO_PIN_1, machine.Pin.OUT)
pin_1.value(1)  # Set GP16 high initially
pin_2 = machine.Pin(GPIO_PIN_2, machine.Pin.OUT)
pin_2.value(1)  # Set GP17 high initially
led = machine.Pin(LED_PIN, machine.Pin.OUT)

# Main loop
while True:
    pin_2.value(1)  # Set GP16 low
    pin_2.value(0)  # Set GP16 high
    utime.sleep_ms(300)  # Sleep for 1 second
    
    pin_1.value(1)  # Set GP17 low
    pin_1.value(0)  # Set GP17 high
    utime.sleep_ms(300)  # Sleep for 1 second
    
    break

