# Simple test of a TCP server and client transferring data using sendall.

import socket

try:
    s = socket.socket()
    if not hasattr(s, "sendall"):
        print("SKIP")
        raise SystemExit
finally:
    s.close()

PORT = 8000

# This is the smallest send buffer size supported by Linux, see socket(7).
SEND_SIZE = 1024
BUFFERS = 8


def rjust_hex(v):
    return (("00000000" + hex(v)[2:])[-8:]).encode("us-ascii")


def calculate_checksum(size):
    checksum = 0
    for i in range(size):
        checksum += i & 0xFF
    return rjust_hex(checksum)


CHECKSUM = calculate_checksum(SEND_SIZE * BUFFERS)


# Server
def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    s = socket.socket()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(socket.getaddrinfo("0.0.0.0", PORT)[0][-1])
    s.listen()
    multitest.next()
    s2, _ = s.accept()
    buffer = s2.recv(SEND_SIZE * BUFFERS)
    s2.send(rjust_hex(len(buffer)))
    checksum = 0
    for byte in buffer:
        checksum += byte
    s2.send(rjust_hex(checksum))
    s2.close()
    s.close()


# Client
def instance1():
    multitest.next()
    s = socket.socket()
    s.connect(socket.getaddrinfo(IP, PORT)[0][-1])
    # Try our best to trigger a partial send in socket.sendall
    if hasattr(socket, "SO_SNDBUF"):
        s.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, SEND_SIZE)
    buffer = bytearray(SEND_SIZE * BUFFERS)
    for index in range(len(buffer)):
        buffer[index] = index & 0xFF
    s.sendall(buffer)
    length = s.recv(8)
    if int(length, 16) != len(buffer):
        print("sendall len fail", length, len(buffer))
    else:
        print("sendall len pass")
    checksum = s.recv(8)
    if checksum != CHECKSUM:
        print("sendall checksum fail", checksum, CHECKSUM)
    else:
        print("sendall checksum pass")
    s.close()
