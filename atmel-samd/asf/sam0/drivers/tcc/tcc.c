/**
 * \file
 *
 * \brief SAM TCC - Timer Counter for Control Applications Driver
 *
 * Copyright (C) 2013-2016 Atmel Corporation. All rights reserved.
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

#include "tcc.h"

#if TCC_ASYNC == true
#  include "tcc_callback.h"
#  include <system_interrupt.h>

/** \internal
 * Converts a given TCC index to its interrupt vector index.
 */
#  define _TCC_INTERRUPT_VECT_NUM(n, unused) \
		SYSTEM_INTERRUPT_MODULE_TCC##n,
#endif

#define _SIZE_MAX(size) ((size==32u) ? 0xFFFFFFFF : ( \
		(1u << size) - 1))

#define _SIZE_MAX_WITH_DITHER 0x03FFFFFF

/* Extension support mapping bits */
#define _TCC_DITHERING_B 16u
#define _TCC_PG_B         8u
#define _TCC_SWAP_B       4u
#define _TCC_DTI_B        2u
#define _TCC_OTMX_B       1u

#if !defined(__DOXYGEN__)

#  define _TCC_GCLK_ID(n,unused)           TPASTE3(TCC,n,_GCLK_ID),
#  if (SAML21) || (SAML22) || (SAMC20) || (SAMC21) || (SAMR30)
#    define _TCC_APBCMASK(n,unused)        TPASTE2(MCLK_APBCMASK_TCC,n),
#  else
#    define _TCC_APBCMASK(n,unused)        TPASTE2(PM_APBCMASK_TCC,n),
#  endif

#  define _TCC_SIZE(n,unused)              TPASTE3(TCC,n,_SIZE),
#  define _TCC_MAX(n,unused)               _SIZE_MAX(TPASTE3(TCC,n,_SIZE)),
#  define _TCC_EXT(n,unused)               TPASTE3(TCC,n,_EXT),
#  define _TCC_CC_NUM(n,unused)            min(TPASTE3(TCC,n,_CC_NUM),TCC_NUM_CHANNELS),
#  define _TCC_OW_NUM(n,unused)            min(TPASTE3(TCC,n,_OW_NUM),TCC_NUM_WAVE_OUTPUTS),

#  define TCC_GCLK_IDS      { MREPEAT(TCC_INST_NUM, _TCC_GCLK_ID, 0) }
#  define TCC_APBCMASKS     { MREPEAT(TCC_INST_NUM, _TCC_APBCMASK, 0) }

#  define TCC_SIZES         { MREPEAT(TCC_INST_NUM, _TCC_SIZE, 0) }
#  define TCC_MAXS          { MREPEAT(TCC_INST_NUM, _TCC_MAX, 0) }
#  define TCC_EXTS          { MREPEAT(TCC_INST_NUM, _TCC_EXT, 0) }
#  define TCC_CC_NUMS       { MREPEAT(TCC_INST_NUM, _TCC_CC_NUM, 0) }
#  define TCC_OW_NUMS       { MREPEAT(TCC_INST_NUM, _TCC_OW_NUM, 0) }

#endif

/* List of available TCC modules. */
const Tcc *const tcc_modules[TCC_INST_NUM] = TCC_INSTS;

/* List of TCC GCLK IDs */
const uint8_t _tcc_gclk_ids[TCC_INST_NUM] = TCC_GCLK_IDS;

/* List of TCC APBC Masks */
const uint32_t _tcc_apbcmasks[TCC_INST_NUM] = TCC_APBCMASKS;

/* List of extension support of TCC modules. */
const uint8_t _tcc_exts[TCC_INST_NUM] = TCC_EXTS;

/* List of sizes support of TCC modules. */
const uint8_t _tcc_sizes[TCC_INST_NUM] = TCC_SIZES;

/* List of maximumvalues supported of TCC modules. */
const uint32_t _tcc_maxs[TCC_INST_NUM] = TCC_MAXS;

/* List of available channel number of TCC modules. */
const uint8_t _tcc_cc_nums[TCC_INST_NUM] = TCC_CC_NUMS;

/* List of available output number of TCC modules. */
const uint8_t _tcc_ow_nums[TCC_INST_NUM] = TCC_OW_NUMS;

/**
 * \internal Find the index of the given TCC module instance.
 *
 * \param[in] The TCC module instance pointer
 *
 * \return Index of the given TCC module instance.
 */
uint8_t _tcc_get_inst_index(
		Tcc *const hw)
{
	/* Find index for TCC instance. */
	for (uint32_t i = 0; i < TCC_INST_NUM; i++) {
		if (hw == tcc_modules[i]) {
			return i;
		}
	}

	/* Invalid data given. */
	Assert(false);
	return 0;
}

/**
 * \brief Initializes config with predefined default values.
 *
 * This function will initialize a given TCC configuration structure to
 * a set of known default values. This function should be called on
 * any new instance of the configuration structures before being
 * modified by the user application.
 *
 * The default configuration is as follows:
 *  \li Don't run in standby
 *  \li When setting top, compare, or pattern by API, do double buffering write
 *  \li The base timer/counter configurations:
 *     - GCLK generator 0 clock source
 *     - No prescaler
 *     - GCLK reload action
 *     - Count upward
 *     - Don't perform one-shot operations
 *     - Counter starts on 0
 *     - Period/top value set to maximum
 *  \li The match/capture configurations:
 *     - All Capture compare channel value set to 0
 *     - No capture enabled (all channels use compare function)
 *     - Normal frequency wave generation
 *     - Waveform generation polarity set to 0
 *     - Don't perform ramp on waveform
 *  \li The waveform extension configurations:
 *     - No recoverable fault is enabled, fault actions are disabled, filter
 *          is set to 0
 *     - No non-recoverable fault state output is enabled and filter is 0
 *     - No inversion of waveform output
 *  \li No channel output enabled
 *  \li No PWM pin output enabled
 *  \li Pin and MUX configuration not set
 *
 * \param[out]  config  Pointer to a TCC module configuration structure to set
 * \param[in]   hw      Pointer to the TCC hardware module
 *
 */
void tcc_get_config_defaults(
		struct tcc_config *const config,
		Tcc *const hw)
{
	/* TCC instance index */
	uint8_t module_index = _tcc_get_inst_index(hw);

	/* Base counter defaults */
	config->counter.count                  = 0;

	config->counter.period                 = _tcc_maxs[module_index];

	config->counter.clock_source           = GCLK_GENERATOR_0;
	config->counter.clock_prescaler        = TCC_CLOCK_PRESCALER_DIV1;
	config->counter.reload_action          = TCC_RELOAD_ACTION_GCLK;

	config->counter.direction              = TCC_COUNT_DIRECTION_UP;
	config->counter.oneshot                = false;

#ifdef FEATURE_TCC_GENERATE_DMA_TRIGGER
	config->counter.dma_trigger_mode       = TCC_COUNT_OVERFLOW_DMA_TRIGGER_MODE_CONTINUE;
#endif

	/* Match/Capture defaults */
#  define _TCC_CHANNEL_MATCH_VALUE_INIT(n, value) \
		config->compare.match[n] = value;
	MREPEAT(TCC_NUM_CHANNELS,
		_TCC_CHANNEL_MATCH_VALUE_INIT, 0)
#  undef _TCC_CHANNEL_MATCH_VALUE_INIT

	/* Wave polarity defaults */
#  define _TCC_CHANNEL_WAVE_POLARITY_INIT(n, value) \
		config->compare.wave_polarity[n] = value;
	MREPEAT(TCC_NUM_CHANNELS,
		_TCC_CHANNEL_WAVE_POLARITY_INIT, TCC_WAVE_POLARITY_0)
#  undef _TCC_CHANNEL_WAVE_POLARITY_INIT

	config->compare.wave_generation = TCC_WAVE_GENERATION_NORMAL_FREQ;
	config->compare.wave_ramp       = TCC_RAMP_RAMP1;

#  define _TCC_CHANNEL_FUNCTION_INIT(n, value) \
		config->compare.channel_function[n] = value;
	MREPEAT(TCC_NUM_CHANNELS,
			_TCC_CHANNEL_FUNCTION_INIT, TCC_CHANNEL_FUNCTION_COMPARE)
#  undef _TCC_CHANNEL_FUNCTION_INIT

	/* Recoverable fault defaults */
#  define _TCC_FAULT_FUNCTION_INIT(n, dummy) \
		config->wave_ext.recoverable_fault[n].filter_value = 0;      \
		config->wave_ext.recoverable_fault[n].blanking_cycles = 0;   \
		config->wave_ext.recoverable_fault[n].restart = false;       \
		config->wave_ext.recoverable_fault[n].keep = false;          \
		config->wave_ext.recoverable_fault[n].qualification = false; \
		config->wave_ext.recoverable_fault[n].source = TCC_FAULT_SOURCE_DISABLE;           \
		config->wave_ext.recoverable_fault[n].blanking = TCC_FAULT_BLANKING_DISABLE;       \
		config->wave_ext.recoverable_fault[n].halt_action = TCC_FAULT_HALT_ACTION_DISABLE; \
		config->wave_ext.recoverable_fault[n].capture_action = TCC_FAULT_CAPTURE_DISABLE;  \
		config->wave_ext.recoverable_fault[n].capture_channel = TCC_FAULT_CAPTURE_CHANNEL_0;
	MREPEAT(TCC_NUM_FAULTS, _TCC_FAULT_FUNCTION_INIT, 0)
#  undef _TCC_FAULT_FUNCTION_INIT

	/* Non-recoverable fault defaults */
#  define _TCC_NRF_FUNCTION_INIT(n, dummy) \
		config->wave_ext.non_recoverable_fault[n].filter_value = 0; \
		config->wave_ext.non_recoverable_fault[n].output = TCC_FAULT_STATE_OUTPUT_OFF;
	MREPEAT(TCC_NUM_WAVE_OUTPUTS, _TCC_NRF_FUNCTION_INIT, 0)
#  undef _TCC_NRF_FUNCTION_INIT

	/* Output inversion defaults */
#  define _TCC_OUT_INVERT_INIT(n, value) \
		config->wave_ext.invert[n] = value;
	MREPEAT(TCC_NUM_WAVE_OUTPUTS, _TCC_OUT_INVERT_INIT, false)
#  undef _TCC_OUT_INVERT_INIT

#  define _TCC_CHANNEL_OUT_PIN_INIT(n, dummy) \
		config->pins.enable_wave_out_pin[n]                = false;\
		config->pins.wave_out_pin[TCC_WAVE_OUTPUT_##n]     = 0;    \
		config->pins.wave_out_pin_mux[TCC_WAVE_OUTPUT_##n] = 0;
	MREPEAT(TCC_NUM_WAVE_OUTPUTS, _TCC_CHANNEL_OUT_PIN_INIT, 0)
#  undef _TCC_CHANNEL_OUT_PIN_INIT

	config->double_buffering_enabled  = true;
	config->run_in_standby            = false;
}


/**
 * \brief Build CTRLA register value from configuration.
 *
 * \param[in]  module_index The software module instance index
 * \param[in]  config       Pointer to the TCC configuration options struct
 * \param[out] value_buffer Pointer to the buffer to fill with built value
 *
 * \return Configuration validation status.
 *
 * \retval STATUS_OK              Configuration values are good and register
 *                                value built and save to buffer
 * \retval STATUS_ERR_INVALID_ARG Invalid parameter found:
 *                                assigned dither mode is invalid for module;
 *                                used capture channel is invalid for module
 */
static inline enum status_code _tcc_build_ctrla(
		const uint8_t module_index,
		const struct tcc_config *const config,
		uint32_t *value_buffer)
{
	uint32_t ctrla = 0;

	int i;
	for (i = 0; i < TCC_NUM_CHANNELS; i ++) {
		if (config->capture.channel_function[i] ==
			TCC_CHANNEL_FUNCTION_CAPTURE) {

			if (i > _tcc_cc_nums[module_index]) {
				/* Channel not supported */
				return STATUS_ERR_INVALID_ARG;
			}
			ctrla |= (TCC_CTRLA_CPTEN0 << i);
		}
	}

	if (config->run_in_standby) {
		ctrla |= TCC_CTRLA_RUNSTDBY;
	}
	ctrla |= config->counter.reload_action << TCC_CTRLA_PRESCSYNC_Pos;
	ctrla |= config->counter.clock_prescaler << TCC_CTRLA_PRESCALER_Pos;

	*value_buffer = ctrla;
	return STATUS_OK;
}

/**
 * \brief Build CTRLB register value from configuration.
 *
 * \param[in]  module_index The software module instance index
 * \param[in]  config       Pointer to the TCC configuration options struct
 * \param[out] value_buffer Pointer to the buffer to fill with built value
 */
static inline void _tcc_build_ctrlb(
		const uint8_t module_index,
		const struct tcc_config *const config,
		uint8_t *value_buffer)
{
	uint8_t ctrlb = 0;

	if (config->counter.oneshot) {
		ctrlb |= TCC_CTRLBSET_ONESHOT;
	}
	if (config->counter.direction == TCC_COUNT_DIRECTION_DOWN) {
		ctrlb |= TCC_CTRLBSET_DIR;
	}

	*value_buffer = ctrlb;
}

/**
 * \brief Build FAULTs register values from configuration.
 *
 * \param[in]  module_index The software module instance index
 * \param[in]  config       Pointer to the TCC configuration options struct
 * \param[out] value_buffer Pointer to the buffer to fill with built values
 *
 * \retval STATUS_OK              Configuration values are good and register
 *                                value built and save to buffer
 * \retval STATUS_ERR_INVALID_ARG Invalid parameter found: assigned fault
 *                                capture channel is invalid; assigned filter
 *                                value is invalid
 */
static inline enum status_code _tcc_build_faults(
		const uint8_t module_index,
		const struct tcc_config *const config,
		uint32_t *value_buffer)
{
	struct tcc_recoverable_fault_config *cfg;
	uint8_t cc_num = _tcc_cc_nums[module_index];
	uint32_t fault;
	int i;
	for (i = 0; i < TCC_NUM_FAULTS; i ++) {
		cfg = (struct tcc_recoverable_fault_config *)
				&config->wave_ext.recoverable_fault[i];
		if (cfg->capture_channel >= cc_num) {
			return STATUS_ERR_INVALID_ARG;
		}
		if (cfg->filter_value > 0xF) {
			return STATUS_ERR_INVALID_ARG;
		}
		fault = TCC_FCTRLA_FILTERVAL(cfg->filter_value)
				| TCC_FCTRLA_BLANKVAL(cfg->blanking_cycles)
				| (cfg->restart ? TCC_FCTRLA_RESTART : 0)
				| (cfg->keep ? TCC_FCTRLA_KEEP : 0)
				| (cfg->qualification ? TCC_FCTRLA_QUAL : 0)
				| TCC_FCTRLA_SRC(cfg->source)
				| TCC_FCTRLA_BLANK(cfg->blanking)
				| TCC_FCTRLA_HALT(cfg->halt_action)
				| TCC_FCTRLA_CAPTURE(cfg->capture_action)
				| TCC_FCTRLA_CHSEL(cfg->capture_channel);
		value_buffer[i] = fault;
	}
	return STATUS_OK;
}

/**
 * \brief Build DRVCTRL register values from configuration.
 *
 * \param[in]  module_index The software module instance index
 * \param[in]  config       Pointer to the TCC configuration options struct
 * \param[out] value_buffer Pointer to the buffer to fill with built value
 *
 * \retval STATUS_OK              Configuration values are good and register
 *                                value built and save to buffer
 * \retval STATUS_ERR_INVALID_ARG Invalid parameter found: assigned output line
 *                                is invalid; filter value is invalid
 */
static inline enum status_code _tcc_build_drvctrl(
		const uint8_t module_index,
		const struct tcc_config *const config,
		uint32_t *value_buffer)
{
	uint32_t i;
	uint8_t ow_num = _tcc_ow_nums[module_index];
	uint32_t drvctrl;

	drvctrl = 0;

	for (i = 0; i < TCC_NUM_WAVE_OUTPUTS; i ++) {
		if (config->wave_ext.invert[i]) {
			if (i >= ow_num) {
				return STATUS_ERR_INVALID_ARG;
			}
			drvctrl |= (TCC_DRVCTRL_INVEN0 << i);
		}
		if (config->wave_ext.non_recoverable_fault[i].output !=
			TCC_FAULT_STATE_OUTPUT_OFF) {
			if (i >= ow_num) {
				return STATUS_ERR_INVALID_ARG;
			}
			if (config->wave_ext.non_recoverable_fault[i].output ==
				TCC_FAULT_STATE_OUTPUT_1) {
				drvctrl |= (TCC_DRVCTRL_NRE0 | TCC_DRVCTRL_NRV0) << i;
			} else {
				drvctrl |= (TCC_DRVCTRL_NRE0) << i;
			}
		}
	}
	*value_buffer = drvctrl;
	return STATUS_OK;
}

/**
 * \brief Build WAVE & WAVEB register values from configuration.
 *
 * \param[in]  module_index The software module instance index
 * \param[in]  config       Pointer to the TCC configuration options struct
 * \param[out] value_buffer Pointer to the buffer to fill with built value
 *
 * \retval STATUS_OK              Configuration values are good and register
 *                                value built and save to buffer
 * \retval STATUS_ERR_INVALID_ARG Invalid parameter found: assigned output line
 *                                is invalid; circular and double buffering
 *                                conflict; assigned function not supported by
 *                                module
 */
static inline enum status_code _tcc_build_waves(
		const uint8_t module_index,
		const struct tcc_config *const config,
		uint32_t *value_buffer)
{
	int n;

	uint8_t cc_num = _tcc_cc_nums[module_index];
	struct tcc_match_wave_config const *wav_cfg = &config->compare;

	uint32_t wave;

	wave = TCC_WAVE_RAMP(wav_cfg->wave_ramp) |
			TCC_WAVE_WAVEGEN(wav_cfg->wave_generation);

	for (n = 0; n < TCC_NUM_CHANNELS; n++) {
		if (wav_cfg->wave_polarity[n]) {
			if (n >= cc_num) {
				return STATUS_ERR_INVALID_ARG;
			}
			wave |= (TCC_WAVE_POL0 << n);
		}
	}

	value_buffer[0] = wave;

	return STATUS_OK;
}

/**
 * \brief Initializes a hardware TCC module instance.
 *
 * Enables the clock and initializes the given TCC module, based on the given
 * configuration values.
 *
 * \param[in,out] module_inst  Pointer to the software module instance struct
 * \param[in]     hw           Pointer to the TCC hardware module
 * \param[in]     config       Pointer to the TCC configuration options struct
 *
 * \return Status of the initialization procedure.
 *
 * \retval STATUS_OK           The module was initialized successfully
 * \retval STATUS_BUSY         The hardware module was busy when the
 *                             initialization procedure was attempted
 * \retval STATUS_INVALID_ARG  An invalid configuration option or argument
 *                             was supplied
 * \retval STATUS_ERR_DENIED   The hardware module was already enabled
 */
enum status_code tcc_init(
		struct tcc_module *const module_inst,
		Tcc *const hw,
		const struct tcc_config *const config)
{
	int i;

	/* Sanity check arguments */
	Assert(hw);
	Assert(module_inst);
	Assert(config);

	/* TCC instance index */
	uint8_t module_index = _tcc_get_inst_index(hw);

	/* Enable the user interface clock for TCC */
	system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBC,
			_tcc_apbcmasks[module_index]);

	/* Check if it's enabled. */
	if (hw->CTRLA.reg & TCC_CTRLA_ENABLE) {
		return STATUS_ERR_DENIED;
	}
	/* Check if it's resetting */
	if (hw->CTRLA.reg & TCC_CTRLA_SWRST) {
		return STATUS_ERR_DENIED;
	}

	enum status_code status;

	/* Check COUNT, PER, CCx */
	uint32_t count_max  = _tcc_maxs[module_index];

	/* Check all counter values */
	if ((config->counter.count > count_max)
		|| (config->counter.period > count_max)
		) {
		return STATUS_ERR_INVALID_ARG;
	}

	/* Check all channel values */
	for (i = 0; i < TCC_NUM_CHANNELS; i ++) {
		if ((config->compare.match[i] > count_max)
			) {
			return STATUS_ERR_INVALID_ARG;
		}
	}

	/* Check all outputs */
	for (i = 0; i < TCC_NUM_WAVE_OUTPUTS; i ++) {
		if (!config->pins.enable_wave_out_pin[i]) {
			continue;
		}
		/* Output line is not supported */
		if (i >= _tcc_ow_nums[module_index]) {
			return STATUS_ERR_INVALID_ARG;
		}
	}

	/* CTRLA settings */
	uint32_t ctrla = 0;
	status = _tcc_build_ctrla(module_index, config, &ctrla);
	if (STATUS_OK != status) {
		return status;
	}

	/* CTRLB settings */
	uint8_t ctrlb;
	_tcc_build_ctrlb(module_index, config, &ctrlb);

	/* FAULTs settings */
	uint32_t faults[TCC_NUM_FAULTS];

	status = _tcc_build_faults(module_index, config, faults);
	if (STATUS_OK != status) {
		return status;
	}

	/* DRVCTRL */
	uint32_t drvctrl = 0;

	status = _tcc_build_drvctrl(module_index, config, &drvctrl);
	if (STATUS_OK != status) {
		return status;
	}

	/* WAVE */
	uint32_t waves[1];

	status = _tcc_build_waves(module_index, config, waves);
	if (STATUS_OK != status) {
		return status;
	}

	/* Initialize module */
#if TCC_ASYNC
	/* Initialize parameters */
	for (i = 0; i < TCC_CALLBACK_N; i ++) {
		module_inst->callback[i] = NULL;
	}
	module_inst->register_callback_mask = 0;
	module_inst->enable_callback_mask = 0;
	_tcc_instances[module_index] = module_inst;
#endif

	module_inst->hw = hw;

	module_inst->double_buffering_enabled = config->double_buffering_enabled;

	/* Setup clock for module */
	struct system_gclk_chan_config gclk_chan_config;
	system_gclk_chan_get_config_defaults(&gclk_chan_config);
	gclk_chan_config.source_generator = config->counter.clock_source;
	system_gclk_chan_set_config(_tcc_gclk_ids[module_index], &gclk_chan_config);
	system_gclk_chan_enable(_tcc_gclk_ids[module_index]);

	/* Initialize pins */
	struct system_pinmux_config pin_config;
	for (i = 0; i <  _tcc_ow_nums[module_index]; i ++) {
		if (!config->pins.enable_wave_out_pin[i]) {
			continue;
		}

		system_pinmux_get_config_defaults(&pin_config);
		pin_config.mux_position = config->pins.wave_out_pin_mux[i];
		pin_config.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
		system_pinmux_pin_set_config(
				config->pins.wave_out_pin[i], &pin_config);
	}

	/* Write to registers */

	hw->CTRLA.reg = ctrla;
	while (hw->SYNCBUSY.reg & TCC_SYNCBUSY_CTRLB) {
		/* Wait for sync */
	}

	hw->CTRLBCLR.reg = 0xFF;
	while (hw->SYNCBUSY.reg & TCC_SYNCBUSY_CTRLB) {
		/* Wait for sync */
	}
	hw->CTRLBSET.reg = ctrlb;

	hw->FCTRLA.reg = faults[0];
	hw->FCTRLB.reg = faults[1];

	hw->DRVCTRL.reg = drvctrl;

#if (!SAML21) && (!SAMC20) && (!SAMC21) && (!SAML22) && (!SAMR30)
	while (hw->SYNCBUSY.reg & (TCC_SYNCBUSY_WAVE | TCC_SYNCBUSY_WAVEB)) {
		/* Wait for sync */
	}
#endif
	hw->WAVE.reg = waves[0];

	while (hw->SYNCBUSY.reg & TCC_SYNCBUSY_COUNT) {
		/* Wait for sync */
	}
	hw->COUNT.reg = config->counter.count;

#if (!SAML21) && (!SAMC20) && (!SAMC21) && (!SAML22) && (!SAMR30)
	while (hw->SYNCBUSY.reg & (TCC_SYNCBUSY_PER | TCC_SYNCBUSY_PERB)) {
		/* Wait for sync */
	}
#endif
	hw->PER.reg = (config->counter.period);

	for (i = 0; i <  _tcc_cc_nums[module_index]; i ++) {
#if (!SAML21) && (!SAMC20) && (!SAMC21) && (!SAML22) && (!SAMR30)
		while (hw->SYNCBUSY.reg & (
			(TCC_SYNCBUSY_CC0 | TCC_SYNCBUSY_CCB0) << i)) {
			/* Wait for sync */
		}
#endif
		hw->CC[i].reg = (config->compare.match[i]);
	}

	return STATUS_OK;
}


/**
 * \brief Enables the TCC module event input or output.
 *
 * Enables one or more input or output events to or from the TCC module.
 * See \ref tcc_events for a list of events this module supports.
 *
 * \note Events cannot be altered while the module is enabled.
 *
 * \param[in]  module_inst  Pointer to the software module instance struct
 * \param[in]  events       Struct containing flags of events to enable or
 *                          configure
 *
 * \return Status of the events setup procedure.
 *
 * \retval STATUS_OK           The module was initialized successfully
 * \retval STATUS_INVALID_ARG  An invalid configuration option or argument
 *                             was supplied
 */
enum status_code tcc_enable_events(
		struct tcc_module *const module_inst,
		struct tcc_events *const events)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);
	Assert(events);

	Tcc *const tcc_module = module_inst->hw;

	/* Check if it's enabled or resetting. */
	if (tcc_module->CTRLA.reg & (TCC_CTRLA_ENABLE | TCC_CTRLA_SWRST)) {
		return STATUS_ERR_DENIED;
	}

	uint32_t evctrl = tcc_module->EVCTRL.reg;

	/* Setup event output action */
	if (events->output_config.modify_generation_selection) {
		evctrl &= ~ TCC_EVCTRL_CNTSEL_Msk;
		switch(events->output_config.generation_selection) {
		case TCC_EVENT_GENERATION_SELECTION_START:
			evctrl |= TCC_EVCTRL_CNTSEL_START;
			break;
		case TCC_EVENT_GENERATION_SELECTION_END:
			evctrl |= TCC_EVCTRL_CNTSEL_END;
			break;
		case TCC_EVENT_GENERATION_SELECTION_BETWEEN:
			evctrl |= TCC_EVCTRL_CNTSEL_BETWEEN;
			break;
		case TCC_EVENT_GENERATION_SELECTION_BOUNDARY:
			evctrl |= TCC_EVCTRL_CNTSEL_BOUNDARY;
			break;
		default:
			Assert(false);
			/* Wrong configuration */
			return STATUS_ERR_INVALID_ARG;
		}
	}
	/* Setup input event0 */
	if (events->on_input_event_perform_action[0]) {
		evctrl |= TCC_EVCTRL_TCEI0;
	}
	if (events->input_config[0].invert) {
		evctrl |= TCC_EVCTRL_TCINV0;
	}
	if (events->input_config[0].modify_action) {
		evctrl &= ~ TCC_EVCTRL_EVACT0_Msk;
		switch(events->input_config[0].action) {
		case TCC_EVENT0_ACTION_OFF:
			evctrl |= TCC_EVCTRL_EVACT0_OFF;
			break;
		case TCC_EVENT0_ACTION_RETRIGGER:
			evctrl |= TCC_EVCTRL_EVACT0_RETRIGGER;
			break;
		case TCC_EVENT0_ACTION_COUNT_EVENT:
			evctrl |= TCC_EVCTRL_EVACT0_COUNTEV;
			break;
		case TCC_EVENT0_ACTION_START:
			evctrl |= TCC_EVCTRL_EVACT0_START;
			break;
		case TCC_EVENT0_ACTION_INCREMENT:
			evctrl |= TCC_EVCTRL_EVACT0_INC;
			break;
		case TCC_EVENT0_ACTION_COUNT_DURING_ACTIVE:
			evctrl |= TCC_EVCTRL_EVACT0_COUNT;
			break;
		case TCC_EVENT0_ACTION_NON_RECOVERABLE_FAULT:
			evctrl |= TCC_EVCTRL_EVACT0_FAULT;
			break;
		default:
			Assert(false);
			/* Wrong configuration */
			return STATUS_ERR_INVALID_ARG;
		}
	}
	/* Setup input event1 */
	if (events->on_input_event_perform_action[1]) {
		evctrl |= TCC_EVCTRL_TCEI1;
	}
	if (events->input_config[1].invert) {
		evctrl |= TCC_EVCTRL_TCINV1;
	}
	if (events->input_config[1].modify_action) {
		evctrl &= ~ TCC_EVCTRL_EVACT1_Msk;
		switch(events->input_config[1].action) {
		case TCC_EVENT1_ACTION_OFF:
			evctrl |= TCC_EVCTRL_EVACT1_OFF;
			break;
		case TCC_EVENT1_ACTION_RETRIGGER:
			evctrl |= TCC_EVCTRL_EVACT1_RETRIGGER;
			break;
		case TCC_EVENT1_ACTION_DIR_CONTROL:
			evctrl |= TCC_EVCTRL_EVACT1_DIR;
			break;
		case TCC_EVENT1_ACTION_STOP:
			evctrl |= TCC_EVCTRL_EVACT1_STOP;
			break;
		case TCC_EVENT1_ACTION_DECREMENT:
			evctrl |= TCC_EVCTRL_EVACT1_DEC;
			break;
		case TCC_EVENT1_ACTION_PERIOD_PULSE_WIDTH_CAPTURE:
			evctrl |= TCC_EVCTRL_EVACT1_PPW |
					TCC_EVCTRL_MCEI0 | TCC_EVCTRL_MCEI1;
			break;
		case TCC_EVENT1_ACTION_PULSE_WIDTH_PERIOD_CAPTURE:
			evctrl |= TCC_EVCTRL_EVACT1_PWP |
					TCC_EVCTRL_MCEI0 | TCC_EVCTRL_MCEI1;
			break;
		case TCC_EVENT1_ACTION_NON_RECOVERABLE_FAULT:
			evctrl |= TCC_EVCTRL_EVACT1_FAULT;
			break;
		default:
			Assert(false);
			/* Wrong configuration */
			return STATUS_ERR_INVALID_ARG;
		}
	}
	uint32_t ch;
	for(ch = 0; ch < TCC_NUM_CHANNELS; ch ++) {
		if (events->generate_event_on_channel[ch]) {
			evctrl |= (TCC_EVCTRL_MCEO(1) << ch);
		}
		if (events->on_event_perform_channel_action[ch]) {
			evctrl |= (TCC_EVCTRL_MCEI(1) << ch);
		}
	}
	if (events->generate_event_on_counter_overflow) {
		evctrl |= TCC_EVCTRL_OVFEO;
	}
	if (events->generate_event_on_counter_retrigger) {
		evctrl |= TCC_EVCTRL_TRGEO;
	}
	if (events->generate_event_on_counter_event) {
		evctrl |= TCC_EVCTRL_CNTEO;
	}

	tcc_module->EVCTRL.reg = evctrl;

	return STATUS_OK;
}

/**
 * \brief Disables the event input or output of a TCC instance.
 *
 * Disables one or more input or output events for the given TCC module.
 * See \ref tcc_events for a list of events this module supports.
 *
 * \note Events cannot be altered while the module is enabled.
 *
 * \param[in]  module_inst  Pointer to the software module instance struct
 * \param[in]  events       Struct containing flags of events to disable
 */
void tcc_disable_events(
		struct tcc_module *const module_inst,
		struct tcc_events *const events)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);
	Assert(events);

	Tcc *const tcc_module = module_inst->hw;

	/* Check if it's enabled or resetting. */
	if (tcc_module->CTRLA.reg & (TCC_CTRLA_ENABLE | TCC_CTRLA_SWRST)) {
		return;
	}


	uint32_t evctrl = 0;
	uint32_t ch;
	for(ch = 0; ch < TCC_NUM_CHANNELS; ch ++) {
		if (events->generate_event_on_channel[ch]) {
			evctrl |= (TCC_EVCTRL_MCEO(1) << ch);
		}
		if (events->on_event_perform_channel_action[ch]) {
			evctrl |= (TCC_EVCTRL_MCEI(1) << ch);
		}
	}
	if (events->generate_event_on_counter_overflow) {
		evctrl |= TCC_EVCTRL_OVFEO;
	}
	if (events->generate_event_on_counter_retrigger) {
		evctrl |= TCC_EVCTRL_TRGEO;
	}
	if (events->generate_event_on_counter_event) {
		evctrl |= TCC_EVCTRL_CNTEO;
	}
	if (events->on_input_event_perform_action[0]) {
		evctrl |= TCC_EVCTRL_TCEI0;
	}
	if (events->on_input_event_perform_action[1]) {
		evctrl |= TCC_EVCTRL_TCEI1;
	}
	if (events->input_config[0].invert) {
		evctrl |= TCC_EVCTRL_TCINV0;
	}
	if (events->input_config[1].invert) {
		evctrl |= TCC_EVCTRL_TCINV1;
	}

	tcc_module->EVCTRL.reg &= ~evctrl;
}



/**
 * \brief Sets count value for the given TCC module.
 *
 * Sets the timer count value of an initialized TCC module. The
 * specified TCC module can remain running or stopped.
 *
 * \param[in] module_inst  Pointer to the software module instance struct
 * \param[in] count        New timer count value to set
 *
 * \return Status which indicates whether the new value is set.
 *
 * \retval STATUS_OK               The timer count was updated successfully
 * \retval STATUS_ERR_INVALID_ARG  An invalid timer counter size was specified
 */
enum status_code tcc_set_count_value(
		const struct tcc_module *const module_inst,
		const uint32_t count)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance*/
	Tcc *const tcc_module = module_inst->hw;
	/* Get a index of the module */
	uint8_t module_index = _tcc_get_inst_index(tcc_module);

	uint32_t max_count = _tcc_maxs[module_index];

	if (count > max_count) {
		return STATUS_ERR_INVALID_ARG;
	}

	while (tcc_module->SYNCBUSY.reg & TCC_SYNCBUSY_COUNT) {
		/* Wait for sync */
	}

	/* Write to based on the TCC dithering */
	tcc_module->COUNT.reg = (count);

	return STATUS_OK;
}

/**
 * \brief Get count value of the given TCC module.
 *
 * Retrieves the current count value of a TCC module. The specified TCC module
 * can remain running or stopped.
 *
 * \param[in] module_inst  Pointer to the software module instance struct
 *
 * \return Count value of the specified TCC module.
 */
uint32_t tcc_get_count_value(
		const struct tcc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance*/
	Tcc *const tcc_module = module_inst->hw;
	uint32_t last_cmd;

	/* Wait last command done */
	do {
		while (tcc_module->SYNCBUSY.reg & TCC_SYNCBUSY_CTRLB) {
			/* Wait for sync */
		}
		last_cmd = tcc_module->CTRLBSET.reg & TCC_CTRLBSET_CMD_Msk;
		if (TCC_CTRLBSET_CMD_NONE == last_cmd) {
			/* Issue read command and break */
			tcc_module->CTRLBSET.bit.CMD = TCC_CTRLBSET_CMD_READSYNC_Val;
			while (tcc_module->SYNCBUSY.reg & TCC_SYNCBUSY_CTRLB) {
				/* Wait for sync */
			}
			break;
		} else if (TCC_CTRLBSET_CMD_READSYNC == last_cmd) {
			/* Command have been issued */
			break;
		}
	} while (1);

	while (tcc_module->SYNCBUSY.reg & TCC_SYNCBUSY_COUNT) {
		/* Wait for sync */
	}
	return (tcc_module->COUNT.reg);
}



/**
 * \brief Gets the TCC module capture value.
 *
 * Retrieves the capture value in the indicated TCC module capture channel.
 *
 * \param[in]  module_inst    Pointer to the software module instance struct
 * \param[in]  channel_index  Index of the Compare Capture channel to read
 *
 * \return Capture value stored in the specified timer channel.
 */
uint32_t tcc_get_capture_value(
		const struct tcc_module *const module_inst,
		const enum tcc_match_capture_channel channel_index)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	Assert(channel_index < _tcc_cc_nums[_tcc_get_inst_index(module_inst->hw)]);

	/* Get a pointer to the module's hardware instance */
	Tcc *const tcc_module = module_inst->hw;

	while(tcc_module->SYNCBUSY.reg  & (TCC_SYNCBUSY_CC0 << channel_index)) {
		/* Sync wait */
	}

	return tcc_module->CC[channel_index].reg;
}

/**
 * \internal
 * \brief Sets a TCC module compare value/buffer.
 *
 * Writes a compare value to the given TCC module compare/capture channel or
 * buffer one.
 *
 * \param[in]  module_inst    Pointer to the software module instance struct
 * \param[in]  channel_index  Index of the compare channel to write to
 * \param[in]  compare        New compare value/buffer value to set
 * \param[in]  double_buffering_enabled Set to \c true to write to CCBx
 *
 * \return Status of the compare update procedure.
 *
 * \retval  STATUS_OK               The compare value was updated successfully
 * \retval  STATUS_ERR_INVALID_ARG  An invalid channel index was supplied or
 *                                  compare value exceed resolution
 */
static enum status_code _tcc_set_compare_value(
		const struct tcc_module *const module_inst,
		const enum tcc_match_capture_channel channel_index,
		const uint32_t compare,
		const bool double_buffering_enabled)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	Tcc *const tcc_module = module_inst->hw;
	/* Get a index of the module */
	uint8_t module_index = _tcc_get_inst_index(tcc_module);

	/* Check index */
	if (channel_index >= _tcc_cc_nums[module_index]) {
		return STATUS_ERR_INVALID_ARG;
	}

	uint32_t max_count = _tcc_maxs[module_index];

	/* Check compare value */
	if (compare > max_count) {
		return STATUS_ERR_INVALID_ARG;
	}

	if (double_buffering_enabled) {
#if (SAML21) || (SAMC20) || (SAMC21) || (SAML22) || (SAMR30)
		tcc_module->CCBUF[channel_index].reg = compare;
#else
		while(tcc_module->SYNCBUSY.reg  &
				(TCC_SYNCBUSY_CCB0 << channel_index)) {
			/* Sync wait */
		}
		tcc_module->CCB[channel_index].reg = compare;
#endif
	} else {
		while(tcc_module->SYNCBUSY.reg  & (TCC_SYNCBUSY_CC0 << channel_index)) {
			/* Sync wait */
		}
		tcc_module->CC[channel_index].reg = compare;
	}
	return STATUS_OK;
}


/**
 * \brief Sets a TCC module compare value.
 *
 * Writes a compare value to the given TCC module compare/capture channel.
 *
 * If double buffering is enabled it always write to the buffer
 * register. The value will then be updated immediately by calling
 * \ref tcc_force_double_buffer_update(), or be updated when the lock update bit
 * is cleared and the UPDATE condition happen.
 *
 * \param[in]  module_inst    Pointer to the software module instance struct
 * \param[in]  channel_index  Index of the compare channel to write to
 * \param[in]  compare        New compare value to set
 *
 * \return Status of the compare update procedure.
 *
 * \retval  STATUS_OK               The compare value was updated successfully
 * \retval  STATUS_ERR_INVALID_ARG  An invalid channel index was supplied or
 *                                  compare value exceed resolution
 */
enum status_code tcc_set_compare_value(
		const struct tcc_module *const module_inst,
		const enum tcc_match_capture_channel channel_index,
		const uint32_t compare)
{
	/* Sanity check arguments */
	Assert(module_inst);

	return _tcc_set_compare_value(module_inst, channel_index, compare,
			module_inst->double_buffering_enabled);
}

/**
 * \brief Sets a TCC module compare value and buffer value.
 *
 * Writes compare value and buffer to the given TCC module compare/capture
 * channel. Usually as preparation for double buffer or circulared double buffer
 * (circular buffer).
 *
 * \param[in]  module_inst    Pointer to the software module instance struct
 * \param[in]  channel_index  Index of the compare channel to write to
 * \param[in]  compare        New compare value to set
 * \param[in]  compare_buffer New compare buffer value to set
 *
 * \return Status of the compare update procedure.
 *
 * \retval  STATUS_OK               The compare value was updated successfully
 * \retval  STATUS_ERR_INVALID_ARG  An invalid channel index was supplied or
 *                                  compare value exceed resolution
 */
enum status_code tcc_set_double_buffer_compare_values(
		struct tcc_module *const module_inst,
		const enum tcc_match_capture_channel channel_index,
		const uint32_t compare, const uint32_t compare_buffer)
{
	/* Sanity check arguments */
	Assert(module_inst);

	enum status_code status;
	status = _tcc_set_compare_value(module_inst, channel_index, compare, false);
	if (status != STATUS_OK) {
		return status;
	}
	return _tcc_set_compare_value(module_inst, channel_index, compare_buffer,
			true);
}


/**
 * \internal
 * \brief Set the timer TOP/PERIOD buffer/value.
 *
 * This function writes the given value to the PER/PERB register.
 *
 * \param[in] module_inst   Pointer to the software module instance struct
 * \param[in] top_value     New value to be loaded into the PER/PERB register
 * \param[in] double_buffering_enabled Set to \c true to write to PERB
 *
 * \return Status of the TOP set procedure.
 *
 * \retval STATUS_OK              The timer TOP value was updated successfully
 * \retval STATUS_ERR_INVALID_ARG An invalid channel index was supplied or
 *                                top/period value exceed resolution
 */
static enum status_code _tcc_set_top_value(
		const struct tcc_module *const module_inst,
		const uint32_t top_value,
		const bool double_buffering_enabled)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	Tcc *const tcc_module = module_inst->hw;
	/* Get a index of the module */
	uint8_t module_index = _tcc_get_inst_index(tcc_module);

	uint32_t max_count = _tcc_maxs[module_index];

	/* Check compare value */
	if (top_value > max_count) {
		return STATUS_ERR_INVALID_ARG;
	}

	if (double_buffering_enabled) {
#if (SAML21) || (SAMC20) || (SAMC21) || (SAML22) || (SAMR30)
		tcc_module->PERBUF.reg = top_value;
#else
		while(tcc_module->SYNCBUSY.reg  & TCC_SYNCBUSY_PERB) {
			/* Sync wait */
		}
		tcc_module->PERB.reg = top_value;
#endif
	} else {
		while(tcc_module->SYNCBUSY.reg  & TCC_SYNCBUSY_PER) {
			/* Sync wait */
		}
		tcc_module->PER.reg = top_value;
	}
	return STATUS_OK;
}


/**
 * \brief Set the timer TOP/PERIOD value.
 *
 * This function writes the given value to the PER/PERB register.
 *
 * If double buffering is enabled it always write to the buffer
 * register (PERB). The value will then be updated immediately by calling
 * \ref tcc_force_double_buffer_update(), or be updated when the lock update bit
 * is cleared and the UPDATE condition happen.
 *
 * When using MFRQ, the top value is defined by the CC0 register value and the
 * PER value is ignored, so
 * \ref tcc_set_compare_value (module,channel_0,value) must be used instead of
 * this function to change the actual top value in that case.
 * For all other waveforms operation the top value is defined by PER register
 * value.
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 * \param[in]  top_value     New value to be loaded into the PER/PERB register
 *
 * \return Status of the TOP set procedure.
 *
 * \retval STATUS_OK              The timer TOP value was updated successfully
 * \retval STATUS_ERR_INVALID_ARG An invalid channel index was supplied or
 *                                top/period value exceed resolution
 */
enum status_code tcc_set_top_value(
		const struct tcc_module *const module_inst,
		const uint32_t top_value)
{
	/* Sanity check arguments */
	Assert(module_inst);

	return _tcc_set_top_value(module_inst, top_value,
			module_inst->double_buffering_enabled);
}

/**
 * \brief Set the timer TOP/PERIOD value and buffer value.
 *
 * This function writes the given value to the PER and PERB register. Usually as
 * preparation for double buffer or circulared double buffer (circular buffer).
 *
 * When using MFRQ, the top values are defined by the CC0 and CCB0, the PER and
 * PERB values are ignored, so
 * \ref tcc_set_double_buffer_compare_values (module,channel_0,value,buffer) must
 * be used instead of this function to change the actual top values in that
 * case. For all other waveforms operation the top values are defined by PER and
 * PERB registers values.
 *
 * \param[in]  module_inst      Pointer to the software module instance struct
 * \param[in]  top_value        New value to be loaded into the PER register
 * \param[in]  top_buffer_value New value to be loaded into the PERB register
 *
 * \return Status of the TOP set procedure.
 *
 * \retval STATUS_OK              The timer TOP value was updated successfully
 * \retval STATUS_ERR_INVALID_ARG An invalid channel index was supplied or
 *                                top/period value exceed resolution
 */
enum status_code tcc_set_double_buffer_top_values(
		const struct tcc_module *const module_inst,
		const uint32_t top_value, const uint32_t top_buffer_value)
{
	/* Sanity check arguments */
	Assert(module_inst);

	enum status_code status;
	status = _tcc_set_top_value(module_inst, top_value, false);
	if (status != STATUS_OK) {
		return status;
	}
	return _tcc_set_top_value(module_inst, top_buffer_value, true);
}


/**
 * \brief Sets the TCC module waveform output pattern.
 *
 * Force waveform output line to generate specific pattern (0, 1, or as is).
 *
 * If double buffering is enabled it always write to the buffer
 * register. The value will then be updated immediately by calling
 * \ref tcc_force_double_buffer_update(), or be updated when the lock update bit
 * is cleared and the UPDATE condition happen.
 *
 * \param[in]  module_inst Pointer to the software module instance struct
 * \param[in]  line_index  Output line index
 * \param[in]  pattern     Output pattern to use (\ref tcc_output_pattern)
 *
 * \return Status of the pattern set procedure.
 *
 * \retval  STATUS_OK               The PATT register is updated successfully
 * \retval  STATUS_ERR_INVALID_ARG  An invalid line index was supplied
 */
enum status_code tcc_set_pattern(
		const struct tcc_module *const module_inst,
		const uint32_t line_index,
		const enum tcc_output_pattern pattern)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	Tcc *const tcc_module = module_inst->hw;
	/* Get a index of the module */
	uint8_t module_index = _tcc_get_inst_index(tcc_module);
	/* Get number of output lines */
	uint8_t ow_num = _tcc_ow_nums[module_index];

	/* Check if line number is OK */
	if (line_index >= ow_num) {
		return STATUS_ERR_INVALID_ARG;
	}

	uint32_t patt_value;

	while(tcc_module->SYNCBUSY.reg  & TCC_SYNCBUSY_PATT) {
		/* Sync wait */
	}
	patt_value = tcc_module->PATT.reg;
	if (TCC_OUTPUT_PATTERN_DISABLE == pattern) {
		patt_value &= ~(TCC_PATT_PGE0 << line_index);
	} else if (TCC_OUTPUT_PATTERN_0 == pattern) {
		patt_value &= ~(TCC_PATT_PGV0 << line_index);
		patt_value |=  (TCC_PATT_PGE0 << line_index);
	} else {
		patt_value |=  ((TCC_PATT_PGE0 | TCC_PATT_PGV0) << line_index);
	}

	if (module_inst->double_buffering_enabled) {
#if (SAML21) || (SAMC20) || (SAMC21) || (SAML22) || (SAMR30)
		tcc_module->PATTBUF.reg = patt_value;
#else
		while(tcc_module->SYNCBUSY.reg  & TCC_SYNCBUSY_PATTB) {
			/* Sync wait */
		}
		tcc_module->PATTB.reg = patt_value;
#endif
	} else {
		tcc_module->PATT.reg = patt_value;
	}
	return STATUS_OK;
}

/**
 * \brief Retrieves the current module status.
 *
 * Retrieves the status of the module, giving overall state information.
 *
 * \param[in] module_inst  Pointer to the TCC software instance struct
 *
 * \return Bitmask of \c TCC_STATUS_* flags.
 *
 * \retval TCC_STATUS_CHANNEL_MATCH_CAPTURE(n)         Channel n match/capture has occured
 * \retval TCC_STATUS_CHANNEL_OUTPUT(n)                Channel n match/capture output state
 * \retval TCC_STATUS_NON_RECOVERABLE_FAULT_OCCUR(x)   Non-recoverable fault x has occured
 * \retval TCC_STATUS_RECOVERABLE_FAULT_OCCUR(n)       Recoverable fault n has occured
 * \retval TCC_STATUS_NON_RECOVERABLE_FAULT_PRESENT(x) Non-recoverable fault x input present
 * \retval TCC_STATUS_RECOVERABLE_FAULT_PRESENT(n)     Recoverable fault n input present
 * \retval TCC_STATUS_SYNC_READY          None of register is syncing
 * \retval TCC_STATUS_CAPTURE_OVERFLOW    Timer capture data has overflowed
 * \retval TCC_STATUS_COUNTER_EVENT       Timer counter event has occurred
 * \retval TCC_STATUS_COUNT_OVERFLOW      Timer count value has overflowed
 * \retval TCC_STATUS_COUNTER_RETRIGGERED Timer counter has been retriggered
 * \retval TCC_STATUS_STOP                Timer counter has been stopped
 * \retval TCC_STATUS_RAMP_CYCLE_INDEX    Wave ramp index for cycle
 */
uint32_t tcc_get_status(
		struct tcc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	uint32_t int_flags = module_inst->hw->INTFLAG.reg;
	uint32_t status_flags = module_inst->hw->STATUS.reg;
	uint32_t status = 0;
	int i;

	/* SYNC */
	if (module_inst->hw->SYNCBUSY.reg == 0) {
		status |= TCC_STATUS_SYNC_READY;
	}

	/* Channels */
	for (i = 0; i < TCC_NUM_CHANNELS; i++) {
		if (int_flags & TCC_INTFLAG_MC(i)) {
			status |= TCC_STATUS_CHANNEL_MATCH_CAPTURE(i);
		}
		if (status_flags & TCC_STATUS_CMP(i)) {
			status |= TCC_STATUS_CHANNEL_OUTPUT(i);
		}
	}
	/* Non-recoverable fault state */
	if ((int_flags & TCC_INTFLAG_FAULT1) ||
		(status_flags & TCC_STATUS_FAULT1)) {
		status |= TCC_STATUS_NON_RECOVERABLE_FAULT_OCCUR(1);
	}
	if ((int_flags & TCC_INTFLAG_FAULT0) ||
		(status_flags & TCC_STATUS_FAULT0)) {
		status |= TCC_STATUS_NON_RECOVERABLE_FAULT_OCCUR(0);
	}
	/* Non-recoverable fault inputs */
	if (status_flags & TCC_STATUS_FAULT0IN) {
		status |= TCC_STATUS_NON_RECOVERABLE_FAULT_PRESENT(0);
	}
	if (status_flags & TCC_STATUS_FAULT1IN) {
		status |= TCC_STATUS_NON_RECOVERABLE_FAULT_PRESENT(1);
	}
	/* Recoverable fault state */
	if ((int_flags & TCC_INTFLAG_FAULTB) ||
		(status_flags & TCC_STATUS_FAULTB)) {
		status |= TCC_STATUS_RECOVERABLE_FAULT_OCCUR(1);
	}
	if ((int_flags & TCC_INTFLAG_FAULTA) ||
		(status_flags & TCC_STATUS_FAULTA)) {
		status |= TCC_STATUS_RECOVERABLE_FAULT_OCCUR(0);
	}
	/* Recoverable fault inputs */
	if (status_flags & TCC_STATUS_FAULTAIN) {
		status |= TCC_STATUS_RECOVERABLE_FAULT_PRESENT(0);
	}
	if (status_flags & TCC_STATUS_FAULTBIN) {
		status |= TCC_STATUS_RECOVERABLE_FAULT_PRESENT(1);
	}

	/* Check for TCC capture overflow */
	if (int_flags & TCC_INTFLAG_ERR) {
		status |= TCC_STATUS_CAPTURE_OVERFLOW;
	}
	/* Check for TCC count counter */
	if (int_flags & TCC_INTFLAG_CNT) {
		status |= TCC_STATUS_COUNTER_EVENT;
	}
	/* Check for TCC count retrigger */
	if (int_flags & TCC_INTFLAG_TRG) {
		status |= TCC_STATUS_COUNTER_RETRIGGERED;
	}
	/* Check for TCC count overflow */
	if (int_flags & TCC_INTFLAG_OVF) {
		status |= TCC_STATUS_COUNT_OVERFLOW;
	}
	/* Check for TCC count stop */
	if (status_flags & TCC_STATUS_STOP) {
		status |= TCC_STATUS_STOPPED;
	}
	/* Check for TCC RAMP index */
	if (status_flags & TCC_STATUS_IDX) {
		status |= TCC_STATUS_RAMP_CYCLE_INDEX;
	}
	return status;
}

/**
 * \brief Clears a module status flag.
 *
 * Clears the given status flag of the module.
 *
 * \param[in] module_inst   Pointer to the TCC software instance struct
 * \param[in] status_flags  Bitmask of \c TCC_STATUS_* flags to clear
 */
void tcc_clear_status(
		struct tcc_module *const module_inst,
		const uint32_t status_flags)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	uint32_t int_clr = 0;
	uint32_t status_clr = 0;
	int i;

	/* Channels */
	for (i = 0; i < TCC_NUM_CHANNELS; i++) {
		if (status_flags & TCC_STATUS_CHANNEL_MATCH_CAPTURE(i)) {
			int_clr |= TCC_INTFLAG_MC(i);
		}
	}
	/* Faults */
	if (status_flags & TCC_STATUS_NON_RECOVERABLE_FAULT_OCCUR(1)) {
		int_clr |= TCC_INTFLAG_FAULT1;
		status_clr |= TCC_STATUS_FAULT1;
	}
	if (status_flags & TCC_STATUS_NON_RECOVERABLE_FAULT_OCCUR(0)) {
		int_clr |= TCC_INTFLAG_FAULT0;
		status_clr |= TCC_STATUS_FAULT0;
	}
	if (status_flags & TCC_STATUS_RECOVERABLE_FAULT_OCCUR(1)) {
		int_clr |= TCC_INTFLAG_FAULTB;
		status_clr |= TCC_STATUS_FAULTB;
	}
	if (status_flags & TCC_STATUS_RECOVERABLE_FAULT_OCCUR(0)) {
		int_clr |= TCC_INTFLAG_FAULTA;
		status_clr |= TCC_STATUS_FAULTA;
	}
	/* Check for TCC capture overflow */
	if (status_flags & TCC_STATUS_CAPTURE_OVERFLOW) {
		int_clr |= TCC_INTFLAG_ERR;
	}
	/* Check for TCC count counter */
	if (status_flags & TCC_STATUS_COUNTER_EVENT) {
		int_clr |= TCC_INTFLAG_CNT;
	}
	/* Check for TCC count retrigger */
	if (status_flags & TCC_STATUS_COUNTER_RETRIGGERED) {
		int_clr = TCC_INTFLAG_TRG;
	}
	/* Check for TCC count overflow */
	if (status_flags & TCC_STATUS_COUNT_OVERFLOW) {
		int_clr |= TCC_INTFLAG_OVF;
	}
	/* Clear status flag */
	module_inst->hw->STATUS.reg = status_clr;
	/* Clear interrupt flag */
	module_inst->hw->INTFLAG.reg = int_clr;
}

/**
 * \brief Enable circular option for double buffered compare values.
 *
 * Enable circular option for the double buffered channel compare values.
 * On each UPDATE condition, the contents of CCBx and CCx are switched, meaning
 * that the contents of CCBx are transferred to CCx and the contents of CCx are
 * transferred to CCBx.
 *
 * \param[in] module_inst     Pointer to the TCC software instance struct
 * \param[in] channel_index   Index of the compare channel to set up to
 *
 * \retval STATUS_OK           The module was initialized successfully
 * \retval STATUS_INVALID_ARG  An invalid channel index is supplied
 */
enum status_code tcc_enable_circular_buffer_compare(
		struct tcc_module *const module_inst,
		enum tcc_match_capture_channel channel_index)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	Tcc *const tcc_module = module_inst->hw;
	/* Get a index of the module */
	uint8_t module_index = _tcc_get_inst_index(tcc_module);

	/* Check index */
	if (channel_index > 3) {
		return STATUS_ERR_INVALID_ARG;
	}
	if (channel_index >= _tcc_cc_nums[module_index]) {
		return STATUS_ERR_INVALID_ARG;
	}

	tcc_module->WAVE.reg |=  (TCC_WAVE_CICCEN0 << channel_index);

	return STATUS_OK;
}

/**
 * \brief Disable circular option for double buffered compare values.
 *
 * Stop circularing the double buffered compare values.
 *
 * \param[in] module_inst     Pointer to the TCC software instance struct
 * \param[in] channel_index   Index of the compare channel to set up to
 *
 * \retval STATUS_OK           The module was initialized successfully
 * \retval STATUS_INVALID_ARG  An invalid channel index is supplied
 */
enum status_code tcc_disable_circular_buffer_compare(
		struct tcc_module *const module_inst,
		enum tcc_match_capture_channel channel_index)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	Tcc *const tcc_module = module_inst->hw;
	/* Get a index of the module */
	uint8_t module_index = _tcc_get_inst_index(tcc_module);

	/* Check index */
	if (channel_index > 3) {
		return STATUS_ERR_INVALID_ARG;
	}
	if (channel_index >= _tcc_cc_nums[module_index]) {
		return STATUS_ERR_INVALID_ARG;
	}

	tcc_module->WAVE.reg &= ~(TCC_WAVE_CICCEN0 << channel_index);

	return STATUS_OK;
}
