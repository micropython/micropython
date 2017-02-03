/**
 * \file
 *
 * \brief USB Device Human Interface Device (HID) keyboard interface.
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

#include "conf_usb.h"
#include "usb_protocol.h"
#include "udd.h"
#include "udc.h"
#include "udi_hid.h"
#include "udi_hid_kbd.h"
#include <string.h>

/**
 * \ingroup udi_hid_keyboard_group
 * \defgroup udi_hid_keyboard_group_udc Interface with USB Device Core (UDC)
 *
 * Structures and functions required by UDC.
 *
 * @{
 */

bool udi_hid_kbd_enable(void);
void udi_hid_kbd_disable(void);
bool udi_hid_kbd_setup(void);
uint8_t udi_hid_kbd_getsetting(void);

//! Global structure which contains standard UDI interface for UDC
UDC_DESC_STORAGE udi_api_t udi_api_hid_kbd = {
	.enable = (bool(*)(void))udi_hid_kbd_enable,
	.disable = (void (*)(void))udi_hid_kbd_disable,
	.setup = (bool(*)(void))udi_hid_kbd_setup,
	.getsetting = (uint8_t(*)(void))udi_hid_kbd_getsetting,
	.sof_notify = NULL,
};
//@}


/**
 * \ingroup udi_hid_keyboard_group
 * \defgroup udi_hid_keyboard_group_internal Implementation of UDI HID keyboard
 *
 * Class internal implementation
 * @{
 */

/**
 * \name Internal defines and variables to manage HID keyboard
 */
//@{

//! Size of report for standard HID keyboard
#define UDI_HID_KBD_REPORT_SIZE  8


//! To store current rate of HID keyboard
COMPILER_WORD_ALIGNED
		static uint8_t udi_hid_kbd_rate;
//! To store current protocol of HID keyboard
COMPILER_WORD_ALIGNED
		static uint8_t udi_hid_kbd_protocol;
//! To store report feedback from USB host
COMPILER_WORD_ALIGNED
		static uint8_t udi_hid_kbd_report_set;
//! To signal if a valid report is ready to send
static bool udi_hid_kbd_b_report_valid;
//! Report ready to send
static uint8_t udi_hid_kbd_report[UDI_HID_KBD_REPORT_SIZE];
//! Signal if a report transfer is on going
static bool udi_hid_kbd_b_report_trans_ongoing;
//! Buffer used to send report
COMPILER_WORD_ALIGNED
		static uint8_t
		udi_hid_kbd_report_trans[UDI_HID_KBD_REPORT_SIZE];

//@}

//! HID report descriptor for standard HID keyboard
UDC_DESC_STORAGE udi_hid_kbd_report_desc_t udi_hid_kbd_report_desc = {
	{
				0x05, 0x01,	/* Usage Page (Generic Desktop)      */
				0x09, 0x06,	/* Usage (Keyboard)                  */
				0xA1, 0x01,	/* Collection (Application)          */
				0x05, 0x07,	/* Usage Page (Keyboard)             */
				0x19, 224,	/* Usage Minimum (224)               */
				0x29, 231,	/* Usage Maximum (231)               */
				0x15, 0x00,	/* Logical Minimum (0)               */
				0x25, 0x01,	/* Logical Maximum (1)               */
				0x75, 0x01,	/* Report Size (1)                   */
				0x95, 0x08,	/* Report Count (8)                  */
				0x81, 0x02,	/* Input (Data, Variable, Absolute)  */
				0x81, 0x01,	/* Input (Constant)                  */
				0x19, 0x00,	/* Usage Minimum (0)                 */
				0x29, 101,	/* Usage Maximum (101)               */
				0x15, 0x00,	/* Logical Minimum (0)               */
				0x25, 101,	/* Logical Maximum (101)             */
				0x75, 0x08,	/* Report Size (8)                   */
				0x95, 0x06,	/* Report Count (6)                  */
				0x81, 0x00,	/* Input (Data, Array)               */
				0x05, 0x08,	/* Usage Page (LED)                  */
				0x19, 0x01,	/* Usage Minimum (1)                 */
				0x29, 0x05,	/* Usage Maximum (5)                 */
				0x15, 0x00,	/* Logical Minimum (0)               */
				0x25, 0x01,	/* Logical Maximum (1)               */
				0x75, 0x01,	/* Report Size (1)                   */
				0x95, 0x05,	/* Report Count (5)                  */
				0x91, 0x02,	/* Output (Data, Variable, Absolute) */
				0x95, 0x03,	/* Report Count (3)                  */
				0x91, 0x01,	/* Output (Constant)                 */
				0xC0	/* End Collection                    */
			}
};

/**
 * \name Internal routines
 */
//@{

/**
 * \brief Changes keyboard report states (like LEDs)
 *
 * \param rate       New rate value
 *
 */
static bool udi_hid_kbd_setreport(void);

/**
 * \brief Send the report
 *
 * \return \c 1 if send on going, \c 0 if delay.
 */
static bool udi_hid_kbd_send_report(void);

/**
 * \brief Callback called when the report is sent
 *
 * \param status     UDD_EP_TRANSFER_OK, if transfer is completed
 * \param status     UDD_EP_TRANSFER_ABORT, if transfer is aborted
 * \param nb_sent    number of data transfered
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
static void udi_hid_kbd_report_sent(udd_ep_status_t status, iram_size_t nb_sent,
		udd_ep_id_t ep);

/**
 * \brief Callback called to update report from USB host
 * udi_hid_kbd_report_set is updated before callback execution
 */
static void udi_hid_kbd_setreport_valid(void);

//@}


//--------------------------------------------
//------ Interface for UDI HID level

bool udi_hid_kbd_enable(void)
{
	// Initialize internal values
	udi_hid_kbd_rate = 0;
	udi_hid_kbd_protocol = 0;
	udi_hid_kbd_b_report_trans_ongoing = false;
	memset(udi_hid_kbd_report, 0, UDI_HID_KBD_REPORT_SIZE);
	udi_hid_kbd_b_report_valid = false;
	return UDI_HID_KBD_ENABLE_EXT();
}


void udi_hid_kbd_disable(void)
{
	UDI_HID_KBD_DISABLE_EXT();
}


bool udi_hid_kbd_setup(void)
{
	return udi_hid_setup(&udi_hid_kbd_rate,
								&udi_hid_kbd_protocol,
								(uint8_t *) &udi_hid_kbd_report_desc,
								udi_hid_kbd_setreport);
}


uint8_t udi_hid_kbd_getsetting(void)
{
	return 0;
}


static bool udi_hid_kbd_setreport(void)
{
	if ((USB_HID_REPORT_TYPE_OUTPUT == (udd_g_ctrlreq.req.wValue >> 8))
			&& (0 == (0xFF & udd_g_ctrlreq.req.wValue))
			&& (1 == udd_g_ctrlreq.req.wLength)) {
		// Report OUT type on report ID 0 from USB Host
		udd_g_ctrlreq.payload = &udi_hid_kbd_report_set;
		udd_g_ctrlreq.callback = udi_hid_kbd_setreport_valid;
		udd_g_ctrlreq.payload_size = 1;
		return true;
	}
	return false;
}


//--------------------------------------------
//------ Interface for application

bool udi_hid_kbd_modifier_up(uint8_t modifier_id)
{
	irqflags_t flags = cpu_irq_save();

	// Fill report
	udi_hid_kbd_report[0] &= ~(unsigned)modifier_id;
	udi_hid_kbd_b_report_valid = true;

	// Send report
	udi_hid_kbd_send_report();

	cpu_irq_restore(flags);
	return true;
}


bool udi_hid_kbd_modifier_down(uint8_t modifier_id)
{
	irqflags_t flags = cpu_irq_save();

	// Fill report
	udi_hid_kbd_report[0] |= modifier_id;
	udi_hid_kbd_b_report_valid = true;

	// Send report
	udi_hid_kbd_send_report();

	cpu_irq_restore(flags);
	return true;
}


bool udi_hid_kbd_up(uint8_t key_id)
{
	uint8_t i;

	irqflags_t flags = cpu_irq_save();

	// Fill report
	for (i = 2; i < UDI_HID_KBD_REPORT_SIZE; i++) {
		if (0 == udi_hid_kbd_report[i]) {
			// Already removed
			cpu_irq_restore(flags);
			return true;
		}
		if (key_id == udi_hid_kbd_report[i])
			break;
	}
	if (UDI_HID_KBD_REPORT_SIZE == i) {
		// Already removed
		cpu_irq_restore(flags);
		return true;
	}
	// Remove key and shift
	while (i < (UDI_HID_KBD_REPORT_SIZE - 1)) {
		udi_hid_kbd_report[i] = udi_hid_kbd_report[i + 1];
		i++;
	}
	udi_hid_kbd_report[UDI_HID_KBD_REPORT_SIZE - 1] = 0x00;
	udi_hid_kbd_b_report_valid = true;

	// Send report
	udi_hid_kbd_send_report();

	cpu_irq_restore(flags);
	return true;
}


bool udi_hid_kbd_down(uint8_t key_id)
{
	uint8_t i;

	irqflags_t flags = cpu_irq_save();

	// Fill report
	for (i = 2; i < UDI_HID_KBD_REPORT_SIZE; i++) {
		if (0 == udi_hid_kbd_report[i])
			break;
		if (key_id == udi_hid_kbd_report[i]) {
			// Already in array
			cpu_irq_restore(flags);
			return true;
		}
	}

	if (UDI_HID_KBD_REPORT_SIZE == i) {
		// Array full
		// TODO manage more than UDI_HID_KBD_REPORT_SIZE key pressed in same time
		cpu_irq_restore(flags);
		return false;
	}
	// Add key at the end of array
	udi_hid_kbd_report[i] = key_id;
	udi_hid_kbd_b_report_valid = true;

	// Send report
	udi_hid_kbd_send_report();

	// Enable IT
	cpu_irq_restore(flags);
	return true;
}


//--------------------------------------------
//------ Internal routines

static bool udi_hid_kbd_send_report(void)
{
	if (udi_hid_kbd_b_report_trans_ongoing)
		return false;
	memcpy(udi_hid_kbd_report_trans, udi_hid_kbd_report,
			UDI_HID_KBD_REPORT_SIZE);
	udi_hid_kbd_b_report_valid = false;
	udi_hid_kbd_b_report_trans_ongoing =
			udd_ep_run(	UDI_HID_KBD_EP_IN,
							false,
							udi_hid_kbd_report_trans,
							UDI_HID_KBD_REPORT_SIZE,
							udi_hid_kbd_report_sent);
	return udi_hid_kbd_b_report_trans_ongoing;
}

static void udi_hid_kbd_report_sent(udd_ep_status_t status, iram_size_t nb_sent,
		udd_ep_id_t ep)
{
	UNUSED(status);
	UNUSED(nb_sent);
	UNUSED(ep);
	udi_hid_kbd_b_report_trans_ongoing = false;
	if (udi_hid_kbd_b_report_valid) {
		udi_hid_kbd_send_report();
	}
}

static void udi_hid_kbd_setreport_valid(void)
{
	UDI_HID_KBD_CHANGE_LED(udi_hid_kbd_report_set);
}

//@}
