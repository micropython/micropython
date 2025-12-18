# Test TCP recv with MSG_PEEK
#
# Note that bare metal LWIP only returns at most one packet's worth of TCP data
# in any recv() call - including when peeking - so can't be too clever with
# different recv() combinations
import socket
import random


# Server
def instance0():
    PORT = random.randrange(10000, 50000)
    multitest.globals(IP=multitest.get_network_ip(), PORT=PORT)
    s = socket.socket()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(socket.getaddrinfo("0.0.0.0", PORT)[0][-1])
    s.listen()
    multitest.next()
    s2, _ = s.accept()
    print(s2.recv(8, socket.MSG_PEEK))
    print(s2.recv(8))
    s2.send(b"1234567890")
    multitest.broadcast("0-sent")
    multitest.wait("1-sent")
    print(s2.recv(5, socket.MSG_PEEK))
    print(s2.recv(5, socket.MSG_PEEK))
    multitest.broadcast("0-recved")
    multitest.wait("1-recved")  # sync here necessary as MP sends RST if closing TCP early
    s2.close()
    s.close()


# Client
def instance1():
    multitest.next()
    s = socket.socket()
    s.connect(socket.getaddrinfo(IP, PORT)[0][-1])
    s.send(b"abcdefgh")
    multitest.broadcast("1-sent")
    multitest.wait("0-sent")
    s.send(b"klmnopqr")
    print(s.recv(5, socket.MSG_PEEK))
    print(s.recv(10))
    multitest.broadcast("1-recved")
    multitest.wait("0-recved")
    s.close()
