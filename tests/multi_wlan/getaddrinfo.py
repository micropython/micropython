# This is a regression test to ensure getaddrinfo() fails (after a timeout)
# when Wi-Fi is disconnected.
#
# It doesn't require multiple instances, but it does require Wi-Fi to be present
# but not active, and for no other network interface to be configured. The only
# tests which already meet these conditions is here in multi_wlan tests.
try:
    from network import WLAN
except (ImportError, NameError):
    print("SKIP")
    raise SystemExit

import socket


def instance0():
    WLAN(WLAN.IF_AP).active(0)
    wlan = WLAN(WLAN.IF_STA)
    wlan.active(0)

    multitest.next()

    # Note: Lookup domain for this test doesn't need to exist, as the board
    # isn't internet connected. It also shouldn't exist, so a cached result is
    # never returned!
    try:
        socket.getaddrinfo("doesnotexist.example.com", 80)
    except OSError as er:
        print(
            "active(0) failed"
        )  # This may fail with code -6 or -2 depending on whether WLAN has been active since reset

    wlan.active(1)

    try:
        socket.getaddrinfo("doesnotexist.example.com", 80)
    except OSError as er:
        print(
            "active(1) failed", er.errno in (-2, -202)
        )  # This one should always be -2 or -202 depending on port

    wlan.active(0)
