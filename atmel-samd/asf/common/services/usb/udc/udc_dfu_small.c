/**
 * \file
 *
 * \brief USB Device Controller (UDC) optimized for DFU FLIP Device
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

#include "conf_usb.h"
#include "usb_protocol.h"
#include "udd.h"
#include "udc_desc.h"
#include "udi.h"
#include "udc.h"
#include "udi_dfu_atmel.h"

extern bool udi_dfu_atmel_enable(void);
extern void udi_dfu_atmel_disable(void);
extern bool udi_dfu_atmel_setup(void);

/**
 * \addtogroup udc_group
 * @{
 */

//! \name Internal variables to manage the USB device
//! @{

//! Device status state (see enum usb_device_status in usb_protocol.h)
static le16_t udc_device_status;
//! Device Configuration number selected by the USB host
static uint8_t udc_num_configuration = 0;
//! Pointer on the selected speed device configuration
//static udc_config_speed_t UDC_DESC_STORAGE *udc_ptr_conf;
//! Pointer on interface descriptor used by SETUP request.
//static usb_iface_desc_t UDC_DESC_STORAGE *udc_ptr_iface;
extern UDC_DESC_STORAGE usb_dev_desc_t udc_device_desc;
//! Structure for USB Device Configuration Descriptor
COMPILER_PACK_SET(1)
typedef struct {
	usb_conf_desc_t conf;
	udi_dfu_atmel_desc_t udi_dfu_atmel;
} udc_desc_t;
COMPILER_PACK_RESET()
extern UDC_DESC_STORAGE udc_desc_t udc_desc_fs;

//! @}


//! \name Internal structure to store the USB device main strings
//! @{

/**
 * \brief USB device manufacture name storage
 * String is allocated only if USB_DEVICE_MANUFACTURE_NAME is declared
 * by usb application configuration
 */
#ifdef USB_DEVICE_MANUFACTURE_NAME
static uint8_t udc_string_manufacturer_name[] = USB_DEVICE_MANUFACTURE_NAME;
#define USB_DEVICE_MANUFACTURE_NAME_SIZE  (sizeof(udc_string_manufacturer_name)-1)
#else
#define USB_DEVICE_MANUFACTURE_NAME_SIZE  0
#endif


/**
 * \brief USB device product name storage
 * String is allocated only if USB_DEVICE_PRODUCT_NAME is declared
 * by usb application configuration
 */
#ifdef USB_DEVICE_PRODUCT_NAME
static uint8_t udc_string_product_name[] = USB_DEVICE_PRODUCT_NAME;
#define USB_DEVICE_PRODUCT_NAME_SIZE  (sizeof(udc_string_product_name)-1)
#else
#define USB_DEVICE_PRODUCT_NAME_SIZE  0
#endif

/**
 * \brief USB device serial number storage
 * String is allocated only if USB_DEVICE_SERIAL_NAME is declared
 * by usb application configuration
 */
#ifdef USB_DEVICE_SERIAL_NAME
static uint8_t udc_string_serial_name[] = USB_DEVICE_SERIAL_NAME;
#define USB_DEVICE_SERIAL_NAME_SIZE  (sizeof(udc_string_serial_name)-1)
#else
#define USB_DEVICE_SERIAL_NAME_SIZE  0
#endif

/**
 * \brief USB device string descriptor
 * Structure used to transfer ASCII strings to USB String descriptor structure.
 */
struct udc_string_desc_t {
	usb_str_desc_t header;
	le16_t string[Max(Max(USB_DEVICE_MANUFACTURE_NAME_SIZE, \
		USB_DEVICE_PRODUCT_NAME_SIZE), USB_DEVICE_SERIAL_NAME_SIZE)];
};
static UDC_DESC_STORAGE struct udc_string_desc_t udc_string_desc = {
	.header.bDescriptorType = USB_DT_STRING
};

/**
 * \brief Language ID of USB device (US ID by default)
 */
static UDC_DESC_STORAGE usb_str_lgid_desc_t udc_string_desc_languageid = {
	.desc.bLength = sizeof(usb_str_lgid_desc_t),
	.desc.bDescriptorType = USB_DT_STRING,
	.string = {LE16(USB_LANGID_EN_US)}
};

//! @}




/**
 * \brief Disables an usb device interface (UDI)
 * This routine call the UDI corresponding to interface number
 *
 * \param iface_num     Interface number to disable
 *
 * \return 1 if it is done or 0 if interface is not found
 */
static bool udc_iface_disable(void)
{
	// Disable interface
	udi_dfu_atmel_disable();
	return true;
}


/**
 * \brief Enables an usb device interface (UDI)
 * This routine calls the UDI corresponding
 * to the interface and setting number.
 *
 * \param iface_num     Interface number to enable
 * \param setting_num   Setting number to enable
 *
 * \return 1 if it is done or 0 if interface is not found
 */
static bool udc_iface_enable(void)
{
	// Enable the interface
	return udi_dfu_atmel_enable();
}

/*! \brief Start the USB Device stack
 */
void udc_start(void)
{
	udd_enable();
}

/*! \brief Stop the USB Device stack
 */
void udc_stop(void)
{
	udd_disable();
}

/**
 * \brief Reset the current configuration of the USB device,
 * This routines can be called by UDD when a RESET on the USB line occurs.
 */
void udc_reset(void)
{
	if (udc_num_configuration) {
		udc_iface_disable();
	}
	udc_num_configuration = 0;
	udc_device_status =
#if (USB_DEVICE_ATTR & USB_CONFIG_ATTR_SELF_POWERED)
			CPU_TO_LE16(USB_DEV_STATUS_SELF_POWERED);
#else
			CPU_TO_LE16(USB_DEV_STATUS_BUS_POWERED);
#endif
}

void udc_sof_notify(void)
{
}

/**
 * \brief Standard device request to get device status
 *
 * \return true if success
 */
static bool udc_req_std_dev_get_status(void)
{
	udd_set_setup_payload(
			(uint8_t *) &udc_device_status,
			sizeof(udc_device_status));
	return true;
}


/**
 * \brief Change the address of device
 * Callback called at the end of request set address
 */
static void udc_valid_address(void)
{
	udd_set_address(udd_g_ctrlreq.req.wValue & 0x7F);
}


/**
 * \brief Standard device request to set device address
 *
 * \return true if success
 */
static bool udc_req_std_dev_set_address(void)
{
	// The address must be changed at the end of setup request after the handshake
	// then we use a callback to change address
	udd_g_ctrlreq.callback = udc_valid_address;
	return true;
}


/**
 * \brief Standard device request to get device string descriptor
 *
 * \return true if success
 */
static bool udc_req_std_dev_get_str_desc(void)
{
	uint8_t i;
	uint8_t *str;
	uint8_t str_lgt=0;

	// Link payload pointer to the string corresponding at request
	switch (udd_g_ctrlreq.req.wValue & 0xff) {
	case 0:
		udd_set_setup_payload(
				(uint8_t *) & udc_string_desc_languageid,
				sizeof(udc_string_desc_languageid));
		break;

#ifdef USB_DEVICE_MANUFACTURE_NAME
	case 1:
		str_lgt = USB_DEVICE_MANUFACTURE_NAME_SIZE;
		str = udc_string_manufacturer_name;
		break;
#endif
#ifdef USB_DEVICE_PRODUCT_NAME
	case 2:
		str_lgt = USB_DEVICE_PRODUCT_NAME_SIZE;
		str = udc_string_product_name;
		break;
#endif
#ifdef USB_DEVICE_SERIAL_NAME
	case 3:
		str_lgt = USB_DEVICE_SERIAL_NAME_SIZE;
		str = udc_string_serial_name;
		break;
#endif
	default:
		return false;
	}

	if (str_lgt != 0) {
		for(i = 0; i < str_lgt; i++) {
			udc_string_desc.string[i] = cpu_to_le16((le16_t)str[i]);
		}

		udc_string_desc.header.bLength = 2 + (str_lgt) * 2;
		udd_set_setup_payload(
				(uint8_t *) &udc_string_desc,
				udc_string_desc.header.bLength);
	}
	return true;
}


/**
 * \brief Standard device request to get descriptors about USB device
 *
 * \return true if success
 */
static bool udc_req_std_dev_get_descriptor(void)
{
	// Check descriptor ID
	switch ((uint8_t) (udd_g_ctrlreq.req.wValue >> 8)) {
	case USB_DT_DEVICE:
		// Device descriptor requested
		udd_set_setup_payload(
				(uint8_t*)&udc_device_desc,
				udc_device_desc.bLength);
		break;

	case USB_DT_CONFIGURATION:
		// Configuration descriptor requested
		// FS descriptor
		udd_set_setup_payload(
				(uint8_t*)&udc_desc_fs,
				le16_to_cpu(udc_desc_fs.conf.wTotalLength));
		break;

	case USB_DT_STRING:
		// String descriptor requested
		if (!udc_req_std_dev_get_str_desc()) {
			return false;
		}
		break;

	default:
		// Unknown descriptor requested
		return false;
	}
	// if the descriptor is larger than length requested, then reduce it
	if (udd_g_ctrlreq.req.wLength < udd_g_ctrlreq.payload_size) {
		udd_g_ctrlreq.payload_size = udd_g_ctrlreq.req.wLength;
	}
	return true;
}


/**
 * \brief Standard device request to get configuration number
 *
 * \return true if success
 */
static bool udc_req_std_dev_get_configuration(void)
{
	udd_set_setup_payload(
			&udc_num_configuration, 1);
	return true;
}


/**
 * \brief Standard device request to enable a configuration
 *
 * \return true if success
 */
static bool udc_req_std_dev_set_configuration(void)
{
	// Reset current configuration
	udc_reset();

	// Enable new configuration
	udc_num_configuration = udd_g_ctrlreq.req.wValue & 0xFF;
	if (udc_num_configuration == 0) {
		return true; // Default empty configuration requested
	}
	// Pointer of the configuration descriptor always good
	// Enable all interfaces of the selected configuration
	if (!udc_iface_enable()) {
		return false;
	}
	return true;
}


/**
 * \brief Main routine to manage the standard USB SETUP request
 *
 * \return true if the request is supported
 */
static bool udc_reqstd(void)
{
	if (USB_REQ_RECIP_DEVICE != Udd_setup_recipient()) {
		return false;
	}

	if (Udd_setup_is_in()) {
		// Standard Get Device request
		switch (udd_g_ctrlreq.req.bRequest) {
		case USB_REQ_GET_STATUS:
			return udc_req_std_dev_get_status();
		case USB_REQ_GET_DESCRIPTOR:
			return udc_req_std_dev_get_descriptor();
		case USB_REQ_GET_CONFIGURATION:
			return udc_req_std_dev_get_configuration();
		}
	} else {
		// Standard Set Device request
		switch (udd_g_ctrlreq.req.bRequest) {
		case USB_REQ_SET_ADDRESS:
			return udc_req_std_dev_set_address();
		case USB_REQ_SET_CONFIGURATION:
			return udc_req_std_dev_set_configuration();
		}
	}
	return false;
}


/**
 * \brief Send the SETUP interface request to UDI
 *
 * \return true if the request is supported
 */
static bool udc_req_iface(void)
{
	// Send the SETUP request to the UDI corresponding to the interface number
	return udi_dfu_atmel_setup();
}


/**
 * \brief Main routine to manage the USB SETUP request.
 *
 * This function parses a USB SETUP request and submits an appropriate
 * response back to the host or, in the case of SETUP OUT requests
 * with data, sets up a buffer for receiving the data payload.
 *
 * The main standard requests defined by the USB 2.0 standard are handled
 * internally. The interface requests are sent to UDI, and the specific request
 * sent to a specific application callback.
 *
 * \return true if the request is supported, else the request is stalled by UDD
 */
bool udc_process_setup(void)
{
	// By default no data (receive/send) and no callbacks registered
	udd_g_ctrlreq.payload_size = 0;
	udd_g_ctrlreq.callback = NULL;
	udd_g_ctrlreq.over_under_run = NULL;

	if (Udd_setup_is_in()) {
		if (udd_g_ctrlreq.req.wLength == 0) {
			return false; // Error from USB host
		}
	}

	// If standard request then try to decode it in UDC
	if (Udd_setup_type() == USB_REQ_TYPE_STANDARD) {
		if (udc_reqstd()) {
			return true;
		}
	}

	// If interface request then try to decode it in UDI
	if (Udd_setup_recipient() == USB_REQ_RECIP_INTERFACE) {
		if (udc_req_iface()) {
			return true;
		}
	}

	// Here SETUP request unknown by UDC and UDIs
#ifdef USB_DEVICE_SPECIFIC_REQUEST
	// Try to decode it in specific callback
	return USB_DEVICE_SPECIFIC_REQUEST(); // Ex: Vendor request,...
#else
	return false;
#endif
}

//! @}
