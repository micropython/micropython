# Test of a ESPnow asyncio echo server and client transferring data.
# Test will SKIP if instance0 (asyncio echo server) does not support asyncio.
# - eg. ESP8266 with 1MB flash.
# Instance1 is not required to support asyncio.

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


def init(e, sta_active=True, ap_active=False):
    wlans = [network.WLAN(i) for i in [network.WLAN.IF_STA, network.WLAN.IF_AP]]
    e.active(True)
    e.set_pmk(default_pmk)
    wlans[0].active(sta_active)
    wlans[1].active(ap_active)
    wlans[0].disconnect()  # Force esp8266 STA interface to disconnect from AP
    return e


async def echo_server(e):
    peers = []
    async for peer, msg in e:
        if peer not in peers:
            peers.append(peer)
            e.add_peer(peer)

        #  Echo the message back to the sender
        if not await e.asend(peer, msg, sync):
            print("ERROR: asend() failed to", peer)
            return

        if msg == b"!done":
            return


# Server
def instance0():
    try:
        import asyncio
        from aioespnow import AIOESPNow
    except ImportError:
        print("SKIP")
        raise SystemExit
    e = AIOESPNow()
    init(e, True, False)
    multitest.globals(PEERS=[network.WLAN(i).config("mac") for i in (0, 1)])
    multitest.next()
    print("Server Start")
    asyncio.run(echo_server(e))
    print("Server Done")
    e.active(False)


def instance1():
    e = espnow.ESPNow()
    init(e, True, False)
    peer = PEERS[0]
    e.add_peer(peer)
    multitest.next()

    for i in range(5):
        msg = bytes([random.getrandbits(8) for _ in range(12)])
        client_send(e, peer, msg, True)
        p2, msg2 = e.irecv(timeout_ms)
        print("OK" if msg2 == msg else "ERROR: Received != Sent")

    # Tell the server to stop
    print("DONE")
    msg = b"!done"
    client_send(e, peer, msg, True)
    p2, msg2 = e.irecv(timeout_ms)
    print("OK" if msg2 == msg else "ERROR: Received != Sent")

    e.active(False)
