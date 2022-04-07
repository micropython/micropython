import array
import usb.core
import sys

# This is a WASD Code Keyboard with a generic controller in it.
USB_VID = 0x04D9
USB_PID = 0x0169
# This is ordered by bit position.
MODIFIERS = []

device = usb.core.find(idVendor=USB_VID, idProduct=USB_PID)

print(device.manufacturer, device.product)

# Test to see if the kernel is using the device and detach it.
if device.is_kernel_driver_active(0):
    device.detach_kernel_driver(0)

# Boot keyboards have 8 byte reports
buf = array.array("B", [0] * 8)
report_count = 0
while True:
    try:
        count = device.read(0x81, buf)
    except usb.core.USBTimeoutError:
        continue
    if report_count % 15 == 0:
        print("modifiers keys")
    print(buf[0], end=" ")
    for i in range(2, 8):
        if buf[i] > 0:
            print(buf[i], end=" ")
    print()
    report_count += 1
