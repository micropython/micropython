//*****************************************************************************
//
//  pin.h
//
//  Defines and Macros for the pin mux module
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

#ifndef __PIN_H__
#define __PIN_H__

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
// Macros Defining Pins
//*****************************************************************************

#define PIN_01 	 0x00000000
#define PIN_02 	 0x00000001
#define PIN_03   0x00000002
#define PIN_04 	 0x00000003
#define PIN_05   0x00000004
#define PIN_06   0x00000005
#define PIN_07   0x00000006
#define PIN_08   0x00000007
#define PIN_11   0x0000000A
#define PIN_12   0x0000000B
#define PIN_13   0x0000000C
#define PIN_14   0x0000000D
#define PIN_15   0x0000000E
#define PIN_16   0x0000000F
#define PIN_17   0x00000010
#define PIN_18   0x00000011
#define PIN_19 	 0x00000012
#define PIN_20   0x00000013
#define PIN_21   0x00000014
#define PIN_45   0x0000002C
#define PIN_46   0x0000002D
#define PIN_47   0x0000002E
#define PIN_48   0x0000002F
#define PIN_49   0x00000030
#define PIN_50   0x00000031
#define PIN_52   0x00000033
#define PIN_53   0x00000034
#define PIN_55   0x00000036
#define PIN_56   0x00000037
#define PIN_57   0x00000038
#define PIN_58   0x00000039
#define PIN_59   0x0000003A
#define PIN_60   0x0000003B
#define PIN_61	 0x0000003C
#define PIN_62	 0x0000003D
#define PIN_63	 0x0000003E
#define PIN_64	 0x0000003F



//*****************************************************************************
// Macros that can be used with PinConfigSet(), PinTypeGet(), PinStrengthGet()
//*****************************************************************************

#define PIN_MODE_0	 0x00000000
#define PIN_MODE_1	 0x00000001
#define PIN_MODE_2	 0x00000002
#define PIN_MODE_3	 0x00000003
#define PIN_MODE_4	 0x00000004
#define PIN_MODE_5	 0x00000005
#define PIN_MODE_6	 0x00000006
#define PIN_MODE_7	 0x00000007
#define PIN_MODE_8	 0x00000008
#define PIN_MODE_9	 0x00000009
#define PIN_MODE_10	 0x0000000A
#define PIN_MODE_11	 0x0000000B
#define PIN_MODE_12	 0x0000000C
#define PIN_MODE_13	 0x0000000D
#define PIN_MODE_14	 0x0000000E
#define PIN_MODE_15	 0x0000000F
// Note : PIN_MODE_255 is a dummy define for pinmux utility code generation
// PIN_MODE_255 should never be used in any user code.  
#define PIN_MODE_255 0x000000FF  

//*****************************************************************************
// Macros that can be used with PinDirModeSet() and returned from
// PinDirModeGet().
//*****************************************************************************
#define PIN_DIR_MODE_IN  0x00000C00 // Pin is input
#define PIN_DIR_MODE_OUT 0x00000800 // Pin is output
#define PIN_DIR_MODE_HW  0x00000000 // Pin is peripheral function

//*****************************************************************************
// Macros that can be used with PinConfigSet()
//*****************************************************************************
#define PIN_STRENGTH_2MA  0x00000020
#define PIN_STRENGTH_4MA  0x00000040
#define PIN_STRENGTH_6MA  0x00000060

#define PIN_TYPE_STD      0x00000000
#define PIN_TYPE_STD_PU   0x00000100
#define PIN_TYPE_STD_PD   0x00000200

#define PIN_TYPE_OD       0x00000010
#define PIN_TYPE_OD_PU    0x00000110
#define PIN_TYPE_OD_PD    0x00000210
#define PIN_TYPE_ANALOG   0x10000000

//*****************************************************************************
// Macros for mode and type
//*****************************************************************************
#define PAD_MODE_MASK       0x0000000F
#define PAD_STRENGTH_MASK   0x000000E0
#define PAD_TYPE_MASK       0x00000310
#define PAD_CONFIG_BASE     ((OCP_SHARED_BASE + OCP_SHARED_O_GPIO_PAD_CONFIG_0))

//*****************************************************************************
//
// API Function prototypes
//
//*****************************************************************************
extern void PinModeSet(unsigned long ulPin, unsigned long ulPinMode);
extern void PinDirModeSet(unsigned long ulPin, unsigned long ulPinIO);
extern unsigned long PinDirModeGet(unsigned long ulPin);
extern unsigned long PinModeGet(unsigned long ulPin);
extern void PinConfigGet(unsigned long ulPin,unsigned long  *pulPinStrength,
	       					unsigned long *pulPinType);
extern void PinConfigSet(unsigned long ulPin,unsigned long  ulPinStrength,
						unsigned long ulPinType);
extern void PinTypeUART(unsigned long ulPin,unsigned long ulPinMode);
extern void PinTypeI2C(unsigned long ulPin,unsigned long ulPinMode);
extern void PinTypeSPI(unsigned long ulPin,unsigned long ulPinMode);
extern void PinTypeI2S(unsigned long ulPin,unsigned long ulPinMode);
extern void PinTypeTimer(unsigned long ulPin,unsigned long ulPinMode);
extern void PinTypeCamera(unsigned long ulPin,unsigned long ulPinMode);
extern void PinTypeGPIO(unsigned long ulPin,unsigned long ulPinMode,
                        tBoolean bOpenDrain);
extern void PinTypeADC(unsigned long ulPin,unsigned long ulPinMode);
extern void PinTypeSDHost(unsigned long ulPin,unsigned long ulPinMode);


#ifdef __cplusplus
}
#endif

#endif //__PIN_H__
