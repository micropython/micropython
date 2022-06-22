# Test of a ESPnow echo server and client transferring data.
# Test the ESP32 extemnsions. Assumes instance1 is an ESP32.
# Instance1 may be and ESP32 or ESP8266

try:
    import network
    import random
    import usys
    from esp import espnow
except ImportError:
    print("SKIP")
    raise SystemExit

# Set read timeout to 5 seconds
timeout = 5000
default_pmk = b"Micropyth0nRules"
sync = True


def echo_server(e):
    peers = []
    while True:
        peer, msg = e.irecv(timeout)
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
    print("OK")


def init(sta_active=True, ap_active=False):
    wlans = [network.WLAN(i) for i in [network.STA_IF, network.AP_IF]]
    e = espnow.ESPNow()
    e.active(True)
    e.set_pmk(default_pmk)
    wlans[0].active(sta_active)
    wlans[1].active(ap_active)
    wlans[0].disconnect()  # Force esp8266 STA interface to disconnect from AP
    return e


def poll(e):
    poll = uselect.poll()
    poll.register(e, uselect.POLLIN)
    p = poll.ipoll(timeout)
    if not p:
        print("ERROR: poll() timeout waiting for response.")
    return p


# Server
def instance0():
    e = init(True, False)
    multitest.globals(PEERS=[network.WLAN(i).config("mac") for i in (0, 1)])
    multitest.next()
    print("Server Start")
    echo_server(e)
    print("Server Done")
    e.active(False)


def instance1():
    print("SKIP")


try:
    import uasyncio as asyncio

    async def client():
        from aioespnow import AIOESPNow

        e = AIOESPNow(init(True, False))
        e.config(timeout=timeout)
        peer = PEERS[0]
        e.add_peer(peer)
        multitest.next()

        print("airecv() test...")
        msgs = []
        for i in range(5):
            # Send messages to the peer who will echo it back
            msgs.append(bytes([random.getrandbits(8) for _ in range(12)]))
            client_send(e, peer, msgs[i], True)

        for i in range(5):
            mac, reply = await e.airecv()
            print("OK" if reply == msgs[i] else "ERROR: Received != Sent")

        # Tell the server to stop
        print("DONE")
        msg = b"!done"
        client_send(e, peer, msg, True)
        mac, reply = await e.airecv()
        print("OK" if reply == msg else "ERROR: Received != Sent")

        e.active(False)

    # Client
    def instance1():
        # Instance 1 (the client) must be an ESP32
        if usys.platform != "esp32":
            print("SKIP")
            raise SystemExit

        asyncio.run(client())

except:
    pass
