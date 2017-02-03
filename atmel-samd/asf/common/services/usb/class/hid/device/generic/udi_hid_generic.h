/**
 * \file
 *
 * \brief USB Device Human Interface Device (HID) generic interface.
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

#ifndef _UDI_HID_GENERIC_H_
#define _UDI_HID_GENERIC_H_

#include "conf_usb.h"
#include "usb_protocol.h"
#include "usb_protocol_hid.h"
#include "udc_desc.h"
#include "udi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup udi_hid_generic_group_udc
 * @{
 */
//! Global structure which contains standard UDI API for UDC
extern UDC_DESC_STORAGE udi_api_t udi_api_hid_generic;
//@}

/**
 * \ingroup udi_hid_generic_group
 * \defgroup udi_hid_generic_group_desc USB interface descriptors
 *
 * The following structures provide predefined USB interface descriptors.
 * It must be used to define the final USB descriptors.
 */
//@{

//! Interface descriptor structure for HID generic
typedef struct {
	usb_iface_desc_t iface;
	usb_hid_descriptor_t hid;
	usb_ep_desc_t ep_in;
	usb_ep_desc_t ep_out;
} udi_hid_generic_desc_t;

//! Report descriptor for HID generic
typedef struct {
	uint8_t array[53];
} udi_hid_generic_report_desc_t;


//! By default no string associated to this interface
#ifndef UDI_HID_GENERIC_STRING_ID
#define UDI_HID_GENERIC_STRING_ID 0
#endif



//! Content of HID generic interface descriptor for all speed
#define UDI_HID_GENERIC_DESC    {\
   .iface.bLength             = sizeof(usb_iface_desc_t),\
   .iface.bDescriptorType     = USB_DT_INTERFACE,\
   .iface.bInterfaceNumber    = UDI_HID_GENERIC_IFACE_NUMBER,\
   .iface.bAlternateSetting   = 0,\
   .iface.bNumEndpoints       = 2,\
   .iface.bInterfaceClass     = HID_CLASS,\
   .iface.bInterfaceSubClass  = HID_SUB_CLASS_NOBOOT,\
   .iface.bInterfaceProtocol  = HID_PROTOCOL_GENERIC,\
   .iface.iInterface          = UDI_HID_GENERIC_STRING_ID,\
   .hid.bLength               = sizeof(usb_hid_descriptor_t),\
   .hid.bDescriptorType       = USB_DT_HID,\
   .hid.bcdHID                = LE16(USB_HID_BDC_V1_11),\
   .hid.bCountryCode          = USB_HID_NO_COUNTRY_CODE,\
   .hid.bNumDescriptors       = USB_HID_NUM_DESC,\
   .hid.bRDescriptorType      = USB_DT_HID_REPORT,\
   .hid.wDescriptorLength     = LE16(sizeof(udi_hid_generic_report_desc_t)),\
   .ep_in.bLength             = sizeof(usb_ep_desc_t),\
   .ep_in.bDescriptorType     = USB_DT_ENDPOINT,\
   .ep_in.bEndpointAddress    = UDI_HID_GENERIC_EP_IN,\
   .ep_in.bmAttributes        = USB_EP_TYPE_INTERRUPT,\
   .ep_in.wMaxPacketSize      = LE16(UDI_HID_GENERIC_EP_SIZE),\
   .ep_in.bInterval           = 4,\
   .ep_out.bLength            = sizeof(usb_ep_desc_t),\
   .ep_out.bDescriptorType    = USB_DT_ENDPOINT,\
   .ep_out.bEndpointAddress   = UDI_HID_GENERIC_EP_OUT,\
   .ep_out.bmAttributes       = USB_EP_TYPE_INTERRUPT,\
   .ep_out.wMaxPacketSize     = LE16(UDI_HID_GENERIC_EP_SIZE),\
   .ep_out.bInterval          = 4,\
   }
//@}


/**
 * \ingroup udi_hid_group
 * \defgroup udi_hid_generic_group USB Device Interface (UDI) for Human Interface Device (HID) Generic Class
 *
 * Common APIs used by high level application to use this USB class.
 *
 * See \ref udi_hid_generic_quickstart.
 * @{
 */

/**
 * \brief Routine used to send a report to USB Host
 *
 * \param data     Pointer on the report to send (size = UDI_HID_REPORT_IN_SIZE)
 * 
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
bool udi_hid_generic_send_report_in(uint8_t *data);

//@}


#ifdef __cplusplus
}
#endif


/**
 * \page udi_hid_generic_quickstart Quick start guide for USB device generic module (UDI generic)
 *
 * This is the quick start guide for the \ref udi_hid_generic_group 
 * "USB device generic module (UDI generic)" with step-by-step instructions on 
 * how to configure and use the modules in a selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 * 
 * Also, you can refer to application note
 * <A href="http://www.atmel.com/dyn/resources/prod_documents/doc8499.pdf">
 * AVR4905: ASF - USB Device HID Generic Application</A>.
 *
 * \section udi_hid_generic_basic_use_case Basic use case
 * In this basic use case, the "USB HID Generic (Single Interface Device)" module is used.
 * The "USB HID Generic (Composite Device)" module usage is described in \ref udi_hid_generic_use_cases
 * "Advanced use cases".
 *
 * \section udi_hid_generic_basic_use_case_setup Setup steps
 * \subsection udi_hid_generic_basic_use_case_setup_prereq Prerequisites
 * \copydetails udc_basic_use_case_setup_prereq
 * \subsection udi_hid_generic_basic_use_case_setup_code Example code
 * \copydetails udc_basic_use_case_setup_code
 * \subsection udi_hid_generic_basic_use_case_setup_flow Workflow
 * \copydetails udc_basic_use_case_setup_flow
 *
 * \section udi_hid_generic_basic_use_case_usage Usage steps
 *
 * \subsection udi_hid_generic_basic_use_case_usage_code Example code
 * Content of conf_usb.h:
 * \code
	 #define UDI_HID_GENERIC_ENABLE_EXT() my_callback_generic_enable()
	 extern bool my_callback_generic_enable(void);
	 #define UDI_HID_GENERIC_DISABLE_EXT() my_callback_generic_disable()
	 extern void my_callback_generic_disable(void);
	 #define  UDI_HID_GENERIC_REPORT_OUT(ptr) my_callback_generic_report_out(ptr)
	 extern void my_callback_generic_report_out(uint8_t *report);
	 #define  UDI_HID_GENERIC_SET_FEATURE(f) my_callback_generic_set_feature(f)
	 extern void my_callback_generic_set_feature(uint8_t *report_feature);

	 #define  UDI_HID_REPORT_IN_SIZE             64
	 #define  UDI_HID_REPORT_OUT_SIZE            64
	 #define  UDI_HID_REPORT_FEATURE_SIZE        4
	 #define  UDI_HID_GENERIC_EP_SIZE            64

	 #include "udi_hid_generic_conf.h" // At the end of conf_usb.h file
\endcode
 *
 * Add to application C-file:
 * \code
	 static bool my_flag_autorize_generic_events = false;
	 bool my_callback_generic_enable(void)
	 {
	    my_flag_autorize_generic_events = true;
	    return true;
	 }
	 void my_callback_generic_disable(void)
	 {
	    my_flag_autorize_generic_events = false;
	 }

	 void my_button_press_event(void)
	 {
	    if (!my_flag_autorize_generic_events) {
	       return;
	    }
	    uint8_t report[] = {0x00,0x01,0x02...};
	    udi_hid_generic_send_report_in(report);
	 }

	 void my_callback_generic_report_out(uint8_t *report)
	 {
	    if ((report[0] == MY_VALUE_0)
	        (report[1] == MY_VALUE_1)) {
	       // The report is correct
	    }
	 }

	 void my_callback_generic_set_feature(uint8_t *report_feature)
	 {
	    if ((report_feature[0] == MY_VALUE_0)
	        (report_feature[1] == MY_VALUE_1)) {
	       // The report feature is correct
	    }
	 }
\endcode
 *
 * \subsection udi_hid_generic_basic_use_case_setup_flow Workflow
 * -# Ensure that conf_usb.h is available and contains the following configuration
 * which is the USB device generic configuration:
 *   - \code #define UDI_HID_GENERIC_ENABLE_EXT() my_callback_generic_enable()
	 extern bool my_callback_generic_enable(void); \endcode
 *     \note After the device enumeration (detecting and identifying USB devices),
 *     the USB host starts the device configuration. When the USB generic interface 
 *     from the device is accepted by the host, the USB host enables this interface and the
 *     UDI_HID_GENERIC_ENABLE_EXT() callback function is called and return true.
 *     Thus, it is recommended to enable sensors used by the generic in this function.
 *   - \code #define UDI_HID_GENERIC_DISABLE_EXT() my_callback_generic_disable()
	 extern void my_callback_generic_disable(void); \endcode
 *     \note When the USB device is unplugged or is reset by the USB host, the USB
 *     interface is disabled and the UDI_HID_GENERIC_DISABLE_EXT() callback function
 *     is called. Thus, it is recommended to disable sensors used by the HID generic
 *     interface in this function.
 *   - \code #define  UDI_HID_GENERIC_REPORT_OUT(ptr) my_callback_generic_report_out(ptr)
	 extern void my_callback_generic_report_out(uint8_t *report); \endcode
 *     \note Callback used to receive the OUT report.
 *   - \code #define  UDI_HID_GENERIC_SET_FEATURE(f) my_callback_generic_set_feature(f)
	 extern void my_callback_generic_set_feature(uint8_t *report_feature); \endcode
 *     \note Callback used to receive the SET FEATURE report.
 *   - \code #define  UDI_HID_REPORT_IN_SIZE             64
	#define  UDI_HID_REPORT_OUT_SIZE            64
	#define  UDI_HID_REPORT_FEATURE_SIZE        4 \endcode
 *     \note The report size are defined by the final application.
 *   - \code #define  UDI_HID_GENERIC_EP_SIZE 64 \endcode
 *     \note The interrupt endpoint size is defined by the final application.

 * -# Send a IN report:
 *   - \code uint8_t report[] = {0x00,0x01,0x02...};
	   udi_hid_generic_send_report_in(report); \endcode
 *
 * \section udi_hid_generic_use_cases Advanced use cases
 * For more advanced use of the UDI HID generic module, see the following use cases:
 * - \subpage udi_hid_generic_use_case_composite
 * - \subpage udc_use_case_1
 * - \subpage udc_use_case_2
 * - \subpage udc_use_case_3
 * - \subpage udc_use_case_5
 * - \subpage udc_use_case_6
 */

/**
 * \page udi_hid_generic_use_case_composite HID generic in a composite device
 *
 * A USB Composite Device is a USB Device which uses more than one USB class.
 * In this use case, the "USB HID Generic (Composite Device)" module is used to
 * create a USB composite device. Thus, this USB module can be associated with
 * another "Composite Device" module, like "USB MSC (Composite Device)".
 * 
 * Also, you can refer to application note
 * <A href="http://www.atmel.com/dyn/resources/prod_documents/doc8445.pdf">
 * AVR4902 ASF - USB Composite Device</A>.
 *
 * \section udi_hid_generic_use_case_composite_setup Setup steps
 * For the setup code of this use case to work, the
 * \ref udi_hid_generic_basic_use_case "basic use case" must be followed.
 *
 * \section udi_hid_generic_use_case_composite_usage Usage steps
 *
 * \subsection udi_hid_generic_use_case_composite_usage_code Example code
 * Content of conf_usb.h:
 * \code
	 #define USB_DEVICE_EP_CTRL_SIZE  64
	 #define USB_DEVICE_NB_INTERFACE (X+1)
	 #define USB_DEVICE_MAX_EP (X+2)

	 #define UDI_HID_GENERIC_EP_IN    (1 | USB_EP_DIR_IN)
	 #define UDI_HID_GENERIC_EP_OUT   (2 | USB_EP_DIR_OUT)
	 #define UDI_HID_GENERIC_IFACE_NUMBER  X

	 #define UDI_COMPOSITE_DESC_T \
	    udi_hid_generic_desc_t udi_hid_generic; \
	    ...
	 #define UDI_COMPOSITE_DESC_FS \
	    .udi_hid_generic = UDI_HID_GENERIC_DESC, \
	    ...
	 #define UDI_COMPOSITE_DESC_HS \
	    .udi_hid_generic = UDI_HID_GENERIC_DESC, \
	    ...
	 #define UDI_COMPOSITE_API \
	    &udi_api_hid_generic, \
	    ...
\endcode
 *
 * \subsection udi_hid_generic_use_case_composite_usage_flow Workflow
 * -# Ensure that conf_usb.h is available and contains the following parameters
 * required for a USB composite device configuration:
 *   - \code // Endpoint control size, This must be:
	// - 8 for low speed
	// - 8, 16, 32 or 64 for full speed device (8 is recommended to save RAM)
	// - 64 for a high speed device
	#define USB_DEVICE_EP_CTRL_SIZE  64
	// Total Number of interfaces on this USB device.
	// Add 1 for HID generic.
	#define USB_DEVICE_NB_INTERFACE (X+1)
	// Total number of endpoints on this USB device.
	// This must include each endpoint for each interface.
	// Add 1 for HID generic.
	#define USB_DEVICE_MAX_EP (X+2) \endcode
 * -# Ensure that conf_usb.h contains the description of
 * composite device:
 *   - \code // The endpoint number chosen by you for the generic.
	// The endpoint number starting from 1.
	#define UDI_HID_GENERIC_EP_IN    (1 | USB_EP_DIR_IN)
	#define UDI_HID_GENERIC_EP_OUT   (2 | USB_EP_DIR_OUT)
	// The interface index of an interface starting from 0
	#define UDI_HID_GENERIC_IFACE_NUMBER  X \endcode
 * -# Ensure that conf_usb.h contains the following parameters
 * required for a USB composite device configuration:
 *   - \code // USB Interfaces descriptor structure
	#define UDI_COMPOSITE_DESC_T \
	   ...
	   udi_hid_generic_desc_t udi_hid_generic; \
	   ...
	// USB Interfaces descriptor value for Full Speed
	#define UDI_COMPOSITE_DESC_FS \
	   ...
	   .udi_hid_generic = UDI_HID_GENERIC_DESC, \
	   ...
	// USB Interfaces descriptor value for High Speed
	#define UDI_COMPOSITE_DESC_HS \
	   ...
	   .udi_hid_generic = UDI_HID_GENERIC_DESC, \
	   ...
	// USB Interface APIs
	#define UDI_COMPOSITE_API \
	   ...
	   &udi_api_hid_generic, \
	   ... \endcode
 *   - \note The descriptors order given in the four lists above must be the
 *     same as the order defined by all interface indexes. The interface index
 *     orders are defined through UDI_X_IFACE_NUMBER defines.
 */

#endif // _UDI_HID_GENERIC_H_
