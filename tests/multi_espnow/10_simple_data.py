# Simple test of a ESPnow server and client transferring data.
# This test works with ESP32 or ESP8266 as server or client.

try:
    import network
    import espnow
except ImportError:
    print("SKIP")
    raise SystemExit

# Set read timeout to 5 seconds
timeout_ms = 5000
default_pmk = b"MicroPyth0nRules"


def init(sta_active=True, ap_active=False):
    wlans = [network.WLAN(i) for i in [network.STA_IF, network.AP_IF]]
    e = espnow.ESPNow()
    e.active(True)
    e.set_pmk(default_pmk)
    wlans[0].active(sta_active)
    wlans[1].active(ap_active)
    wlans[0].disconnect()  # Force esp8266 STA interface to disconnect from AP
    e.set_pmk(default_pmk)
    return e


# Server
def instance0():
    e = init(True, False)
    multitest.globals(PEERS=[network.WLAN(i).config("mac") for i in (0, 1)])
    multitest.next()
    peer, msg1 = e.recv(timeout_ms)
    if msg1 is None:
        print("e.recv({timeout_ms}): Timeout waiting for message.")
        e.active(False)
        return
    print(bytes(msg1))
    msg2 = b"server to client"
    e.add_peer(peer)
    e.send(peer, msg2)
    print(bytes(msg2))
    e.active(False)


# Client
def instance1():
    e = init(True, False)
    multitest.next()
    peer = PEERS[0]
    e.add_peer(peer)
    msg1 = b"client to server"
    e.send(peer, msg1)
    print(bytes(msg1))
    peer2, msg2 = e.recv(timeout_ms)
    print(bytes(msg2))
    e.active(False)
