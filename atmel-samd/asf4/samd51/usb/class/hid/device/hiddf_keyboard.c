/**
 * \file
 *
 * \brief USB Device Stack HID Keyboard Function Implementation.
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
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
 *    Atmel micro controller product.
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

#include "hiddf_keyboard.h"
#include <string.h>

#define HIDDF_KEYBOARD_VERSION 0x00000001u

#define KEYBOARD_REPORT_DESC_LEN 59

/** USB Device HID Keyboard Function Specific Data */
struct hiddf_keyboard_func_data {
	/* HID Device Keyboard Report */
	uint8_t kb_report[8];
	/** HID Device Keyboard Interface information */
	uint8_t func_iface;
	/** HID Device Keyboard IN Endpoint */
	uint8_t func_ep_in;
	/** HID Device Keyboard OUT Endpoint */
	uint8_t func_ep_out;
	/** HID Device Keyboard Enable Flag */
	bool enabled;
};

/* HID report descriptor for standard HID keyboard */
const uint8_t keyboard_report_desc[KEYBOARD_REPORT_DESC_LEN] = {
    0x05, 0x01, /* Usage Page (Generic Desktop)      */
    0x09, 0x06, /* Usage (Keyboard)                  */
    0xA1, 0x01, /* Collection (Application)          */
    0x05, 0x07, /* Usage Page (Keyboard)             */
    0x19, 0xE0, /* Usage Minimum (224)               */
    0x29, 0xE7, /* Usage Maximum (231)               */
    0x15, 0x00, /* Logical Minimum (0)               */
    0x25, 0x01, /* Logical Maximum (1)               */
    0x75, 0x01, /* Report Size (1)                   */
    0x95, 0x08, /* Report Count (8)                  */
    0x81, 0x02, /* Input (Data, Variable, Absolute)  */
    0x81, 0x01, /* Input (Constant)                  */
    0x19, 0x00, /* Usage Minimum (0)                 */
    0x29, 0x65, /* Usage Maximum (101)               */
    0x15, 0x00, /* Logical Minimum (0)               */
    0x25, 0x65, /* Logical Maximum (101)             */
    0x75, 0x08, /* Report Size (8)                   */
    0x95, 0x06, /* Report Count (6)                  */
    0x81, 0x00, /* Input (Data, Array)               */
    0x05, 0x08, /* Usage Page (LED)                  */
    0x19, 0x01, /* Usage Minimum (1)                 */
    0x29, 0x05, /* Usage Maximum (5)                 */
    0x15, 0x00, /* Logical Minimum (0)               */
    0x25, 0x01, /* Logical Maximum (1)               */
    0x75, 0x01, /* Report Size (1)                   */
    0x95, 0x05, /* Report Count (5)                  */
    0x91, 0x02, /* Output (Data, Variable, Absolute) */
    0x95, 0x03, /* Report Count (3)                  */
    0x91, 0x01, /* Output (Constant)                 */
    0xC0        /* End Collection                    */
};

/* USB Device HID Keyboard Function Instance */
static struct usbdf_driver _hiddf_keyboard;

/* USB Device HID Keyboard Function Data Instance */
static struct hiddf_keyboard_func_data _hiddf_keyboard_funcd;

/**
 * \brief Enable HID Keyboard Function
 * \param[in] drv Pointer to USB device function driver
 * \param[in] desc Pointer to USB interface descriptor
 * \return Operation status.
 */
static int32_t hid_keyboard_enable(struct usbdf_driver *drv, struct usbd_descriptors *desc)
{
	uint8_t *        ifc, *ep, i;
	usb_iface_desc_t ifc_desc;
	usb_ep_desc_t    ep_desc;

	struct hiddf_keyboard_func_data *func_data = (struct hiddf_keyboard_func_data *)(drv->func_data);

	ifc = desc->sod;
	if (NULL == ifc) {
		return ERR_NOT_FOUND;
	}

	ifc_desc.bInterfaceNumber = ifc[2];
	ifc_desc.bInterfaceClass  = ifc[5];

	if (HID_CLASS == ifc_desc.bInterfaceClass) {
		if (func_data->func_iface == ifc_desc.bInterfaceNumber) { // Initialized
			return ERR_ALREADY_INITIALIZED;
		} else if (func_data->func_iface != 0xFF) { // Occupied
			return ERR_NO_RESOURCE;
		} else {
			func_data->func_iface = ifc_desc.bInterfaceNumber;
		}
	} else { // Not supported by this function driver
		return ERR_NOT_FOUND;
	}

	// Install endpoints
	for (i = 0; i < 2; i++) {
		ep        = usb_find_ep_desc(usb_desc_next(desc->sod), desc->eod);
		desc->sod = ep;
		if (NULL != ep) {
			ep_desc.bEndpointAddress = ep[2];
			ep_desc.bmAttributes     = ep[3];
			ep_desc.wMaxPacketSize   = usb_get_u16(ep + 4);
			if (usb_d_ep_init(ep_desc.bEndpointAddress, ep_desc.bmAttributes, ep_desc.wMaxPacketSize)) {
				return ERR_NOT_INITIALIZED;
			}
			if (ep_desc.bEndpointAddress & USB_EP_DIR_IN) {
				func_data->func_ep_in = ep_desc.bEndpointAddress;
				usb_d_ep_enable(func_data->func_ep_in);
			} else {
				func_data->func_ep_out = ep_desc.bEndpointAddress;
				usb_d_ep_enable(func_data->func_ep_out);
			}
		} else {
			return ERR_NOT_FOUND;
		}
	}

	// Installed
	_hiddf_keyboard_funcd.enabled = true;
	return ERR_NONE;
}

/**
 * \brief Disable HID Keyboard Function
 * \param[in] drv Pointer to USB device function driver
 * \param[in] desc Pointer to USB device descriptor
 * \return Operation status.
 */
static int32_t hid_keyboard_disable(struct usbdf_driver *drv, struct usbd_descriptors *desc)
{
	struct hiddf_keyboard_func_data *func_data = (struct hiddf_keyboard_func_data *)(drv->func_data);

	usb_iface_desc_t ifc_desc;

	if (desc) {
		ifc_desc.bInterfaceClass = desc->sod[5];
		if (ifc_desc.bInterfaceClass != HID_CLASS) {
			return ERR_NOT_FOUND;
		}
	}

	if (func_data->func_iface != 0xFF) {
		func_data->func_iface = 0xFF;
	}

	if (func_data->func_ep_in != 0xFF) {
		usb_d_ep_deinit(func_data->func_ep_in);
		func_data->func_ep_in = 0xFF;
	}

	if (func_data->func_ep_out != 0xFF) {
		usb_d_ep_deinit(func_data->func_ep_out);
		func_data->func_ep_out = 0xFF;
	}

	_hiddf_keyboard_funcd.enabled = false;
	return ERR_NONE;
}

/**
 * \brief HID Keyboard Control Function
 * \param[in] drv Pointer to USB device function driver
 * \param[in] ctrl USB device general function control type
 * \param[in] param Parameter pointer
 * \return Operation status.
 */
static int32_t hid_keyboard_ctrl(struct usbdf_driver *drv, enum usbdf_control ctrl, void *param)
{
	switch (ctrl) {
	case USBDF_ENABLE:
		return hid_keyboard_enable(drv, (struct usbd_descriptors *)param);

	case USBDF_DISABLE:
		return hid_keyboard_disable(drv, (struct usbd_descriptors *)param);

	case USBDF_GET_IFACE:
		return ERR_UNSUPPORTED_OP;

	default:
		return ERR_INVALID_ARG;
	}
}

/**
 * \brief Process the HID class get descriptor
 * \param[in] ep Endpoint address.
 * \param[in] req Pointer to the request.
 * \return Operation status.
 */
static int32_t hid_keyboard_get_desc(uint8_t ep, struct usb_req *req)
{
	switch (req->wValue >> 8) {
	case USB_DT_HID_REPORT:
		return usbdc_xfer(ep, (uint8_t *)keyboard_report_desc, KEYBOARD_REPORT_DESC_LEN, false);
	default:
		return ERR_INVALID_ARG;
	}
}

/**
 * \brief Process the HID class request
 * \param[in] ep Endpoint address.
 * \param[in] req Pointer to the request.
 * \return Operation status.
 */
static int32_t hid_keyboard_req(uint8_t ep, struct usb_req *req, enum usb_ctrl_stage stage)
{
	if ((0x81 == req->bmRequestType) && (0x06 == req->bRequest) && (req->wIndex == _hiddf_keyboard_funcd.func_iface)) {
		return hid_keyboard_get_desc(ep, req);
	} else {
		if (0x01 != ((req->bmRequestType >> 5) & 0x03)) { // class request
			return ERR_NOT_FOUND;
		}
		if (req->wIndex == _hiddf_keyboard_funcd.func_iface) {
			if (req->bmRequestType & USB_EP_DIR_IN) {
				return ERR_INVALID_ARG;
			} else {
				return usbdc_xfer(0, NULL, 0, 0);
			}
		} else {
			return ERR_NOT_FOUND;
		}
	}
	(void)stage;
}

/** USB Device HID Keyboard Handler Struct */
static struct usbdc_handler hid_keyboard_req_h = {NULL, (FUNC_PTR)hid_keyboard_req};

/**
 * \brief Initialize the USB HID Keyboard Function Driver
 */
int32_t hiddf_keyboard_init(void)
{
	if (usbdc_get_state() > USBD_S_POWER) {
		return ERR_DENIED;
	}

	_hiddf_keyboard.ctrl      = hid_keyboard_ctrl;
	_hiddf_keyboard.func_data = &_hiddf_keyboard_funcd;

	usbdc_register_function(&_hiddf_keyboard);
	usbdc_register_handler(USBDC_HDL_REQ, &hid_keyboard_req_h);
	return ERR_NONE;
}

/**
 * \brief Deinitialize the USB HID Keyboard Function Driver
 */
int32_t hiddf_keyboard_deinit(void)
{
	if (usbdc_get_state() > USBD_S_POWER) {
		return ERR_DENIED;
	}

	_hiddf_keyboard.ctrl      = NULL;
	_hiddf_keyboard.func_data = NULL;

	usbdc_unregister_function(&_hiddf_keyboard);
	usbdc_unregister_handler(USBDC_HDL_REQ, &hid_keyboard_req_h);
	return ERR_NONE;
}

/**
 * \brief Check whether HID Keyboard Function is enabled
 */
bool hiddf_keyboard_is_enabled(void)
{
	return _hiddf_keyboard_funcd.enabled;
}

/**
 * \brief Process the HID Keyboard key state change request
 * \param keys_desc[]  keys_descriptor array for state changing
 * \param keys_count   total keys amount for state changing
 * \return Operation status.
 */
int32_t hiddf_keyboard_keys_state_change(struct hiddf_kb_key_descriptors keys_desc[], uint8_t keys_count)
{
	uint8_t i, j;
	uint8_t modifier_keys, regular_keys;

	if (!hiddf_keyboard_is_enabled()) {
		return ERR_DENIED;
	}

	memset(_hiddf_keyboard_funcd.kb_report, 0x00, 8);
	modifier_keys = 0;

	for (i = 0; i < keys_count; i++) {
		if (true == keys_desc[i].b_modifier) {
			modifier_keys++;
		}
	}

	regular_keys = keys_count - modifier_keys;

	if (regular_keys > 6) {
		memset(&_hiddf_keyboard_funcd.kb_report[2], 0xFF, 6);
	} else {
		i = 2;
		for (j = 0; j < keys_count; j++) {
			if (HID_KB_KEY_DOWN == keys_desc[j].state) {
				if (true == keys_desc[j].b_modifier) {
					_hiddf_keyboard_funcd.kb_report[0] |= keys_desc[j].key_id;
				} else {
					_hiddf_keyboard_funcd.kb_report[i++] = keys_desc[j].key_id;
				}
			}
		}
	}
	return usbdc_xfer(_hiddf_keyboard_funcd.func_ep_in, &_hiddf_keyboard_funcd.kb_report[0], 8, false);
}

/**
 * \brief Return version
 */
uint32_t hiddf_keyboard_get_version(void)
{
	return HIDDF_KEYBOARD_VERSION;
}
