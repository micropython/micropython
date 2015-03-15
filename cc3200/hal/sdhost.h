//*****************************************************************************
//
//  sdhost.h
//
//  Defines and Macros for the SDHost.
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

#ifndef __SDHOST_H__
#define __SDHOST_H__


//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
//{
#endif


//*****************************************************************************
// Values that can be passed to SDHostRespGet().
//*****************************************************************************
#define SDHOST_RESP_10 0x00000003
#define SDHOST_RESP_32 0x00000002
#define SDHOST_RESP_54 0x00000001
#define SDHOST_RESP_76 0x00000000


//*****************************************************************************
// Values that can be passed to SDHostIntEnable(), SDHostIntDisable(),
// SDHostIntClear() ,and returned from SDHostIntStatus().
//*****************************************************************************
#define SDHOST_INT_CC    0x00000001
#define SDHOST_INT_TC    0x00000002
#define SDHOST_INT_BWR   0x00000010
#define SDHOST_INT_BRR   0x00000020
#define SDHOST_INT_ERRI  0x00008000
#define SDHOST_INT_CTO   0x00010000
#define SDHOST_INT_CEB   0x00040000
#define SDHOST_INT_DTO   0x00100000
#define SDHOST_INT_DCRC  0x00200000
#define SDHOST_INT_DEB   0x00400000
#define SDHOST_INT_CERR  0x10000000
#define SDHOST_INT_BADA  0x20000000
#define SDHOST_INT_DMARD 0x40000000
#define SDHOST_INT_DMAWR 0x80000000

//*****************************************************************************
// Values that can be passed to SDHostCmdSend().
//*****************************************************************************
#define SDHOST_CMD_0     0x00000000
#define SDHOST_CMD_1     0x01000000
#define SDHOST_CMD_2     0x02000000
#define SDHOST_CMD_3     0x03000000
#define SDHOST_CMD_4     0x04000000
#define SDHOST_CMD_5     0x05000000
#define SDHOST_CMD_6     0x06000000
#define SDHOST_CMD_7     0x07000000
#define SDHOST_CMD_8     0x08000000
#define SDHOST_CMD_9     0x09000000
#define SDHOST_CMD_10    0x0A000000
#define SDHOST_CMD_11    0x0B000000
#define SDHOST_CMD_12    0x0C000000
#define SDHOST_CMD_13    0x0D000000
#define SDHOST_CMD_14    0x0E000000
#define SDHOST_CMD_15    0x0F000000
#define SDHOST_CMD_16    0x10000000
#define SDHOST_CMD_17    0x11000000
#define SDHOST_CMD_18    0x12000000
#define SDHOST_CMD_19    0x13000000
#define SDHOST_CMD_20    0x14000000
#define SDHOST_CMD_21    0x15000000
#define SDHOST_CMD_22    0x16000000
#define SDHOST_CMD_23    0x17000000
#define SDHOST_CMD_24    0x18000000
#define SDHOST_CMD_25    0x19000000
#define SDHOST_CMD_26    0x1A000000
#define SDHOST_CMD_27    0x1B000000
#define SDHOST_CMD_28    0x1C000000
#define SDHOST_CMD_29    0x1D000000
#define SDHOST_CMD_30    0x1E000000
#define SDHOST_CMD_31    0x1F000000
#define SDHOST_CMD_32    0x20000000
#define SDHOST_CMD_33    0x21000000
#define SDHOST_CMD_34    0x22000000
#define SDHOST_CMD_35    0x23000000
#define SDHOST_CMD_36    0x24000000
#define SDHOST_CMD_37    0x25000000
#define SDHOST_CMD_38    0x26000000
#define SDHOST_CMD_39    0x27000000
#define SDHOST_CMD_40    0x28000000
#define SDHOST_CMD_41    0x29000000
#define SDHOST_CMD_42    0x2A000000
#define SDHOST_CMD_43    0x2B000000
#define SDHOST_CMD_44    0x2C000000
#define SDHOST_CMD_45    0x2D000000
#define SDHOST_CMD_46    0x2E000000
#define SDHOST_CMD_47    0x2F000000
#define SDHOST_CMD_48    0x30000000
#define SDHOST_CMD_49    0x31000000
#define SDHOST_CMD_50    0x32000000
#define SDHOST_CMD_51    0x33000000
#define SDHOST_CMD_52    0x34000000
#define SDHOST_CMD_53    0x35000000
#define SDHOST_CMD_54    0x36000000
#define SDHOST_CMD_55    0x37000000
#define SDHOST_CMD_56    0x38000000
#define SDHOST_CMD_57    0x39000000
#define SDHOST_CMD_58    0x3A000000
#define SDHOST_CMD_59    0x3B000000
#define SDHOST_CMD_60    0x3C000000
#define SDHOST_CMD_61    0x3D000000
#define SDHOST_CMD_62    0x3E000000
#define SDHOST_CMD_63    0x3F000000

//*****************************************************************************
// Values that can be logically ORed with ulCmd parameter for SDHostCmdSend().
//*****************************************************************************
#define SDHOST_MULTI_BLK    0x00000022
#define SDHOST_DMA_EN       0x00000001
#define SDHOST_WR_CMD       0x00200000
#define SDHOST_RD_CMD       0x00200010
#define SDHOST_RESP_LEN_136 0x00010000
#define SDHOST_RESP_LEN_48  0x00020000
#define SDHOST_RESP_LEN_48B 0x00030000


//*****************************************************************************
//
// API Function prototypes
//
//*****************************************************************************
extern void SDHostCmdReset(unsigned long ulBase);
extern void SDHostInit(unsigned long ulBase);
extern long SDHostCmdSend(unsigned long ulBase,unsigned long ulCmd,
                       unsigned ulArg);
extern void SDHostIntRegister(unsigned long ulBase, void (*pfnHandler)(void));
extern void SDHostIntUnregister(unsigned long ulBase);
extern void SDHostIntEnable(unsigned long ulBase,unsigned long ulIntFlags);
extern void SDHostIntDisable(unsigned long ulBase,unsigned long ulIntFlags);
extern unsigned long SDHostIntStatus(unsigned long ulBase);
extern void SDHostIntClear(unsigned long ulBase,unsigned long ulIntFlags);
extern void SDHostCardErrorMaskSet(unsigned long ulBase,
                                   unsigned long ulErrMask);
extern unsigned long SDHostCardErrorMaskGet(unsigned long ulBase);
extern void SDHostSetExpClk(unsigned long ulBase, unsigned long ulSDHostClk,
              unsigned long ulCardClk);
extern void SDHostRespGet(unsigned long ulBase, unsigned long ulRespnse[4]);
extern void SDHostBlockSizeSet(unsigned long ulBase, unsigned short ulBlkSize);
extern void SDHostBlockCountSet(unsigned long ulBase,
                                unsigned short ulBlkCount);
extern tBoolean SDHostDataNonBlockingWrite(unsigned long ulBase,
                                        unsigned long ulData);
extern tBoolean SDHostDataNonBlockingRead(unsigned long ulBase,
                                   unsigned long *pulData);
extern void SDHostDataWrite(unsigned long ulBase, unsigned long ulData);
extern void SDHostDataRead(unsigned long ulBase, unsigned long *ulData);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
//}
#endif

#endif // __SDHOST_H__
