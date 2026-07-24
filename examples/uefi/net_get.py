# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# net_get.py — fetch a URL over HTTP (resolve, connect, GET, read the reply).
#   exec(open('/samples/net_get.py').read())
# Needs networking — launch with `make run`.

import network
import socket
import time

HOST = "example.com"
PATH = "/"

nic = network.LAN()
nic.active(True)
nic.ipconfig(dhcp4=True)
t0 = time.ticks_ms()
while not nic.isconnected() and time.ticks_diff(time.ticks_ms(), t0) < 15000:
    time.sleep_ms(200)
if not nic.isconnected():
    raise OSError("no network — see net_info.py")

# Inside the dev container SLIRP's DNS forwarder (10.0.2.3) can't reach an upstream
# resolver, so point at a public one. On real hardware the DHCP DNS works too.
network.ipconfig(dns="8.8.8.8")

addr = socket.getaddrinfo(HOST, 80)[0][-1]
print("GET http://%s%s  ->  %s" % (HOST, PATH, addr))

s = socket.socket()
s.settimeout(10)
s.connect(addr)
req = (
    b"GET "
    + PATH.encode()
    + b" HTTP/1.1\r\nHost: "
    + HOST.encode()
    + b"\r\nConnection: close\r\n\r\n"
)
s.send(req)

resp = b""
while True:
    chunk = s.recv(512)
    if not chunk:
        break
    resp += chunk
s.close()

header = resp.split(b"\r\n\r\n", 1)[0]
print(header.decode("utf-8", "replace"))
print("... %d bytes total" % len(resp))
