/**
 * \file
 *
 * \brief SAM USB host HPL
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

#ifndef _HPL_USB_HOST_H_INCLUDED
#define _HPL_USB_HOST_H_INCLUDED

#include <hpl_usb.h>
#include <hpl_irq.h>
#include "hpl_usb_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Driver version */
#define USB_H_VERSION 0x00000001

/**
 * @brief      USB HCD callback types
 */
enum usb_h_cb_type {
	/** SOF generated */
	USB_H_CB_SOF,
	/** Root Hub change detected */
	USB_H_CB_ROOTHUB_CHANGE,
	/** Number of USB HCD callback types */
	USB_H_CB_N
};

/**
 * @brief      USB HCD resource strategy
 */
enum usb_h_rsc_strategy {
	/** Normal resource allocation, e.g.,
     *  1 bank for interrupt endpoint,
     *  2 bank for bulk endpoint and normal iso endpoint,
     *  3 bank for iso high bandwidth endpoint.
     */
	USB_H_RSC_NORMAL = false,
	/** Minimal resource allocation, e.g., only 1 bank for bulk endpoints */
	USB_H_RSC_MINIMAL = true
};

/**
 * @brief      USB HCD pipe states
 */
enum usb_h_pipe_state {
	/** Pipe is free to allocate */
	USB_H_PIPE_S_FREE = 0x00,
	/** Pipe is in configuration */
	USB_H_PIPE_S_CFG = 0x01,
	/** Pipe is allocated and idle */
	USB_H_PIPE_S_IDLE = 0x02,
	/** Pipe in control setup stage */
	USB_H_PIPE_S_SETUP = 0x03,
	/** Pipe in data IN stage */
	USB_H_PIPE_S_DATI = 0x05,
	/** Pipe in data OUT stage */
	USB_H_PIPE_S_DATO = 0x06,
	/** Pipe in data IN ZLP stage */
	USB_H_PIPE_S_ZLPI = 0x07,
	/** Pipe in data OUT ZLP stage */
	USB_H_PIPE_S_ZLPO = 0x08,
	/** Pipe in control status IN stage */
	USB_H_PIPE_S_STATI = 0x09,
	/** Pipe in control status OUT stage */
	USB_H_PIPE_S_STATO = 0x0A,
	/** Taken by physical pipe (in process) */
	USB_H_PIPE_S_TAKEN = 0x10
};

/**
 * @brief      USB HCD status code
 */
enum usb_h_status {
	/** OK */
	USB_H_OK = ERR_NONE,
	/** Busy */
	USB_H_BUSY = ERR_BUSY,
	/** Denied */
	USB_H_DENIED = ERR_DENIED,
	/** Timeout */
	USB_H_TIMEOUT = ERR_TIMEOUT,
	/** Abort */
	USB_H_ABORT = ERR_ABORTED,
	/** Stall protocol */
	USB_H_STALL = ERR_PROTOCOL,
	/** Transfer reset by pipe re-configure */
	USB_H_RESET = ERR_REQ_FLUSHED,
	/** Argument error */
	USB_H_ERR_ARG = ERR_INVALID_ARG,
	/** Operation not supported */
	USB_H_ERR_UNSP_OP = ERR_UNSUPPORTED_OP,
	/** No resource */
	USB_H_ERR_NO_RSC = ERR_NO_RESOURCE,
	/** Not initialized */
	USB_H_ERR_NOT_INIT = ERR_NOT_INITIALIZED,
	/** Some general error */
	USB_H_ERR = ERR_IO
};

/** Forward declare for pipe structure */
struct usb_h_pipe;

/** Forward declare for driver descriptor structure */
struct usb_h_desc;

/**
 * \brief Prototyping USB HCD callback of SOF
 */
typedef void (*usb_h_cb_sof_t)(struct usb_h_desc *drv);

/**
 * \brief Prototyping USB HCD callback of root hub changing.
 * According to the bitmap size, max port number is 31.
 */
typedef void (*usb_h_cb_roothub_t)(struct usb_h_desc *drv, uint8_t port, uint8_t ftr);

/**
 * Prototyping USB HCD callback of pipe transfer done.
 * For control pipe, it's forced to call even if there is no transfer
 * in progress, since the pipe size could be changed at run time.
 */
typedef void (*usb_h_pipe_cb_xfer_t)(struct usb_h_pipe *pipe);

/** Access to max packet size of a pipe */
#define usb_h_pipe_max_pkt_size(p) (p->max_pkt_size)

/** Access to device address of a pipe */
#define usb_h_pipe_dev_addr(p) (p->dev)

/** Access to endpoint address of a pipe */
#define usb_h_pipe_ep_addr(p) (p->ep)

/** Access to state of a pipe */
#define usb_h_pipe_state(p) (p->x.general.state)

/** Access to status of a pipe */
#define usb_h_pipe_status(p) (p->x.general.status)

/**
 * @brief      USB Host Controller device structure
 */
struct usb_h_desc {
	/** Pointer to hardware base */
	void *hw;
	/** Pointer to private data for Host Controller driver */
	void *prvt;
	/** Interrupt handling descriptor */
	struct _irq_descriptor irq;
	/** Callback of SOF */
	usb_h_cb_sof_t sof_cb;
	/** Callback of root hub change */
	usb_h_cb_roothub_t rh_cb;
#if CONF_USB_H_INST_OWNER_SP
	/** Extension for the driver owner (upper layer user) */
	void *owner;
#endif
};

/**
 * @brief      Transfer descriptor for control transfer
 *
 * Timing in USB 2.0 spec.:
 * - 9.2.6.1 : USB sets an upper limit of 5 seconds as the upper limit for any
 *             command to be processed.
 * - 9.2.6.3 : if a device receives a SetAddress() request, the device must be
 *             able to complete processing of the request and be able to
 *             successfully complete the Status stage of the request within
 *             50 ms.
 *             After successful completion of the Status stage, the device is
 *             allowed a SetAddress() recovery interval of 2 ms. At the end of
 *             this interval, the device must be able to accept Setup packets
 *             addressed to the new address.
 * - 9.2.6.4 : For standard device requests that require no Data stage, a device
 *             must be able to complete the request and be able to successfully
 *             complete the Status stage of the request within 50 ms of receipt
 *             of the request. This limitation applies to requests to the
 *             device, interface, or endpoint.
 *             For standard device requests that require data stage transfer to
 *             the host, the device must be able to return the first data packet
 *             to the host within 500 ms of receipt of the request. For
 *             subsequent data packets, if any, the device must be able to
 *             return them within 500 ms of successful completion of the
 *             transmission of the previous packet. The device must then be
 *             able to successfully complete the status stage within 50 ms after
 *             returning the last data packet.
 *             For standard device requests that require a data stage transfer
 *             to the device, the 5-second limit applies.
 * - 9.2.6.5 : Unless specifically exempted in the class document, all
 *             class-specific requests must meet the timing limitations for
 *             standard device requests.
 *
 * Conclusion:
 * 1. Whole request with data: 5 seconds
 * 2. Whole request without data: 50 ms
 * 3. Data packets: 500 ms
 */
struct usb_h_ctrl_xfer {
	/** Pointer to transfer data */
	uint8_t *data;
	/** Pointer to setup packet */
	uint8_t *setup;
	/** Expected transfer size */
	uint16_t size;
	/** Transfer count */
	uint16_t count;
	/** Timeout for request, -1 if disable timeout */
	int16_t req_timeout;
	/** Timeout between packets
	 *  (500ms for data and 50ms for status), -1 if disabled */
	int16_t pkt_timeout;
	/** Packet size during transfer (<= allocate max packet size) */
	uint16_t pkt_size;

	/** Transfer state */
	uint8_t state;
	/** Last transfer status */
	int8_t status;
};

/**
 * @brief      Transfer descriptor for bulk / interrupt / iso transfer
 */
struct usb_h_bulk_int_iso_xfer {
	/** Expected transfer size */
	uint32_t size;
	/** Transfer count */
	uint32_t count;
	/** Pointer to transfer data */
	uint8_t *data;
	/** Reserved */
	uint16_t reserved[3];

	/** Transfer state */
	uint8_t state;
	/** Last transfer status */
	int8_t status;
};

/**
 * @brief      Transfer descriptor for periodic high bandwidth transfer
 */
struct usb_h_high_bw_xfer {
	/** Expected transfer size */
	uint32_t size;
	/** Transfer count */
	uint32_t count;
	/** Pointer to transfer data */
	uint8_t *data;
	/** Micro frame packet sizes */
	uint16_t pkt_size[3];

	/** Transfer state */
	uint8_t state;
	/** Last transfer status */
	int8_t status;
};

/**
 * @brief General transfer descriptor
 */
struct usb_h_xfer {
	/** Reserved for different transfer */
	union {
		uint16_t u16[9];
		uint8_t  u8[18];
	} reserved;
	/** Transfer state */
	uint8_t state;
	/** Last transfer status */
	int8_t status;
};

/**
 * @brief      USB Host Controller Driver Pipe structure
 */
struct usb_h_pipe {
	/** Pointer to the USB Host Controller Driver */
	struct usb_h_desc *hcd;
	/** Pointer to the callback for transfer done */
	usb_h_pipe_cb_xfer_t done;
#if CONF_USB_H_INST_OWNER_SP
	/** Pointer to the pipe owner */
	void *owner;
#endif

	/** Endpoint max packet size (bits 10..0) */
	uint16_t max_pkt_size;
	/** Device address */
	uint8_t dev;
	/** Endpoint address */
	uint8_t ep;

	/** Endpoint interval */
	uint8_t interval;
	/** Endpoint type: Control, Isochronous, Bulk or Interrupt */
	uint8_t type;
	/** Current toggle (driver dependent) */
	uint8_t toggle;
	/** Endpoint number of banks (HW dependent) */
	uint8_t bank : 2;
	/** Transfer speed (HW dependent) */
	uint8_t speed : 2;
	/** High bandwidth periodic out */
	uint8_t high_bw_out : 1;
	/** Uses DMA (on transfer) */
	uint8_t dma : 1;
	/** Transfer ZLP support */
	uint8_t zlp : 1;
	/** Transfer periodic */
	uint8_t periodic_start : 1;

	/** Transfer status */
	union {
		/** General transfer info */
		struct usb_h_xfer general;
		/** Control transfer status */
		struct usb_h_ctrl_xfer ctrl;
		/** Bulk interrupt iso transfer status */
		struct usb_h_bulk_int_iso_xfer bii;
		/** Periodic high bandwidth transfer status */
		struct usb_h_high_bw_xfer hbw;
	} x;
};

/**
 * @brief      USB HCD Initialization
 *
 * @param      drv     Pointer to the HCD driver instance
 * @param[in]  hw      Pointer to hardware base
 * @param[in]  prvt    The private driver data (implement specific)
 *
 * @return     Operation result status
 * @retval     ERR_DENIED Hardware has been enabled
 * @retval     ERR_NONE Operation done successfully
 */
int32_t _usb_h_init(struct usb_h_desc *drv, void *hw, void *prvt);

/**
 * @brief      USB HCD de-initialization
 *
 * @param      drv   The driver
 */
void _usb_h_deinit(struct usb_h_desc *drv);

/**
 * @brief      USB HCD enable
 *
 * @param      drv   The driver
 */
void _usb_h_enable(struct usb_h_desc *drv);

/**
 * @brief      USB HCD disable
 *
 * @param      drv   The driver
 */
void _usb_h_disable(struct usb_h_desc *drv);

/**
 * @brief      Register callbacks for USB HCD
 *
 * @param      drv   The driver
 * @param[in]  type  The callback type
 * @param[in]  cb    The callback function entry
 *
 * @return     Operation result status
 * @retval     ERR_INVALID_ARG Argument error
 * @retval     ERR_NONE Operation done successfully
 */
int32_t _usb_h_register_callback(struct usb_h_desc *drv, enum usb_h_cb_type type, FUNC_PTR cb);

/**
 * @brief      Return current frame number
 *
 * @param      drv   The driver
 *
 * @return     current frame number
 */
uint16_t _usb_h_get_frame_n(struct usb_h_desc *drv);

/**
 * @brief      Return current micro frame number
 *
 * @param      drv   The driver
 *
 * @return     current micro frame number
 */
uint8_t _usb_h_get_microframe_n(struct usb_h_desc *drv);

/**
 * @brief Suspend the USB bus
 *
 * @param drv The driver
 */
void _usb_h_suspend(struct usb_h_desc *drv);

/**
 * @brief Resume the USB bus
 *
 * @param drv The driver
 */
void _usb_h_resume(struct usb_h_desc *drv);

/* Root hub related APIs */

/**
 * \brief Reset the root hub port
 *
 * \param[in,out] drv  Pointer to the USB HCD driver
 * \param[in]     port Root hub port, ignored if there is only one port
 */
void _usb_h_rh_reset(struct usb_h_desc *drv, uint8_t port);

/**
 * \brief Suspend the root hub port
 *
 * \param[in,out] drv  Pointer to the USB HCD driver
 * \param[in]     port Root hub port, ignored if there is only one port
 */
void _usb_h_rh_suspend(struct usb_h_desc *drv, uint8_t port);

/**
 * \brief Resume the root hub port
 *
 * \param[in,out] drv  Pointer to the USB HCD driver
 * \param[in]     port Root hub port, ignored if there is only one port
 */
void _usb_h_rh_resume(struct usb_h_desc *drv, uint8_t port);

/**
 * \brief Root hub or port feature status check
 *
 * Check USB Spec. for hub status and feature selectors.
 *
 * \param[in] drv  Pointer to the USB HCD driver
 * \param[in] port Set to 0 to get hub status, otherwise to get port status
 * \param[in] ftr  Hub feature/status selector
 *                 (0: connection, 2: suspend, 4: reset, 9: LS, 10: HS)
 *
 * \return     \c true if the status bit is 1
 */
bool _usb_h_rh_check_status(struct usb_h_desc *drv, uint8_t port, uint8_t ftr);

/* Pipe transfer functions */

/**
 * @brief      Allocate a pipe for USB host communication
 *
 * @param      drv           The USB HCD driver
 * @param[in]  dev           The device address
 * @param[in]  ep            The endpoint address
 * @param[in]  max_pkt_size  The endpoint maximum packet size
 * @param[in]  attr          The endpoint attribute
 * @param[in]  interval      The endpoint interval
 *                           (bInterval of USB Endpoint Descriptor)
 * @param[in]  speed         The transfer speed of the endpoint
 * @param[in]  minimum_rsc   Minimum the resource usage, \sa usb_h_rsc_strategy
 *
 * @return     Pointer to allocated pipe structure instance
 * @retval     NULL allocation fail
 */
struct usb_h_pipe *_usb_h_pipe_allocate(struct usb_h_desc *drv, uint8_t dev, uint8_t ep, uint16_t max_pkt_size,
                                        uint8_t attr, uint8_t interval, uint8_t speed, bool minimum_rsc);

/**
 * @brief      Free an allocated pipe
 *
 * @param      pipe  The pipe
 *
 * @return     Operation result status
 * @retval     ERR_BUSY Pipe is busy, use \ref _usb_h_pipe_abort to abort
 * @retval     ERR_NONE Operation done successfully
 */
int32_t _usb_h_pipe_free(struct usb_h_pipe *pipe);

/**
 * @brief      Modify parameters of an allocated control pipe
 *
 * @param      pipe          The pipe
 * @param[in]  dev           The device address
 * @param[in]  ep            The endpoint address
 * @param[in]  max_pkt_size  The maximum packet size, must be equal or
 *                           less than allocated size
 * @param[in]  speed         The working speed
 *
 * @return     Operation result status
 * @retval     ERR_NOT_INITIALIZED Pipe is not allocated
 * @retval     ERR_BUSY Pipe is busy transferring
 * @retval     ERR_INVALID_ARG Argument error
 * @retval     ERR_UNSUPPORTED_OP Pipe is not control pipe
 * @retval     ERR_NONE Operation done successfully
 */
int32_t _usb_h_pipe_set_control_param(struct usb_h_pipe *pipe, uint8_t dev, uint8_t ep, uint16_t max_pkt_size,
                                      uint8_t speed);

/**
 * @brief      Register transfer callback on a pipe
 *
 * @param      pipe  The pipe
 * @param[in]  cb    Transfer callback function
 *
 * @return     Operation result status
 * @retval     ERR_INVALID_ARG Argument error
 * @retval     ERR_NONE Operation done successfully
 */
int32_t _usb_h_pipe_register_callback(struct usb_h_pipe *pipe, usb_h_pipe_cb_xfer_t cb);

/**
 * @brief      Issue a control transfer (request) on a pipe
 *
 * \note When there is data stage, timeout between data packets is 500ms, the
 *       timeout between last data packet and the status packet is 50ms.
 *
 * @param         pipe    The pipe
 * @param[in]     setup   Pointer to the setup packet
 * @param[in,out] data    Pointer to the data buffer
 * @param[in]     length  The data length
 * @param[in]     timeout Timeout for whole request in ms
 *
 * @return     Operation result status
 * @retval     ERR_NOT_INITIALIZED Pipe is not allocated
 * @retval     ERR_BUSY Pipe is busy transferring
 * @retval     ERR_INVALID_ARG Argument error
 * @retval     ERR_UNSUPPORTED_OP Pipe is not control pipe
 * @retval     ERR_NONE Operation done successfully
 */
int32_t _usb_h_control_xfer(struct usb_h_pipe *pipe, uint8_t *setup, uint8_t *data, uint16_t length, int16_t timeout);

/**
 * @brief      Issue a bulk / interrupt / iso transfer on a pipe
 *
 * @param         pipe     The pipe
 * @param[in,out] data     Pointer to the data buffer
 * @param[in]     length   The data length
 * @param[in]     auto_zlp Auto append ZLP for OUT
 *
 * @return     Operation result status
 * @retval     ERR_NOT_INITIALIZED Pipe is not allocated
 * @retval     ERR_BUSY Pipe is busy transferring
 * @retval     ERR_INVALID_ARG Argument error
 * @retval     ERR_UNSUPPORTED_OP Pipe is control pipe
 * @retval     ERR_NONE Operation done successfully
 */
int32_t _usb_h_bulk_int_iso_xfer(struct usb_h_pipe *pipe, uint8_t *data, uint32_t length, bool auto_zlp);

/**
 * @brief      Issue a periodic high bandwidth output on a pipe
 *
 * @param         pipe             The pipe
 * @param[in,out] data             Pointer to the data buffer
 * @param[in]     length           The data length
 * @param[in]     trans_pkt_size   The transaction packet sizes in a micro frame,
 *                                 0 to use endpoint max packet size
 *
 * @return     Operation result status
 * @retval     ERR_NOT_INITIALIZED Pipe is not allocated
 * @retval     ERR_BUSY Pipe is busy transferring
 * @retval     ERR_INVALID_ARG Argument error
 * @retval     ERR_UNSUPPORTED_OP Pipe is not high bandwidth periodic pipe, or
 *                                DMA feature not enabled, or
 *                                high bandwidth not enabled
 * @retval     ERR_NONE Operation done successfully
 */
int32_t _usb_h_high_bw_out(struct usb_h_pipe *pipe, uint8_t *data, uint32_t length, uint16_t trans_pkt_size[3]);

/**
 * @brief      Check if pipe is busy transferring
 *
 * @param      pipe  The pipe
 *
 * @return     \c true if pipe is busy
 */
bool _usb_h_pipe_is_busy(struct usb_h_pipe *pipe);

/**
 * @brief      Abort pending transfer on a pipe
 *
 * @param      pipe  The pipe
 */
void _usb_h_pipe_abort(struct usb_h_pipe *pipe);

#ifdef __cplusplus
}
#endif

#endif /* _HPL_USB_HOST_H_INCLUDED */
