/**
 * \file
 *
 * \brief USB Device Human Interface Device (HID) interface.
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


/**
 * \ingroup udi_hid_group
 * \defgroup udi_hid_group_internal Implementation of HID common library
 * @{
 */

/**
 * \brief Send the specific descriptors requested by SETUP request
 *
 * \retval true if the descriptor is supported
 */
static bool udi_hid_reqstdifaceget_descriptor(uint8_t *report_desc);

bool udi_hid_setup( uint8_t *rate, uint8_t *protocol, uint8_t *report_desc, bool (*setup_report)(void) )
{
	if (Udd_setup_is_in()) {
		// Requests Interface GET
		if (Udd_setup_type() == USB_REQ_TYPE_STANDARD) {
			// Requests Standard Interface Get
			switch (udd_g_ctrlreq.req.bRequest) {

			case USB_REQ_GET_DESCRIPTOR:
				return udi_hid_reqstdifaceget_descriptor(report_desc);
			}
		}
		if (Udd_setup_type() == USB_REQ_TYPE_CLASS) {
			// Requests Class Interface Get
			switch (udd_g_ctrlreq.req.bRequest) {

			case USB_REQ_HID_GET_REPORT:
				return setup_report();

			case USB_REQ_HID_GET_IDLE:
				udd_g_ctrlreq.payload = rate;
				udd_g_ctrlreq.payload_size = 1;
				return true;

			case USB_REQ_HID_GET_PROTOCOL:
				udd_g_ctrlreq.payload = protocol;
				udd_g_ctrlreq.payload_size = 1;
				return true;
			}
		}
	}
	if (Udd_setup_is_out()) {
		// Requests Interface SET
		if (Udd_setup_type() == USB_REQ_TYPE_CLASS) {
			// Requests Class Interface Set
			switch (udd_g_ctrlreq.req.bRequest) {

			case USB_REQ_HID_SET_REPORT:
				return setup_report();

			case USB_REQ_HID_SET_IDLE:
				*rate = udd_g_ctrlreq.req.wValue >> 8;
				return true;

			case USB_REQ_HID_SET_PROTOCOL:
				if (0 != udd_g_ctrlreq.req.wLength)
					return false;
				*protocol = udd_g_ctrlreq.req.wValue;
				return true;
			}
		}
	}
	return false;	// Request not supported
}


//---------------------------------------------
//------- Internal routines

static bool udi_hid_reqstdifaceget_descriptor(uint8_t *report_desc)
{
	usb_hid_descriptor_t UDC_DESC_STORAGE *ptr_hid_desc;

	// Get the USB descriptor which is located after the interface descriptor
	// This descriptor must be the HID descriptor
	ptr_hid_desc = (usb_hid_descriptor_t UDC_DESC_STORAGE *) ((uint8_t *)
			udc_get_interface_desc() + sizeof(usb_iface_desc_t));
	if (USB_DT_HID != ptr_hid_desc->bDescriptorType)
		return false;

	// The SETUP request can ask for:
	// - an USB_DT_HID descriptor
	// - or USB_DT_HID_REPORT descriptor
	// - or USB_DT_HID_PHYSICAL descriptor
	if (USB_DT_HID == (uint8_t) (udd_g_ctrlreq.req.wValue >> 8)) {
		// USB_DT_HID descriptor requested then send it
		udd_g_ctrlreq.payload = (uint8_t *) ptr_hid_desc;
		udd_g_ctrlreq.payload_size =
				min(udd_g_ctrlreq.req.wLength,
				ptr_hid_desc->bLength);
		return true;
	}
	// The HID_X descriptor requested must correspond to report type
	// included in the HID descriptor
	if (ptr_hid_desc->bRDescriptorType ==
			(uint8_t) (udd_g_ctrlreq.req.wValue >> 8)) {
		// Send HID Report descriptor given by high level
		udd_g_ctrlreq.payload = report_desc;
		udd_g_ctrlreq.payload_size =
				min(udd_g_ctrlreq.req.wLength,
				le16_to_cpu(ptr_hid_desc->wDescriptorLength));
		return true;
	}
	return false;
}

//@}
