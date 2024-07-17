/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef _ALIF_TUSB_CONFIG_H_
#define _ALIF_TUSB_CONFIG_H_

// --------------------------------------------------------------------+
// Board Specific Configuration
// --------------------------------------------------------------------+

#define CFG_TUSB_MCU OPT_MCU_NONE
// #define CFG_TUSB_RHPORT0_MODE OPT_MODE_DEVICE
#define CFG_TUSB_RHPORT0_MODE (OPT_MODE_DEVICE | OPT_MODE_HIGH_SPEED)
#define TUP_DCD_ENDPOINT_MAX 8
#define TUD_OPT_RHPORT 0

// --------------------------------------------------------------------
// COMMON CONFIGURATION
// --------------------------------------------------------------------

#define CFG_TUSB_OS           OPT_OS_NONE
#define CFG_TUSB_DEBUG        0

// Enable Device stack
#define CFG_TUD_ENABLED       (CORE_M55_HP)

// Default is max speed that hardware controller could support with on-chip PHY
#define CFG_TUD_MAX_SPEED OPT_MODE_HIGH_SPEED

/* USB DMA on some MCUs can only access a specific SRAM region with restriction on alignment.
 * Tinyusb use follows macros to declare transferring memory so that they can be put
 * into those specific section.
 */
#define CFG_TUSB_MEM_SECTION        __attribute__((section(".bss.sram0")))

#define CFG_TUSB_MEM_ALIGN          __attribute__ ((aligned(32)))

// --------------------------------------------------------------------
// DEVICE CONFIGURATION
// --------------------------------------------------------------------

#define CFG_TUD_ENDPOINT0_SIZE    64

// CDC Endpoint transfer buffer size, more is faster
#define CFG_TUD_CDC_EP_BUFSIZE  (4096)
#define CFG_TUD_CDC_RX_BUFSIZE  (4096)
#define CFG_TUD_CDC_TX_BUFSIZE  (4096)

#include "shared/tinyusb/tusb_config.h"

#endif /* _ALIF_TUSB_CONFIG_H_ */
