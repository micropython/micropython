# Test L2CAP COC send/recv.

# Sends a sequence of varying-sized payloads from central->peripheral, and
# verifies that the other device sees the same data, then does the same thing
# peripheral->central.

from micropython import const
import time, machine, bluetooth, random

TIMEOUT_MS = 1000

_IRQ_CENTRAL_CONNECT = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)
_IRQ_PERIPHERAL_CONNECT = const(7)
_IRQ_PERIPHERAL_DISCONNECT = const(8)
_IRQ_L2CAP_ACCEPT = const(22)
_IRQ_L2CAP_CONNECT = const(23)
_IRQ_L2CAP_DISCONNECT = const(24)
_IRQ_L2CAP_RECV = const(25)
_IRQ_L2CAP_SEND_READY = const(26)

_L2CAP_MTU = const(450)
_L2CAP_PSM = const(22)

_PAYLOAD_LEN = const(_L2CAP_MTU - 50)
_PAYLOAD_LEN_STEP = -23
_NUM_PAYLOADS = const(16)

_RANDOM_SEED = 22


waiting_events = {}


def irq(event, data):
    if event == _IRQ_CENTRAL_CONNECT:
        conn_handle, addr_type, addr = data
        print("_IRQ_CENTRAL_CONNECT")
        waiting_events[event] = conn_handle
    elif event == _IRQ_CENTRAL_DISCONNECT:
        print("_IRQ_CENTRAL_DISCONNECT")
    elif event == _IRQ_PERIPHERAL_CONNECT:
        conn_handle, addr_type, addr = data
        print("_IRQ_PERIPHERAL_CONNECT")
        waiting_events[event] = conn_handle
    elif event == _IRQ_PERIPHERAL_DISCONNECT:
        print("_IRQ_PERIPHERAL_DISCONNECT")
    elif event == _IRQ_L2CAP_ACCEPT:
        conn_handle, cid, psm, our_mtu, peer_mtu = data
        print("_IRQ_L2CAP_ACCEPT", psm, our_mtu, peer_mtu)
        waiting_events[event] = (conn_handle, cid, psm)
    elif event == _IRQ_L2CAP_CONNECT:
        conn_handle, cid, psm, our_mtu, peer_mtu = data
        print("_IRQ_L2CAP_CONNECT", psm, our_mtu, peer_mtu)
        waiting_events[event] = (conn_handle, cid, psm, our_mtu, peer_mtu)
    elif event == _IRQ_L2CAP_DISCONNECT:
        conn_handle, cid, psm, status = data
        print("_IRQ_L2CAP_DISCONNECT", psm, status)
    elif event == _IRQ_L2CAP_RECV:
        conn_handle, cid = data
    elif event == _IRQ_L2CAP_SEND_READY:
        conn_handle, cid, status = data

    if event not in waiting_events:
        waiting_events[event] = None


def wait_for_event(event, timeout_ms):
    t0 = time.ticks_ms()
    while time.ticks_diff(time.ticks_ms(), t0) < timeout_ms:
        if event in waiting_events:
            return waiting_events.pop(event)
        machine.idle()
    raise ValueError("Timeout waiting for {}".format(event))


def send_data(ble, conn_handle, cid):
    buf = bytearray(_PAYLOAD_LEN)
    mv = memoryview(buf)
    print("l2cap_send", _NUM_PAYLOADS, _PAYLOAD_LEN)
    for i in range(_NUM_PAYLOADS):
        n = _PAYLOAD_LEN + i * _PAYLOAD_LEN_STEP
        for j in range(n):
            buf[j] = random.randint(0, 255)
        if not ble.l2cap_send(conn_handle, cid, mv[:n]):
            wait_for_event(_IRQ_L2CAP_SEND_READY, TIMEOUT_MS)


def recv_data(ble, conn_handle, cid):
    buf = bytearray(_PAYLOAD_LEN)
    recv_bytes = 0
    recv_correct = 0
    expected_bytes = (
        _PAYLOAD_LEN * _NUM_PAYLOADS + _PAYLOAD_LEN_STEP * _NUM_PAYLOADS * (_NUM_PAYLOADS - 1) // 2
    )
    print("l2cap_recvinto", expected_bytes)
    while recv_bytes < expected_bytes:
        wait_for_event(_IRQ_L2CAP_RECV, TIMEOUT_MS)
        while True:
            n = ble.l2cap_recvinto(conn_handle, cid, buf)
            if n == 0:
                break
            recv_bytes += n
            for i in range(n):
                if buf[i] == random.randint(0, 255):
                    recv_correct += 1
    return recv_bytes, recv_correct


# Acting in peripheral role.
def instance0():
    multitest.globals(BDADDR=ble.config("mac"))
    print("gap_advertise")
    ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")
    multitest.next()
    try:
        # Wait for central to connect to us.
        conn_handle = wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)

        print("l2cap_listen")
        ble.l2cap_listen(_L2CAP_PSM, _L2CAP_MTU)

        conn_handle, cid, psm = wait_for_event(_IRQ_L2CAP_ACCEPT, TIMEOUT_MS)
        conn_handle, cid, psm, our_mtu, peer_mtu = wait_for_event(_IRQ_L2CAP_CONNECT, TIMEOUT_MS)

        random.seed(_RANDOM_SEED)

        recv_bytes, recv_correct = recv_data(ble, conn_handle, cid)
        send_data(ble, conn_handle, cid)

        wait_for_event(_IRQ_L2CAP_DISCONNECT, TIMEOUT_MS)

        print("received", recv_bytes, recv_correct)

        # Wait for the central to disconnect.
        wait_for_event(_IRQ_CENTRAL_DISCONNECT, TIMEOUT_MS)
    finally:
        ble.active(0)


# Acting in central role.
def instance1():
    multitest.next()
    try:
        # Connect to peripheral and then disconnect.
        print("gap_connect")
        ble.gap_connect(*BDADDR)
        conn_handle = wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS)

        print("l2cap_connect")
        ble.l2cap_connect(conn_handle, _L2CAP_PSM, _L2CAP_MTU)
        conn_handle, cid, psm, our_mtu, peer_mtu = wait_for_event(_IRQ_L2CAP_CONNECT, TIMEOUT_MS)

        random.seed(_RANDOM_SEED)

        send_data(ble, conn_handle, cid)
        recv_bytes, recv_correct = recv_data(ble, conn_handle, cid)

        # Disconnect channel.
        print("l2cap_disconnect")
        ble.l2cap_disconnect(conn_handle, cid)
        wait_for_event(_IRQ_L2CAP_DISCONNECT, TIMEOUT_MS)

        print("received", recv_bytes, recv_correct)

        # Disconnect from peripheral.
        print("gap_disconnect:", ble.gap_disconnect(conn_handle))
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)
    finally:
        ble.active(0)


ble = bluetooth.BLE()
ble.active(1)
ble.irq(irq)
