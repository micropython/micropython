/**
 * \file
 *
 * \brief DAC related functionality declaration.
 *
 * Copyright (C) 2014-2017 Atmel Corporation. All rights reserved.
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
#ifndef _HPL_DAC_H_INCLUDED
#define _HPL_DAC_H_INCLUDED

/**
 * \addtogroup hpl__dac__group DAC HPL APIs
 * See interface description here: \ref hpl__dac__doc
 *
 */

/**@{*/

#include <compiler.h>
#include "hpl_irq.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * DAC hardware Channel Number
 */

#ifndef CHANNEL_NUM
#define CHANNEL_NUM 1
#endif

/**
 * \brief DAC callback types
 */
enum _dac_callback_type { DAC_DEVICE_CONVERSION_DONE_CB, DAC_DEVICE_ERROR_CB };

/**
 * \brief DAC device structure
 *
 * The DAC device structure forward declaration.
 */
struct _dac_async_device;

/**
 * \brief DAC interrupt handlers structure
 */
struct _dac_callback {
	/** Transmit ready(Data Buffer empty) handler */
	void (*tx_ready_cb)(struct _dac_async_device *device, const uint8_t ch);
	/** Error handler */
	void (*tx_error_cb)(struct _dac_async_device *device, const uint8_t ch);
};

/**
 * \brief DAC descriptor device structure.
 */
struct _dac_async_device {
	struct _dac_callback   dac_cb; /*!< Interrupt handers  */
	struct _irq_descriptor irq;    /*!< Interrupt descriptor */
	void *                 hw;     /*!< Hardware module instance handler */
};

/**
 * \brief Initialize asynchronous DAC.
 *
 * This function does low level DAC configuration.
 *
 * \param[in] device The pointer to DAC device instance
 * \param[in] hw The pointer to hardware instance
 *
 * \return Initialization status
 */
int32_t _dac_async_init(struct _dac_async_device *const device, void *const hw);

/**
 * \brief Deinitialize DAC.
 *
 * \param[in] device The pointer to DAC device instance
 */
void _dac_async_deinit(struct _dac_async_device *const device);

/**
 * \brief Enable DAC Channel.
 *
 * \param[in] device The pointer to DAC device instance
 * \param[in] ch  channel number
 */
void _dac_async_enable_channel(struct _dac_async_device *const device, const uint8_t ch);

/**
 * \brief Disable DAC Channel.
 *
 * \param[in] device The pointer to DAC device instance
 * \param[in] ch  channel number
 */
void _dac_async_disable_channel(struct _dac_async_device *const device, const uint8_t ch);

/**
 * \brief Checks if DAC channel is enabled
 *
 * \param[in] device The pointer to DAC device instance
 * \param[in] ch  channel number
 *
 * \return true channel is enabled, false otherwise
 */
bool _dac_async_is_channel_enable(struct _dac_async_device *const device, const uint8_t ch);

/**
 * \brief Write DAC data for output.
 *
 * \param[in] device The pointer to DAC device instance
 * \param[in] data Digital data which to be converted
 * \param[in] ch The channel selected to output
 */
void _dac_async_write_data(struct _dac_async_device *const device, const uint16_t data, const uint8_t ch);

/**
 * \brief Enable/disable DAC interrupt
 *
 * param[in] device The pointer to DAC device instance
 * param[in] type The type of interrupt to disable/enable if applicable
 * param[in] state Enable or disable
 */
void _dac_async_set_irq_state(struct _dac_async_device *const device, const enum _dac_callback_type type,
                              const bool state);

#ifdef __cplusplus
}
#endif

/**@}*/

/**
 * \page hpl__dac__doc DAC HPL Interface Driver
 *
 * \section hpl__dac__desc DAC HPL Description
 *
 * The following device can use this HPL driver
 * - Atmel | SMART SAM D21
 *
 * \section hpl__dac__imple DAC HPL Implements
 * - \subpage hpl__dac__hw_module1__doc
 */

#endif /* _HPL_DAC_H_INCLUDED */
