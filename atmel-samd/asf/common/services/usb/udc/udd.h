/**
 * \file
 *
 * \brief Common API for USB Device Drivers (UDD)
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

#ifndef _UDD_H_
#define _UDD_H_

#include "usb_protocol.h"
#include "udc_desc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup usb_device_group
 * \defgroup udd_group USB Device Driver (UDD)
 *
 * The UDD driver provides a low-level abstraction of the device
 * controller hardware. Most events coming from the hardware such as
 * interrupts, which may cause the UDD to call into the UDC and UDI.
 *
 * @{
 */

//! \brief Endpoint identifier
typedef uint8_t udd_ep_id_t;

//! \brief Endpoint transfer status
//! Returned in parameters of callback register via udd_ep_run routine.
typedef enum {
	UDD_EP_TRANSFER_OK = 0,
	UDD_EP_TRANSFER_ABORT = 1,
} udd_ep_status_t;

/**
 * \brief Global variable to give and record information of the setup request management
 *
 * This global variable allows to decode and response a setup request.
 * It can be updated by udc_process_setup() from UDC or *setup() from UDIs.
 */
typedef struct {
	//! Data received in USB SETUP packet
	//! Note: The swap of "req.wValues" from uin16_t to le16_t is done by UDD.
	usb_setup_req_t req;

	//! Point to buffer to send or fill with data following SETUP packet
	//! This buffer must be word align for DATA IN phase (use prefix COMPILER_WORD_ALIGNED for buffer)
	uint8_t *payload;

	//! Size of buffer to send or fill, and content the number of byte transfered
	uint16_t payload_size;

	//! Callback called after reception of ZLP from setup request
	void (*callback) (void);

	//! Callback called when the buffer given (.payload) is full or empty.
	//! This one return false to abort data transfer, or true with a new buffer in .payload.
	bool(*over_under_run) (void);
} udd_ctrl_request_t;
extern udd_ctrl_request_t udd_g_ctrlreq;

//! Return true if the setup request \a udd_g_ctrlreq indicates IN data transfer
#define  Udd_setup_is_in()       \
      (USB_REQ_DIR_IN == (udd_g_ctrlreq.req.bmRequestType & USB_REQ_DIR_MASK))

//! Return true if the setup request \a udd_g_ctrlreq indicates OUT data transfer
#define  Udd_setup_is_out()      \
      (USB_REQ_DIR_OUT == (udd_g_ctrlreq.req.bmRequestType & USB_REQ_DIR_MASK))

//! Return the type of the SETUP request \a udd_g_ctrlreq. \see usb_reqtype.
#define  Udd_setup_type()        \
      (udd_g_ctrlreq.req.bmRequestType & USB_REQ_TYPE_MASK)

//! Return the recipient of the SETUP request \a udd_g_ctrlreq. \see usb_recipient
#define  Udd_setup_recipient()   \
      (udd_g_ctrlreq.req.bmRequestType & USB_REQ_RECIP_MASK)

/**
 * \brief End of halt callback function type.
 * Registered by routine udd_ep_wait_stall_clear()
 * Callback called when endpoint stall is cleared.
 */
typedef void (*udd_callback_halt_cleared_t) (void);

/**
 * \brief End of transfer callback function type.
 * Registered by routine udd_ep_run()
 * Callback called by USB interrupt after data transfer or abort (reset,...).
 *
 * \param status     UDD_EP_TRANSFER_OK, if transfer is complete
 * \param status     UDD_EP_TRANSFER_ABORT, if transfer is aborted
 * \param n          number of data transfered
 */
typedef void (*udd_callback_trans_t) (udd_ep_status_t status,
		iram_size_t nb_transfered, udd_ep_id_t ep);

/**
 * \brief Authorizes the VBUS event
 *
 * \return true, if the VBUS monitoring is possible.
 */
bool udd_include_vbus_monitoring(void);

/**
 * \brief Enables the USB Device mode
 */
void udd_enable(void);

/**
 * \brief Disables the USB Device mode
 */
void udd_disable(void);

/**
 * \brief Attach device to the bus when possible
 *
 * \warning If a VBus control is included in driver,
 * then it will attach device when an acceptable Vbus
 * level from the host is detected.
 */
void udd_attach(void);

/**
 * \brief Detaches the device from the bus
 *
 * The driver must remove pull-up on USB line D- or D+.
 */
void udd_detach(void);

/**
 * \brief Test whether the USB Device Controller is running at high
 * speed or not.
 *
 * \return \c true if the Device is running at high speed mode, otherwise \c false.
 */
bool udd_is_high_speed(void);

/**
 * \brief Changes the USB address of device
 *
 * \param address    New USB address
 */
void udd_set_address(uint8_t address);

/**
 * \brief Returns the USB address of device
 *
 * \return USB address
 */
uint8_t udd_getaddress(void);

/**
 * \brief Returns the current start of frame number
 *
 * \return current start of frame number.
 */
uint16_t udd_get_frame_number(void);

/**
 * \brief Returns the current micro start of frame number
 *
 * \return current micro start of frame number required in high speed mode.
 */
uint16_t udd_get_micro_frame_number(void);

/*! \brief The USB driver sends a resume signal called Upstream Resume
 */
void udd_send_remotewakeup(void);

/**
 * \brief Load setup payload
 *
 * \param payload       Pointer on payload
 * \param payload_size  Size of payload
 */
void udd_set_setup_payload( uint8_t *payload, uint16_t payload_size );


/**
 * \name Endpoint Management
 *
 * The following functions allow drivers to create and remove
 * endpoints, as well as set, clear and query their "halted" and
 * "wedged" states.
 */
//@{

#if (USB_DEVICE_MAX_EP != 0)

/**
 * \brief Configures and enables an endpoint
 *
 * \param ep               Endpoint number including direction (USB_EP_DIR_IN/USB_EP_DIR_OUT).
 * \param bmAttributes     Attributes of endpoint declared in the descriptor.
 * \param MaxEndpointSize  Endpoint maximum size
 *
 * \return \c 1 if the endpoint is enabled, otherwise \c 0.
 */
bool udd_ep_alloc(udd_ep_id_t ep, uint8_t bmAttributes,
		uint16_t MaxEndpointSize);

/**
 * \brief Disables an endpoint
 *
 * \param ep               Endpoint number including direction (USB_EP_DIR_IN/USB_EP_DIR_OUT).
 */
void udd_ep_free(udd_ep_id_t ep);

/**
 * \brief Check if the endpoint \a ep is halted.
 *
 * \param ep The ID of the endpoint to check.
 *
 * \return \c 1 if \a ep is halted, otherwise \c 0.
 */
bool udd_ep_is_halted(udd_ep_id_t ep);

/**
 * \brief Set the halted state of the endpoint \a ep
 *
 * After calling this function, any transaction on \a ep will result
 * in a STALL handshake being sent. Any pending transactions will be
 * performed first, however.
 *
 * \param ep The ID of the endpoint to be halted
 *
 * \return \c 1 if \a ep is halted, otherwise \c 0.
 */
bool udd_ep_set_halt(udd_ep_id_t ep);

/**
 * \brief Clear the halted state of the endpoint \a ep
 *
 * After calling this function, any transaction on \a ep will
 * be handled normally, i.e. a STALL handshake will not be sent, and
 * the data toggle sequence will start at DATA0.
 *
 * \param ep The ID of the endpoint to be un-halted
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
bool udd_ep_clear_halt(udd_ep_id_t ep);

/**
 * \brief Registers a callback to call when endpoint halt is cleared
 *
 * \param ep            The ID of the endpoint to use
 * \param callback      NULL or function to call when endpoint halt is cleared
 *
 * \warning if the endpoint is not halted then the \a callback is called immediately.
 *
 * \return \c 1 if the register is accepted, otherwise \c 0.
 */
bool udd_ep_wait_stall_clear(udd_ep_id_t ep,
		udd_callback_halt_cleared_t callback);

/**
 * \brief Allows to receive or send data on an endpoint
 *
 * The driver uses a specific DMA USB to transfer data
 * from internal RAM to endpoint, if this one is available.
 * When the transfer is finished or aborted (stall, reset, ...), the \a callback is called.
 * The \a callback returns the transfer status and eventually the number of byte transfered.
 * Note: The control endpoint is not authorized.
 *
 * \param ep            The ID of the endpoint to use
 * \param b_shortpacket Enabled automatic short packet
 * \param buf           Buffer on Internal RAM to send or fill.
 *                      It must be align, then use COMPILER_WORD_ALIGNED.
 * \param buf_size      Buffer size to send or fill
 * \param callback      NULL or function to call at the end of transfer
 *
 * \warning About \a b_shortpacket, for IN endpoint it means that a short packet
 * (or a Zero Length Packet) will be sent to the USB line to properly close the usb
 * transfer at the end of the data transfer.
 * For Bulk and Interrupt OUT endpoint, it will automatically stop the transfer
 * at the end of the data transfer (received short packet).
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
bool udd_ep_run(udd_ep_id_t ep, bool b_shortpacket,
		uint8_t * buf, iram_size_t buf_size,
		udd_callback_trans_t callback);
/**
 * \brief Aborts transfer on going on endpoint
 *
 * If a transfer is on going, then it is stopped and
 * the callback registered is called to signal the end of transfer.
 * Note: The control endpoint is not authorized.
 *
 * \param ep            Endpoint to abort
 */
void udd_ep_abort(udd_ep_id_t ep);

#endif

//@}


/**
 * \name High speed test mode management
 *
 * The following functions allow the device to jump to a specific test mode required in high speed mode.
 */
//@{
void udd_test_mode_j(void);
void udd_test_mode_k(void);
void udd_test_mode_se0_nak(void);
void udd_test_mode_packet(void);
//@}


/**
 * \name UDC callbacks to provide for UDD
 *
 * The following callbacks are used by UDD.
 */
//@{

/**
 * \brief Decodes and manages a setup request
 *
 * The driver call it when a SETUP packet is received.
 * The \c udd_g_ctrlreq contains the data of SETUP packet.
 * If this callback accepts the setup request then it must
 * return \c 1 and eventually update \c udd_g_ctrlreq to send or receive data.
 *
 * \return \c 1 if the request is accepted, otherwise \c 0.
 */
extern bool udc_process_setup(void);

/**
 * \brief Reset the UDC
 *
 * The UDC must reset all configuration.
 */
extern void udc_reset(void);

/**
 * \brief To signal that a SOF is occurred
 *
 * The UDC must send the signal to all UDIs enabled
 */
extern void udc_sof_notify(void);

//@}

//@}

#ifdef __cplusplus
}
#endif
#endif // _UDD_H_
