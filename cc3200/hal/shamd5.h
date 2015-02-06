//*****************************************************************************
//
//  shamd5.h
//
//  Defines and Macros for the SHA/MD5.
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

#ifndef __DRIVERLIB_SHAMD5_H__
#define __DRIVERLIB_SHAMD5_H__

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
// The following defines are used to specify the algorithm in use in the
// SHA/MD5 module.
//
//*****************************************************************************
#define SHAMD5_ALGO_MD5         0x00000018  // MD5
#define SHAMD5_ALGO_SHA1        0x0000001a  // SHA-1
#define SHAMD5_ALGO_SHA224      0x0000001c  // SHA-224
#define SHAMD5_ALGO_SHA256      0x0000001e  // SHA-256
#define SHAMD5_ALGO_HMAC_MD5    0x00000000  // HMAC-MD5
#define SHAMD5_ALGO_HMAC_SHA1   0x00000002  // HMAC-SHA-1
#define SHAMD5_ALGO_HMAC_SHA224 0x00000004  // HMAC-SHA-224
#define SHAMD5_ALGO_HMAC_SHA256 0x00000006  // HMAC-SHA-256

//*****************************************************************************
//
// The following defines are used to represent the different interrupt sources
// in SHAMD5IntEnable(), SHAMD5IntDisable(), SHAMD5GetIntStatus(), and
// SHAMD5BlockOnIntStatus() functions.
//
//*****************************************************************************
#define SHAMD5_INT_CONTEXT_READY   0x00000008
#define SHAMD5_INT_PARTHASH_READY  0x00000004
#define SHAMD5_INT_INPUT_READY     0x00000002
#define SHAMD5_INT_OUTPUT_READY    0x00000001
#define SHAMD5_INT_DMA_CONTEXT_IN  0x00010000
#define SHAMD5_INT_DMA_DATA_IN     0x00020000
#define SHAMD5_INT_DMA_CONTEXT_OUT 0x00040000

//*****************************************************************************
//
// Function prototypes
//
//*****************************************************************************
extern void SHAMD5ConfigSet(uint32_t ui32Base, uint32_t ui32Mode);
extern bool SHAMD5DataProcess(uint32_t ui32Base, uint8_t *pui8DataSrc,
                  uint32_t ui32DataLength, uint8_t *pui8HashResult);
extern void SHAMD5DataWrite(uint32_t ui32Base, uint8_t *pui8Src);
extern void SHAMD5DataWriteMultiple(uint8_t *pui8DataSrc, uint32_t ui32DataLength);
extern bool SHAMD5DataWriteNonBlocking(uint32_t ui32Base, uint8_t *pui8Src);
extern void SHAMD5DMADisable(uint32_t ui32Base);
extern void SHAMD5DMAEnable(uint32_t ui32Base);
extern void SHAMD5DataLengthSet(uint32_t ui32Base, uint32_t ui32Length);
extern void SHAMD5HMACKeySet(uint32_t ui32Base, uint8_t *pui8Src);
extern void SHAMD5HMACPPKeyGenerate(uint32_t ui32Base, uint8_t *pui8Key,
                        uint8_t *pui8PPKey);
extern void SHAMD5HMACPPKeySet(uint32_t ui32Base, uint8_t *pui8Src);
extern bool SHAMD5HMACProcess(uint32_t ui32Base, uint8_t *pui8DataSrc,
                  uint32_t ui32DataLength, uint8_t *pui8HashResult);
extern void SHAMD5IntClear(uint32_t ui32Base, uint32_t ui32IntFlags);
extern void SHAMD5IntDisable(uint32_t ui32Base, uint32_t ui32IntFlags);
extern void SHAMD5IntEnable(uint32_t ui32Base, uint32_t ui32IntFlags);
extern void SHAMD5IntRegister(uint32_t ui32Base, void(*pfnHandler)(void));
extern uint32_t SHAMD5IntStatus(uint32_t ui32Base, bool bMasked);
extern void SHAMD5IntUnregister(uint32_t ui32Base);
extern void SHAMD5ResultRead(uint32_t ui32Base, uint8_t *pui8Dest);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //  __DRIVERLIB_SHAMD5_H__
