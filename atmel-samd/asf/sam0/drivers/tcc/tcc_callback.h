/**
 * \file
 *
 * \brief SAM TCC - Timer Counter for Control Applications Callback Driver
 *
 * Copyright (C) 2013-2015 Atmel Corporation. All rights reserved.
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


#ifndef TCC_CALLBACK_H_INCLUDED
#define TCC_CALLBACK_H_INCLUDED

#include "tcc.h"
#include <system_interrupt.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__DOXYGEN__)
extern void *_tcc_instances[TCC_INST_NUM];
#endif


/**
 * \name Callback Management
 * {@
 */

enum status_code tcc_register_callback(
		struct tcc_module *const module,
		tcc_callback_t callback_func,
		const enum tcc_callback callback_type);

enum status_code tcc_unregister_callback(
		struct tcc_module *const module,
		const enum tcc_callback callback_type);

void tcc_enable_callback(
		struct tcc_module *const module,
		const enum tcc_callback callback_type);

void tcc_disable_callback(
		struct tcc_module *const module,
		const enum tcc_callback callback_type);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* TCC_CALLBACK_H_INCLUDED */
