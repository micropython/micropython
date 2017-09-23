/**
 * \file
 *
 * \brief SAM Analog Digital Converter
 *
 * Copyright (C) 2015-2017 Atmel Corporation. All rights reserved.
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

#include <hpl_adc_async.h>
#include <hpl_adc_dma.h>
#include <hpl_adc_sync.h>
#include <utils_assert.h>
#include <utils_repeat_macro.h>
#include <hpl_adc_config.h>

#define ADC_CONFIGURATION(n)                                                                                           \
	{                                                                                                                  \
		(n), (CONF_ADC_##n##_RUNSTDBY << ADC_CTRLA_RUNSTDBY_Pos) | (CONF_ADC_##n##_ENABLE << ADC_CTRLA_ENABLE_Pos),    \
		    (CONF_ADC_##n##_REFCOMP << ADC_REFCTRL_REFCOMP_Pos) | ADC_REFCTRL_REFSEL(CONF_ADC_##n##_REFSEL),           \
		    ADC_AVGCTRL_ADJRES(CONF_ADC_##n##_ADJRES) | ADC_AVGCTRL_SAMPLENUM(CONF_ADC_##n##_SAMPLENUM),               \
		    ADC_SAMPCTRL_SAMPLEN(CONF_ADC_##n##_SAMPLEN),                                                              \
		    ADC_CTRLB_PRESCALER(CONF_ADC_##n##_PRESCALER) | (CONF_ADC_##n##_RESSEL << ADC_CTRLB_RESSEL_Pos)            \
		        | (CONF_ADC_##n##_CORREN << ADC_CTRLB_CORREN_Pos) | (CONF_ADC_##n##_FREERUN << ADC_CTRLB_FREERUN_Pos)  \
		        | (CONF_ADC_##n##_LEFTADJ << ADC_CTRLB_LEFTADJ_Pos)                                                    \
		        | (CONF_ADC_##n##_DIFFMODE << ADC_CTRLB_DIFFMODE_Pos),                                                 \
		    CONF_ADC_##n##_WINMODE,                                                                                    \
		    ADC_INPUTCTRL_GAIN(CONF_ADC_##n##_GAIN) | ADC_INPUTCTRL_INPUTOFFSET(CONF_ADC_##n##_INPUTOFFSET)            \
		        | ADC_INPUTCTRL_INPUTSCAN(CONF_ADC_##n##_INPUTSCAN)                                                    \
		        | (CONF_ADC_##n##_MUXNEG << ADC_INPUTCTRL_MUXNEG_Pos)                                                  \
		        | (CONF_ADC_##n##_MUXPOS << ADC_INPUTCTRL_MUXPOS_Pos),                                                 \
		    (CONF_ADC_##n##_WINMONEO << ADC_EVCTRL_WINMONEO_Pos)                                                       \
		        | (CONF_ADC_##n##_RESRDYEO << ADC_EVCTRL_RESRDYEO_Pos)                                                 \
		        | (CONF_ADC_##n##_SYNCEI << ADC_EVCTRL_SYNCEI_Pos)                                                     \
		        | (CONF_ADC_##n##_STARTEI << ADC_EVCTRL_STARTEI_Pos),                                                  \
		    CONF_ADC_##n##_WINLT, CONF_ADC_##n##_WINUT, ADC_GAINCORR_GAINCORR(CONF_ADC_##n##_GAINCORR),                \
		    ADC_OFFSETCORR_OFFSETCORR(CONF_ADC_##n##_OFFSETCORR), (CONF_ADC_##n##_DBGRUN << ADC_DBGCTRL_DBGRUN_Pos),   \
	}

/**
 * \brief ADC configuration type
 */
struct adc_configuration {
	uint8_t                  number;
	hri_adc_ctrla_reg_t      ctrl_a;
	hri_adc_refctrl_reg_t    ref_ctrl;
	hri_adc_avgctrl_reg_t    avg_ctrl;
	hri_adc_sampctrl_reg_t   samp_ctrl;
	hri_adc_ctrlb_reg_t      ctrl_b;
	hri_adc_winctrl_reg_t    win_ctrl;
	hri_adc_inputctrl_reg_t  input_ctrl;
	hri_adc_evctrl_reg_t     ev_ctrl;
	hri_adc_winlt_reg_t      win_lt;
	hri_adc_winut_reg_t      win_ut;
	hri_adc_gaincorr_reg_t   gain_corr;
	hri_adc_offsetcorr_reg_t offset_corr;
	hri_adc_dbgctrl_reg_t    dbg_ctrl;
};

#define ADC_AMOUNT (CONF_ADC_0_ENABLE)

/**
 * \brief Array of ADC configurations
 */
static struct adc_configuration _adcs[] = {
#if CONF_ADC_0_ENABLE == 1
    ADC_CONFIGURATION(0),
#endif
};

/*!< Pointer to hpl device */
static struct _adc_async_device *_adc_dev = NULL;

static void _adc_set_resolution(void *const hw, const adc_resolution_t resolution);
static void _adc_set_conversion_mode(void *const hw, const enum adc_conversion_mode mode);
static void _adc_set_channel_differential_mode(void *const hw, const uint8_t channel,
                                               const enum adc_differential_mode mode);

/**
 * \brief Retrieve ordinal number of the given adc hardware instance
 */
static uint8_t _adc_get_hardware_index(const void *const hw)
{
	(void)hw;
	return 0;
}

/** \brief Return the pointer to register settings of specific ADC
 *  \param[in] hw_addr The hardware register base address.
 *  \return Pointer to register settings of specific ADC.
 */
static uint8_t _adc_get_regs(const uint32_t hw_addr)
{
	uint8_t n = _adc_get_hardware_index((const void *)hw_addr);
	uint8_t i;

	for (i = 0; i < sizeof(_adcs) / sizeof(struct adc_configuration); i++) {
		if (_adcs[i].number == n) {
			return i;
		}
	}

	ASSERT(false);
	return 0;
}

/**
 * \brief Retrieve IRQ number for the given hardware instance
 */
static uint8_t _adc_get_irq_num(const struct _adc_async_device *const device)
{
	(void)device;
	return ADC_IRQn;
}

/**
 * \brief Initialize ADC
 *
 * \param[in] hw The pointer to hardware instance
 * \param[in] i  The number of hardware instance
 */
static int32_t _adc_init(void *const hw, const uint8_t i)
{
	ASSERT(hw == ADC);

	hri_adc_wait_for_sync(hw);
	if (hri_adc_get_CTRLA_ENABLE_bit(hw)) {
		return ERR_DENIED;
	}
	hri_adc_set_CTRLA_SWRST_bit(hw);
	hri_adc_wait_for_sync(hw);

	hri_adc_write_REFCTRL_reg(hw, _adcs[i].ref_ctrl);
	hri_adc_write_AVGCTRL_reg(hw, _adcs[i].avg_ctrl);
	hri_adc_write_SAMPCTRL_reg(hw, _adcs[i].samp_ctrl);
	hri_adc_write_EVCTRL_reg(hw, _adcs[i].ev_ctrl);
	hri_adc_write_GAINCORR_reg(hw, _adcs[i].gain_corr);
	hri_adc_write_OFFSETCORR_reg(hw, _adcs[i].offset_corr);
	hri_adc_write_DBGCTRL_reg(hw, _adcs[i].dbg_ctrl);
	hri_adc_write_CTRLB_reg(hw, _adcs[i].ctrl_b);
	hri_adc_write_INPUTCTRL_reg(hw, _adcs[i].input_ctrl);
	hri_adc_write_WINCTRL_reg(hw, _adcs[i].win_ctrl);
	hri_adc_write_WINLT_reg(hw, _adcs[i].win_lt);
	hri_adc_write_WINUT_reg(hw, _adcs[i].win_ut);
	hri_adc_write_CTRLA_reg(hw, _adcs[i].ctrl_a);

	return ERR_NONE;
}

/**
 * \brief De-initialize ADC
 *
 * \param[in] hw The pointer to hardware instance
 */
static inline void _adc_deinit(void *const hw)
{
	hri_adc_clear_CTRLA_ENABLE_bit(hw);
	hri_adc_set_CTRLA_SWRST_bit(hw);
}

/**
 * \brief Initialize ADC
 */
int32_t _adc_sync_init(struct _adc_sync_device *const device, void *const hw)
{
	ASSERT(device);

	device->hw = hw;

	return _adc_init(hw, _adc_get_regs((uint32_t)hw));
}

/**
 * \brief Initialize ADC
 */
int32_t _adc_async_init(struct _adc_async_device *const device, void *const hw)
{
	int32_t init_status;

	ASSERT(device);

	init_status = _adc_init(hw, _adc_get_regs((uint32_t)hw));
	if (init_status) {
		return init_status;
	}
	device->hw = hw;
	_adc_dev   = device;

	NVIC_DisableIRQ(ADC_IRQn);
	NVIC_ClearPendingIRQ(ADC_IRQn);
	NVIC_EnableIRQ(ADC_IRQn);

	return ERR_NONE;
}

/**
 * \brief Initialize ADC
 */
int32_t _adc_dma_init(struct _adc_dma_device *const device, void *const hw)
{
	ASSERT(device);

	device->hw = hw;

	return _adc_init(hw, _adc_get_regs((uint32_t)hw));
}

/**
 * \brief De-initialize ADC
 */
void _adc_sync_deinit(struct _adc_sync_device *const device)
{
	_adc_deinit(device->hw);
}

/**
 * \brief De-initialize ADC
 */
void _adc_async_deinit(struct _adc_async_device *const device)
{
	NVIC_DisableIRQ(_adc_get_irq_num(device));
	NVIC_ClearPendingIRQ(_adc_get_irq_num(device));

	_adc_deinit(device->hw);
}

/**
 * \brief De-initialize ADC
 */
void _adc_dma_deinit(struct _adc_dma_device *const device)
{
	_adc_deinit(device->hw);
}

/**
 * \brief Enable ADC
 */
void _adc_sync_enable_channel(struct _adc_sync_device *const device, const uint8_t channel)
{
	(void)channel;

	hri_adc_set_CTRLA_ENABLE_bit(device->hw);
}

/**
 * \brief Enable ADC
 */
void _adc_async_enable_channel(struct _adc_async_device *const device, const uint8_t channel)
{
	(void)channel;

	hri_adc_set_CTRLA_ENABLE_bit(device->hw);
}

/**
 * \brief Enable ADC
 */
void _adc_dma_enable_channel(struct _adc_dma_device *const device, const uint8_t channel)
{
	(void)channel;

	hri_adc_set_CTRLA_ENABLE_bit(device->hw);
}

/**
 * \brief Disable ADC
 */
void _adc_sync_disable_channel(struct _adc_sync_device *const device, const uint8_t channel)
{
	(void)channel;

	hri_adc_clear_CTRLA_ENABLE_bit(device->hw);
}

/**
 * \brief Disable ADC
 */
void _adc_async_disable_channel(struct _adc_async_device *const device, const uint8_t channel)
{
	(void)channel;

	hri_adc_clear_CTRLA_ENABLE_bit(device->hw);
}

/**
 * \brief Disable ADC
 */
void _adc_dma_disable_channel(struct _adc_dma_device *const device, const uint8_t channel)
{
	(void)channel;

	hri_adc_clear_CTRLA_ENABLE_bit(device->hw);
}

/**
 * \brief Return address of ADC DMA source
 */
uint32_t _adc_get_source_for_dma(struct _adc_dma_device *const device)
{
	return (uint32_t) & (((Adc *)(device->hw))->RESULT.reg);
}

/**
 * \brief Retrieve ADC conversion data size
 */
uint8_t _adc_sync_get_data_size(const struct _adc_sync_device *const device)
{
	return hri_adc_read_CTRLB_RESSEL_bf(device->hw) == ADC_CTRLB_RESSEL_8BIT_Val ? 1 : 2;
}

/**
 * \brief Retrieve ADC conversion data size
 */
uint8_t _adc_async_get_data_size(const struct _adc_async_device *const device)
{
	return hri_adc_read_CTRLB_RESSEL_bf(device->hw) == ADC_CTRLB_RESSEL_8BIT_Val ? 1 : 2;
}

/**
 * \brief Retrieve ADC conversion data size
 */
uint8_t _adc_dma_get_data_size(const struct _adc_dma_device *const device)
{
	return hri_adc_read_CTRLB_RESSEL_bf(device->hw) == ADC_CTRLB_RESSEL_8BIT_Val ? 1 : 2;
}

/**
 * \brief Check if conversion is done
 */
bool _adc_sync_is_channel_conversion_done(const struct _adc_sync_device *const device, const uint8_t channel)
{
	(void)channel;

	return hri_adc_get_interrupt_RESRDY_bit(device->hw);
}

/**
 * \brief Check if conversion is done
 */
bool _adc_async_is_channel_conversion_done(const struct _adc_async_device *const device, const uint8_t channel)
{
	(void)channel;

	return hri_adc_get_interrupt_RESRDY_bit(device->hw);
}

/**
 * \brief Check if conversion is done
 */
bool _adc_dma_is_conversion_done(const struct _adc_dma_device *const device)
{
	return hri_adc_get_interrupt_RESRDY_bit(device->hw);
}

/**
 * \brief Make conversion
 */
void _adc_sync_convert(struct _adc_sync_device *const device)
{
	hri_adc_set_SWTRIG_START_bit(device->hw);
}

/**
 * \brief Make conversion
 */
void _adc_async_convert(struct _adc_async_device *const device)
{
	hri_adc_set_SWTRIG_START_bit(device->hw);
}

/**
 * \brief Make conversion
 */
void _adc_dma_convert(struct _adc_dma_device *const device)
{
	hri_adc_set_SWTRIG_START_bit(device->hw);
}

/**
 * \brief Retrieve the conversion result
 */
uint16_t _adc_sync_read_channel_data(const struct _adc_sync_device *const device, const uint8_t channel)
{
	(void)channel;

	return hri_adc_read_RESULT_reg(device->hw);
}

/**
 * \brief Retrieve the conversion result
 */
uint16_t _adc_async_read_channel_data(const struct _adc_async_device *const device, const uint8_t channel)
{
	(void)channel;

	return hri_adc_read_RESULT_reg(device->hw);
}

/**
 * \brief Set reference source
 */
void _adc_sync_set_reference_source(struct _adc_sync_device *const device, const adc_reference_t reference)
{
	hri_adc_write_REFCTRL_REFSEL_bf(device->hw, reference);
}

/**
 * \brief Set reference source
 */
void _adc_async_set_reference_source(struct _adc_async_device *const device, const adc_reference_t reference)
{
	hri_adc_write_REFCTRL_REFSEL_bf(device->hw, reference);
}

/**
 * \brief Set reference source
 */
void _adc_dma_set_reference_source(struct _adc_dma_device *const device, const adc_reference_t reference)
{
	hri_adc_write_REFCTRL_REFSEL_bf(device->hw, reference);
}

/**
 * \brief Set resolution
 */
void _adc_sync_set_resolution(struct _adc_sync_device *const device, const adc_resolution_t resolution)
{
	_adc_set_resolution(device->hw, resolution);
}

/**
 * \brief Set resolution
 */
void _adc_async_set_resolution(struct _adc_async_device *const device, const adc_resolution_t resolution)
{
	_adc_set_resolution(device->hw, resolution);
}

/**
 * \brief Set resolution
 */
void _adc_dma_set_resolution(struct _adc_dma_device *const device, const adc_resolution_t resolution)
{
	hri_adc_write_CTRLB_RESSEL_bf(device->hw, resolution);
}

/**
 * \brief Set channels input sources
 */
void _adc_sync_set_inputs(struct _adc_sync_device *const device, const adc_pos_input_t pos_input,
                          const adc_neg_input_t neg_input, const uint8_t channel)
{
	(void)channel;

	hri_adc_write_INPUTCTRL_MUXPOS_bf(device->hw, pos_input);
	hri_adc_write_INPUTCTRL_MUXNEG_bf(device->hw, neg_input);
}

/**
 * \brief Set channels input sources
 */
void _adc_async_set_inputs(struct _adc_async_device *const device, const adc_pos_input_t pos_input,
                           const adc_neg_input_t neg_input, const uint8_t channel)
{
	(void)channel;

	hri_adc_write_INPUTCTRL_MUXPOS_bf(device->hw, pos_input);
	hri_adc_write_INPUTCTRL_MUXNEG_bf(device->hw, neg_input);
}

/**
 * \brief Set channels input source
 */
void _adc_dma_set_inputs(struct _adc_dma_device *const device, const adc_pos_input_t pos_input,
                         const adc_neg_input_t neg_input, const uint8_t channel)
{
	(void)channel;

	hri_adc_write_INPUTCTRL_MUXPOS_bf(device->hw, pos_input);
	hri_adc_write_INPUTCTRL_MUXNEG_bf(device->hw, neg_input);
}

/**
 * \brief Set thresholds
 */
void _adc_sync_set_thresholds(struct _adc_sync_device *const device, const adc_threshold_t low_threshold,
                              const adc_threshold_t up_threshold)
{
	hri_adc_write_WINLT_reg(device->hw, low_threshold);
	hri_adc_write_WINUT_reg(device->hw, up_threshold);
}

/**
 * \brief Set threshold
 */
void _adc_async_set_thresholds(struct _adc_async_device *const device, const adc_threshold_t low_threshold,
                               const adc_threshold_t up_threshold)
{
	hri_adc_write_WINLT_reg(device->hw, low_threshold);
	hri_adc_write_WINUT_reg(device->hw, up_threshold);
}

/**
 * \brief Set thresholds
 */
void _adc_dma_set_thresholds(struct _adc_dma_device *const device, const adc_threshold_t low_threshold,
                             const adc_threshold_t up_threshold)
{
	hri_adc_write_WINLT_reg(device->hw, low_threshold);
	hri_adc_write_WINUT_reg(device->hw, up_threshold);
}

/**
 * \brief Set gain
 */
void _adc_sync_set_channel_gain(struct _adc_sync_device *const device, const uint8_t channel, const adc_gain_t gain)
{
	(void)channel;

	hri_adc_write_INPUTCTRL_GAIN_bf(device->hw, gain);
}

/**
 * \brief Set gain
 */
void _adc_async_set_channel_gain(struct _adc_async_device *const device, const uint8_t channel, const adc_gain_t gain)
{
	(void)channel;

	hri_adc_write_INPUTCTRL_GAIN_bf(device->hw, gain);
}

/**
 * \brief Set gain
 */
void _adc_dma_set_channel_gain(struct _adc_dma_device *const device, const uint8_t channel, const adc_gain_t gain)
{
	(void)channel;

	hri_adc_write_INPUTCTRL_GAIN_bf(device->hw, gain);
}

/**
 * \brief Set conversion mode
 */
void _adc_sync_set_conversion_mode(struct _adc_sync_device *const device, const enum adc_conversion_mode mode)
{
	_adc_set_conversion_mode(device->hw, mode);
}

/**
 * \brief Set conversion mode
 */
void _adc_async_set_conversion_mode(struct _adc_async_device *const device, const enum adc_conversion_mode mode)
{
	_adc_set_conversion_mode(device->hw, mode);
}

/**
 * \brief Set conversion mode
 */
void _adc_dma_set_conversion_mode(struct _adc_dma_device *const device, const enum adc_conversion_mode mode)
{
	_adc_set_conversion_mode(device->hw, mode);
}

/**
 * \brief Set differential mode
 */
void _adc_sync_set_channel_differential_mode(struct _adc_sync_device *const device, const uint8_t channel,
                                             const enum adc_differential_mode mode)
{
	_adc_set_channel_differential_mode(device->hw, channel, mode);
}

/**
 * \brief Set differential mode
 */
void _adc_async_set_channel_differential_mode(struct _adc_async_device *const device, const uint8_t channel,
                                              const enum adc_differential_mode mode)
{
	_adc_set_channel_differential_mode(device->hw, channel, mode);
}

/**
 * \brief Set differential mode
 */
void _adc_dma_set_channel_differential_mode(struct _adc_dma_device *const device, const uint8_t channel,
                                            const enum adc_differential_mode mode)
{
	(void)channel;

	_adc_set_channel_differential_mode(device->hw, channel, mode);
}

/**
 * \brief Set window mode
 */
void _adc_sync_set_window_mode(struct _adc_sync_device *const device, const adc_window_mode_t mode)
{
	hri_adc_write_WINCTRL_WINMODE_bf(device->hw, mode);
}

/**
 * \brief Set window mode
 */
void _adc_async_set_window_mode(struct _adc_async_device *const device, const adc_window_mode_t mode)
{
	hri_adc_write_WINCTRL_WINMODE_bf(device->hw, mode);
}

/**
 * \brief Set window mode
 */
void _adc_dma_set_window_mode(struct _adc_dma_device *const device, const adc_window_mode_t mode)
{
	hri_adc_write_WINCTRL_WINMODE_bf(device->hw, mode);
}

/**
 * \brief Retrieve threshold state
 */
void _adc_sync_get_threshold_state(const struct _adc_sync_device *const device, adc_threshold_status_t *const state)
{
	*state = hri_adc_get_interrupt_WINMON_bit(device->hw);
}

/**
 * \brief Retrieve threshold state
 */
void _adc_async_get_threshold_state(const struct _adc_async_device *const device, adc_threshold_status_t *const state)
{
	*state = hri_adc_get_interrupt_WINMON_bit(device->hw);
}

/**
 * \brief Retrieve threshold state
 */
void _adc_dma_get_threshold_state(const struct _adc_dma_device *const device, adc_threshold_status_t *const state)
{
	*state = hri_adc_get_interrupt_WINMON_bit(device->hw);
}

/**
 * \brief Enable/disable ADC interrupt
 *
 * param[in] device The pointer to ADC device instance
 * param[in] type The type of interrupt to disable/enable if applicable
 * param[in] state Enable or disable
 */
void _adc_async_set_irq_state(struct _adc_async_device *const device, const uint8_t channel,
                              const enum _adc_async_callback_type type, const bool state)
{
	(void)channel;

	void *const hw = device->hw;

	if (ADC_ASYNC_DEVICE_MONITOR_CB == type) {
		hri_adc_write_INTEN_WINMON_bit(hw, state);
	} else if (ADC_ASYNC_DEVICE_ERROR_CB == type) {
		hri_adc_write_INTEN_OVERRUN_bit(hw, state);
	} else if (ADC_ASYNC_DEVICE_CONVERT_CB == type) {
		hri_adc_write_INTEN_RESRDY_bit(hw, state);
	}
}

/**
 * \brief SetADC resolution
 *
 * \param[in] hw The pointer to hardware instance
 * \param[in] resolution The resolution to set
 */
static void _adc_set_resolution(void *const hw, const adc_resolution_t resolution)
{
	bool enabled = hri_adc_get_CTRLA_ENABLE_bit(hw);

	hri_adc_clear_CTRLA_ENABLE_bit(hw);
	hri_adc_write_CTRLB_RESSEL_bf(hw, resolution);

	if (enabled) {
		hri_adc_set_CTRLA_ENABLE_bit(hw);
	}
}

/**
 * \brief SetADC resolution
 *
 * \param[in] hw The pointer to hardware instance
 * \param[in] mode The mode to set
 */
static void _adc_set_conversion_mode(void *const hw, const enum adc_conversion_mode mode)
{
	bool enabled = hri_adc_get_CTRLA_ENABLE_bit(hw);

	hri_adc_clear_CTRLA_ENABLE_bit(hw);
	hri_adc_clear_CTRLB_FREERUN_bit(hw);
	if (ADC_CONVERSION_MODE_FREERUN == mode) {
		hri_adc_set_CTRLB_FREERUN_bit(hw);
	}

	if (enabled) {
		hri_adc_set_CTRLA_ENABLE_bit(hw);
	}
}

/**
 * \brief SetADC resolution
 *
 * \param[in] hw The pointer to hardware instance
 * \param[in] mode The mode to set
 */
static void _adc_set_channel_differential_mode(void *const hw, const uint8_t channel,
                                               const enum adc_differential_mode mode)
{
	(void)channel;
	bool enabled = hri_adc_get_CTRLA_ENABLE_bit(hw);

	hri_adc_clear_CTRLA_ENABLE_bit(hw);
	hri_adc_clear_CTRLB_DIFFMODE_bit(hw);
	if (ADC_DIFFERENTIAL_MODE_DIFFERENTIAL == mode) {
		hri_adc_set_CTRLB_DIFFMODE_bit(hw);
	}

	if (enabled) {
		hri_adc_set_CTRLA_ENABLE_bit(hw);
	}
}
