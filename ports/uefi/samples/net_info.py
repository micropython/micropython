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

# net_info.py — bring up the wired interface (DHCP) and show its configuration.
#   exec(open('/samples/net_info.py').read())
# Needs networking — launch with `make run` (or `make run-gfx`).

import network
import time

nic = network.LAN()
nic.active(True)  # connect the network stack + start the NIC

if not nic.isconnected():
    print("Requesting a DHCP lease...")
    nic.ipconfig(dhcp4=True)  # non-blocking; poll until the lease lands
    t0 = time.ticks_ms()
    while not nic.isconnected() and time.ticks_diff(time.ticks_ms(), t0) < 15000:
        time.sleep_ms(200)

mac = nic.config("mac")
print("MAC     :", ":".join("%02x" % b for b in mac))

if nic.isconnected():
    ip, mask = nic.ipconfig("addr4")
    print("IPv4    :", ip, "/", mask)
    print("Gateway :", nic.ipconfig("gw4"))
    print("Link    : up")
else:
    print("No lease — is the firmware networked? (use `make run`, and build")
    print("          firmware/net with `bash docker/build-ovmf-net.sh`)")
