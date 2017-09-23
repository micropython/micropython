/**
 * \file
 *
 * \brief USB Device Stack CDC ACM Function Implementation.
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

#include "cdcdf_acm.h"

#define CDCDF_ACM_VERSION 0x00000001u
#define CDCDF_ACM_COMM_EP_INDEX 0
#define CDCDF_ACM_DATA_EP_INDEX 1

/** USB Device CDC ACM Fucntion Specific Data */
struct cdcdf_acm_func_data {
	/** CDC Device ACM Interface information */
	uint8_t func_iface[2];
	/** CDC Device ACM IN Endpoint */
	uint8_t func_ep_in[2];
	/** CDC Device ACM OUT Endpoint */
	uint8_t func_ep_out;
	/** CDC Device ACM Enable Flag */
	bool enabled;
};

static struct usbdf_driver        _cdcdf_acm;
static struct cdcdf_acm_func_data _cdcdf_acm_funcd;
static struct usb_cdc_line_coding usbd_cdc_line_coding;

static cdcdf_acm_notify_state_t    cdcdf_acm_notify_state    = NULL;
static cdcdf_acm_set_line_coding_t cdcdf_acm_set_line_coding = NULL;

/**
 * \brief Enable CDC ACM Function
 * \param[in] drv Pointer to USB device function driver
 * \param[in] desc Pointer to USB interface descriptor
 * \return Operation status.
 */
static int32_t cdcdf_acm_enable(struct usbdf_driver *drv, struct usbd_descriptors *desc)
{
	struct cdcdf_acm_func_data *func_data = (struct cdcdf_acm_func_data *)(drv->func_data);

	usb_ep_desc_t    ep_desc;
	usb_iface_desc_t ifc_desc;
	uint8_t *        ifc, *ep;
	uint8_t          i;

	ifc = desc->sod;
	for (i = 0; i < 2; i++) {
		if (NULL == ifc) {
			return ERR_NOT_FOUND;
		}

		ifc_desc.bInterfaceNumber = ifc[2];
		ifc_desc.bInterfaceClass  = ifc[5];

		if ((CDC_CLASS_COMM == ifc_desc.bInterfaceClass) || (CDC_CLASS_DATA == ifc_desc.bInterfaceClass)) {
			if (func_data->func_iface[i] == ifc_desc.bInterfaceNumber) { // Initialized
				return ERR_ALREADY_INITIALIZED;
			} else if (func_data->func_iface[i] != 0xFF) { // Occupied
				return ERR_NO_RESOURCE;
			} else {
				func_data->func_iface[i] = ifc_desc.bInterfaceNumber;
			}
		} else { // Not supported by this function driver
			return ERR_NOT_FOUND;
		}

		// Install endpoints
		ep = usb_find_desc(ifc, desc->eod, USB_DT_ENDPOINT);
		while (NULL != ep) {
			ep_desc.bEndpointAddress = ep[2];
			ep_desc.bmAttributes     = ep[3];
			ep_desc.wMaxPacketSize   = usb_get_u16(ep + 4);
			if (usb_d_ep_init(ep_desc.bEndpointAddress, ep_desc.bmAttributes, ep_desc.wMaxPacketSize)) {
				return ERR_NOT_INITIALIZED;
			}
			if (ep_desc.bEndpointAddress & USB_EP_DIR_IN) {
				func_data->func_ep_in[i] = ep_desc.bEndpointAddress;
				usb_d_ep_enable(func_data->func_ep_in[i]);
			} else {
				func_data->func_ep_out = ep_desc.bEndpointAddress;
				usb_d_ep_enable(func_data->func_ep_out);
			}
			desc->sod = ep;
			ep        = usb_find_ep_desc(usb_desc_next(desc->sod), desc->eod);
		}
		ifc = usb_find_desc(usb_desc_next(desc->sod), desc->eod, USB_DT_INTERFACE);
	}
	// Installed
	_cdcdf_acm_funcd.enabled = true;
	return ERR_NONE;
}

/**
 * \brief Disable CDC ACM Function
 * \param[in] drv Pointer to USB device function driver
 * \param[in] desc Pointer to USB device descriptor
 * \return Operation status.
 */
static int32_t cdcdf_acm_disable(struct usbdf_driver *drv, struct usbd_descriptors *desc)
{
	struct cdcdf_acm_func_data *func_data = (struct cdcdf_acm_func_data *)(drv->func_data);

	usb_iface_desc_t ifc_desc;
	uint8_t          i;

	if (desc) {
		ifc_desc.bInterfaceClass = desc->sod[5];
		// Check interface
		if ((ifc_desc.bInterfaceClass != CDC_CLASS_COMM) && (ifc_desc.bInterfaceClass != CDC_CLASS_DATA)) {
			return ERR_NOT_FOUND;
		}
	}

	for (i = 0; i < 2; i++) {
		if (func_data->func_iface[i] == 0xFF) {
			continue;
		} else {
			func_data->func_iface[i] = 0xFF;
			if (func_data->func_ep_in[i] != 0xFF) {
				usb_d_ep_deinit(func_data->func_ep_in[i]);
				func_data->func_ep_in[i] = 0xFF;
			}
		}
	}

	if (func_data->func_ep_out != 0xFF) {
		usb_d_ep_deinit(func_data->func_ep_out);
		func_data->func_ep_out = 0xFF;
	}

	_cdcdf_acm_funcd.enabled = false;
	return ERR_NONE;
}

/**
 * \brief CDC ACM Control Function
 * \param[in] drv Pointer to USB device function driver
 * \param[in] ctrl USB device general function control type
 * \param[in] param Parameter pointer
 * \return Operation status.
 */
static int32_t cdcdf_acm_ctrl(struct usbdf_driver *drv, enum usbdf_control ctrl, void *param)
{
	switch (ctrl) {
	case USBDF_ENABLE:
		return cdcdf_acm_enable(drv, (struct usbd_descriptors *)param);

	case USBDF_DISABLE:
		return cdcdf_acm_disable(drv, (struct usbd_descriptors *)param);

	case USBDF_GET_IFACE:
		return ERR_UNSUPPORTED_OP;

	default:
		return ERR_INVALID_ARG;
	}
}

/**
 * \brief Process the CDC class set request
 * \param[in] ep Endpoint address.
 * \param[in] req Pointer to the request.
 * \return Operation status.
 */
static int32_t cdcdf_acm_set_req(uint8_t ep, struct usb_req *req, enum usb_ctrl_stage stage)
{
	struct usb_cdc_line_coding line_coding_tmp;
	uint16_t                   len      = req->wLength;
	uint8_t *                  ctrl_buf = usbdc_get_ctrl_buffer();

	switch (req->bRequest) {
	case USB_REQ_CDC_SET_LINE_CODING:
		if (sizeof(struct usb_cdc_line_coding) != len) {
			return ERR_INVALID_DATA;
		}
		if (USB_SETUP_STAGE == stage) {
			return usbdc_xfer(ep, ctrl_buf, len, false);
		} else {
			memcpy(&line_coding_tmp, ctrl_buf, sizeof(struct usb_cdc_line_coding));
			if ((NULL == cdcdf_acm_set_line_coding) || (true == cdcdf_acm_set_line_coding(&line_coding_tmp))) {
				usbd_cdc_line_coding = line_coding_tmp;
			}
			return ERR_NONE;
		}
	case USB_REQ_CDC_SET_CONTROL_LINE_STATE:
		usbdc_xfer(0, NULL, 0, 0);
		if (NULL != cdcdf_acm_notify_state) {
			cdcdf_acm_notify_state(req->wValue);
		}
		return ERR_NONE;
	default:
		return ERR_INVALID_ARG;
	}
}

/**
 * \brief Process the CDC class get request
 * \param[in] ep Endpoint address.
 * \param[in] req Pointer to the request.
 * \return Operation status.
 */
static int32_t cdcdf_acm_get_req(uint8_t ep, struct usb_req *req, enum usb_ctrl_stage stage)
{
	uint16_t len = req->wLength;

	if (USB_DATA_STAGE == stage) {
		return ERR_NONE;
	}

	switch (req->bRequest) {
	case USB_REQ_CDC_GET_LINE_CODING:
		if (sizeof(struct usb_cdc_line_coding) != len) {
			return ERR_INVALID_DATA;
		}
		return usbdc_xfer(ep, (uint8_t *)&usbd_cdc_line_coding, len, false);
	default:
		return ERR_INVALID_ARG;
	}
}

/**
 * \brief Process the CDC class request
 * \param[in] ep Endpoint address.
 * \param[in] req Pointer to the request.
 * \return Operation status.
 */
static int32_t cdcdf_acm_req(uint8_t ep, struct usb_req *req, enum usb_ctrl_stage stage)
{
	if (0x01 != ((req->bmRequestType >> 5) & 0x03)) { // class request
		return ERR_NOT_FOUND;
	}
	if ((req->wIndex == _cdcdf_acm_funcd.func_iface[0]) || (req->wIndex == _cdcdf_acm_funcd.func_iface[1])) {
		if (req->bmRequestType & USB_EP_DIR_IN) {
			return cdcdf_acm_get_req(ep, req, stage);
		} else {
			return cdcdf_acm_set_req(ep, req, stage);
		}
	} else {
		return ERR_NOT_FOUND;
	}
}

/** USB Device CDC ACM Handler Struct */
static struct usbdc_handler cdcdf_acm_req_h = {NULL, (FUNC_PTR)cdcdf_acm_req};

/**
 * \brief Initialize the USB CDC ACM Function Driver
 */
int32_t cdcdf_acm_init(void)
{
	if (usbdc_get_state() > USBD_S_POWER) {
		return ERR_DENIED;
	}

	_cdcdf_acm.ctrl      = cdcdf_acm_ctrl;
	_cdcdf_acm.func_data = &_cdcdf_acm_funcd;

	usbdc_register_function(&_cdcdf_acm);
	usbdc_register_handler(USBDC_HDL_REQ, &cdcdf_acm_req_h);
	return ERR_NONE;
}

/**
 * \brief Deinitialize the USB CDC ACM Function Driver
 */
void cdcdf_acm_deinit(void)
{
	usb_d_ep_deinit(_cdcdf_acm_funcd.func_ep_in[CDCDF_ACM_COMM_EP_INDEX]);
	usb_d_ep_deinit(_cdcdf_acm_funcd.func_ep_in[CDCDF_ACM_DATA_EP_INDEX]);
	usb_d_ep_deinit(_cdcdf_acm_funcd.func_ep_out);
}

/**
 * \brief USB CDC ACM Function Read Data
 */
int32_t cdcdf_acm_read(uint8_t *buf, uint32_t size)
{
	if (!cdcdf_acm_is_enabled()) {
		return ERR_DENIED;
	}
	return usbdc_xfer(_cdcdf_acm_funcd.func_ep_out, buf, size, false);
}

/**
 * \brief USB CDC ACM Function Write Data
 */
int32_t cdcdf_acm_write(uint8_t *buf, uint32_t size)
{
	if (!cdcdf_acm_is_enabled()) {
		return ERR_DENIED;
	}
	return usbdc_xfer(_cdcdf_acm_funcd.func_ep_in[CDCDF_ACM_DATA_EP_INDEX], buf, size, true);
}

/**
 * \brief USB CDC ACM Stop the data transfer
 */
void cdcdf_acm_stop_xfer(void)
{
	/* Stop transfer. */
	usb_d_ep_abort(_cdcdf_acm_funcd.func_ep_in[CDCDF_ACM_DATA_EP_INDEX]);
	usb_d_ep_abort(_cdcdf_acm_funcd.func_ep_out);
}

/**
 * \brief USB CDC ACM Function Register Callback
 */
int32_t cdcdf_acm_register_callback(enum cdcdf_acm_cb_type cb_type, FUNC_PTR func)
{
	switch (cb_type) {
	case CDCDF_ACM_CB_READ:
		usb_d_ep_register_callback(_cdcdf_acm_funcd.func_ep_out, USB_D_EP_CB_XFER, func);
		break;
	case CDCDF_ACM_CB_WRITE:
		usb_d_ep_register_callback(_cdcdf_acm_funcd.func_ep_in[CDCDF_ACM_DATA_EP_INDEX], USB_D_EP_CB_XFER, func);
		break;
	case CDCDF_ACM_CB_LINE_CODING_C:
		cdcdf_acm_set_line_coding = (cdcdf_acm_set_line_coding_t)func;
		break;
	case CDCDF_ACM_CB_STATE_C:
		cdcdf_acm_notify_state = (cdcdf_acm_notify_state_t)func;
		break;
	default:
		return ERR_INVALID_ARG;
	}
	return ERR_NONE;
}

/**
 * \brief Check whether CDC ACM Function is enabled
 */
bool cdcdf_acm_is_enabled(void)
{
	return _cdcdf_acm_funcd.enabled;
}

/**
 * \brief Return the CDC ACM line coding structure start address
 */
const struct usb_cdc_line_coding *cdcdf_acm_get_line_coding(void)
{
	return (const struct usb_cdc_line_coding *)&usbd_cdc_line_coding;
}

/**
 * \brief Return version
 */
uint32_t cdcdf_acm_get_version(void)
{
	return CDCDF_ACM_VERSION;
}
