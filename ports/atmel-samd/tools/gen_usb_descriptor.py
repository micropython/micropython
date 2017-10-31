import argparse

import os
import sys

# path hacking
sys.path.append("../../tools")

from usb_descriptor import core
from usb_descriptor import cdc

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

langid = core.StringDescriptor("\u0409")
manufacturer = core.StringDescriptor(args.manufacturer)
product = core.StringDescriptor(args.product)
serial_number = core.StringDescriptor("serial number. you should fill in a unique serial number here."[:args.serial_number_length])
strings = [langid, manufacturer, product, serial_number]

# vid = 0x239A
# pid = 0x8021

device = core.DeviceDescriptor(
    idVendor=args.vid,
    idProduct=args.pid,
    iManufacturer=strings.index(manufacturer),
    iProduct=strings.index(product),
    iSerialNumber=strings.index(serial_number))

# Interface numbers are interface set local and endpoints are interface local
# until core.join_interfaces renumbers them.
cdc_interfaces = [
    core.InterfaceDescriptor(
        bInterfaceClass=0x2,  # Communications Device Class
        bInterfaceSubClass=0x02,  # Abstract control model
        bInterfaceProtocol=0x01,  # Common AT Commands
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
                      bSlaveInterface=[0x01]),
            core.EndpointDescriptor(
                bEndpointAddress=0x0 | core.EndpointDescriptor.DIRECTION_IN,
                bmAttributes=core.EndpointDescriptor.TYPE_INTERRUPT,
                wMaxPacketSize=0x8,
                bInterval=10)
        ]
    ),
    core.InterfaceDescriptor(
        bInterfaceClass=0x0a,
        subdescriptors=[
            core.EndpointDescriptor(
                bEndpointAddress=0x0 | core.EndpointDescriptor.DIRECTION_IN,
                bmAttributes=core.EndpointDescriptor.TYPE_BULK),
            core.EndpointDescriptor(
                bEndpointAddress=0x0 | core.EndpointDescriptor.DIRECTION_OUT,
                bmAttributes=core.EndpointDescriptor.TYPE_BULK)
        ]
    )
]

msc_interfaces = [
    core.InterfaceDescriptor(
        bInterfaceClass=0x08,
        bInterfaceSubClass=0x06,
        bInterfaceProtocol=0x50,
        subdescriptors=[
            core.EndpointDescriptor(
                bEndpointAddress=0x0 | core.EndpointDescriptor.DIRECTION_IN,
                bmAttributes=core.EndpointDescriptor.TYPE_BULK),
            core.EndpointDescriptor(
                bEndpointAddress=0x1 | core.EndpointDescriptor.DIRECTION_OUT,
                bmAttributes=core.EndpointDescriptor.TYPE_BULK)
        ]
    )
]

interfaces = core.join_interfaces(cdc_interfaces, msc_interfaces)

cdc_function = core.InterfaceAssociationDescriptor(
    bFirstInterface=interfaces.index(cdc_interfaces[0]),
    bInterfaceCount=len(cdc_interfaces),
    bFunctionClass=0x2,  # Communications Device Class
    bFunctionSubClass=0x2,  # Abstract control model
    bFunctionProtocol=0x1)  # Common AT Commands

configuration = core.ConfigurationDescriptor(
    wTotalLength=(core.ConfigurationDescriptor.bLength +
                  cdc_function.bLength +
                  sum([len(bytes(x)) for x in interfaces])),
    bNumInterfaces=len(interfaces))

descriptor_list = [device, configuration, cdc_function]
descriptor_list.extend(interfaces)
descriptor_list.extend(strings)

output_file = args.output_file

output_file.write("#include <stdint.h>\n\n")
output_file.write("#include \"tools/autogen_usb_descriptor.h\"\n\n")
output_file.write("uint8_t usb_descriptors[] = {\n")
descriptor_length = 0
serial_number_offset = None
for descriptor in descriptor_list:
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

    output_file.write("\n")
output_file.write("};\n\n")
output_file.write("struct usbd_descriptors descriptor_bounds = " +
                  "{usb_descriptors," +
                  " usb_descriptors + sizeof(usb_descriptors)};\n")
output_file.write("uint8_t* serial_number = usb_descriptors + " +
                  str(serial_number_offset) + ";\n")
output_file.write("uint8_t serial_number_length = " +
                  str(args.serial_number_length) + ";\n")

output_file.close()
