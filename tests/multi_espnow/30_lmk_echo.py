# Test of a ESPnow echo server and client transferring encrypted data.
# This test works with ESP32 or ESP8266 as server or client.

# First instance (echo server):
#   Set the shared PMK
#   Set the PEERS global to our mac addresses
#   Run the echo server
#     First exchange an unencrypted message from the client (so we
#     can get its MAC address) and echo the message back (unenecrypted).
#     Then set the peer LMK so all further communications are encrypted.

# Second instance (echo client):
#   Set the shared PMK
#   Send an unencrypted message to the server and wait for echo response.
#   Set the LMK for the peer communications so all further comms are encrypted.
#   Send random messages and compare with response from server.

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
default_lmk = b"0123456789abcdef"
sync = True


def echo_server(e):
    peers = []
    while True:
        # Wait for messages from the client
        peer, msg = e.recv(timeout_ms)
        if peer is None:
            return
        if peer not in peers:
            # If this is first message, add the peer unencrypted
            e.add_peer(peer)

        #  Echo the message back to the sender
        if not e.send(peer, msg, sync):
            print("ERROR: send() failed to", peer)
            return

        if peer not in peers:
            # If this is first message, add the peer encrypted
            peers.append(peer)
            e.del_peer(peer)
            e.add_peer(peer, default_lmk)

        if msg == b"!done":
            return


# Send a message from the client and compare with response from server.
def echo_test(e, peer, msg, sync):
    print("TEST: send/recv(msglen=", len(msg), ",sync=", sync, "): ", end="", sep="")
    try:
        if not e.send(peer, msg, sync):
            print("ERROR: Send failed.")
            return
    except OSError as exc:
        # Don't print exc as it is differs for esp32 and esp8266
        print("ERROR: OSError:")
        return

    p2, msg2 = e.recv(timeout_ms)
    if p2 is None:
        print("ERROR: No response from server.")
        raise SystemExit

    print("OK" if msg2 == msg else "ERROR: Received != Sent")


# Send some random messages to server and check the responses
def echo_client(e, peer, msglens):
    for sync in [True, False]:
        for msglen in msglens:
            msg = bytearray(msglen)
            if msglen > 0:
                msg[0] = b"_"[0]  # Random message must not start with '!'
            for i in range(1, msglen):
                msg[i] = random.getrandbits(8)
            echo_test(e, peer, msg, sync)


# Initialise the wifi and espnow hardware and software
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
    macs = [network.WLAN(i).config("mac") for i in (0, 1)]
    print("Server Start")
    multitest.globals(PEERS=macs)
    multitest.next()
    echo_server(e)
    print("Server Done")
    e.active(False)


# Client
def instance1():
    e = init(True, False)
    multitest.next()
    peer = PEERS[0]
    e.add_peer(peer)
    echo_test(e, peer, b"start", True)
    # Wait long enough for the server to set the lmk
    time.sleep(0.1)
    e.del_peer(peer)
    e.add_peer(peer, default_lmk)
    echo_client(e, peer, [250])
    echo_test(e, peer, b"!done", True)
    e.active(False)
