/**
 * \file
 *
 * \brief USB Device Stack HID Generic Function Definition.
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

#ifndef USBDF_HID_GENERIC_H_
#define USBDF_HID_GENERIC_H_

#include "usbdc.h"
#include "usb_protocol_hid.h"

/** HID Generic Callback Type */
enum hiddf_generic_cb_type { HIDDF_GENERIC_CB_READ, HIDDF_GENERIC_CB_WRITE, HIDDF_GENERIC_CB_SET_REPORT };

/** HID Generic Set Report Callback. */
typedef bool (*hiddf_generic_set_report_t)(uint8_t *, uint16_t);

/**
 * \brief Initialize the USB HID Generic Function Driver
 * \param[in] report_desc Point to usr HID Report Descriptor
 * \param[in] len length of usr HID Report Descriptor
 * \return Operation status.
 */
int32_t hiddf_generic_init(const uint8_t *report_desc, uint32_t len);

/**
 * \brief Deinitialize the USB HID Generic Function Driver
 * \return Operation status.
 */
int32_t hiddf_generic_deinit(void);

/**
 * \brief Check whether HID Generic Function is enabled
 * \return Operation status.
 * \return true HID Generic Function is enabled
 * \return false HID Generic Function is disabled
 */
bool hiddf_generic_is_enabled(void);

/**
 * \brief USB HID Generic Function Read Data
 * \param[in] buf Pointer to the buffer which receives data
 * \param[in] size the size of data to be received
 * \return Operation status.
 */
int32_t hiddf_generic_read(uint8_t *buf, uint32_t size);

/**
 * \brief USB HID Generic Function Write Data
 * \param[in] buf Pointer to the buffer which stores data
 * \param[in] size the size of data to be sent
 * \return Operation status.
 */
int32_t hiddf_generic_write(uint8_t *buf, uint32_t size);

/**
 * \brief USB HID Generic Function Register Callback
 * \param[in] cb_type Callback type of HID Generic Function
 * \param[in] func Pointer to callback function
 * \return Operation status.
 */
int32_t hiddf_generic_register_callback(enum hiddf_generic_cb_type cb_type, FUNC_PTR func);

/**
 * \brief Return version
 */
uint32_t hiddf_generic_get_version(void);

#endif /* USBDF_HID_GENERIC_H_ */
