/**
 * \file
 *
 * \brief USB host driver for Human Interface Device (HID) mouse interface.
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
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

#ifndef _UHI_HID_MOUSE_H_
#define _UHI_HID_MOUSE_H_

#include "conf_usb_host.h"
#include "usb_protocol.h"
#include "usb_protocol_hid.h"
#include "uhi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup uhi_hid_mouse_group
 * \defgroup uhi_hid_mouse_group_uhc Interface with USB Host Core (UHC)
 *
 * Define and functions required by UHC.
 *
 * @{
 */

//! Global define which contains standard UHI API for UHC
//! It must be added in USB_HOST_UHI define from conf_usb_host.h file.
#define UHI_HID_MOUSE { \
	.install = uhi_hid_mouse_install, \
	.enable = uhi_hid_mouse_enable, \
	.uninstall = uhi_hid_mouse_uninstall, \
	.sof_notify = NULL, \
}

/**
 * \name Functions required by UHC
 * @{
 */
extern uhc_enum_status_t uhi_hid_mouse_install(uhc_device_t* dev);
extern void uhi_hid_mouse_enable(uhc_device_t* dev);
extern void uhi_hid_mouse_uninstall(uhc_device_t* dev);
//@}
//@}

/**
 * \ingroup uhi_group
 * \defgroup uhi_hid_mouse_group UHI for Human Interface Device Mouse Class
 *
 * Common APIs used by high level application to use this USB host class.
 * 
 * This API requires only callback definitions in conf_usb_host.h file
 * through following defines:
 * - \code  #define UHI_HID_MOUSE_CHANGE(dev,b_plug)
	#define UHI_HID_MOUSE_EVENT_BTN_LEFT(b_state)
	#define UHI_HID_MOUSE_EVENT_BTN_RIGHT(b_state)
	#define UHI_HID_MOUSE_EVENT_BTN_MIDDLE(b_state)
	#define UHI_HID_MOUSE_EVENT_MOUVE(x,y,scroll) \endcode
 *
 * See \ref uhi_hid_mouse_quickstart.
 * @{
 */
//@}


/**
 * \page uhi_hid_mouse_quickstart Quick start guide for USB host mouse module (UHI mouse)
 *
 * This is the quick start guide for the \ref uhi_hid_mouse_group 
 * "USB host mouse module (UHI mouse)" with step-by-step instructions on 
 * how to configure and use the modules in a selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 * 
 * \section uhi_hid_mouse_basic_use_case Basic use case
 * In this basic use case, the "USB Host HID Mouse (Single Class support)" module is used.
 * The "USB Host HID Mouse (Multiple Classes support)" module usage is described
 * in \ref uhi_hid_mouse_use_cases "Advanced use cases".
 *
 * \section uhi_hid_mouse_basic_use_case_setup Setup steps
 * \subsection uhi_hid_mouse_basic_use_case_setup_prereq Prerequisites
 * \copydetails uhc_basic_use_case_setup_prereq
 * \subsection uhi_hid_mouse_basic_use_case_setup_code Example code
 * \copydetails uhc_basic_use_case_setup_code
 * \subsection uhi_hid_mouse_basic_use_case_setup_flow Workflow
 * \copydetails uhc_basic_use_case_setup_flow
 *
 * \section uhi_hid_mouse_basic_use_case_usage Usage steps
 *
 * \subsection uhi_hid_mouse_basic_use_case_usage_code Example code
 * Content of conf_usb_host.h:
 * \code
	#define USB_HOST_UHI        UHI_HID_MOUSE
	#define UHI_HID_MOUSE_CHANGE(dev, b_plug) my_callback_mouse_change(dev, b_plug)
	extern bool my_callback_mouse_change(uhc_device_t* dev, bool b_plug);
	#define UHI_HID_MOUSE_EVENT_BTN_LEFT(b_state) my_callback_event_btn_left(b_state)
	extern void my_callback_event_btn_left(bool b_state);
	#define UHI_HID_MOUSE_EVENT_BTN_RIGHT(b_state) my_callback_event_btn_right(b_state)
	extern void my_callback_event_btn_right(bool b_state);
	#define UHI_HID_MOUSE_EVENT_BTN_MIDDLE(b_state) my_callback_event_btn_middle(b_state)
	extern void my_callback_event_btn_middle(bool b_state);
	#define UHI_HID_MOUSE_EVENT_MOUVE(x, y, scroll) my_callback_event_mouse(x, y, scroll)
	extern void my_callback_event_mouse(int8_t x, int8_t y, int8_t scroll);
	#include "uhi_hid_mouse.h" // At the end of conf_usb_host.h file
\endcode
 *
 * Add to application C-file:
 * \code
	 bool my_callback_mouse_change(uhc_device_t* dev, bool b_plug)
	 {
	    if (b_plug) {
	       my_display_on_mouse_icon();
	    } else {
	       my_display_off_mouse_icon();
	    }
	 }

	 void my_callback_event_btn_left(bool b_state)
	 {
	    if (b_state) {
	       // Here mouse button left pressed
	    } else {
	       // Here mouse button left released
	    }
	 }

	 void my_callback_event_mouse(int8_t x, int8_t y, int8_t scroll)
	 {
	    if (!x) {
	       // Here mouse are moved on axe X
	       cursor_x += x;
	    }
	    if (!y) {
	       // Here mouse are moved on axe Y
	       cursor_y += y;
	    }
	    if (!scroll) {
	       // Here mouse are moved the wheel
	       wheel += scroll;
	    }
	 }
\endcode
 *
 * \subsection uhi_hid_mouse_basic_use_case_setup_flow Workflow
 * -# Ensure that conf_usb_host.h is available and contains the following configuration
 * which is the USB host mouse configuration:
 *   - \code #define USB_HOST_UHI   UHI_HID_MOUSE \endcode
 *     \note It defines the list of UHI supported by USB host.
 *   - \code #define UHI_HID_MOUSE_CHANGE(dev, b_plug) my_callback_mouse_change(dev, b_plug)
	 extern bool my_callback_mouse_change(uhc_device_t* dev, bool b_plug); \endcode
 *     \note This callback is called when a USB device mouse is plugged or unplugged.
 *   - \code #define UHI_HID_MOUSE_EVENT_BTN_LEFT(b_state) my_callback_event_btn_left(b_state)
	extern void my_callback_event_btn_left(bool b_state);
	#define UHI_HID_MOUSE_EVENT_BTN_RIGHT(b_state) my_callback_event_btn_right(b_state)
	extern void my_callback_event_btn_right(bool b_state);
	#define UHI_HID_MOUSE_EVENT_BTN_MIDDLE(b_state) my_callback_event_btn_middle(b_state)
	extern void my_callback_event_btn_middle(bool b_state);
	#define UHI_HID_MOUSE_EVENT_MOUVE(x, y, scroll) my_callback_event_mouse(x, y, scroll)
	extern void my_callback_event_mouse(int8_t x, int8_t y, int8_t scroll) \endcode
 *     \note These callbacks are called when a USB device mouse event is received.
 *
 * \section uhi_hid_mouse_use_cases Advanced use cases
 * For more advanced use of the UHI HID mouse module, see the following use cases:
 * - \subpage uhc_use_case_1
 * - \subpage uhc_use_case_2
 * - \subpage uhc_use_case_3
 */


#ifdef __cplusplus
}
#endif
#endif // _UHI_HID_MOUSE_H_
