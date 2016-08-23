/**
 * \file
 *
 * \brief SAM D21/R21/DA0/DA1 Generic Clock Driver
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

#include <gclk.h>
#include <clock.h>
#include <system_interrupt.h>

/**
 * \brief Determines if the hardware module(s) are currently synchronizing to the bus.
 *
 * Checks to see if the underlying hardware peripheral module(s) are currently
 * synchronizing across multiple clock domains to the hardware bus, This
 * function can be used to delay further operations on a module until such time
 * that it is ready, to prevent blocking delays for synchronization in the
 * user application.
 *
 * \return Synchronization status of the underlying hardware module(s).
 *
 * \retval false if the module has completed synchronization
 * \retval true if the module synchronization is ongoing
 */
static inline bool system_gclk_is_syncing(void)
{
	if (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY){
		return true;
	}

	return false;
}

/**
 * \brief Initializes the GCLK driver.
 *
 * Initializes the Generic Clock module, disabling and resetting all active
 * Generic Clock Generators and Channels to their power-on default values.
 */
void system_gclk_init(void)
{
	/* Turn on the digital interface clock */
	system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBA, PM_APBAMASK_GCLK);

	/* Software reset the module to ensure it is re-initialized correctly */
	GCLK->CTRL.reg = GCLK_CTRL_SWRST;
	while (GCLK->CTRL.reg & GCLK_CTRL_SWRST) {
		/* Wait for reset to complete */
	}
}

/**
 * \brief Writes a Generic Clock Generator configuration to the hardware module.
 *
 * Writes out a given configuration of a Generic Clock Generator configuration
 * to the hardware module.
 *
 * \note Changing the clock source on the fly (on a running
 *       generator) can take additional time if the clock source is configured
 *       to only run on-demand (ONDEMAND bit is set) and it is not currently
 *       running (no peripheral is requesting the clock source). In this case
 *       the GCLK will request the new clock while still keeping a request to
 *       the old clock source until the new clock source is ready.
 *
 * \note This function will not start a generator that is not already running;
 *       to start the generator, call \ref system_gclk_gen_enable()
 *       after configuring a generator.
 *
 * \param[in] generator  Generic Clock Generator index to configure
 * \param[in] config     Configuration settings for the generator
 */
void system_gclk_gen_set_config(
		const uint8_t generator,
		struct system_gclk_gen_config *const config)
{
	/* Sanity check arguments */
	Assert(config);

	/* Cache new register configurations to minimize sync requirements. */
	uint32_t new_genctrl_config = (generator << GCLK_GENCTRL_ID_Pos);
	uint32_t new_gendiv_config  = (generator << GCLK_GENDIV_ID_Pos);

	/* Select the requested source clock for the generator */
	new_genctrl_config |= config->source_clock << GCLK_GENCTRL_SRC_Pos;

	/* Configure the clock to be either high or low when disabled */
	if (config->high_when_disabled) {
		new_genctrl_config |= GCLK_GENCTRL_OOV;
	}

	/* Configure if the clock output to I/O pin should be enabled. */
	if (config->output_enable) {
		new_genctrl_config |= GCLK_GENCTRL_OE;
	}

	/* Set division factor */
	if (config->division_factor > 1) {
		/* Check if division is a power of two */
		if (((config->division_factor & (config->division_factor - 1)) == 0)) {
			/* Determine the index of the highest bit set to get the
			 * division factor that must be loaded into the division
			 * register */

			uint32_t div2_count = 0;

			uint32_t mask;
			for (mask = (1UL << 1); mask < config->division_factor;
						mask <<= 1) {
				div2_count++;
			}

			/* Set binary divider power of 2 division factor */
			new_gendiv_config  |= div2_count << GCLK_GENDIV_DIV_Pos;
			new_genctrl_config |= GCLK_GENCTRL_DIVSEL;
		} else {
			/* Set integer division factor */

			new_gendiv_config  |=
					(config->division_factor) << GCLK_GENDIV_DIV_Pos;

			/* Enable non-binary division with increased duty cycle accuracy */
			new_genctrl_config |= GCLK_GENCTRL_IDC;
		}

	}

	/* Enable or disable the clock in standby mode */
	if (config->run_in_standby) {
		new_genctrl_config |= GCLK_GENCTRL_RUNSTDBY;
	}

	while (system_gclk_is_syncing()) {
		/* Wait for synchronization */
	};

	system_interrupt_enter_critical_section();

	/* Select the correct generator */
	*((uint8_t*)&GCLK->GENDIV.reg) = generator;

	/* Write the new generator configuration */
	while (system_gclk_is_syncing()) {
		/* Wait for synchronization */
	};
	GCLK->GENDIV.reg  = new_gendiv_config;

	while (system_gclk_is_syncing()) {
		/* Wait for synchronization */
	};
	GCLK->GENCTRL.reg = new_genctrl_config | (GCLK->GENCTRL.reg & GCLK_GENCTRL_GENEN);

	system_interrupt_leave_critical_section();
}

/**
 * \brief Enables a Generic Clock Generator that was previously configured.
 *
 * Starts the clock generation of a Generic Clock Generator that was previously
 * configured via a call to \ref system_gclk_gen_set_config().
 *
 * \param[in] generator  Generic Clock Generator index to enable
 */
void system_gclk_gen_enable(
		const uint8_t generator)
{
	while (system_gclk_is_syncing()) {
		/* Wait for synchronization */
	};

	system_interrupt_enter_critical_section();

	/* Select the requested generator */
	*((uint8_t*)&GCLK->GENCTRL.reg) = generator;
	while (system_gclk_is_syncing()) {
		/* Wait for synchronization */
	};

	/* Enable generator */
	GCLK->GENCTRL.reg |= GCLK_GENCTRL_GENEN;

	system_interrupt_leave_critical_section();
}

/**
 * \brief Disables a Generic Clock Generator that was previously enabled.
 *
 * Stops the clock generation of a Generic Clock Generator that was previously
 * started via a call to \ref system_gclk_gen_enable().
 *
 * \param[in] generator  Generic Clock Generator index to disable
 */
void system_gclk_gen_disable(
		const uint8_t generator)
{
	while (system_gclk_is_syncing()) {
		/* Wait for synchronization */
	};

	system_interrupt_enter_critical_section();

	/* Select the requested generator */
	*((uint8_t*)&GCLK->GENCTRL.reg) = generator;
	while (system_gclk_is_syncing()) {
		/* Wait for synchronization */
	};

	/* Disable generator */
	GCLK->GENCTRL.reg &= ~GCLK_GENCTRL_GENEN;
	while (GCLK->GENCTRL.reg & GCLK_GENCTRL_GENEN) {
		/* Wait for clock to become disabled */
	}

	system_interrupt_leave_critical_section();
}

/**
 * \brief Determins if the specified Generic Clock Generator is enabled.
 *
 * \param[in] generator  Generic Clock Generator index to check
 *
 * \return The enabled status.
 * \retval true The Generic Clock Generator is enabled
 * \retval false The Generic Clock Generator is disabled
 */
bool system_gclk_gen_is_enabled(
		const uint8_t generator)
{
	bool enabled;

	system_interrupt_enter_critical_section();

	/* Select the requested generator */
	*((uint8_t*)&GCLK->GENCTRL.reg) = generator;
	/* Obtain the enabled status */
	enabled = (GCLK->GENCTRL.reg & GCLK_GENCTRL_GENEN);

	system_interrupt_leave_critical_section();

	return enabled;
}

/**
 * \brief Retrieves the clock frequency of a Generic Clock generator.
 *
 * Determines the clock frequency (in Hz) of a specified Generic Clock
 * generator, used as a source to a Generic Clock Channel module.
 *
 * \param[in] generator  Generic Clock Generator index
 *
 * \return The frequency of the generic clock generator, in Hz.
 */
uint32_t system_gclk_gen_get_hz(
		const uint8_t generator)
{
	while (system_gclk_is_syncing()) {
		/* Wait for synchronization */
	};

	system_interrupt_enter_critical_section();

	/* Select the appropriate generator */
	*((uint8_t*)&GCLK->GENCTRL.reg) = generator;
	while (system_gclk_is_syncing()) {
		/* Wait for synchronization */
	};

	/* Get the frequency of the source connected to the GCLK generator */
	uint32_t gen_input_hz = system_clock_source_get_hz(
			(enum system_clock_source)GCLK->GENCTRL.bit.SRC);

	*((uint8_t*)&GCLK->GENCTRL.reg) = generator;

	uint8_t divsel = GCLK->GENCTRL.bit.DIVSEL;

	/* Select the appropriate generator division register */
	*((uint8_t*)&GCLK->GENDIV.reg) = generator;
	while (system_gclk_is_syncing()) {
		/* Wait for synchronization */
	};

	uint32_t divider = GCLK->GENDIV.bit.DIV;

	system_interrupt_leave_critical_section();

	/* Check if the generator is using fractional or binary division */
	if (!divsel && divider > 1) {
		gen_input_hz /= divider;
	} else if (divsel) {
		gen_input_hz >>= (divider+1);
	}

	return gen_input_hz;
}

/**
 * \brief Writes a Generic Clock configuration to the hardware module.
 *
 * Writes out a given configuration of a Generic Clock configuration to the
 * hardware module. If the clock is currently running, it will be stopped.
 *
 * \note Once called the clock will not be running; to start the clock,
 *       call \ref system_gclk_chan_enable() after configuring a clock channel.
 *
 * \param[in] channel   Generic Clock channel to configure
 * \param[in] config    Configuration settings for the clock
 *
 */
void system_gclk_chan_set_config(
		const uint8_t channel,
		struct system_gclk_chan_config *const config)
{
	/* Sanity check arguments */
	Assert(config);

	/* Cache the new config to reduce sync requirements */
	uint32_t new_clkctrl_config = (channel << GCLK_CLKCTRL_ID_Pos);

	/* Select the desired generic clock generator */
	new_clkctrl_config |= config->source_generator << GCLK_CLKCTRL_GEN_Pos;

	/* Disable generic clock channel */
	system_gclk_chan_disable(channel);

	/* Write the new configuration */
	GCLK->CLKCTRL.reg = new_clkctrl_config;
}

/**
 * \brief Enables a Generic Clock that was previously configured.
 *
 * Starts the clock generation of a Generic Clock that was previously
 * configured via a call to \ref system_gclk_chan_set_config().
 *
 * \param[in] channel   Generic Clock channel to enable
 */
void system_gclk_chan_enable(
		const uint8_t channel)
{
	system_interrupt_enter_critical_section();

	/* Select the requested generator channel */
	*((uint8_t*)&GCLK->CLKCTRL.reg) = channel;

	/* Enable the generic clock */
	GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_CLKEN;

	system_interrupt_leave_critical_section();
}

/**
 * \brief Disables a Generic Clock that was previously enabled.
 *
 * Stops the clock generation of a Generic Clock that was previously started
 * via a call to \ref system_gclk_chan_enable().
 *
 * \param[in] channel  Generic Clock channel to disable
 */
void system_gclk_chan_disable(
		const uint8_t channel)
{
	system_interrupt_enter_critical_section();

	/* Select the requested generator channel */
	*((uint8_t*)&GCLK->CLKCTRL.reg) = channel;

	/* Sanity check WRTLOCK */
	Assert(!GCLK->CLKCTRL.bit.WRTLOCK);

	/* Switch to known-working source so that the channel can be disabled */
	uint32_t prev_gen_id = GCLK->CLKCTRL.bit.GEN;
	GCLK->CLKCTRL.bit.GEN = 0;

	/* Disable the generic clock */
	GCLK->CLKCTRL.reg &= ~GCLK_CLKCTRL_CLKEN;
	while (GCLK->CLKCTRL.reg & GCLK_CLKCTRL_CLKEN) {
		/* Wait for clock to become disabled */
	}

	/* Restore previous configured clock generator */
	GCLK->CLKCTRL.bit.GEN = prev_gen_id;

	system_interrupt_leave_critical_section();
}

/**
 * \brief Determins if the specified Generic Clock channel is enabled.
 *
 * \param[in] channel  Generic Clock Channel index
 *
 * \return The enabled status.
 * \retval true The Generic Clock channel is enabled
 * \retval false The Generic Clock channel is disabled
 */
bool system_gclk_chan_is_enabled(
		const uint8_t channel)
{
	bool enabled;

	system_interrupt_enter_critical_section();

	/* Select the requested generic clock channel */
	*((uint8_t*)&GCLK->CLKCTRL.reg) = channel;
	enabled = GCLK->CLKCTRL.bit.CLKEN;

	system_interrupt_leave_critical_section();

	return enabled;
}

/**
 * \brief Locks a Generic Clock channel from further configuration writes.
 *
 * Locks a generic clock channel from further configuration writes. It is only
 * possible to unlock the channel configuration through a power on reset.
 *
 * \param[in] channel   Generic Clock channel to enable
 */
void system_gclk_chan_lock(
		const uint8_t channel)
{
	system_interrupt_enter_critical_section();

	/* Select the requested generator channel */
	*((uint8_t*)&GCLK->CLKCTRL.reg) = channel;

	/* Lock the generic clock */
	GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_WRTLOCK | GCLK_CLKCTRL_CLKEN;

	system_interrupt_leave_critical_section();
}

/**
 * \brief Determins if the specified Generic Clock channel is locked.
 *
 * \param[in] channel  Generic Clock Channel index
 *
 * \return The lock status.
 * \retval true The Generic Clock channel is locked
 * \retval false The Generic Clock channel is not locked
 */
bool system_gclk_chan_is_locked(
		const uint8_t channel)
{
	bool locked;

	system_interrupt_enter_critical_section();

	/* Select the requested generic clock channel */
	*((uint8_t*)&GCLK->CLKCTRL.reg) = channel;
	locked = GCLK->CLKCTRL.bit.WRTLOCK;

	system_interrupt_leave_critical_section();

	return locked;
}

/**
 * \brief Retrieves the clock frequency of a Generic Clock channel.
 *
 * Determines the clock frequency (in Hz) of a specified Generic Clock
 * channel, used as a source to a device peripheral module.
 *
 * \param[in] channel  Generic Clock Channel index
 *
 * \return The frequency of the generic clock channel, in Hz.
 */
uint32_t system_gclk_chan_get_hz(
		const uint8_t channel)
{
	uint8_t gen_id;

	system_interrupt_enter_critical_section();

	/* Select the requested generic clock channel */
	*((uint8_t*)&GCLK->CLKCTRL.reg) = channel;
	gen_id = GCLK->CLKCTRL.bit.GEN;

	system_interrupt_leave_critical_section();

	/* Return the clock speed of the associated GCLK generator */
	return system_gclk_gen_get_hz(gen_id);
}
