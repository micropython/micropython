# Send L2CAP data as fast as possible and time it.

from micropython import const
import time, machine, bluetooth, random

if not hasattr(bluetooth.BLE, "l2cap_connect"):
    print("SKIP")
    raise SystemExit

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

_L2CAP_PSM = const(22)
_L2CAP_MTU = const(512)

_PAYLOAD_LEN = const(_L2CAP_MTU)
_NUM_PAYLOADS = const(20)

_RANDOM_SEED = 22


waiting_events = {}


def irq(event, data):
    if event == _IRQ_CENTRAL_CONNECT:
        conn_handle, addr_type, addr = data
        waiting_events[event] = conn_handle
    elif event == _IRQ_PERIPHERAL_CONNECT:
        conn_handle, addr_type, addr = data
        waiting_events[event] = conn_handle
    elif event == _IRQ_L2CAP_ACCEPT:
        conn_handle, cid, psm, our_mtu, peer_mtu = data
        waiting_events[event] = (conn_handle, cid, psm)
    elif event == _IRQ_L2CAP_CONNECT:
        conn_handle, cid, psm, our_mtu, peer_mtu = data
        waiting_events[event] = (conn_handle, cid, psm, our_mtu, peer_mtu)

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
    for i in range(_NUM_PAYLOADS):
        for j in range(_PAYLOAD_LEN):
            buf[j] = random.randint(0, 255)
        if not ble.l2cap_send(conn_handle, cid, buf):
            wait_for_event(_IRQ_L2CAP_SEND_READY, TIMEOUT_MS)


def recv_data(ble, conn_handle, cid):
    buf = bytearray(_PAYLOAD_LEN)
    recv_bytes = 0
    recv_correct = 0
    expected_bytes = _PAYLOAD_LEN * _NUM_PAYLOADS
    ticks_first_byte = 0
    while recv_bytes < expected_bytes:
        wait_for_event(_IRQ_L2CAP_RECV, TIMEOUT_MS)
        if not ticks_first_byte:
            ticks_first_byte = time.ticks_ms()
        while True:
            n = ble.l2cap_recvinto(conn_handle, cid, buf)
            if n == 0:
                break
            recv_bytes += n
            for i in range(n):
                if buf[i] == random.randint(0, 255):
                    recv_correct += 1
    ticks_end = time.ticks_ms()
    return recv_bytes, recv_correct, time.ticks_diff(ticks_end, ticks_first_byte)


# Acting in peripheral role.
def instance0():
    multitest.globals(BDADDR=ble.config("mac"))
    ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")
    multitest.next()
    try:
        # Wait for central to connect to us.
        conn_handle = wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)

        ble.l2cap_listen(_L2CAP_PSM, _L2CAP_MTU)

        conn_handle, cid, psm = wait_for_event(_IRQ_L2CAP_ACCEPT, TIMEOUT_MS)
        conn_handle, cid, psm, our_mtu, peer_mtu = wait_for_event(_IRQ_L2CAP_CONNECT, TIMEOUT_MS)

        random.seed(_RANDOM_SEED)

        send_data(ble, conn_handle, cid)

        wait_for_event(_IRQ_L2CAP_DISCONNECT, TIMEOUT_MS)

        # Wait for the central to disconnect.
        wait_for_event(_IRQ_CENTRAL_DISCONNECT, TIMEOUT_MS)
    finally:
        ble.active(0)


# Acting in central role.
def instance1():
    multitest.next()
    try:
        # Connect to peripheral and then disconnect.
        ble.gap_connect(*BDADDR)
        conn_handle = wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS)

        ble.l2cap_connect(conn_handle, _L2CAP_PSM, _L2CAP_MTU)
        conn_handle, cid, psm, our_mtu, peer_mtu = wait_for_event(_IRQ_L2CAP_CONNECT, TIMEOUT_MS)

        random.seed(_RANDOM_SEED)

        recv_bytes, recv_correct, total_ticks = recv_data(ble, conn_handle, cid)

        # Disconnect channel.
        ble.l2cap_disconnect(conn_handle, cid)
        wait_for_event(_IRQ_L2CAP_DISCONNECT, TIMEOUT_MS)

        print(
            "Received {}/{} bytes in {} ms. {} B/s".format(
                recv_bytes, recv_correct, total_ticks, recv_bytes * 1000 // total_ticks
            )
        )

        # Disconnect from peripheral.
        ble.gap_disconnect(conn_handle)
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)
    finally:
        ble.active(0)


ble = bluetooth.BLE()
ble.active(1)
ble.irq(irq)
