# Test TCP recv with MSG_PEEK
#
# Note that bare metal LWIP only returns at most one packet's worth of TCP data
# in any recv() call - including when peeking - so can't be too clever with
# different recv() combinations
import socket

PORT = 8000


# Server
def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    s = socket.socket()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(socket.getaddrinfo("0.0.0.0", PORT)[0][-1])
    s.listen()
    multitest.next()
    s2, _ = s.accept()
    print(s2.recv(8, socket.MSG_PEEK))
    print(s2.recv(8))
    s2.send(b"1234567890")
    multitest.next()
    print(s2.recv(5, socket.MSG_PEEK))
    print(s2.recv(5, socket.MSG_PEEK))
    multitest.next()
    s2.close()
    s.close()


# Client
def instance1():
    multitest.next()
    s = socket.socket()
    s.connect(socket.getaddrinfo(IP, PORT)[0][-1])
    s.send(b"abcdefgh")
    multitest.next()
    s.send(b"klmnopqr")
    print(s.recv(5, socket.MSG_PEEK))
    print(s.recv(10))
    multitest.next()
    s.close()
