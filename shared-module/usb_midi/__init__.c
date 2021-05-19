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

#include "py/gc.h"
#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/objtuple.h"
#include "shared-bindings/usb_midi/PortIn.h"
#include "shared-bindings/usb_midi/PortOut.h"
#include "supervisor/memory.h"
#include "supervisor/usb.h"
#include "tusb.h"

static const uint8_t usb_midi_descriptor_template[] = {
    // Audio Interface Descriptor
    0x09,        //  0 bLength
    0x04,        //  1 bDescriptorType (Interface)
    0xFF,        //  2 bInterfaceNumber [SET AT RUNTIME]
#define MIDI_AUDIO_CONTROL_INTERFACE_NUMBER_INDEX (2)
    0x00,        //  3 bAlternateSetting
    0x00,        //  4 bNumEndpoints 0
    0x01,        //  5 bInterfaceClass (Audio)
    0x01,        //  6 bInterfaceSubClass (Audio Control)
    0x00,        //  7 bInterfaceProtocol
    0xFF,        //  8 iInterface (String Index) [SET AT RUNTIME]
#define MIDI_AUDIO_CONTROL_INTERFACE_STRING_INDEX (8)

    // Audio10 Control Interface Descriptor
    0x09,        //  9  bLength
    0x24,        // 10 bDescriptorType (See Next Line)
    0x01,        // 11 bDescriptorSubtype (CS_INTERFACE -> HEADER)
    0x00, 0x01,  // 12,13 bcdADC 1.00
    0x09, 0x00,  // 14,15 wTotalLength 9
    0x01,        // 16 binCollection 0x01
    0xFF,        // 17 baInterfaceNr [SET AT RUNTIME: one-element list: same as 20]
#define MIDI_STREAMING_INTERFACE_NUMBER_INDEX_2 (17)

    // MIDI Streaming Interface Descriptor
    0x09,        // 18 bLength
    0x04,        // 19 bDescriptorType (Interface)
    0xFF,        // 20 bInterfaceNumber [SET AT RUNTIME]
#define MIDI_STREAMING_INTERFACE_NUMBER_INDEX (20)
    0x00,        // 21 bAlternateSetting
    0x02,        // 22 bNumEndpoints 2
    0x01,        // 23 bInterfaceClass (Audio)
    0x03,        // 24 bInterfaceSubClass (MIDI Streaming)
    0x00,        // 25 bInterfaceProtocol
    0xFF,        // 26 iInterface (String Index) [SET AT RUNTIME]
#define MIDI_STREAMING_INTERFACE_STRING_INDEX (26)

    // MIDI Header Descriptor
    0x07,        // 27 bLength
    0x24,        // 28 bDescriptorType: CLASS SPECIFIC INTERFACE
    0x01,        // 29 bDescriptorSubtype: MIDI STREAMING HEADER
    0x00, 0x01,  // 30,31 bsdMSC (MIDI STREAMING) version 1.0
    0x25, 0x00,  // 32,33 wLength

    // MIDI Embedded In Jack Descriptor
    0x06,        // 34 bLength
    0x24,        // 35 bDescriptorType: CLASS SPECIFIC INTERFACE
    0x02,        // 36 bDescriptorSubtype: MIDI IN JACK
    0x01,        // 37 bJackType: EMBEDDED
    0x01,        // 38 id (always 1)
    0xFF,        // 39 iJack (String Index)  [SET AT RUNTIME]
#define MIDI_IN_JACK_STRING_INDEX (39)

    // MIDI External In Jack Descriptor
    0x06,        // 40 bLength
    0x24,        // 41 bDescriptorType: CLASS SPECIFIC INTERFACE
    0x02,        // 42 bDescriptorSubtype: MIDI IN JACK
    0x02,        // 43 bJackType: EXTERNAL
    0x02,        // 44 bJackId (always 2)
    0x00,        // 45 iJack (String Index)

    // MIDI Embedded Out Jack Descriptor
    0x09,        // 46 bLength
    0x24,        // 47 bDescriptorType: CLASS SPECIFIC INTERFACE
    0x03,        // 48 bDescriptorSubtype: MIDI OUT JACK
    0x01,        // 49 bJackType: EMBEDDED
    0x03,        // 50 bJackID (always 3)
    0x01,        // 51 bNrInputPins (always 1)
    0x02,        // 52 BaSourceID(1) (always 2)
    0x01,        // 53 BaSourcePin(1) (always 1)
    0xFF,        // 54 iJack (String Index)  [SET AT RUNTIME]
#define MIDI_OUT_JACK_STRING_INDEX (54)

    // MIDI External Out Jack Descriptor
    0x09,        // 55 bLength
    0x24,        // 56 bDescriptorType: CLASS SPECIFIC INTERFACE
    0x03,        // 57 bDescriptorSubtype: MIDI OUT JACK
    0x02,        // 58 bJackType: EXTERNAL
    0x04,        // 59 bJackID (always 4)
    0x01,        // 60 bNrInputPins (always 1)
    0x01,        // 61 BaSourceID(1) (always 1)
    0x01,        // 62 BaSourcePin(1) (always 1)
    0x00,        // 63 iJack (String Index)

    // MIDI Streaming Endpoint OUT Descriptor
    0x07,        // 64 bLength
    0x05,        // 65 bDescriptorType (EndPoint)
    0xFF,        // 66 bEndpointAddress (OUT/H2D) [SET AT RUNTIME]
#define MIDI_STREAMING_OUT_ENDPOINT_INDEX (66)
    0x02,        // 67 bmAttributes (Bulk)
    #if USB_HIGHSPEED
    0x00, 0x02,  // 68,69 wMaxPacketSize (512)
    #else
    0x40, 0x00,  // 68,69 wMaxPacketSize (64)
    #endif
    0x00,        // 70 bInterval 0 (unit depends on device speed)

    // MIDI Data Endpoint Descriptor
    0x05,        // 71 bLength
    0x25,        // 72 bDescriptorType: CLASS SPECIFIC ENDPOINT
    0x01,        // 73 bDescriptorSubtype: MIDI STREAMING 1.0
    0x01,        // 74 bNumGrpTrmBlock (always 1)
    0x01,        // 75 baAssoGrpTrmBlkID(1) (always 1)

    // MIDI IN Data Endpoint
    0x07,        // 76 bLength
    0x05,        // 77 bDescriptorType: Endpoint
    0xFF,        // 78 bEndpointAddress (IN/D2H) [SET AT RUNTIME: 0x80 | number]
#define MIDI_STREAMING_IN_ENDPOINT_INDEX (78)
    0x02,        // 79 bmAttributes (Bulk)
    #if USB_HIGHSPEED
    0x00, 0x02,  // 80, 81 wMaxPacketSize (512)
    #else
    0x40, 0x00,  // 80, 81 wMaxPacketSize (64)
    #endif
    0x00,        // 82 bInterval 0 (unit depends on device speed)

    // MIDI Data Endpoint Descriptor
    0x05,        // 83 bLength
    0x25,        // 84 bDescriptorType: CLASS SPECIFIC ENDPOINT
    0x01,        // 85 bDescriptorSubtype: MIDI STREAMING 1.0
    0x01,        // 86 bNumGrpTrmBlock (always 1)
    0x03,        // 87 baAssoGrpTrmBlkID(1) (always 3)
};

// Is the USB MIDI device enabled?
static bool usb_midi_is_enabled;

void usb_midi_set_defaults(void) {
    usb_midi_is_enabled = CIRCUITPY_USB_MIDI_ENABLED_DEFAULT;
}

bool usb_midi_enabled(void) {
    return usb_midi_is_enabled;
}


size_t usb_midi_descriptor_length(void) {
    return sizeof(usb_midi_descriptor_template);
}

static const char midi_streaming_interface_name[] = USB_INTERFACE_NAME " MIDI";
static const char midi_audio_control_interface_name[] = USB_INTERFACE_NAME " Audio";
static const char midi_in_jack_name[] = USB_INTERFACE_NAME " usb_midi.ports[0]";
static const char midi_out_jack_name[] = USB_INTERFACE_NAME " usb_midi.ports[0]";

size_t usb_midi_add_descriptor(uint8_t *descriptor_buf, descriptor_counts_t *descriptor_counts, uint8_t *current_interface_string) {
    memcpy(descriptor_buf, usb_midi_descriptor_template, sizeof(usb_midi_descriptor_template));

    descriptor_buf[MIDI_AUDIO_CONTROL_INTERFACE_NUMBER_INDEX] = descriptor_counts->current_interface;
    descriptor_counts->current_interface++;

    descriptor_buf[MIDI_STREAMING_IN_ENDPOINT_INDEX] =
        0x80 | (USB_MIDI_EP_NUM_IN ? USB_MIDI_EP_NUM_IN : descriptor_counts->current_endpoint);
    descriptor_counts->num_in_endpoints++;
    descriptor_buf[MIDI_STREAMING_OUT_ENDPOINT_INDEX] =
        USB_MIDI_EP_NUM_OUT ? USB_MIDI_EP_NUM_OUT : descriptor_counts->current_endpoint;
    descriptor_counts->num_out_endpoints++;
    descriptor_counts->current_endpoint++;

    descriptor_buf[MIDI_STREAMING_INTERFACE_NUMBER_INDEX] = descriptor_counts->current_interface;
    descriptor_buf[MIDI_STREAMING_INTERFACE_NUMBER_INDEX_2] = descriptor_counts->current_interface;
    descriptor_counts->current_interface++;

    usb_add_interface_string(*current_interface_string, midi_streaming_interface_name);
    descriptor_buf[MIDI_STREAMING_INTERFACE_STRING_INDEX] = *current_interface_string;
    (*current_interface_string)++;

    usb_add_interface_string(*current_interface_string, midi_audio_control_interface_name);
    descriptor_buf[MIDI_AUDIO_CONTROL_INTERFACE_STRING_INDEX] = *current_interface_string;
    (*current_interface_string)++;

    usb_add_interface_string(*current_interface_string, midi_in_jack_name);
    descriptor_buf[MIDI_IN_JACK_STRING_INDEX] = *current_interface_string;
    (*current_interface_string)++;

    usb_add_interface_string(*current_interface_string, midi_out_jack_name);
    descriptor_buf[MIDI_OUT_JACK_STRING_INDEX] = *current_interface_string;
    (*current_interface_string)++;

    return sizeof(usb_midi_descriptor_template);
}

static const usb_midi_portin_obj_t midi_portin_obj = {
    .base = {
        .type = &usb_midi_portin_type,
    },
};

static const usb_midi_portout_obj_t midi_portout_obj = {
    .base = {
        .type = &usb_midi_portout_type,
    }
};

static const mp_rom_obj_tuple_t midi_ports_tuple = {
    .base = {
        .type = &mp_type_tuple,
    },
    .len = 2,
    .items = {
        MP_ROM_PTR(&midi_portin_obj),
        MP_ROM_PTR(&midi_portout_obj),
    },
};

void usb_midi_setup_ports(void) {
    // Right now midi_ports_tuple contains no heap objects, but if it does in the future,
    // it will need to be protected against gc.

    mp_obj_tuple_t *ports = usb_midi_is_enabled ? MP_OBJ_FROM_PTR(&midi_ports_tuple) : mp_const_empty_tuple;
    mp_map_lookup(&usb_midi_module_globals.map, MP_ROM_QSTR(MP_QSTR_ports), MP_MAP_LOOKUP)->value =
        MP_OBJ_FROM_PTR(ports);
}

static bool usb_midi_set_enabled(bool enabled) {
    // We can't change the descriptors once we're connected.
    if (tud_connected()) {
        return false;
    }
    usb_midi_is_enabled = enabled;
    return true;
}

bool common_hal_usb_midi_disable(void) {
    return usb_midi_set_enabled(false);
}

bool common_hal_usb_midi_enable(void) {
    return usb_midi_set_enabled(true);
}
