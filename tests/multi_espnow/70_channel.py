# Test that ESP-NOW picks up the channel configuration for STA
# mode on ESP32.
#
# Note that setting the channel on a peer in ESP-NOW on modern ESP-IDF only
# checks it against the configured channel, it doesn't ever change the radio
# channel
import sys
import time

try:
    import network
    import espnow
except ImportError:
    print("SKIP")
    raise SystemExit

# ESP8266 doesn't support config('channel') on the STA interface,
# and the channel parameter to add_peer doesn't appear to set the
# channel either.
if sys.platform == "esp8266":
    print("SKIP")
    raise SystemExit


timeout_ms = 1000
default_pmk = b"MicroPyth0nRules"

CHANNEL = 3
WRONG_CHANNEL = 8


def init_sta():
    sta = network.WLAN(network.WLAN.IF_STA)
    e = espnow.ESPNow()
    e.active(True)
    sta.active(True)
    sta.disconnect()  # Force AP disconnect for any saved config, important so the channel doesn't change
    sta.config(channel=CHANNEL)
    e.set_pmk(default_pmk)
    return sta, e


# Receiver
def instance0():
    sta, e = init_sta()
    multitest.globals(PEER=sta.config("mac"))
    multitest.next()
    print(sta.config("channel"))
    while True:
        peer, msg = e.recv(timeout_ms)
        if peer is None:
            print("Timeout")
            break
        print(msg)
    e.active(False)


# Sender
def instance1():
    sta, e = init_sta()
    multitest.next()
    peer = PEER

    # both instances set channel via sta.config(), above
    msg = b"sent to right channel 1"
    e.add_peer(peer, channel=CHANNEL)
    for _ in range(3):
        e.send(peer, msg)
    e.del_peer(peer)
    print(sta.config("channel"))

    sta.config(channel=WRONG_CHANNEL)
    msg = b"sent to wrong channel"
    e.add_peer(peer, channel=WRONG_CHANNEL)
    for _ in range(3):
        e.send(peer, msg)
    e.del_peer(peer)
    print(sta.config("channel"))

    # switching back to the correct channel should also work
    sta.config(channel=CHANNEL)
    msg = b"sent to right channel 2"
    e.add_peer(peer, channel=CHANNEL)
    for _ in range(3):
        e.send(peer, msg)
    e.del_peer(peer)
    print(sta.config("channel"))

    e.active(False)
