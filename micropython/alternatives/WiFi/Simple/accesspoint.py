import socket
import network

# Access Point (AP) settings
ssid = "MicroPython-AP"
password = "123456789"
ap_port = 12345

# Initialize Wi-Fi interface and configure SoftAP mode
ap = network.WLAN(network.AP_IF)
ap.config(essid=ssid, password=password)
ap.active(True)

# Print SoftAP configuration
print("Access point active")
print("SSID:", ssid)
print("Password:", password)
print("IP Address:", ap.ifconfig()[0])

# Create a TCP/IP socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to the IP address and port
server_socket.bind((ap.ifconfig()[0], ap_port))

# Listen for incoming connections
server_socket.listen(1)

print("Waiting for incoming connections...")

# Accept incoming client connections
client_socket, client_address = server_socket.accept()
print("Client connected:", client_address)

# Receive data from the client
received_data = client_socket.recv(1024)
print("Received data from client:", received_data)

# Close the client connection
client_socket.close()
print("Client disconnected:", client_address)
