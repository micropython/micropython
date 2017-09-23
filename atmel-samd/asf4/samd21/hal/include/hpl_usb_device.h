/**
 * \file
 *
 * \brief SAM USB HPL
 *
 * Copyright (C) 2016 Atmel Corporation. All rights reserved.
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

#ifndef _HPL_USB_DEVICE_H_INCLUDED
#define _HPL_USB_DEVICE_H_INCLUDED

#include <hpl_usb.h>
#include "hpl_usb_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/** USB Device callback type. */
enum usb_d_cb_type {
	/** USB device SOF callback. */
	USB_D_CB_SOF,
	/** USB device events callbacks. */
	USB_D_CB_EVENT,
	/** Number of types of USB device callback types. */
	USB_D_CB_N
};

/** USB Device endpoint callback type. */
enum usb_d_ep_cb_type {
	/** USB device endpoint setup callback. */
	USB_D_EP_CB_SETUP,
	/** USB device endpoint more data callback. */
	USB_D_EP_CB_MORE,
	/** USB device endpoint transaction done or error callback. */
	USB_D_EP_CB_XFER,
	/** Number of types of USB device endpoint callback types. */
	USB_D_EP_CB_N
};

/** Control action for USB device LPM handshake. */
enum usb_d_lpm_ctrl {
	/** No LPM handshake, not supported. */
	USB_D_LPM_DISABLE,
	/** ACK the LPM transaction. */
	USB_D_LPM_ACK,
	/** NYET the LPM transaction. */
	USB_D_LPM_NYET
};

/**
 *  USB device transfer descriptor.
 */
struct usb_d_transfer {
	/** Pointer to data buffer to transfer.
	 *  Note that it's recommended that the buffer is 32-bit aligned since
	 *  some of USB peripheral require this.
	 */
	uint8_t *buf;
	/** Transfer size, in number of bytes.
	 *  Note that it's recommended that the buffer size is 32-bit aligned
	 *  (modeled by 4) since some of USB peripheral require this.
	 */
	uint32_t size;
	/** Endpoint address. */
	uint8_t ep;
	/** Append ZLP for IN transfer, wait ZLP for OUT transfer. */
	uint8_t zlp;
};

/** USB device transactions status structure. */
struct usb_d_trans_status {
	/** Total data size. */
	uint32_t size;
	/** Total transfered data count. */
	uint32_t count;
	/** Endpoint address. */
	uint8_t ep;
	/** Endpoint type - CTRL/ISO/INT/BULK. */
	uint8_t xtype : 2;
	/** Transactions state, busy or not. */
	uint8_t busy : 1;
	/** Transactions state, setup received or not. */
	uint8_t setup : 1;
	/** Transactions state, stall or not. */
	uint8_t stall : 1;
	/** Transactions direction. */
	uint8_t dir : 1;
};

/** Prototype function for callback that is invoked on USB device SOF. */
typedef void (*_usb_d_dev_sof_cb_t)(void);

/** Prototype function for callback that is invoked on USB device events. */
typedef void (*_usb_d_dev_event_cb_t)(const enum usb_event, const uint32_t param);

/** HPL USB device callbacks. */
struct _usb_d_dev_callbacks {
	/** Callback that is invoked on SOF. */
	_usb_d_dev_sof_cb_t sof;
	/** Callback that is invoked on USB RESET/WAKEUP/RESUME/SUSPEND. */
	_usb_d_dev_event_cb_t event;
};

/** USB device endpoint callbacks. */
enum usb_d_dev_ep_cb_type {
	/** Setup packet is received. */
	USB_D_DEV_EP_CB_SETUP,
	/** Try to require more data. */
	USB_D_DEV_EP_CB_MORE,
	/** Transaction done OK/ERROR. */
	USB_D_DEV_EP_CB_DONE,
	/** Number of device endpoint callbacks. */
	USB_D_DEV_EP_CB_N
};

/**
 *  Callback that is invoked when control SETUP packet has bee received.
 *  \ref _usb_d_dev_ep_read_req() must be invoked to read setup data, and allow
 *  IN/OUT transactions on control endpoint.
 */
typedef void (*_usb_d_dev_ep_cb_setup_t)(const uint8_t ep);

/** Callback that is invoked when buffer is done, but last packet is full size
 *  packet without ZLP. Return \c true if more data has been requested. */
typedef bool (*_usb_d_dev_ep_cb_more_t)(const uint8_t ep, const uint32_t transfered);

/** Callback that is invoked when all data is finished, including background
 *  transfer, or error happens. */
typedef void (*_usb_d_dev_ep_cb_done_t)(const uint8_t ep, const int32_t code, const uint32_t transfered);

/** Callbacks for HPL USB device endpoint. */
struct _usb_d_dev_ep_callbacks {
	/** Callback that is invoked when SETUP packet is received.
	 *  \ref _usb_d_dev_ep_read_req() must be invoked to read setup data, and
	 *  allow IN/OUT transactions on control endpoint.
	 */
	_usb_d_dev_ep_cb_setup_t setup;
	/** Callback that is invoked to check if buffer is NULL and more data is
	 *  required.
	 *  It's called when last packet is full size packet, without
	 *  auto ZLP enabled.
	 *  It could be called when background transfer is still in progress.
	 */
	_usb_d_dev_ep_cb_more_t more;
	/** Callback that is invoked when transaction is done, including background
	 *  transfer, or error occurs.
	 */
	_usb_d_dev_ep_cb_done_t done;
};

/**
 * \brief Initialize the USB device instance
 * \return Operation result status.
 * \retval 0 Success.
 * \retval <0 Error code.
 */
int32_t _usb_d_dev_init(void);

/**
 * \brief Deinitialize the USB device instance
 * \return Operation result status.
 * \retval 0 Success.
 * \retval <0 Error code.
 */
void _usb_d_dev_deinit(void);

/**
 * \brief Register callback to handle USB device events
 * \param[in] type Callback type. See \ref usb_d_cb_type.
 * \param[in] func Pointer to callback function.
 *         Refer to \ref _usb_d_dev_callbacks for the prototypes.
 */
void _usb_d_dev_register_callback(const enum usb_d_cb_type type, const FUNC_PTR func);

/**
 * \brief Register callback to handle USB device endpoint events
 * \param[in] type Callback type. See \ref usb_d_dev_ep_cb_type.
 * \param[in] func Pointer to callback function.
 *         Refer to \ref _usb_d_dev_ep_callbacks for the prototypes.
 */
void _usb_d_dev_register_ep_callback(const enum usb_d_dev_ep_cb_type type, const FUNC_PTR func);

/**
 * \brief Enable the USB device
 * \return Operation result status.
 * \retval 0 Success.
 * \retval <0 Error code.
 */
int32_t _usb_d_dev_enable(void);

/**
 * \brief Disable the USB device
 * \return Operation result status.
 * \retval 0 Success.
 * \retval <0 Error code.
 */
int32_t _usb_d_dev_disable(void);

/**
 * \brief Attach the USB device
 */
void _usb_d_dev_attach(void);

/**
 * \brief Detach the USB device
 */
void _usb_d_dev_detach(void);

/**
 * \brief Send the USB device remote wakeup to host
 */
void _usb_d_dev_send_remotewakeup(void);

/**
 * \brief Get the USB device working speed
 * \return USB speed. See \ref usb_speed.
 */
enum usb_speed _usb_d_dev_get_speed(void);

/**
 * \brief Set the USB device address
 * \param[in] addr Address to be used.
 */
void _usb_d_dev_set_address(const uint8_t addr);

/**
 * \brief Get the USB device address
 * \return Address that is used.
 */
uint8_t _usb_d_dev_get_address(void);

/**
 * \brief Get the USB device frame number
 * \return The frame number.
 */
uint16_t _usb_d_dev_get_frame_n(void);

/**
 * \brief Get the USB device micro frame number
 * \return The micro frame number inside one frame (0~7).
 */
uint8_t _usb_d_dev_get_uframe_n(void);

/**
 * \brief Initialize and enable the USB device default endpoint 0
 * \param[in] max_pkt_siz Max endpoint size.
 * \return Operation result status.
 * \retval 0 Success.
 * \retval <0 Error code.
 */
int32_t _usb_d_dev_ep0_init(const uint8_t max_pkt_siz);

/**
 * \brief Initialize and enable the USB device endpoint
 * \param[in] ep Endpoint address,
 *               see endpoint descriptor details in USB spec.
 * \param[in] attr Endpoint attributes,
 *                 see endpoint descriptor details in USB spec.
 * \param[in] max_pkt_siz Endpoint size,
 *                        see endpoint descriptor details in USB spec.
 * \return Operation result status.
 * \retval 0 Success.
 * \retval <0 Error code.
 */
int32_t _usb_d_dev_ep_init(const uint8_t ep, const uint8_t attr, uint16_t max_pkt_siz);

/**
 * \brief Disable and deinitialize the USB device endpoint

 * \param[in] ep The endpoint to deinitialize.
 */
void _usb_d_dev_ep_deinit(const uint8_t ep);

/**
 * \brief Enable the endpoint
 * \param[in] ep The endpoint to enable.
 * \return Operation result status.
 * \retval 0 Success.
 * \retval <0 Error code.
 */
int32_t _usb_d_dev_ep_enable(const uint8_t ep);

/**
 * \brief Disable the endpoint
 * \param[in] ep The endpoint to disable.
 */
void _usb_d_dev_ep_disable(const uint8_t ep);

/**
 * \brief Set/Clear/Get USB device endpoint stall status
 * \param[in] ep Endpoint address.
 * \param[in] ctrl Operation selector. See \ref usb_ep_stall_ctrl.
 * \return Operation result or stall status.
 * \retval 0 Success or not stall.
 * \retval 1 Endpoint is stalled.
 * \retval -1 error.
 */
int32_t _usb_d_dev_ep_stall(const uint8_t ep, const enum usb_ep_stall_ctrl ctrl);

/**
 * \brief Read setup request data from specific endpoint
 * \param[in] ep Endpoint address.
 * \param[out] req_buf Pointer to buffer to locate the setup packet.
 * \return Number of bytes or error code.
 * \retval <0 error code.
 * \retval 0 No setup packet ready for read.
 * \retval >0 Size of bytes read, and ready to start IN/OUT. Note that if
 *         this number is over 8, only first 8 bytes will be copied.
 */
int32_t _usb_d_dev_ep_read_req(const uint8_t ep, uint8_t *req_buf);

/**
 * \brief Start USB device transfer
 *
 * On different USB peripheral hardware the transaction buffer address and size
 * may have different constraints. E.g., some hardware may require input address
 * 32-bit aligned, and input size 32-bit aligned. Refer to the corresponding
 * hardware usage reference documents.
 * The constraints are checked in implementation, with error code returned.
 *
 * \param[in] trans Pointer to the transaction description.
 * \return Operation result status.
 * \retval 1 Busy.
 * \retval 0 Success.
 * \retval <0 Error code.
 */
int32_t _usb_d_dev_ep_trans(const struct usb_d_transfer *trans);

/**
 * \brief Abort pending USB device transaction on specific endpoint
 * \param[in] ep Endpoint address to abort.
 */
void _usb_d_dev_ep_abort(const uint8_t ep);

/**
 * \brief Retrieve endpoint status.
 * \param[in] ep Endpoint address.
 * \param[out] stat Pointer to buffer to fill status description.
 * \return Status.
 * \retval 2 Packet writing.
 * \retval 1 Busy.
 * \retval 0 Ready.
 * \retval <0 Error code.
 */
int32_t _usb_d_dev_ep_get_status(const uint8_t ep, struct usb_d_trans_status *stat);

#ifdef __cplusplus
}
#endif

#endif /* _HPL_USB_DEVICE_H_INCLUDED */
