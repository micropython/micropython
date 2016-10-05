/**
 * \file
 *
 * \brief SAM USB Dual Role driver header file.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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

#ifndef _USB_DUAL_H_
#define _USB_DUAL_H_

#include "compiler.h"
#include "preprocessor.h"

/* Get USB pads pins configuration in board configuration */
#include "conf_board.h"
#include "board.h"
#include "extint.h"
#include "port.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup usb_group
 * \defgroup usb_dual_group USB dual role driver
 * USB low-level driver for dual role features
 *
 * @{
 */

bool usb_dual_enable(void);
void usb_dual_disable(void);

/**
 * @name USB ID pin management
 *
 * The ID pin come from the USB connector (A and B receptable) and
 * allows to select the USB mode between host or device.
 * The ID pin can be managed through EIC pin.
 * This feature is optional, and it is enabled if USB_ID_PIN
 * is defined in board.h and CONF_BOARD_USB_ID_DETECT defined in
 * conf_board.h.
*
* @{
*/
#define USB_ID_DETECT        (defined(CONF_BOARD_USB_ID_DETECT))
#define USB_ID_EIC           (defined(USB_ID_PIN) && USB_ID_DETECT)
/** @} */

/**
 * @name USB Vbus management
 *
 * The VBus line can be monitored through a EIC pin and
 * a basic resistor voltage divider.
 * This feature is optional, and it is enabled if USB_VBUS_PIN
 * is defined in board.h and CONF_BOARD_USB_VBUS_DETECT defined in
 * conf_board.h.
 *
 * @{
 */
#define USB_VBUS_DETECT      (defined(CONF_BOARD_USB_VBUS_DETECT))
#define USB_VBUS_EIC         (defined(USB_VBUS_PIN) && USB_VBUS_DETECT)
/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif // _USB_DUAL_H_
