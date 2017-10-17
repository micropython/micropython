import network
nic = network.WIZNET5K(pyb.SPI(1), pyb.Pin.board.X5, pyb.Pin.board.X4)
while not nic.isconnected():
    pass
nic.active(1)
nic.ifconfig('dhcp')
print(nic.ifconfig())
import socket
print(socket.getaddrinfo('micropython.org', 80))
