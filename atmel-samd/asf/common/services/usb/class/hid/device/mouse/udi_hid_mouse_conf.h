/**
 * \file
 *
 * \brief Default HID mouse configuration for a USB Device
 * with a single interface HID mouse
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

#ifndef _UDI_HID_MOUSE_CONF_H_
#define _UDI_HID_MOUSE_CONF_H_

/**
 * \addtogroup udi_hid_mouse_group_single_desc
 * @{
 */

//! Control endpoint size
#ifdef USB_DEVICE_HS_SUPPORT
#  define  USB_DEVICE_EP_CTRL_SIZE       64
#else
#  define  USB_DEVICE_EP_CTRL_SIZE       8
#endif


//! Endpoint number used by HID mouse interface
#define  UDI_HID_MOUSE_EP_IN            (1 | USB_EP_DIR_IN)

//! Interface number
#define  UDI_HID_MOUSE_IFACE_NUMBER     0


/**
 * \name UDD Configuration
 */
//@{
#undef USB_DEVICE_MAX_EP   // undefine this definition in header file
//! 1 endpoint used by HID mouse standard interface
#define  USB_DEVICE_MAX_EP             1
//@}

//@}

#include "udi_hid_mouse.h"

#endif // _UDI_HID_MOUSE_CONF_H_
