//*****************************************************************************
//
//  i2s.h
//
//  Defines and Macros for the I2S.
//
//  Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#ifndef __I2S_H__
#define __I2S_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// I2S DMA ports.
//
//*****************************************************************************
#define I2S_TX_DMA_PORT      0x4401E200
#define I2S_RX_DMA_PORT      0x4401E280

//*****************************************************************************
//
// Values that can be passed to I2SConfigSetExpClk() as the ulConfig parameter.
//
//*****************************************************************************
#define I2S_SLOT_SIZE_8       0x00300032
#define I2S_SLOT_SIZE_16      0x00700074
#define I2S_SLOT_SIZE_24      0x00B000B6


#define I2S_PORT_CPU          0x00080008
#define I2S_PORT_DMA          0x00000000

#define I2S_MODE_MASTER       0x00000000
#define I2S_MODE_SLAVE        0x00008000

//*****************************************************************************
//
// Values that can be passed as ulDataLine parameter.
//
//*****************************************************************************
#define I2S_DATA_LINE_0       0x00000001
#define I2S_DATA_LINE_1       0x00000002

//*****************************************************************************
//
// Values that can be passed to I2SSerializerConfig() as the ulSerMode
// parameter.
//
//*****************************************************************************
#define I2S_SER_MODE_TX       0x00000001
#define I2S_SER_MODE_RX       0x00000002
#define I2S_SER_MODE_DISABLE  0x00000000

//*****************************************************************************
//
// Values that can be passed to I2SSerializerConfig() as the ulInActState
// parameter.
//
//*****************************************************************************
#define I2S_INACT_TRI_STATE   0x00000000
#define I2S_INACT_LOW_LEVEL   0x00000008
#define I2S_INACT_HIGH_LEVEL  0x0000000C

//*****************************************************************************
//
// Values that can be passed to I2SIntEnable() and I2SIntDisable() as the
// ulIntFlags parameter.
//
//*****************************************************************************
#define I2S_INT_XUNDRN	      0x00000001
#define I2S_INT_XSYNCERR      0x00000002
#define I2S_INT_XLAST         0x00000010
#define I2S_INT_XDATA         0x00000020
#define I2S_INT_XSTAFRM       0x00000080
#define I2S_INT_XDMA	      0x80000000
#define I2S_INT_ROVRN         0x00010000
#define I2S_INT_RSYNCERR      0x00020000
#define I2S_INT_RLAST         0x00100000
#define I2S_INT_RDATA         0x00200000
#define I2S_INT_RSTAFRM       0x00800000
#define I2S_INT_RDMA          0x40000000


//*****************************************************************************
//
// Values that can be passed to I2SRxActiveSlotSet() and I2STxActiveSlotSet
//
//*****************************************************************************
#define I2S_ACT_SLOT_EVEN     0x00000001
#define I2S_ACT_SLOT_ODD      0x00000002

//*****************************************************************************
//
// Values that can be passed to I2SIntClear() as the
// ulIntFlags parameter and returned from I2SIntStatus().
//
//*****************************************************************************
#define I2S_STS_XERR          0x00000100
#define I2S_STS_XDMAERR	      0x00000080
#define I2S_STS_XSTAFRM	      0x00000040
#define I2S_STS_XDATA	      0x00000020
#define I2S_STS_XLAST	      0x00000010
#define I2S_STS_XSYNCERR      0x00000002
#define I2S_STS_XUNDRN	      0x00000001
#define I2S_STS_XDMA	      0x80000000
#define I2S_STS_RERR          0x01000000
#define I2S_STS_RDMAERR       0x00800000
#define I2S_STS_RSTAFRM       0x00400000
#define I2S_STS_RDATA         0x00200000
#define I2S_STS_RLAST         0x00100000
#define I2S_STS_RSYNCERR      0x00020000
#define I2S_STS_ROVERN	      0x00010000
#define I2S_STS_RDMA          0x40000000

//*****************************************************************************
//
// Values that can be passed to I2SEnable() as the ulMode parameter.
//
//*****************************************************************************
#define I2S_MODE_TX_ONLY      0x00000001
#define I2S_MODE_TX_RX_SYNC   0x00000003


//*****************************************************************************
//
// API Function prototypes
//
//*****************************************************************************
extern void I2SEnable(unsigned long ulBase, unsigned long ulMode);
extern void I2SDisable(unsigned long ulBase);

extern void I2SDataPut(unsigned long ulBase, unsigned long ulDataLine,
                unsigned long ulData);
extern long I2SDataPutNonBlocking(unsigned long ulBase,
                               unsigned long ulDataLine, unsigned long ulData);

extern void I2SDataGet(unsigned long ulBase, unsigned long ulDataLine,
                unsigned long *pulData);
extern long I2SDataGetNonBlocking(unsigned long ulBase,
                unsigned long ulDataLine, unsigned long *pulData);

extern void I2SConfigSetExpClk(unsigned long ulBase, unsigned long ulI2SClk,
                               unsigned long ulBitClk, unsigned long ulConfig);

extern void I2STxFIFOEnable(unsigned long ulBase, unsigned long ulTxLevel,
                     unsigned long ulWordsPerTransfer);
extern void I2STxFIFODisable(unsigned long ulBase);
extern void I2SRxFIFOEnable(unsigned long ulBase, unsigned long ulRxLevel,
                     unsigned long ulWordsPerTransfer);
extern void I2SRxFIFODisable(unsigned long ulBase);
extern unsigned long I2STxFIFOStatusGet(unsigned long ulBase);
extern unsigned long I2SRxFIFOStatusGet(unsigned long ulBase);

extern void I2SSerializerConfig(unsigned long ulBase, unsigned long ulDataLine,
                         unsigned long ulSerMode, unsigned long ulInActState);

extern void I2SIntEnable(unsigned long ulBase, unsigned long ulIntFlags);
extern void I2SIntDisable(unsigned long ulBase, unsigned long ulIntFlags);
extern unsigned long I2SIntStatus(unsigned long ulBase);
extern void I2SIntClear(unsigned long ulBase, unsigned long ulIntFlags);
extern void I2SIntRegister(unsigned long ulBase, void (*pfnHandler)(void));
extern void I2SIntUnregister(unsigned long ulBase);
extern void I2STxActiveSlotSet(unsigned long ulBase, unsigned long ulActSlot);
extern void I2SRxActiveSlotSet(unsigned long ulBase, unsigned long ulActSlot);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //__I2S_H__

