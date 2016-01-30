/*
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Lennart Augustsson (lennart@augustsson.net) at
 * Carlstedt Research & Technology.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* Modified by Synopsys, Inc, 12/12/2007 */


#ifndef _USB_H_
#define _USB_H_
#include "basic_types.h"
#include "osdep_service.h"
//#include "osdep_api.h"
#include "hal_util.h"

#include "errno.h"
#include "diag.h"
#include "usb_ch9.h"
#include "usb_defs.h"

/*
 * The USB records contain some unaligned little-endian word
 * components.  The U[SG]ETW macros take care of both the alignment
 * and endian problem and should always be used to access non-byte
 * values.
 */
typedef u8  uByte;
typedef u8  uWord[2];
typedef u8  uDWord[4];

#define USETW2(w,h,l) ((w)[0] = (u_int8_t)(l), (w)[1] = (u_int8_t)(h))
#define UCONSTW(x)	{ (x) & 0xff, ((x) >> 8) & 0xff }
#define UCONSTDW(x)	{ (x) & 0xff, ((x) >> 8) & 0xff, \
			  ((x) >> 16) & 0xff, ((x) >> 24) & 0xff }

#if 1
#define UGETW(w) ((w)[0] | ((w)[1] << 8))
#define USETW(w,v) ((w)[0] = (u_int8_t)(v), (w)[1] = (u_int8_t)((v) >> 8))
#define UGETDW(w) ((w)[0] | ((w)[1] << 8) | ((w)[2] << 16) | ((w)[3] << 24))
#define USETDW(w,v) ((w)[0] = (u_int8_t)(v), \
		     (w)[1] = (u_int8_t)((v) >> 8), \
		     (w)[2] = (u_int8_t)((v) >> 16), \
		     (w)[3] = (u_int8_t)((v) >> 24))
#else
/*
 * On little-endian machines that can handle unanliged accesses
 * (e.g. i386) these macros can be replaced by the following.
 */
#define UGETW(w) (*(u_int16_t *)(w))
#define USETW(w,v) (*(u_int16_t *)(w) = (v))
#define UGETDW(w) (*(u_int32_t *)(w))
#define USETDW(w,v) (*(u_int32_t *)(w) = (v))
#endif

/*
 * Macros for accessing UAS IU fields, which are big-endian
 */
#define IUSETW2(w,h,l) ((w)[0] = (u_int8_t)(h), (w)[1] = (u_int8_t)(l))
#define IUCONSTW(x)	{ ((x) >> 8) & 0xff, (x) & 0xff }
#define IUCONSTDW(x)	{ ((x) >> 24) & 0xff, ((x) >> 16) & 0xff, \
			((x) >> 8) & 0xff, (x) & 0xff }
#define IUGETW(w) (((w)[0] << 8) | (w)[1])
#define IUSETW(w,v) ((w)[0] = (u_int8_t)((v) >> 8), (w)[1] = (u_int8_t)(v))
#define IUGETDW(w) (((w)[0] << 24) | ((w)[1] << 16) | ((w)[2] << 8) | (w)[3])
#define IUSETDW(w,v) ((w)[0] = (u_int8_t)((v) >> 24), \
		      (w)[1] = (u_int8_t)((v) >> 16), \
		      (w)[2] = (u_int8_t)((v) >> 8), \
		      (w)[3] = (u_int8_t)(v))

//#define UPACKED __attribute__((__packed__))
#define UPACKED
typedef struct {
	uByte		bmRequestType;
	uByte		bRequest;
	uWord		wValue;
	uWord		wIndex;
	uWord		wLength;
} UPACKED usb_device_request_t;

#define UT_GET_DIR(a) ((a) & 0x80)
#define UT_WRITE		0x00
#define UT_READ			0x80

#define UT_GET_TYPE(a) ((a) & 0x60)
#define UT_STANDARD		0x00
#define UT_CLASS		0x20
#define UT_VENDOR		0x40

#define UT_GET_RECIPIENT(a) ((a) & 0x1f)
#define UT_DEVICE		0x00
#define UT_INTERFACE		0x01
#define UT_ENDPOINT		0x02
#define UT_OTHER		0x03

#define UT_READ_DEVICE		(UT_READ  | UT_STANDARD | UT_DEVICE)
#define UT_READ_INTERFACE	(UT_READ  | UT_STANDARD | UT_INTERFACE)
#define UT_READ_ENDPOINT	(UT_READ  | UT_STANDARD | UT_ENDPOINT)
#define UT_WRITE_DEVICE		(UT_WRITE | UT_STANDARD | UT_DEVICE)
#define UT_WRITE_INTERFACE	(UT_WRITE | UT_STANDARD | UT_INTERFACE)
#define UT_WRITE_ENDPOINT	(UT_WRITE | UT_STANDARD | UT_ENDPOINT)
#define UT_READ_CLASS_DEVICE	(UT_READ  | UT_CLASS | UT_DEVICE)
#define UT_READ_CLASS_INTERFACE	(UT_READ  | UT_CLASS | UT_INTERFACE)
#define UT_READ_CLASS_OTHER	(UT_READ  | UT_CLASS | UT_OTHER)
#define UT_READ_CLASS_ENDPOINT	(UT_READ  | UT_CLASS | UT_ENDPOINT)
#define UT_WRITE_CLASS_DEVICE	(UT_WRITE | UT_CLASS | UT_DEVICE)
#define UT_WRITE_CLASS_INTERFACE (UT_WRITE | UT_CLASS | UT_INTERFACE)
#define UT_WRITE_CLASS_OTHER	(UT_WRITE | UT_CLASS | UT_OTHER)
#define UT_WRITE_CLASS_ENDPOINT	(UT_WRITE | UT_CLASS | UT_ENDPOINT)
#define UT_READ_VENDOR_DEVICE	(UT_READ  | UT_VENDOR | UT_DEVICE)
#define UT_READ_VENDOR_INTERFACE (UT_READ  | UT_VENDOR | UT_INTERFACE)
#define UT_READ_VENDOR_OTHER	(UT_READ  | UT_VENDOR | UT_OTHER)
#define UT_READ_VENDOR_ENDPOINT	(UT_READ  | UT_VENDOR | UT_ENDPOINT)
#define UT_WRITE_VENDOR_DEVICE	(UT_WRITE | UT_VENDOR | UT_DEVICE)
#define UT_WRITE_VENDOR_INTERFACE (UT_WRITE | UT_VENDOR | UT_INTERFACE)
#define UT_WRITE_VENDOR_OTHER	(UT_WRITE | UT_VENDOR | UT_OTHER)
#define UT_WRITE_VENDOR_ENDPOINT (UT_WRITE | UT_VENDOR | UT_ENDPOINT)

/* Requests */
#define UR_GET_STATUS		0x00
#define  USTAT_STANDARD_STATUS  0x00
#define  WUSTAT_WUSB_FEATURE    0x01
#define  WUSTAT_CHANNEL_INFO    0x02
#define  WUSTAT_RECEIVED_DATA   0x03
#define  WUSTAT_MAS_AVAILABILITY 0x04
#define  WUSTAT_CURRENT_TRANSMIT_POWER 0x05
#define UR_CLEAR_FEATURE	0x01
#define UR_SET_FEATURE		0x03
#define UR_SET_AND_TEST_FEATURE 0x0c
#define UR_SET_ADDRESS		0x05
#define UR_GET_DESCRIPTOR	0x06
#define  UDESC_DEVICE		0x01
#define  UDESC_CONFIG		0x02
#define  UDESC_STRING		0x03
#define  UDESC_INTERFACE	0x04
#define  UDESC_ENDPOINT		0x05
#define  UDESC_SS_USB_COMPANION	0x30
#define  UDESC_DEVICE_QUALIFIER	0x06
#define  UDESC_OTHER_SPEED_CONFIGURATION 0x07
#define  UDESC_INTERFACE_POWER	0x08
#define  UDESC_OTG		0x09
#define  WUDESC_SECURITY	0x0c
#define  WUDESC_KEY		0x0d
#define   WUD_GET_KEY_INDEX(_wValue_) ((_wValue_) & 0xf)
#define   WUD_GET_KEY_TYPE(_wValue_) (((_wValue_) & 0x30) >> 4)
#define    WUD_KEY_TYPE_ASSOC    0x01
#define    WUD_KEY_TYPE_GTK      0x02
#define   WUD_GET_KEY_ORIGIN(_wValue_) (((_wValue_) & 0x40) >> 6)
#define    WUD_KEY_ORIGIN_HOST   0x00
#define    WUD_KEY_ORIGIN_DEVICE 0x01
#define  WUDESC_ENCRYPTION_TYPE	0x0e
#define  WUDESC_BOS		0x0f
#define  WUDESC_DEVICE_CAPABILITY 0x10
#define  WUDESC_WIRELESS_ENDPOINT_COMPANION 0x11
#define  UDESC_BOS		0x0f
#define  UDESC_DEVICE_CAPABILITY 0x10
#define  UDESC_CS_DEVICE	0x21	/* class specific */
#define  UDESC_CS_CONFIG	0x22
#define  UDESC_CS_STRING	0x23
#define  UDESC_CS_INTERFACE	0x24
#define  UDESC_CS_ENDPOINT	0x25
#define  UDESC_HUB		0x29
#define UR_SET_DESCRIPTOR	0x07
#define UR_GET_CONFIG		0x08
#define UR_SET_CONFIG		0x09
#define UR_GET_INTERFACE	0x0a
#define UR_SET_INTERFACE	0x0b
#define UR_SYNCH_FRAME		0x0c
#define WUR_SET_ENCRYPTION      0x0d
#define WUR_GET_ENCRYPTION	0x0e
#define WUR_SET_HANDSHAKE	0x0f
#define WUR_GET_HANDSHAKE	0x10
#define WUR_SET_CONNECTION	0x11
#define WUR_SET_SECURITY_DATA	0x12
#define WUR_GET_SECURITY_DATA	0x13
#define WUR_SET_WUSB_DATA	0x14
#define  WUDATA_DRPIE_INFO	0x01
#define  WUDATA_TRANSMIT_DATA	0x02
#define  WUDATA_TRANSMIT_PARAMS	0x03
#define  WUDATA_RECEIVE_PARAMS	0x04
#define  WUDATA_TRANSMIT_POWER	0x05
#define WUR_LOOPBACK_DATA_WRITE	0x15
#define WUR_LOOPBACK_DATA_READ	0x16
#define WUR_SET_INTERFACE_DS	0x17

/* Feature numbers */
#define UF_ENDPOINT_HALT	0
#define UF_DEVICE_REMOTE_WAKEUP	1
#define UF_TEST_MODE		2
#define UF_DEVICE_B_HNP_ENABLE	3
#define UF_DEVICE_A_HNP_SUPPORT	4
#define UF_DEVICE_A_ALT_HNP_SUPPORT 5
#define WUF_WUSB		3
#define  WUF_TX_DRPIE		0x0
#define  WUF_DEV_XMIT_PACKET	0x1
#define  WUF_COUNT_PACKETS	0x2
#define  WUF_CAPTURE_PACKETS	0x3
#define UF_FUNCTION_SUSPEND	0
#define UF_U1_ENABLE		48
#define UF_U2_ENABLE		49
#define UF_LTM_ENABLE		50

/* Class requests from the USB 2.0 hub spec, table 11-15 */
#define UCR_CLEAR_HUB_FEATURE		(0x2000 | UR_CLEAR_FEATURE)
#define UCR_CLEAR_PORT_FEATURE		(0x2300 | UR_CLEAR_FEATURE)
#define UCR_GET_HUB_DESCRIPTOR		(0xa000 | UR_GET_DESCRIPTOR)
#define UCR_GET_HUB_STATUS		(0xa000 | UR_GET_STATUS)
#define UCR_GET_PORT_STATUS		(0xa300 | UR_GET_STATUS)
#define UCR_SET_HUB_FEATURE		(0x2000 | UR_SET_FEATURE)
#define UCR_SET_PORT_FEATURE		(0x2300 | UR_SET_FEATURE)
#define UCR_SET_AND_TEST_PORT_FEATURE	(0xa300 | UR_SET_AND_TEST_FEATURE)








#define USB_MAX_STRING_LEN 128
#define USB_LANGUAGE_TABLE 0	/* # of the string language id table */

/* Hub specific request */
#define UR_GET_BUS_STATE	0x02
#define UR_CLEAR_TT_BUFFER	0x08
#define UR_RESET_TT		0x09
#define UR_GET_TT_STATE		0x0a
#define UR_STOP_TT		0x0b

/* Hub features */
#define UHF_C_HUB_LOCAL_POWER	0
#define UHF_C_HUB_OVER_CURRENT	1
#define UHF_PORT_CONNECTION	0
#define UHF_PORT_ENABLE		1
#define UHF_PORT_SUSPEND	2
#define UHF_PORT_OVER_CURRENT	3
#define UHF_PORT_RESET		4
#define UHF_PORT_L1		5
#define UHF_PORT_POWER		8
#define UHF_PORT_LOW_SPEED	9
#define UHF_PORT_HIGH_SPEED	10
#define UHF_C_PORT_CONNECTION	16
#define UHF_C_PORT_ENABLE	17
#define UHF_C_PORT_SUSPEND	18
#define UHF_C_PORT_OVER_CURRENT	19
#define UHF_C_PORT_RESET	20
#define UHF_C_PORT_L1		23
#define UHF_PORT_TEST		21
#define UHF_PORT_INDICATOR	22




typedef struct {
	uByte		bLength;
	uByte		bDescriptorType;
	uWord		bcdUSB;
	uByte		bDeviceClass;
	uByte		bDeviceSubClass;
	uByte		bDeviceProtocol;
	uByte		bMaxPacketSize0;
	uByte		bNumConfigurations;
	uByte		bReserved;
} UPACKED usb_device_qualifier_t;
#define USB_DEVICE_QUALIFIER_SIZE 10

typedef struct {
	uByte		bLength;
	uByte		bDescriptorType;
	uByte		bmAttributes;
#define UOTG_SRP	0x01
#define UOTG_HNP	0x02
} UPACKED usb_otg_descriptor_t;

/* OTG feature selectors */
#define UOTG_B_HNP_ENABLE	3
#define UOTG_A_HNP_SUPPORT	4
#define UOTG_A_ALT_HNP_SUPPORT	5

typedef struct {
	uWord		wStatus;
/* Device status flags */
#define UDS_SELF_POWERED		0x0001
#define UDS_REMOTE_WAKEUP		0x0002
/* Endpoint status flags */
#define UES_HALT			0x0001
} UPACKED usb_status_t;

typedef struct {
	uWord		wHubStatus;
#define UHS_LOCAL_POWER			0x0001
#define UHS_OVER_CURRENT		0x0002
	uWord		wHubChange;
} UPACKED usb_hub_status_t;

typedef struct {
	uWord		wPortStatus;
#define UPS_CURRENT_CONNECT_STATUS	0x0001
#define UPS_PORT_ENABLED		0x0002
#define UPS_SUSPEND			0x0004
#define UPS_OVERCURRENT_INDICATOR	0x0008
#define UPS_RESET			0x0010
#define UPS_PORT_POWER			0x0100
#define UPS_LOW_SPEED			0x0200
#define UPS_HIGH_SPEED			0x0400
#define UPS_PORT_TEST			0x0800
#define UPS_PORT_INDICATOR		0x1000
	uWord		wPortChange;
#define UPS_C_CONNECT_STATUS		0x0001
#define UPS_C_PORT_ENABLED		0x0002
#define UPS_C_SUSPEND			0x0004
#define UPS_C_OVERCURRENT_INDICATOR	0x0008
#define UPS_C_PORT_RESET		0x0010
} UPACKED usb_port_status_t;


/* Device class codes */
#define UDCLASS_IN_INTERFACE	0x00
#define UDCLASS_COMM		0x02
#define UDCLASS_HUB		0x09
#define  UDSUBCLASS_HUB		0x00
#define  UDPROTO_FSHUB		0x00
#define  UDPROTO_HSHUBSTT	0x01
#define  UDPROTO_HSHUBMTT	0x02
#define UDCLASS_DIAGNOSTIC	0xdc
#define UDCLASS_WIRELESS	0xe0
#define  UDSUBCLASS_RF		0x01
#define   UDPROTO_BLUETOOTH	0x01
#define UDCLASS_VENDOR		0xff

/* Interface class codes */
#define UICLASS_UNSPEC		0x00

#define UICLASS_AUDIO		0x01
#define  UISUBCLASS_AUDIOCONTROL	1
#define  UISUBCLASS_AUDIOSTREAM		2
#define  UISUBCLASS_MIDISTREAM		3

#define UICLASS_CDC		0x02 /* communication */
#define  UISUBCLASS_DIRECT_LINE_CONTROL_MODEL	1
#define  UISUBCLASS_ABSTRACT_CONTROL_MODEL	2
#define  UISUBCLASS_TELEPHONE_CONTROL_MODEL	3
#define  UISUBCLASS_MULTICHANNEL_CONTROL_MODEL	4
#define  UISUBCLASS_CAPI_CONTROLMODEL		5
#define  UISUBCLASS_ETHERNET_NETWORKING_CONTROL_MODEL 6
#define  UISUBCLASS_ATM_NETWORKING_CONTROL_MODEL 7
#define   UIPROTO_CDC_AT			1

#define UICLASS_HID		0x03
#define  UISUBCLASS_BOOT	1
#define  UIPROTO_BOOT_KEYBOARD	1

#define UICLASS_PHYSICAL	0x05

#define UICLASS_IMAGE		0x06

#define UICLASS_PRINTER		0x07
#define  UISUBCLASS_PRINTER	1
#define  UIPROTO_PRINTER_UNI	1
#define  UIPROTO_PRINTER_BI	2
#define  UIPROTO_PRINTER_1284	3

#define UICLASS_MASS		0x08
#define  UISUBCLASS_RBC		1
#define  UISUBCLASS_SFF8020I	2
#define  UISUBCLASS_QIC157	3
#define  UISUBCLASS_UFI		4
#define  UISUBCLASS_SFF8070I	5
#define  UISUBCLASS_SCSI	6
#define  UIPROTO_MASS_CBI_I	0
#define  UIPROTO_MASS_CBI	1
#define  UIPROTO_MASS_BBB_OLD	2	/* Not in the spec anymore */
#define  UIPROTO_MASS_BBB	80	/* 'P' for the Iomega Zip drive */

#define UICLASS_HUB		0x09
#define  UISUBCLASS_HUB		0
#define  UIPROTO_FSHUB		0
#define  UIPROTO_HSHUBSTT	0 /* Yes, same as previous */
#define  UIPROTO_HSHUBMTT	1

#define UICLASS_CDC_DATA	0x0a
#define  UISUBCLASS_DATA		0
#define   UIPROTO_DATA_ISDNBRI		0x30    /* Physical iface */
#define   UIPROTO_DATA_HDLC		0x31    /* HDLC */
#define   UIPROTO_DATA_TRANSPARENT	0x32    /* Transparent */
#define   UIPROTO_DATA_Q921M		0x50    /* Management for Q921 */
#define   UIPROTO_DATA_Q921		0x51    /* Data for Q921 */
#define   UIPROTO_DATA_Q921TM		0x52    /* TEI multiplexer for Q921 */
#define   UIPROTO_DATA_V42BIS		0x90    /* Data compression */
#define   UIPROTO_DATA_Q931		0x91    /* Euro-ISDN */
#define   UIPROTO_DATA_V120		0x92    /* V.24 rate adaption */
#define   UIPROTO_DATA_CAPI		0x93    /* CAPI 2.0 commands */
#define   UIPROTO_DATA_HOST_BASED	0xfd    /* Host based driver */
#define   UIPROTO_DATA_PUF		0xfe    /* see Prot. Unit Func. Desc.*/
#define   UIPROTO_DATA_VENDOR		0xff    /* Vendor specific */

#define UICLASS_SMARTCARD	0x0b

/*#define UICLASS_FIRM_UPD	0x0c*/

#define UICLASS_SECURITY	0x0d

#define UICLASS_DIAGNOSTIC	0xdc

#define UICLASS_WIRELESS	0xe0
#define  UISUBCLASS_RF			0x01
#define   UIPROTO_BLUETOOTH		0x01

#define UICLASS_APPL_SPEC	0xfe
#define  UISUBCLASS_FIRMWARE_DOWNLOAD	1
#define  UISUBCLASS_IRDA		2
#define  UIPROTO_IRDA			0

#define UICLASS_VENDOR		0xff

#define USB_HUB_MAX_DEPTH 5

/*
 * Minimum time a device needs to be powered down to go through
 * a power cycle.  XXX Are these time in the spec?
 */
#define USB_POWER_DOWN_TIME	200 /* ms */
#define USB_PORT_POWER_DOWN_TIME	100 /* ms */

#if 0
/* These are the values from the spec. */
#define USB_PORT_RESET_DELAY	10  /* ms */
#define USB_PORT_ROOT_RESET_DELAY 50  /* ms */
#define USB_PORT_RESET_RECOVERY	10  /* ms */
#define USB_PORT_POWERUP_DELAY	100 /* ms */
#define USB_SET_ADDRESS_SETTLE	2   /* ms */
#define USB_RESUME_DELAY	(20*5)  /* ms */
#define USB_RESUME_WAIT		10  /* ms */
#define USB_RESUME_RECOVERY	10  /* ms */
#define USB_EXTRA_POWER_UP_TIME	0   /* ms */
#else
/* Allow for marginal (i.e. non-conforming) devices. */
#define USB_PORT_RESET_DELAY	50  /* ms */
#define USB_PORT_ROOT_RESET_DELAY 250  /* ms */
#define USB_PORT_RESET_RECOVERY	250  /* ms */
#define USB_PORT_POWERUP_DELAY	300 /* ms */
#define USB_SET_ADDRESS_SETTLE	10  /* ms */
#define USB_RESUME_DELAY	(50*5)  /* ms */
#define USB_RESUME_WAIT		50  /* ms */
#define USB_RESUME_RECOVERY	50  /* ms */
#define USB_EXTRA_POWER_UP_TIME	20  /* ms */
#endif

#define USB_MIN_POWER		100 /* mA */
#define USB_MAX_POWER		500 /* mA */

#define USB_BUS_RESET_DELAY	100 /* ms XXX?*/

#define USB_UNCONFIG_NO 0
#define USB_UNCONFIG_INDEX (-1)

/*** ioctl() related stuff ***/

struct usb_ctl_request {
	int	ucr_addr;
	usb_device_request_t ucr_request;
	void	*ucr_data;
	int	ucr_flags;
#define USBD_SHORT_XFER_OK	0x04	/* allow short reads */
	int	ucr_actlen;		/* actual length transferred */
};

struct usb_alt_interface {
	int	uai_config_index;
	int	uai_interface_index;
	int	uai_alt_no;
};

#define USB_CURRENT_CONFIG_INDEX (-1)
#define USB_CURRENT_ALT_INDEX (-1)



struct usb_full_desc {
	int	ufd_config_index;
	u16 ufd_size;
	u8  *ufd_data;
};


struct usb_ctl_report_desc {
	int	ucrd_size;
	u8	ucrd_data[1024];	/* filled data size will vary */
};

typedef struct { u32 cookie; } usb_event_cookie_t;

#define USB_MAX_DEVNAMES 4
#define USB_MAX_DEVNAMELEN 16
struct usb_device_info {
	u8	udi_bus;
	u8	udi_addr;	/* device address */
	usb_event_cookie_t udi_cookie;
	char		udi_product[USB_MAX_STRING_LEN];
	char		udi_vendor[USB_MAX_STRING_LEN];
	char		udi_release[8];
	u16	udi_productNo;
	u16	udi_vendorNo;
	u16	udi_releaseNo;
	u8	udi_class;
	u8	udi_subclass;
	u8	udi_protocol;
	u8	udi_config;
	u8	udi_speed;
#if 0    
#define USB_SPEED_UNKNOWN	0
#define USB_SPEED_LOW		1
#define USB_SPEED_FULL		2
#define USB_SPEED_HIGH		3
#define USB_SPEED_VARIABLE	4
#define USB_SPEED_SUPER		5
#endif
	int		udi_power;	/* power consumption in mA, 0 if selfpowered */
	int		udi_nports;
	char		udi_devnames[USB_MAX_DEVNAMES][USB_MAX_DEVNAMELEN];
	u8	udi_ports[16];/* hub only: addresses of devices on ports */
#define USB_PORT_ENABLED 0xff
#define USB_PORT_SUSPENDED 0xfe
#define USB_PORT_POWERED 0xfd
#define USB_PORT_DISABLED 0xfc
};

struct usb_ctl_report {
	int	ucr_report;
	u8	ucr_data[1024];	/* filled data size will vary */
};

struct usb_device_stats {
	u32	uds_requests[4];	/* indexed by transfer type UE_* */
};

#define WUSB_MIN_IE			0x80
#define WUSB_WCTA_IE			0x80
#define WUSB_WCONNECTACK_IE		0x81
#define WUSB_WHOSTINFO_IE		0x82
#define  WUHI_GET_CA(_bmAttributes_) ((_bmAttributes_) & 0x3)
#define   WUHI_CA_RECONN		0x00
#define   WUHI_CA_LIMITED		0x01
#define   WUHI_CA_ALL			0x03
#define  WUHI_GET_MLSI(_bmAttributes_) (((_bmAttributes_) & 0x38) >> 3)
#define WUSB_WCHCHANGEANNOUNCE_IE	0x83
#define WUSB_WDEV_DISCONNECT_IE		0x84
#define WUSB_WHOST_DISCONNECT_IE	0x85
#define WUSB_WRELEASE_CHANNEL_IE	0x86
#define WUSB_WWORK_IE			0x87
#define WUSB_WCHANNEL_STOP_IE		0x88
#define WUSB_WDEV_KEEPALIVE_IE		0x89
#define WUSB_WISOCH_DISCARD_IE		0x8A
#define WUSB_WRESETDEVICE_IE		0x8B
#define WUSB_WXMIT_PACKET_ADJUST_IE	0x8C
#define WUSB_MAX_IE			0x8C

/* Device Notification Types */

#define WUSB_DN_MIN			0x01
#define WUSB_DN_CONNECT			0x01
# define WUSB_DA_OLDCONN	0x00
# define WUSB_DA_NEWCONN	0x01
# define WUSB_DA_SELF_BEACON	0x02
# define WUSB_DA_DIR_BEACON	0x04
# define WUSB_DA_NO_BEACON	0x06
#define WUSB_DN_DISCONNECT		0x02
#define WUSB_DN_EPRDY			0x03
#define WUSB_DN_MASAVAILCHANGED		0x04
#define WUSB_DN_REMOTEWAKEUP		0x05
#define WUSB_DN_SLEEP			0x06
#define WUSB_DN_ALIVE			0x07
#define WUSB_DN_MAX			0x07


/* WUSB Handshake Data.  Used during the SET/GET HANDSHAKE requests */
typedef struct wusb_hndshk_data {
	uByte bMessageNumber;
	uByte bStatus;
	uByte tTKID[3];
	uByte bReserved;
	uByte CDID[16];
	uByte Nonce[16];
	uByte MIC[8];
} UPACKED wusb_hndshk_data_t;
#define WUSB_HANDSHAKE_LEN_FOR_MIC	38

/* WUSB Connection Context */
typedef struct wusb_conn_context {
	uByte CHID [16];
	uByte CDID [16];
	uByte CK [16];
} UPACKED wusb_conn_context_t;

/* WUSB Security Descriptor */
typedef struct wusb_security_desc {
	uByte bLength;
	uByte bDescriptorType;
	uWord wTotalLength;
	uByte bNumEncryptionTypes;
} UPACKED wusb_security_desc_t;

/* WUSB Encryption Type Descriptor */
typedef struct wusb_encrypt_type_desc {
	uByte bLength;
	uByte bDescriptorType;

	uByte bEncryptionType;
#define WUETD_UNSECURE		0
#define WUETD_WIRED		1
#define WUETD_CCM_1		2
#define WUETD_RSA_1		3

	uByte bEncryptionValue;
	uByte bAuthKeyIndex;
} UPACKED wusb_encrypt_type_desc_t;

/* WUSB Key Descriptor */
typedef struct wusb_key_desc {
	uByte bLength;
	uByte bDescriptorType;
	uByte tTKID[3];
	uByte bReserved;
	uByte KeyData[1];	/* variable length */
} UPACKED wusb_key_desc_t;

/* WUSB BOS Descriptor (Binary device Object Store) */
typedef struct wusb_bos_desc {
	uByte bLength;
	uByte bDescriptorType;
	uWord wTotalLength;
	uByte bNumDeviceCaps;
} UPACKED wusb_bos_desc_t;

#define USB_DEVICE_CAPABILITY_20_EXTENSION	0x02
typedef struct usb_dev_cap_20_ext_desc {
	uByte bLength;
	uByte bDescriptorType;
	uByte bDevCapabilityType;
#define USB_20_EXT_LPM				0x02
	uDWord bmAttributes;
} UPACKED usb_dev_cap_20_ext_desc_t;

#define USB_DEVICE_CAPABILITY_SS_USB		0x03
typedef struct usb_dev_cap_ss_usb {
	uByte bLength;
	uByte bDescriptorType;
	uByte bDevCapabilityType;
#define USB_DC_SS_USB_LTM_CAPABLE		0x02
	uByte bmAttributes;
#define USB_DC_SS_USB_SPEED_SUPPORT_LOW		0x01
#define USB_DC_SS_USB_SPEED_SUPPORT_FULL	0x02
#define USB_DC_SS_USB_SPEED_SUPPORT_HIGH	0x04
#define USB_DC_SS_USB_SPEED_SUPPORT_SS		0x08
	uWord wSpeedsSupported;
	uByte bFunctionalitySupport;
	uByte bU1DevExitLat;
	uWord wU2DevExitLat;
} UPACKED usb_dev_cap_ss_usb_t;

#define USB_DEVICE_CAPABILITY_CONTAINER_ID	0x04
typedef struct usb_dev_cap_container_id {
	uByte bLength;
	uByte bDescriptorType;
	uByte bDevCapabilityType;
	uByte bReserved;
	uByte containerID[16];
} UPACKED usb_dev_cap_container_id_t;

/* Device Capability Type Codes */
#define WUSB_DEVICE_CAPABILITY_WIRELESS_USB 0x01

/* Device Capability Descriptor */
typedef struct wusb_dev_cap_desc {
	uByte bLength;
	uByte bDescriptorType;
	uByte bDevCapabilityType;
	uByte caps[1];	/* Variable length */
} UPACKED wusb_dev_cap_desc_t;

/* Device Capability Descriptor */
typedef struct wusb_dev_cap_uwb_desc {
	uByte bLength;
	uByte bDescriptorType;
	uByte bDevCapabilityType;
	uByte bmAttributes;
	uWord wPHYRates;	/* Bitmap */
	uByte bmTFITXPowerInfo;
	uByte bmFFITXPowerInfo;
	uWord bmBandGroup;
	uByte bReserved;
} UPACKED wusb_dev_cap_uwb_desc_t;

/* Wireless USB Endpoint Companion Descriptor */
typedef struct wusb_endpoint_companion_desc {
	uByte bLength;
	uByte bDescriptorType;
	uByte bMaxBurst;
	uByte bMaxSequence;
	uWord wMaxStreamDelay;
	uWord wOverTheAirPacketSize;
	uByte bOverTheAirInterval;
	uByte bmCompAttributes;
} UPACKED wusb_endpoint_companion_desc_t;

/* Wireless USB Numeric Association M1 Data Structure */
typedef struct wusb_m1_data {
	uByte version;
	uWord langId;
	uByte deviceFriendlyNameLength;
	uByte sha_256_m3[32];
	uByte deviceFriendlyName[256];
} UPACKED wusb_m1_data_t;

typedef struct wusb_m2_data {
	uByte version;
	uWord langId;
	uByte hostFriendlyNameLength;
	uByte pkh[384];
	uByte hostFriendlyName[256];
} UPACKED wusb_m2_data_t;

typedef struct wusb_m3_data {
	uByte pkd[384];
	uByte nd;
} UPACKED wusb_m3_data_t;

typedef struct wusb_m4_data {
	uDWord _attributeTypeIdAndLength_1;
	uWord  associationTypeId;

	uDWord _attributeTypeIdAndLength_2;
	uWord  associationSubTypeId;

	uDWord _attributeTypeIdAndLength_3;
	uDWord length;

	uDWord _attributeTypeIdAndLength_4;
	uDWord associationStatus;

	uDWord _attributeTypeIdAndLength_5;
	uByte  chid[16];

	uDWord _attributeTypeIdAndLength_6;
	uByte  cdid[16];

	uDWord _attributeTypeIdAndLength_7;
	uByte  bandGroups[2];
} UPACKED wusb_m4_data_t;



/* Original Host */
/* USB directions */
#define USB_DIR_OUT           0
#define USB_DIR_IN            0x80

/* Endpoints */
#define USB_ENDPOINT_NUMBER_MASK  0x0f	/* in bEndpointAddress */
#define USB_ENDPOINT_DIR_MASK     0x80

#define USB_ENDPOINT_XFERTYPE_MASK 0x03	/* in bmAttributes */
#define USB_ENDPOINT_XFER_CONTROL  0
#define USB_ENDPOINT_XFER_ISOC     1
#define USB_ENDPOINT_XFER_BULK     2
#define USB_ENDPOINT_XFER_INT      3

#define USB_ENDPOINT_HALT		   0	/* IN/OUT will STALL */





#if 1
#define UGETW(w) ((w)[0] | ((w)[1] << 8))
#define USETW(w,v) ((w)[0] = (u_int8_t)(v), (w)[1] = (u_int8_t)((v) >> 8))
#define UGETDW(w) ((w)[0] | ((w)[1] << 8) | ((w)[2] << 16) | ((w)[3] << 24))
#define USETDW(w,v) ((w)[0] = (u_int8_t)(v), \
		     (w)[1] = (u_int8_t)((v) >> 8), \
		     (w)[2] = (u_int8_t)((v) >> 16), \
		     (w)[3] = (u_int8_t)((v) >> 24))
#else
/*
 * On little-endian machines that can handle unanliged accesses
 * (e.g. i386) these macros can be replaced by the following.
 */
#define UGETW(w) (*(u_int16_t *)(w))
#define USETW(w,v) (*(u_int16_t *)(w) = (v))
#define UGETDW(w) (*(u_int32_t *)(w))
#define USETDW(w,v) (*(u_int32_t *)(w) = (v))
#endif

//#define UPACKED __attribute__((__packed__))






/* Everything is aribtrary */
#define USB_ALTSETTINGALLOC		4
#define USB_MAXALTSETTING		128	/* Hard limit */

#define USB_MAX_DEVICE			3       // at least 2: 1 for the root hub device, 1 for usb device
#define USB_MAXCONFIG			8
#define USB_MAXINTERFACES		8
#define USB_MAXENDPOINTS		16
#define USB_MAXCHILDREN			8	/* This is arbitrary */
#define USB_MAX_HUB			    2

#define USB_MAXIADS		        (USB_MAXINTERFACES/2)

#define USB_CNTL_TIMEOUT 5000 /* 100ms timeout */




/* Requests */
#define UR_GET_STATUS		0x00
#define  USTAT_STANDARD_STATUS  0x00
#define  WUSTAT_WUSB_FEATURE    0x01
#define  WUSTAT_CHANNEL_INFO    0x02
#define  WUSTAT_RECEIVED_DATA   0x03
#define  WUSTAT_MAS_AVAILABILITY 0x04
#define  WUSTAT_CURRENT_TRANSMIT_POWER 0x05
#define UR_CLEAR_FEATURE	0x01
#define UR_SET_FEATURE		0x03
#define UR_SET_AND_TEST_FEATURE 0x0c
#define UR_SET_ADDRESS		0x05
#define UR_GET_DESCRIPTOR	0x06
#define  UDESC_DEVICE		0x01
#define  UDESC_CONFIG		0x02
#define  UDESC_STRING		0x03
#define  UDESC_INTERFACE	0x04
#define  UDESC_ENDPOINT		0x05
#define  UDESC_SS_USB_COMPANION	0x30
#define  UDESC_DEVICE_QUALIFIER	0x06
#define  UDESC_OTHER_SPEED_CONFIGURATION 0x07
#define  UDESC_INTERFACE_POWER	0x08
#define  UDESC_OTG		0x09
#define  WUDESC_SECURITY	0x0c
#define  WUDESC_KEY		0x0d
#define   WUD_GET_KEY_INDEX(_wValue_) ((_wValue_) & 0xf)
#define   WUD_GET_KEY_TYPE(_wValue_) (((_wValue_) & 0x30) >> 4)
#define    WUD_KEY_TYPE_ASSOC    0x01
#define    WUD_KEY_TYPE_GTK      0x02
#define   WUD_GET_KEY_ORIGIN(_wValue_) (((_wValue_) & 0x40) >> 6)
#define    WUD_KEY_ORIGIN_HOST   0x00
#define    WUD_KEY_ORIGIN_DEVICE 0x01
#define  WUDESC_ENCRYPTION_TYPE	0x0e
#define  WUDESC_BOS		0x0f
#define  WUDESC_DEVICE_CAPABILITY 0x10
#define  WUDESC_WIRELESS_ENDPOINT_COMPANION 0x11
#define  UDESC_BOS		0x0f
#define  UDESC_DEVICE_CAPABILITY 0x10
#define  UDESC_CS_DEVICE	0x21	/* class specific */
#define  UDESC_CS_CONFIG	0x22
#define  UDESC_CS_STRING	0x23
#define  UDESC_CS_INTERFACE	0x24
#define  UDESC_CS_ENDPOINT	0x25
#define  UDESC_HUB		0x29
#define UR_SET_DESCRIPTOR	0x07
#define UR_GET_CONFIG		0x08
#define UR_SET_CONFIG		0x09
#define UR_GET_INTERFACE	0x0a
#define UR_SET_INTERFACE	0x0b
#define UR_SYNCH_FRAME		0x0c
#define WUR_SET_ENCRYPTION      0x0d
#define WUR_GET_ENCRYPTION	0x0e
#define WUR_SET_HANDSHAKE	0x0f
#define WUR_GET_HANDSHAKE	0x10
#define WUR_SET_CONNECTION	0x11
#define WUR_SET_SECURITY_DATA	0x12
#define WUR_GET_SECURITY_DATA	0x13
#define WUR_SET_WUSB_DATA	0x14
#define  WUDATA_DRPIE_INFO	0x01
#define  WUDATA_TRANSMIT_DATA	0x02
#define  WUDATA_TRANSMIT_PARAMS	0x03
#define  WUDATA_RECEIVE_PARAMS	0x04
#define  WUDATA_TRANSMIT_POWER	0x05
#define WUR_LOOPBACK_DATA_WRITE	0x15
#define WUR_LOOPBACK_DATA_READ	0x16
#define WUR_SET_INTERFACE_DS	0x17


/* Hub features */
#define UHF_C_HUB_LOCAL_POWER	0
#define UHF_C_HUB_OVER_CURRENT	1
#define UHF_PORT_CONNECTION	0
#define UHF_PORT_ENABLE		1
#define UHF_PORT_SUSPEND	2
#define UHF_PORT_OVER_CURRENT	3
#define UHF_PORT_RESET		4
#define UHF_PORT_L1		5
#define UHF_PORT_POWER		8
#define UHF_PORT_LOW_SPEED	9
#define UHF_PORT_HIGH_SPEED	10
#define UHF_C_PORT_CONNECTION	16
#define UHF_C_PORT_ENABLE	17
#define UHF_C_PORT_SUSPEND	18
#define UHF_C_PORT_OVER_CURRENT	19
#define UHF_C_PORT_RESET	20
#define UHF_C_PORT_L1		23
#define UHF_PORT_TEST		21
#define UHF_PORT_INDICATOR	22


typedef struct {
	uByte		bLength;
	uByte		bDescriptorType;
	uByte		bDescriptorSubtype;
} UPACKED usb_descriptor_t;

typedef struct {
	uByte		bLength;
	uByte		bDescriptorType;
} UPACKED usb_descriptor_header_t;

typedef struct {
	uByte		bLength;
	uByte		bDescriptorType;
	uWord		bcdUSB;
#define UD_USB_2_0		0x0200
#define UD_IS_USB2(d) (UGETW((d)->bcdUSB) >= UD_USB_2_0)
	uByte		bDeviceClass;
	uByte		bDeviceSubClass;
	uByte		bDeviceProtocol;
	uByte		bMaxPacketSize;
	/* The fields below are not part of the initial descriptor. */
	uWord		idVendor;
	uWord		idProduct;
	uWord		bcdDevice;
	uByte		iManufacturer;
	uByte		iProduct;
	uByte		iSerialNumber;
	uByte		bNumConfigurations;
} UPACKED usb_device_descriptor_t;

typedef struct {
	uByte		bLength;
	uByte		bDescriptorType;
	uWord		wTotalLength;
	uByte		bNumInterface;
	uByte		bConfigurationValue;
	uByte		iConfiguration;
#define UC_ATT_ONE		(1 << 7)	/* must be set */
#define UC_ATT_SELFPOWER	(1 << 6)	/* self powered */
#define UC_ATT_WAKEUP		(1 << 5)	/* can wakeup */
#define UC_ATT_BATTERY		(1 << 4)	/* battery powered */
	uByte		bmAttributes;
#define UC_BUS_POWERED		0x80
#define UC_SELF_POWERED		0x40
#define UC_REMOTE_WAKEUP	0x20
	uByte		bMaxPower; /* max current in 2 mA units */
#define UC_POWER_FACTOR 2
} UPACKED usb_config_descriptor_t;
#define USB_CONFIG_DESCRIPTOR_SIZE 9

typedef struct {
	uByte		bLength;
	uByte		bDescriptorType;
	uByte		bInterfaceNumber;
	uByte		bAlternateSetting;
	uByte		bNumEndpoints;
	uByte		bInterfaceClass;
	uByte		bInterfaceSubClass;
	uByte		bInterfaceProtocol;
	uByte		iInterface;
} UPACKED usb_interface_descriptor_t;
#define USB_INTERFACE_DESCRIPTOR_SIZE 9



/* String descriptor */


/* device request (setup) */
struct devrequest {
	unsigned char	requesttype;
	unsigned char	request;
	unsigned short	value;
	unsigned short	index;
	unsigned short	length;
} UPACKED;

/* All standard descriptors have these 2 fields in common */


/* Device descriptor */


/* Endpoint descriptor */


/* Interface descriptor */


/* Configuration descriptor information.. */


/* USB_DT_INTERFACE_ASSOCIATION: groups interfaces */

enum {
	/* Maximum packet size; encoded as 0,1,2,3 = 8,16,32,64 */
	PACKET_SIZE_8   = 0,
	PACKET_SIZE_16  = 1,
	PACKET_SIZE_32  = 2,
	PACKET_SIZE_64  = 3,
};




struct usb_host_endpoint {
	struct usb_endpoint_descriptor		desc;
	_list		urb_list;
	void				*hcpriv;
	unsigned char *extra;   /* Extra descriptors */
	int extralen;
	int enabled;
};

/* host-side wrapper for one interface setting's parsed descriptors */
struct usb_host_interface {
	struct usb_interface_descriptor	desc;

	int extralen;
	unsigned char *extra;   /* Extra descriptors */

	/* array of desc.bNumEndpoint endpoints associated with this
	 * interface setting.  these will be in no particular order.
	 */
	struct usb_host_endpoint *endpoint;

	char *string;		/* iInterface string, if present */
};

struct usb_interface {
	/* array of alternate settings for this interface,
	 * stored in no particular order */
	struct usb_host_interface *altsetting;

	struct usb_host_interface *cur_altsetting;	/* the currently
					 * active alternate setting */
	unsigned num_altsetting;	/* number of alternate settings */

	/* If there is an interface association descriptor then it will list
	 * the associated interfaces */
	struct usb_interface_assoc_descriptor *intf_assoc;

//	int minor;			/* minor number this interface is
//					 * bound to */
	unsigned ep_devs_created:1;	/* endpoint "devices" exist */
	unsigned unregistering:1;	/* unregistration is in progress */
	unsigned needs_altsetting0:1;	/* switch to altsetting 0 is pending */
	unsigned reset_running:1;
	unsigned resetting_device:1;	/* true: bandwidth alloc after reset */

	void *dev_prive_data;/* interface specific device info i.e struct v4l2_device pointer */

	void *driver;
    void *drv_priv;     // functional driver priv data
	void *usb_dev;
};
#define	to_usb_interface(d) container_of(d, struct usb_interface, usb_dev)

struct usb_interface_cache {
	unsigned num_altsetting;	/* number of alternate settings */

	/* variable-length array of alternate settings for this interface,
	 * stored in no particular order */
	struct usb_host_interface altsetting[0];
};

struct usb_host_config {
	struct usb_config_descriptor	desc;

	char *string;		/* iConfiguration string, if present */

	/* List of any Interface Association Descriptors in this
	 * configuration. */
	struct usb_interface_assoc_descriptor *intf_assoc[USB_MAXIADS];

	/* the interfaces associated with this configuration,
	 * stored in no particular order */
	struct usb_interface *interface[USB_MAXINTERFACES];

	/* Interface information available even when this is not the
	 * active configuration */
	struct usb_interface_cache *intf_cache[USB_MAXINTERFACES];

	unsigned char *extra;   /* Extra descriptors */
	int extralen;
};



struct usb_device {
	int		devnum;
	u32		route;
	enum usb_device_state	state;
	enum usb_device_speed	speed;

	unsigned int toggle[2];

	struct usb_device *parent;
	struct usb_device *children[USB_MAXCHILDREN];
    void *hcd;
	struct usb_host_endpoint ep0;

	struct usb_device_descriptor descriptor;
	struct usb_host_config *config;

	struct usb_host_config *actconfig;
	struct usb_host_endpoint *ep_in[16];
	struct usb_host_endpoint *ep_out[16];
	int configno;			/* selected config number */

	char **rawdescriptors;
    unsigned int rawdeslen[USB_MAXCONFIG];

//	unsigned short bus_mA;
	u8 portnum;
	u8 level;

	unsigned can_submit:1;
	unsigned persist_enabled:1;
	unsigned have_langid:1;
	unsigned authorized:1;
	unsigned authenticated:1;
	int string_langid;

	/* static strings from the device */
//	char *product;
//	char *manufacturer;
//	char *serial;

	int maxchild;

	u32 quirks;
	atomic_t urbnum;

//	unsigned long active_duration;

	char mf[32];			/* manufacturer */
	char prod[32];		/* product */
	char serial[32];		/* serial number */

    _mutex Mutex;
};

/*
 * urb->transfer_flags:
 *
 * Note: URB_DIR_IN/OUT is automatically set in usb_submit_urb().
 */
#define URB_SHORT_NOT_OK	0x0001	/* report short reads as errors */
#define URB_ISO_ASAP		0x0002	/* iso-only, urb->start_frame
					 * ignored */
#define URB_NO_TRANSFER_DMA_MAP	0x0004	/* urb->transfer_dma valid on submit */
#define URB_NO_FSBR		0x0020	/* UHCI-specific */
#define URB_ZERO_PACKET		0x0040	/* Finish bulk OUT with short packet */
#define URB_NO_INTERRUPT	0x0080	/* HINT: no non-error interrupt
					 * needed */
#define URB_FREE_BUFFER		0x0100	/* Free transfer buffer with the URB */

/* The following flags are used internally by usbcore and HCDs */
#define URB_DIR_IN		0x0200	/* Transfer from device to host */
#define URB_DIR_OUT		0
#define URB_DIR_MASK		URB_DIR_IN

#define URB_DMA_MAP_SINGLE	0x00010000	/* Non-scatter-gather mapping */
#define URB_DMA_MAP_PAGE	0x00020000	/* HCD-unsupported S-G */
#define URB_DMA_MAP_SG		0x00040000	/* HCD-supported S-G */
#define URB_MAP_LOCAL		0x00080000	/* HCD-local-memory mapping */
#define URB_SETUP_MAP_SINGLE	0x00100000	/* Setup packet DMA mapped */
#define URB_SETUP_MAP_LOCAL	0x00200000	/* HCD-local setup packet */
#define URB_DMA_SG_COMBINED	0x00400000	/* S-G entries were combined */
#define URB_ALIGNED_TEMP_BUFFER	0x00800000	/* Temp buffer was alloc'd */

struct usb_iso_packet_descriptor {
	unsigned int offset;
	unsigned int length;		/* expected length */
	unsigned int actual_length;
	int status;
};

struct urb;

typedef void (*usb_complete_t)(struct urb *);

struct urb {
	/* private: usb core and host controller only fields in the urb */
	void *hcpriv;			/* private data for host controller */
	atomic_t use_count;		/* concurrent submissions counter */
	atomic_t reject;		/* submissions will fail */
	int unlinked;			/* unlink error code */

	/* public: documented fields in the urb that can be used by drivers */
	_list urb_list;	/* list head for use by the urb's
					 * current owner */

	struct usb_device *dev;		/* (in) pointer to associated device */
	struct usb_host_endpoint *ep;	/* (internal) pointer to endpoint */
	unsigned int pipe;		/* (in) pipe information */
	unsigned int stream_id;		/* (in) stream ID */
	int status;			/* (return) non-ISO status */
	unsigned int transfer_flags;	/* (in) URB_SHORT_NOT_OK | ...*/
	void *transfer_buffer;		/* (in) associated data buffer */
	dma_addr_t transfer_dma;	/* (in) dma addr for transfer_buffer */
	u32 transfer_buffer_length;	/* (in) data buffer length */
	u32 actual_length;		/* (return) actual transfer length */
	unsigned char *setup_packet;	/* (in) setup packet (control only) */

	int start_frame;		/* (modify) start frame (ISO) */
	int number_of_packets;		/* (in) number of ISO packets */
	int interval;			/* (modify) transfer interval
					 * (INT/ISO) */
	int error_count;		/* (return) number of ISO errors */
	void *context;			/* (in) context for completion */
    _mutex Mutex;   // mutext for atomic or link-list operation
	usb_complete_t complete;	/* (in) completion routine */
    unsigned int iso_packets;
	struct usb_iso_packet_descriptor iso_frame_desc[0];
					/* (in) ISO ONLY */
};

struct usb_hcd {

	/*
	 * housekeeping
	 */
	void *rhdev;        /* self usb_dev */
	const char *product_desc;	/* product/vendor string */
	int	speed;		/* Speed for this roothub.
						 * May be different from
						 * hcd->driver->flags & HCD_MASK
						 */

	struct urb *status_urb;	/* the current status urb */

	/*
	 * hardware info/state
	 */



	/* Flags that need to be manipulated atomically because they can
	 * change while the host controller is running.  Always use
	 * set_bit() or clear_bit() to change their values.
	 */
	unsigned long flags;

	/* Flags that get set only during HCD registration or removal. */
	unsigned rh_registered:1;/* is root hub registered? */
	unsigned rh_pollable:1;	/* may we poll the root hub? */
	unsigned msix_enabled:1;	/* driver has MSI-X enabled? */

	/* The next flag is a stopgap, to be removed when all the HCDs
	 * support the new root-hub polling mechanism. */
	unsigned uses_new_polling:1;
	unsigned authorized_default:1;
	unsigned has_tt:1;	/* Integrated TT in root hub */


	/* bandwidth_mutex should be taken before adding or removing
	 * any new bus bandwidth constraints:
	 *   1. Before adding a configuration for a new device.
	 *   2. Before removing the configuration to put the device into
	 *      the addressed state.
	 *   3. Before selecting a different configuration.
	 *   4. Before selecting an alternate interface setting.
	 *
	 * bandwidth_mutex should be dropped after a successful control message
	 * to the device, or resetting the bandwidth after a failed attempt.
	 */
	_mutex		bandwidth_mutex;

	int	state;
	 
    int devnum_next;        /* Next open device number in
                             * round-robin allocation */

    _mutex hcd_urb_list_lock;
    _mutex hcd_urb_unlink_lock;
    _mutex hcd_root_hub_lock;   // mutext for atomic or link-list operation

	/* The HC driver's private data is stored at the end of
	 * this structure.
	 */
#ifdef __ICCARM__
    #pragma pack(64)
    unsigned long hcd_priv[0];
#elif defined (__GNUC__)
	unsigned long hcd_priv[0];
	__attribute__ ((aligned(sizeof(s64))));
#endif
};


#define le16_to_cpu rtk_le16_to_cpu
#define cpu_to_le16 rtk_cpu_to_le16

#if 0
/*
 * Calling this entity a "pipe" is glorifying it. A USB pipe
 * is something embarrassingly simple: it basically consists
 * of the following information:
 *  - device number (7 bits)
 *  - endpoint number (4 bits)
 *  - current Data0/1 state (1 bit)
 *  - direction (1 bit)
 *  - speed (2 bits)
 *  - max packet size (2 bits: 8, 16, 32 or 64)
 *  - pipe type (2 bits: control, interrupt, bulk, isochronous)
 *
 * That's 18 bits. Really. Nothing more. And the USB people have
 * documented these eighteen bits as some kind of glorious
 * virtual data structure.
 *
 * Let's not fall in that trap. We'll just encode it as a simple
 * unsigned int. The encoding is:
 *
 *  - max size:		bits 0-1	(00 = 8, 01 = 16, 10 = 32, 11 = 64)
 *  - direction:	bit 7		(0 = Host-to-Device [Out],
 *					(1 = Device-to-Host [In])
 *  - device:		bits 8-14
 *  - endpoint:		bits 15-18
 *  - Data0/1:		bit 19
 *  - speed:		bit 26		(0 = Full, 1 = Low Speed, 2 = High)
 *  - pipe type:	bits 30-31	(00 = isochronous, 01 = interrupt,
 *					 10 = control, 11 = bulk)
 *
 * Why? Because it's arbitrary, and whatever encoding we select is really
 * up to us. This one happens to share a lot of bit positions with the UHCI
 * specification, so that much of the uhci driver can just mask the bits
 * appropriately.
 */
/* Create various pipes... */
#define create_pipe(dev,endpoint) \
		(((dev)->devnum << 8) | (endpoint << 15) | \
		((dev)->speed << 26) | (dev)->maxpacketsize)
#define default_pipe(dev) ((dev)->speed << 26)

#define usb_sndctrlpipe(dev, endpoint)	((PIPE_CONTROL << 30) | \
					 create_pipe(dev, endpoint))
#define usb_rcvctrlpipe(dev, endpoint)	((PIPE_CONTROL << 30) | \
					 create_pipe(dev, endpoint) | \
					 USB_DIR_IN)
#define usb_sndisocpipe(dev, endpoint)	((PIPE_ISOCHRONOUS << 30) | \
					 create_pipe(dev, endpoint))
#define usb_rcvisocpipe(dev, endpoint)	((PIPE_ISOCHRONOUS << 30) | \
					 create_pipe(dev, endpoint) | \
					 USB_DIR_IN)
#define usb_sndbulkpipe(dev, endpoint)	((PIPE_BULK << 30) | \
					 create_pipe(dev, endpoint))
#define usb_rcvbulkpipe(dev, endpoint)	((PIPE_BULK << 30) | \
					 create_pipe(dev, endpoint) | \
					 USB_DIR_IN)
#define usb_sndintpipe(dev, endpoint)	((PIPE_INTERRUPT << 30) | \
					 create_pipe(dev, endpoint))
#define usb_rcvintpipe(dev, endpoint)	((PIPE_INTERRUPT << 30) | \
					 create_pipe(dev, endpoint) | \
					 USB_DIR_IN)
#define usb_snddefctrl(dev)		((PIPE_CONTROL << 30) | \
					 default_pipe(dev))
#define usb_rcvdefctrl(dev)		((PIPE_CONTROL << 30) | \
					 default_pipe(dev) | \
					 USB_DIR_IN)

/* The D0/D1 toggle bits */
#define usb_gettoggle(dev, ep, out) (((dev)->toggle[out] >> ep) & 1)
#define usb_dotoggle(dev, ep, out)  ((dev)->toggle[out] ^= (1 << ep))
#define usb_settoggle(dev, ep, out, bit) ((dev)->toggle[out] = \
						((dev)->toggle[out] & \
						 ~(1 << ep)) | ((bit) << ep))

/* Endpoint halt control/status */
#define usb_endpoint_out(ep_dir)	(((ep_dir >> 7) & 1) ^ 1)
#define usb_endpoint_halt(dev, ep, out) ((dev)->halted[out] |= (1 << (ep)))
#define usb_endpoint_running(dev, ep, out) ((dev)->halted[out] &= ~(1 << (ep)))
#define usb_endpoint_halted(dev, ep, out) ((dev)->halted[out] & (1 << (ep)))

#define usb_packetid(pipe)	(((pipe) & USB_DIR_IN) ? USB_PID_IN : \
				 USB_PID_OUT)

#define usb_pipeout(pipe)	((((pipe) >> 7) & 1) ^ 1)
#define usb_pipein(pipe)	(((pipe) >> 7) & 1)
#define usb_pipedevice(pipe)	(((pipe) >> 8) & 0x7f)
#define usb_pipe_endpdev(pipe)	(((pipe) >> 8) & 0x7ff)
#define usb_pipeendpoint(pipe)	(((pipe) >> 15) & 0xf)
#define usb_pipedata(pipe)	(((pipe) >> 19) & 1)
#define usb_pipespeed(pipe)	(((pipe) >> 26) & 3)
#define usb_pipeslow(pipe)	(usb_pipespeed(pipe) == USB_SPEED_LOW)
#define usb_pipetype(pipe)	(((pipe) >> 30) & 3)
#define usb_pipeisoc(pipe)	(usb_pipetype((pipe)) == PIPE_ISOCHRONOUS)
#define usb_pipeint(pipe)	(usb_pipetype((pipe)) == PIPE_INTERRUPT)
#define usb_pipecontrol(pipe)	(usb_pipetype((pipe)) == PIPE_CONTROL)
#define usb_pipebulk(pipe)	(usb_pipetype((pipe)) == PIPE_BULK)

#else
/* ----------------------------------------------------------------------- */

/*
 * For various legacy reasons, Linux has a small cookie that's paired with
 * a struct usb_device to identify an endpoint queue.  Queue characteristics
 * are defined by the endpoint's descriptor.  This cookie is called a "pipe",
 * an unsigned int encoded as:
 *
 *  - direction:	bit 7		(0 = Host-to-Device [Out],
 *					 1 = Device-to-Host [In] ...
 *					like endpoint bEndpointAddress)
 *  - device address:	bits 8-14       ... bit positions known to uhci-hcd
 *  - endpoint:		bits 15-18      ... bit positions known to uhci-hcd
 *  - pipe type:	bits 30-31	(00 = isochronous, 01 = interrupt,
 *					 10 = control, 11 = bulk)
 *
 * Given the device address and endpoint descriptor, pipes are redundant.
 */

/* NOTE:  these are not the standard USB_ENDPOINT_XFER_* values!! */
/* (yet ... they're the values used by usbfs) */

#define usb_pipein(pipe)	(((pipe) & USB_DIR_IN) >> 7)
#define usb_pipeout(pipe)	(!usb_pipein(pipe))

#define usb_pipedevice(pipe)	(((pipe) >> 8) & 0x7f)
#define usb_pipeendpoint(pipe)	(((pipe) >> 15) & 0xf)

#define usb_pipetype(pipe)	(((pipe) >> 30) & 3)
#define usb_pipeisoc(pipe)	(usb_pipetype((pipe)) == PIPE_ISOCHRONOUS)
#define usb_pipeint(pipe)	(usb_pipetype((pipe)) == PIPE_INTERRUPT)
#define usb_pipecontrol(pipe)	(usb_pipetype((pipe)) == PIPE_CONTROL)
#define usb_pipebulk(pipe)	(usb_pipetype((pipe)) == PIPE_BULK)

static inline unsigned int __create_pipe(struct usb_device *dev,
		unsigned int endpoint)
{
	return (dev->devnum << 8) | (endpoint << 15);
}

#define usb_sndaddr0pipe()	(PIPE_CONTROL << 30)
#define usb_rcvaddr0pipe()	((PIPE_CONTROL << 30) | USB_DIR_IN)

/* Create various pipes... */
#define usb_sndctrlpipe(dev, endpoint)	\
	((PIPE_CONTROL << 30) | __create_pipe(dev, endpoint))
#define usb_rcvctrlpipe(dev, endpoint)	\
	((PIPE_CONTROL << 30) | __create_pipe(dev, endpoint) | USB_DIR_IN)
#define usb_sndisocpipe(dev, endpoint)	\
	((PIPE_ISOCHRONOUS << 30) | __create_pipe(dev, endpoint))
#define usb_rcvisocpipe(dev, endpoint)	\
	((PIPE_ISOCHRONOUS << 30) | __create_pipe(dev, endpoint) | USB_DIR_IN)
#define usb_sndbulkpipe(dev, endpoint)	\
	((PIPE_BULK << 30) | __create_pipe(dev, endpoint))
#define usb_rcvbulkpipe(dev, endpoint)	\
	((PIPE_BULK << 30) | __create_pipe(dev, endpoint) | USB_DIR_IN)
#define usb_sndintpipe(dev, endpoint)	\
	((PIPE_INTERRUPT << 30) | __create_pipe(dev, endpoint))
#define usb_rcvintpipe(dev, endpoint)	\
	((PIPE_INTERRUPT << 30) | __create_pipe(dev, endpoint) | USB_DIR_IN)

static inline struct usb_host_endpoint *
usb_pipe_endpoint(struct usb_device *dev, unsigned int pipe)
{
	struct usb_host_endpoint **eps;
	eps = usb_pipein(pipe) ? dev->ep_in : dev->ep_out;
	return eps[usb_pipeendpoint(pipe)];
}

/* The D0/D1 toggle bits ... USE WITH CAUTION (they're almost hcd-internal) */
#define usb_gettoggle(dev, ep, out) (((dev)->toggle[out] >> (ep)) & 1)
#define	usb_dotoggle(dev, ep, out)  ((dev)->toggle[out] ^= (1 << (ep)))
#define usb_settoggle(dev, ep, out, bit) \
		((dev)->toggle[out] = ((dev)->toggle[out] & ~(1 << (ep))) | \
		 ((bit) << (ep)))
#endif

/*************************************************************************
 * Hub Stuff
 */
struct usb_port_status {
	unsigned short wPortStatus;
	unsigned short wPortChange;
} UPACKED;

struct usb_hub_status {
	unsigned short wHubStatus;
	unsigned short wHubChange;
}UPACKED;


/* Hub descriptor */
struct usb_hub_descriptor {
	unsigned char  bLength;
	unsigned char  bDescriptorType;
	unsigned char  bNbrPorts;
	unsigned short wHubCharacteristics;
	unsigned char  bPwrOn2PwrGood;
	unsigned char  bHubContrCurrent;
	unsigned char  DeviceRemovable[(USB_MAXCHILDREN+1+7)/8];
	unsigned char  PortPowerCtrlMask[(USB_MAXCHILDREN+1+7)/8];
	/* DeviceRemovable and PortPwrCtrlMask want to be variable-length
	   bitmaps that hold max 255 entries. (bit0 is ignored) */
}UPACKED;


struct usb_hub_device {
	struct usb_device *pusb_dev;
	struct usb_hub_descriptor desc;
};

/*-------------------------------------------------------------------------*/

/**
 * usb_endpoint_num - get the endpoint's number
 * @epd: endpoint to be checked
 *
 * Returns @epd's number: 0 to 15.
 */
static inline int usb_endpoint_num(const struct usb_endpoint_descriptor *epd)
{
	return epd->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
}

/**
 * usb_endpoint_type - get the endpoint's transfer type
 * @epd: endpoint to be checked
 *
 * Returns one of USB_ENDPOINT_XFER_{CONTROL, ISOC, BULK, INT} according
 * to @epd's transfer type.
 */
static inline int usb_endpoint_type(const struct usb_endpoint_descriptor *epd)
{
	return epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK;
}

/**
 * usb_endpoint_dir_in - check if the endpoint has IN direction
 * @epd: endpoint to be checked
 *
 * Returns true if the endpoint is of type IN, otherwise it returns false.
 */
static inline int usb_endpoint_dir_in(const struct usb_endpoint_descriptor *epd)
{
	return ((epd->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_IN);
}

/**
 * usb_endpoint_dir_out - check if the endpoint has OUT direction
 * @epd: endpoint to be checked
 *
 * Returns true if the endpoint is of type OUT, otherwise it returns false.
 */
static inline int usb_endpoint_dir_out(
				const struct usb_endpoint_descriptor *epd)
{
	return ((epd->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_OUT);
}

#define usb_endpoint_out(ep_dir)	(!((ep_dir) & USB_DIR_IN))

/**
 * usb_endpoint_xfer_bulk - check if the endpoint has bulk transfer type
 * @epd: endpoint to be checked
 *
 * Returns true if the endpoint is of type bulk, otherwise it returns false.
 */
static inline int usb_endpoint_xfer_bulk(
				const struct usb_endpoint_descriptor *epd)
{
	return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		USB_ENDPOINT_XFER_BULK);
}

/**
 * usb_endpoint_xfer_control - check if the endpoint has control transfer type
 * @epd: endpoint to be checked
 *
 * Returns true if the endpoint is of type control, otherwise it returns false.
 */
static inline int usb_endpoint_xfer_control(
				const struct usb_endpoint_descriptor *epd)
{
	return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		USB_ENDPOINT_XFER_CONTROL);
}

/**
 * usb_endpoint_xfer_int - check if the endpoint has interrupt transfer type
 * @epd: endpoint to be checked
 *
 * Returns true if the endpoint is of type interrupt, otherwise it returns
 * false.
 */
static inline int usb_endpoint_xfer_int(
				const struct usb_endpoint_descriptor *epd)
{
	return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		USB_ENDPOINT_XFER_INT);
}

/**
 * usb_endpoint_xfer_isoc - check if the endpoint has isochronous transfer type
 * @epd: endpoint to be checked
 *
 * Returns true if the endpoint is of type isochronous, otherwise it returns
 * false.
 */
static inline int usb_endpoint_xfer_isoc(
				const struct usb_endpoint_descriptor *epd)
{
	return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		USB_ENDPOINT_XFER_ISOC);
}

/**
 * usb_endpoint_is_bulk_in - check if the endpoint is bulk IN
 * @epd: endpoint to be checked
 *
 * Returns true if the endpoint has bulk transfer type and IN direction,
 * otherwise it returns false.
 */
static inline int usb_endpoint_is_bulk_in(
				const struct usb_endpoint_descriptor *epd)
{
	return usb_endpoint_xfer_bulk(epd) && usb_endpoint_dir_in(epd);
}

/**
 * usb_endpoint_is_bulk_out - check if the endpoint is bulk OUT
 * @epd: endpoint to be checked
 *
 * Returns true if the endpoint has bulk transfer type and OUT direction,
 * otherwise it returns false.
 */
static inline int usb_endpoint_is_bulk_out(
				const struct usb_endpoint_descriptor *epd)
{
	return usb_endpoint_xfer_bulk(epd) && usb_endpoint_dir_out(epd);
}

/**
 * usb_endpoint_is_int_in - check if the endpoint is interrupt IN
 * @epd: endpoint to be checked
 *
 * Returns true if the endpoint has interrupt transfer type and IN direction,
 * otherwise it returns false.
 */
static inline int usb_endpoint_is_int_in(
				const struct usb_endpoint_descriptor *epd)
{
	return usb_endpoint_xfer_int(epd) && usb_endpoint_dir_in(epd);
}

/**
 * usb_endpoint_is_int_out - check if the endpoint is interrupt OUT
 * @epd: endpoint to be checked
 *
 * Returns true if the endpoint has interrupt transfer type and OUT direction,
 * otherwise it returns false.
 */
static inline int usb_endpoint_is_int_out(
				const struct usb_endpoint_descriptor *epd)
{
	return usb_endpoint_xfer_int(epd) && usb_endpoint_dir_out(epd);
}

/**
 * usb_endpoint_is_isoc_in - check if the endpoint is isochronous IN
 * @epd: endpoint to be checked
 *
 * Returns true if the endpoint has isochronous transfer type and IN direction,
 * otherwise it returns false.
 */
static inline int usb_endpoint_is_isoc_in(
				const struct usb_endpoint_descriptor *epd)
{
	return usb_endpoint_xfer_isoc(epd) && usb_endpoint_dir_in(epd);
}

/**
 * usb_endpoint_is_isoc_out - check if the endpoint is isochronous OUT
 * @epd: endpoint to be checked
 *
 * Returns true if the endpoint has isochronous transfer type and OUT direction,
 * otherwise it returns false.
 */
static inline int usb_endpoint_is_isoc_out(
				const struct usb_endpoint_descriptor *epd)
{
	return usb_endpoint_xfer_isoc(epd) && usb_endpoint_dir_out(epd);
}

/**
 * usb_endpoint_maxp - get endpoint's max packet size
 * @epd: endpoint to be checked
 *
 * Returns @epd's max packet
 */
static inline int usb_endpoint_maxp(const struct usb_endpoint_descriptor *epd)
{
	return rtk_le16_to_cpu(epd->wMaxPacketSize);
}

/**
 * usb_urb_dir_in - check if an URB describes an IN transfer
 * @urb: URB to be checked
 *
 * Returns 1 if @urb describes an IN transfer (device-to-host),
 * otherwise 0.
 */
static inline int usb_urb_dir_in(struct urb *urb)
{
	return (urb->transfer_flags & URB_DIR_MASK) == URB_DIR_IN;
}

/**
 * usb_urb_dir_out - check if an URB describes an OUT transfer
 * @urb: URB to be checked
 *
 * Returns 1 if @urb describes an OUT transfer (host-to-device),
 * otherwise 0.
 */
static inline int usb_urb_dir_out(struct urb *urb)
{
	return (urb->transfer_flags & URB_DIR_MASK) == URB_DIR_OUT;
}

/**
 * usb_get_dev - increments the reference count of the usb device structure
 * @dev: the device being referenced
 *
 * Each live reference to a device should be refcounted.
 *
 * Drivers for USB interfaces should normally record such references in
 * their probe() methods, when they bind to an interface, and release
 * them by calling usb_put_dev(), in their disconnect() methods.
 *
 * A pointer to the device with the incremented reference counter is returned.
 */
static inline void *usb_get_dev(struct usb_device *dev)
{
	return dev;
}

/**
 * usb_put_dev - release a use of the usb device structure
 * @dev: device that's been disconnected
 *
 * Must be called when a user of a device is finished with it.  When the last
 * user of the device calls this function, the memory of the device is freed.
 */
static inline void usb_put_dev(struct usb_device *dev)
{
}

/**
 * usb_get_intf - increments the reference count of the usb interface structure
 * @intf: the interface being referenced
 *
 * Each live reference to a interface must be refcounted.
 *
 * Drivers for USB interfaces should normally record such references in
 * their probe() methods, when they bind to an interface, and release
 * them by calling usb_put_intf(), in their disconnect() methods.
 *
 * A pointer to the interface with the incremented reference counter is
 * returned.
 */
static inline void *usb_get_intf(struct usb_interface *intf)
{
	return intf;
}

/**
 * usb_put_intf - release a use of the usb interface structure
 * @intf: interface that's been decremented
 *
 * Must be called when a user of an interface is finished with it.  When the
 * last user of the interface calls this function, the memory of the interface
 * is freed.
 */
static inline void usb_put_intf(struct usb_interface *intf)
{
}

static inline void *usb_get_intfdata(struct usb_interface *intf)
{
    return (intf->drv_priv);
}

static inline void usb_set_intfdata(struct usb_interface *intf, void *data)
{
    intf->drv_priv = data;
}

static inline struct usb_device *interface_to_usbdev(struct usb_interface *intf)
{
	return intf->usb_dev;
}

static inline struct usb_driver *interface_to_usbdri(struct usb_interface *intf)
{
	return intf->driver;
}

/*-------------------------------------------------------------------------*/

/*=====================================================*/


#ifdef DWC_HOST_ONLY
typedef struct {
	uByte		bLength;
	uByte		bDescriptorType;
	uByte		bEndpointAddress;
#define UE_GET_DIR(a)	((a) & 0x80)
#define UE_SET_DIR(a,d)	((a) | (((d)&1) << 7))
#define UE_DIR_IN	0x01
#define UE_DIR_OUT	0x00
#define UE_ADDR		0x0f
#define UE_GET_ADDR(a)	((a) & UE_ADDR)
	uByte		bmAttributes;
#define UE_XFERTYPE	0x03
#define  UE_CONTROL	0x00
#define  UE_ISOCHRONOUS	0x01
#define  UE_BULK	0x02
#define  UE_INTERRUPT	0x03
#define UE_GET_XFERTYPE(a)	((a) & UE_XFERTYPE)
#if 0
#define UE_ISO_TYPE	0x0c
#define  UE_ISO_ASYNC	0x04
#define  UE_ISO_ADAPT	0x08
#define  UE_ISO_SYNC	0x0c
#define UE_GET_ISO_TYPE(a)	((a) & UE_ISO_TYPE)
#endif
	uWord		wMaxPacketSize;
	uByte		bInterval;
} usb_endpoint_descriptor_t;//UPACKED usb_endpoint_descriptor_t;
#define USB_ENDPOINT_DESCRIPTOR_SIZE 7
#endif
#ifdef DWC_DEVICE_ONLY
typedef struct {
	uByte		bLength;
	uByte		bDescriptorType;
	uByte		bEndpointAddress;
#define UE_GET_DIR(a)	((a) & 0x80)
#define UE_SET_DIR(a,d)	((a) | (((d)&1) << 7))
#define UE_DIR_IN	0x80
#define UE_DIR_OUT	0x00
#define UE_ADDR		0x0f
#define UE_GET_ADDR(a)	((a) & UE_ADDR)
	uByte		bmAttributes;
#define UE_XFERTYPE	0x03
#define  UE_CONTROL	0x00
#define  UE_ISOCHRONOUS	0x01
#define  UE_BULK	0x02
#define  UE_INTERRUPT	0x03
#define UE_GET_XFERTYPE(a)	((a) & UE_XFERTYPE)
#define UE_ISO_TYPE	0x0c
#define  UE_ISO_ASYNC	0x04
#define  UE_ISO_ADAPT	0x08
#define  UE_ISO_SYNC	0x0c
#define UE_GET_ISO_TYPE(a)	((a) & UE_ISO_TYPE)
	uWord		wMaxPacketSize;
	uByte		bInterval;
} UPACKED usb_endpoint_descriptor_t;
#endif

typedef struct ss_endpoint_companion_descriptor {
	uByte bLength;
	uByte bDescriptorType;
	uByte bMaxBurst;
#define USSE_GET_MAX_STREAMS(a)		((a) & 0x1f)
#define USSE_SET_MAX_STREAMS(a, b)	((a) | ((b) & 0x1f))
#define USSE_GET_MAX_PACKET_NUM(a)	((a) & 0x03)
#define USSE_SET_MAX_PACKET_NUM(a, b)	((a) | ((b) & 0x03))
	uByte bmAttributes;
	uWord wBytesPerInterval;
} UPACKED ss_endpoint_companion_descriptor_t;
#define USB_SS_ENDPOINT_COMPANION_DESCRIPTOR_SIZE 6

typedef struct {
	uByte		bLength;
	uByte		bDescriptorType;
	uWord		bString[127];
} UPACKED usb_string_descriptor_t;
#define USB_MAX_STRING_LEN 128
#define USB_LANGUAGE_TABLE 0	/* # of the string language id table */


/* USB_DT_DEVICE_QUALIFIER: Device Qualifier descriptor */

/* USB_DT_OTG (from OTG 1.0a supplement) */

/* from usb_otg_descriptor.bmAttributes */
#define USB_OTG_SRP		(1 << 0)
#define USB_OTG_HNP		(1 << 1)	/* swap host/device roles */


extern int usb_init(void);
extern int usb_stop(void);
extern void usb_disable_asynch(int disable);
extern unsigned short usb_maxpacket(struct usb_device *udev, int pipe, int is_out);
extern int usb_set_interface(struct usb_device *dev, int interface, int alternate);
extern int usb_set_configuration(struct usb_device *dev, int configuration);

extern int usb_set_protocol(struct usb_device *dev, int ifnum, int protocol);
extern int usb_set_idle(struct usb_device *dev, int ifnum, int duration, int report_id);
extern int usb_get_report(struct usb_device *dev, int ifnum, unsigned char type,
        unsigned char id, void *buf, int size);
extern int usb_get_class_descriptor(struct usb_device *dev, int ifnum,
		unsigned char type, unsigned char id, void *buf, int size);
extern int usb_string(struct usb_device *dev, int index, char *buf, int size);

extern void usb_hcd_giveback_urb(struct usb_hcd *hcd, struct urb *urb, int status);
extern struct usb_hcd *usb_create_hcd(unsigned int priv_size);
extern int usb_add_hcd(struct usb_hcd *hcd);

extern struct urb *usb_alloc_urb(int iso_packets);
extern void usb_free_urb(struct urb *urb);
extern void usb_kill_urb(struct urb *urb);
extern void usb_fill_control_urb(struct urb *urb,
					struct usb_device *dev,
					unsigned int pipe,
					unsigned char *setup_packet,
					void *transfer_buffer,
					int buffer_length,
					usb_complete_t complete_fn,
					void *context);
extern void usb_fill_bulk_urb(struct urb *urb,
				     struct usb_device *dev,
				     unsigned int pipe,
				     void *transfer_buffer,
				     int buffer_length,
				     usb_complete_t complete_fn,
				     void *context);
extern void usb_fill_int_urb(struct urb *urb,
				    struct usb_device *dev,
				    unsigned int pipe,
				    void *transfer_buffer,
				    int buffer_length,
				    usb_complete_t complete_fn,
				    void *context,
				    int interval);
extern int usb_submit_urb(struct urb *urb);

extern void usb_reset_endpoint(struct usb_device *dev, unsigned int epaddr);
extern void usb_disable_interface(struct usb_device *dev, struct usb_interface *intf,
		bool reset_hardware);
extern void usb_enable_interface(struct usb_device *dev,
		struct usb_interface *intf, bool reset_eps);

extern int usb_control_msg(struct usb_device *dev, unsigned int pipe, u8 request,
		    u8 requesttype, u16 value, u16 index, void *data,
		    u16 size, int timeout);
extern int usb_bulk_msg(struct usb_device *usb_dev, unsigned int pipe,
		 void *data, int len, int *actual_length, int timeout);
extern int usb_interrupt_msg(struct usb_device *usb_dev, unsigned int pipe,
		      void *data, int len, int *actual_length, int timeout);
extern int usb_get_descriptor(struct usb_device *dev, unsigned char type,
		       unsigned char index, void *buf, int size);
extern int usb_get_device_descriptor(struct usb_device *dev, unsigned int size);
extern int usb_clear_halt(struct usb_device *dev, int pipe);

extern _sema  CtrlUrbCompSema;  /* Semaphore for for Control URB Complete waiting */    
extern _sema  UrbKillSema;  /* Semaphore for for URB Kill waiting */

typedef unsigned long kernel_ulong_t;

struct usb_device_id {
	/* which fields to match against? */
	u16		match_flags;

	/* Used for product specific matches; range is inclusive */
	u16		idVendor;
	u16		idProduct;
	u16		bcdDevice_lo;
	u16		bcdDevice_hi;

	/* Used for device class matches */
	u8		bDeviceClass;
	u8		bDeviceSubClass;
	u8		bDeviceProtocol;

	/* Used for interface class matches */
	u8		bInterfaceClass;
	u8		bInterfaceSubClass;
	u8		bInterfaceProtocol;

	/* Used for vendor-specific interface matches */
	u8		bInterfaceNumber;

	/* not matched against */
#ifdef __ICCARM__
        #pragma pack(64)
        kernel_ulong_t	driver_info;
#elif defined (__GNUC__)
	kernel_ulong_t	driver_info;
	__attribute__((aligned(sizeof(kernel_ulong_t))));
#endif
};

/* Some useful macros to use to create struct usb_device_id */
#define USB_DEVICE_ID_MATCH_VENDOR		0x0001
#define USB_DEVICE_ID_MATCH_PRODUCT		0x0002
#define USB_DEVICE_ID_MATCH_DEV_LO		0x0004
#define USB_DEVICE_ID_MATCH_DEV_HI		0x0008
#define USB_DEVICE_ID_MATCH_DEV_CLASS		0x0010
#define USB_DEVICE_ID_MATCH_DEV_SUBCLASS	0x0020
#define USB_DEVICE_ID_MATCH_DEV_PROTOCOL	0x0040
#define USB_DEVICE_ID_MATCH_INT_CLASS		0x0080
#define USB_DEVICE_ID_MATCH_INT_SUBCLASS	0x0100
#define USB_DEVICE_ID_MATCH_INT_PROTOCOL	0x0200
#define USB_DEVICE_ID_MATCH_INT_NUMBER		0x0400

#define USB_DEVICE_ID_MATCH_DEVICE \
		(USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT)
#define USB_DEVICE_ID_MATCH_DEV_RANGE \
		(USB_DEVICE_ID_MATCH_DEV_LO | USB_DEVICE_ID_MATCH_DEV_HI)
#define USB_DEVICE_ID_MATCH_DEVICE_AND_VERSION \
		(USB_DEVICE_ID_MATCH_DEVICE | USB_DEVICE_ID_MATCH_DEV_RANGE)
#define USB_DEVICE_ID_MATCH_DEV_INFO \
		(USB_DEVICE_ID_MATCH_DEV_CLASS | \
		USB_DEVICE_ID_MATCH_DEV_SUBCLASS | \
		USB_DEVICE_ID_MATCH_DEV_PROTOCOL)
#define USB_DEVICE_ID_MATCH_INT_INFO \
		(USB_DEVICE_ID_MATCH_INT_CLASS | \
		USB_DEVICE_ID_MATCH_INT_SUBCLASS | \
		USB_DEVICE_ID_MATCH_INT_PROTOCOL)
/**
 * USB_DEVICE - macro used to describe a specific usb device
 * @vend: the 16 bit USB Vendor ID
 * @prod: the 16 bit USB Product ID
 *
 * This macro is used to create a struct usb_device_id that matches a
 * specific device.
 */                  
#define USB_DEVICE(vend, prod) \
	.match_flags = USB_DEVICE_ID_MATCH_DEVICE, \
	.idVendor = (vend), \
	.idProduct = (prod)
          
/**
 * USB_DEVICE_VER - describe a specific usb device with a version range
 * @vend: the 16 bit USB Vendor ID
 * @prod: the 16 bit USB Product ID
 * @lo: the bcdDevice_lo value
 * @hi: the bcdDevice_hi value
 *
 * This macro is used to create a struct usb_device_id that matches a
 * specific device, with a version range.
 */
#define USB_DEVICE_VER(vend, prod, lo, hi) \
	.match_flags = USB_DEVICE_ID_MATCH_DEVICE_AND_VERSION, \
	.idVendor = (vend), \
	.idProduct = (prod), \
	.bcdDevice_lo = (lo), \
	.bcdDevice_hi = (hi)

/**
 * USB_INTERFACE_INFO - macro used to describe a class of usb interfaces
 * @cl: bInterfaceClass value
 * @sc: bInterfaceSubClass value
 * @pr: bInterfaceProtocol value
 *
 * This macro is used to create a struct usb_device_id that matches a
 * specific class of interfaces.
 */
#define USB_INTERFACE_INFO(cl, sc, pr) \
	.match_flags = USB_DEVICE_ID_MATCH_INT_INFO, \
	.bInterfaceClass = (cl), \
	.bInterfaceSubClass = (sc), \
	.bInterfaceProtocol = (pr)                

#endif /* _USB_H_ */
