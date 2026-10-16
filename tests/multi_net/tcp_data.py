# Simple test of a TCP server and client transferring data

import socket

PORT = 8000


# Server
def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    s = socket.socket(multitest.AF_FAMILY, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(socket.getaddrinfo(multitest.BIND_ADDR, PORT, multitest.AF_FAMILY)[0][-1])
    s.listen()
    multitest.next()
    s2, _ = s.accept()
    print(s2.recv(16))
    s2.send(b"server to client")
    s2.close()
    s.close()


# Client
def instance1():
    multitest.next()
    s = socket.socket(multitest.AF_FAMILY, socket.SOCK_STREAM)
    s.connect(socket.getaddrinfo(IP, PORT, multitest.AF_FAMILY)[0][-1])
    s.send(b"client to server")
    print(s.recv(16))
    s.close()
