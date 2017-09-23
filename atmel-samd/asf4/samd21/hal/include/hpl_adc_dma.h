/**
 * \file
 *
 * \brief ADC related functionality declaration.
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

#ifndef _HPL_ADC_DMA_H_INCLUDED
#define _HPL_ADC_DMA_H_INCLUDED

/**
 * \addtogroup HPL ADC
 *
 * \section hpl_dma_adc_rev Revision History
 * - v1.0.0 Initial Release
 *
 *@{
 */

#include <hpl_adc_sync.h>
#include <hpl_irq.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief ADC device structure
 *
 * The ADC device structure forward declaration.
 */
struct _adc_dma_device;

/**
 * \brief ADC callback types
 */
enum _adc_dma_callback_type { ADC_DMA_DEVICE_COMPLETE_CB, ADC_DMA_DEVICE_ERROR_CB };

/**
 * \brief ADC interrupt callbacks
 */
struct _adc_dma_callbacks {
	void (*complete)(struct _adc_dma_device *device, const uint16_t data);
	void (*error)(struct _adc_dma_device *device);
};

/**
 * \brief ADC descriptor device structure
 */
struct _adc_dma_device {
	struct _adc_dma_callbacks adc_dma_cb;
	struct _irq_descriptor    irq;
	void *                    hw;
};

/**
 * \name HPL functions
 */
//@{
/**
 * \brief Initialize synchronous ADC
 *
 * This function does low level ADC configuration.
 *
 * param[in] device The pointer to ADC device instance
 * param[in] hw The pointer to hardware instance
 *
 * \return Initialization status
 */
int32_t _adc_dma_init(struct _adc_dma_device *const device, void *const hw);

/**
 * \brief Deinitialize ADC
 *
 * \param[in] device The pointer to ADC device instance
 */
void _adc_dma_deinit(struct _adc_dma_device *const device);

/**
 * \brief Enable ADC peripheral
 *
 * \param[in] device   The pointer to ADC device instance
 * \param[in] channel  Channel number
 */
void _adc_dma_enable_channel(struct _adc_dma_device *const device, const uint8_t channel);

/**
 * \brief Disable ADC peripheral
 *
 * \param[in] device   The pointer to ADC device instance
 * \param[in] channel  Channel number
 */
void _adc_dma_disable_channel(struct _adc_dma_device *const device, const uint8_t channel);

/**
 * \brief Return address of ADC DMA source
 *
 * \param[in] device The pointer to ADC device instance
 *
 * \return ADC DMA source address
 */
uint32_t _adc_get_source_for_dma(struct _adc_dma_device *const device);

/**
 * \brief Retrieve ADC conversion data size
 *
 * \param[in] device The pointer to ADC device instance
 *
 * \return The data size in bytes
 */
uint8_t _adc_dma_get_data_size(const struct _adc_dma_device *const device);

/**
 * \brief Check if conversion is done
 *
 * \param[in] device   The pointer to ADC device instance
 *
 * \return The status of conversion
 * \retval true The conversion is done
 * \retval false The conversion is not done
 */
bool _adc_dma_is_conversion_done(const struct _adc_dma_device *const device);

/**
 * \brief Make conversion
 *
 * \param[in] device The pointer to ADC device instance
 */
void _adc_dma_convert(struct _adc_dma_device *const device);

/**
 * \brief Set reference source
 *
 * \param[in] device The pointer to ADC device instance
 * \param[in] reference A reference source to set
 */
void _adc_dma_set_reference_source(struct _adc_dma_device *const device, const adc_reference_t reference);

/**
 * \brief Set resolution
 *
 * \param[in] device The pointer to ADC device instance
 * \param[in] resolution A resolution to set
 */
void _adc_dma_set_resolution(struct _adc_dma_device *const device, const adc_resolution_t resolution);

/**
 * \brief Set ADC input source of a channel
 *
 * \param[in] device    The pointer to ADC device instance
 * \param[in] pos_input A positive input source to set
 * \param[in] neg_input A negative input source to set
 * \param[in] channel   Channel number
 */
void _adc_dma_set_inputs(struct _adc_dma_device *const device, const adc_pos_input_t pos_input,
                         const adc_neg_input_t neg_input, const uint8_t channel);

/**
 * \brief Set conversion mode
 *
 * \param[in] device The pointer to ADC device instance
 * \param[in] mode A conversion mode to set
 */
void _adc_dma_set_conversion_mode(struct _adc_dma_device *const device, const enum adc_conversion_mode mode);

/**
 * \brief Set differential mode
 *
 * \param[in] device  The pointer to ADC device instance
 * \param[in] channel Channel number
 * \param[in] mode    A differential mode to set
 */
void _adc_dma_set_channel_differential_mode(struct _adc_dma_device *const device, const uint8_t channel,
                                            const enum adc_differential_mode mode);

/**
 * \brief Set gain
 *
 * \param[in] device  The pointer to ADC device instance
 * \param[in] channel Channel number
 * \param[in] gain    A gain to set
 */
void _adc_dma_set_channel_gain(struct _adc_dma_device *const device, const uint8_t channel, const adc_gain_t gain);

/**
 * \brief Set window mode
 *
 * \param[in] device The pointer to ADC device instance
 * \param[in] mode A mode to set
 */
void _adc_dma_set_window_mode(struct _adc_dma_device *const device, const adc_window_mode_t mode);

/**
 * \brief Set thresholds
 *
 * \param[in] device        The pointer to ADC device instance
 * \param[in] low_threshold A lower thresholds to set
 * \param[in] up_threshold  An upper thresholds to set
 */
void _adc_dma_set_thresholds(struct _adc_dma_device *const device, const adc_threshold_t low_threshold,
                             const adc_threshold_t up_threshold);

/**
 * \brief Retrieve threshold state
 *
 * \param[in] device The pointer to ADC device instance
 * \param[out] state The threshold state
 */
void _adc_dma_get_threshold_state(const struct _adc_dma_device *const device, adc_threshold_status_t *const state);

//@}

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* _HPL_ADC_DMA_H_INCLUDED */
