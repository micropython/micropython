/**************************************************************************/
/*!
    @file     tusb_config.h
    @author   hathach (tinyusb.org)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2013, hathach (tinyusb.org)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    INCLUDING NEGLIGENCE OR OTHERWISE ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
/**************************************************************************/

#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------+
// COMMON CONFIGURATION
// --------------------------------------------------------------------+
#ifndef CFG_TUSB_DEBUG
#define CFG_TUSB_DEBUG              0
#endif

/*------------- RTOS -------------*/
#ifndef CFG_TUSB_OS
#define CFG_TUSB_OS                 OPT_OS_NONE
#endif
// #define CFG_TUD_TASK_QUEUE_SZ     16

// --------------------------------------------------------------------+
// DEVICE CONFIGURATION
// --------------------------------------------------------------------+

#if USB_HIGHSPEED
#define CFG_TUSB_RHPORT0_MODE       (OPT_MODE_DEVICE | OPT_MODE_HIGH_SPEED)
#else
#define CFG_TUSB_RHPORT0_MODE       (OPT_MODE_DEVICE)
#endif

// Vendor name included in Inquiry response, max 8 bytes
#define CFG_TUD_MSC_VENDOR          USB_MANUFACTURER_8

// Product name included in Inquiry response, max 16 bytes
#define CFG_TUD_MSC_PRODUCT         USB_PRODUCT_16
#define CFG_TUD_ENDPOINT0_SIZE       64

// ------------- CLASS -------------//

// Will be set to 2 in supervisor.mk if CIRCUITPY_USB_CDC is set.
#ifndef CFG_TUD_CDC
#define CFG_TUD_CDC                 1
#endif

#define CFG_TUD_MSC                 CIRCUITPY_USB_MSC
#define CFG_TUD_HID                 CIRCUITPY_USB_HID
#define CFG_TUD_MIDI                CIRCUITPY_USB_MIDI
#define CFG_TUD_VENDOR              CIRCUITPY_USB_VENDOR
#define CFG_TUD_CUSTOM_CLASS        0

/*------------------------------------------------------------------*/
/* CLASS DRIVER
 *------------------------------------------------------------------*/

// Product revision string included in Inquiry response, max 4 bytes
#define CFG_TUD_MSC_PRODUCT_REV     "1.0"


// --------------------------------------------------------------------+
// USB RAM PLACEMENT
// --------------------------------------------------------------------+
#define CFG_TUSB_ATTR_USBRAM
#define CFG_TUSB_MEM_ALIGN          __attribute__ ((aligned(4)))


#ifdef __cplusplus
}
#endif

#endif /* _TUSB_CONFIG_H_ */
