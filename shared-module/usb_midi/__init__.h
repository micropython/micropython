/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#ifndef SHARED_MODULE_USB_MIDI___INIT___H
#define SHARED_MODULE_USB_MIDI___INIT___H

extern bool usb_midi_enabled;

void usb_midi_init(void);
void usb_midi_usb_init(void);

size_t usb_midi_descriptor_length(void);
size_t usb_midi_add_descriptor(uint8_t *descriptor_buf, uint8_t audio_control_interface, uint8_t midi_streaming_interface, uint8_t midi_streaming_in_endpoint, uint8_t midi_streaming_out_endpoint, uint8_t audio_control_interface_string, uint8_t midi_streaming_interface_string, uint8_t in_jack_string, uint8_t out_jack_string);

#endif /* SHARED_MODULE_USB_MIDI___INIT___H */
