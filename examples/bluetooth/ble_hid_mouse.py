# Implements a BLE HID mouse

from micropython import const
import struct
import bluetooth


def ble_irq(event, data):
    global conn_handle
    if event == 1:
        print("connect")
        conn_handle = data[0]
    else:
        print("event:", event, data)


ble = bluetooth.BLE()
ble.active(1)
ble.irq(ble_irq)

UUID = bluetooth.UUID

F_READ = bluetooth.FLAG_READ
F_WRITE = bluetooth.FLAG_WRITE
F_READ_WRITE = bluetooth.FLAG_READ | bluetooth.FLAG_WRITE
F_READ_NOTIFY = bluetooth.FLAG_READ | bluetooth.FLAG_NOTIFY

ATT_F_READ = 0x01
ATT_F_WRITE = 0x02

hid_service = (
    UUID(0x1812),  # Human Interface Device
    (
        (UUID(0x2A4A), F_READ),  # HID information
        (UUID(0x2A4B), F_READ),  # HID report map
        (UUID(0x2A4C), F_WRITE),  # HID control point
        (UUID(0x2A4D), F_READ_NOTIFY, ((UUID(0x2908), ATT_F_READ),)),  # HID report / reference
        (UUID(0x2A4E), F_READ_WRITE),  # HID protocol mode
    ),
)

# fmt: off
HID_REPORT_MAP = bytes([
    0x05, 0x01,     # Usage Page (Generic Desktop)
    0x09, 0x02,     # Usage (Mouse)
    0xA1, 0x01,     # Collection (Application)
    0x09, 0x01,     #     Usage (Pointer)
    0xA1, 0x00,     #     Collection (Physical)
    0x85, 0x01,     #         Report ID (1)
    0x95, 0x03,     #         Report Count (3)
    0x75, 0x01,     #         Report Size (1)
    0x05, 0x09,     #         Usage Page (Buttons)
    0x19, 0x01,     #         Usage Minimum (1)
    0x29, 0x03,     #         Usage Maximum (3)
    0x15, 0x00,     #         Logical Minimum (0)
    0x25, 0x01,     #         Logical Maximum (1)
    0x81, 0x02,     #         Input(Data, Variable, Absolute); 3 button bits
    0x95, 0x01,     #         Report Count(1)
    0x75, 0x05,     #         Report Size(5)
    0x81, 0x01,     #         Input(Constant);                 5 bit padding
    0x75, 0x08,     #         Report Size (8)
    0x95, 0x02,     #         Report Count (3)
    0x05, 0x01,     #         Usage Page (Generic Desktop)
    0x09, 0x30,     #         Usage (X)
    0x09, 0x31,     #         Usage (Y)
    0x09, 0x38,     #         Usage (Wheel)
    0x15, 0x81,     #         Logical Minimum (-127)
    0x25, 0x7F,     #         Logical Maximum (127)
    0x81, 0x06,     #         Input(Data, Variable, Relative); 3 position bytes (X,Y,Wheel)
    0xC0,           #     End Collection
    0xC0,           # End Collection
])
# fmt: on

# register services
ble.config(gap_name="MP-mouse")
handles = ble.gatts_register_services((hid_service,))
print(handles)
h_info, h_hid, _, h_rep, h_d1, h_proto = handles[0]

# set initial data
ble.gatts_write(h_info, b"\x01\x01\x00\x02")  # HID info: ver=1.1, country=0, flags=normal
ble.gatts_write(h_hid, HID_REPORT_MAP)  # HID report map
ble.gatts_write(h_d1, struct.pack("<BB", 1, 1))  # report: id=1, type=input
ble.gatts_write(h_proto, b"\x01")  # protocol mode: report

# advertise
adv = (
    b"\x02\x01\x06"
    b"\x03\x03\x12\x18"  # complete list of 16-bit service UUIDs: 0x1812
    b"\x03\x19\xc2\x03"  # appearance: mouse
    b"\x09\x09MP-mouse"  # complete local name
)
conn_handle = None
ble.gap_advertise(100_000, adv)

# once connected use the following to send reports


def send_mouse(button_mask, x, y, wheel):
    ble.gatts_notify(conn_handle, h_rep, struct.pack("4B", button_mask, x, y, wheel))


def send_click(button):
    send_mouse(1 << button, 0, 0, 0)
    send_mouse(0, 0, 0, 0)


def send_motion(x, y):
    send_mouse(0, x, y, 0)
