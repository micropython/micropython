# Test BLE GAP advertising and scanning

from micropython import const
import time, machine, bluetooth

_IRQ_SCAN_RESULT = const(1 << 4)
_IRQ_SCAN_COMPLETE = const(1 << 5)

ADV_TIME_S = 3


def instance0():
    multitest.globals(BDADDR=ble.config("mac"))
    multitest.next()

    print("gap_advertise(100_000, connectable=False)")
    ble.gap_advertise(100_000, b"\x02\x01\x06\x04\xffMPY", connectable=False)
    time.sleep(ADV_TIME_S)

    print("gap_advertise(20_000, connectable=True)")
    ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY", connectable=True)
    time.sleep(ADV_TIME_S)

    print("gap_advertise(None)")
    ble.gap_advertise(None)

    ble.active(0)


def instance1():
    multitest.next()
    finished = False
    adv_types = set()
    adv_data = None

    def irq(ev, data):
        nonlocal finished, adv_types, adv_data
        if ev == _IRQ_SCAN_RESULT:
            if data[1] == BDADDR:
                adv_types.add(data[2])
                if adv_data is None:
                    adv_data = bytes(data[4])
                else:
                    if adv_data != data[4]:
                        adv_data = b"MISMATCH"
        elif ev == _IRQ_SCAN_COMPLETE:
            finished = True

    ble.config(rxbuf=2000)
    ble.irq(irq)
    ble.gap_scan(2 * ADV_TIME_S * 1000, 10000, 10000)
    while not finished:
        machine.idle()
    ble.active(0)
    print("adv_types:", sorted(adv_types))
    print("adv_data:", adv_data)


ble = bluetooth.BLE()
ble.active(1)
