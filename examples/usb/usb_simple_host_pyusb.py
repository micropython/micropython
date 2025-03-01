#!/usr/bin/env python3
#
# Host side of the `usb_simple_device.py` example.  This must be run using standard
# Python on a PC.  See further instructions in `usb_simple_device.py`.

import sys
import usb.core
import usb.util

# VID and PID of the custom USB device.
VID = 0xF055
PID = 0x9999

# USB endpoints used by the device.
EP_OUT = 0x01
EP_IN = 0x81


def main():
    # Search for the custom USB device by VID/PID.
    dev = usb.core.find(idVendor=VID, idProduct=PID)

    if dev is None:
        print("No USB device found")
        sys.exit(1)

    # Claim the USB device.
    usb.util.claim_interface(dev, 0)

    # Read the device's strings.
    for i in range(0x11, 0x17):
        print(f"str{i}:", usb.util.get_string(dev, i))

    # Test writing to the device.
    ret = dev.write(EP_OUT, b"01234567", timeout=1000)
    print(ret)

    # Test reading from the device.
    print(dev.read(EP_IN, 64))

    # Release the USB device.
    usb.util.release_interface(dev, 0)
    usb.util.dispose_resources(dev)


if __name__ == "__main__":
    main()
