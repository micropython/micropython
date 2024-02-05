# Test BLE GAP advertising and scanning

from micropython import const
import time, machine, bluetooth

_IRQ_SCAN_RESULT = const(5)
_IRQ_SCAN_DONE = const(6)

ADV_TIME_MS = 3000


def instance0():
    multitest.globals(BDADDR=ble.config("mac"))
    multitest.next()

    adv_data = b"\x02\x01\x06\x04\xffMPY"

    print("gap_advertise(100_000, connectable=False)")
    ble.gap_advertise(100_000, adv_data, connectable=False)
    time.sleep_ms(ADV_TIME_MS)

    print("gap_advertise(20_000, connectable=True)")
    ble.gap_advertise(20_000, adv_data, connectable=True)
    time.sleep_ms(ADV_TIME_MS)

    print("gap_advertise(None)")
    ble.gap_advertise(None)

    ble.active(0)


def instance1():
    multitest.next()
    finished = False
    matched_adv_types = {}
    matched_adv_data = None

    def irq(ev, data):
        nonlocal finished, matched_adv_types, matched_adv_data
        if ev == _IRQ_SCAN_RESULT:
            addr_type, addr, adv_type, rssi, adv_data = data
            if addr_type == BDADDR[0] and addr == BDADDR[1]:
                matched_adv_types[adv_type] = True
                if matched_adv_data is None:
                    matched_adv_data = bytes(adv_data)
                else:
                    if adv_data != matched_adv_data:
                        matched_adv_data = b"MISMATCH"
        elif ev == _IRQ_SCAN_DONE:
            finished = True

    try:
        ble.config(rxbuf=2000)
    except:
        pass
    ble.irq(irq)
    ble.gap_scan(2 * ADV_TIME_MS, 30000, 30000)
    while not finished:
        machine.idle()
    ble.active(0)
    print("adv_types:", sorted(matched_adv_types))
    print("adv_data:", matched_adv_data)


ble = bluetooth.BLE()
ble.active(1)
