
//#include "../otg/osk/sys-support.h" //ModifiedByJD    

/*
 * USB Communications Device Class (CDC) definitions
 *
 * CDC says how to talk to lots of different types of network adapters,
 * notably ethernet adapters and various modems.  It's used mostly with
 * firmware based USB peripherals.
 */

#define USB_CDC_SUBCLASS_ACM			0x02
#define USB_CDC_SUBCLASS_ETHERNET		0x06
#define USB_CDC_SUBCLASS_WHCM			0x08
#define USB_CDC_SUBCLASS_DMM			0x09
#define USB_CDC_SUBCLASS_MDLM			0x0a
#define USB_CDC_SUBCLASS_OBEX			0x0b

#define USB_CDC_PROTO_NONE			0

#define USB_CDC_ACM_PROTO_AT_V25TER		1
#define USB_CDC_ACM_PROTO_AT_PCCA101		2
#define USB_CDC_ACM_PROTO_AT_PCCA101_WAKE	3
#define USB_CDC_ACM_PROTO_AT_GSM		4
#define USB_CDC_ACM_PROTO_AT_3G			5
#define USB_CDC_ACM_PROTO_AT_CDMA		6
#define USB_CDC_ACM_PROTO_VENDOR		0xff

/*-------------------------------------------------------------------------*/
//#define UPACKED __attribute__ ((packed))
#define UPACKED 
/*
 * Class-Specific descriptors ... there are a couple dozen of them
 */

#define USB_CDC_HEADER_TYPE		0x00		/* header_desc */
#define USB_CDC_CALL_MANAGEMENT_TYPE	0x01		/* call_mgmt_descriptor */
#define USB_CDC_ACM_TYPE		0x02		/* acm_descriptor */
#define USB_CDC_UNION_TYPE		0x06		/* union_desc */
#define USB_CDC_COUNTRY_TYPE		0x07
#define USB_CDC_NETWORK_TERMINAL_TYPE	0x0a		/* network_terminal_desc */
#define USB_CDC_ETHERNET_TYPE		0x0f		/* ether_desc */
#define USB_CDC_WHCM_TYPE		0x11
#define USB_CDC_MDLM_TYPE		0x12		/* mdlm_desc */
#define USB_CDC_MDLM_DETAIL_TYPE	0x13		/* mdlm_detail_desc */
#define USB_CDC_DMM_TYPE		0x14
#define USB_CDC_OBEX_TYPE		0x15

//ModifiedByJD    (>>>) modify the data type to useable ones.
/* "Header Functional Descriptor" from CDC spec  5.2.3.1 */
struct usb_cdc_header_desc {
	u8	bLength;
	u8	bDescriptorType;
    u8	bDescriptorSubType;

	u16	bcdCDC;
} UPACKED;

/* "Call Management Descriptor" from CDC spec  5.2.3.2 */
struct usb_cdc_call_mgmt_descriptor {
	u8	bLength;
	u8	bDescriptorType;
	u8	bDescriptorSubType;

	u8	bmCapabilities;
#define USB_CDC_CALL_MGMT_CAP_CALL_MGMT		0x01
#define USB_CDC_CALL_MGMT_CAP_DATA_INTF		0x02

	u8	bDataInterface;
} UPACKED;

/* "Abstract Control Management Descriptor" from CDC spec  5.2.3.3 */
struct usb_cdc_acm_descriptor {
	u8	bLength;
	u8	bDescriptorType;
	u8	bDescriptorSubType;

	u8	bmCapabilities;
} UPACKED;

/* "Union Functional Descriptor" from CDC spec 5.2.3.8 */
struct usb_cdc_union_desc {
	u8	bLength;
	u8	bDescriptorType;
	u8	bDescriptorSubType;

	u8	bMasterInterface0;
	u8	bSlaveInterface0;
	/* ... and there could be other slave interfaces */
} UPACKED;

/* "Network Channel Terminal Functional Descriptor" from CDC spec 5.2.3.11 */
struct usb_cdc_network_terminal_desc {
	u8	bLength;
    u8	bDescriptorType;
    u8	bDescriptorSubType;

	u8	bEntityId;
	u8	iName;
	u8	bChannelIndex;
	u8	bPhysicalInterface;
} UPACKED;

/* "Ethernet Networking Functional Descriptor" from CDC spec 5.2.3.16 */
struct usb_cdc_ether_desc {
	u8	bLength;
    u8	bDescriptorType;
	u8	bDescriptorSubType;

	u8	iMACAddress;
	u32	bmEthernetStatistics;
	u16	wMaxSegmentSize;
	u16	wNumberMCFilters;
	u8	bNumberPowerFilters;
} UPACKED;

/* "MDLM Functional Descriptor" from CDC WMC spec 6.7.2.3 */
struct usb_cdc_mdlm_desc {
	u8	bLength;
	u8	bDescriptorType;
	u8	bDescriptorSubType;

	u16	bcdVersion;
	u8	bGUID[16];
}UPACKED;

/* "MDLM Detail Functional Descriptor" from CDC WMC spec 6.7.2.4 */
struct usb_cdc_mdlm_detail_desc {
	u8	bLength;
	u8	bDescriptorType;
	u8	bDescriptorSubType;

	/* type is associated with mdlm_desc.bGUID */
	u8	bGuidDescriptorType;
	u8	bDetailData[0];
} UPACKED;

/*-------------------------------------------------------------------------*/

/*
 * Class-Specific Control Requests (6.2)
 *
 * section 3.6.2.1 table 4 has the ACM profile, for modems.
 * section 3.8.2 table 10 has the ethernet profile.
 *
 * Microsoft's RNDIS stack for Ethernet is a vendor-specific CDC ACM variant,
 * heavily dependent on the encapsulated (proprietary) command mechanism.
 */

#define USB_CDC_SEND_ENCAPSULATED_COMMAND	0x00
#define USB_CDC_GET_ENCAPSULATED_RESPONSE	0x01
#define USB_CDC_REQ_SET_LINE_CODING		0x20
#define USB_CDC_REQ_GET_LINE_CODING		0x21
#define USB_CDC_REQ_SET_CONTROL_LINE_STATE	0x22
#define USB_CDC_REQ_SEND_BREAK			0x23
#define USB_CDC_SET_ETHERNET_MULTICAST_FILTERS	0x40
#define USB_CDC_SET_ETHERNET_PM_PATTERN_FILTER	0x41
#define USB_CDC_GET_ETHERNET_PM_PATTERN_FILTER	0x42
#define USB_CDC_SET_ETHERNET_PACKET_FILTER	0x43
#define USB_CDC_GET_ETHERNET_STATISTIC		0x44

/* Line Coding Structure from CDC spec 6.2.13 */
struct usb_cdc_line_coding {
	u32	dwDTERate;
	u8	bCharFormat;
#define USB_CDC_1_STOP_BITS			0
#define USB_CDC_1_5_STOP_BITS			1
#define USB_CDC_2_STOP_BITS			2

	u8	bParityType;
#define USB_CDC_NO_PARITY			0
#define USB_CDC_ODD_PARITY			1
#define USB_CDC_EVEN_PARITY			2
#define USB_CDC_MARK_PARITY			3
#define USB_CDC_SPACE_PARITY			4

	u8	bDataBits;
} UPACKED;

/* table 62; bits in multicast filter */
#define	USB_CDC_PACKET_TYPE_PROMISCUOUS		(1 << 0)
#define	USB_CDC_PACKET_TYPE_ALL_MULTICAST	(1 << 1) /* no filter */
#define	USB_CDC_PACKET_TYPE_DIRECTED		(1 << 2)
#define	USB_CDC_PACKET_TYPE_BROADCAST		(1 << 3)
#define	USB_CDC_PACKET_TYPE_MULTICAST		(1 << 4) /* filtered */


/*-------------------------------------------------------------------------*/

/*
 * Class-Specific Notifications (6.3) sent by interrupt transfers
 *
 * section 3.8.2 table 11 of the CDC spec lists Ethernet notifications
 * section 3.6.2.1 table 5 specifies ACM notifications, accepted by RNDIS
 * RNDIS also defines its own bit-incompatible notifications
 */

#define USB_CDC_NOTIFY_NETWORK_CONNECTION	0x00
#define USB_CDC_NOTIFY_RESPONSE_AVAILABLE	0x01
#define USB_CDC_NOTIFY_SERIAL_STATE		0x20
#define USB_CDC_NOTIFY_SPEED_CHANGE		0x2a

struct usb_cdc_notification {
	u8	bmRequestType;
	u8	bNotificationType;
	u16	wValue;
	u16	wIndex;
	u16	wLength;
}UPACKED;
//ModifiedByJD    (<<<)

