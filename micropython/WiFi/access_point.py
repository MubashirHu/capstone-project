import socket
import network

# Configure SoftAP settings
ssid = "PicoAP"
password = "mypassword"

# Initialize wifi interface
ap = network.WLAN(network.AP_IF)

# Configure and activate SoftAP mode
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
server_socket.bind(("0.0.0.0", 80))

# Listen for incoming connections
server_socket.listen(5)

print("Waiting for incoming connections...")

# Main loop to handle incoming client connections
while True:
    
    
    
    # Accept incoming client connections
    client_socket, client_address = server_socket.accept()
    print("Client connected:", client_address)
    
    # Handle client communication
    while True:
        data = client_socket.recv(1024)
        if not data:
            break
        print("Received from client:", data)
        
        # Echo back the received data
        client_socket.sendall(data)
    
    # Close the client connection
    client_socket.close()
    print("Client disconnected:", client_address)
