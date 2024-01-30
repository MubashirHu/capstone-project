import machine
import utime

# Configure UART port (UART0)
uart = machine.UART(0, baudrate=115200, tx=None, rx=1)  # Change baudrate and RX pin as needed

# Main loop
while True:
    if uart.any():
        # Read data from UART
        data = uart.readline()
        if data:
            # Decode and print received data
            print("Received:", data.decode().strip())
    utime.sleep_ms(100)  # Adjust sleep time as needed
