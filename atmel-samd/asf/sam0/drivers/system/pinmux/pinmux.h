/**
 * \file
 *
 * \brief SAM Pin Multiplexer Driver
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
#ifndef PINMUX_H_INCLUDED
#define PINMUX_H_INCLUDED

/**
 * \defgroup asfdoc_sam0_system_pinmux_group SAM System Pin Multiplexer (SYSTEM PINMUX) Driver
 *
 * This driver for Atmel&reg; | SMART ARM&reg;-based microcontrollers provides
 * an interface for the configuration and management of the device's physical
 * I/O Pins, to alter the direction and input/drive characteristics as well as
 * to configure the pin peripheral multiplexer selection.
 *
 * The following peripheral is used by this module:
 *  - PORT (Port I/O Management)
 *
 * The following devices can use this module:
 *  - Atmel | SMART SAM D20/D21
 *  - Atmel | SMART SAM R21
 *  - Atmel | SMART SAM D09/D10/D11
 *  - Atmel | SMART SAM L21/L22
 *  - Atmel | SMART SAM DA1
 *  - Atmel | SMART SAM C20/C21
 *
 * The outline of this documentation is as follows:
 *  - \ref asfdoc_sam0_system_pinmux_prerequisites
 *  - \ref asfdoc_sam0_system_pinmux_module_overview
 *  - \ref asfdoc_sam0_system_pinmux_special_considerations
 *  - \ref asfdoc_sam0_system_pinmux_extra_info
 *  - \ref asfdoc_sam0_system_pinmux_examples
 *  - \ref asfdoc_sam0_system_pinmux_api_overview
 *
 *
 * \section asfdoc_sam0_system_pinmux_prerequisites Prerequisites
 *
 * There are no prerequisites for this module.
 *
 *
 * \section asfdoc_sam0_system_pinmux_module_overview Module Overview
 *
 * The SAM devices contain a number of General Purpose I/O pins, used to
 * interface the user application logic and internal hardware peripherals to
 * an external system. The Pin Multiplexer (PINMUX) driver provides a method
 * of configuring the individual pin peripheral multiplexers to select
 * alternate pin functions.
 *
 * \subsection asfdoc_sam0_system_pinmux_features Driver Feature Macro Definition
 * <table>
 *  <tr>
 *    <th>Driver Feature Macro</th>
 *    <th>Supported devices</th>
 *  </tr>
 *  <tr>
 *    <td>FEATURE_SYSTEM_PINMUX_DRIVE_STRENGTH</td>
 *    <td>SAM L21, SAM C20/C21</td>
 *  </tr>
 * </table>
 * \note The specific features are only available in the driver when the
 * selected device supports those features.
 *
 * \subsection asfdoc_sam0_system_pinmux_physical_logical_pins Physical and Logical GPIO Pins
 * SAM devices use two naming conventions for the I/O pins in the device; one
 * physical and one logical. Each physical pin on a device package is assigned
 * both a physical port and pin identifier (e.g. "PORTA.0") as well as a
 * monotonically incrementing logical GPIO number (e.g. "GPIO0"). While the
 * former is used to map physical pins to their physical internal device module
 * counterparts, for simplicity the design of this driver uses the logical GPIO
 * numbers instead.
 *
 * \subsection asfdoc_sam0_system_pinmux_peripheral_muxing Peripheral Multiplexing
 * SAM devices contain a peripheral MUX, which is individually controllable
 * for each I/O pin of the device. The peripheral MUX allows you to select the
 * function of a physical package pin - whether it will be controlled as a user
 * controllable GPIO pin, or whether it will be connected internally to one of
 * several peripheral modules (such as an I<SUP>2</SUP>C module). When a pin is
 * configured in GPIO mode, other peripherals connected to the same pin will be
 * disabled.
 *
 * \subsection asfdoc_sam0_system_pinmux_pad_characteristics Special Pad Characteristics
 * There are several special modes that can be selected on one or more I/O pins
 * of the device, which alter the input and output characteristics of the pad.
 *
 * \subsubsection asfdoc_sam0_system_pinmux_drive_strength Drive Strength
 * The Drive Strength configures the strength of the output driver on the
 * pad. Normally, there is a fixed current limit that each I/O pin can safely
 * drive, however some I/O pads offer a higher drive mode which increases this
 * limit for that I/O pin at the expense of an increased power consumption.
 *
 * \subsubsection asfdoc_sam0_system_pinmux_slew_rate Slew Rate
 * The Slew Rate configures the slew rate of the output driver, limiting the
 * rate at which the pad output voltage can change with time.
 *
 * \subsubsection asfdoc_sam0_system_pinmux_input_sample_mode Input Sample Mode
 * The Input Sample Mode configures the input sampler buffer of the pad. By
 * default, the input buffer is only sampled "on-demand", i.e. when the user
 * application attempts to read from the input buffer. This mode is the most
 * power efficient, but increases the latency of the input sample by two clock
 * cycles of the port clock. To reduce latency, the input sampler can instead
 * be configured to always sample the input buffer on each port clock cycle, at
 * the expense of an increased power consumption.
 *
 * \subsection asfdoc_sam0_system_pinmux_module_overview_physical Physical Connection
 *
 * \ref asfdoc_sam0_system_pinmux_intconnections "The diagram below" shows
 * how this module is interconnected within the device:
 *
 * \anchor asfdoc_sam0_system_pinmux_intconnections
 * \dot
 * digraph overview {
 *   node [label="Port Pad" shape=square] pad;
 *
 *   subgraph driver {
 *     node [label="Peripheral MUX" shape=trapezium] pinmux;
 *     node [label="GPIO Module" shape=ellipse shape=ellipse style=filled fillcolor=lightgray] gpio;
 *     node [label="Other Peripheral Modules" shape=ellipse style=filled fillcolor=lightgray] peripherals;
 *   }
 *
 *   pinmux -> gpio;
 *   pad    -> pinmux;
 *   pinmux -> peripherals;
 * }
 * \enddot
 *
 * \section asfdoc_sam0_system_pinmux_special_considerations Special Considerations
 *
 * The SAM port pin input sampling mode is set in groups of four physical
 * pins; setting the sampling mode of any pin in a sub-group of eight I/O pins
 * will configure the sampling mode of the entire sub-group.
 *
 * High Drive Strength output driver mode is not available on all device pins -
 * refer to your device specific datasheet.
 *
 *
 * \section asfdoc_sam0_system_pinmux_extra_info Extra Information
 *
 * For extra information, see \ref asfdoc_sam0_system_pinmux_extra. This includes:
 *  - \ref asfdoc_sam0_system_pinmux_extra_acronyms
 *  - \ref asfdoc_sam0_system_pinmux_extra_dependencies
 *  - \ref asfdoc_sam0_system_pinmux_extra_errata
 *  - \ref asfdoc_sam0_system_pinmux_extra_history
 *
 *
 * \section asfdoc_sam0_system_pinmux_examples Examples
 *
 * For a list of examples related to this driver, see
 * \ref asfdoc_sam0_system_pinmux_exqsg.
 *
 *
 * \section asfdoc_sam0_system_pinmux_api_overview API Overview
 * @{
 */

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/*@{*/
#if (SAML21) || (SAMC20) || (SAMC21) || (SAMD21) || (SAMD10) || (SAMD11) || (SAMR30) || defined(__DOXYGEN__)
/** Output Driver Strength Selection feature support */
#  define FEATURE_SYSTEM_PINMUX_DRIVE_STRENGTH
#endif
/*@}*/

/** Peripheral multiplexer index to select GPIO mode for a pin */
#define SYSTEM_PINMUX_GPIO    (1 << 7)

/**
 * \brief Port pin direction configuration enum.
 *
 * Enum for the possible pin direction settings of the port pin configuration
 * structure, to indicate the direction the pin should use.
 */
enum system_pinmux_pin_dir {
	/** The pin's input buffer should be enabled, so that the pin state can
	 *  be read */
	SYSTEM_PINMUX_PIN_DIR_INPUT,
	/** The pin's output buffer should be enabled, so that the pin state can
	 *  be set (but not read back) */
	SYSTEM_PINMUX_PIN_DIR_OUTPUT,
	/** The pin's output and input buffers should both be enabled, so that the
	 *  pin state can be set and read back */
	SYSTEM_PINMUX_PIN_DIR_OUTPUT_WITH_READBACK,
};

/**
 * \brief Port pin input pull configuration enum.
 *
 * Enum for the possible pin pull settings of the port pin configuration
 * structure, to indicate the type of logic level pull the pin should use.
 */
enum system_pinmux_pin_pull {
	/** No logical pull should be applied to the pin */
	SYSTEM_PINMUX_PIN_PULL_NONE,
	/** Pin should be pulled up when idle */
	SYSTEM_PINMUX_PIN_PULL_UP,
	/** Pin should be pulled down when idle */
	SYSTEM_PINMUX_PIN_PULL_DOWN,
};

/**
 * \brief Port pin digital input sampling mode enum.
 *
 * Enum for the possible input sampling modes for the port pin configuration
 * structure, to indicate the type of sampling a port pin should use.
 */
enum system_pinmux_pin_sample {
	/** Pin input buffer should continuously sample the pin state */
	SYSTEM_PINMUX_PIN_SAMPLE_CONTINUOUS,
	/** Pin input buffer should be enabled when the IN register is read */
	SYSTEM_PINMUX_PIN_SAMPLE_ONDEMAND,
};

/**
 * \brief Port pin configuration structure.
 *
 * Configuration structure for a port pin instance. This structure should
 * be initialized by the \ref system_pinmux_get_config_defaults() function
 * before being modified by the user application.
 */
struct system_pinmux_config {
	/** MUX index of the peripheral that should control the pin, if peripheral
	 *  control is desired. For GPIO use, this should be set to
	 *  \ref SYSTEM_PINMUX_GPIO. */
	uint8_t mux_position;

	/** Port buffer input/output direction */
	enum system_pinmux_pin_dir direction;

	/** Logic level pull of the input buffer */
	enum system_pinmux_pin_pull input_pull;

	/** Enable lowest possible powerstate on the pin
	 *
	 *  \note All other configurations will be ignored, the pin will be disabled.
	 */
	bool powersave;
};

/** \name Configuration and Initialization
 * @{
 */

/**
 * \brief Initializes a Port pin configuration structure to defaults.
 *
 * Initializes a given Port pin configuration structure to a set of
 * known default values. This function should be called on all new
 * instances of these configuration structures before being modified by the
 * user application.
 *
 * The default configuration is as follows:
 *  \li Non peripheral (i.e. GPIO) controlled
 *  \li Input mode with internal pull-up enabled
 *
 * \param[out] config  Configuration structure to initialize to default values
 */
static inline void system_pinmux_get_config_defaults(
		struct system_pinmux_config *const config)
{
	/* Sanity check arguments */
	Assert(config);

	/* Default configuration values */
	config->mux_position = SYSTEM_PINMUX_GPIO;
	config->direction    = SYSTEM_PINMUX_PIN_DIR_INPUT;
	config->input_pull   = SYSTEM_PINMUX_PIN_PULL_UP;
	config->powersave    = false;
}

void system_pinmux_pin_set_config(
		const uint8_t gpio_pin,
		const struct system_pinmux_config *const config);

void system_pinmux_group_set_config(
		PortGroup *const port,
		const uint32_t mask,
		const struct system_pinmux_config *const config);

/** @} */

/** \name Special Mode Configuration (Physical Group Orientated)
 *  @{
 */

/**
 * \brief Retrieves the PORT module group instance from a given GPIO pin number.
 *
 * Retrieves the PORT module group instance associated with a given logical
 * GPIO pin number.
 *
 * \param[in] gpio_pin  Index of the GPIO pin to convert
 *
 * \return Base address of the associated PORT module.
 */
static inline PortGroup* system_pinmux_get_group_from_gpio_pin(
		const uint8_t gpio_pin)
{
	uint8_t port_index  = (gpio_pin / 128);
	uint8_t group_index = (gpio_pin / 32);

	/* Array of available ports */
	Port *const ports[PORT_INST_NUM] = PORT_INSTS;

	if (port_index < PORT_INST_NUM) {
		return &(ports[port_index]->Group[group_index]);
	} else {
		Assert(false);
		return NULL;
	}
}

void system_pinmux_group_set_input_sample_mode(
		PortGroup *const port,
		const uint32_t mask,
		const enum system_pinmux_pin_sample mode);

/** @} */

/** \name Special Mode Configuration (Logical Pin Orientated)
 *  @{
 */

/**
 * \brief Retrieves the currently selected MUX position of a logical pin.
 *
 * Retrieves the selected MUX peripheral on a given logical GPIO pin.
 *
 * \param[in]  gpio_pin  Index of the GPIO pin to configure
 *
 * \return Currently selected peripheral index on the specified pin.
 */
static inline uint8_t system_pinmux_pin_get_mux_position(
		const uint8_t gpio_pin)
{
	PortGroup *const port = system_pinmux_get_group_from_gpio_pin(gpio_pin);
	uint32_t pin_index = (gpio_pin % 32);

	if (!(port->PINCFG[pin_index].reg & PORT_PINCFG_PMUXEN)) {
		return SYSTEM_PINMUX_GPIO;
	}

	uint32_t pmux_reg = port->PMUX[pin_index / 2].reg;

	if (pin_index & 1) {
		return (pmux_reg & PORT_PMUX_PMUXO_Msk) >> PORT_PMUX_PMUXO_Pos;
	}
	else {
		return (pmux_reg & PORT_PMUX_PMUXE_Msk) >> PORT_PMUX_PMUXE_Pos;
	}
}

/**
 * \brief Configures the input sampling mode for a GPIO pin.
 *
 * Configures the input sampling mode for a GPIO input, to
 * control when the physical I/O pin value is sampled and
 * stored inside the microcontroller.
 *
 * \param[in] gpio_pin Index of the GPIO pin to configure
 * \param[in] mode     New pin sampling mode to configure
 */
static inline void system_pinmux_pin_set_input_sample_mode(
		const uint8_t gpio_pin,
		const enum system_pinmux_pin_sample mode)
{
	PortGroup* const port = system_pinmux_get_group_from_gpio_pin(gpio_pin);
	uint32_t pin_index = (gpio_pin % 32);

	if (mode == SYSTEM_PINMUX_PIN_SAMPLE_ONDEMAND) {
		port->CTRL.reg |= (1 << pin_index);
	} else {
		port->CTRL.reg &= ~(1 << pin_index);
	}
}

/** @} */

#ifdef FEATURE_SYSTEM_PINMUX_DRIVE_STRENGTH
/**
 * \brief Port pin drive output strength enum.
 *
 * Enum for the possible output drive strengths for the port pin
 * configuration structure, to indicate the driver strength the pin should
 * use.
 */
enum system_pinmux_pin_strength {
  /** Normal output driver strength */
  SYSTEM_PINMUX_PIN_STRENGTH_NORMAL,
  /** High current output driver strength */
  SYSTEM_PINMUX_PIN_STRENGTH_HIGH,
};

/**
 * \brief Configures the output driver strength mode for a GPIO pin.
 *
 * Configures the output drive strength for a GPIO output, to
 * control the amount of current the pad is able to sink/source.
 *
 * \param[in] gpio_pin  Index of the GPIO pin to configure
 * \param[in] mode      New output driver strength mode to configure
 */
static inline void system_pinmux_pin_set_output_strength(
    const uint8_t gpio_pin,
    const enum system_pinmux_pin_strength mode)
{
  PortGroup* const port = system_pinmux_get_group_from_gpio_pin(gpio_pin);
  uint32_t pin_index = (gpio_pin % 32);

  if (mode == SYSTEM_PINMUX_PIN_STRENGTH_HIGH) {
    port->PINCFG[pin_index].reg |=  PORT_PINCFG_DRVSTR;
  }
 else {
    port->PINCFG[pin_index].reg &= ~PORT_PINCFG_DRVSTR;
 }
}

void system_pinmux_group_set_output_strength(
    PortGroup *const port,
    const uint32_t mask,
    const enum system_pinmux_pin_strength mode);
#endif

#ifdef FEATURE_SYSTEM_PINMUX_SLEWRATE_LIMITER
/**
 * \brief Port pin output slew rate enum.
 *
 * Enum for the possible output drive slew rates for the port pin
 * configuration structure, to indicate the driver slew rate the pin should
 * use.
 */
enum system_pinmux_pin_slew_rate {
  /** Normal pin output slew rate */
  SYSTEM_PINMUX_PIN_SLEW_RATE_NORMAL,
  /** Enable slew rate limiter on the pin */
  SYSTEM_PINMUX_PIN_SLEW_RATE_LIMITED,
};

/**
 * \brief Configures the output slew rate mode for a GPIO pin.
 *
 * Configures the output slew rate mode for a GPIO output, to
 * control the speed at which the physical output pin can react to
 * logical changes of the I/O pin value.
 *
 * \param[in] gpio_pin  Index of the GPIO pin to configure
 * \param[in] mode      New pin slew rate mode to configure
 */
static inline void system_pinmux_pin_set_output_slew_rate(
    const uint8_t gpio_pin,
    const enum system_pinmux_pin_slew_rate mode)
{
  PortGroup* const port = system_pinmux_get_group_from_gpio_pin(gpio_pin);
  uint32_t pin_index = (gpio_pin % 32);

  if (mode == SYSTEM_PINMUX_PIN_SLEW_RATE_LIMITED) {
    port->PINCFG[pin_index].reg |=  PORT_PINCFG_SLEWLIM;
  }
  else {
    port->PINCFG[pin_index].reg &= ~PORT_PINCFG_SLEWLIM;
  }
}

void system_pinmux_group_set_output_slew_rate(
   PortGroup *const port,
    const uint32_t mask,
    const enum system_pinmux_pin_slew_rate mode);
#endif

#ifdef FEATURE_SYSTEM_PINMUX_OPEN_DRAIN
/**
 * \brief Port pin output drive mode enum.
 *
 * Enum for the possible output drive modes for the port pin configuration
 * structure, to indicate the output mode the pin should use.
 */
enum system_pinmux_pin_drive {
  /** Use totem pole output drive mode */
  SYSTEM_PINMUX_PIN_DRIVE_TOTEM,
  /** Use open drain output drive mode */
  SYSTEM_PINMUX_PIN_DRIVE_OPEN_DRAIN,
};

/**
 * \brief Configures the output driver mode for a GPIO pin.
 *
 * Configures the output driver mode for a GPIO output, to
 * control the pad behavior.
 *
 * \param[in] gpio_pin  Index of the GPIO pin to configure
 * \param[in] mode      New pad output driver mode to configure
 */
static inline void system_pinmux_pin_set_output_drive(
    const uint8_t gpio_pin,
    const enum system_pinmux_pin_drive mode)
{
  PortGroup* const port = system_pinmux_get_group_from_gpio_pin(gpio_pin);
  uint32_t pin_index = (gpio_pin % 32);

  if (mode == SYSTEM_PINMUX_PIN_DRIVE_OPEN_DRAIN) {
    port->PINCFG[pin_index].reg |=  PORT_PINCFG_ODRAIN;
  }
  else {
    port->PINCFG[pin_index].reg &= ~PORT_PINCFG_ODRAIN;
  }
}

void system_pinmux_group_set_output_drive(
    PortGroup *const port,
    const uint32_t mask,
    const enum system_pinmux_pin_drive mode);
#endif

#ifdef __cplusplus
}
#endif

/** @} */

/**
 * \page asfdoc_sam0_system_pinmux_extra Extra Information for SYSTEM PINMUX Driver
 *
 * \section asfdoc_sam0_system_pinmux_extra_acronyms Acronyms
 * The table below presents the acronyms used in this module:
 *
 * <table>
 *	<tr>
 *		<th>Acronym</th>
 *		<th>Description</th>
 *	</tr>
 *	<tr>
 *		<td>GPIO</td>
 *		<td>General Purpose Input/Output</td>
 *	</tr>
 *	<tr>
 *		<td>MUX</td>
 *		<td>Multiplexer</td>
 *	</tr>
 * </table>
 *
 *
 * \section asfdoc_sam0_system_pinmux_extra_dependencies Dependencies
 * This driver has the following dependencies:
 *
 *  - None
 *
 *
 * \section asfdoc_sam0_system_pinmux_extra_errata Errata
 * There are no errata related to this driver.
 *
 *
 * \section asfdoc_sam0_system_pinmux_extra_history Module History
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
 *		<td>Removed code of open drain, slew limit and drive strength
 *		features</td>
 *	</tr>
 *	<tr>
 *		<td>Fixed broken sampling mode function implementations, which wrote
 *		    corrupt configuration values to the device registers</td>
 *	</tr>
 *	<tr>
 *		<td>Added missing NULL pointer asserts to the PORT driver functions</td>
 *	</tr>
 *	<tr>
 *		<td>Initial Release</td>
 *	</tr>
 * </table>
 */

/**
 * \page asfdoc_sam0_system_pinmux_exqsg Examples for SYSTEM PINMUX Driver
 *
 * This is a list of the available Quick Start guides (QSGs) and example
 * applications for \ref asfdoc_sam0_system_pinmux_group. QSGs are simple
 * examples with step-by-step instructions to configure and use this driver in a
 * selection of use cases. Note that a QSG can be compiled as a standalone
 * application or be added to the user application.
 *
 *  - \subpage asfdoc_sam0_system_pinmux_basic_use_case
 *
 * \page asfdoc_sam0_system_pinmux_document_revision_history Document Revision History
 *
 * <table>
 *	<tr>
 *		<th>Doc. Rev.</td>
 *		<th>Date</td>
 *		<th>Comments</td>
 *	</tr>
 *	<tr>
 *		<td>42121F</td>
 *		<td>12/2015</td>
 *		<td>Added support for SAM L21/L22, SAM DA1, SAM D09, and SAM C20/C21</td>
 *	</tr>
 *	<tr>
 *		<td>42121E</td>
 *		<td>12/2014</td>
 *		<td>Added support for SAM R21 and SAM D10/D11</td>
 *	</tr>
 *	<tr>
 *		<td>42121D</td>
 *		<td>01/2014</td>
 *		<td>Added support for SAM D21</td>
 *	</tr>
 *	<tr>
 *		<td>42121C</td>
 *		<td>09/2013</td>
 *		<td>Fixed incorrect documentation for the device pin sampling mode</td>
 *	</tr>
 *	<tr>
 *		<td>42121B</td>
 *		<td>06/2013</td>
 *		<td>Corrected documentation typos</td>
 *	</tr>
 *	<tr>
 *		<td>42121A</td>
 *		<td>06/2013</td>
 *		<td>Initial release</td>
 *	</tr>
 * </table>
 */

#endif
