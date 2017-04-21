/*
 * \file
 *
 * \brief SAM Event System Controller Driver
 *
 * Copyright (C) 2013-2015 Atmel Corporation. All rights reserved.
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

#include <events.h>
#include <system.h>
#include <system_interrupt.h>
#include <status_codes.h>

#define EVENTS_INVALID_CHANNEL                  0xff

struct _events_module _events_inst = {
		.allocated_channels = 0,
		.free_channels      = EVSYS_CHANNELS,

#if EVENTS_INTERRUPT_HOOKS_MODE == true
		.interrupt_flag_buffer     = 0,
		.interrupt_flag_ack_buffer = 0,

		.hook_list                 = NULL,
#endif
};

/**
 * \internal
 *
 */
uint32_t _events_find_bit_position(uint8_t channel, uint8_t start_offset)
{
	uint32_t pos;

	if (channel < _EVENTS_START_OFFSET_BUSY_BITS) {
		pos = 0x01UL << (start_offset + channel);
	} else {
		pos = 0x01UL << (start_offset + channel + _EVENTS_START_OFFSET_BUSY_BITS);
	}

	return pos;
}

static uint8_t _events_find_first_free_channel_and_allocate(void)
{
	uint8_t count;
	uint32_t tmp;
	bool allocated = false;

	system_interrupt_enter_critical_section();

	tmp = _events_inst.allocated_channels;

	for(count = 0; count < EVSYS_CHANNELS; ++count) {

		if(!(tmp & 0x00000001)) {
			/* If free channel found, set as allocated and return number */

			_events_inst.allocated_channels |= 1 << count;
			_events_inst.free_channels--;
			allocated = true;

			break;

		}

		tmp = tmp >> 1;
	}

	system_interrupt_leave_critical_section();

	if(!allocated) {
		return EVENTS_INVALID_CHANNEL;
	} else {
		return count;
	}
}

static void _events_release_channel(uint8_t channel)
{
	system_interrupt_enter_critical_section();

	_events_inst.allocated_channels &= ~(1 << channel);
	_events_inst.free_channels++;

	system_interrupt_leave_critical_section();
}


/* This function is called by the system_init function, but should not be a public API call */
#if defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wmissing-prototypes"
#endif
void _system_events_init(void)
{
	/* Enable EVSYS register interface */
	system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBC, PM_APBCMASK_EVSYS);

	/* Make sure the EVSYS module is properly reset */
	EVSYS->CTRL.reg = EVSYS_CTRL_SWRST;

	while (EVSYS->CTRL.reg & EVSYS_CTRL_SWRST) {
	}
}
#if defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif

void events_get_config_defaults(struct events_config *config)
{
	/* Check that config is something other than NULL */
	Assert(config);

	config->edge_detect  = EVENTS_EDGE_DETECT_RISING;
	config->path         = EVENTS_PATH_SYNCHRONOUS;
	config->generator    = EVSYS_ID_GEN_NONE;
	config->clock_source = GCLK_GENERATOR_0;
}

enum status_code events_allocate(
		struct events_resource *resource,
		struct events_config *config)
{
	uint8_t new_channel;

	Assert(resource);

	new_channel = _events_find_first_free_channel_and_allocate();

	if(new_channel == EVENTS_INVALID_CHANNEL) {
		return STATUS_ERR_NOT_FOUND;
	}

	resource->channel = new_channel;

	if (config->path != EVENTS_PATH_ASYNCHRONOUS) {
		/* Set up a GLCK channel to use with the specific channel */
		struct system_gclk_chan_config gclk_chan_conf;

		system_gclk_chan_get_config_defaults(&gclk_chan_conf);
		gclk_chan_conf.source_generator =
				(enum gclk_generator)config->clock_source;
		system_gclk_chan_set_config(EVSYS_GCLK_ID_0 + new_channel, &gclk_chan_conf);
		system_gclk_chan_enable(EVSYS_GCLK_ID_0 + new_channel);
	}

	/* Save channel setting and configure it after user multiplexer */
	resource->channel_reg = EVSYS_CHANNEL_CHANNEL(new_channel)       |
			     EVSYS_CHANNEL_EVGEN(config->generator)   |
			     EVSYS_CHANNEL_PATH(config->path)         |
			     EVSYS_CHANNEL_EDGSEL(config->edge_detect);


	return STATUS_OK;
}


enum status_code events_release(struct events_resource *resource)
{
	enum status_code err = STATUS_OK;

	Assert(resource);

	/* Check if channel is busy */
	if(events_is_busy(resource)) {
		return STATUS_BUSY;
	}

	if (!(_events_inst.allocated_channels & (1<<resource->channel))) {
		err = STATUS_ERR_NOT_INITIALIZED;
	} else {
		_events_release_channel(resource->channel);
	}

	return err;
}

enum status_code events_trigger(struct events_resource *resource)
{

	Assert(resource);

	system_interrupt_enter_critical_section();

	/* Because of indirect access the channel must be set first */
	((uint8_t*)&EVSYS->CHANNEL)[0] = EVSYS_CHANNEL_CHANNEL(resource->channel);

	/* Assert if event path is asynchronous */
	if (EVSYS->CHANNEL.reg & EVSYS_CHANNEL_PATH(EVENTS_PATH_ASYNCHRONOUS)) {
		return STATUS_ERR_UNSUPPORTED_DEV;
	}

	/* Assert if event edge detection is not set to RISING */
	if (!(EVSYS->CHANNEL.reg & EVSYS_CHANNEL_EDGSEL(EVENTS_EDGE_DETECT_RISING))) {
		return STATUS_ERR_UNSUPPORTED_DEV;
	}


	/* The GCLKREQ bit has to be set while triggering the software event */
	EVSYS->CTRL.reg = EVSYS_CTRL_GCLKREQ;

	((uint16_t*)&EVSYS->CHANNEL)[0] = EVSYS_CHANNEL_CHANNEL(resource->channel) |
					  EVSYS_CHANNEL_SWEVT;

	EVSYS->CTRL.reg &= ~EVSYS_CTRL_GCLKREQ;

	system_interrupt_leave_critical_section();

	return STATUS_OK;
}

bool events_is_busy(struct events_resource *resource)
{
	Assert(resource);

	return EVSYS->CHSTATUS.reg & (_events_find_bit_position(resource->channel,
			_EVENTS_START_OFFSET_BUSY_BITS));
}

bool events_is_users_ready(struct events_resource *resource)
{
	Assert(resource);

	return EVSYS->CHSTATUS.reg & (_events_find_bit_position(resource->channel,
			_EVENTS_START_OFFSET_USER_READY_BIT));
}

bool events_is_detected(struct events_resource *resource)
{
	Assert(resource);

	uint32_t flag = _events_find_bit_position(resource->channel,
			_EVENTS_START_OFFSET_DETECTION_BIT);

	/* Clear flag when read */
	if (EVSYS->INTFLAG.reg & flag) {
		EVSYS->INTFLAG.reg = flag;
		return true;
	}

	return false;
}

bool events_is_overrun(struct events_resource *resource)
{
	Assert(resource);

	uint32_t flag = _events_find_bit_position(resource->channel,
			_EVENTS_START_OFFSET_OVERRUN_BIT);

	/* Clear flag when read */
	if (EVSYS->INTFLAG.reg & flag) {
		EVSYS->INTFLAG.reg = flag;
		return true;
	}

	return false;
}

enum status_code events_attach_user(struct events_resource *resource, uint8_t user_id)
{
	Assert(resource);

	/* First configure user multiplexer: channel number is n + 1 */
	EVSYS->USER.reg = EVSYS_USER_CHANNEL(resource->channel + 1) |
			  EVSYS_USER_USER(user_id);

	/* Then configure the channel */
	EVSYS->CHANNEL.reg = resource->channel_reg;

	return STATUS_OK;
}

enum status_code events_detach_user(struct events_resource *resource, uint8_t user_id)
{

	Assert(resource);

	/* Write 0 to the channel bit field to select no input */
	EVSYS->USER.reg = EVSYS_USER_USER(user_id);

	return STATUS_OK;
}

uint8_t events_get_free_channels()
{
	return _events_inst.free_channels;
}
