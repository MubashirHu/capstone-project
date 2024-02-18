import machine
import utime

# Configure UART port (UART0)
uart = machine.UART(0, baudrate=115200, tx=None, rx=1)  # RX pin is GP1

# Main loop
while True:
    if uart.any():
        # Read data from UART
        data = uart.read(uart.any())  # Read all available bytes
        if data:
            try:
                # Decode and print received data
                print("Received:", data.decode('utf-8'))
            except UnicodeError:
                # Print raw data if decoding fails
                print("Received (raw):", data)
    utime.sleep_ms(100)  # Adjust sleep time as needed