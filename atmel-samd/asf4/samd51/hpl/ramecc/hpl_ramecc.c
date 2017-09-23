/**
 * \file
 *
 * \brief Generic RAMECC related functionality.
 *
 * Copyright (C) 2016 -2107 Atmel Corporation. All rights reserved.
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

#include <utils.h>
#include <utils_assert.h>
#include <hpl_ramecc.h>

/* RAMECC device descriptor */
struct _ramecc_device device;

/**
 * \brief Initialize RAMECC
 */
int32_t _ramecc_init(void)
{
	if (hri_ramecc_get_STATUS_ECCDIS_bit(RAMECC)) {
		return ERR_ABORTED;
	}

	NVIC_DisableIRQ(RAMECC_IRQn);
	NVIC_ClearPendingIRQ(RAMECC_IRQn);
	NVIC_EnableIRQ(RAMECC_IRQn);

	return ERR_NONE;
}

void _ramecc_register_callback(const enum _ramecc_callback_type type, ramecc_cb_t cb)
{
	if (RAMECC_DUAL_ERROR_CB == type) {
		device.ramecc_cb.dual_bit_err = cb;
		hri_ramecc_write_INTEN_DUALE_bit(RAMECC, NULL != cb);
	} else if (RAMECC_SINGLE_ERROR_CB == type) {
		device.ramecc_cb.single_bit_err = cb;
		hri_ramecc_write_INTEN_SINGLEE_bit(RAMECC, NULL != cb);
	}
}

/**
 * \internal RAMECC interrupt handler
 */
void RAMECC_Handler(void)
{
	struct _ramecc_device *dev      = (struct _ramecc_device *)&device;
	volatile uint32_t      int_mask = hri_ramecc_read_INTFLAG_reg(RAMECC);

	if (int_mask & RAMECC_INTFLAG_DUALE && dev->ramecc_cb.dual_bit_err) {
		dev->ramecc_cb.dual_bit_err((uint32_t)hri_ramecc_read_ERRADDR_reg(RAMECC));
	} else if (int_mask & RAMECC_INTFLAG_SINGLEE && dev->ramecc_cb.single_bit_err) {
		dev->ramecc_cb.single_bit_err((uint32_t)hri_ramecc_read_ERRADDR_reg(RAMECC));
	} else {
		return;
	}
}
