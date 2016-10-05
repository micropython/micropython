/**
 * \file
 *
 * \brief USB Device wrapper layer for compliance with common driver UDD
 *
 * Copyright (C) 2014-2016 Atmel Corporation. All rights reserved.
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
#include <string.h>
#include <stdlib.h>

// Get USB device configuration
#include "conf_usb.h"
#include "udd.h"
#include "usb.h"
#include "usb_dual.h"
#include "sleepmgr.h"

/**
 * \ingroup usb_device_group
 * \defgroup usb_device_udd_group USB Device Driver Implement (UDD)
 * USB low-level driver for USB device mode
 * @{
 */
// Check USB device configuration
#ifdef USB_DEVICE_HS_SUPPORT
#  error The High speed mode is not supported on this part, please remove USB_DEVICE_HS_SUPPORT in conf_usb.h
#endif

#if !(SAMD21) && !(SAMR21) && !(SAMD11) && !(SAML21) && !(SAML22) && !(SAMDA1) && !(SAMR30)
# error The current USB Device Driver supports only SAMD21/R21/D11/L21/L22/DA1
#endif

#ifndef UDC_REMOTEWAKEUP_LPM_ENABLE
#define UDC_REMOTEWAKEUP_LPM_ENABLE()
#endif
#ifndef UDC_REMOTEWAKEUP_LPM_DISABLE
#define UDC_REMOTEWAKEUP_LPM_DISABLE()
#endif
#ifndef UDC_SUSPEND_LPM_EVENT
#define UDC_SUSPEND_LPM_EVENT()
#endif

/* for debug text */
#ifdef USB_DEBUG
#   define dbg_print printf
#else
#   define dbg_print(...)
#endif

/** Maximum size of a transfer in multi-packet mode */
#define UDD_ENDPOINT_MAX_TRANS ((8*1024)-1)

/** USB software device instance structure */
struct usb_module usb_device;

/**
 * \name Clock management
 *
 * @{
 */
#ifndef UDD_CLOCK_GEN
#if (SAML21) || (SAML22) || (SAMR30)
#  define UDD_CLOCK_GEN      GCLK_GENERATOR_3
#elif (SAMDA1)
#  define UDD_CLOCK_GEN      GCLK_GENERATOR_1
#else
#  define UDD_CLOCK_GEN      GCLK_GENERATOR_0
#endif
#endif
#ifndef UDD_CLOCK_SOURCE
#  define UDD_CLOCK_SOURCE   SYSTEM_CLOCK_SOURCE_DFLL
#endif
static inline void udd_wait_clock_ready(void)
{

	if (UDD_CLOCK_SOURCE == SYSTEM_CLOCK_SOURCE_DPLL) {
#if (SAML21) || (SAML22) || (SAMR30)
	#define DPLL_READY_FLAG (OSCCTRL_DPLLSTATUS_CLKRDY | OSCCTRL_DPLLSTATUS_LOCK)

		while((OSCCTRL->DPLLSTATUS.reg & DPLL_READY_FLAG) != DPLL_READY_FLAG);
	}
#else
#define DPLL_READY_FLAG (SYSCTRL_DPLLSTATUS_ENABLE | \
		SYSCTRL_DPLLSTATUS_CLKRDY | SYSCTRL_DPLLSTATUS_LOCK)

		while((SYSCTRL->DPLLSTATUS.reg & DPLL_READY_FLAG) != DPLL_READY_FLAG);
	}
#endif

	if (UDD_CLOCK_SOURCE == SYSTEM_CLOCK_SOURCE_DFLL) {
#if (SAML21) || (SAML22) || (SAMR30)
#define DFLL_READY_FLAG (OSCCTRL_STATUS_DFLLRDY | \
		OSCCTRL_STATUS_DFLLLCKF | OSCCTRL_STATUS_DFLLLCKC)

		/* In USB recovery mode the status is not checked */
		if (!(OSCCTRL->DFLLCTRL.reg & OSCCTRL_DFLLCTRL_USBCRM)) {
			while((OSCCTRL->STATUS.reg & DFLL_READY_FLAG) != DFLL_READY_FLAG);
		} else {
			while((OSCCTRL->STATUS.reg & OSCCTRL_STATUS_DFLLRDY) != OSCCTRL_STATUS_DFLLRDY);
		}
	}
#else
#define DFLL_READY_FLAG (SYSCTRL_PCLKSR_DFLLRDY | \
		SYSCTRL_PCLKSR_DFLLLCKF | SYSCTRL_PCLKSR_DFLLLCKC)

		/* In USB recovery mode the status is not checked */
		if (!(SYSCTRL->DFLLCTRL.reg & SYSCTRL_DFLLCTRL_USBCRM)) {
			while((SYSCTRL->PCLKSR.reg & DFLL_READY_FLAG) != DFLL_READY_FLAG);
		} else {
			while((SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY) != SYSCTRL_PCLKSR_DFLLRDY);
		}
	}
#endif
}
/** @} */

/**
 * \name Power management
 *
 * @{
 */
#ifndef UDD_NO_SLEEP_MGR

#include "sleepmgr.h"
/** States of USB interface */
enum udd_usb_state_enum {
	UDD_STATE_OFF,
	UDD_STATE_SUSPEND,
	UDD_STATE_SUSPEND_LPM,
	UDD_STATE_IDLE,
};

/** \brief Manages the sleep mode following the USB state
 *
 * \param new_state  New USB state
 */
static void udd_sleep_mode(enum udd_usb_state_enum new_state)
{
	enum sleepmgr_mode sleep_mode[] = {
		SLEEPMGR_ACTIVE,  /* UDD_STATE_OFF (not used) */
	#if (SAML21) || (SAML22) || (SAMR30)
		SLEEPMGR_IDLE,  /* UDD_STATE_SUSPEND */
		SLEEPMGR_IDLE,  /* UDD_STATE_SUSPEND_LPM */
		SLEEPMGR_IDLE,  /* UDD_STATE_IDLE */
	#else
		SLEEPMGR_IDLE_2,  /* UDD_STATE_SUSPEND */
		SLEEPMGR_IDLE_1,  /* UDD_STATE_SUSPEND_LPM */
		SLEEPMGR_IDLE_0,  /* UDD_STATE_IDLE */
	#endif
	};

	static enum udd_usb_state_enum udd_state = UDD_STATE_OFF;

	if (udd_state == new_state) {
		return; // No change
	}
	if (new_state != UDD_STATE_OFF) {
		/* Lock new limit */
		sleepmgr_lock_mode(sleep_mode[new_state]);
	}
	if (udd_state != UDD_STATE_OFF) {
		/* Unlock old limit */
		sleepmgr_unlock_mode(sleep_mode[udd_state]);
	}
	udd_state = new_state;
}

#else
#  define udd_sleep_mode(arg)
#endif
/** @} */

/**
 * \name Control endpoint low level management routine.
 *
 * This function performs control endpoint management.
 * It handles the SETUP/DATA/HANDSHAKE phases of a control transaction.
 *
 * @{
 */

/**
 * \brief Buffer to store the data received on control endpoint (SETUP/OUT endpoint 0)
 *
 * Used to avoid a RAM buffer overflow in case of the payload buffer
 * is smaller than control endpoint size
 */
UDC_BSS(4)
uint8_t udd_ctrl_buffer[USB_DEVICE_EP_CTRL_SIZE];

/** Bit definitions about endpoint control state machine for udd_ep_control_state */
typedef enum {
	UDD_EPCTRL_SETUP                  = 0, //!< Wait a SETUP packet
	UDD_EPCTRL_DATA_OUT               = 1, //!< Wait a OUT data packet
	UDD_EPCTRL_DATA_IN                = 2, //!< Wait a IN data packet
	UDD_EPCTRL_HANDSHAKE_WAIT_IN_ZLP  = 3, //!< Wait a IN ZLP packet
	UDD_EPCTRL_HANDSHAKE_WAIT_OUT_ZLP = 4, //!< Wait a OUT ZLP packet
	UDD_EPCTRL_STALL_REQ              = 5, //!< STALL enabled on IN & OUT packet
} udd_ctrl_ep_state_t;

/** Global variable to give and record information of the set up request management */
udd_ctrl_request_t udd_g_ctrlreq;

/** State of the endpoint control management */
static udd_ctrl_ep_state_t udd_ep_control_state;

/** Total number of data received/sent during data packet phase with previous payload buffers */
static uint16_t udd_ctrl_prev_payload_nb_trans;

/** Number of data received/sent to/from udd_g_ctrlreq.payload buffer */
static uint16_t udd_ctrl_payload_nb_trans;

/** @} */

/**
 * \name Management of bulk/interrupt/isochronous endpoints
 *
 * The UDD manages the data transfer on endpoints:
 * - Start data transfer on endpoint with USB Device DMA
 * - Send a ZLP packet if requested
 * - Call callback registered to signal end of transfer
 * The transfer abort and stall feature are supported.
 *
 * @{
 */

/**
 * \brief Buffer to store the data received on bulk/interrupt endpoints
 *
 * Used to avoid a RAM buffer overflow in case of the user buffer
 * is smaller than endpoint size
 *
 * \warning The protected interrupt endpoint size is 512 bytes maximum.
 * \warning The isochronous and endpoint is not protected by this system and
 *          the user must always use a buffer corresponding at endpoint size.
 */
#if (defined USB_DEVICE_LOW_SPEED)
UDC_BSS(4) uint8_t udd_ep_out_cache_buffer[USB_DEVICE_MAX_EP][8];
#elif (defined USB_DEVICE_HS_SUPPORT)
UDC_BSS(4) uint8_t udd_ep_out_cache_buffer[USB_DEVICE_MAX_EP][512];
#else
UDC_BSS(4) uint8_t udd_ep_out_cache_buffer[USB_DEVICE_MAX_EP][64];
#endif

/** Structure definition about job registered on an endpoint */
typedef struct {
	union {
		//! Callback to call at the end of transfer
		udd_callback_trans_t call_trans;
		//! Callback to call when the endpoint halt is cleared
		udd_callback_halt_cleared_t call_nohalt;
	};
	//! Buffer located in internal RAM to send or fill during job
	uint8_t *buf;
	//! Size of buffer to send or fill
	iram_size_t buf_size;
	//! Total number of data transferred on endpoint
	iram_size_t nb_trans;
	//! Endpoint size
	uint16_t ep_size;
	//! A job is registered on this endpoint
	uint8_t busy:1;
	//! A short packet is requested for this job on endpoint IN
	uint8_t b_shortpacket:1;
	//! The cache buffer is currently used on endpoint OUT
	uint8_t b_use_out_cache_buffer:1;
} udd_ep_job_t;

/** Array to register a job on bulk/interrupt/isochronous endpoint */
static udd_ep_job_t udd_ep_job[2 * USB_DEVICE_MAX_EP];

/** @} */

/**
 * \brief     Get the detailed job by endpoint number
 * \param[in] ep  Endpoint Address
 * \retval    pointer to an udd_ep_job_t structure instance
 */
static udd_ep_job_t* udd_ep_get_job(udd_ep_id_t ep)
{
	return &udd_ep_job[(2 * (ep & USB_EP_ADDR_MASK) + ((ep & USB_EP_DIR_IN) ? 1 : 0)) - 2];
}

/**
 * \brief     Endpoint IN process, continue to send packets or zero length packet
 * \param[in] pointer Pointer to the endpoint transfer status parameter struct from driver layer.
 */
static void udd_ep_trans_in_next(void* pointer)
{
	struct usb_endpoint_callback_parameter *ep_callback_para = (struct usb_endpoint_callback_parameter*)pointer;
	udd_ep_id_t ep = ep_callback_para->endpoint_address;
	uint16_t ep_size, nb_trans;
	uint16_t next_trans;
	udd_ep_id_t ep_num;
	udd_ep_job_t *ptr_job;

	ptr_job = udd_ep_get_job(ep);
	ep_num = ep & USB_EP_ADDR_MASK;

	ep_size = ptr_job->ep_size;
	/* Update number of data transferred */
	nb_trans = ep_callback_para->sent_bytes;
	ptr_job->nb_trans += nb_trans;

	/* Need to send other data */
	if (ptr_job->nb_trans != ptr_job->buf_size) {
		next_trans = ptr_job->buf_size - ptr_job->nb_trans;
		if (UDD_ENDPOINT_MAX_TRANS < next_trans) {
		/* The USB hardware support a maximum
		 * transfer size of UDD_ENDPOINT_MAX_TRANS Bytes */
			next_trans = UDD_ENDPOINT_MAX_TRANS -(UDD_ENDPOINT_MAX_TRANS % ep_size);
		}
		/* Need ZLP, if requested and last packet is not a short packet */
		ptr_job->b_shortpacket = ptr_job->b_shortpacket && (0 == (next_trans % ep_size));
		usb_device_endpoint_write_buffer_job(&usb_device,ep_num,&ptr_job->buf[ptr_job->nb_trans],next_trans);
		return;
	}

	/* Need to send a ZLP after all data transfer */
	if (ptr_job->b_shortpacket) {
		ptr_job->b_shortpacket = false;
		/* Start new transfer */
		usb_device_endpoint_write_buffer_job(&usb_device,ep_num,&ptr_job->buf[ptr_job->nb_trans],0);
		return;
	}

	/* Job complete then call callback */
	ptr_job->busy = false;
	if (NULL != ptr_job->call_trans) {
		ptr_job->call_trans(UDD_EP_TRANSFER_OK, ptr_job->nb_trans, ep);
	}
}

/**
 * \brief     Endpoint OUT process, continue to receive packets or zero length packet
 * \param[in] pointer Pointer to the endpoint transfer status parameter struct from driver layer.
 */
static void udd_ep_trans_out_next(void* pointer)
{
	struct usb_endpoint_callback_parameter *ep_callback_para = (struct usb_endpoint_callback_parameter*)pointer;
	udd_ep_id_t ep = ep_callback_para->endpoint_address;
	uint16_t ep_size, nb_trans;
	uint16_t next_trans;
	udd_ep_id_t ep_num;
	udd_ep_job_t *ptr_job;

	ptr_job = udd_ep_get_job(ep);
	ep_num = ep & USB_EP_ADDR_MASK;

	ep_size = ptr_job->ep_size;
	/* Update number of data transferred */
	nb_trans = ep_callback_para->received_bytes;

	/* Can be necessary to copy data receive from cache buffer to user buffer */
	if (ptr_job->b_use_out_cache_buffer) {
		memcpy(&ptr_job->buf[ptr_job->nb_trans], udd_ep_out_cache_buffer[ep_num - 1], ptr_job->buf_size % ep_size);
	}

	/* Update number of data transferred */
	ptr_job->nb_trans += nb_trans;
	if (ptr_job->nb_trans > ptr_job->buf_size) {
		ptr_job->nb_trans = ptr_job->buf_size;
	}

	/* If all previous data requested are received and user buffer not full
	 * then need to receive other data */
	if ((nb_trans == ep_callback_para->out_buffer_size) && (ptr_job->nb_trans != ptr_job->buf_size)) {
		next_trans = ptr_job->buf_size - ptr_job->nb_trans;
		if (UDD_ENDPOINT_MAX_TRANS < next_trans) {
		/* The USB hardware support a maximum transfer size
		 * of UDD_ENDPOINT_MAX_TRANS Bytes */
		next_trans = UDD_ENDPOINT_MAX_TRANS - (UDD_ENDPOINT_MAX_TRANS % ep_size);
		} else {
			next_trans -= next_trans % ep_size;
		}

		if (next_trans < ep_size) {
			/* Use the cache buffer for Bulk or Interrupt size endpoint */
			ptr_job->b_use_out_cache_buffer = true;
			usb_device_endpoint_read_buffer_job(&usb_device,ep_num,udd_ep_out_cache_buffer[ep_num - 1],ep_size);
		} else {
			usb_device_endpoint_read_buffer_job(&usb_device,ep_num,&ptr_job->buf[ptr_job->nb_trans],next_trans);
		}
		return;
	}

	/* Job complete then call callback */
	ptr_job->busy = false;
	if (NULL != ptr_job->call_trans) {
		ptr_job->call_trans(UDD_EP_TRANSFER_OK, ptr_job->nb_trans, ep);
	}
}

/**
 * \brief     Endpoint Transfer Complete callback function, to do the next transfer depends on the direction(IN or OUT)
 * \param[in] module_inst Pointer to USB module instance
 * \param[in] pointer Pointer to the endpoint transfer status parameter struct from driver layer.
 */
static void udd_ep_transfer_process(struct usb_module *module_inst, void* pointer)
{
	struct usb_endpoint_callback_parameter *ep_callback_para = (struct usb_endpoint_callback_parameter*)pointer;
	udd_ep_id_t ep = ep_callback_para->endpoint_address;

	if (ep & USB_EP_DIR_IN) {
		udd_ep_trans_in_next(pointer);
	} else {
		udd_ep_trans_out_next(pointer);
	}
}

void udd_ep_abort(udd_ep_id_t ep)
{
	udd_ep_job_t *ptr_job;

	usb_device_endpoint_abort_job(&usb_device, ep);

	/* Job complete then call callback */
	ptr_job = udd_ep_get_job(ep);
	if (!ptr_job->busy) {
		return;
	}
	ptr_job->busy = false;
	if (NULL != ptr_job->call_trans) {
		/* It can be a Transfer or stall callback */
		ptr_job->call_trans(UDD_EP_TRANSFER_ABORT, ptr_job->nb_trans, ep);
	}
}

bool udd_is_high_speed(void)
{
#if SAMD21 || SAMR21 || SAMD11 || SAML21  || SAML22 || SAMDA1 || SAMR30
	return false;
#endif
}

uint16_t udd_get_frame_number(void)
{
	return usb_device_get_frame_number(&usb_device);
}

uint16_t udd_get_micro_frame_number(void)
{
	return usb_device_get_micro_frame_number(&usb_device);
}

void udd_ep_free(udd_ep_id_t ep)
{
	struct usb_device_endpoint_config config_ep;
	usb_device_endpoint_get_config_defaults(&config_ep);

	uint8_t ep_num = ep & USB_EP_ADDR_MASK;
	udd_ep_abort(ep);

	config_ep.ep_address = ep;
	config_ep.ep_type = USB_DEVICE_ENDPOINT_TYPE_DISABLE;
	usb_device_endpoint_set_config(&usb_device, &config_ep);
	usb_device_endpoint_unregister_callback(&usb_device,ep_num,USB_DEVICE_ENDPOINT_CALLBACK_TRCPT);
	usb_device_endpoint_disable_callback(&usb_device,ep,USB_DEVICE_ENDPOINT_CALLBACK_TRCPT);
}

bool udd_ep_alloc(udd_ep_id_t ep, uint8_t bmAttributes, uint16_t MaxEndpointSize)
{
	struct usb_device_endpoint_config config_ep;
	usb_device_endpoint_get_config_defaults(&config_ep);

	config_ep.ep_address = ep;

	if(MaxEndpointSize <= 8) {
		config_ep.ep_size = USB_ENDPOINT_8_BYTE;
	} else if(MaxEndpointSize <= 16) {
		config_ep.ep_size = USB_ENDPOINT_16_BYTE;
	} else if(MaxEndpointSize <= 32) {
		config_ep.ep_size = USB_ENDPOINT_32_BYTE;
	} else if(MaxEndpointSize <= 64) {
		config_ep.ep_size = USB_ENDPOINT_64_BYTE;
	} else if(MaxEndpointSize <= 128) {
		config_ep.ep_size = USB_ENDPOINT_128_BYTE;
	} else if(MaxEndpointSize <= 256) {
		config_ep.ep_size = USB_ENDPOINT_256_BYTE;
	} else if(MaxEndpointSize <= 512) {
		config_ep.ep_size = USB_ENDPOINT_512_BYTE;
	} else if(MaxEndpointSize <= 1023) {
		config_ep.ep_size = USB_ENDPOINT_1023_BYTE;
	} else {
		return false;
	}
	udd_ep_job_t *ptr_job = udd_ep_get_job(ep);
	ptr_job->ep_size = MaxEndpointSize;

	bmAttributes = bmAttributes & USB_EP_TYPE_MASK;

	/* Check endpoint type */
	if(USB_EP_TYPE_ISOCHRONOUS == bmAttributes) {
		config_ep.ep_type = USB_DEVICE_ENDPOINT_TYPE_ISOCHRONOUS;
	} else if (USB_EP_TYPE_BULK == bmAttributes) {
		config_ep.ep_type = USB_DEVICE_ENDPOINT_TYPE_BULK;
	} else if (USB_EP_TYPE_INTERRUPT == bmAttributes) {
		config_ep.ep_type = USB_DEVICE_ENDPOINT_TYPE_INTERRUPT;
	} else {
		return false;
	}

	uint8_t ep_num = ep & USB_EP_ADDR_MASK;

	if (STATUS_OK != usb_device_endpoint_set_config(&usb_device, &config_ep)) {
		return false;
	}
	usb_device_endpoint_register_callback(&usb_device,ep_num,USB_DEVICE_ENDPOINT_CALLBACK_TRCPT,udd_ep_transfer_process);
	usb_device_endpoint_enable_callback(&usb_device,ep,USB_DEVICE_ENDPOINT_CALLBACK_TRCPT);
	usb_device_endpoint_enable_callback(&usb_device,ep,USB_DEVICE_ENDPOINT_CALLBACK_TRFAIL);

	return true;
}

bool udd_ep_is_halted(udd_ep_id_t ep)
{
	return usb_device_endpoint_is_halted(&usb_device, ep);
}

bool udd_ep_set_halt(udd_ep_id_t ep)
{
	uint8_t ep_num = ep & USB_EP_ADDR_MASK;

	if (USB_DEVICE_MAX_EP < ep_num) {
		return false;
	}

	usb_device_endpoint_set_halt(&usb_device, ep);

	udd_ep_abort(ep);
	return true;
}

bool udd_ep_clear_halt(udd_ep_id_t ep)
{
	udd_ep_job_t *ptr_job;
	uint8_t ep_num = ep & USB_EP_ADDR_MASK;

	if (USB_DEVICE_MAX_EP < ep_num) {
		return false;
	}
	ptr_job = udd_ep_get_job(ep);

	usb_device_endpoint_clear_halt(&usb_device, ep);

	/* If a job is register on clear halt action then execute callback */
	if (ptr_job->busy == true) {
		ptr_job->busy = false;
		ptr_job->call_nohalt();
	}

	return true;
}

bool udd_ep_wait_stall_clear(udd_ep_id_t ep, udd_callback_halt_cleared_t callback)
{
	udd_ep_id_t ep_num;
	udd_ep_job_t *ptr_job;

	ep_num = ep & USB_EP_ADDR_MASK;
	if (USB_DEVICE_MAX_EP < ep_num) {
		return false;
	}

	ptr_job = udd_ep_get_job(ep);
	if (ptr_job->busy == true) {
		return false; /* Job already on going */
	}

	/* Wait clear halt endpoint */
	if (usb_device_endpoint_is_halted(&usb_device, ep)) {
		/* Endpoint halted then registers the callback */
		ptr_job->busy = true;
		ptr_job->call_nohalt = callback;
		return true;
	} else if (usb_device_endpoint_is_configured(&usb_device, ep)) {
		callback(); /* Endpoint not halted then call directly callback */
		return true;
	} else {
		return false;
	}
}

/**
 * \brief Control Endpoint stall sending data
 */
static void udd_ctrl_stall_data(void)
{
	udd_ep_control_state = UDD_EPCTRL_STALL_REQ;

	usb_device_endpoint_set_halt(&usb_device, USB_EP_DIR_IN);
	usb_device_endpoint_clear_halt(&usb_device, USB_EP_DIR_OUT);
}

bool udd_ep_run(udd_ep_id_t ep, bool b_shortpacket, uint8_t * buf, iram_size_t buf_size, udd_callback_trans_t callback)
{
	udd_ep_id_t ep_num;
	udd_ep_job_t *ptr_job;
	irqflags_t flags;

	ep_num = ep & USB_EP_ADDR_MASK;

	if ((USB_DEVICE_MAX_EP < ep_num) || (udd_ep_is_halted(ep))) {
		return false;
	}

	ptr_job = udd_ep_get_job(ep);

	flags = cpu_irq_save();
	if (ptr_job->busy == true) {
		cpu_irq_restore(flags);
		return false; /* Job already on going */
	}
	ptr_job->busy = true;
	cpu_irq_restore(flags);

	/* No job running, set up a new one */
	ptr_job->buf = buf;
	ptr_job->buf_size = buf_size;
	ptr_job->nb_trans = 0;
	ptr_job->call_trans = callback;
	ptr_job->b_shortpacket = b_shortpacket;
	ptr_job->b_use_out_cache_buffer = false;

	/* Initialize value to simulate a empty transfer */
	uint16_t next_trans;

	if (ep & USB_EP_DIR_IN) {
		if (0 != ptr_job->buf_size) {
			next_trans = ptr_job->buf_size;
			if (UDD_ENDPOINT_MAX_TRANS < next_trans) {
				next_trans = UDD_ENDPOINT_MAX_TRANS -
						(UDD_ENDPOINT_MAX_TRANS % ptr_job->ep_size);
			}
			ptr_job->b_shortpacket = ptr_job->b_shortpacket &&
					(0 == (next_trans % ptr_job->ep_size));
		} else if (true == ptr_job->b_shortpacket) {
			ptr_job->b_shortpacket = false; /* avoid to send zero length packet again */
			next_trans = 0;
		} else {
			ptr_job->busy = false;
			if (NULL != ptr_job->call_trans) {
				ptr_job->call_trans(UDD_EP_TRANSFER_OK, 0, ep);
			}
			return true;
		}
		return (STATUS_OK ==
				usb_device_endpoint_write_buffer_job(&usb_device,
						ep_num,&ptr_job->buf[0],next_trans));
	} else {
		if (0 != ptr_job->buf_size) {
			next_trans = ptr_job->buf_size;
			if (UDD_ENDPOINT_MAX_TRANS < next_trans) {
				/* The USB hardware support a maximum transfer size
				 * of UDD_ENDPOINT_MAX_TRANS Bytes */
				next_trans = UDD_ENDPOINT_MAX_TRANS -
						(UDD_ENDPOINT_MAX_TRANS % ptr_job->ep_size);
			} else {
				next_trans -= next_trans % ptr_job->ep_size;
			}
			if (next_trans < ptr_job->ep_size) {
				ptr_job->b_use_out_cache_buffer = true;
				return (STATUS_OK ==
						usb_device_endpoint_read_buffer_job(&usb_device, ep_num,
								udd_ep_out_cache_buffer[ep_num - 1],
								ptr_job->ep_size));
			} else {
				return (STATUS_OK ==
						usb_device_endpoint_read_buffer_job(&usb_device, ep_num,
								&ptr_job->buf[0],next_trans));
			}
		} else {
			ptr_job->busy = false;
			if (NULL != ptr_job->call_trans) {
				ptr_job->call_trans(UDD_EP_TRANSFER_OK, 0, ep);
			}
			return true;
		}
	}
}

void udd_set_address(uint8_t address)
{
	usb_device_set_address(&usb_device,address);
}

uint8_t udd_getaddress(void)
{
	return usb_device_get_address(&usb_device);
}

void udd_send_remotewakeup(void)
{
	uint32_t try = 5;
	udd_wait_clock_ready();
	udd_sleep_mode(UDD_STATE_IDLE);
	while(2 != usb_get_state_machine_status(&usb_device) && try --) {
		usb_device_send_remote_wake_up(&usb_device);
	}
}

void udd_set_setup_payload( uint8_t *payload, uint16_t payload_size )
{
	udd_g_ctrlreq.payload = payload;
	udd_g_ctrlreq.payload_size = payload_size;
}

/**
 * \brief Control Endpoint translate the data in buffer into Device Request Struct
 */
static void udd_ctrl_fetch_ram(void)
{
	udd_g_ctrlreq.req.bmRequestType = udd_ctrl_buffer[0];
	udd_g_ctrlreq.req.bRequest = udd_ctrl_buffer[1];
	udd_g_ctrlreq.req.wValue = ((uint16_t)(udd_ctrl_buffer[3]) << 8) + udd_ctrl_buffer[2];
	udd_g_ctrlreq.req.wIndex = ((uint16_t)(udd_ctrl_buffer[5]) << 8) + udd_ctrl_buffer[4];
	udd_g_ctrlreq.req.wLength = ((uint16_t)(udd_ctrl_buffer[7]) << 8) + udd_ctrl_buffer[6];
}

/**
 * \brief Control Endpoint send out zero length packet
 */
static void udd_ctrl_send_zlp_in(void)
{
	udd_ep_control_state = UDD_EPCTRL_HANDSHAKE_WAIT_IN_ZLP;
	usb_device_endpoint_setup_buffer_job(&usb_device,udd_ctrl_buffer);
	usb_device_endpoint_write_buffer_job(&usb_device,0,udd_g_ctrlreq.payload,0);
}

/**
 * \brief Process control endpoint IN transaction
 */
static void udd_ctrl_in_sent(void)
{
	static bool b_shortpacket = false;
	uint16_t nb_remain;

	nb_remain = udd_g_ctrlreq.payload_size - udd_ctrl_payload_nb_trans;

	if (0 == nb_remain) {
		/* All content of current buffer payload are sent Update number of total data sending by previous payload buffer */
		udd_ctrl_prev_payload_nb_trans += udd_ctrl_payload_nb_trans;
		if ((udd_g_ctrlreq.req.wLength == udd_ctrl_prev_payload_nb_trans) || b_shortpacket) {
			/* All data requested are transferred or a short packet has been sent, then it is the end of data phase.
			 * Generate an OUT ZLP for handshake phase */
			udd_ep_control_state = UDD_EPCTRL_HANDSHAKE_WAIT_OUT_ZLP;
			usb_device_endpoint_setup_buffer_job(&usb_device,udd_ctrl_buffer);
			return;
		}
		/* Need of new buffer because the data phase is not complete */
		if ((!udd_g_ctrlreq.over_under_run) || (!udd_g_ctrlreq.over_under_run())) {
			/* Under run then send zlp on IN
			 * Here nb_remain=0, this allows to send a IN ZLP */
		} else {
			/* A new payload buffer is given */
			udd_ctrl_payload_nb_trans = 0;
			nb_remain = udd_g_ctrlreq.payload_size;
		}
	}

	/* Continue transfer and send next data */
	if (nb_remain >= USB_DEVICE_EP_CTRL_SIZE) {
		nb_remain = USB_DEVICE_EP_CTRL_SIZE;
		b_shortpacket = false;
	} else {
		b_shortpacket = true;
	}

	/* Link payload buffer directly on USB hardware */
	usb_device_endpoint_write_buffer_job(&usb_device,0,udd_g_ctrlreq.payload + udd_ctrl_payload_nb_trans,nb_remain);

	udd_ctrl_payload_nb_trans += nb_remain;
}

/**
 * \brief Process control endpoint OUT transaction
 * \param[in] pointer Pointer to the endpoint transfer status parameter struct from driver layer.
 */
static void udd_ctrl_out_received(void* pointer)
{
	struct usb_endpoint_callback_parameter *ep_callback_para = (struct usb_endpoint_callback_parameter*)pointer;

	uint16_t nb_data;
	nb_data = ep_callback_para->received_bytes; /* Read data received during OUT phase */

	if (udd_g_ctrlreq.payload_size < (udd_ctrl_payload_nb_trans + nb_data)) {
		/* Payload buffer too small */
		nb_data = udd_g_ctrlreq.payload_size - udd_ctrl_payload_nb_trans;
	}

	memcpy((uint8_t *) (udd_g_ctrlreq.payload + udd_ctrl_payload_nb_trans), udd_ctrl_buffer, nb_data);
	udd_ctrl_payload_nb_trans += nb_data;

	if ((USB_DEVICE_EP_CTRL_SIZE != nb_data) || \
	(udd_g_ctrlreq.req.wLength <= (udd_ctrl_prev_payload_nb_trans + udd_ctrl_payload_nb_trans))) {
		/* End of reception because it is a short packet
		 * or all data are transferred */

		/* Before send ZLP, call intermediate callback
		 * in case of data receive generate a stall */
		udd_g_ctrlreq.payload_size = udd_ctrl_payload_nb_trans;
		if (NULL != udd_g_ctrlreq.over_under_run) {
			if (!udd_g_ctrlreq.over_under_run()) {
				/* Stall ZLP */
				udd_ep_control_state = UDD_EPCTRL_STALL_REQ;
				/* Stall all packets on IN & OUT control endpoint */
				udd_ep_set_halt(0);
				/* Ack reception of OUT to replace NAK by a STALL */
				return;
			}
		}
		/* Send IN ZLP to ACK setup request */
		udd_ctrl_send_zlp_in();
		return;
	}

	if (udd_g_ctrlreq.payload_size == udd_ctrl_payload_nb_trans) {
		/* Overrun then request a new payload buffer */
		if (!udd_g_ctrlreq.over_under_run) {
			/* No callback available to request a new payload buffer
			 * Stall ZLP */
			udd_ep_control_state = UDD_EPCTRL_STALL_REQ;
			/* Stall all packets on IN & OUT control endpoint */
			udd_ep_set_halt(0);
			return;
		}
		if (!udd_g_ctrlreq.over_under_run()) {
			/* No new payload buffer delivered
			 * Stall ZLP */
			udd_ep_control_state = UDD_EPCTRL_STALL_REQ;
			/* Stall all packets on IN & OUT control endpoint */
			udd_ep_set_halt(0);
			return;
		}
		/* New payload buffer available
		 * Update number of total data received */
		udd_ctrl_prev_payload_nb_trans += udd_ctrl_payload_nb_trans;

		/* Reinitialize reception on payload buffer */
		udd_ctrl_payload_nb_trans = 0;
	}
	usb_device_endpoint_read_buffer_job(&usb_device,0,udd_ctrl_buffer,USB_DEVICE_EP_CTRL_SIZE);
}

/**
 * \internal
 * \brief     Endpoint 0 (control) SETUP received callback
 * \param[in] module_inst pointer to USB module instance
 * \param[in] pointer Pointer to the endpoint transfer status parameter struct from driver layer.
 */
static void _usb_ep0_on_setup(struct usb_module *module_inst, void* pointer)
{
	struct usb_endpoint_callback_parameter *ep_callback_para = (struct usb_endpoint_callback_parameter*)pointer;

	if (UDD_EPCTRL_SETUP != udd_ep_control_state) {
		if (NULL != udd_g_ctrlreq.callback) {
			udd_g_ctrlreq.callback();
		}
		udd_ep_control_state = UDD_EPCTRL_SETUP;
	}
	if ( 8 != ep_callback_para->received_bytes) {
		udd_ctrl_stall_data();
		return;
	} else {
		udd_ctrl_fetch_ram();
		if (false == udc_process_setup()) {
			udd_ctrl_stall_data();
			return;
		} else if (Udd_setup_is_in()) {
			udd_ctrl_prev_payload_nb_trans = 0;
			udd_ctrl_payload_nb_trans = 0;
			udd_ep_control_state = UDD_EPCTRL_DATA_IN;
			usb_device_endpoint_read_buffer_job(&usb_device,0,udd_ctrl_buffer,USB_DEVICE_EP_CTRL_SIZE);
			udd_ctrl_in_sent();
		} else {
			if(0 == udd_g_ctrlreq.req.wLength) {
				udd_ctrl_send_zlp_in();
				return;
			} else {
				udd_ctrl_prev_payload_nb_trans = 0;
				udd_ctrl_payload_nb_trans = 0;
				udd_ep_control_state = UDD_EPCTRL_DATA_OUT;
				/* Initialize buffer size and enable OUT bank */
				usb_device_endpoint_read_buffer_job(&usb_device,0,udd_ctrl_buffer,USB_DEVICE_EP_CTRL_SIZE);
			}
		}
	}
}

/**
 * \brief Control Endpoint Process when underflow condition has occurred
 * \param[in] pointer Pointer to the endpoint transfer status parameter struct from driver layer.
 */
static void udd_ctrl_underflow(void* pointer)
{
	struct usb_endpoint_callback_parameter *ep_callback_para = (struct usb_endpoint_callback_parameter*)pointer;

	if (UDD_EPCTRL_DATA_OUT == udd_ep_control_state) {
		/* Host want to stop OUT transaction
		 * then stop to wait OUT data phase and wait IN ZLP handshake */
		udd_ctrl_send_zlp_in();
	} else if (UDD_EPCTRL_HANDSHAKE_WAIT_OUT_ZLP == udd_ep_control_state) {
		/* A OUT handshake is waiting by device,
		 * but host want extra IN data then stall extra IN data */
		usb_device_endpoint_set_halt(&usb_device, ep_callback_para->endpoint_address);
	}
}

/**
 * \brief Control Endpoint Process when overflow condition has occurred
 * \param[in] pointer Pointer to the endpoint transfer status parameter struct from driver layer.
 */
static void udd_ctrl_overflow(void* pointer)
{
	struct usb_endpoint_callback_parameter *ep_callback_para = (struct usb_endpoint_callback_parameter*)pointer;

	if (UDD_EPCTRL_DATA_IN == udd_ep_control_state) {
		/* Host want to stop IN transaction
		 * then stop to wait IN data phase and wait OUT ZLP handshake */
		udd_ep_control_state = UDD_EPCTRL_HANDSHAKE_WAIT_OUT_ZLP;
	} else if (UDD_EPCTRL_HANDSHAKE_WAIT_IN_ZLP == udd_ep_control_state) {
		/* A IN handshake is waiting by device,
		 * but host want extra OUT data then stall extra OUT data and following status stage */
		usb_device_endpoint_set_halt(&usb_device, ep_callback_para->endpoint_address);
	}
}

/**
 * \internal
 * \brief Control endpoint transfer fail callback function
 * \param[in] module_inst Pointer to USB module instance
 * \param[in] pointer Pointer to the endpoint transfer status parameter struct from driver layer.
 */
static void _usb_ep0_on_tansfer_fail(struct usb_module *module_inst, void* pointer)
{
	struct usb_endpoint_callback_parameter *ep_callback_para = (struct usb_endpoint_callback_parameter*)pointer;

	if(ep_callback_para->endpoint_address & USB_EP_DIR_IN) {
		udd_ctrl_underflow(pointer);
	} else {
		udd_ctrl_overflow(pointer);
	}
}

/**
 * \internal
 * \brief Control endpoint transfer complete callback function
 * \param[in] module_inst Pointer to USB module instance
 * \param[in] pointer Pointer to the endpoint transfer status parameter struct from driver layer.
 */
static void _usb_ep0_on_tansfer_ok(struct usb_module *module_inst, void * pointer)
{
		if (UDD_EPCTRL_DATA_OUT  == udd_ep_control_state) { /* handshake Out for status stage */
			udd_ctrl_out_received(pointer);
		} else if (UDD_EPCTRL_DATA_IN == udd_ep_control_state) { /* handshake In for status stage */
			udd_ctrl_in_sent();
		} else {
			if (NULL != udd_g_ctrlreq.callback) {
				udd_g_ctrlreq.callback();
			}
			udd_ep_control_state = UDD_EPCTRL_SETUP;
		}
}

/**
 * \brief Enable Control Endpoint
 * \param[in] module_inst Pointer to USB module instance
 */
static void udd_ctrl_ep_enable(struct usb_module *module_inst)
{
	/* USB Device Endpoint0 Configuration */
	 struct usb_device_endpoint_config config_ep0;

	 usb_device_endpoint_get_config_defaults(&config_ep0);
	 config_ep0.ep_size = (enum usb_endpoint_size)(32 - clz(((uint32_t)Min(Max(USB_DEVICE_EP_CTRL_SIZE, 8), 1024) << 1) - 1) - 1 - 3);
	 usb_device_endpoint_set_config(module_inst,&config_ep0);

	 usb_device_endpoint_setup_buffer_job(module_inst,udd_ctrl_buffer);

	 usb_device_endpoint_register_callback(module_inst,0,USB_DEVICE_ENDPOINT_CALLBACK_RXSTP, _usb_ep0_on_setup );
	 usb_device_endpoint_register_callback(module_inst,0,USB_DEVICE_ENDPOINT_CALLBACK_TRCPT,_usb_ep0_on_tansfer_ok );
	 usb_device_endpoint_register_callback(module_inst,0,USB_DEVICE_ENDPOINT_CALLBACK_TRFAIL,_usb_ep0_on_tansfer_fail );
	 usb_device_endpoint_enable_callback(module_inst,0,USB_DEVICE_ENDPOINT_CALLBACK_RXSTP);
	 usb_device_endpoint_enable_callback(module_inst,0,USB_DEVICE_ENDPOINT_CALLBACK_TRCPT);
	 usb_device_endpoint_enable_callback(module_inst,0,USB_DEVICE_ENDPOINT_CALLBACK_TRFAIL);

#ifdef  USB_DEVICE_LPM_SUPPORT
	 // Enable LPM feature
	 usb_device_set_lpm_mode(module_inst, USB_DEVICE_LPM_ACK);
#endif

	 udd_ep_control_state = UDD_EPCTRL_SETUP;
}

/**
 * \internal
 * \brief Control endpoint Suspend callback function
 * \param[in] module_inst Pointer to USB module instance
 * \param[in] pointer Pointer to the callback parameter from driver layer.
 */
static void _usb_on_suspend(struct usb_module *module_inst, void *pointer)
{
	usb_device_disable_callback(&usb_device, USB_DEVICE_CALLBACK_SUSPEND);
	usb_device_enable_callback(&usb_device, USB_DEVICE_CALLBACK_WAKEUP);
	udd_sleep_mode(UDD_STATE_SUSPEND);
#ifdef UDC_SUSPEND_EVENT
	UDC_SUSPEND_EVENT();
#endif
}

#ifdef  USB_DEVICE_LPM_SUPPORT
static void _usb_device_lpm_suspend(struct usb_module *module_inst, void *pointer)
{
	dbg_print("LPM_SUSP\n");

	uint32_t *lpm_wakeup_enable;
	lpm_wakeup_enable = (uint32_t *)pointer;

	usb_device_disable_callback(&usb_device, USB_DEVICE_CALLBACK_LPMSUSP);
	usb_device_disable_callback(&usb_device, USB_DEVICE_CALLBACK_SUSPEND);
	usb_device_enable_callback(&usb_device, USB_DEVICE_CALLBACK_WAKEUP);

//#warning Here the sleep mode must be choose to have a DFLL startup time < bmAttribut.HIRD
	udd_sleep_mode(UDD_STATE_SUSPEND_LPM);  // Enter in LPM SUSPEND mode
	if ((*lpm_wakeup_enable)) {
		UDC_REMOTEWAKEUP_LPM_ENABLE();
	}
	if (!(*lpm_wakeup_enable)) {
		UDC_REMOTEWAKEUP_LPM_DISABLE();
	}
	UDC_SUSPEND_LPM_EVENT();
}
#endif

/**
 * \internal
 * \brief Control endpoint SOF callback function
 * \param[in] module_inst Pointer to USB module instance
 * \param[in] pointer Pointer to the callback parameter from driver layer.
 */
static void _usb_on_sof_notify(struct usb_module *module_inst, void *pointer)
{
	udc_sof_notify();
#ifdef UDC_SOF_EVENT
	UDC_SOF_EVENT();
#endif
}

/**
 * \internal
 * \brief Control endpoint Reset callback function
 * \param[in] module_inst Pointer to USB module instance
 * \param[in] pointer Pointer to the callback parameter from driver layer.
 */
static void _usb_on_bus_reset(struct usb_module *module_inst, void *pointer)
{
	// Reset USB Device Stack Core
	udc_reset();
	usb_device_set_address(module_inst,0);
	udd_ctrl_ep_enable(module_inst);
}

/**
 * \internal
 * \brief Control endpoint Wakeup callback function
 * \param[in] module_inst Pointer to USB module instance
 * \param[in] pointer Pointer to the callback parameter from driver layer.
 */
static void _usb_on_wakeup(struct usb_module *module_inst, void *pointer)
{
	udd_wait_clock_ready();

	usb_device_disable_callback(&usb_device, USB_DEVICE_CALLBACK_WAKEUP);
	usb_device_enable_callback(&usb_device, USB_DEVICE_CALLBACK_SUSPEND);
#ifdef  USB_DEVICE_LPM_SUPPORT
	usb_device_register_callback(&usb_device, USB_DEVICE_CALLBACK_LPMSUSP, _usb_device_lpm_suspend);
	usb_device_enable_callback(&usb_device, USB_DEVICE_CALLBACK_LPMSUSP);
#endif
	udd_sleep_mode(UDD_STATE_IDLE);
#ifdef UDC_RESUME_EVENT
	UDC_RESUME_EVENT();
#endif
}

void udd_detach(void)
{
	usb_device_detach(&usb_device);
	udd_sleep_mode(UDD_STATE_SUSPEND);
}

void udd_attach(void)
{
	udd_sleep_mode(UDD_STATE_IDLE);
	usb_device_attach(&usb_device);

	usb_device_register_callback(&usb_device, USB_DEVICE_CALLBACK_SUSPEND, _usb_on_suspend);
	usb_device_register_callback(&usb_device, USB_DEVICE_CALLBACK_SOF, _usb_on_sof_notify);
	usb_device_register_callback(&usb_device, USB_DEVICE_CALLBACK_RESET, _usb_on_bus_reset);
	usb_device_register_callback(&usb_device, USB_DEVICE_CALLBACK_WAKEUP, _usb_on_wakeup);

	usb_device_enable_callback(&usb_device, USB_DEVICE_CALLBACK_SUSPEND);
	usb_device_enable_callback(&usb_device, USB_DEVICE_CALLBACK_SOF);
	usb_device_enable_callback(&usb_device, USB_DEVICE_CALLBACK_RESET);
	usb_device_enable_callback(&usb_device, USB_DEVICE_CALLBACK_WAKEUP);
#ifdef  USB_DEVICE_LPM_SUPPORT
	usb_device_register_callback(&usb_device, USB_DEVICE_CALLBACK_LPMSUSP, _usb_device_lpm_suspend);
	usb_device_enable_callback(&usb_device, USB_DEVICE_CALLBACK_LPMSUSP);
#endif
}

#if USB_VBUS_EIC
/**
 * \name USB VBUS PAD management
 *
 * @{
 */

 /** Check if USB VBus is available */
# define is_usb_vbus_high()           port_pin_get_input_level(USB_VBUS_PIN)

/**
 * \internal
 * \brief USB VBUS pin change handler
 */
static void _uhd_vbus_handler(void)
{
	extint_chan_disable_callback(USB_VBUS_EIC_LINE,
			EXTINT_CALLBACK_TYPE_DETECT);
# ifndef USB_DEVICE_ATTACH_AUTO_DISABLE
	if (is_usb_vbus_high()) {
		udd_attach();
	} else {
		udd_detach();
	}
# endif
# ifdef UDC_VBUS_EVENT
	UDC_VBUS_EVENT(is_usb_vbus_high());
# endif
	extint_chan_enable_callback(USB_VBUS_EIC_LINE,
			EXTINT_CALLBACK_TYPE_DETECT);
}

/**
 * \internal
 * \brief USB VBUS pin configuration
 */
static void _usb_vbus_config(void)
{

	/* Initialize EIC for vbus checking */
	struct extint_chan_conf eint_chan_conf;
	extint_chan_get_config_defaults(&eint_chan_conf);

	eint_chan_conf.gpio_pin           = USB_VBUS_PIN;
	eint_chan_conf.gpio_pin_mux       = USB_VBUS_EIC_MUX;
	eint_chan_conf.gpio_pin_pull      = EXTINT_PULL_NONE;
	eint_chan_conf.detection_criteria = EXTINT_DETECT_BOTH;
	eint_chan_conf.filter_input_signal = true;

	extint_chan_disable_callback(USB_VBUS_EIC_LINE,
			EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_set_config(USB_VBUS_EIC_LINE, &eint_chan_conf);
	extint_register_callback(_uhd_vbus_handler,
			USB_VBUS_EIC_LINE,
			EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(USB_VBUS_EIC_LINE,
			EXTINT_CALLBACK_TYPE_DETECT);
}
/** @} */
#endif

bool udd_include_vbus_monitoring(void)
{
#if USB_VBUS_EIC
	return true;
#else
	return false;
#endif
}

void udd_enable(void)
{
	irqflags_t flags;

	/* To avoid USB interrupt before end of initialization */
	flags = cpu_irq_save();

#if USB_ID_EIC
	if (usb_dual_enable()) {
		/* The current mode has been started by otg_dual_enable() */
		cpu_irq_restore(flags);
		return;
	}
#endif
	struct usb_config config_usb;

	/* USB Module configuration */
	usb_get_config_defaults(&config_usb);
	config_usb.source_generator = UDD_CLOCK_GEN;
	usb_init(&usb_device, USB, &config_usb);

	/* USB Module Enable */
	usb_enable(&usb_device);

	/* Check clock after enable module, request the clock */
	udd_wait_clock_ready();

	udd_sleep_mode(UDD_STATE_SUSPEND);

#if USB_VBUS_EIC
	_usb_vbus_config();
	if (is_usb_vbus_high()) {
		/* USB Attach */
		_uhd_vbus_handler();
	}
#else
	// No VBus detect, assume always high
# ifndef USB_DEVICE_ATTACH_AUTO_DISABLE
	udd_attach();
# endif
#endif

	cpu_irq_restore(flags);
}

void udd_disable(void)
{
	irqflags_t flags;

	udd_detach();

	udd_sleep_mode(UDD_STATE_OFF);

	flags = cpu_irq_save();
	usb_dual_disable();
	cpu_irq_restore(flags);
}
/** @} */
