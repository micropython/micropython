# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

def fix_note(note):
    index = note.rfind(" object at")
    if index >= 0:
        note = note[:index] + ">"
    return note

def fix_notes(notes):
    count = len(notes)
    index = 0
    while index < count:
        notes[index] = fix_note(notes[index])
        index += 1

    return notes

import argparse

import os
import sys

sys.path.append("../../tools/usb_descriptor")

from adafruit_usb_descriptor import audio, audio10, cdc, hid, midi, msc, standard, util
import hid_report_descriptors

DEFAULT_INTERFACE_NAME = 'CircuitPython'
ALL_DEVICES='CDC,MSC,AUDIO,HID,VENDOR'
ALL_DEVICES_SET=frozenset(ALL_DEVICES.split(','))
DEFAULT_DEVICES='CDC,MSC,AUDIO,HID'

ALL_HID_DEVICES='KEYBOARD,MOUSE,CONSUMER,SYS_CONTROL,GAMEPAD,DIGITIZER,XAC_COMPATIBLE_GAMEPAD,RAW'
ALL_HID_DEVICES_SET=frozenset(ALL_HID_DEVICES.split(','))
# Digitizer works on Linux but conflicts with mouse, so omit it.
DEFAULT_HID_DEVICES='KEYBOARD,MOUSE,CONSUMER,GAMEPAD'

parser = argparse.ArgumentParser(description='Generate USB descriptors.')
parser.add_argument('--highspeed', default=False, action='store_true',
                    help='descriptor for highspeed device')
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
parser.add_argument('--devices', type=lambda l: tuple(l.split(',')), default=DEFAULT_DEVICES,
                    help='devices to include in descriptor (AUDIO includes MIDI support)')
parser.add_argument('--hid_devices', type=lambda l: tuple(l.split(',')), default=DEFAULT_HID_DEVICES,
                    help='HID devices to include in HID report descriptor')
parser.add_argument('--interface_name', type=str,
                    help='The name/prefix to use in the interface descriptions',
                    default=DEFAULT_INTERFACE_NAME)
parser.add_argument('--no-renumber_endpoints', dest='renumber_endpoints', action='store_false',
                    help='use to not renumber endpoint')
parser.add_argument('--cdc_ep_num_notification', type=int, default=0,
                    help='endpoint number of CDC NOTIFICATION')
parser.add_argument('--cdc_ep_num_data_out', type=int, default=0,
                    help='endpoint number of CDC DATA OUT')
parser.add_argument('--cdc_ep_num_data_in', type=int, default=0,
                    help='endpoint number of CDC DATA IN')
parser.add_argument('--msc_ep_num_out', type=int, default=0,
                    help='endpoint number of MSC OUT')
parser.add_argument('--msc_ep_num_in', type=int, default=0,
                    help='endpoint number of MSC IN')
parser.add_argument('--hid_ep_num_out', type=int, default=0,
                    help='endpoint number of HID OUT')
parser.add_argument('--hid_ep_num_in', type=int, default=0,
                    help='endpoint number of HID IN')
parser.add_argument('--midi_ep_num_out', type=int, default=0,
                    help='endpoint number of MIDI OUT')
parser.add_argument('--midi_ep_num_in', type=int, default=0,
                    help='endpoint number of MIDI IN')
parser.add_argument('--vendor_ep_num_out', type=int, default=0,
                    help='endpoint number of VENDOR OUT')
parser.add_argument('--vendor_ep_num_in', type=int, default=0,
                    help='endpoint number of VENDOR IN')
parser.add_argument('--max_ep', type=int, default=0,
                    help='total number of endpoints available')
parser.add_argument('--output_c_file', type=argparse.FileType('w', encoding='UTF-8'), required=True)
parser.add_argument('--output_h_file', type=argparse.FileType('w', encoding='UTF-8'), required=True)

args = parser.parse_args()

unknown_devices = list(frozenset(args.devices) - ALL_DEVICES_SET)
if unknown_devices:
    raise ValueError("Unknown device(s)", unknown_devices)

unknown_hid_devices = list(frozenset(args.hid_devices) - ALL_HID_DEVICES_SET)
if unknown_hid_devices:
    raise ValueError("Unknown HID devices(s)", unknown_hid_devices)

if not args.renumber_endpoints:
    if 'CDC' in args.devices:
        if args.cdc_ep_num_notification == 0:
            raise ValueError("CDC notification endpoint number must not be 0")
        elif args.cdc_ep_num_data_out == 0:
            raise ValueError("CDC data OUT endpoint number must not be 0")
        elif args.cdc_ep_num_data_in == 0:
            raise ValueError("CDC data IN endpoint number must not be 0")

    if 'MSC' in args.devices:
        if args.msc_ep_num_out == 0:
            raise ValueError("MSC endpoint OUT number must not be 0")
        elif args.msc_ep_num_in == 0:
            raise ValueError("MSC endpoint IN number must not be 0")

    if 'HID' in args.devices:
        if args.args.hid_ep_num_out == 0:
            raise ValueError("HID endpoint OUT number must not be 0")
        elif args.hid_ep_num_in == 0:
            raise ValueError("HID endpoint IN number must not be 0")

    if 'AUDIO' in args.devices:
        if args.args.midi_ep_num_out == 0:
            raise ValueError("MIDI endpoint OUT number must not be 0")
        elif args.midi_ep_num_in == 0:
            raise ValueError("MIDI endpoint IN number must not be 0")

    if 'VENDOR' in args.devices:
        if args.vendor_ep_num_out == 0:
            raise ValueError("VENDOR endpoint OUT number must not be 0")
        elif args.vendor_ep_num_in == 0:
            raise ValueError("VENDOR endpoint IN number must not be 0")

class StringIndex:
    """Assign a monotonically increasing index to each unique string. Start with 0."""
    string_to_index = {}
    index_to_variable = {}
    strings = []

    @classmethod
    def index(cls, string, *, variable_name = None):
        if string in cls.string_to_index:
            idx = cls.string_to_index[string]
            if not cls.index_to_variable[idx]:
                cls.index_to_variable[idx] = variable_name
            return idx
        else:
            idx = len(cls.strings)
            cls.string_to_index[string] = idx
            cls.strings.append(string)
            cls.index_to_variable[idx] = variable_name
            return idx

    @classmethod
    def strings_in_order(cls):
        return cls.strings



# langid must be the 0th string descriptor
LANGID_INDEX = StringIndex.index("\u0409", variable_name="language_id")
assert LANGID_INDEX == 0
SERIAL_NUMBER_INDEX = StringIndex.index("S" * args.serial_number_length, variable_name="usb_serial_number")

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
    iInterface=StringIndex.index("{} CDC control".format(args.interface_name)),
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
            bEndpointAddress=args.cdc_ep_num_notification | standard.EndpointDescriptor.DIRECTION_IN,
            bmAttributes=standard.EndpointDescriptor.TYPE_INTERRUPT,
            wMaxPacketSize=0x0040,
            bInterval=0x10)
    ])

cdc_data_interface = standard.InterfaceDescriptor(
    description="CDC data",
    bInterfaceClass=cdc.CDC_CLASS_DATA,
    iInterface=StringIndex.index("{} CDC data".format(args.interface_name)),
    subdescriptors=[
        standard.EndpointDescriptor(
            description="CDC data out",
            bEndpointAddress=args.cdc_ep_num_data_out | standard.EndpointDescriptor.DIRECTION_OUT,
            bmAttributes=standard.EndpointDescriptor.TYPE_BULK,
            bInterval=0,
            wMaxPacketSize=512 if args.highspeed else 64),
        standard.EndpointDescriptor(
            description="CDC data in",
            bEndpointAddress=args.cdc_ep_num_data_in | standard.EndpointDescriptor.DIRECTION_IN,
            bmAttributes=standard.EndpointDescriptor.TYPE_BULK,
            bInterval=0,
            wMaxPacketSize=512 if args.highspeed else 64),
    ])

cdc_interfaces = [cdc_comm_interface, cdc_data_interface]

msc_interfaces = [
    standard.InterfaceDescriptor(
        description="MSC",
        bInterfaceClass=msc.MSC_CLASS,
        bInterfaceSubClass=msc.MSC_SUBCLASS_TRANSPARENT,
        bInterfaceProtocol=msc.MSC_PROTOCOL_BULK,
        iInterface=StringIndex.index("{} Mass Storage".format(args.interface_name)),
        subdescriptors=[
            standard.EndpointDescriptor(
                description="MSC in",
                bEndpointAddress=args.msc_ep_num_in | standard.EndpointDescriptor.DIRECTION_IN,
                bmAttributes=standard.EndpointDescriptor.TYPE_BULK,
                bInterval=0,
                wMaxPacketSize=512 if args.highspeed else 64),
            standard.EndpointDescriptor(
                description="MSC out",
                bEndpointAddress=(args.msc_ep_num_out | standard.EndpointDescriptor.DIRECTION_OUT),
                bmAttributes=standard.EndpointDescriptor.TYPE_BULK,
                bInterval=0,
                wMaxPacketSize=512 if args.highspeed else 64),
        ]
    )
]

# When there's only one hid_device, it shouldn't have a report id.
# Otherwise, report ids are assigned sequentially:
# args.hid_devices[0] has report_id 1
# args.hid_devices[1] has report_id 2
# etc.

report_ids = {}

if len(args.hid_devices) == 1:
    name = args.hid_devices[0]
    combined_hid_report_descriptor = hid.ReportDescriptor(
        description=name,
        report_descriptor=bytes(hid_report_descriptors.REPORT_DESCRIPTOR_FUNCTIONS[name](0)))
    report_ids[name] = 0
else:
    report_id = 1
    concatenated_descriptors = bytearray()
    for name in args.hid_devices:
        concatenated_descriptors.extend(
            bytes(hid_report_descriptors.REPORT_DESCRIPTOR_FUNCTIONS[name](report_id)))
        report_ids[name] = report_id
        report_id += 1
    combined_hid_report_descriptor = hid.ReportDescriptor(
        description="MULTIDEVICE",
        report_descriptor=bytes(concatenated_descriptors))

# ASF4 expects keyboard and generic devices to have both in and out endpoints,
# and will fail (possibly silently) if both are not supplied.
hid_endpoint_in_descriptor = standard.EndpointDescriptor(
    description="HID in",
    bEndpointAddress=args.hid_ep_num_in | standard.EndpointDescriptor.DIRECTION_IN,
    bmAttributes=standard.EndpointDescriptor.TYPE_INTERRUPT,
    bInterval=8)

hid_endpoint_out_descriptor = standard.EndpointDescriptor(
    description="HID out",
    bEndpointAddress=args.hid_ep_num_out | standard.EndpointDescriptor.DIRECTION_OUT,
    bmAttributes=standard.EndpointDescriptor.TYPE_INTERRUPT,
    bInterval=8)

hid_interfaces = [
    standard.InterfaceDescriptor(
        description="HID Multiple Devices",
        bInterfaceClass=hid.HID_CLASS,
        bInterfaceSubClass=hid.HID_SUBCLASS_NOBOOT,
        bInterfaceProtocol=hid.HID_PROTOCOL_NONE,
        iInterface=StringIndex.index("{} HID".format(args.interface_name)),
        subdescriptors=[
            hid.HIDDescriptor(
                description="HID",
                wDescriptorLength=len(bytes(combined_hid_report_descriptor))),
            hid_endpoint_in_descriptor,
            hid_endpoint_out_descriptor,
            ]
        ),
    ]

# Audio!
# In and out here are relative to CircuitPython

# USB OUT -> midi_in_jack_emb -> midi_out_jack_ext -> CircuitPython
midi_in_jack_emb = midi.InJackDescriptor(
    description="MIDI PC -> {}".format(args.interface_name),
    bJackType=midi.JACK_TYPE_EMBEDDED,
    iJack=StringIndex.index("{} usb_midi.ports[0]".format(args.interface_name)))
midi_out_jack_ext = midi.OutJackDescriptor(
                    description="MIDI data out to user code.",
                    bJackType=midi.JACK_TYPE_EXTERNAL,
                    input_pins=[(midi_in_jack_emb, 1)],
                    iJack=0)

# USB IN <- midi_out_jack_emb <- midi_in_jack_ext <- CircuitPython
midi_in_jack_ext = midi.InJackDescriptor(
                    description="MIDI data in from user code.",
                    bJackType=midi.JACK_TYPE_EXTERNAL,
                    iJack=0)
midi_out_jack_emb = midi.OutJackDescriptor(
    description="MIDI PC <- {}".format(args.interface_name),
    bJackType=midi.JACK_TYPE_EMBEDDED,
    input_pins=[(midi_in_jack_ext, 1)],
    iJack=StringIndex.index("{} usb_midi.ports[1]".format(args.interface_name)))


audio_midi_interface = standard.InterfaceDescriptor(
    description="Midi goodness",
    bInterfaceClass=audio.AUDIO_CLASS_DEVICE,
    bInterfaceSubClass=audio.AUDIO_SUBCLASS_MIDI_STREAMING,
    bInterfaceProtocol=audio.AUDIO_PROTOCOL_V1,
    iInterface=StringIndex.index("{} MIDI".format(args.interface_name)),
    subdescriptors=[
        midi.Header(
            jacks_and_elements=[
                midi_in_jack_emb,
                midi_in_jack_ext,
                midi_out_jack_emb,
                midi_out_jack_ext
            ],
        ),
        standard.EndpointDescriptor(
            description="MIDI data out to {}".format(args.interface_name),
            bEndpointAddress=args.midi_ep_num_out | standard.EndpointDescriptor.DIRECTION_OUT,
            bmAttributes=standard.EndpointDescriptor.TYPE_BULK,
            bInterval=0,
            wMaxPacketSize=512 if args.highspeed else 64),
        midi.DataEndpointDescriptor(baAssocJack=[midi_in_jack_emb]),
        standard.EndpointDescriptor(
            description="MIDI data in from {}".format(args.interface_name),
            bEndpointAddress=args.midi_ep_num_in | standard.EndpointDescriptor.DIRECTION_IN,
            bmAttributes=standard.EndpointDescriptor.TYPE_BULK,
            bInterval = 0x0,
            wMaxPacketSize=512 if args.highspeed else 64),
        midi.DataEndpointDescriptor(baAssocJack=[midi_out_jack_emb]),
    ])

cs_ac_interface = audio10.AudioControlInterface(
        description="Empty audio control",
        audio_streaming_interfaces = [],
        midi_streaming_interfaces = [
            audio_midi_interface
        ]
    )

audio_control_interface = standard.InterfaceDescriptor(
        description="All the audio",
        bInterfaceClass=audio.AUDIO_CLASS_DEVICE,
        bInterfaceSubClass=audio.AUDIO_SUBCLASS_CONTROL,
        bInterfaceProtocol=audio.AUDIO_PROTOCOL_V1,
        iInterface=StringIndex.index("{} Audio".format(args.interface_name)),
        subdescriptors=[
            cs_ac_interface,
        ])

# Audio streaming interfaces must occur before MIDI ones.
audio_interfaces = [audio_control_interface] + cs_ac_interface.audio_streaming_interfaces + cs_ac_interface.midi_streaming_interfaces

# TODO New code goes here to create vendor objects

vendor_interfaces = [] # TODO Fix this!

interfaces_to_join = []

if 'CDC' in args.devices:
    interfaces_to_join.append(cdc_interfaces)

if 'MSC' in args.devices:
    interfaces_to_join.append(msc_interfaces)

if 'HID' in args.devices:
    interfaces_to_join.append(hid_interfaces)

if 'AUDIO' in args.devices:
    interfaces_to_join.append(audio_interfaces)

if 'VENDOR' in args.devices:
    interfaces_to_join.append(vendor_interfaces)

# util.join_interfaces() will renumber the endpoints to make them unique across descriptors,
# and renumber the interfaces in order. But we still need to fix up certain
# interface cross-references.
interfaces = util.join_interfaces(interfaces_to_join, renumber_endpoints=args.renumber_endpoints)

if args.max_ep != 0:
    for interface in interfaces:
        for subdescriptor in interface.subdescriptors:
            endpoint_address = getattr(subdescriptor, 'bEndpointAddress', 0) & 0x7f
            if endpoint_address >= args.max_ep:
                raise ValueError("Endpoint address %d of %s must be less than %d" % (endpoint_address & 0x7f, interface.description, args.max_ep))
else:
    print("Unable to check whether maximum number of endpoints is respected", file=sys.stderr)

# Now adjust the CDC interface cross-references.

cdc_union.bMasterInterface = cdc_comm_interface.bInterfaceNumber
cdc_union.bSlaveInterface_list = [cdc_data_interface.bInterfaceNumber]

cdc_call_management.bDataInterface = cdc_data_interface.bInterfaceNumber

cdc_iad = standard.InterfaceAssociationDescriptor(
    description="CDC IAD",
    bFirstInterface=cdc_comm_interface.bInterfaceNumber,
    bInterfaceCount=len(cdc_interfaces),
    bFunctionClass=cdc.CDC_CLASS_COMM,  # Communications Device Class
    bFunctionSubClass=cdc.CDC_SUBCLASS_ACM,  # Abstract control model
    bFunctionProtocol=cdc.CDC_PROTOCOL_NONE)

descriptor_list = []

if 'CDC' in args.devices:
    # Put the CDC IAD just before the CDC interfaces.
    # There appears to be a bug in the Windows composite USB driver that requests the
    # HID report descriptor with the wrong interface number if the HID interface is not given
    # first. However, it still fetches the descriptor anyway. We could reorder the interfaces but
    # the Windows 7 Adafruit_usbser.inf file thinks CDC is at Interface 0, so we'll leave it
    # there for backwards compatibility.
    descriptor_list.append(cdc_iad)
    descriptor_list.extend(cdc_interfaces)

if 'MSC' in args.devices:
    descriptor_list.extend(msc_interfaces)

if 'HID' in args.devices:
    descriptor_list.extend(hid_interfaces)

if 'AUDIO' in args.devices:
    # Only add the control interface because other audio interfaces are managed by it to ensure the
    # correct ordering.
    descriptor_list.append(audio_control_interface)

if 'VENDOR' in args.devices:
    descriptor_list.extend(vendor_interfaces)

# Finally, build the composite descriptor.

configuration = standard.ConfigurationDescriptor(
    description="Composite configuration",
    wTotalLength=(standard.ConfigurationDescriptor.bLength +
                  sum([len(bytes(x)) for x in descriptor_list])),
    bNumInterfaces=len(interfaces))
descriptor_list.insert(0, configuration)

string_descriptors = [standard.StringDescriptor(string) for string in StringIndex.strings_in_order()]
serial_number_descriptor = string_descriptors[SERIAL_NUMBER_INDEX]

c_file = args.output_c_file
h_file = args.output_h_file


c_file.write("""\
#include <stdint.h>

#include "py/objtuple.h"
#include "shared-bindings/usb_hid/Device.h"
#include "{H_FILE_NAME}"

""".format(H_FILE_NAME=h_file.name))

c_file.write("""\
// {DESCRIPTION} : {CLASS}
""".format(DESCRIPTION=device.description,
           CLASS=device.__class__))

c_file.write("""\
const uint8_t usb_desc_dev[] = {
""")
for b in bytes(device):
    c_file.write("0x{:02x}, ".format(b))

c_file.write("""\
};
""")

c_file.write("""\
const uint8_t usb_desc_cfg[] = {
""")

# Write out all the regular descriptors as one long array (that's how ASF4 does it).
descriptor_length = 0
for descriptor in descriptor_list:
    c_file.write("""\
// {DESCRIPTION} : {CLASS}
""".format(DESCRIPTION=descriptor.description,
               CLASS=descriptor.__class__))

    b = bytes(descriptor)
    notes = descriptor.notes()
    notes = fix_notes(notes) # for comparision of files beteen runs
    i = 0

    # This prints each subdescriptor on a separate line.
    n = 0
    while i < len(b):
        length = b[i]
        for j in range(length):
            c_file.write("0x{:02x}, ".format(b[i + j]))
        c_file.write("// " + notes[n])
        n += 1
        c_file.write("\n")
        i += length
    descriptor_length += len(b)

c_file.write("""\
};
""")

pointers_to_strings = []

for idx, descriptor in enumerate(string_descriptors):
    c_file.write("""\
// {DESCRIPTION} : {CLASS}
""".format(DESCRIPTION=descriptor.description,
               CLASS=descriptor.__class__))

    b = bytes(descriptor)
    notes = descriptor.notes()
    notes = fix_notes(notes) # for comparision of files beteen runs
    i = 0

    # This prints each subdescriptor on a separate line.
    variable_name = StringIndex.index_to_variable[idx]
    if not variable_name:
        variable_name = "string_descriptor{}".format(idx)

    const = "const "
    if variable_name == "usb_serial_number":
        const = ""
    c_file.write("""\
{const}uint16_t {NAME}[] = {{
""".format(const=const, NAME=variable_name))
    pointers_to_strings.append("{name}".format(name=variable_name))
    n = 0
    while i < len(b):
        length = b[i]
        for j in range(length // 2):
            c_file.write("0x{:04x}, ".format(b[i + 2*j + 1] << 8 | b[i + 2*j]))
        n += 1
        c_file.write("\n")
        i += length
    c_file.write("""\
};
""")

c_file.write("""\
// array of pointer to string descriptors
uint16_t const * const string_desc_arr [] =
{
""")
c_file.write(""",\

""".join(pointers_to_strings))

c_file.write("""
};
""")

c_file.write("\n")

hid_descriptor_length = len(bytes(combined_hid_report_descriptor))

# Now the values we need for the .h file.
h_file.write("""\
#ifndef MICROPY_INCLUDED_AUTOGEN_USB_DESCRIPTOR_H
#define MICROPY_INCLUDED_AUTOGEN_USB_DESCRIPTOR_H

#include <stdint.h>

extern const uint8_t usb_desc_dev[{device_length}];
extern const uint8_t usb_desc_cfg[{configuration_length}];
extern uint16_t usb_serial_number[{serial_number_length}];
extern uint16_t const * const string_desc_arr [{string_descriptor_length}];

extern const uint8_t hid_report_descriptor[{hid_report_descriptor_length}];

#define CFG_TUSB_RHPORT0_MODE       ({rhport0_mode})

#define USB_HID_NUM_DEVICES         {hid_num_devices}

// Vendor name included in Inquiry response, max 8 bytes
#define CFG_TUD_MSC_VENDOR          "{msc_vendor}"

// Product name included in Inquiry response, max 16 bytes
#define CFG_TUD_MSC_PRODUCT         "{msc_product}"

"""
.format(serial_number_length=len(bytes(serial_number_descriptor)) // 2,
        device_length=len(bytes(device)),
        configuration_length=descriptor_length,
        max_configuration_length=max(hid_descriptor_length, descriptor_length),
        string_descriptor_length=len(pointers_to_strings),
        hid_report_descriptor_length=len(bytes(combined_hid_report_descriptor)),
        rhport0_mode='OPT_MODE_DEVICE | OPT_MODE_HIGH_SPEED' if args.highspeed else 'OPT_MODE_DEVICE',
        hid_num_devices=len(args.hid_devices),
        msc_vendor=args.manufacturer[:8],
        msc_product=args.product[:16]))

# Write out the report descriptor and info
c_file.write("""\
const uint8_t hid_report_descriptor[{HID_DESCRIPTOR_LENGTH}] = {{
""".format(HID_DESCRIPTOR_LENGTH=hid_descriptor_length))

for b in bytes(combined_hid_report_descriptor):
    c_file.write("0x{:02x}, ".format(b))
c_file.write("""\
};

""")

# Write out USB HID report buffer definitions.
for name in args.hid_devices:
    c_file.write("""\
static uint8_t {name}_report_buffer[{report_length}];
""".format(name=name.lower(), report_length=hid_report_descriptors.HID_DEVICE_DATA[name].report_length))

    if hid_report_descriptors.HID_DEVICE_DATA[name].out_report_length > 0:
        c_file.write("""\
static uint8_t {name}_out_report_buffer[{report_length}];
""".format(name=name.lower(), report_length=hid_report_descriptors.HID_DEVICE_DATA[name].out_report_length))

# Write out table of device objects.
c_file.write("""
usb_hid_device_obj_t usb_hid_devices[] = {
""")
for name in args.hid_devices:
    device_data = hid_report_descriptors.HID_DEVICE_DATA[name]
    out_report_buffer = '{}_out_report_buffer'.format(name.lower()) if device_data.out_report_length > 0 else 'NULL'
    c_file.write("""\
    {{
        .base          = {{ .type = &usb_hid_device_type }},
        .report_buffer = {name}_report_buffer,
        .report_id     = {report_id},
        .report_length = {report_length},
        .usage_page    = {usage_page:#04x},
        .usage         = {usage:#04x},
        .out_report_buffer = {out_report_buffer},
        .out_report_length = {out_report_length},
    }},
""".format(name=name.lower(), report_id=report_ids[name],
           report_length=device_data.report_length,
           usage_page=device_data.usage_page,
           usage=device_data.usage,
           out_report_buffer=out_report_buffer,
           out_report_length=device_data.out_report_length))
c_file.write("""\
};
""")

# Write out tuple of device objects.
c_file.write("""
mp_obj_tuple_t common_hal_usb_hid_devices = {{
    .base = {{
        .type = &mp_type_tuple,
    }},
    .len = {num_devices},
    .items = {{
""".format(num_devices=len(args.hid_devices)))
for idx in range(len(args.hid_devices)):
    c_file.write("""\
         (mp_obj_t) &usb_hid_devices[{idx}],
""".format(idx=idx))
c_file.write("""\
    },
};
""")

h_file.write("""\
#endif // MICROPY_INCLUDED_AUTOGEN_USB_DESCRIPTOR_H
""")
