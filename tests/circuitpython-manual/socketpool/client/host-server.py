#!/usr/bin/env python3
import socket

TIMEOUT = 10
HOST = "192.168.10.179"
PORT = 5000

print("Create Socket")
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.settimeout(TIMEOUT)
    s.bind((HOST, PORT))
    s.listen()
    print("Accepting connections")
    while True:
        try:
            conn, addr = s.accept()
            break
        except BlockingIOError:
            pass

    with conn:
        s.settimeout(TIMEOUT)
        print("Connected by", addr)
        data = conn.recv(128)
        print("got: " + str(data))
        conn.sendall(data)
        print("sent: " + str(data))
