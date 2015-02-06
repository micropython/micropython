//*****************************************************************************
//
//  i2s.c
//
//  Driver for the I2S interface.
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
//! \addtogroup I2S_api
//! @{
//
//*****************************************************************************
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_mcasp.h"
#include "inc/hw_apps_config.h"
#include "interrupt.h"
#include "i2s.h"

//*****************************************************************************
// Macros
//*****************************************************************************
#define MCASP_GBL_RCLK		0x00000001
#define MCASP_GBL_RHCLK		0x00000002
#define MCASP_GBL_RSER		0x00000004
#define MCASP_GBL_RSM		0x00000008
#define MCASP_GBL_RFSYNC	0x00000010
#define MCASP_GBL_XCLK		0x00000100
#define MCASP_GBL_XHCLK		0x00000200
#define MCASP_GBL_XSER		0x00000400
#define MCASP_GBL_XSM		0x00000800
#define MCASP_GBL_XFSYNC	0x00001000


//*****************************************************************************
//
//! \internal
//! Releases the specifed submodule out of reset.
//!
//! \param ulBase is the base address of the I2S module.
//! \param ulFlag is one of the valid sub module.
//!
//! This function Releases the specifed submodule out of reset.
//!
//! \return None.
//
//*****************************************************************************
static void I2SGBLEnable(unsigned long ulBase, unsigned long ulFlag)
{
  unsigned long ulReg;

  //
  // Read global control register
  //
  ulReg = HWREG(ulBase + MCASP_O_GBLCTL);

  //
  // Remove the sub modules reset as specified by ulFlag parameter
  //
  ulReg |= ulFlag;

  //
  // Write the configuration
  //
  HWREG(ulBase + MCASP_O_GBLCTL) = ulReg;

  //
  // Wait for write completeion
  //
  while(HWREG(ulBase + MCASP_O_GBLCTL) != ulReg)
  {

  }

}

//*****************************************************************************
//
//! Enables transmit and/or receive.
//!
//! \param ulBase is the base address of the I2S module.
//! \param ulMode is one of the valid modes.
//!
//! This function enables the I2S module in specified mode. The parameter
//! \e ulMode should be one of the following
//!
//! -\b I2S_MODE_TX_ONLY
//! -\b I2S_MODE_TX_RX_SYNC
//!
//! \return None.
//
//*****************************************************************************
void I2SEnable(unsigned long ulBase, unsigned long ulMode)
{
  //
  // Set FSYNC anc BitClk as output
  //
  HWREG(ulBase + MCASP_O_PDIR) |= 0x14000000;

  if(ulMode & 0x2)
  {
    //
    // Remove Rx HCLK reset
    //
    I2SGBLEnable(ulBase, MCASP_GBL_RHCLK);

    //
    // Remove Rx XCLK reset
    //
    I2SGBLEnable(ulBase, MCASP_GBL_RCLK);

    //
    // Enable Rx SERDES(s)
    //
    I2SGBLEnable(ulBase, MCASP_GBL_RSER);

    //
    // Enable Rx state machine
    //
    I2SGBLEnable(ulBase, MCASP_GBL_RSM);

    //
    // Enable FSync generator
    //
    I2SGBLEnable(ulBase, MCASP_GBL_RFSYNC);
  }

  if(ulMode & 0x1)
  {
    //
    // Remove Tx HCLK reset
    //
    I2SGBLEnable(ulBase, MCASP_GBL_XHCLK);

    //
    // Remove Tx XCLK reset
    //
    I2SGBLEnable(ulBase, MCASP_GBL_XCLK);

    //
    // Enable Tx SERDES(s)
    //
    I2SGBLEnable(ulBase, MCASP_GBL_XSER);

    //
    // Enable Tx state machine
    //
    I2SGBLEnable(ulBase, MCASP_GBL_XSM);

    //
    // Enable FSync generator
    //
    I2SGBLEnable(ulBase, MCASP_GBL_XFSYNC);
  }
}

//*****************************************************************************
//
//! Disables transmit and/or receive.
//!
//! \param ulBase is the base address of the I2S module.
//!
//! This function disables transmit and/or receive from I2S module.
//!
//! \return None.
//
//*****************************************************************************
void I2SDisable(unsigned long ulBase)
{
  //
  // Reset all sub modules
  //
  HWREG(ulBase + MCASP_O_GBLCTL) = 0;

  //
  // Wait for write to complete
  //
  while( HWREG(ulBase + MCASP_O_GBLCTL) != 0)
  {

  }
}

//*****************************************************************************
//
//! Waits to send data over the specified data line
//!
//! \param ulBase is the base address of the I2S module.
//! \param ulDataLine is one of the valid data lines.
//! \param ulData is the data to be transmitted.
//!
//! This function sends the \e ucData to the transmit register for the
//! specified data line. If there is no space available, this
//! function waits until there is space available before returning.
//!
//! \return None.
//
//*****************************************************************************
void I2SDataPut(unsigned long ulBase, unsigned long ulDataLine,
                unsigned long ulData)
{
  //
  // Compute register the offeset
  //
  ulDataLine = (ulDataLine-1) << 2;

  //
  // Wait for free space in fifo
  //
  while(!( HWREG(ulBase + MCASP_O_TXSTAT) & MCASP_TXSTAT_XDATA))
  {

  }

  //
  // Write Data into the FIFO
  //
  HWREG(ulBase + MCASP_O_TXBUF0 + ulDataLine) = ulData;
}

//*****************************************************************************
//
//! Sends data over the specified data line
//!
//! \param ulBase is the base address of the I2S module.
//! \param ulDataLine is one of the valid data lines.
//! \param ulData is the data to be transmitted.
//!
//! This function writes the \e ucData to the transmit register for
//! the specified data line. This function does not block, so if there is no
//! space available, then \b -1 is returned, and the application must retry the
//! function later.
//!
//! \return Returns 0 on success, -1 otherwise.
//
//*****************************************************************************
long I2SDataPutNonBlocking(unsigned long ulBase,  unsigned long ulDataLine,
                           unsigned long ulData)
{

  //
  // Compute register the offeset
  //
  ulDataLine = (ulDataLine-1) << 2;

  //
  // Send Data if fifo has free space
  //
  if( HWREG(ulBase + MCASP_O_TXSTAT) & MCASP_TXSTAT_XDATA)
  {
    //
    // Write data into the FIFO
    //
    HWREG(ulBase + MCASP_O_TXBUF0 + ulDataLine) = ulData;
    return 0;
  }

  //
  // FIFO is full
  //
  return(-1);
}

//*****************************************************************************
//
//! Waits for data from the specified data line.
//!
//! \param ulBase is the base address of the I2S module.
//! \param ulDataLine is one of the valid data lines.
//! \param pulData is pointer to receive data variable.
//!
//! This function gets data from the receive register for the specified
//! data line. If there are no data available, this function waits until a
//! receive before returning.
//!
//! \return None.
//
//*****************************************************************************
void I2SDataGet(unsigned long ulBase, unsigned long ulDataLine,
                unsigned long *pulData)
{

  //
  // Compute register the offeset
  //
  ulDataLine = (ulDataLine-1) << 2;

  //
  // Wait for atleat on word in FIFO
  //
  while(!(HWREG(ulBase + MCASP_O_RXSTAT) & MCASP_RXSTAT_RDATA))
  {

  }

  //
  // Read the Data
  //
  *pulData = HWREG(ulBase + MCASP_O_RXBUF0 + ulDataLine);
}


//*****************************************************************************
//
//! Receives data from the specified data line.
//!
//! \param ulBase is the base address of the I2S module.
//! \param ulDataLine is one of the valid data lines.
//! \param pulData is pointer to receive data variable.
//!
//! This function gets data from the receive register for the specified
//! data line.
//!
//!
//! \return Returns 0 on success, -1 otherwise.
//
//*****************************************************************************
long I2SDataGetNonBlocking(unsigned long ulBase, unsigned long ulDataLine,
                unsigned long *pulData)
{

  //
  // Compute register the offeset
  //
  ulDataLine = (ulDataLine-1) << 2;

  //
  // Check if data is available in FIFO
  //
  if(HWREG(ulBase + MCASP_O_RXSTAT) & MCASP_RXSTAT_RDATA)
  {
    //
    // Read the Data
    //
    *pulData = HWREG(ulBase + MCASP_O_RXBUF0 + ulDataLine);
    return 0;
  }

  //
  // FIFO is empty
  //
  return -1;
}


//*****************************************************************************
//
//! Sets the configuration of the I2S module.
//!
//! \param ulBase is the base address of the I2S module.
//! \param ulI2SClk is the rate of the clock supplied to the I2S module.
//! \param ulBitClk is the desired bit rate.
//! \param ulConfig is the data format.
//!
//! This function configures the I2S for operation in the specified data
//! format.  The bit rate is provided in the \e ulBitClk parameter and the data
//! format in the \e ulConfig parameter.
//!
//! The \e ulConfig parameter is the logical OR of two values: the slot size
//! and the data read/write port select.
//!
//! Following selects the slot size:
//! -\b I2S_SLOT_SIZE_24
//! -\b I2S_SLOT_SIZE_16
//!
//! Following selects the data read/write port:
//! -\b I2S_PORT_DMA
//! -\b I2S_PORT_CPU
//!
//! \return None.
//
//*****************************************************************************
void I2SConfigSetExpClk(unsigned long ulBase, unsigned long ulI2SClk,
                               unsigned long ulBitClk, unsigned long ulConfig)
{
  unsigned long ulHClkDiv;
  unsigned long ulClkDiv;

  //
  // Calculate clock dividers
  //
  ulHClkDiv = ((ulI2SClk/ulBitClk)-1);
  ulClkDiv  = 0;

  //
  // Check if HCLK divider is overflowing
  //
  if(ulHClkDiv > 0xFFF)
  {
    ulHClkDiv = 0xFFF;

    //
    // Calculate clock divider
    //
    ulClkDiv = ((ulI2SClk/(ulBitClk * (ulHClkDiv + 1))) & 0x1F);
  }

  HWREG(ulBase + MCASP_O_ACLKXCTL) = (0xA0|ulClkDiv);

  HWREG(ulBase + MCASP_O_AHCLKXCTL) = (0x8000|ulHClkDiv);

  //
  // Write the Tx format register
  //
  HWREG(ulBase + MCASP_O_TXFMT) = (0x18000 | (ulConfig & 0xFFFF));

  //
  // Write the Rx format register
  //
  HWREG(ulBase + MCASP_O_RXFMT) = (0x18000 | ((ulConfig >> 16) &0xFFFF));

  //
  // Configure Tx FSync generator in I2S mode
  //
  HWREG(ulBase + MCASP_O_TXFMCTL) = 0x113;

  //
  // Configure Rx FSync generator in I2S mode
  //
  HWREG(ulBase + MCASP_O_RXFMCTL) = 0x113;

  //
  // Set Tx bit valid mask
  //
  HWREG(ulBase + MCASP_O_TXMASK) = 0xFFFFFFFF;

  //
  // Set Rx bit valid mask
  //
  HWREG(ulBase + MCASP_O_RXMASK) = 0xFFFFFFFF;

  //
  // Set Tx slot valid mask
  //
  HWREG(ulBase + MCASP_O_TXTDM) = 0x3;

  //
  // Set Rx slot valid mask
  //
  HWREG(ulBase + MCASP_O_RXTDM) = 0x3;
}

//*****************************************************************************
//
//! Configure and enable transmit FIFO.
//!
//! \param ulBase is the base address of the I2S module.
//! \param ulTxLevel is the transmit FIFO DMA request level.
//! \param ulWordsPerTransfer is the nuber of words transferred from the FIFO.
//!
//! This function configures and enable I2S transmit FIFO.
//!
//! The parameter \e ulTxLevel sets the level at which transmit DMA requests
//! are generated. This should be non-zero integer multiple of number of
//! serializers enabled as transmitters
//!
//! The parameter \e ulWordsPerTransfer sets the number of words that are
//! transferred from the transmit FIFO to the data line(s). This value must
//! equal the number of serializers used as transmitters.
//!
//! \return None.
//
//*****************************************************************************
void I2STxFIFOEnable(unsigned long ulBase, unsigned long ulTxLevel,
                     unsigned long ulWordsPerTransfer)
{
  //
  // Set transmit FIFO configuration and
  // enable it
  //
  HWREG(ulBase + MCASP_0_WFIFOCTL) = ((1 <<16) | ((ulTxLevel & 0xFF) << 8)
					| (ulWordsPerTransfer & 0x1F));

}

//*****************************************************************************
//
//! Disables transmit FIFO.
//!
//! \param ulBase is the base address of the I2S module.
//!
//! This function disables the I2S transmit FIFO.
//!
//! \return None.
//
//*****************************************************************************
void I2STxFIFODisable(unsigned long ulBase)
{
  //
  // Disable transmit FIFO.
  //
  HWREG(ulBase + MCASP_0_WFIFOCTL) = 0;
}

//*****************************************************************************
//
//! Configure and enable receive FIFO.
//!
//! \param ulBase is the base address of the I2S module.
//! \param ulRxLevel is the receive FIFO DMA request level.
//! \param ulWordsPerTransfer is the nuber of words transferred from the FIFO.
//!
//! This function configures and enable I2S receive FIFO.
//!
//! The parameter \e ulRxLevel sets the level at which receive DMA requests
//! are generated. This should be non-zero integer multiple of number of
//! serializers enabled as receivers.
//!
//! The parameter \e ulWordsPerTransfer sets the number of words that are
//! transferred to the receive FIFO from the data line(s). This value must
//! equal the number of serializers used as receivers.
//!
//! \return None.
//
//*****************************************************************************
void I2SRxFIFOEnable(unsigned long ulBase, unsigned long ulRxLevel,
                     unsigned long ulWordsPerTransfer)
{
  //
  // Set FIFO configuration
  //
  HWREG(ulBase + MCASP_0_RFIFOCTL) = ( (1 <<16) | ((ulRxLevel & 0xFF) << 8)
					| (ulWordsPerTransfer & 0x1F));

}

//*****************************************************************************
//
//! Disables receive FIFO.
//!
//! \param ulBase is the base address of the I2S module.
//!
//! This function disables the I2S receive FIFO.
//!
//! \return None.
//
//*****************************************************************************
void I2SRxFIFODisable(unsigned long ulBase)
{
  //
  // Disable receive FIFO.
  //
  HWREG(ulBase + MCASP_0_RFIFOCTL) = 0;
}

//*****************************************************************************
//
//! Get the transmit FIFO status.
//!
//! \param ulBase is the base address of the I2S module.
//!
//! This function gets the number of 32-bit words currently in the transmit
//! FIFO.
//!
//! \return Returns transmit FIFO status.
//
//*****************************************************************************
unsigned long I2STxFIFOStatusGet(unsigned long ulBase)
{
  //
  // Return transmit FIFO level
  //
  return HWREG(ulBase + MCASP_0_WFIFOSTS);
}

//*****************************************************************************
//
//! Get the receive FIFO status.
//!
//! \param ulBase is the base address of the I2S module.
//!
//! This function gets the number of 32-bit words currently in the receive
//! FIFO.
//!
//! \return Returns receive FIFO status.
//
//*****************************************************************************
unsigned long I2SRxFIFOStatusGet(unsigned long ulBase)
{
  //
  // Return receive FIFO level
  //
  return HWREG(ulBase + MCASP_0_RFIFOSTS);
}

//*****************************************************************************
//
//! Configure the serializer in specified mode.
//!
//! \param ulBase is the base address of the I2S module.
//! \param ulDataLine is the data line (serilizer) to be configured.
//! \param ulSerMode is the required serializer mode.
//! \param ulInActState sets the inactive state of the data line.
//!
//! This function configure and enable the serializer associated with the given
//! data line in specified mode.
//!
//! The paramenter \e ulDataLine selects to data line to be configured and
//! can be one of the following:
//! -\b I2S_DATA_LINE_0
//! -\b I2S_DATA_LINE_1
//!
//! The parameter \e ulSerMode can be one of the following:
//! -\b I2S_SER_MODE_TX
//! -\b I2S_SER_MODE_RX
//! -\b I2S_SER_MODE_DISABLE
//!
//! The parameter \e ulInActState can be one of the following
//! -\b I2S_INACT_TRI_STATE
//! -\b I2S_INACT_LOW_LEVEL
//! -\b I2S_INACT_LOW_HIGH
//!
//! \return Returns receive FIFO status.
//
//*****************************************************************************
void I2SSerializerConfig(unsigned long ulBase, unsigned long ulDataLine,
                         unsigned long ulSerMode, unsigned long ulInActState)
{
  if( ulSerMode == I2S_SER_MODE_TX)
  {
    //
    // Set the data line in output mode
    //
    HWREG(ulBase + MCASP_O_PDIR) |= ulDataLine;
  }
  else
  {
    //
    // Set the data line in input mode
    //
    HWREG(ulBase + MCASP_O_PDIR) &= ~ulDataLine;
  }

  //
  // Set the serializer configuration.
  //
  HWREG(ulBase + MCASP_O_XRSRCTL0 + ((ulDataLine-1) << 2))
                                            = (ulSerMode | ulInActState);
}

//*****************************************************************************
//
//! Enables individual I2S interrupt sources.
//!
//! \param ulBase is the base address of the I2S module.
//! \param ulIntFlags is the bit mask of the interrupt sources to be enabled.
//!
//! This function enables the indicated I2S interrupt sources.  Only the
//! sources that are enabled can be reflected to the processor interrupt;
//! disabled sources have no effect on the processor.
//!
//! The \e ulIntFlags parameter is the logical OR of any of the following:
//!
//! -\b I2S_INT_XUNDRN
//! -\b I2S_INT_XSYNCERR
//! -\b I2S_INT_XLAST
//! -\b I2S_INT_XDATA
//! -\b I2S_INT_XSTAFRM
//! -\b I2S_INT_XDMA
//! -\b I2S_INT_ROVRN
//! -\b I2S_INT_RSYNCERR
//! -\b I2S_INT_RLAST
//! -\b I2S_INT_RDATA
//! -\b I2S_INT_RSTAFRM
//! -\b I2S_INT_RDMA
//!
//! \return None.
//
//*****************************************************************************
void I2SIntEnable(unsigned long ulBase, unsigned long ulIntFlags)
{

  //
  // Enable DMA done interrupts
  //
  HWREG(APPS_CONFIG_BASE + APPS_CONFIG_O_DMA_DONE_INT_MASK_CLR )
     |= ((ulIntFlags &0xC0000000) >> 20);

  //
  // Enable specific Tx Interrupts
  //
  HWREG(ulBase + MCASP_O_EVTCTLX) |= (ulIntFlags & 0xFF);

  //
  // Enable specific Rx Interrupts
  //
  HWREG(ulBase + MCASP_O_EVTCTLR) |= ((ulIntFlags >> 16) & 0xFF);
}

//*****************************************************************************
//
//! Disables individual I2S interrupt sources.
//!
//! \param ulBase is the base address of the I2S module.
//! \param ulIntFlags is the bit mask of the interrupt sources to be disabled.
//!
//! This function disables the indicated I2S interrupt sources.  Only the
//! sources that are enabled can be reflected to the processor interrupt;
//! disabled sources have no effect on the processor.
//!
//! The \e ulIntFlags parameter has the same definition as the \e ulIntFlags
//! parameter to I2SIntEnable().
//!
//! \return None.
//
//*****************************************************************************
void I2SIntDisable(unsigned long ulBase, unsigned long ulIntFlags)
{
  //
  // Disable DMA done interrupts
  //
  HWREG(APPS_CONFIG_BASE + APPS_CONFIG_O_DMA_DONE_INT_MASK_SET)
     |= ((ulIntFlags &0xC0000000) >> 20);

  //
  // Disable specific Tx Interrupts
  //
  HWREG(ulBase + MCASP_O_EVTCTLX) &= ~(ulIntFlags & 0xFF);

  //
  // Disable specific Rx Interrupts
  //
  HWREG(ulBase + MCASP_O_EVTCTLR) &= ~((ulIntFlags >> 16) & 0xFF);
}


//*****************************************************************************
//
//! Gets the current interrupt status.
//!
//! \param ulBase is the base address of the I2S module.
//!
//! This function returns the raw interrupt status for I2S enumerated
//! as a bit field of values:
//! -\b I2S_STS_XERR
//! -\b I2S_STS_XDMAERR
//! -\b I2S_STS_XSTAFRM
//! -\b I2S_STS_XDATA
//! -\b I2S_STS_XLAST
//! -\b I2S_STS_XSYNCERR
//! -\b I2S_STS_XUNDRN
//! -\b I2S_STS_XDMA
//! -\b I2S_STS_RERR
//! -\b I2S_STS_RDMAERR
//! -\b I2S_STS_RSTAFRM
//! -\b I2S_STS_RDATA
//! -\b I2S_STS_RLAST
//! -\b I2S_STS_RSYNCERR
//! -\b I2S_STS_ROVERN
//! -\b I2S_STS_RDMA
//!
//! \return Returns the current interrupt status, enumerated as a bit field of
//! values described above.
//
//*****************************************************************************
unsigned long I2SIntStatus(unsigned long ulBase)
{
  unsigned long ulStatus;

  //
  // Get DMA interrupt status
  //
  ulStatus =
    HWREG(APPS_CONFIG_BASE + APPS_CONFIG_O_DMA_DONE_INT_STS_RAW) << 20;

  ulStatus &= 0xC0000000;

  //
  // Read Tx Interrupt status
  //
  ulStatus |= HWREG(ulBase + MCASP_O_TXSTAT);

  //
  // Read Rx Interrupt status
  //
  ulStatus |= HWREG(ulBase + MCASP_O_RXSTAT) << 16;

  //
  // Return the status
  //
  return ulStatus;
}

//*****************************************************************************
//
//! Clears I2S interrupt sources.
//!
//! \param ulBase is the base address of the I2S module.
//! \param ulStatFlags is a bit mask of the interrupt sources to be cleared.
//!
//! The specified I2S interrupt sources are cleared, so that they no longer
//! assert.  This function must be called in the interrupt handler to keep the
//! interrupt from being recognized again immediately upon exit.
//!
//! The \e ulIntFlags parameter is the logical OR of any of the value
//! describe in I2SIntStatus().
//!
//! \return None.
//
//*****************************************************************************
void I2SIntClear(unsigned long ulBase, unsigned long ulStatFlags)
{
  //
  // Clear DMA done interrupts
  //
  HWREG(APPS_CONFIG_BASE + APPS_CONFIG_O_DMA_DONE_INT_ACK)
     |= ((ulStatFlags &0xC0000000) >> 20);

  //
  // Clear Tx Interrupt
  //
  HWREG(ulBase + MCASP_O_TXSTAT) = ulStatFlags & 0x1FF ;

  //
  // Clear Rx Interrupt
  //
  HWREG(ulBase + MCASP_O_RXSTAT) = (ulStatFlags >> 16) & 0x1FF;
}

//*****************************************************************************
//
//! Registers an interrupt handler for a I2S interrupt.
//!
//! \param ulBase is the base address of the I2S module.
//! \param pfnHandler is a pointer to the function to be called when the
//! I2S interrupt occurs.
//!
//! This function does the actual registering of the interrupt handler.  This
//! function enables the global interrupt in the interrupt controller; specific
//! I2S interrupts must be enabled via I2SIntEnable().  It is the interrupt
//! handler's responsibility to clear the interrupt source.
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \return None.
//
//*****************************************************************************
void I2SIntRegister(unsigned long ulBase, void (*pfnHandler)(void))
{
  //
  // Register the interrupt handler
  //
  IntRegister(INT_I2S,pfnHandler);

  //
  // Enable the interrupt
  //
  IntEnable(INT_I2S);
}

//*****************************************************************************
//
//! Unregisters an interrupt handler for a I2S interrupt.
//!
//! \param ulBase is the base address of the I2S module.
//!
//! This function does the actual unregistering of the interrupt handler.  It
//! clears the handler to be called when a I2S interrupt occurs.  This
//! function also masks off the interrupt in the interrupt controller so that
//! the interrupt handler no longer is called.
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \return None.
//
//*****************************************************************************
void I2SIntUnregister(unsigned long ulBase)
{
  //
  // Disable interrupt
  //
  IntDisable(INT_I2S);

  //
  // Unregister the handler
  //
  IntUnregister(INT_I2S);

}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
