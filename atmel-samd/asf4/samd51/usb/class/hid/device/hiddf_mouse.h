/**
 * \file
 *
 * \brief USB Device Stack HID Mouse Function Definition.
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

#ifndef USBDF_HID_MOUSE_H_
#define USBDF_HID_MOUSE_H_

#include "usbdc.h"
#include "usb_protocol_hid.h"

/** HID Mouse Class Button State Type */
enum hiddf_mouse_button_state { HID_MOUSE_BTN_UP, HID_MOUSE_BTN_DOWN };

/* HID Mosue Class Button Type */
enum hiddf_mouse_button_type { HID_MOUSE_LEFT_BTN = 0x01, HID_MOUSE_RIGHT_BTN = 0x02, HID_MOUSE_MIDDLE_BTN = 0x04 };

/* HID Mouse Class Pointer Move Type */
enum hiddf_mouse_move_type { HID_MOUSE_X_AXIS_MV = 0x01, HID_MOUSE_Y_AXIS_MV = 0x02, HID_MOUSE_SCROLL_MV = 0x03 };

/**
 * \brief Initialize the USB HID Mouse Function Driver
 * \return Operation status.
 */
int32_t hiddf_mouse_init(void);

/**
 * \brief Deinitialize the USB HID Mouse Function Driver
 * \return Operation status.
 */
int32_t hiddf_mouse_deinit(void);

/**
 * \brief Check whether HID Mouse Function is enabled
 * \return Operation status.
 * \return true HID Mouse Function is enabled
 * \return false HID Mouse Function is disabled
 */
bool hiddf_mouse_is_enabled(void);

/**
 * \brief Move the mouse pointer
 * \param pos     Signed value to move
 * \param type    HID mouse class pointer move type
 * \return Operation status.
 */
int32_t hiddf_mouse_move(int8_t pos, enum hiddf_mouse_move_type type);

/**
 * \brief Changes button state
 * \param b_state    New button state
 * \param type       Button type
 * \return Operation status.
 */
int32_t hiddf_mouse_button_change(enum hiddf_mouse_button_state b_state, enum hiddf_mouse_button_type type);

/**
 * \brief Return version
 */
uint32_t hiddf_mouse_get_version(void);

#endif /* USBDF_CDC_ACM_SER_H_ */
