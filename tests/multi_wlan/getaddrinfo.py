try:
    import network

    network.WLAN
except (ImportError, NameError):
    print("SKIP")
    raise SystemExit

import socket

# This is a regression test to ensure getaddrinfo() fails (after a timeout)
# when Wi-Fi is disconnected.
#
# It doesn't require multiple instances, but it does require Wi-Fi to be present
# but not active, and for no other network interface to be configured. The only
# tests which already meet these conditions is here in multi_wlan tests.


def instance0():
    wlan = network.WLAN()
    wlan.active(0)

    try:
        socket.getaddrinfo("micropython.org", 80)
    except OSError as er:
        print(
            "active(0) failed"
        )  # This may fail with code -6 or -2 depending on whether WLAN has been active since reset

    wlan.active(1)

    try:
        socket.getaddrinfo("micropython.org", 80)
    except OSError as er:
        print(
            "active(1) failed", er.errno in (-2, -202)
        )  # This one should always be -2 or -202 depending on port

    wlan.active(0)
