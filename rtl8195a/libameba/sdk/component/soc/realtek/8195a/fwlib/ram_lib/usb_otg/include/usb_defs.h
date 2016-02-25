/*
 * (C) Copyright 2001
 * Denis Peter, MPL AG Switzerland
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Note: Part of this code has been derived from linux
 *
 */
#ifndef _USB_DEFS_H_
#define _USB_DEFS_H_
/* USB constants */

/* Device and/or Interface Class codes */
#define USB_CLASS_PER_INTERFACE  0	/* for DeviceClass */
#define USB_CLASS_AUDIO          1
#define USB_CLASS_COMM           2
#define USB_CLASS_HID            3
#define USB_CLASS_PHYSICAL		5
#define USB_CLASS_STILL_IMAGE		6

#define USB_CLASS_PRINTER	       7
#define USB_CLASS_MASS_STORAGE   8
#define USB_CLASS_HUB            9
#define USB_CLASS_CDC_DATA		0x0a

#define USB_CLASS_DATA           10
#define USB_CLASS_CSCID			0x0b	/* chip+ smart card */
#define USB_CLASS_CONTENT_SEC		0x0d	/* content security */
#define USB_CLASS_VIDEO			0x0e
#define USB_CLASS_WIRELESS_CONTROLLER	0xe0
#define USB_CLASS_APP_SPEC		0xfe

#define USB_CLASS_VENDOR_SPEC    0xff

/* some HID sub classes */
#define USB_SUB_HID_NONE        0
#define USB_SUB_HID_BOOT        1

/* some UID Protocols */
#define USB_PROT_HID_NONE       0
#define USB_PROT_HID_KEYBOARD   1
#define USB_PROT_HID_MOUSE      2


/* Sub STORAGE Classes */
#define US_SC_RBC              1		/* Typically, flash devices */
#define US_SC_8020             2		/* CD-ROM */
#define US_SC_QIC              3		/* QIC-157 Tapes */
#define US_SC_UFI              4		/* Floppy */
#define US_SC_8070             5		/* Removable media */
#define US_SC_SCSI             6		/* Transparent */
#define US_SC_MIN              US_SC_RBC
#define US_SC_MAX              US_SC_SCSI

/* STORAGE Protocols */
#define US_PR_CB               1		/* Control/Bulk w/o interrupt */
#define US_PR_CBI              0		/* Control/Bulk/Interrupt */
#define US_PR_BULK             0x50		/* bulk only */

/* USB types */
#define USB_TYPE_STANDARD   (0x00 << 5)
#define USB_TYPE_CLASS      (0x01 << 5)
#define USB_TYPE_VENDOR     (0x02 << 5)
#define USB_TYPE_RESERVED   (0x03 << 5)

/* USB recipients */
#define USB_RECIP_DEVICE      0x00
#define USB_RECIP_INTERFACE   0x01
#define USB_RECIP_ENDPOINT    0x02
#define USB_RECIP_OTHER       0x03


#define USB_DT_CS_DEVICE		0x21
#define USB_DT_CS_CONFIG		0x22
#define USB_DT_CS_STRING		0x23
#define USB_DT_CS_INTERFACE		0x24
#define USB_DT_CS_ENDPOINT		0x25


/* USB directions */
#define USB_DIR_OUT			0		/* to device */
#define USB_DIR_IN			0x80		/* to host */

#if 0
enum usb_device_speed {
	USB_SPEED_UNKNOWN = 0,			/* enumerating */
	USB_SPEED_LOW, 
	USB_SPEED_FULL,		/* usb 1.1 */
	USB_SPEED_HIGH,				/* usb 2.0 */
};
#else
enum usb_device_speed {
	USB_SPEED_UNKNOWN = 0,			/* enumerating */
	USB_SPEED_LOW, USB_SPEED_FULL,		/* usb 1.1 */
	USB_SPEED_HIGH,				/* usb 2.0 */
	USB_SPEED_VARIABLE,			/* wireless (usb 2.5) */
};

#endif
/* Descriptor types */
#define USB_DT_DEVICE        0x01
#define USB_DT_CONFIG        0x02
#define USB_DT_STRING        0x03
#define USB_DT_INTERFACE     0x04
#define USB_DT_ENDPOINT      0x05
#define USB_DT_DEVICE_QUALIFIER		0x06
#define USB_DT_OTHER_SPEED_CONFIG	0x07
#define USB_DT_INTERFACE_POWER		0x08
/* these are from a minor usb 2.0 revision (ECN) */
#define USB_DT_OTG			0x09
#define USB_DT_DEBUG			0x0a
#define USB_DT_INTERFACE_ASSOCIATION	0x0b
/* these are from the Wireless USB spec */
#define USB_DT_SECURITY			0x0c
#define USB_DT_KEY			0x0d
#define USB_DT_ENCRYPTION_TYPE		0x0e
#define USB_DT_BOS			0x0f
#define USB_DT_DEVICE_CAPABILITY	0x10
#define USB_DT_WIRELESS_ENDPOINT_COMP	0x11
#define USB_DT_WIRE_ADAPTER		0x21
#define USB_DT_RPIPE			0x22

//#define USB_DT_INTERFACE_ASSOCIATION	0x0b

#define USB_DT_HID          (USB_TYPE_CLASS | 0x01)
#define USB_DT_REPORT       (USB_TYPE_CLASS | 0x02)
#define USB_DT_PHYSICAL     (USB_TYPE_CLASS | 0x03)
#define USB_DT_HUB          (USB_TYPE_CLASS | 0x09)

/* Descriptor sizes per descriptor type */
#define USB_DT_DEVICE_SIZE      18
#define USB_DT_CONFIG_SIZE      9
#define USB_DT_INTERFACE_SIZE   9
#define USB_DT_ENDPOINT_SIZE    7
#define USB_DT_ENDPOINT_AUDIO_SIZE  9	/* Audio extension */
#define USB_DT_HUB_NONVAR_SIZE  7
#define USB_DT_HID_SIZE         9

/* Endpoints */
#define USB_ENDPOINT_NUMBER_MASK  0x0f	/* in bEndpointAddress */
#define USB_ENDPOINT_DIR_MASK     0x80

#define USB_ENDPOINT_XFERTYPE_MASK 0x03	/* in bmAttributes */
#define USB_ENDPOINT_XFER_CONTROL  0
#define USB_ENDPOINT_XFER_ISOC     1
#define USB_ENDPOINT_XFER_BULK     2
#define USB_ENDPOINT_XFER_INT      3

#define USB_ENDPOINT_HALT		   0	/* IN/OUT will STALL */

/* USB Packet IDs (PIDs) */
#define USB_PID_UNDEF_0             0xf0
#define USB_PID_OUT                 0xe1
#define USB_PID_ACK                 0xd2
#define USB_PID_DATA0               0xc3
#define USB_PID_UNDEF_4             0xb4
#define USB_PID_SOF                 0xa5
#define USB_PID_UNDEF_6             0x96
#define USB_PID_UNDEF_7             0x87
#define USB_PID_UNDEF_8             0x78
#define USB_PID_IN                  0x69
#define USB_PID_NAK                 0x5a
#define USB_PID_DATA1               0x4b
#define USB_PID_PREAMBLE            0x3c
#define USB_PID_SETUP               0x2d
#define USB_PID_STALL               0x1e
#define USB_PID_UNDEF_F             0x0f

/* Standard requests */
#define USB_REQ_GET_STATUS          0x00
#define USB_REQ_CLEAR_FEATURE       0x01
#define USB_REQ_SET_FEATURE         0x03
#define USB_REQ_SET_ADDRESS         0x05
#define USB_REQ_GET_DESCRIPTOR      0x06
#define USB_REQ_SET_DESCRIPTOR      0x07
#define USB_REQ_GET_CONFIGURATION   0x08
#define USB_REQ_SET_CONFIGURATION   0x09
#define USB_REQ_GET_INTERFACE       0x0A
#define USB_REQ_SET_INTERFACE       0x0B
#define USB_REQ_SYNCH_FRAME         0x0C

/* HID requests */
#define USB_REQ_GET_REPORT          0x01
#define USB_REQ_GET_IDLE            0x02
#define USB_REQ_GET_PROTOCOL        0x03
#define USB_REQ_SET_REPORT          0x09
#define USB_REQ_SET_IDLE            0x0A
#define USB_REQ_SET_PROTOCOL        0x0B


/* "pipe" definitions */

#define PIPE_ISOCHRONOUS    0
#define PIPE_INTERRUPT      1
#define PIPE_CONTROL        2
#define PIPE_BULK           3
#define PIPE_DEVEP_MASK     0x0007ff00

#define USB_ISOCHRONOUS    0
#define USB_INTERRUPT      1
#define USB_CONTROL        2
#define USB_BULK           3

/* USB-status codes: */
#define USB_ST_ACTIVE           0x1		/* TD is active */
#define USB_ST_STALLED          0x2		/* TD is stalled */
#define USB_ST_BUF_ERR          0x4		/* buffer error */
#define USB_ST_BABBLE_DET       0x8		/* Babble detected */
#define USB_ST_NAK_REC          0x10	/* NAK Received*/
#define USB_ST_CRC_ERR          0x20	/* CRC/timeout Error */
#define USB_ST_BIT_ERR          0x40	/* Bitstuff error */
#define USB_ST_NOT_PROC         0x80000000L	/* Not yet processed */


/*************************************************************************
 * Hub defines
 */

/*
 * Hub request types
 */

#define USB_RT_HUB	(USB_TYPE_CLASS | USB_RECIP_DEVICE)
#define USB_RT_PORT	(USB_TYPE_CLASS | USB_RECIP_OTHER)

/*
 * Hub Class feature numbers
 */
#define C_HUB_LOCAL_POWER   0
#define C_HUB_OVER_CURRENT  1

/*
 * Port feature numbers
 */
#define USB_PORT_FEAT_CONNECTION     0
#define USB_PORT_FEAT_ENABLE         1
#define USB_PORT_FEAT_SUSPEND        2
#define USB_PORT_FEAT_OVER_CURRENT   3
#define USB_PORT_FEAT_RESET          4
#define USB_PORT_FEAT_POWER          8
#define USB_PORT_FEAT_LOWSPEED       9
#define USB_PORT_FEAT_HIGHSPEED      10
#define USB_PORT_FEAT_C_CONNECTION   16
#define USB_PORT_FEAT_C_ENABLE       17
#define USB_PORT_FEAT_C_SUSPEND      18
#define USB_PORT_FEAT_C_OVER_CURRENT 19
#define USB_PORT_FEAT_C_RESET        20

/* wPortStatus bits */
#define USB_PORT_STAT_CONNECTION    0x0001
#define USB_PORT_STAT_ENABLE        0x0002
#define USB_PORT_STAT_SUSPEND       0x0004
#define USB_PORT_STAT_OVERCURRENT   0x0008
#define USB_PORT_STAT_RESET         0x0010
#define USB_PORT_STAT_POWER         0x0100
#define USB_PORT_STAT_LOW_SPEED     0x0200
#define USB_PORT_STAT_HIGH_SPEED    0x0400	/* support for EHCI */
#define USB_PORT_STAT_SPEED	\
	(USB_PORT_STAT_LOW_SPEED | USB_PORT_STAT_HIGH_SPEED)

/* wPortChange bits */
#define USB_PORT_STAT_C_CONNECTION  0x0001
#define USB_PORT_STAT_C_ENABLE      0x0002
#define USB_PORT_STAT_C_SUSPEND     0x0004
#define USB_PORT_STAT_C_OVERCURRENT 0x0008
#define USB_PORT_STAT_C_RESET       0x0010

/* wHubCharacteristics (masks) */
#define HUB_CHAR_LPSM               0x0003
#define HUB_CHAR_COMPOUND           0x0004
#define HUB_CHAR_OCPM               0x0018

/*
 *Hub Status & Hub Change bit masks
 */
#define HUB_STATUS_LOCAL_POWER	0x0001
#define HUB_STATUS_OVERCURRENT	0x0002

#define HUB_CHANGE_LOCAL_POWER	0x0001
#define HUB_CHANGE_OVERCURRENT	0x0002

/* Struct USB_HCD defination */
// for flags
#define HCD_FLAG_HW_ACCESSIBLE		0	/* at full power */
#define HCD_FLAG_POLL_RH		2	/* poll for rh status? */
#define HCD_FLAG_POLL_PENDING		3	/* status has changed? */
#define HCD_FLAG_WAKEUP_PENDING		4	/* root hub is resuming? */
#define HCD_FLAG_RH_RUNNING		5	/* root hub is running? */
#define HCD_FLAG_DEAD			6	/* controller has died? */

/* The flags can be tested using these macros; they are likely to
 * be slightly faster than test_bit().
 */
#define HCD_HW_ACCESSIBLE(hcd)	((hcd)->flags & (1U << HCD_FLAG_HW_ACCESSIBLE))
#define HCD_POLL_RH(hcd)	((hcd)->flags & (1U << HCD_FLAG_POLL_RH))
#define HCD_POLL_PENDING(hcd)	((hcd)->flags & (1U << HCD_FLAG_POLL_PENDING))
#define HCD_WAKEUP_PENDING(hcd)	((hcd)->flags & (1U << HCD_FLAG_WAKEUP_PENDING))
#define HCD_RH_RUNNING(hcd)	((hcd)->flags & (1U << HCD_FLAG_RH_RUNNING))
#define HCD_DEAD(hcd)		((hcd)->flags & (1U << HCD_FLAG_DEAD))

// for state
#define	__ACTIVE		0x01
#define	__SUSPEND		0x04
#define	__TRANSIENT		0x80

#define	HC_STATE_HALT		0
#define	HC_STATE_RUNNING	(__ACTIVE)
#define	HC_STATE_QUIESCING	(__SUSPEND|__TRANSIENT|__ACTIVE)
#define	HC_STATE_RESUMING	(__SUSPEND|__TRANSIENT)
#define	HC_STATE_SUSPENDED	(__SUSPEND)

#define	HC_IS_RUNNING(state) ((state) & __ACTIVE)
#define	HC_IS_SUSPENDED(state) ((state) & __SUSPEND)

/*
 * USB feature flags are written using USB_REQ_{CLEAR,SET}_FEATURE, and
 * are read as a bit array returned by USB_REQ_GET_STATUS.  (So there
 * are at most sixteen features of each type.)  Hubs may also support a
 * new USB_REQ_TEST_AND_SET_FEATURE to put ports into L1 suspend.
 */
#define USB_DEVICE_SELF_POWERED		0	/* (read only) */
#define USB_DEVICE_REMOTE_WAKEUP	1	/* dev may initiate wakeup */
#define USB_DEVICE_TEST_MODE		2	/* (wired high speed only) */
#define USB_DEVICE_BATTERY		    2	/* (wireless) */
#define USB_DEVICE_B_HNP_ENABLE		3	/* (otg) dev may initiate HNP */
#define USB_DEVICE_WUSB_DEVICE		3	/* (wireless)*/
#define USB_DEVICE_A_HNP_SUPPORT	4	/* (otg) RH port supports HNP */
#define USB_DEVICE_A_ALT_HNP_SUPPORT	5	/* (otg) other RH port does */
#define USB_DEVICE_DEBUG_MODE		6	/* (special devices only) */

/* (shifted) direction/type/recipient from the USB 2.0 spec, table 9.2 */
#define DeviceRequest \
	((USB_DIR_IN|USB_TYPE_STANDARD|USB_RECIP_DEVICE)<<8)
#define DeviceOutRequest \
	((USB_DIR_OUT|USB_TYPE_STANDARD|USB_RECIP_DEVICE)<<8)

#define InterfaceRequest \
	((USB_DIR_IN|USB_TYPE_STANDARD|USB_RECIP_INTERFACE)<<8)

#define EndpointRequest \
	((USB_DIR_IN|USB_TYPE_STANDARD|USB_RECIP_INTERFACE)<<8)
#define EndpointOutRequest \
	((USB_DIR_OUT|USB_TYPE_STANDARD|USB_RECIP_INTERFACE)<<8)

/* class requests from the USB 2.0 hub spec, table 11-15 */
/* GetBusState and SetHubDescriptor are optional, omitted */
#define ClearHubFeature		(0x2000 | USB_REQ_CLEAR_FEATURE)
#define ClearPortFeature	(0x2300 | USB_REQ_CLEAR_FEATURE)
#define GetHubDescriptor	(0xa000 | USB_REQ_GET_DESCRIPTOR)
#define GetHubStatus		(0xa000 | USB_REQ_GET_STATUS)
#define GetPortStatus		(0xa300 | USB_REQ_GET_STATUS)
#define SetHubFeature		(0x2000 | USB_REQ_SET_FEATURE)
#define SetPortFeature		(0x2300 | USB_REQ_SET_FEATURE)

/* from config descriptor bmAttributes */
#define USB_CONFIG_ATT_ONE		(1 << 7)	/* must be set */
#define USB_CONFIG_ATT_SELFPOWER	(1 << 6)	/* self powered */
#define USB_CONFIG_ATT_WAKEUP		(1 << 5)	/* can wakeup */
#define USB_CONFIG_ATT_BATTERY		(1 << 4)	/* battery powered */

#endif /*_USB_DEFS_H_ */
