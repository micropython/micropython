/*
 * Wrapper for Micro Python SPI functions for CC3100 Driver
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Kimball Johnson
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

#ifndef __CC3100_SPI_WRAPPER_H__
#define	__CC3100_SPI_WRAPPER_H__

#ifdef	__cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdarg.h>
#include <errno.h>


#include "py/nlr.h"
#include "py/objtuple.h"
#include "py/objlist.h"
#include "py/stream.h"
#include "py/runtime.h"
#include "netutils.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "spi.h"
#include "pybioctl.h"
#include "simplelink.h"

#define Fd_t int

typedef void (*irq_handler_t)(void *pVal);
#define SL_P_EVENT_HANDLER irq_handler_t

int spi_Close(Fd_t Fd);
Fd_t spi_Open(char* pIfName , unsigned long flags);

// Contrary to the documentation pBuff is unsigned char type not char type
int spi_Read(Fd_t Fd , unsigned char* pBuff , int Len);
int spi_Write(Fd_t Fd , unsigned char* pBuff , int Len);
void NwpPowerOnPreamble(void);
void NwpPowerOn(void);
void NwpPowerOff(void);

int NwpRegisterInterruptHandler(SL_P_EVENT_HANDLER InterruptHdl , void* pValue);
void NwpMaskInterrupt();
void NwpUnMaskInterrupt();


#ifdef  __cplusplus
}
#endif // __cplusplus


#endif
