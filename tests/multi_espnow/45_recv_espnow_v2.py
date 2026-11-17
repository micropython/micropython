# Test of a ESP-NOW V2 echo server and client transferring data, as a variant of
# 40_recv_test.py.
#
# This test requires both instances to be ESP32s built with ESP-IDF V5.5 or
# newer for ESP-NOW V2 support.
#
# Explicitly tests the irecv(), rev() and recvinto() methods.

try:
    import network, os
    import random
    import espnow
except ImportError:
    print("SKIP")
    raise SystemExit

if espnow.MAX_DATA_LEN < 1470:  # Check for V2 support
    print("SKIP")
    raise SystemExit

# Set read timeout to 8 seconds
timeout_ms = 8000
default_pmk = b"VerySecretValue!"
sync = True


def echo_server(e):
    peers = []
    while True:
        peer, msg = e.irecv(timeout_ms)
        if peer is None:
            return
        if peer not in peers:
            peers.append(peer)
            e.add_peer(peer)

        #  Echo the MAC and message back to the sender
        if not e.send(peer, msg, sync):
            print("ERROR: send() failed to", peer.hex())
            return

        if msg == b"!done":
            return


def client_send(e, peer, msg, sync):
    import time

    time.sleep_ms(100)
    print("TEST: send/recv(msglen=", len(msg), ",sync=", sync, "): ", end="", sep="")
    try:
        if not e.send(peer, msg, sync):
            print("ERROR: Send failed.")
            return
    except OSError as exc:
        # Don't print exc as it is differs for esp32 and esp8266
        print("ERROR: OSError:", exc)  # FIXME
        return


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


def check_recv(s, r):
    if s == r:
        print("OK")
    else:
        print("ERROR: Received != Sent")
        print("Sent", len(s), "bytes")
        if r is None:
            print("Receive timed out")
        else:
            print("Received", len(r), "bytes")


# Client
def instance1():
    # Instance 1 (the client)
    e = init(True, False)
    e.config(timeout_ms=timeout_ms)
    multitest.next()
    peer = PEERS[0]
    e.add_peer(peer)

    print("RECVINTO() test...")
    msg = os.urandom(768)
    client_send(e, peer, msg, True)
    data = [bytearray(espnow.ADDR_LEN), bytearray(espnow.MAX_DATA_LEN)]
    n = e.recvinto(data)
    check_recv(msg, data[1])

    print("IRECV() test...")
    msg = os.urandom(768)
    client_send(e, peer, msg, True)
    p2, msg2 = e.irecv()
    check_recv(msg, msg2)

    print("RECV() test...")
    msg = os.urandom(1024)
    client_send(e, peer, msg, True)
    p2, msg2 = e.recv()
    check_recv(msg, msg2)

    print("ITERATOR() test...")
    msg = os.urandom(espnow.MAX_DATA_LEN)
    client_send(e, peer, msg, True)
    p2, msg2 = next(e)
    check_recv(msg, msg2)

    # Tell the server to stop
    print("DONE")
    msg = b"!done"
    client_send(e, peer, msg, True)
    p2, msg2 = e.irecv()
    check_recv(msg, msg2)

    e.active(False)
