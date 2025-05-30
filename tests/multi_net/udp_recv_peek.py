# Test UDP recv and recvfrom with MSG_PEEK
import random
import socket
import time


# Server
def instance0():
    PORT = random.randrange(10000, 50000)
    multitest.globals(IP=multitest.get_network_ip(), PORT=PORT)
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(socket.getaddrinfo("0.0.0.0", PORT)[0][-1])
    multitest.next()
    peek_bytes, peek_addr = s.recvfrom(8, socket.MSG_PEEK)
    print(peek_bytes)
    real_bytes, real_addr = s.recvfrom(8)
    print(real_bytes)
    print(peek_addr == real_addr)  # source addr should be the same for each
    res = s.sendto(b"1234567890", peek_addr)
    print(res)
    print(s.recv(5, socket.MSG_PEEK))
    print(s.recv(5, socket.MSG_PEEK))
    s.close()


# Client
def instance1():
    multitest.next()
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(socket.getaddrinfo(IP, PORT)[0][-1])
    s.send(b"abcdefgh")
    s.send(b"klmnopqr")
    print(s.recv(5, socket.MSG_PEEK))
    print(s.recv(10))
    s.close()
