/**
 * \file
 *
 * \brief USB Device Stack CDC ACM Function Definition.
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

#ifndef USBDF_CDC_ACM_SER_H_
#define USBDF_CDC_ACM_SER_H_

#include "usbdc.h"
#include "usb_protocol_cdc.h"

/** CDC ACM Class Callback Type */
enum cdcdf_acm_cb_type { CDCDF_ACM_CB_READ, CDCDF_ACM_CB_WRITE, CDCDF_ACM_CB_LINE_CODING_C, CDCDF_ACM_CB_STATE_C };

/** CDC ACM Notify Line State Callback. */
typedef void (*cdcdf_acm_notify_state_t)(uint16_t);

/** CDC ACM Set Line Coding Callback. */
typedef bool (*cdcdf_acm_set_line_coding_t)(struct usb_cdc_line_coding *);

/**
 * \brief Initialize the USB CDC ACM Function Driver
 * \return Operation status.
 */
int32_t cdcdf_acm_init(void);

/**
 * \brief Deinitialize the USB CDC ACM Function Driver
 * \return Operation status.
 */
void cdcdf_acm_deinit(void);

/**
 * \brief USB CDC ACM Function Read Data
 * \param[in] buf Pointer to the buffer which receives data
 * \param[in] size the size of data to be received
 * \return Operation status.
 */
int32_t cdcdf_acm_read(uint8_t *buf, uint32_t size);

/**
 * \brief USB CDC ACM Function Write Data
 * \param[in] buf Pointer to the buffer which stores data
 * \param[in] size the size of data to be sent
 * \return Operation status.
 */
int32_t cdcdf_acm_write(uint8_t *buf, uint32_t size);

/**
 * \brief USB CDC ACM Stop the currnet data transfer
 */
void cdcdf_acm_stop_xfer(void);

/**
 * \brief USB CDC ACM Function Register Callback
 * \param[in] cb_type Callback type of CDC ACM Function
 * \param[in] func Pointer to callback function
 * \return Operation status.
 */
int32_t cdcdf_acm_register_callback(enum cdcdf_acm_cb_type cb_type, FUNC_PTR func);

/**
 * \brief Check whether CDC ACM Function is enabled
 * \return Operation status.
 * \return true CDC ACM Function is enabled
 * \return false CDC ACM Function is disabled
 */
bool cdcdf_acm_is_enabled(void);

/**
 * \brief Return the CDC ACM line coding structure start address
 * \return Pointer to USB CDC ACM line coding data.
 */
const struct usb_cdc_line_coding *cdcdf_acm_get_line_coding(void);

/**
 * \brief Return version
 */
uint32_t cdcdf_acm_get_version(void);

#endif /* USBDF_CDC_ACM_SER_H_ */
