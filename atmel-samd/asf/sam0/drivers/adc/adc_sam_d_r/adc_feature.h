/**
 * \file
 *
 * \brief SAM ADC functionality
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef ADC_FEATURE_H_INCLUDED
#define ADC_FEATURE_H_INCLUDED

/**
 * \addtogroup asfdoc_sam0_adc_group
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#if ADC_CALLBACK_MODE == true
#  include <system_interrupt.h>

#if !defined(__DOXYGEN__)
extern struct adc_module *_adc_instances[ADC_INST_NUM];
#endif

/** Forward definition of the device instance. */
struct adc_module;

/** Type of the callback functions. */
typedef void (*adc_callback_t)(struct adc_module *const module);

/**
 * \brief ADC Callback enum
 *
 * Callback types for ADC callback driver.
 *
 */
enum adc_callback {
	/** Callback for buffer received */
	ADC_CALLBACK_READ_BUFFER,
	/** Callback when window is hit */
	ADC_CALLBACK_WINDOW,
	/** Callback for error */
	ADC_CALLBACK_ERROR,
#  if !defined(__DOXYGEN__)
	/** Number of available callbacks */
	ADC_CALLBACK_N,
#  endif
};

#endif

/**
 * \brief ADC reference voltage enum
 *
 * Enum for the possible reference voltages for the ADC.
 *
 */
enum adc_reference {
	/** 1.0V voltage reference */
	ADC_REFERENCE_INT1V   = ADC_REFCTRL_REFSEL_INT1V,
	/** 1/1.48V<SUB>CC</SUB> reference */
	ADC_REFERENCE_INTVCC0 = ADC_REFCTRL_REFSEL_INTVCC0,
	/** 1/2V<SUB>CC</SUB> (only for internal V<SUB>CC</SUB> > 2.1V) */
	ADC_REFERENCE_INTVCC1 = ADC_REFCTRL_REFSEL_INTVCC1,
	/** External reference A */
	ADC_REFERENCE_AREFA   = ADC_REFCTRL_REFSEL_AREFA,
	/** External reference B */
	ADC_REFERENCE_AREFB   = ADC_REFCTRL_REFSEL_AREFB,
};

/**
 * \brief ADC clock prescaler enum
 *
 * Enum for the possible clock prescaler values for the ADC.
 *
 */
enum adc_clock_prescaler {
	/** ADC clock division factor 4 */
	ADC_CLOCK_PRESCALER_DIV4   = ADC_CTRLB_PRESCALER_DIV4,
	/** ADC clock division factor 8 */
	ADC_CLOCK_PRESCALER_DIV8   = ADC_CTRLB_PRESCALER_DIV8,
	/** ADC clock division factor 16 */
	ADC_CLOCK_PRESCALER_DIV16  = ADC_CTRLB_PRESCALER_DIV16,
	/** ADC clock division factor 32 */
	ADC_CLOCK_PRESCALER_DIV32  = ADC_CTRLB_PRESCALER_DIV32,
	/** ADC clock division factor 64 */
	ADC_CLOCK_PRESCALER_DIV64  = ADC_CTRLB_PRESCALER_DIV64,
	/** ADC clock division factor 128 */
	ADC_CLOCK_PRESCALER_DIV128 = ADC_CTRLB_PRESCALER_DIV128,
	/** ADC clock division factor 256 */
	ADC_CLOCK_PRESCALER_DIV256 = ADC_CTRLB_PRESCALER_DIV256,
	/** ADC clock division factor 512 */
	ADC_CLOCK_PRESCALER_DIV512 = ADC_CTRLB_PRESCALER_DIV512,
};

/**
 * \brief ADC resolution enum
 *
 * Enum for the possible resolution values for the ADC.
 *
 */
enum adc_resolution {
	/** ADC 12-bit resolution */
	ADC_RESOLUTION_12BIT = ADC_CTRLB_RESSEL_12BIT,
	/** ADC 16-bit resolution using oversampling and decimation */
	ADC_RESOLUTION_16BIT = ADC_CTRLB_RESSEL_16BIT,
	/** ADC 10-bit resolution */
	ADC_RESOLUTION_10BIT = ADC_CTRLB_RESSEL_10BIT,
	/** ADC 8-bit resolution */
	ADC_RESOLUTION_8BIT  = ADC_CTRLB_RESSEL_8BIT,
	/** ADC 13-bit resolution using oversampling and decimation */
	ADC_RESOLUTION_13BIT,
	/** ADC 14-bit resolution using oversampling and decimation */
	ADC_RESOLUTION_14BIT,
	/** ADC 15-bit resolution using oversampling and decimation */
	ADC_RESOLUTION_15BIT,
	/** ADC 16-bit result register for use with averaging. When using this mode
	  * the ADC result register will be set to 16-bit wide, and the number of
	  * samples to accumulate and the division factor is configured by the
	  * \ref adc_config.accumulate_samples and \ref adc_config.divide_result
	  * members in the configuration struct.
	  */
	ADC_RESOLUTION_CUSTOM,
};

/**
 * \brief ADC window monitor mode enum
 *
 * Enum for the possible window monitor modes for the ADC.
 *
 */
enum adc_window_mode {
	/** No window mode */
	ADC_WINDOW_MODE_DISABLE          = ADC_WINCTRL_WINMODE_DISABLE,
	/** RESULT > WINLT */
	ADC_WINDOW_MODE_ABOVE_LOWER      = ADC_WINCTRL_WINMODE_MODE1,
	/** RESULT < WINUT */
	ADC_WINDOW_MODE_BELOW_UPPER      = ADC_WINCTRL_WINMODE_MODE2,
	/** WINLT < RESULT < WINUT */
	ADC_WINDOW_MODE_BETWEEN          = ADC_WINCTRL_WINMODE_MODE3,
	/** !(WINLT < RESULT < WINUT) */
	ADC_WINDOW_MODE_BETWEEN_INVERTED = ADC_WINCTRL_WINMODE_MODE4,
};

/**
 * \brief ADC gain factor selection enum
 *
 * Enum for the possible gain factor values for the ADC.
 *
 */
enum adc_gain_factor {
	/** 1x gain */
	ADC_GAIN_FACTOR_1X   = ADC_INPUTCTRL_GAIN_1X,
	/** 2x gain */
	ADC_GAIN_FACTOR_2X   = ADC_INPUTCTRL_GAIN_2X,
	/** 4x gain */
	ADC_GAIN_FACTOR_4X   = ADC_INPUTCTRL_GAIN_4X,
	/** 8x gain */
	ADC_GAIN_FACTOR_8X   = ADC_INPUTCTRL_GAIN_8X,
	/** 16x gain */
	ADC_GAIN_FACTOR_16X  = ADC_INPUTCTRL_GAIN_16X,
	/** 1/2x gain */
	ADC_GAIN_FACTOR_DIV2 = ADC_INPUTCTRL_GAIN_DIV2,
};

/**
 * \brief ADC event action enum
 *
 * Enum for the possible actions to take on an incoming event.
 *
 */
enum adc_event_action {
	/** Event action disabled */
	ADC_EVENT_ACTION_DISABLED         = 0,
	/** Flush ADC and start conversion */
	ADC_EVENT_ACTION_FLUSH_START_CONV = ADC_EVCTRL_SYNCEI,
	/** Start conversion */
	ADC_EVENT_ACTION_START_CONV       = ADC_EVCTRL_STARTEI,
};

/**
 * \brief ADC positive MUX input selection enum
 *
 * Enum for the possible positive MUX input selections for the ADC.
 *
 */
enum adc_positive_input {
	/** ADC0 pin */
	ADC_POSITIVE_INPUT_PIN0          = ADC_INPUTCTRL_MUXPOS_PIN0,
	/** ADC1 pin */
	ADC_POSITIVE_INPUT_PIN1          = ADC_INPUTCTRL_MUXPOS_PIN1,
	/** ADC2 pin */
	ADC_POSITIVE_INPUT_PIN2          = ADC_INPUTCTRL_MUXPOS_PIN2,
	/** ADC3 pin */
	ADC_POSITIVE_INPUT_PIN3          = ADC_INPUTCTRL_MUXPOS_PIN3,
	/** ADC4 pin */
	ADC_POSITIVE_INPUT_PIN4          = ADC_INPUTCTRL_MUXPOS_PIN4,
	/** ADC5 pin */
	ADC_POSITIVE_INPUT_PIN5          = ADC_INPUTCTRL_MUXPOS_PIN5,
	/** ADC6 pin */
	ADC_POSITIVE_INPUT_PIN6          = ADC_INPUTCTRL_MUXPOS_PIN6,
	/** ADC7 pin */
	ADC_POSITIVE_INPUT_PIN7          = ADC_INPUTCTRL_MUXPOS_PIN7,
	/** ADC8 pin */
	ADC_POSITIVE_INPUT_PIN8          = ADC_INPUTCTRL_MUXPOS_PIN8,
	/** ADC9 pin */
	ADC_POSITIVE_INPUT_PIN9          = ADC_INPUTCTRL_MUXPOS_PIN9,
	/** ADC10 pin */
	ADC_POSITIVE_INPUT_PIN10         = ADC_INPUTCTRL_MUXPOS_PIN10,
	/** ADC11 pin */
	ADC_POSITIVE_INPUT_PIN11         = ADC_INPUTCTRL_MUXPOS_PIN11,
	/** ADC12 pin */
	ADC_POSITIVE_INPUT_PIN12         = ADC_INPUTCTRL_MUXPOS_PIN12,
	/** ADC13 pin */
	ADC_POSITIVE_INPUT_PIN13         = ADC_INPUTCTRL_MUXPOS_PIN13,
	/** ADC14 pin */
	ADC_POSITIVE_INPUT_PIN14         = ADC_INPUTCTRL_MUXPOS_PIN14,
	/** ADC15 pin */
	ADC_POSITIVE_INPUT_PIN15         = ADC_INPUTCTRL_MUXPOS_PIN15,
	/** ADC16 pin */
	ADC_POSITIVE_INPUT_PIN16         = ADC_INPUTCTRL_MUXPOS_PIN16,
	/** ADC17 pin */
	ADC_POSITIVE_INPUT_PIN17         = ADC_INPUTCTRL_MUXPOS_PIN17,
	/** ADC18 pin */
	ADC_POSITIVE_INPUT_PIN18         = ADC_INPUTCTRL_MUXPOS_PIN18,
	/** ADC19 pin */
	ADC_POSITIVE_INPUT_PIN19         = ADC_INPUTCTRL_MUXPOS_PIN19,
	/** Temperature reference */
	ADC_POSITIVE_INPUT_TEMP          = ADC_INPUTCTRL_MUXPOS_TEMP,
	/** Bandgap voltage */
	ADC_POSITIVE_INPUT_BANDGAP       = ADC_INPUTCTRL_MUXPOS_BANDGAP,
	/** 1/4 scaled core supply */
	ADC_POSITIVE_INPUT_SCALEDCOREVCC = ADC_INPUTCTRL_MUXPOS_SCALEDCOREVCC,
	/** 1/4 scaled I/O supply */
	ADC_POSITIVE_INPUT_SCALEDIOVCC   = ADC_INPUTCTRL_MUXPOS_SCALEDIOVCC,
	/** DAC input */
	ADC_POSITIVE_INPUT_DAC           = ADC_INPUTCTRL_MUXPOS_DAC,
};

/**
 * \brief ADC negative Multiplexer(MUX) input selection enum
 *
 * Enum for the possible negative Multiplexer(MUX) input selections for the ADC.
 *
 */
enum adc_negative_input {
	/** ADC0 pin */
	ADC_NEGATIVE_INPUT_PIN0          = ADC_INPUTCTRL_MUXNEG_PIN0,
	/** ADC1 pin */
	ADC_NEGATIVE_INPUT_PIN1          = ADC_INPUTCTRL_MUXNEG_PIN1,
	/** ADC2 pin */
	ADC_NEGATIVE_INPUT_PIN2          = ADC_INPUTCTRL_MUXNEG_PIN2,
	/** ADC3 pin */
	ADC_NEGATIVE_INPUT_PIN3          = ADC_INPUTCTRL_MUXNEG_PIN3,
	/** ADC4 pin */
	ADC_NEGATIVE_INPUT_PIN4          = ADC_INPUTCTRL_MUXNEG_PIN4,
	/** ADC5 pin */
	ADC_NEGATIVE_INPUT_PIN5          = ADC_INPUTCTRL_MUXNEG_PIN5,
	/** ADC6 pin */
	ADC_NEGATIVE_INPUT_PIN6          = ADC_INPUTCTRL_MUXNEG_PIN6,
	/** ADC7 pin */
	ADC_NEGATIVE_INPUT_PIN7          = ADC_INPUTCTRL_MUXNEG_PIN7,
	/** Internal ground */
	ADC_NEGATIVE_INPUT_GND           = ADC_INPUTCTRL_MUXNEG_GND,
	/** I/O ground */
	ADC_NEGATIVE_INPUT_IOGND         = ADC_INPUTCTRL_MUXNEG_IOGND,
};

/**
 * \brief ADC number of accumulated samples enum
 *
 * Enum for the possible numbers of ADC samples to accumulate.
 * This setting is only used when the \ref ADC_RESOLUTION_CUSTOM
 * resolution setting is used.
 *
 */
enum adc_accumulate_samples {
	/** No averaging */
	ADC_ACCUMULATE_DISABLE      = ADC_AVGCTRL_SAMPLENUM_1,
	/** Average 2 samples */
	ADC_ACCUMULATE_SAMPLES_2    = ADC_AVGCTRL_SAMPLENUM_2,
	/** Average 4 samples */
	ADC_ACCUMULATE_SAMPLES_4    = ADC_AVGCTRL_SAMPLENUM_4,
	/** Average 8 samples */
	ADC_ACCUMULATE_SAMPLES_8    = ADC_AVGCTRL_SAMPLENUM_8,
	/** Average 16 samples */
	ADC_ACCUMULATE_SAMPLES_16   = ADC_AVGCTRL_SAMPLENUM_16,
	/** Average 32 samples */
	ADC_ACCUMULATE_SAMPLES_32   = ADC_AVGCTRL_SAMPLENUM_32,
	/** Average 64 samples */
	ADC_ACCUMULATE_SAMPLES_64   = ADC_AVGCTRL_SAMPLENUM_64,
	/** Average 128 samples */
	ADC_ACCUMULATE_SAMPLES_128  = ADC_AVGCTRL_SAMPLENUM_128,
	/** Average 256 samples */
	ADC_ACCUMULATE_SAMPLES_256  = ADC_AVGCTRL_SAMPLENUM_256,
	/** Average 512 samples */
	ADC_ACCUMULATE_SAMPLES_512  = ADC_AVGCTRL_SAMPLENUM_512,
	/** Average 1024 samples */
	ADC_ACCUMULATE_SAMPLES_1024 = ADC_AVGCTRL_SAMPLENUM_1024,
};

/**
 * \brief ADC possible dividers for the result register
 *
 * Enum for the possible division factors to use when accumulating
 * multiple samples. To keep the same resolution for the averaged
 * result and the actual input value, the division factor must
 * be equal to the number of samples accumulated. This setting is only
 * used when the \ref ADC_RESOLUTION_CUSTOM resolution setting is used.
 */
enum adc_divide_result {
	/** Don't divide result register after accumulation */
	ADC_DIVIDE_RESULT_DISABLE = 0,
	/** Divide result register by 2 after accumulation */
	ADC_DIVIDE_RESULT_2       = 1,
	/** Divide result register by 4 after accumulation */
	ADC_DIVIDE_RESULT_4       = 2,
	/** Divide result register by 8 after accumulation */
	ADC_DIVIDE_RESULT_8       = 3,
	/** Divide result register by 16 after accumulation */
	ADC_DIVIDE_RESULT_16      = 4,
	/** Divide result register by 32 after accumulation */
	ADC_DIVIDE_RESULT_32      = 5,
	/** Divide result register by 64 after accumulation */
	ADC_DIVIDE_RESULT_64      = 6,
	/** Divide result register by 128 after accumulation */
	ADC_DIVIDE_RESULT_128     = 7,
};

#if ADC_CALLBACK_MODE == true
/**
 * Enum for the possible ADC interrupt flags.
 */
enum adc_interrupt_flag {
	/** ADC result ready */
	ADC_INTERRUPT_RESULT_READY = ADC_INTFLAG_RESRDY,
	/** Window monitor match */
	ADC_INTERRUPT_WINDOW       = ADC_INTFLAG_WINMON,
	/** ADC result overwritten before read */
	ADC_INTERRUPT_OVERRUN      = ADC_INTFLAG_OVERRUN,
};
#endif

/**
 * \brief ADC oversampling and decimation enum
 *
 * Enum for the possible numbers of bits resolution can be increased by when
 * using oversampling and decimation.
 *
 */
enum adc_oversampling_and_decimation {
	/** Don't use oversampling and decimation mode */
	ADC_OVERSAMPLING_AND_DECIMATION_DISABLE = 0,
	/** 1-bit resolution increase */
	ADC_OVERSAMPLING_AND_DECIMATION_1BIT,
	/** 2-bit resolution increase */
	ADC_OVERSAMPLING_AND_DECIMATION_2BIT,
	/** 3-bit resolution increase */
	ADC_OVERSAMPLING_AND_DECIMATION_3BIT,
	/** 4-bit resolution increase */
	ADC_OVERSAMPLING_AND_DECIMATION_4BIT
};

/**
 * \brief Window monitor configuration structure
 *
 * Window monitor configuration structure.
 */
struct adc_window_config {
	/** Selected window mode */
	enum adc_window_mode window_mode;
	/** Lower window value */
	int32_t window_lower_value;
	/** Upper window value */
	int32_t window_upper_value;
};

/**
 * \brief ADC event enable/disable structure.
 *
 * Event flags for the ADC module. This is used to enable and
 * disable events via \ref adc_enable_events() and \ref adc_disable_events().
 */
struct adc_events {
	/** Enable event generation on conversion done */
	bool generate_event_on_conversion_done;
	/** Enable event generation on window monitor */
	bool generate_event_on_window_monitor;
};

/**
 * \brief Gain and offset correction configuration structure
 *
 * Gain and offset correction configuration structure.
 * Part of the \ref adc_config struct and will  be initialized by
 * \ref adc_get_config_defaults.
 */
struct adc_correction_config {
	/**
	 * Enables correction for gain and offset based on values of gain_correction and
	 * offset_correction if set to true
	 */
	bool correction_enable;
	/**
	 * This value defines how the ADC conversion result is compensated for gain
	 * error before written to the result register. This is a fractional value,
	 * 1-bit integer plus an 11-bit fraction, therefore
	 * 1/2 <= gain_correction < 2. Valid \c gain_correction values ranges from
	 * \c 0b010000000000 to \c 0b111111111111.
	 */
	uint16_t gain_correction;
	/**
	 * This value defines how the ADC conversion result is compensated for
	 * offset error before written to the result register. This is a 12-bit
	 * value in two's complement format.
	 */
	int16_t offset_correction;
};

/**
 * \brief Pin scan configuration structure
 *
 * Pin scan configuration structure. Part of the \ref adc_config struct and will
 * be initialized by \ref adc_get_config_defaults.
 */
struct adc_pin_scan_config {
	/**
	 * Offset (relative to selected positive input) of the first input pin to be
	 * used in pin scan mode
	 */
	uint8_t offset_start_scan;
	/**
	 * Number of input pins to scan in pin scan mode. A value below two will
	 * disable pin scan mode.
	 */
	uint8_t inputs_to_scan;
};

/**
 * \brief ADC configuration structure
 *
 * Configuration structure for an ADC instance. This structure should be
 * initialized by the \ref adc_get_config_defaults()
 * function before being modified by the user application.
 */
struct adc_config {
	/** GCLK generator used to clock the peripheral */
	enum gclk_generator clock_source;
	/** Voltage reference */
	enum adc_reference reference;
	/** Clock prescaler */
	enum adc_clock_prescaler clock_prescaler;
	/** Result resolution */
	enum adc_resolution resolution;
	/** Gain factor */
	enum adc_gain_factor gain_factor;
	/** Positive Multiplexer (MUX) input */
	enum adc_positive_input positive_input;
	/** Negative MUX input. For singled-ended conversion mode, the negative
	 * input must be connected to ground. This ground could be the internal
	 * GND, IOGND or an external ground connected to a pin. */
	enum adc_negative_input negative_input;
	/** Number of ADC samples to accumulate when using the
	 *  \c ADC_RESOLUTION_CUSTOM mode. Note: if the result width increases,
	 *  result resolution will be changed accordingly.
	 */
	enum adc_accumulate_samples accumulate_samples;
	/** Division ration when using the ADC_RESOLUTION_CUSTOM mode */
	enum adc_divide_result divide_result;
	/** Left adjusted result */
	bool left_adjust;
	/** Enables differential mode if true. 
	 * if false, ADC will run in singled-ended mode. */
	bool differential_mode;
	/** Enables free running mode if true */
	bool freerunning;
	/** Enables ADC in standby sleep mode if true */
	bool run_in_standby;
	/**
	 * Enables reference buffer offset compensation if true.
	 * This will increase the accuracy of the gain stage, but decreases the input
	 * impedance; therefore the startup time of the reference must be increased.
	 */
	bool reference_compensation_enable;
	/**
	 * This value (0-63) control the ADC sampling time in number of half ADC
	 * prescaled clock cycles (depends of \c ADC_PRESCALER value), thus
	 * controlling the ADC input impedance. Sampling time is set according to
	 * the formula:
	 * Sample time = (sample_length+1) * (ADCclk / 2).
	 */
	uint8_t sample_length;
	/** Window monitor configuration structure */
	struct adc_window_config window;
	/** Gain and offset correction configuration structure */
	struct adc_correction_config correction;
	/** Event action to take on incoming event */
	enum adc_event_action event_action;
	/** Pin scan configuration structure */
	struct adc_pin_scan_config pin_scan;
};

/**
 * \brief ADC software device instance structure.
 *
 * ADC software instance structure, used to retain software state information
 * of an associated hardware module instance.
 *
 * \note The fields of this structure should not be altered by the user
 *       application; they are reserved for module-internal use only.
 */
struct adc_module {
#if !defined(__DOXYGEN__)
	/** Pointer to ADC hardware module */
	Adc *hw;
	/** Keep reference configuration so we know when enable is called */
	enum adc_reference reference;
#  if ADC_CALLBACK_MODE == true
	/** Array to store callback functions */
	adc_callback_t callback[ADC_CALLBACK_N];
	/** Pointer to buffer used for ADC results */
	volatile uint16_t *job_buffer;
	/** Remaining number of conversions in current job */
	volatile uint16_t remaining_conversions;
	/** Bit mask for callbacks registered */
	uint8_t registered_callback_mask;
	/** Bit mask for callbacks enabled */
	uint8_t enabled_callback_mask;
	/** Holds the status of the ongoing or last conversion job */
	volatile enum status_code job_status;
	/** If software triggering is needed */
	bool software_trigger;
#  endif
#endif
};

#if !defined(__DOXYGEN__)

/**
 * \brief Determines if the hardware module(s) are currently synchronizing to the bus.
 *
 * Checks to see if the underlying hardware peripheral module(s) are currently
 * synchronizing across multiple clock domains to the hardware bus. This
 * function can be used to delay further operations on a module until such time
 * that it is ready, to prevent blocking delays for synchronization in the
 * user application.
 *
 * \param[in] module_inst  Pointer to the ADC software instance struct
 *
 * \return Synchronization status of the underlying hardware module(s).
 *
 * \retval true if the module synchronization is ongoing
 * \retval false if the module has completed synchronization
 */
static inline bool adc_is_syncing(
	struct adc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);

	Adc *const adc_module = module_inst->hw;

	if (adc_module->STATUS.reg & ADC_STATUS_SYNCBUSY) {
		return true;
	}

	return false;
}
#endif

/**
 * \name ADC Gain and Pin Scan Mode
 * @{
 */

/**
 * \brief Sets ADC gain factor
 *
 * Sets the ADC gain factor to a specified gain setting.
 *
 * \param[in] module_inst  Pointer to the ADC software instance struct
 * \param[in] gain_factor  Gain factor value to set
 */
static inline void adc_set_gain(
		struct adc_module *const module_inst,
		const enum adc_gain_factor gain_factor)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}

	/* Set new gain factor */
	adc_module->INPUTCTRL.reg =
			(adc_module->INPUTCTRL.reg & ~ADC_INPUTCTRL_GAIN_Msk) |
			(gain_factor);
}

/**
 * \brief Sets the ADC pin scan mode
 *
 * Configures the pin scan mode of the ADC module. In pin scan mode, the first
 * conversion will start at the configured positive input + start_offset. When
 * a conversion is done, a conversion will start on the next input, until
 * \c inputs_to_scan number of conversions are made.
 *
 * \param[in] module_inst     Pointer to the ADC software instance struct
 * \param[in] inputs_to_scan  Number of input pins to perform a conversion on
 *                            (must be two or more)
 * \param[in] start_offset    Offset of first pin to scan (relative to
 *                            configured positive input)
 *
 * \return Status of the pin scan configuration set request.
 *
 * \retval STATUS_OK               Pin scan mode has been set successfully
 * \retval STATUS_ERR_INVALID_ARG  Number of input pins to scan or offset has
 *                                 an invalid value
 */
static inline enum status_code adc_set_pin_scan_mode(
		struct adc_module *const module_inst,
		uint8_t inputs_to_scan,
		const uint8_t start_offset)

{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

	if (inputs_to_scan > 0) {
		/*
		* Number of input sources included is the value written to INPUTSCAN
		* plus 1.
		*/
		inputs_to_scan--;
	}

	if (inputs_to_scan > (ADC_INPUTCTRL_INPUTSCAN_Msk >> ADC_INPUTCTRL_INPUTSCAN_Pos) ||
			start_offset > (ADC_INPUTCTRL_INPUTOFFSET_Msk >> ADC_INPUTCTRL_INPUTOFFSET_Pos)) {
		/* Invalid number of input pins */
		return STATUS_ERR_INVALID_ARG;
	}

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}

	/* Set pin scan mode */
	adc_module->INPUTCTRL.reg =
			(adc_module->INPUTCTRL.reg &
			~(ADC_INPUTCTRL_INPUTSCAN_Msk | ADC_INPUTCTRL_INPUTOFFSET_Msk)) |
			(start_offset   << ADC_INPUTCTRL_INPUTOFFSET_Pos) |
			(inputs_to_scan << ADC_INPUTCTRL_INPUTSCAN_Pos);

	return STATUS_OK;
}

/**
 * \brief Disables pin scan mode
 *
 * Disables pin scan mode. The next conversion will be made on only one pin
 * (the configured positive input pin).
 *
 * \param[in] module_inst  Pointer to the ADC software instance struct
 */
static inline void adc_disable_pin_scan_mode(
		struct adc_module *const module_inst)
{
	/* Disable pin scan mode */
	adc_set_pin_scan_mode(module_inst, 0, 0);
}

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* ADC_FEATURE_H_INCLUDED */

