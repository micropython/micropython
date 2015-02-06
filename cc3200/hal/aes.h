//*****************************************************************************
//
//  aes.h
//
//  Defines and Macros for the AES module.
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

#ifndef __DRIVERLIB_AES_H__
#define __DRIVERLIB_AES_H__

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
// The following defines are used to specify the operation direction in the
// ui32Config argument in the AESConfig function.  Only one is permitted.
//
//*****************************************************************************
#define AES_CFG_DIR_ENCRYPT     0x00000004
#define AES_CFG_DIR_DECRYPT     0x00000000

//*****************************************************************************
//
// The following defines are used to specify the key size in the ui32Config
// argument in the AESConfig function.  Only one is permitted.
//
//*****************************************************************************
#define AES_CFG_KEY_SIZE_128BIT 0x00000008
#define AES_CFG_KEY_SIZE_192BIT 0x00000010
#define AES_CFG_KEY_SIZE_256BIT 0x00000018

//*****************************************************************************
//
// The following defines are used to specify the mode of operation in the
// ui32Config argument in the AESConfig function.  Only one is permitted.
//
//*****************************************************************************
#define AES_CFG_MODE_M          0x2007fe60
#define AES_CFG_MODE_ECB        0x00000000
#define AES_CFG_MODE_CBC        0x00000020
#define AES_CFG_MODE_CTR        0x00000040
#define AES_CFG_MODE_ICM        0x00000200
#define AES_CFG_MODE_CFB        0x00000400
#define AES_CFG_MODE_XTS_TWEAKJL \
                                0x00000800
#define AES_CFG_MODE_XTS_K2IJL \
                                0x00001000
#define AES_CFG_MODE_XTS_K2ILJ0 \
                                0x00001800
#define AES_CFG_MODE_F8         0x00002000
#define AES_CFG_MODE_F9         0x20004000
#define AES_CFG_MODE_CBCMAC     0x20008000
#define AES_CFG_MODE_GCM_HLY0ZERO \
                                0x20010040
#define AES_CFG_MODE_GCM_HLY0CALC \
                                0x20020040
#define AES_CFG_MODE_GCM_HY0CALC \
                                0x20030040
#define AES_CFG_MODE_CCM        0x20040040

//*****************************************************************************
//
// The following defines are used to specify the counter width in the
// ui32Config argument in the AESConfig function.  It is only required to
// be defined when using CTR, CCM, or GCM modes. Only one length is permitted.
//
//*****************************************************************************
#define AES_CFG_CTR_WIDTH_32    0x00000000
#define AES_CFG_CTR_WIDTH_64    0x00000080
#define AES_CFG_CTR_WIDTH_96    0x00000100
#define AES_CFG_CTR_WIDTH_128   0x00000180

//*****************************************************************************
//
// The following defines are used to define the width of the length field for
// CCM operation through the ui32Config argument in the AESConfig function.
// This value is also known as L.  Only one is permitted.
//
//*****************************************************************************
#define AES_CFG_CCM_L_2         0x00080000
#define AES_CFG_CCM_L_4         0x00180000
#define AES_CFG_CCM_L_8         0x00380000

//*****************************************************************************
//
// The following defines are used to define the length of the authentication
// field for CCM operations through the ui32Config argument in the AESConfig
// function.  This value is also known as M.  Only one is permitted.
//
//*****************************************************************************
#define AES_CFG_CCM_M_4         0x00400000
#define AES_CFG_CCM_M_6         0x00800000
#define AES_CFG_CCM_M_8         0x00c00000
#define AES_CFG_CCM_M_10        0x01000000
#define AES_CFG_CCM_M_12        0x01400000
#define AES_CFG_CCM_M_14        0x01800000
#define AES_CFG_CCM_M_16        0x01c00000

//*****************************************************************************
//
// Interrupt flags for use with the AESIntEnable, AESIntDisable, and
// AESIntStatus functions.
//
//*****************************************************************************
#define AES_INT_CONTEXT_IN      0x00000001
#define AES_INT_CONTEXT_OUT     0x00000008
#define AES_INT_DATA_IN         0x00000002
#define AES_INT_DATA_OUT        0x00000004
#define AES_INT_DMA_CONTEXT_IN  0x00010000
#define AES_INT_DMA_CONTEXT_OUT 0x00020000
#define AES_INT_DMA_DATA_IN     0x00040000
#define AES_INT_DMA_DATA_OUT    0x00080000

//*****************************************************************************
//
// Defines used when enabling and disabling DMA requests in the
// AESEnableDMA and AESDisableDMA functions.
//
//*****************************************************************************
#define AES_DMA_DATA_IN         0x00000040
#define AES_DMA_DATA_OUT        0x00000020
#define AES_DMA_CONTEXT_IN      0x00000080
#define AES_DMA_CONTEXT_OUT     0x00000100

//*****************************************************************************
//
// Function prototypes.
//
//*****************************************************************************
extern void AESConfigSet(uint32_t ui32Base, uint32_t ui32Config);
extern void AESKey1Set(uint32_t ui32Base, uint8_t *pui8Key,
                       uint32_t ui32Keysize);
extern void AESKey2Set(uint32_t ui32Base, uint8_t *pui8Key,
                       uint32_t ui32Keysize);
extern void AESKey3Set(uint32_t ui32Base, uint8_t *pui8Key);
extern void AESIVSet(uint32_t ui32Base, uint8_t *pui8IVdata);
extern void AESTagRead(uint32_t ui32Base, uint8_t *pui8TagData);
extern void AESDataLengthSet(uint32_t ui32Base, uint64_t ui64Length);
extern void AESAuthDataLengthSet(uint32_t ui32Base, uint32_t ui32Length);
extern bool AESDataReadNonBlocking(uint32_t ui32Base, uint8_t *pui8Dest,
                                   uint8_t ui8Length);
extern void AESDataRead(uint32_t ui32Base, uint8_t *pui8Dest,
                        uint8_t ui8Length);
extern bool AESDataWriteNonBlocking(uint32_t ui32Base, uint8_t *pui8Src,
                                    uint8_t ui8Length);
extern void AESDataWrite(uint32_t ui32Base, uint8_t *pui8Src,
                         uint8_t ui8Length);
extern bool AESDataProcess(uint32_t ui32Base, uint8_t *pui8Src,
                           uint8_t *pui8Dest,
               uint32_t ui32Length);
extern bool AESDataMAC(uint32_t ui32Base, uint8_t *pui8Src,
                       uint32_t ui32Length,
            uint8_t *pui8Tag);
extern bool AESDataProcessAE(uint32_t ui32Base, uint8_t *pui8Src,
                             uint8_t *pui8Dest, uint32_t ui32Length,
                             uint8_t *pui8AuthSrc, uint32_t ui32AuthLength,
                             uint8_t *pui8Tag);
extern uint32_t AESIntStatus(uint32_t ui32Base, bool bMasked);
extern void AESIntEnable(uint32_t ui32Base, uint32_t ui32IntFlags);
extern void AESIntDisable(uint32_t ui32Base, uint32_t ui32IntFlags);
extern void AESIntClear(uint32_t ui32Base, uint32_t ui32IntFlags);
extern void AESIntRegister(uint32_t ui32Base, void(*pfnHandler)(void));
extern void AESIntUnregister(uint32_t ui32Base);
extern void AESDMAEnable(uint32_t ui32Base, uint32_t ui32Flags);
extern void AESDMADisable(uint32_t ui32Base, uint32_t ui32Flags);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //  __DRIVERLIB_AES_H__
