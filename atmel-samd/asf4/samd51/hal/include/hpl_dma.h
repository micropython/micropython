/**
 * \file
 *
 * \brief DMA related functionality declaration.
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

#ifndef _HPL_DMA_H_INCLUDED
#define _HPL_DMA_H_INCLUDED

/**
 * \addtogroup HPL DMA
 *
 * \section hpl_dma_rev Revision History
 * - v1.0.0 Initial Release
 *
 *@{
 */

#include <compiler.h>
#include <hpl_irq.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _dma_resource;

/**
 * \brief DMA callback types
 */
enum _dma_callback_type { DMA_TRANSFER_COMPLETE_CB, DMA_TRANSFER_ERROR_CB };

/**
 * \brief DMA interrupt callbacks
 */
struct _dma_callbacks {
	void (*transfer_done)(struct _dma_resource *resource);
	void (*error)(struct _dma_resource *resource);
};

/**
 * \brief DMA resource structure
 */
struct _dma_resource {
	struct _dma_callbacks dma_cb;
	void *                back;
};

/**
 * \brief Initialize DMA
 *
 * This function does low level DMA configuration.
 *
 * \return initialize status
 */
int32_t _dma_init(void);

/**
 * \brief Set destination address
 *
 * \param[in] channel DMA channel to set destination address for
 * \param[in] dst Destination address
 *
 * \return setting status
 */
int32_t _dma_set_destination_address(const uint8_t channel, const void *const dst);

/**
 * \brief Set source address
 *
 * \param[in] channel DMA channel to set source address for
 * \param[in] src Source address
 *
 * \return setting status
 */
int32_t _dma_set_source_address(const uint8_t channel, const void *const src);

/**
 * \brief Enable/disable source address incrementation during DMA transaction
 *
 * \param[in] channel DMA channel to set source address for
 * \param[in] enable True to enable, false to disable
 *
 * \return status of operation
 */
int32_t _dma_srcinc_enable(const uint8_t channel, const bool enable);

/**
 * \brief Set the amount of data to be transfered per transaction
 *
 * \param[in] channel DMA channel to set data amount for
 * \param[in] amount Data amount
 *
 * \return status of operation
 */
int32_t _dma_set_data_amount(const uint8_t channel, const uint32_t amount);

/**
 * \brief Trigger DMA transaction on the given channel
 *
 * \param[in] channel DMA channel to trigger transaction on
 *
 * \return status of operation
 */
int32_t _dma_enable_transaction(const uint8_t channel, const bool software_trigger);

/**
 * \brief Retrieves DMA resource structure
 *
 * \param[out] resource The resource to be retrieved
 * \param[in] channel DMA channel to retrieve structure for
 *
 * \return status of operation
 */
int32_t _dma_get_channel_resource(struct _dma_resource **resource, const uint8_t channel);

/**
 * \brief Enable/disable DMA interrupt
 *
 * \param[in] channel DMA channel to enable/disable interrupt for
 * \param[in] type The type of interrupt to disable/enable if applicable
 * \param[in] state Enable or disable
 */
void _dma_set_irq_state(const uint8_t channel, const enum _dma_callback_type type, const bool state);

#ifdef __cplusplus
}
#endif

#endif /* HPL_DMA_H_INCLUDED */
