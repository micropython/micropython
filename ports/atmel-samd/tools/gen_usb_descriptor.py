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

class StringIndex:
    """Assign a monotonically increasing index to each unique string. Start with 0."""
    string_to_index = {}
    strings = []

    @classmethod
    def index(cls, string):
        if string in cls.string_to_index:
            return cls.string_to_index[string]
        else:
            idx = len(cls.strings)
            cls.string_to_index[string] = idx
            cls.strings.append(string)
            return idx

    @classmethod
    def strings_in_order(cls):
        return cls.strings



# langid must be the 0th string descriptor
LANGID_INDEX = StringIndex.index("\u0409")
assert LANGID_INDEX == 0
SERIAL_NUMBER_INDEX = StringIndex.index("S" * args.serial_number_length)

device = standard.DeviceDescriptor(
    description="top",
    idVendor=args.vid,
    idProduct=args.pid,
    iManufacturer=StringIndex.index(args.manufacturer),
    iProduct=StringIndex.index(args.product),
    iSerialNumber=SERIAL_NUMBER_INDEX)

# Interface numbers are interface-set local and endpoints are interface local
# until util.join_interfaces renumbers them.

cdc_union = cdc.Union(
    description="CDC comm",
    bMasterInterface=0x00,       # Adjust this after interfaces are renumbered.
    bSlaveInterface_list=[0x01]) # Adjust this after interfaces are renumbered.

cdc_call_management = cdc.CallManagement(
    description="CDC comm",
    bmCapabilities=0x01,
    bDataInterface=0x01)         # Adjust this after interfaces are renumbered.

cdc_comm_interface = standard.InterfaceDescriptor(
    description="CDC comm",
    bInterfaceClass=cdc.CDC_CLASS_COMM,  # Communications Device Class
    bInterfaceSubClass=cdc.CDC_SUBCLASS_ACM,  # Abstract control model
    bInterfaceProtocol=cdc.CDC_PROTOCOL_NONE,
    iInterface=StringIndex.index("CircuitPython CDC control"),
    subdescriptors=[
        cdc.Header(
            description="CDC comm",
            bcdCDC=0x0110),
        cdc_call_management,
        cdc.AbstractControlManagement(
            description="CDC comm",
            bmCapabilities=0x02),
        cdc_union,
        standard.EndpointDescriptor(
            description="CDC comm in",
            bEndpointAddress=0x0 | standard.EndpointDescriptor.DIRECTION_IN,
            bmAttributes=standard.EndpointDescriptor.TYPE_INTERRUPT,
            wMaxPacketSize=0x0040,
            bInterval=0x10)
    ])

cdc_data_interface = standard.InterfaceDescriptor(
    description="CDC data",
    bInterfaceClass=cdc.CDC_CLASS_DATA,
    iInterface=StringIndex.index("CircuitPython CDC data"),
    subdescriptors=[
        standard.EndpointDescriptor(
            description="CDC data out",
            bEndpointAddress=0x0 | standard.EndpointDescriptor.DIRECTION_OUT,
            bmAttributes=standard.EndpointDescriptor.TYPE_BULK),
        standard.EndpointDescriptor(
            description="CDC data in",
            bEndpointAddress=0x0 | standard.EndpointDescriptor.DIRECTION_IN,
            bmAttributes=standard.EndpointDescriptor.TYPE_BULK),
    ])

cdc_interfaces = [cdc_comm_interface, cdc_data_interface]

msc_interfaces = [
    standard.InterfaceDescriptor(
        description="MSC",
        bInterfaceClass=msc.MSC_CLASS,
        bInterfaceSubClass=msc.MSC_SUBCLASS_TRANSPARENT,
        bInterfaceProtocol=msc.MSC_PROTOCOL_BULK,
        iInterface=StringIndex.index("CircuitPython Mass Storage"),
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
    bInterval=0x02)

hid_endpoint_out_descriptor = standard.EndpointDescriptor(
    description="HID out",
    bEndpointAddress=0x0 | standard.EndpointDescriptor.DIRECTION_OUT,
    bmAttributes=standard.EndpointDescriptor.TYPE_INTERRUPT)

hid_interfaces = [
    standard.InterfaceDescriptor(
        description="HID Multiple Devices",
        bInterfaceClass=hid.HID_CLASS,
        bInterfaceSubClass=hid.HID_SUBCLASS_NOBOOT,
        bInterfaceProtocol=hid.HID_PROTOCOL_NONE,
        iInterface=StringIndex.index("CircuitPython HID"),
        subdescriptors=[
            hid.HIDDescriptor(
                description="HID",
                wDescriptorLength=len(bytes(hid_report_descriptor))),
            hid_endpoint_in_descriptor,
            hid_endpoint_out_descriptor,
            ]
        ),
    ]

# This will renumber the endpoints to make them unique across descriptors,
# and renumber the interfaces in order. But we still need to fix up certain
# interface cross-references.
interfaces = util.join_interfaces(cdc_interfaces, msc_interfaces, hid_interfaces)

# Now adjust the CDC interface cross-references.

cdc_union.bMasterInterface = cdc_comm_interface.bInterfaceNumber
cdc_union.bSlaveInterface_list = [cdc_data_interface.bInterfaceNumber]

cdc_call_management.bDataInterface = cdc_data_interface.bInterfaceNumber

cdc_iad = standard.InterfaceAssociationDescriptor(
    description="CDC IAD",
    bFirstInterface=cdc_comm_interface.bInterfaceNumber,
    bInterfaceCount=len(cdc_interfaces),
    bFunctionClass=0x2,  # Communications Device Class
    bFunctionSubClass=0x2,  # Abstract control model
    bFunctionProtocol=0x1)

configuration = standard.ConfigurationDescriptor(
    description="Composite configuration",
    wTotalLength=(standard.ConfigurationDescriptor.bLength +
                  cdc_iad.bLength +
                  sum([len(bytes(x)) for x in interfaces])),
    bNumInterfaces=len(interfaces))

descriptor_list = []
descriptor_list.append(device)
descriptor_list.append(configuration)
descriptor_list.append(cdc_iad)
descriptor_list.extend(cdc_interfaces)
descriptor_list.extend(msc_interfaces)
# Put the CDC IAD just before the CDC interfaces.
# There appears to be a bug in the Windows composite USB driver that requests the
# HID report descriptor with the wrong interface number if the HID interface is not given
# first. However, it still fetches the descriptor anyway. We could reorder the interfaces but
# the Windows 7 Adafruit_usbser.inf file thinks CDC is at Interface 0, so we'll leave it
# there for backwards compatibility.
descriptor_list.extend(hid_interfaces)

string_descriptors = [standard.StringDescriptor(string) for string in StringIndex.strings_in_order()]
serial_number_descriptor = string_descriptors[SERIAL_NUMBER_INDEX]
descriptor_list.extend(string_descriptors)

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

    if descriptor == serial_number_descriptor:
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
