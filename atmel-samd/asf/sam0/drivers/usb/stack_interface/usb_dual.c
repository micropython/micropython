/**
 * \file
 *
 * \brief SAM USB Dual Role driver file.
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
#include <compiler.h>
#include "usb_dual.h"

#ifndef UDD_ENABLE
# define udc_start()
# define udc_stop()
#else
#include <udc.h>
#endif

#ifndef UHD_ENABLE
# define uhc_start(void)
# define uhc_stop(b_id_stop)
#else
#include <uhc.h>
#endif

/* State of USB dual role initialization */
static bool _initialized = false;

#define _usb_is_id_device()         port_pin_get_input_level(USB_ID_PIN)

#if USB_ID_EIC
static void usb_id_handler(void);

/**
 * \name USB ID PAD management
 *
 * @{
 */

/**
 * USB ID pin configuration
 */
static void usb_id_config(void)
{
	struct extint_chan_conf eint_chan_conf;
	extint_chan_get_config_defaults(&eint_chan_conf);

	eint_chan_conf.gpio_pin           = USB_ID_PIN;
	eint_chan_conf.gpio_pin_mux       = USB_ID_EIC_MUX;
	eint_chan_conf.detection_criteria = EXTINT_DETECT_BOTH;
	eint_chan_conf.filter_input_signal = true;

	extint_chan_disable_callback(USB_ID_EIC_LINE,
			EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_set_config(USB_ID_EIC_LINE, &eint_chan_conf);
	extint_register_callback(usb_id_handler,
			USB_ID_EIC_LINE,
			EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(USB_ID_EIC_LINE,
			EXTINT_CALLBACK_TYPE_DETECT);
}

/**
 * USB ID pin change handler
 */
static void usb_id_handler(void)
{
	extint_chan_disable_callback(USB_ID_EIC_LINE,
			EXTINT_CALLBACK_TYPE_DETECT);
	if (_usb_is_id_device()) {
		uhc_stop(false);
		UHC_MODE_CHANGE(false);
		udc_start();
	} else {
		udc_stop();
		UHC_MODE_CHANGE(true);
		uhc_start();
	}
	extint_chan_enable_callback(USB_ID_EIC_LINE,
			EXTINT_CALLBACK_TYPE_DETECT);
}
#endif
/** @} */

/**
 * \brief Initialize the USB peripheral and set right role according to ID pin
 *
 * \return \c true if the ID pin management has been started, otherwise \c false.
 */
bool usb_dual_enable(void)
{
	if (_initialized) {
		return false; // Dual role already initialized
	}

#if USB_ID_EIC
	_initialized = true;

	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

	/* Set USB ID Pin as inputs */
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_UP;
	port_pin_set_config(USB_ID_PIN, &pin_conf);

	usb_id_config();
	if (_usb_is_id_device()) {
		UHC_MODE_CHANGE(false);
		udc_start();
	} else {
		UHC_MODE_CHANGE(true);
		uhc_start();
	}

	/**
	 * End of host or device startup,
	 * the current mode selected is already started now
	 */
	return true; // ID pin management has been enabled
#else
	return false; // ID pin management has not been enabled
#endif
}

/**
 * \brief Deinitialize the dual role driver
 */
void usb_dual_disable(void)
{
	if (!_initialized) {
		return; // Dual role not initialized
	}
	_initialized = false;

#if USB_ID_EIC
	extint_chan_disable_callback(USB_ID_EIC_LINE,
			EXTINT_CALLBACK_TYPE_DETECT);
#endif
}

