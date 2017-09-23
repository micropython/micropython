/**
 * \file
 *
 * \brief USB protocol implementation.
 *
 * This file contains the USB definitions and data structures provided by the
 * USB 2.0 specification.
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

#define CONF_NO_ASSERT_CHECK

#include "usb_protocol.h"

#ifdef CONF_NO_ASSERT_CHECK
#define _param_error_check(cond)
#define _desc_len_check()                                                                                              \
	if (usb_desc_len(desc) < 2) {                                                                                      \
		/* Encounter an invalid descriptor. */                                                                         \
		return NULL;                                                                                                   \
	}
#else
#define _param_error_check(cond) ASSERT(cond)
#define _desc_len_check() ASSERT(usb_desc_len(desc) >= 2)
#endif

uint8_t *usb_find_desc(uint8_t *desc, uint8_t *eof, uint8_t type)
{
	_param_error_check(desc && eof && (desc < eof));

	while (desc < eof) {
		_desc_len_check();
		if (type == usb_desc_type(desc)) {
			return desc;
		}
		desc = usb_desc_next(desc);
	}
	return NULL;
}

uint8_t *usb_find_iface_after(uint8_t *desc, uint8_t *eof, uint8_t iface_n)
{
	_param_error_check(desc && eof && (desc < eof));

	while (desc < eof) {
		_desc_len_check();
		if (USB_DT_INTERFACE == usb_desc_type(desc)) {
			if (iface_n != desc[2]) {
				return desc;
			}
		}
		desc = usb_desc_next(desc);
	}
	return eof;
}

uint8_t *usb_find_ep_desc(uint8_t *desc, uint8_t *eof)
{
	_param_error_check(desc && eof && (desc < eof));

	while (desc < eof) {
		_desc_len_check();
		if (USB_DT_INTERFACE == usb_desc_type(desc)) {
			break;
		}
		if (USB_DT_ENDPOINT == usb_desc_type(desc)) {
			return desc;
		}
		desc = usb_desc_next(desc);
	}
	return NULL;
}

uint8_t *usb_find_cfg_desc(uint8_t *desc, uint8_t *eof, uint8_t cfg_value)
{
	_param_error_check(desc && eof && (desc < eof));

	desc = usb_find_desc(desc, eof, USB_DT_CONFIG);
	if (!desc) {
		return NULL;
	}
	while (desc < eof) {
		_desc_len_check();
		if (desc[1] != USB_DT_CONFIG) {
			break;
		}
		if (desc[5] == cfg_value) {
			return desc;
		}
		desc = usb_cfg_desc_next(desc);
	}
	return NULL;
}

uint8_t *usb_find_str_desc(uint8_t *desc, uint8_t *eof, uint8_t str_index)
{
	uint8_t i;

	_param_error_check(desc && eof && (desc < eof));

	for (i = 0; desc < eof;) {
		desc = usb_find_desc(desc, eof, USB_DT_STRING);
		if (desc) {
			_desc_len_check();
			if (i == str_index) {
				return desc;
			}
			i++;
			desc = usb_desc_next(desc);
		} else {
			return NULL;
		}
	}
	return NULL;
}
