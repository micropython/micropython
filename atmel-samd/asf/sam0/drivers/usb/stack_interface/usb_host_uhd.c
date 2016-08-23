/**
 * \file
 *
 * \brief USB peripheral host wrapper for ASF Stack USB Host Driver (UHD)
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

/* Get USB host configuration */
#include "conf_usb_host.h"
#include "uhd.h"
#include "usb.h"
#include "usb_dual.h"

/* Optional UHC callbacks */
#ifndef UHC_MODE_CHANGE
# define UHC_MODE_CHANGE(arg)
#endif
#ifndef UHC_SOF_EVENT
# define UHC_SOF_EVENT()
#endif
#ifndef UHC_VBUS_CHANGE
# define UHC_VBUS_CHANGE(b_present)
#endif
#ifndef UHC_VBUS_ERROR
# define UHC_VBUS_ERROR()
#endif

/**
 * \ingroup usb_host_group
 * \defgroup usb_host_uhd_group USB Host Driver Implement (UHD)
 * USB low-level driver for USB Host mode
 * @{
 */

/* Function declare */
static void _uhd_ctrl_phase_setup(void);
static void _uhd_ctrl_phase_data_in_start(void);
static void _uhd_ctrl_phase_data_in(uint16_t nb_byte_received);
static void _uhd_ctrl_phase_zlp_in(void);
static void _uhd_ctrl_phase_data_out(void);
static void _uhd_ctrl_phase_zlp_out(void);
static void _uhd_ctrl_request_end(uhd_trans_status_t status);
static uint8_t _uhd_get_pipe(usb_add_t add, usb_ep_t endp);
static void _uhd_pipe_trans_complete(struct usb_module *module_inst, void *);
static void _uhd_ep_abort_pipe(uint8_t pipe, uhd_trans_status_t status);
static void _uhd_pipe_finish_job(uint8_t pipe, uhd_trans_status_t status);

/* for debug text */
#ifdef USB_DEBUG
#   define dbg_print printf
#else
#   define dbg_print(...)
#endif

/* Maximum size of a transfer in multipacket mode */
#define UHD_ENDPOINT_MAX_TRANS ((8 *1024 ) - 1)

/* pipe error status */
#define USB_STATUS_PIPE_DTGLER    (1 << 0)
#define USB_STATUS_PIPE_DAPIDER   (1 << 1)
#define USB_STATUS_PIPE_PIDER     (1 << 2)
#define USB_STATUS_PIPE_TOUTER    (1 << 3)
#define USB_STATUS_PIPE_CRC16ER   (1 << 4)

/* Check USB host configuration */
#ifdef USB_HOST_HS_SUPPORT
#  error The High speed mode is not supported on this part, please remove USB_HOST_HS_SUPPORT in conf_usb_host.h
#endif

#if (!(SAMD21) && !(SAMR21) && !(SAML21)) && !(SAMDA1) && !(SAMR30)
# error The current USB Host Driver supports only SAMD21/R21/L21/R30
#endif

#ifdef USB_HOST_LPM_SUPPORT
/** Notify that USB Host is enter in suspend LPM state */
static bool uhd_lpm_suspend = false;
#endif

/** Store the callback to be called at the end of reset signal */
static uhd_callback_reset_t uhd_reset_callback = NULL;

/**
 * \name Power management
 *
 * @{
 */
#ifndef UHD_NO_SLEEP_MGR

#include "sleepmgr.h"
/** States of USB interface */
enum uhd_usb_state_enum {
	UHD_STATE_OFF = 0,
	UHD_STATE_WAIT_ID_HOST = 1,
	UHD_STATE_NO_VBUS = 2,
	UHD_STATE_DISCONNECT = 3,
	UHD_STATE_SUSPEND = 4,
	UHD_STATE_SUSPEND_LPM = 5,
	UHD_STATE_IDLE = 6,
};

enum sleepmgr_mode sleep_mode[] = {
	SLEEPMGR_STANDBY,  // UHD_STATE_OFF (not used)
#if SAML21 || SAMR30
	SLEEPMGR_IDLE,   // UHD_STATE_WAIT_ID_HOST
	SLEEPMGR_IDLE,   // UHD_STATE_NO_VBUS
	SLEEPMGR_IDLE,   // UHD_STATE_DISCONNECT
	SLEEPMGR_IDLE,   // UHD_STATE_SUSPEND
	SLEEPMGR_IDLE,   // UHD_STATE_SUSPEND_LPM
	SLEEPMGR_IDLE,   // UHD_STATE_IDLE
#else
	SLEEPMGR_IDLE_0,   // UHD_STATE_WAIT_ID_HOST
	SLEEPMGR_IDLE_0,   // UHD_STATE_NO_VBUS
	SLEEPMGR_IDLE_0,   // UHD_STATE_DISCONNECT
	SLEEPMGR_IDLE_2,   // UHD_STATE_SUSPEND
	SLEEPMGR_IDLE_2,   // UHD_STATE_SUSPEND_LPM
	SLEEPMGR_IDLE_0,   // UHD_STATE_IDLE
#endif	
};

static enum uhd_usb_state_enum uhd_state = UHD_STATE_OFF;

/** \brief Manages the sleep mode following the USB state
 *
 * \param new_state  New USB state
 */
static void uhd_sleep_mode(enum uhd_usb_state_enum new_state)
{
	if (uhd_state == new_state) {
		return; // No change
	}
	if (new_state != UHD_STATE_OFF) {
		/* Lock new limit */
		sleepmgr_lock_mode(sleep_mode[new_state]);
	}
	if (uhd_state != UHD_STATE_OFF) {
		/* Unlock old limit */
		sleepmgr_unlock_mode(sleep_mode[uhd_state]);
	}
	uhd_state = new_state;
}

#else
#  define uhd_sleep_mode(arg)
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
 * \brief Buffer to store the sent/received data on control endpoint
 *
 * Used to avoid a RAM buffer overflow when the payload buffer
 * is smaller than control endpoint size.
 * Also used when payload buffer is not word aligned.
 */
UHC_BSS(4)
uint8_t uhd_ctrl_buffer[64];

/**
 * \brief Structure to store the high level setup request
 */

struct uhd_ctrl_request_t{
	/** Buffer to store the setup DATA phase */
	uint8_t *payload;
	/** Callback called when buffer is empty or full */
	uhd_callback_setup_run_t callback_run;
	/** Callback called when request is completed */
	uhd_callback_setup_end_t callback_end;
	/** Next setup request to process */
	struct uhd_ctrl_request_t *next_request;
	/** Setup request definition */
	usb_setup_req_t req;
	/** Size of buffer used in DATA phase */
	uint16_t payload_size;
	/** USB address of control endpoint */
	usb_add_t add;
};

/** Entry points of setup request list */
struct uhd_ctrl_request_t *uhd_ctrl_request_first;
struct uhd_ctrl_request_t *uhd_ctrl_request_last;

/** Remaining time for on-going setup request (No request on-going if equal 0) */
volatile uint16_t uhd_ctrl_request_timeout;

/** Number of transfered byte on DATA phase of current setup request */
uint16_t uhd_ctrl_nb_trans;

/** Flag to delay a suspend request after all on-going setup request */
static bool uhd_b_suspend_requested;

/** Bit definitions to store setup request state machine */
typedef enum {
	/** Wait a SETUP packet */
	UHD_CTRL_REQ_PHASE_SETUP = 0,
	/** Wait a OUT data packet */
	UHD_CTRL_REQ_PHASE_DATA_OUT = 1,
	/** Wait a IN data packet */
	UHD_CTRL_REQ_PHASE_DATA_IN = 2,
	/** Wait a IN ZLP packet */
	UHD_CTRL_REQ_PHASE_ZLP_IN = 3,
	/** Wait a OUT ZLP packet */
	UHD_CTRL_REQ_PHASE_ZLP_OUT = 4,
} uhd_ctrl_request_phase_t;
uhd_ctrl_request_phase_t uhd_ctrl_request_phase;

/** @} */

/**
 * \name Management of bulk/interrupt/isochronous endpoints
 *
 * The UHD manages the data transfer on endpoints:
 * - Start data transfer on endpoint with USB DMA
 * - Send a ZLP packet if requested
 * - Call registered callback to signal end of transfer
 * The transfer abort and stall feature are supported.
 *
 * @{
 */

/** Structure definition to store registered jobs on a pipe */
typedef struct {
	/** Buffer located in internal RAM to send or fill during job */
	uint8_t *buf;
	/** Internal buffer allocated in internal RAM to receive data in case of small user buffer */
	uint8_t *buf_internal;
	/** Size of buffer to send or fill */
	iram_size_t buf_size;
	/** Total number of transferred data on endpoint */
	iram_size_t nb_trans;
	/** Callback to call at the end of transfer */
	uhd_callback_trans_t call_end;

	/** timeout on this request (ms) */
	uint16_t timeout;

	/** A job is registered on this pipe */
	uint8_t busy:1;
	/** A short packet is requested for this job on endpoint IN */
	uint8_t b_shortpacket:1;
} uhd_pipe_job_t;

/** Array to register a job on bulk/interrupt/isochronous endpoint */
static uhd_pipe_job_t uhd_pipe_job[USB_PIPE_NUM- 1];

/** Variables to manage the suspend/resume sequence */
static uint8_t uhd_suspend_start;
static uint8_t uhd_resume_start;
static uint8_t uhd_pipes_unfreeze;

/** @} */


struct usb_module dev;

/**
 * \internal
 * \brief Sends a USB setup packet to start a control request sequence
 */
static void _uhd_ctrl_phase_setup(void)
{
	usb_setup_req_t setup_req;
	struct usb_host_pipe_config cfg;
	usb_host_pipe_get_config(&dev, 0, &cfg);

	uhd_ctrl_request_phase = UHD_CTRL_REQ_PHASE_SETUP;
	memcpy( &setup_req, &uhd_ctrl_request_first->req, sizeof(usb_setup_req_t));
	/* Manage LSB/MSB to fit with CPU usage */
	setup_req.wValue = cpu_to_le16(setup_req.wValue);
	setup_req.wIndex = cpu_to_le16(setup_req.wIndex);
	setup_req.wLength = cpu_to_le16(setup_req.wLength);
	uhd_ctrl_nb_trans = 0;

  	/* Check pipe */
#ifdef USB_HOST_HUB_SUPPORT
	if (cfg.pipe_type == USB_HOST_PIPE_TYPE_DISABLE) {
		_uhd_ctrl_request_end(UHD_TRANS_DISCONNECT);
		return; // Endpoint not valid
	}
#error TODO check address in list
	/* Reconfigure USB address of pipe 0 used for all control endpoints */
	uhd_udesc_set_uhaddr(0, uhd_ctrl_request_first->add);
#else
	if ((cfg.pipe_type == USB_HOST_PIPE_TYPE_DISABLE) ||
			(uhd_ctrl_request_first->add != cfg.device_address)) {
		_uhd_ctrl_request_end(UHD_TRANS_DISCONNECT);
		return; // Endpoint not valid
	}
#endif

	/* Fill pipe */
	memcpy(uhd_ctrl_buffer, &setup_req, sizeof(setup_req));
	uhd_ctrl_request_timeout = 5000;

	/* Start transfer */
	usb_host_pipe_setup_job(&dev, 0, uhd_ctrl_buffer);
	usb_host_pipe_enable_callback(&dev, 0, USB_HOST_PIPE_CALLBACK_SETUP);
}

/**
 * \internal
 * \brief Starts the DATA IN phase on control endpoint
 */
static void _uhd_ctrl_phase_data_in_start(void)
{
	struct usb_host_pipe_config cfg;
	usb_host_pipe_get_config(&dev, 0, &cfg);

	uhd_ctrl_request_phase = UHD_CTRL_REQ_PHASE_DATA_IN;

	usb_host_pipe_read_job(&dev, 0, uhd_ctrl_buffer, cfg.size);
}

/**
 * \internal
 * \brief Manages the DATA IN phase on control endpoint
 *
 * \param nb_byte_received Number of bytes received
 */
static void _uhd_ctrl_phase_data_in(uint16_t nb_byte_received)
{
	bool b_short_packet;
	uint8_t *ptr_ep_data;

	struct usb_host_pipe_config cfg;
	usb_host_pipe_get_config(&dev, 0, &cfg);

	/** In HUB mode, the control pipe is always configured to 64B */
	/** thus the short packet flag must be computed */
	b_short_packet = (nb_byte_received != cfg.size);

	ptr_ep_data = uhd_ctrl_buffer;
uhd_ctrl_receiv_in_read_data:
	/* Copy data from pipe to payload buffer */
	while (uhd_ctrl_request_first->payload_size && nb_byte_received) {
		*uhd_ctrl_request_first->payload++ = *ptr_ep_data++;
		uhd_ctrl_nb_trans++;
		uhd_ctrl_request_first->payload_size--;
		nb_byte_received--;
	}

	if (!uhd_ctrl_request_first->payload_size && nb_byte_received) {
		/* payload buffer is full to store data remaining */
		if (uhd_ctrl_request_first->callback_run == NULL
				|| !uhd_ctrl_request_first->callback_run(
				cfg.device_address,
				&uhd_ctrl_request_first->payload,
				&uhd_ctrl_request_first->payload_size)) {
			/* DATA phase aborted by host */
			goto uhd_ctrl_phase_data_in_end;
		}
		/*
		 * The payload buffer has been updated by the callback
		 *  thus the data load can restart.
		 */
		goto uhd_ctrl_receiv_in_read_data;
	}

	/* Test short packet */
	if ((uhd_ctrl_nb_trans == uhd_ctrl_request_first->req.wLength)
			|| b_short_packet) {
		/* End of DATA phase or DATA phase abort from device */
uhd_ctrl_phase_data_in_end:
		_uhd_ctrl_phase_zlp_out();
		return;
	}

	usb_host_pipe_read_job(&dev, 0, uhd_ctrl_buffer, cfg.size);
}

/**
 * \internal
 * \brief Starts the ZLP IN phase on control endpoint
 */
static void _uhd_ctrl_phase_zlp_in(void)
{
	uhd_ctrl_request_phase = UHD_CTRL_REQ_PHASE_ZLP_IN;

	usb_host_pipe_set_toggle(&dev, 0);

	usb_host_pipe_read_job(&dev, 0, uhd_ctrl_buffer, 0);
}

/**
 * \internal
 * \brief Manages the DATA OUT phase on control endpoint
 */
static void _uhd_ctrl_phase_data_out(void)
{
	struct usb_host_pipe_config cfg;
	usb_host_pipe_get_config(&dev, 0, &cfg);

	uhd_ctrl_request_phase = UHD_CTRL_REQ_PHASE_DATA_OUT;
	uint16_t nb_trans;

	if (uhd_ctrl_nb_trans == uhd_ctrl_request_first->req.wLength) {
		/* End of DATA phase */
		_uhd_ctrl_phase_zlp_in();
		return;
	}

	if (!uhd_ctrl_request_first->payload_size) {
		/* Buffer empty, then request a new buffer */
		if (uhd_ctrl_request_first->callback_run==NULL
				|| !uhd_ctrl_request_first->callback_run(
				cfg.device_address,
				&uhd_ctrl_request_first->payload,
				&uhd_ctrl_request_first->payload_size)) {
			/* DATA phase aborted by host */
			_uhd_ctrl_phase_zlp_in();
			return;
		}
	}

	nb_trans = uhd_ctrl_request_first->payload_size;
	if (nb_trans > cfg.size) {
		nb_trans = cfg.size;
	}

	/* Link the user buffer directly on USB hardware DMA */
	memcpy(uhd_ctrl_buffer, uhd_ctrl_request_first->payload, nb_trans);

	/* Update counters */
	uhd_ctrl_request_first->payload += nb_trans;
	uhd_ctrl_nb_trans += nb_trans;
	uhd_ctrl_request_first->payload_size -= nb_trans;

	/* Start transfer */
	usb_host_pipe_write_job(&dev, 0, uhd_ctrl_buffer, nb_trans);
}

/**
 * \internal
 * \brief Starts the ZLP OUT phase on control endpoint
 */
static void _uhd_ctrl_phase_zlp_out(void)
{
	uhd_ctrl_request_phase = UHD_CTRL_REQ_PHASE_ZLP_OUT;

	usb_host_pipe_set_toggle(&dev, 0);

	/* No need to link a user buffer directly on USB hardware DMA */
	/* Start transfer */
	usb_host_pipe_write_job(&dev, 0, uhd_ctrl_buffer, 0);
}

/**
 * \internal
 * \brief Call the callback linked to control request and start the next request
 *
 * \param status Request finish status passed to callback
 */
static void _uhd_ctrl_request_end(uhd_trans_status_t status)
{
	irqflags_t flags;
	uhd_callback_setup_end_t callback_end;
	struct uhd_ctrl_request_t *request_to_free;
	bool b_new_request;

	struct usb_host_pipe_config cfg;
	usb_host_pipe_get_config(&dev, 0, &cfg);

	Assert (uhd_ctrl_request_first != NULL);

	uhd_ctrl_request_timeout = 0;

	/* Remove request from the control request list */
	callback_end = uhd_ctrl_request_first->callback_end;
	request_to_free = uhd_ctrl_request_first;
	flags = cpu_irq_save();
	uhd_ctrl_request_first = uhd_ctrl_request_first->next_request;
	b_new_request = (uhd_ctrl_request_first != NULL);
	cpu_irq_restore(flags);
	free(request_to_free);

	/* Call callback */
	if (callback_end != NULL) {
		callback_end(cfg.device_address, status, uhd_ctrl_nb_trans);
	}

	/* If a setup request is pending and no started by previous callback */
	if (b_new_request) {
		_uhd_ctrl_phase_setup();
	}
	if (uhd_b_suspend_requested) {
		/* A suspend request has been delay after all setup request */
		uhd_b_suspend_requested = false;
		uhd_suspend();
	}
}

/**
 * \internal
 * \brief Manages timeouts and actions based on SOF events
 * - Suspend delay
 * - Resume delay
 * - Setup packet delay
 * - Timeout on endpoint control transfer
 * - Timeouts on bulk/interrupt/isochronous endpoint transfers
 * - UHC user notification
 * - SOF user notification
 *
 * \param module_inst Pointer to USB module instance
 */
static void _uhd_sof_interrupt(struct usb_module *module_inst)
{
	/* Manage a delay to enter in suspend */
	if (uhd_suspend_start) {
		if (--uhd_suspend_start == 0) {
			/* In case of high CPU frequency,
			 *  the current Keep-Alive/SOF can be always on-going
			 *  then wait end of SOF generation
			 *  to be sure that disable SOF has been accepted
			 */
			dbg_print("SUSP\n");
			usb_host_disable_sof(&dev);
			/* Enable wakeup/resumes interrupts */
			usb_host_enable_callback(&dev, USB_HOST_CALLBACK_WAKEUP);
			usb_host_enable_callback(&dev, USB_HOST_CALLBACK_DNRSM);
			usb_host_enable_callback(&dev, USB_HOST_CALLBACK_UPRSM);

			/* Check that the hardware state machine has left the IDLE/Active mode
			 * before freeze USB clock
			 */
			while (2==usb_get_state_machine_status(&dev));
			uhd_sleep_mode(UHD_STATE_SUSPEND);
		}
		return; // Abort SOF events
	}
	/* Manage a delay to exit of suspend */
	if (uhd_resume_start) {
		if (--uhd_resume_start == 0) {
			/* Restore pipes unfrozen */
			for (uint8_t pipe = 1; pipe < USB_PIPE_NUM; pipe++) {
				if ((uhd_pipes_unfreeze >> pipe) & 0x01) {
					usb_host_pipe_unfreeze(&dev, pipe);
				}
			}
			uhc_notify_resume();
		}
		return; // Abort SOF events
	}
	/* Manage the timeout on endpoint control transfer */
	if (uhd_ctrl_request_timeout) {
		/* Setup request on-going */
		if (--uhd_ctrl_request_timeout == 0) {
			/* Stop request */
			usb_host_pipe_freeze(&dev, 0);
			_uhd_ctrl_request_end(UHD_TRANS_TIMEOUT);
		}
	}
	/* Manage the timeouts on endpoint transfer */
	uhd_pipe_job_t *ptr_job;
	for (uint8_t pipe = 1; pipe < USB_PIPE_NUM; pipe++) {
		ptr_job = &uhd_pipe_job[pipe - 1];
		if (ptr_job->busy == true) {
			if (ptr_job->timeout) {
				/* Timeout enabled on this job */
				if (--ptr_job->timeout == 0) {
					/* Abort job */
					_uhd_ep_abort_pipe(pipe,UHD_TRANS_TIMEOUT);
				}
			}
		}
	}

	/* Notify the UHC */
	uhc_notify_sof(false);

	/* Notify the user application */
	UHC_SOF_EVENT();
}

/**
 * \internal
 * \brief Manages bus reset interrupt
 *
 * \param module_inst Pointer to USB module instance
 */
static void _uhd_reset(struct usb_module *module_inst)
{
	if (uhd_reset_callback != NULL) {
		uhd_reset_callback();
	}
}

/**
 * \internal
 * \brief Manages wakeup interrupt
 *
 * \param module_inst Pointer to USB module instance
 */
static void _uhd_wakeup(struct usb_module *module_inst)
{
	/* Here the wakeup interrupt has been used to detect:
	 * - connection with an asynchronous interrupt
	 * - down/upstream resume with an asynchronous interrupt
	 */
	dbg_print("WAKEUP\n");
#ifdef USB_HOST_LPM_SUPPORT
	if (uhd_lpm_suspend) {
		usb_host_send_l1_resume(&dev);
		uhd_lpm_suspend = false;
		uhc_notify_resume_lpm();
	} else
#endif
	{
		usb_host_send_resume(&dev);
		/* Wait 50ms before restarting transfer */
		uhd_resume_start = 50;
	}
	/* Disable wakeup/resumes interrupts */
	usb_host_disable_callback(&dev, USB_HOST_CALLBACK_WAKEUP);
	usb_host_disable_callback(&dev, USB_HOST_CALLBACK_DNRSM);
	usb_host_disable_callback(&dev, USB_HOST_CALLBACK_UPRSM);
	uhd_sleep_mode(UHD_STATE_IDLE);
}

/**
 * \internal
 * \brief Manages downstream resume interrupt
 *
 * \param module_inst Pointer to USB module instance
 */
static void _uhd_downstream_resume(struct usb_module *module_inst)
{
	dbg_print("DOWN RES\n");
	/* Disable wakeup/resumes interrupts */
	usb_host_disable_callback(&dev, USB_HOST_CALLBACK_WAKEUP);
	usb_host_disable_callback(&dev, USB_HOST_CALLBACK_DNRSM);
	usb_host_disable_callback(&dev, USB_HOST_CALLBACK_UPRSM);
#ifdef USB_HOST_LPM_SUPPORT
	if (uhd_lpm_suspend) {
		uhd_lpm_suspend = false;
		uhc_notify_resume_lpm();
	} else
#endif
	{
		/* Wait 50ms before restarting transfer */
		uhd_resume_start = 50;
	}
	uhd_sleep_mode(UHD_STATE_IDLE);
}

/**
 * \internal
 * \brief Manages upstream resume interrupt
 *
 * \param module_inst Pointer to USB module instance
 */
static void _uhd_upstream_resume(struct usb_module *module_inst)
{
	dbg_print("UP RES\n");
#ifdef USB_HOST_LPM_SUPPORT
	if (uhd_lpm_suspend) {
		usb_host_send_l1_resume(&dev);
		uhd_lpm_suspend = false;
		uhc_notify_resume_lpm();
	} else
#endif
	{
		usb_host_send_resume(&dev);
		/* Wait 50ms before restarting transfer */
		uhd_resume_start = 50;
	}
	/* Disable wakeup/resumes interrupts */
	usb_host_disable_callback(&dev, USB_HOST_CALLBACK_WAKEUP);
	usb_host_disable_callback(&dev, USB_HOST_CALLBACK_DNRSM);
	usb_host_disable_callback(&dev, USB_HOST_CALLBACK_UPRSM);
	uhd_sleep_mode(UHD_STATE_IDLE);
}

/**
 * \internal
 * \brief Manages ram access error interrupt
 *
 * \param module_inst Pointer to USB module instance
 */
static void _uhd_ram_error(struct usb_module *module_inst)
{
#ifdef UHC_RAM_ACCESS_ERR_EVENT
	UHC_RAM_ACCESS_ERR_EVENT();
#endif	
	dbg_print("!!!! RAM ERR !!!!\n");
}

/**
 * \internal
 * \brief Manages connection interrupt
 *
 * \param module_inst Pointer to USB module instance
 */
static void _uhd_connect(struct usb_module *module_inst)
{
	usb_host_disable_callback(&dev, USB_HOST_CALLBACK_CONNECT);
	dbg_print("CONN\n");
	usb_host_enable_callback(&dev, USB_HOST_CALLBACK_DISCONNECT);
	usb_host_disable_callback(&dev, USB_HOST_CALLBACK_WAKEUP);
	usb_host_enable_sof(&dev);
	uhd_sleep_mode(UHD_STATE_IDLE);
	uhd_suspend_start = 0;
	uhd_resume_start = 0;
	uhc_notify_connection(true);
}

/**
 * \internal
 * \brief Manages disconnection interrupt
 *
 * \param module_inst Pointer to USB module instance
 */
static void _uhd_disconnect(struct usb_module *module_inst)
{
	/* This should be the normal way to handle this event. */
	usb_host_disable_callback(&dev, USB_HOST_CALLBACK_DISCONNECT);
	dbg_print("DISC\n");
	/* Disable wakeup/resumes interrupts,
	 * in case of disconnection during suspend mode
	 */
	usb_host_disable_callback(&dev, USB_HOST_CALLBACK_WAKEUP);
	/* Enable asynchronous wakeup interrupt to allow a CPU wakeup
	 * when a connection occurs.
	 */
	usb_host_enable_callback(&dev, USB_HOST_CALLBACK_CONNECT);
	usb_host_enable_callback(&dev, USB_HOST_CALLBACK_WAKEUP);
	uhd_suspend_start = 0;
	uhd_resume_start = 0;
	uhd_sleep_mode(UHD_STATE_DISCONNECT);
	uhc_notify_connection(false);
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
 * USB VBUS pin change handler
 */
static void _uhd_vbus_handler(void)
{
	extint_chan_disable_callback(USB_VBUS_EIC_LINE,
			EXTINT_CALLBACK_TYPE_DETECT);
	if (is_usb_vbus_high()) {
		UHC_VBUS_CHANGE(true);
	}
	if (!is_usb_vbus_high()) {
		uhd_sleep_mode(UHD_STATE_NO_VBUS);
		UHC_VBUS_CHANGE(false);
	}
	extint_chan_enable_callback(USB_VBUS_EIC_LINE,
			EXTINT_CALLBACK_TYPE_DETECT);
}

/**
 * USB VBUS pin config
 */
static void _usb_vbus_config(void)
{
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

	/* Set USB VBUS Pin as inputs */
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_UP;
	port_pin_set_config(USB_VBUS_PIN, &pin_conf);

	/* Initialize EIC for vbus checking */
	struct extint_chan_conf eint_chan_conf;
	extint_chan_get_config_defaults(&eint_chan_conf);

	eint_chan_conf.gpio_pin           = USB_VBUS_PIN;
	eint_chan_conf.gpio_pin_mux       = USB_VBUS_EIC_MUX;
	eint_chan_conf.detection_criteria = EXTINT_DETECT_LOW;
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

void uhd_enable(void)
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
	uhd_ctrl_request_first = NULL;
	uhd_ctrl_request_last = NULL;
	uhd_ctrl_request_timeout = 0;
	uhd_suspend_start = 0;
	uhd_resume_start = 0;
	uhd_b_suspend_requested = false;

	struct usb_config cfg;
	usb_get_config_defaults(&cfg);
	cfg.select_host_mode = 1;
	usb_init(&dev,USB, &cfg);
	usb_enable(&dev);

	uhd_sleep_mode(UHD_STATE_NO_VBUS);

#if USB_VBUS_EIC
	_usb_vbus_config();
	if (is_usb_vbus_high()) {
		/* Force Vbus interrupt when Vbus is always high */
		_uhd_vbus_handler();
		usb_host_enable(&dev);
		uhd_sleep_mode(UHD_STATE_DISCONNECT);
	} else {
		dbg_print("VBUS low, there is some power issue on board!!! \n");
	}
#else
	usb_host_enable(&dev);
#endif

	usb_host_register_callback(&dev, USB_HOST_CALLBACK_SOF, _uhd_sof_interrupt);
	usb_host_register_callback(&dev, USB_HOST_CALLBACK_RESET, _uhd_reset);
	usb_host_register_callback(&dev, USB_HOST_CALLBACK_WAKEUP, _uhd_wakeup);
	usb_host_register_callback(&dev, USB_HOST_CALLBACK_DNRSM, _uhd_downstream_resume);
	usb_host_register_callback(&dev, USB_HOST_CALLBACK_UPRSM, _uhd_upstream_resume);
	usb_host_register_callback(&dev, USB_HOST_CALLBACK_RAMACER, _uhd_ram_error);
	usb_host_register_callback(&dev, USB_HOST_CALLBACK_CONNECT, _uhd_connect);
	usb_host_register_callback(&dev, USB_HOST_CALLBACK_DISCONNECT, _uhd_disconnect);

	/* Enable main control interrupt */
	/* Connection, SOF and reset */
	usb_host_enable_callback(&dev, USB_HOST_CALLBACK_SOF);
	usb_host_enable_callback(&dev, USB_HOST_CALLBACK_RESET);
	usb_host_enable_callback(&dev, USB_HOST_CALLBACK_RAMACER);
	usb_host_enable_callback(&dev, USB_HOST_CALLBACK_CONNECT);
	usb_host_enable_callback(&dev, USB_HOST_CALLBACK_DISCONNECT);

	cpu_irq_restore(flags);
}

void uhd_disable(bool b_id_stop)
{
	irqflags_t flags;
	UNUSED(b_id_stop);

	/* Disable Vbus change interrupts */
#if USB_VBUS_EIC
	extint_chan_disable_callback(USB_VBUS_EIC_LINE,
			EXTINT_CALLBACK_TYPE_DETECT);
#endif

 	/* Disable main control interrupts */
	usb_host_disable_callback(&dev, USB_HOST_CALLBACK_SOF);
	usb_host_disable_callback(&dev, USB_HOST_CALLBACK_RESET);
	usb_host_disable_callback(&dev, USB_HOST_CALLBACK_RAMACER);
	usb_host_disable_callback(&dev, USB_HOST_CALLBACK_CONNECT);
	usb_host_disable_callback(&dev, USB_HOST_CALLBACK_DISCONNECT);
	usb_host_disable_sof(&dev);
	uhc_notify_connection(false);

#if USB_ID_EIC
	uhd_sleep_mode(UHD_STATE_WAIT_ID_HOST);
	if (!b_id_stop) {
		return; // No need to disable host, it is done automatically by hardware
	}
#endif

	flags = cpu_irq_save();
	usb_dual_disable();
	cpu_irq_restore(flags);
}

uhd_speed_t uhd_get_speed(void)
{
	switch (usb_host_get_speed(&dev)) {
	case USB_SPEED_LOW:
		return UHD_SPEED_LOW;
	case USB_SPEED_FULL:
		return UHD_SPEED_FULL;
	default:
		Assert(false);
		return UHD_SPEED_LOW;
	}
}

uint16_t uhd_get_frame_number(void)
{
	return usb_host_get_frame_number(&dev);
}

uint16_t uhd_get_microframe_number(void)
{
	/* nothing to do */
	return 0;
}

void uhd_send_reset(uhd_callback_reset_t callback)
{
	uhd_reset_callback = callback;
	usb_host_send_reset(&dev);
}

void uhd_suspend(void)
{
	if (uhd_ctrl_request_timeout) {
		/* Delay suspend after setup requests */
		uhd_b_suspend_requested = true;
		return;
	}
	/* Save pipe freeze states and freeze pipes */
	uhd_pipes_unfreeze = 0;
	for (uint8_t pipe = 1; pipe < USB_PIPE_NUM; pipe++) {
		uhd_pipes_unfreeze |= (!usb_host_pipe_is_frozen(&dev, pipe)) << pipe;
		usb_host_pipe_freeze(&dev, pipe);
	}
	/* Wait three SOFs before entering in suspend state */
	uhd_suspend_start = 3;
}

bool uhd_is_suspend(void)
{
	return !usb_host_is_sof_enabled(&dev);
}

void uhd_resume(void)
{
	if (usb_host_is_sof_enabled(&dev)) {
		/* Currently in IDLE mode (!=Suspend) */
		if (uhd_suspend_start) {
			/* Suspend mode on going
			 * then stop it and start resume event
			 */
			uhd_suspend_start = 0;
			uhd_resume_start = 1;
		}
		return;
	}

#ifdef USB_HOST_LPM_SUPPORT
	struct usb_host_pipe_config cfg;
	usb_host_pipe_get_config(&dev, 0, &cfg);

	if (cfg.pipe_type == USB_HOST_PIPE_TYPE_EXTENDED) {
		/* LPM Suspend command is on going, then ignore resume */
		return;
	}

	if (uhd_lpm_suspend) {
		dbg_print("SEND_RESUME_L1\n");
		usb_host_send_l1_resume(&dev);
	} else
#endif
	{
		dbg_print("SEND_RESUME\n");
		usb_host_send_resume(&dev);
	}
	uhd_sleep_mode(UHD_STATE_IDLE);
}

#ifdef USB_HOST_LPM_SUPPORT
bool uhd_suspend_lpm(bool b_remotewakeup, uint8_t hird)
{
	if (uhd_ctrl_request_timeout) {
		return false;
	}
	/* Do not freeze pipe to restart its immediatly after resume */
	dbg_print("EXT_LPM\n");

	/* Set the LPM job */
	usb_host_pipe_lpm_job(&dev, 0, b_remotewakeup, hird);	

	/* Wait LPM ACK through interrupt */
	return true;
}
#endif

bool uhd_setup_request(
		usb_add_t add,
		usb_setup_req_t *req,
		uint8_t *payload,
		uint16_t payload_size,
		uhd_callback_setup_run_t callback_run,
		uhd_callback_setup_end_t callback_end)
{
	irqflags_t flags;
	struct uhd_ctrl_request_t *request;
	bool b_start_request = false;

	request = malloc(sizeof(struct uhd_ctrl_request_t));
	if (request == NULL) {
		Assert(false);
		return false;
	}

	/* Fill structure */
	request->add = (uint8_t) add;
	memcpy(&request->req, req, sizeof(usb_setup_req_t));
	request->payload = payload;
	request->payload_size = payload_size;
	request->callback_run = callback_run;
	request->callback_end = callback_end;
	request->next_request = NULL;

	/* Add this request in the queue */
	flags = cpu_irq_save();
	if (uhd_ctrl_request_first == NULL) {
		uhd_ctrl_request_first = request;
		b_start_request = true;
	} else {
		uhd_ctrl_request_last->next_request = request;
	}
	uhd_ctrl_request_last = request;
	cpu_irq_restore(flags);

	if (b_start_request) {
		/* Start immediately request */
		_uhd_ctrl_phase_setup();
	}
	return true;
}

/**
 * \internal
 * \brief Manages endpoint 0 transfer complete interrupt
 *
 * \param module_inst Pointer to USB module instance
 * \param pointer     Pointer to USB pipe transfer callback status parameters
 */
static void _uhd_ep0_transfer_complete(struct usb_module *module_inst, void *pointer)
{
	struct usb_pipe_callback_parameter *p_callback_para;
	p_callback_para = (struct usb_pipe_callback_parameter *)pointer;

	struct usb_host_pipe_config cfg;
	usb_host_pipe_get_config(&dev, 0, &cfg);

#ifdef USB_HOST_LPM_SUPPORT
	if (cfg.pipe_type == USB_HOST_PIPE_TYPE_EXTENDED) {
		dbg_print("EXT_LPM_ACK\n");
		cfg.pipe_type = USB_HOST_PIPE_TYPE_CONTROL;
		usb_host_pipe_set_config(&dev, 0, &cfg);
		/* Enable wakeup/resumes interrupts */
		usb_host_enable_callback(&dev, USB_HOST_CALLBACK_WAKEUP);
		usb_host_enable_callback(&dev, USB_HOST_CALLBACK_DNRSM);
		usb_host_enable_callback(&dev, USB_HOST_CALLBACK_UPRSM);
		uhd_lpm_suspend = true;
		uhd_sleep_mode(UHD_STATE_SUSPEND_LPM);
		return;
	}
#endif

	usb_host_pipe_freeze(&dev, 0);
	switch(uhd_ctrl_request_phase) {
	case UHD_CTRL_REQ_PHASE_DATA_IN:
		_uhd_ctrl_phase_data_in(p_callback_para->transfered_size);
		break;
	case UHD_CTRL_REQ_PHASE_ZLP_IN:
		_uhd_ctrl_request_end(UHD_TRANS_NOERROR);
		break;
	case UHD_CTRL_REQ_PHASE_DATA_OUT:
		_uhd_ctrl_phase_data_out();
		break;
	case UHD_CTRL_REQ_PHASE_ZLP_OUT:
		_uhd_ctrl_request_end(UHD_TRANS_NOERROR);
		break;
	default:
		Assert(false);
		break;
	}
}

/**
 * \internal
 * \brief Manages endpoint 0 error interrupt
 *
 * \param module_inst Pointer to USB module instance
 * \param pointer     Pointer to USB pipe transfer callback status parameters
 */
static void _uhd_ep0_error(struct usb_module *module_inst, void *pointer)
{
#ifdef USB_HOST_LPM_SUPPORT
	struct usb_host_pipe_config cfg;
	usb_host_pipe_get_config(&dev, 0, &cfg);

	if (cfg.pipe_type == USB_HOST_PIPE_TYPE_EXTENDED) {
		dbg_print("EXT_LPM_ERROR\n");
		cfg.pipe_type = USB_HOST_PIPE_TYPE_CONTROL;
		usb_host_pipe_set_config(&dev, 0, &cfg);
		return;
	}
#endif

	dbg_print("CTRL Error\n");
	struct usb_pipe_callback_parameter *p_callback_para;
	p_callback_para = (struct usb_pipe_callback_parameter *)pointer;

	uhd_trans_status_t uhd_error;

	/* Get and ack error */
	switch(p_callback_para->pipe_error_status) {
	case USB_STATUS_PIPE_DTGLER:
		uhd_error = UHD_TRANS_DT_MISMATCH;
		break;
	case USB_STATUS_PIPE_TOUTER:
		uhd_error = UHD_TRANS_NOTRESPONDING;
		break;
	case USB_STATUS_PIPE_CRC16ER:
		uhd_error = UHD_TRANS_CRC;
		break;
	case USB_STATUS_PIPE_DAPIDER:
	case USB_STATUS_PIPE_PIDER:
		uhd_error = UHD_TRANS_PIDFAILURE;
		break;
	default:
		uhd_error = UHD_TRANS_TIMEOUT;
		break;
	}

	/* Get and ack error */
	_uhd_ctrl_request_end(uhd_error);
}

/**
 * \internal
 * \brief Manages endpoint 0 setup interrupt
 *
 * \param module_inst Pointer to USB module instance
 * \param null        Not used
 */
static void _uhd_ep0_setup(struct usb_module *module_inst, void *null)
{
	/* SETUP packet sent */
	usb_host_pipe_freeze(&dev, 0);
	dbg_print("CTRL Setup\n");
	Assert(uhd_ctrl_request_phase == UHD_CTRL_REQ_PHASE_SETUP);

	/* Start DATA phase */
	if ((uhd_ctrl_request_first->req.bmRequestType & USB_REQ_DIR_MASK)
			== USB_REQ_DIR_IN ) {
		_uhd_ctrl_phase_data_in_start();
	} else {
		if (uhd_ctrl_request_first->req.wLength) {
			_uhd_ctrl_phase_data_out();
		} else {
			/* No DATA phase */
			_uhd_ctrl_phase_zlp_in();
		}
	}
}

/**
 * \internal
 * \brief Manages endpoint 0 stall interrupt
 *
 * \param module_inst Pointer to USB module instance
 * \param null        Not used
 */
static void _uhd_ep0_stall(struct usb_module *module_inst, void *null)
{
#ifdef USB_HOST_LPM_SUPPORT
	struct usb_host_pipe_config cfg;
	usb_host_pipe_get_config(&dev, 0, &cfg);

	if (cfg.pipe_type == USB_HOST_PIPE_TYPE_EXTENDED) {
		dbg_print("EXT_LPM_STALL\n");
		cfg.pipe_type = USB_HOST_PIPE_TYPE_CONTROL;
		usb_host_pipe_set_config(&dev, 0, &cfg);
		return;
	}
#endif

	dbg_print("CTRL Stall\n");
	/* Stall Handshake received */
	_uhd_ctrl_request_end(UHD_TRANS_STALL);
}


/**
 * \internal
 * \brief Returns the pipe number matching a USB endpoint
 *
 * \param add   USB device address
 * \param endp  Endpoint Address
 *
 * \return Pipe number
 */
static uint8_t _uhd_get_pipe(usb_add_t add, usb_ep_t endp)
{
	struct usb_host_pipe_config cfg;
	uint8_t usb_pipe = 1;
	for (;usb_pipe < USB_PIPE_NUM;usb_pipe ++) {
		usb_host_pipe_get_config(&dev, usb_pipe, &cfg);
		if ((add == cfg.device_address) && (endp == cfg.endpoint_address)) {
			return usb_pipe;
		}
	}
	return 0;
}

/**
 * \internal
 * \brief Computes and starts the next transfer on a pipe
 *
 * \param module_inst Pointer to USB module instance
 * \param pointer     Pointer to USB pipe transfer callback status parameters
 */
static void _uhd_pipe_trans_complete(struct usb_module *module_inst, void *pointer)
{
	struct usb_pipe_callback_parameter *p_callback_para;
	p_callback_para = (struct usb_pipe_callback_parameter *)pointer;

	struct usb_host_pipe_config cfg;
	usb_host_pipe_get_config(&dev, p_callback_para->pipe_num, &cfg);

	uhd_pipe_job_t *ptr_job;
	uint16_t pipe_size, nb_trans;
	uint16_t max_trans;
	iram_size_t next_trans;
	irqflags_t flags;

	pipe_size = cfg.size;

	/* Get job corresponding at endpoint */
	ptr_job = &uhd_pipe_job[p_callback_para->pipe_num - 1];

	if (!ptr_job->busy) {
		return; // No job is running, then ignore it (system error)
	}

	if (!(cfg.endpoint_address & USB_EP_DIR_IN)) {
		usb_host_pipe_freeze(&dev, p_callback_para->pipe_num);
		/* Transfer complete on OUT */
		nb_trans = p_callback_para->transfered_size;

		/* Update number of transferred data */
		ptr_job->nb_trans += nb_trans;

		/* Need to send other data */
		if ((ptr_job->nb_trans != ptr_job->buf_size)
				|| ptr_job->b_shortpacket) {
			next_trans = ptr_job->buf_size - ptr_job->nb_trans;
			if (UHD_ENDPOINT_MAX_TRANS < next_trans) {
				/**
				 * The USB hardware supports a maximum
				 * transfer size of UHD_ENDPOINT_MAX_TRANS Bytes
				 */
				next_trans = UHD_ENDPOINT_MAX_TRANS -
					(UHD_ENDPOINT_MAX_TRANS % pipe_size);
				usb_host_pipe_set_auto_zlp(&dev, p_callback_para->pipe_num, false);
			} else {
				/* Need ZLP, if requested and last packet is not a short packet */
				usb_host_pipe_set_auto_zlp(&dev, p_callback_para->pipe_num, ptr_job->b_shortpacket);
				ptr_job->b_shortpacket = false; // No need to request another ZLP
			}
			usb_host_pipe_write_job(&dev, p_callback_para->pipe_num, &ptr_job->buf[ptr_job->nb_trans], next_trans);

			/* Enable interrupt */
			flags = cpu_irq_save();
			usb_host_pipe_enable_callback(&dev,p_callback_para->pipe_num,USB_HOST_PIPE_CALLBACK_TRANSFER_COMPLETE);
			cpu_irq_restore(flags);
			return;
		}
	} else {
		/* Transfer complete on IN */
		nb_trans = p_callback_para->transfered_size;

		/* May be required to copy received data from cache buffer to user buffer */
		if (ptr_job->buf_internal != NULL) {
			memcpy(&ptr_job->buf[ptr_job->nb_trans],
					ptr_job->buf_internal,
					ptr_job->buf_size % pipe_size);
			free(ptr_job->buf_internal);
			ptr_job->buf_internal = NULL;
		}

		/* Update number of transfered data */
		ptr_job->nb_trans += nb_trans;
		if (ptr_job->nb_trans > ptr_job->buf_size) {
			ptr_job->nb_trans = ptr_job->buf_size;
		}

		/**
		 * If all previous requested data have been received and user buffer not full
		 * then need to receive other data
		 */
		if ((nb_trans == p_callback_para->required_size)
			&& (ptr_job->nb_trans != ptr_job->buf_size)) {
			next_trans = ptr_job->buf_size - ptr_job->nb_trans;
			max_trans = UHD_ENDPOINT_MAX_TRANS;
			/* 256 is the maximum of IN requests via UPINRQ */
			if ((256L * pipe_size) < UHD_ENDPOINT_MAX_TRANS) {
				max_trans = 256L * pipe_size;
			}
			if (max_trans < next_trans) {
				/* The USB hardware support a maximum transfer size
				  * of UHD_ENDPOINT_MAX_TRANS Bytes
				  */
				next_trans = max_trans;
			}

			if (next_trans < pipe_size) {
				/* Use the cache buffer for Bulk or Interrupt size endpoint */
				ptr_job->buf_internal = malloc(pipe_size);
				if (ptr_job->buf_internal == NULL) {
					Assert(ptr_job->buf_internal != NULL);
					goto uhd_pipe_trans_complet_end;
				}
				usb_host_pipe_read_job(&dev, p_callback_para->pipe_num, ptr_job->buf_internal, pipe_size);
			} else {
				next_trans -= next_trans % pipe_size;
				/* Link the user buffer directly on USB hardware DMA */
				usb_host_pipe_read_job(&dev, p_callback_para->pipe_num, &ptr_job->buf[ptr_job->nb_trans], next_trans);
			}
			/* Enable interrupt */
			flags = cpu_irq_save();
			usb_host_pipe_enable_callback(&dev,p_callback_para->pipe_num,USB_HOST_PIPE_CALLBACK_TRANSFER_COMPLETE);
			cpu_irq_restore(flags);
			return;
		}
	}

uhd_pipe_trans_complet_end:
	/* Call callback to signal end of transfer */
	_uhd_pipe_finish_job(p_callback_para->pipe_num, UHD_TRANS_NOERROR);
	return;
}

/**
 * \internal
 * \brief Aborts the on-going transfer on a pipe
 *
 * \param pipe   Pipe number
 * \param status Reason of abort
 */
static void _uhd_ep_abort_pipe(uint8_t pipe, uhd_trans_status_t status)
{
	usb_host_pipe_freeze(&dev, pipe);
	_uhd_pipe_finish_job(pipe, status);
}

/**
 * \internal
 * \brief Call the callback linked to the end of pipe transfer
 *
 * \param pipe   Pipe number
 * \param status Status of the transfer
 */
static void _uhd_pipe_finish_job(uint8_t pipe, uhd_trans_status_t status)
{
	uhd_pipe_job_t *ptr_job;

	struct usb_host_pipe_config cfg;
	usb_host_pipe_get_config(&dev, pipe, &cfg);

	/* Get job corresponding at endpoint */
	ptr_job = &uhd_pipe_job[pipe - 1];
	if (ptr_job->busy == false) {
		return; // No job running
	}
	/* In case of abort, free the internal buffer */
	if (ptr_job->buf_internal != NULL) {
		free(ptr_job->buf_internal);
		ptr_job->buf_internal = NULL;
	}
	ptr_job->busy = false;
	if (NULL == ptr_job->call_end) {
		return; // No callback linked to job
	}
	ptr_job->call_end(cfg.device_address,
			cfg.endpoint_address,
			status, ptr_job->nb_trans);
}

/**
 * \internal
 * \brief Manages pipe endpoint error callback
 *
 * \param module_inst Pointer to USB module instance
 * \param pointer     Pointer to USB pipe transfer callback status parameters
 */
static void _uhd_ep_error(struct usb_module *module_inst, void *pointer)
{
	uhd_trans_status_t uhd_error;
	struct usb_pipe_callback_parameter *p_callback_para;
	p_callback_para = (struct usb_pipe_callback_parameter *)pointer;

	dbg_print("Tr Error %x\n", p_callback_para->pipe_num);
	/* Get and ack error */
	switch(p_callback_para->pipe_error_status) {
	case USB_STATUS_PIPE_DTGLER:
		uhd_error = UHD_TRANS_DT_MISMATCH;
		break;
	case USB_STATUS_PIPE_TOUTER:
		uhd_error = UHD_TRANS_NOTRESPONDING;
		break;
	case USB_STATUS_PIPE_CRC16ER:
		uhd_error = UHD_TRANS_CRC;
		break;
	case USB_STATUS_PIPE_DAPIDER:
	case USB_STATUS_PIPE_PIDER:
		uhd_error = UHD_TRANS_PIDFAILURE;
		break;
	default:
		uhd_error = UHD_TRANS_TIMEOUT;
		break;
	}

	_uhd_ep_abort_pipe(p_callback_para->pipe_num, uhd_error);
}

/**
 * \internal
 * \brief Manages pipe endpoint stall interrupt
 *
 * \param module_inst Pointer to USB module instance
 * \param pointer     Pointer to USB pipe transfer callback status parameters
 */
static void _uhd_ep_stall(struct usb_module *module_inst, void *pointer)
{
	struct usb_pipe_callback_parameter *p_callback_para;
	p_callback_para = (struct usb_pipe_callback_parameter *)pointer;

	dbg_print("Tr Stall %x\n", p_callback_para->pipe_num);
	usb_host_pipe_clear_toggle(&dev, p_callback_para->pipe_num);
	_uhd_ep_abort_pipe(p_callback_para->pipe_num, UHD_TRANS_STALL);
}

bool uhd_ep0_alloc(usb_add_t add, uint8_t ep_size)
{
	struct usb_host_pipe_config cfg;

	if (ep_size < 8) {
		return false;
	}

	usb_host_pipe_get_config_defaults(&cfg);
	cfg.device_address = add;
	cfg.size = ep_size;
	cfg.binterval = 0;
	usb_host_pipe_set_config(&dev,0,&cfg);
	usb_host_pipe_register_callback(&dev, 0,
			USB_HOST_PIPE_CALLBACK_TRANSFER_COMPLETE, _uhd_ep0_transfer_complete);
	usb_host_pipe_register_callback(&dev, 0,
			USB_HOST_PIPE_CALLBACK_ERROR, _uhd_ep0_error);
	usb_host_pipe_register_callback(&dev, 0,
			USB_HOST_PIPE_CALLBACK_SETUP, _uhd_ep0_setup);
	usb_host_pipe_register_callback(&dev, 0,
			USB_HOST_PIPE_CALLBACK_STALL, _uhd_ep0_stall);
	/* Always enable stall and error interrupts of control endpoint */
	usb_host_pipe_enable_callback(&dev,0,USB_HOST_PIPE_CALLBACK_TRANSFER_COMPLETE);
	usb_host_pipe_enable_callback(&dev,0,USB_HOST_PIPE_CALLBACK_ERROR);
	usb_host_pipe_enable_callback(&dev,0,USB_HOST_PIPE_CALLBACK_SETUP);
	usb_host_pipe_enable_callback(&dev,0,USB_HOST_PIPE_CALLBACK_STALL);
	return true;
}

bool uhd_ep_alloc(usb_add_t add, usb_ep_desc_t *ep_desc)
{
	uint8_t pipe = 1;
	struct usb_host_pipe_config cfg;
	uint8_t ep_type;
	uint8_t ep_interval;

	for (pipe = 1; pipe < USB_PIPE_NUM; pipe++) {
		usb_host_pipe_get_config(&dev, pipe, &cfg);
		if (cfg.pipe_type != USB_HOST_PIPE_TYPE_DISABLE) {
			continue;
		}
		usb_host_pipe_get_config_defaults(&cfg);
		/* Enable pipe */
		ep_type = (ep_desc->bmAttributes & USB_EP_TYPE_MASK) + 1;
		if (ep_type == USB_HOST_PIPE_TYPE_BULK) {
			ep_interval = 0; // Ignore bInterval for bulk endpoint
		} else {
			ep_interval = ep_desc->bInterval;
		}
		cfg.device_address = add;
		cfg.endpoint_address = ep_desc->bEndpointAddress;
		cfg.pipe_type = (enum usb_host_pipe_type)ep_type;
		cfg.binterval = ep_interval;
		cfg.size = le16_to_cpu(ep_desc->wMaxPacketSize);
		usb_host_pipe_set_config(&dev,pipe,&cfg);

		usb_host_pipe_register_callback(&dev, pipe,
				USB_HOST_PIPE_CALLBACK_TRANSFER_COMPLETE, _uhd_pipe_trans_complete);
		usb_host_pipe_register_callback(&dev,pipe,
				USB_HOST_PIPE_CALLBACK_ERROR, _uhd_ep_error);
		usb_host_pipe_register_callback(&dev,pipe,
				USB_HOST_PIPE_CALLBACK_STALL, _uhd_ep_stall);
		/* Enable endpoint interrupts */
		usb_host_pipe_enable_callback(&dev,pipe,USB_HOST_PIPE_CALLBACK_TRANSFER_COMPLETE);
		usb_host_pipe_enable_callback(&dev,pipe,USB_HOST_PIPE_CALLBACK_ERROR);
		usb_host_pipe_enable_callback(&dev,pipe,USB_HOST_PIPE_CALLBACK_STALL);
		return true;
	}
	return false;
}


void uhd_ep_free(usb_add_t add, usb_ep_t endp)
{
	uint8_t usb_pipe = 0;
	struct usb_host_pipe_config cfg;

	/* Search endpoint(s) in all pipes */
	for (usb_pipe = 0; usb_pipe < USB_PIPE_NUM; usb_pipe++) {
		usb_host_pipe_get_config(&dev, usb_pipe, &cfg);

		if (cfg.pipe_type == USB_HOST_PIPE_TYPE_DISABLE) {
			continue;
		}

		if (add != cfg.device_address) {
			continue;
		}

		if (endp != 0xFF) {
			/* Disable specific endpoint number */
			if (!((endp == 0) && (0 == cfg.endpoint_address))) {
				/* It is not the control endpoint */
				if (endp != cfg.endpoint_address) {
					continue; // Mismatch
				}
			}
		}

		if (usb_pipe == 0) {
			/* Disable and stop transfer on control endpoint */
			if (cfg.device_address == add) {
				usb_host_pipe_freeze(&dev, 0);
				if (uhd_ctrl_request_timeout ||
						(uhd_ctrl_request_first != NULL)) {
					_uhd_ctrl_request_end(UHD_TRANS_DISCONNECT);
				}
				continue;
			}
		}

		/* Endpoint interrupt, bulk or isochronous */
		/* Disable and stop transfer on this pipe */
		usb_host_pipe_freeze(&dev, usb_pipe);
		_uhd_pipe_finish_job(usb_pipe, UHD_TRANS_DISCONNECT);
	}
}

bool uhd_ep_run(
		usb_add_t add,
		usb_ep_t endp,
		bool b_shortpacket,
		uint8_t *buf,
		iram_size_t buf_size,
		uint16_t timeout,
		uhd_callback_trans_t callback)
{
	irqflags_t flags;
	uhd_pipe_job_t *ptr_job;
	uint8_t pipe = 0;

	struct usb_pipe_callback_parameter callback_para;

	pipe = _uhd_get_pipe(add,endp);
	if (!pipe) {
		return false;
	}
	/* Get job about pipe */
	ptr_job = &uhd_pipe_job[pipe - 1];
	flags = cpu_irq_save();
	if (ptr_job->busy == true) {
		cpu_irq_restore(flags);
		return false; // Job already on going
	}
	ptr_job->busy = true;

	/* No job running. Let's setup a new one. */
	ptr_job->buf = buf;
	ptr_job->buf_size = buf_size;
	ptr_job->nb_trans = 0;
	ptr_job->timeout = timeout;
	ptr_job->b_shortpacket = b_shortpacket;
	ptr_job->call_end = callback;
	cpu_irq_restore(flags);

	/* Request first transfer */
	callback_para.pipe_num = pipe;
	callback_para.transfered_size = 0;
	callback_para.required_size = 0;
	_uhd_pipe_trans_complete(&dev, &callback_para);
	return true;
}

void uhd_ep_abort(usb_add_t add, usb_ep_t endp)
{
	uint8_t usb_pipe = 0;

	usb_pipe = _uhd_get_pipe(add, endp);
	if (usb_pipe) {
		_uhd_ep_abort_pipe(usb_pipe, UHD_TRANS_ABORTED);
	}
}

/** @} */
