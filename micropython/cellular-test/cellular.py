import machine
import time

# UART configuration
uart = machine.UART(0, baudrate=115200, tx=0, rx=1, timeout=1000)

def send_at_command(command):
    # Send AT command
    uart.write(command + b'\r\n')
      # Wait for the module to respond
    time.sleep(0.5)
    # Read and print the response
    response = uart.read()
    print(response.decode('utf-8'))

# Example: Send AT command to check module status

#HTTPGET
send_at_command(b'AT+HTTPINIT')
send_at_command(b'AT+HTTPPARA="URL","https://test-f1e70.firebaseio.com/pothole.json"')
send_at_command(b'AT+HTTPACTION=0')
send_at_command(b'AT+HTTPREAD=0,290')
send_at_command(b'AT+HTTPTERM')

#HTTPPOST
send_at_command(b'AT+CSQ')
send_at_command(b'AT+HTTPINIT')
send_at_command(b'AT+HTTPPARA="URL","https://test-f1e70.firebaseio.com/pothole.json"')
send_at_command(b'AT+HTTPPARA="CONTENT","application/json"')
send_at_command(b'AT+HTTPDATA=50,5000')
send_at_command(b'{"uid": "test","gps": "test2","speed": "100"}')
send_at_command(b'\n')
send_at_command(b'AT+HTTPACTION=1')
send_at_command(b'AT+HTTPREAD=0,250')
send_at_command(b'AT+HTTPTERM')

# Add more AT commands as needed
# For example: send_at_command(b'AT+CGMM')  # Get module model
