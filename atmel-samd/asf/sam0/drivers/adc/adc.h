/**
 * \file
 *
 * \brief SAM Peripheral Analog-to-Digital Converter Driver
 *
 * Copyright (C) 2012-2016 Atmel Corporation. All rights reserved.
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

#ifndef ADC_H_INCLUDED
#define ADC_H_INCLUDED

/**
 * \defgroup asfdoc_sam0_adc_group SAM Analog-to-Digital Converter (ADC) Driver
 *
 * This driver for Atmel&reg; | SMART ARM&reg;-based microcontrollers provides an interface for the configuration
 * and management of the device's Analog-to-Digital Converter functionality, for
 * the conversion of analog voltages into a corresponding digital form.
 * The following driver Application Programming Interface (API) modes are covered by this manual:
 * - Polled APIs
 * \if ADC_CALLBACK_MODE
 * - Callback APIs
 * \endif
 *
 * The following peripheral is used by this module:
 *  - ADC (Analog-to-Digital Converter)
 *
 * The following devices can use this module:
 * \if DEVICE_SAML21_SUPPORT
 *  - Atmel | SMART SAM L21/L22
 *  - Atmel | SMART SAM C20/C21
 * \else
 *  - Atmel | SMART SAM D20/D21
 *  - Atmel | SMART SAM R21
 *  - Atmel | SMART SAM D09/D10/D11
 *  - Atmel | SMART SAM DA1
 * \endif
 *
 * The outline of this documentation is as follows:
 *  - \ref asfdoc_sam0_adc_prerequisites
 *  - \ref asfdoc_sam0_adc_module_overview
 *  - \ref asfdoc_sam0_adc_special_considerations
 *  - \ref asfdoc_sam0_adc_extra_info
 *  - \ref asfdoc_sam0_adc_examples
 *  - \ref asfdoc_sam0_adc_api_overview
 *
 *
 * \section asfdoc_sam0_adc_prerequisites Prerequisites
 *
 * There are no prerequisites for this module.
 *
 *
 * \section asfdoc_sam0_adc_module_overview Module Overview
 *
 * This driver provides an interface for the Analog-to-Digital conversion
 * functions on the device, to convert analog voltages to a corresponding
 * digital value. The ADC has up to 12-bit resolution, and is capable of
 * \if DEVICE_SAML21_SUPPORT
 * converting up to 1,000,000 samples per second (MSPS).
 * \else
 * converting up to 500K samples per second (KSPS).
 * \endif
 *
 * The ADC has a compare function for accurate monitoring of user defined
 * thresholds with minimum software intervention required.
 * The ADC may be configured for 8-, 10-, or 12-bit result, reducing the
 * conversion time. ADC conversion results are provided left or right adjusted
 * which eases calculation when the result is represented as a signed integer.
 *
 * The input selection is flexible, and both single-ended and differential
 * measurements can be made. For differential measurements, an optional gain
 * stage is available to increase the dynamic range. In addition, several
 * internal signal inputs are available. The ADC can provide both signed and
 * unsigned results.
 *
 * The ADC measurements can either be started by application software or an
 * incoming event from another peripheral in the device, and both internal and
 * external reference voltages can be selected.
 *
 * \note Internal references will be enabled by the driver, but not disabled.
 *       Any reference not used by the application should be disabled by the application.
 *
 * A simplified block diagram of the ADC can be seen in
 * \ref asfdoc_sam0_adc_module_block_diagram "the figure below".
 *
 * \anchor asfdoc_sam0_adc_module_block_diagram
 * \dot
 * digraph overview {
 * splines = false;
 * rankdir=LR;
 *
 * mux1 [label="Positive input", shape=box];
 * mux2 [label="Negative input", shape=box];
 *
 *
 * mux3 [label="Reference", shape=box];
 *
 * adc [label="ADC", shape=polygon, sides=5, orientation=90, distortion=-0.6, style=filled, fillcolor=darkolivegreen1, height=1, width=1];
 * prescaler [label="PRESCALER", shape=box, style=filled, fillcolor=lightblue];
 *
 * mux1 -> adc;
 * mux2 -> adc;
 * mux3 -> adc:sw;
 * prescaler -> adc;
 *
 * postproc [label="Post processing", shape=box];
 * result [label="RESULT", shape=box, style=filled, fillcolor=lightblue];
 *
 * adc:e -> postproc:w;
 * postproc:e -> result:w;
 *
 * {rank=same; mux1 mux2}
 * {rank=same; prescaler adc}
 *
 * }
 * \enddot
 *
 *
 * \subsection asfdoc_sam0_adc_module_overview_prescaler Sample Clock Prescaler
 * The ADC features a prescaler, which enables conversion at lower clock rates
 * than the input Generic Clock to the ADC module. This feature can be used to
 * lower the synchronization time of the digital interface to the ADC module
 * via a high speed Generic Clock frequency, while still allowing the ADC
 * sampling rate to be reduced.
 *
 * \subsection asfdoc_sam0_adc_module_overview_resolution ADC Resolution
 * The ADC supports full 8-, 10-, or 12-bit resolution. Hardware
 * oversampling and decimation can be used to increase the
 * effective resolution at the expense of throughput. Using oversampling and
 * decimation mode the ADC resolution is increased from 12-bit to an effective
 * 13-, 14-, 15-, or 16-bit. In these modes the conversion rate is reduced, as
 * a greater number of samples is used to achieve the increased resolution. The
 * available resolutions and effective conversion rate is listed in
 * \ref asfdoc_sam0_adc_module_conversion_rate "the table below".
 *
 * \anchor asfdoc_sam0_adc_module_conversion_rate
 * <table>
 *	<caption>Effective ADC Conversion Speed Using Oversampling</caption>
 *	<tr>
 *		<th>Resolution</th>
 *		<th>Effective conversion rate</th>
 *	</tr>
 *	<tr>
 *		<td>13-bit</td>
 *		<td>Conversion rate divided by 4</td>
 *	</tr>
 *	<tr>
 *		<td>14-bit</td>
 *		<td>Conversion rate divided by 16</td>
 *	</tr>
 *	<tr>
 *		<td>15-bit</td>
 *		<td>Conversion rate divided by 64</td>
 *	</tr>
 *	<tr>
 *		<td>16-bit</td>
 *		<td>Conversion rate divided by 256</td>
 *	</tr>
 * </table>
 *
 * \subsection asfdoc_sam0_adc_module_overview_conversion Conversion Modes
 * ADC conversions can be software triggered on demand by the user application,
 * if continuous sampling is not required. It is also possible to configure the
 * ADC in free running mode, where new conversions are started as soon as the
 * previous conversion is completed, or configure the ADC to scan across a
 * number of input pins (see \ref asfdoc_sam0_adc_module_overview_pin_scan).
 *
 * \subsection asfdoc_sam0_adc_module_overview_diff_mode Differential and Single-ended Conversion
 * The ADC has two conversion modes; differential and single-ended. When
 * measuring signals where the positive input pin is always at a higher voltage
 * than the negative input pin, the single-ended conversion mode should be used
 * in order to achieve a full 12-bit output resolution.
 *
 * If however the positive input pin voltage may drop below the negative input
 * pin the signed differential mode should be used.
 *
 * \subsection asfdoc_sam0_adc_module_overview_sample_time Sample Time
 * The sample time for each ADC conversion is configurable as a number of half
 * prescaled ADC clock cycles (depending on the prescaler value), allowing the
 * user application to achieve faster or slower sampling depending on the
 * source impedance of the ADC input channels. For applications with high
 * impedance inputs the sample time can be increased to give the ADC an adequate
 * time to sample and convert the input channel.
 *
 * The resulting sampling time is given by the following equation:
 * \f[
 * t_{SAMPLE} = (sample\_length+1) \times \frac{ADC_{CLK}} {2}
 * \f]
 *
 * \subsection asfdoc_sam0_adc_module_overview_averaging Averaging
 * The ADC can be configured to trade conversion speed for accuracy by averaging
 * multiple samples in hardware. This feature is suitable when operating in
 * noisy conditions.
 *
 * You can specify any number of samples to accumulate (up to 1024) and the
 * divide ratio to use (up to divide by 128). To modify these settings the
 * ADC_RESOLUTION_CUSTOM needs to be set as the resolution. When this is set
 * the number of samples to accumulate and the division ratio can be set by
 * the configuration struct members \ref adc_config.accumulate_samples and
 * \ref adc_config.divide_result. When using this mode the ADC result register
 * will be set to be 16-bit wide to accommodate the larger result sizes
 * produced by the accumulator.
 *
 * The effective ADC conversion rate will be reduced by a factor of the number
 * of accumulated samples;
 * however, the effective resolution will be increased according to
 * \ref asfdoc_sam0_adc_module_hw_av_resolution "the table below".
 *
 * \anchor asfdoc_sam0_adc_module_hw_av_resolution
 * <table>
 *   <caption>Effective ADC Resolution From Various Hardware Averaging Modes</caption>
 *   <tr>
 *     <th>Number of samples</tr>
 *     <th>Final result</tr>
 *   </tr>
 *   <tr>
 *     <td>1</td>
 *     <td>12-bit</td>
 *   </tr>
 *   <tr>
 *      <td>2</td>
 *      <td>13-bit</td>
 *   </tr>
 *   <tr>
 *      <td>4</td>
 *      <td>14-bit</td>
 *   </tr>
 *   <tr>
 *      <td>8</td>
 *      <td>15-bit</td>
 *   </tr>
 *   <tr>
 *      <td>16</td>
 *      <td>16-bit</td>
 *   </tr>
 *   <tr>
 *      <td>32</td>
 *      <td>16-bit</td>
 *   </tr>
 *   <tr>
 *      <td>64</td>
 *      <td>16-bit</td>
 *   </tr>
 *   <tr>
 *      <td>128</td>
 *      <td>16-bit</td>
 *   </tr>
 *   <tr>
 *      <td>256</td>
 *      <td>16-bit</td>
 *   </tr>
 *   <tr>
 *      <td>512</td>
 *      <td>16-bit</td>
 *   </tr>
 *   <tr>
 *      <td>1024</td>
 *      <td>16-bit</td>
 *   </tr>
 * </table>
 *
 *
 * \subsection asfdoc_sam0_adc_module_overview_offset_corr Offset and Gain Correction
 * Inherent gain and offset errors affect the absolute accuracy of the ADC.
 *
 * The offset error is defined as the deviation of the ADC's actual transfer
 * function from ideal straight line at zero input voltage.
 *
 * The gain error is defined as the deviation of the last output step's
 * midpoint from the ideal straight line, after compensating for offset error.
 *
 * The offset correction value is subtracted from the converted data before the
 * result is ready. The gain correction value is multiplied with the offset
 * corrected value.
 *
 * The equation for both offset and gain error compensation is shown below:
 * \f[
 * ADC_{RESULT} = (VALUE_{CONV} + CORR_{OFFSET}) \times CORR_{GAIN}
 * \f]
 *
 * When enabled, a given set of offset and gain correction values can be applied
 * to the sampled data in hardware, giving a corrected stream of sample data to
 * the user application at the cost of an increased sample latency.
 *
 * In single conversion, a latency of 13 ADC Generic Clock cycles is added for
 * the final sample result availability. As the correction time is always less
 * than the propagation delay, in free running mode this latency appears only
 * during the first conversion. After the first conversion is complete, future
 * conversion results are available at the defined sampling rate.
 *
 * \subsection asfdoc_sam0_adc_module_overview_pin_scan Pin Scan
 * In pin scan mode, the first ADC conversion will begin from the configured
 * positive channel, plus the requested starting offset. When the first
 * conversion is completed, the next conversion will start at the next positive
 * input channel and so on, until all requested pins to scan have been sampled
 * and converted.
 * SAM L21/L22 has automatic sequences feature instead of pin scan mode. In automatic
 * sequence mode, all of 32 positives inputs can be included in a sequence. The
 * sequence starts from the lowest input, and go to the next enabled input
 * automatically.
 *
 * Pin scanning gives a simple mechanism to sample a large number of physical
 * input channel samples, using a single physical ADC channel.
 *
 * \subsection asfdoc_sam0_adc_module_overview_window_monitor Window Monitor
 * The ADC module window monitor function can be used to automatically compare
 * the conversion result against a preconfigured pair of upper and lower
 * threshold values.
 *
 * The threshold values are evaluated differently, depending on whether
 * differential or single-ended mode is selected. In differential mode, the
 * upper and lower thresholds are evaluated as signed values for the comparison,
 * while in single-ended mode the comparisons are made as a set of unsigned
 * values.
 *
 * The significant bits of the lower window monitor threshold and upper window
 * monitor threshold values are user-configurable, and follow the overall ADC
 * sampling bit precision set when the ADC is configured by the user application.
 * For example, only the eight lower bits of the window threshold values will be
 * compared to the sampled data whilst the ADC is configured in 8-bit mode.
 * In addition, if using differential mode, the 8<SUP>th</SUP> bit will be considered as
 * the sign bit even if bit 9 is zero.
 *
 * \subsection asfdoc_sam0_adc_module_overview_events Events
 * Event generation and event actions are configurable in the ADC.
 *
 * The ADC has two actions that can be triggered upon event reception:
 * \li Start conversion
 * \li Flush pipeline and start conversion
 *
 * The ADC can generate two events:
 * \li Window monitor
 * \li Result ready
 *
 * If the event actions are enabled in the configuration, any incoming event
 * will trigger the action.
 *
 * If the window monitor event is enabled, an event will be generated
 * when the configured window condition is detected.
 *
 * If the result ready event is enabled, an event will be generated when a
 * conversion is completed.
 *
 * \note The connection of events between modules requires the use of the
 *       \ref asfdoc_sam0_events_group "SAM Event System Driver (EVENTS)"
 *       to route output event of one module to the input event of another.
 *       For more information on event routing, refer to the event driver
 *       documentation.
 *
 *
 * \section asfdoc_sam0_adc_special_considerations Special Considerations
 *
 * An integrated analog temperature sensor is available for use with the ADC.
 * The bandgap voltage, as well as the scaled I/O and core voltages can also be
 * measured by the ADC. For internal ADC inputs, the internal source(s) may need
 * to be manually enabled by the user application before they can be measured.
 *
 *
 * \section asfdoc_sam0_adc_extra_info Extra Information
 *
 * For extra information, see \ref asfdoc_sam0_adc_extra. This includes:
 *  - \ref asfdoc_sam0_adc_extra_acronyms
 *  - \ref asfdoc_sam0_adc_extra_dependencies
 *  - \ref asfdoc_sam0_adc_extra_errata
 *  - \ref asfdoc_sam0_adc_extra_history
 *
 *
 * \section asfdoc_sam0_adc_examples Examples
 *
 * For a list of examples related to this driver, see
 * \ref asfdoc_sam0_adc_exqsg.
 *
 *
 * \section asfdoc_sam0_adc_api_overview API Overview
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <compiler.h>
#include <system.h>
#include <adc_feature.h>

/**
 * \name Module Status Flags
 *
 * ADC status flags, returned by \ref adc_get_status() and cleared by
 * \ref adc_clear_status().
 *
 * @{
 */

/** ADC result ready. */
#define ADC_STATUS_RESULT_READY  (1UL << 0)
/** Window monitor match. */
#define ADC_STATUS_WINDOW        (1UL << 1)
/** ADC result overwritten before read. */
#define ADC_STATUS_OVERRUN       (1UL << 2)

/** @} */

#if ADC_CALLBACK_MODE == true
#   if (ADC_INST_NUM > 1)
#       define _ADC_INTERRUPT_VECT_NUM(n, unused) \
			SYSTEM_INTERRUPT_MODULE_ADC##n,
/**
 * \internal Get the interrupt vector for the given device instance
 *
 * \param[in] The ADC module instance number
 *
 * \return Interrupt vector for of the given ADC module instance.
 */
static enum system_interrupt_vector _adc_interrupt_get_interrupt_vector(
		uint32_t inst_num)
{
	static uint8_t adc_interrupt_vectors[ADC_INST_NUM] = {
		MREPEAT(ADC_INST_NUM, _ADC_INTERRUPT_VECT_NUM, 0)
	};

	return (enum system_interrupt_vector)adc_interrupt_vectors[inst_num];
}
#   endif
#endif

#if !defined(__DOXYGEN__)
uint8_t _adc_get_inst_index(
		Adc *const hw);
#endif

/**
 * \name Driver Initialization and Configuration
 * @{
 */
enum status_code adc_init(
		struct adc_module *const module_inst,
		Adc *hw,
		struct adc_config *config);

void adc_get_config_defaults(
		struct adc_config *const config);

#if (SAMD) || (SAMR21)
void adc_regular_ain_channel(
		uint32_t *pin_array, uint8_t size);
#endif

/** @} */

/**
 * \name Status Management
 * @{
 */

/**
 * \brief Retrieves the current module status.
 *
 * Retrieves the status of the module, giving overall state information.
 *
 * \param[in] module_inst  Pointer to the ADC software instance struct
 *
 * \return Bitmask of \c ADC_STATUS_* flags.
 *
 * \retval ADC_STATUS_RESULT_READY  ADC result is ready to be read
 * \retval ADC_STATUS_WINDOW        ADC has detected a value inside the set
 *                                  window range
 * \retval ADC_STATUS_OVERRUN       ADC result has overrun
 */
static inline uint32_t adc_get_status(
		struct adc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

	uint32_t int_flags = adc_module->INTFLAG.reg;

	uint32_t status_flags = 0;

	/* Check for ADC Result Ready */
	if (int_flags & ADC_INTFLAG_RESRDY) {
		status_flags |= ADC_STATUS_RESULT_READY;
	}

	/* Check for ADC Window Match */
	if (int_flags & ADC_INTFLAG_WINMON) {
		status_flags |= ADC_STATUS_WINDOW;
	}

	/* Check for ADC Overrun */
	if (int_flags & ADC_INTFLAG_OVERRUN) {
		status_flags |= ADC_STATUS_OVERRUN;
	}

	return status_flags;
}

/**
 * \brief Clears a module status flag.
 *
 * Clears the given status flag of the module.
 *
 * \param[in] module_inst   Pointer to the ADC software instance struct
 * \param[in] status_flags  Bitmask of \c ADC_STATUS_* flags to clear
 */
static inline void adc_clear_status(
		struct adc_module *const module_inst,
		const uint32_t status_flags)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

	uint32_t int_flags = 0;

	/* Check for ADC Result Ready */
	if (status_flags & ADC_STATUS_RESULT_READY) {
		int_flags |= ADC_INTFLAG_RESRDY;
	}

	/* Check for ADC Window Match */
	if (status_flags & ADC_STATUS_WINDOW) {
		int_flags |= ADC_INTFLAG_WINMON;
	}

	/* Check for ADC Overrun */
	if (status_flags & ADC_STATUS_OVERRUN) {
		int_flags |= ADC_INTFLAG_OVERRUN;
	}

	/* Clear interrupt flag */
	adc_module->INTFLAG.reg = int_flags;
}
/** @} */

/**
 * \name Enable, Disable, and Reset ADC Module, Start Conversion and Read Result
 * @{
 */

/**
 * \brief Enables the ADC module.
 *
 * Enables an ADC module that has previously been configured. If any internal reference
 * is selected it will be enabled.
 *
 * \param[in] module_inst  Pointer to the ADC software instance struct
 */
static inline enum status_code adc_enable(
		struct adc_module *const module_inst)
{
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}

#if ADC_CALLBACK_MODE == true
#   if (ADC_INST_NUM > 1)
	system_interrupt_enable(_adc_interrupt_get_interrupt_vector(
			_adc_get_inst_index(adc_module)));
#   elif (SAMC20)
		system_interrupt_enable(SYSTEM_INTERRUPT_MODULE_ADC0);
#	else
		system_interrupt_enable(SYSTEM_INTERRUPT_MODULE_ADC);
#   endif
#endif

	/* Disbale interrupt */
	adc_module->INTENCLR.reg = ADC_INTENCLR_MASK;
	/* Clear interrupt flag */
	adc_module->INTFLAG.reg = ADC_INTFLAG_MASK;

	adc_module->CTRLA.reg |= ADC_CTRLA_ENABLE;

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}
	return STATUS_OK;
}

/**
 * \brief Disables the ADC module.
 *
 * Disables an ADC module that was previously enabled.
 *
 * \param[in] module_inst Pointer to the ADC software instance struct
 */
static inline enum status_code adc_disable(
		struct adc_module *const module_inst)
{
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

#if ADC_CALLBACK_MODE == true
#   if (ADC_INST_NUM > 1)
	system_interrupt_disable(_adc_interrupt_get_interrupt_vector(
			_adc_get_inst_index(adc_module)));
#   elif (SAMC20)
		system_interrupt_disable(SYSTEM_INTERRUPT_MODULE_ADC0);
#	else
		system_interrupt_disable(SYSTEM_INTERRUPT_MODULE_ADC);
#   endif
#endif

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}

	/* Disbale interrupt */
	adc_module->INTENCLR.reg = ADC_INTENCLR_MASK;
	/* Clear interrupt flag */
	adc_module->INTFLAG.reg = ADC_INTFLAG_MASK;

	adc_module->CTRLA.reg &= ~ADC_CTRLA_ENABLE;

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}
	return STATUS_OK;
}

/**
 * \brief Resets the ADC module.
 *
 * Resets an ADC module, clearing all module state, and registers to their
 * default values.
 *
 * \param[in] module_inst  Pointer to the ADC software instance struct
 */
static inline enum status_code adc_reset(
		struct adc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

	/* Disable to make sure the pipeline is flushed before reset */
	adc_disable(module_inst);

	/* Software reset the module */
	adc_module->CTRLA.reg |= ADC_CTRLA_SWRST;

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}
	return STATUS_OK;
}


/**
 * \brief Enables an ADC event input or output.
 *
 *  Enables one or more input or output events to or from the ADC module. See
 *  \ref adc_events "Struct adc_events" for a list of events this module supports.
 *
 *  \note Events cannot be altered while the module is enabled.
 *
 *  \param[in] module_inst  Software instance for the ADC peripheral
 *  \param[in] events       Struct containing flags of events to enable
 */
static inline void adc_enable_events(
		struct adc_module *const module_inst,
		struct adc_events *const events)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);
	Assert(events);

	Adc *const adc_module = module_inst->hw;

	uint32_t event_mask = 0;

	/* Configure Window Monitor event */
	if (events->generate_event_on_window_monitor) {
		event_mask |= ADC_EVCTRL_WINMONEO;
	}

	/* Configure Result Ready event */
	if (events->generate_event_on_conversion_done) {
		event_mask |= ADC_EVCTRL_RESRDYEO;
	}

	adc_module->EVCTRL.reg |= event_mask;
}

/**
 * \brief Disables an ADC event input or output.
 *
 *  Disables one or more input or output events to or from the ADC module. See
 *  \ref adc_events "Struct adc_events" for a list of events this module supports.
 *
 *  \note Events cannot be altered while the module is enabled.
 *
 *  \param[in] module_inst  Software instance for the ADC peripheral
 *  \param[in] events       Struct containing flags of events to disable
 */
static inline void adc_disable_events(
		struct adc_module *const module_inst,
		struct adc_events *const events)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);
	Assert(events);

	Adc *const adc_module = module_inst->hw;

	uint32_t event_mask = 0;

	/* Configure Window Monitor event */
	if (events->generate_event_on_window_monitor) {
		event_mask |= ADC_EVCTRL_WINMONEO;
	}

	/* Configure Result Ready event */
	if (events->generate_event_on_conversion_done) {
		event_mask |= ADC_EVCTRL_RESRDYEO;
	}

	adc_module->EVCTRL.reg &= ~event_mask;
}

/**
 * \brief Starts an ADC conversion.
 *
 * Starts a new ADC conversion.
 *
 * \param[in] module_inst  Pointer to the ADC software instance struct
 */
static inline void adc_start_conversion(
		struct adc_module *const module_inst)
{
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}

	adc_module->SWTRIG.reg |= ADC_SWTRIG_START;

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}
}

/**
 * \brief Reads the ADC result.
 *
 * Reads the result from an ADC conversion that was previously started.
 *
 * \param[in]  module_inst  Pointer to the ADC software instance struct
 * \param[out] result       Pointer to store the result value in
 *
 * \return Status of the ADC read request.
 * \retval STATUS_OK           The result was retrieved successfully
 * \retval STATUS_BUSY         A conversion result was not ready
 * \retval STATUS_ERR_OVERFLOW The result register has been overwritten by the
 *                             ADC module before the result was read by the software
 */
static inline enum status_code adc_read(
		struct adc_module *const module_inst,
		uint16_t *result)
{
	Assert(module_inst);
	Assert(module_inst->hw);
	Assert(result);

	if (!(adc_get_status(module_inst) & ADC_STATUS_RESULT_READY)) {
		/* Result not ready */
		return STATUS_BUSY;
	}

	Adc *const adc_module = module_inst->hw;

#if (SAMD) || (SAMR21)
	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}
#endif

	/* Get ADC result */
	*result = adc_module->RESULT.reg;

	/* Reset ready flag */
	adc_clear_status(module_inst, ADC_STATUS_RESULT_READY);

	if (adc_get_status(module_inst) & ADC_STATUS_OVERRUN) {
		adc_clear_status(module_inst, ADC_STATUS_OVERRUN);
		return STATUS_ERR_OVERFLOW;
	}

	return STATUS_OK;
}

/** @} */

/**
 * \name Runtime Changes of ADC Module
 * @{
 */

/**
 * \brief Flushes the ADC pipeline.
 *
 * Flushes the pipeline and restarts the ADC clock on the next peripheral clock
 * edge. All conversions in progress will be lost. When flush is complete, the
 * module will resume where it left off.
 *
 * \param[in] module_inst  Pointer to the ADC software instance struct
 */
static inline void adc_flush(
		struct adc_module *const module_inst)
{
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}

	adc_module->SWTRIG.reg |= ADC_SWTRIG_FLUSH;

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}
}
void adc_set_window_mode(
		struct adc_module *const module_inst,
		const enum adc_window_mode window_mode,
		const int16_t window_lower_value,
		const int16_t window_upper_value);

/**
 * \brief Sets positive ADC input pin.
 *
 * Sets the positive ADC input pin selection.
 *
 * \param[in] module_inst     Pointer to the ADC software instance struct
 * \param[in] positive_input  Positive input pin
 */
static inline void adc_set_positive_input(
		struct adc_module *const module_inst,
		const enum adc_positive_input positive_input)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}

	/* Set positive input pin */
	adc_module->INPUTCTRL.reg =
			(adc_module->INPUTCTRL.reg & ~ADC_INPUTCTRL_MUXPOS_Msk) |
			(positive_input);

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}
}


/**
 * \brief Sets negative ADC input pin for differential mode.
 *
 * Sets the negative ADC input pin, when the ADC is configured in differential
 * mode.
 *
 * \param[in] module_inst     Pointer to the ADC software instance struct
 * \param[in] negative_input  Negative input pin
 */
static inline void adc_set_negative_input(
		struct adc_module *const module_inst,
		const enum adc_negative_input negative_input)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}

	/* Set negative input pin */
	adc_module->INPUTCTRL.reg =
			(adc_module->INPUTCTRL.reg & ~ADC_INPUTCTRL_MUXNEG_Msk) |
			(negative_input);

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}
}

/** @} */

#if ADC_CALLBACK_MODE == true
/**
 * \name Enable and Disable Interrupts
 * @{
 */

/**
 * \brief Enable interrupt.
 *
 * Enable the given interrupt request from the ADC module.
 *
 * \param[in] module_inst Pointer to the ADC software instance struct
 * \param[in] interrupt Interrupt to enable
 */
static inline void adc_enable_interrupt(struct adc_module *const module_inst,
		enum adc_interrupt_flag interrupt)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;
	/* Enable interrupt */
	adc_module->INTENSET.reg = interrupt;
}

/**
 * \brief Disable interrupt.
 *
 * Disable the given interrupt request from the ADC module.
 *
 * \param[in] module_inst Pointer to the ADC software instance struct
 * \param[in] interrupt Interrupt to disable
 */
static inline void adc_disable_interrupt(struct adc_module *const module_inst,
		enum adc_interrupt_flag interrupt)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;
	/* Enable interrupt */
	adc_module->INTENCLR.reg = interrupt;
}

/** @} */
#endif /* ADC_CALLBACK_MODE == true */

#ifdef __cplusplus
}
#endif

/** @} */


/**
 * \page asfdoc_sam0_adc_extra Extra Information for ADC Driver
 *
 * \section asfdoc_sam0_adc_extra_acronyms Acronyms
 * Below is a table listing the acronyms used in this module, along with their
 * intended meanings.
 *
 * <table>
 *	<tr>
 *		<th>Acronym</th>
 *		<th>Description</th>
 *	</tr>
 *  <tr>
 *		<td>ADC</td>
 *		<td>Analog-to-Digital Converter</td>
 *	</tr>
 *  <tr>
 *		<td>DAC</td>
 *		<td>Digital-to-Analog Converter</td>
 *	</tr>
 *	<tr>
 *		<td>LSB</td>
 *		<td>Least Significant Bit</td>
 *	</tr>
 *	<tr>
 *		<td>MSB</td>
 *		<td>Most Significant Bit</td>
 *	</tr>
 *	<tr>
 *		<td>DMA</td>
 *		<td>Direct Memory Access</td>
 *	</tr>
 * </table>
 *
 *
 * \section asfdoc_sam0_adc_extra_dependencies Dependencies
 * This driver has the following dependencies:
 *
 *  - \ref asfdoc_sam0_system_pinmux_group "System Pin Multiplexer Driver"
 *
 *
 * \section asfdoc_sam0_adc_extra_errata Errata
 * There are no errata related to this driver.
 *
 *
 * \section asfdoc_sam0_adc_extra_history Module History
 * An overview of the module history is presented in the table below, with
 * details on the enhancements and fixes made to the module since its first
 * release. The current version of this corresponds to the newest version in
 * the table.
 *
 * <table>
 *	<tr>
 *		<th>Changelog</th>
 *	</tr>
 * \if DEVICE_SAML21_SUPPORT
 *  <tr>
 *		<td>Initial Release</td>
 * </tr>
 * \else
 *	<tr>
 *		<td>Added support for SAM R21</td>
 *	</tr>
 *	<tr>
 *		<td>Added support for SAM D21 and new DMA quick start guide</td>
 *	</tr>
 *	<tr>
 *		<td>Added ADC calibration constant loading from the device signature
 *          row when the module is initialized</td>
 *	</tr>
 *	<tr>
 *		<td>Initial Release</td>
 *	</tr>
 * \endif
 * </table>
 */

/**
 * \page asfdoc_sam0_adc_exqsg Examples for ADC Driver
 *
 * This is a list of the available Quick Start guides (QSGs) and example
 * applications for \ref asfdoc_sam0_adc_group. QSGs are simple examples with
 * step-by-step instructions to configure and use this driver in a selection of
 * use cases. Note that a QSG can be compiled as a standalone application or be
 * added to the user application.
 *
 *  - \subpage asfdoc_sam0_adc_basic_use_case
 * \if ADC_CALLBACK_MODE
 *  - \subpage asfdoc_sam0_adc_basic_use_case_callback
 * \endif
 *  - \subpage asfdoc_sam0_adc_dma_use_case
 *
 * \page asfdoc_sam0_adc_document_revision_history Document Revision History
 *
 * <table>
 *	<tr>
 *		<th>Doc. Rev.</th>
 *		<th>Date</th>
 *		<th>Comments</th>
 *	</tr>
 * \if DEVICE_SAML21_SUPPORT
  *	<tr>
 *		<td>42451B</td>
 *		<td>12/2015</td>
 *		<td>Added support for SAM L22</td>
 *	</tr>
 *  <tr>
 *      <td>42451A</td>
 *      <td>07/2015</td>
 *      <td>Initial document release</td>
 * </tr>
 * \else
 *	<tr>
 *		<td>42109E</td>
 *		<td>12/2015</td>
 *		<td>Added support for SAM DA1 and SAM D09</td>
 *	</tr>
 *	<tr>
 *		<td>42109D</td>
 *		<td>12/2014</td>
 *		<td>Added support for SAM R21 and SAM D10/D11</td>
 *	</tr>
 *	<tr>
 *		<td>42109C</td>
 *		<td>01/2014</td>
 *		<td>Added support for SAM D21</td>
 *	</tr>
 *	<tr>
 *		<td>42109B</td>
 *		<td>06/2013</td>
 *		<td>Added additional documentation on the event system. Corrected
 *          documentation typos.</td>
 *	</tr>
 *	<tr>
 *		<td>42109A</td>
 *		<td>06/2013</td>
 *		<td>Initial release</td>
 *	</tr>
 * \endif
 * </table>
 */

#endif /* ADC_H_INCLUDED */
