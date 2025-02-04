# Basic Wi-Fi MAC layer test where one device creates an Access Point and the
# other device connects to it as a Station. Also tests channel assignment (where
# possible) and disconnection.

try:
    import network

    network.WLAN
except (ImportError, NameError):
    print("SKIP")
    raise SystemExit

import os
import sys
import time

CHANNEL = 8

# Note that on slower Wi-Fi stacks this bumps up against the run-multitests.py
# timeout which expects <10s between lines of output. We work around this by
# logging something half way through the wait_for loop...
CONNECT_TIMEOUT = 15000


def wait_for(test_func):
    has_printed = False
    start = time.ticks_ms()
    while not test_func():
        time.sleep(0.1)
        delta = time.ticks_diff(time.ticks_ms(), start)
        if not has_printed and delta > CONNECT_TIMEOUT / 2:
            print("...")
            has_printed = True
        elif delta > CONNECT_TIMEOUT:
            break

    if not has_printed:
        print("...")  # keep the output consistent

    return test_func()


# AP
def instance0():
    ap = network.WLAN(network.WLAN.IF_AP)
    ssid = "MP-test-" + os.urandom(6).hex()
    psk = "Secret-" + os.urandom(6).hex()

    # stop any previous activity
    network.WLAN(network.WLAN.IF_STA).active(False)
    ap.active(False)

    ap.active(True)
    ap.config(ssid=ssid, key=psk, channel=CHANNEL, security=network.WLAN.SEC_WPA_WPA2)

    # print("AP setup", ssid, psk)
    print("AP started")

    multitest.globals(SSID=ssid, PSK=psk)
    multitest.next()

    # Wait for station
    if not wait_for(ap.isconnected):
        raise RuntimeError("Timed out waiting for station, status ", ap.status())

    print("AP got station")
    time.sleep(
        3
    )  # depending on port, may still need to negotiate DHCP lease for STA to see connection

    print("AP disabling...")
    ap.active(False)


# STA
def instance1():
    sta = network.WLAN(network.WLAN.IF_STA)

    # stop any previous activity
    network.WLAN(network.WLAN.IF_AP).active(False)
    sta.active(False)

    multitest.next()
    ssid = SSID
    psk = PSK

    # print("STA setup", ssid, psk)

    sta.active(True)
    sta.connect(ssid, psk)

    print("STA connecting...")

    if not wait_for(sta.isconnected):
        raise RuntimeError("Timed out waiting to connect, status ", sta.status())

    print("STA connected")

    print("channel", sta.config("channel"))

    print("STA waiting for disconnect...")

    # Expect the AP to disconnect us immediately
    if not wait_for(lambda: not sta.isconnected()):
        raise RuntimeError("Timed out waiting for AP to disconnect us, status ", sta.status())

    print("STA disconnected")

    sta.active(False)
