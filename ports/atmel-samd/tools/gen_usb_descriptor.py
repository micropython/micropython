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
parser.add_argument('--output_c_file', type=argparse.FileType('w'), required=True)
parser.add_argument('--output_h_file', type=argparse.FileType('w'), required=True)

args = parser.parse_args()

langid = standard.StringDescriptor("\u0409")
manufacturer = standard.StringDescriptor(args.manufacturer)
product = standard.StringDescriptor(args.product)
serial_number = standard.StringDescriptor("serial number. you should fill in a unique serial number here."[:args.serial_number_length])
strings = [langid, manufacturer, product, serial_number]

# vid = 0x239A
# pid = 0x8021

device = standard.DeviceDescriptor(
    description="top",
    idVendor=args.vid,
    idProduct=args.pid,
    iManufacturer=strings.index(manufacturer),
    iProduct=strings.index(product),
    iSerialNumber=strings.index(serial_number))

# Interface numbers are interface set local and endpoints are interface local
# until core.join_interfaces renumbers them.
cdc_interfaces = [
    standard.InterfaceDescriptor(
        description="CDC comm",
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
            cdc.Header(
                description="CDC comm",
                bcdCDC=0x0110),
            cdc.CallManagement(
                description="CDC comm",
                bmCapabilities=0x03,
                bDataInterface=0x01),
            cdc.AbstractControlManagement(
                description="CDC comm",
                bmCapabilities=0x02),
            cdc.Union(
                description="CDC comm",
                bMasterInterface=0x00,
                bSlaveInterface_list=[0x01]),
            standard.EndpointDescriptor(
                description="CDC comm in",
                bEndpointAddress=0x0 | standard.EndpointDescriptor.DIRECTION_IN,
                bmAttributes=standard.EndpointDescriptor.TYPE_INTERRUPT,
                wMaxPacketSize=0x0040,
                bInterval=0x10)
        ]
    ),
    standard.InterfaceDescriptor(
        description="CDC data",
        bInterfaceClass=cdc.CDC_CLASS_DATA,
        subdescriptors=[
            standard.EndpointDescriptor(
                description="CDC data in",
                bEndpointAddress=0x0 | standard.EndpointDescriptor.DIRECTION_IN,
                bmAttributes=standard.EndpointDescriptor.TYPE_BULK),
            standard.EndpointDescriptor(
                description="CDC data out",
                bEndpointAddress=0x0 | standard.EndpointDescriptor.DIRECTION_OUT,
                bmAttributes=standard.EndpointDescriptor.TYPE_BULK)
        ]
    )
]

msc_interfaces = [
    standard.InterfaceDescriptor(
        description="MSC",
        bInterfaceClass=msc.MSC_CLASS,
        bInterfaceSubClass=msc.MSC_SUBCLASS_TRANSPARENT,
        bInterfaceProtocol=msc.MSC_PROTOCOL_BULK,
        subdescriptors=[
            standard.EndpointDescriptor(
                description="MSC in",
                bEndpointAddress=0x0 | standard.EndpointDescriptor.DIRECTION_IN,
                bmAttributes=standard.EndpointDescriptor.TYPE_BULK),
            standard.EndpointDescriptor(
                description="MSC out",
                bEndpointAddress=0x1 | standard.EndpointDescriptor.DIRECTION_OUT,
                bmAttributes=standard.EndpointDescriptor.TYPE_BULK)
        ]
    )
]

hid_report_descriptor = hid.ReportDescriptor.MOUSE_KEYBOARD_CONSUMER_SYS_CONTROL_REPORT
hid_report_ids = hid.ReportDescriptor.REPORT_IDS
hid_report_lengths = hid.ReportDescriptor.REPORT_LENGTHS
hid_max_report_length = max(hid_report_lengths.values())

# ASF4 expects keyboard and generic devices to have both in and out endpoints,
# and will fail (possibly silently) if both are not supplied.
hid_endpoint_in_descriptor = standard.EndpointDescriptor(
    description="HID in",
    bEndpointAddress=0x0 | standard.EndpointDescriptor.DIRECTION_IN,
    bmAttributes=standard.EndpointDescriptor.TYPE_INTERRUPT,
    wMaxPacketSize=hid_max_report_length + 1, # +1 for the Report ID
    bInterval=0x02)

hid_endpoint_out_descriptor = standard.EndpointDescriptor(
    description="HID out",
    bEndpointAddress=0x0 | standard.EndpointDescriptor.DIRECTION_OUT,
    bmAttributes=standard.EndpointDescriptor.TYPE_INTERRUPT)

hid_interfaces = [
    standard.InterfaceDescriptor(
        description="HID Keyboard",
        bInterfaceClass=hid.HID_CLASS,
        bInterfaceSubClass=hid.HID_SUBCLASS_NOBOOT,
        bInterfaceProtocol=hid.HID_PROTOCOL_KEYBOARD,
        subdescriptors=[
            hid.HIDDescriptor(wDescriptorLength=len(bytes(hid_report_descriptor))),
            hid_endpoint_in_descriptor,
            hid_endpoint_out_descriptor,
            ]
        ),
    ]

# This will renumber the endpoints to make them unique across descriptors.
interfaces = util.join_interfaces(cdc_interfaces, msc_interfaces, hid_interfaces)
#interfaces = util.join_interfaces(cdc_interfaces, hid_interfaces, msc_interfaces)
#interfaces = util.join_interfaces(cdc_interfaces, hid_interfaces)

cdc_function = standard.InterfaceAssociationDescriptor(
    description="CDC function",
    bFirstInterface=interfaces.index(cdc_interfaces[0]),
    bInterfaceCount=len(cdc_interfaces),
    bFunctionClass=0x2,  # Communications Device Class
    bFunctionSubClass=0x2,  # Abstract control model
    bFunctionProtocol=0x1)  # Common AT Commands

configuration = standard.ConfigurationDescriptor(
    description="Composite configuration",
    wTotalLength=(standard.ConfigurationDescriptor.bLength +
                  cdc_function.bLength +
                  sum([len(bytes(x)) for x in interfaces])),
    bNumInterfaces=len(interfaces))

descriptor_list = [device, configuration, cdc_function]
descriptor_list.extend(interfaces)
descriptor_list.extend(strings)

c_file = args.output_c_file
h_file = args.output_h_file


c_file.write("""\
#include <stdint.h>

#include "{H_FILE_NAME}"

#include "usb/device/usbdc.h"

""".format(H_FILE_NAME=h_file.name))

c_file.write("""\
uint8_t usb_descriptors[] = {
""")

# Write out all the regular descriptors as one long array (that's how ASF4 does it).
descriptor_length = 0
serial_number_offset = None
for descriptor in descriptor_list:
    c_file.write("""\
// {DESCRIPTION} : {CLASS}
""".format(DESCRIPTION=descriptor.description,
               CLASS=descriptor.__class__))

    b = bytes(descriptor)
    i = 0

    if descriptor == serial_number:
        # Add two for bLength and bDescriptorType.
        serial_number_offset = descriptor_length + 2

    # This prints each subdescriptor on a separate line.
    while i < len(b):
        length = b[i]
        for j in range(length):
            c_file.write("0x{:02x}, ".format(b[i + j]))
        c_file.write("\n")
        i += length
        descriptor_length += length


c_file.write("""\
};
""")

# Now we values we need for the .h file.
h_file.write("""\
#ifndef MICROPY_INCLUDED_AUTOGEN_USB_DESCRIPTOR_H
#define MICROPY_INCLUDED_AUTOGEN_USB_DESCRIPTOR_H

#define SERIAL_NUMBER_OFFSET {SERIAL_NUMBER_OFFSET}
#define SERIAL_NUMBER_LENGTH {SERIAL_NUMBER_LENGTH}
uint8_t* serial_number;

uint8_t hid_report_descriptor[{HID_REPORT_DESCRIPTOR_LENGTH}];
#define USB_HID_ENDPOINT_IN {HID_ENDPOINT_IN_ADDRESS}
#define USB_HID_ENDPOINT_OUT {HID_ENDPOINT_OUT_ADDRESS}

"""
.format(SERIAL_NUMBER_OFFSET=serial_number_offset,
        SERIAL_NUMBER_LENGTH=args.serial_number_length,
        HID_REPORT_DESCRIPTOR_LENGTH=len(bytes(hid_report_descriptor)),
        HID_ENDPOINT_IN_ADDRESS=hex(hid_endpoint_in_descriptor.bEndpointAddress),
        HID_ENDPOINT_OUT_ADDRESS=hex(hid_endpoint_out_descriptor.bEndpointAddress)))

# Write out #define's that declare which endpoints are in use.
# These provide information for declaring cache sizes and perhaps other things at compile time
for interface in interfaces:
    for subdescriptor in interface.subdescriptors:
        if isinstance(subdescriptor, standard.EndpointDescriptor):
            endpoint_num = subdescriptor.bEndpointAddress & standard.EndpointDescriptor.NUMBER_MASK
            endpoint_in = ((subdescriptor.bEndpointAddress & standard.EndpointDescriptor.DIRECTION_MASK) ==
                           standard.EndpointDescriptor.DIRECTION_IN)
            h_file.write("""\
#define USB_ENDPOINT_{NUMBER}_{DIRECTION}_USED 1
""".format(NUMBER=endpoint_num,
           DIRECTION="IN" if endpoint_in else "OUT"))

h_file.write("\n")

# #define the report ID's used in the combined HID descriptor
for name, id in hid_report_ids.items():
    h_file.write("""\
#define USB_HID_REPORT_ID_{NAME} {ID}
""".format(NAME=name,
           ID = id))

h_file.write("\n")

# #define the report sizes used in the combined HID descriptor
for name, length in hid_report_lengths.items():
    h_file.write("""\
#define USB_HID_REPORT_LENGTH_{NAME} {LENGTH}
""".format(NAME=name,
           LENGTH=length))

h_file.write("\n")

h_file.write("""\
#define USB_HID_NUM_DEVICES {NUM_DEVICES}
#define USB_HID_MAX_REPORT_LENGTH {MAX_LENGTH}
""".format(NUM_DEVICES=len(hid_report_lengths),
           MAX_LENGTH=hid_max_report_length))



# Write out the report descriptor and info
c_file.write("""\
uint8_t hid_report_descriptor[{HID_DESCRIPTOR_LENGTH}] = {{
""".format(HID_DESCRIPTOR_LENGTH=len(bytes(hid_report_descriptor))))

for b in bytes(hid_report_descriptor):
    c_file.write("0x{:02x}, ".format(b))
c_file.write("""
};
""")

c_file.write("""\

struct usbd_descriptors descriptor_bounds = {{usb_descriptors, usb_descriptors + sizeof(usb_descriptors)}};
uint8_t* serial_number = usb_descriptors + {SERIAL_NUMBER_OFFSET};
""".format(SERIAL_NUMBER_OFFSET=serial_number_offset))

h_file.write("""\
#endif // MICROPY_INCLUDED_AUTOGEN_USB_DESCRIPTOR_H
""")
