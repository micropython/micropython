//*****************************************************************************
//
//  adc.h
//
//  Defines and Macros for the ADC.
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

#ifndef __ADC_H__
#define __ADC_H__

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
// Values that can be passed to APIs as ulChannel parameter
//*****************************************************************************
#define ADC_CH_0   0x00000000
#define ADC_CH_1   0x00000008
#define ADC_CH_2   0x00000010
#define ADC_CH_3   0x00000018


//*****************************************************************************
//
// Values that can be passed to ADCIntEnable(), ADCIntDisable()
// and ADCIntClear() as ulIntFlags, and returned from ADCIntStatus()
//
//*****************************************************************************
#define ADC_DMA_DONE        0x00000010
#define ADC_FIFO_OVERFLOW   0x00000008
#define ADC_FIFO_UNDERFLOW  0x00000004
#define ADC_FIFO_EMPTY      0x00000002
#define ADC_FIFO_FULL       0x00000001


//*****************************************************************************
//
// API Function prototypes
//
//*****************************************************************************
extern void ADCEnable(unsigned long ulBase);
extern void ADCDisable(unsigned long ulBase);
extern void ADCChannelEnable(unsigned long ulBase,unsigned long ulChannel);
extern void ADCChannelDisable(unsigned long ulBase,unsigned long ulChannel);
extern void ADCIntRegister(unsigned long ulBase, unsigned long ulChannel,
                    void (*pfnHandler)(void));
extern void ADCIntUnregister(unsigned long ulBase, unsigned long ulChannel);
extern void ADCIntEnable(unsigned long ulBase, unsigned long ulChannel,
                  unsigned long ulIntFlags);
extern void ADCIntDisable(unsigned long ulBase, unsigned long ulChannel,
                  unsigned long ulIntFlags);
extern unsigned long ADCIntStatus(unsigned long ulBase,unsigned long ulChannel);
extern void ADCIntClear(unsigned long ulBase, unsigned long ulChannel,
                  unsigned long ulIntFlags);
extern void ADCDMAEnable(unsigned long ulBase, unsigned long ulChannel);
extern void ADCDMADisable(unsigned long ulBase, unsigned long ulChannel);
extern void ADCTimerConfig(unsigned long ulBase, unsigned long ulValue);
extern void ADCTimerEnable(unsigned long ulBase);
extern void ADCTimerDisable(unsigned long ulBase);
extern void ADCTimerReset(unsigned long ulBase);
extern unsigned long ADCTimerValueGet(unsigned long ulBase);
extern unsigned char ADCFIFOLvlGet(unsigned long ulBase,
                                   unsigned long ulChannel);
extern unsigned long ADCFIFORead(unsigned long ulBase,
                                   unsigned long ulChannel);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __ADC_H__

