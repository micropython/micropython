//*****************************************************************************
//
//  i2c.c
//
//  Driver for Inter-IC (I2C) bus block.
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
//! \addtogroup I2C_api
//! @{
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "debug.h"
#include "i2c.h"
#include "interrupt.h"

//*****************************************************************************
//
// A mapping of I2C base address to interrupt number.
//
//*****************************************************************************
static const uint32_t g_ppui32I2CIntMap[][2] =
{
    { I2CA0_BASE, INT_I2CA0},
};

static const int_fast8_t g_i8I2CIntMapRows =
    sizeof(g_ppui32I2CIntMap) / sizeof(g_ppui32I2CIntMap[0]);

//*****************************************************************************
//
//! \internal
//! Checks an I2C base address.
//!
//! \param ui32Base is the base address of the I2C module.
//!
//! This function determines if a I2C module base address is valid.
//!
//! \return Returns \b true if the base address is valid and \b false
//! otherwise.
//
//*****************************************************************************
#ifdef DEBUG
static bool
_I2CBaseValid(uint32_t ui32Base)
{
    return((ui32Base == I2CA0_BASE));
}
#else
#define _I2CBaseValid(ui32Base)         (ui32Base)
#endif

//*****************************************************************************
//
//! \internal
//! Gets the I2C interrupt number.
//!
//! \param ui32Base is the base address of the I2C Master module.
//!
//! Given a I2C base address, this function returns the corresponding
//! interrupt number.
//!
//! \return Returns an I2C interrupt number, or 0 if \e ui32Base is invalid.
//
//*****************************************************************************
static uint32_t
_I2CIntNumberGet(uint32_t ui32Base)
{
    int_fast8_t i8Idx, i8Rows;
    const uint32_t (*ppui32I2CIntMap)[2];

    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    ppui32I2CIntMap = g_ppui32I2CIntMap;
    i8Rows = g_i8I2CIntMapRows;

    //
    // Loop through the table that maps I2C base addresses to interrupt
    // numbers.
    //
    for(i8Idx = 0; i8Idx < i8Rows; i8Idx++)
    {
        //
        // See if this base address matches.
        //
        if(ppui32I2CIntMap[i8Idx][0] == ui32Base)
        {
            //
            // Return the corresponding interrupt number.
            //
            return(ppui32I2CIntMap[i8Idx][1]);
        }
    }

    //
    // The base address could not be found, so return an error.
    //
    return(0);
}

//*****************************************************************************
//
//! Initializes the I2C Master block.
//!
//! \param ui32Base is the base address of the I2C Master module.
//! \param ui32I2CClk is the rate of the clock supplied to the I2C module.
//! \param bFast set up for fast data transfers.
//!
//! This function initializes operation of the I2C Master block by configuring
//! the bus speed for the master and enabling the I2C Master block.
//!
//! If the parameter \e bFast is \b true, then the master block is set up to
//! transfer data at 400 Kbps; otherwise, it is set up to transfer data at
//! 100 Kbps.  If Fast Mode Plus (1 Mbps) is desired, software should manually
//! write the I2CMTPR after calling this function.  For High Speed (3.4 Mbps)
//! mode, a specific command is used to switch to the faster clocks after the
//! initial communication with the slave is done at either 100 Kbps or
//! 400 Kbps.
//!
//! The peripheral clock is the same as the processor clock.  This value is
//! returned by SysCtlClockGet(), or it can be explicitly hard coded if it is
//! constant and known (to save the code/execution overhead of a call to
//! SysCtlClockGet()).
//!
//! \return None.
//
//*****************************************************************************
void
I2CMasterInitExpClk(uint32_t ui32Base, uint32_t ui32SCLFreq)
{
    uint32_t ui32TPR;

    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Must enable the device before doing anything else.
    //
    I2CMasterEnable(ui32Base);

    //
    // Compute the clock divider that achieves the fastest speed less than or
    // equal to the desired speed.  The numerator is biased to favor a larger
    // clock divider so that the resulting clock is always less than or equal
    // to the desired clock, never greater.
    //
    ui32TPR = ((80000000 + (2 * 10 * ui32SCLFreq) - 1) /
               (2 * 10 * ui32SCLFreq)) - 1;
    HWREG(ui32Base + I2C_O_MTPR) = ui32TPR;

    //
    // Check to see if this I2C peripheral is High-Speed enabled.  If yes, also
    // choose the fastest speed that is less than or equal to 3.4 Mbps.
    //
    if(HWREG(ui32Base + I2C_O_PP) & I2C_PP_HS)
    {
        ui32TPR = ((80000000 + (2 * 3 * 3400000) - 1) /
                   (2 * 3 * 3400000)) - 1;
        HWREG(ui32Base + I2C_O_MTPR) = I2C_MTPR_HS | ui32TPR;
    }
}

//*****************************************************************************
//
//! Initializes the I2C Slave block.
//!
//! \param ui32Base is the base address of the I2C Slave module.
//! \param ui8SlaveAddr 7-bit slave address
//!
//! This function initializes operation of the I2C Slave block by configuring
//! the slave address and enabling the I2C Slave block.
//!
//! The parameter \e ui8SlaveAddr is the value that is compared against the
//! slave address sent by an I2C master.
//!
//! \return None.
//
//*****************************************************************************
void
I2CSlaveInit(uint32_t ui32Base, uint8_t ui8SlaveAddr)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));
    ASSERT(!(ui8SlaveAddr & 0x80));

    //
    // Must enable the device before doing anything else.
    //
    I2CSlaveEnable(ui32Base);

    //
    // Set up the slave address.
    //
    HWREG(ui32Base + I2C_O_SOAR) = ui8SlaveAddr;
}

//*****************************************************************************
//
//! Sets the I2C slave address.
//!
//! \param ui32Base is the base address of the I2C Slave module.
//! \param ui8AddrNum determines which slave address is set.
//! \param ui8SlaveAddr is the 7-bit slave address
//!
//! This function writes the specified slave address.  The \e ui32AddrNum field
//! dictates which slave address is configured.  For example, a value of 0
//! configures the primary address and a value of 1 configures the secondary.
//!
//!
//! \return None.
//
//*****************************************************************************
void
I2CSlaveAddressSet(uint32_t ui32Base, uint8_t ui8AddrNum, uint8_t ui8SlaveAddr)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));
    ASSERT(!(ui8AddrNum > 1));
    ASSERT(!(ui8SlaveAddr & 0x80));

    //
    // Determine which slave address is being set.
    //
    switch(ui8AddrNum)
    {
        //
        // Set up the primary slave address.
        //
        case 0:
        {
            HWREG(ui32Base + I2C_O_SOAR) = ui8SlaveAddr;
            break;
        }

        //
        // Set up and enable the secondary slave address.
        //
        case 1:
        {
            HWREG(ui32Base + I2C_O_SOAR2) = I2C_SOAR2_OAR2EN | ui8SlaveAddr;
            break;
        }
    }
}

//*****************************************************************************
//
//! Enables the I2C Master block.
//!
//! \param ui32Base is the base address of the I2C Master module.
//!
//! This function enables operation of the I2C Master block.
//!
//! \return None.
//
//*****************************************************************************
void
I2CMasterEnable(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Enable the master block.
    //
    HWREG(ui32Base + I2C_O_MCR) |= I2C_MCR_MFE;
}

//*****************************************************************************
//
//! Enables the I2C Slave block.
//!
//! \param ui32Base is the base address of the I2C Slave module.
//!
//! This fucntion enables operation of the I2C Slave block.
//!
//! \return None.
//
//*****************************************************************************
void
I2CSlaveEnable(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Enable the clock to the slave block.
    //
    HWREG(ui32Base + I2C_O_MCR) |= I2C_MCR_SFE;

    //
    // Enable the slave.
    //
    HWREG(ui32Base + I2C_O_SCSR) = I2C_SCSR_DA;
}

//*****************************************************************************
//
//! Disables the I2C master block.
//!
//! \param ui32Base is the base address of the I2C Master module.
//!
//! This function disables operation of the I2C master block.
//!
//! \return None.
//
//*****************************************************************************
void
I2CMasterDisable(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Disable the master block.
    //
    HWREG(ui32Base + I2C_O_MCR) &= ~(I2C_MCR_MFE);
}

//*****************************************************************************
//
//! Disables the I2C slave block.
//!
//! \param ui32Base is the base address of the I2C Slave module.
//!
//! This function disables operation of the I2C slave block.
//!
//! \return None.
//
//*****************************************************************************
void
I2CSlaveDisable(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Disable the slave.
    //
    HWREG(ui32Base + I2C_O_SCSR) = 0;

    //
    // Disable the clock to the slave block.
    //
    HWREG(ui32Base + I2C_O_MCR) &= ~(I2C_MCR_SFE);
}

//*****************************************************************************
//
//! Registers an interrupt handler for the I2C module.
//!
//! \param ui32Base is the base address of the I2C Master module.
//! \param pfnHandler is a pointer to the function to be called when the
//! I2C interrupt occurs.
//!
//! This function sets the handler to be called when an I2C interrupt occurs.
//! This function enables the global interrupt in the interrupt controller;
//! specific I2C interrupts must be enabled via I2CMasterIntEnable() and
//! I2CSlaveIntEnable().  If necessary, it is the interrupt handler's
//! responsibility to clear the interrupt source via I2CMasterIntClear() and
//! I2CSlaveIntClear().
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \return None.
//
//*****************************************************************************
void
I2CIntRegister(uint32_t ui32Base, void (*pfnHandler)(void))
{
    uint32_t ui32Int;

    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Determine the interrupt number based on the I2C port.
    //
    ui32Int = _I2CIntNumberGet(ui32Base);

    ASSERT(ui32Int != 0);

    //
    // Register the interrupt handler, returning an error if an error occurs.
    //
    IntRegister(ui32Int, pfnHandler);

    //
    // Enable the I2C interrupt.
    //
    IntEnable(ui32Int);
}

//*****************************************************************************
//
//! Unregisters an interrupt handler for the I2C module.
//!
//! \param ui32Base is the base address of the I2C Master module.
//!
//! This function clears the handler to be called when an I2C interrupt
//! occurs.  This function also masks off the interrupt in the interrupt r
//! controller so that the interrupt handler no longer is called.
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \return None.
//
//*****************************************************************************
void
I2CIntUnregister(uint32_t ui32Base)
{
    uint32_t ui32Int;

    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Determine the interrupt number based on the I2C port.
    //
    ui32Int = _I2CIntNumberGet(ui32Base);

    ASSERT(ui32Int != 0);

    //
    // Disable the interrupt.
    //
    IntDisable(ui32Int);

    //
    // Unregister the interrupt handler.
    //
    IntUnregister(ui32Int);
}

//*****************************************************************************
//
//! Enables the I2C Master interrupt.
//!
//! \param ui32Base is the base address of the I2C Master module.
//!
//! This function enables the I2C Master interrupt source.
//!
//! \return None.
//
//*****************************************************************************
void
I2CMasterIntEnable(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Enable the master interrupt.
    //
    HWREG(ui32Base + I2C_O_MIMR) = 1;
}

//*****************************************************************************
//
//! Enables individual I2C Master interrupt sources.
//!
//! \param ui32Base is the base address of the I2C Master module.
//! \param ui32IntFlags is the bit mask of the interrupt sources to be enabled.
//!
//! This function enables the indicated I2C Master interrupt sources.  Only the
//! sources that are enabled can be reflected to the processor interrupt;
//! disabled sources have no effect on the processor.
//!
//! The \e ui32IntFlags parameter is the logical OR of any of the following:
//!
//! - \b I2C_MASTER_INT_RX_FIFO_FULL - RX FIFO Full interrupt
//! - \b I2C_MASTER_INT_TX_FIFO_EMPTY - TX FIFO Empty interrupt
//! - \b I2C_MASTER_INT_RX_FIFO_REQ - RX FIFO Request interrupt
//! - \b I2C_MASTER_INT_TX_FIFO_REQ - TX FIFO Request interrupt
//! - \b I2C_MASTER_INT_ARB_LOST - Arbitration Lost interrupt
//! - \b I2C_MASTER_INT_STOP - Stop Condition interrupt
//! - \b I2C_MASTER_INT_START - Start Condition interrupt
//! - \b I2C_MASTER_INT_NACK - Address/Data NACK interrupt
//! - \b I2C_MASTER_INT_TX_DMA_DONE - TX DMA Complete interrupt
//! - \b I2C_MASTER_INT_RX_DMA_DONE - RX DMA Complete interrupt
//! - \b I2C_MASTER_INT_TIMEOUT - Clock Timeout interrupt
//! - \b I2C_MASTER_INT_DATA - Data interrupt
//!
//!
//! \return None.
//
//*****************************************************************************
void
I2CMasterIntEnableEx(uint32_t ui32Base, uint32_t ui32IntFlags)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Enable the master interrupt.
    //
    HWREG(ui32Base + I2C_O_MIMR) |= ui32IntFlags;
}

//*****************************************************************************
//
//! Enables the I2C Slave interrupt.
//!
//! \param ui32Base is the base address of the I2C Slave module.
//!
//! This function enables the I2C Slave interrupt source.
//!
//! \return None.
//
//*****************************************************************************
void
I2CSlaveIntEnable(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Enable the slave interrupt.
    //
    HWREG(ui32Base + I2C_O_SIMR) |= I2C_SLAVE_INT_DATA;
}

//*****************************************************************************
//
//! Enables individual I2C Slave interrupt sources.
//!
//! \param ui32Base is the base address of the I2C Slave module.
//! \param ui32IntFlags is the bit mask of the interrupt sources to be enabled.
//!
//! This function enables the indicated I2C Slave interrupt sources.  Only the
//! sources that are enabled can be reflected to the processor interrupt;
//! disabled sources have no effect on the processor.
//!
//! The \e ui32IntFlags parameter is the logical OR of any of the following:
//!
//! - \b I2C_SLAVE_INT_RX_FIFO_FULL - RX FIFO Full interrupt
//! - \b I2C_SLAVE_INT_TX_FIFO_EMPTY - TX FIFO Empty interrupt
//! - \b I2C_SLAVE_INT_RX_FIFO_REQ - RX FIFO Request interrupt
//! - \b I2C_SLAVE_INT_TX_FIFO_REQ - TX FIFO Request interrupt
//! - \b I2C_SLAVE_INT_TX_DMA_DONE - TX DMA Complete interrupt
//! - \b I2C_SLAVE_INT_RX_DMA_DONE - RX DMA Complete interrupt
//! - \b I2C_SLAVE_INT_STOP - Stop condition detected interrupt
//! - \b I2C_SLAVE_INT_START - Start condition detected interrupt
//! - \b I2C_SLAVE_INT_DATA - Data interrupt
//!
//!
//! \return None.
//
//*****************************************************************************
void
I2CSlaveIntEnableEx(uint32_t ui32Base, uint32_t ui32IntFlags)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Enable the slave interrupt.
    //
    HWREG(ui32Base + I2C_O_SIMR) |= ui32IntFlags;
}

//*****************************************************************************
//
//! Disables the I2C Master interrupt.
//!
//! \param ui32Base is the base address of the I2C Master module.
//!
//! This function disables the I2C Master interrupt source.
//!
//! \return None.
//
//*****************************************************************************
void
I2CMasterIntDisable(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Disable the master interrupt.
    //
    HWREG(ui32Base + I2C_O_MIMR) = 0;
}

//*****************************************************************************
//
//! Disables individual I2C Master interrupt sources.
//!
//! \param ui32Base is the base address of the I2C Master module.
//! \param ui32IntFlags is the bit mask of the interrupt sources to be
//!        disabled.
//!
//! This function disables the indicated I2C Master interrupt sources.  Only
//! the sources that are enabled can be reflected to the processor interrupt;
//! disabled sources have no effect on the processor.
//!
//! The \e ui32IntFlags parameter has the same definition as the
//! \e ui32IntFlags parameter to I2CMasterIntEnableEx().
//!
//! \return None.
//
//*****************************************************************************
void
I2CMasterIntDisableEx(uint32_t ui32Base, uint32_t ui32IntFlags)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Disable the master interrupt.
    //
    HWREG(ui32Base + I2C_O_MIMR) &= ~ui32IntFlags;
}

//*****************************************************************************
//
//! Disables the I2C Slave interrupt.
//!
//! \param ui32Base is the base address of the I2C Slave module.
//!
//! This function disables the I2C Slave interrupt source.
//!
//! \return None.
//
//*****************************************************************************
void
I2CSlaveIntDisable(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Disable the slave interrupt.
    //
    HWREG(ui32Base + I2C_O_SIMR) &= ~I2C_SLAVE_INT_DATA;
}

//*****************************************************************************
//
//! Disables individual I2C Slave interrupt sources.
//!
//! \param ui32Base is the base address of the I2C Slave module.
//! \param ui32IntFlags is the bit mask of the interrupt sources to be
//!        disabled.
//!
//! This function disables the indicated I2C Slave interrupt sources.  Only
//! the sources that are enabled can be reflected to the processor interrupt;
//! disabled sources have no effect on the processor.
//!
//! The \e ui32IntFlags parameter has the same definition as the
//! \e ui32IntFlags parameter to I2CSlaveIntEnableEx().
//!
//! \return None.
//
//*****************************************************************************
void
I2CSlaveIntDisableEx(uint32_t ui32Base, uint32_t ui32IntFlags)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Disable the slave interrupt.
    //
    HWREG(ui32Base + I2C_O_SIMR) &= ~ui32IntFlags;
}

//*****************************************************************************
//
//! Gets the current I2C Master interrupt status.
//!
//! \param ui32Base is the base address of the I2C Master module.
//! \param bMasked is false if the raw interrupt status is requested and
//! true if the masked interrupt status is requested.
//!
//! This function returns the interrupt status for the I2C Master module.
//! Either the raw interrupt status or the status of interrupts that are
//! allowed to reflect to the processor can be returned.
//!
//! \return The current interrupt status, returned as \b true if active
//! or \b false if not active.
//
//*****************************************************************************
bool
I2CMasterIntStatus(uint32_t ui32Base, bool bMasked)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Return either the interrupt status or the raw interrupt status as
    // requested.
    //
    if(bMasked)
    {
        return((HWREG(ui32Base + I2C_O_MMIS)) ? true : false);
    }
    else
    {
        return((HWREG(ui32Base + I2C_O_MRIS)) ? true : false);
    }
}

//*****************************************************************************
//
//! Gets the current I2C Master interrupt status.
//!
//! \param ui32Base is the base address of the I2C Master module.
//! \param bMasked is false if the raw interrupt status is requested and
//! true if the masked interrupt status is requested.
//!
//! This function returns the interrupt status for the I2C Master module.
//! Either the raw interrupt status or the status of interrupts that are
//! allowed to reflect to the processor can be returned.
//!
//! \return Returns the current interrupt status, enumerated as a bit field of
//! values described in I2CMasterIntEnableEx().
//
//*****************************************************************************
uint32_t
I2CMasterIntStatusEx(uint32_t ui32Base, bool bMasked)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Return either the interrupt status or the raw interrupt status as
    // requested.
    //
    if(bMasked)
    {
        return(HWREG(ui32Base + I2C_O_MMIS));
    }
    else
    {
        return(HWREG(ui32Base + I2C_O_MRIS));
    }
}

//*****************************************************************************
//
//! Gets the current I2C Slave interrupt status.
//!
//! \param ui32Base is the base address of the I2C Slave module.
//! \param bMasked is false if the raw interrupt status is requested and
//! true if the masked interrupt status is requested.
//!
//! This function returns the interrupt status for the I2C Slave module.
//! Either the raw interrupt status or the status of interrupts that are
//! allowed to reflect to the processor can be returned.
//!
//! \return The current interrupt status, returned as \b true if active
//! or \b false if not active.
//
//*****************************************************************************
bool
I2CSlaveIntStatus(uint32_t ui32Base, bool bMasked)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Return either the interrupt status or the raw interrupt status as
    // requested.
    //
    if(bMasked)
    {
        return((HWREG(ui32Base + I2C_O_SMIS)) ? true : false);
    }
    else
    {
        return((HWREG(ui32Base + I2C_O_SRIS)) ? true : false);
    }
}

//*****************************************************************************
//
//! Gets the current I2C Slave interrupt status.
//!
//! \param ui32Base is the base address of the I2C Slave module.
//! \param bMasked is false if the raw interrupt status is requested and
//! true if the masked interrupt status is requested.
//!
//! This function returns the interrupt status for the I2C Slave module.
//! Either the raw interrupt status or the status of interrupts that are
//! allowed to reflect to the processor can be returned.
//!
//! \return Returns the current interrupt status, enumerated as a bit field of
//! values described in I2CSlaveIntEnableEx().
//
//*****************************************************************************
uint32_t
I2CSlaveIntStatusEx(uint32_t ui32Base, bool bMasked)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Return either the interrupt status or the raw interrupt status as
    // requested.
    //
    if(bMasked)
    {
        return(HWREG(ui32Base + I2C_O_SMIS));
    }
    else
    {
        return(HWREG(ui32Base + I2C_O_SRIS));
    }
}

//*****************************************************************************
//
//! Clears I2C Master interrupt sources.
//!
//! \param ui32Base is the base address of the I2C Master module.
//!
//! The I2C Master interrupt source is cleared, so that it no longer
//! asserts.  This function must be called in the interrupt handler to keep the
//! interrupt from being triggered again immediately upon exit.
//!
//! \note Because there is a write buffer in the Cortex-M processor, it may
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
I2CMasterIntClear(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Clear the I2C master interrupt source.
    //
    HWREG(ui32Base + I2C_O_MICR) = I2C_MICR_IC;

    //
    // Workaround for I2C master interrupt clear errata for some
    // devices.  For later devices, this write is ignored and therefore
    // harmless (other than the slight performance hit).
    //
    HWREG(ui32Base + I2C_O_MMIS) = I2C_MICR_IC;
}

//*****************************************************************************
//
//! Clears I2C Master interrupt sources.
//!
//! \param ui32Base is the base address of the I2C Master module.
//! \param ui32IntFlags is a bit mask of the interrupt sources to be cleared.
//!
//! The specified I2C Master interrupt sources are cleared, so that they no
//! longer assert.  This function must be called in the interrupt handler to
//! keep the interrupt from being triggered again immediately upon exit.
//!
//! The \e ui32IntFlags parameter has the same definition as the
//! \e ui32IntFlags parameter to I2CMasterIntEnableEx().
//!
//! \note Because there is a write buffer in the Cortex-M processor, it may
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
I2CMasterIntClearEx(uint32_t ui32Base, uint32_t ui32IntFlags)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Clear the I2C master interrupt source.
    //
    HWREG(ui32Base + I2C_O_MICR) = ui32IntFlags;
}

//*****************************************************************************
//
//! Clears I2C Slave interrupt sources.
//!
//! \param ui32Base is the base address of the I2C Slave module.
//!
//! The I2C Slave interrupt source is cleared, so that it no longer asserts.
//! This function must be called in the interrupt handler to keep the interrupt
//! from being triggered again immediately upon exit.
//!
//! \note Because there is a write buffer in the Cortex-M processor, it may
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
I2CSlaveIntClear(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Clear the I2C slave interrupt source.
    //
    HWREG(ui32Base + I2C_O_SICR) = I2C_SICR_DATAIC;
}

//*****************************************************************************
//
//! Clears I2C Slave interrupt sources.
//!
//! \param ui32Base is the base address of the I2C Slave module.
//! \param ui32IntFlags is a bit mask of the interrupt sources to be cleared.
//!
//! The specified I2C Slave interrupt sources are cleared, so that they no
//! longer assert.  This function must be called in the interrupt handler to
//! keep the interrupt from being triggered again immediately upon exit.
//!
//! The \e ui32IntFlags parameter has the same definition as the
//! \e ui32IntFlags parameter to I2CSlaveIntEnableEx().
//!
//! \note Because there is a write buffer in the Cortex-M processor, it may
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
I2CSlaveIntClearEx(uint32_t ui32Base, uint32_t ui32IntFlags)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Clear the I2C slave interrupt source.
    //
    HWREG(ui32Base + I2C_O_SICR) = ui32IntFlags;
}

//*****************************************************************************
//
//! Sets the address that the I2C Master places on the bus.
//!
//! \param ui32Base is the base address of the I2C Master module.
//! \param ui8SlaveAddr 7-bit slave address
//! \param bReceive flag indicating the type of communication with the slave
//!
//! This function configures the address that the I2C Master places on the
//! bus when initiating a transaction.  When the \e bReceive parameter is set
//! to \b true, the address indicates that the I2C Master is initiating a
//! read from the slave; otherwise the address indicates that the I2C
//! Master is initiating a write to the slave.
//!
//! \return None.
//
//*****************************************************************************
void
I2CMasterSlaveAddrSet(uint32_t ui32Base, uint8_t ui8SlaveAddr,
                      bool bReceive)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));
    ASSERT(!(ui8SlaveAddr & 0x80));

    //
    // Set the address of the slave with which the master will communicate.
    //
    HWREG(ui32Base + I2C_O_MSA) = (ui8SlaveAddr << 1) | bReceive;
}

//*****************************************************************************
//
//! Reads the state of the SDA and SCL pins.
//!
//! \param ui32Base is the base address of the I2C Master module.
//!
//! This function returns the state of the I2C bus by providing the real time
//! values of the SDA and SCL pins.
//!
//!
//! \return Returns the state of the bus with SDA in bit position 1 and SCL in
//! bit position 0.
//
//*****************************************************************************
uint32_t
I2CMasterLineStateGet(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Return the line state.
    //
    return(HWREG(ui32Base + I2C_O_MBMON));
}

//*****************************************************************************
//
//! Indicates whether or not the I2C Master is busy.
//!
//! \param ui32Base is the base address of the I2C Master module.
//!
//! This function returns an indication of whether or not the I2C Master is
//! busy transmitting or receiving data.
//!
//! \return Returns \b true if the I2C Master is busy; otherwise, returns
//! \b false.
//
//*****************************************************************************
bool
I2CMasterBusy(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Return the busy status.
    //
    if(HWREG(ui32Base + I2C_O_MCS) & I2C_MCS_BUSY)
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

//*****************************************************************************
//
//! Indicates whether or not the I2C bus is busy.
//!
//! \param ui32Base is the base address of the I2C Master module.
//!
//! This function returns an indication of whether or not the I2C bus is busy.
//! This function can be used in a multi-master environment to determine if
//! another master is currently using the bus.
//!
//! \return Returns \b true if the I2C bus is busy; otherwise, returns
//! \b false.
//
//*****************************************************************************
bool
I2CMasterBusBusy(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Return the bus busy status.
    //
    if(HWREG(ui32Base + I2C_O_MCS) & I2C_MCS_BUSBSY)
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

//*****************************************************************************
//
//! Controls the state of the I2C Master module.
//!
//! \param ui32Base is the base address of the I2C Master module.
//! \param ui32Cmd command to be issued to the I2C Master module.
//!
//! This function is used to control the state of the Master module send and
//! receive operations.  The \e ui8Cmd parameter can be one of the following
//! values:
//!
//! - \b I2C_MASTER_CMD_SINGLE_SEND
//! - \b I2C_MASTER_CMD_SINGLE_RECEIVE
//! - \b I2C_MASTER_CMD_BURST_SEND_START
//! - \b I2C_MASTER_CMD_BURST_SEND_CONT
//! - \b I2C_MASTER_CMD_BURST_SEND_FINISH
//! - \b I2C_MASTER_CMD_BURST_SEND_ERROR_STOP
//! - \b I2C_MASTER_CMD_BURST_RECEIVE_START
//! - \b I2C_MASTER_CMD_BURST_RECEIVE_CONT
//! - \b I2C_MASTER_CMD_BURST_RECEIVE_FINISH
//! - \b I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP
//! - \b I2C_MASTER_CMD_QUICK_COMMAND
//! - \b I2C_MASTER_CMD_HS_MASTER_CODE_SEND
//! - \b I2C_MASTER_CMD_FIFO_SINGLE_SEND
//! - \b I2C_MASTER_CMD_FIFO_SINGLE_RECEIVE
//! - \b I2C_MASTER_CMD_FIFO_BURST_SEND_START
//! - \b I2C_MASTER_CMD_FIFO_BURST_SEND_CONT
//! - \b I2C_MASTER_CMD_FIFO_BURST_SEND_FINISH
//! - \b I2C_MASTER_CMD_FIFO_BURST_SEND_ERROR_STOP
//! - \b I2C_MASTER_CMD_FIFO_BURST_RECEIVE_START
//! - \b I2C_MASTER_CMD_FIFO_BURST_RECEIVE_CONT
//! - \b I2C_MASTER_CMD_FIFO_BURST_RECEIVE_FINISH
//! - \b I2C_MASTER_CMD_FIFO_BURST_RECEIVE_ERROR_STOP
//!
//!
//! \return None.
//
//*****************************************************************************
void
I2CMasterControl(uint32_t ui32Base, uint32_t ui32Cmd)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));
    ASSERT((ui32Cmd == I2C_MASTER_CMD_SINGLE_SEND) ||
           (ui32Cmd == I2C_MASTER_CMD_SINGLE_RECEIVE) ||
           (ui32Cmd == I2C_MASTER_CMD_BURST_SEND_START) ||
           (ui32Cmd == I2C_MASTER_CMD_BURST_SEND_CONT) ||
           (ui32Cmd == I2C_MASTER_CMD_BURST_SEND_FINISH) ||
           (ui32Cmd == I2C_MASTER_CMD_BURST_SEND_ERROR_STOP) ||
           (ui32Cmd == I2C_MASTER_CMD_BURST_RECEIVE_START) ||
           (ui32Cmd == I2C_MASTER_CMD_BURST_RECEIVE_CONT) ||
           (ui32Cmd == I2C_MASTER_CMD_BURST_RECEIVE_FINISH) ||
           (ui32Cmd == I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP) ||
           (ui32Cmd == I2C_MASTER_CMD_QUICK_COMMAND) ||
           (ui32Cmd == I2C_MASTER_CMD_FIFO_SINGLE_SEND) ||
           (ui32Cmd == I2C_MASTER_CMD_FIFO_SINGLE_RECEIVE) ||
           (ui32Cmd == I2C_MASTER_CMD_FIFO_BURST_SEND_START) ||
           (ui32Cmd == I2C_MASTER_CMD_FIFO_BURST_SEND_CONT) ||
           (ui32Cmd == I2C_MASTER_CMD_FIFO_BURST_SEND_FINISH) ||
           (ui32Cmd == I2C_MASTER_CMD_FIFO_BURST_SEND_ERROR_STOP) ||
           (ui32Cmd == I2C_MASTER_CMD_FIFO_BURST_RECEIVE_START) ||
           (ui32Cmd == I2C_MASTER_CMD_FIFO_BURST_RECEIVE_CONT) ||
           (ui32Cmd == I2C_MASTER_CMD_FIFO_BURST_RECEIVE_FINISH) ||
           (ui32Cmd == I2C_MASTER_CMD_FIFO_BURST_RECEIVE_ERROR_STOP) ||
           (ui32Cmd == I2C_MASTER_CMD_HS_MASTER_CODE_SEND));

    //
    // Send the command.
    //
    HWREG(ui32Base + I2C_O_MCS) = ui32Cmd;
}

//*****************************************************************************
//
//! Gets the error status of the I2C Master module.
//!
//! \param ui32Base is the base address of the I2C Master module.
//!
//! This function is used to obtain the error status of the Master module send
//! and receive operations.
//!
//! \return Returns the error status, as one of \b I2C_MASTER_ERR_NONE,
//! \b I2C_MASTER_ERR_ADDR_ACK, \b I2C_MASTER_ERR_DATA_ACK, or
//! \b I2C_MASTER_ERR_ARB_LOST.
//
//*****************************************************************************
uint32_t
I2CMasterErr(uint32_t ui32Base)
{
    uint32_t ui32Err;

    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Get the raw error state
    //
    ui32Err = HWREG(ui32Base + I2C_O_MCS);

    //
    // If the I2C master is busy, then all the other bit are invalid, and
    // don't have an error to report.
    //
    if(ui32Err & I2C_MCS_BUSY)
    {
        return(I2C_MASTER_ERR_NONE);
    }

    //
    // Check for errors.
    //
    if(ui32Err & (I2C_MCS_ERROR | I2C_MCS_ARBLST))
    {
        return(ui32Err & (I2C_MCS_ARBLST | I2C_MCS_ACK | I2C_MCS_ADRACK));
    }
    else
    {
        return(I2C_MASTER_ERR_NONE);
    }
}

//*****************************************************************************
//
//! Transmits a byte from the I2C Master.
//!
//! \param ui32Base is the base address of the I2C Master module.
//! \param ui8Data data to be transmitted from the I2C Master.
//!
//! This function places the supplied data into I2C Master Data Register.
//!
//! \return None.
//
//*****************************************************************************
void
I2CMasterDataPut(uint32_t ui32Base, uint8_t ui8Data)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Write the byte.
    //
    HWREG(ui32Base + I2C_O_MDR) = ui8Data;
}

//*****************************************************************************
//
//! Receives a byte that has been sent to the I2C Master.
//!
//! \param ui32Base is the base address of the I2C Master module.
//!
//! This function reads a byte of data from the I2C Master Data Register.
//!
//! \return Returns the byte received from by the I2C Master, cast as an
//! uint32_t.
//
//*****************************************************************************
uint32_t
I2CMasterDataGet(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Read a byte.
    //
    return(HWREG(ui32Base + I2C_O_MDR));
}

//*****************************************************************************
//
//! Sets the Master clock timeout value.
//!
//! \param ui32Base is the base address of the I2C Master module.
//! \param ui32Value is the number of I2C clocks before the timeout is
//!        asserted.
//!
//! This function enables and configures the clock low timeout feature in the
//! I2C peripheral.  This feature is implemented as a 12-bit counter, with the
//! upper 8-bits being programmable.  For example, to program a timeout of 20ms
//! with a 100kHz SCL frequency, \e ui32Value would be 0x7d.
//!
//!
//! \return None.
//
//*****************************************************************************
void
I2CMasterTimeoutSet(uint32_t ui32Base, uint32_t ui32Value)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Write the timeout value.
    //
    HWREG(ui32Base + I2C_O_MCLKOCNT) = ui32Value;
}

//*****************************************************************************
//
//! Configures ACK override behavior of the I2C Slave.
//!
//! \param ui32Base is the base address of the I2C Slave module.
//! \param bEnable enables or disables ACK override.
//!
//! This function enables or disables ACK override, allowing the user
//! application to drive the value on SDA during the ACK cycle.
//!
//!
//! \return None.
//
//*****************************************************************************
void
I2CSlaveACKOverride(uint32_t ui32Base, bool bEnable)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Enable or disable based on bEnable.
    //
    if(bEnable)
    {
        HWREG(ui32Base + I2C_O_SACKCTL) |= I2C_SACKCTL_ACKOEN;
    }
    else
    {
        HWREG(ui32Base + I2C_O_SACKCTL) &= ~I2C_SACKCTL_ACKOEN;
    }
}

//*****************************************************************************
//
//! Writes the ACK value.
//!
//! \param ui32Base is the base address of the I2C Slave module.
//! \param bACK chooses whether to ACK (true) or NACK (false) the transfer.
//!
//! This function puts the desired ACK value on SDA during the ACK cycle.  The
//! value written is only valid when ACK override is enabled using
//! I2CSlaveACKOverride().
//!
//! \return None.
//
//*****************************************************************************
void
I2CSlaveACKValueSet(uint32_t ui32Base, bool bACK)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // ACK or NACK based on the value of bACK.
    //
    if(bACK)
    {
        HWREG(ui32Base + I2C_O_SACKCTL) &= ~I2C_SACKCTL_ACKOVAL;
    }
    else
    {
        HWREG(ui32Base + I2C_O_SACKCTL) |= I2C_SACKCTL_ACKOVAL;
    }
}

//*****************************************************************************
//
//! Gets the I2C Slave module status
//!
//! \param ui32Base is the base address of the I2C Slave module.
//!
//! This function returns the action requested from a master, if any.
//! Possible values are:
//!
//! - \b I2C_SLAVE_ACT_NONE
//! - \b I2C_SLAVE_ACT_RREQ
//! - \b I2C_SLAVE_ACT_TREQ
//! - \b I2C_SLAVE_ACT_RREQ_FBR
//! - \b I2C_SLAVE_ACT_OWN2SEL
//! - \b I2C_SLAVE_ACT_QCMD
//! - \b I2C_SLAVE_ACT_QCMD_DATA
//!
//! \note Not all devices support the second I2C slave's own address
//! or the quick command function.  Please consult the device data sheet to
//! determine if these features are supported.
//!
//! \return Returns \b I2C_SLAVE_ACT_NONE to indicate that no action has been
//! requested of the I2C Slave module, \b I2C_SLAVE_ACT_RREQ to indicate that
//! an I2C master has sent data to the I2C Slave module, \b I2C_SLAVE_ACT_TREQ
//! to indicate that an I2C master has requested that the I2C Slave module send
//! data, \b I2C_SLAVE_ACT_RREQ_FBR to indicate that an I2C master has sent
//! data to the I2C slave and the first byte following the slave's own address
//! has been received, \b I2C_SLAVE_ACT_OWN2SEL to indicate that the second I2C
//! slave address was matched, \b I2C_SLAVE_ACT_QCMD to indicate that a quick
//! command was received, and \b I2C_SLAVE_ACT_QCMD_DATA to indicate that the
//! data bit was set when the quick command was received.
//
//*****************************************************************************
uint32_t
I2CSlaveStatus(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Return the slave status.
    //
    return(HWREG(ui32Base + I2C_O_SCSR));
}

//*****************************************************************************
//
//! Transmits a byte from the I2C Slave.
//!
//! \param ui32Base is the base address of the I2C Slave module.
//! \param ui8Data is the data to be transmitted from the I2C Slave
//!
//! This function places the supplied data into I2C Slave Data Register.
//!
//! \return None.
//
//*****************************************************************************
void
I2CSlaveDataPut(uint32_t ui32Base, uint8_t ui8Data)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Write the byte.
    //
    HWREG(ui32Base + I2C_O_SDR) = ui8Data;
}

//*****************************************************************************
//
//! Receives a byte that has been sent to the I2C Slave.
//!
//! \param ui32Base is the base address of the I2C Slave module.
//!
//! This function reads a byte of data from the I2C Slave Data Register.
//!
//! \return Returns the byte received from by the I2C Slave, cast as an
//! uint32_t.
//
//*****************************************************************************
uint32_t
I2CSlaveDataGet(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Read a byte.
    //
    return(HWREG(ui32Base + I2C_O_SDR));
}

//*****************************************************************************
//
//! Configures the I2C transmit (TX) FIFO.
//!
//! \param ui32Base is the base address of the I2C Master or Slave module.
//! \param ui32Config is the configuration of the FIFO using specified macros.
//!
//! This configures the I2C peripheral's transmit FIFO.  The transmit FIFO can
//! be used by the master or slave, but not both.  The following macros are
//! used to configure the TX FIFO behavior for master or slave, with or without
//! DMA:
//!
//! \b I2C_FIFO_CFG_TX_MASTER, \b I2C_FIFO_CFG_TX_SLAVE,
//! \b I2C_FIFO_CFG_TX_MASTER_DMA, \b I2C_FIFO_CFG_TX_SLAVE_DMA
//!
//! To select the trigger level, one of the following macros should be used:
//!
//! \b I2C_FIFO_CFG_TX_TRIG_1, \b I2C_FIFO_CFG_TX_TRIG_2,
//! \b I2C_FIFO_CFG_TX_TRIG_3, \b I2C_FIFO_CFG_TX_TRIG_4,
//! \b I2C_FIFO_CFG_TX_TRIG_5, \b I2C_FIFO_CFG_TX_TRIG_6,
//! \b I2C_FIFO_CFG_TX_TRIG_7, \b I2C_FIFO_CFG_TX_TRIG_8
//!
//!
//! \return None.
//
//*****************************************************************************
void
I2CTxFIFOConfigSet(uint32_t ui32Base, uint32_t ui32Config)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Clear transmit configuration data.
    //
    HWREG(ui32Base + I2C_O_FIFOCTL) &= 0xffff0000;

    //
    // Store new transmit configuration data.
    //
    HWREG(ui32Base + I2C_O_FIFOCTL) |= ui32Config;
}

//*****************************************************************************
//
//! Flushes the transmit (TX) FIFO.
//!
//! \param ui32Base is the base address of the I2C Master or Slave module.
//!
//! This function flushes the I2C transmit FIFO.
//!
//!
//! \return None.
//
//*****************************************************************************
void
I2CTxFIFOFlush(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Flush the TX FIFO.
    //
    HWREG(ui32Base + I2C_O_FIFOCTL) |= I2C_FIFOCTL_TXFLUSH;
}

//*****************************************************************************
//
//! Configures the I2C receive (RX) FIFO.
//!
//! \param ui32Base is the base address of the I2C Master or Slave module.
//! \param ui32Config is the configuration of the FIFO using specified macros.
//!
//! This configures the I2C peripheral's receive FIFO.  The receive FIFO can be
//! used by the master or slave, but not both.  The following macros are used
//! to configure the RX FIFO behavior for master or slave, with or without DMA:
//!
//! \b I2C_FIFO_CFG_RX_MASTER, \b I2C_FIFO_CFG_RX_SLAVE,
//! \b I2C_FIFO_CFG_RX_MASTER_DMA, \b I2C_FIFO_CFG_RX_SLAVE_DMA
//!
//! To select the trigger level, one of the following macros should be used:
//!
//! \b I2C_FIFO_CFG_RX_TRIG_1, \b I2C_FIFO_CFG_RX_TRIG_2,
//! \b I2C_FIFO_CFG_RX_TRIG_3, \b I2C_FIFO_CFG_RX_TRIG_4,
//! \b I2C_FIFO_CFG_RX_TRIG_5, \b I2C_FIFO_CFG_RX_TRIG_6,
//! \b I2C_FIFO_CFG_RX_TRIG_7, \b I2C_FIFO_CFG_RX_TRIG_8
//!
//!
//! \return None.
//
//*****************************************************************************
void
I2CRxFIFOConfigSet(uint32_t ui32Base, uint32_t ui32Config)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Clear receive configuration data.
    //
    HWREG(ui32Base + I2C_O_FIFOCTL) &= 0x0000ffff;

    //
    // Store new receive configuration data.
    //
    HWREG(ui32Base + I2C_O_FIFOCTL) |= ui32Config;
}

//*****************************************************************************
//
//! Flushes the receive (RX) FIFO.
//!
//! \param ui32Base is the base address of the I2C Master or Slave module.
//!
//! This function flushes the I2C receive FIFO.
//!
//! \return None.
//
//*****************************************************************************
void
I2CRxFIFOFlush(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Flush the TX FIFO.
    //
    HWREG(ui32Base + I2C_O_FIFOCTL) |= I2C_FIFOCTL_RXFLUSH;
}

//*****************************************************************************
//
//! Gets the current FIFO status.
//!
//! \param ui32Base is the base address of the I2C Master or Slave module.
//!
//! This function retrieves the status for both the transmit (TX) and receive
//! (RX) FIFOs.  The trigger level for the transmit FIFO is set using
//! I2CTxFIFOConfigSet() and for the receive FIFO using I2CTxFIFOConfigSet().
//!
//! \return Returns the FIFO status, enumerated as a bit field containing
//! \b I2C_FIFO_RX_BELOW_TRIG_LEVEL, \b I2C_FIFO_RX_FULL, \b I2C_FIFO_RX_EMPTY,
//! \b I2C_FIFO_TX_BELOW_TRIG_LEVEL, \b I2C_FIFO_TX_FULL, and
//! \b I2C_FIFO_TX_EMPTY.
//
//*****************************************************************************
uint32_t
I2CFIFOStatus(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Return the contents of the FIFO status register.
    //
    return(HWREG(ui32Base + I2C_O_FIFOSTATUS));
}

//*****************************************************************************
//
//! Writes a data byte to the I2C transmit FIFO.
//!
//! \param ui32Base is the base address of the I2C Master or Slave module.
//! \param ui8Data is the data to be placed into the transmit FIFO.
//!
//! This function adds a byte of data to the I2C transmit FIFO.  If there is
//! no space available in the FIFO,  this function waits for space to become
//! available before returning.
//!
//! \return None.
//
//*****************************************************************************
void
I2CFIFODataPut(uint32_t ui32Base, uint8_t ui8Data)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Wait until there is space.
    //
    while(HWREG(ui32Base + I2C_O_FIFOSTATUS) & I2C_FIFOSTATUS_TXFF)
    {
    }

    //
    // Place data into the FIFO.
    //
    HWREG(ui32Base + I2C_O_FIFODATA) = ui8Data;
}

//*****************************************************************************
//
//! Writes a data byte to the I2C transmit FIFO.
//!
//! \param ui32Base is the base address of the I2C Master or Slave module.
//! \param ui8Data is the data to be placed into the transmit FIFO.
//!
//! This function adds a byte of data to the I2C transmit FIFO.  If there is
//! no space available in the FIFO, this function returns a zero.
//!
//! \return The number of elements added to the I2C transmit FIFO.
//
//*****************************************************************************
uint32_t
I2CFIFODataPutNonBlocking(uint32_t ui32Base, uint8_t ui8Data)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // If FIFO is full, return zero.
    //
    if(HWREG(ui32Base + I2C_O_FIFOSTATUS) & I2C_FIFOSTATUS_TXFF)
    {
        return(0);
    }
    else
    {
        HWREG(ui32Base + I2C_O_FIFODATA) = ui8Data;
        return(1);
    }
}

//*****************************************************************************
//
//! Reads a byte from the I2C receive FIFO.
//!
//! \param ui32Base is the base address of the I2C Master or Slave module.
//!
//! This function reads a byte of data from I2C receive FIFO and places it in
//! the location specified by the \e pui8Data parameter.  If there is no data
//! available, this function waits until data is received before returning.
//!
//! \return The data byte.
//
//*****************************************************************************
uint32_t
I2CFIFODataGet(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Wait until there is data to read.
    //
    while(HWREG(ui32Base + I2C_O_FIFOSTATUS) & I2C_FIFOSTATUS_RXFE)
    {
    }

    //
    // Read a byte.
    //
    return(HWREG(ui32Base + I2C_O_FIFODATA));
}

//*****************************************************************************
//
//! Reads a byte from the I2C receive FIFO.
//!
//! \param ui32Base is the base address of the I2C Master or Slave module.
//! \param pui8Data is a pointer where the read data is stored.
//!
//! This function reads a byte of data from I2C receive FIFO and places it in
//! the location specified by the \e pui8Data parameter.  If there is no data
//! available, this functions returns 0.
//!
//! \return The number of elements read from the I2C receive FIFO.
//
//*****************************************************************************
uint32_t
I2CFIFODataGetNonBlocking(uint32_t ui32Base, uint8_t *pui8Data)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // If nothing in the FIFO, return zero.
    //
    if(HWREG(ui32Base + I2C_O_FIFOSTATUS) & I2C_FIFOSTATUS_RXFE)
    {
        return(0);
    }
    else
    {
        *pui8Data = HWREG(ui32Base + I2C_O_FIFODATA);
        return(1);
    }
}

//*****************************************************************************
//
//! Set the burst length for a I2C master FIFO operation.
//!
//! \param ui32Base is the base address of the I2C Master module.
//! \param ui8Length is the length of the burst transfer.
//!
//! This function configures the burst length for a I2C Master FIFO operation.
//! The burst field is limited to 8 bits or 256 bytes.  The burst length
//! applies to a single I2CMCS BURST operation meaning that it specifies the
//! burst length for only the current operation (can be TX or RX).  Each burst
//! operation must configure the burst length prior to writing the BURST bit
//! in the I2CMCS using I2CMasterControl().
//!
//! \return None.
//
//*****************************************************************************
void
I2CMasterBurstLengthSet(uint32_t ui32Base, uint8_t ui8Length)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base) && (ui8Length < 255));

    //
    // Set the burst length.
    //
    HWREG(ui32Base + I2C_O_MBLEN) = ui8Length;
}

//*****************************************************************************
//
//! Returns the current value of the burst transfer counter.
//!
//! \param ui32Base is the base address of the I2C Master module.
//!
//! This function returns the current value of the burst transfer counter that
//! is used by the FIFO mechanism.  Software can use this value to determine
//! how many bytes remain in a transfer, or where in the transfer the burst
//! operation was if an error has occurred.
//!
//! \return None.
//
//*****************************************************************************
uint32_t
I2CMasterBurstCountGet(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Get burst count.
    //
    return(HWREG(ui32Base + I2C_O_MBCNT));
}

//*****************************************************************************
//
//! Configures the I2C Master glitch filter.
//!
//! \param ui32Base is the base address of the I2C Master module.
//! \param ui32Config is the glitch filter configuration.
//!
//! This function configures the I2C Master glitch filter.  The value passed in
//! to \e ui32Config determines the sampling range of the glitch filter, which
//! is configurable between 1 and 32 system clock cycles.  The default
//! configuration of the glitch filter is 0 system clock cycles, which means
//! that it's disabled.
//!
//! The \e ui32Config field should be any of the following values:
//!
//! - \b I2C_MASTER_GLITCH_FILTER_DISABLED
//! - \b I2C_MASTER_GLITCH_FILTER_1
//! - \b I2C_MASTER_GLITCH_FILTER_2
//! - \b I2C_MASTER_GLITCH_FILTER_3
//! - \b I2C_MASTER_GLITCH_FILTER_4
//! - \b I2C_MASTER_GLITCH_FILTER_8
//! - \b I2C_MASTER_GLITCH_FILTER_16
//! - \b I2C_MASTER_GLITCH_FILTER_32
//!
//! \return None.
//
//*****************************************************************************
void
I2CMasterGlitchFilterConfigSet(uint32_t ui32Base, uint32_t ui32Config)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Configure the glitch filter field of MTPR.
    //
    HWREG(ui32Base + I2C_O_MTPR) |= ui32Config;
}

//*****************************************************************************
//
//! Enables FIFO usage for the I2C Slave module.
//!
//! \param ui32Base is the base address of the I2C Slave module.
//! \param ui32Config is the desired FIFO configuration of the I2C Slave.
//!
//! This function configures the I2C Slave module to use the FIFO(s).  This
//! function should be used in combination with I2CTxFIFOConfigSet() and/or
//! I2CRxFIFOConfigSet(), which configure the FIFO trigger level and tell
//! the FIFO hardware whether to interact with the I2C Master or Slave.  The
//! application appropriate combination of \b I2C_SLAVE_TX_FIFO_ENABLE and
//! \b I2C_SLAVE_RX_FIFO_ENABLE should be passed in to the \e ui32Config
//! field.
//!
//! The Slave I2CSCSR register is write-only, so any call to I2CSlaveEnable(),
//! I2CSlaveDisable or I2CSlaveFIFOEnable() overwrites the slave configuration.
//! Therefore, application software should call I2CSlaveEnable() followed by
//! I2CSlaveFIFOEnable() with the desired FIFO configuration.
//!
//! \return None.
//
//*****************************************************************************
void
I2CSlaveFIFOEnable(uint32_t ui32Base, uint32_t ui32Config)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Enable the FIFOs for the slave.
    //
    HWREG(ui32Base + I2C_O_SCSR) = ui32Config | I2C_SCSR_DA;
}

//*****************************************************************************
//
//! Disable FIFO usage for the I2C Slave module.
//!
//! \param ui32Base is the base address of the I2C Slave module.
//!
//! This function disables the FIFOs for the I2C Slave.  After calling this
//! this function, the FIFOs are disabled, but the Slave remains active.
//!
//! \return None.
//
//*****************************************************************************
void
I2CSlaveFIFODisable(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Disable slave FIFOs.
    //
    HWREG(ui32Base + I2C_O_SCSR) = I2C_SCSR_DA;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
