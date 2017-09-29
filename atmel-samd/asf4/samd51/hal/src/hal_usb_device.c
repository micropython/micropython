/**
 * \file
 *
 * \brief SAM USB device HAL
 *
 * Copyright (C) 2015-2016 Atmel Corporation. All rights reserved.
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

#include "hal_usb_device.h"
#include "hal_atomic.h"

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/** USB device HAL driver version. */
#define USB_D_VERSION 0x00000001u

/**
 *  Endpoint callbacks for data transfer.
 */
struct usb_d_ep_callbacks {
	/** Callback that is invoked when setup packet is received. */
	usb_d_ep_cb_setup_t req;
	/** Callback invoked when buffer is done, but last packet is full size
	 *  packet without ZLP. Return \c true if new transfer has been submitted.
	 */
	usb_d_ep_cb_more_t more;
	/** Callback invoked when transfer is finished/halted/aborted or error
	 *  occurs.
	 */
	usb_d_ep_cb_xfer_t xfer;
};

/**
 *  Endpoint transfer descriptor header.
 */
struct usb_ep_xfer_hdr {
	/** Transfer type, reuse \ref usb_ep_type. */
	uint8_t type;
	/** Endpoint address. */
	uint8_t ep;
	/** Endpoint state. */
	uint8_t state;
	/** Last status code. */
	uint8_t status;
};

/**
 *  Transfer descriptor.
 */
struct usb_ep_xfer {
	/** General transfer descriptor. */
	struct usb_ep_xfer_hdr hdr;
	/** Pointer to data buffer. */
	uint8_t *buf;
	/** Transfer size. */
	uint32_t size;
	/** Control request packet. */
	uint8_t req[8];
};

/**
 *  USB device endpoint descriptor.
 */
struct usb_d_ep {
	/** On-going transfer on the endpoint. */
	struct usb_ep_xfer xfer;
	/** Endpoint callbacks. */
	struct usb_d_ep_callbacks callbacks;
};

/**
 *  USB device HAL driver descriptor.
 */
struct usb_d_descriptor {
	/** USB device endpoints. */
	struct usb_d_ep ep[CONF_USB_D_NUM_EP_SP];
};

/** The USB HAL driver descriptor instance. */
static struct usb_d_descriptor usb_d_inst;

/** \brief Find the endpoint.
 * \param[in] ep Endpoint address.
 * \return Index of endpoint descriptor.
 * \retval >=0 The index.
 * \retval <0 Not found (endpoint is not initialized).
 */
static int8_t _usb_d_find_ep(const uint8_t ep)
{
	int8_t i;
	for (i = 0; i < CONF_USB_D_NUM_EP_SP; i++) {
		if (usb_d_inst.ep[i].xfer.hdr.ep == ep) {
			return i;
		}
		if (usb_d_inst.ep[i].xfer.hdr.type == USB_EP_XTYPE_CTRL
		    && (ep & USB_EP_N_MASK) == usb_d_inst.ep[i].xfer.hdr.ep) {
			return i;
		}
	}
	return -1;
}

/**
 * \brief Start transactions
 * \param[in] ep Endpoint address.
 * \param[in] dir Endpoint transfer direction.
 * \param[in] buf Pointer to transfer buffer.
 * \param[in] size Transfer size.
 * \param[in] zlp Auto append ZLP for IN, or wait ZLP for OUT.
 */
static inline int32_t _usb_d_trans(const uint8_t ep, const bool dir, const uint8_t *buf, const uint32_t size,
                                   const uint8_t zlp)
{
	struct usb_d_transfer trans
	    = {(uint8_t *)buf, size, dir ? (uint8_t)(ep | USB_EP_DIR) : (uint8_t)(ep & USB_EP_N_MASK), zlp};

	return _usb_d_dev_ep_trans(&trans);
}

/**
 * \brief Dummy callback that returns false
 * \param[in] unused0 Unused parameter.
 * \param[in] unused1 Unused parameter.
 * \param[in] unused2 Unused parameter.
 * \return Always \c false.
 */
static bool usb_d_dummy_cb_false(uint32_t unused0, uint32_t unused1, uint32_t unused2)
{
	(void)unused0;
	(void)unused1;
	(void)unused2;
	return false;
}

/**
 * \brief Callback invoked when SETUP packet is ready
 * \param[in] ep Endpoint number with transfer direction on bit 8.
 */
static void usb_d_cb_trans_setup(const uint8_t ep)
{
	int8_t           ep_index = _usb_d_find_ep(ep);
	struct usb_d_ep *ept      = &usb_d_inst.ep[ep_index];
	uint8_t *        req      = ept->xfer.req;

	uint8_t n = _usb_d_dev_ep_read_req(ep, req);
	if (n != 8) {
		_usb_d_dev_ep_stall(ep, USB_EP_STALL_SET);
		_usb_d_dev_ep_stall(ep | USB_EP_DIR, USB_EP_STALL_SET);
		return;
	}

	_usb_d_dev_ep_stall(ep, USB_EP_STALL_CLR);
	_usb_d_dev_ep_stall(ep | USB_EP_DIR, USB_EP_STALL_CLR);
	ept->xfer.hdr.state = USB_EP_S_IDLE;
	if (!ept->callbacks.req(ep, req)) {
		ept->xfer.hdr.state = USB_EP_S_HALTED;
		_usb_d_dev_ep_stall(ep, USB_EP_STALL_SET);
		_usb_d_dev_ep_stall(ep | USB_EP_DIR, USB_EP_STALL_SET);
	}
}

/**
 * \brief Callback invoked when request more data
 * \param[in] ep Endpoint number with transfer direction on bit 8.
 * \param[in] transfered Number of bytes transfered.
 */
static bool usb_d_cb_trans_more(const uint8_t ep, const uint32_t transfered)
{
	int8_t           ep_index = _usb_d_find_ep(ep);
	struct usb_d_ep *ept      = &usb_d_inst.ep[ep_index];
	if (ept->xfer.hdr.state == USB_EP_S_X_DATA) {
		return ept->callbacks.more(ep, transfered);
	}
	return false;
}

/**
 * \brief Handles the case that control endpoint transactions are done
 * \param[in,out] ept Pointer to endpoint information.
 */
static inline void usb_d_ctrl_trans_done(struct usb_d_ep *ept)
{
	uint8_t state   = ept->xfer.hdr.state;
	bool    req_dir = USB_GET_bmRequestType(ept->xfer.req) & USB_REQ_TYPE_IN;

	if (state == USB_EP_S_X_DATA) {
		/* Data stage -> Status stage */
		bool err = ept->callbacks.xfer(ept->xfer.hdr.ep, USB_XFER_DATA, ept->xfer.req);
		if (err) {
			ept->xfer.hdr.state  = USB_EP_S_HALTED;
			ept->xfer.hdr.status = USB_XFER_HALT;
			_usb_d_dev_ep_stall(req_dir ? ept->xfer.hdr.ep : (ept->xfer.hdr.ep | USB_EP_DIR), USB_EP_STALL_SET);
		} else {
			ept->xfer.hdr.state = USB_EP_S_X_STATUS;
			_usb_d_trans(ept->xfer.hdr.ep, !req_dir, NULL, 0, 1);
		}
	} else {
		/* Status stage done */
		ept->callbacks.xfer(ept->xfer.hdr.ep, USB_XFER_DONE, ept->xfer.req);
		ept->xfer.hdr.state = USB_EP_S_X_SETUP;
	}
}

/**
 * Callback when USB transactions are finished.
 */
static void _usb_d_cb_trans_done(const uint8_t ep, const int32_t code, const uint32_t transferred)
{
	int8_t           ep_index = _usb_d_find_ep(ep);
	struct usb_d_ep *ept      = &usb_d_inst.ep[ep_index];

	if (code == USB_TRANS_DONE) {
		ept->xfer.hdr.status = USB_XFER_DONE;
		if (ept->xfer.hdr.type == USB_EP_XTYPE_CTRL) {
			usb_d_ctrl_trans_done(ept);
			return;
		}
		ept->xfer.hdr.state = USB_EP_S_IDLE;
	} else if (code == USB_TRANS_STALL) {
		ept->xfer.hdr.status = USB_XFER_HALT;
		if (ept->xfer.hdr.type == USB_EP_XTYPE_CTRL) {
			ept->xfer.hdr.state = USB_EP_S_X_SETUP;
			_usb_d_dev_ep_stall(ep, USB_EP_STALL_CLR);
		} else {
			ept->xfer.hdr.state = USB_EP_S_HALTED;
		}
	} else if (code == USB_TRANS_ABORT) {
		ept->xfer.hdr.status = USB_XFER_ABORT;
		if (ept->xfer.hdr.type == USB_EP_XTYPE_CTRL) {
			ept->xfer.hdr.state = USB_EP_S_X_SETUP;
			return;
		}
		ept->xfer.hdr.state = USB_EP_S_IDLE;
	} else if (code == USB_TRANS_RESET) {
		ept->xfer.hdr.state  = USB_EP_S_DISABLED;
		ept->xfer.hdr.status = USB_XFER_RESET;
	} else {
		ept->xfer.hdr.state  = USB_EP_S_ERROR;
		ept->xfer.hdr.status = USB_XFER_ERROR;
	}

	ept->callbacks.xfer(ep, (enum usb_xfer_code)ept->xfer.hdr.status, (void *)transferred);
}

int32_t usb_d_init(void)
{
	int32_t rc = _usb_d_dev_init();
	uint8_t i;
	if (rc < 0) {
		return rc;
	}
	memset(usb_d_inst.ep, 0x00, sizeof(struct usb_d_ep) * CONF_USB_D_NUM_EP_SP);
	for (i = 0; i < CONF_USB_D_NUM_EP_SP; i++) {
		usb_d_inst.ep[i].xfer.hdr.ep    = 0xFF;
		usb_d_inst.ep[i].callbacks.req  = (usb_d_ep_cb_setup_t)usb_d_dummy_cb_false;
		usb_d_inst.ep[i].callbacks.more = (usb_d_ep_cb_more_t)usb_d_dummy_cb_false;
		usb_d_inst.ep[i].callbacks.xfer = (usb_d_ep_cb_xfer_t)usb_d_dummy_cb_false;
	}
	/* Handles device driver endpoint callbacks to build transfer. */
	_usb_d_dev_register_ep_callback(USB_D_DEV_EP_CB_SETUP, (FUNC_PTR)usb_d_cb_trans_setup);
	_usb_d_dev_register_ep_callback(USB_D_DEV_EP_CB_MORE, (FUNC_PTR)usb_d_cb_trans_more);
	_usb_d_dev_register_ep_callback(USB_D_DEV_EP_CB_DONE, (FUNC_PTR)_usb_d_cb_trans_done);
	return ERR_NONE;
}

void usb_d_deinit(void)
{
	_usb_d_dev_deinit();
}

void usb_d_register_callback(const enum usb_d_cb_type type, const FUNC_PTR func)
{
	/* Directly uses device driver callback. */
	_usb_d_dev_register_callback(type, func);
}

int32_t usb_d_enable(void)
{
	return _usb_d_dev_enable();
}

void usb_d_disable(void)
{
	_usb_d_dev_disable();
}

void usb_d_attach(void)
{
	_usb_d_dev_attach();
}

void usb_d_detach(void)
{
	_usb_d_dev_detach();
}

enum usb_speed usb_d_get_speed(void)
{
	return _usb_d_dev_get_speed();
}

uint16_t usb_d_get_frame_num(void)
{
	return _usb_d_dev_get_frame_n();
}

uint8_t usb_d_get_uframe_num(void)
{
	return _usb_d_dev_get_uframe_n();
}

void usb_d_set_address(const uint8_t addr)
{
	_usb_d_dev_set_address(addr);
}

void usb_d_send_remotewakeup(void)
{
	_usb_d_dev_send_remotewakeup();
}

int32_t usb_d_ep0_init(const uint8_t max_pkt_size)
{
	return usb_d_ep_init(0, USB_EP_XTYPE_CTRL, max_pkt_size);
}

int32_t usb_d_ep_init(const uint8_t ep, const uint8_t attr, const uint16_t max_pkt_size)
{
	int32_t          rc;
	int8_t           ep_index = _usb_d_find_ep(ep);
	struct usb_d_ep *ept      = &usb_d_inst.ep[ep_index];
	if (ep_index >= 0) {
		return -USB_ERR_REDO;
	} else {
		ep_index = _usb_d_find_ep(0xFF);
		if (ep_index < 0) {
			return -USB_ERR_ALLOC_FAIL;
		}
		ept = &usb_d_inst.ep[ep_index];
	}
	rc = _usb_d_dev_ep_init(ep, attr, max_pkt_size);
	if (rc < 0) {
		return rc;
	}
	ept->xfer.hdr.ep   = ep;
	ept->xfer.hdr.type = attr & USB_EP_XTYPE_MASK;
	return ERR_NONE;
}

void usb_d_ep_deinit(const uint8_t ep)
{
	int8_t           ep_index = _usb_d_find_ep(ep);
	struct usb_d_ep *ept      = &usb_d_inst.ep[ep_index];
	if (ep_index < 0) {
		return;
	}
	_usb_d_dev_ep_deinit(ep);
	ept->xfer.hdr.ep = 0xFF;
}

int32_t usb_d_ep_enable(const uint8_t ep)
{
	int8_t           ep_index = _usb_d_find_ep(ep);
	struct usb_d_ep *ept      = &usb_d_inst.ep[ep_index];
	int32_t          rc;
	if (ep_index < 0) {
		return -USB_ERR_PARAM;
	}
	ept->xfer.hdr.state = (ept->xfer.hdr.type == USB_EP_XTYPE_CTRL) ? USB_EP_S_X_SETUP : USB_EP_S_IDLE;
	rc                  = _usb_d_dev_ep_enable(ep);
	if (rc < 0) {
		ept->xfer.hdr.state = USB_EP_S_DISABLED;
	}
	return rc;
}

void usb_d_ep_disable(const uint8_t ep)
{
	int8_t           ep_index = _usb_d_find_ep(ep);
	struct usb_d_ep *ept      = &usb_d_inst.ep[ep_index];
	if (ep_index < 0) {
		return;
	}
	_usb_d_dev_ep_disable(ep);
	ept->xfer.hdr.state = USB_EP_S_DISABLED;
}

uint8_t *usb_d_ep_get_req(const uint8_t ep)
{
	int8_t ep_index = _usb_d_find_ep(ep);
	if (ep_index < 0) {
		return NULL;
	}
	return usb_d_inst.ep[ep_index].xfer.req;
}

int32_t usb_d_ep_transfer(const struct usb_d_transfer *xfer)
{
	int8_t                ep_index = _usb_d_find_ep(xfer->ep);
	struct usb_d_ep *     ept      = &usb_d_inst.ep[ep_index];
	bool                  dir = USB_EP_GET_DIR(xfer->ep), zlp = xfer->zlp;
	uint32_t              len = xfer->size;
	int32_t               rc;
	volatile uint8_t      state;
	volatile hal_atomic_t flags;

	if (ep_index < 0) {
		return -USB_ERR_PARAM;
	}

	atomic_enter_critical(&flags);
	state = ept->xfer.hdr.state;
	if (state == USB_EP_S_IDLE) {
		ept->xfer.hdr.state = USB_EP_S_X_DATA;
		atomic_leave_critical(&flags);
	} else {
		atomic_leave_critical(&flags);
		switch (state) {
		case USB_EP_S_HALTED:
			return USB_HALTED;
		case USB_EP_S_ERROR:
			return -USB_ERROR;
		case USB_EP_S_DISABLED:
			return -USB_ERR_FUNC;
		default: /* USB_EP_S_X_xxxx  */
			return USB_BUSY;
		}
	}

	if (ept->xfer.hdr.type == USB_EP_XTYPE_CTRL) {
		uint16_t req_len = USB_GET_wLength(ept->xfer.req);
		/* SETUP without data: ZLP IN as status. */
		if (req_len == 0) {
			dir                 = true;
			len                 = 0;
			zlp                 = true;
			ept->xfer.hdr.state = USB_EP_S_X_STATUS;
		} else {
			dir = (USB_GET_bmRequestType(ept->xfer.req) & USB_REQ_TYPE_IN);
			/* Data length not exceed requested. */
			if (len > req_len) {
				len = req_len;
			}
			if (dir) {
				/* Setup -> In */
				zlp = (req_len > len);
			} else {
				zlp = false;
			}
		}
	}

	rc = _usb_d_trans(xfer->ep, dir, xfer->buf, len, zlp);
	return rc;
}

void usb_d_ep_abort(const uint8_t ep)
{
	int8_t           ep_index = _usb_d_find_ep(ep);
	struct usb_d_ep *ept      = &usb_d_inst.ep[ep_index];
	if (ep_index < 0) {
		return;
	}
	_usb_d_dev_ep_abort(ep);
	ept->xfer.hdr.state  = USB_EP_S_IDLE;
	ept->xfer.hdr.status = USB_XFER_ABORT;
}

int32_t usb_d_ep_get_status(const uint8_t ep, struct usb_d_ep_status *stat)
{
	int8_t                    ep_index = _usb_d_find_ep(ep);
	struct usb_d_ep *         ept      = &usb_d_inst.ep[ep_index];
	struct usb_d_trans_status tmp;
	uint8_t                   state = ept->xfer.hdr.state;
	if (ep_index < 0) {
		return -USB_ERR_PARAM;
	}
	if (stat) {
		/* Check transaction status if transferring data. */
		_usb_d_dev_ep_get_status(ep, &tmp);
		stat->ep    = ep;
		stat->state = state;
		stat->code  = ept->xfer.hdr.status;
		stat->count = tmp.count;
		stat->size  = tmp.size;
	}
	switch (state) {
	case USB_EP_S_IDLE:
		return USB_OK;
	case USB_EP_S_HALTED:
		return USB_HALTED;
	case USB_EP_S_ERROR:
		return -USB_ERROR;
	case USB_EP_S_DISABLED:
		return -USB_ERR_FUNC;
	default:
		/* Busy */
		return USB_BUSY;
	}
}

static inline int32_t _usb_d_ep_halt_clr(const uint8_t ep)
{
	int8_t           ep_index = _usb_d_find_ep(ep);
	struct usb_d_ep *ept      = &usb_d_inst.ep[ep_index];
	int32_t          rc;
	if (ep_index < 0) {
		return -USB_ERR_PARAM;
	}
	if (_usb_d_dev_ep_stall(ep, USB_EP_STALL_GET)) {
		rc = _usb_d_dev_ep_stall(ep, USB_EP_STALL_CLR);
		if (rc < 0) {
			return rc;
		}
		ept->xfer.hdr.state  = USB_EP_S_IDLE;
		ept->xfer.hdr.status = USB_XFER_UNHALT;
		ept->callbacks.xfer(ep, USB_XFER_UNHALT, NULL);
	}
	return ERR_NONE;
}

int32_t usb_d_ep_halt(const uint8_t ep, const enum usb_ep_halt_ctrl ctrl)
{
	if (ctrl == USB_EP_HALT_CLR) {
		return _usb_d_ep_halt_clr(ep);
	} else if (ctrl == USB_EP_HALT_SET) {
		return _usb_d_dev_ep_stall(ep, USB_EP_STALL_SET);
	} else {
		return _usb_d_dev_ep_stall(ep, USB_EP_STALL_GET);
	}
}

void usb_d_ep_register_callback(const uint8_t ep, const enum usb_d_ep_cb_type type, const FUNC_PTR func)
{
	int8_t           ep_index = _usb_d_find_ep(ep);
	struct usb_d_ep *ept      = &usb_d_inst.ep[ep_index];
	FUNC_PTR         f        = func ? (FUNC_PTR)func : (FUNC_PTR)usb_d_dummy_cb_false;
	if (ep_index < 0) {
		return;
	}
	switch (type) {
	case USB_D_EP_CB_SETUP:
		ept->callbacks.req = (usb_d_ep_cb_setup_t)f;
		break;
	case USB_D_EP_CB_MORE:
		ept->callbacks.more = (usb_d_ep_cb_more_t)f;
		break;
	case USB_D_EP_CB_XFER:
		ept->callbacks.xfer = (usb_d_ep_cb_xfer_t)f;
		break;
	default:
		break;
	}
}

uint32_t usb_d_get_version(void)
{
	return USB_D_VERSION;
}

#ifdef __cplusplus
}
#endif
