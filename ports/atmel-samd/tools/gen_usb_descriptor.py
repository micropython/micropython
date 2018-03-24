import argparse

import os
import sys

# path hacking
sys.path.append("../../tools/usb_descriptor")

from adafruit_usb_descriptor import cdc, hid, msc, standard, util

parser = argparse.ArgumentParser(description='Generate USB descriptors.')
parser.add_argument('--manufacturer', type=str,
                    help='manufacturer of the device')
parser.add_argument('--product', type=str,
                    help='product name of the device')
parser.add_argument('--vid', type=lambda x: int(x, 16),
                    help='vendor id')
parser.add_argument('--pid', type=lambda x: int(x, 16),
                    help='product id')
parser.add_argument('--serial_number_length', type=int, default=32,
                    help='length needed for the serial number in digits')
parser.add_argument('output_file', type=argparse.FileType('w'))

args = parser.parse_args()

langid = standard.StringDescriptor("\u0409")
manufacturer = standard.StringDescriptor(args.manufacturer)
product = standard.StringDescriptor(args.product)
serial_number = standard.StringDescriptor("serial number. you should fill in a unique serial number here."[:args.serial_number_length])
strings = [langid, manufacturer, product, serial_number]

# vid = 0x239A
# pid = 0x8021

device = standard.DeviceDescriptor(
    idVendor=args.vid,
    idProduct=args.pid,
    iManufacturer=strings.index(manufacturer),
    iProduct=strings.index(product),
    iSerialNumber=strings.index(serial_number))

# Interface numbers are interface set local and endpoints are interface local
# until core.join_interfaces renumbers them.
cdc_interfaces = [
    standard.InterfaceDescriptor(
        bInterfaceClass=cdc.CDC_CLASS_COMM,  # Communications Device Class
        bInterfaceSubClass=cdc.CDC_SUBCLASS_ACM,  # Abstract control model
        bInterfaceProtocol=cdc.CDC_PROTOCOL_V25TER,  # Common AT Commands
        subdescriptors=[
            # Working 2.x
            # radix: hexadecimal
            # 05 24 00 10 01 header
            # 05 24 01 03 01 call manage
            # 04 24 02 06 acm
            # 05 24 06 00 01 union
            cdc.Header(bcdCDC=0x0110),
            cdc.CallManagement(bmCapabilities=0x03, bDataInterface=0x01),
            cdc.AbstractControlManagement(bmCapabilities=0x02),
            cdc.Union(bMasterInterface=0x00,
                      bSlaveInterface_list=[0x01]),
            standard.EndpointDescriptor(
                bEndpointAddress=0x0 | standard.EndpointDescriptor.DIRECTION_IN,
                bmAttributes=standard.EndpointDescriptor.TYPE_INTERRUPT,
                wMaxPacketSize=0x0040,
                bInterval=0x10)
        ]
    ),
    standard.InterfaceDescriptor(
        bInterfaceClass=cdc.CDC_CLASS_DATA,
        subdescriptors=[
            standard.EndpointDescriptor(
                bEndpointAddress=0x0 | standard.EndpointDescriptor.DIRECTION_IN,
                bmAttributes=standard.EndpointDescriptor.TYPE_BULK),
            standard.EndpointDescriptor(
                bEndpointAddress=0x0 | standard.EndpointDescriptor.DIRECTION_OUT,
                bmAttributes=standard.EndpointDescriptor.TYPE_BULK)
        ]
    )
]

msc_interfaces = [
    standard.InterfaceDescriptor(
        bInterfaceClass=msc.MSC_CLASS,
        bInterfaceSubClass=msc.MSC_SUBCLASS_TRANSPARENT,
        bInterfaceProtocol=msc.MSC_PROTOCOL_BULK,
        subdescriptors=[
            standard.EndpointDescriptor(
                bEndpointAddress=0x0 | standard.EndpointDescriptor.DIRECTION_IN,
                bmAttributes=standard.EndpointDescriptor.TYPE_BULK),
            standard.EndpointDescriptor(
                bEndpointAddress=0x1 | standard.EndpointDescriptor.DIRECTION_OUT,
                bmAttributes=standard.EndpointDescriptor.TYPE_BULK)
        ]
    )
]

hid_report_descriptors = [
    hid.ReportDescriptor.GENERIC_MOUSE_REPORT,
    hid.ReportDescriptor.GENERIC_KEYBOARD_REPORT,
]

mouse_endpoint_descriptor = standard.EndpointDescriptor(
                bEndpointAddress=0x0 | standard.EndpointDescriptor.DIRECTION_IN,
                bmAttributes=standard.EndpointDescriptor.TYPE_INTERRUPT,
                wMaxPacketSize=8,    # mouse report is small
                bInterval=0x0a)

keyboard_endpoint_descriptor = standard.EndpointDescriptor(
                bEndpointAddress=0x0 | standard.EndpointDescriptor.DIRECTION_IN,
                bmAttributes=standard.EndpointDescriptor.TYPE_INTERRUPT,
                wMaxPacketSize=8,    # keyboard report is 8 bytes
                bInterval=0x02)

hid_interfaces = [
    standard.InterfaceDescriptor(
        bInterfaceClass = hid.HID_CLASS,
        bInterfaceSubClass = hid.HID_SUBCLASS_BOOT,
        bInterfaceProtocol = hid.HID_PROTOCOL_MOUSE,
        subdescriptors=[
            mouse_endpoint_descriptor,
            hid.HIDDescriptor(wDescriptorLength=len(bytes(hid_report_descriptors[0]))),
            ]
        ),
    standard.InterfaceDescriptor(
        bInterfaceClass = hid.HID_CLASS,
        bInterfaceSubClass = hid.HID_SUBCLASS_NOBOOT,
        bInterfaceProtocol = hid.HID_PROTOCOL_KEYBOARD,
        subdescriptors=[
            keyboard_endpoint_descriptor,
            hid.HIDDescriptor(wDescriptorLength=len(bytes(hid_report_descriptors[1]))),
            ]
        ),
    ]

# This will renumber the endpoints to make them unique across descriptors.
print(mouse_endpoint_descriptor.bEndpointAddress, keyboard_endpoint_descriptor.bEndpointAddress)
###interfaces = util.join_interfaces(cdc_interfaces, msc_interfaces, hid_interfaces)
interfaces = util.join_interfaces(cdc_interfaces, hid_interfaces)
print(mouse_endpoint_descriptor.bEndpointAddress, keyboard_endpoint_descriptor.bEndpointAddress)


cdc_function = standard.InterfaceAssociationDescriptor(
    bFirstInterface=interfaces.index(cdc_interfaces[0]),
    bInterfaceCount=len(cdc_interfaces),
    bFunctionClass=0x2,  # Communications Device Class
    bFunctionSubClass=0x2,  # Abstract control model
    bFunctionProtocol=0x1)  # Common AT Commands

configuration = standard.ConfigurationDescriptor(
    wTotalLength=(standard.ConfigurationDescriptor.bLength +
                  cdc_function.bLength +
                  sum([len(bytes(x)) for x in interfaces])),
    bNumInterfaces=len(interfaces))

descriptor_list = [device, configuration, cdc_function]
descriptor_list.extend(interfaces)
##descriptor_list.extend(hid_report_descriptors)
descriptor_list.extend(strings)

output_file = args.output_file

output_file.write("""
#include <stdint.h>

#include "tools/autogen_usb_descriptor.h"

uint8_t usb_descriptors[] = {
""")

descriptor_length = 0
serial_number_offset = None
for descriptor in descriptor_list:
    print(str(descriptor))
    print([hex(x) for x in bytes(descriptor)])
    output_file.write("// " + str(descriptor) + "\n")
    b = bytes(descriptor)
    i = 0
    if descriptor == serial_number:
        # Add two for the length and descriptor type bytes.
        serial_number_offset = descriptor_length + 2
    # This prints each subdescriptor on a separate line.
    while i < len(b):
        length = b[i]
        for j in range(length):
            output_file.write("0x{:02x}, ".format(b[i + j]))
        output_file.write("\n")
        i += length
        descriptor_length += length


output_file.write("""
};

""")

output_file.write("""

struct usbd_descriptors descriptor_bounds = {{usb_descriptors, usb_descriptors + sizeof(usb_descriptors)}};
uint8_t* serial_number = usb_descriptors + {SERIAL_NUMBER_OFFSET};
uint8_t serial_number_length = {SERIAL_NUMBER_LENGTH};

uint8_t hid_mouse_endpoint_in = {MOUSE_ENDPOINT_ADDRESS};
uint8_t hid_keyboard_endpoint_in = {KEYBOARD_ENDPOINT_ADDRESS};
""".format(SERIAL_NUMBER_OFFSET=serial_number_offset,
           SERIAL_NUMBER_LENGTH=args.serial_number_length,
           MOUSE_ENDPOINT_ADDRESS=mouse_endpoint_descriptor.bEndpointAddress,
           KEYBOARD_ENDPOINT_ADDRESS=keyboard_endpoint_descriptor.bEndpointAddress
           )
)
