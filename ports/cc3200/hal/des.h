//*****************************************************************************
//
//  des.h
//
//  Defines and Macros for the DES module.
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

#ifndef __DRIVERLIB_DES_H__
#define __DRIVERLIB_DES_H__

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
// The following defines are used to specify the direction with the
// ui32Config argument in the DESConfig() function.  Only one is permitted.
//
//*****************************************************************************
#define DES_CFG_DIR_DECRYPT     0x00000000
#define DES_CFG_DIR_ENCRYPT     0x00000004

//*****************************************************************************
//
// The following defines are used to specify the operational with the
// ui32Config argument in the DESConfig() function.  Only one is permitted.
//
//*****************************************************************************
#define DES_CFG_MODE_ECB        0x00000000
#define DES_CFG_MODE_CBC        0x00000010
#define DES_CFG_MODE_CFB        0x00000020

//*****************************************************************************
//
// The following defines are used to select between single DES and triple DES
// with the ui32Config argument in the DESConfig() function.  Only one is
// permitted.
//
//*****************************************************************************
#define DES_CFG_SINGLE          0x00000000
#define DES_CFG_TRIPLE          0x00000008

//*****************************************************************************
//
// The following defines are used with the DESIntEnable(), DESIntDisable() and
// DESIntStatus() functions.
//
//*****************************************************************************
#define DES_INT_CONTEXT_IN      0x00000001
#define DES_INT_DATA_IN         0x00000002
#define DES_INT_DATA_OUT        0x00000004
#define DES_INT_DMA_CONTEXT_IN  0x00010000
#define DES_INT_DMA_DATA_IN     0x00020000
#define DES_INT_DMA_DATA_OUT    0x00040000

//*****************************************************************************
//
// The following defines are used with the DESEnableDMA() and DESDisableDMA()
// functions.
//
//*****************************************************************************
#define DES_DMA_CONTEXT_IN      0x00000080
#define DES_DMA_DATA_OUT        0x00000040
#define DES_DMA_DATA_IN         0x00000020

//*****************************************************************************
//
// API Function prototypes
//
//*****************************************************************************
extern void DESConfigSet(uint32_t ui32Base, uint32_t ui32Config);
extern void DESDataRead(uint32_t ui32Base, uint8_t *pui8Dest,
                        uint8_t ui8Length);
extern bool DESDataReadNonBlocking(uint32_t ui32Base, uint8_t *pui8Dest,
                                   uint8_t ui8Length);
extern bool DESDataProcess(uint32_t ui32Base, uint8_t *pui8Src,
                           uint8_t *pui8Dest, uint32_t ui32Length);
extern void DESDataWrite(uint32_t ui32Base, uint8_t *pui8Src,
                         uint8_t ui8Length);
extern bool DESDataWriteNonBlocking(uint32_t ui32Base, uint8_t *pui8Src,
                                    uint8_t ui8Length);
extern void DESDMADisable(uint32_t ui32Base, uint32_t ui32Flags);
extern void DESDMAEnable(uint32_t ui32Base, uint32_t ui32Flags);
extern void DESIntClear(uint32_t ui32Base, uint32_t ui32IntFlags);
extern void DESIntDisable(uint32_t ui32Base, uint32_t ui32IntFlags);
extern void DESIntEnable(uint32_t ui32Base, uint32_t ui32IntFlags);
extern void DESIntRegister(uint32_t ui32Base, void(*pfnHandler)(void));
extern uint32_t DESIntStatus(uint32_t ui32Base, bool bMasked);
extern void DESIntUnregister(uint32_t ui32Base);
extern bool DESIVSet(uint32_t ui32Base, uint8_t *pui8IVdata);
extern void DESKeySet(uint32_t ui32Base, uint8_t *pui8Key);
extern void DESDataLengthSet(uint32_t ui32Base, uint32_t ui32Length);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //  __DRIVERLIB_DES_H__
