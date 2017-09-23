/**
 * \file
 *
 * \brief USB Device Stack HID Keyboard Function Definition.
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 * Atmel AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef USBDF_HID_KEYBOARD_H_
#define USBDF_HID_KEYBOARD_H_

#include "usbdc.h"
#include "usb_protocol_hid.h"

/** HID Keyboard Class Key State Type */
enum hiddf_kb_key_state { HID_KB_KEY_UP, HID_KB_KEY_DOWN };

/** Describes the USB HID Keyboard Key descriptors. */
struct hiddf_kb_key_descriptors {
	/* HID Key Value, defined in usb_protocol_hid.h */
	uint8_t key_id;
	/* Flag whether it is a modifier key */
	bool b_modifier;
	/* Key State */
	enum hiddf_kb_key_state state;
};

/**
 * \brief Initialize the USB HID Keyboard Function Driver
 * \return Operation status.
 */
int32_t hiddf_keyboard_init(void);

/**
 * \brief Deinitialize the USB HID Keyboard Function Driver
 * \return Operation status.
 */
int32_t hiddf_keyboard_deinit(void);

/**
 * \brief Check whether HID Keyboard Function is enabled
 * \return Operation status.
 * \return true HID Keyboard Function is enabled
 * \return false HID Keyboard Function is disabled
 */
bool hiddf_keyboard_is_enabled(void);

/**
 * \brief Process the HID Keyboard key state change request
 * \param keys_desc[]  keys_descriptor array for state changing
 * \param keys_count   total keys amount for state changing
 * \return Operation status.
 */
int32_t hiddf_keyboard_keys_state_change(struct hiddf_kb_key_descriptors keys_desc[], uint8_t keys_count);

/**
 * \brief Return version
 */
uint32_t hiddf_keyboard_get_version(void);

#endif /* USBDF_HID_KEYBOARD_H_ */
