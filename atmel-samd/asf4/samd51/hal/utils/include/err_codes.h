/**
 * \file
 *
 * \brief Error code definitions.
 *
 * This file defines various status codes returned by functions,
 * indicating success or failure as well as what kind of failure.
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
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

#ifndef ERROR_CODES_H_INCLUDED
#define ERROR_CODES_H_INCLUDED

#define ERR_NONE 0
#define ERR_INVALID_DATA -1
#define ERR_NO_CHANGE -2
#define ERR_ABORTED -3
#define ERR_BUSY -4
#define ERR_SUSPEND -5
#define ERR_IO -6
#define ERR_REQ_FLUSHED -7
#define ERR_TIMEOUT -8
#define ERR_BAD_DATA -9
#define ERR_NOT_FOUND -10
#define ERR_UNSUPPORTED_DEV -11
#define ERR_NO_MEMORY -12
#define ERR_INVALID_ARG -13
#define ERR_BAD_ADDRESS -14
#define ERR_BAD_FORMAT -15
#define ERR_BAD_FRQ -16
#define ERR_DENIED -17
#define ERR_ALREADY_INITIALIZED -18
#define ERR_OVERFLOW -19
#define ERR_NOT_INITIALIZED -20
#define ERR_SAMPLERATE_UNAVAILABLE -21
#define ERR_RESOLUTION_UNAVAILABLE -22
#define ERR_BAUDRATE_UNAVAILABLE -23
#define ERR_PACKET_COLLISION -24
#define ERR_PROTOCOL -25
#define ERR_PIN_MUX_INVALID -26
#define ERR_UNSUPPORTED_OP -27
#define ERR_NO_RESOURCE -28
#define ERR_NOT_READY -29
#define ERR_FAILURE -30
#define ERR_WRONG_LENGTH -31

#endif
