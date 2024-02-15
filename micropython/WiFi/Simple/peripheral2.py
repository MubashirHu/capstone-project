import socket

# Access Point (AP) settings
ap_ip = "192.168.4.1"  # IP address of the Access Point
ap_port = 1234        # Port number to communicate with the Access Point

# Data to send
data_to_send = b"Hello, Access Point! this is from peripheral 2"

# Create a socketQ
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect to the Access Point
client_socket.connect((ap_ip, ap_port))

# Send data to the Access Point
client_socket.sendall(data_to_send)

# Close the socket
client_socket.close()

