/**
 * \file
 *
 * \brief USB Device Mass Storage Class (MSC) interface definitions.
 *
 * Copyright (c) 2009-2016 Atmel Corporation. All rights reserved.
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

#ifndef _UDI_MSC_H_
#define _UDI_MSC_H_

#include "conf_usb.h"
#include "usb_protocol.h"
#include "usb_protocol_msc.h"
#include "udd.h"
#include "udc_desc.h"
#include "udi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup udi_msc_group_udc
 * @{
 */
//! Global structure which contains standard UDI interface for UDC
extern UDC_DESC_STORAGE udi_api_t udi_api_msc;
//@}

/**
 * \ingroup udi_msc_group
 * \defgroup udi_msc_group USB interface descriptors
 *
 * The following structures provide predefined USB interface descriptors.
 * It must be used to define the final USB descriptors.
 */
//@{

//! Interface descriptor structure for MSC
typedef struct {
	usb_iface_desc_t iface;
	usb_ep_desc_t ep_in;
	usb_ep_desc_t ep_out;
} udi_msc_desc_t;

//! By default no string associated to this interface
#ifndef UDI_MSC_STRING_ID
#define UDI_MSC_STRING_ID     0
#endif

//! MSC endpoints size for full speed
#define UDI_MSC_EPS_SIZE_FS   64
//! MSC endpoints size for high speed
#define UDI_MSC_EPS_SIZE_HS   512

//! Content of MSC interface descriptor for all speeds
#define UDI_MSC_DESC      \
   .iface.bLength             = sizeof(usb_iface_desc_t),\
   .iface.bDescriptorType     = USB_DT_INTERFACE,\
   .iface.bInterfaceNumber    = UDI_MSC_IFACE_NUMBER,\
   .iface.bAlternateSetting   = 0,\
   .iface.bNumEndpoints       = 2,\
   .iface.bInterfaceClass     = MSC_CLASS,\
   .iface.bInterfaceSubClass  = MSC_SUBCLASS_TRANSPARENT,\
   .iface.bInterfaceProtocol  = MSC_PROTOCOL_BULK,\
   .iface.iInterface          = UDI_MSC_STRING_ID,\
   .ep_in.bLength             = sizeof(usb_ep_desc_t),\
   .ep_in.bDescriptorType     = USB_DT_ENDPOINT,\
   .ep_in.bEndpointAddress    = UDI_MSC_EP_IN,\
   .ep_in.bmAttributes        = USB_EP_TYPE_BULK,\
   .ep_in.bInterval           = 0,\
   .ep_out.bLength            = sizeof(usb_ep_desc_t),\
   .ep_out.bDescriptorType    = USB_DT_ENDPOINT,\
   .ep_out.bEndpointAddress   = UDI_MSC_EP_OUT,\
   .ep_out.bmAttributes       = USB_EP_TYPE_BULK,\
   .ep_out.bInterval          = 0,

//! Content of MSC interface descriptor for full speed only
#define UDI_MSC_DESC_FS   {\
   UDI_MSC_DESC \
   .ep_in.wMaxPacketSize      = LE16(UDI_MSC_EPS_SIZE_FS),\
   .ep_out.wMaxPacketSize     = LE16(UDI_MSC_EPS_SIZE_FS),\
   }

//! Content of MSC interface descriptor for high speed only
#define UDI_MSC_DESC_HS   {\
   UDI_MSC_DESC \
   .ep_in.wMaxPacketSize      = LE16(UDI_MSC_EPS_SIZE_HS),\
   .ep_out.wMaxPacketSize     = LE16(UDI_MSC_EPS_SIZE_HS),\
   }
//@}


/**
 * \ingroup udi_group
 * \defgroup udi_msc_group USB Device Interface (UDI) for Mass Storage Class (MSC)
 *
 * Common APIs used by high level application to use this USB class.
 *
 * These routines are used by memory to transfer its data
 * to/from USB MSC endpoints.
 *
 * See \ref udi_msc_quickstart.
 * @{
 */

/**
 * \brief Process the background read/write commands
 *
 * Routine called by the main loop
 */
bool udi_msc_process_trans(void);

/**
 * \brief Transfers data to/from USB MSC endpoints
 *
 *
 * \param b_read        Memory to USB, if true
 * \param block         Buffer on Internal RAM to send or fill
 * \param block_size    Buffer size to send or fill
 * \param callback      Function to call at the end of transfer.
 *                      If NULL then the routine exit when transfer is finish.
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
bool udi_msc_trans_block(bool b_read, uint8_t * block, iram_size_t block_size,
		void (*callback) (udd_ep_status_t status, iram_size_t n, udd_ep_id_t ep));
//@}

#ifdef __cplusplus
}
#endif


/**
 * \page udi_msc_quickstart Quick start guide for USB device Mass Storage module (UDI MSC)
 *
 * This is the quick start guide for the \ref udi_msc_group 
 * "USB device interface MSC module (UDI MSC)" with step-by-step instructions on 
 * how to configure and use the modules in a selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 * 
 * \section udi_msc_basic_use_case Basic use case
 * In this basic use case, the "USB MSC (Single Interface Device)" module is used.
 * The "USB MSC (Composite Device)" module usage is described in \ref udi_msc_use_cases
 * "Advanced use cases".
 *
 * \section udi_msc_basic_use_case_setup Setup steps
 * \subsection udi_msc_basic_use_case_setup_prereq Prerequisites
 * \copydetails udc_basic_use_case_setup_prereq
 * \subsection udi_msc_basic_use_case_setup_code Example code
 * \copydetails udc_basic_use_case_setup_code
 * \subsection udi_msc_basic_use_case_setup_flow Workflow
 * \copydetails udc_basic_use_case_setup_flow
 *
 * \section udi_msc_basic_use_case_usage Usage steps
 *
 * \subsection udi_msc_basic_use_case_usage_code Example code
 * Content of conf_usb.h:
 * \code
	#define  USB_DEVICE_SERIAL_NAME  "12...EF" // Disk SN for MSC
	#define UDI_MSC_GLOBAL_VENDOR_ID \
	   'A', 'T', 'M', 'E', 'L', ' ', ' ', ' '
	#define UDI_MSC_GLOBAL_PRODUCT_VERSION \
	   '1', '.', '0', '0'
	#define UDI_MSC_ENABLE_EXT() my_callback_msc_enable()
	extern bool my_callback_msc_enable(void);
	#define UDI_MSC_DISABLE_EXT() my_callback_msc_disable()
	extern void my_callback_msc_disable(void);
	#include "udi_msc_conf.h" // At the end of conf_usb.h file
\endcode
 *
 * Add to application C-file:
 * \code
	 static bool my_flag_autorize_msc_transfert = false;
	 bool my_callback_msc_enable(void)
	 {
	    my_flag_autorize_msc_transfert = true;
	    return true;
	 }
	 void my_callback_msc_disable(void)
	 {
	    my_flag_autorize_msc_transfert = false;
	 }

	 void task(void)
	 {
	    udi_msc_process_trans();
	 }
\endcode
 *
 * \subsection udi_msc_basic_use_case_setup_flow Workflow
 * -# Ensure that conf_usb.h is available and contains the following configuration,
 * which is the USB device MSC configuration:
 *   - \code #define USB_DEVICE_SERIAL_NAME  "12...EF" // Disk SN for MSC \endcode
 *     \note The USB serial number is mandatory when a MSC interface is used.
 *   - \code //! Vendor name and Product version of MSC interface
	#define UDI_MSC_GLOBAL_VENDOR_ID \
	   'A', 'T', 'M', 'E', 'L', ' ', ' ', ' '
	#define UDI_MSC_GLOBAL_PRODUCT_VERSION \
	   '1', '.', '0', '0' \endcode
 *     \note The USB MSC interface requires a vendor ID (8 ASCII characters)
 *     and a product version (4 ASCII characters).
 *   - \code #define UDI_MSC_ENABLE_EXT() my_callback_msc_enable()
	extern bool my_callback_msc_enable(void); \endcode
 *     \note After the device enumeration (detecting and identifying USB devices),
 *     the USB host starts the device configuration. When the USB MSC interface 
 *     from the device is accepted by the host, the USB host enables this interface and the
 *     UDI_MSC_ENABLE_EXT() callback function is called and return true.
 *     Thus, when this event is received, the tasks which call
 *     udi_msc_process_trans() must be enabled.
 *   - \code #define UDI_MSC_DISABLE_EXT() my_callback_msc_disable()
	extern void my_callback_msc_disable(void); \endcode
 *     \note When the USB device is unplugged or is reset by the USB host, the USB
 *     interface is disabled and the UDI_MSC_DISABLE_EXT() callback function
 *     is called. Thus, it is recommended to disable the task which is called udi_msc_process_trans().
 * -# The MSC is automatically linked with memory control access component 
 * which provides the memories interfaces. However, the memory data transfers
 * must be done outside USB interrupt routine. This is done in the MSC process
 * ("udi_msc_process_trans()") called by main loop:
 *   - \code  * void task(void) {
	udi_msc_process_trans();
	} \endcode
 * -# The MSC speed depends on task periodicity. To get the best speed
 * the notification callback "UDI_MSC_NOTIFY_TRANS_EXT" can be used to wakeup
 * this task (Example, through a mutex):
 *   - \code #define  UDI_MSC_NOTIFY_TRANS_EXT()    msc_notify_trans()
	void msc_notify_trans(void) {
	wakeup_my_task();
	} \endcode
 *
 * \section udi_msc_use_cases Advanced use cases
 * For more advanced use of the UDI MSC module, see the following use cases:
 * - \subpage udi_msc_use_case_composite
 * - \subpage udc_use_case_1
 * - \subpage udc_use_case_2
 * - \subpage udc_use_case_3
 * - \subpage udc_use_case_5
 * - \subpage udc_use_case_6
 */

/**
 * \page udi_msc_use_case_composite MSC in a composite device
 *
 * A USB Composite Device is a USB Device which uses more than one USB class.
 * In this use case, the "USB MSC (Composite Device)" module is used to
 * create a USB composite device. Thus, this USB module can be associated with
 * another "Composite Device" module, like "USB HID Mouse (Composite Device)".
 * 
 * Also, you can refer to application note
 * <A href="http://www.atmel.com/dyn/resources/prod_documents/doc8445.pdf">
 * AVR4902 ASF - USB Composite Device</A>.
 *
 * \section udi_msc_use_case_composite_setup Setup steps
 * For the setup code of this use case to work, the
 * \ref udi_msc_basic_use_case "basic use case" must be followed.
 *
 * \section udi_msc_use_case_composite_usage Usage steps
 *
 * \subsection udi_msc_use_case_composite_usage_code Example code
 * Content of conf_usb.h:
 * \code
	 #define USB_DEVICE_EP_CTRL_SIZE  64
	 #define USB_DEVICE_NB_INTERFACE (X+1)
	 #define USB_DEVICE_MAX_EP (X+2)

	 #define UDI_MSC_EP_IN  (X | USB_EP_DIR_IN)
	 #define UDI_MSC_EP_OUT (Y | USB_EP_DIR_OUT)
	 #define UDI_MSC_IFACE_NUMBER  X

	 #define UDI_COMPOSITE_DESC_T \
	    udi_msc_desc_t udi_msc; \
	    ...
	 #define UDI_COMPOSITE_DESC_FS \
	    .udi_msc = UDI_MSC_DESC, \
	    ...
	 #define UDI_COMPOSITE_DESC_HS \
	    .udi_msc = UDI_MSC_DESC, \
	    ...
	 #define UDI_COMPOSITE_API \
	    &udi_api_msc, \
	    ...
\endcode
 *
 * \subsection udi_msc_use_case_composite_usage_flow Workflow
 * -# Ensure that conf_usb.h is available and contains the following parameters
 * required for a USB composite device configuration:
 *   - \code // Endpoint control size, This must be:
	// - 8, 16, 32 or 64 for full speed device (8 is recommended to save RAM)
	// - 64 for a high speed device
	#define USB_DEVICE_EP_CTRL_SIZE  64
	// Total Number of interfaces on this USB device.
	// Add 1 for MSC.
	#define USB_DEVICE_NB_INTERFACE (X+1)
	// Total number of endpoints on this USB device.
	// This must include each endpoint for each interface.
	// Add 2 for MSC.
	#define USB_DEVICE_MAX_EP (X+2) \endcode
 * -# Ensure that conf_usb.h contains the description of
 * composite device:
 *   - \code // The endpoint numbers chosen by you for the MSC.
	// The endpoint numbers starting from 1.
	#define UDI_MSC_EP_IN  (X | USB_EP_DIR_IN)
	#define UDI_MSC_EP_OUT (Y | USB_EP_DIR_OUT)
	// The interface index of an interface starting from 0
	#define UDI_MSC_IFACE_NUMBER  X \endcode
 * -# Ensure that conf_usb.h contains the following parameters
 * required for a USB composite device configuration:
 *   - \code // USB Interfaces descriptor structure
	#define UDI_COMPOSITE_DESC_T \
	   ...
	   udi_msc_desc_t udi_msc; \
	   ...
	// USB Interfaces descriptor value for Full Speed
	#define UDI_COMPOSITE_DESC_FS \
	   ...
	   .udi_msc = UDI_MSC_DESC_FS, \
	   ...
	// USB Interfaces descriptor value for High Speed
	#define UDI_COMPOSITE_DESC_HS \
	   ...
	   .udi_msc = UDI_MSC_DESC_HS, \
	   ...
	// USB Interface APIs
	#define UDI_COMPOSITE_API \
	   ...
	   &udi_api_msc, \
	   ... \endcode
 *   - \note The descriptors order given in the four lists above must be the
 *     same as the order defined by all interface indexes. The interface index
 *     orders are defined through UDI_X_IFACE_NUMBER defines.
 */

#endif // _UDI_MSC_H_
