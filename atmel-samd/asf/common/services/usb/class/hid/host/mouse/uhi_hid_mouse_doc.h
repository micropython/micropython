/**
 * \file
 *
 * \brief USB host driver for Human Interface Device (HID) mouse interface.
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

/**
 * \defgroup asfdoc_uhi_hid_mouse_group USB Host Interface (UHI) for Human Interface Device Mouse (HID Mouse)
 *
 * USB Host Interface (UHI) for Human Interface Device Mouse (HID Mouse) provides an
 * interface for the configuration and management of USB HID mouse host.
 *
 * The outline of this documentation is as follows:
 * - \ref asfdoc_uhi_hid_mouse_api_overview
 * - \ref asfdoc_uhi_hid_mouse_exqsg
 * - \ref asfdoc_uhi_hid_mouse_config_examples
 *
 * For more details for Atmel&reg; Software Framework (ASF) USB Host Stack,
 * refer to following application note:
 * - <a href="http://www.atmel.com/dyn/resources/prod_documents/doc8486.pdf">
 *   AVR4950: ASF - USB Host Stack</a>
 *
 * \section asfdoc_uhi_hid_mouse_api_overview API Overview
 * @{
 */

/**
 * \name Interface with USB Host Core (UHC)
 *
 * Define and functions required by UHC.
 *
 * @{
 */

/** Global define which contains standard UHI API for UHC.
 *
 *  It must be added in USB_HOST_UHI define from conf_usb_host.h file. */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#define UHI_HID_MOUSE { \
	.install = uhi_hid_mouse_install, \
	.enable = uhi_hid_mouse_enable, \
	.uninstall = uhi_hid_mouse_uninstall, \
	.sof_notify = NULL, \
}
/**@}*/

/**
 * \name Functions Required by UHC
 * @{
 */

/**
 * \brief Install interface
 * Allocate interface endpoints if supported.
 *
 * \param[in] uhc_device_t	  Device to request
 *
 * \return Status of the install.
 */
extern uhc_enum_status_t uhi_hid_mouse_install(uhc_device_t* dev);

/**
 * \brief Enable the interface.
 *
 * Enable a USB interface corresponding to UHI.
 *
 * \param[in] uhc_device_t	  Device to request
 */
extern void uhi_hid_mouse_enable(uhc_device_t* dev);

/**
 * \brief Uninstall the interface (if installed).
 *
 * \param[in] uhc_device_t	  Device to request
 */
extern void uhi_hid_mouse_uninstall(uhc_device_t* dev);
/**@}*/


/**
 * \name UHI for Human Interface Device Mouse Class
 *
 * Common APIs used by high level application to use this USB host class.
 *
 * These APIs require only callback definitions in conf_usb_host.h file
 * through following defines:
  * @{
 */
#define UHI_HID_MOUSE_CHANGE(dev,b_plug)
#define UHI_HID_MOUSE_EVENT_BTN_LEFT(b_state)
#define UHI_HID_MOUSE_EVENT_BTN_RIGHT(b_state)
#define UHI_HID_MOUSE_EVENT_BTN_MIDDLE(b_state)
#define UHI_HID_MOUSE_EVENT_MOUVE(x,y,scroll)

/**@}*/

/**@}*/

/**
 * \page asfdoc_uhi_hid_mouse_exqsg Quick Start Guide for USB Host Mouse Module (UHI Mouse)
 *
 * This is the quick start guide for the \ref asfdoc_uhi_hid_mouse_group
 * "USB Host Mouse Module (UHI Mouse)" with step-by-step instructions on
 * how to configure and use the modules in a selection of use cases.
 *
 * The use cases highlights several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 *
 * \section uhi_hid_mouse_basic_use_case Basic Use Case
 * In this basic use case, the "USB Host HID Mouse (Single Class support)" module is used.
 * The "USB Host HID Mouse (Multiple Classes support)" module usage is described
 * in \ref uhi_hid_mouse_use_cases "Advanced Use Cases".
 *
 * \subsection uhi_hid_mouse_basic_use_case_setup Setup Steps
 * As a USB host, it follows common USB host setup steps. Refer to
 * \ref asfdoc_uhc_basic_use_case_setup "USB Host Basic Setup".

 *
 * \subsection uhi_hid_mouse_basic_use_case_usage Usage Steps
 *
 * \subsubsection uhi_hid_mouse_basic_use_case_usage_code Example Code
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
 *
 * \subsubsection uhi_hid_mouse_basic_use_case_setup_flow Workflow
 * -# Ensure that conf_usb_host.h is available and contains the following configuration
 * which is the USB host mouse configuration:
 *
 * \code
 #define USB_HOST_UHI   UHI_HID_MOUSE
 \endcode
 * \note It defines the list of UHI supported by USB host.
 *
 * \code
 #define UHI_HID_MOUSE_CHANGE(dev, b_plug) my_callback_mouse_change(dev, b_plug)
 extern bool my_callback_mouse_change(uhc_device_t* dev, bool b_plug);
 \endcode
 * \note This callback is called when a USB device mouse is plugged or unplugged.
 *
 * \code
 #define UHI_HID_MOUSE_EVENT_BTN_LEFT(b_state) my_callback_event_btn_left(b_state)
 extern void my_callback_event_btn_left(bool b_state);
 #define UHI_HID_MOUSE_EVENT_BTN_RIGHT(b_state) my_callback_event_btn_right(b_state)
 extern void my_callback_event_btn_right(bool b_state);
 #define UHI_HID_MOUSE_EVENT_BTN_MIDDLE(b_state) my_callback_event_btn_middle(b_state)
 extern void my_callback_event_btn_middle(bool b_state);
 #define UHI_HID_MOUSE_EVENT_MOUVE(x, y, scroll) my_callback_event_mouse(x, y, scroll)
 extern void my_callback_event_mouse(int8_t x, int8_t y, int8_t scroll)
 \endcode
 * \note These callbacks are called when a USB device mouse event is received.
 *
 * \section uhi_hid_mouse_use_cases Advanced Use Cases
 * \ifnot ASF_MANUAL
 * For more advanced use of the UHI HID mouse module, see the following use cases:
 * - \subpage uhc_use_case_1
 * - \subpage uhc_use_case_2
 * - \subpage uhc_use_case_3
 * \else
 * For more advanced use of the UHI HID mouse module, see the following:
 * - \ref asfdoc_uhc_use_cases
 * \endif
 */


/**
 * \page asfdoc_uhi_hid_mouse_config_examples Configuration File Examples
 *
 * \section asfdoc_uhi_hid_mouse_config_examples_1 conf_usb_host.h
 * \subsection asfdoc_uhi_hid_mouse_config_examples_1_1  UHI HID MOUSE Single
 * \include module_config/conf_usb_host.h
 * \subsection asfdoc_uhi_hid_mouse_config_examples_1_2  UHI HID MOUSE Multiple (Composite)
 * \include composite/host/module_config/conf_usb_host.h
 *
 * \section asfdoc_uhi_hid_mouse_config_examples_2 conf_clock.h
 *
 * \subsection asfdoc_uhi_hid_mouse_config_examples_2_1 AT32UC3A0, AT32UC3A1, AT32UC3B Devices (USBB)
 * \include example/at32uc3a0512_evk1100/conf_clock.h
 *
 * \subsection asfdoc_uhi_hid_mouse_config_examples_2_2 AT32UC3A3 and AT32UC3A4 Devices (USBB with High Speed Support)
 * \include example/at32uc3a3256_evk1104/conf_clock.h
 *
 * \subsection asfdoc_uhi_hid_mouse_config_examples_2_3 AT32UC3C, ATUCXXD, ATUCXXL3U, ATUCXXL4U Devices (USBC)
 * \include example/at32uc3c0512c_uc3c_ek/conf_clock.h
 *
 * \subsection asfdoc_uhi_hid_mouse_config_examples_2_4 SAM3X and SAM3A Devices (UOTGHS: USB OTG High Speed)
 * \include example/sam3x8h_sam3x_ek/conf_clock.h
 *
 * \section asfdoc_uhi_hid_mouse_config_examples_3 conf_clocks.h
 *
 * \subsection asfdoc_uhi_hid_mouse_config_examples_3_1 SAM D21 Devices (USB)
 * \include example/samd21j18a_samd21_xplained_pro/conf_clocks.h
 *
 * \section asfdoc_uhi_hid_mouse_config_examples_4 conf_board.h
 *
 * \subsection asfdoc_uhi_hid_mouse_config_examples_4_1 AT32UC3A0, AT32UC3A1, AT32UC3B Devices (USBB)
 * \include example/at32uc3a0512_evk1100/conf_board.h
 *
 * \subsection asfdoc_uhi_hid_mouse_config_examples_4_2 AT32UC3A3 and AT32UC3A4 Devices (USBB with High Speed Support)
 * \include example/at32uc3a3256_evk1104/conf_board.h
 *
 * \subsection asfdoc_uhi_hid_mouse_config_examples_4_3 AT32UC3C, ATUCXXD, ATUCXXL3U, ATUCXXL4U Devices (USBC)
 * \include example/at32uc3c0512c_uc3c_ek/conf_board.h
 *
 * \subsection asfdoc_uhi_hid_mouse_config_examples_4_4 SAM3X and SAM3A Devices (UOTGHS: USB OTG High Speed)
 * \include example/sam3x8h_sam3x_ek/conf_board.h
 *
 * \subsection asfdoc_uhi_hid_mouse_config_examples_4_5 SAM D21 Devices (USB)
 * \include example/samd21j18a_samd21_xplained_pro/conf_board.h
 */

/**
 * \page asfdoc_uhi_hid_mouse_document_revision_history Document Revision History
 *
 * <table>
 * <tr>
 *    <th>Doc. Rev.</th>
 *    <th>Date</th>
 *    <th>Comments</th>
 * </tr>
 * <tr>
 *    <td>42342B</td>
 *    <td>12/2015</td>
 *    <td>Fixed typos</td>
 * </tr>
 * <tr>
 *    <td>42342A</td>
 *    <td>12/2014</td>
 *    <td>Initial release</td>
 * </tr>
 * </table>
 */

