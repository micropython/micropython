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
    print("SKIP")


try:
    import uasyncio as asyncio
    from aioespnow import AIOESPNow

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

    def instance0():
        if usys.platform != "esp32":
            print("SKIP")
            raise SystemExit
        e = init(True, False)
        multitest.globals(PEERS=[network.WLAN(i).config("mac") for i in (0, 1)])
        multitest.next()
        print("Server Start")
        asyncio.run(echo_server(AIOESPNow(e)))
        print("Server Done")
        e.active(False)

except:
    pass


def instance1():
    e = init(True, False)
    peer = PEERS[0]
    e.add_peer(peer)
    multitest.next()

    for i in range(5):
        msg = bytes([random.getrandbits(8) for _ in range(12)])
        client_send(e, peer, msg, True)
        p2, msg2 = e.irecv(timeout)
        print("OK" if msg2 == msg else "ERROR: Received != Sent")

    # Tell the server to stop
    print("DONE")
    msg = b"!done"
    client_send(e, peer, msg, True)
    p2, msg2 = e.irecv(timeout)
    print("OK" if msg2 == msg else "ERROR: Received != Sent")

    e.active(False)
