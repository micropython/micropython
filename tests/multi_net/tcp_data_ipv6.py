# Simple test of IPv6 TCP server and client transferring data

import socket

PORT = 8000


def check_ipv6_support():
    try:
        socket.AF_INET6
        # Try to create an IPv6 socket to verify support
        s = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
        s.close()
        return True
    except (AttributeError, OSError):
        return False


if not check_ipv6_support():
    print("SKIP")
    raise SystemExit


# Server
def instance0():
    multitest.globals(IP=multitest.get_network_ip_ipv6())
    s = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(socket.getaddrinfo("::", PORT, socket.AF_INET6)[0][-1])
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
    s = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
    s.connect(socket.getaddrinfo(IP, PORT, socket.AF_INET6)[0][-1])
    s.send(b"client to server")
    print(s.recv(16))
    s.close()
