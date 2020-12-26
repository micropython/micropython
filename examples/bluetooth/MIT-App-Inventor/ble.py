# This code requires the micropython_dotstar and ble_advertising libraries

from micropython import const
from micropython_dotstar import DotStar
from machine import Pin, SPI

import bluetooth
from ble_advertising import advertising_payload

_IRQ_CENTRAL_CONNECT = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)
_IRQ_GATTS_WRITE = const(3)


def irq(event, data):
    global dots
    if event == _IRQ_CENTRAL_CONNECT:
        print("connect")
        conn_handle, addr_type, addr = data
    elif event == _IRQ_CENTRAL_DISCONNECT:
        print("disconnect")
        conn_handle, addr_type, addr = data
        ble.gap_advertise(100, adv_data=None)
    elif event == _IRQ_GATTS_WRITE:
        conn_handle, attr_handle = data
        value = ble.gatts_read(attr_handle)
        value = int.from_bytes(value, "little")
        r = value & 0xFF
        g = (value >> 8) & 0xFF
        b = (value >> 16) & 0xFF
        dots[0] = (r, g, b)


# https://docs.micropython.org/en/latest/library/ubluetooth.html#ubluetooth.BLE.gatts_register_services
# No official protocol used, so I need random UUID as a service and
# characteristic
ble_service = (
    bluetooth.UUID("aa4a619f-04ea-43ac-854a-7e49cd1437d9"),
    ((bluetooth.UUID("5cbed0dc-eeb2-4145-a297-a03ccf464563"), bluetooth.FLAG_WRITE),),
)


# Initialize the dotstar (pins used by TinyPICO's builtin one)
spi = SPI(baudrate=50000, sck=Pin(12), mosi=Pin(2), miso=Pin(19))
dots = DotStar(spi, 1, brightness=0.5)  # Just one DotStar, half brightness

# Initialize bluetooth
ble = bluetooth.BLE()
ble.irq(irq)
ble.active(True)
ble.gatts_register_services((ble_service,))
ble.gap_advertise(100, adv_data=advertising_payload(name="BLE-led", services=(ble_service[0],)))
