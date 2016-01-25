/***********************************************************************
* $Id:: mw_usbd_cdc.h 165 2011-04-14 17:41:11Z usb10131                       $
*
* Project: USB device ROM Stack
*
* Description:
*     USB Communication Device Class User module Definitions.
*
***********************************************************************
*   Copyright(C) 2011, NXP Semiconductor
*   All rights reserved.
*
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/
#ifndef __CDC_H
#define __CDC_H

#include "usbd.h"

/*----------------------------------------------------------------------------
 *      Definitions  based on usbcdc11.pdf (www.usb.org)
 *---------------------------------------------------------------------------*/
/* Communication device class specification version 1.10 */
#define CDC_V1_10                               0x0110

/* Communication interface class code */
/* (usbcdc11.pdf, 4.2, Table 15) */
#define CDC_COMMUNICATION_INTERFACE_CLASS       0x02

/* Communication interface class subclass codes */
/* (usbcdc11.pdf, 4.3, Table 16) */
#define CDC_DIRECT_LINE_CONTROL_MODEL           0x01
#define CDC_ABSTRACT_CONTROL_MODEL              0x02
#define CDC_TELEPHONE_CONTROL_MODEL             0x03
#define CDC_MULTI_CHANNEL_CONTROL_MODEL         0x04
#define CDC_CAPI_CONTROL_MODEL                  0x05
#define CDC_ETHERNET_NETWORKING_CONTROL_MODEL   0x06
#define CDC_ATM_NETWORKING_CONTROL_MODEL        0x07

/* Communication interface class control protocol codes */
/* (usbcdc11.pdf, 4.4, Table 17) */
#define CDC_PROTOCOL_COMMON_AT_COMMANDS         0x01

/* Data interface class code */
/* (usbcdc11.pdf, 4.5, Table 18) */
#define CDC_DATA_INTERFACE_CLASS                0x0A

/* Data interface class protocol codes */
/* (usbcdc11.pdf, 4.7, Table 19) */
#define CDC_PROTOCOL_ISDN_BRI                   0x30
#define CDC_PROTOCOL_HDLC                       0x31
#define CDC_PROTOCOL_TRANSPARENT                0x32
#define CDC_PROTOCOL_Q921_MANAGEMENT            0x50
#define CDC_PROTOCOL_Q921_DATA_LINK             0x51
#define CDC_PROTOCOL_Q921_MULTIPLEXOR           0x52
#define CDC_PROTOCOL_V42                        0x90
#define CDC_PROTOCOL_EURO_ISDN                  0x91
#define CDC_PROTOCOL_V24_RATE_ADAPTATION        0x92
#define CDC_PROTOCOL_CAPI                       0x93
#define CDC_PROTOCOL_HOST_BASED_DRIVER          0xFD
#define CDC_PROTOCOL_DESCRIBED_IN_PUFD          0xFE

/* Type values for bDescriptorType field of functional descriptors */
/* (usbcdc11.pdf, 5.2.3, Table 24) */
#define CDC_CS_INTERFACE                        0x24
#define CDC_CS_ENDPOINT                         0x25

/* Type values for bDescriptorSubtype field of functional descriptors */
/* (usbcdc11.pdf, 5.2.3, Table 25) */
#define CDC_HEADER                              0x00
#define CDC_CALL_MANAGEMENT                     0x01
#define CDC_ABSTRACT_CONTROL_MANAGEMENT         0x02
#define CDC_DIRECT_LINE_MANAGEMENT              0x03
#define CDC_TELEPHONE_RINGER                    0x04
#define CDC_REPORTING_CAPABILITIES              0x05
#define CDC_UNION                               0x06
#define CDC_COUNTRY_SELECTION                   0x07
#define CDC_TELEPHONE_OPERATIONAL_MODES         0x08
#define CDC_USB_TERMINAL                        0x09
#define CDC_NETWORK_CHANNEL                     0x0A
#define CDC_PROTOCOL_UNIT                       0x0B
#define CDC_EXTENSION_UNIT                      0x0C
#define CDC_MULTI_CHANNEL_MANAGEMENT            0x0D
#define CDC_CAPI_CONTROL_MANAGEMENT             0x0E
#define CDC_ETHERNET_NETWORKING                 0x0F
#define CDC_ATM_NETWORKING                      0x10

/* CDC class-specific request codes */
/* (usbcdc11.pdf, 6.2, Table 46) */
/* see Table 45 for info about the specific requests. */
#define CDC_SEND_ENCAPSULATED_COMMAND           0x00
#define CDC_GET_ENCAPSULATED_RESPONSE           0x01
#define CDC_SET_COMM_FEATURE                    0x02
#define CDC_GET_COMM_FEATURE                    0x03
#define CDC_CLEAR_COMM_FEATURE                  0x04
#define CDC_SET_AUX_LINE_STATE                  0x10
#define CDC_SET_HOOK_STATE                      0x11
#define CDC_PULSE_SETUP                         0x12
#define CDC_SEND_PULSE                          0x13
#define CDC_SET_PULSE_TIME                      0x14
#define CDC_RING_AUX_JACK                       0x15
#define CDC_SET_LINE_CODING                     0x20
#define CDC_GET_LINE_CODING                     0x21
#define CDC_SET_CONTROL_LINE_STATE              0x22
#define CDC_SEND_BREAK                          0x23
#define CDC_SET_RINGER_PARMS                    0x30
#define CDC_GET_RINGER_PARMS                    0x31
#define CDC_SET_OPERATION_PARMS                 0x32
#define CDC_GET_OPERATION_PARMS                 0x33
#define CDC_SET_LINE_PARMS                      0x34
#define CDC_GET_LINE_PARMS                      0x35
#define CDC_DIAL_DIGITS                         0x36
#define CDC_SET_UNIT_PARAMETER                  0x37
#define CDC_GET_UNIT_PARAMETER                  0x38
#define CDC_CLEAR_UNIT_PARAMETER                0x39
#define CDC_GET_PROFILE                         0x3A
#define CDC_SET_ETHERNET_MULTICAST_FILTERS      0x40
#define CDC_SET_ETHERNET_PMP_FILTER             0x41
#define CDC_GET_ETHERNET_PMP_FILTER             0x42
#define CDC_SET_ETHERNET_PACKET_FILTER          0x43
#define CDC_GET_ETHERNET_STATISTIC              0x44
#define CDC_SET_ATM_DATA_FORMAT                 0x50
#define CDC_GET_ATM_DEVICE_STATISTICS           0x51
#define CDC_SET_ATM_DEFAULT_VC                  0x52
#define CDC_GET_ATM_VC_STATISTICS               0x53

/* Communication feature selector codes */
/* (usbcdc11.pdf, 6.2.2..6.2.4, Table 47) */
#define CDC_ABSTRACT_STATE                      0x01
#define CDC_COUNTRY_SETTING                     0x02

/* Feature Status returned for ABSTRACT_STATE Selector */
/* (usbcdc11.pdf, 6.2.3, Table 48) */
#define CDC_IDLE_SETTING                        (1 << 0)
#define CDC_DATA_MULTPLEXED_STATE               (1 << 1)


/* Control signal bitmap values for the SetControlLineState request */
/* (usbcdc11.pdf, 6.2.14, Table 51) */
#define CDC_DTE_PRESENT                         (1 << 0)
#define CDC_ACTIVATE_CARRIER                    (1 << 1)

/* CDC class-specific notification codes */
/* (usbcdc11.pdf, 6.3, Table 68) */
/* see Table 67 for Info about class-specific notifications */
#define CDC_NOTIFICATION_NETWORK_CONNECTION     0x00
#define CDC_RESPONSE_AVAILABLE                  0x01
#define CDC_AUX_JACK_HOOK_STATE                 0x08
#define CDC_RING_DETECT                         0x09
#define CDC_NOTIFICATION_SERIAL_STATE           0x20
#define CDC_CALL_STATE_CHANGE                   0x28
#define CDC_LINE_STATE_CHANGE                   0x29
#define CDC_CONNECTION_SPEED_CHANGE             0x2A

/* UART state bitmap values (Serial state notification). */
/* (usbcdc11.pdf, 6.3.5, Table 69) */
#define CDC_SERIAL_STATE_OVERRUN                (1 << 6)  /* receive data overrun error has occurred */
#define CDC_SERIAL_STATE_PARITY                 (1 << 5)  /* parity error has occurred */
#define CDC_SERIAL_STATE_FRAMING                (1 << 4)  /* framing error has occurred */
#define CDC_SERIAL_STATE_RING                   (1 << 3)  /* state of ring signal detection */
#define CDC_SERIAL_STATE_BREAK                  (1 << 2)  /* state of break detection */
#define CDC_SERIAL_STATE_TX_CARRIER             (1 << 1)  /* state of transmission carrier */
#define CDC_SERIAL_STATE_RX_CARRIER             (1 << 0)  /* state of receiver carrier */


/*----------------------------------------------------------------------------
 *      Structures  based on usbcdc11.pdf (www.usb.org)
 *---------------------------------------------------------------------------*/

/* Header functional descriptor */
/* (usbcdc11.pdf, 5.2.3.1) */
/* This header must precede any list of class-specific descriptors. */
PRE_PACK struct POST_PACK _CDC_HEADER_DESCRIPTOR{
  uint8_t  bFunctionLength;                      /* size of this descriptor in bytes */
  uint8_t  bDescriptorType;                      /* CS_INTERFACE descriptor type */
  uint8_t  bDescriptorSubtype;                   /* Header functional descriptor subtype */
  uint16_t bcdCDC;                               /* USB CDC specification release version */
};
typedef struct _CDC_HEADER_DESCRIPTOR CDC_HEADER_DESCRIPTOR;

/* Call management functional descriptor */
/* (usbcdc11.pdf, 5.2.3.2) */
/* Describes the processing of calls for the communication class interface. */
PRE_PACK struct POST_PACK _CDC_CALL_MANAGEMENT_DESCRIPTOR {
  uint8_t  bFunctionLength;                      /* size of this descriptor in bytes */
  uint8_t  bDescriptorType;                      /* CS_INTERFACE descriptor type */
  uint8_t  bDescriptorSubtype;                   /* call management functional descriptor subtype */
  uint8_t  bmCapabilities;                       /* capabilities that this configuration supports */
  uint8_t  bDataInterface;                       /* interface number of the data class interface used for call management (optional) */
};
typedef struct _CDC_CALL_MANAGEMENT_DESCRIPTOR CDC_CALL_MANAGEMENT_DESCRIPTOR;

/* Abstract control management functional descriptor */
/* (usbcdc11.pdf, 5.2.3.3) */
/* Describes the command supported by the communication interface class with the Abstract Control Model subclass code. */
PRE_PACK struct POST_PACK _CDC_ABSTRACT_CONTROL_MANAGEMENT_DESCRIPTOR {
  uint8_t  bFunctionLength;                      /* size of this descriptor in bytes */
  uint8_t  bDescriptorType;                      /* CS_INTERFACE descriptor type */
  uint8_t  bDescriptorSubtype;                   /* abstract control management functional descriptor subtype */
  uint8_t  bmCapabilities;                       /* capabilities supported by this configuration */
};
typedef struct _CDC_ABSTRACT_CONTROL_MANAGEMENT_DESCRIPTOR CDC_ABSTRACT_CONTROL_MANAGEMENT_DESCRIPTOR;

/* Union functional descriptors */
/* (usbcdc11.pdf, 5.2.3.8) */
/* Describes the relationship between a group of interfaces that can be considered to form a functional unit. */
PRE_PACK struct POST_PACK _CDC_UNION_DESCRIPTOR {
  uint8_t  bFunctionLength;                      /* size of this descriptor in bytes */
  uint8_t  bDescriptorType;                      /* CS_INTERFACE descriptor type */
  uint8_t  bDescriptorSubtype;                   /* union functional descriptor subtype */
  uint8_t  bMasterInterface;                     /* interface number designated as master */
};
typedef struct _CDC_UNION_DESCRIPTOR CDC_UNION_DESCRIPTOR;

/* Union functional descriptors with one slave interface */
/* (usbcdc11.pdf, 5.2.3.8) */
PRE_PACK struct POST_PACK _CDC_UNION_1SLAVE_DESCRIPTOR {
  CDC_UNION_DESCRIPTOR sUnion;              /* Union functional descriptor */
  uint8_t              bSlaveInterfaces[1]; /* Slave interface 0 */
};
typedef struct _CDC_UNION_1SLAVE_DESCRIPTOR CDC_UNION_1SLAVE_DESCRIPTOR;

/* Line coding structure */
/* Format of the data returned when a GetLineCoding request is received */
/* (usbcdc11.pdf, 6.2.13) */
PRE_PACK struct POST_PACK _CDC_LINE_CODING {
  uint32_t dwDTERate;                            /* Data terminal rate in bits per second */
  uint8_t  bCharFormat;                          /* Number of stop bits */
  uint8_t  bParityType;                          /* Parity bit type */
  uint8_t  bDataBits;                            /* Number of data bits */
};
typedef struct _CDC_LINE_CODING CDC_LINE_CODING;

/* Notification header */
/* Data sent on the notification endpoint must follow this header. */
/* see  USB_SETUP_PACKET in file usb.h */
typedef USB_SETUP_PACKET CDC_NOTIFICATION_HEADER;

#endif /* __CDC_H */

