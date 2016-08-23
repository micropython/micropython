/**
 * \file
 *
 * \brief Common API for USB Device Interface
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

#ifndef _UDC_DESC_H_
#define _UDC_DESC_H_

#include "conf_usb.h"
#include "usb_protocol.h"
#include "udi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup udc_group
 * \defgroup udc_desc_group USB Device Descriptor
 *
 * @{
 */

/**
 * \brief Defines the memory's location of USB descriptors
 *
 * By default the Descriptor is stored in RAM
 * (UDC_DESC_STORAGE is defined empty).
 *
 * If you have need to free RAM space,
 * it is possible to put descriptor in flash in following case:
 * - USB driver authorize flash transfer (USBB on UC3 and USB on Mega)
 * - USB Device is not high speed (UDC no need to change USB descriptors)
 *
 * For UC3 application used "const".
 *
 * For Mega application used "code".
 */
#define  UDC_DESC_STORAGE
	// Descriptor storage in internal RAM
#if (defined UDC_DATA_USE_HRAM_SUPPORT)
#	if defined(__GNUC__)
#		define UDC_DATA(x)              COMPILER_WORD_ALIGNED __attribute__((__section__(".data_hram0")))
#		define UDC_BSS(x)               COMPILER_ALIGNED(x)   __attribute__((__section__(".bss_hram0")))
#	elif defined(__ICCAVR32__)
#		define UDC_DATA(x)              COMPILER_ALIGNED(x)   __data32
#		define UDC_BSS(x)               COMPILER_ALIGNED(x)   __data32
#	endif
#else
#	define UDC_DATA(x)              COMPILER_ALIGNED(x)
#	define UDC_BSS(x)               COMPILER_ALIGNED(x)
#endif



/**
 * \brief Configuration descriptor and UDI link for one USB speed
 */
typedef struct {
	//! USB configuration descriptor
	usb_conf_desc_t UDC_DESC_STORAGE *desc;
	//! Array of UDI API pointer
	udi_api_t UDC_DESC_STORAGE *UDC_DESC_STORAGE * udi_apis;
} udc_config_speed_t;


/**
 * \brief All information about the USB Device
 */
typedef struct {
	//! USB device descriptor for low or full speed
	usb_dev_desc_t UDC_DESC_STORAGE *confdev_lsfs;
	//! USB configuration descriptor and UDI API pointers for low or full speed
	udc_config_speed_t UDC_DESC_STORAGE *conf_lsfs;
#ifdef USB_DEVICE_HS_SUPPORT
	//! USB device descriptor for high speed
	usb_dev_desc_t UDC_DESC_STORAGE *confdev_hs;
	//! USB device qualifier, only use in high speed mode
	usb_dev_qual_desc_t UDC_DESC_STORAGE *qualifier;
	//! USB configuration descriptor and UDI API pointers for high speed
	udc_config_speed_t UDC_DESC_STORAGE *conf_hs;
#endif
	usb_dev_bos_desc_t UDC_DESC_STORAGE *conf_bos;
} udc_config_t;

//! Global variables of USB Device Descriptor and UDI links
extern UDC_DESC_STORAGE udc_config_t udc_config;

//@}

#ifdef __cplusplus
}
#endif
#endif // _UDC_DESC_H_
