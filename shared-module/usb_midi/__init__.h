// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "supervisor/usb.h"

bool usb_midi_enabled(void);
void usb_midi_set_defaults(void);
void usb_midi_setup_ports(void);

size_t usb_midi_descriptor_length(void);
size_t usb_midi_add_descriptor(uint8_t *descriptor_buf, descriptor_counts_t *descriptor_counts, uint8_t *current_interface_string);

extern char *custom_usb_midi_streaming_interface_name;
extern char *custom_usb_midi_audio_control_interface_name;
extern char *custom_usb_midi_in_jack_name;
extern char *custom_usb_midi_out_jack_name;
