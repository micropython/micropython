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

# net_ping.py — ping a host a few times over ICMP.
#   exec(open('/samples/net_ping.py').read())
# Needs networking — launch with `make run`.

import network
import time

HOST = "8.8.8.8"

nic = network.LAN()
nic.active(True)
nic.ipconfig(dhcp4=True)
t0 = time.ticks_ms()
while not nic.isconnected() and time.ticks_diff(time.ticks_ms(), t0) < 15000:
    time.sleep_ms(200)

if not nic.isconnected():
    print("No network — see net_info.py")
else:
    print("PING", HOST)
    ok = 0
    for i in range(4):
        rtt = network.ping(HOST, 2000)
        if rtt is None:
            print("  seq=%d  timeout" % i)
        else:
            ok += 1
            print("  seq=%d  rtt=%d ms" % (i, rtt))
        time.sleep_ms(500)
    print("%d/4 replies" % ok)
