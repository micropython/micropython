# Test WPA3 and WPA2/WPA3 transition mode Wi-Fi connectivity.
#
# Tests that a Station can connect to an Access Point configured in
# WPA2/WPA3 transition mode (SEC_WPA2_WPA3). This verifies that the
# driver correctly negotiates WPA2 or WPA3 as needed.
#
# Related issue: https://github.com/micropython/micropython/issues/18597

try:
    import network

    network.WLAN
    # SEC_WPA2_WPA3 may not be available on all ports.
    network.WLAN.SEC_WPA2_WPA3
except (ImportError, NameError, AttributeError):
    print("SKIP")
    raise SystemExit

import os
import time

CHANNEL = 8
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
    ssid = "MP-wpa3-" + os.urandom(6).hex()
    psk = "Secret-" + os.urandom(6).hex()

    # stop any previous activity
    network.WLAN(network.WLAN.IF_STA).active(False)
    ap.active(False)

    # --- Test 1: AP in WPA2/WPA3 transition mode, STA connects ---
    ap.active(True)
    ap.config(ssid=ssid, key=psk, channel=CHANNEL, security=network.WLAN.SEC_WPA2_WPA3)
    print("AP started SEC_WPA2_WPA3")

    multitest.globals(SSID=ssid, PSK=psk)
    multitest.next()

    if not wait_for(ap.isconnected):
        raise RuntimeError("Timed out waiting for station, status ", ap.status())

    print("AP got station")
    time.sleep(3)

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

    # --- Test 1: Connect to WPA2/WPA3 AP ---
    sta.active(True)
    sta.connect(ssid, psk)

    print("STA connecting to WPA2/WPA3 AP...")

    if not wait_for(sta.isconnected):
        raise RuntimeError("Timed out waiting to connect, status ", sta.status())

    print("STA connected")
    print("channel", sta.config("channel"))

    print("STA waiting for disconnect...")

    if not wait_for(lambda: not sta.isconnected()):
        raise RuntimeError("Timed out waiting for AP to disconnect us, status ", sta.status())

    print("STA disconnected")

    sta.active(False)
