import errno
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
    s2.settimeout(0.2)
    try:
        s2.recv(1)
    except OSError as er:
        print(er.errno in (errno.ETIMEDOUT, errno.EAGAIN) or str(er) == "timed out")
    multitest.next()
    s2.close()
    s.close()


# Client
def instance1():
    multitest.next()
    s = socket.socket()
    s.connect(socket.getaddrinfo(IP, PORT)[0][-1])
    s.settimeout(0.2)
    try:
        s.recv(1)
    except OSError as er:
        print(er.errno in (errno.ETIMEDOUT, errno.EAGAIN) or str(er) == "timed out")
    multitest.next()
    s.close()
