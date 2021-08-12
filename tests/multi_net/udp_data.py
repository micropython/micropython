# Simple test of a UDP server and client transferring data

import socket

NUM_NEW_SOCKETS = 4
NUM_TRANSFERS = 4
PORT = 8000

# Server
def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    multitest.next()
    for i in range(NUM_NEW_SOCKETS):
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.bind(socket.getaddrinfo("0.0.0.0", PORT)[0][-1])
        multitest.broadcast("server ready")
        for j in range(NUM_TRANSFERS):
            data, addr = s.recvfrom(1000)
            print(data)
            s.sendto(b"server to client %d %d" % (i, j), addr)
        s.close()


# Client
def instance1():
    multitest.next()
    ai = socket.getaddrinfo(IP, PORT)[0][-1]
    for i in range(NUM_NEW_SOCKETS):
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        multitest.wait("server ready")
        for j in range(NUM_TRANSFERS):
            s.sendto(b"client to server %d %d" % (i, j), ai)
            data, addr = s.recvfrom(1000)
            print(data)
        s.close()
