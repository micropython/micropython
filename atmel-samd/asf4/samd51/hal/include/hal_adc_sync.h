/**
 * \file
 *
 * \brief ADC functionality declaration.
 *
 * Copyright (C) 2014-2016 Atmel Corporation. All rights reserved.
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

#ifndef _HAL_ADC_SYNC_H_INCLUDED
#define _HAL_ADC_SYNC_H_INCLUDED

#include <hpl_adc_sync.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup doc_driver_hal_adc_sync
 *
 * @{
 */

/**
 * \brief ADC descriptor
 *
 * The ADC descriptor forward declaration.
 */
struct adc_sync_descriptor;

/**
 * \brief ADC descriptor
 */
struct adc_sync_descriptor {
	/** ADC device */
	struct _adc_sync_device device;
};

/**
 * \brief Initialize ADC
 *
 * This function initializes the given ADC descriptor.
 * It checks if the given hardware is not initialized and if the given hardware
 * is permitted to be initialized.
 *
 * \param[out] descr An ADC descriptor to initialize
 * \param[in] hw     The pointer to hardware instance
 * \param[in] func   The pointer to a set of functions pointers
 *
 * \return Initialization status.
 */
int32_t adc_sync_init(struct adc_sync_descriptor *const descr, void *const hw, void *const func);

/**
 * \brief Deinitialize ADC
 *
 * This function deinitializes the given ADC descriptor.
 * It checks if the given hardware is initialized and if the given hardware is
 * permitted to be deinitialized.
 *
 * \param[in] descr An ADC descriptor to deinitialize
 *
 * \return De-initialization status.
 */
int32_t adc_sync_deinit(struct adc_sync_descriptor *const descr);

/**
 * \brief Enable ADC
 *
 * Use this function to set the ADC peripheral to enabled state.
 *
 * \param[in] descr    Pointer to the ADC descriptor
 * \param[in] channel  Channel number
 *
 * \return Operation status
 *
 */
int32_t adc_sync_enable_channel(struct adc_sync_descriptor *const descr, const uint8_t channel);

/**
 * \brief Disable ADC
 *
 * Use this function to set the ADC peripheral to disabled state.
 *
 * \param[in] descr   Pointer to the ADC descriptor
 * \param[in] channel Channel number
 *
 * \return Operation status
 *
 */
int32_t adc_sync_disable_channel(struct adc_sync_descriptor *const descr, const uint8_t channel);

/**
 * \brief Read data from ADC
 *
 * \param[in] descr    The pointer to the ADC descriptor
 * \param[in] channel  Channel number
 * \param[in] buf      A buffer to read data to
 * \param[in] length   The size of a buffer
 *
 * \return The number of bytes read.
 */
int32_t adc_sync_read_channel(struct adc_sync_descriptor *const descr, const uint8_t channel, uint8_t *const buffer,
                              const uint16_t length);

/**
 * \brief Set ADC reference source
 *
 * This function sets ADC reference source.
 *
 * \param[in] descr     The pointer to the ADC descriptor
 * \param[in] reference A reference source to set
 *
 * \return Status of the ADC reference source setting.
 */
int32_t adc_sync_set_reference(struct adc_sync_descriptor *const descr, const adc_reference_t reference);

/**
 * \brief Set ADC resolution
 *
 * This function sets ADC resolution.
 *
 * \param[in] descr      The pointer to the ADC descriptor
 * \param[in] resolution A resolution to set
 *
 * \return Status of the ADC resolution setting.
 */
int32_t adc_sync_set_resolution(struct adc_sync_descriptor *const descr, const adc_resolution_t resolution);

/**
 * \brief Set ADC input source of a channel
 *
 * This function sets ADC positive and negative input sources.
 *
 * \param[in] descr     The pointer to the ADC descriptor
 * \param[in] pos_input A positive input source to set
 * \param[in] neg_input A negative input source to set
 * \param[in] channel   Channel number
 *
 * \return Status of the ADC channels setting.
 */
int32_t adc_sync_set_inputs(struct adc_sync_descriptor *const descr, const adc_pos_input_t pos_input,
                            const adc_neg_input_t neg_input, const uint8_t channel);

/**
 * \brief Set ADC conversion mode
 *
 * This function sets ADC conversion mode.
 *
 * \param[in] descr The pointer to the ADC descriptor
 * \param[in] mode  A conversion mode to set
 *
 * \return Status of the ADC conversion mode setting.
 */
int32_t adc_sync_set_conversion_mode(struct adc_sync_descriptor *const descr, const enum adc_conversion_mode mode);

/**
 * \brief Set ADC differential mode
 *
 * This function sets ADC differential mode.
 *
 * \param[in] descr   The pointer to the ADC descriptor
 * \param[in] channel Channel number
 * \param[in] mode    A differential mode to set
 *
 * \return Status of the ADC differential mode setting.
 */
int32_t adc_sync_set_channel_differential_mode(struct adc_sync_descriptor *const descr, const uint8_t channel,
                                               const enum adc_differential_mode mode);

/**
 * \brief Set ADC channel gain
 *
 * This function sets ADC channel gain.
 *
 * \param[in] descr   The pointer to the ADC descriptor
 * \param[in] channel Channel number
 * \param[in] gain    A gain to set
 *
 * \return Status of the ADC gain setting.
 */
int32_t adc_sync_set_channel_gain(struct adc_sync_descriptor *const descr, const uint8_t channel,
                                  const adc_gain_t gain);

/**
 * \brief Set ADC window mode
 *
 * This function sets ADC window mode.
 *
 * \param[in] descr The pointer to the ADC descriptor
 * \param[in] mode  A window mode to set
 *
 * \return Status of the ADC window mode setting.
 */
int32_t adc_sync_set_window_mode(struct adc_sync_descriptor *const descr, const adc_window_mode_t mode);

/**
 * \brief Set ADC thresholds
 *
 * This function sets ADC positive and negative thresholds.
 *
 * \param[in] descr         The pointer to the ADC descriptor
 * \param[in] low_threshold A lower thresholds to set
 * \param[in] up_threshold  An upper thresholds to set
 *
 * \return Status of the ADC thresholds setting.
 */
int32_t adc_sync_set_thresholds(struct adc_sync_descriptor *const descr, const adc_threshold_t low_threshold,
                                const adc_threshold_t up_threshold);

/**
 * \brief Retrieve threshold state
 *
 * This function retrieves ADC threshold state.
 *
 * \param[in] descr  The pointer to the ADC descriptor
 * \param[out] state The threshold state
 *
 * \return The state of ADC thresholds state retrieving.
 */
int32_t adc_sync_get_threshold_state(const struct adc_sync_descriptor *const descr,
                                     adc_threshold_status_t *const           state);

/**
 * \brief Check if conversion is complete
 *
 * This function checks if the ADC has finished the conversion.
 *
 * \param[in] descr   The pointer to the ADC descriptor
 * \param[in] channel Channel number
 *
 * \return The status of ADC conversion completion checking.
 * \retval 1 The conversion is complete
 * \retval 0 The conversion is not complete
 */
int32_t adc_sync_is_channel_conversion_complete(const struct adc_sync_descriptor *const descr, const uint8_t channel);

/**
 * \brief Retrieve the current driver version
 *
 * \return Current driver version.
 */
uint32_t adc_sync_get_version(void);
/**@}*/

#ifdef __cplusplus
}
#endif

#include <hpl_missing_features.h>

#endif /* _HAL_ADC_SYNC_H_INCLUDED */
