# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

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
    print("          firmware/net with `bash tools/build-ovmf.sh --net`)")
