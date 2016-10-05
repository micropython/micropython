/**
 * \file
 *
 * \brief SAM GPIO Port Driver
 *
 * Copyright (C) 2012-2015 Atmel Corporation. All rights reserved.
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
#include <port.h>

/**
 *  \brief Writes a Port pin configuration to the hardware module.
 *
 *  Writes out a given configuration of a Port pin configuration to the hardware
 *  module.
 *
 *  \note If the pin direction is set as an output, the pull-up/pull-down input
 *        configuration setting is ignored.
 *
 *  \param[in] gpio_pin  Index of the GPIO pin to configure
 *  \param[in] config    Configuration settings for the pin
 */
void port_pin_set_config(
		const uint8_t gpio_pin,
		const struct port_config *const config)
{
	/* Sanity check arguments */
	Assert(config);

	struct system_pinmux_config pinmux_config;
	system_pinmux_get_config_defaults(&pinmux_config);

	pinmux_config.mux_position = SYSTEM_PINMUX_GPIO;
	pinmux_config.direction    = (enum system_pinmux_pin_dir)config->direction;
	pinmux_config.input_pull   = (enum system_pinmux_pin_pull)config->input_pull;
	pinmux_config.powersave    = config->powersave;

	system_pinmux_pin_set_config(gpio_pin, &pinmux_config);
}

/**
 *  \brief Writes a Port group configuration group to the hardware module.
 *
 *  Writes out a given configuration of a Port group configuration to the
 *  hardware module.
 *
 *  \note If the pin direction is set as an output, the pull-up/pull-down input
 *        configuration setting is ignored.
 *
 *  \param[out] port    Base of the PORT module to write to
 *  \param[in]  mask    Mask of the port pin(s) to configure
 *  \param[in]  config  Configuration settings for the pin group
 */
void port_group_set_config(
		PortGroup *const port,
		const uint32_t mask,
		const struct port_config *const config)
{
	/* Sanity check arguments */
	Assert(port);
	Assert(config);

	struct system_pinmux_config pinmux_config;
	system_pinmux_get_config_defaults(&pinmux_config);

	pinmux_config.mux_position = SYSTEM_PINMUX_GPIO;
	pinmux_config.direction    = (enum system_pinmux_pin_dir)config->direction;
	pinmux_config.input_pull   = (enum system_pinmux_pin_pull)config->input_pull;
	pinmux_config.powersave    = config->powersave;

	system_pinmux_group_set_config(port, mask, &pinmux_config);
}
