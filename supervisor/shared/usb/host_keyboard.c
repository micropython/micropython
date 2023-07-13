/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2023 Jeff Epler for Adafruit Industries
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

#include "tusb.h"

#include "py/ringbuf.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "supervisor/usb.h"

// Buffer the incoming serial data in the background so that we can look for the
// interrupt character.
STATIC ringbuf_t _incoming_ringbuf;
STATIC uint8_t _buf[16];

uint8_t _dev_addr;
uint8_t _interface;

#define FLAG_ALPHABETIC (1)
#define FLAG_SHIFT (2)
#define FLAG_NUMLOCK (4)
#define FLAG_CTRL (8)
#define FLAG_LUT (16)

const char *const lut[] = {
    "!@#$%^&*()",                            /* 0 - shifted numeric keys */
    "\r\x1b\10\t -=[]\\#;'`,./",             /* 1 - symbol keys */
    "\n\x1b\177\t _+{}|~:\"~<>?",            /* 2 - shifted */
    "\12\13\10\22",                          /* 3 - arrow keys RLDU */
    "/*-+\n1234567890.",                     /* 4 - keypad w/numlock */
    "/*-+\n\xff\2\xff\4\xff\3\xff\1\xff\xff.", /* 5 - keypad w/o numlock */
};

struct keycode_mapper {
    uint8_t first, last, code, flags;
} keycode_to_ascii[] = {
    { HID_KEY_A, HID_KEY_Z, 'a', FLAG_ALPHABETIC, },

    { HID_KEY_1, HID_KEY_9, 0, FLAG_SHIFT | FLAG_LUT, },
    { HID_KEY_1, HID_KEY_9, '1', 0, },
    { HID_KEY_0, HID_KEY_0, ')', FLAG_SHIFT, },
    { HID_KEY_0, HID_KEY_0, '0', 0, },

    { HID_KEY_ENTER, HID_KEY_ENTER, '\n', FLAG_CTRL },
    { HID_KEY_ENTER, HID_KEY_SLASH, 2, FLAG_SHIFT | FLAG_LUT, },
    { HID_KEY_ENTER, HID_KEY_SLASH, 1, FLAG_LUT, },

    { HID_KEY_F1, HID_KEY_F1, 0x1e, 0, }, // help key on xerox 820 kbd

    { HID_KEY_ARROW_RIGHT, HID_KEY_ARROW_UP, 3, FLAG_LUT },

    { HID_KEY_KEYPAD_DIVIDE, HID_KEY_KEYPAD_DECIMAL, 4, FLAG_NUMLOCK | FLAG_LUT },
    { HID_KEY_KEYPAD_DIVIDE, HID_KEY_KEYPAD_DECIMAL, 5, FLAG_LUT },
};

STATIC bool report_contains(const hid_keyboard_report_t *report, uint8_t key) {
    for (int i = 0; i < 6; i++) {
        if (report->keycode[i] == key) {
            return true;
        }
    }
    return false;
}

int old_ascii = -1;
uint32_t repeat_timeout;
// this matches Linux default of 500ms to first repeat, 1/20s thereafter
const uint32_t default_repeat_time = 50;
const uint32_t initial_repeat_time = 500;

STATIC void send_ascii(uint8_t code, uint32_t repeat_time) {
    old_ascii = code;
    // repeat_timeout = millis() + repeat_time;
    if (code == mp_interrupt_char) {
        mp_sched_keyboard_interrupt();
        return;
    }
    if (ringbuf_num_empty(&_incoming_ringbuf) == 0) {
        // Drop on the floor
        return;
    }
    ringbuf_put(&_incoming_ringbuf, code);
}

hid_keyboard_report_t old_report;

STATIC void process_event(uint8_t dev_addr, uint8_t instance, const hid_keyboard_report_t *report) {
    bool alt = report->modifier & 0x44;
    bool shift = report->modifier & 0x22;
    bool ctrl = report->modifier & 0x11;
    bool caps = old_report.reserved & 1;
    bool num = old_report.reserved & 2;
    uint8_t code = 0;

    if (report->keycode[0] == 1 && report->keycode[1] == 1) {
        // keyboard says it has exceeded max kro
        return;
    }

    // something was pressed or release, so cancel any key repeat
    old_ascii = -1;

    for (int i = 0; i < 6; i++) {
        uint8_t keycode = report->keycode[i];
        if (keycode == 0) {
            continue;
        }
        if (report_contains(&old_report, keycode)) {
            continue;
        }

        /* key is newly pressed */
        if (keycode == HID_KEY_NUM_LOCK) {
            num = !num;
        } else if (keycode == HID_KEY_CAPS_LOCK) {
            caps = !caps;
        } else {
            for (size_t j = 0; j < MP_ARRAY_SIZE(keycode_to_ascii); j++) {
                struct keycode_mapper *mapper = &keycode_to_ascii[j];
                if (!(keycode >= mapper->first && keycode <= mapper->last)) {
                    continue;
                }
                if (mapper->flags & FLAG_SHIFT && !shift) {
                    continue;
                }
                if (mapper->flags & FLAG_NUMLOCK && !num) {
                    continue;
                }
                if (mapper->flags & FLAG_CTRL && !ctrl) {
                    continue;
                }
                if (mapper->flags & FLAG_LUT) {
                    code = lut[mapper->code][keycode - mapper->first];
                } else {
                    code = keycode - mapper->first + mapper->code;
                }
                if (mapper->flags & FLAG_ALPHABETIC) {
                    if (shift ^ caps) {
                        code ^= ('a' ^ 'A');
                    }
                }
                if (ctrl) {
                    code &= 0x1f;
                }
                if (alt) {
                    code ^= 0x80;
                }
                send_ascii(code, initial_repeat_time);
                break;
            }
        }
    }

    uint8_t leds = (caps | (num << 1));
    if (leds != old_report.reserved) {
        tuh_hid_set_report(dev_addr, instance /*idx*/, 0 /*report_id*/, HID_REPORT_TYPE_OUTPUT /*report_type*/, &leds, sizeof(leds));
    }
    old_report = *report;
    old_report.reserved = leds;
}

bool usb_keyboard_in_use(uint8_t dev_addr, uint8_t interface) {
    return _dev_addr == dev_addr && _interface == interface;
}

void usb_keyboard_detach(uint8_t dev_addr, uint8_t interface) {
    if (!usb_keyboard_in_use(dev_addr, interface)) {
        return;
    }
    _dev_addr = 0;
    _interface = 0;
}

void usb_keyboard_attach(uint8_t dev_addr, uint8_t interface) {
    if (usb_keyboard_in_use(dev_addr, interface) || _dev_addr != 0) {
        return;
    }
    uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, interface);
    if (itf_protocol == HID_ITF_PROTOCOL_KEYBOARD) {
        _dev_addr = dev_addr;
        _interface = interface;
        tuh_hid_receive_report(dev_addr, interface);
    }
}

void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t interface, uint8_t const *desc_report, uint16_t desc_len) {
    usb_keyboard_attach(dev_addr, interface);
}

void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t interface) {
    usb_keyboard_detach(dev_addr, interface);
}

void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *report, uint16_t len) {
    if (len != sizeof(hid_keyboard_report_t)) {
        return;
    } else {
        process_event(dev_addr, instance, (hid_keyboard_report_t *)report);
    }
    // continue to request to receive report
    tuh_hid_receive_report(dev_addr, instance);
}

void usb_keyboard_init(void) {
    ringbuf_init(&_incoming_ringbuf, _buf, sizeof(_buf) - 1);
}

bool usb_keyboard_chars_available(void) {
    return ringbuf_num_filled(&_incoming_ringbuf) > 0;
}

char usb_keyboard_read_char(void) {
    if (ringbuf_num_filled(&_incoming_ringbuf) > 0) {
        return ringbuf_get(&_incoming_ringbuf);
    }
    return -1;
}
