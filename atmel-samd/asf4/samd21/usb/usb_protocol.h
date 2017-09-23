/**
 * \file
 *
 * \brief USB protocol definitions.
 *
 * This file contains the USB definitions and data structures provided by the
 * USB 2.0 specification.
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 * Atmel AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef _USB_PROTOCOL_H_
#define _USB_PROTOCOL_H_

#include "usb_includes.h"

/**
 * \ingroup usb_group
 * \defgroup usb_protocol_group USB Protocol Definitions
 *
 * This module defines constants and data structures provided by the USB
 * 2.0 specification.
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __CC_ARM
#pragma anon_unions
#endif

/*! Value for field bcdUSB */
#define USB_V2_0 (0x0200) /*!< USB Specification version 2.00 */
#define USB_V2_1 (0x0201) /*!< USB Specification version 2.01 (support BOS) */

/*! \name Generic definitions (Class, subclass and protocol)
 */
/*! @{ */
#define USB_CLASS_NO (0x00)
#define USB_SUBCLASS_NO (0x00)
#define USB_PROTOCOL_NO (0x00)
/*! @} */

/*! \name IAD (Interface Association Descriptor) constants */
/*! @{ */
#define USB_CLASS_IAD (0xEF)
#define USB_SUBCLASS_IAD (0x02)
#define USB_PROTOCOL_IAD (0x01)
/*! @} */

/**
 * \brief USB request data transfer direction (bmRequestType)
 */
#define USB_REQT_DIR_OUT (0 << 7)  /*!< Host to device */
#define USB_REQT_DIR_H2D (0 << 7)  /*!< Host to device */
#define USB_REQT_DIR_IN (1 << 7)   /*!< Device to host */
#define USB_REQT_DIR_D2H (1 << 7)  /*!< Device to host */
#define USB_REQT_DIR_MASK (1 << 7) /*!< Mask */

/**
 * \brief USB request types (bmRequestType)
 */
#define USB_REQT_TYPE_STANDARD (0 << 5) /*!< Standard request */
#define USB_REQT_TYPE_CLASS (1 << 5)    /*!< Class-specific request */
#define USB_REQT_TYPE_VENDOR (2 << 5)   /*!< Vendor-specific request */
#define USB_REQT_TYPE_MASK (3 << 5)     /*!< Mask */

/**
 * \brief USB recipient codes (bmRequestType)
 */
#define USB_REQT_RECIP_DEVICE (0 << 0)    /*!< Recipient device */
#define USB_REQT_RECIP_INTERFACE (1 << 0) /*!< Recipient interface */
#define USB_REQT_RECIP_ENDPOINT (2 << 0)  /*!< Recipient endpoint */
#define USB_REQT_RECIP_OTHER (3 << 0)     /*!< Recipient other */
#define USB_REQT_RECIP_MASK (0x1F)        /*!< Mask */

/**
 * \brief Standard USB control transfer stages.
 */
enum usb_ctrl_stage { USB_SETUP_STAGE = 0, USB_DATA_STAGE = 1, USB_STATUS_STAGE = 2 };

/**
 * \brief Standard USB requests (bRequest)
 */
enum usb_req_code {
	USB_REQ_GET_STATUS    = 0,
	USB_REQ_CLEAR_FTR     = 1,
	USB_REQ_SET_FTR       = 3,
	USB_REQ_SET_ADDRESS   = 5,
	USB_REQ_GET_DESC      = 6,
	USB_REQ_SET_DESC      = 7,
	USB_REQ_GET_CONFIG    = 8,
	USB_REQ_SET_CONFIG    = 9,
	USB_REQ_GET_INTERFACE = 10,
	USB_REQ_SET_INTERFACE = 11,
	USB_REQ_SYNCH_FRAME   = 12,
	USB_REQ_SET_SEL       = 48,
	USB_REQ_ISOCH_DELAY   = 49
};

/**
 * \brief Standard USB device status flags
 *
 */
enum usb_dev_status {
	USB_DEV_STAT_BUS_POWERED  = 0,
	USB_DEV_STAT_SELF_POWERED = 1,
	USB_DEV_STAT_REMOTEWAKEUP = 2,
	USB_DEV_STAT_U1_ENABLE    = 4,
	USB_DEV_STAT_U2_ENABLE    = 8,
	USB_DEV_STAT_LTM_ENABLE   = 16
};

/**
 * \brief Standard USB Interface status flags
 *
 */
enum usb_interface_status {
	USB_IFACE_STAT_RESERVED       = 0,
	USB_IFACE_STAT_REMOTEWAKE_CAP = 1,
	USB_IFACE_STAT_REMOTEWAKE     = 2
};

/**
 * \brief Standard USB endpoint status flags
 *
 */
enum usb_endpoint_status { USB_EP_STAT_HALT = 1 };

/**
 * \brief Standard USB device feature flags
 *
 * \note valid for SetFeature request.
 */
enum usb_device_feature {
	USB_DEV_FTR_REMOTE_WAKEUP    = 1, /*!< Remote wakeup enabled */
	USB_DEV_FTR_TEST_MODE        = 2, /*!< USB test mode */
	USB_DEV_FTR_OTG_B_HNP_ENABLE = 3,
	USB_DEV_FTR_OTG_A_HNP_SP     = 4,
	USB_DEV_FTR_OTG_A_ALT_HNP_SP = 5,
	USB_DEV_FTR_U1_ENABLE        = 48,
	USB_DEV_FTR_U2_ENABLE        = 49,
	USB_DEV_FTR_LTM_ENABLE       = 50
};

/**
 * \brief Test Mode possible on HS USB device
 *
 * \note valid for USB_DEV_FTR_TEST_MODE request.
 */
enum usb_device_hs_test_mode {
	USB_DEV_TEST_MODE_J            = 1,
	USB_DEV_TEST_MODE_K            = 2,
	USB_DEV_TEST_MODE_SE0_NAK      = 3,
	USB_DEV_TEST_MODE_PACKET       = 4,
	USB_DEV_TEST_MODE_FORCE_ENABLE = 5
};

/**
 * \brief Standard Feature Selectors for Interface
 */
enum usb_iface_feature { USB_IFACE_FTR_FUNC_SUSP = 0 };

/**
 * \brief Standard USB endpoint feature/status flags
 */
enum usb_endpoint_feature { USB_EP_FTR_HALT = 0 };

/**
 * \brief Standard USB Test Mode Selectors
 */
enum usb_test_mode_selector {
	USB_TEST_J            = 0x01,
	USB_TEST_K            = 0x02,
	USB_TEST_SE0_NAK      = 0x03,
	USB_TEST_PACKET       = 0x04,
	USB_TEST_FORCE_ENABLE = 0x05
};

/**
 * \brief Standard USB descriptor types
 */
enum usb_descriptor_type {
	USB_DT_DEVICE             = 1,
	USB_DT_CONFIG             = 2,
	USB_DT_STRING             = 3,
	USB_DT_INTERFACE          = 4,
	USB_DT_ENDPOINT           = 5,
	USB_DT_DEVICE_QUALIFIER   = 6,
	USB_DT_OTHER_SPEED_CONFIG = 7,
	USB_DT_INTERFACE_POWER    = 8,
	USB_DT_OTG                = 9,
	USB_DT_DEBUG              = 10,
	USB_DT_IAD                = 11,
	USB_DT_BOS                = 15,
	USB_DT_DEV_CAP            = 16,
	USB_DT_SS_EP_COMPANION    = 48
};

/**
 * \brief Capability types
 */
enum usb_capability_type {
	USB_CAPT_WIRELESS     = 1,
	USB_CAPT_2_0_EXT      = 2,
	USB_CAPT_SUPER_SPEED  = 3,
	USB_CAPT_CONTAINER_ID = 4
};

/**
 * \brief USB 2.0 Extension attributes
 */
enum usb_2_0_ext_attr { USB_2_0_EXT_LPM_SP = 1 };

/**
 * \brief USB SuperSpeed Capability attributes
 */
enum usb_ss_cap_attr { USB_SS_LTM_SP };

/**
 * \brief USB Speed Supports
 */
enum usb_speed_sp {
	USB_SPEED_LOW_SP   = 1,
	USB_SPEED_LS_SP    = 1,
	USB_SPEED_FULL_SP  = 2,
	USB_SPEED_FS_SP    = 2,
	USB_SPEED_HIGH_SP  = 4,
	USB_SPEED_HS_SP    = 4,
	USB_SPEED_SUPER_SP = 8,
	USB_SPEED_SS_SP    = 8
};

/**
 * \brief Standard USB endpoint transfer types
 */
enum usb_ep_type {
	USB_EP_TYPE_CONTROL     = 0x00,
	USB_EP_TYPE_ISOCHRONOUS = 0x01,
	USB_EP_TYPE_BULK        = 0x02,
	USB_EP_TYPE_INTERRUPT   = 0x03,
	USB_EP_TYPE_MASK        = 0x03u
};

/**
 * \brief USB endpoint interrupt types
 */
enum usb_ep_int_type { USB_EP_INT_T_PERIODIC = 0x00u, USB_EP_INT_T_NOTIFICATION = 0x01u, USB_EP_INT_T_MASK = 0x03u };

/**
 * \brief Standard USB endpoint synchronization types
 */
enum usb_ep_sync_type {
	USB_EP_SYNC_T_NO       = 0x00u,
	USB_EP_SYNC_T_ASYNC    = 0x02u,
	USB_EP_SYNC_T_ADAPTIVE = 0x02u,
	USB_EP_SYNC_T_SYNC     = 0x03u,
	USB_EP_SYNC_T_MASK     = 0x03u
};

/**
 * \brief Standard USB endpoint usage types
 */
enum usb_ep_usage_type {
	USB_EP_USAGE_T_DATA          = 0x00u,
	USB_EP_USAGE_T_FEEDBACK      = 0x01u,
	USB_EP_USAGE_T_FEEDBACK_DATA = 0x02u,
	USB_EP_USAGE_T_MASK          = 0x03u
};

/**
 * \brief Standard USB language IDs for string descriptors
 */
enum usb_langid {
	USB_LANGID_EN_US = 0x0409 /*!< English (United States) */
};

/**
 * \brief Mask selecting the index part of an endpoint address
 */
#define USB_EP_ADDR_MASK 0x0f
/**
 * \brief Endpoint transfer direction is IN
 */
#define USB_EP_DIR_IN 0x80
/**
 * \brief Endpoint transfer direction is OUT
 */
#define USB_EP_DIR_OUT 0x00

/**
 * \brief Maximum length in bytes of a USB descriptor
 *
 * The maximum length of a USB descriptor is limited by the 8-bit
 * bLength field.
 */
#define USB_DESC_LEN_MAX 255

/*
 * 2-byte alignment requested for all USB structures.
 */
COMPILER_PACK_SET(1)

/**
 * \brief A USB Device SETUP request
 *
 * The data payload of SETUP packets always follows this structure.
 */
typedef struct usb_req {
	uint8_t bmRequestType;
	uint8_t bRequest;
	union {
		le16_t wValue;
		struct {
			uint8_t l;
			uint8_t h;
		} wValueBytes;
	};
	union {
		le16_t wIndex;
		struct {
			uint8_t l;
			uint8_t h;
		} wIndexBytes;
	};
	union {
		le16_t wLength;
		struct {
			uint8_t l;
			uint8_t h;
		} wLengthBytes;
	};
} usb_req_t;

/**
 * \brief Standard USB device descriptor structure
 */
typedef struct usb_dev_desc {
	uint8_t bLength;
	uint8_t bDescriptorType;
	le16_t  bcdUSB;
	uint8_t bDeviceClass;
	uint8_t bDeviceSubClass;
	uint8_t bDeviceProtocol;
	uint8_t bMaxPacketSize0;
	le16_t  idVendor;
	le16_t  idProduct;
	le16_t  bcdDevice;
	uint8_t iManufacturer;
	uint8_t iProduct;
	uint8_t iSerialNumber;
	uint8_t bNumConfigurations;
} usb_dev_desc_t;

/**
 * \brief Binary device Object Store (BOS) descriptor structure
 */
typedef struct usb_bos_desc {
	uint8_t bLength;
	uint8_t bDescriptorType;
	le16_t  wTotalLength;
	uint8_t bNumDeviceCaps;
} usb_bos_desc_t;

/**
 * \brief Device Capability Descriptor structure
 */
typedef struct usb_cap_desc {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDevCapabilityType;
	uint8_t Vars[1];
} usb_cap_desc_t;

/**
 * \brief USB 2.0 Extension Descriptor structure
 */
typedef struct usb_2_0_ext {
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint8_t  bDevCapabilityType;
	uint32_t bmAttributes;
} usb_2_0_ext_t;

/**
 * \brief LPM Device Capabilities descriptor structure
 */
typedef struct usb_2_0_ext usb_lpm_cap_desc_t;

/**
 * \brief SuperSpeed USB Device Capability structure
 */
typedef struct usb_ss_cap_desc {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDevCapabilityType;
	uint8_t bmAttributes;
	le16_t  wSpeedsSupported;
	uint8_t bFunctionalitySupport;
	uint8_t bU1DevExitLat;
	uint8_t bU2DevExitLat;
} usb_ss_cap_desc_t;

/**
 * \brief USB Container ID Descriptor structure
 */
typedef struct usb_container_id_desc {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDevCapabilityType;
	uint8_t bReserved;
	uint8_t ContainerID[16];
} usb_container_id_desc_t;

/**
 * \brief Standard USB device qualifier descriptor structure
 *
 * This descriptor contains information about the device when running at
 * the "other" speed (i.e. if the device is currently operating at high
 * speed, this descriptor can be used to determine what would change if
 * the device was operating at full speed.)
 */
typedef struct usb_dev_qual_desc {
	uint8_t bLength;
	uint8_t bDescriptorType;
	le16_t  bcdUSB;
	uint8_t bDeviceClass;
	uint8_t bDeviceSubClass;
	uint8_t bDeviceProtocol;
	uint8_t bMaxPacketSize0;
	uint8_t bNumConfigurations;
	uint8_t bReserved;
} usb_dev_qual_desc_t;

/**
 * \brief Standard USB configuration descriptor structure
 */
typedef struct usb_config_desc {
	uint8_t bLength;
	uint8_t bDescriptorType;
	le16_t  wTotalLength;
	uint8_t bNumInterfaces;
	uint8_t bConfigurationValue;
	uint8_t iConfiguration;
	uint8_t bmAttributes;
	uint8_t bMaxPower;
} usb_config_desc_t;

#define USB_CONFIG_ATTR_MUST_SET (1 << 7)      /*!< Must always be set */
#define USB_CONFIG_ATTR_BUS_POWERED (0 << 6)   /*!< Bus-powered */
#define USB_CONFIG_ATTR_SELF_POWERED (1 << 6)  /*!< Self-powered */
#define USB_CONFIG_ATTR_REMOTE_WAKEUP (1 << 5) /*!< remote wakeup supported */

#define USB_CONFIG_MAX_POWER(ma) (((ma) + 1) / 2) /*!< Max power in mA */

/**
 * \brief Standard USB association descriptor structure
 */
typedef struct usb_iad_desc {
	uint8_t bLength;           /*!< Size of this descriptor in bytes */
	uint8_t bDescriptorType;   /*!< Interface descriptor type */
	uint8_t bFirstInterface;   /*!< Number of interface */
	uint8_t bInterfaceCount;   /*!< value to select alternate setting */
	uint8_t bFunctionClass;    /*!< Class code assigned by the USB */
	uint8_t bFunctionSubClass; /*!< Sub-class code assigned by the USB */
	uint8_t bFunctionProtocol; /*!< Protocol code assigned by the USB */
	uint8_t iFunction;         /*!< Index of string descriptor */
} usb_iad_desc_t;

/**
 * \brief Standard USB interface descriptor structure
 */
typedef struct usb_iface_desc {
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
typedef struct usb_ep_desc {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bEndpointAddress;
	uint8_t bmAttributes;
	le16_t  wMaxPacketSize;
	uint8_t bInterval;
} usb_ep_desc_t;

/**
 * \brief SuperSpeed Endpoint Companion descriptor structure
 */
typedef struct usb_ss_ep_comp_desc {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bMaxBurst;
	uint8_t bmAttributes;
	le16_t  wBytesPerInterval;
} usb_ss_ep_comp_desc_t;

/**
 * \brief LPM Token bmAttributes structure
 */
typedef struct usb_lpm_attributes {
	uint8_t bLinkState : 4;
	uint8_t HIRD : 4;
	uint8_t bRemoteWake : 1;
	uint8_t Reserved : 2;
} usb_lpm_attributes_t;

/**
 * \brief A standard USB string descriptor structure
 */
typedef struct usb_str_desc {
	uint8_t bLength;
	uint8_t bDescriptorType;
} usb_str_desc_t;

typedef struct usb_str_langid_desc {
	usb_str_desc_t desc;
	le16_t         string[1];
} usb_str_langid_desc_t;

COMPILER_PACK_RESET()

/** \name Macros to build USB standard descriptors */
/*@{*/

/** Build bytes for USB device descriptor. */
#define USB_DEV_DESC_BYTES(bcdUSB,                                                                                     \
                           bDeviceClass,                                                                               \
                           bDeviceSubClass,                                                                            \
                           bDeviceProtocol,                                                                            \
                           bMaxPacketSize0,                                                                            \
                           idVendor,                                                                                   \
                           idProduct,                                                                                  \
                           bcdDevice,                                                                                  \
                           iManufacturer,                                                                              \
                           iProduct,                                                                                   \
                           iSerialNumber,                                                                              \
                           bNumConfigurations)                                                                         \
	18,       /* bLength */                                                                                            \
	    0x01, /* bDescriptorType: DEVICE */                                                                            \
	    LE_BYTE0(bcdUSB), LE_BYTE1(bcdUSB), bDeviceClass, bDeviceSubClass, bDeviceProtocol, bMaxPacketSize0,           \
	    LE_BYTE0(idVendor), LE_BYTE1(idVendor), LE_BYTE0(idProduct), LE_BYTE1(idProduct), LE_BYTE0(bcdDevice),         \
	    LE_BYTE1(bcdDevice), iManufacturer, iProduct, iSerialNumber, bNumConfigurations

#define USB_DEV_DESC_LEN 18

/** Build bytes for USB configuration descriptor. */
#define USB_CONFIG_DESC_BYTES(                                                                                         \
    wTotalLength, bNumInterfaces, bConfigurationValue, iConfiguration, bmAttributes, bMaxPower)                        \
	9,        /* bLength */                                                                                            \
	    0x02, /* bDescriptorType: CONFIGURATION */                                                                     \
	    LE_BYTE0(wTotalLength), LE_BYTE1(wTotalLength), bNumInterfaces, bConfigurationValue, iConfiguration,           \
	    bmAttributes, bMaxPower

#define USB_CONFIG_DESC_LEN 9

/** Build bytes for USB IAD descriptor. */
#define USB_IAD_DESC_BYTES(                                                                                            \
    bFirstInterface, bInterfaceCount, bFunctionClass, bFunctionSubClass, bFunctionProtocol, iFunction)                 \
	8,              /* bLength */                                                                                      \
	    USB_DT_IAD, /* bDescriptorType */                                                                              \
	    bFirstInterface, bInterfaceCount, bFunctionClass, bFunctionSubClass, bFunctionProtocol, iFunction

#define USB_IAD_DESC_LEN 8

/** Build bytes for USB interface descriptor. */
#define USB_IFACE_DESC_BYTES(bInterfaceNumber,                                                                         \
                             bAlternateSetting,                                                                        \
                             bNumEndpoints,                                                                            \
                             bInterfaceClass,                                                                          \
                             bInterfaceSubClass,                                                                       \
                             bInterfaceProtocol,                                                                       \
                             iInterface)                                                                               \
	9,        /* bLength */                                                                                            \
	    0x04, /* bDescriptorType: INTERFACE */                                                                         \
	    bInterfaceNumber, bAlternateSetting, bNumEndpoints, bInterfaceClass, bInterfaceSubClass, bInterfaceProtocol,   \
	    iInterface

#define USB_IFACE_DESC_LEN 9

/** Build bytes for USB endpoint descriptor. */
#define USB_ENDP_DESC_BYTES(bEndpointAddress, bmAttributes, wMaxPacketSize, bInterval)                                 \
	7,        /* bLength */                                                                                            \
	    0x05, /* bDescriptorType: ENDPOINT */                                                                          \
	    bEndpointAddress, bmAttributes, LE_BYTE0(wMaxPacketSize), LE_BYTE1(wMaxPacketSize), bInterval

#define USB_ENDP_DESC_LEN 7

/*@}*/

/** \brief Get a word (calculate by little endian 16-bit data)
 *  \param[in] ptr Byte pointer to the address to get data
 *  \return a 16-bit word
 */
static inline uint16_t usb_get_u16(const uint8_t *ptr)
{
	return (ptr[0] + (ptr[1] << 8));
}

/** \brief Get a double word (calculate by little endian 32-bit data)
 *  \param[in] ptr Byte pointer to the address to get data
 *  \return a 32-bit word
 */
static inline uint32_t usb_get_u32(const uint8_t *ptr)
{
	return (ptr[0] + (ptr[1] << 8) + (ptr[2] << 16) + (ptr[3] << 24));
}

/** \brief Get descriptor length
 *  \param[in] desc Byte pointer to the descriptor start address
 *  \return descriptor length
 */
static inline uint8_t usb_desc_len(const uint8_t *desc)
{
	return desc[0];
}

/** \brief Get descriptor type
 *  \param[in] desc Byte pointer to the descriptor start address
 *  \return descriptor type
 */
static inline uint8_t usb_desc_type(const uint8_t *desc)
{
	return desc[1];
}

/** \brief Get next USB descriptor
 *  \param[in] desc Byte pointer to the descriptor start address
 *  \return Byte pointer to the next descriptor
 */
static inline uint8_t *usb_desc_next(uint8_t *desc)
{
	return (desc + usb_desc_len(desc));
}

/** \brief Get idVendor of USB Device Descriptor
 *  \param[in] dev_desc Byte pointer to the descriptor start address
 *  \return 16-bit idVendor value
 */
static inline uint16_t usb_dev_desc_vid(const uint8_t *dev_desc)
{
	return usb_get_u16(dev_desc + 8);
}

/** \brief Get idProduct of USB Device Descriptor
 *  \param[in] dev_desc Byte pointer to the descriptor start address
 *  \return 16-bit idProduct value
 */
static inline uint16_t usb_dev_desc_pid(const uint8_t *dev_desc)
{
	return usb_get_u16(dev_desc + 10);
}

/** \brief Get wTotalLength of USB Configuration Descriptor
 *  \param[in] cfg_desc Byte pointer to the descriptor start address
 *  \return 16-bit total length of configuration list
 */
static inline uint16_t usb_cfg_desc_total_len(const uint8_t *cfg_desc)
{
	return usb_get_u16(cfg_desc + 2);
}

/** \brief Get Next USB Descriptor After the Configuration Descriptors list
 *  \param[in] cfg_desc Byte pointer to the descriptor start address
 *  \return Byte pointer to descriptor after configuration end
 */
static inline uint8_t *usb_cfg_desc_next(uint8_t *cfg_desc)
{
	return (cfg_desc + usb_cfg_desc_total_len(cfg_desc));
}

/** \brief Find specific USB Descriptor by its type
 *  \param[in] desc Byte pointer to the descriptor start address
 *  \param[in] eof  Byte pointer to the descriptor end address
 *  \param[in] type The descriptor type expected
 *  \return Pointer to the descriptor
 *  \retval NULL if not found
 */
uint8_t *usb_find_desc(uint8_t *desc, uint8_t *eof, uint8_t type);

/** Get interface descriptor next to the specified one (by interface number)
 *  \param[in] desc Byte pointer to the descriptor start address
 *  \param[in] eof  Byte pointer to the descriptor end address
 *  \param[in] iface_n The interface number to check
 *  \return Pointer to the descriptor
 *  \retval >= eof if not found
 */
uint8_t *usb_find_iface_after(uint8_t *desc, uint8_t *eof, uint8_t iface_n);

/** Find endpoint descriptor, breaks if interface descriptor detected
 *  \param[in] desc Byte pointer to the descriptor start address
 *  \param[in] eof  Byte pointer to the descriptor end address
 *  \return Pointer to the descriptor
 *  \retval NULL if not found
 */
uint8_t *usb_find_ep_desc(uint8_t *desc, uint8_t *eof);

/** Find configuration descriptor by its configuration number
 *  \param[in] desc Byte pointer to the descriptor start address
 *  \param[in] eof  Byte pointer to the descriptor end address
 *  \param[in] cfg_value The configure value expected
 *  \return Pointer to the descriptor
 *  \retval NULL if not found
 */
uint8_t *usb_find_cfg_desc(uint8_t *desc, uint8_t *eof, uint8_t cfg_value);

/** Find string descriptor by its index
 *  \param[in] desc Byte pointer to the descriptor start address
 *  \param[in] eof  Byte pointer to the descriptor end address
 *  \param[in] str_index The string index expected
 *  \return Pointer to the descriptor
 *  \retval NULL if not found
 */
uint8_t *usb_find_str_desc(uint8_t *desc, uint8_t *eof, uint8_t str_index);

#ifdef __cplusplus
}
#endif

/*! @} */

#endif /* _USB_PROTOCOL_H_ */
