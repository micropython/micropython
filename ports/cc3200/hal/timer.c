//*****************************************************************************
//
//  timer.c
//
//  Driver for the timer module.
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
//! \addtogroup GPT_General_Purpose_Timer_api
//! @{
//
//*****************************************************************************

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "inc/hw_types.h"
#include "debug.h"
#include "interrupt.h"
#include "timer.h"


//*****************************************************************************
//
//! \internal
//! Checks a timer base address.
//!
//! \param ulBase is the base address of the timer module.
//!
//! This function determines if a timer module base address is valid.
//!
//! \return Returns \b true if the base address is valid and \b false
//! otherwise.
//
//*****************************************************************************
#ifdef DEBUG
static tBoolean
TimerBaseValid(unsigned long ulBase)
{
    return((ulBase == TIMERA0_BASE) || (ulBase == TIMERA1_BASE) ||
           (ulBase == TIMERA2_BASE) || (ulBase == TIMERA3_BASE));
}
#else
#define TimerBaseValid(ulBase)      (ulBase)
#endif

//*****************************************************************************
//
//! Enables the timer(s).
//!
//! \param ulBase is the base address of the timer module.
//! \param ulTimer specifies the timer(s) to enable; must be one of \b TIMER_A,
//! \b TIMER_B, or \b TIMER_BOTH.
//!
//! This function enables operation of the timer module.  The timer must be
//! configured before it is enabled.
//!
//! \return None.
//
//*****************************************************************************
void
TimerEnable(unsigned long ulBase, unsigned long ulTimer)
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));
    ASSERT((ulTimer == TIMER_A) || (ulTimer == TIMER_B) ||
           (ulTimer == TIMER_BOTH));

    //
    // Enable the timer(s) module.
    //
    HWREG(ulBase + TIMER_O_CTL) |= ulTimer & (TIMER_CTL_TAEN | TIMER_CTL_TBEN);
}

//*****************************************************************************
//
//! Disables the timer(s).
//!
//! \param ulBase is the base address of the timer module.
//! \param ulTimer specifies the timer(s) to disable; must be one of
//! \b TIMER_A, \b TIMER_B, or \b TIMER_BOTH.
//!
//! This function disables operation of the timer module.
//!
//! \return None.
//
//*****************************************************************************
void
TimerDisable(unsigned long ulBase, unsigned long ulTimer)
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));
    ASSERT((ulTimer == TIMER_A) || (ulTimer == TIMER_B) ||
           (ulTimer == TIMER_BOTH));

    //
    // Disable the timer module.
    //
    HWREG(ulBase + TIMER_O_CTL) &= ~(ulTimer &
                                     (TIMER_CTL_TAEN | TIMER_CTL_TBEN));
}

//*****************************************************************************
//
//! Configures the timer(s).
//!
//! \param ulBase is the base address of the timer module.
//! \param ulConfig is the configuration for the timer.
//!
//! This function configures the operating mode of the timer(s).  The timer
//! module is disabled before being configured, and is left in the disabled
//! state. The 16/32-bit timer is comprised of two 16-bit timers that can
//! operate independently or be concatenated to form a 32-bit timer.
//!
//! The configuration is specified in \e ulConfig as one of the following
//! values:
//!
//! - \b TIMER_CFG_ONE_SHOT - Full-width one-shot timer
//! - \b TIMER_CFG_ONE_SHOT_UP - Full-width one-shot timer that counts up
//!   instead of down (not available on all parts)
//! - \b TIMER_CFG_PERIODIC - Full-width periodic timer
//! - \b TIMER_CFG_PERIODIC_UP - Full-width periodic timer that counts up
//!   instead of down (not available on all parts)
//! - \b TIMER_CFG_SPLIT_PAIR - Two half-width timers
//!
//! When configured for a pair of half-width timers, each timer is separately
//! configured.  The first timer is configured by setting \e ulConfig to
//! the result of a logical OR operation between one of the following values
//! and \e ulConfig:
//!
//! - \b TIMER_CFG_A_ONE_SHOT - Half-width one-shot timer
//! - \b TIMER_CFG_A_ONE_SHOT_UP - Half-width one-shot timer that counts up
//!   instead of down (not available on all parts)
//! - \b TIMER_CFG_A_PERIODIC - Half-width periodic timer
//! - \b TIMER_CFG_A_PERIODIC_UP - Half-width periodic timer that counts up
//!   instead of down (not available on all parts)
//! - \b TIMER_CFG_A_CAP_COUNT - Half-width edge count capture
//! - \b TIMER_CFG_A_CAP_TIME - Half-width edge time capture
//! - \b TIMER_CFG_A_PWM - Half-width PWM output
//!
//! Similarly, the second timer is configured by setting \e ulConfig to
//! the result of a logical OR operation between one of the corresponding
//! \b TIMER_CFG_B_* values and \e ulConfig.
//!
//! \return None.
//
//*****************************************************************************
void
TimerConfigure(unsigned long ulBase, unsigned long ulConfig)
{

    ASSERT((ulConfig == TIMER_CFG_ONE_SHOT) ||
           (ulConfig == TIMER_CFG_ONE_SHOT_UP) ||
           (ulConfig == TIMER_CFG_PERIODIC) ||
           (ulConfig == TIMER_CFG_PERIODIC_UP) ||
           ((ulConfig & 0xff000000) == TIMER_CFG_SPLIT_PAIR));
    ASSERT(((ulConfig & 0xff000000) != TIMER_CFG_SPLIT_PAIR) ||
           ((((ulConfig & 0x000000ff) == TIMER_CFG_A_ONE_SHOT) ||
             ((ulConfig & 0x000000ff) == TIMER_CFG_A_ONE_SHOT_UP) ||
             ((ulConfig & 0x000000ff) == TIMER_CFG_A_PERIODIC) ||
             ((ulConfig & 0x000000ff) == TIMER_CFG_A_PERIODIC_UP) ||
             ((ulConfig & 0x000000ff) == TIMER_CFG_A_CAP_COUNT) ||
             ((ulConfig & 0x000000ff) == TIMER_CFG_A_CAP_TIME) ||
             ((ulConfig & 0x000000ff) == TIMER_CFG_A_PWM)) &&
            (((ulConfig & 0x0000ff00) == TIMER_CFG_B_ONE_SHOT) ||
             ((ulConfig & 0x0000ff00) == TIMER_CFG_B_ONE_SHOT_UP) ||
             ((ulConfig & 0x0000ff00) == TIMER_CFG_B_PERIODIC) ||
             ((ulConfig & 0x0000ff00) == TIMER_CFG_B_PERIODIC_UP) ||
             ((ulConfig & 0x0000ff00) == TIMER_CFG_B_CAP_COUNT) ||
             ((ulConfig & 0x0000ff00) == TIMER_CFG_B_CAP_TIME) ||
             ((ulConfig & 0x0000ff00) == TIMER_CFG_B_PWM))));

    //
    // Enable CCP to IO path
    //
    HWREG(0x440260B0) = 0xFF;

    //
    // Disable the timers.
    //
    HWREG(ulBase + TIMER_O_CTL) &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);

    //
    // Set the global timer configuration.
    //
    HWREG(ulBase + TIMER_O_CFG) = ulConfig >> 24;

    //
    // Set the configuration of the A and B timers.  Note that the B timer
    // configuration is ignored by the hardware in 32-bit modes.
    //
    HWREG(ulBase + TIMER_O_TAMR) = ulConfig & 255;
    HWREG(ulBase + TIMER_O_TBMR) = (ulConfig >> 8) & 255;
}

//*****************************************************************************
//
//! Controls the output level.
//!
//! \param ulBase is the base address of the timer module.
//! \param ulTimer specifies the timer(s) to adjust; must be one of \b TIMER_A,
//! \b TIMER_B, or \b TIMER_BOTH.
//! \param bInvert specifies the output level.
//!
//! This function sets the PWM output level for the specified timer.  If the
//! \e bInvert parameter is \b true, then the timer's output is made active
//! low; otherwise, it is made active high.
//!
//! \return None.
//
//*****************************************************************************
void
TimerControlLevel(unsigned long ulBase, unsigned long ulTimer,
                  tBoolean bInvert)
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));
    ASSERT((ulTimer == TIMER_A) || (ulTimer == TIMER_B) ||
           (ulTimer == TIMER_BOTH));

    //
    // Set the output levels as requested.
    //
    ulTimer &= TIMER_CTL_TAPWML | TIMER_CTL_TBPWML;
    HWREG(ulBase + TIMER_O_CTL) = (bInvert ?
                                   (HWREG(ulBase + TIMER_O_CTL) | ulTimer) :
                                   (HWREG(ulBase + TIMER_O_CTL) & ~(ulTimer)));
}

//*****************************************************************************
//
//! Controls the event type.
//!
//! \param ulBase is the base address of the timer module.
//! \param ulTimer specifies the timer(s) to be adjusted; must be one of
//! \b TIMER_A, \b TIMER_B, or \b TIMER_BOTH.
//! \param ulEvent specifies the type of event; must be one of
//! \b TIMER_EVENT_POS_EDGE, \b TIMER_EVENT_NEG_EDGE, or
//! \b TIMER_EVENT_BOTH_EDGES.
//!
//! This function sets the signal edge(s) that triggers the timer when in
//! capture mode.
//!
//! \return None.
//
//*****************************************************************************
void
TimerControlEvent(unsigned long ulBase, unsigned long ulTimer,
                  unsigned long ulEvent)
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));
    ASSERT((ulTimer == TIMER_A) || (ulTimer == TIMER_B) ||
           (ulTimer == TIMER_BOTH));

    //
    // Set the event type.
    //
    ulEvent &= ulTimer & (TIMER_CTL_TAEVENT_M | TIMER_CTL_TBEVENT_M);
    HWREG(ulBase + TIMER_O_CTL) = ((HWREG(ulBase + TIMER_O_CTL) &
                                    ~(TIMER_CTL_TAEVENT_M |
                                      TIMER_CTL_TBEVENT_M)) | ulEvent);
}

//*****************************************************************************
//
//! Controls the stall handling.
//!
//! \param ulBase is the base address of the timer module.
//! \param ulTimer specifies the timer(s) to be adjusted; must be one of
//! \b TIMER_A, \b TIMER_B, or \b TIMER_BOTH.
//! \param bStall specifies the response to a stall signal.
//!
//! This function controls the stall response for the specified timer.  If the
//! \e bStall parameter is \b true, then the timer stops counting if the
//! processor enters debug mode; otherwise the timer keeps running while in
//! debug mode.
//!
//! \return None.
//
//*****************************************************************************
void
TimerControlStall(unsigned long ulBase, unsigned long ulTimer,
                  tBoolean  bStall)
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));
    ASSERT((ulTimer == TIMER_A) || (ulTimer == TIMER_B) ||
           (ulTimer == TIMER_BOTH));

    //
    // Set the stall mode.
    //
    ulTimer &= TIMER_CTL_TASTALL | TIMER_CTL_TBSTALL;
    HWREG(ulBase + TIMER_O_CTL) = (bStall ?
                                   (HWREG(ulBase + TIMER_O_CTL) | ulTimer) :
                                   (HWREG(ulBase + TIMER_O_CTL) & ~(ulTimer)));
}

//*****************************************************************************
//
//! Set the timer prescale value.
//!
//! \param ulBase is the base address of the timer module.
//! \param ulTimer specifies the timer(s) to adjust; must be one of \b TIMER_A,
//! \b TIMER_B, or \b TIMER_BOTH.
//! \param ulValue is the timer prescale value which must be between 0 and 255
//! (inclusive) for 16/32-bit timers.
//!
//! This function sets the value of the input clock prescaler.  The prescaler
//! is only operational when in half-width mode and is used to extend the range
//! of the half-width timer modes.
//!
//! \return None.
//
//*****************************************************************************
void
TimerPrescaleSet(unsigned long ulBase, unsigned long ulTimer,
                 unsigned long ulValue)
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));
    ASSERT((ulTimer == TIMER_A) || (ulTimer == TIMER_B) ||
           (ulTimer == TIMER_BOTH));
    ASSERT(ulValue < 256);

    //
    // Set the timer A prescaler if requested.
    //
    if(ulTimer & TIMER_A)
    {
        HWREG(ulBase + TIMER_O_TAPR) = ulValue;
    }

    //
    // Set the timer B prescaler if requested.
    //
    if(ulTimer & TIMER_B)
    {
        HWREG(ulBase + TIMER_O_TBPR) = ulValue;
    }
}


//*****************************************************************************
//
//! Get the timer prescale value.
//!
//! \param ulBase is the base address of the timer module.
//! \param ulTimer specifies the timer; must be one of \b TIMER_A or
//! \b TIMER_B.
//!
//! This function gets the value of the input clock prescaler.  The prescaler
//! is only operational when in half-width mode and is used to extend the range
//! of the half-width timer modes.
//!
//! \return The value of the timer prescaler.
//
//*****************************************************************************

unsigned long
TimerPrescaleGet(unsigned long ulBase, unsigned long ulTimer)
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));
    ASSERT((ulTimer == TIMER_A) || (ulTimer == TIMER_B) ||
           (ulTimer == TIMER_BOTH));

    //
    // Return the appropriate prescale value.
    //
    return((ulTimer == TIMER_A) ? HWREG(ulBase + TIMER_O_TAPR) :
           HWREG(ulBase + TIMER_O_TBPR));
}

//*****************************************************************************
//
//! Set the timer prescale match value.
//!
//! \param ulBase is the base address of the timer module.
//! \param ulTimer specifies the timer(s) to adjust; must be one of \b TIMER_A,
//! \b TIMER_B, or \b TIMER_BOTH.
//! \param ulValue is the timer prescale match value which must be between 0
//! and 255 (inclusive) for 16/32-bit timers.
//!
//! This function sets the value of the input clock prescaler match value.
//! When in a half-width mode that uses the counter match and the prescaler,
//! the prescale match effectively extends the range of the match.
//!
//! \note The availability of the prescaler match varies with the
//! part and timer mode in use.  Please consult the datasheet for the part you
//! are using to determine whether this support is available.
//!
//! \return None.
//
//*****************************************************************************
void
TimerPrescaleMatchSet(unsigned long ulBase, unsigned long ulTimer,
                      unsigned long ulValue)
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));
    ASSERT((ulTimer == TIMER_A) || (ulTimer == TIMER_B) ||
           (ulTimer == TIMER_BOTH));
    ASSERT(ulValue < 256);

    //
    // Set the timer A prescale match if requested.
    //
    if(ulTimer & TIMER_A)
    {
        HWREG(ulBase + TIMER_O_TAPMR) = ulValue;
    }

    //
    // Set the timer B prescale match if requested.
    //
    if(ulTimer & TIMER_B)
    {
        HWREG(ulBase + TIMER_O_TBPMR) = ulValue;
    }
}

//*****************************************************************************
//
//! Get the timer prescale match value.
//!
//! \param ulBase is the base address of the timer module.
//! \param ulTimer specifies the timer; must be one of \b TIMER_A or
//! \b TIMER_B.
//!
//! This function gets the value of the input clock prescaler match value.
//! When in a half-width mode that uses the counter match and prescaler, the
//! prescale match effectively extends the range of the match.
//!
//! \note The availability of the prescaler match varies with the
//! part and timer mode in use.  Please consult the datasheet for the part you
//! are using to determine whether this support is available.
//!
//! \return The value of the timer prescale match.
//
//*****************************************************************************
unsigned long
TimerPrescaleMatchGet(unsigned long ulBase, unsigned long ulTimer)
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));
    ASSERT((ulTimer == TIMER_A) || (ulTimer == TIMER_B) ||
           (ulTimer == TIMER_BOTH));

    //
    // Return the appropriate prescale match value.
    //
    return((ulTimer == TIMER_A) ? HWREG(ulBase + TIMER_O_TAPMR) :
           HWREG(ulBase + TIMER_O_TBPMR));
}

//*****************************************************************************
//
//! Sets the timer load value.
//!
//! \param ulBase is the base address of the timer module.
//! \param ulTimer specifies the timer(s) to adjust; must be one of \b TIMER_A,
//! \b TIMER_B, or \b TIMER_BOTH.  Only \b TIMER_A should be used when the
//! timer is configured for full-width operation.
//! \param ulValue is the load value.
//!
//! This function sets the timer load value; if the timer is running then the
//! value is immediately loaded into the timer.
//!
//! \note This function can be used for both full- and half-width modes of
//! 16/32-bit timers.
//!
//! \return None.
//
//*****************************************************************************
void
TimerLoadSet(unsigned long ulBase, unsigned long ulTimer,
             unsigned long ulValue)
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));
    ASSERT((ulTimer == TIMER_A) || (ulTimer == TIMER_B) ||
           (ulTimer == TIMER_BOTH));

    //
    // Set the timer A load value if requested.
    //
    if(ulTimer & TIMER_A)
    {
        HWREG(ulBase + TIMER_O_TAILR) = ulValue;
    }

    //
    // Set the timer B load value if requested.
    //
    if(ulTimer & TIMER_B)
    {
        HWREG(ulBase + TIMER_O_TBILR) = ulValue;
    }
}

//*****************************************************************************
//
//! Gets the timer load value.
//!
//! \param ulBase is the base address of the timer module.
//! \param ulTimer specifies the timer; must be one of \b TIMER_A or
//! \b TIMER_B.  Only \b TIMER_A should be used when the timer is configured
//! for full-width operation.
//!
//! This function gets the currently programmed interval load value for the
//! specified timer.
//!
//! \note This function can be used for both full- and half-width modes of
//! 16/32-bit timers.
//!
//! \return Returns the load value for the timer.
//
//*****************************************************************************
unsigned long
TimerLoadGet(unsigned long ulBase, unsigned long ulTimer)
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));
    ASSERT((ulTimer == TIMER_A) || (ulTimer == TIMER_B));

    //
    // Return the appropriate load value.
    //
    return((ulTimer == TIMER_A) ? HWREG(ulBase + TIMER_O_TAILR) :
           HWREG(ulBase + TIMER_O_TBILR));
}

//*****************************************************************************
//
//! Gets the current timer value.
//!
//! \param ulBase is the base address of the timer module.
//! \param ulTimer specifies the timer; must be one of \b TIMER_A or
//! \b TIMER_B.  Only \b TIMER_A should be used when the timer is configured
//! for 32-bit operation.
//!
//! This function reads the current value of the specified timer.
//!
//! \return Returns the current value of the timer.
//
//*****************************************************************************
unsigned long
TimerValueGet(unsigned long ulBase, unsigned long ulTimer)
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));
    ASSERT((ulTimer == TIMER_A) || (ulTimer == TIMER_B));

    //
    // Return the appropriate timer value.
    //
    return((ulTimer == TIMER_A) ? HWREG(ulBase + TIMER_O_TAR) :
           HWREG(ulBase + TIMER_O_TBR));
}

//*****************************************************************************
//
//! Sets the current timer value.
//!
//! \param ulBase is the base address of the timer module.
//! \param ulTimer specifies the timer; must be one of \b TIMER_A or
//! \b TIMER_B.  Only \b TIMER_A should be used when the timer is configured
//! for 32-bit operation.
//! \param ulValue is the new value of the timer to be set.
//!
//! This function sets the current value of the specified timer.
//!
//! \return None.
//
//*****************************************************************************
void
TimerValueSet(unsigned long ulBase, unsigned long ulTimer,
              unsigned long ulValue)
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));
    ASSERT((ulTimer == TIMER_A) || (ulTimer == TIMER_B));

    //
    // Set the appropriate timer value.
    //
    if( (ulTimer == TIMER_A) )
    {
      HWREG(ulBase + TIMER_O_TAV) = ulValue;
    }
    else
    {
      HWREG(ulBase + TIMER_O_TBV) = ulValue;
    }
}


//*****************************************************************************
//
//! Sets the timer match value.
//!
//! \param ulBase is the base address of the timer module.
//! \param ulTimer specifies the timer(s) to adjust; must be one of \b TIMER_A,
//! \b TIMER_B, or \b TIMER_BOTH.  Only \b TIMER_A should be used when the
//! timer is configured for 32-bit operation.
//! \param ulValue is the match value.
//!
//! This function sets the match value for a timer.  This is used in capture
//! count mode to determine when to interrupt the processor and in PWM mode to
//! determine the duty cycle of the output signal.
//!
//! \return None.
//
//*****************************************************************************
void
TimerMatchSet(unsigned long ulBase, unsigned long ulTimer,
              unsigned long ulValue)
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));
    ASSERT((ulTimer == TIMER_A) || (ulTimer == TIMER_B) ||
           (ulTimer == TIMER_BOTH));

    //
    // Set the timer A match value if requested.
    //
    if(ulTimer & TIMER_A)
    {
        HWREG(ulBase + TIMER_O_TAMATCHR) = ulValue;
    }

    //
    // Set the timer B match value if requested.
    //
    if(ulTimer & TIMER_B)
    {
        HWREG(ulBase + TIMER_O_TBMATCHR) = ulValue;
    }
}

//*****************************************************************************
//
//! Gets the timer match value.
//!
//! \param ulBase is the base address of the timer module.
//! \param ulTimer specifies the timer; must be one of \b TIMER_A or
//! \b TIMER_B.  Only \b TIMER_A should be used when the timer is configured
//! for 32-bit operation.
//!
//! This function gets the match value for the specified timer.
//!
//! \return Returns the match value for the timer.
//
//********************************************************************************
unsigned long
TimerMatchGet(unsigned long ulBase, unsigned long ulTimer)
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));
    ASSERT((ulTimer == TIMER_A) || (ulTimer == TIMER_B));

    //
    // Return the appropriate match value.
    //
    return((ulTimer == TIMER_A) ? HWREG(ulBase + TIMER_O_TAMATCHR) :
           HWREG(ulBase + TIMER_O_TBMATCHR));
}


//*****************************************************************************
//
//! Registers an interrupt handler for the timer interrupt.
//!
//! \param ulBase is the base address of the timer module.
//! \param ulTimer specifies the timer(s); must be one of \b TIMER_A,
//! \b TIMER_B, or \b TIMER_BOTH.
//! \param pfnHandler is a pointer to the function to be called when the timer
//! interrupt occurs.
//!
//! This function sets the handler to be called when a timer interrupt occurs.
//! In addition, this function enables the global interrupt in the interrupt
//! controller; specific timer interrupts must be enabled via TimerIntEnable().
//! It is the interrupt handler's responsibility to clear the interrupt source
//! via TimerIntClear().
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \return None.
//
//*****************************************************************************
void
TimerIntRegister(unsigned long ulBase, unsigned long ulTimer,
                 void (*pfnHandler)(void))
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));
    ASSERT((ulTimer == TIMER_A) || (ulTimer == TIMER_B) ||
           (ulTimer == TIMER_BOTH));

    ulBase = ((ulBase == TIMERA0_BASE) ? INT_TIMERA0A :
              ((ulBase == TIMERA1_BASE) ? INT_TIMERA1A :
               ((ulBase == TIMERA2_BASE) ? INT_TIMERA2A : INT_TIMERA3A)));

    //
    // Register an interrupt handler for timer A if requested.
    //
    if(ulTimer & TIMER_A)
    {
        //
        // Register the interrupt handler.
        //
        IntRegister(ulBase, pfnHandler);

        //
        // Enable the interrupt.
        //
        IntEnable(ulBase);
    }

    //
    // Register an interrupt handler for timer B if requested.
    //
    if(ulTimer & TIMER_B)
    {
        //
        // Register the interrupt handler.
        //
        IntRegister(ulBase + 1, pfnHandler);

        //
        // Enable the interrupt.
        //
        IntEnable(ulBase + 1);
    }
}

//*****************************************************************************
//
//! Unregisters an interrupt handler for the timer interrupt.
//!
//! \param ulBase is the base address of the timer module.
//! \param ulTimer specifies the timer(s); must be one of \b TIMER_A,
//! \b TIMER_B, or \b TIMER_BOTH.
//!
//! This function clears the handler to be called when a timer interrupt
//! occurs.  This function also masks off the interrupt in the interrupt
//! controller so that the interrupt handler no longer is called.
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \return None.
//
//*****************************************************************************
void
TimerIntUnregister(unsigned long ulBase, unsigned long ulTimer)
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));
    ASSERT((ulTimer == TIMER_A) || (ulTimer == TIMER_B) ||
           (ulTimer == TIMER_BOTH));

    //
    // Get the interrupt number for this timer module.
    //

    ulBase = ((ulBase == TIMERA0_BASE) ? INT_TIMERA0A :
              ((ulBase == TIMERA1_BASE) ? INT_TIMERA1A :
               ((ulBase == TIMERA2_BASE) ? INT_TIMERA2A : INT_TIMERA3A)));



    //
    // Unregister the interrupt handler for timer A if requested.
    //
    if(ulTimer & TIMER_A)
    {
        //
        // Disable the interrupt.
        //
        IntDisable(ulBase);

        //
        // Unregister the interrupt handler.
        //
        IntUnregister(ulBase);
    }

    //
    // Unregister the interrupt handler for timer B if requested.
    //
    if(ulTimer & TIMER_B)
    {
        //
        // Disable the interrupt.
        //
        IntDisable(ulBase + 1);

        //
        // Unregister the interrupt handler.
        //
        IntUnregister(ulBase + 1);
    }
}

//*****************************************************************************
//
//! Enables individual timer interrupt sources.
//!
//! \param ulBase is the base address of the timer module.
//! \param ulIntFlags is the bit mask of the interrupt sources to be enabled.
//!
//! Enables the indicated timer interrupt sources.  Only the sources that are
//! enabled can be reflected to the processor interrupt; disabled sources have
//! no effect on the processor.
//!
//! The \e ulIntFlags parameter must be the logical OR of any combination of
//! the following:
//!
//! - \b TIMER_CAPB_EVENT  - Capture B event interrupt
//! - \b TIMER_CAPB_MATCH  - Capture B match interrupt
//! - \b TIMER_TIMB_TIMEOUT  - Timer B timeout interrupt
//! - \b TIMER_CAPA_EVENT  - Capture A event interrupt
//! - \b TIMER_CAPA_MATCH  - Capture A match interrupt
//! - \b TIMER_TIMA_TIMEOUT  - Timer A timeout interrupt
//!
//! \return None.
//
//*****************************************************************************
void
TimerIntEnable(unsigned long ulBase, unsigned long ulIntFlags)
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));

    //
    // Enable the specified interrupts.
    //
    HWREG(ulBase + TIMER_O_IMR) |= ulIntFlags;
}

//*****************************************************************************
//
//! Disables individual timer interrupt sources.
//!
//! \param ulBase is the base address of the timer module.
//! \param ulIntFlags is the bit mask of the interrupt sources to be disabled.
//!
//! Disables the indicated timer interrupt sources.  Only the sources that are
//! enabled can be reflected to the processor interrupt; disabled sources have
//! no effect on the processor.
//!
//! The \e ulIntFlags parameter has the same definition as the \e ulIntFlags
//! parameter to TimerIntEnable().
//!
//! \return None.
//
//*****************************************************************************
void
TimerIntDisable(unsigned long ulBase, unsigned long ulIntFlags)
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));

    //
    // Disable the specified interrupts.
    //
    HWREG(ulBase + TIMER_O_IMR) &= ~(ulIntFlags);
}

//*****************************************************************************
//
//! Gets the current interrupt status.
//!
//! \param ulBase is the base address of the timer module.
//! \param bMasked is false if the raw interrupt status is required and true if
//! the masked interrupt status is required.
//!
//! This function returns the interrupt status for the timer module.  Either
//! the raw interrupt status or the status of interrupts that are allowed to
//! reflect to the processor can be returned.
//!
//! \return The current interrupt status, enumerated as a bit field of
//! values described in TimerIntEnable().
//
//*****************************************************************************
unsigned long
TimerIntStatus(unsigned long ulBase, tBoolean bMasked)
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));

    //
    // Return either the interrupt status or the raw interrupt status as
    // requested.
    //
    return(bMasked ? HWREG(ulBase + TIMER_O_MIS) :
           HWREG(ulBase + TIMER_O_RIS));
}

//*****************************************************************************
//
//! Clears timer interrupt sources.
//!
//! \param ulBase is the base address of the timer module.
//! \param ulIntFlags is a bit mask of the interrupt sources to be cleared.
//!
//! The specified timer interrupt sources are cleared, so that they no longer
//! assert.  This function must be called in the interrupt handler to keep the
//! interrupt from being triggered again immediately upon exit.
//!
//! The \e ulIntFlags parameter has the same definition as the \e ulIntFlags
//! parameter to TimerIntEnable().
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
TimerIntClear(unsigned long ulBase, unsigned long ulIntFlags)
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));

    //
    // Clear the requested interrupt sources.
    //
    HWREG(ulBase + TIMER_O_ICR) = ulIntFlags;
}

//*****************************************************************************
//
//! Enables the events that can trigger a DMA request.
//!
//! \param ulBase is the base address of the timer module.
//! \param ulDMAEvent is a bit mask of the events that can trigger DMA.
//!
//! This function enables the timer events that can trigger the start of a DMA
//! sequence.  The DMA trigger events are specified in the \e ui32DMAEvent
//! parameter by passing in the logical OR of the following values:
//!
//! - \b TIMER_DMA_MODEMATCH_B - The mode match DMA trigger for timer B is
//!   enabled.
//! - \b TIMER_DMA_CAPEVENT_B - The capture event DMA trigger for timer B is
//!   enabled.
//! - \b TIMER_DMA_CAPMATCH_B - The capture match DMA trigger for timer B is
//!   enabled.
//! - \b TIMER_DMA_TIMEOUT_B - The timeout DMA trigger for timer B is enabled.
//! - \b TIMER_DMA_MODEMATCH_A - The mode match DMA trigger for timer A is
//!   enabled.
//! - \b TIMER_DMA_CAPEVENT_A - The capture event DMA trigger for timer A is
//!   enabled.
//! - \b TIMER_DMA_CAPMATCH_A - The capture match DMA trigger for timer A is
//!   enabled.
//! - \b TIMER_DMA_TIMEOUT_A - The timeout DMA trigger for timer A is enabled.
//!
//! \return None.
//
//*****************************************************************************
void
TimerDMAEventSet(unsigned long ulBase, unsigned long ulDMAEvent)
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));

    //
    // Set the DMA triggers.
    //
    HWREG(ulBase + TIMER_O_DMAEV) = ulDMAEvent;
}

//*****************************************************************************
//
//! Returns the events that can trigger a DMA request.
//!
//! \param ulBase is the base address of the timer module.
//!
//! This function returns the timer events that can trigger the start of a DMA
//! sequence.  The DMA trigger events are the logical OR of the following
//! values:
//!
//! - \b TIMER_DMA_MODEMATCH_B - Enables the mode match DMA trigger for timer
//!   B.
//! - \b TIMER_DMA_CAPEVENT_B - Enables the capture event DMA trigger for
//!   timer B.
//! - \b TIMER_DMA_CAPMATCH_B - Enables the capture match DMA trigger for
//!   timer B.
//! - \b TIMER_DMA_TIMEOUT_B - Enables the timeout DMA trigger for timer B.
//! - \b TIMER_DMA_MODEMATCH_A - Enables the mode match DMA trigger for timer
//!   A.
//! - \b TIMER_DMA_CAPEVENT_A - Enables the capture event DMA trigger for
//!   timer A.
//! - \b TIMER_DMA_CAPMATCH_A - Enables the capture match DMA trigger for
//!   timer A.
//! - \b TIMER_DMA_TIMEOUT_A - Enables the timeout DMA trigger for timer A.
//!
//! \return The timer events that trigger the uDMA.
//
//*****************************************************************************
unsigned long
TimerDMAEventGet(unsigned long ulBase)
{
    //
    // Check the arguments.
    //
    ASSERT(TimerBaseValid(ulBase));

    //
    // Return the current DMA triggers.
    //
    return(HWREG(ulBase + TIMER_O_DMAEV));
}
//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
