/**
 * \file
 *
 * \brief USB Device Communication Device Class (CDC) interface definitions.
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

#ifndef _UDI_CDC_H_
#define _UDI_CDC_H_

#include "conf_usb.h"
#include "usb_protocol.h"
#include "usb_protocol_cdc.h"
#include "udd.h"
#include "udc_desc.h"
#include "udi.h"

// Check the number of port
#ifndef  UDI_CDC_PORT_NB
# define  UDI_CDC_PORT_NB 1
#endif
#if (UDI_CDC_PORT_NB < 1) || (UDI_CDC_PORT_NB > 7)
# error UDI_CDC_PORT_NB must be between 1 and 7
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup udi_cdc_group_udc
 * @{
 */

//! Global structure which contains standard UDI API for UDC
extern UDC_DESC_STORAGE udi_api_t udi_api_cdc_comm;
extern UDC_DESC_STORAGE udi_api_t udi_api_cdc_data;
//@}

/**
 * \ingroup udi_cdc_group
 * \defgroup udi_cdc_group_desc USB interface descriptors
 *
 * The following structures provide predefined USB interface descriptors.
 * It must be used to define the final USB descriptors.
 */
//@{

/**
 * \brief Communication Class interface descriptor
 *
 * Interface descriptor with associated functional and endpoint
 * descriptors for the CDC Communication Class interface.
 */
typedef struct {
	//! Standard interface descriptor
	usb_iface_desc_t iface;
	//! CDC Header functional descriptor
	usb_cdc_hdr_desc_t header;
	//! CDC Abstract Control Model functional descriptor
	usb_cdc_acm_desc_t acm;
	//! CDC Union functional descriptor
	usb_cdc_union_desc_t union_desc;
	//! CDC Call Management functional descriptor
	usb_cdc_call_mgmt_desc_t call_mgmt;
	//! Notification endpoint descriptor
	usb_ep_desc_t ep_notify;
} udi_cdc_comm_desc_t;


/**
 * \brief Data Class interface descriptor
 *
 * Interface descriptor with associated endpoint descriptors for the
 * CDC Data Class interface.
 */
typedef struct {
	//! Standard interface descriptor
	usb_iface_desc_t iface;
	//! Data IN/OUT endpoint descriptors
	usb_ep_desc_t ep_in;
	usb_ep_desc_t ep_out;
} udi_cdc_data_desc_t;


//! CDC communication endpoints size for all speeds
#define UDI_CDC_COMM_EP_SIZE        64
//! CDC data endpoints size for FS speed (8B, 16B, 32B, 64B)
#define UDI_CDC_DATA_EPS_FS_SIZE    64
//! CDC data endpoints size for HS speed (512B only)
#define UDI_CDC_DATA_EPS_HS_SIZE    512

/**
 * \name Content of interface descriptors
 * Up to 7 CDC interfaces can be implemented on a USB device.
 */
//@{
//! By default no string associated to these interfaces
#ifndef UDI_CDC_IAD_STRING_ID_0
#define UDI_CDC_IAD_STRING_ID_0   0
#endif
#ifndef UDI_CDC_COMM_STRING_ID_0
#define UDI_CDC_COMM_STRING_ID_0   0
#endif
#ifndef UDI_CDC_DATA_STRING_ID_0
#define UDI_CDC_DATA_STRING_ID_0   0
#endif
#define UDI_CDC_IAD_DESC_0      UDI_CDC_IAD_DESC(0)
#define UDI_CDC_COMM_DESC_0     UDI_CDC_COMM_DESC(0)
#define UDI_CDC_DATA_DESC_0_FS  UDI_CDC_DATA_DESC_FS(0)
#define UDI_CDC_DATA_DESC_0_HS  UDI_CDC_DATA_DESC_HS(0)

//! By default no string associated to these interfaces
#ifndef UDI_CDC_IAD_STRING_ID_1
#define UDI_CDC_IAD_STRING_ID_1  0
#endif
#ifndef UDI_CDC_COMM_STRING_ID_1
#define UDI_CDC_COMM_STRING_ID_1 0
#endif
#ifndef UDI_CDC_DATA_STRING_ID_1
#define UDI_CDC_DATA_STRING_ID_1 0
#endif
#define UDI_CDC_IAD_DESC_1      UDI_CDC_IAD_DESC(1)
#define UDI_CDC_COMM_DESC_1     UDI_CDC_COMM_DESC(1)
#define UDI_CDC_DATA_DESC_1_FS  UDI_CDC_DATA_DESC_FS(1)
#define UDI_CDC_DATA_DESC_1_HS  UDI_CDC_DATA_DESC_HS(1)

//! By default no string associated to these interfaces
#ifndef UDI_CDC_IAD_STRING_ID_2
#define UDI_CDC_IAD_STRING_ID_2   0
#endif
#ifndef UDI_CDC_COMM_STRING_ID_2
#define UDI_CDC_COMM_STRING_ID_2   0
#endif
#ifndef UDI_CDC_DATA_STRING_ID_2
#define UDI_CDC_DATA_STRING_ID_2   0
#endif
#define UDI_CDC_IAD_DESC_2      UDI_CDC_IAD_DESC(2)
#define UDI_CDC_COMM_DESC_2     UDI_CDC_COMM_DESC(2)
#define UDI_CDC_DATA_DESC_2_FS  UDI_CDC_DATA_DESC_FS(2)
#define UDI_CDC_DATA_DESC_2_HS  UDI_CDC_DATA_DESC_HS(2)

//! By default no string associated to these interfaces
#ifndef UDI_CDC_IAD_STRING_ID_3
#define UDI_CDC_IAD_STRING_ID_3   0
#endif
#ifndef UDI_CDC_COMM_STRING_ID_3
#define UDI_CDC_COMM_STRING_ID_3   0
#endif
#ifndef UDI_CDC_DATA_STRING_ID_3
#define UDI_CDC_DATA_STRING_ID_3   0
#endif
#define UDI_CDC_IAD_DESC_3      UDI_CDC_IAD_DESC(3)
#define UDI_CDC_COMM_DESC_3     UDI_CDC_COMM_DESC(3)
#define UDI_CDC_DATA_DESC_3_FS  UDI_CDC_DATA_DESC_FS(3)
#define UDI_CDC_DATA_DESC_3_HS  UDI_CDC_DATA_DESC_HS(3)

//! By default no string associated to these interfaces
#ifndef UDI_CDC_IAD_STRING_ID_4
#define UDI_CDC_IAD_STRING_ID_4   0
#endif
#ifndef UDI_CDC_COMM_STRING_ID_4
#define UDI_CDC_COMM_STRING_ID_4   0
#endif
#ifndef UDI_CDC_DATA_STRING_ID_4
#define UDI_CDC_DATA_STRING_ID_4   0
#endif
#define UDI_CDC_IAD_DESC_4      UDI_CDC_IAD_DESC(4)
#define UDI_CDC_COMM_DESC_4     UDI_CDC_COMM_DESC(4)
#define UDI_CDC_DATA_DESC_4_FS  UDI_CDC_DATA_DESC_FS(4)
#define UDI_CDC_DATA_DESC_4_HS  UDI_CDC_DATA_DESC_HS(4)

//! By default no string associated to these interfaces
#ifndef UDI_CDC_IAD_STRING_ID_5
#define UDI_CDC_IAD_STRING_ID_5   0
#endif
#ifndef UDI_CDC_COMM_STRING_ID_5
#define UDI_CDC_COMM_STRING_ID_5   0
#endif
#ifndef UDI_CDC_DATA_STRING_ID_5
#define UDI_CDC_DATA_STRING_ID_5   0
#endif
#define UDI_CDC_IAD_DESC_5      UDI_CDC_IAD_DESC(5)
#define UDI_CDC_COMM_DESC_5     UDI_CDC_COMM_DESC(5)
#define UDI_CDC_DATA_DESC_5_FS  UDI_CDC_DATA_DESC_FS(5)
#define UDI_CDC_DATA_DESC_5_HS  UDI_CDC_DATA_DESC_HS(5)

//! By default no string associated to these interfaces
#ifndef UDI_CDC_IAD_STRING_ID_6
#define UDI_CDC_IAD_STRING_ID_6   0
#endif
#ifndef UDI_CDC_COMM_STRING_ID_6
#define UDI_CDC_COMM_STRING_ID_6   0
#endif
#ifndef UDI_CDC_DATA_STRING_ID_6
#define UDI_CDC_DATA_STRING_ID_6   0
#endif
#define UDI_CDC_IAD_DESC_6      UDI_CDC_IAD_DESC(6)
#define UDI_CDC_COMM_DESC_6     UDI_CDC_COMM_DESC(6)
#define UDI_CDC_DATA_DESC_6_FS  UDI_CDC_DATA_DESC_FS(6)
#define UDI_CDC_DATA_DESC_6_HS  UDI_CDC_DATA_DESC_HS(6)
//@}


//! Content of CDC IAD interface descriptor for all speeds
#define UDI_CDC_IAD_DESC(port) { \
   .bLength                      = sizeof(usb_iad_desc_t),\
   .bDescriptorType              = USB_DT_IAD,\
   .bInterfaceCount              = 2,\
   .bFunctionClass               = CDC_CLASS_COMM,\
   .bFunctionSubClass            = CDC_SUBCLASS_ACM,\
   .bFunctionProtocol            = CDC_PROTOCOL_V25TER,\
   .bFirstInterface              = UDI_CDC_COMM_IFACE_NUMBER_##port,\
   .iFunction                    = UDI_CDC_IAD_STRING_ID_##port,\
   }

//! Content of CDC COMM interface descriptor for all speeds
#define UDI_CDC_COMM_DESC(port) { \
   .iface.bLength                = sizeof(usb_iface_desc_t),\
   .iface.bDescriptorType        = USB_DT_INTERFACE,\
   .iface.bAlternateSetting      = 0,\
   .iface.bNumEndpoints          = 1,\
   .iface.bInterfaceClass        = CDC_CLASS_COMM,\
   .iface.bInterfaceSubClass     = CDC_SUBCLASS_ACM,\
   .iface.bInterfaceProtocol     = CDC_PROTOCOL_V25TER,\
   .header.bFunctionLength       = sizeof(usb_cdc_hdr_desc_t),\
   .header.bDescriptorType       = CDC_CS_INTERFACE,\
   .header.bDescriptorSubtype    = CDC_SCS_HEADER,\
   .header.bcdCDC                = LE16(0x0110),\
   .call_mgmt.bFunctionLength    = sizeof(usb_cdc_call_mgmt_desc_t),\
   .call_mgmt.bDescriptorType    = CDC_CS_INTERFACE,\
   .call_mgmt.bDescriptorSubtype = CDC_SCS_CALL_MGMT,\
   .call_mgmt.bmCapabilities     = \
			CDC_CALL_MGMT_SUPPORTED | CDC_CALL_MGMT_OVER_DCI,\
   .acm.bFunctionLength          = sizeof(usb_cdc_acm_desc_t),\
   .acm.bDescriptorType          = CDC_CS_INTERFACE,\
   .acm.bDescriptorSubtype       = CDC_SCS_ACM,\
   .acm.bmCapabilities           = CDC_ACM_SUPPORT_LINE_REQUESTS,\
   .union_desc.bFunctionLength   = sizeof(usb_cdc_union_desc_t),\
   .union_desc.bDescriptorType   = CDC_CS_INTERFACE,\
   .union_desc.bDescriptorSubtype= CDC_SCS_UNION,\
   .ep_notify.bLength            = sizeof(usb_ep_desc_t),\
   .ep_notify.bDescriptorType    = USB_DT_ENDPOINT,\
   .ep_notify.bmAttributes       = USB_EP_TYPE_INTERRUPT,\
   .ep_notify.wMaxPacketSize     = LE16(UDI_CDC_COMM_EP_SIZE),\
   .ep_notify.bInterval          = 0x10,\
   .ep_notify.bEndpointAddress   = UDI_CDC_COMM_EP_##port,\
   .iface.bInterfaceNumber       = UDI_CDC_COMM_IFACE_NUMBER_##port,\
   .call_mgmt.bDataInterface     = UDI_CDC_DATA_IFACE_NUMBER_##port,\
   .union_desc.bMasterInterface  = UDI_CDC_COMM_IFACE_NUMBER_##port,\
   .union_desc.bSlaveInterface0  = UDI_CDC_DATA_IFACE_NUMBER_##port,\
   .iface.iInterface             = UDI_CDC_COMM_STRING_ID_##port,\
   }

//! Content of CDC DATA interface descriptors
#define UDI_CDC_DATA_DESC_COMMON \
   .iface.bLength                = sizeof(usb_iface_desc_t),\
   .iface.bDescriptorType        = USB_DT_INTERFACE,\
   .iface.bAlternateSetting      = 0,\
   .iface.bNumEndpoints          = 2,\
   .iface.bInterfaceClass        = CDC_CLASS_DATA,\
   .iface.bInterfaceSubClass     = 0,\
   .iface.bInterfaceProtocol     = 0,\
   .ep_in.bLength                = sizeof(usb_ep_desc_t),\
   .ep_in.bDescriptorType        = USB_DT_ENDPOINT,\
   .ep_in.bmAttributes           = USB_EP_TYPE_BULK,\
   .ep_in.bInterval              = 0,\
   .ep_out.bLength               = sizeof(usb_ep_desc_t),\
   .ep_out.bDescriptorType       = USB_DT_ENDPOINT,\
   .ep_out.bmAttributes          = USB_EP_TYPE_BULK,\
   .ep_out.bInterval             = 0,

#define UDI_CDC_DATA_DESC_FS(port) { \
   UDI_CDC_DATA_DESC_COMMON \
   .ep_in.wMaxPacketSize         = LE16(UDI_CDC_DATA_EPS_FS_SIZE),\
   .ep_out.wMaxPacketSize        = LE16(UDI_CDC_DATA_EPS_FS_SIZE),\
   .ep_in.bEndpointAddress       = UDI_CDC_DATA_EP_IN_##port,\
   .ep_out.bEndpointAddress      = UDI_CDC_DATA_EP_OUT_##port,\
   .iface.bInterfaceNumber       = UDI_CDC_DATA_IFACE_NUMBER_##port,\
   .iface.iInterface             = UDI_CDC_DATA_STRING_ID_##port,\
   }

#define UDI_CDC_DATA_DESC_HS(port) { \
   UDI_CDC_DATA_DESC_COMMON \
   .ep_in.wMaxPacketSize         = LE16(UDI_CDC_DATA_EPS_HS_SIZE),\
   .ep_out.wMaxPacketSize        = LE16(UDI_CDC_DATA_EPS_HS_SIZE),\
   .ep_in.bEndpointAddress       = UDI_CDC_DATA_EP_IN_##port,\
   .ep_out.bEndpointAddress      = UDI_CDC_DATA_EP_OUT_##port,\
   .iface.bInterfaceNumber       = UDI_CDC_DATA_IFACE_NUMBER_##port,\
   .iface.iInterface             = UDI_CDC_DATA_STRING_ID_##port,\
   }

//@}

/**
 * \ingroup udi_group
 * \defgroup udi_cdc_group USB Device Interface (UDI) for Communication Class Device (CDC)
 *
 * Common APIs used by high level application to use this USB class.
 *
 * These routines are used to transfer and control data
 * to/from USB CDC endpoint.
 *
 * See \ref udi_cdc_quickstart.
 * @{
 */

/**
 * \name Interface for application with single CDC interface support
 */
//@{

/**
 * \brief Notify a state change of DCD signal
 *
 * \param b_set      DCD is enabled if true, else disabled
 */
void udi_cdc_ctrl_signal_dcd(bool b_set);

/**
 * \brief Notify a state change of DSR signal
 *
 * \param b_set      DSR is enabled if true, else disabled
 */
void udi_cdc_ctrl_signal_dsr(bool b_set);

/**
 * \brief Notify a framing error
 */
void udi_cdc_signal_framing_error(void);

/**
 * \brief Notify a parity error
 */
void udi_cdc_signal_parity_error(void);

/**
 * \brief Notify a overrun
 */
void udi_cdc_signal_overrun(void);

/**
 * \brief Gets the number of byte received
 *
 * \return the number of data available
 */
iram_size_t udi_cdc_get_nb_received_data(void);

/**
 * \brief This function checks if a character has been received on the CDC line
 *
 * \return \c 1 if a byte is ready to be read.
 */
bool udi_cdc_is_rx_ready(void);

/**
 * \brief Waits and gets a value on CDC line
 *
 * \return value read on CDC line
 */
int udi_cdc_getc(void);

/**
 * \brief Reads a RAM buffer on CDC line
 *
 * \param buf       Values read
 * \param size      Number of value read
 *
 * \return the number of data remaining
 */
iram_size_t udi_cdc_read_buf(void* buf, iram_size_t size);

/**
 * \brief Non polling reads of a up to 'size' data from CDC line
 *
 * \param port      Communication port number to manage
 * \param buf       Buffer where to store read data
 * \param size      Maximum number of data to read (size of buffer)
 *
 * \return the number of data effectively read
 */
iram_size_t udi_cdc_read_no_polling(void* buf, iram_size_t size);

/**
 * \brief Gets the number of free byte in TX buffer
 *
 * \return the number of free byte in TX buffer
 */
iram_size_t udi_cdc_get_free_tx_buffer(void);

/**
 * \brief This function checks if a new character sent is possible
 * The type int is used to support scanf redirection from compiler LIB.
 *
 * \return \c 1 if a new character can be sent
 */
bool udi_cdc_is_tx_ready(void);

/**
 * \brief Puts a byte on CDC line
 * The type int is used to support printf redirection from compiler LIB.
 *
 * \param value      Value to put
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
int udi_cdc_putc(int value);

/**
 * \brief Writes a RAM buffer on CDC line
 *
 * \param buf       Values to write
 * \param size      Number of value to write
 *
 * \return the number of data remaining
 */
iram_size_t udi_cdc_write_buf(const void* buf, iram_size_t size);
//@}

/**
 * \name Interface for application with multi CDC interfaces support
 */
//@{

/**
 * \brief Notify a state change of DCD signal
 *
 * \param port       Communication port number to manage
 * \param b_set      DCD is enabled if true, else disabled
 */
void udi_cdc_multi_ctrl_signal_dcd(uint8_t port, bool b_set);

/**
 * \brief Notify a state change of DSR signal
 *
 * \param port       Communication port number to manage
 * \param b_set      DSR is enabled if true, else disabled
 */
void udi_cdc_multi_ctrl_signal_dsr(uint8_t port, bool b_set);

/**
 * \brief Notify a framing error
 *
 * \param port       Communication port number to manage
 */
void udi_cdc_multi_signal_framing_error(uint8_t port);

/**
 * \brief Notify a parity error
 *
 * \param port       Communication port number to manage
 */
void udi_cdc_multi_signal_parity_error(uint8_t port);

/**
 * \brief Notify a overrun
 *
 * \param port       Communication port number to manage
 */
void udi_cdc_multi_signal_overrun(uint8_t port);

/**
 * \brief Gets the number of byte received
 *
 * \param port       Communication port number to manage
 *
 * \return the number of data available
 */
iram_size_t udi_cdc_multi_get_nb_received_data(uint8_t port);

/**
 * \brief This function checks if a character has been received on the CDC line
 *
 * \param port       Communication port number to manage
 *
 * \return \c 1 if a byte is ready to be read.
 */
bool udi_cdc_multi_is_rx_ready(uint8_t port);

/**
 * \brief Waits and gets a value on CDC line
 *
 * \param port       Communication port number to manage
 *
 * \return value read on CDC line
 */
int udi_cdc_multi_getc(uint8_t port);

/**
 * \brief Reads a RAM buffer on CDC line
 *
 * \param port       Communication port number to manage
 * \param buf       Values read
 * \param size      Number of values read
 *
 * \return the number of data remaining
 */
iram_size_t udi_cdc_multi_read_buf(uint8_t port, void* buf, iram_size_t size);

/**
 * \brief Gets the number of free byte in TX buffer
 *
 * \param port       Communication port number to manage
 *
 * \return the number of free byte in TX buffer
 */
iram_size_t udi_cdc_multi_get_free_tx_buffer(uint8_t port);

/**
 * \brief This function checks if a new character sent is possible
 * The type int is used to support scanf redirection from compiler LIB.
 *
 * \param port       Communication port number to manage
 *
 * \return \c 1 if a new character can be sent
 */
bool udi_cdc_multi_is_tx_ready(uint8_t port);

/**
 * \brief Puts a byte on CDC line
 * The type int is used to support printf redirection from compiler LIB.
 *
 * \param port       Communication port number to manage
 * \param value      Value to put
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
int udi_cdc_multi_putc(uint8_t port, int value);

/**
 * \brief Writes a RAM buffer on CDC line
 *
 * \param port       Communication port number to manage
 * \param buf       Values to write
 * \param size      Number of value to write
 *
 * \return the number of data remaining
 */
iram_size_t udi_cdc_multi_write_buf(uint8_t port, const void* buf, iram_size_t size);
//@}

//@}

/**
 * \page udi_cdc_quickstart Quick start guide for USB device Communication Class Device module (UDI CDC)
 *
 * This is the quick start guide for the \ref udi_cdc_group
 * "USB device interface CDC module (UDI CDC)" with step-by-step instructions on
 * how to configure and use the modules in a selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 *
 * \section udi_cdc_basic_use_case Basic use case
 * In this basic use case, the "USB CDC (Single Interface Device)" module is used
 * with only one communication port.
 * The "USB CDC (Composite Device)" module usage is described in \ref udi_cdc_use_cases
 * "Advanced use cases".
 *
 * \section udi_cdc_basic_use_case_setup Setup steps
 * \subsection udi_cdc_basic_use_case_setup_prereq Prerequisites
 * \copydetails udc_basic_use_case_setup_prereq
 * \subsection udi_cdc_basic_use_case_setup_code Example code
 * \copydetails udc_basic_use_case_setup_code
 * \subsection udi_cdc_basic_use_case_setup_flow Workflow
 * \copydetails udc_basic_use_case_setup_flow
 *
 * \section udi_cdc_basic_use_case_usage Usage steps
 *
 * \subsection udi_cdc_basic_use_case_usage_code Example code
 * Content of conf_usb.h:
 * \code
	 #define UDI_CDC_ENABLE_EXT(port) my_callback_cdc_enable()
	 extern bool my_callback_cdc_enable(void);
	 #define UDI_CDC_DISABLE_EXT(port) my_callback_cdc_disable()
	 extern void my_callback_cdc_disable(void);
	 #define  UDI_CDC_LOW_RATE

	 #define  UDI_CDC_DEFAULT_RATE             115200
	 #define  UDI_CDC_DEFAULT_STOPBITS         CDC_STOP_BITS_1
	 #define  UDI_CDC_DEFAULT_PARITY           CDC_PAR_NONE
	 #define  UDI_CDC_DEFAULT_DATABITS         8

	 #include "udi_cdc_conf.h" // At the end of conf_usb.h file
\endcode
 *
 * Add to application C-file:
 * \code
	 static bool my_flag_autorize_cdc_transfert = false;
	 bool my_callback_cdc_enable(void)
	 {
	    my_flag_autorize_cdc_transfert = true;
	    return true;
	 }
	 void my_callback_cdc_disable(void)
	 {
	    my_flag_autorize_cdc_transfert = false;
	 }

	 void task(void)
	 {
	    if (my_flag_autorize_cdc_transfert) {
	        udi_cdc_putc('A');
	        udi_cdc_getc();
	    }
	 }
\endcode
 *
 * \subsection udi_cdc_basic_use_case_setup_flow Workflow
 * -# Ensure that conf_usb.h is available and contains the following configuration,
 * which is the USB device CDC configuration:
 *   - \code #define USB_DEVICE_SERIAL_NAME  "12...EF" // Disk SN for CDC \endcode
 *     \note The USB serial number is mandatory when a CDC interface is used.
 *   - \code #define UDI_CDC_ENABLE_EXT(port) my_callback_cdc_enable()
	 extern bool my_callback_cdc_enable(void); \endcode
 *     \note After the device enumeration (detecting and identifying USB devices),
 *     the USB host starts the device configuration. When the USB CDC interface
 *     from the device is accepted by the host, the USB host enables this interface and the
 *     UDI_CDC_ENABLE_EXT() callback function is called and return true.
 *     Thus, when this event is received, the data transfer on CDC interface are authorized.
 *   - \code #define UDI_CDC_DISABLE_EXT(port) my_callback_cdc_disable()
	 extern void my_callback_cdc_disable(void); \endcode
 *     \note When the USB device is unplugged or is reset by the USB host, the USB
 *     interface is disabled and the UDI_CDC_DISABLE_EXT() callback function
 *     is called. Thus, the data transfer must be stopped on CDC interface.
 *   - \code #define  UDI_CDC_LOW_RATE \endcode
 *     \note  Define it when the transfer CDC Device to Host is a low rate
 *     (<512000 bauds) to reduce CDC buffers size.
 *   - \code #define  UDI_CDC_DEFAULT_RATE             115200
	#define  UDI_CDC_DEFAULT_STOPBITS         CDC_STOP_BITS_1
	#define  UDI_CDC_DEFAULT_PARITY           CDC_PAR_NONE
	#define  UDI_CDC_DEFAULT_DATABITS         8 \endcode
 *     \note Default configuration of communication port at startup.
 * -# Send or wait data on CDC line:
 *   - \code // Waits and gets a value on CDC line
	int udi_cdc_getc(void);
	// Reads a RAM buffer on CDC line
	iram_size_t udi_cdc_read_buf(int* buf, iram_size_t size);
	// Puts a byte on CDC line
	int udi_cdc_putc(int value);
	// Writes a RAM buffer on CDC line
	iram_size_t udi_cdc_write_buf(const int* buf, iram_size_t size); \endcode
 *
 * \section udi_cdc_use_cases Advanced use cases
 * For more advanced use of the UDI CDC module, see the following use cases:
 * - \subpage udi_cdc_use_case_composite
 * - \subpage udc_use_case_1
 * - \subpage udc_use_case_2
 * - \subpage udc_use_case_3
 * - \subpage udc_use_case_4
 * - \subpage udc_use_case_5
 * - \subpage udc_use_case_6
 */

/**
 * \page udi_cdc_use_case_composite CDC in a composite device
 *
 * A USB Composite Device is a USB Device which uses more than one USB class.
 * In this use case, the "USB CDC (Composite Device)" module is used to
 * create a USB composite device. Thus, this USB module can be associated with
 * another "Composite Device" module, like "USB HID Mouse (Composite Device)".
 *
 * Also, you can refer to application note
 * <A href="http://www.atmel.com/dyn/resources/prod_documents/doc8445.pdf">
 * AVR4902 ASF - USB Composite Device</A>.
 *
 * \section udi_cdc_use_case_composite_setup Setup steps
 * For the setup code of this use case to work, the
 * \ref udi_cdc_basic_use_case "basic use case" must be followed.
 *
 * \section udi_cdc_use_case_composite_usage Usage steps
 *
 * \subsection udi_cdc_use_case_composite_usage_code Example code
 * Content of conf_usb.h:
 * \code
	 #define USB_DEVICE_EP_CTRL_SIZE  64
	 #define USB_DEVICE_NB_INTERFACE (X+2)
	 #define USB_DEVICE_MAX_EP (X+3)

	 #define  UDI_CDC_DATA_EP_IN_0          (1 | USB_EP_DIR_IN)  // TX
	 #define  UDI_CDC_DATA_EP_OUT_0         (2 | USB_EP_DIR_OUT) // RX
	 #define  UDI_CDC_COMM_EP_0             (3 | USB_EP_DIR_IN)  // Notify endpoint
	 #define  UDI_CDC_COMM_IFACE_NUMBER_0   X+0
	 #define  UDI_CDC_DATA_IFACE_NUMBER_0   X+1

	 #define UDI_COMPOSITE_DESC_T \
	    usb_iad_desc_t udi_cdc_iad; \
	    udi_cdc_comm_desc_t udi_cdc_comm; \
	    udi_cdc_data_desc_t udi_cdc_data; \
	    ...
	 #define UDI_COMPOSITE_DESC_FS \
	    .udi_cdc_iad               = UDI_CDC_IAD_DESC_0, \
	    .udi_cdc_comm              = UDI_CDC_COMM_DESC_0, \
	    .udi_cdc_data              = UDI_CDC_DATA_DESC_0_FS, \
	    ...
	 #define UDI_COMPOSITE_DESC_HS \
	    .udi_cdc_iad               = UDI_CDC_IAD_DESC_0, \
	    .udi_cdc_comm              = UDI_CDC_COMM_DESC_0, \
	    .udi_cdc_data              = UDI_CDC_DATA_DESC_0_HS, \
	    ...
	 #define UDI_COMPOSITE_API \
	    &udi_api_cdc_comm,       \
	    &udi_api_cdc_data,       \
	    ...
\endcode
 *
 * \subsection udi_cdc_use_case_composite_usage_flow Workflow
 * -# Ensure that conf_usb.h is available and contains the following parameters
 * required for a USB composite device configuration:
 *   - \code // Endpoint control size, This must be:
	// - 8, 16, 32 or 64 for full speed device (8 is recommended to save RAM)
	// - 64 for a high speed device
	#define USB_DEVICE_EP_CTRL_SIZE  64
	// Total Number of interfaces on this USB device.
	// Add 2 for CDC.
	#define USB_DEVICE_NB_INTERFACE (X+2)
	// Total number of endpoints on this USB device.
	// This must include each endpoint for each interface.
	// Add 3 for CDC.
	#define USB_DEVICE_MAX_EP (X+3) \endcode
 * -# Ensure that conf_usb.h contains the description of
 * composite device:
 *   - \code // The endpoint numbers chosen by you for the CDC.
	// The endpoint numbers starting from 1.
	#define  UDI_CDC_DATA_EP_IN_0            (1 | USB_EP_DIR_IN)  // TX
	#define  UDI_CDC_DATA_EP_OUT_0           (2 | USB_EP_DIR_OUT) // RX
	#define  UDI_CDC_COMM_EP_0               (3 | USB_EP_DIR_IN)  // Notify endpoint
	// The interface index of an interface starting from 0
	#define  UDI_CDC_COMM_IFACE_NUMBER_0     X+0
	#define  UDI_CDC_DATA_IFACE_NUMBER_0     X+1 \endcode
 * -# Ensure that conf_usb.h contains the following parameters
 * required for a USB composite device configuration:
 *   - \code // USB Interfaces descriptor structure
	#define UDI_COMPOSITE_DESC_T \
	   ...
	   usb_iad_desc_t udi_cdc_iad; \
	   udi_cdc_comm_desc_t udi_cdc_comm; \
	   udi_cdc_data_desc_t udi_cdc_data; \
	   ...
	// USB Interfaces descriptor value for Full Speed
	#define UDI_COMPOSITE_DESC_FS \
	   ...
	   .udi_cdc_iad               = UDI_CDC_IAD_DESC_0, \
	   .udi_cdc_comm              = UDI_CDC_COMM_DESC_0, \
	   .udi_cdc_data              = UDI_CDC_DATA_DESC_0_FS, \
	   ...
	// USB Interfaces descriptor value for High Speed
	#define UDI_COMPOSITE_DESC_HS \
	   ...
	   .udi_cdc_iad               = UDI_CDC_IAD_DESC_0, \
	   .udi_cdc_comm              = UDI_CDC_COMM_DESC_0, \
	   .udi_cdc_data              = UDI_CDC_DATA_DESC_0_HS, \
	   ...
	// USB Interface APIs
	#define UDI_COMPOSITE_API \
	   ...
	   &udi_api_cdc_comm,       \
	   &udi_api_cdc_data,       \
	   ... \endcode
 *   - \note The descriptors order given in the four lists above must be the
 *     same as the order defined by all interface indexes. The interface index
 *     orders are defined through UDI_X_IFACE_NUMBER defines.\n
 *     Also, the CDC requires a USB Interface Association Descriptor (IAD) for
 *     composite device.
 */

#ifdef __cplusplus
}
#endif
#endif // _UDI_CDC_H_
