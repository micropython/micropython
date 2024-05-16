# Implementation of a very simple, custom USB device in Python.  The device has an
# IN and OUT endpoint, accepts up to 64 bytes of data on the OUT endpoint, and echos
# that data back to the IN endpoint.
#
# To run, just execute this file on a device with machine.USBDevice support.  The device
# will then change to the custom USB device.
#
# For example, use `mpremote` (the `--no-follow` option starts the script running
# without waiting for a response, because there won't be a response after the device
# changes its USB mode):
#
# $ mpremote run --no-follow usb_simple_device.py
#
# Then, run the host side of the example on your PC using:
#
# $ sudo python usb_simple_host_pyusb.py
#
# You'll need to have `pyusb` installed via `pip install pyusb` to run the host PC code.
# And `sudo` is most likely needed to access the custom USB device.
#
# Once you have finished running the code, you will need to reset or unplug the USB
# device to stop it.

import machine

# VID and PID of the USB device.
VID = 0xF055
PID = 0x9999

# USB endpoints used by the device.
EP_OUT = 0x01
EP_IN = 0x81

# USB device descriptor.
_desc_dev = bytes(
    [
        0x12,  # bLength
        0x01,  # bDescriptorType: Device
        0x00,
        0x02,  # USB version: 2.00
        0xFF,  # bDeviceClass: vendor
        0x00,  # bDeviceSubClass
        0x01,  # bDeviceProtocol
        0x40,  # bMaxPacketSize
        VID & 0xFF,
        VID >> 8 & 0xFF,  # VID
        PID & 0xFF,
        PID >> 8 & 0xFF,  # PID
        0x00,
        0x01,  # bcdDevice: 1.00
        0x11,  # iManufacturer
        0x12,  # iProduct
        0x13,  # iSerialNumber
        0x01,  # bNumConfigurations: 1
    ]
)

# USB configuration descriptor.
_desc_cfg = bytes(
    [
        # Configuration Descriptor.
        0x09,  # bLength
        0x02,  # bDescriptorType: configuration
        0x20,
        0x00,  # wTotalLength: 32
        0x01,  # bNumInterfaces
        0x01,  # bConfigurationValue
        0x14,  # iConfiguration
        0xA0,  # bmAttributes
        0x96,  # bMaxPower
        # Interface Descriptor.
        0x09,  # bLength
        0x04,  # bDescriptorType: interface
        0x00,  # bInterfaceNumber
        0x00,  # bAlternateSetting
        0x02,  # bNumEndpoints
        0xFF,  # bInterfaceClass
        0x03,  # bInterfaceSubClass
        0x00,  # bInterfaceProtocol
        0x15,  # iInterface
        # Endpoint IN1.
        0x07,  # bLength
        0x05,  # bDescriptorType: endpoint
        EP_IN,  # bEndpointAddress
        0x03,  # bmAttributes: interrupt
        0x40,
        0x00,  # wMaxPacketSize
        0x0A,  # bInterval
        # Endpoint OUT1.
        0x07,  # bLength
        0x05,  # bDescriptorType: endpoint
        EP_OUT,  # bEndpointAddress
        0x02,  # bmAttributes: bulk
        0x40,
        0x00,  # wMaxPacketSize
        0x00,  # bInterval
    ]
)

# USB strings.
_desc_strs = {
    0x11: b"iManufacturer",
    0x12: b"iProduct",
    0x13: b"iSerial",
    0x14: b"iInterface",
    0x15: b"iInterface",
    0x16: b"Extra information",
}


# USB callback for when our custom USB interface is opened by the host.
def _open_itf_cb(interface_desc_view):
    print("_open_itf_cb", bytes(interface_desc_view))
    # Prepare to receive first data packet on the OUT endpoint.
    usbd.submit_xfer(EP_OUT, usbd_buf)


# USB callback for when a data transfer (IN or OUT) has completed.
def _xfer_cb(ep_addr, result, xferred_bytes):
    print("_xfer_cb", ep_addr, result, xferred_bytes)
    if ep_addr == EP_OUT:
        # Received data packet from the host, print it out.
        print(usbd_buf)
        # And then echo the data back to the host.
        usbd.submit_xfer(EP_IN, memoryview(usbd_buf)[:xferred_bytes])
    elif ep_addr == EP_IN:
        # Host got our data, prepare to receive the next data packet.
        usbd.submit_xfer(EP_OUT, usbd_buf)


# USB data buffer, for IN and OUT transfers.
usbd_buf = bytearray(64)

# Switch the USB device to our custom USB driver.
usbd = machine.USBDevice()
usbd.builtin_driver = usbd.BUILTIN_NONE
usbd.config(
    desc_dev=_desc_dev,
    desc_cfg=_desc_cfg,
    desc_strs=_desc_strs,
    open_itf_cb=_open_itf_cb,
    xfer_cb=_xfer_cb,
)
usbd.active(1)
