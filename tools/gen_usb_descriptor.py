import argparse

import os
import sys

sys.path.append("../../tools/usb_descriptor")

from adafruit_usb_descriptor import audio, audio10, cdc, hid, midi, msc, standard, util
import hid_report_descriptors

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
                bmAttributes=standard.EndpointDescriptor.TYPE_BULK,
                bInterval=0),
            standard.EndpointDescriptor(
                description="MSC out",
                bEndpointAddress=0x1 | standard.EndpointDescriptor.DIRECTION_OUT,
                bmAttributes=standard.EndpointDescriptor.TYPE_BULK,
                bInterval=0)
        ]
    )
]

# Include only these HID devices.
# DIGITIZER works on Linux but conflicts with MOUSE, so leave it out for now.
hid_devices = ("KEYBOARD", "MOUSE", "CONSUMER", "GAMEPAD")

combined_hid_report_descriptor = hid.ReportDescriptor(
    description="MULTIDEVICE",
    report_descriptor=b''.join(
        hid_report_descriptors.REPORT_DESCRIPTORS[name].report_descriptor for name in hid_devices ))

hid_report_ids_dict = { name: hid_report_descriptors.REPORT_IDS[name] for name in hid_devices }
hid_report_lengths_dict = { name: hid_report_descriptors.REPORT_LENGTHS[name] for name in hid_devices }
hid_max_report_length = max(hid_report_lengths_dict.values())

# ASF4 expects keyboard and generic devices to have both in and out endpoints,
# and will fail (possibly silently) if both are not supplied.
hid_endpoint_in_descriptor = standard.EndpointDescriptor(
    description="HID in",
    bEndpointAddress=0x0 | standard.EndpointDescriptor.DIRECTION_IN,
    bmAttributes=standard.EndpointDescriptor.TYPE_INTERRUPT,
    bInterval=10)

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
                wDescriptorLength=len(bytes(combined_hid_report_descriptor))),
            hid_endpoint_in_descriptor,
            ]
        ),
    ]

# Audio!
# In and out here are relative to CircuitPython

# USB OUT -> midi_in_jack_emb -> midi_out_jack_ext -> CircuitPython
midi_in_jack_emb = midi.InJackDescriptor(
    description="MIDI PC -> CircuitPython",
    bJackType=midi.JACK_TYPE_EMBEDDED,
    iJack=StringIndex.index("CircuitPython usb_midi.ports[0]"))
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
    description="MIDI PC <- CircuitPython",
    bJackType=midi.JACK_TYPE_EMBEDDED,
    input_pins=[(midi_in_jack_ext, 1)],
    iJack=StringIndex.index("CircuitPython usb_midi.ports[1]"))


audio_midi_interface = standard.InterfaceDescriptor(
    description="Midi goodness",
    bInterfaceClass=audio.AUDIO_CLASS_DEVICE,
    bInterfaceSubClass=audio.AUDIO_SUBCLASS_MIDI_STREAMING,
    bInterfaceProtocol=audio.AUDIO_PROTOCOL_V1,
    iInterface=StringIndex.index("CircuitPython MIDI"),
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
            description="MIDI data out to CircuitPython",
            bEndpointAddress=0x0 | standard.EndpointDescriptor.DIRECTION_OUT,
            bmAttributes=standard.EndpointDescriptor.TYPE_BULK),
        midi.DataEndpointDescriptor(baAssocJack=[midi_in_jack_emb]),
        standard.EndpointDescriptor(
            description="MIDI data in from CircuitPython",
            bEndpointAddress=0x0 | standard.EndpointDescriptor.DIRECTION_IN,
            bmAttributes=standard.EndpointDescriptor.TYPE_BULK,
            bInterval = 0x0),
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
        iInterface=StringIndex.index("CircuitPython Audio"),
        subdescriptors=[
            cs_ac_interface,
        ])

# Audio streaming interfaces must occur before MIDI ones.
audio_interfaces = [audio_control_interface] + cs_ac_interface.audio_streaming_interfaces + cs_ac_interface.midi_streaming_interfaces

# This will renumber the endpoints to make them unique across descriptors,
# and renumber the interfaces in order. But we still need to fix up certain
# interface cross-references.
interfaces = util.join_interfaces(cdc_interfaces, msc_interfaces, hid_interfaces, audio_interfaces)

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
descriptor_list.append(cdc_iad)
descriptor_list.extend(cdc_interfaces)
descriptor_list.extend(msc_interfaces)
# Only add the control interface because other audio interfaces are managed by it to ensure the
# correct ordering.
descriptor_list.append(audio_control_interface)
# Put the CDC IAD just before the CDC interfaces.
# There appears to be a bug in the Windows composite USB driver that requests the
# HID report descriptor with the wrong interface number if the HID interface is not given
# first. However, it still fetches the descriptor anyway. We could reorder the interfaces but
# the Windows 7 Adafruit_usbser.inf file thinks CDC is at Interface 0, so we'll leave it
# there for backwards compatibility.
descriptor_list.extend(hid_interfaces)

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

c_file.write("\n");

hid_descriptor_length = len(bytes(combined_hid_report_descriptor))

# Now we values we need for the .h file.
h_file.write("""\
#ifndef MICROPY_INCLUDED_AUTOGEN_USB_DESCRIPTOR_H
#define MICROPY_INCLUDED_AUTOGEN_USB_DESCRIPTOR_H

#include <stdint.h>

const uint8_t usb_desc_dev[{device_length}];
// Make sure the control buffer is big enough to fit the descriptor.
#define CFG_TUD_ENUM_BUFFER_SIZE {max_configuration_length}
const uint8_t usb_desc_cfg[{configuration_length}];
uint16_t usb_serial_number[{serial_number_length}];
uint16_t const * const string_desc_arr [{string_descriptor_length}];

const uint8_t hid_report_descriptor[{HID_REPORT_DESCRIPTOR_LENGTH}];

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
        HID_REPORT_DESCRIPTOR_LENGTH=len(bytes(combined_hid_report_descriptor)),
        msc_vendor=args.manufacturer[:8],
        msc_product=args.product[:16]))

# #define the report ID's used in the combined HID descriptor
for name, id in hid_report_ids_dict.items():
    h_file.write("""\
#define USB_HID_REPORT_ID_{name} {id}
""".format(name=name,
           id=id))

h_file.write("\n")

# #define the report sizes used in the combined HID descriptor
for name, length in hid_report_lengths_dict.items():
    h_file.write("""\
#define USB_HID_REPORT_LENGTH_{name} {length}
""".format(name=name,
           length=length))

h_file.write("\n")

h_file.write("""\
#define USB_HID_NUM_DEVICES {num_devices}
#define USB_HID_MAX_REPORT_LENGTH {max_length}
""".format(num_devices=len(hid_report_lengths_dict),
           max_length=hid_max_report_length))



# Write out the report descriptor and info
c_file.write("""\
const uint8_t hid_report_descriptor[{HID_DESCRIPTOR_LENGTH}] = {{
""".format(HID_DESCRIPTOR_LENGTH=hid_descriptor_length))

for b in bytes(combined_hid_report_descriptor):
    c_file.write("0x{:02x}, ".format(b))
c_file.write("""
};
""")

h_file.write("""\
#endif // MICROPY_INCLUDED_AUTOGEN_USB_DESCRIPTOR_H
""")
