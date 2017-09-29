/**
 * \file
 *
 * \brief USB Communication Device Class (CDC) protocol definitions
 *
 * Copyright (c) 2015 - 2017 Atmel Corporation. All rights reserved.
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
#ifndef _USB_PROTOCOL_CDC_H_
#define _USB_PROTOCOL_CDC_H_

#include "usb_includes.h"

/**
 * \ingroup usb_protocol_group
 * \defgroup cdc_protocol_group Communication Device Class Definitions
 * @{
 */

/**
 * \name Possible values of class
 */
//@{
#define CDC_CLASS_DEVICE 0x02 //!< USB Communication Device Class
#define CDC_CLASS_COMM 0x02   //!< CDC Communication Class Interface
#define CDC_CLASS_DATA 0x0A   //!< CDC Data Class Interface
//@}

//! \name USB CDC Subclass IDs
//@{
#define CDC_SUBCLASS_DLCM 0x01 //!< Direct Line Control Model
#define CDC_SUBCLASS_ACM 0x02  //!< Abstract Control Model
#define CDC_SUBCLASS_TCM 0x03  //!< Telephone Control Model
#define CDC_SUBCLASS_MCCM 0x04 //!< Multi-Channel Control Model
#define CDC_SUBCLASS_CCM 0x05  //!< CAPI Control Model
#define CDC_SUBCLASS_ETH 0x06  //!< Ethernet Networking Control Model
#define CDC_SUBCLASS_ATM 0x07  //!< ATM Networking Control Model
//@}

//! \name USB CDC Communication Interface Protocol IDs
//@{
#define CDC_PROTOCOL_V25TER 0x01 //!< Common AT commands
//@}

//! \name USB CDC Data Interface Protocol IDs
//@{
#define CDC_PROTOCOL_I430 0x30   //!< ISDN BRI
#define CDC_PROTOCOL_HDLC 0x31   //!< HDLC
#define CDC_PROTOCOL_TRANS 0x32  //!< Transparent
#define CDC_PROTOCOL_Q921M 0x50  //!< Q.921 management protocol
#define CDC_PROTOCOL_Q921 0x51   //!< Q.931 [sic] Data link protocol
#define CDC_PROTOCOL_Q921TM 0x52 //!< Q.921 TEI-multiplexor
#define CDC_PROTOCOL_V42BIS 0x90 //!< Data compression procedures
#define CDC_PROTOCOL_Q931 0x91   //!< Euro-ISDN protocol control
#define CDC_PROTOCOL_V120 0x92   //!< V.24 rate adaption to ISDN
#define CDC_PROTOCOL_CAPI20 0x93 //!< CAPI Commands
#define CDC_PROTOCOL_HOST 0xFD   //!< Host based driver
                                 /**
                                  * \brief Describes the Protocol Unit Functional Descriptors [sic]
                                  * on Communication Class Interface
                                  */
#define CDC_PROTOCOL_PUFD 0xFE
//@}

//! \name USB CDC Functional Descriptor Types
//@{
#define CDC_CS_INTERFACE 0x24 //!< Interface Functional Descriptor
#define CDC_CS_ENDPOINT 0x25  //!< Endpoint Functional Descriptor
//@}

//! \name USB CDC Functional Descriptor Subtypes
//@{
#define CDC_SCS_HEADER 0x00    //!< Header Functional Descriptor
#define CDC_SCS_CALL_MGMT 0x01 //!< Call Management
#define CDC_SCS_ACM 0x02       //!< Abstract Control Management
#define CDC_SCS_UNION 0x06     //!< Union Functional Descriptor
//@}

//! \name USB CDC Request IDs
//@{
#define USB_REQ_CDC_SEND_ENCAPSULATED_COMMAND 0x00
#define USB_REQ_CDC_GET_ENCAPSULATED_RESPONSE 0x01
#define USB_REQ_CDC_SET_COMM_FEATURE 0x02
#define USB_REQ_CDC_GET_COMM_FEATURE 0x03
#define USB_REQ_CDC_CLEAR_COMM_FEATURE 0x04
#define USB_REQ_CDC_SET_AUX_LINE_STATE 0x10
#define USB_REQ_CDC_SET_HOOK_STATE 0x11
#define USB_REQ_CDC_PULSE_SETUP 0x12
#define USB_REQ_CDC_SEND_PULSE 0x13
#define USB_REQ_CDC_SET_PULSE_TIME 0x14
#define USB_REQ_CDC_RING_AUX_JACK 0x15
#define USB_REQ_CDC_SET_LINE_CODING 0x20
#define USB_REQ_CDC_GET_LINE_CODING 0x21
#define USB_REQ_CDC_SET_CONTROL_LINE_STATE 0x22
#define USB_REQ_CDC_SEND_BREAK 0x23
#define USB_REQ_CDC_SET_RINGER_PARMS 0x30
#define USB_REQ_CDC_GET_RINGER_PARMS 0x31
#define USB_REQ_CDC_SET_OPERATION_PARMS 0x32
#define USB_REQ_CDC_GET_OPERATION_PARMS 0x33
#define USB_REQ_CDC_SET_LINE_PARMS 0x34
#define USB_REQ_CDC_GET_LINE_PARMS 0x35
#define USB_REQ_CDC_DIAL_DIGITS 0x36
#define USB_REQ_CDC_SET_UNIT_PARAMETER 0x37
#define USB_REQ_CDC_GET_UNIT_PARAMETER 0x38
#define USB_REQ_CDC_CLEAR_UNIT_PARAMETER 0x39
#define USB_REQ_CDC_GET_PROFILE 0x3A
#define USB_REQ_CDC_SET_ETHERNET_MULTICAST_FILTERS 0x40
#define USB_REQ_CDC_SET_ETHERNET_POWER_MANAGEMENT_PATTERNFILTER 0x41
#define USB_REQ_CDC_GET_ETHERNET_POWER_MANAGEMENT_PATTERNFILTER 0x42
#define USB_REQ_CDC_SET_ETHERNET_PACKET_FILTER 0x43
#define USB_REQ_CDC_GET_ETHERNET_STATISTIC 0x44
#define USB_REQ_CDC_SET_ATM_DATA_FORMAT 0x50
#define USB_REQ_CDC_GET_ATM_DEVICE_STATISTICS 0x51
#define USB_REQ_CDC_SET_ATM_DEFAULT_VC 0x52
#define USB_REQ_CDC_GET_ATM_VC_STATISTICS 0x53
// Added bNotification codes according cdc spec 1.1 chapter 6.3
#define USB_REQ_CDC_NOTIFY_RING_DETECT 0x09
#define USB_REQ_CDC_NOTIFY_SERIAL_STATE 0x20
#define USB_REQ_CDC_NOTIFY_CALL_STATE_CHANGE 0x28
#define USB_REQ_CDC_NOTIFY_LINE_STATE_CHANGE 0x29
//@}

/*
 * Need to pack structures tightly, or the compiler might insert padding
 * and violate the spec-mandated layout.
 */
COMPILER_PACK_SET(1)

//! \name USB CDC Descriptors
//@{

//! CDC Header Functional Descriptor
typedef struct usb_cdc_hdr_desc {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	le16_t  bcdCDC;
} usb_cdc_hdr_desc_t;

#define USB_CDC_HDR_DESC_LEN 5
#define USB_CDC_HDR_DESC_BYTES(bcdCDC)                                                                                 \
	USB_CDC_HDR_DESC_LEN,                  /* bFunctionLength */                                                       \
	    CDC_CS_INTERFACE,                  /* bDescriptorType */                                                       \
	    CDC_SCS_HEADER,                    /* bDescriptorSubtype */                                                    \
	    LE_BYTE0(bcdCDC), LE_BYTE1(bcdCDC) /* bcdCDC */

//! CDC Call Management Functional Descriptor
typedef struct usb_cdc_call_mgmt_desc {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bmCapabilities;
	uint8_t bDataInterface;
} usb_cdc_call_mgmt_desc_t;

#define USB_CDC_CALL_MGMT_DESC_LEN 5
#define USB_CDC_CALL_MGMT_DESC_BYTES(bmCapabilities, bDataInterface)                                                   \
	USB_CDC_CALL_MGMT_DESC_LEN, /* bFunctionLength */                                                                  \
	    CDC_CS_INTERFACE,       /* bDescriptorType */                                                                  \
	    CDC_SCS_CALL_MGMT,      /* bDescriptorSubtype */                                                               \
	    bmCapabilities, bDataInterface

//! CDC ACM Functional Descriptor
typedef struct usb_cdc_acm_desc {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bmCapabilities;
} usb_cdc_acm_desc_t;

#define USB_CDC_ACM_DESC_LEN 4
#define USB_CDC_ACM_DESC_BYTES(bmCapabilities)                                                                         \
	USB_CDC_ACM_DESC_LEN, /* bFunctionLength */                                                                        \
	    CDC_CS_INTERFACE, /* bDescriptorType */                                                                        \
	    CDC_SCS_ACM,      /* bDescriptorSubType */                                                                     \
	    bmCapabilities

//! CDC Union Functional Descriptor
typedef struct usb_cdc_union_desc {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bMasterInterface;
	uint8_t bSlaveInterface0;
} usb_cdc_union_desc_t;

#define USB_CDC_UNION_DESC_LEN 5
#define USB_CDC_UNION_DESC_BYTES(bMasterInterface, bSlaveInterface)                                                    \
	USB_CDC_UNION_DESC_LEN, CDC_CS_INTERFACE, CDC_SCS_UNION, bMasterInterface, bSlaveInterface

//! \name USB CDC Call Management Capabilities
//@{
//! Device handles call management itself
#define CDC_CALL_MGMT_SUPPORTED (1 << 0)
//! Device can send/receive call management info over a Data Class interface
#define CDC_CALL_MGMT_OVER_DCI (1 << 1)
//@}

//! \name USB CDC ACM Capabilities
//@{
//! Device supports the request combination of
//! Set_Comm_Feature, Clear_Comm_Feature, and Get_Comm_Feature.
#define CDC_ACM_SUPPORT_FEATURE_REQUESTS (1 << 0)
//! Device supports the request combination of
//! Set_Line_Coding, Set_Control_Line_State, Get_Line_Coding,
//! and the notification Serial_State.
#define CDC_ACM_SUPPORT_LINE_REQUESTS (1 << 1)
//! Device supports the request Send_Break
#define CDC_ACM_SUPPORT_SENDBREAK_REQUESTS (1 << 2)
//! Device supports the notification Network_Connection.
#define CDC_ACM_SUPPORT_NOTIFY_REQUESTS (1 << 3)
//@}
//@}

//! \name USB CDC line control
//@{

//! \name USB CDC line coding
//@{

//! Line Coding structure
typedef struct usb_cdc_line_coding {
	le32_t  dwDTERate;   //!< Data rate, bits per second
	uint8_t bCharFormat; //!< 0-1 Stop bit,1-1.5 Stop bits,2-2 Stop bits
	uint8_t bParityType; //!< 0-None,1-Odd,2-Even,3-Mark,4-Space
	uint8_t bDataBits;   //!< 5,6,7,8 or 16
} usb_cdc_line_coding_t;

//! Possible values of bCharFormat
enum cdc_char_format {
	CDC_STOP_BITS_1   = 0, //!< 1 stop bit
	CDC_STOP_BITS_1_5 = 1, //!< 1.5 stop bits
	CDC_STOP_BITS_2   = 2  //!< 2 stop bits
};

//! Possible values of bParityType
enum cdc_parity {
	CDC_PAR_NONE  = 0, //!< No parity
	CDC_PAR_ODD   = 1, //!< Odd parity
	CDC_PAR_EVEN  = 2, //!< Even parity
	CDC_PAR_MARK  = 3, //!< Parity forced to 1 (mark)
	CDC_PAR_SPACE = 4  //!< Parity forced to 0 (space)
};
//@}

//! \name USB CDC control signals
//! spec 1.1 chapter 6.2.14
//@{

//! Control signal structure
typedef struct usb_cdc_control_signal {
	union {
		le16_t value;
		struct {
			uint8_t dte_present;
			uint8_t carrier_ctrl;
		} modem;
		struct {
			uint8_t DTR : 1; //!< Data Terminal Ready
			uint8_t RTS : 1; //!< Request To Send
		} rs232;
		struct {
			uint8_t s108_2 : 1; //!< V.24 signal 108/2
			uint8_t s105 : 1;   //!< V.24 signal 105
		} v24;
	};
} usb_cdc_control_signal_t;

//! \name Possible values in usb_cdc_control_signal_t
//@{
//! Carrier control for half duplex modems.
//! This signal corresponds to V.24 signal 105 and RS-232 signal RTS.
//! The device ignores the value of this bit
//! when operating in full duplex mode.
#define CDC_CTRL_SIGNAL_ACTIVATE_CARRIER (1 << 1)
//! Indicates to DCE if DTE is present or not.
//! This signal corresponds to V.24 signal 108/2 and RS-232 signal DTR.
#define CDC_CTRL_SIGNAL_DTE_PRESENT (1 << 0)
//@}
//@}

//! \name USB CDC notification message
//@{

typedef struct usb_cdc_notify_msg {
	uint8_t bmRequestType;
	uint8_t bNotification;
	union {
		le16_t wValue;
		struct {
			uint8_t low;
			uint8_t high;
		} wValueBytes;
	};
	union {
		le16_t wIndex;
		struct {
			uint8_t low;
			uint8_t high;
		} wIndexBytes;
	};
	union {
		le16_t wLength;
		struct {
			uint8_t low;
			uint8_t high;
		} wLengthBytes;
	};
} usb_cdc_notify_msg_t;

//! \name USB CDC serial state
//@{*

//! UART State Bitmap (cdc spec 1.1 chapter 6.3.5)
typedef union usb_cdc_uart_state {
	le16_t value;
	struct {
		uint8_t bRxCarrier : 1;
		uint8_t bTxCarrier : 1;
		uint8_t bBreak : 1;
		uint8_t bRingSignal : 1;
		uint8_t bFraming : 1;
		uint8_t bParity : 1;
		uint8_t bOverRun;
	} bitmap;
	struct {
		uint8_t bDCD : 1;
		uint8_t bDSR : 1;
		uint8_t bBreak : 1;
		uint8_t bRingSignal : 1;
		uint8_t bFraming : 1;
		uint8_t bParity : 1;
		uint8_t bOverRun;
	} rs232;
	struct {
		uint8_t bS109 : 1; //!< V.24 signal 109
		uint8_t bS106 : 1; //!< V.24 signal 106
		uint8_t bBreak : 1;
		uint8_t bRingSignal : 1;
		uint8_t bFraming : 1;
		uint8_t bParity : 1;
		uint8_t bOverRun;
	} v24;
} usb_cdc_uart_state_t;

//! Hardware handshake support (cdc spec 1.1 chapter 6.3.5)
typedef struct usb_cdc_notify_serial_state {
	usb_cdc_notify_msg_t     header;
	union usb_cdc_uart_state state;
} usb_cdc_notify_serial_state_t;

//! \name Possible values in usb_cdc_notify_serial_state_t
//@{
#define CDC_SERIAL_STATE_DCD CPU_TO_LE16((1 << 0))
#define CDC_SERIAL_STATE_DSR CPU_TO_LE16((1 << 1))
#define CDC_SERIAL_STATE_BREAK CPU_TO_LE16((1 << 2))
#define CDC_SERIAL_STATE_RING CPU_TO_LE16((1 << 3))
#define CDC_SERIAL_STATE_FRAMING CPU_TO_LE16((1 << 4))
#define CDC_SERIAL_STATE_PARITY CPU_TO_LE16((1 << 5))
#define CDC_SERIAL_STATE_OVERRUN CPU_TO_LE16((1 << 6))
//@}
//! @}

//! @}

COMPILER_PACK_RESET()

//! @}

/**
 * \brief Fill a CDC SetLineCoding request
 * \param[out] req   Pointer to the request to fill
 * \param[in]  iface Interface Number
 */
static inline void usb_fill_SetLineCoding_req(struct usb_req *req, uint8_t iface)
{
	req->bmRequestType = 0x21;
	req->bRequest      = USB_REQ_CDC_SET_LINE_CODING;
	req->wValue        = 0;
	req->wIndex        = iface;
	req->wLength       = sizeof(usb_cdc_line_coding_t);
}

/**
 * \brief Fill a CDC SetControlLineState request
 * \param[out] req   Pointer to the request to fill
 * \param[in]  iface Interface Number
 * \param[in]  ctrl  Control Signal Bitmap
 */
static inline void usb_fill_SetControlLineState_req(struct usb_req *req, uint8_t iface, uint16_t ctrl)
{
	req->bmRequestType = 0x21;
	req->bRequest      = USB_REQ_CDC_SET_CONTROL_LINE_STATE;
	req->wValue        = ctrl;
	req->wIndex        = iface;
	req->wLength       = 0;
}

#endif // _USB_PROTOCOL_CDC_H_
