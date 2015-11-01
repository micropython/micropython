//*****************************************************************************
//
//  spi.c
//
//  Driver for the SPI.
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
//! \addtogroup SPI_Serial_Peripheral_Interface_api
//! @{
//
//*****************************************************************************


#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_mcspi.h"
#include "inc/hw_apps_config.h"
#include "interrupt.h"
#include "spi.h"


//*****************************************************************************
//
// A mapping of SPI base address to interupt number.
//
//*****************************************************************************
static const unsigned long g_ppulSPIIntMap[][3] =
{
    { SSPI_BASE, INT_SSPI }, // Shared SPI
    { GSPI_BASE, INT_GSPI }, // Generic SPI
    { LSPI_BASE, INT_LSPI }, // LINK SPI
};

//*****************************************************************************
//
// A mapping of SPI base address to DMA done interrupt mask bit(s).
//
//*****************************************************************************
static const unsigned long g_ulSPIDmaMaskMap[][2]=
{
  {SSPI_BASE,APPS_CONFIG_DMA_DONE_INT_MASK_SHSPI_WR_DMA_DONE_INT_MASK},
  {LSPI_BASE,APPS_CONFIG_DMA_DONE_INT_MASK_HOSTSPI_WR_DMA_DONE_INT_MASK},
  {GSPI_BASE,APPS_CONFIG_DMA_DONE_INT_MASK_APPS_SPI_WR_DMA_DONE_INT_MASK},
};

//*****************************************************************************
//
//! \internal
//! Transfer bytes over SPI channel
//!
//! \param ulBase is the base address of SPI module
//! \param ucDout is the pointer to Tx data buffer or 0.
//! \param ucDin is pointer to Rx data buffer or 0
//! \param ulCount is the size of data in bytes.
//!
//! This function transfers \e ulCount bytes of data over SPI channel.
//!
//! The function will not return until data has been transmitted
//!
//! \return Returns 0 on success, -1 otherwise.
//
//*****************************************************************************
static long SPITransfer8(unsigned long ulBase, unsigned char *ucDout,
                   unsigned char *ucDin, unsigned long ulCount,
                   unsigned long ulFlags)
{
  unsigned long ulReadReg;
  unsigned long ulWriteReg;
  unsigned long ulStatReg;
  unsigned long ulOutIncr;
  unsigned long ulInIncr;
  unsigned long ulTxDummy;
  unsigned long ulRxDummy;

  //
  // Initialize the variables
  //
  ulOutIncr = 1;
  ulInIncr  = 1;

  //
  // Check if output buffer pointer is 0
  //
  if(ucDout == 0)
  {
    ulOutIncr = 0;
    ulTxDummy = 0xFFFFFFFF;
    ucDout = (unsigned char *)&ulTxDummy;
  }

  //
  // Check if input buffer pointer is 0
  //
  if(ucDin == 0)
  {
    ulInIncr = 0;
    ucDin = (unsigned char *)&ulRxDummy;
  }

  //
  // Load the register addresses.
  //
  ulReadReg  = (ulBase + MCSPI_O_RX0);
  ulWriteReg = (ulBase + MCSPI_O_TX0);
  ulStatReg  = (ulBase + MCSPI_O_CH0STAT);

  //
  // Enable CS based on Flag
  //
  if( ulFlags & SPI_CS_ENABLE)
  {
    HWREG( ulBase + MCSPI_O_CH0CONF) |= MCSPI_CH0CONF_FORCE;
  }

  while(ulCount)
  {
    //
    // Wait for space in output register/FIFO.
    //
    while( !(HWREG(ulStatReg) & MCSPI_CH0STAT_TXS) )
    {
    }

    //
    // Write the data
    //
    HWREG(ulWriteReg) = *ucDout;

    //
    // Wait for data in input register/FIFO.
    //
    while( !( HWREG(ulStatReg) & MCSPI_CH0STAT_RXS) )
    {
    }

    //
    // Read the data
    //
    *ucDin = HWREG(ulReadReg);

    //
    // Increment pointers.
    //
    ucDout = ucDout + ulOutIncr;
    ucDin  = ucDin  + ulInIncr;

    //
    // Decrement the count.
    //
    ulCount--;
  }

  //
  // Disable CS based on Flag
  //
  if( ulFlags & SPI_CS_DISABLE)
  {
    HWREG( ulBase + MCSPI_O_CH0CONF) &= ~MCSPI_CH0CONF_FORCE;
  }

  return 0;
}

//*****************************************************************************
//
//! \internal
//! Transfer half-words over SPI channel
//!
//! \param ulBase is the base address of SPI module
//! \param usDout is the pointer to Tx data buffer or 0.
//! \param usDin is pointer to Rx data buffer or 0
//! \param ulCount is the size of data in bytes.
//!
//! This function transfers \e ulCount bytes of data over SPI channel. Since
//! the API sends a half-word at a time \e ulCount should be a multiple
//! of two.
//!
//! The function will not return until data has been transmitted
//!
//! \return Returns 0 on success, -1 otherwise.
//
//*****************************************************************************
static long SPITransfer16(unsigned long ulBase, unsigned short *usDout,
                   unsigned short *usDin, unsigned long ulCount,
                   unsigned long ulFlags)
{
  unsigned long ulReadReg;
  unsigned long ulWriteReg;
  unsigned long ulStatReg;
  unsigned long ulOutIncr;
  unsigned long ulInIncr;
  unsigned long ulTxDummy;
  unsigned long ulRxDummy;

  //
  // Initialize the variables.
  //
  ulOutIncr = 1;
  ulInIncr  = 1;

  //
  // Check if count is multiple of half-word
  //
  if(ulCount%2)
  {
    return -1;
  }

  //
  // Compute number of half words.
  //
  ulCount = ulCount/2;

  //
  // Check if output buffer pointer is 0
  //
  if(usDout == 0)
  {
    ulOutIncr = 0;
    ulTxDummy = 0xFFFFFFFF;
    usDout = (unsigned short *)&ulTxDummy;
  }

  //
  // Check if input buffer pointer is 0
  //
  if(usDin == 0)
  {
    ulInIncr = 0;
    usDin = (unsigned short *)&ulRxDummy;
  }

  //
  // Load the register addresses.
  //
  ulReadReg  = (ulBase + MCSPI_O_RX0);
  ulWriteReg = (ulBase + MCSPI_O_TX0);
  ulStatReg  = (ulBase + MCSPI_O_CH0STAT);

  //
  // Enable CS based on Flag
  //
  if( ulFlags & SPI_CS_ENABLE)
  {
    HWREG( ulBase + MCSPI_O_CH0CONF) |= MCSPI_CH0CONF_FORCE;
  }

  while(ulCount)
  {
    //
    // Wait for space in output register/FIFO.
    //
    while( !(HWREG(ulStatReg) & MCSPI_CH0STAT_TXS) )
    {
    }

    //
    // Write the data
    //
    HWREG(ulWriteReg) = *usDout;

    //
    // Wait for data in input register/FIFO.
    //
    while( !( HWREG(ulStatReg) & MCSPI_CH0STAT_RXS) )
    {
    }

    //
    // Read the data
    //
    *usDin = HWREG(ulReadReg);

    //
    // Increment pointers.
    //
    usDout = usDout + ulOutIncr;
    usDin  = usDin  + ulInIncr;

    //
    // Decrement the count.
    //
    ulCount--;
  }

  //
  // Disable CS based on Flag
  //
  if( ulFlags & SPI_CS_DISABLE)
  {
    HWREG( ulBase + MCSPI_O_CH0CONF) &= ~MCSPI_CH0CONF_FORCE;
  }

  return 0;
}

//*****************************************************************************
//
//! \internal
//! Transfer words over SPI channel
//!
//! \param ulBase is the base address of SPI module
//! \param ulDout is the pointer to Tx data buffer or 0.
//! \param ulDin is pointer to Rx data buffer or 0
//! \param ulCount is the size of data in bytes.
//!
//! This function transfers \e ulCount bytes of data over SPI channel. Since
//! the API sends a word at a time \e ulCount should be a multiple of four.
//!
//! The function will not return until data has been transmitted
//!
//! \return Returns 0 on success, -1 otherwise.
//
//*****************************************************************************
static long SPITransfer32(unsigned long ulBase, unsigned long *ulDout,
                   unsigned long *ulDin, unsigned long ulCount,
                   unsigned long ulFlags)
{
  unsigned long ulReadReg;
  unsigned long ulWriteReg;
  unsigned long ulStatReg;
  unsigned long ulOutIncr;
  unsigned long ulInIncr;
  unsigned long ulTxDummy;
  unsigned long ulRxDummy;

  //
  // Initialize the variables.
  //
  ulOutIncr = 1;
  ulInIncr  = 1;

  //
  // Check if count is multiple of word
  //
  if(ulCount%4)
  {
    return -1;
  }

  //
  // Compute the number of words to be transferd
  //
  ulCount = ulCount/4;

  //
  // Check if output buffer pointer is 0
  //
  if(ulDout == 0)
  {
    ulOutIncr = 0;
    ulTxDummy = 0xFFFFFFFF;
    ulDout = &ulTxDummy;
  }

  //
  // Check if input buffer pointer is 0
  //
  if(ulDin == 0)
  {
    ulInIncr = 0;
    ulDin = &ulRxDummy;
  }


  //
  // Load the register addresses.
  //
  ulReadReg  = (ulBase + MCSPI_O_RX0);
  ulWriteReg = (ulBase + MCSPI_O_TX0);
  ulStatReg  = (ulBase + MCSPI_O_CH0STAT);

  //
  // Enable CS based on Flag
  //
  if( ulFlags & SPI_CS_ENABLE)
  {
    HWREG( ulBase + MCSPI_O_CH0CONF) |= MCSPI_CH0CONF_FORCE;
  }

  while(ulCount)
  {
    //
    // Wait for space in output register/FIFO.
    //
    while( !(HWREG(ulStatReg) & MCSPI_CH0STAT_TXS) )
    {
    }

    //
    // Write the data
    //
    HWREG(ulWriteReg) = *ulDout;

    //
    // Wait for data in input register/FIFO.
    //
    while( !( HWREG(ulStatReg) & MCSPI_CH0STAT_RXS) )
    {
    }

    //
    // Read the data
    //
    *ulDin = HWREG(ulReadReg);

    //
    // Increment pointers.
    //
    ulDout = ulDout + ulOutIncr;
    ulDin  = ulDin  + ulInIncr;

    //
    // Decrement the count.
    //
    ulCount--;
  }

  //
  // Disable CS based on Flag
  //
  if( ulFlags & SPI_CS_DISABLE)
  {
    HWREG( ulBase + MCSPI_O_CH0CONF) &= ~MCSPI_CH0CONF_FORCE;
  }

  return 0;
}

//*****************************************************************************
//
//! \internal
//! Gets the SPI interrupt number.
//!
//! \param ulBase is the base address of the SPI module
//!
//! Given a SPI base address, returns the corresponding interrupt number.
//!
//! \return Returns a SPI interrupt number, or -1 if \e ulBase is invalid.
//
//*****************************************************************************
static long
SPIIntNumberGet(unsigned long ulBase)
{
    unsigned long ulIdx;

    //
    // Loop through the table that maps SPI base addresses to interrupt
    // numbers.
    //
    for(ulIdx = 0; ulIdx < (sizeof(g_ppulSPIIntMap) /
                            sizeof(g_ppulSPIIntMap[0])); ulIdx++)
    {
        //
        // See if this base address matches.
        //
        if(g_ppulSPIIntMap[ulIdx][0] == ulBase)
        {
            //
            // Return the corresponding interrupt number.
            //
            return(g_ppulSPIIntMap[ulIdx][1]);
        }
    }

    //
    // The base address could not be found, so return an error.
    //
    return(-1);
}

//*****************************************************************************
//
//! \internal
//! Gets the SPI DMA interrupt mask bit.
//!
//! \param ulBase is the base address of the SPI module
//!
//! Given a SPI base address, DMA interrupt mask bit.
//!
//! \return Returns a DMA interrupt mask bit, or -1 if \e ulBase is invalid.
//
//*****************************************************************************
static long
SPIDmaMaskGet(unsigned long ulBase)
{
    unsigned long ulIdx;

    //
    // Loop through the table that maps SPI base addresses to interrupt
    // numbers.
    //
    for(ulIdx = 0; ulIdx < (sizeof(g_ulSPIDmaMaskMap) /
                            sizeof(g_ulSPIDmaMaskMap[0])); ulIdx++)
    {
        //
        // See if this base address matches.
        //
        if(g_ulSPIDmaMaskMap[ulIdx][0] == ulBase)
        {
            //
            // Return the corresponding interrupt number.
            //
            return(g_ulSPIDmaMaskMap[ulIdx][1]);
        }
    }

    //
    // The base address could not be found, so return an error.
    //
    return(-1);
}

//*****************************************************************************
//
//! Enables transmitting and receiving.
//!
//! \param ulBase is the base address of the SPI module
//!
//! This function enables the SPI channel for transmitting and receiving.
//!
//! \return None
//!
//
//*****************************************************************************
void
SPIEnable(unsigned long ulBase)
{
  //
  // Set Channel Enable Bit
  //
  HWREG(ulBase + MCSPI_O_CH0CTRL) |= MCSPI_CH0CTRL_EN;
}

//*****************************************************************************
//
//! Disables the transmitting and receiving.
//!
//! \param ulBase is the base address of the SPI module
//!
//! This function disables the SPI channel for transmitting and receiving.
//!
//! \return None
//!
//
//*****************************************************************************
void
SPIDisable(unsigned long ulBase)
{
  //
  // Reset Channel Enable Bit
  //
  HWREG(ulBase + MCSPI_O_CH0CTRL) &= ~MCSPI_CH0CTRL_EN;
}


//*****************************************************************************
//
//! Enables the SPI DMA operation for transmitting and/or receving.
//!
//! \param ulBase is the base address of the SPI module
//! \param ulFlags selectes the DMA signal for transmit and/or receive.
//!
//! This function enables transmit and/or receive DMA request based on the
//! \e ulFlags parameter.
//!
//! The parameter \e ulFlags is the logical OR of one or more of
//! the following :
//! - \b SPI_RX_DMA
//! - \b SPI_TX_DMA
//!
//! \return None.
//
//*****************************************************************************
void
SPIDmaEnable(unsigned long ulBase, unsigned long ulFlags)
{
  //
  // Enable DMA based on ulFlags
  //
  HWREG(ulBase + MCSPI_O_CH0CONF) |= ulFlags;
}

//*****************************************************************************
//
//! Disables the SPI DMA operation for transmitting and/or receving.
//!
//! \param ulBase is the base address of the SPI module
//! \param ulFlags selectes the DMA signal for transmit and/or receive.
//!
//! This function disables transmit and/or receive DMA request based on the
//! \e ulFlags parameter.
//!
//! The parameter \e ulFlags is the logical OR of one or more of
//! the following :
//! - \b SPI_RX_DMA
//! - \b SPI_TX_DMA
//!
//! \return None.
//
//*****************************************************************************
void
SPIDmaDisable(unsigned long ulBase, unsigned long ulFlags)
{
  //
  // Disable DMA based on ulFlags
  //
  HWREG(ulBase + MCSPI_O_CH0CONF) &= ulFlags;
}

//*****************************************************************************
//
//! Performs a software reset of the specified SPI module
//!
//! \param ulBase is the base address of the SPI module
//!
//! This function performs a software reset of the specified SPI module
//!
//! \return None.
//
//*****************************************************************************
void
SPIReset(unsigned long ulBase)
{

  //
  // Assert soft reset (auto clear)
  //
  HWREG(ulBase + MCSPI_O_SYSCONFIG) |= MCSPI_SYSCONFIG_SOFTRESET;

  //
  // wait until reset is done
  //
  while(!(HWREG(ulBase + MCSPI_O_SYSSTATUS)& MCSPI_SYSSTATUS_RESETDONE))
  {
  }
}

//*****************************************************************************
//
//! Sets the configuration of a SPI module
//!
//! \param ulBase is the base address of the SPI module
//! \param ulSPIClk is the rate of clock supplied to the SPI module.
//! \param ulBitRate is the desired bit rate.(master mode)
//! \param ulMode is the mode of operation.
//! \param ulSubMode is one of the valid sub-modes.
//! \param ulConfig is logical OR of configuration paramaters.
//!
//! This function configures SPI port for operation in specified sub-mode and
//! required bit rated as specified by \e ulMode and \e ulBitRate parameters
//! respectively.
//!
//! The SPI module can operate in either master or slave mode. The parameter
//! \e ulMode can be one of the following
//! -\b SPI_MODE_MASTER
//! -\b SPI_MODE_SLAVE
//!
//! The SPI module supports 4 sub modes based on SPI clock polarity and phase.
//!
//! <pre>
//! Polarity Phase  Sub-Mode
//!   0       0        0
//!   0       1        1
//!   1       0        2
//!   1       1        3
//! </pre>
//!
//! Required sub mode can be select by setting \e ulSubMode parameter to one
//! of the following
//! - \b SPI_SUB_MODE_0
//! - \b SPI_SUB_MODE_1
//! - \b SPI_SUB_MODE_2
//! - \b SPI_SUB_MODE_3
//!
//! The parameter \e ulConfig is logical OR of five values: the word length,
//! active level for chip select, software or hardware controled chip select,
//! 3 or 4 pin mode and turbo mode.
//! mode.
//!
//! SPI support 8, 16 and 32 bit word lengths defined by:-
//! - \b SPI_WL_8
//! - \b SPI_WL_16
//! - \b SPI_WL_32
//!
//! Active state of Chip Select can be defined by:-
//! - \b SPI_CS_ACTIVELOW
//! - \b SPI_CS_ACTIVEHIGH
//!
//! SPI chip select can be configured to be controlled either by hardware or
//! software:-
//! - \b SPI_SW_CS
//! - \b SPI_HW_CS
//!
//! The module can work in 3 or 4 pin mode defined by:-
//! - \b SPI_3PIN_MODE
//! - \b SPI_4PIN_MODE
//!
//! Turbo mode can be set on or turned off using:-
//! - \b SPI_TURBO_MODE_ON
//! - \b SPI_TURBO_MODE_OFF
//!
//! \return None.
//
//*****************************************************************************
void
SPIConfigSetExpClk(unsigned long ulBase,unsigned long ulSPIClk,
            unsigned long ulBitRate, unsigned long ulMode,
            unsigned long ulSubMode, unsigned long ulConfig)
{

  unsigned long ulRegData;
  unsigned long ulDivider;

  //
  // Read MODULCTRL register
  //
  ulRegData = HWREG(ulBase + MCSPI_O_MODULCTRL);

  //
  // Set Master mode with h/w chip select
  //
  ulRegData &= ~(MCSPI_MODULCTRL_MS |
                 MCSPI_MODULCTRL_SINGLE);

  //
  // Enable software control Chip Select, Init delay
  // and 3-pin mode
  //
  ulRegData |= (((ulConfig >> 24) | ulMode) & 0xFF); 

  //
  // Write the configuration
  //
  HWREG(ulBase + MCSPI_O_MODULCTRL) = ulRegData;

  //
  // Set IS, DPE0, DPE1 based on master or slave mode
  //
  if(ulMode == SPI_MODE_MASTER)
  {
    ulRegData = 0x1 << 16;
  }
  else
  {
    ulRegData = 0x6 << 16;
  }

  //
  // set clock divider granularity to 1 cycle
  //
  ulRegData |= MCSPI_CH0CONF_CLKG;

  //
  // Get the divider value
  //
  ulDivider = ((ulSPIClk/ulBitRate) - 1);

  //
  // The least significant four bits of the divider is used to configure
  // CLKD in MCSPI_CHCONF next eight least significant bits are used to
  // configure the EXTCLK in MCSPI_CHCTRL
  //
  ulRegData |= ((ulDivider & 0x0000000F) << 2);
  HWREG(ulBase + MCSPI_O_CH0CTRL) = ((ulDivider & 0x00000FF0) << 4);

  //
  // Set the protocol, CS polarity, word length
  // and turbo mode
  //
  ulRegData = ((ulRegData  |
                ulSubMode) | (ulConfig & 0x0008FFFF));

  //
  // Write back the CONF register
  //
  HWREG(ulBase + MCSPI_O_CH0CONF) = ulRegData;

}

//*****************************************************************************
//
//! Receives a word from the specified port.
//!
//! \param ulBase is the base address of the SPI module.
//! \param pulData is pointer to receive data variable.
//!
//! This function gets a SPI word from the receive FIFO for the specified
//! port.
//!
//! \return Returns the number of elements read from the receive FIFO.
//
//*****************************************************************************
long
SPIDataGetNonBlocking(unsigned long ulBase, unsigned long *pulData)
{
  unsigned long ulRegVal;

  //
  // Read register status register
  //
  ulRegVal = HWREG(ulBase + MCSPI_O_CH0STAT);

  //
  // Check is data is available
  //
  if(ulRegVal & MCSPI_CH0STAT_RXS)
  {
    *pulData = HWREG(ulBase + MCSPI_O_RX0);
    return(1);
  }

   return(0);
}

//*****************************************************************************
//
//! Waits for the word to be received on the specified port.
//!
//! \param ulBase is the base address of the SPI module.
//! \param pulData is pointer to receive data variable.
//!
//! This function gets a SPI word from the receive FIFO for the specified
//! port.  If there is no word available, this function waits until a
//! word is received before returning.
//!
//! \return Returns the word read from the specified port, cast as an
//! \e unsigned long.
//
//*****************************************************************************
void
SPIDataGet(unsigned long ulBase, unsigned long *pulData)
{
  //
  // Wait for Rx data
  //
  while(!(HWREG(ulBase + MCSPI_O_CH0STAT) & MCSPI_CH0STAT_RXS))
  {
  }

  //
  // Read the value
  //
  *pulData = HWREG(ulBase + MCSPI_O_RX0);
}

//*****************************************************************************
//
//! Transmits a word on the specified port.
//!
//! \param ulBase is the base address of the SPI module
//! \param ulData is data to be transmitted.
//!
//! This function transmits a SPI word on the transmit FIFO for the specified
//! port.
//!
//! \return  Returns the number of elements written to the transmit FIFO.
//!
//*****************************************************************************
long
SPIDataPutNonBlocking(unsigned long ulBase, unsigned long ulData)
{
    unsigned long ulRegVal;

    //
    // Read status register
    //
    ulRegVal = HWREG(ulBase + MCSPI_O_CH0STAT);

    //
    // Write value into Tx register/FIFO
    // if space is available
    //
    if(ulRegVal & MCSPI_CH0STAT_TXS)
    {
      HWREG(ulBase + MCSPI_O_TX0) = ulData;
      return(1);
    }

    return(0);
}

//*****************************************************************************
//
//! Waits until the word is transmitted on the specified port.
//!
//! \param ulBase is the base address of the SPI module
//! \param ulData is data to be transmitted.
//!
//! This function transmits a SPI word on the transmit FIFO for the specified
//! port. This function waits until the space is available on transmit FIFO
//!
//! \return None
//!
//*****************************************************************************
void
SPIDataPut(unsigned long ulBase, unsigned long ulData)
{
  //
  // Wait for space in FIFO
  //
  while(!(HWREG(ulBase + MCSPI_O_CH0STAT)&MCSPI_CH0STAT_TXS))
  {
  }

  //
  // Write the data
  //
  HWREG(ulBase + MCSPI_O_TX0) = ulData;
}

//*****************************************************************************
//
//! Enables the transmit and/or receive FIFOs.
//!
//! \param ulBase is the base address of the SPI module
//! \param ulFlags selects the FIFO(s) to be enabled
//!
//! This function enables the transmit and/or receive FIFOs as specified by
//! \e ulFlags.
//! The parameter \e ulFlags shoulde be logical OR of one or more of the
//! following:
//! - \b SPI_TX_FIFO
//! - \b SPI_RX_FIFO
//!
//! \return None.
//
//*****************************************************************************
void
SPIFIFOEnable(unsigned long ulBase, unsigned long ulFlags)
{
  //
  // Set FIFO enable bits.
  //
  HWREG(ulBase + MCSPI_O_CH0CONF) |= ulFlags;
}

//*****************************************************************************
//
//! Disables the transmit and/or receive FIFOs.
//!
//! \param ulBase is the base address of the SPI module
//! \param ulFlags selects the FIFO(s) to be enabled
//!
//! This function disables transmit and/or receive FIFOs. as specified by
//! \e ulFlags.
//! The parameter \e ulFlags shoulde be logical OR of one or more of the
//! following:
//! - \b SPI_TX_FIFO
//! - \b SPI_RX_FIFO
//!
//! \return None.
//
//*****************************************************************************
void
SPIFIFODisable(unsigned long ulBase, unsigned long ulFlags)
{
  //
  // Reset FIFO Enable bits.
  //
  HWREG(ulBase + MCSPI_O_CH0CONF) &= ~(ulFlags);
}

//*****************************************************************************
//
//! Sets the FIFO level at which DMA requests or interrupts are generated.
//!
//! \param ulBase is the base address of the SPI module
//! \param ulTxLevel is the Almost Empty Level for transmit FIFO.
//! \param ulRxLevel is the Almost Full Level for the receive FIFO.
//!
//! This function Sets the FIFO level at which DMA requests or interrupts
//! are generated.
//!
//! \return None.
//
//*****************************************************************************
void SPIFIFOLevelSet(unsigned long ulBase, unsigned long ulTxLevel,
                   unsigned long ulRxLevel)
{
  unsigned long ulRegVal;

  //
  // Read the current configuration
  //
  ulRegVal = HWREG(ulBase + MCSPI_O_XFERLEVEL);

  //
  // Mask and set new FIFO thresholds.
  //
  ulRegVal = ((ulRegVal & 0xFFFF0000) | (((ulRxLevel-1) << 8) | (ulTxLevel-1)));

  //
  // Set the transmit and receive FIFO thresholds.
  //
  HWREG(ulBase + MCSPI_O_XFERLEVEL) = ulRegVal;

}

//*****************************************************************************
//
//! Gets the FIFO level at which DMA requests or interrupts are generated.
//!
//! \param ulBase is the base address of the SPI module
//! \param pulTxLevel is a pointer to storage for the transmit FIFO level
//! \param pulRxLevel is a pointer to storage for the receive FIFO level
//!
//! This function gets the FIFO level at which DMA requests or interrupts
//! are generated.
//!
//! \return None.
//
//*****************************************************************************
void
SPIFIFOLevelGet(unsigned long ulBase, unsigned long *pulTxLevel,
                 unsigned long *pulRxLevel)
{
  unsigned long ulRegVal;

  //
  // Read the current configuration
  //
  ulRegVal = HWREG(ulBase + MCSPI_O_XFERLEVEL);

  *pulTxLevel = (ulRegVal & 0xFF);

  *pulRxLevel = ((ulRegVal >> 8) & 0xFF);

}

//*****************************************************************************
//
//! Sets the word count.
//!
//! \param ulBase is the base address of the SPI module
//! \param ulWordCount is number of SPI words to be transmitted.
//!
//! This function sets the word count, which is the number of SPI word to
//! be transferred on channel when using the FIFO buffer.
//!
//! \return None.
//
//*****************************************************************************
void
SPIWordCountSet(unsigned long ulBase, unsigned long ulWordCount)
{
  unsigned long ulRegVal;

  //
  // Read the current configuration
  //
  ulRegVal = HWREG(ulBase + MCSPI_O_XFERLEVEL);

  //
  // Mask and set the word count
  //
  HWREG(ulBase + MCSPI_O_XFERLEVEL) = ((ulRegVal & 0x0000FFFF)|
                                      (ulWordCount & 0xFFFF) << 16);
}

//*****************************************************************************
//
//! Registers an interrupt handler for a SPI interrupt.
//!
//! \param ulBase is the base address of the SPI module
//! \param pfnHandler is a pointer to the function to be called when the
//! SPI interrupt occurs.
//!
//! This function does the actual registering of the interrupt handler.  This
//! function enables the global interrupt in the interrupt controller; specific
//! SPI interrupts must be enabled via SPIIntEnable(). It is the interrupt
//! handler's responsibility to clear the interrupt source.
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \return None.
//
//*****************************************************************************
void
SPIIntRegister(unsigned long ulBase, void(*pfnHandler)(void))
{
    unsigned long ulInt;

    //
    // Determine the interrupt number based on the SPI module
    //
    ulInt = SPIIntNumberGet(ulBase);

    //
    // Register the interrupt handler.
    //
    IntRegister(ulInt, pfnHandler);

    //
    // Enable the SPI interrupt.
    //
    IntEnable(ulInt);
}

//*****************************************************************************
//
//! Unregisters an interrupt handler for a SPI interrupt.
//!
//! \param ulBase is the base address of the SPI module
//!
//! This function does the actual unregistering of the interrupt handler.  It
//! clears the handler to be called when a SPI interrupt occurs.  This
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
SPIIntUnregister(unsigned long ulBase)
{
    unsigned long ulInt;

    //
    // Determine the interrupt number based on the SPI module
    //
    ulInt = SPIIntNumberGet(ulBase);

    //
    // Disable the interrupt.
    //
    IntDisable(ulInt);

    //
    // Unregister the interrupt handler.
    //
    IntUnregister(ulInt);
}

//*****************************************************************************
//
//! Enables individual SPI interrupt sources.
//!
//! \param ulBase is the base address of the SPI module
//! \param ulIntFlags is the bit mask of the interrupt sources to be enabled.
//!
//! This function enables the indicated SPI interrupt sources.  Only the
//! sources that are enabled can be reflected to the processor interrupt;
//! disabled sources have no effect on the processor.
//!
//! The \e ulIntFlags parameter is the logical OR of any of the following:
//!
//! - \b SPI_INT_DMATX
//! - \b SPI_INT_DMARX
//! - \b SPI_INT_EOW
//! - \b SPI_INT_RX_OVRFLOW
//! - \b SPI_INT_RX_FULL
//! - \b SPI_INT_TX_UDRFLOW
//! - \b SPI_INT_TX_EMPTY
//!
//! \return None.
//
//*****************************************************************************
void
SPIIntEnable(unsigned long ulBase, unsigned long ulIntFlags)
{
  unsigned long ulDmaMsk;

  //
  // Enable DMA  Tx Interrupt
  //
  if(ulIntFlags & SPI_INT_DMATX)
  {
      ulDmaMsk = SPIDmaMaskGet(ulBase);
      HWREG(APPS_CONFIG_BASE + APPS_CONFIG_O_DMA_DONE_INT_MASK_CLR) = ulDmaMsk;
  }

  //
  // Enable DMA  Rx Interrupt
  //
  if(ulIntFlags & SPI_INT_DMARX)
  {
      ulDmaMsk = (SPIDmaMaskGet(ulBase) >> 1);
      HWREG(APPS_CONFIG_BASE + APPS_CONFIG_O_DMA_DONE_INT_MASK_CLR) = ulDmaMsk;
  }

  //
  // Enable the specific Interrupts
  //
  HWREG(ulBase + MCSPI_O_IRQENABLE) |= (ulIntFlags & 0x0003000F);
}


//*****************************************************************************
//
//! Disables individual SPI interrupt sources.
//!
//! \param ulBase is the base address of the SPI module
//! \param ulIntFlags is the bit mask of the interrupt sources to be disabled.
//!
//! This function disables the indicated SPI interrupt sources.  Only the
//! sources that are enabled can be reflected to the processor interrupt;
//! disabled sources have no effect on the processor.
//!
//! The \e ulIntFlags parameter has the same definition as the \e ulIntFlags
//! parameter to SPIIntEnable().
//!
//! \return None.
//
//*****************************************************************************
void
SPIIntDisable(unsigned long ulBase, unsigned long ulIntFlags)
{
  unsigned long ulDmaMsk;

  //
  // Disable DMA  Tx Interrupt
  //
  if(ulIntFlags & SPI_INT_DMATX)
  {
      ulDmaMsk = SPIDmaMaskGet(ulBase);
      HWREG(APPS_CONFIG_BASE + APPS_CONFIG_O_DMA_DONE_INT_MASK_SET) = ulDmaMsk;
  }

  //
  // Disable DMA  Tx Interrupt
  //
  if(ulIntFlags & SPI_INT_DMARX)
  {
      ulDmaMsk = (SPIDmaMaskGet(ulBase) >> 1);
      HWREG(APPS_CONFIG_BASE + APPS_CONFIG_O_DMA_DONE_INT_MASK_SET) = ulDmaMsk;
  }

  //
  // Disable the specific Interrupts
  //
  HWREG(ulBase + MCSPI_O_IRQENABLE) &= ~(ulIntFlags & 0x0003000F);
}

//*****************************************************************************
//
//! Gets the current interrupt status.
//!
//! \param ulBase is the base address of the SPI module
//! \param bMasked is \b false if the raw interrupt status is required and
//! \b true if the masked interrupt status is required.
//!
//! This function returns the interrupt status for the specified SPI.
//! The status of interrupts that are allowed to reflect to the processor can
//! be returned.
//!
//! \return Returns the current interrupt status, enumerated as a bit field of
//! values described in SPIIntEnable().
//
//*****************************************************************************
unsigned long
SPIIntStatus(unsigned long ulBase, tBoolean bMasked)
{
  unsigned long ulIntStat;
  unsigned long ulIntFlag;
  unsigned long ulDmaMsk;

  //
  // Get SPI interrupt status
  //
  ulIntFlag = HWREG(ulBase + MCSPI_O_IRQSTATUS) & 0x0003000F;

  if(bMasked)
  {
     ulIntFlag &= HWREG(ulBase + MCSPI_O_IRQENABLE);
  }

  //
  // Get the interrupt bit
  //
  ulDmaMsk = SPIDmaMaskGet(ulBase);

  //
  // Get the DMA interrupt status
  //
  if(bMasked)
  {
    ulIntStat = HWREG(APPS_CONFIG_BASE + APPS_CONFIG_O_DMA_DONE_INT_STS_MASKED);
  }
  else
  {
    ulIntStat = HWREG(APPS_CONFIG_BASE + APPS_CONFIG_O_DMA_DONE_INT_STS_RAW);
  }

  //
  // Get SPI Tx DMA done status
  //
  if(ulIntStat & ulDmaMsk)
  {
     ulIntFlag |= SPI_INT_DMATX;
  }

  //
  // Get SPI Rx DMA done status
  //
  if(ulIntStat & (ulDmaMsk >> 1))
  {
     ulIntFlag |= SPI_INT_DMARX;
  }

  //
  // Return status
  //
  return(ulIntFlag);
}

//*****************************************************************************
//
//! Clears SPI interrupt sources.
//!
//! \param ulBase is the base address of the SPI module
//! \param ulIntFlags is a bit mask of the interrupt sources to be cleared.
//!
//! The specified SPI interrupt sources are cleared, so that they no longer
//! assert.  This function must be called in the interrupt handler to keep the
//! interrupt from being recognized again immediately upon exit.
//!
//! The \e ulIntFlags parameter has the same definition as the \e ulIntFlags
//! parameter to SPIIntEnable().
//!
//! \return None.
//
//*****************************************************************************
void
SPIIntClear(unsigned long ulBase, unsigned long ulIntFlags)
{
  unsigned long ulDmaMsk;

  //
  // Disable DMA  Tx Interrupt
  //
  if(ulIntFlags & SPI_INT_DMATX)
  {
      ulDmaMsk = SPIDmaMaskGet(ulBase);
      HWREG(APPS_CONFIG_BASE + APPS_CONFIG_O_DMA_DONE_INT_ACK) = ulDmaMsk;
  }

  //
  // Disable DMA  Tx Interrupt
  //
  if(ulIntFlags & SPI_INT_DMARX)
  {
      ulDmaMsk = (SPIDmaMaskGet(ulBase) >> 1);
      HWREG(APPS_CONFIG_BASE + APPS_CONFIG_O_DMA_DONE_INT_ACK) = ulDmaMsk;
  }

  //
  // Clear Interrupts
  //
  HWREG(ulBase + MCSPI_O_IRQSTATUS) = (ulIntFlags & 0x0003000F);
}

//*****************************************************************************
//
//! Enables the chip select in software controlled mode
//!
//! \param ulBase is the base address of the SPI module.
//!
//! This function enables the Chip select in software controlled mode. The
//! active state of CS will depend on the configuration done via
//! \sa SPIConfigExpClkSet().
//!
//! \return None.
//
//*****************************************************************************
void SPICSEnable(unsigned long ulBase)
{
  //
  // Set Chip Select enable bit.
  //
  HWREG( ulBase+MCSPI_O_CH0CONF) |= MCSPI_CH0CONF_FORCE;
}

//*****************************************************************************
//
//! Disables the chip select in software controlled mode
//!
//! \param ulBase is the base address of the SPI module.
//!
//! This function disables the Chip select in software controlled mode. The
//! active state of CS will depend on the configuration done via
//! sa SPIConfigSetExpClk().
//!
//! \return None.
//
//*****************************************************************************
void SPICSDisable(unsigned long ulBase)
{
  //
  // Reset Chip Select enable bit.
  //
  HWREG( ulBase+MCSPI_O_CH0CONF) &= ~MCSPI_CH0CONF_FORCE;
}

//*****************************************************************************
//
//! Send/Receive data buffer over SPI channel
//!
//! \param ulBase is the base address of SPI module
//! \param ucDout is the pointer to Tx data buffer or 0.
//! \param ucDin is pointer to Rx data buffer or 0
//! \param ulCount is the size of data in bytes.
//! \param ulFlags controlls chip select toggling.
//!
//! This function transfers \e ulCount bytes of data over SPI channel. Since
//! the API sends a SPI word at a time \e ulCount should be a multiple of
//! word length set using SPIConfigSetExpClk().
//!
//! If the \e ucDout parameter is set to 0, the function will send 0xFF over
//! the SPI MOSI line.
//!
//! If the \e ucDin parameter is set to 0, the function will ignore data on SPI
//! MISO line.
//!
//! The parameter \e ulFlags is logical OR of one or more of the following
//!
//! - \b SPI_CS_ENABLE if CS needs to be enabled at start of transfer.
//! - \b SPI_CS_DISABLE if CS need to be disabled at the end of transfer.
//!
//! This function will not return until data has been transmitted
//!
//! \return Returns 0 on success, -1 otherwise.
//
//*****************************************************************************
long SPITransfer(unsigned long ulBase, unsigned char *ucDout,
                   unsigned char *ucDin, unsigned long ulCount,
                   unsigned long ulFlags)
{
  unsigned long ulWordLength;
  long lRet;

  //
  // Get the word length
  //
  ulWordLength = (HWREG(ulBase + MCSPI_O_CH0CONF) & MCSPI_CH0CONF_WL_M);

  //
  // Check for word length.
  //
  if( !((ulWordLength == SPI_WL_8) || (ulWordLength == SPI_WL_16) ||
     (ulWordLength == SPI_WL_32)) )
  {
    return -1;
  }

  if( ulWordLength == SPI_WL_8 )
  {
    //
    // Do byte transfer
    //
    lRet = SPITransfer8(ulBase,ucDout,ucDin,ulCount,ulFlags);
  }
  else if( ulWordLength == SPI_WL_16 )
  {

    //
    // Do half-word transfer
    //
    lRet = SPITransfer16(ulBase,(unsigned short *)ucDout,
                  (unsigned short *)ucDin,ulCount,ulFlags);
  }
  else
  {
    //
    // Do word transfer
    //
    lRet = SPITransfer32(ulBase,(unsigned long *)ucDout,
                  (unsigned long *)ucDin,ulCount,ulFlags);
  }

  //
  // return
  //
  return lRet;

}
//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
