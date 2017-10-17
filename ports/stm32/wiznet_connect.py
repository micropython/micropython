import time, network
nic = network.WIZNET5K(pyb.SPI(1), pyb.Pin.board.X5, pyb.Pin.board.X4)
nic.active(1)
while not nic.isconnected():
    time.sleep_ms(50) # needed to poll the NIC
print(nic.ifconfig())
import socket
print(socket.getaddrinfo('micropython.org', 80))
