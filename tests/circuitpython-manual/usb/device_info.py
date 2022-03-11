import usb_host
import board
import digitalio
import usb.core
import time

if hasattr(board, "USB_HOST_POWER"):
    d = digitalio.DigitalInOut(board.USB_HOST_POWER)
    d.switch_to_output(value=True)
    print("USB power on")

h = usb_host.Port(board.USB_HOST_DP, board.USB_HOST_DM)

while True:
    for device in usb.core.find(find_all=True):
        print(f"{device.idVendor:04x}:{device.idProduct:04x}")
        print(device.manufacturer, device.product)
        print(device.serial_number)
    print()
    time.sleep(10)
