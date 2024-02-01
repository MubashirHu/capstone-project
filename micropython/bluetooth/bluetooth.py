import machine
import time

# UART configuration, BAUD rate for the HC-05 has to be 38400 when in AT mode
uart = machine.UART(0, baudrate=38400, tx=0, rx=1, timeout=1000)

def send_at_command(command):
    print(f'Sending AT command: {command.decode("utf-8")}')
    uart.write(command + b'\r\n')
    time.sleep(1)  # Adjust the sleep duration based on the response time
    response = uart.read()
    if response is not None:
        print(f'Response: {response.decode("utf-8")}')
        if b'OK' in response:
            print('Command executed successfully.')
        elif b'ERROR' in response:
            print('Command execution failed.')
    else:
        print('No response received.')


# slave mode sequence
# Example: Send AT command to check module status
#send_at_command(b'AT')
#send_at_command(b'AT+INQ')
#send_at_command(b'AT+ROLE=0')
#send_at_command(b'AT+NAME=Mubashir')
#send_at_command(b'AT+NAME?')
#send_at_command(b'AT+UART=9600,0,0')
#send_at_command(b'AT+RESET')

# No authentication required
send_at_command(b'AT+INIT')
send_at_command(b'AT+INQ')
send_at_command(b'AT+CMODE=1')
send_at_command(b'AT+NAME=Mubashir')
send_at_command(b'AT+PSWD="0000"')
send_at_command(b'AT+UART=9600,0,0')
send_at_command(b'AT+RESET')

# Define the GPIO pin to know whether Bluetooth is connected
gpio_pin = machine.Pin(13, machine.Pin.IN)

# Main loop
while True:
    # Read GPIO 13 state
    state = gpio_pin.value()
    
    if state:
        print("GPIO 13 is HIGH.")
    else:
        print("GPIO 13 is LOW.")

    time.sleep(1)  # Adjust the sleep duration based on your requirements
