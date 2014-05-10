/*****************************************************************************
*
*  spi.h  - CC3000 Host Driver Implementation.
*  Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
*
* Adapted for use with the Arduino/AVR by KTOWN (Kevin Townsend)
* & Limor Fried for Adafruit Industries
* This library works with the Adafruit CC3000 breakout
*	----> https://www.adafruit.com/products/1469
* Adafruit invests time and resources providing this open source code,
* please support Adafruit and open-source hardware by purchasing
* products from Adafruit!
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/


#ifndef __SPI_H__
#define __SPI_H__

//#include <string.h>
//#include <stdlib.h>
//#include <stdio.h>

//#include "wlan.h"

typedef void (*gcSpiHandleRx)(void *p);
typedef void (*gcSpiHandleTx)(void);

extern unsigned char wlan_tx_buffer[];

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
extern void SpiInit(void);
extern void SpiOpen(gcSpiHandleRx pfRxHandler);
extern void SpiClose(void);
extern long SpiWrite(unsigned char *pUserBuffer, unsigned short usLength);
extern void SpiResumeSpi(void);
extern void SpiCleanGPIOISR(void);
extern long TXBufferIsEmpty(void);
extern long RXBufferIsEmpty(void);
extern void CC3000_UsynchCallback(long lEventType, char * data, unsigned char length);
extern void WriteWlanPin( unsigned char val );
extern long ReadWlanInterruptPin(void);
extern void WlanInterruptEnable();
extern void WlanInterruptDisable();
extern char *sendDriverPatch(unsigned long *Length);
extern char *sendBootLoaderPatch(unsigned long *Length);
extern char *sendWLFWPatch(unsigned long *Length);
extern void SpiIntGPIOHandler(void);

#endif

