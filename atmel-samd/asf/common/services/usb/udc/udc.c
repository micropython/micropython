/**
 * \file
 *
 * \brief USB Device Controller (UDC)
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

#include "conf_usb.h"
#include "usb_protocol.h"
#include "udd.h"
#include "udc_desc.h"
#include "udi.h"
#include "udc.h"

/**
 * \ingroup udc_group
 * \defgroup udc_group_interne Implementation of UDC
 *
 * Internal implementation
 * @{
 */

//! \name Internal variables to manage the USB device
//! @{

//! Device status state (see enum usb_device_status in usb_protocol.h)
static le16_t udc_device_status;

COMPILER_WORD_ALIGNED
//! Device interface setting value
static uint8_t udc_iface_setting = 0;

//! Device Configuration number selected by the USB host
COMPILER_WORD_ALIGNED
static uint8_t udc_num_configuration = 0;

//! Pointer on the selected speed device configuration
static udc_config_speed_t UDC_DESC_STORAGE *udc_ptr_conf;

//! Pointer on interface descriptor used by SETUP request.
static usb_iface_desc_t UDC_DESC_STORAGE *udc_ptr_iface;

//! @}


//! \name Internal structure to store the USB device main strings
//! @{

/**
 * \brief Language ID of USB device (US ID by default)
 */
COMPILER_WORD_ALIGNED
static UDC_DESC_STORAGE usb_str_lgid_desc_t udc_string_desc_languageid = {
	.desc.bLength = sizeof(usb_str_lgid_desc_t),
	.desc.bDescriptorType = USB_DT_STRING,
	.string = {LE16(USB_LANGID_EN_US)}
};

/**
 * \brief USB device manufacture name storage
 * String is allocated only if USB_DEVICE_MANUFACTURE_NAME is declared
 * by usb application configuration
 */
#ifdef USB_DEVICE_MANUFACTURE_NAME
static uint8_t udc_string_manufacturer_name[] = USB_DEVICE_MANUFACTURE_NAME;
#  define USB_DEVICE_MANUFACTURE_NAME_SIZE  \
	(sizeof(udc_string_manufacturer_name)-1)
#else
#  define USB_DEVICE_MANUFACTURE_NAME_SIZE  0
#endif

/**
 * \brief USB device product name storage
 * String is allocated only if USB_DEVICE_PRODUCT_NAME is declared
 * by usb application configuration
 */
#ifdef USB_DEVICE_PRODUCT_NAME
static uint8_t udc_string_product_name[] = USB_DEVICE_PRODUCT_NAME;
#  define USB_DEVICE_PRODUCT_NAME_SIZE  (sizeof(udc_string_product_name)-1)
#else
#  define USB_DEVICE_PRODUCT_NAME_SIZE  0
#endif

/**
 * \brief Get USB device serial number
 *
 * Use the define USB_DEVICE_SERIAL_NAME to set static serial number.
 *
 * For dynamic serial number set the define USB_DEVICE_GET_SERIAL_NAME_POINTER
 * to a suitable pointer. This will also require the serial number length
 * define USB_DEVICE_GET_SERIAL_NAME_LENGTH.
 */
#if defined USB_DEVICE_GET_SERIAL_NAME_POINTER
	static const uint8_t *udc_get_string_serial_name(void)
	{
		return (const uint8_t *)USB_DEVICE_GET_SERIAL_NAME_POINTER;
	}
#  define USB_DEVICE_SERIAL_NAME_SIZE \
	USB_DEVICE_GET_SERIAL_NAME_LENGTH
#elif defined USB_DEVICE_SERIAL_NAME
	static const uint8_t *udc_get_string_serial_name(void)
	{
		return (const uint8_t *)USB_DEVICE_SERIAL_NAME;
	}
#  define USB_DEVICE_SERIAL_NAME_SIZE \
	(sizeof(USB_DEVICE_SERIAL_NAME)-1)
#else
#  define USB_DEVICE_SERIAL_NAME_SIZE  0
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
COMPILER_WORD_ALIGNED
static UDC_DESC_STORAGE struct udc_string_desc_t udc_string_desc = {
	.header.bDescriptorType = USB_DT_STRING
};
//! @}

usb_iface_desc_t UDC_DESC_STORAGE *udc_get_interface_desc(void)
{
	return udc_ptr_iface;
}

/**
 * \brief Returns a value to check the end of USB Configuration descriptor
 *
 * \return address after the last byte of USB Configuration descriptor
 */
static usb_conf_desc_t UDC_DESC_STORAGE *udc_get_eof_conf(void)
{
	return (UDC_DESC_STORAGE usb_conf_desc_t *) ((uint8_t *)
			udc_ptr_conf->desc +
			le16_to_cpu(udc_ptr_conf->desc->wTotalLength));
}

#if (0!=USB_DEVICE_MAX_EP)
/**
 * \brief Search specific descriptor in global interface descriptor
 *
 * \param desc       Address of interface descriptor
 *                   or previous specific descriptor found
 * \param desc_id    Descriptor ID to search
 *
 * \return address of specific descriptor found
 * \return NULL if it is the end of global interface descriptor
 */
static usb_conf_desc_t UDC_DESC_STORAGE *udc_next_desc_in_iface(usb_conf_desc_t
		UDC_DESC_STORAGE * desc, uint8_t desc_id)
{
	usb_conf_desc_t UDC_DESC_STORAGE *ptr_eof_desc;

	ptr_eof_desc = udc_get_eof_conf();
	// Go to next descriptor
	desc = (UDC_DESC_STORAGE usb_conf_desc_t *) ((uint8_t *) desc +
			desc->bLength);
	// Check the end of configuration descriptor
	while (ptr_eof_desc > desc) {
		// If new interface descriptor is found,
		// then it is the end of the current global interface descriptor
		if (USB_DT_INTERFACE == desc->bDescriptorType) {
			break; // End of global interface descriptor
		}
		if (desc_id == desc->bDescriptorType) {
			return desc; // Specific descriptor found
		}
		// Go to next descriptor
		desc = (UDC_DESC_STORAGE usb_conf_desc_t *) ((uint8_t *) desc +
				desc->bLength);
	}
	return NULL; // No specific descriptor found
}
#endif

/**
 * \brief Search an interface descriptor
 * This routine updates the internal pointer udc_ptr_iface.
 *
 * \param iface_num     Interface number to find in Configuration Descriptor
 * \param setting_num   Setting number of interface to find
 *
 * \return 1 if found or 0 if not found
 */
static bool udc_update_iface_desc(uint8_t iface_num, uint8_t setting_num)
{
	usb_conf_desc_t UDC_DESC_STORAGE *ptr_end_desc;

	if (0 == udc_num_configuration) {
		return false;
	}

	if (iface_num >= udc_ptr_conf->desc->bNumInterfaces) {
		return false;
	}

	// Start at the beginning of configuration descriptor
	udc_ptr_iface = (UDC_DESC_STORAGE usb_iface_desc_t *)
			udc_ptr_conf->desc;

	// Check the end of configuration descriptor
	ptr_end_desc = udc_get_eof_conf();
	while (ptr_end_desc >
			(UDC_DESC_STORAGE usb_conf_desc_t *) udc_ptr_iface) {
		if (USB_DT_INTERFACE == udc_ptr_iface->bDescriptorType) {
			// A interface descriptor is found
			// Check interface and alternate setting number
			if ((iface_num == udc_ptr_iface->bInterfaceNumber) &&
					(setting_num ==
					udc_ptr_iface->bAlternateSetting)) {
				return true; // Interface found
			}
		}
		// Go to next descriptor
		udc_ptr_iface = (UDC_DESC_STORAGE usb_iface_desc_t *) (
				(uint8_t *) udc_ptr_iface +
				udc_ptr_iface->bLength);
	}
	return false; // Interface not found
}

/**
 * \brief Disables an usb device interface (UDI)
 * This routine call the UDI corresponding to interface number
 *
 * \param iface_num     Interface number to disable
 *
 * \return 1 if it is done or 0 if interface is not found
 */
static bool udc_iface_disable(uint8_t iface_num)
{
	udi_api_t UDC_DESC_STORAGE *udi_api;

	// Select first alternate setting of the interface
	// to update udc_ptr_iface before call iface->getsetting()
	if (!udc_update_iface_desc(iface_num, 0)) {
		return false;
	}

	// Select the interface with the current alternate setting
	udi_api = udc_ptr_conf->udi_apis[iface_num];

#if (0!=USB_DEVICE_MAX_EP)
	if (!udc_update_iface_desc(iface_num, udi_api->getsetting())) {
		return false;
	}

	// Start at the beginning of interface descriptor
	{
		usb_ep_desc_t UDC_DESC_STORAGE *ep_desc;
		ep_desc = (UDC_DESC_STORAGE usb_ep_desc_t *) udc_ptr_iface;
		while (1) {
			// Search Endpoint descriptor included in global interface descriptor
			ep_desc = (UDC_DESC_STORAGE usb_ep_desc_t *)
					udc_next_desc_in_iface((UDC_DESC_STORAGE
					usb_conf_desc_t *)
					ep_desc, USB_DT_ENDPOINT);
			if (NULL == ep_desc) {
				break;
			}
			// Free the endpoint used by the interface
			udd_ep_free(ep_desc->bEndpointAddress);
		}
	}
#endif

	// Disable interface
	udi_api->disable();
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
static bool udc_iface_enable(uint8_t iface_num, uint8_t setting_num)
{
	// Select the interface descriptor
	if (!udc_update_iface_desc(iface_num, setting_num)) {
		return false;
	}

#if (0!=USB_DEVICE_MAX_EP)
	usb_ep_desc_t UDC_DESC_STORAGE *ep_desc;

	// Start at the beginning of the global interface descriptor
	ep_desc = (UDC_DESC_STORAGE usb_ep_desc_t *) udc_ptr_iface;
	while (1) {
		// Search Endpoint descriptor included in the global interface descriptor
		ep_desc = (UDC_DESC_STORAGE usb_ep_desc_t *)
				udc_next_desc_in_iface((UDC_DESC_STORAGE
						usb_conf_desc_t *) ep_desc,
				USB_DT_ENDPOINT);
		if (NULL == ep_desc)
			break;
		// Alloc the endpoint used by the interface
		if (!udd_ep_alloc(ep_desc->bEndpointAddress,
				ep_desc->bmAttributes,
				le16_to_cpu
				(ep_desc->wMaxPacketSize))) {
			return false;
		}
	}
#endif
	// Enable the interface
	return udc_ptr_conf->udi_apis[iface_num]->enable();
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
	udc_reset();
}

/**
 * \brief Reset the current configuration of the USB device,
 * This routines can be called by UDD when a RESET on the USB line occurs.
 */
void udc_reset(void)
{
	uint8_t iface_num;

	if (udc_num_configuration) {
		for (iface_num = 0;
				iface_num < udc_ptr_conf->desc->bNumInterfaces;
				iface_num++) {
			udc_iface_disable(iface_num);
		}
	}
	udc_num_configuration = 0;
#if (USB_CONFIG_ATTR_REMOTE_WAKEUP \
	== (USB_DEVICE_ATTR & USB_CONFIG_ATTR_REMOTE_WAKEUP))
	if (CPU_TO_LE16(USB_DEV_STATUS_REMOTEWAKEUP) & udc_device_status) {
		// Remote wakeup is enabled then disable it
		UDC_REMOTEWAKEUP_DISABLE();
	}
#endif
	udc_device_status =
#if (USB_DEVICE_ATTR & USB_CONFIG_ATTR_SELF_POWERED)
			CPU_TO_LE16(USB_DEV_STATUS_SELF_POWERED);
#else
			CPU_TO_LE16(USB_DEV_STATUS_BUS_POWERED);
#endif
}

void udc_sof_notify(void)
{
	uint8_t iface_num;

	if (udc_num_configuration) {
		for (iface_num = 0;
				iface_num < udc_ptr_conf->desc->bNumInterfaces;
				iface_num++) {
			if (udc_ptr_conf->udi_apis[iface_num]->sof_notify != NULL) {
				udc_ptr_conf->udi_apis[iface_num]->sof_notify();
			}
		}
	}
}

/**
 * \brief Standard device request to get device status
 *
 * \return true if success
 */
static bool udc_req_std_dev_get_status(void)
{
	if (udd_g_ctrlreq.req.wLength != sizeof(udc_device_status)) {
		return false;
	}

	udd_set_setup_payload( (uint8_t *) & udc_device_status,
			sizeof(udc_device_status));
	return true;
}

#if (0!=USB_DEVICE_MAX_EP)
/**
 * \brief Standard endpoint request to get endpoint status
 *
 * \return true if success
 */
static bool udc_req_std_ep_get_status(void)
{
	static le16_t udc_ep_status;

	if (udd_g_ctrlreq.req.wLength != sizeof(udc_ep_status)) {
		return false;
	}

	udc_ep_status = udd_ep_is_halted(udd_g_ctrlreq.req.
			wIndex & 0xFF) ? CPU_TO_LE16(USB_EP_STATUS_HALTED) : 0;

	udd_set_setup_payload( (uint8_t *) & udc_ep_status,
			sizeof(udc_ep_status));
	return true;
}
#endif

/**
 * \brief Standard device request to change device status
 *
 * \return true if success
 */
static bool udc_req_std_dev_clear_feature(void)
{
	if (udd_g_ctrlreq.req.wLength) {
		return false;
	}

	if (udd_g_ctrlreq.req.wValue == USB_DEV_FEATURE_REMOTE_WAKEUP) {
		udc_device_status &= CPU_TO_LE16(~(uint32_t)USB_DEV_STATUS_REMOTEWAKEUP);
#if (USB_CONFIG_ATTR_REMOTE_WAKEUP \
	== (USB_DEVICE_ATTR & USB_CONFIG_ATTR_REMOTE_WAKEUP))
		UDC_REMOTEWAKEUP_DISABLE();
#endif
		return true;
	}
	return false;
}

#if (0!=USB_DEVICE_MAX_EP)
/**
 * \brief Standard endpoint request to clear endpoint feature
 *
 * \return true if success
 */
static bool udc_req_std_ep_clear_feature(void)
{
	if (udd_g_ctrlreq.req.wLength) {
		return false;
	}

	if (udd_g_ctrlreq.req.wValue == USB_EP_FEATURE_HALT) {
		return udd_ep_clear_halt(udd_g_ctrlreq.req.wIndex & 0xFF);
	}
	return false;
}
#endif

/**
 * \brief Standard device request to set a feature
 *
 * \return true if success
 */
static bool udc_req_std_dev_set_feature(void)
{
	if (udd_g_ctrlreq.req.wLength) {
		return false;
	}

	switch (udd_g_ctrlreq.req.wValue) {

	case USB_DEV_FEATURE_REMOTE_WAKEUP:
#if (USB_CONFIG_ATTR_REMOTE_WAKEUP \
	== (USB_DEVICE_ATTR & USB_CONFIG_ATTR_REMOTE_WAKEUP))
		udc_device_status |= CPU_TO_LE16(USB_DEV_STATUS_REMOTEWAKEUP);
		UDC_REMOTEWAKEUP_ENABLE();
		return true;
#else
		return false;
#endif

#ifdef USB_DEVICE_HS_SUPPORT
	case USB_DEV_FEATURE_TEST_MODE:
		if (!udd_is_high_speed()) {
			break;
		}
		if (udd_g_ctrlreq.req.wIndex & 0xff) {
			break;
		}
		// Unconfigure the device, terminating all ongoing requests
		udc_reset();
		switch ((udd_g_ctrlreq.req.wIndex >> 8) & 0xFF) {
		case USB_DEV_TEST_MODE_J:
			udd_g_ctrlreq.callback = udd_test_mode_j;
			return true;

		case USB_DEV_TEST_MODE_K:
			udd_g_ctrlreq.callback = udd_test_mode_k;
			return true;

		case USB_DEV_TEST_MODE_SE0_NAK:
			udd_g_ctrlreq.callback = udd_test_mode_se0_nak;
			return true;

		case USB_DEV_TEST_MODE_PACKET:
			udd_g_ctrlreq.callback = udd_test_mode_packet;
			return true;

		case USB_DEV_TEST_MODE_FORCE_ENABLE: // Only for downstream facing hub ports
		default:
			break;
		}
		break;
#endif
	default:
		break;
	}
	return false;
}

/**
 * \brief Standard endpoint request to halt an endpoint
 *
 * \return true if success
 */
#if (0!=USB_DEVICE_MAX_EP)
static bool udc_req_std_ep_set_feature(void)
{
	if (udd_g_ctrlreq.req.wLength) {
		return false;
	}
	if (udd_g_ctrlreq.req.wValue == USB_EP_FEATURE_HALT) {
		udd_ep_abort(udd_g_ctrlreq.req.wIndex & 0xFF);
		return udd_ep_set_halt(udd_g_ctrlreq.req.wIndex & 0xFF);
	}
	return false;
}
#endif

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
	if (udd_g_ctrlreq.req.wLength) {
		return false;
	}

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
	const uint8_t *str;
	uint8_t str_length = 0;

	// Link payload pointer to the string corresponding at request
	switch (udd_g_ctrlreq.req.wValue & 0xff) {
	case 0:
		udd_set_setup_payload((uint8_t *) &udc_string_desc_languageid,
				sizeof(udc_string_desc_languageid));
		break;

#ifdef USB_DEVICE_MANUFACTURE_NAME
	case 1:
		str_length = USB_DEVICE_MANUFACTURE_NAME_SIZE;
		str = udc_string_manufacturer_name;
		break;
#endif
#ifdef USB_DEVICE_PRODUCT_NAME
	case 2:
		str_length = USB_DEVICE_PRODUCT_NAME_SIZE;
		str = udc_string_product_name;
		break;
#endif
#if defined USB_DEVICE_SERIAL_NAME || defined USB_DEVICE_GET_SERIAL_NAME_POINTER
	case 3:
		str_length = USB_DEVICE_SERIAL_NAME_SIZE;
		str = udc_get_string_serial_name();
		break;
#endif
	default:
#ifdef UDC_GET_EXTRA_STRING
		if (UDC_GET_EXTRA_STRING()) {
			break;
		}
#endif
		return false;
	}

	if (str_length) {
		for(i = 0; i < str_length; i++) {
			udc_string_desc.string[i] = cpu_to_le16((le16_t)str[i]);
		}

		udc_string_desc.header.bLength = 2 + (str_length) * 2;
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
	uint8_t conf_num;

	conf_num = udd_g_ctrlreq.req.wValue & 0xff;

	// Check descriptor ID
	switch ((uint8_t) (udd_g_ctrlreq.req.wValue >> 8)) {
	case USB_DT_DEVICE:
		// Device descriptor requested
#ifdef USB_DEVICE_HS_SUPPORT
		if (!udd_is_high_speed()) {
			udd_set_setup_payload(
				(uint8_t *) udc_config.confdev_hs,
				udc_config.confdev_hs->bLength);
		} else
#endif
		{
			udd_set_setup_payload(
				(uint8_t *) udc_config.confdev_lsfs,
				udc_config.confdev_lsfs->bLength);
		}
		break;

	case USB_DT_CONFIGURATION:
		// Configuration descriptor requested
#ifdef USB_DEVICE_HS_SUPPORT
		if (udd_is_high_speed()) {
			// HS descriptor
			if (conf_num >= udc_config.confdev_hs->
					bNumConfigurations) {
				return false;
			}
			udd_set_setup_payload(
				(uint8_t *)udc_config.conf_hs[conf_num].desc,
				le16_to_cpu(udc_config.conf_hs[conf_num].desc->wTotalLength));
		} else
#endif
		{
			// FS descriptor
			if (conf_num >= udc_config.confdev_lsfs->
					bNumConfigurations) {
				return false;
			}
			udd_set_setup_payload(
				(uint8_t *)udc_config.conf_lsfs[conf_num].desc,
				le16_to_cpu(udc_config.conf_lsfs[conf_num].desc->wTotalLength));
		}
		((usb_conf_desc_t *) udd_g_ctrlreq.payload)->bDescriptorType =
				USB_DT_CONFIGURATION;
		break;

#ifdef USB_DEVICE_HS_SUPPORT
	case USB_DT_DEVICE_QUALIFIER:
		// Device qualifier descriptor requested
		udd_set_setup_payload( (uint8_t *) udc_config.qualifier,
				udc_config.qualifier->bLength);
		break;

	case USB_DT_OTHER_SPEED_CONFIGURATION:
		// Other configuration descriptor requested
		if (!udd_is_high_speed()) {
			// HS descriptor
			if (conf_num >= udc_config.confdev_hs->
					bNumConfigurations) {
				return false;
			}
			udd_set_setup_payload(
				(uint8_t *)udc_config.conf_hs[conf_num].desc,
				le16_to_cpu(udc_config.conf_hs[conf_num].desc->wTotalLength));
		} else {
			// FS descriptor
			if (conf_num >= udc_config.confdev_lsfs->
					bNumConfigurations) {
				return false;
			}
			udd_set_setup_payload(
				(uint8_t *)udc_config.conf_lsfs[conf_num].desc,
				le16_to_cpu(udc_config.conf_lsfs[conf_num].desc->wTotalLength));
		}
		((usb_conf_desc_t *) udd_g_ctrlreq.payload)->bDescriptorType =
				USB_DT_OTHER_SPEED_CONFIGURATION;
		break;
#endif

	case USB_DT_BOS:
		// Device BOS descriptor requested
		if (udc_config.conf_bos == NULL) {
			return false;
		}
		udd_set_setup_payload( (uint8_t *) udc_config.conf_bos,
				udc_config.conf_bos->wTotalLength);
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
	if (udd_g_ctrlreq.req.wLength != 1) {
		return false;
	}

	udd_set_setup_payload(&udc_num_configuration,1);
	return true;
}

/**
 * \brief Standard device request to enable a configuration
 *
 * \return true if success
 */
static bool udc_req_std_dev_set_configuration(void)
{
	uint8_t iface_num;

	// Check request length
	if (udd_g_ctrlreq.req.wLength) {
		return false;
	}
	// Authorize configuration only if the address is valid
	if (!udd_getaddress()) {
		return false;
	}
	// Check the configuration number requested
#ifdef USB_DEVICE_HS_SUPPORT
	if (udd_is_high_speed()) {
		// HS descriptor
		if ((udd_g_ctrlreq.req.wValue & 0xFF) >
				udc_config.confdev_hs->bNumConfigurations) {
			return false;
		}
	} else
#endif
	{
		// FS descriptor
		if ((udd_g_ctrlreq.req.wValue & 0xFF) >
				udc_config.confdev_lsfs->bNumConfigurations) {
			return false;
		}
	}

	// Reset current configuration
	udc_reset();

	// Enable new configuration
	udc_num_configuration = udd_g_ctrlreq.req.wValue & 0xFF;
	if (udc_num_configuration == 0) {
		return true; // Default empty configuration requested
	}
	// Update pointer of the configuration descriptor
#ifdef USB_DEVICE_HS_SUPPORT
	if (udd_is_high_speed()) {
		// HS descriptor
		udc_ptr_conf = &udc_config.conf_hs[udc_num_configuration - 1];
	} else
#endif
	{
		// FS descriptor
		udc_ptr_conf = &udc_config.conf_lsfs[udc_num_configuration - 1];
	}
	// Enable all interfaces of the selected configuration
	for (iface_num = 0; iface_num < udc_ptr_conf->desc->bNumInterfaces;
			iface_num++) {
		if (!udc_iface_enable(iface_num, 0)) {
			return false;
		}
	}
	return true;
}

/**
 * \brief Standard interface request
 * to get the alternate setting number of an interface
 *
 * \return true if success
 */
static bool udc_req_std_iface_get_setting(void)
{
	uint8_t iface_num;
	udi_api_t UDC_DESC_STORAGE *udi_api;

	if (udd_g_ctrlreq.req.wLength != 1) {
		return false; // Error in request
	}
	if (!udc_num_configuration) {
		return false; // The device is not is configured state yet
	}

	// Check the interface number included in the request
	iface_num = udd_g_ctrlreq.req.wIndex & 0xFF;
	if (iface_num >= udc_ptr_conf->desc->bNumInterfaces) {
		return false;
	}

	// Select first alternate setting of the interface to update udc_ptr_iface
	// before call iface->getsetting()
	if (!udc_update_iface_desc(iface_num, 0)) {
		return false;
	}
	// Get alternate setting from UDI
	udi_api = udc_ptr_conf->udi_apis[iface_num];
	udc_iface_setting = udi_api->getsetting();

	// Link value to payload pointer of request
	udd_set_setup_payload(&udc_iface_setting,1);
	return true;
}

/**
 * \brief Standard interface request
 * to set an alternate setting of an interface
 *
 * \return true if success
 */
static bool udc_req_std_iface_set_setting(void)
{
	uint8_t iface_num, setting_num;

	if (udd_g_ctrlreq.req.wLength) {
		return false; // Error in request
	}
	if (!udc_num_configuration) {
		return false; // The device is not is configured state yet
	}

	iface_num = udd_g_ctrlreq.req.wIndex & 0xFF;
	setting_num = udd_g_ctrlreq.req.wValue & 0xFF;

	// Disable current setting
	if (!udc_iface_disable(iface_num)) {
		return false;
	}

	// Enable new setting
	return udc_iface_enable(iface_num, setting_num);
}

/**
 * \brief Main routine to manage the standard USB SETUP request
 *
 * \return true if the request is supported
 */
static bool udc_reqstd(void)
{
	if (Udd_setup_is_in()) {
		// GET Standard Requests
		if (udd_g_ctrlreq.req.wLength == 0) {
			return false; // Error for USB host
		}

		if (USB_REQ_RECIP_DEVICE == Udd_setup_recipient()) {
			// Standard Get Device request
			switch (udd_g_ctrlreq.req.bRequest) {
			case USB_REQ_GET_STATUS:
				return udc_req_std_dev_get_status();
			case USB_REQ_GET_DESCRIPTOR:
				return udc_req_std_dev_get_descriptor();
			case USB_REQ_GET_CONFIGURATION:
				return udc_req_std_dev_get_configuration();
			default:
				break;
			}
		}

		if (USB_REQ_RECIP_INTERFACE == Udd_setup_recipient()) {
			// Standard Get Interface request
			switch (udd_g_ctrlreq.req.bRequest) {
			case USB_REQ_GET_INTERFACE:
				return udc_req_std_iface_get_setting();
			default:
				break;
			}
		}
#if (0!=USB_DEVICE_MAX_EP)
		if (USB_REQ_RECIP_ENDPOINT == Udd_setup_recipient()) {
			// Standard Get Endpoint request
			switch (udd_g_ctrlreq.req.bRequest) {
			case USB_REQ_GET_STATUS:
				return udc_req_std_ep_get_status();
			default:
				break;
			}
		}
#endif
	} else {
		// SET Standard Requests
		if (USB_REQ_RECIP_DEVICE == Udd_setup_recipient()) {
			// Standard Set Device request
			switch (udd_g_ctrlreq.req.bRequest) {
			case USB_REQ_SET_ADDRESS:
				return udc_req_std_dev_set_address();
			case USB_REQ_CLEAR_FEATURE:
				return udc_req_std_dev_clear_feature();
			case USB_REQ_SET_FEATURE:
				return udc_req_std_dev_set_feature();
			case USB_REQ_SET_CONFIGURATION:
				return udc_req_std_dev_set_configuration();
			case USB_REQ_SET_DESCRIPTOR:
				/* Not supported (defined as optional by the USB 2.0 spec) */
				break;
			default:
				break;
			}
		}

		if (USB_REQ_RECIP_INTERFACE == Udd_setup_recipient()) {
			// Standard Set Interface request
			switch (udd_g_ctrlreq.req.bRequest) {
			case USB_REQ_SET_INTERFACE:
				return udc_req_std_iface_set_setting();
			default:
				break;
			}
		}
#if (0!=USB_DEVICE_MAX_EP)
		if (USB_REQ_RECIP_ENDPOINT == Udd_setup_recipient()) {
			// Standard Set Endpoint request
			switch (udd_g_ctrlreq.req.bRequest) {
			case USB_REQ_CLEAR_FEATURE:
				return udc_req_std_ep_clear_feature();
			case USB_REQ_SET_FEATURE:
				return udc_req_std_ep_set_feature();
			default:
				break;
			}
		}
#endif
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
	uint8_t iface_num;
	udi_api_t UDC_DESC_STORAGE *udi_api;

	if (0 == udc_num_configuration) {
		return false; // The device is not is configured state yet
	}
	// Check interface number
	iface_num = udd_g_ctrlreq.req.wIndex & 0xFF;
	if (iface_num >= udc_ptr_conf->desc->bNumInterfaces) {
		return false;
	}

	//* To update udc_ptr_iface with the selected interface in request
	// Select first alternate setting of interface to update udc_ptr_iface
	// before calling udi_api->getsetting()
	if (!udc_update_iface_desc(iface_num, 0)) {
		return false;
	}
	// Select the interface with the current alternate setting
	udi_api = udc_ptr_conf->udi_apis[iface_num];
	if (!udc_update_iface_desc(iface_num, udi_api->getsetting())) {
		return false;
	}

	// Send the SETUP request to the UDI corresponding to the interface number
	return udi_api->setup();
}

/**
 * \brief Send the SETUP interface request to UDI
 *
 * \return true if the request is supported
 */
static bool udc_req_ep(void)
{
	uint8_t iface_num;
	udi_api_t UDC_DESC_STORAGE *udi_api;

	if (0 == udc_num_configuration) {
		return false; // The device is not is configured state yet
	}
	// Send this request on all enabled interfaces
	iface_num = udd_g_ctrlreq.req.wIndex & 0xFF;
	for (iface_num = 0; iface_num < udc_ptr_conf->desc->bNumInterfaces;
			iface_num++) {
		// Select the interface with the current alternate setting
		udi_api = udc_ptr_conf->udi_apis[iface_num];
		if (!udc_update_iface_desc(iface_num, udi_api->getsetting())) {
			return false;
		}

		// Send the SETUP request to the UDI
		if (udi_api->setup()) {
			return true;
		}
	}
	return false;
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

	// If endpoint request then try to decode it in UDI
	if (Udd_setup_recipient() == USB_REQ_RECIP_ENDPOINT) {
		if (udc_req_ep()) {
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
