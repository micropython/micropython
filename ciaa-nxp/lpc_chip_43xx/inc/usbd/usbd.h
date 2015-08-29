/***********************************************************************
* $Id:: mw_usbd.h 575 2012-11-20 01:35:56Z usb10131                           $
*
* Project: USB device ROM Stack
*
* Description:
*     USB Definitions.
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

#ifndef __USBD_H__
#define __USBD_H__

/** \file
 *  \brief Common definitions and declarations for the USB stack.
 *
 *  Common definitions and declarations for the USB stack.
 *  \addtogroup USBD_Core 
 *  @{
 */

#include "lpc_types.h"

#if defined(__GNUC__)
/* As per http://gcc.gnu.org/onlinedocs/gcc/Attribute-Syntax.html#Attribute-Syntax,
6.29 Attributes Syntax
"An attribute specifier list may appear as part of a struct, union or
enum specifier. It may go either immediately after the struct, union
or enum keyword, or after the closing brace. The former syntax is
preferred. Where attribute specifiers follow the closing brace, they
are considered to relate to the structure, union or enumerated type
defined, not to any enclosing declaration the type specifier appears
in, and the type defined is not complete until after the attribute
specifiers."
So use POST_PACK immediately after struct keyword
*/
#define PRE_PACK
#define POST_PACK	__attribute__((__packed__))
#define ALIGNED(n)      __attribute__((aligned (n)))

#elif defined(__arm)
#define PRE_PACK	__packed
#define POST_PACK
#define ALIGNED(n)      __align(n)

#elif defined(__ICCARM__)
#define PRE_PACK                __packed
#define POST_PACK
#define PRAGMA_ALIGN_4096       _Pragma("data_alignment=4096")
#define PRAGMA_ALIGN_2048       _Pragma("data_alignment=2048")
#define PRAGMA_ALIGN_512        _Pragma("data_alignment=512")
#define PRAGMA_ALIGN_256        _Pragma("data_alignment=256")
#define PRAGMA_ALIGN_128        _Pragma("data_alignment=128")
#define PRAGMA_ALIGN_64         _Pragma("data_alignment=64")
#define PRAGMA_ALIGN_48         _Pragma("data_alignment=48")
#define PRAGMA_ALIGN_32         _Pragma("data_alignment=32")
#define PRAGMA_ALIGN_4          _Pragma("data_alignment=4")
#define ALIGNED(n)              PRAGMA_ALIGN_##n

#pragma diag_suppress=Pe021
#endif

/** Structure to pack lower and upper byte to form 16 bit word. */
PRE_PACK struct POST_PACK _WB_T
{
  uint8_t L; /**< lower byte */
  uint8_t H; /**< upper byte */
};
/** Structure to pack lower and upper byte to form 16 bit word.*/
typedef struct _WB_T WB_T;

/** Union of \ref _WB_T struct and 16 bit word.*/
PRE_PACK union POST_PACK __WORD_BYTE
{
  uint16_t W; /**< data member to do 16 bit access */
  WB_T WB; /**< data member to do 8 bit access */
} ;
/** Union of \ref _WB_T struct and 16 bit word.*/
typedef union __WORD_BYTE WORD_BYTE;

/** bmRequestType.Dir defines 
 * @{ 
 */
/** Request from host to device */
#define REQUEST_HOST_TO_DEVICE     0
/** Request from device to host */
#define REQUEST_DEVICE_TO_HOST     1
/** @} */

/** bmRequestType.Type defines  
 * @{ 
 */
/** Standard Request */
#define REQUEST_STANDARD           0
/** Class Request */
#define REQUEST_CLASS              1
/** Vendor Request */
#define REQUEST_VENDOR             2
/** Reserved Request */
#define REQUEST_RESERVED           3
/** @} */

/** bmRequestType.Recipient defines  
 * @{ 
 */
/** Request to device */
#define REQUEST_TO_DEVICE          0
/** Request to interface */
#define REQUEST_TO_INTERFACE       1
/** Request to endpoint */
#define REQUEST_TO_ENDPOINT        2
/** Request to other */
#define REQUEST_TO_OTHER           3
/** @} */

/** Structure to define 8 bit USB request.*/
PRE_PACK struct POST_PACK _BM_T
{
  uint8_t Recipient :  5; /**< Recipient type. */
  uint8_t Type      :  2; /**< Request type.  */
  uint8_t Dir       :  1; /**< Direction type. */
};
/** Structure to define 8 bit USB request.*/
typedef struct _BM_T BM_T;

/** Union of \ref _BM_T struct and 8 bit byte.*/
PRE_PACK union POST_PACK _REQUEST_TYPE
{
  uint8_t B; /**< byte wide access memeber */
  BM_T BM;   /**< bitfield structure access memeber */
} ;
/** Union of \ref _BM_T struct and 8 bit byte.*/
typedef union _REQUEST_TYPE REQUEST_TYPE;

/** USB Standard Request Codes 
 * @{ 
 */
/** GET_STATUS request */
#define USB_REQUEST_GET_STATUS                 0
/** CLEAR_FEATURE request */
#define USB_REQUEST_CLEAR_FEATURE              1
/** SET_FEATURE request */
#define USB_REQUEST_SET_FEATURE                3
/** SET_ADDRESS request */
#define USB_REQUEST_SET_ADDRESS                5
/** GET_DESCRIPTOR request */
#define USB_REQUEST_GET_DESCRIPTOR             6
/** SET_DESCRIPTOR request */
#define USB_REQUEST_SET_DESCRIPTOR             7
/** GET_CONFIGURATION request */
#define USB_REQUEST_GET_CONFIGURATION          8
/** SET_CONFIGURATION request */
#define USB_REQUEST_SET_CONFIGURATION          9
/** GET_INTERFACE request */
#define USB_REQUEST_GET_INTERFACE              10
/** SET_INTERFACE request */
#define USB_REQUEST_SET_INTERFACE              11
/** SYNC_FRAME request */
#define USB_REQUEST_SYNC_FRAME                 12
/** @} */

/** USB GET_STATUS Bit Values 
 * @{ 
 */
/** SELF_POWERED status*/
#define USB_GETSTATUS_SELF_POWERED             0x01
/** REMOTE_WAKEUP capable status*/
#define USB_GETSTATUS_REMOTE_WAKEUP            0x02
/** ENDPOINT_STALL status*/
#define USB_GETSTATUS_ENDPOINT_STALL           0x01
/** @} */

/** USB Standard Feature selectors 
 * @{ 
 */
/** ENDPOINT_STALL feature*/
#define USB_FEATURE_ENDPOINT_STALL             0
/** REMOTE_WAKEUP feature*/
#define USB_FEATURE_REMOTE_WAKEUP              1
/** TEST_MODE feature*/
#define USB_FEATURE_TEST_MODE                  2
/** @} */

/** USB Default Control Pipe Setup Packet*/
PRE_PACK struct POST_PACK _USB_SETUP_PACKET
{
  REQUEST_TYPE bmRequestType; /**< This bitmapped field identifies the characteristics
                              of the specific request. \sa _BM_T.
                              */
  uint8_t      bRequest; /**< This field specifies the particular request. The 
                         Type bits in the bmRequestType field modify the meaning 
                         of this field. \sa USBD_REQUEST.
                         */
  WORD_BYTE    wValue; /**< Used to pass a parameter to the device, specific
                        to the request.
                        */
  WORD_BYTE    wIndex; /**< Used to pass a parameter to the device, specific
                        to the request. The wIndex field is often used in 
                        requests to specify an endpoint or an interface.
                        */
  uint16_t     wLength; /**< This field specifies the length of the data 
                        transferred during the second phase of the control 
                        transfer.
                        */
} ;
/** USB Default Control Pipe Setup Packet*/
typedef struct _USB_SETUP_PACKET USB_SETUP_PACKET;


/** USB Descriptor Types 
 * @{ 
 */
/** Device descriptor type  */
#define USB_DEVICE_DESCRIPTOR_TYPE             1
/** Configuration descriptor type  */
#define USB_CONFIGURATION_DESCRIPTOR_TYPE      2
/** String descriptor type  */
#define USB_STRING_DESCRIPTOR_TYPE             3
/** Interface descriptor type  */
#define USB_INTERFACE_DESCRIPTOR_TYPE          4
/** Endpoint descriptor type  */
#define USB_ENDPOINT_DESCRIPTOR_TYPE           5
/** Device qualifier descriptor type  */
#define USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE   6
/** Other speed configuration descriptor type  */
#define USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE 7
/** Interface power descriptor type  */
#define USB_INTERFACE_POWER_DESCRIPTOR_TYPE    8
/** OTG descriptor type  */
#define USB_OTG_DESCRIPTOR_TYPE                     9
/** Debug descriptor type  */
#define USB_DEBUG_DESCRIPTOR_TYPE                  10
/** Interface association descriptor type  */
#define USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE  11
/** @} */

/** USB Device Classes 
 * @{ 
 */
/** Reserved device class  */
#define USB_DEVICE_CLASS_RESERVED              0x00
/** Audio device class  */
#define USB_DEVICE_CLASS_AUDIO                 0x01
/** Communications device class  */
#define USB_DEVICE_CLASS_COMMUNICATIONS        0x02
/** Human interface device class  */
#define USB_DEVICE_CLASS_HUMAN_INTERFACE       0x03
/** monitor device class  */
#define USB_DEVICE_CLASS_MONITOR               0x04
/** physical interface device class  */
#define USB_DEVICE_CLASS_PHYSICAL_INTERFACE    0x05
/** power device class  */
#define USB_DEVICE_CLASS_POWER                 0x06
/** Printer device class  */
#define USB_DEVICE_CLASS_PRINTER               0x07
/** Storage device class  */
#define USB_DEVICE_CLASS_STORAGE               0x08
/** Hub device class  */
#define USB_DEVICE_CLASS_HUB                   0x09
/** miscellaneous device class  */
#define USB_DEVICE_CLASS_MISCELLANEOUS         0xEF
/** Application device class  */
#define USB_DEVICE_CLASS_APP                   0xFE
/** Vendor specific device class  */
#define USB_DEVICE_CLASS_VENDOR_SPECIFIC       0xFF
/** @} */

/** bmAttributes in Configuration Descriptor 
 * @{ 
 */
/** Power field mask */
#define USB_CONFIG_POWERED_MASK                0x40
/** Bus powered */
#define USB_CONFIG_BUS_POWERED                 0x80
/** Self powered */
#define USB_CONFIG_SELF_POWERED                0xC0
/** remote wakeup */
#define USB_CONFIG_REMOTE_WAKEUP               0x20
/** @} */

/** bMaxPower in Configuration Descriptor */
#define USB_CONFIG_POWER_MA(mA)                ((mA)/2)

/** bEndpointAddress in Endpoint Descriptor 
 * @{ 
 */
/** Endopint address mask */
#define USB_ENDPOINT_DIRECTION_MASK            0x80
/** Macro to convert OUT endopint number to endpoint address value. */
#define USB_ENDPOINT_OUT(addr)                 ((addr) | 0x00)
/** Macro to convert IN endopint number to endpoint address value. */
#define USB_ENDPOINT_IN(addr)                  ((addr) | 0x80)
/** @} */

/** bmAttributes in Endpoint Descriptor 
 * @{ 
 */
/** Endopint type mask */
#define USB_ENDPOINT_TYPE_MASK                 0x03
/** Control Endopint type */
#define USB_ENDPOINT_TYPE_CONTROL              0x00
/** isochronous Endopint type */
#define USB_ENDPOINT_TYPE_ISOCHRONOUS          0x01
/** bulk Endopint type */
#define USB_ENDPOINT_TYPE_BULK                 0x02
/** interrupt Endopint type */
#define USB_ENDPOINT_TYPE_INTERRUPT            0x03
/** Endopint sync type mask */
#define USB_ENDPOINT_SYNC_MASK                 0x0C
/** no synchronization Endopint */
#define USB_ENDPOINT_SYNC_NO_SYNCHRONIZATION   0x00
/** Asynchronous sync Endopint */
#define USB_ENDPOINT_SYNC_ASYNCHRONOUS         0x04
/** Adaptive sync Endopint */
#define USB_ENDPOINT_SYNC_ADAPTIVE             0x08
/** Synchronous sync Endopint */
#define USB_ENDPOINT_SYNC_SYNCHRONOUS          0x0C
/** Endopint usage type mask */
#define USB_ENDPOINT_USAGE_MASK                0x30
/** Endopint data usage type  */
#define USB_ENDPOINT_USAGE_DATA                0x00
/** Endopint feedback usage type  */
#define USB_ENDPOINT_USAGE_FEEDBACK            0x10
/** Endopint implicit feedback usage type  */
#define USB_ENDPOINT_USAGE_IMPLICIT_FEEDBACK   0x20
/** Endopint reserved usage type  */
#define USB_ENDPOINT_USAGE_RESERVED            0x30
/** @} */

/** Control endopint EP0's maximum packet size in high-speed mode.*/
#define USB_ENDPOINT_0_HS_MAXP                 64
/** Control endopint EP0's maximum packet size in low-speed mode.*/
#define USB_ENDPOINT_0_LS_MAXP                 8
/** Bulk endopint's maximum packet size in high-speed mode.*/
#define USB_ENDPOINT_BULK_HS_MAXP              512

/** USB Standard Device Descriptor */
PRE_PACK struct POST_PACK _USB_DEVICE_DESCRIPTOR
{
  uint8_t  bLength;     /**< Size of this descriptor in bytes. */
  uint8_t  bDescriptorType; /**< DEVICE Descriptor Type. */
  uint16_t bcdUSB; /**< BUSB Specification Release Number in
                    Binary-Coded Decimal (i.e., 2.10 is 210H).
                    This field identifies the release of the USB
                    Specification with which the device and its
                    descriptors are compliant.
                   */
  uint8_t  bDeviceClass; /**< Class code (assigned by the USB-IF).
                          If this field is reset to zero, each interface
                          within a configuration specifies its own
                          class information and the various
                          interfaces operate independently.\n
                          If this field is set to a value between 1 and
                          FEH, the device supports different class
                          specifications on different interfaces and
                          the interfaces may not operate
                          independently. This value identifies the
                          class definition used for the aggregate
                          interfaces. \n
                          If this field is set to FFH, the device class
                          is vendor-specific.
                          */
  uint8_t  bDeviceSubClass; /**< Subclass code (assigned by the USB-IF).
                            These codes are qualified by the value of
                            the bDeviceClass field. \n
                            If the bDeviceClass field is reset to zero,
                            this field must also be reset to zero. \n
                            If the bDeviceClass field is not set to FFH,
                            all values are reserved for assignment by
                            the USB-IF. 
                            */
  uint8_t  bDeviceProtocol; /**< Protocol code (assigned by the USB-IF).
                            These codes are qualified by the value of
                            the bDeviceClass and the
                            bDeviceSubClass fields. If a device
                            supports class-specific protocols on a
                            device basis as opposed to an interface
                            basis, this code identifies the protocols
                            that the device uses as defined by the
                            specification of the device class. \n
                            If this field is reset to zero, the device
                            does not use class-specific protocols on a
                            device basis. However, it may use classspecific
                            protocols on an interface basis. \n
                            If this field is set to FFH, the device uses a
                            vendor-specific protocol on a device basis. 
                            */
  uint8_t  bMaxPacketSize0; /**< Maximum packet size for endpoint zero
                            (only 8, 16, 32, or 64 are valid). For HS devices
                            is fixed to 64.
                            */

  uint16_t idVendor; /**< Vendor ID (assigned by the USB-IF). */
  uint16_t idProduct; /**< Product ID (assigned by the manufacturer). */
  uint16_t bcdDevice; /**< Device release number in binary-coded decimal. */
  uint8_t  iManufacturer; /**< Index of string descriptor describing manufacturer. */
  uint8_t  iProduct; /**< Index of string descriptor describing product. */
  uint8_t  iSerialNumber; /**< Index of string descriptor describing the device’s 
                          serial number.
                          */
  uint8_t  bNumConfigurations; /**< Number of possible configurations. */
} ;
/** USB Standard Device Descriptor */
typedef struct _USB_DEVICE_DESCRIPTOR USB_DEVICE_DESCRIPTOR;

/** USB 2.0 Device Qualifier Descriptor */
PRE_PACK struct POST_PACK _USB_DEVICE_QUALIFIER_DESCRIPTOR
{
  uint8_t  bLength; /**< Size of descriptor */
  uint8_t  bDescriptorType; /**< Device Qualifier Type */
  uint16_t bcdUSB; /**< USB specification version number (e.g., 0200H for V2.00) */
  uint8_t  bDeviceClass; /**< Class Code */
  uint8_t  bDeviceSubClass; /**< SubClass Code */
  uint8_t  bDeviceProtocol; /**< Protocol Code */
  uint8_t  bMaxPacketSize0; /**< Maximum packet size for other speed */
  uint8_t  bNumConfigurations; /**< Number of Other-speed Configurations */
  uint8_t  bReserved; /**< Reserved for future use, must be zero */
} ;
/** USB 2.0 Device Qualifier Descriptor */
typedef struct _USB_DEVICE_QUALIFIER_DESCRIPTOR USB_DEVICE_QUALIFIER_DESCRIPTOR;

/** USB Standard Configuration Descriptor */
PRE_PACK struct POST_PACK _USB_CONFIGURATION_DESCRIPTOR
{
  uint8_t  bLength; /**< Size of this descriptor in bytes */
  uint8_t  bDescriptorType; /**< CONFIGURATION Descriptor Type*/
  uint16_t wTotalLength; /**< Total length of data returned for this
                          configuration. Includes the combined length
                          of all descriptors (configuration, interface,
                          endpoint, and class- or vendor-specific)
                          returned for this configuration.*/
  uint8_t  bNumInterfaces; /**< Number of interfaces supported by this configuration*/
  uint8_t  bConfigurationValue; /**< Value to use as an argument to the
                                SetConfiguration() request to select this 
                                configuration. */
  uint8_t  iConfiguration; /**< Index of string descriptor describing this
                            configuration*/
  uint8_t  bmAttributes; /**< Configuration characteristics \n
                          D7: Reserved (set to one)\n
                          D6: Self-powered \n
                          D5: Remote Wakeup \n
                          D4...0: Reserved (reset to zero) \n
                          D7 is reserved and must be set to one for
                          historical reasons. \n
                          A device configuration that uses power from
                          the bus and a local source reports a non-zero
                          value in bMaxPower to indicate the amount of
                          bus power required and sets D6. The actual
                          power source at runtime may be determined
                          using the GetStatus(DEVICE) request (see
                          USB 2.0 spec Section 9.4.5). \n
                          If a device configuration supports remote
                          wakeup, D5 is set to one.*/
  uint8_t  bMaxPower; /**< Maximum power consumption of the USB
                      device from the bus in this specific
                      configuration when the device is fully
                      operational. Expressed in 2 mA units
                      (i.e., 50 = 100 mA). \n
                      Note: A device configuration reports whether
                      the configuration is bus-powered or selfpowered.
                      Device status reports whether the
                      device is currently self-powered. If a device is
                      disconnected from its external power source, it
                      updates device status to indicate that it is no
                      longer self-powered. \n
                      A device may not increase its power draw
                      from the bus, when it loses its external power
                      source, beyond the amount reported by its
                      configuration. \n
                      If a device can continue to operate when
                      disconnected from its external power source, it
                      continues to do so. If the device cannot
                      continue to operate, it fails operations it can
                      no longer support. The USB System Software
                      may determine the cause of the failure by
                      checking the status and noting the loss of the
                      device’s power source.*/
} ;
/** USB Standard Configuration Descriptor */
typedef struct _USB_CONFIGURATION_DESCRIPTOR USB_CONFIGURATION_DESCRIPTOR;

/** USB Standard Interface Association Descriptor */
PRE_PACK struct POST_PACK _USB_IAD_DESCRIPTOR
{
  uint8_t  bLength; /**< Size of this descriptor in bytes*/
  uint8_t  bDescriptorType; /**< INTERFACE ASSOCIATION Descriptor Type*/
  uint8_t  bFirstInterface; /**< Interface number of the first interface that is
                            associated with this function.*/
  uint8_t  bInterfaceCount; /**< Number of contiguous interfaces that are
                            associated with this function. */
  uint8_t  bFunctionClass; /**< Class code (assigned by USB-IF). \n
                            A value of zero is not allowed in this descriptor.
                            If this field is FFH, the function class is vendorspecific.
                            All other values are reserved for assignment by
                            the USB-IF.*/
  uint8_t  bFunctionSubClass; /**< Subclass code (assigned by USB-IF). \n
                            If the bFunctionClass field is not set to FFH all
                            values are reserved for assignment by the USBIF.*/
  uint8_t  bFunctionProtocol; /**< Protocol code (assigned by the USB). \n
                                These codes are qualified by the values of the
                                bFunctionClass and bFunctionSubClass fields.*/
  uint8_t  iFunction; /**< Index of string descriptor describing this function.*/
} ;
/** USB Standard Interface Association Descriptor */
typedef struct _USB_IAD_DESCRIPTOR USB_IAD_DESCRIPTOR;

/** USB Standard Interface Descriptor */
PRE_PACK struct POST_PACK _USB_INTERFACE_DESCRIPTOR
{
  uint8_t  bLength; /**< Size of this descriptor in bytes*/
  uint8_t  bDescriptorType; /**< INTERFACE Descriptor Type*/
  uint8_t  bInterfaceNumber; /**< Number of this interface. Zero-based
                              value identifying the index in the array of
                              concurrent interfaces supported by this
                              configuration.*/
  uint8_t  bAlternateSetting; /**< Value used to select this alternate setting
                              for the interface identified in the prior field*/
  uint8_t  bNumEndpoints; /**< Number of endpoints used by this
                          interface (excluding endpoint zero). If this
                          value is zero, this interface only uses the
                          Default Control Pipe.*/
  uint8_t  bInterfaceClass; /**< Class code (assigned by the USB-IF). \n
                            A value of zero is reserved for future
                            standardization. \n
                            If this field is set to FFH, the interface
                            class is vendor-specific. \n
                            All other values are reserved for
                            assignment by the USB-IF.*/
  uint8_t  bInterfaceSubClass; /**< Subclass code (assigned by the USB-IF). \n
                              These codes are qualified by the value of
                              the bInterfaceClass field. \n
                              If the bInterfaceClass field is reset to zero,
                              this field must also be reset to zero. \n
                              If the bInterfaceClass field is not set to
                              FFH, all values are reserved for
                              assignment by the USB-IF.*/
  uint8_t  bInterfaceProtocol; /**< Protocol code (assigned by the USB). \n
                                These codes are qualified by the value of
                                the bInterfaceClass and the
                                bInterfaceSubClass fields. If an interface
                                supports class-specific requests, this code
                                identifies the protocols that the device
                                uses as defined by the specification of the
                                device class. \n
                                If this field is reset to zero, the device
                                does not use a class-specific protocol on
                                this interface. \n
                                If this field is set to FFH, the device uses
                                a vendor-specific protocol for this
                                interface.*/
  uint8_t  iInterface; /**< Index of string descriptor describing this interface*/
} ;
/** USB Standard Interface Descriptor */
typedef struct _USB_INTERFACE_DESCRIPTOR USB_INTERFACE_DESCRIPTOR;

/** USB Standard Endpoint Descriptor */
PRE_PACK struct POST_PACK _USB_ENDPOINT_DESCRIPTOR
{
  uint8_t  bLength; /**< Size of this descriptor in bytes*/
  uint8_t  bDescriptorType; /**< ENDPOINT Descriptor Type*/
  uint8_t  bEndpointAddress; /**< The address of the endpoint on the USB device
                            described by this descriptor. The address is
                            encoded as follows: \n
                            Bit 3...0: The endpoint number \n
                            Bit 6...4: Reserved, reset to zero \n
                            Bit 7: Direction, ignored for control endpoints
                            0 = OUT endpoint
                            1 = IN endpoint.  \n \sa USBD_ENDPOINT_ADR_Type*/
  uint8_t  bmAttributes; /**< This field describes the endpoint’s attributes when it is
                          configured using the bConfigurationValue. \n
                          Bits 1..0: Transfer Type
                          \li 00 = Control
                          \li 01 = Isochronous
                          \li 10 = Bulk
                          \li 11 = Interrupt  \n
                          If not an isochronous endpoint, bits 5..2 are reserved
                          and must be set to zero. If isochronous, they are
                          defined as follows: \n
                          Bits 3..2: Synchronization Type
                          \li 00 = No Synchronization
                          \li 01 = Asynchronous
                          \li 10 = Adaptive
                          \li 11 = Synchronous \n
                          Bits 5..4: Usage Type
                          \li 00 = Data endpoint
                          \li 01 = Feedback endpoint
                          \li 10 = Implicit feedback Data endpoint
                          \li 11 = Reserved \n
                          Refer to Chapter 5 of USB 2.0 specification for more information. \n
                          All other bits are reserved and must be reset to zero.
                          Reserved bits must be ignored by the host.
                         \n \sa USBD_EP_ATTR_Type*/
  uint16_t wMaxPacketSize; /**< Maximum packet size this endpoint is capable of
                          sending or receiving when this configuration is
                          selected. \n
                          For isochronous endpoints, this value is used to
                          reserve the bus time in the schedule, required for the
                          per-(micro)frame data payloads. The pipe may, on an
                          ongoing basis, actually use less bandwidth than that
                          reserved. The device reports, if necessary, the actual
                          bandwidth used via its normal, non-USB defined
                          mechanisms. \n
                          For all endpoints, bits 10..0 specify the maximum
                          packet size (in bytes). \n
                          For high-speed isochronous and interrupt endpoints: \n
                          Bits 12..11 specify the number of additional transaction
                          opportunities per microframe: \n
                          \li 00 = None (1 transaction per microframe)
                          \li 01 = 1 additional (2 per microframe)
                          \li 10 = 2 additional (3 per microframe)
                          \li 11 = Reserved \n
                          Bits 15..13 are reserved and must be set to zero.*/
  uint8_t  bInterval; /**< Interval for polling endpoint for data transfers.
                      Expressed in frames or microframes depending on the
                      device operating speed (i.e., either 1 millisecond or
                      125 µs units). 
                      \li For full-/high-speed isochronous endpoints, this value
                      must be in the range from 1 to 16. The bInterval value
                      is used as the exponent for a \f$ 2^(bInterval-1) \f$ value; e.g., a
                      bInterval of 4 means a period of 8 (\f$ 2^(4-1) \f$). 
                      \li For full-/low-speed interrupt endpoints, the value of
                      this field may be from 1 to 255.
                      \li For high-speed interrupt endpoints, the bInterval value
                      is used as the exponent for a \f$ 2^(bInterval-1) \f$ value; e.g., a
                      bInterval of 4 means a period of 8 (\f$ 2^(4-1) \f$) . This value
                      must be from 1 to 16.
                      \li For high-speed bulk/control OUT endpoints, the
                      bInterval must specify the maximum NAK rate of the
                      endpoint. A value of 0 indicates the endpoint never
                      NAKs. Other values indicate at most 1 NAK each
                      bInterval number of microframes. This value must be
                      in the range from 0 to 255. \n
                      Refer to Chapter 5 of USB 2.0 specification for more information.
                      */
} ;
/** USB Standard Endpoint Descriptor */
typedef struct _USB_ENDPOINT_DESCRIPTOR USB_ENDPOINT_DESCRIPTOR;

/** USB String Descriptor */
PRE_PACK struct POST_PACK _USB_STRING_DESCRIPTOR
{
  uint8_t  bLength; /**< Size of this descriptor in bytes*/
  uint8_t  bDescriptorType; /**< STRING Descriptor Type*/
  uint16_t bString/*[]*/; /**< UNICODE encoded string */
}  ;
/** USB String Descriptor */
typedef struct _USB_STRING_DESCRIPTOR USB_STRING_DESCRIPTOR;

/** USB Common Descriptor */
PRE_PACK struct POST_PACK _USB_COMMON_DESCRIPTOR
{
  uint8_t  bLength; /**< Size of this descriptor in bytes*/
  uint8_t  bDescriptorType; /**< Descriptor Type*/
} ;
/** USB Common Descriptor */
typedef struct _USB_COMMON_DESCRIPTOR USB_COMMON_DESCRIPTOR;

/** USB Other Speed Configuration */
PRE_PACK struct POST_PACK _USB_OTHER_SPEED_CONFIGURATION
{
  uint8_t  bLength; /**< Size of descriptor*/
  uint8_t  bDescriptorType; /**< Other_speed_Configuration Type*/
  uint16_t wTotalLength; /**< Total length of data returned*/
  uint8_t  bNumInterfaces; /**< Number of interfaces supported by this speed configuration*/
  uint8_t  bConfigurationValue; /**< Value to use to select configuration*/
  uint8_t  IConfiguration; /**< Index of string descriptor*/
  uint8_t  bmAttributes; /**< Same as Configuration descriptor*/
  uint8_t  bMaxPower; /**< Same as Configuration descriptor*/
} ;
/** USB Other Speed Configuration */
typedef struct _USB_OTHER_SPEED_CONFIGURATION USB_OTHER_SPEED_CONFIGURATION;

/** \ingroup USBD_Core 
 * USB device stack/module handle. 
 */
typedef void* USBD_HANDLE_T;

#define WBVAL(x) ((x) & 0xFF),(((x) >> 8) & 0xFF)
#define B3VAL(x) ((x) & 0xFF),(((x) >> 8) & 0xFF),(((x) >> 16) & 0xFF)

#define USB_DEVICE_DESC_SIZE        (sizeof(USB_DEVICE_DESCRIPTOR))
#define USB_CONFIGURATION_DESC_SIZE (sizeof(USB_CONFIGURATION_DESCRIPTOR))
#define USB_INTERFACE_DESC_SIZE     (sizeof(USB_INTERFACE_DESCRIPTOR))
#define USB_INTERFACE_ASSOC_DESC_SIZE   (sizeof(USB_IAD_DESCRIPTOR))
#define USB_ENDPOINT_DESC_SIZE      (sizeof(USB_ENDPOINT_DESCRIPTOR))
#define USB_DEVICE_QUALI_SIZE       (sizeof(USB_DEVICE_QUALIFIER_DESCRIPTOR))
#define USB_OTHER_SPEED_CONF_SIZE   (sizeof(USB_OTHER_SPEED_CONFIGURATION))

/** @}*/

#endif  /* __USBD_H__ */
