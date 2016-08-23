/**
 * \file
 *
 * \brief Common API for USB Device Interface
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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

#ifndef _UDI_H_
#define _UDI_H_

#include "conf_usb.h"
#include "usb_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup usb_device_group
 * \defgroup udi_group USB Device Interface (UDI)
 * The UDI provides a common API for all classes,
 * and this is used by UDC for the main control of USB Device interface.
 * @{
 */

/**
 * \brief UDI API.
 *
 * The callbacks within this structure are called only by
 * USB Device Controller (UDC)
 *
 * The udc_get_interface_desc() can be use by UDI to know the interface descriptor
 * selected by UDC.
 */
typedef struct {
	/**
	 * \brief Enable the interface.
	 *
	 * This function is called when the host selects a configuration
	 * to which this interface belongs through a Set Configuration
	 * request, and when the host selects an alternate setting of
	 * this interface through a Set Interface request.
	 *
	 * \return \c 1 if function was successfully done, otherwise \c 0.
	 */
	bool(*enable) (void);

	/**
	 * \brief Disable the interface.
	 *
	 * This function is called when this interface is currently
	 * active, and
	 * - the host selects any configuration through a Set
	 *   Configuration request, or
	 * - the host issues a USB reset, or
	 * - the device is detached from the host (i.e. Vbus is no
	 *   longer present)
	 */
	void (*disable) (void);

	/**
	 * \brief Handle a control request directed at an interface.
	 *
	 * This function is called when this interface is currently
	 * active and the host sends a SETUP request
	 * with this interface as the recipient.
	 *
	 * Use udd_g_ctrlreq to decode and response to SETUP request.
	 *
	 * \return \c 1 if this interface supports the SETUP request, otherwise \c 0.
	 */
	bool(*setup) (void);

	/**
	 * \brief Returns the current setting of the selected interface.
	 *
	 * This function is called when UDC when know alternate setting of selected interface.
	 *
	 * \return alternate setting of selected interface
	 */
	uint8_t(*getsetting) (void);

	/**
	 * \brief To signal that a SOF is occurred
	 */
	void(*sof_notify) (void);
} udi_api_t;

//@}

#ifdef __cplusplus
}
#endif
#endif // _UDI_H_
