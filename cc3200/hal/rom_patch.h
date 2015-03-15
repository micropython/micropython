//*****************************************************************************
//
//  rom_patch.h
//
//  Macros to facilitate patching driverlib API's in the ROM.
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

//*****************************************************************************
//
// List of API's in the ROM that need to be patched.
// For e.g. to patch ROM_UARTCharPut add the line #undef ROM_UARTCharPut
//*****************************************************************************
#undef ROM_ADCIntClear
#undef ROM_IntEnable
#undef ROM_IntDisable
#undef ROM_IntPendSet
#undef ROM_SDHostCardErrorMaskSet
#undef ROM_SDHostCardErrorMaskGet
#undef ROM_TimerConfigure
#undef ROM_TimerDMAEventSet
#undef ROM_TimerDMAEventGet
#undef ROM_SDHostDataNonBlockingWrite
#undef ROM_SDHostDataWrite
#undef ROM_SDHostDataRead
#undef ROM_SDHostDataNonBlockingRead
#undef ROM_PRCMSysResetCauseGet
#undef ROM_PRCMPeripheralClkEnable
#undef ROM_PRCMLPDSWakeUpGPIOSelect
#undef ROM_PRCMHibernateWakeupSourceEnable
#undef ROM_PRCMHibernateWakeupSourceDisable
#undef ROM_PRCMHibernateWakeupCauseGet
#undef ROM_PRCMHibernateIntervalSet
#undef ROM_PRCMHibernateWakeUpGPIOSelect
#undef ROM_PRCMHibernateEnter
#undef ROM_PRCMSlowClkCtrGet
#undef ROM_PRCMSlowClkCtrMatchSet
#undef ROM_PRCMSlowClkCtrMatchGet
#undef ROM_PRCMOCRRegisterWrite
#undef ROM_PRCMOCRRegisterRead
#undef ROM_PRCMIntEnable
#undef ROM_PRCMIntDisable
#undef ROM_PRCMRTCInUseSet
#undef ROM_PRCMRTCInUseGet
#undef ROM_PRCMRTCSet
#undef ROM_PRCMRTCGet
#undef ROM_PRCMRTCMatchSet
#undef ROM_PRCMRTCMatchGet
#undef ROM_PRCMPeripheralClkDisable
#undef ROM_PRCMPeripheralReset
#undef ROM_PRCMPeripheralStatusGet
#undef ROM_SPIConfigSetExpClk
#undef ROM_GPIODirModeGet
#undef ROM_GPIOIntTypeGet
#undef ROM_I2CMasterInitExpClk
#undef ROM_AESDataProcess
#undef ROM_DESDataProcess
#undef ROM_I2SEnable
#undef ROM_I2SConfigSetExpClk
#undef ROM_PinConfigSet
#undef ROM_PRCMLPDSEnter
#undef ROM_PRCMCC3200MCUInit
#undef ROM_SDHostIntStatus
#undef ROM_SDHostBlockCountSet
#undef ROM_UARTModemControlSet
#undef ROM_UARTModemControlClear

