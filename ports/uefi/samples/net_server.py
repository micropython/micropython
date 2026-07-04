# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2026 Nicko van Someren
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

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
