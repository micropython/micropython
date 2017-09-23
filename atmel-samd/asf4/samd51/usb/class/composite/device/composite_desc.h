/**
 * \file
 *
 * \brief USB Device Stack Composite Class Descriptor Setting.
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

#ifndef USBDF_COMPOSITE_DESC_H_
#define USBDF_COMPOSITE_DESC_H_

#include "usb_protocol.h"
#include "usbd_composite_config.h"

#if CONF_USB_COMPOSITE_CDC_ACM_EN == 1
#define CONF_CDC_ACM_IFC_LEN 66
#define CONF_CDC_ACM_IFC_NUM 2
#define CONF_USB_COMPOSITE_CDC_ACM_COMM_BIFCNUM 0
#define CONF_USB_COMPOSITE_CDC_ACM_DATA_BIFCNUM 1
#define CONF_CDC_ACM_IFC_DESC                                                                                          \
	USB_IAD_DESC_BYTES(                                                                                                \
	    CONF_USB_COMPOSITE_CDC_ACM_COMM_BIFCNUM, 0x02, CDC_CLASS_DEVICE, CDC_SUBCLASS_ACM, CDC_PROTOCOL_V25TER, 0x00)  \
	,                                                                                                                  \
	    USB_IFACE_DESC_BYTES(CONF_USB_COMPOSITE_CDC_ACM_COMM_BIFCNUM,                                                  \
	                         0x00,                                                                                     \
	                         0x01,                                                                                     \
	                         CDC_CLASS_DEVICE,                                                                         \
	                         CDC_SUBCLASS_ACM,                                                                         \
	                         CDC_PROTOCOL_V25TER,                                                                      \
	                         0x00),                                                                                    \
	    USB_CDC_HDR_DESC_BYTES(0x1001), USB_CDC_CALL_MGMT_DESC_BYTES(0x01, 0x00), USB_CDC_ACM_DESC_BYTES(0x02),        \
	    USB_CDC_UNION_DESC_BYTES(CONF_USB_COMPOSITE_CDC_ACM_COMM_BIFCNUM, 0x01),                                       \
	    USB_ENDP_DESC_BYTES(                                                                                           \
	        CONF_USB_COMPOSITE_CDC_ACM_COMM_INT_EPADDR, 3, CONF_USB_COMPOSITE_CDC_ACM_COMM_INT_MAXPKSZ, 10),           \
	    USB_IFACE_DESC_BYTES(CONF_USB_COMPOSITE_CDC_ACM_DATA_BIFCNUM, 0x00, 2, 0x0A, 0x00, 0x00, 0x00),                \
	    USB_ENDP_DESC_BYTES(                                                                                           \
	        CONF_USB_COMPOSITE_CDC_ACM_DATA_BULKOUT_EPADDR, 2, CONF_USB_COMPOSITE_CDC_ACM_DATA_BULKOUT_MAXPKSZ, 0x00), \
	    USB_ENDP_DESC_BYTES(                                                                                           \
	        CONF_USB_COMPOSITE_CDC_ACM_DATA_BULKIN_EPADDR, 2, CONF_USB_COMPOSITE_CDC_ACM_DATA_BULKIN_MAXPKSZ, 0x00),
#else
#define CONF_CDC_ACM_IFC_LEN 0
#define CONF_CDC_ACM_IFC_NUM 0
#define CONF_USB_COMPOSITE_CDC_ACM_COMM_BIFCNUM -2
#define CONF_USB_COMPOSITE_CDC_ACM_DATA_BIFCNUM -1
#define CONF_CDC_ACM_IFC_DESC
#endif

#if CONF_USB_COMPOSITE_HID_MOUSE_EN == 1
#define CONF_HID_MOUSE_IFC_LEN 25
#define CONF_HID_MOUSE_IFC_NUM 1
#define CONF_USB_COMPOSITE_HID_MOUSE_BIFCNUM (CONF_USB_COMPOSITE_CDC_ACM_DATA_BIFCNUM + 1)
#define CONF_HID_MOUSE_IFC_DESC                                                                                        \
	USB_IFACE_DESC_BYTES(CONF_USB_COMPOSITE_HID_MOUSE_BIFCNUM, 0x00, 0x01, 0x03, 0x01, 0x02, 0x00)                     \
	, USB_HID_DESC_BYTES(0x09, 0x21, 0x01, 0x22, 0x34),                                                                \
	    USB_ENDP_DESC_BYTES(                                                                                           \
	        CONF_USB_COMPOSITE_HID_MOUSE_INTIN_EPADDR, 0x03, CONF_USB_COMPOSITE_HID_MOUSE_INTIN_MAXPKSZ, 10),
#else
#define CONF_HID_MOUSE_IFC_LEN 0
#define CONF_HID_MOUSE_IFC_NUM 0
#define CONF_USB_COMPOSITE_HID_MOUSE_BIFCNUM CONF_USB_COMPOSITE_CDC_ACM_DATA_BIFCNUM
#define CONF_HID_MOUSE_IFC_DESC
#endif

#if CONF_USB_COMPOSITE_HID_KEYBOARD_EN == 1
#define CONF_HID_KEYBOARD_IFC_LEN 32
#define CONF_HID_KEYBOARD_IFC_NUM 1
#define CONF_USB_COMPOSITE_HID_KEYBOARD_BIFCNUM (CONF_USB_COMPOSITE_HID_MOUSE_BIFCNUM + 1)
#define CONF_HID_KEYBOARD_IFC_DESC                                                                                     \
	USB_IFACE_DESC_BYTES(CONF_USB_COMPOSITE_HID_KEYBOARD_BIFCNUM, 0x00, 0x02, 0x03, 0x01, 0x01, 0x00)                  \
	, USB_HID_DESC_BYTES(0x09, 0x21, 0x01, 0x22, 59),                                                                  \
	    USB_ENDP_DESC_BYTES(                                                                                           \
	        CONF_USB_COMPOSITE_HID_KEYBOARD_INTIN_EPADDR, 0x03, CONF_USB_COMPOSITE_HID_KEYBOARD_INTIN_MAXPKSZ, 10),    \
	    USB_ENDP_DESC_BYTES(                                                                                           \
	        CONF_USB_COMPOSITE_HID_KEYBOARD_INTOUT_EPADDR, 0x03, CONF_USB_COMPOSITE_HID_KEYBOARD_INTOUT_MAXPKSZ, 10),
#else
#define CONF_HID_KEYBOARD_IFC_LEN 0
#define CONF_HID_KEYBOARD_IFC_NUM 0
#define CONF_USB_COMPOSITE_HID_KEYBOARD_BIFCNUM CONF_USB_COMPOSITE_HID_MOUSE_BIFCNUM
#define CONF_HID_KEYBOARD_IFC_DESC
#endif

#if CONF_USB_COMPOSITE_HID_GENERIC_EN == 1
#define CONF_HID_GENERIC_IFC_LEN 32
#define CONF_HID_GENERIC_IFC_NUM 1
#define CONF_USB_COMPOSITE_HID_GENERIC_BIFCNUM (CONF_USB_COMPOSITE_HID_KEYBOARD_BIFCNUM + 1)
#define CONF_HID_GENERIC_IFC_DESC                                                                                      \
	USB_IFACE_DESC_BYTES(CONF_USB_COMPOSITE_HID_GENERIC_BIFCNUM, 0x00, 0x02, 0x03, 0x00, 0x00, 0x00)                   \
	, USB_HID_DESC_BYTES(0x09, 0x21, 0x01, 0x22, CONF_USB_COMPOSITE_HID_GENERIC_REPORT_LEN),                           \
	    USB_ENDP_DESC_BYTES(                                                                                           \
	        CONF_USB_COMPOSITE_HID_GENERIC_INTIN_EPADDR, 0x03, CONF_USB_COMPOSITE_HID_GENERIC_INTIN_MAXPKSZ, 10),      \
	    USB_ENDP_DESC_BYTES(                                                                                           \
	        CONF_USB_COMPOSITE_HID_GENERIC_INTOUT_EPADDR, 0x03, CONF_USB_COMPOSITE_HID_GENERIC_INTOUT_MAXPKSZ, 10),
#else
#define CONF_HID_GENERIC_IFC_LEN 0
#define CONF_HID_GENERIC_IFC_NUM 0
#define CONF_USB_COMPOSITE_HID_GENERIC_BIFCNUM CONF_USB_COMPOSITE_HID_KEYBOARD_BIFCNUM
#define CONF_HID_GENERIC_IFC_DESC
#endif

#define CONF_USB_COMPOSITE_TOTAL_LEN                                                                                   \
	(USB_CONFIG_DESC_LEN + CONF_CDC_ACM_IFC_LEN + CONF_HID_MOUSE_IFC_LEN + CONF_HID_KEYBOARD_IFC_LEN                   \
	 + CONF_HID_GENERIC_IFC_LEN)

#define CONF_USB_COMPOSITE_IFC_NUM                                                                                     \
	(CONF_CDC_ACM_IFC_NUM + CONF_HID_MOUSE_IFC_NUM + CONF_HID_KEYBOARD_IFC_NUM + CONF_HID_GENERIC_IFC_NUM)

#define COMPOSITE_DEV_DESC                                                                                             \
	USB_DEV_DESC_BYTES(CONF_USB_COMPOSITE_BCDUSB,                                                                      \
	                   0xEF,                                                                                           \
	                   0x02,                                                                                           \
	                   0x01,                                                                                           \
	                   CONF_USB_COMPOSITE_BMAXPKSZ0,                                                                   \
	                   CONF_USB_COMPOSITE_IDVENDER,                                                                    \
	                   CONF_USB_COMPOSITE_IDPRODUCT,                                                                   \
	                   CONF_USB_COMPOSITE_BCDDEVICE,                                                                   \
	                   CONF_USB_COMPOSITE_IMANUFACT,                                                                   \
	                   CONF_USB_COMPOSITE_IPRODUCT,                                                                    \
	                   CONF_USB_COMPOSITE_ISERIALNUM,                                                                  \
	                   CONF_USB_COMPOSITE_BNUMCONFIG)

#define COMPOSITE_CFG_DESC                                                                                             \
	USB_CONFIG_DESC_BYTES(CONF_USB_COMPOSITE_TOTAL_LEN,                                                                \
	                      CONF_USB_COMPOSITE_IFC_NUM,                                                                  \
	                      CONF_USB_COMPOSITE_BCONFIGVAL,                                                               \
	                      CONF_USB_COMPOSITE_ICONFIG,                                                                  \
	                      CONF_USB_COMPOSITE_BMATTRI,                                                                  \
	                      CONF_USB_COMPOSITE_BMAXPOWER)

#define COMPOSITE_IFACE_DESCES                                                                                         \
	CONF_CDC_ACM_IFC_DESC                                                                                              \
	CONF_HID_MOUSE_IFC_DESC                                                                                            \
	CONF_HID_KEYBOARD_IFC_DESC                                                                                         \
	CONF_HID_GENERIC_IFC_DESC

/** USB Device descriptors and configuration descriptors */
#define COMPOSITE_DESCES_LS_FS                                                                                         \
	COMPOSITE_DEV_DESC, COMPOSITE_CFG_DESC, COMPOSITE_IFACE_DESCES CONF_USB_COMPOSITE_LANGUAGE_ID_STR_DESC,            \
	    CONF_USB_COMPOSITE_MANUFACTOR_STR_DESC, CONF_USB_COMPOSITE_PRODUCT_STR_DESC

#endif /* USBDF_COMPOSITE_DESC_H_ */
