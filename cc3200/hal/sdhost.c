//*****************************************************************************
//
//  sdhost.c
//
//  Driver for the SD Host (SDHost) Interface
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
//! \addtogroup Secure_Digital_Host_api
//! @{
//
//*****************************************************************************

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_mmchs.h"
#include "inc/hw_ints.h"
#include "inc/hw_apps_config.h"
#include "interrupt.h"
#include "sdhost.h"


//*****************************************************************************
//
//! Configures SDHost module.
//!
//! \param ulBase is the base address of SDHost module.
//!
//! This function configures the SDHost module, enabling internal sub-modules.
//!
//! \return None.
//
//*****************************************************************************
void
SDHostInit(unsigned long ulBase)
{
  //
  // Assert module reset
  //
  HWREG(ulBase + MMCHS_O_SYSCONFIG) = 0x2;

  //
  // Wait for soft reset to complete
  //
  while( !(HWREG(ulBase + MMCHS_O_SYSCONFIG) & 0x1) )
  {

  }

  //
  // Assert internal reset
  //
  HWREG(ulBase + MMCHS_O_SYSCTL) |= (1 << 24);

  //
  // Wait for Reset to complete
  //
  while( (HWREG(ulBase + MMCHS_O_SYSCTL) & (0x1 << 24)) )
  {

  }

  //
  // Set capability register, 1.8 and 3.0 V
  //
  HWREG(ulBase + MMCHS_O_CAPA) = (0x7 <<24);

  //
  // Select bus voltage, 3.0 V
  //
  HWREG(ulBase + MMCHS_O_HCTL) |= 0x7 << 9;

  //
  // Power up the bus
  //
  HWREG(ulBase + MMCHS_O_HCTL) |= 1 << 8;

  //
  // Wait for power on
  //
  while( !(HWREG(ulBase + MMCHS_O_HCTL) & (1<<8)) )
  {

  }

  HWREG(ulBase + MMCHS_O_CON) |= 1 << 21;

  //
  // Un-mask all events
  //
  HWREG(ulBase + MMCHS_O_IE) = 0xFFFFFFFF;
}


//*****************************************************************************
//
//! Resets SDHost command line
//!
//! \param ulBase is the base address of SDHost module.
//!
//! This function assers a soft reset for the command line
//!
//! \return None.
//
//*****************************************************************************
void
SDHostCmdReset(unsigned long ulBase)
{
  HWREG(ulBase + MMCHS_O_SYSCTL) |= 1 << 25;
  while( (HWREG(ulBase + MMCHS_O_SYSCTL) & (1 << 25)) )
  {

  }
}

//*****************************************************************************
//
//! Sends command over SDHost interface
//!
//! \param ulBase is the base address of SDHost module.
//! \param ulCmd is the command to send.
//! \param ulArg is the argument for the command.
//!
//! This function send command to the attached card over the SDHost interface.
//!
//! The \e ulCmd parameter can be one of \b SDHOST_CMD_0 to \b SDHOST_CMD_63.
//! It can be logically ORed with one or more of the following:
//! - \b SDHOST_MULTI_BLK for multi-block transfer
//! - \b SDHOST_WR_CMD          if command is followed by write data
//! - \b SDHOST_RD_CMD          if command is followed by read data
//! - \b SDHOST_DMA_EN          if SDHost need to generate DMA request.
//! - \b SDHOST_RESP_LEN_136    if 136 bit response is expected
//! - \b SDHOST_RESP_LEN_48     if 48 bit response is expected
//! - \b SDHOST_RESP_LEN_48B    if 48 bit response with busy bit is expected
//!
//! The parameter \e ulArg is the argument for the command
//!
//! \return Returns 0 on success, -1 otherwise.
//
//*****************************************************************************
long
SDHostCmdSend(unsigned long ulBase, unsigned long ulCmd, unsigned ulArg)
{
  //
  // Set Data Timeout
  //
  HWREG(ulBase + MMCHS_O_SYSCTL) |= 0x000E0000;

  //
  // Check for cmd inhabit
  //
  if( (HWREG(ulBase + MMCHS_O_PSTATE) & 0x1))
  {
    return -1;
  }

  //
  // Set the argument
  //
  HWREG(ulBase + MMCHS_O_ARG) = ulArg;

  //
  // Send the command
  //
  HWREG(ulBase + MMCHS_O_CMD) = ulCmd;

  return 0;
}

//*****************************************************************************
//
//! Writes a data word into the SDHost write buffer.
//!
//! \param ulBase is the base address of SDHost module.
//! \param ulData is data word to be transfered.
//!
//! This function writes a single data word into the SDHost write buffer. The
//! function returns \b true if there was a space available in the buffer else
//! returns \b false.
//!
//! \return Return \b true on success, \b false otherwise.
//
//*****************************************************************************
tBoolean
SDHostDataNonBlockingWrite(unsigned long ulBase, unsigned long ulData)
{

  //
  // See if there is a space in the write buffer
  //
  if( (HWREG(ulBase + MMCHS_O_PSTATE) & (1<<10)) )
  {
    //
    // Write the data into the buffer
    //
    HWREG(ulBase + MMCHS_O_DATA) = ulData;

    //
    // Success.
    //
    return(true);
  }
  else
  {
    //
    // No free sapce, failure.
    //
    return(false);
  }
}

//*****************************************************************************
//
//! Waits to write a data word into the SDHost write buffer.
//!
//! \param ulBase is the base address of SDHost module.
//! \param ulData is data word to be transfered.
//!
//! This function writes \e ulData into the SDHost write buffer. If there is no
//! space in the write buffer this function waits until there is a space
//! available before returning.
//!
//! \return None.
//
//*****************************************************************************
void
SDHostDataWrite(unsigned long ulBase, unsigned long ulData)
{
  //
  // Wait until space is available
  //
  while( !(HWREG(ulBase + MMCHS_O_PSTATE) & (1<<10)) )
  {

  }

  //
  // Write the data
  //
  HWREG(ulBase + MMCHS_O_DATA) = ulData;
}


//*****************************************************************************
//
//! Waits for a data word from the SDHost read buffer
//!
//! \param ulBase is the base address of SDHost module.
//! \param pulData is pointer to read data variable.
//!
//! This function reads a single data word from the SDHost read buffer. If there
//! is no data available in the buffer the function will wait until a data
//! word is received before returning.
//!
//! \return None.
//
//*****************************************************************************
void
SDHostDataRead(unsigned long ulBase, unsigned long *pulData)
{
  //
  // Wait until data is available
  //
  while( !(HWREG(ulBase + MMCHS_O_PSTATE) & (1<<11)) )
  {

  }

  //
  // Read the data
  //
  *pulData = HWREG(ulBase + MMCHS_O_DATA);
}

//*****************************************************************************
//
//! Reads single data word from the SDHost read buffer
//!
//! \param ulBase is the base address of SDHost module.
//! \param pulData is pointer to read data variable.
//!
//! This function reads a data word from the SDHost read buffer. The
//! function returns \b true if there was data available in to buffer else
//! returns \b false.
//!
//! \return Return \b true on success, \b false otherwise.
//
//*****************************************************************************
tBoolean
SDHostDataNonBlockingRead(unsigned long ulBase, unsigned long *pulData)
{

  //
  // See if there is any data in the read buffer.
  //
  if( (HWREG(ulBase + MMCHS_O_PSTATE) & (1<11)) )
  {
    //
    // Read the data word.
    //
    *pulData = HWREG(ulBase + MMCHS_O_DATA);

    //
    // Success
    //
    return(true);
  }
  else
  {
    //
    // No data available, failure.
    //
    return(false);
  }
}


//*****************************************************************************
//
//! Registers the interrupt handler for SDHost interrupt
//!
//! \param ulBase is the base address of SDHost module
//! \param pfnHandler is a pointer to the function to be called when the
//! SDHost interrupt occurs.
//!
//! This function does the actual registering of the interrupt handler.  This
//! function enables the global interrupt in the interrupt controller; specific
//! SDHost interrupts must be enabled via SDHostIntEnable(). It is the
//! interrupt handler's responsibility to clear the interrupt source.
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \return None.
//
//*****************************************************************************
void
SDHostIntRegister(unsigned long ulBase, void (*pfnHandler)(void))
{
  //
  // Register the interrupt handler.
  //
  IntRegister(INT_MMCHS, pfnHandler);

  //
  // Enable the SDHost interrupt.
  //
  IntEnable(INT_MMCHS);
}

//*****************************************************************************
//
//! Unregisters the interrupt handler for SDHost interrupt
//!
//! \param ulBase is the base address of SDHost module
//!
//! This function does the actual unregistering of the interrupt handler.  It
//! clears the handler to be called when a SDHost interrupt occurs.  This
//! function also masks off the interrupt in the interrupt controller so that
//! the interrupt handler no longer is called.
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \return None.
//
//*****************************************************************************
void
SDHostIntUnregister(unsigned long ulBase)
{
  //
  // Disable the SDHost interrupt.
  //
  IntDisable(INT_MMCHS);

  //
  // Unregister the interrupt handler.
  //
  IntUnregister(INT_MMCHS);
}

//*****************************************************************************
//
//! Enable individual interrupt source for the specified SDHost
//!
//! \param ulBase is the base address of SDHost module.
//! \param ulIntFlags is a bit mask of the interrupt sources to be enabled.
//!
//! This function enables the indicated SDHost interrupt sources.  Only the
//! sources that are enabled can be reflected to the processor interrupt;
//! disabled sources have no effect on the processor.
//!
//! The \e ulIntFlags parameter is the logical OR of any of the following:
//! - \b SDHOST_INT_CC    Command Complete interrupt
//! - \b SDHOST_INT_TC    Transfer Complete interrupt
//! - \b SDHOST_INT_BWR   Buffer Write Ready interrupt
//! - \b SDHOST_INT_BRR   Buffer Read Ready interrupt
//! - \b SDHOST_INT_ERRI  Error interrupt
//! - \b SDHOST_INT_CTO   Command Timeout error interrupt
//! - \b SDHOST_INT_CEB   Command End Bit error interrupt
//! - \b SDHOST_INT_DTO   Data Timeout error interrupt
//! - \b SDHOST_INT_DCRC  Data CRC error interrupt
//! - \b SDHOST_INT_DEB   Data End Bit error
//! - \b SDHOST_INT_CERR  Cart Status Error interrupt
//! - \b SDHOST_INT_BADA  Bad Data error interrupt
//! - \b SDHOST_INT_DMARD Read DMA done interrupt
//! - \b SDHOST_INT_DMAWR Write DMA done interrupt
//!
//!     Note that SDHOST_INT_ERRI can only be used with \sa SDHostIntStatus()
//! and is internally logical OR of all error status bits. Setting this bit
//! alone as \e ulIntFlags doesn't generates any interrupt.
//!
//! \return None.
//
//*****************************************************************************
void
SDHostIntEnable(unsigned long ulBase,unsigned long ulIntFlags)
{
  //
  // Enable DMA done interrupts
  //
  HWREG(APPS_CONFIG_BASE + APPS_CONFIG_O_DMA_DONE_INT_MASK_CLR) =
                                                          (ulIntFlags >> 30);

  //
  // Enable the individual interrupt sources
  //
  HWREG(ulBase + MMCHS_O_ISE) |= (ulIntFlags & 0x3FFFFFFF);
}

//*****************************************************************************
//
//! Enable individual interrupt source for the specified SDHost
//!
//! \param ulBase is the base address of SDHost module.
//! \param ulIntFlags is a bit mask of the interrupt sources to be enabled.
//!
//! This function disables the indicated SDHost interrupt sources.  Only the
//! sources that are enabled can be reflected to the processor interrupt;
//! disabled sources have no effect on the processor.
//!
//! The \e ulIntFlags parameter has the same definition as the \e ulIntFlags
//! parameter to SDHostIntEnable().
//!
//! \return None.
//
//*****************************************************************************
void
SDHostIntDisable(unsigned long ulBase,unsigned long ulIntFlags)
{
  //
  // Disable DMA done interrupts
  //
  HWREG(APPS_CONFIG_BASE + APPS_CONFIG_O_DMA_DONE_INT_MASK_SET) =
                                                          (ulIntFlags >> 30);
  //
  // Disable the individual interrupt sources
  //
  HWREG(ulBase + MMCHS_O_ISE) &= ~(ulIntFlags & 0x3FFFFFFF);
}

//*****************************************************************************
//
//! Gets the current interrupt status.
//!
//! \param ulBase is the base address of SDHost module.
//!
//! This function returns the interrupt status for the specified SDHost.
//!
//! \return Returns the current interrupt status, enumerated as a bit field of
//! values described in SDHostIntEnable().
//
//*****************************************************************************
unsigned long
SDHostIntStatus(unsigned long ulBase)
{
  unsigned long ulIntStatus;

  //
  // Get DMA done interrupt status
  //
  ulIntStatus = HWREG(APPS_CONFIG_BASE + APPS_CONFIG_O_DMA_DONE_INT_MASK_SET);
  ulIntStatus = (ulIntStatus << 30);

  //
  // Return the status of individual interrupt sources
  //
  ulIntStatus |= (HWREG(ulBase + MMCHS_O_STAT) & 0x3FFFFFFF);

  return(ulIntStatus);
}

//*****************************************************************************
//
//! Clears the individual interrupt sources.
//!
//! \param ulBase is the base address of SDHost module.
//! \param ulIntFlags is a bit mask of the interrupt sources to be cleared.
//!
//! The specified SDHost interrupt sources are cleared, so that they no longer
//! assert.  This function must be called in the interrupt handler to keep the
//! interrupt from being recognized again immediately upon exit.
//!
//! The \e ulIntFlags parameter has the same definition as the \e ulIntFlags
//! parameter to SDHostIntEnable().
//!
//! \return None.
//
//*****************************************************************************
void
SDHostIntClear(unsigned long ulBase,unsigned long ulIntFlags)
{
  //
  // Clear DMA done interrupts
  //
  HWREG(APPS_CONFIG_BASE + APPS_CONFIG_O_DMA_DONE_INT_ACK) =
                                                          (ulIntFlags >> 30);
  //
  // Clear the individual interrupt sources
  //
  HWREG(ulBase + MMCHS_O_STAT) = (ulIntFlags & 0x3FFFFFFF);
}

//*****************************************************************************
//
//! Sets the card status error mask.
//!
//! \param ulBase is the base address of SDHost module
//! \param ulErrMask is the bit mask of card status errors to be enabled
//!
//! This function sets the card status error mask for response type R1, R1b,
//! R5, R5b and R6 response. The parameter \ulErrMask is the bit mask of card
//! status errors to be enabled, if the corresponding bits in the 'card status'
//! field of a respose are set then the host controller indicates a card error
//! interrupt status. Only bits referenced as type E (error) in status field in
//! the response can set a card status error.
//!
//! \return None
//
//*****************************************************************************
void
SDHostCardErrorMaskSet(unsigned long ulBase, unsigned long ulErrMask)
{
  //
  // Set the card status error mask
  //
  HWREG(ulBase + MMCHS_O_CSRE) = ulErrMask;
}


//*****************************************************************************
//
//! Gets the card status error mask.
//!
//! \param ulBase is the base address of SDHost module
//!
//! This function gets the card status error mask for response type R1, R1b,
//! R5, R5b and R6 response.
//!
//! \return Returns the current card status error.
//
//*****************************************************************************
unsigned long
SDHostCardErrorMaskGet(unsigned long ulBase)
{
  //
  // Return the card status error mask
  //
  return(HWREG(ulBase + MMCHS_O_CSRE));
}

//*****************************************************************************
//
//! Sets the SD Card clock.
//!
//! \param ulBase is the base address of SDHost module
//! \param ulSDHostClk is the rate of clock supplied to SDHost module
//! \param ulCardClk is the required SD interface clock
//!
//! This function configures the SDHost interface to supply the specified clock
//! to the connected card.
//!
//! \return None.
//
//*****************************************************************************
void
SDHostSetExpClk(unsigned long ulBase, unsigned long ulSDHostClk,
             unsigned long ulCardClk)
{
  unsigned long ulDiv;

  //
  // Disable card clock
  //
  HWREG(ulBase + MMCHS_O_SYSCTL) &= ~0x4;

  //
  // Enable internal clock
  //
  HWREG(ulBase + MMCHS_O_SYSCTL) |= 0x1;

  ulDiv = ((ulSDHostClk/ulCardClk) & 0x3FF);

  //
  // Set clock divider,
  //
  HWREG(ulBase + MMCHS_O_SYSCTL) = ((HWREG(ulBase + MMCHS_O_SYSCTL) &
                                     ~0x0000FFC0)| (ulDiv) << 6);

  //
  // Wait for clock to stablize
  //
  while( !(HWREG(ulBase + MMCHS_O_SYSCTL) & 0x2) )
  {

  }

  //
  // Enable card clock
  //
  HWREG(ulBase + MMCHS_O_SYSCTL) |= 0x4;
}

//*****************************************************************************
//
//! Get the response for the last command.
//!
//! \param ulBase is the base address of SDHost module
//! \param ulRespnse is 128-bit response.
//!
//! This function gets the response from the SD card for the last command
//! send.
//!
//! \return None.
//
//*****************************************************************************
void
SDHostRespGet(unsigned long ulBase, unsigned long ulRespnse[4])
{

  //
  // Read the responses.
  //
  ulRespnse[0] = HWREG(ulBase + MMCHS_O_RSP10);
  ulRespnse[1] = HWREG(ulBase + MMCHS_O_RSP32);
  ulRespnse[2] = HWREG(ulBase + MMCHS_O_RSP54);
  ulRespnse[3] = HWREG(ulBase + MMCHS_O_RSP76);

}

//*****************************************************************************
//
//! Set the block size for data transfer
//!
//! \param ulBase is the base address of SDHost module
//! \param ulBlkSize is the transfer block size in bytes
//!
//! This function sets the block size the data transfer.
//!
//! The parameter \e ulBlkSize is size of each data block in bytes.
//! This should be in range 0 - 2^10.
//!
//! \return None.
//
//*****************************************************************************
void
SDHostBlockSizeSet(unsigned long ulBase, unsigned short ulBlkSize)
{
  //
  // Set the block size
  //
  HWREG(ulBase + MMCHS_O_BLK) = ((HWREG(ulBase + MMCHS_O_BLK) & 0x00000FFF)|
                                  (ulBlkSize & 0xFFF));
}

//*****************************************************************************
//
//! Set the block size and count for data transfer
//!
//! \param ulBase is the base address of SDHost module
//! \param ulBlkCount is the number of blocks
//!
//! This function sets block count for the data transfer. This needs to be set
//! for each block transfer. \sa SDHostBlockSizeSet()
//!
//! \return None.
//
//*****************************************************************************
void
SDHostBlockCountSet(unsigned long ulBase, unsigned short ulBlkCount)
{
  unsigned long ulRegVal;

  //
  // Read the current value
  //
  ulRegVal = HWREG(ulBase + MMCHS_O_BLK);

  //
  // Set the number of blocks
  //
  HWREG(ulBase + MMCHS_O_BLK) |= ((ulRegVal & 0x0000FFFF)|
                                  (ulBlkCount << 16));
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
