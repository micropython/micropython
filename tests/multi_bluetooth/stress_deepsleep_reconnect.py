# Test BLE re-connect after peripheral deepsleep.
# instance0 needs to be a device capable of deepsleep without disconnecting the
# REPL, eg. STM32WB55 Nucleo board with REPL on UART port.

from micropython import const
import time, machine, bluetooth

# Note: This value can be much lower most of the time, but an ESP32 with a boot.py
# that connects to Wi-Fi may take an extra 5 seconds after reboot.
TIMEOUT_MS = 8000

_IRQ_CENTRAL_CONNECT = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)
_IRQ_PERIPHERAL_CONNECT = const(7)
_IRQ_PERIPHERAL_DISCONNECT = const(8)

waiting_events = {}


def irq(event, data):
    if event == _IRQ_CENTRAL_CONNECT:
        print("_IRQ_CENTRAL_CONNECT")
        waiting_events[event] = data[0]
    elif event == _IRQ_CENTRAL_DISCONNECT:
        print("_IRQ_CENTRAL_DISCONNECT")
    elif event == _IRQ_PERIPHERAL_CONNECT:
        print("_IRQ_PERIPHERAL_CONNECT")
        waiting_events[event] = data[0]
    elif event == _IRQ_PERIPHERAL_DISCONNECT:
        print("_IRQ_PERIPHERAL_DISCONNECT")

    if event not in waiting_events:
        waiting_events[event] = None


def wait_for_event(event, timeout_ms):
    t0 = time.ticks_ms()
    while time.ticks_diff(time.ticks_ms(), t0) < timeout_ms:
        if event in waiting_events:
            return waiting_events.pop(event)
        machine.idle()
    raise ValueError("Timeout waiting for {}".format(event))


# Acting in peripheral role.
def instance0():
    multitest.globals(BDADDR=ble.config("mac"))
    print("gap_advertise")
    ble.gap_advertise(20_000, b"\x02\x01\x06\x04\tMPY")
    multitest.next()
    # Wait for central to connect, then wait for it to disconnect.
    wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)
    wait_for_event(_IRQ_CENTRAL_DISCONNECT, TIMEOUT_MS)

    multitest.expect_reboot(resume="instance0_resume", delay_ms=100)
    machine.deepsleep(100)


# Acting in peripheral role.
def instance0_resume():
    # continue here after reboot
    print("restarted")
    print("gap_advertise")
    ble.gap_advertise(20_000, b"\x02\x01\x06\x04\tMPY")
    try:
        # Wait for central to connect, then disconnect it.
        conn_handle = wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)
        print("gap_disconnect:", ble.gap_disconnect(conn_handle))
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
        print("gap_disconnect:", ble.gap_disconnect(conn_handle))
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)

        # Connect to peripheral and then let the peripheral disconnect us.
        # Extra scan timeout allows for the peripheral to receive the disconnect
        # event and start advertising again.
        print("gap_connect")
        ble.gap_connect(BDADDR[0], BDADDR[1], 5000)
        wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS)
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)
    finally:
        ble.active(0)


ble = bluetooth.BLE()
ble.active(1)
ble.irq(irq)
