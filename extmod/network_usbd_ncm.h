/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Blake W. Felt & Angus Gratton
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
 */

#ifndef MICROPY_INCLUDED_SHARED_TINYUSB_NETWORK_USBD_NCM_H
#define MICROPY_INCLUDED_SHARED_TINYUSB_NETWORK_USBD_NCM_H

#include "lwipopts.h"

// TinyUSB NCM CLASS CONFIGURATION, SEE "ncm.h" FOR PERFORMANCE TUNING

// Must be >> MTU
// Can be set to 2048 without impact
#define CFG_TUD_NCM_IN_NTB_MAX_SIZE (2 * TCP_MSS + 100)

// Must be >> MTU
// Can be set to smaller values if wNtbOutMaxDatagrams==1
#define CFG_TUD_NCM_OUT_NTB_MAX_SIZE (2 * TCP_MSS + 100)

// Number of NCM transfer blocks for reception side
#ifndef CFG_TUD_NCM_OUT_NTB_N
  #define CFG_TUD_NCM_OUT_NTB_N 1
#endif

// Number of NCM transfer blocks for transmission side
#ifndef CFG_TUD_NCM_IN_NTB_N
  #define CFG_TUD_NCM_IN_NTB_N 1
#endif

void network_usbd_ncm_init(void);
void network_usbd_ncm_service_traffic(void);

#endif // MICROPY_INCLUDED_SHARED_TINYUSB_NETWORK_USBD_NCM_H
