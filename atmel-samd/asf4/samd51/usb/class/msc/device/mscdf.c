/**
 * \file
 *
 * \brief USB Device Stack MSC Function Implementation.
 *
 * Copyright (C) 2016 - 2017 Atmel Corporation. All rights reserved.
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

#include "mscdf.h"
#include <string.h>

#define MSCDF_VERSION 0x00000001u

#define ERR_RPT_ZLP 0 /* Uses ZLP on IN error case */

/** MSC Class Transfer Stage Type */
enum mscdf_xfer_stage_type { MSCDF_CMD_STAGE, MSCDF_DATA_STAGE, MSCDF_STATUS_STAGE };

/** USB Device MSC Function Specific Data */
struct mscdf_func_data {
	/** MSC Device Interface information */
	uint8_t func_iface;
	/** MSC Device IN Endpoint */
	uint8_t func_ep_in;
	/** MSC Device OUT Endpoint */
	uint8_t func_ep_out;
	/** MSC Device Max LUN */
	uint8_t func_max_lun;
	/** MSC Transfer Block Address */
	uint8_t *xfer_blk_addr;
	/** MSC Transfer Block Size */
	uint32_t xfer_blk_size;
	/** MSC Transfer Total Bytes */
	uint32_t xfer_tot_bytes;
	/** MSC Transfer Stage */
	enum mscdf_xfer_stage_type xfer_stage;
	/** MSC Transfer Busy Flag */
	bool xfer_busy;
	/** MSC Device Enable Flag */
	bool enabled;
};

static struct usbdf_driver    _mscdf;
static struct mscdf_func_data _mscdf_funcd;

/* If callbacks are not registered:
 * - Return default inquiry information
 * - Return NOT FOUND for all other CBW
 */
static mscdf_inquiry_disk_t      mscdf_inquiry_disk      = NULL;
static mscdf_get_disk_capacity_t mscdf_get_disk_capacity = NULL;
static mscdf_eject_disk_t        mscdf_eject_disk        = NULL;
static mscdf_start_read_disk_t   mscdf_read_disk         = NULL;
static mscdf_start_write_disk_t  mscdf_write_disk        = NULL;
static mscdf_test_disk_ready_t   mscdf_test_disk_ready   = NULL;
static mscdf_xfer_blocks_done_t  mscdf_xfer_blocks_done  = NULL;

COMPILER_ALIGNED(4)
static struct scsi_inquiry_data _inquiry_default = {
    0x00,                                             /* Peripheral Qual / Peripheral Dev Type */
    SCSI_INQ_RMB,                                     /* Flags 1 */
    0x00,                                             /* Version */
    0x01,                                             /* Flags 3 */
    31,                                               /* Additional Length (n-4) */
    0x00,                                             /* Flags 5 */
    0x00,                                             /* Flags 6 */
    0x00,                                             /* Flags 7 */
    {0, 0, 0, 0, 0, 0, 0, 0},                         /* VID[8] */
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* PID[16] */
    {0, 0, 0, 0}                                      /* PREV[4] */
};

COMPILER_ALIGNED(4)
static struct usb_msc_cbw mscdf_cbw;

COMPILER_ALIGNED(4)
static struct usb_msc_csw mscdf_csw = {USB_CSW_SIGNATURE, 0, 0, 0};

COMPILER_ALIGNED(4)
static struct scsi_request_sense_data mscdf_sense_data
    = {SCSI_SENSE_CURRENT, 0x00, 0x00, {0x00, 0x00, 0x00, 0x00}, 0x0A};

/**
 * \brief USB MSC wait Command Block
 */
static bool mscdf_wait_cbw(void)
{
	_mscdf_funcd.xfer_stage = MSCDF_CMD_STAGE;
	return ERR_NONE == usbdc_xfer(_mscdf_funcd.func_ep_out, (uint8_t *)&mscdf_cbw, 31, false);
}

/**
 * \brief USB MSC Send Command Status
 */
static bool mscdf_send_csw(void)
{
	_mscdf_funcd.xfer_stage = MSCDF_STATUS_STAGE;
	return ERR_NONE == usbdc_xfer(_mscdf_funcd.func_ep_in, (uint8_t *)&mscdf_csw, sizeof(struct usb_msc_csw), false);
}

#if ERR_RPT_ZLP
/**
 * \brief USB MSC Send ZLP data
 */
static bool mscdf_send_zlp(void)
{
	_mscdf_funcd.xfer_stage = MSCDF_DATA_STAGE;
	return ERR_NONE == usbdc_xfer(_mscdf_funcd.func_ep_in, (uint8_t *)&mscdf_csw, 0, true);
}
#define mscdf_terminate_in() mscdf_send_zlp()
#else
/**
 * \brief USB MSC Halt IN endpoint
 */
static bool mscdf_halt_in(void)
{
	_mscdf_funcd.xfer_stage = MSCDF_DATA_STAGE;
	return ERR_NONE == usb_d_ep_halt(_mscdf_funcd.func_ep_in, USB_EP_HALT_SET);
}
#define mscdf_terminate_in() mscdf_halt_in()
#endif

/**
 * \brief USB MSC Request Sense
 * \param[in] err_codes Error code
 */
static void mscdf_request_sense(int32_t err_codes)
{
	switch (err_codes) {
	case ERR_NOT_FOUND:
		mscdf_sense_data.sense_flag_key = SCSI_SK_NOT_READY;
		mscdf_sense_data.AddSense       = BE16(SCSI_ASC_MEDIUM_NOT_PRESENT);
		break;

	case ERR_BUSY:
		mscdf_sense_data.sense_flag_key = SCSI_SK_UNIT_ATTENTION;
		mscdf_sense_data.AddSense       = BE16(SCSI_ASC_NOT_READY_TO_READY_CHANGE);
		break;

	case ERR_DENIED:
		mscdf_sense_data.sense_flag_key = SCSI_SK_DATA_PROTECT;
		mscdf_sense_data.AddSense       = BE16(SCSI_ASC_WRITE_PROTECTED);
		break;

	default:
		mscdf_sense_data.sense_flag_key = SCSI_SK_ILLEGAL_REQUEST;
		mscdf_sense_data.AddSense       = BE16(SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
		break;
	}
}

/**
 * \brief USB MSC Stack invalid command process
 */
static bool mscdf_invalid_cmd(void)
{
	struct usb_msc_cbw *pcbw = &mscdf_cbw;
	struct usb_msc_csw *pcsw = &mscdf_csw;

	pcsw->bCSWStatus = USB_CSW_STATUS_FAIL;

	mscdf_request_sense(ERR_UNSUPPORTED_OP);

	if ((pcbw->bmCBWFlags & USB_EP_DIR_IN) && pcsw->dCSWDataResidue) {
		return mscdf_terminate_in();
	} else {
		return mscdf_send_csw();
	}
}

/**
 * \brief USB MSC Function Read / Write Data
 * \param[in] count the amount of bytes has been transferred
 * \return Operation status.
 */
static bool mscdf_read_write(uint32_t count)
{
	struct usb_msc_cbw *pcbw = &mscdf_cbw;
	struct usb_msc_csw *pcsw = &mscdf_csw;
	int32_t             ret  = ERR_UNSUPPORTED_OP;
	uint32_t            address, nblocks;
	uint8_t             ep;

	if (_mscdf_funcd.xfer_stage == MSCDF_CMD_STAGE) {
		address = (uint32_t)(pcbw->CDB[2] << 24) + (uint32_t)(pcbw->CDB[3] << 16) + (uint32_t)(pcbw->CDB[4] << 8)
		          + pcbw->CDB[5];
		nblocks = (uint32_t)(pcbw->CDB[7] << 8) + pcbw->CDB[8];
		if (pcbw->CDB[0] == SBC_READ10) {
			if (NULL != mscdf_read_disk) {
				ret = mscdf_read_disk(pcbw->bCBWLUN, address, nblocks);
			} else {
				ret = ERR_NOT_FOUND;
			}
		} else if (pcbw->CDB[0] == SBC_WRITE10) {
			if (NULL != mscdf_write_disk) {
				ret = mscdf_write_disk(pcbw->bCBWLUN, address, nblocks);
			} else {
				ret = ERR_NOT_FOUND;
			}
		}
		if (ERR_NONE == ret) {
			_mscdf_funcd.xfer_stage = MSCDF_DATA_STAGE;
			return false;
		}
		pcsw->bCSWStatus = USB_CSW_STATUS_FAIL;
		mscdf_request_sense(ret);
		return mscdf_terminate_in();

	} else if (_mscdf_funcd.xfer_stage == MSCDF_DATA_STAGE) {
		if (count == 0) {
			return mscdf_send_csw();
		}
		if (pcsw->dCSWDataResidue < count || _mscdf_funcd.xfer_tot_bytes < count) {
			return true;
		}

		pcsw->dCSWDataResidue -= count;
		_mscdf_funcd.xfer_tot_bytes -= count;

		if (_mscdf_funcd.xfer_tot_bytes == 0) {
			_mscdf_funcd.xfer_busy = false;
			if (NULL != mscdf_xfer_blocks_done) {
				mscdf_xfer_blocks_done(pcbw->bCBWLUN);
			}
			if (pcsw->dCSWDataResidue == 0 && pcbw->CDB[0] == SBC_READ10) {
				pcsw->bCSWStatus = USB_CSW_STATUS_PASS;
				return mscdf_send_csw();
			} else {
				return false;
			}
		} else {
			_mscdf_funcd.xfer_blk_addr += count;
			if (pcbw->CDB[0] == SBC_READ10) {
				ep = _mscdf_funcd.func_ep_in;
			} else {
				ep = _mscdf_funcd.func_ep_out;
			}
			return usbdc_xfer(ep, _mscdf_funcd.xfer_blk_addr, _mscdf_funcd.xfer_tot_bytes, false);
		}
	} else {
		return true;
	}
}

/**
 * \brief Callback invoked when bulk IN data received
 * \param[in] ep Endpoint number
 * \param[in] rc transfer return status
 * \param[in] count the amount of bytes has been transferred
 * \return Operation status.
 */
static bool mscdf_cb_ep_bulk_in(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	struct usb_msc_cbw *pcbw = &mscdf_cbw;
	(void)ep;
	(void)rc;

	if (rc == USB_XFER_UNHALT) {
		if (_mscdf_funcd.xfer_stage != MSCDF_CMD_STAGE) {
			return mscdf_send_csw();
		}
	}

	if (_mscdf_funcd.xfer_stage == MSCDF_DATA_STAGE) {
		if (pcbw->CDB[0] == SBC_READ10) {
			return mscdf_read_write(count);
		} else {
			return mscdf_send_csw();
		}
	} else if (_mscdf_funcd.xfer_stage == MSCDF_STATUS_STAGE) {
		return mscdf_wait_cbw();
	} else {
		return true;
	}
}

/**
 * \brief Callback invoked when bulk OUT data received
 * \param[in] ep Endpoint number
 * \param[in] rc transfer return status
 * \param[in] count the amount of bytes has been transferred
 * \return Operation status.
 */
static bool mscdf_cb_ep_bulk_out(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	struct usb_msc_cbw *pcbw = &mscdf_cbw;
	struct usb_msc_csw *pcsw = &mscdf_csw;
	uint8_t *           pbuf = NULL;
	int32_t             ret;

	(void)ep;
	if (rc == USB_XFER_UNHALT) {
		return mscdf_wait_cbw();
	}

	if (_mscdf_funcd.xfer_stage == MSCDF_CMD_STAGE) {
		if (pcbw->dCBWSignature == USB_CBW_SIGNATURE) {
			pcsw->dCSWTag         = pcbw->dCBWTag;
			pcsw->dCSWDataResidue = pcbw->dCBWDataTransferLength;

			switch (pcbw->CDB[0]) {
			case SPC_INQUIRY:
				if (NULL != mscdf_inquiry_disk) {
					pbuf = mscdf_inquiry_disk(pcbw->bCBWLUN);
				}
				if (NULL == pbuf) {
					pbuf = (uint8_t *)&_inquiry_default;
				}
				_mscdf_funcd.xfer_stage = MSCDF_DATA_STAGE;
				pcsw->bCSWStatus        = USB_CSW_STATUS_PASS;
				pcsw->dCSWDataResidue   = 0;
				return usbdc_xfer(_mscdf_funcd.func_ep_in, pbuf, 36, false);

			case SBC_READ_CAPACITY10:
				if (NULL != mscdf_get_disk_capacity) {
					pbuf = mscdf_get_disk_capacity(pcbw->bCBWLUN);
				}
				if (NULL != pbuf) {
					_mscdf_funcd.xfer_stage = MSCDF_DATA_STAGE;
					_mscdf_funcd.xfer_blk_size
					    = (uint32_t)(pbuf[4] << 24) + (uint32_t)(pbuf[5] << 16) + (uint32_t)(pbuf[6] << 8) + pbuf[7];
					pcsw->bCSWStatus      = USB_CSW_STATUS_PASS;
					pcsw->dCSWDataResidue = 0;
					return usbdc_xfer(_mscdf_funcd.func_ep_in, pbuf, 8, false);
				} else {
					pcsw->bCSWStatus = USB_CSW_STATUS_FAIL;
					mscdf_request_sense(ERR_NOT_FOUND);
					return mscdf_terminate_in();
				}

			case SBC_READ10:
			case SBC_WRITE10:
				return mscdf_read_write(count);

			case SPC_PREVENT_ALLOW_MEDIUM_REMOVAL:
				if (0x00 == pcbw->CDB[4]) {
					pcsw->bCSWStatus      = USB_CSW_STATUS_PASS;
					pcsw->dCSWDataResidue = 0;
					return mscdf_send_csw();
				}
				break;

			case SBC_START_STOP_UNIT:
				if (0x02 == pcbw->CDB[4]) {
					if (NULL != mscdf_eject_disk) {
						ret = mscdf_eject_disk(pcbw->bCBWLUN);
						if (ERR_NONE == ret) {
							pcsw->bCSWStatus      = USB_CSW_STATUS_PASS;
							pcsw->dCSWDataResidue = 0;
						} else {
							pcsw->bCSWStatus = USB_CSW_STATUS_FAIL;
							mscdf_request_sense(ret);
						}
					} else {
						pcsw->bCSWStatus = USB_CSW_STATUS_FAIL;
						mscdf_request_sense(ERR_NOT_FOUND);
					}
					return mscdf_send_csw();
				}
				break;

			case SPC_REQUEST_SENSE:
				_mscdf_funcd.xfer_stage = MSCDF_DATA_STAGE;
				pcsw->bCSWStatus        = USB_CSW_STATUS_PASS;
				pcsw->dCSWDataResidue   = 0;
				return usbdc_xfer(_mscdf_funcd.func_ep_in,
				                  (uint8_t *)&mscdf_sense_data,
				                  sizeof(struct scsi_request_sense_data),
				                  false);

			case SPC_TEST_UNIT_READY:
				if (NULL != mscdf_test_disk_ready) {
					ret = mscdf_test_disk_ready(pcbw->bCBWLUN);
					if (ERR_NONE == ret) {
						pcsw->bCSWStatus      = USB_CSW_STATUS_PASS;
						pcsw->dCSWDataResidue = 0;
					} else {
						pcsw->bCSWStatus = USB_CSW_STATUS_FAIL;
						mscdf_request_sense(ret);
					}
				} else {
					pcsw->bCSWStatus = USB_CSW_STATUS_FAIL;
					mscdf_request_sense(ERR_NOT_FOUND);
				}
				return mscdf_send_csw();

			default:
				break;
			}
			return mscdf_invalid_cmd();
		} else {
			return true;
		}
	} else if (_mscdf_funcd.xfer_stage == MSCDF_DATA_STAGE) {
		return mscdf_read_write(count);
	} else {
		return true;
	}
}

/**
 * \brief Enable MSC Function
 * \param[in] drv Pointer to USB device function driver
 * \param[in] desc Pointer to USB interface descriptor
 * \return Operation status.
 */
static int32_t mscdf_enable(struct usbdf_driver *drv, struct usbd_descriptors *desc)
{
	struct mscdf_func_data *func_data = (struct mscdf_func_data *)(drv->func_data);

	usb_ep_desc_t    ep_desc;
	usb_iface_desc_t ifc_desc;
	uint8_t *        ifc, *ep;

	ifc = desc->sod;
	if (NULL == ifc) {
		return ERR_NOT_FOUND;
	}

	ifc_desc.bInterfaceNumber = ifc[2];
	ifc_desc.bInterfaceClass  = ifc[5];

	if (MSC_CLASS == ifc_desc.bInterfaceClass) {
		if (func_data->func_iface == ifc_desc.bInterfaceNumber) { /* Initialized */
			return ERR_ALREADY_INITIALIZED;
		} else if (func_data->func_iface != 0xFF) { /* Occupied */
			return ERR_NO_RESOURCE;
		} else {
			func_data->func_iface = ifc_desc.bInterfaceNumber;
		}
	} else { /* Not supported by this function driver */
		return ERR_NOT_FOUND;
	}

	/* Install endpoints */
	ep = usb_find_desc(ifc, desc->eod, USB_DT_ENDPOINT);
	while (NULL != ep) {
		ep_desc.bEndpointAddress = ep[2];
		ep_desc.bmAttributes     = ep[3];
		ep_desc.wMaxPacketSize   = usb_get_u16(ep + 4);
		if (usb_d_ep_init(ep_desc.bEndpointAddress, ep_desc.bmAttributes, ep_desc.wMaxPacketSize)) {
			return ERR_NOT_INITIALIZED;
		}
		if (ep_desc.bEndpointAddress & USB_EP_DIR_IN) {
			func_data->func_ep_in = ep_desc.bEndpointAddress;
			usb_d_ep_enable(func_data->func_ep_in);
			usb_d_ep_register_callback(func_data->func_ep_in, USB_D_EP_CB_XFER, (FUNC_PTR)mscdf_cb_ep_bulk_in);
		} else {
			func_data->func_ep_out = ep_desc.bEndpointAddress;
			usb_d_ep_enable(func_data->func_ep_out);
			usb_d_ep_register_callback(func_data->func_ep_out, USB_D_EP_CB_XFER, (FUNC_PTR)mscdf_cb_ep_bulk_out);
		}
		desc->sod = ep;
		ep        = usb_find_ep_desc(usb_desc_next(desc->sod), desc->eod);
	}
	// Installed
	_mscdf_funcd.enabled = true;
	return mscdf_wait_cbw();
}

/**
 * \brief Disable MSC Function
 * \param[in] drv Pointer to USB device function driver
 * \param[in] desc Pointer to USB device descriptor
 * \return Operation status.
 */
static int32_t mscdf_disable(struct usbdf_driver *drv, struct usbd_descriptors *desc)
{
	struct mscdf_func_data *func_data = (struct mscdf_func_data *)(drv->func_data);

	usb_iface_desc_t ifc_desc;

	if (desc) {
		ifc_desc.bInterfaceClass = desc->sod[5];
		// Check interface
		if (ifc_desc.bInterfaceClass != MSC_CLASS) {
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

	func_data->xfer_stage = MSCDF_CMD_STAGE;
	func_data->xfer_busy  = false;
	func_data->enabled    = false;

	return ERR_NONE;
}

/**
 * \brief MSC Control Function
 * \param[in] drv Pointer to USB device function driver
 * \param[in] ctrl USB device general function control type
 * \param[in] param Parameter pointer
 * \return Operation status.
 */
static int32_t mscdf_ctrl(struct usbdf_driver *drv, enum usbdf_control ctrl, void *param)
{
	switch (ctrl) {
	case USBDF_ENABLE:
		return mscdf_enable(drv, (struct usbd_descriptors *)param);

	case USBDF_DISABLE:
		return mscdf_disable(drv, (struct usbd_descriptors *)param);

	case USBDF_GET_IFACE:
		return ERR_UNSUPPORTED_OP;

	default:
		return ERR_INVALID_ARG;
	}
}

/**
 * \brief Process the MSC class set request
 * \param[in] ep Endpoint address.
 * \param[in] req Pointer to the request.
 * \return Operation status.
 */
static int32_t mscdf_set_req(uint8_t ep, struct usb_req *req)
{
	(void)ep;
	switch (req->bRequest) {
	case USB_REQ_MSC_BULK_RESET:
		_mscdf_funcd.xfer_stage = MSCDF_CMD_STAGE;
		usb_d_ep_halt(_mscdf_funcd.func_ep_in, USB_EP_HALT_SET);
		usb_d_ep_halt(_mscdf_funcd.func_ep_out, USB_EP_HALT_SET);
		return usbdc_xfer(0, NULL, 0, 0);
	default:
		return ERR_INVALID_ARG;
	}
}

/**
 * \brief Process the MSC class get request
 * \param[in] ep Endpoint address.
 * \param[in] req Pointer to the request.
 * \param[in] stage USB control transfer stages.
 * \return Operation status.
 */
static int32_t mscdf_get_req(uint8_t ep, struct usb_req *req, enum usb_ctrl_stage stage)
{
	uint16_t len = req->wLength;

	if (USB_DATA_STAGE == stage) {
		return ERR_NONE;
	}

	switch (req->bRequest) {
	case USB_REQ_MSC_GET_MAX_LUN:
		return usbdc_xfer(ep, &_mscdf_funcd.func_max_lun, len, false);
	default:
		return ERR_INVALID_ARG;
	}
}

/**
 * \brief Process the MSC class request
 * \param[in] ep Endpoint address.
 * \param[in] req Pointer to the request.
 * \param[in] stage USB control transfer stages.
 * \return Operation status.
 */
static int32_t mscdf_req(uint8_t ep, struct usb_req *req, enum usb_ctrl_stage stage)
{
	if (0x01 != ((req->bmRequestType >> 5) & 0x03)) { /* class request */
		return ERR_NOT_FOUND;
	}
	if (req->wIndex == _mscdf_funcd.func_iface) {
		if (req->bmRequestType & USB_EP_DIR_IN) {
			return mscdf_get_req(ep, req, stage);
		} else {
			return mscdf_set_req(ep, req);
		}
	} else {
		return ERR_NOT_FOUND;
	}
}

/** USB Device MSC Handler Struct */
static struct usbdc_handler mscdf_req_h = {NULL, (FUNC_PTR)mscdf_req};

/**
 * \brief Initialize the USB MSC Function Driver
 */
int32_t mscdf_init(uint8_t max_lun)
{
	if (usbdc_get_state() > USBD_S_POWER) {
		return ERR_DENIED;
	}

	_mscdf.ctrl               = mscdf_ctrl;
	_mscdf.func_data          = &_mscdf_funcd;
	_mscdf_funcd.func_max_lun = max_lun;

	usbdc_register_function(&_mscdf);
	usbdc_register_handler(USBDC_HDL_REQ, &mscdf_req_h);
	return ERR_NONE;
}

/**
 * \brief De-initialize the USB MSC Function Driver
 */
int32_t mscdf_deinit(void)
{
	if (usbdc_get_state() > USBD_S_POWER) {
		return ERR_DENIED;
	}

	_mscdf.ctrl      = NULL;
	_mscdf.func_data = NULL;

	usbdc_unregister_function(&_mscdf);
	usbdc_unregister_handler(USBDC_HDL_REQ, &mscdf_req_h);
	return ERR_NONE;
}

/**
 * \brief USB MSC Function Register Callback
 */
int32_t mscdf_register_callback(enum mscdf_cb_type cb_type, FUNC_PTR func)
{
	switch (cb_type) {
	case MSCDF_CB_INQUIRY_DISK:
		mscdf_inquiry_disk = (mscdf_inquiry_disk_t)func;
		break;
	case MSCDF_CB_GET_DISK_CAPACITY:
		mscdf_get_disk_capacity = (mscdf_get_disk_capacity_t)func;
		break;
	case MSCDF_CB_EJECT_DISK:
		mscdf_eject_disk = (mscdf_eject_disk_t)func;
		break;
	case MSCDF_CB_START_READ_DISK:
		mscdf_read_disk = (mscdf_start_read_disk_t)func;
		break;
	case MSCDF_CB_START_WRITE_DISK:
		mscdf_write_disk = (mscdf_start_write_disk_t)func;
		break;
	case MSCDF_CB_TEST_DISK_READY:
		mscdf_test_disk_ready = (mscdf_test_disk_ready_t)func;
		break;
	case MSCDF_CB_XFER_BLOCKS_DONE:
		mscdf_xfer_blocks_done = (mscdf_xfer_blocks_done_t)func;
		break;
	default:
		return ERR_INVALID_ARG;
	}
	return ERR_NONE;
}

/**
 * \brief Check whether MSC Function is enabled
 */
bool mscdf_is_enabled(void)
{
	return _mscdf_funcd.enabled;
}

/**
 * \brief Process the transfer between USB and Memory.
 *
 * Routine called by the main loop
 */
int32_t mscdf_xfer_blocks(bool rd, uint8_t *blk_addr, uint32_t blk_cnt)
{
	uint8_t ep;

	if (false == mscdf_is_enabled()) {
		return ERR_DENIED;
	} else if (true == _mscdf_funcd.xfer_busy) {
		return ERR_BUSY;
	} else {
		_mscdf_funcd.xfer_blk_addr  = blk_addr;
		_mscdf_funcd.xfer_tot_bytes = _mscdf_funcd.xfer_blk_size * blk_cnt;
		if (0 == _mscdf_funcd.xfer_tot_bytes) {
			if (false == rd) {
				/* For write command, this means no need for more data to receive.
				 * All the data have been written into disk.
				 */
				mscdf_csw.bCSWStatus = USB_CSW_STATUS_PASS;
				return mscdf_send_csw();
			} else {
				return ERR_INVALID_ARG;
			}
		} else {
			if (NULL == blk_addr) {
				return ERR_INVALID_ARG;
			}
			if (true == rd) {
				ep = _mscdf_funcd.func_ep_in;
			} else {
				ep = _mscdf_funcd.func_ep_out;
			}
			_mscdf_funcd.xfer_busy = true;
			usbdc_xfer(ep, blk_addr, _mscdf_funcd.xfer_tot_bytes, false);
			return ERR_NONE;
		}
	}
}

/**
 * \brief Return version
 */
uint32_t mscdf_get_version(void)
{
	return MSCDF_VERSION;
}
