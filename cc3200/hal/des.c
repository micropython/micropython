//*****************************************************************************
//
//  des.c
//
//  Driver for the DES data transformation.
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
//! \addtogroup DES_Data_Encryption_Standard_api
//! @{
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_des.h"
#include "inc/hw_dthe.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "debug.h"
#include "des.h"
#include "interrupt.h"


//*****************************************************************************
//
//! Configures the DES module for operation.
//!
//! \param ui32Base is the base address of the DES module.
//! \param ui32Config is the configuration of the DES module.
//!
//! This function configures the DES module for operation.
//!
//! The \e ui32Config parameter is a bit-wise OR of a number of configuration
//! flags. The valid flags are grouped below based on their function.
//!
//! The direction of the operation is specified with one of the following two
//! flags.  Only one is permitted.
//!
//! - \b DES_CFG_DIR_ENCRYPT - Encryption
//! - \b DES_CFG_DIR_DECRYPT - Decryption
//!
//! The operational mode of the DES engine is specified with one of the
//! following flags. Only one is permitted.
//!
//! - \b DES_CFG_MODE_ECB - Electronic Codebook Mode
//! - \b DES_CFG_MODE_CBC - Cipher-Block Chaining Mode
//! - \b DES_CFG_MODE_CFB - Cipher Feedback Mode
//!
//! The selection of single DES or triple DES is specified with one of the
//! following two flags. Only one is permitted.
//!
//! - \b DES_CFG_SINGLE - Single DES
//! - \b DES_CFG_TRIPLE - Triple DES
//!
//! \return None.
//
//*****************************************************************************
void
DESConfigSet(uint32_t ui32Base, uint32_t ui32Config)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == DES_BASE);

    //
    // Backup the save context field.
    //
    ui32Config |= (HWREG(ui32Base + DES_O_CTRL) & DES_CTRL_CONTEXT);

    //
    // Write the control register.
    //
    HWREG(ui32Base + DES_O_CTRL) = ui32Config;
}

//*****************************************************************************
//
//! Sets the key used for DES operations.
//!
//! \param ui32Base is the base address of the DES module.
//! \param pui8Key is a pointer to an array that holds the key
//!
//! This function sets the key used for DES operations.
//!
//! \e pui8Key should be 64 bits long (2 words) if single DES is being used or
//! 192 bits (6 words) if triple DES is being used.
//!
//! \return None.
//
//*****************************************************************************
void
DESKeySet(uint32_t ui32Base, uint8_t *pui8Key)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == DES_BASE);

    //
    // Write the first part of the key.
    //
    HWREG(ui32Base + DES_O_KEY1_L) =  * ((uint32_t *)(pui8Key + 0));
    HWREG(ui32Base + DES_O_KEY1_H) = * ((uint32_t *)(pui8Key + 4));

    //
    // If we are performing triple DES, then write the key registers for
    // the second and third rounds.
    //
    if(HWREG(ui32Base + DES_O_CTRL) & DES_CFG_TRIPLE)
    {
        HWREG(ui32Base + DES_O_KEY2_L) = * ((uint32_t *)(pui8Key + 8));
        HWREG(ui32Base + DES_O_KEY2_H) = * ((uint32_t *)(pui8Key + 12));
        HWREG(ui32Base + DES_O_KEY3_L) = * ((uint32_t *)(pui8Key + 16));
        HWREG(ui32Base + DES_O_KEY3_H) = * ((uint32_t *)(pui8Key + 20));
    }
}

//*****************************************************************************
//
//! Sets the initialization vector in the DES module.
//!
//! \param ui32Base is the base address of the DES module.
//! \param pui8IVdata is a pointer to an array of 64 bits (2 words) of data to
//! be written into the initialization vectors registers.
//!
//! This function sets the initialization vector in the DES module.  It returns
//! true if the registers were successfully written.  If the context registers
//! cannot be written at the time the function was called, then false is
//! returned.
//!
//! \return True or false.
//
//*****************************************************************************
bool
DESIVSet(uint32_t ui32Base, uint8_t *pui8IVdata)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == DES_BASE);

    //
    // Check to see if context registers can be overwritten.  If not, return
    // false.
    //
    if((HWREG(ui32Base + DES_O_CTRL) & DES_CTRL_CONTEXT) == 0)
    {
        return(false);
    }

    //
    // Write the initialization vector registers.
    //
    HWREG(ui32Base + DES_O_IV_L) =  *((uint32_t *) (pui8IVdata + 0));
    HWREG(ui32Base + DES_O_IV_H) = *((uint32_t *) (pui8IVdata + 4));

    //
    // Return true to indicate the write was successful.
    //
    return(true);
}

//*****************************************************************************
//
//! Sets the crytographic data length in the DES module.
//!
//! \param ui32Base is the base address of the DES module.
//! \param ui32Length is the length of the data in bytes.
//!
//! This function writes the cryptographic data length into the DES module.
//! When this register is written, the engine is triggersed to start using
//! this context.
//!
//! \note Data lengths up to (2^32 - 1) bytes are allowed.
//!
//! \return None.
//
//*****************************************************************************
void
DESDataLengthSet(uint32_t ui32Base, uint32_t ui32Length)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == DES_BASE);

    //
    // Write the length register.
    //
    HWREG(ui32Base + DES_O_LENGTH) = ui32Length;
}

//*****************************************************************************
//
//! Reads plaintext/ciphertext from data registers without blocking
//!
//! \param ui32Base is the base address of the DES module.
//! \param pui8Dest is a pointer to an array of 2 words.
//! \param ui8Length the length can be from 1 to 8
//!
//! This function returns true if the data was ready when the function was
//! called. If the data was not ready, false is returned.
//!
//! \return True or false.
//
//*****************************************************************************
bool
DESDataReadNonBlocking(uint32_t ui32Base, uint8_t *pui8Dest, uint8_t ui8Length)
{
	volatile uint32_t pui32Dest[2];
	uint8_t ui8BytCnt;
	uint8_t *pui8DestTemp;

	//
    // Check the arguments.
    //
    ASSERT(ui32Base == DES_BASE);
    if((ui8Length == 0)||(ui8Length>8))
    {
       	return(false);
    }

    //
    // Check to see if the data is ready to be read.
    //
    if((DES_CTRL_OUTPUT_READY & (HWREG(ui32Base + DES_O_CTRL))) == 0)
    {
        return(false);
    }

    //
    // Read two words of data from the data registers.
    //
    pui32Dest[0] = HWREG(DES_BASE + DES_O_DATA_L);
    pui32Dest[1] = HWREG(DES_BASE + DES_O_DATA_H);

    //
    //Copy the data to a block memory
    //
    pui8DestTemp = (uint8_t *)pui32Dest;
    for(ui8BytCnt = 0; ui8BytCnt < ui8Length ; ui8BytCnt++)
    {
 		*(pui8Dest+ui8BytCnt) = *(pui8DestTemp+ui8BytCnt);
    }

    //
    // Return true to indicate a successful write.
    //
    return(true);
}

//*****************************************************************************
//
//! Reads plaintext/ciphertext from data registers with blocking.
//!
//! \param ui32Base is the base address of the DES module.
//! \param pui8Dest is a pointer to an array of bytes.
//! \param ui8Length the length can be from 1 to 8
//!
//! This function waits until the DES module is finished and encrypted or
//! decrypted data is ready.  The output data is then stored in the pui8Dest
//! array.
//!
//! \return None
//
//*****************************************************************************
void
DESDataRead(uint32_t ui32Base, uint8_t *pui8Dest, uint8_t ui8Length)
{
	volatile uint32_t pui32Dest[2];
	uint8_t ui8BytCnt;
	uint8_t *pui8DestTemp;

	//
	// Check the arguments.
	//
	ASSERT(ui32Base == DES_BASE);
	if((ui8Length == 0)||(ui8Length>8))
	{
		return;
	}
    //
    // Wait for data output to be ready.
    //
    while((HWREG(ui32Base + DES_O_CTRL) & DES_CTRL_OUTPUT_READY) == 0)
    {
    }

    //
    // Read two words of data from the data registers.
    //
    pui32Dest[0] = HWREG(DES_BASE + DES_O_DATA_L);
    pui32Dest[1] = HWREG(DES_BASE + DES_O_DATA_H);

    //
    //Copy the data to a block memory
    //
    pui8DestTemp = (uint8_t *)pui32Dest;
    for(ui8BytCnt = 0; ui8BytCnt < ui8Length ; ui8BytCnt++)
    {
    	*(pui8Dest+ui8BytCnt) = *(pui8DestTemp+ui8BytCnt);
    }
}

//*****************************************************************************
//
//! Writes plaintext/ciphertext to data registers without blocking
//!
//! \param ui32Base is the base address of the DES module.
//! \param pui8Src is a pointer to an array of 2 words.
//! \param ui8Length the length can be from 1 to 8
//!
//! This function returns false if the DES module is not ready to accept
//! data.  It returns true if the data was written successfully.
//!
//! \return true or false.
//
//*****************************************************************************
bool
DESDataWriteNonBlocking(uint32_t ui32Base, uint8_t *pui8Src, uint8_t ui8Length)
{

    volatile uint32_t pui32Src[2]={0,0};
    uint8_t ui8BytCnt;
    uint8_t *pui8SrcTemp;

    //
    // Check the arguments.
    //
    ASSERT(ui32Base == DES_BASE);

    if((ui8Length == 0)||(ui8Length>8))
    {
            return(false);
    }

    //
    // Check if the DES module is ready to encrypt or decrypt data.  If it
    // is not, return false.
    //
    if(!(DES_CTRL_INPUT_READY & (HWREG(ui32Base + DES_O_CTRL))))
    {
        return(false);
    }

    //
    // Copy the data to a block memory
    //
    pui8SrcTemp = (uint8_t *)pui32Src;
    for(ui8BytCnt = 0; ui8BytCnt < ui8Length ; ui8BytCnt++)
    {
            *(pui8SrcTemp+ui8BytCnt) = *(pui8Src+ui8BytCnt);
    }

    //
    // Write the data.
    //
    HWREG(DES_BASE + DES_O_DATA_L) = pui32Src[0];
    HWREG(DES_BASE + DES_O_DATA_H) = pui32Src[1];

    //
    // Return true to indicate a successful write.
    //
    return(true);
}

//*****************************************************************************
//
//! Writes plaintext/ciphertext to data registers without blocking
//!
//! \param ui32Base is the base address of the DES module.
//! \param pui8Src is a pointer to an array of bytes.
//! \param ui8Length the length can be from 1 to 8
//!
//! This function waits until the DES module is ready before writing the
//! data contained in the pui8Src array.
//!
//! \return None.
//
//*****************************************************************************
void
DESDataWrite(uint32_t ui32Base, uint8_t *pui8Src, uint8_t ui8Length)
{
    volatile uint32_t pui32Src[2]={0,0};
    uint8_t ui8BytCnt;
    uint8_t *pui8SrcTemp;

    //
    // Check the arguments.
    //
    ASSERT(ui32Base == DES_BASE);

    if((ui8Length == 0)||(ui8Length>8))
    {
            return;
    }

    //
    // Wait for the input ready bit to go high.
    //
    while(((HWREG(ui32Base + DES_O_CTRL) & DES_CTRL_INPUT_READY)) == 0)
    {
    }

    //
    //Copy the data to a block memory
    //
    pui8SrcTemp = (uint8_t *)pui32Src;
    for(ui8BytCnt = 0; ui8BytCnt < ui8Length ; ui8BytCnt++)
    {
            *(pui8SrcTemp+ui8BytCnt) = *(pui8Src+ui8BytCnt);
    }

    //
    // Write the data.
    //
    HWREG(DES_BASE + DES_O_DATA_L) = pui32Src[0];
    HWREG(DES_BASE + DES_O_DATA_H) = pui32Src[1];
}

//*****************************************************************************
//
//! Processes blocks of data through the DES module.
//!
//! \param ui32Base is the base address of the DES module.
//! \param pui8Src is a pointer to an array of words that contains the
//! source data for processing.
//! \param pui8Dest is a pointer to an array of words consisting of the
//! processed data.
//! \param ui32Length is the length of the cryptographic data in bytes.
//! It must be a multiple of eight.
//!
//! This function takes the data contained in the pui8Src array and processes
//! it using the DES engine.  The resulting data is stored in the
//! pui8Dest array.  The function blocks until all of the data has been
//! processed. If processing is successful, the function returns true.
//!
//! \note This functions assumes that the DES module has been configured,
//! and initialization values and keys have been written.
//!
//! \return true or false.
//
//*****************************************************************************
bool
DESDataProcess(uint32_t ui32Base, uint8_t *pui8Src, uint8_t *pui8Dest,
               uint32_t ui32Length)
{
    uint32_t ui32Count, ui32BlkCount, ui32ByteCount;

    //
    // Check the arguments.
    //
    ASSERT(ui32Base == DES_BASE);
    ASSERT((ui32Length % 8) == 0);

    //
    // Write the length register first. This triggers the engine to start
    // using this context.
    //
    HWREG(ui32Base + DES_O_LENGTH) = ui32Length;


    //
    // Now loop until the blocks are written.
    //
    ui32BlkCount = ui32Length/8;
    for(ui32Count = 0; ui32Count <ui32BlkCount; ui32Count ++)
    {
      //
      // Check if the input ready is fine
      //
      while((DES_CTRL_INPUT_READY & (HWREG(ui32Base + DES_O_CTRL))) == 0)
      {
      }

      //
      // Write the data registers.
      //
      DESDataWriteNonBlocking(ui32Base, pui8Src + ui32Count*8 ,8);

      //
      // Wait for the output ready
      //
      while((DES_CTRL_OUTPUT_READY & (HWREG(ui32Base + DES_O_CTRL))) == 0)
      {
      }

      //
      // Read the data registers.
      //
      DESDataReadNonBlocking(ui32Base, pui8Dest + ui32Count*8 ,8);
    }

    //
    //Now handle the residue bytes
    //
    ui32ByteCount = ui32Length%8;
    if(ui32ByteCount)
    {
      //
      // Check if the input ready is fine
      //
      while((DES_CTRL_INPUT_READY & (HWREG(ui32Base + DES_O_CTRL))) == 0)
      {
      }
      //
      // Write the data registers.
      //
      DESDataWriteNonBlocking(ui32Base, pui8Src + (ui32Count*ui32BlkCount) ,
                              ui32ByteCount);
      //
      // Wait for the output ready
      //
      while((DES_CTRL_OUTPUT_READY & (HWREG(ui32Base + DES_O_CTRL))) == 0)
      {
      }

      //
      // Read the data registers.
      //
      DESDataReadNonBlocking(ui32Base, pui8Dest + (ui32Count*ui32BlkCount) ,
                             ui32ByteCount);
    }



    //
    // Return true to indicate the process was successful.
    //
    return(true);
}

//*****************************************************************************
//
//! Returns the current interrupt status of the DES module.
//!
//! \param ui32Base is the base address of the DES module.
//! \param bMasked is \b false if the raw interrupt status is required and
//! \b true if the masked interrupt status is required.
//!
//! This function gets the current interrupt status of the DES module.
//! The value returned is a logical OR of the following values:
//!
//! - \b DES_INT_CONTEXT_IN - Context interrupt
//! - \b DES_INT_DATA_IN - Data input interrupt
//! - \b DES_INT_DATA_OUT_INT - Data output interrupt
//! - \b DES_INT_DMA_CONTEXT_IN - Context DMA done interrupt
//! - \b DES_INT_DMA_DATA_IN - Data input DMA done interrupt
//! - \b DES_INT_DMA_DATA_OUT - Data output DMA done interrupt
//!
//! \return A bit mask of the current interrupt status.
//
//*****************************************************************************
uint32_t
DESIntStatus(uint32_t ui32Base, bool bMasked)
{
   uint32_t ui32IntStatus;
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == DES_BASE);

    //
    // Read the status register and return the value.
    //
    if(bMasked)
    {
        ui32IntStatus = HWREG(ui32Base + DES_O_IRQSTATUS);
        ui32IntStatus &= HWREG(ui32Base + DES_O_IRQENABLE);
        ui32IntStatus |= ((HWREG(DTHE_BASE + DTHE_O_DES_MIS) & 0x7) << 16);

        return(ui32IntStatus);
    }
    else
    {
        ui32IntStatus = HWREG(ui32Base + DES_O_IRQSTATUS);
        ui32IntStatus |= ((HWREG(DTHE_BASE + DTHE_O_DES_MIS) & 0xD) << 16);
        return(ui32IntStatus);
    }
}

//*****************************************************************************
//
//! Enables interrupts in the DES module.
//!
//! \param ui32Base is the base address of the DES module.
//! \param ui32IntFlags is a bit mask of the interrupts to be enabled.
//!
//! \e ui32IntFlags should be a logical OR of one or more of the following
//! values:
//!
//! - \b DES_INT_CONTEXT_IN - Context interrupt
//! - \b DES_INT_DATA_IN - Data input interrupt
//! - \b DES_INT_DATA_OUT - Data output interrupt
//! - \b DES_INT_DMA_CONTEXT_IN - Context DMA done interrupt
//! - \b DES_INT_DMA_DATA_IN - Data input DMA done interrupt
//! - \b DES_INT_DMA_DATA_OUT - Data output DMA done interrupt
//!
//! \return None.
//
//*****************************************************************************
void
DESIntEnable(uint32_t ui32Base, uint32_t ui32IntFlags)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == DES_BASE);
    ASSERT((ui32IntFlags & DES_INT_CONTEXT_IN) ||
           (ui32IntFlags & DES_INT_DATA_IN) ||
           (ui32IntFlags & DES_INT_DATA_OUT) ||
           (ui32IntFlags & DES_INT_DMA_CONTEXT_IN) ||
           (ui32IntFlags & DES_INT_DMA_DATA_IN) ||
           (ui32IntFlags & DES_INT_DMA_DATA_OUT));

    //
    // Enable the interrupts from the flags.
    //
    HWREG(DTHE_BASE + DTHE_O_DES_IM)  &= ~((ui32IntFlags & 0x00070000) >> 16);
    HWREG(ui32Base + DES_O_IRQENABLE) |= ui32IntFlags & 0x0000ffff;
}

//*****************************************************************************
//
//! Disables interrupts in the DES module.
//!
//! \param ui32Base is the base address of the DES module.
//! \param ui32IntFlags is a bit mask of the interrupts to be disabled.
//!
//! This function disables interrupt sources in the DES module.
//! \e ui32IntFlags should be a logical OR of one or more of the following
//! values:
//!
//! - \b DES_INT_CONTEXT_IN - Context interrupt
//! - \b DES_INT_DATA_IN - Data input interrupt
//! - \b DES_INT_DATA_OUT - Data output interrupt
//! - \b DES_INT_DMA_CONTEXT_IN - Context DMA done interrupt
//! - \b DES_INT_DMA_DATA_IN - Data input DMA done interrupt
//! - \b DES_INT_DMA_DATA_OUT - Data output DMA done interrupt
//!
//! \return None.
//
//*****************************************************************************
void
DESIntDisable(uint32_t ui32Base, uint32_t ui32IntFlags)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == DES_BASE);
    ASSERT((ui32IntFlags & DES_INT_CONTEXT_IN) ||
           (ui32IntFlags & DES_INT_DATA_IN) ||
           (ui32IntFlags & DES_INT_DATA_OUT) ||
           (ui32IntFlags & DES_INT_DMA_CONTEXT_IN) ||
           (ui32IntFlags & DES_INT_DMA_DATA_IN) ||
           (ui32IntFlags & DES_INT_DMA_DATA_OUT));

    //
    // Clear the interrupts from the flags.
    //
    HWREG(DTHE_BASE + DTHE_O_AES_IM)  |= ((ui32IntFlags & 0x00070000) >> 16);
    HWREG(ui32Base + DES_O_IRQENABLE) &= ~(ui32IntFlags & 0x0000ffff);
}

//*****************************************************************************
//
//! Clears interrupts in the DES module.
//!
//! \param ui32Base is the base address of the DES module.
//! \param ui32IntFlags is a bit mask of the interrupts to be disabled.
//!
//! This function disables interrupt sources in the DES module.
//! \e ui32IntFlags should be a logical OR of one or more of the following
//! values:
//!
//! - \b DES_INT_DMA_CONTEXT_IN - Context interrupt
//! - \b DES_INT_DMA_DATA_IN - Data input interrupt
//! - \b DES_INT_DMA_DATA_OUT - Data output interrupt
//!
//! \note The DMA done interrupts are the only interrupts that can be cleared.
//! The remaining interrupts can be disabled instead using DESIntDisable().
//!
//! \return None.
//
//*****************************************************************************
void
DESIntClear(uint32_t ui32Base, uint32_t ui32IntFlags)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == DES_BASE);
    ASSERT((ui32IntFlags & DES_INT_DMA_CONTEXT_IN) ||
           (ui32IntFlags & DES_INT_DMA_DATA_IN) ||
           (ui32IntFlags & DES_INT_DMA_DATA_OUT));

    HWREG(DTHE_BASE + DTHE_O_DES_IC) = ((ui32IntFlags & 0x00070000) >> 16);
}

//*****************************************************************************
//
//! Registers an interrupt handler for the DES module.
//!
//! \param ui32Base is the base address of the DES module.
//! \param pfnHandler is a pointer to the function to be called when the
//! enabled DES interrupts occur.
//!
//! This function registers the interrupt handler in the interrupt vector
//! table, and enables DES interrupts on the interrupt controller; specific DES
//! interrupt sources must be enabled using DESIntEnable().  The interrupt
//! handler being registered must clear the source of the interrupt using
//! DESIntClear().
//!
//! If the application is using a static interrupt vector table stored in
//! flash, then it is not necessary to register the interrupt handler this way.
//! Instead, IntEnable() should be used to enable DES interrupts on the
//! interrupt controller.
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \return None.
//
//*****************************************************************************
void
DESIntRegister(uint32_t ui32Base, void(*pfnHandler)(void))
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == DES_BASE);

    //
    // Register the interrupt handler.
    //
    IntRegister(INT_DES, pfnHandler);

    //
    // Enable the interrupt.
    //
    IntEnable(INT_DES);
}

//*****************************************************************************
//
//! Unregisters an interrupt handler for the DES module.
//!
//! \param ui32Base is the base address of the DES module.
//!
//! This function unregisters the previously registered interrupt handler and
//! disables the interrupt in the interrupt controller.
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \return None.
//
//*****************************************************************************
void
DESIntUnregister(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == DES_BASE);

    //
    // Disable the interrupt.
    //
    IntDisable(INT_DES);

    //
    // Unregister the interrupt handler.
    //
    IntUnregister(INT_DES);
}

//*****************************************************************************
//
//! Enables DMA request sources in the DES module.
//!
//! \param ui32Base is the base address of the DES module.
//! \param ui32Flags is a bit mask of the DMA requests to be enabled.
//!
//! This function enables DMA request sources in the DES module.  The
//! \e ui32Flags parameter should be the logical OR of any of the following:
//!
//! - \b DES_DMA_CONTEXT_IN - Context In
//! - \b DES_DMA_DATA_OUT - Data Out
//! - \b DES_DMA_DATA_IN - Data In
//!
//! \return None.
//
//*****************************************************************************
void
DESDMAEnable(uint32_t ui32Base, uint32_t ui32Flags)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == DES_BASE);
    ASSERT((ui32Flags & DES_DMA_CONTEXT_IN) ||
           (ui32Flags & DES_DMA_DATA_OUT) ||
           (ui32Flags & DES_DMA_DATA_IN));

    //
    // Set the data in and data out DMA request enable bits.
    //
    HWREG(ui32Base + DES_O_SYSCONFIG) |= ui32Flags;
}

//*****************************************************************************
//
//! Disables DMA request sources in the DES module.
//!
//! \param ui32Base is the base address of the DES module.
//! \param ui32Flags is a bit mask of the DMA requests to be disabled.
//!
//! This function disables DMA request sources in the DES module.  The
//! \e ui32Flags parameter should be the logical OR of any of the following:
//!
//! - \b DES_DMA_CONTEXT_IN - Context In
//! - \b DES_DMA_DATA_OUT - Data Out
//! - \b DES_DMA_DATA_IN - Data In
//!
//! \return None.
//
//*****************************************************************************
void
DESDMADisable(uint32_t ui32Base, uint32_t ui32Flags)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == DES_BASE);
    ASSERT((ui32Flags & DES_DMA_CONTEXT_IN) ||
           (ui32Flags & DES_DMA_DATA_OUT) ||
           (ui32Flags & DES_DMA_DATA_IN));

    //
    // Disable the DMA sources.
    //
    HWREG(ui32Base + DES_O_SYSCONFIG) &= ~ui32Flags;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
