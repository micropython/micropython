//*****************************************************************************
//
//  interrupt.c
//
//  Driver for the NVIC Interrupt Controller.
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
//! \addtogroup interrupt_api
//! @{
//
//*****************************************************************************

#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "cpu.h"
#include "debug.h"
#include "interrupt.h"

//*****************************************************************************
//
// This is a mapping between priority grouping encodings and the number of
// preemption priority bits.
//
//*****************************************************************************
static const unsigned long g_pulPriority[] =
{
    NVIC_APINT_PRIGROUP_0_8, NVIC_APINT_PRIGROUP_1_7, NVIC_APINT_PRIGROUP_2_6,
    NVIC_APINT_PRIGROUP_3_5, NVIC_APINT_PRIGROUP_4_4, NVIC_APINT_PRIGROUP_5_3,
    NVIC_APINT_PRIGROUP_6_2, NVIC_APINT_PRIGROUP_7_1
};

//*****************************************************************************
//
// This is a mapping between interrupt number and the register that contains
// the priority encoding for that interrupt.
//
//*****************************************************************************
static const unsigned long g_pulRegs[] =
{
    0, NVIC_SYS_PRI1, NVIC_SYS_PRI2, NVIC_SYS_PRI3, NVIC_PRI0, NVIC_PRI1,
    NVIC_PRI2, NVIC_PRI3, NVIC_PRI4, NVIC_PRI5, NVIC_PRI6, NVIC_PRI7,
    NVIC_PRI8, NVIC_PRI9, NVIC_PRI10, NVIC_PRI11, NVIC_PRI12, NVIC_PRI13,
    NVIC_PRI14, NVIC_PRI15, NVIC_PRI16, NVIC_PRI17, NVIC_PRI18, NVIC_PRI19,
    NVIC_PRI20, NVIC_PRI21, NVIC_PRI22, NVIC_PRI23, NVIC_PRI24, NVIC_PRI25,
    NVIC_PRI26, NVIC_PRI27, NVIC_PRI28, NVIC_PRI29, NVIC_PRI30, NVIC_PRI31,
    NVIC_PRI32, NVIC_PRI33, NVIC_PRI34, NVIC_PRI35, NVIC_PRI36, NVIC_PRI37,
    NVIC_PRI38, NVIC_PRI39, NVIC_PRI40, NVIC_PRI41, NVIC_PRI42, NVIC_PRI43,
    NVIC_PRI44, NVIC_PRI45, NVIC_PRI46, NVIC_PRI47, NVIC_PRI48

};


//*****************************************************************************
//
// This is a mapping between interrupt number (for the peripheral interrupts
// only) and the register that contains the interrupt enable for that
// interrupt.
//
//*****************************************************************************
static const unsigned long g_pulEnRegs[] =
{
    NVIC_EN0, NVIC_EN1, NVIC_EN2, NVIC_EN3, NVIC_EN4, NVIC_EN5
};

//*****************************************************************************
//
// This is a mapping between interrupt number (for the peripheral interrupts
// only) and the register that contains the interrupt disable for that
// interrupt.
//
//*****************************************************************************
static const unsigned long g_pulDisRegs[] =
{
    NVIC_DIS0, NVIC_DIS1, NVIC_DIS2, NVIC_DIS3, NVIC_DIS4, NVIC_DIS5
};

//*****************************************************************************
//
// This is a mapping between interrupt number (for the peripheral interrupts
// only) and the register that contains the interrupt pend for that interrupt.
//
//*****************************************************************************
static const unsigned long g_pulPendRegs[] =
{
    NVIC_PEND0, NVIC_PEND1, NVIC_PEND2, NVIC_PEND3, NVIC_PEND4, NVIC_PEND5
};

//*****************************************************************************
//
// This is a mapping between interrupt number (for the peripheral interrupts
// only) and the register that contains the interrupt unpend for that
// interrupt.
//
//*****************************************************************************
static const unsigned long g_pulUnpendRegs[] =
{
    NVIC_UNPEND0, NVIC_UNPEND1, NVIC_UNPEND2, NVIC_UNPEND3, NVIC_UNPEND4,
    NVIC_UNPEND5
};


//*****************************************************************************
//
//! \internal
//! The default interrupt handler.
//!
//! This is the default interrupt handler for all interrupts.  It simply loops
//! forever so that the system state is preserved for observation by a
//! debugger.  Since interrupts should be disabled before unregistering the
//! corresponding handler, this should never be called.
//!
//! \return None.
//
//*****************************************************************************
static void
IntDefaultHandler(void)
{
    //
    // Go into an infinite loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
//! Enables the processor interrupt.
//!
//! Allows the processor to respond to interrupts.  This does not affect the
//! set of interrupts enabled in the interrupt controller; it just gates the
//! single interrupt from the controller to the processor.
//!
//! \note Previously, this function had no return value.  As such, it was
//! possible to include <tt>interrupt.h</tt> and call this function without
//! having included <tt>hw_types.h</tt>.  Now that the return is a
//! <tt>tBoolean</tt>, a compiler error will occur in this case.  The solution
//! is to include <tt>hw_types.h</tt> before including <tt>interrupt.h</tt>.
//!
//! \return Returns \b true if interrupts were disabled when the function was
//! called or \b false if they were initially enabled.
//
//*****************************************************************************
tBoolean
IntMasterEnable(void)
{
    //
    // Enable processor interrupts.
    //
    return(CPUcpsie());
}

//*****************************************************************************
//
//! Disables the processor interrupt.
//!
//! Prevents the processor from receiving interrupts.  This does not affect the
//! set of interrupts enabled in the interrupt controller; it just gates the
//! single interrupt from the controller to the processor.
//!
//! \note Previously, this function had no return value.  As such, it was
//! possible to include <tt>interrupt.h</tt> and call this function without
//! having included <tt>hw_types.h</tt>.  Now that the return is a
//! <tt>tBoolean</tt>, a compiler error will occur in this case.  The solution
//! is to include <tt>hw_types.h</tt> before including <tt>interrupt.h</tt>.
//!
//! \return Returns \b true if interrupts were already disabled when the
//! function was called or \b false if they were initially enabled.
//
//*****************************************************************************
tBoolean
IntMasterDisable(void)
{
    //
    // Disable processor interrupts.
    //
    return(CPUcpsid());
}
//*****************************************************************************
//
//! Sets the NVIC VTable base.
//!
//! \param ulVtableBase specifies the new base address of VTable
//!
//! This function is used to specify a new base address for the VTable.
//! This function must be called before using IntRegister() for registering
//! any interrupt handler.
//!
//!
//! \return None.
//
//*****************************************************************************
void
IntVTableBaseSet(unsigned long ulVtableBase)
{
    HWREG(NVIC_VTABLE) = ulVtableBase;
}

//*****************************************************************************
//
//! Registers a function to be called when an interrupt occurs.
//!
//! \param ulInterrupt specifies the interrupt in question.
//! \param pfnHandler is a pointer to the function to be called.
//!
//! This function is used to specify the handler function to be called when the
//! given interrupt is asserted to the processor.  When the interrupt occurs,
//! if it is enabled (via IntEnable()), the handler function will be called in
//! interrupt context.  Since the handler function can preempt other code, care
//! must be taken to protect memory or peripherals that are accessed by the
//! handler and other non-handler code.
//!
//!
//! \return None.
//
//*****************************************************************************
void
IntRegister(unsigned long ulInterrupt, void (*pfnHandler)(void))
{
    unsigned long *ulNvicTbl;

    //
    // Check the arguments.
    //
    ASSERT(ulInterrupt < NUM_INTERRUPTS);

    ulNvicTbl = (unsigned long *)HWREG(NVIC_VTABLE);
    ulNvicTbl[ulInterrupt]= (unsigned long)pfnHandler;
}

//*****************************************************************************
//
//! Unregisters the function to be called when an interrupt occurs.
//!
//! \param ulInterrupt specifies the interrupt in question.
//!
//! This function is used to indicate that no handler should be called when the
//! given interrupt is asserted to the processor.  The interrupt source will be
//! automatically disabled (via IntDisable()) if necessary.
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \return None.
//
//*****************************************************************************
void
IntUnregister(unsigned long ulInterrupt)
{
  unsigned long *ulNvicTbl;

  //
  // Check the arguments.
  //
  ASSERT(ulInterrupt < NUM_INTERRUPTS);

  ulNvicTbl = (unsigned long *)HWREG(NVIC_VTABLE);
  ulNvicTbl[ulInterrupt]= (unsigned long)IntDefaultHandler;
}

//*****************************************************************************
//
//! Sets the priority grouping of the interrupt controller.
//!
//! \param ulBits specifies the number of bits of preemptable priority.
//!
//! This function specifies the split between preemptable priority levels and
//! subpriority levels in the interrupt priority specification.  The range of
//! the grouping values are dependent upon the hardware implementation; on
//! the CC3200 , three bits are available for hardware interrupt
//! prioritization and therefore priority grouping values of three through
//! seven have the same effect.
//!
//! \return None.
//
//*****************************************************************************
void
IntPriorityGroupingSet(unsigned long ulBits)
{
    //
    // Check the arguments.
    //
    ASSERT(ulBits < NUM_PRIORITY);

    //
    // Set the priority grouping.
    //
    HWREG(NVIC_APINT) = NVIC_APINT_VECTKEY | g_pulPriority[ulBits];
}

//*****************************************************************************
//
//! Gets the priority grouping of the interrupt controller.
//!
//! This function returns the split between preemptable priority levels and
//! subpriority levels in the interrupt priority specification.
//!
//! \return The number of bits of preemptable priority.
//
//*****************************************************************************
unsigned long
IntPriorityGroupingGet(void)
{
    unsigned long ulLoop, ulValue;

    //
    // Read the priority grouping.
    //
    ulValue = HWREG(NVIC_APINT) & NVIC_APINT_PRIGROUP_M;

    //
    // Loop through the priority grouping values.
    //
    for(ulLoop = 0; ulLoop < NUM_PRIORITY; ulLoop++)
    {
        //
        // Stop looping if this value matches.
        //
        if(ulValue == g_pulPriority[ulLoop])
        {
            break;
        }
    }

    //
    // Return the number of priority bits.
    //
    return(ulLoop);
}

//*****************************************************************************
//
//! Sets the priority of an interrupt.
//!
//! \param ulInterrupt specifies the interrupt in question.
//! \param ucPriority specifies the priority of the interrupt.
//!
//! This function is used to set the priority of an interrupt.  When multiple
//! interrupts are asserted simultaneously, the ones with the highest priority
//! are processed before the lower priority interrupts.  Smaller numbers
//! correspond to higher interrupt priorities; priority 0 is the highest
//! interrupt priority.
//!
//! The hardware priority mechanism will only look at the upper N bits of the
//! priority level (where N is 3), so any prioritization must be performed in
//! those bits.  The remaining bits can be used to sub-prioritize the interrupt
//! sources, and may be used by the hardware priority mechanism on a future
//! part.  This arrangement allows priorities to migrate to different NVIC
//! implementations without changing the gross prioritization of the
//! interrupts.
//!
//! The parameter \e ucPriority can be any one of the following
//! -\b INT_PRIORITY_LVL_0
//! -\b INT_PRIORITY_LVL_1
//! -\b INT_PRIORITY_LVL_2
//! -\b INT_PRIORITY_LVL_3
//! -\b INT_PRIORITY_LVL_4
//! -\b INT_PRIORITY_LVL_5
//! -\b INT_PRIORITY_LVL_6
//! -\b INT_PRIORITY_LVL_7
//!
//! \return None.
//
//*****************************************************************************
void
IntPrioritySet(unsigned long ulInterrupt, unsigned char ucPriority)
{
    unsigned long ulTemp;

    //
    // Check the arguments.
    //
    ASSERT((ulInterrupt >= 4) && (ulInterrupt < NUM_INTERRUPTS));

    //
    // Set the interrupt priority.
    //
    ulTemp = HWREG(g_pulRegs[ulInterrupt >> 2]);
    ulTemp &= ~(0xFF << (8 * (ulInterrupt & 3)));
    ulTemp |= ucPriority << (8 * (ulInterrupt & 3));
    HWREG(g_pulRegs[ulInterrupt >> 2]) = ulTemp;
}

//*****************************************************************************
//
//! Gets the priority of an interrupt.
//!
//! \param ulInterrupt specifies the interrupt in question.
//!
//! This function gets the priority of an interrupt.  See IntPrioritySet() for
//! a definition of the priority value.
//!
//! \return Returns the interrupt priority, or -1 if an invalid interrupt was
//! specified.
//
//*****************************************************************************
long
IntPriorityGet(unsigned long ulInterrupt)
{
    //
    // Check the arguments.
    //
    ASSERT((ulInterrupt >= 4) && (ulInterrupt < NUM_INTERRUPTS));

    //
    // Return the interrupt priority.
    //
    return((HWREG(g_pulRegs[ulInterrupt >> 2]) >> (8 * (ulInterrupt & 3))) &
           0xFF);
}

//*****************************************************************************
//
//! Enables an interrupt.
//!
//! \param ulInterrupt specifies the interrupt to be enabled.
//!
//! The specified interrupt is enabled in the interrupt controller.  Other
//! enables for the interrupt (such as at the peripheral level) are unaffected
//! by this function.
//!
//! \return None.
//
//*****************************************************************************
void
IntEnable(unsigned long ulInterrupt)
{
    //
    // Check the arguments.
    //
    ASSERT(ulInterrupt < NUM_INTERRUPTS);

    //
    // Determine the interrupt to enable.
    //
    if(ulInterrupt == FAULT_MPU)
    {
        //
        // Enable the MemManage interrupt.
        //
        HWREG(NVIC_SYS_HND_CTRL) |= NVIC_SYS_HND_CTRL_MEM;
        __asm(" dsb     ");
        __asm(" isb     ");
    }
    else if(ulInterrupt == FAULT_BUS)
    {
        //
        // Enable the bus fault interrupt.
        //
        HWREG(NVIC_SYS_HND_CTRL) |= NVIC_SYS_HND_CTRL_BUS;
        __asm(" dsb     ");
        __asm(" isb     ");
    }
    else if(ulInterrupt == FAULT_USAGE)
    {
        //
        // Enable the usage fault interrupt.
        //
        HWREG(NVIC_SYS_HND_CTRL) |= NVIC_SYS_HND_CTRL_USAGE;
        __asm(" dsb     ");
        __asm(" isb     ");
    }
    else if(ulInterrupt == FAULT_SYSTICK)
    {
        //
        // Enable the System Tick interrupt.
        //
        HWREG(NVIC_ST_CTRL) |= NVIC_ST_CTRL_INTEN;
        __asm(" dsb     ");
        __asm(" isb     ");
    }
    else if(ulInterrupt >= 16)
    {
        //
        // Enable the general interrupt.
        //
        HWREG(g_pulEnRegs[(ulInterrupt - 16) / 32]) =
            1 << ((ulInterrupt - 16) & 31);
        __asm(" dsb     ");
        __asm(" isb     ");
    }
}

//*****************************************************************************
//
//! Disables an interrupt.
//!
//! \param ulInterrupt specifies the interrupt to be disabled.
//!
//! The specified interrupt is disabled in the interrupt controller.  Other
//! enables for the interrupt (such as at the peripheral level) are unaffected
//! by this function.
//!
//! \return None.
//
//*****************************************************************************
void
IntDisable(unsigned long ulInterrupt)
{
    //
    // Check the arguments.
    //
    ASSERT(ulInterrupt < NUM_INTERRUPTS);

    //
    // Determine the interrupt to disable.
    //
    if(ulInterrupt == FAULT_MPU)
    {
        //
        // Disable the MemManage interrupt.
        //
        HWREG(NVIC_SYS_HND_CTRL) &= ~(NVIC_SYS_HND_CTRL_MEM);
        __asm(" dsb     ");
        __asm(" isb     ");
    }
    else if(ulInterrupt == FAULT_BUS)
    {
        //
        // Disable the bus fault interrupt.
        //
        HWREG(NVIC_SYS_HND_CTRL) &= ~(NVIC_SYS_HND_CTRL_BUS);
        __asm(" dsb     ");
        __asm(" isb     ");
    }
    else if(ulInterrupt == FAULT_USAGE)
    {
        //
        // Disable the usage fault interrupt.
        //
        HWREG(NVIC_SYS_HND_CTRL) &= ~(NVIC_SYS_HND_CTRL_USAGE);
        __asm(" dsb     ");
        __asm(" isb     ");
    }
    else if(ulInterrupt == FAULT_SYSTICK)
    {
        //
        // Disable the System Tick interrupt.
        //
        HWREG(NVIC_ST_CTRL) &= ~(NVIC_ST_CTRL_INTEN);
        __asm(" dsb     ");
        __asm(" isb     ");
    }
    else if(ulInterrupt >= 16)
    {
        //
        // Disable the general interrupt.
        //
        HWREG(g_pulDisRegs[(ulInterrupt - 16) / 32]) =
            1 << ((ulInterrupt - 16) & 31);
        __asm(" dsb     ");
        __asm(" isb     ");
    }

}

//*****************************************************************************
//
//! Pends an interrupt.
//!
//! \param ulInterrupt specifies the interrupt to be pended.
//!
//! The specified interrupt is pended in the interrupt controller.  This will
//! cause the interrupt controller to execute the corresponding interrupt
//! handler at the next available time, based on the current interrupt state
//! priorities.  For example, if called by a higher priority interrupt handler,
//! the specified interrupt handler will not be called until after the current
//! interrupt handler has completed execution.  The interrupt must have been
//! enabled for it to be called.
//!
//! \return None.
//
//*****************************************************************************
void
IntPendSet(unsigned long ulInterrupt)
{
    //
    // Check the arguments.
    //
    ASSERT(ulInterrupt < NUM_INTERRUPTS);

    //
    // Determine the interrupt to pend.
    //
    if(ulInterrupt == FAULT_NMI)
    {
        //
        // Pend the NMI interrupt.
        //
        HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_NMI_SET;
        __asm(" dsb     ");
        __asm(" isb     ");
    }
    else if(ulInterrupt == FAULT_PENDSV)
    {
        //
        // Pend the PendSV interrupt.
        //
        HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV;
        __asm(" dsb     ");
        __asm(" isb     ");
    }
    else if(ulInterrupt == FAULT_SYSTICK)
    {
        //
        // Pend the SysTick interrupt.
        //
        HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PENDSTSET;
        __asm(" dsb     ");
        __asm(" isb     ");
    }
    else if(ulInterrupt >= 16)
    {
        //
        // Pend the general interrupt.
        //
        HWREG(g_pulPendRegs[(ulInterrupt - 16) / 32]) =
            1 << ((ulInterrupt - 16) & 31);
        __asm(" dsb     ");
        __asm(" isb     ");
    }

}

//*****************************************************************************
//
//! Unpends an interrupt.
//!
//! \param ulInterrupt specifies the interrupt to be unpended.
//!
//! The specified interrupt is unpended in the interrupt controller.  This will
//! cause any previously generated interrupts that have not been handled yet
//! (due to higher priority interrupts or the interrupt no having been enabled
//! yet) to be discarded.
//!
//! \return None.
//
//*****************************************************************************
void
IntPendClear(unsigned long ulInterrupt)
{
    //
    // Check the arguments.
    //
    ASSERT(ulInterrupt < NUM_INTERRUPTS);

    //
    // Determine the interrupt to unpend.
    //
    if(ulInterrupt == FAULT_PENDSV)
    {
        //
        // Unpend the PendSV interrupt.
        //
        HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_UNPEND_SV;
    }
    else if(ulInterrupt == FAULT_SYSTICK)
    {
        //
        // Unpend the SysTick interrupt.
        //
        HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PENDSTCLR;
    }
    else if(ulInterrupt >= 16)
    {
        //
        // Unpend the general interrupt.
        //
        HWREG(g_pulUnpendRegs[(ulInterrupt - 16) / 32]) =
            1 << ((ulInterrupt - 16) & 31);
    }
}

//*****************************************************************************
//
//! Sets the priority masking level
//!
//! \param ulPriorityMask is the priority level that will be masked.
//!
//! This function sets the interrupt priority masking level so that all
//! interrupts at the specified or lesser priority level is masked.  This
//! can be used to globally disable a set of interrupts with priority below
//! a predetermined threshold.  A value of 0 disables priority
//! masking.
//!
//! Smaller numbers correspond to higher interrupt priorities.  So for example
//! a priority level mask of 4 will allow interrupts of priority level 0-3,
//! and interrupts with a numerical priority of 4 and greater will be blocked.
//!
//! The hardware priority mechanism will only look at the upper N bits of the
//! priority level (where N is 3), so any
//! prioritization must be performed in those bits.
//!
//! \return None.
//
//*****************************************************************************
void
IntPriorityMaskSet(unsigned long ulPriorityMask)
{
    CPUbasepriSet(ulPriorityMask);
}

//*****************************************************************************
//
//! Gets the priority masking level
//!
//! This function gets the current setting of the interrupt priority masking
//! level.  The value returned is the priority level such that all interrupts
//! of that and lesser priority are masked.  A value of 0 means that priority
//! masking is disabled.
//!
//! Smaller numbers correspond to higher interrupt priorities.  So for example
//! a priority level mask of 4 will allow interrupts of priority level 0-3,
//! and interrupts with a numerical priority of 4 and greater will be blocked.
//!
//! The hardware priority mechanism will only look at the upper N bits of the
//! priority level (where N is 3), so any
//! prioritization must be performed in those bits.
//!
//! \return Returns the value of the interrupt priority level mask.
//
//*****************************************************************************
unsigned long
IntPriorityMaskGet(void)
{
    return(CPUbasepriGet());
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
