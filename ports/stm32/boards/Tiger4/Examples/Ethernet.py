import network
import time
import socket
from machine import Pin

# Initialize Ethernet interface
nic = network.LAN(phy_addr=0x01, phy_type=network.LAN.PHY_DP83848)
# Activate Ethernet
nic.active(True)

# Network configuration (optional, if using DHCP, this can be omitted)
# Example of a static IP configuration
ip = '192.168.1.245'
subnet_mask = '255.255.255.0'
gateway = '192.168.1.253'
dns = '8.8.8.8'

# Apply network configuration
nic.ifconfig((ip, subnet_mask, gateway, dns))

# Wait until the connection is established
while not nic.isconnected():
    print("Waiting for network connection...")
    time.sleep(1)

# Once the connection is successfully established
print("Network connected!")
print("IP address:", nic.ifconfig()[0])

# Create a web server
addr = socket.getaddrinfo('0.0.0.0', 80)[0][-1]  # Web server on port 80
s = socket.socket()  # Create a socket
s.bind(addr)  # Bind the socket to the address
s.listen(1)  # Listen for incoming connections

print("Web server running at http://{}:80/".format(nic.ifconfig()[0]))

while True:
    cl, addr = s.accept()  # Accept a connection
    print('Connection from', addr)
    
    # Process HTTP request
    request = cl.recv(1024)  # Receive request from client
    print("Request:", request)
    
    # Simple HTTP response
    response = """\
HTTP/1.1 200 OK

<!DOCTYPE html>
<html>
<head>
    <title>MicroPython Webserver</title>
</head>
<body>
    <h1>Hello, MicroPython!</h1>
    <p>Welcome to my web server.</p>
</body>
</html>
"""
    cl.send(response)  # Send response
    cl.close()  # Close the connection