/**
 * \file
 *
 * \brief USB Mass Storage Class (MSC) protocol definitions.
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

#ifndef _USB_PROTOCOL_MSC_H_
#define _USB_PROTOCOL_MSC_H_


/**
 * \ingroup usb_protocol_group
 * \defgroup usb_msc_protocol USB Mass Storage Class (MSC) protocol definitions
 *
 * @{
 */

/**
 * \name Possible Class value
 */
//@{
#define  MSC_CLASS                  0x08
//@}

/**
 * \name Possible SubClass value
 * \note In practise, most devices should use
 * #MSC_SUBCLASS_TRANSPARENT and specify the actual command set in
 * the standard INQUIRY data block, even if the MSC spec indicates
 * otherwise. In particular, RBC is not supported by certain major
 * operating systems like Windows XP.
 */
//@{
#define  MSC_SUBCLASS_RBC           0x01	//!< Reduced Block Commands
#define  MSC_SUBCLASS_ATAPI         0x02	//!< CD/DVD devices
#define  MSC_SUBCLASS_QIC_157       0x03	//!< Tape devices
#define  MSC_SUBCLASS_UFI           0x04	//!< Floppy disk drives
#define  MSC_SUBCLASS_SFF_8070I     0x05	//!< Floppy disk drives
#define  MSC_SUBCLASS_TRANSPARENT   0x06	//!< Determined by INQUIRY
//@}

/**
 * \name Possible protocol value
 * \note Only the BULK protocol should be used in new designs.
 */
//@{
#define  MSC_PROTOCOL_CBI           0x00	//!< Command/Bulk/Interrupt
#define  MSC_PROTOCOL_CBI_ALT       0x01	//!< W/o command completion
#define  MSC_PROTOCOL_BULK          0x50	//!< Bulk-only
//@}


/**
 * \brief MSC USB requests (bRequest)
 */
enum usb_reqid_msc {
	USB_REQ_MSC_BULK_RESET = 0xFF,	//!< Mass Storage Reset
	USB_REQ_MSC_GET_MAX_LUN = 0xFE,	//!< Get Max LUN
};


COMPILER_PACK_SET(1)

/**
 * \name A Command Block Wrapper (CBW).
 */
//@{
struct usb_msc_cbw {
	le32_t dCBWSignature;	//!< Must contain 'USBC'
	le32_t dCBWTag;	//!< Unique command ID
	le32_t dCBWDataTransferLength;	//!< Number of bytes to transfer
	uint8_t bmCBWFlags;	//!< Direction in bit 7
	uint8_t bCBWLUN;	//!< Logical Unit Number
	uint8_t bCBWCBLength;	//!< Number of valid CDB bytes
	uint8_t CDB[16];	//!< SCSI Command Descriptor Block
};

#define  USB_CBW_SIGNATURE          0x55534243	//!< dCBWSignature value
#define  USB_CBW_DIRECTION_IN       (1<<7)	//!< Data from device to host
#define  USB_CBW_DIRECTION_OUT      (0<<7)	//!< Data from host to device
#define  USB_CBW_LUN_MASK           0x0F	//!< Valid bits in bCBWLUN
#define  USB_CBW_LEN_MASK           0x1F	//!< Valid bits in bCBWCBLength
//@}


/**
 * \name A Command Status Wrapper (CSW).
 */
//@{
struct usb_msc_csw {
	le32_t dCSWSignature;	//!< Must contain 'USBS'
	le32_t dCSWTag;	//!< Same as dCBWTag
	le32_t dCSWDataResidue;	//!< Number of bytes not transfered
	uint8_t bCSWStatus;	//!< Status code
};

#define  USB_CSW_SIGNATURE          0x55534253	//!< dCSWSignature value
#define  USB_CSW_STATUS_PASS        0x00	//!< Command Passed
#define  USB_CSW_STATUS_FAIL        0x01	//!< Command Failed
#define  USB_CSW_STATUS_PE          0x02	//!< Phase Error
//@}

COMPILER_PACK_RESET()

//@}

#endif // _USB_PROTOCOL_MSC_H_
