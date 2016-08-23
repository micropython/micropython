/**
 * \file
 *
 * \brief USB protocol definitions.
 *
 * This file contains the USB definitions and data structures provided by the
 * USB 2.0 specification.
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

#ifndef _USB_PROTOCOL_H_
#define _USB_PROTOCOL_H_

#include "usb_atmel.h"

/**
 * \ingroup usb_group
 * \defgroup usb_protocol_group USB Protocol Definitions
 *
 * This module defines constants and data structures provided by the USB
 * 2.0 specification.
 *
 * @{
 */

//! Value for field bcdUSB
#define  USB_V2_0    0x0200 //!< USB Specification version 2.00
#define  USB_V2_1    0x0201 //!< USB Specification version 2.01

/*! \name Generic definitions (Class, subclass and protocol)
 */
//! @{
#define  NO_CLASS                0x00
#define  CLASS_VENDOR_SPECIFIC   0xFF
#define  NO_SUBCLASS             0x00
#define  NO_PROTOCOL             0x00
//! @}

//! \name IAD (Interface Association Descriptor) constants
//! @{
#define  CLASS_IAD               0xEF
#define  SUB_CLASS_IAD           0x02
#define  PROTOCOL_IAD            0x01
//! @}

/**
 * \brief USB request data transfer direction (bmRequestType)
 */
#define  USB_REQ_DIR_OUT         (0<<7) //!< Host to device
#define  USB_REQ_DIR_IN          (1<<7) //!< Device to host
#define  USB_REQ_DIR_MASK        (1<<7) //!< Mask

/**
 * \brief USB request types (bmRequestType)
 */
#define  USB_REQ_TYPE_STANDARD   (0<<5) //!< Standard request
#define  USB_REQ_TYPE_CLASS      (1<<5) //!< Class-specific request
#define  USB_REQ_TYPE_VENDOR     (2<<5) //!< Vendor-specific request
#define  USB_REQ_TYPE_MASK       (3<<5) //!< Mask

/**
 * \brief USB recipient codes (bmRequestType)
 */
#define  USB_REQ_RECIP_DEVICE    (0<<0) //!< Recipient device
#define  USB_REQ_RECIP_INTERFACE (1<<0) //!< Recipient interface
#define  USB_REQ_RECIP_ENDPOINT  (2<<0) //!< Recipient endpoint
#define  USB_REQ_RECIP_OTHER     (3<<0) //!< Recipient other
#define  USB_REQ_RECIP_MASK      (0x1F) //!< Mask

/**
 * \brief Standard USB requests (bRequest)
 */
enum usb_reqid {
	USB_REQ_GET_STATUS = 0,
	USB_REQ_CLEAR_FEATURE = 1,
	USB_REQ_SET_FEATURE = 3,
	USB_REQ_SET_ADDRESS = 5,
	USB_REQ_GET_DESCRIPTOR = 6,
	USB_REQ_SET_DESCRIPTOR = 7,
	USB_REQ_GET_CONFIGURATION = 8,
	USB_REQ_SET_CONFIGURATION = 9,
	USB_REQ_GET_INTERFACE = 10,
	USB_REQ_SET_INTERFACE = 11,
	USB_REQ_SYNCH_FRAME = 12,
};

/**
 * \brief Standard USB device status flags
 *
 */
enum usb_device_status {
	USB_DEV_STATUS_BUS_POWERED = 0,
	USB_DEV_STATUS_SELF_POWERED = 1,
	USB_DEV_STATUS_REMOTEWAKEUP = 2
};

/**
 * \brief Standard USB Interface status flags
 *
 */
enum usb_interface_status {
	USB_IFACE_STATUS_RESERVED = 0
};

/**
 * \brief Standard USB endpoint status flags
 *
 */
enum usb_endpoint_status {
	USB_EP_STATUS_HALTED = 1,
};

/**
 * \brief Standard USB device feature flags
 *
 * \note valid for SetFeature request.
 */
enum usb_device_feature {
	USB_DEV_FEATURE_REMOTE_WAKEUP = 1, //!< Remote wakeup enabled
	USB_DEV_FEATURE_TEST_MODE = 2,     //!< USB test mode
	USB_DEV_FEATURE_OTG_B_HNP_ENABLE = 3,
	USB_DEV_FEATURE_OTG_A_HNP_SUPPORT = 4,
	USB_DEV_FEATURE_OTG_A_ALT_HNP_SUPPORT = 5
};

/**
 * \brief Test Mode possible on HS USB device
 *
 * \note valid for USB_DEV_FEATURE_TEST_MODE request.
 */
enum usb_device_hs_test_mode {
	USB_DEV_TEST_MODE_J = 1,
	USB_DEV_TEST_MODE_K = 2,
	USB_DEV_TEST_MODE_SE0_NAK = 3,
	USB_DEV_TEST_MODE_PACKET = 4,
	USB_DEV_TEST_MODE_FORCE_ENABLE = 5,
};

/**
 * \brief Standard USB endpoint feature/status flags
 */
enum usb_endpoint_feature {
	USB_EP_FEATURE_HALT = 0,
};

/**
 * \brief Standard USB Test Mode Selectors
 */
enum usb_test_mode_selector {
	USB_TEST_J = 0x01,
	USB_TEST_K = 0x02,
	USB_TEST_SE0_NAK = 0x03,
	USB_TEST_PACKET = 0x04,
	USB_TEST_FORCE_ENABLE = 0x05,
};

/**
 * \brief Standard USB descriptor types
 */
enum usb_descriptor_type {
	USB_DT_DEVICE = 1,
	USB_DT_CONFIGURATION = 2,
	USB_DT_STRING = 3,
	USB_DT_INTERFACE = 4,
	USB_DT_ENDPOINT = 5,
	USB_DT_DEVICE_QUALIFIER = 6,
	USB_DT_OTHER_SPEED_CONFIGURATION = 7,
	USB_DT_INTERFACE_POWER = 8,
	USB_DT_OTG = 9,
	USB_DT_IAD = 0x0B,
	USB_DT_BOS = 0x0F,
	USB_DT_DEVICE_CAPABILITY = 0x10,
};

/**
 * \brief USB Device Capability types
 */
enum usb_capability_type {
	USB_DC_USB20_EXTENSION = 0x02,
};

/**
 * \brief USB Device Capability - USB 2.0 Extension
 * To fill bmAttributes field of usb_capa_ext_desc_t structure.
 */
enum usb_capability_extension_attr {
	USB_DC_EXT_LPM  = 0x00000002,
};

#define HIRD_50_US    0
#define HIRD_125_US   1
#define HIRD_200_US   2
#define HIRD_275_US   3
#define HIRD_350_US   4
#define HIRD_425_US   5
#define HIRD_500_US   6
#define HIRD_575_US  7
#define HIRD_650_US  8
#define HIRD_725_US  9
#define HIRD_800_US  10
#define HIRD_875_US  11
#define HIRD_950_US  12
#define HIRD_1025_US  13
#define HIRD_1100_US  14
#define HIRD_1175_US  15

/** Fields definition from a LPM TOKEN  */
#define  USB_LPM_ATTRIBUT_BLINKSTATE_MASK      (0xF << 0)
#define  USB_LPM_ATTRIBUT_FIRD_MASK            (0xF << 4)
#define  USB_LPM_ATTRIBUT_REMOTEWAKE_MASK      (1 << 8)
#define  USB_LPM_ATTRIBUT_BLINKSTATE(value)    ((value & 0xF) << 0)
#define  USB_LPM_ATTRIBUT_FIRD(value)          ((value & 0xF) << 4)
#define  USB_LPM_ATTRIBUT_REMOTEWAKE(value)    ((value & 1) << 8)
#define  USB_LPM_ATTRIBUT_BLINKSTATE_L1        USB_LPM_ATTRIBUT_BLINKSTATE(1)

/**
 * \brief Standard USB endpoint transfer types
 */
enum usb_ep_type {
	USB_EP_TYPE_CONTROL = 0x00,
	USB_EP_TYPE_ISOCHRONOUS = 0x01,
	USB_EP_TYPE_BULK = 0x02,
	USB_EP_TYPE_INTERRUPT = 0x03,
	USB_EP_TYPE_MASK = 0x03,
};

/**
 * \brief Standard USB language IDs for string descriptors
 */
enum usb_langid {
	USB_LANGID_EN_US = 0x0409, //!< English (United States)
};

/**
 * \brief Mask selecting the index part of an endpoint address
 */
#define  USB_EP_ADDR_MASK     0x0f

//! \brief USB address identifier
typedef uint8_t usb_add_t;

/**
 * \brief Endpoint transfer direction is IN
 */
#define  USB_EP_DIR_IN        0x80

/**
 * \brief Endpoint transfer direction is OUT
 */
#define  USB_EP_DIR_OUT       0x00

//! \brief Endpoint identifier
typedef uint8_t usb_ep_t;

/**
 * \brief Maximum length in bytes of a USB descriptor
 *
 * The maximum length of a USB descriptor is limited by the 8-bit
 * bLength field.
 */
#define  USB_MAX_DESC_LEN     255

/*
 * 2-byte alignment requested for all USB structures.
 */
COMPILER_PACK_SET(1)

/**
 * \brief A USB Device SETUP request
 *
 * The data payload of SETUP packets always follows this structure.
 */
typedef struct {
	uint8_t bmRequestType;
	uint8_t bRequest;
	le16_t wValue;
	le16_t wIndex;
	le16_t wLength;
} usb_setup_req_t;

/**
 * \brief Standard USB device descriptor structure
 */
typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	le16_t bcdUSB;
	uint8_t bDeviceClass;
	uint8_t bDeviceSubClass;
	uint8_t bDeviceProtocol;
	uint8_t bMaxPacketSize0;
	le16_t idVendor;
	le16_t idProduct;
	le16_t bcdDevice;
	uint8_t iManufacturer;
	uint8_t iProduct;
	uint8_t iSerialNumber;
	uint8_t bNumConfigurations;
} usb_dev_desc_t;

/**
 * \brief Standard USB device qualifier descriptor structure
 *
 * This descriptor contains information about the device when running at
 * the "other" speed (i.e. if the device is currently operating at high
 * speed, this descriptor can be used to determine what would change if
 * the device was operating at full speed.)
 */
typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	le16_t bcdUSB;
	uint8_t bDeviceClass;
	uint8_t bDeviceSubClass;
	uint8_t bDeviceProtocol;
	uint8_t bMaxPacketSize0;
	uint8_t bNumConfigurations;
	uint8_t bReserved;
} usb_dev_qual_desc_t;

/**
 * \brief USB Device BOS descriptor structure
 *
 * The BOS descriptor (Binary device Object Store) defines a root
 * descriptor that is similar to the configuration descriptor, and is
 * the base descriptor for accessing a family of related descriptors.
 * A host can read a BOS descriptor and learn from the wTotalLength field
 * the entire size of the device-level descriptor set, or it can read in
 * the entire BOS descriptor set of device capabilities.
 * The host accesses this descriptor using the GetDescriptor() request.
 * The descriptor type in the GetDescriptor() request is set to BOS.
 */
typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	le16_t  wTotalLength;
	uint8_t bNumDeviceCaps;
} usb_dev_bos_desc_t;


/**
 * \brief USB Device Capabilities - USB 2.0 Extension Descriptor structure
 *
 * Defines the set of USB 1.1-specific device level capabilities.
 */
typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDevCapabilityType;
	le32_t  bmAttributes;
} usb_dev_capa_ext_desc_t;

/**
 * \brief USB Device LPM Descriptor structure
 *
 * The BOS descriptor and capabilities descriptors for LPM.
 */
typedef struct {
	usb_dev_bos_desc_t bos;
	usb_dev_capa_ext_desc_t capa_ext;
} usb_dev_lpm_desc_t;

/**
 * \brief Standard USB Interface Association Descriptor structure
 */
typedef struct {
	uint8_t bLength;          //!< size of this descriptor in bytes
	uint8_t bDescriptorType;  //!< INTERFACE descriptor type
	uint8_t bFirstInterface;  //!< Number of interface
	uint8_t bInterfaceCount;  //!< value to select alternate setting
	uint8_t bFunctionClass;   //!< Class code assigned by the USB
	uint8_t bFunctionSubClass;//!< Sub-class code assigned by the USB
	uint8_t bFunctionProtocol;//!< Protocol code assigned by the USB
	uint8_t iFunction;        //!< Index of string descriptor
} usb_association_desc_t;


/**
 * \brief Standard USB configuration descriptor structure
 */
typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	le16_t wTotalLength;
	uint8_t bNumInterfaces;
	uint8_t bConfigurationValue;
	uint8_t iConfiguration;
	uint8_t bmAttributes;
	uint8_t bMaxPower;
} usb_conf_desc_t;


#define  USB_CONFIG_ATTR_MUST_SET         (1 << 7) //!< Must always be set
#define  USB_CONFIG_ATTR_BUS_POWERED      (0 << 6) //!< Bus-powered
#define  USB_CONFIG_ATTR_SELF_POWERED     (1 << 6) //!< Self-powered
#define  USB_CONFIG_ATTR_REMOTE_WAKEUP    (1 << 5) //!< remote wakeup supported

#define  USB_CONFIG_MAX_POWER(ma)         (((ma) + 1) / 2) //!< Max power in mA

/**
 * \brief Standard USB association descriptor structure
 */
typedef struct {
	uint8_t bLength;              //!< Size of this descriptor in bytes
	uint8_t bDescriptorType;      //!< Interface descriptor type
	uint8_t bFirstInterface;      //!< Number of interface
	uint8_t bInterfaceCount;      //!< value to select alternate setting
	uint8_t bFunctionClass;       //!< Class code assigned by the USB
	uint8_t bFunctionSubClass;    //!< Sub-class code assigned by the USB
	uint8_t bFunctionProtocol;    //!< Protocol code assigned by the USB
	uint8_t iFunction;            //!< Index of string descriptor
} usb_iad_desc_t;

/**
 * \brief Standard USB interface descriptor structure
 */
typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bInterfaceNumber;
	uint8_t bAlternateSetting;
	uint8_t bNumEndpoints;
	uint8_t bInterfaceClass;
	uint8_t bInterfaceSubClass;
	uint8_t bInterfaceProtocol;
	uint8_t iInterface;
} usb_iface_desc_t;

/**
 * \brief Standard USB endpoint descriptor structure
 */
typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bEndpointAddress;
	uint8_t bmAttributes;
	le16_t wMaxPacketSize;
	uint8_t bInterval;
} usb_ep_desc_t;


/**
 * \brief A standard USB string descriptor structure
 */
typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
} usb_str_desc_t;

typedef struct {
	usb_str_desc_t desc;
	le16_t string[1];
} usb_str_lgid_desc_t;

COMPILER_PACK_RESET()

//! @}

#endif /* _USB_PROTOCOL_H_ */
