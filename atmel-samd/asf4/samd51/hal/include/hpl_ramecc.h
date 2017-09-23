/**
 * \file
 *
 * \brief RAMECC related functionality declaration.
 *
 * Copyright (C) 2016 Atmel Corporation. All rights reserved.
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

#ifndef _HPL_RAMECC_H_INCLUDED
#define _HPL_RAMECC_H_INCLUDED

/**
 * \addtogroup HPL RAMECC
 *
 * \section hpl_ramecc_rev Revision History
 * - v1.0.0 Initial Release
 *
 *@{
 */

#include <compiler.h>
#include <hpl_irq.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief RAMECC callback type
 */
typedef void (*ramecc_cb_t)(const uint32_t data);

/**
 * \brief RAMECC callback types
 */
enum _ramecc_callback_type { RAMECC_DUAL_ERROR_CB, RAMECC_SINGLE_ERROR_CB };

/**
 * \brief RAMECC interrupt callbacks
 */
struct _ramecc_callbacks {
	ramecc_cb_t dual_bit_err;
	ramecc_cb_t single_bit_err;
};

/**
 * \brief RAMECC device structure
 */
struct _ramecc_device {
	struct _ramecc_callbacks ramecc_cb;
	struct _irq_descriptor   irq;
};

/**
 * \brief Initialize RAMECC
 *
 * This function does low level RAMECC configuration.
 *
 * \return initialize status
 */
int32_t _ramecc_init(void);

/**
 * \brief Register RAMECC callback
 *
 * \param[in] type The type of callback
 * \param[in] cb A callback function
 */
void _ramecc_register_callback(const enum _ramecc_callback_type type, ramecc_cb_t cb);

#ifdef __cplusplus
}
#endif

#endif /* _HPL_RAMECC_H_INCLUDED */
