/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 hathach for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "shared-bindings/usb_midi/__init__.h"

#include "genhdr/autogen_usb_descriptor.h"
#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/objtuple.h"
#include "shared-bindings/usb_midi/PortIn.h"
#include "shared-bindings/usb_midi/PortOut.h"
#include "supervisor/memory.h"
#include "tusb.h"

supervisor_allocation *usb_midi_allocation;

static uint8_t[] usb_midi_descriptor[] = {
    0x09,        //   0 bLength
    0x04,        //   1 bDescriptorType (Interface)
    0xFF,        //   2 bInterfaceNumber [SET AT RUNTIME]
    0x00,        //   3 bAlternateSetting
    0x00,        //   4 bNumEndpoints 0
    0x01,        //   5 bInterfaceClass (Audio)
    0x01,        //   6 bInterfaceSubClass (Audio Control)
    0x00,        //   7 bInterfaceProtocol
    0x00,        //   8 iInterface (String Index) [SET AT RUNTIME]

    0x09,        //   9  bLength
    0x24,        //  10 bDescriptorType (See Next Line)
    0x01,        //  11 bDescriptorSubtype (CS_INTERFACE -> HEADER)
    0x00, 0x01,  //  12,13 bcdADC 1.00
    0x09, 0x00,  //  14,15 wTotalLength 9
    0x01,        //  16 binCollection 0x01
    0xFF,        //  17 baInterfaceNr [SET AT RUNTIME: one-element list: same as 20]

    0x09,        //  18 bLength
    0x04,        //  19 bDescriptorType (Interface)
    0xFF,        //  20 bInterfaceNumber [SET AT RUNTIME]
    0x00,        //  21 bAlternateSetting
    0x02,        //  22 bNumEndpoints 2
    0x01,        //  23 bInterfaceClass (Audio)
    0x03,        //  24 bInterfaceSubClass (MIDI Streaming)
    0x00,        //  25 bInterfaceProtocol
    0x0A,        //  26 iInterface (String Index) [SET AT RUNTIME]

    0x07,        //  27 bLength
    0x24,        //  28 bDescriptorType (See Next Line)
    0x01, 0x00, 0x01, 0x25, 0x00,  // 29,30,31,32,33
    0x06,        //  34 bLength
    0x24,        //  35 bDescriptorType (See Next Line)
    0x02, 0x01, 0x01, 0x08,  // 36,37,38,39
    0x06,        // 40 bLength
    0x24,        // 41 bDescriptorType (See Next Line)
    0x02, 0x02, 0x02, 0x00,  // 42,43,44,45
    0x09,        // 46 bLength
    0x24,        // 47 bDescriptorType (See Next Line)
    0x03, 0x01, 0x03, 0x01, 0x02, 0x01, 0x09,  // 48,49,50,51,52,53,54
    0x09,        // 56 bLength
    0x24,        // 57 bDescriptorType (See Next Line)
    0x03, 0x02, 0x04, 0x01, 0x01, 0x01, 0x00,  // 58,59,60,61,62,63,64
    0x07,        // 65 bLength
    0x05,        // 66 bDescriptorType (See Next Line)
    0xFF,        // 67 bEndpointAddress (OUT/H2D) [SET AT RUNTIME]
    0x02,        // 68 bmAttributes (Bulk)
    0x40, 0x00,  // 69,70 wMaxPacketSize 64
    0x00,        // 71 bInterval 0 (unit depends on device speed)

    0x05,        // 72 bLength
    0x25,        // 73 bDescriptorType (See Next Line)
    0x01, 0x01, 0x01,  // 74,75,76
    0x07,        // 77 bLength
    0x05,        // 78 bDescriptorType (See Next Line)
    0xFF,        // 79 bEndpointAddress (IN/D2H) [SET AT RUNTIME: 0x8 | number]
    0x02,        // 80 bmAttributes (Bulk)
    0x40, 0x00,  // 81,82 wMaxPacketSize 64
    0x00,        // 83 bInterval 0 (unit depends on device speed)

    0x05,        // 84 bLength
    0x25,        // 85 bDescriptorType (See Next Line)
    0x01, 0x01, 0x03,  // 86,87,88
};

// Is the USB MIDI device enabled?
bool usb_midi_enabled;

// Indices into usb_midi_descriptor for values that must be set at runtime.

#define MIDI_AUDIO_CONTROL_INTERFACE_NUMBER_INDEX 2
#define MIDI_AUDIO_CONTROL_INTERFACE_STRING_INDEX 8

// These two get the same value.
#define MIDI_MIDI_STREAMING_INTERFACE_NUMBER_INDEX 20
#define MIDI_MIDI_STREAMING_INTERFACE_NUMBER_XREF_INDEX 17

#define MIDI_MIDI_STREAMING_INTERFACE_STRING_INDEX 26

#define MIDI_MIDI_STREAMING_OUT_ENDPOINT_INDEX 67
#define MIDI_MIDI_STREAMING_IN_ENDPOINT_INDEX 79

size_t usb_midi_desc_length(void) {
    return sizeof(usb_midi_descriptor);
}

size_t usb_midi_add_desc(uint8_t *desc_buf,
                         uint8_t audio_control_interface_number,
                         uint8_t midi_streaming_interface_number,
                         uint8_t midi_streaming_in_endpoint_address,
                         uint8_t midi_streaming_out_endpoint_address,
                         uint8_t audio_control_interface_string,
                         uint8_t midi_streaming_interface_string) {
    memcpy(descriptor_buf, usb_midi_descriptor, sizeof(usb_midi_descriptor));
    desc_buf[MIDI_AUDIO_CONTROL_INTERFACE_NUMBER_INDEX] = audio_control_interface_number;
    desc_buf[MIDI_AUDIO_CONTROL_INTERFACE_STRING_INDEX] = audio_control_interface_string;

    desc_buf[MIDI_MIDI_STREAMING_INTERFACE_NUMBER_INDEX] = midi_streaming_interface_number;
    desc_buf[MIDI_MIDI_STREAMING_INTERFACE_NUMBER_XREF_INDEX] = midi_streaming_interface_number;
    desc_buf[MIDI_MIDI_STREAMING_INTERFACE_STRING_INDEX] = midi_streaming_interface_string;

    desc_buf[MSC_IN_ENDPOINT_INDEX] = midi_streaming_in_endpoint_address;
    desc_buf[MSC_OUT_ENDPOINT_INDEX] = 0x80 | midi_streaming_out_endpoint_address;
    return sizeof(usb_midi_descriptor);
}


void usb_midi_init(void) {
    usb_midi_enabled = true;
}

void usb_midi_usb_init(void) {
    mp_obj_tuple_t *ports;

    if (usb_midi_enabled) {
        // TODO(tannewt): Make this dynamic.
        size_t tuple_size = align32_size(sizeof(mp_obj_tuple_t) + sizeof(mp_obj_t *) * 2);
        size_t portin_size = align32_size(sizeof(usb_midi_portin_obj_t));
        size_t portout_size = align32_size(sizeof(usb_midi_portout_obj_t));

        // For each embedded MIDI Jack in the descriptor we create a Port
        usb_midi_allocation = allocate_memory(tuple_size + portin_size + portout_size, false, false);

        ports = (mp_obj_tuple_t *)usb_midi_allocation->ptr;
        ports->base.type = &mp_type_tuple;
        ports->len = 2;

        usb_midi_portin_obj_t *in = (usb_midi_portin_obj_t *)(usb_midi_allocation->ptr + tuple_size / 4);
        in->base.type = &usb_midi_portin_type;
        ports->items[0] = MP_OBJ_FROM_PTR(in);

        usb_midi_portout_obj_t *out = (usb_midi_portout_obj_t *)(usb_midi_allocation->ptr + tuple_size / 4 + portin_size / 4);
        out->base.type = &usb_midi_portout_type;
        ports->items[1] = MP_OBJ_FROM_PTR(out);
    } else {
        ports = mp_const_empty_tuple;
    }

    mp_map_lookup(&usb_midi_module_globals.map, MP_ROM_QSTR(MP_QSTR_ports), MP_MAP_LOOKUP)->value = MP_OBJ_FROM_PTR(ports);
}

bool common_hal_usb_midi_configure_usb(bool enabled) {
    // We can't change the descriptors once we're connected.
    if (tud_connected()) {
        return false;
    }
    usb_midi_enabled = enabled;
    return true;
}
