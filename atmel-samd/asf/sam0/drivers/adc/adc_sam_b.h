/**
 * \file
 *
 * \brief SAMB Peripheral Analog-to-Digital Converter Driver
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef ADC_SAM_B_H_INCLUDED
#define ADC_SAM_B_H_INCLUDED

/**
 * \defgroup asfdoc_samb_adc_group SAM Analog-to-Digital Converter (ADC) Driver
 *
 * This driver for Atmel&reg; | SMART SAM devices provides an interface for the
 * configuration and management of the device's Analog-to-Digital Converter
 * functionality, for the conversion of analog voltages into a corresponding
 * digital form.
 *
 * The following peripheral is used by this module:
 *  - ADC (Analog-to-Digital Converter)
 *
 * The following devices can use this module:
 *  - Atmel | SMART SAM B11
 *
 * The outline of this documentation is as follows:
 *  - \ref asfdoc_samb_adc_prerequisites
 *  - \ref asfdoc_samb_adc_module_overview
 *  - \ref asfdoc_samb_adc_special_considerations
 *  - \ref asfdoc_samb_adc_extra_info
 *  - \ref asfdoc_samb_adc_examples
 *  - \ref asfdoc_samb_adc_api_overview
 *
 *
 * \section asfdoc_samb_adc_prerequisites Prerequisites
 *
 * There are no prerequisites for this module.
 *
 *
 * \section asfdoc_samb_adc_module_overview Module Overview
 *
 * This driver provides an interface for the Analog-to-Digital conversion
 * functions on the device, to convert analog voltages to a corresponding
 * digital value. The ADC has up to 12-bit resolution.
 *
 * \subsection asfdoc_samb_adc_module_overview_prescaler Sample Clock Prescaler
 * \f[
 *      F_{clk} = N + 2 \times {(throughput\_frequency)}
 * \f]
 * \f[
 *      (N = ADC resolution)
 * \f]
 *
 * Example: For throughput of 100KS/s use sampling clock (Fclk=1.3MHz)
 *
 * \subsection asfdoc_samb_adc_module_overview_resolution ADC Resolution
 * There are two ADC resolution of operation:
 * - \b High Resolution (11-bit)
 *
 *   Set the reference voltage to half the supply voltage or below. In this
 *   mode the input signal dynamic range equals twice the reference voltage.
 *
 *   Example: If supply voltage =3V and reference voltage= 1.4V, input signal
 *   dynamic range can be from ground to 2*reference voltage (0~2.8V).
 * - \b Medium Resolution (10-bit)
 *
 *   Set the reference voltage to any value below supply voltage (up to
 *   supply voltage - 300mV) and in this condition the input dynamic range is
 *   from zero to reference voltage.
 *
 *   Example: If supply voltage =3V and reference voltage= 2.7V, input signal
 *   dynamic range can be from ground to reference voltage (0~2.7V).
 *
 * \subsection asfdoc_samb_adc_module_overview_conversion Channel Modes
 * There are two modes for input channel selection:
 * - Either to assign a specific input channel
 * - Time domain multiplexing between 4 input channels
 *   - Channel1 to channel4
 *   - channel5 to channel8
 *
 * \section asfdoc_samb_adc_special_considerations Special Considerations
 * An integrated analog temperature sensor is available for use with the ADC.
 * The internal specific voltage can also be measured by the ADC. For internal
 * ADC inputs, the internal source(s) may need to be manually enabled by the
 * user application before they can be measured.
 *
 *
 * \section asfdoc_samb_adc_extra_info Extra Information
 *
 * For extra information, see \ref asfdoc_samb_adc_extra. This includes:
 *  - \ref asfdoc_samb_adc_extra_acronyms
 *  - \ref asfdoc_samb_adc_extra_dependencies
 *  - \ref asfdoc_samb_adc_extra_errata
 *  - \ref asfdoc_samb_adc_extra_history
 *
 *
 * \section asfdoc_samb_adc_examples Examples
 *
 * For a list of examples related to this driver, see
 * \ref asfdoc_samb_adc_exqsg.
 *
 *
 * \section asfdoc_samb_adc_api_overview API Overview
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <compiler.h>
#include <system_sam_b.h>

/**
 * \brief ADC internal reference voltage level enum
 *
 * Enum for the possible internal reference voltage level for the ADC.
 *
 */
enum adc_internal_buf {
	/** Internal buffer voltage level: 0.5V */
	ADC_INTERNAL_BUF_0_5 = 0,
	/** Internal buffer voltage level: 0.6V */
	ADC_INTERNAL_BUF_0_6,
	/** Internal buffer voltage level: 0.7V */
	ADC_INTERNAL_BUF_0_7,
	/** Internal buffer voltage level: 0.8V */
	ADC_INTERNAL_BUF_0_8,
	/** Internal buffer voltage level: 0.9V */
	ADC_INTERNAL_BUF_0_9,
	/** Internal buffer voltage level: 1.0V */
	ADC_INTERNAL_BUF_1_0,
	/** Internal buffer voltage level: 1.1V */
	ADC_INTERNAL_BUF_1_1,
	/** Internal buffer voltage level: 1.2V */
	ADC_INTERNAL_BUF_1_2,
	/** Internal buffer voltage level: 1.3V */
	ADC_INTERNAL_BUF_1_3,
	/** Internal buffer voltage level: 1.4V */
	ADC_INTERNAL_BUF_1_4,
	/** Internal buffer voltage level: 1.5V */
	ADC_INTERNAL_BUF_1_5,
	/** Internal buffer voltage level: 1.6V */
	ADC_INTERNAL_BUF_1_6,
	/** Internal buffer voltage level: 1.7V */
	ADC_INTERNAL_BUF_1_7,
	/** Internal buffer voltage level: 1.8V */
	ADC_INTERNAL_BUF_1_8,
	/** Internal buffer voltage level: 1.9V */
	ADC_INTERNAL_BUF_1_9,
	/** Internal buffer voltage level: 2.0V */
	ADC_INTERNAL_BUF_2_0,
};

/**
 * \brief ADC reference voltage enum
 *
 * Enum for the possible reference voltages for the ADC.
 *
 */
enum adc_reference {
	/** Internal form buffer reference */
	ADC_REFERENCE_INTERNAL_BUF = 0,
	/** Internal from IxR reference */
	ADC_REFERENCE_INTERNAL_IR,
	/**  VBATT/2 reference */
	ADC_REFERENCE_VBATT_2,
	/** GPIO_MS1 reference */
	ADC_REFERENCE_GPIO_MS1,
	/** GPIO_MS2 reference */
	ADC_REFERENCE_GPIO_MS2,
	/** GPIO_MS3 reference */
	ADC_REFERENCE_GPIO_MS3,
	/** GPIO_MS4 reference */
	ADC_REFERENCE_GPIO_MS4,
	/** VBATTERY reference */
	ADC_REFERENCE_VBATT,
};

/**
 * \brief ADC input channel selection enum
 *
 * Enum for the possible input channel selections for the ADC.
 *
 */
enum adc_input_channel {
	/** GPIO_MS1 input */
	ADC_INPUT_CH_GPIO_MS1 = 0,
	/** GPIO_MS2 input */
	ADC_INPUT_CH_GPIO_MS2,
	/** GPIO_MS3 input */
	ADC_INPUT_CH_GPIO_MS3,
	/** GPIO_MS4 input */
	ADC_INPUT_CH_GPIO_MS4,
	/** On chip temperature measurement input */
	ADC_INPUT_CH_TEMPERATURE,
	/** VBATT divided by 4 voltage level */
	ADC_INPUT_CH_VBATT_4,
	/** LP_LDO_OUT_1P2 low power domain voltage */
	ADC_INPUT_CH_LPD0_LDO,
	/**  ADC reference voltage level */
	ADC_INPUT_CH_VREF,
};

/**
 * \brief ADC input channel time multiplexing selection mode enum
 *
 * Enum for the possible channel time multiplexing mode for the ADC.
 *
 */
enum adc_channel_mode {
	/**
	 * Input channels time multiplexing is between channel 0 to channel 3.
	 * In this mode, ADC input channel selection is invalid.
	 */
	ADC_CH_MODE_CH0_TO_CH3,
	/**
	 * Input channels time multiplexing is between channel 4 to channel 7.
	 * In this mode, ADC input channel selection is invalid.
	 */
	ADC_CH_MODE_CH4_TO_CH7,
	/** Assign a specific input channel using \ref adc_input_channel */
	ADC_CH_MODE_ASSIGN,
};

/**
 * \brief ADC input dynamic range selection enum
 *
 * Enum for the possible input dynamic range for the ADC.
 *
 */
enum adc_input_dynamic_range{
	/** Input dynamic range from 0V to VBATT voltage level */
	ADC_INPUT_DYNAMIC_RANGE_0 = 0,
	/** Input dynamic range from 0.08*VBATT to 0.92*VBATT voltage level */
	ADC_INPUT_DYNAMIC_RANGE_1,
	/** Input dynamic range from 0.17*VBATT to 0.83*VBATT voltage level */
	ADC_INPUT_DYNAMIC_RANGE_2,
	/** Input dynamic range from 0.25*VBATT to 0.75*VBATT voltage level */
	ADC_INPUT_DYNAMIC_RANGE_3,
};

/**
 * \brief ADC comparator biasing current enum
 *
 * Enum for the possible comparator biasing current for the ADC different
 * sampling rates.
 *
 */
enum adc_bias_current {
	/** Comparator bias current is 4uA for throughput 100KS/s */
	ADC_BIAS_CURRENT_0 = 0,
	/** Comparator bias current is 8uA */
	ADC_BIAS_CURRENT_1,
	/** Comparator bias current is 20uA */
	ADC_BIAS_CURRENT_2,
	/** Comparator bias current is 24uA for throughput 1MS/s */
	ADC_BIAS_CURRENT_3,
};

/**
 * \brief ADC configuration structure
 *
 * Configuration structure for an ADC instance. This structure should be
 * initialized by the \ref adc_get_config_defaults()
 * function before being modified by the user application.
 */
struct adc_config {
	/** Voltage reference */
	enum adc_reference reference;
	/** Internal reference voltage level */
	enum adc_internal_buf internal_vref;
	/** Input channel */
	enum adc_input_channel input_channel;
	/** Input channel selection */
	enum adc_channel_mode channel_mode;
	/** Input channel time multiplexing selection mode */
	enum adc_input_dynamic_range input_dynamic_range;
	/** Comparator biasing current selection */
	enum adc_bias_current bias_current;
	/** Invert ADC Clock */
	bool invert_clock;
	/** Fractional part for the clock divider */
	uint8_t frac_part;
	/** Integer part for the clock divider */
	uint16_t int_part;
};

/** \name Configuration, initialization and get status
 * @{
 */
void adc_get_config_defaults(struct adc_config *const config);
void adc_init(struct adc_config *config);
uint32_t adc_get_status(void);
/** @}*/

/** \name Enable/Disable and Reset
 * @{
 */
void adc_enable(void);
void adc_disable(void);
void adc_reset(void);
/** @}*/

/** \name Read Result
 * @{
 */
enum status_code adc_read(enum adc_input_channel input_channel, uint16_t *result);
/** @}*/

/** @}*/

#ifdef __cplusplus
}
#endif


/**
 * \page asfdoc_samb_adc_extra Extra Information for ADC Driver
 *
 * \section asfdoc_samb_adc_extra_acronyms Acronyms
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
 * </table>
 *
 *
 * \section asfdoc_samb_adc_extra_dependencies Dependencies
 * There are no dependencies related to this driver.
 *
 *
 * \section asfdoc_samb_adc_extra_errata Errata
 * There are no errata related to this driver.
 *
 *
 * \section asfdoc_samb_adc_extra_history Module History
 * An overview of the module history is presented in the table below, with
 * details on the enhancements and fixes made to the module since its first
 * release. The current version of this corresponds to the newest version in
 * the table.
 *
 * <table>
 *	<tr>
 *		<th>Changelog</th>
 *	</tr>
 *	<tr>
 *		<td>Initial Release</td>
 *	</tr>
 * </table>
 */

/**
 * \page asfdoc_samb_adc_exqsg Examples for ADC Driver
 *
 * This is a list of the available Quick Start guides (QSGs) and example
 * applications for \ref asfdoc_samb_adc_group. QSGs are simple examples with
 * step-by-step instructions to configure and use this driver in a selection of
 * use cases. Note that a QSG can be compiled as a standalone application or be
 * added to the user application.
 *
 *  - \subpage asfdoc_samb_adc_basic_use_case
 *
 * \page asfdoc_samb_adc_document_revision_history Document Revision History
 *
 * <table>
 *	<tr>
 *		<th>Doc. Rev.</td>
 *		<th>Date</td>
 *		<th>Comments</td>
 *	</tr>
 *  <tr>
 *      <td>A</td>
 *      <td>09/2015</td>
 *      <td>Initial document release</td>
 * </tr>
 * </table>
 */

#endif /* ADC_SAM_B_H_INCLUDED */
