/**
 * \file
 *
 * \brief SAM USB HPL
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

#ifndef _HPL_USB_H_INCLUDED
#define _HPL_USB_H_INCLUDED

#include <compiler.h>
#include <utils.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \name USB Spec definitions */
/*@{*/

/** Return 8-bit unsigned USB data. */
#define USB_GET_U8(addr, offset) (((uint8_t *)(addr))[(offset)])

/** Return 16-bit unsigned USB data. */
#define USB_GET_U16(addr, offset) ((((uint8_t *)(addr))[(offset) + 0] << 0) + (((uint8_t *)(addr))[(offset) + 1] << 8))

/** Return 32-bit unsigned USB data. */
#define USB_GET_U32(addr, offset)                                                                                      \
	((((uint8_t *)(addr))[(offset) + 0] << 0) + (((uint8_t *)(addr))[(offset) + 1] << 8)                               \
	 + (((uint8_t *)(addr))[(offset) + 2] << 16)                                                                       \
	 + (((uint8_t *)(addr))[(offset) + 3] << 32))

/** Offset of bmRequestType in USB request buffer. */
#define USB_bmRequestType_Offset 0

/** Offset of bRequest in USB request buffer. */
#define USB_bRequest_Offset 1

/** Offset of wValue in USB request buffer. */
#define USB_wValue_Offset 2

/** Offset of wIndex in USB request buffer. */
#define USB_wIndex_Offset 4

/** Offset of wLength in USB request buffer. */
#define USB_wLength_Offset 6

/** Get value of bmRequestType from USB request. */
#define USB_GET_bmRequestType(req) USB_GET_U8((req), USB_bmRequestType_Offset)

/** Get value of bRequest from USB request. */
#define USB_GET_bRequest(req) USB_GET_U8((req), USB_bRequest_Offset)

/** Get value of wValue from USB request. */
#define USB_GET_wValue(req) USB_GET_U16((req), USB_wValue_Offset)

/** Get value of wIndex from USB request. */
#define USB_GET_wIndex(req) USB_GET_U16((req), USB_wIndex_Offset)

/** Get value of wLength from USB request. */
#define USB_GET_wLength(req) USB_GET_U16((req), USB_wLength_Offset)

/** USB request IN indication of bmRequestType. */
#define USB_REQ_TYPE_IN 0x80u

/** USB endpoint number mask of bEndpointAddress. */
#define USB_EP_N_MASK 0x0Fu

/** USB endpoint direction bit of bEndpointAddress. */
#define USB_EP_DIR 0x80u

/** Get USB endpoint direction from endpoint address. */
#define USB_EP_GET_DIR(ep_addr) ((ep_addr)&USB_EP_DIR)

/** Get USB endpoint number from endpoint address. */
#define USB_EP_GET_N(ep_addr) ((ep_addr)&USB_EP_N_MASK)

/** Transfer type in EP descriptor bmAttributes: Control. */
#define USB_EP_XTYPE_CTRL 0x0

/** Transfer type in EP descriptor bmAttributes: Isochronous. */
#define USB_EP_XTYPE_ISOCH 0x1

/** Transfer type in EP descriptor bmAttributes: Bulk. */
#define USB_EP_XTYPE_BULK 0x2

/** Transfer type in EP descriptor bmAttributes: Interrupt. */
#define USB_EP_XTYPE_INTERRUPT 0x3

/** Transfer type mask in EP descriptor bmAttributes. */
#define USB_EP_XTYPE_MASK 0x3u

/*@}*/

/** \name USB status codes
 *@{
 */
/** USB operation is done successfully. */
#define USB_OK 0
/** USB (endpoint) is busy. */
#define USB_BUSY 1
/** USB (endpoint) is halted. */
#define USB_HALTED 2

/** General error. */
#define USB_ERROR 0x10
/** Operation is denied by hardware (e.g., syncing). */
#define USB_ERR_DENIED 0x11
/** Input parameter error. */
#define USB_ERR_PARAM 0x12
/** Functionality is not supported (e.g., initialize endpoint without cache to be control). */
#define USB_ERR_FUNC 0x13
/** Re-initialize, re-enable ... */
#define USB_ERR_REDO 0x14
/** Not enough resource (memory, endpoints ...). */
#define USB_ERR_ALLOC_FAIL 0x15
/*@}*/

/** USB speed. */
enum usb_speed {
	/** USB Low Speed. */
	USB_SPEED_LS,
	/** USB Full Speed. */
	USB_SPEED_FS,
	/** USB High Speed. */
	USB_SPEED_HS,
	/** USB Super Speed. */
	USB_SPEED_SS
};

/** USB transaction type. */
enum usb_trans_type {
	/** USB SETUP transaction. */
	USB_TRANS_SETUP,
	/** USB IN transaction. */
	USB_TRANS_IN,
	/** USB OUT transaction. */
	USB_TRANS_OUT
};

/** USB events that generates the device callbacks. */
enum usb_event {
	/** USB VBus changed, with parameter as present/not present. */
	USB_EV_VBUS,
	/** USB RESET detected on bus. */
	USB_EV_RESET,
	/** USB wakeup. */
	USB_EV_WAKEUP,
	/** USB LPM suspend, with parameter as \ref usb_lpm_attributes. */
	USB_EV_LPM_SUSPEND,
	/** USB suspend. */
	USB_EV_SUSPEND,
	/** USB error, with parameter as error code. */
	USB_EV_ERROR,
	/** Number of USB event types. */
	USB_EV_N
};

/** Control action for USB device endpoint stall. */
enum usb_ep_stall_ctrl {
	/** Clear stall of the endpoint. */
	USB_EP_STALL_CLR,
	/** Stall the endpoint. */
	USB_EP_STALL_SET,
	/** Return the stall status. */
	USB_EP_STALL_GET
};

/** Control action for USB device endpoint halt. */
enum usb_ep_halt_ctrl {
	/** Clear halt of the endpoint. */
	USB_EP_HALT_CLR = USB_EP_STALL_CLR,
	/** Stall the endpoint. */
	USB_EP_HALT_SET = USB_EP_STALL_SET,
	/** Return the halt status. */
	USB_EP_HALT_GET = USB_EP_STALL_GET
};

/** USB transactions status codes. */
enum usb_trans_code {
	/** TX or RX has been done without error. */
	USB_TRANS_DONE,
	/** The endpoint is stalled. */
	USB_TRANS_STALL,
	/** The endpoint transactions are aborted (cancel, control setup/status). */
	USB_TRANS_ABORT,
	/** The endpoint transactions are aborted by reset/disable. */
	USB_TRANS_RESET,
	/** Error is reported on the endpoint. */
	USB_TRANS_ERROR
};

/** Transfer status codes. */
enum usb_xfer_code {
	/** Transfer is done without error, for ctrl it means status packet done. */
	USB_XFER_DONE,
	/** For control transfer only, data stage is done without error. */
	USB_XFER_DATA,
	/** Endpoint stall is set. */
	USB_XFER_HALT,
	/** Endpoint stall is cleared. */
	USB_XFER_UNHALT,
	/** Transfer is aborted. */
	USB_XFER_ABORT,
	/** Transfer is aborted because endpoint reset/disable. */
	USB_XFER_RESET,
	/** There was an error. */
	USB_XFER_ERROR
};

/** USB endpoint errors. */
enum usb_ep_error {
	/** No error. */
	USB_EP_NO_ERROR,
	/** CRC error. */
	USB_EP_ERR_CRC,
	/** Endpoint transfer overflow. */
	USB_EP_ERR_OVERFLOW,
	/** Other endpoint errors. */
	USB_EP_ERR_GENERAL
};

/** Endpoint transfer state. */
enum usb_ep_state {
	/** Endpoint is disabled. */
	USB_EP_S_DISABLED,
	/** Endpoint is not busy. */
	USB_EP_S_IDLE,
	/** Control transfer only, endpoint is transferring setup packet. */
	USB_EP_S_X_SETUP,
	/** Endpoint is transferring data. */
	USB_EP_S_X_DATA,
	/** Control transfer only, endpoint is in status stage. */
	USB_EP_S_X_STATUS,
	/** Endpoint is halted. */
	USB_EP_S_HALTED,
	/** Endpoint error. */
	USB_EP_S_ERROR
};

#ifdef __cplusplus
}
#endif

#endif /* _HPL_USB_H_INCLUDED */
