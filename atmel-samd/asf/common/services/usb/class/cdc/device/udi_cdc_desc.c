/**
 * \file
 *
 * \brief Default descriptors for a USB Device with a single interface CDC
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
#include "udd.h"
#include "udc_desc.h"
#include "udi_cdc.h"


/**
 * \defgroup udi_cdc_group_single_desc USB device descriptors for a single interface
 *
 * The following structures provide the USB device descriptors required for
 * USB Device with a single interface CDC.
 *
 * It is ready to use and do not require more definition.
 *
 * @{
 */

//! Two interfaces for a CDC device
#define  USB_DEVICE_NB_INTERFACE       (2*UDI_CDC_PORT_NB)

#ifdef USB_DEVICE_LPM_SUPPORT
# define USB_VERSION   USB_V2_1
#else
# define USB_VERSION   USB_V2_0
#endif

//! USB Device Descriptor
COMPILER_WORD_ALIGNED
UDC_DESC_STORAGE usb_dev_desc_t udc_device_desc = {
	.bLength                   = sizeof(usb_dev_desc_t),
	.bDescriptorType           = USB_DT_DEVICE,
	.bcdUSB                    = LE16(USB_VERSION),
#if UDI_CDC_PORT_NB > 1
	.bDeviceClass              = 0,
#else
	.bDeviceClass              = CDC_CLASS_DEVICE,
#endif
	.bDeviceSubClass           = 0,
	.bDeviceProtocol           = 0,
	.bMaxPacketSize0           = USB_DEVICE_EP_CTRL_SIZE,
	.idVendor                  = LE16(USB_DEVICE_VENDOR_ID),
	.idProduct                 = LE16(USB_DEVICE_PRODUCT_ID),
	.bcdDevice                 = LE16((USB_DEVICE_MAJOR_VERSION << 8)
			| USB_DEVICE_MINOR_VERSION),
#ifdef USB_DEVICE_MANUFACTURE_NAME
	.iManufacturer = 1,
#else
	.iManufacturer             = 0,  // No manufacture string
#endif
#ifdef USB_DEVICE_PRODUCT_NAME
	.iProduct = 2,
#else
	.iProduct                  = 0,  // No product string
#endif
#ifdef USB_DEVICE_SERIAL_NAME
	.iSerialNumber = 3,
#else
	.iSerialNumber             = 0,  // No serial string
#endif
	.bNumConfigurations = 1
};


#ifdef USB_DEVICE_HS_SUPPORT
//! USB Device Qualifier Descriptor for HS
COMPILER_WORD_ALIGNED
UDC_DESC_STORAGE usb_dev_qual_desc_t udc_device_qual = {
	.bLength                   = sizeof(usb_dev_qual_desc_t),
	.bDescriptorType           = USB_DT_DEVICE_QUALIFIER,
	.bcdUSB                    = LE16(USB_VERSION),
#if UDI_CDC_PORT_NB > 1
	.bDeviceClass              = 0,
#else
	.bDeviceClass              = CDC_CLASS_DEVICE,
#endif
	.bDeviceSubClass           = 0,
	.bDeviceProtocol           = 0,
	.bMaxPacketSize0           = USB_DEVICE_EP_CTRL_SIZE,
	.bNumConfigurations        = 1
};
#endif

#ifdef USB_DEVICE_LPM_SUPPORT
//! USB Device Qualifier Descriptor
COMPILER_WORD_ALIGNED
UDC_DESC_STORAGE usb_dev_lpm_desc_t udc_device_lpm = {
	.bos.bLength               = sizeof(usb_dev_bos_desc_t),
	.bos.bDescriptorType       = USB_DT_BOS,
	.bos.wTotalLength          = LE16(sizeof(usb_dev_bos_desc_t) + sizeof(usb_dev_capa_ext_desc_t)),
	.bos.bNumDeviceCaps        = 1,
	.capa_ext.bLength          = sizeof(usb_dev_capa_ext_desc_t),
	.capa_ext.bDescriptorType  = USB_DT_DEVICE_CAPABILITY,
	.capa_ext.bDevCapabilityType = USB_DC_USB20_EXTENSION,
	.capa_ext.bmAttributes     = USB_DC_EXT_LPM,
};
#endif

//! Structure for USB Device Configuration Descriptor
COMPILER_PACK_SET(1)
typedef struct {
	usb_conf_desc_t conf;
#if UDI_CDC_PORT_NB == 1
	udi_cdc_comm_desc_t udi_cdc_comm_0;
	udi_cdc_data_desc_t udi_cdc_data_0;
#else
#  define UDI_CDC_DESC_STRUCTURE(index, unused) \
	usb_iad_desc_t      udi_cdc_iad_##index; \
	udi_cdc_comm_desc_t udi_cdc_comm_##index; \
	udi_cdc_data_desc_t udi_cdc_data_##index;
	MREPEAT(UDI_CDC_PORT_NB, UDI_CDC_DESC_STRUCTURE, ~)
#  undef UDI_CDC_DESC_STRUCTURE
#endif
} udc_desc_t;
COMPILER_PACK_RESET()

//! USB Device Configuration Descriptor filled for full and high speed
COMPILER_WORD_ALIGNED
UDC_DESC_STORAGE udc_desc_t udc_desc_fs = {
	.conf.bLength              = sizeof(usb_conf_desc_t),
	.conf.bDescriptorType      = USB_DT_CONFIGURATION,
	.conf.wTotalLength         = LE16(sizeof(udc_desc_t)),
	.conf.bNumInterfaces       = USB_DEVICE_NB_INTERFACE,
	.conf.bConfigurationValue  = 1,
	.conf.iConfiguration       = 0,
	.conf.bmAttributes         = USB_CONFIG_ATTR_MUST_SET | USB_DEVICE_ATTR,
	.conf.bMaxPower            = USB_CONFIG_MAX_POWER(USB_DEVICE_POWER),
#if UDI_CDC_PORT_NB == 1
	.udi_cdc_comm_0            = UDI_CDC_COMM_DESC_0,
	.udi_cdc_data_0            = UDI_CDC_DATA_DESC_0_FS,
#else
#  define UDI_CDC_DESC_FS(index, unused) \
	.udi_cdc_iad_##index             = UDI_CDC_IAD_DESC_##index,\
	.udi_cdc_comm_##index            = UDI_CDC_COMM_DESC_##index,\
	.udi_cdc_data_##index            = UDI_CDC_DATA_DESC_##index##_FS,
	MREPEAT(UDI_CDC_PORT_NB, UDI_CDC_DESC_FS, ~)
#  undef UDI_CDC_DESC_FS
#endif
};

#ifdef USB_DEVICE_HS_SUPPORT
COMPILER_WORD_ALIGNED
UDC_DESC_STORAGE udc_desc_t udc_desc_hs = {
	.conf.bLength              = sizeof(usb_conf_desc_t),
	.conf.bDescriptorType      = USB_DT_CONFIGURATION,
	.conf.wTotalLength         = LE16(sizeof(udc_desc_t)),
	.conf.bNumInterfaces       = USB_DEVICE_NB_INTERFACE,
	.conf.bConfigurationValue  = 1,
	.conf.iConfiguration       = 0,
	.conf.bmAttributes         = USB_CONFIG_ATTR_MUST_SET | USB_DEVICE_ATTR,
	.conf.bMaxPower            = USB_CONFIG_MAX_POWER(USB_DEVICE_POWER),
#if UDI_CDC_PORT_NB == 1
	.udi_cdc_comm_0            = UDI_CDC_COMM_DESC_0,
	.udi_cdc_data_0            = UDI_CDC_DATA_DESC_0_HS,
#else
#  define UDI_CDC_DESC_HS(index, unused) \
	.udi_cdc_iad_##index             = UDI_CDC_IAD_DESC_##index, \
	.udi_cdc_comm_##index            = UDI_CDC_COMM_DESC_##index, \
	.udi_cdc_data_##index            = UDI_CDC_DATA_DESC_##index##_HS,
	MREPEAT(UDI_CDC_PORT_NB, UDI_CDC_DESC_HS, ~)
#  undef UDI_CDC_DESC_HS
#endif
};
#endif

/**
 * \name UDC structures which content all USB Device definitions
 */
//@{

//! Associate an UDI for each USB interface
UDC_DESC_STORAGE udi_api_t *udi_apis[USB_DEVICE_NB_INTERFACE] = {
#  define UDI_CDC_API(index, unused) \
	&udi_api_cdc_comm, \
	&udi_api_cdc_data,
	MREPEAT(UDI_CDC_PORT_NB, UDI_CDC_API, ~)
#  undef UDI_CDC_API
};

//! Add UDI with USB Descriptors FS & HS
UDC_DESC_STORAGE udc_config_speed_t udc_config_fs[1] = { {
	.desc          = (usb_conf_desc_t UDC_DESC_STORAGE*)&udc_desc_fs,
	.udi_apis = udi_apis,
}};
#ifdef USB_DEVICE_HS_SUPPORT
UDC_DESC_STORAGE udc_config_speed_t udc_config_hs[1] = { {
	.desc          = (usb_conf_desc_t UDC_DESC_STORAGE*)&udc_desc_hs,
	.udi_apis = udi_apis,
}};
#endif

//! Add all information about USB Device in global structure for UDC
UDC_DESC_STORAGE udc_config_t udc_config = {
	.confdev_lsfs = &udc_device_desc,
	.conf_lsfs = udc_config_fs,
#ifdef USB_DEVICE_HS_SUPPORT
	.confdev_hs = &udc_device_desc,
	.qualifier = &udc_device_qual,
	.conf_hs = udc_config_hs,
#endif
#ifdef USB_DEVICE_LPM_SUPPORT
	.conf_bos = &udc_device_lpm.bos,
#else
	.conf_bos = NULL,
#endif
};

//@}
//@}
