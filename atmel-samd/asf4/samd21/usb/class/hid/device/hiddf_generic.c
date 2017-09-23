/**
 * \file
 *
 * \brief USB Device Stack HID Generic Function Implementation.
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

#include "hiddf_generic.h"
#include <string.h>

#define HIDDF_GENERIC_VERSION 0x00000001u

/** USB Device HID Generic Function Specific Data */
struct hiddf_generic_func_data {
	/** HID Device Generic Interface information */
	uint8_t func_iface;
	/** HID Device Generic IN Endpoint */
	uint8_t func_ep_in;
	/** HID Device Generic OUT Endpoint */
	uint8_t func_ep_out;
	/** HID Device Generic Report Descriptor */
	const uint8_t *report_desc;
	/** HID Device Generic Report Descriptor Length */
	uint32_t report_desc_len;
	/** HID Device Generic Enable Flag */
	bool enabled;
};

/* USB Device HID Generic Function Instance */
static struct usbdf_driver _hiddf_generic;

/* USB Device HID Generic Function Data Instance */
static struct hiddf_generic_func_data _hiddf_generic_funcd;

/* USB Device HID Generic Set Report Function Callback */
static hiddf_generic_set_report_t hiddf_generic_set_report = NULL;

/**
 * \brief Enable HID Generic Function
 * \param[in] drv Pointer to USB device function driver
 * \param[in] desc Pointer to USB interface descriptor
 * \return Operation status.
 */
static int32_t hid_generic_enable(struct usbdf_driver *drv, struct usbd_descriptors *desc)
{
	uint8_t *        ifc, *ep, i;
	usb_iface_desc_t ifc_desc;
	usb_ep_desc_t    ep_desc;

	struct hiddf_generic_func_data *func_data = (struct hiddf_generic_func_data *)(drv->func_data);

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
	_hiddf_generic_funcd.enabled = true;
	return ERR_NONE;
}

/**
 * \brief Disable HID Generic Function
 * \param[in] drv Pointer to USB device function driver
 * \param[in] desc Pointer to USB device descriptor
 * \return Operation status.
 */
static int32_t hid_generic_disable(struct usbdf_driver *drv, struct usbd_descriptors *desc)
{
	struct hiddf_generic_func_data *func_data = (struct hiddf_generic_func_data *)(drv->func_data);

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

	_hiddf_generic_funcd.enabled = false;
	return ERR_NONE;
}

/**
 * \brief HID Generic Control Function
 * \param[in] drv Pointer to USB device function driver
 * \param[in] ctrl USB device general function control type
 * \param[in] param Parameter pointer
 * \return Operation status.
 */
static int32_t hid_generic_ctrl(struct usbdf_driver *drv, enum usbdf_control ctrl, void *param)
{
	switch (ctrl) {
	case USBDF_ENABLE:
		return hid_generic_enable(drv, (struct usbd_descriptors *)param);

	case USBDF_DISABLE:
		return hid_generic_disable(drv, (struct usbd_descriptors *)param);

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
static int32_t hid_generic_get_desc(uint8_t ep, struct usb_req *req)
{
	switch (req->wValue >> 8) {
	case USB_DT_HID_REPORT:
		return usbdc_xfer(ep, (uint8_t *)_hiddf_generic_funcd.report_desc, _hiddf_generic_funcd.report_desc_len, false);
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
static int32_t hid_generic_req(uint8_t ep, struct usb_req *req, enum usb_ctrl_stage stage)
{
	uint8_t *ctrl_buf = usbdc_get_ctrl_buffer();
	uint16_t len      = req->wLength;

	if ((0x81 == req->bmRequestType) && (0x06 == req->bRequest) && (req->wIndex == _hiddf_generic_funcd.func_iface)) {
		return hid_generic_get_desc(ep, req);
	} else {
		if (0x01 != ((req->bmRequestType >> 5) & 0x03)) { // class request
			return ERR_NOT_FOUND;
		}
		if (req->wIndex == _hiddf_generic_funcd.func_iface) {
			if (req->bmRequestType & USB_EP_DIR_IN) {
				return ERR_INVALID_ARG;
			} else {
				if (USB_REQ_HID_SET_REPORT == req->bRequest) {
					if (USB_SETUP_STAGE == stage) {
						return usbdc_xfer(ep, ctrl_buf, len, false);
					} else {
						if (NULL != hiddf_generic_set_report) {
							hiddf_generic_set_report(ctrl_buf, len);
						}
						return ERR_NONE;
					}
				} else {
					return usbdc_xfer(0, NULL, 0, 0);
				}
			}
		} else {
			return ERR_NOT_FOUND;
		}
	}
}

/** USB Device HID Generic Handler Struct */
static struct usbdc_handler hid_generic_req_h = {NULL, (FUNC_PTR)hid_generic_req};

/**
 * \brief Initialize the USB HID Generic Function Driver
 */
int32_t hiddf_generic_init(const uint8_t *report_desc, uint32_t len)
{
	if (NULL == report_desc || 0 == len) {
		return ERR_INVALID_ARG;
	}

	if (usbdc_get_state() > USBD_S_POWER) {
		return ERR_DENIED;
	}

	_hiddf_generic_funcd.report_desc     = report_desc;
	_hiddf_generic_funcd.report_desc_len = len;
	_hiddf_generic.ctrl                  = hid_generic_ctrl;
	_hiddf_generic.func_data             = &_hiddf_generic_funcd;

	usbdc_register_function(&_hiddf_generic);
	usbdc_register_handler(USBDC_HDL_REQ, &hid_generic_req_h);

	return ERR_NONE;
}

/**
 * \brief Deinitialize the USB HID Generic Function Driver
 */
int32_t hiddf_generic_deinit(void)
{
	if (usbdc_get_state() > USBD_S_POWER) {
		return ERR_DENIED;
	}

	_hiddf_generic.ctrl      = NULL;
	_hiddf_generic.func_data = NULL;

	usbdc_unregister_function(&_hiddf_generic);
	usbdc_unregister_handler(USBDC_HDL_REQ, &hid_generic_req_h);
	return ERR_NONE;
}

/**
 * \brief Check whether HID Generic Function is enabled
 */
bool hiddf_generic_is_enabled(void)
{
	return _hiddf_generic_funcd.enabled;
}

/**
 * \brief USB HID Generic Function Read Data
 */
int32_t hiddf_generic_read(uint8_t *buf, uint32_t size)
{
	if (!hiddf_generic_is_enabled()) {
		return ERR_DENIED;
	}
	return usbdc_xfer(_hiddf_generic_funcd.func_ep_out, buf, size, false);
}

/**
 * \brief USB HID Generic Function Write Data
 */
int32_t hiddf_generic_write(uint8_t *buf, uint32_t size)
{
	if (!hiddf_generic_is_enabled()) {
		return ERR_DENIED;
	}
	return usbdc_xfer(_hiddf_generic_funcd.func_ep_in, buf, size, false);
}

/**
 * \brief USB HID Generic Function Register Callback
 */
int32_t hiddf_generic_register_callback(enum hiddf_generic_cb_type cb_type, FUNC_PTR func)
{
	if (!hiddf_generic_is_enabled()) {
		return ERR_DENIED;
	}
	switch (cb_type) {
	case HIDDF_GENERIC_CB_READ:
		usb_d_ep_register_callback(_hiddf_generic_funcd.func_ep_out, USB_D_EP_CB_XFER, func);
		break;
	case HIDDF_GENERIC_CB_WRITE:
		usb_d_ep_register_callback(_hiddf_generic_funcd.func_ep_in, USB_D_EP_CB_XFER, func);
		break;
	case HIDDF_GENERIC_CB_SET_REPORT:
		hiddf_generic_set_report = (hiddf_generic_set_report_t)func;
		break;
	default:
		return ERR_INVALID_ARG;
	}

	return ERR_NONE;
}

/**
 * \brief Return version
 */
uint32_t hiddf_generic_get_version(void)
{
	return HIDDF_GENERIC_VERSION;
}
