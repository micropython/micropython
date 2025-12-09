# Test configuring transmit rates for ESP-NOW on ESP32

import sys
import time

try:
    import network
    import espnow
except ImportError:
    print("SKIP")
    raise SystemExit

# ESP8266 doesn't support multiple ESP-NOW data rates
if sys.platform == "esp8266":
    print("SKIP")
    raise SystemExit

# Currently the config(rate=...) implementation is not compatible with ESP32-C6
# (this test passes when C6 is receiver, but not if C6 is sender.)
if "ESP32C6" in sys.implementation._machine:
    print("SKIP")
    raise SystemExit

# ESP32-C2 doesn't support Long Range mode. This test is currently written assuming
# LR mode can be enabled.
if "ESP32C2" in sys.implementation._machine:
    print("SKIP")
    raise SystemExit


timeout_ms = 1000
default_pmk = b"MicroPyth0nRules"
CHANNEL = 9


def init_sta():
    sta = network.WLAN(network.WLAN.IF_STA)
    e = espnow.ESPNow()
    e.active(True)
    sta.active(True)
    sta.disconnect()  # Force AP disconnect for any saved config, important so the channel doesn't change
    sta.config(channel=CHANNEL)
    e.set_pmk(default_pmk)
    # Enable both default 802.11 modes and Long Range modes
    sta.config(protocol=network.WLAN.PROTOCOL_LR | network.WLAN.PROTOCOL_DEFAULT)
    return sta, e


# Receiver
def instance0():
    sta, e = init_sta()
    multitest.globals(PEER=sta.config("mac"))
    multitest.next()
    while True:
        peer, msg = e.recv(timeout_ms)
        if peer is None:
            print("Timeout")
            break
        # Note that we don't have any way in Python to tell what data rate this message
        # was received with, so we're assuming the rate was correct.
        print(msg)
    e.active(False)


# Sender
def instance1():
    sta, e = init_sta()
    multitest.next()
    peer = PEER

    e.add_peer(peer)
    # Test normal, non-LR rates
    for msg, rate in (
        (b"default rate", None),
        (b"5Mbit", espnow.RATE_5M),
        (b"11Mbit", espnow.RATE_11M),
        (b"24Mbit", espnow.RATE_24M),
        (b"54Mbit", espnow.RATE_54M),
        (b"250K LR", espnow.RATE_LORA_250K),
        (b"500K LR", espnow.RATE_LORA_500K),
        # switch back to non-LR rates to check it's all OK
        (b"1Mbit again", espnow.RATE_1M),
        (b"11Mbit again", espnow.RATE_11M),
    ):
        if rate is not None:
            e.config(rate=rate)
        for _ in range(3):
            e.send(peer, msg)
        time.sleep_ms(50)  # give messages some time to be received before continuing
    e.del_peer(peer)

    e.active(False)
