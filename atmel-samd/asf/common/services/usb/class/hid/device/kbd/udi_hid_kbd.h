/**
 * \file
 *
 * \brief USB Device Human Interface Device (HID) keyboard interface.
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

#ifndef _UDC_HID_KBD_H_
#define _UDC_HID_KBD_H_

#include "conf_usb.h"
#include "usb_protocol.h"
#include "usb_protocol_hid.h"
#include "udc_desc.h"
#include "udi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup udi_hid_keyboard_group_udc
 * @{
 */
//! Global structure which contains standard UDI API for UDC
extern UDC_DESC_STORAGE udi_api_t udi_api_hid_kbd;
//@}

/**
 * \ingroup udi_hid_keyboard_group
 * \defgroup udi_hid_keyboard_group_desc USB interface descriptors
 *
 * The following structures provide predefined USB interface descriptors.
 * It must be used to define the final USB descriptors.
 */
//@{

//! Interface descriptor structure for HID keyboard
typedef struct {
	usb_iface_desc_t iface;
	usb_hid_descriptor_t hid;
	usb_ep_desc_t ep;
} udi_hid_kbd_desc_t;

//! Report descriptor for HID keyboard
typedef struct {
	uint8_t array[59];
} udi_hid_kbd_report_desc_t;


//! By default no string associated to this interface
#ifndef UDI_HID_KBD_STRING_ID
#define UDI_HID_KBD_STRING_ID 0
#endif

//! HID keyboard endpoints size
#define UDI_HID_KBD_EP_SIZE  8

//! Content of HID keyboard interface descriptor for all speed
#define UDI_HID_KBD_DESC    {\
	.iface.bLength             = sizeof(usb_iface_desc_t),\
	.iface.bDescriptorType     = USB_DT_INTERFACE,\
	.iface.bInterfaceNumber    = UDI_HID_KBD_IFACE_NUMBER,\
	.iface.bAlternateSetting   = 0,\
	.iface.bNumEndpoints       = 1,\
	.iface.bInterfaceClass     = HID_CLASS,\
	.iface.bInterfaceSubClass  = HID_SUB_CLASS_NOBOOT,\
	.iface.bInterfaceProtocol  = HID_PROTOCOL_KEYBOARD,\
	.iface.iInterface          = UDI_HID_KBD_STRING_ID,\
	.hid.bLength               = sizeof(usb_hid_descriptor_t),\
	.hid.bDescriptorType       = USB_DT_HID,\
	.hid.bcdHID                = LE16(USB_HID_BDC_V1_11),\
	.hid.bCountryCode          = USB_HID_NO_COUNTRY_CODE,\
	.hid.bNumDescriptors       = USB_HID_NUM_DESC,\
	.hid.bRDescriptorType      = USB_DT_HID_REPORT,\
	.hid.wDescriptorLength     = LE16(sizeof(udi_hid_kbd_report_desc_t)),\
	.ep.bLength                = sizeof(usb_ep_desc_t),\
	.ep.bDescriptorType        = USB_DT_ENDPOINT,\
	.ep.bEndpointAddress       = UDI_HID_KBD_EP_IN,\
	.ep.bmAttributes           = USB_EP_TYPE_INTERRUPT,\
	.ep.wMaxPacketSize         = LE16(UDI_HID_KBD_EP_SIZE),\
	.ep.bInterval              = 2,\
	}
//@}


/**
 * \ingroup udi_hid_group
 * \defgroup udi_hid_keyboard_group USB Device Interface (UDI) for Human Interface Device (HID) Keyboard Class
 *
 * Common APIs used by high level application to use this USB class.
 *
 * See \ref udi_hid_keyboard_quickstart.
 * @{
 */

/**
 * \brief Send events key modifier released
 *
 * \param modifier_id   ID of key modifier
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
bool udi_hid_kbd_modifier_up(uint8_t modifier_id);

/**
 * \brief Send events key modifier pressed
 *
 * \param modifier_id   ID of key modifier
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
bool udi_hid_kbd_modifier_down(uint8_t modifier_id);


/**
 * \brief Send events key released
 *
 * \param key_id   ID of key
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
bool udi_hid_kbd_up(uint8_t key_id);

/**
 * \brief Send events key pressed
 *
 * \param key_id   ID of key
 *
 */
bool udi_hid_kbd_down(uint8_t key_id);

//@}

#ifdef __cplusplus
}
#endif

/**
 * \page udi_hid_keyboard_quickstart Quick start guide for USB device keyboard module (UDI keyboard)
 *
 * This is the quick start guide for the \ref udi_hid_keyboard_group 
 * "USB device keyboard module (UDI keyboard)" with step-by-step instructions on 
 * how to configure and use the modules in a selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 * 
 * Also, you can refer to application note
 * <A href="http://www.atmel.com/dyn/resources/prod_documents/doc8446.pdf">
 * AVR4904: ASF - USB Device HID Keyboard Application</A>.
 *
 * \section udi_hid_keyboard_basic_use_case Basic use case
 * In this basic use case, the "USB HID Keyboard (Single Interface Device)" module is used.
 * The "USB HID Keyboard (Composite Device)" module usage is described in \ref udi_hid_keyboard_use_cases
 * "Advanced use cases".
 *
 * \section udi_hid_keyboard_basic_use_case_setup Setup steps
 * \subsection udi_hid_keyboard_basic_use_case_setup_prereq Prerequisites
 * \copydetails udc_basic_use_case_setup_prereq
 * \subsection udi_hid_keyboard_basic_use_case_setup_code Example code
 * \copydetails udc_basic_use_case_setup_code
 * \subsection udi_hid_keyboard_basic_use_case_setup_flow Workflow
 * \copydetails udc_basic_use_case_setup_flow
 *
 * \section udi_hid_keyboard_basic_use_case_usage Usage steps
 *
 * \subsection udi_hid_keyboard_basic_use_case_usage_code Example code
 * Content of conf_usb.h:
 * \code
	#define UDI_HID_KBD_ENABLE_EXT() my_callback_keyboard_enable()
	extern bool my_callback_keyboard_enable(void);
	#define UDI_HID_KBD_DISABLE_EXT() my_callback_keyboard_disable()
	extern void my_callback_keyboard_disable(void);
	#include "udi_hid_keyboard_conf.h" // At the end of conf_usb.h file
\endcode
 *
 * Add to application C-file:
 * \code
	 static bool my_flag_autorize_keyboard_events = false;
	 bool my_callback_keyboard_enable(void)
	 {
	    my_flag_autorize_keyboard_events = true;
	    return true;
	 }
	 void my_callback_keyboard_disable(void)
	 {
	    my_flag_autorize_keyboard_events = false;
	 }

	 void my_key_A_press_event(void)
	 {
	    if (!my_flag_autorize_keyboard_events) {
	       return;
	    }
	    udi_hid_kbd_up(HID_A);
	 }
\endcode
 *
 * \subsection udi_hid_keyboard_basic_use_case_setup_flow Workflow
 * -# Ensure that conf_usb.h is available and contains the following configuration
 * which is the USB device keyboard configuration:
 *   - \code #define UDI_HID_KBD_ENABLE_EXT() my_callback_keyboard_enable()
	 extern bool my_callback_keyboard_enable(void); \endcode
 *     \note After the device enumeration (detecting and identifying USB devices),
 *     the USB host starts the device configuration. When the USB keyboard interface 
 *     from the device is accepted by the host, the USB host enables this interface and the
 *     UDI_HID_KBD_ENABLE_EXT() callback function is called and return true.
 *     Thus, it is recommended to enable sensors used by the keyboard in this function.
 *   - \code #define UDI_HID_KBD_DISABLE_EXT() my_callback_keyboard_disable()
	 extern void my_callback_keyboard_disable(void); \endcode
 *     \note When the USB device is unplugged or is reset by the USB host, the USB
 *     interface is disabled and the UDI_HID_KBD_DISABLE_EXT() callback function
 *     is called. Thus, it is recommended to disable sensors used by the keyboard
 *     in this function.
 * -# send keyboard events:
 *   - \code // Send events key modifier released
	udi_hid_kbd_modifier_up(uint8_t modifier_id);
	// Send events key modifier pressed
	udi_hid_kbd_modifier_down(uint8_t modifier_id);
	// Send events key released
	udi_hid_kbd_up(uint8_t key_id);
	// Send events key pressed
	udi_hid_kbd_down(uint8_t key_id); \endcode
 *
 * \section udi_hid_keyboard_use_cases Advanced use cases
 * For more advanced use of the UDI HID keyboard module, see the following use cases:
 * - \subpage udi_hid_keyboard_use_case_composite
 * - \subpage udc_use_case_1
 * - \subpage udc_use_case_2
 * - \subpage udc_use_case_3
 * - \subpage udc_use_case_5
 * - \subpage udc_use_case_6
 */

/**
 * \page udi_hid_keyboard_use_case_composite HID keyboard in a composite device
 *
 * A USB Composite Device is a USB Device which uses more than one USB class.
 * In this use case, the "USB HID Keyboard (Composite Device)" module is used to
 * create a USB composite device. Thus, this USB module can be associated with
 * another "Composite Device" module, like "USB MSC (Composite Device)".
 * 
 * Also, you can refer to application note
 * <A href="http://www.atmel.com/dyn/resources/prod_documents/doc8445.pdf">
 * AVR4902 ASF - USB Composite Device</A>.
 *
 * \section udi_hid_keyboard_use_case_composite_setup Setup steps
 * For the setup code of this use case to work, the
 * \ref udi_hid_keyboard_basic_use_case "basic use case" must be followed.
 *
 * \section udi_hid_keyboard_use_case_composite_usage Usage steps
 *
 * \subsection udi_hid_keyboard_use_case_composite_usage_code Example code
 * Content of conf_usb.h:
 * \code
	 #define USB_DEVICE_EP_CTRL_SIZE  64
	 #define USB_DEVICE_NB_INTERFACE (X+1)
	 #define USB_DEVICE_MAX_EP (X+1)

	 #define UDI_HID_KBD_EP_IN  (X | USB_EP_DIR_IN)
	 #define UDI_HID_KBD_IFACE_NUMBER  X

	 #define UDI_COMPOSITE_DESC_T \
	    udi_hid_kbd_desc_t udi_hid_kbd; \
	    ...
	 #define UDI_COMPOSITE_DESC_FS \
	    .udi_hid_kbd = UDI_HID_KBD_DESC, \
	    ...
	 #define UDI_COMPOSITE_DESC_HS \
	    .udi_hid_kbd = UDI_HID_KBD_DESC, \
	    ...
	 #define UDI_COMPOSITE_API \
	    &udi_api_hid_kbd, \
	    ...
\endcode
 *
 * \subsection udi_hid_keyboard_use_case_composite_usage_flow Workflow
 * -# Ensure that conf_usb.h is available and contains the following parameters
 * required for a USB composite device configuration:
 *   - \code // Endpoint control size, This must be:
	// - 8 for low speed
	// - 8, 16, 32 or 64 for full speed device (8 is recommended to save RAM)
	// - 64 for a high speed device
	#define USB_DEVICE_EP_CTRL_SIZE  64
	// Total Number of interfaces on this USB device.
	// Add 1 for HID keyboard.
	#define USB_DEVICE_NB_INTERFACE (X+1)
	// Total number of endpoints on this USB device.
	// This must include each endpoint for each interface.
	// Add 1 for HID keyboard.
	#define USB_DEVICE_MAX_EP (X+1) \endcode
 * -# Ensure that conf_usb.h contains the description of
 * composite device:
 *   - \code // The endpoint number chosen by you for the keyboard.
	// The endpoint number starting from 1.
	#define UDI_HID_KBD_EP_IN  (X | USB_EP_DIR_IN)
	// The interface index of an interface starting from 0
	#define UDI_HID_KBD_IFACE_NUMBER  X \endcode
 * -# Ensure that conf_usb.h contains the following parameters
 * required for a USB composite device configuration:
 *   - \code // USB Interfaces descriptor structure
	#define UDI_COMPOSITE_DESC_T \
	   ...
	   udi_hid_kbd_desc_t udi_hid_kbd; \
	   ...
	// USB Interfaces descriptor value for Full Speed
	#define UDI_COMPOSITE_DESC_FS \
	   ...
	   .udi_hid_kbd = UDI_HID_KBD_DESC, \
	   ...
	// USB Interfaces descriptor value for High Speed
	#define UDI_COMPOSITE_DESC_HS \
	   ...
	   .udi_hid_kbd = UDI_HID_KBD_DESC, \
	   ...
	// USB Interface APIs
	#define UDI_COMPOSITE_API \
	   ...
	   &udi_api_hid_kbd, \
	   ... \endcode
 *   - \note The descriptors order given in the four lists above must be the
 *     same as the order defined by all interface indexes. The interface index
 *     orders are defined through UDI_X_IFACE_NUMBER defines.
 */

#endif // _UDC_HID_KBD_H_
