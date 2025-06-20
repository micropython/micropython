# Test of a ESPnow echo server and client transferring data.
# Test the ESP32 extemnsions. Assumes instance1 is an ESP32.
# Instance0 may be an ESP32 or ESP8266

try:
    import network
    import random
    import time
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
    while True:
        peer, msg = e.irecv(timeout_ms)
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


def client_send(e, peer, msg, sync):
    print("TEST: send/recv(msglen=", len(msg), ",sync=", sync, "): ", end="", sep="")
    try:
        if not e.send(peer, msg, sync):
            print("ERROR: Send failed.")
            return
    except OSError as exc:
        # Don't print exc as it is differs for esp32 and esp8266
        print("ERROR: OSError:")
        return


def init(sta_active=True, ap_active=False):
    wlans = [network.WLAN(i) for i in [network.STA_IF, network.AP_IF]]
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


done = False


# Client
def instance1():
    # Instance 1 (the client)
    e = init(True, False)
    try:
        e.irq(None)
    except AttributeError:
        print("SKIP")
        raise SystemExit

    e.config(timeout_ms=timeout_ms)
    multitest.next()
    peer = PEERS[0]
    e.add_peer(peer)

    def on_recv_cb(e):
        global done
        p2, msg2 = e.irecv(0)
        print("OK" if msg2 == msg else "ERROR: Received != Sent")
        done = True

    global done
    print("IRQ() test...")
    e.irq(on_recv_cb)
    done = False
    msg = bytes([random.getrandbits(8) for _ in range(12)])
    client_send(e, peer, msg, True)
    start = time.ticks_ms()
    while not done:
        if time.ticks_diff(time.ticks_ms(), start) > timeout_ms:
            print("Timeout waiting for response.")
            raise SystemExit
    e.irq(None)

    # Tell the server to stop
    print("DONE")
    msg = b"!done"
    client_send(e, peer, msg, True)
    p2, msg2 = e.irecv()
    print("OK" if msg2 == msg else "ERROR: Received != Sent")

    e.active(False)
