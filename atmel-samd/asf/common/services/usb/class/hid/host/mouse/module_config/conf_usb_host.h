/**
 * \file
 *
 * \brief USB host configuration file
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

#ifndef _CONF_USB_HOST_H_
#define _CONF_USB_HOST_H_

#include "compiler.h"

/**
 * USB Host Configuration
 * @{
 */

//! Array of UHI APIs Define the list of UHI supported by USB host.
#define USB_HOST_UHI        UHI_HID_MOUSE

//! Maximum current allowed on Vbus (mA)
#define USB_HOST_POWER_MAX  500

//! Authorize the USB HUB support
// #define USB_HOST_HUB_SUPPORT

//! Authorize the USB host to run in High Speed
#if (UC3A3 || UC3A4)
#  define USB_HOST_HS_SUPPORT
#endif

/**
 * USB Host callbacks
 * @{
 */

//! To notify that the USB mode are switched automatically.
//! This is possible only when ID pin is available.
//#define UHC_MODE_CHANGE(b_host_mode)        usb_host_mode_change(b_host_mode)

//! To notify that the Vbus level has changed
//! Available only in USB hardware with Vbus monitoring.
//#define UHC_VBUS_CHANGE(b_present)          usb_host_vbus_change(b_present)

//! To notify that a Vbus error has occurred
//! Available only in USB hardware with Vbus monitoring.
//#define UHC_VBUS_ERROR()                    usb_host_vbus_error()

//! To notify that a device has been connected or disconnected.
//#define UHC_CONNECTION_EVENT(dev,b_present) usb_host_connection_event(dev,b_present)

//! Called when a USB device or the host have wake up the USB line.
//#define UHC_WAKEUP_EVENT()                  usb_host_wakeup_event()

//! Called for each received SOF each 1 ms
//! Note: Available in High and Full speed mode
//#define UHC_SOF_EVENT()                     usb_host_sof_event()

//! Called when a USB device configuration must be chosen.
//! Thus, the application can choose either a configuration number for this device
//! or a configuration number 0 to reject it.
//! If callback not defined the configuration 1 is chosen.
//#define UHC_DEVICE_CONF(dev)                uint8_t usb_host_device_conf(dev)

//! Called when a USB device enumeration is completed.
//#define UHC_ENUM_EVENT(dev,b_status)        usb_host_enum_event(dev,b_status)


//@}



/**
 * USB Interface Configuration
 * @{
 */
/**
 * Configuration of HID Mouse interface
 * @{
 */
#define UHI_HID_MOUSE_CHANGE(dev,b_plug)
#define UHI_HID_MOUSE_EVENT_BTN_LEFT(b_state)
#define UHI_HID_MOUSE_EVENT_BTN_RIGHT(b_state)
#define UHI_HID_MOUSE_EVENT_BTN_MIDDLE(b_state)
#define UHI_HID_MOUSE_EVENT_MOUVE(x,y,scroll)
//@}
//@}

//@}

#include "uhi_hid_mouse.h"

#endif // _CONF_USB_HOST_H_
