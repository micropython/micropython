# Test of a ESPnow echo server and client transferring data.
# This test works with ESP32 or ESP8266 as server or client.

try:
    import network
    import random
    import espnow
except ImportError:
    print("SKIP")
    raise SystemExit

# Set read timeout to 5 seconds
timeout_ms = 5000
default_pmk = b"MicroPyth0nRules"
sync = True


def echo_server(e):
    peers = []
    i = 0
    while True:
        peer, msg = e.irecv(timeout_ms)
        i += 1
        if i % 10 == 0:
            print("OK:", i)
        if peer is None:
            return
        if peer not in peers:
            peers.append(peer)
            e.add_peer(peer)

        #  Echo the MAC and message back to the sender
        if not e.send(peer, msg, sync):
            print("ERROR: send() failed to", peer)
            return

        if msg == b"!done":
            return


def echo_test(e, peer, msg, sync):
    try:
        if not e.send(peer, msg, sync):
            print("ERROR: Send failed.")
            return
    except OSError as exc:
        # Don't print exc as it is differs for esp32 and esp8266
        print("ERROR: OSError:")
        return

    p2, msg2 = e.irecv(timeout_ms)
    if msg2 != msg:
        print("ERROR: Received != Sent")


def echo_client(e, peer, msglens):
    for sync in [True]:
        for msglen in msglens:
            msg = bytearray(msglen)
            if msglen > 0:
                msg[0] = b"_"[0]  # Random message must not start with '!'
            for i in range(1, msglen):
                msg[i] = random.getrandbits(8)
            echo_test(e, peer, msg, sync)


def init(sta_active=True, ap_active=False):
    wlans = [network.WLAN(i) for i in [network.WLAN.IF_STA, network.WLAN.IF_AP]]
    e = espnow.ESPNow()
    e.active(True)
    e.set_pmk(default_pmk)
    wlans[0].active(sta_active)
    wlans[1].active(ap_active)
    wlans[0].disconnect()  # Force esp8266 STA interface to disconnect from AP
    return e


# Server
def instance0():
    e = init(True, False)
    multitest.globals(PEERS=[network.WLAN(i).config("mac") for i in (0, 1)])
    multitest.next()
    print("Server Start")
    echo_server(e)
    print("Server Done")
    e.active(False)


# Client
def instance1():
    e = init(True, False)
    multitest.next()
    peer = PEERS[0]
    e.add_peer(peer)
    echo_test(e, peer, b"ping", True)
    gc.collect()
    mem_start = gc.mem_alloc()
    for i in range(10):
        echo_client(e, peer, [250] * 10)
        print("OK:", (i + 1) * 10)
    echo_test(e, peer, b"!done", True)
    gc.collect()
    mem_end = gc.mem_alloc()
    if mem_end - mem_start < 1024:
        print("OK: Less than 1024 bytes consumed")
    else:
        print("Error: Memory consumed is", mem_end - mem_start)
    e.active(False)
