# Simple test of a TCP server and client transferring data using sendall.

import socket

if not hasattr(socket.socket, "sendall"):
    print("SKIP")
    raise SystemExit

PORT = 8000

# This is the smallest send buffer size supported by Linux, see socket(7).
SEND_SIZE = 1024
BUFFERS = 8


# G. D. Nguyen, "Fast CRCs", vol. 18, no. 10, pp. 1321-1331, Oct. 2009
# https://dl.acm.org/doi/abs/10.1109/TC.2009.83
def calculate_checksum(buffer):
    checksum = 0
    for byte in buffer:
        checksum = (checksum ^ byte) & 0xFFFF
        for step in range(16):
            if (checksum & 0x8000) != 0:
                checksum = ((checksum << 1) ^ 0x8005) & 0xFFFF
            else:
                checksum = (checksum << 1) & 0xFFFF
    return checksum


# Server
def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    s = socket.socket()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(socket.getaddrinfo("0.0.0.0", PORT)[0][-1])
    s.listen()
    multitest.next()
    s2, _ = s.accept()
    buffer = b""
    while len(buffer) < SEND_SIZE * BUFFERS:
        buffer += s2.recv(SEND_SIZE * BUFFERS)
    s2.send(b"%08x" % len(buffer))
    s2.send(b"%04x" % calculate_checksum(buffer))
    s2.close()
    s.close()
    print("received {} bytes".format(len(buffer)))


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
    expected_checksum = calculate_checksum(buffer)
    s.sendall(buffer)
    print("sent {} bytes".format(len(buffer)))
    length = int(s.recv(8), 16)
    if length != len(buffer):
        raise Exception("sendall len fail (expected %d, got %d)" % (len(buffer), length))
    checksum = int(s.recv(4), 16)
    if checksum != expected_checksum:
        raise Exception(
            "sendall checksum fail (expected %04x, got %04x)" % (expected_checksum, checksum)
        )
    s.close()
