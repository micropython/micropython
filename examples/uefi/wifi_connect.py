# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# wifi_connect.py — scan, join a WPA2 network, get a DHCP lease over EFI WiFi2.
#   SSID, KEY = "my-ap", "my-passphrase"
#   exec(open('/samples/wifi_connect.py').read())
# Needs real WiFi hardware with UEFI WiFi2 drivers — QEMU has no 802.11, so this
# only runs on metal (e.g. over the serial REPL, `make run`).

import network
import time

# Set these before running (or edit them here).
try:
    SSID
except NameError:
    SSID, KEY = "your-ssid", "your-key"

wlan = network.WLAN(network.WLAN.IF_STA)
wlan.active(True)

# scan() -> (ssid, bssid, channel, RSSI, security, hidden). NOTE: EFI WiFi2 exposes
# no BSSID/channel (bssid is b'', channel 0) and RSSI is a 0..100 link quality.
print("Scanning...")
for ssid, bssid, chan, rssi, sec, hidden in wlan.scan():
    print("  %-32s qual=%-3d sec=%d" % (ssid.decode() or "<hidden>", rssi, sec))

# connect() is non-blocking: it kicks off the scan + association, then we poll
# status(). Start DHCP too (also non-blocking) so a lease is requested as soon as
# the link comes up; status() reaches GOT_IP once an address is bound.
print("Connecting to", SSID, "...")
wlan.connect(SSID, KEY)
wlan.ipconfig(dhcp4=True)

STAT = {
    network.STAT_IDLE: "idle",
    network.STAT_CONNECTING: "connecting",
    network.STAT_GOT_IP: "got-ip",
    network.STAT_NO_AP_FOUND: "no-ap-found",
    network.STAT_WRONG_PASSWORD: "wrong-password",
    network.STAT_CONNECT_FAIL: "connect-fail",
}

t0 = time.ticks_ms()
while time.ticks_diff(time.ticks_ms(), t0) < 20000:
    st = wlan.status()
    if st == network.STAT_GOT_IP or st in (
        network.STAT_NO_AP_FOUND,
        network.STAT_WRONG_PASSWORD,
        network.STAT_CONNECT_FAIL,
    ):
        break
    time.sleep_ms(250)

print("Status  :", STAT.get(wlan.status(), wlan.status()))
if wlan.isconnected():
    ip, mask = wlan.ipconfig("addr4")
    print("IPv4    :", ip, "/", mask)
    print("Gateway :", wlan.ipconfig("gw4"))
    # From here sockets/ssl work exactly as on the wired LAN, e.g.:
    #   import socket; socket.getaddrinfo('micropython.org', 443)
else:
    print("Not connected — check the SSID/key and that the AP is in range.")
    # wlan.disconnect()  # tear down and release the supplicant credentials
