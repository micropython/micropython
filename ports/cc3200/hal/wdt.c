//*****************************************************************************
//
//  wdt.c
//
//  Driver for the Watchdog Timer Module.
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
//! \addtogroup WDT_Watchdog_Timer_api
//! @{
//
//*****************************************************************************

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_wdt.h"
#include "debug.h"
#include "interrupt.h"
#include "wdt.h"

//*****************************************************************************
//
//! Determines if the watchdog timer is enabled.
//!
//! \param ulBase is the base address of the watchdog timer module.
//!
//! This will check to see if the watchdog timer is enabled.
//!
//! \return Returns \b true if the watchdog timer is enabled, and \b false
//! if it is not.
//
//*****************************************************************************
tBoolean
WatchdogRunning(unsigned long ulBase)
{
    //
    // Check the arguments.
    //
    ASSERT((ulBase == WDT_BASE));

    //
    // See if the watchdog timer module is enabled, and return.
    //
    return(HWREG(ulBase + WDT_O_CTL) & WDT_CTL_INTEN);
}

//*****************************************************************************
//
//! Enables the watchdog timer.
//!
//! \param ulBase is the base address of the watchdog timer module.
//!
//! This will enable the watchdog timer counter and interrupt.
//!
//! \note This function will have no effect if the watchdog timer has
//! been locked.
//!
//! \sa WatchdogLock(), WatchdogUnlock()
//!
//! \return None.
//
//*****************************************************************************
void
WatchdogEnable(unsigned long ulBase)
{
    //
    // Check the arguments.
    //
    ASSERT((ulBase == WDT_BASE));

    //
    // Enable the watchdog timer module.
    //
    HWREG(ulBase + WDT_O_CTL) |= WDT_CTL_INTEN;
}

//*****************************************************************************
//
//! Enables the watchdog timer lock mechanism.
//!
//! \param ulBase is the base address of the watchdog timer module.
//!
//! Locks out write access to the watchdog timer configuration registers.
//!
//! \return None.
//
//*****************************************************************************
void
WatchdogLock(unsigned long ulBase)
{
    //
    // Check the arguments.
    //
    ASSERT((ulBase == WDT_BASE));

    //
    // Lock out watchdog register writes.  Writing anything to the WDT_O_LOCK
    // register causes the lock to go into effect.
    //
    HWREG(ulBase + WDT_O_LOCK) = WDT_LOCK_LOCKED;
}

//*****************************************************************************
//
//! Disables the watchdog timer lock mechanism.
//!
//! \param ulBase is the base address of the watchdog timer module.
//!
//! Enables write access to the watchdog timer configuration registers.
//!
//! \return None.
//
//*****************************************************************************
void
WatchdogUnlock(unsigned long ulBase)
{
    //
    // Check the arguments.
    //
    ASSERT((ulBase == WDT_BASE));

    //
    // Unlock watchdog register writes.
    //
    HWREG(ulBase + WDT_O_LOCK) = WDT_LOCK_UNLOCK;
}

//*****************************************************************************
//
//! Gets the state of the watchdog timer lock mechanism.
//!
//! \param ulBase is the base address of the watchdog timer module.
//!
//! Returns the lock state of the watchdog timer registers.
//!
//! \return Returns \b true if the watchdog timer registers are locked, and
//! \b false if they are not locked.
//
//*****************************************************************************
tBoolean
WatchdogLockState(unsigned long ulBase)
{
    //
    // Check the arguments.
    //
    ASSERT((ulBase == WDT_BASE));

    //
    // Get the lock state.
    //
    return((HWREG(ulBase + WDT_O_LOCK) == WDT_LOCK_LOCKED) ? true : false);
}

//*****************************************************************************
//
//! Sets the watchdog timer reload value.
//!
//! \param ulBase is the base address of the watchdog timer module.
//! \param ulLoadVal is the load value for the watchdog timer.
//!
//! This function sets the value to load into the watchdog timer when the count
//! reaches zero for the first time; if the watchdog timer is running when this
//! function is called, then the value will be immediately loaded into the
//! watchdog timer counter.  If the \e ulLoadVal parameter is 0, then an
//! interrupt is immediately generated.
//!
//! \note This function will have no effect if the watchdog timer has
//! been locked.
//!
//! \sa WatchdogLock(), WatchdogUnlock(), WatchdogReloadGet()
//!
//! \return None.
//
//*****************************************************************************
void
WatchdogReloadSet(unsigned long ulBase, unsigned long ulLoadVal)
{
    //
    // Check the arguments.
    //
    ASSERT((ulBase == WDT_BASE));

    //
    // Set the load register.
    //
    HWREG(ulBase + WDT_O_LOAD) = ulLoadVal;
}

//*****************************************************************************
//
//! Gets the watchdog timer reload value.
//!
//! \param ulBase is the base address of the watchdog timer module.
//!
//! This function gets the value that is loaded into the watchdog timer when
//! the count reaches zero for the first time.
//!
//! \sa WatchdogReloadSet()
//!
//! \return None.
//
//*****************************************************************************
unsigned long
WatchdogReloadGet(unsigned long ulBase)
{
    //
    // Check the arguments.
    //
    ASSERT((ulBase == WDT_BASE));

    //
    // Get the load register.
    //
    return(HWREG(ulBase + WDT_O_LOAD));
}

//*****************************************************************************
//
//! Gets the current watchdog timer value.
//!
//! \param ulBase is the base address of the watchdog timer module.
//!
//! This function reads the current value of the watchdog timer.
//!
//! \return Returns the current value of the watchdog timer.
//
//*****************************************************************************
unsigned long
WatchdogValueGet(unsigned long ulBase)
{
    //
    // Check the arguments.
    //
    ASSERT((ulBase == WDT_BASE));

    //
    // Get the current watchdog timer register value.
    //
    return(HWREG(ulBase + WDT_O_VALUE));
}

//*****************************************************************************
//
//! Registers an interrupt handler for watchdog timer interrupt.
//!
//! \param ulBase is the base address of the watchdog timer module.
//! \param pfnHandler is a pointer to the function to be called when the
//! watchdog timer interrupt occurs.
//!
//! This function does the actual registering of the interrupt handler.  This
//! will enable the global interrupt in the interrupt controller; the watchdog
//! timer interrupt must be enabled via WatchdogEnable().  It is the interrupt
//! handler's responsibility to clear the interrupt source via
//! WatchdogIntClear().
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \note This function will only register the standard watchdog interrupt
//! handler.  To register the NMI watchdog handler, use IntRegister()
//! to register the handler for the \b FAULT_NMI interrupt.
//!
//! \return None.
//
//*****************************************************************************
void
WatchdogIntRegister(unsigned long ulBase, void (*pfnHandler)(void))
{
    //
    // Check the arguments.
    //
    ASSERT((ulBase == WDT_BASE));

    //
    // Register the interrupt handler and
    // Enable the watchdog timer interrupt.
    //
    IntRegister(INT_WDT, pfnHandler);
    IntEnable(INT_WDT);
}

//*****************************************************************************
//
//! Unregisters an interrupt handler for the watchdog timer interrupt.
//!
//! \param ulBase is the base address of the watchdog timer module.
//!
//! This function does the actual unregistering of the interrupt handler.  This
//! function will clear the handler to be called when a watchdog timer
//! interrupt occurs.  This will also mask off the interrupt in the interrupt
//! controller so that the interrupt handler no longer is called.
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \note This function will only unregister the standard watchdog interrupt
//! handler.  To unregister the NMI watchdog handler, use IntUnregister()
//! to unregister the handler for the \b FAULT_NMI interrupt.
//!
//! \return None.
//
//*****************************************************************************
void
WatchdogIntUnregister(unsigned long ulBase)
{
    //
    // Check the arguments.
    //
    ASSERT((ulBase == WDT_BASE));

    //
    // Disable the interrupt
    IntDisable(INT_WDT);

    //
    // Unregister the interrupt handler.
    //
    IntUnregister(INT_WDT);
}

//*****************************************************************************
//
//! Gets the current watchdog timer interrupt status.
//!
//! \param ulBase is the base address of the watchdog timer module.
//! \param bMasked is \b false if the raw interrupt status is required and
//! \b true if the masked interrupt status is required.
//!
//! This returns the interrupt status for the watchdog timer module.  Either
//! the raw interrupt status or the status of interrupt that is allowed to
//! reflect to the processor can be returned.
//!
//! \return Returns the current interrupt status, where a 1 indicates that the
//! watchdog interrupt is active, and a 0 indicates that it is not active.
//
//*****************************************************************************
unsigned long
WatchdogIntStatus(unsigned long ulBase, tBoolean bMasked)
{
    //
    // Check the arguments.
    //
    ASSERT((ulBase == WDT_BASE));

    //
    // Return either the interrupt status or the raw interrupt status as
    // requested.
    //
    if(bMasked)
    {
        return(HWREG(ulBase + WDT_O_MIS));
    }
    else
    {
        return(HWREG(ulBase + WDT_O_RIS));
    }
}

//*****************************************************************************
//
//! Clears the watchdog timer interrupt.
//!
//! \param ulBase is the base address of the watchdog timer module.
//!
//! The watchdog timer interrupt source is cleared, so that it no longer
//! asserts.
//!
//! \note Because there is a write buffer in the Cortex-M3 processor, it may
//! take several clock cycles before the interrupt source is actually cleared.
//! Therefore, it is recommended that the interrupt source be cleared early in
//! the interrupt handler (as opposed to the very last action) to avoid
//! returning from the interrupt handler before the interrupt source is
//! actually cleared.  Failure to do so may result in the interrupt handler
//! being immediately reentered (because the interrupt controller still sees
//! the interrupt source asserted).
//!
//! \return None.
//
//*****************************************************************************
void
WatchdogIntClear(unsigned long ulBase)
{
    //
    // Check the arguments.
    //
    ASSERT((ulBase == WDT_BASE));

    //
    // Clear the interrupt source.
    //
    HWREG(ulBase + WDT_O_ICR) = WDT_INT_TIMEOUT;
}

//*****************************************************************************
//
//! Enables stalling of the watchdog timer during debug events.
//!
//! \param ulBase is the base address of the watchdog timer module.
//!
//! This function allows the watchdog timer to stop counting when the processor
//! is stopped by the debugger.  By doing so, the watchdog is prevented from
//! expiring (typically almost immediately from a human time perspective) and
//! resetting the system (if reset is enabled).  The watchdog will instead
//! expired after the appropriate number of processor cycles have been executed
//! while debugging (or at the appropriate time after the processor has been
//! restarted).
//!
//! \return None.
//
//*****************************************************************************
void
WatchdogStallEnable(unsigned long ulBase)
{
    //
    // Check the arguments.
    //
    ASSERT((ulBase == WDT_BASE));

    //
    // Enable timer stalling.
    //
    HWREG(ulBase + WDT_O_TEST) |= WDT_TEST_STALL;
}

//*****************************************************************************
//
//! Disables stalling of the watchdog timer during debug events.
//!
//! \param ulBase is the base address of the watchdog timer module.
//!
//! This function disables the debug mode stall of the watchdog timer.  By
//! doing so, the watchdog timer continues to count regardless of the processor
//! debug state.
//!
//! \return None.
//
//*****************************************************************************
void
WatchdogStallDisable(unsigned long ulBase)
{
    //
    // Check the arguments.
    //
    ASSERT((ulBase == WDT_BASE));

    //
    // Disable timer stalling.
    //
    HWREG(ulBase + WDT_O_TEST) &= ~(WDT_TEST_STALL);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
