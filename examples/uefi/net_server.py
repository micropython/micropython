# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# net_server.py — a tiny HTTP server on port 8080.
#   exec(open('/samples/net_server.py').read())
# The run/run-gfx targets map host tcp/8080 -> guest tcp/8080 (QEMU hostfwd), so
# once it's serving you can open http://localhost:8080/ on your host. Ctrl-C stops.

import network
import socket
import time

nic = network.LAN()
nic.active(True)
nic.ipconfig(dhcp4=True)
t0 = time.ticks_ms()
while not nic.isconnected() and time.ticks_diff(time.ticks_ms(), t0) < 15000:
    time.sleep_ms(200)
if not nic.isconnected():
    raise OSError("no network — see net_info.py")

srv = socket.socket()
srv.bind(("0.0.0.0", 8080))
srv.listen(2)
print("Serving on http://localhost:8080/  (Ctrl-C to stop)")

BODY = b"<html><body><h1>Hello from MicroPython on UEFI!</h1></body></html>"
RESP = (
    b"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n"
    b"Content-Length: " + str(len(BODY)).encode() + b"\r\n\r\n" + BODY
)

try:
    while True:
        conn, peer = srv.accept()
        req = conn.recv(512)
        line = req.split(b"\r\n", 1)[0] if req else b""
        print("%s -> %s" % (peer[0], line.decode("utf-8", "replace")))
        conn.send(RESP)
        conn.close()
except KeyboardInterrupt:
    print("\nstopped")
finally:
    srv.close()
