/**
 * \file
 *
 * \brief Status code definitions.
 *
 * This file defines various status codes returned by functions,
 * indicating success or failure as well as what kind of failure.
 *
 * Copyright (C) 2012-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef STATUS_CODES_H_INCLUDED
#define STATUS_CODES_H_INCLUDED

#include <stdint.h>

/**
 * \defgroup group_sam0_utils_status_codes Status Codes
 *
 * \ingroup group_sam0_utils
 *
 * @{
 */

/** Mask to retrieve the error category of a status code. */
#define STATUS_CATEGORY_MASK  0xF0

/** Mask to retrieve the error code within the category of a status code. */
#define STATUS_ERROR_MASK     0x0F

/** Status code error categories. */
enum status_categories {
	STATUS_CATEGORY_OK                = 0x00,
	STATUS_CATEGORY_COMMON            = 0x10,
	STATUS_CATEGORY_ANALOG            = 0x30,
	STATUS_CATEGORY_COM               = 0x40,
	STATUS_CATEGORY_IO                = 0x50,
};

/**
 * Status code that may be returned by shell commands and protocol
 * implementations.
 *
 * \note Any change to these status codes and the corresponding
 * message strings is strictly forbidden. New codes can be added,
 * however, but make sure that any message string tables are updated
 * at the same time.
 */
enum status_code {
	STATUS_OK                         = STATUS_CATEGORY_OK     | 0x00,
	STATUS_VALID_DATA                 = STATUS_CATEGORY_OK     | 0x01,
	STATUS_NO_CHANGE                  = STATUS_CATEGORY_OK     | 0x02,
	STATUS_ABORTED                    = STATUS_CATEGORY_OK     | 0x04,
	STATUS_BUSY                       = STATUS_CATEGORY_OK     | 0x05,
	STATUS_SUSPEND                    = STATUS_CATEGORY_OK     | 0x06,

	STATUS_ERR_IO                     = STATUS_CATEGORY_COMMON | 0x00,
	STATUS_ERR_REQ_FLUSHED            = STATUS_CATEGORY_COMMON | 0x01,
	STATUS_ERR_TIMEOUT                = STATUS_CATEGORY_COMMON | 0x02,
	STATUS_ERR_BAD_DATA               = STATUS_CATEGORY_COMMON | 0x03,
	STATUS_ERR_NOT_FOUND              = STATUS_CATEGORY_COMMON | 0x04,
	STATUS_ERR_UNSUPPORTED_DEV        = STATUS_CATEGORY_COMMON | 0x05,
	STATUS_ERR_NO_MEMORY              = STATUS_CATEGORY_COMMON | 0x06,
	STATUS_ERR_INVALID_ARG            = STATUS_CATEGORY_COMMON | 0x07,
	STATUS_ERR_BAD_ADDRESS            = STATUS_CATEGORY_COMMON | 0x08,
	STATUS_ERR_BAD_FORMAT             = STATUS_CATEGORY_COMMON | 0x0A,
	STATUS_ERR_BAD_FRQ                = STATUS_CATEGORY_COMMON | 0x0B,
	STATUS_ERR_DENIED                 = STATUS_CATEGORY_COMMON | 0x0c,
	STATUS_ERR_ALREADY_INITIALIZED    = STATUS_CATEGORY_COMMON | 0x0d,
	STATUS_ERR_OVERFLOW               = STATUS_CATEGORY_COMMON | 0x0e,
	STATUS_ERR_NOT_INITIALIZED        = STATUS_CATEGORY_COMMON | 0x0f,

	STATUS_ERR_SAMPLERATE_UNAVAILABLE = STATUS_CATEGORY_ANALOG | 0x00,
	STATUS_ERR_RESOLUTION_UNAVAILABLE = STATUS_CATEGORY_ANALOG | 0x01,

	STATUS_ERR_BAUDRATE_UNAVAILABLE   = STATUS_CATEGORY_COM    | 0x00,
	STATUS_ERR_PACKET_COLLISION       = STATUS_CATEGORY_COM    | 0x01,
	STATUS_ERR_PROTOCOL               = STATUS_CATEGORY_COM    | 0x02,

	STATUS_ERR_PIN_MUX_INVALID        = STATUS_CATEGORY_IO     | 0x00,
};
typedef enum status_code status_code_genare_t;

/**
  Status codes used by MAC stack.
 */
enum status_code_wireless {
	//STATUS_OK               =  0, //!< Success
	ERR_IO_ERROR            =  -1, //!< I/O error
	ERR_FLUSHED             =  -2, //!< Request flushed from queue
	ERR_TIMEOUT             =  -3, //!< Operation timed out
	ERR_BAD_DATA            =  -4, //!< Data integrity check failed
	ERR_PROTOCOL            =  -5, //!< Protocol error
	ERR_UNSUPPORTED_DEV     =  -6, //!< Unsupported device
	ERR_NO_MEMORY           =  -7, //!< Insufficient memory
	ERR_INVALID_ARG         =  -8, //!< Invalid argument
	ERR_BAD_ADDRESS         =  -9, //!< Bad address
	ERR_BUSY                =  -10, //!< Resource is busy
	ERR_BAD_FORMAT          =  -11, //!< Data format not recognized
	ERR_NO_TIMER            =  -12, //!< No timer available
	ERR_TIMER_ALREADY_RUNNING   =  -13, //!< Timer already running
	ERR_TIMER_NOT_RUNNING   =  -14, //!< Timer not running

	/**
	 * \brief Operation in progress
	 *
	 * This status code is for driver-internal use when an operation
	 * is currently being performed.
	 *
	 * \note Drivers should never return this status code to any
	 * callers. It is strictly for internal use.
	 */
	OPERATION_IN_PROGRESS	= -128,
};

typedef enum status_code_wireless status_code_t;

/** @} */

#endif /* STATUS_CODES_H_INCLUDED */
