import array
import usb.core
import sys

# This is a basic Microsoft optical mouse with two buttons and a wheel that can
# also be pressed.
USB_VID = 0x045E
USB_PID = 0x0040
# This is ordered by bit position.
BUTTONS = ["left", "right", "middle"]

device = usb.core.find(idVendor=USB_VID, idProduct=USB_PID)

print(device.manufacturer, device.product)

# Test to see if the kernel is using the device and detach it.
if device.is_kernel_driver_active(0):
    device.detach_kernel_driver(0)

# Boot mice have 4 byte reports
buf = array.array("b", [0] * 4)
report_count = 0
while True:
    try:
        count = device.read(0x81, buf)
    except usb.core.USBTimeoutError:
        continue
    if report_count % 15 == 0:
        print("x y wheel buttons")
    print(buf[1], buf[2], buf[3], end=" ")
    for i, button in enumerate(BUTTONS):
        if buf[0] & (1 << i) != 0:
            print(button, end=" ")
    print()
    report_count += 1
