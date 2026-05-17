//*****************************************************************************
//
//  wdt.h - Prototypes for the Watchdog Timer API
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

#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

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
// Prototypes for the APIs.
//
//*****************************************************************************
extern tBoolean WatchdogRunning(unsigned long ulBase);
extern void WatchdogEnable(unsigned long ulBase);
extern void WatchdogLock(unsigned long ulBase);
extern void WatchdogUnlock(unsigned long ulBase);
extern tBoolean WatchdogLockState(unsigned long ulBase);
extern void WatchdogReloadSet(unsigned long ulBase, unsigned long ulLoadVal);
extern unsigned long WatchdogReloadGet(unsigned long ulBase);
extern unsigned long WatchdogValueGet(unsigned long ulBase);
extern void WatchdogIntRegister(unsigned long ulBase, void(*pfnHandler)(void));
extern void WatchdogIntUnregister(unsigned long ulBase);
extern unsigned long WatchdogIntStatus(unsigned long ulBase, tBoolean bMasked);
extern void WatchdogIntClear(unsigned long ulBase);
extern void WatchdogStallEnable(unsigned long ulBase);
extern void WatchdogStallDisable(unsigned long ulBase);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __WATCHDOG_H__
