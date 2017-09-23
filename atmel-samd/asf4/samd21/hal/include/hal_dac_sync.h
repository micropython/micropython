/**
 * \file
 *
 * \brief DAC functionality declaration.
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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

#ifndef HAL_SYNC_DAC_H_INCLUDED
#define HAL_SYNC_DAC_H_INCLUDED

#include <hpl_dac_sync.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup doc_driver_hal_dac_sync
 *
 *@{
 */

/**
 * \brief DAC synchronous descriptor.
 *
 * The DAC descriptor forward declaration.
 */
struct dac_sync_descriptor;

/**
 * \brief DAC synchronous channel descriptor.
 *
 */
struct dac_sync_channel {
	/** Pointer to buffer what to be converted */
	uint16_t *buffer;
	/** The length of buffer */
	uint32_t length;
};

/**
 * \brief DAC synchronous descriptor
 */
struct dac_sync_descriptor {
	/** DAC device */
	struct _dac_sync_device device;
	/** DAC selected channel */
	struct dac_sync_channel sel_ch[CHANNEL_NUM];
};

/** \brief Initialize the DAC HAL instance and hardware.
 *  \param[out] descr A DAC descriptor to initialize
 *  \param[in] hw The pointer to hardware instance
 *  \return Operation status.
 */
int32_t dac_sync_init(struct dac_sync_descriptor *const descr, void *const hw);

/** \brief Deinitialize the DAC HAL instance and hardware.
 *  \param[in] descr Pointer to the HAL DAC descriptor
 *  \return Operation status.
 */
int32_t dac_sync_deinit(struct dac_sync_descriptor *const descr);

/** \brief Enable DAC channel.
 *  \param[in] descr Pointer to the HAL DAC descriptor
 *  \param[in] ch  Channel number
 *  \return Operation status.
 */
int32_t dac_sync_enable_channel(struct dac_sync_descriptor *const descr, const uint8_t ch);

/** \brief Disable DAC channel.
 *  \param[in] descr Pointer to the HAL DAC descriptor
 *  \param[in] ch  Channel number
 *  \return Operation status.
 */
int32_t dac_sync_disable_channel(struct dac_sync_descriptor *const descr, const uint8_t ch);

/** \brief DAC converts digital data to analog output.
 *  \param[in] descr   Pointer to the HAL DAC descriptor
 *  \param[in] ch the  Channel selected to output
 *  \param[in] buffer  Pointer to digital data or buffer
 *  \param[in] length  The number of elements in the buffer
 *  \return Operation status.
 */
int32_t dac_sync_write(struct dac_sync_descriptor *const descr, const uint8_t ch, uint16_t *buffer, uint32_t length);

/** \brief Get DAC driver version
 *
 *  \return Current driver version.
 */
uint32_t dac_sync_get_version(void);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* HAL_DAC_H_INCLUDED */
