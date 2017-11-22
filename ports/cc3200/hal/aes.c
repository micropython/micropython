//*****************************************************************************
//
//  aes.c
//
//  Driver for the AES module.
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
//! \addtogroup AES_Advanced_Encryption_Standard_api
//! @{
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_aes.h"
#include "inc/hw_dthe.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "aes.h"
#include "debug.h"
#include "interrupt.h"

#define AES_BLOCK_SIZE_IN_BYTES 16

//*****************************************************************************
//
//! Configures the AES module.
//!
//! \param ui32Base is the base address of the AES module.
//! \param ui32Config is the configuration of the AES module.
//!
//! This function configures the AES module based on the specified parameters.
//! It does not change any DMA- or interrupt-related parameters.
//!
//! The ui32Config parameter is a bit-wise OR of a number of configuration
//! flags. The valid flags are grouped based on their function.
//!
//! The direction of the operation is specified with only of following flags:
//!
//! - \b AES_CFG_DIR_ENCRYPT - Encryption mode
//! - \b AES_CFG_DIR_DECRYPT - Decryption mode
//!
//! The key size is specified with only one of the following flags:
//!
//! - \b AES_CFG_KEY_SIZE_128BIT - Key size of 128 bits
//! - \b AES_CFG_KEY_SIZE_192BIT - Key size of 192 bits
//! - \b AES_CFG_KEY_SIZE_256BIT - Key size of 256 bits
//!
//! The mode of operation is specified with only one of the following flags.
//!
//! - \b AES_CFG_MODE_ECB - Electronic codebook mode
//! - \b AES_CFG_MODE_CBC - Cipher-block chaining mode
//! - \b AES_CFG_MODE_CFB - Cipher feedback mode
//! - \b AES_CFG_MODE_CTR - Counter mode
//! - \b AES_CFG_MODE_ICM - Integer counter mode
//! - \b AES_CFG_MODE_XTS - Ciphertext stealing mode
//! - \b AES_CFG_MODE_XTS_TWEAKJL - XEX-based tweaked-codebook mode with
//! ciphertext stealing with previous/intermediate tweak value and j loaded
//! - \b AES_CFG_MODE_XTS_K2IJL - XEX-based tweaked-codebook mode with
//! ciphertext stealing with key2, i and j loaded
//! - \b AES_CFG_MODE_XTS_K2ILJ0 - XEX-based tweaked-codebook mode with
//! ciphertext stealing with key2 and i loaded, j = 0
//! - \b AES_CFG_MODE_F8 - F8 mode
//! - \b AES_CFG_MODE_F9 - F9 mode
//! - \b AES_CFG_MODE_CBCMAC - Cipher block chaining message authentication
//! code mode
//! - \b AES_CFG_MODE_GCM - Galois/counter mode
//! - \b AES_CFG_MODE_GCM_HLY0ZERO - Galois/counter mode with GHASH with H
//! loaded and Y0-encrypted forced to zero
//! - \b AES_CFG_MODE_GCM_HLY0CALC - Galois/counter mode with GHASH with H
//! loaded and Y0-encrypted calculated internally
//! - \b AES_CFG_MODE_GCM_HY0CALC - Galois/Counter mode with autonomous GHASH
//! (both H and Y0-encrypted calculated internally)
//! - \b AES_CFG_MODE_CCM - Counter with CBC-MAC mode
//!
//! The following defines are used to specify the counter width.  It is only
//! required to be defined when using CTR, CCM, or GCM modes, only one of the
//! following defines must be used to specify the counter width length:
//!
//! - \b AES_CFG_CTR_WIDTH_32 - Counter is 32 bits
//! - \b AES_CFG_CTR_WIDTH_64 - Counter is 64 bits
//! - \b AES_CFG_CTR_WIDTH_96 - Counter is 96 bits
//! - \b AES_CFG_CTR_WIDTH_128 - Counter is 128 bits
//!
//! Only one of the following defines must be used to specify the length field
//! for CCM operations (L):
//!
//! - \b AES_CFG_CCM_L_2 - 2 bytes
//! - \b AES_CFG_CCM_L_4 - 4 bytes
//! - \b AES_CFG_CCM_L_8 - 8 bytes
//!
//! Only one of the following defines must be used to specify the length of the
//! authentication field for CCM operations (M) through the \e ui32Config
//! argument in the AESConfigSet() function:
//!
//! - \b AES_CFG_CCM_M_4 - 4 bytes
//! - \b AES_CFG_CCM_M_6 - 6 bytes
//! - \b AES_CFG_CCM_M_8 - 8 bytes
//! - \b AES_CFG_CCM_M_10 - 10 bytes
//! - \b AES_CFG_CCM_M_12 - 12 bytes
//! - \b AES_CFG_CCM_M_14 - 14 bytes
//! - \b AES_CFG_CCM_M_16 - 16 bytes
//!
//! \return None.
//
//*****************************************************************************
void
AESConfigSet(uint32_t ui32Base, uint32_t ui32Config)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);
    ASSERT((ui32Config & AES_CFG_DIR_ENCRYPT) ||
           (ui32Config & AES_CFG_DIR_DECRYPT));
    ASSERT((ui32Config & AES_CFG_KEY_SIZE_128BIT) ||
           (ui32Config & AES_CFG_KEY_SIZE_192BIT) ||
           (ui32Config & AES_CFG_KEY_SIZE_256BIT));
    ASSERT((ui32Config & AES_CFG_MODE_ECB) ||
           (ui32Config & AES_CFG_MODE_CBC) ||
           (ui32Config & AES_CFG_MODE_CTR) ||
           (ui32Config & AES_CFG_MODE_ICM) ||
           (ui32Config & AES_CFG_MODE_CFB) ||
           (ui32Config & AES_CFG_MODE_XTS_TWEAKJL) ||
           (ui32Config & AES_CFG_MODE_XTS_K2IJL) ||
           (ui32Config & AES_CFG_MODE_XTS_K2ILJ0) ||
           (ui32Config & AES_CFG_MODE_F8) ||
           (ui32Config & AES_CFG_MODE_F9) ||
           (ui32Config & AES_CFG_MODE_CTR) ||
           (ui32Config & AES_CFG_MODE_CBCMAC) ||
           (ui32Config & AES_CFG_MODE_GCM_HLY0ZERO) ||
           (ui32Config & AES_CFG_MODE_GCM_HLY0CALC) ||
           (ui32Config & AES_CFG_MODE_GCM_HY0CALC) ||
           (ui32Config & AES_CFG_MODE_CCM));
    ASSERT(((ui32Config & AES_CFG_MODE_CTR) ||
            (ui32Config & AES_CFG_MODE_GCM_HLY0ZERO) ||
            (ui32Config & AES_CFG_MODE_GCM_HLY0CALC) ||
            (ui32Config & AES_CFG_MODE_GCM_HY0CALC) ||
            (ui32Config & AES_CFG_MODE_CCM)) &&
           ((ui32Config & AES_CFG_CTR_WIDTH_32) ||
            (ui32Config & AES_CFG_CTR_WIDTH_64) ||
            (ui32Config & AES_CFG_CTR_WIDTH_96) ||
            (ui32Config & AES_CFG_CTR_WIDTH_128)));
    ASSERT((ui32Config & AES_CFG_MODE_CCM) &&
           ((ui32Config & AES_CFG_CCM_L_2) ||
            (ui32Config & AES_CFG_CCM_L_4) ||
            (ui32Config & AES_CFG_CCM_L_8)) &&
           ((ui32Config & AES_CFG_CCM_M_4) ||
            (ui32Config & AES_CFG_CCM_M_6) ||
            (ui32Config & AES_CFG_CCM_M_8) ||
            (ui32Config & AES_CFG_CCM_M_10) ||
            (ui32Config & AES_CFG_CCM_M_12) ||
            (ui32Config & AES_CFG_CCM_M_14) ||
            (ui32Config & AES_CFG_CCM_M_16)));

    //
    // Backup the save context field before updating the register.
    //
    if(HWREG(ui32Base + AES_O_CTRL) & AES_CTRL_SAVE_CONTEXT)
    {
        ui32Config |= AES_CTRL_SAVE_CONTEXT;
    }

    //
    // Write the CTRL register with the new value
    //
    HWREG(ui32Base + AES_O_CTRL) = ui32Config;
}

//*****************************************************************************
//
//! Writes the key 1 configuration registers, which are used for encryption or
//! decryption.
//!
//! \param ui32Base is the base address for the AES module.
//! \param pui8Key is an array of bytes, containing the key to be
//! configured.  The least significant word in the 0th index.
//! \param ui32Keysize is the size of the key, which must be one of the
//! following values:  \b AES_CFG_KEY_SIZE_128, \b AES_CFG_KEY_SIZE_192, or
//! \b AES_CFG_KEY_SIZE_256.
//!
//! This function writes key 1 configuration registers based on the key
//! size.  This function is used in all modes.
//!
//! \return None.
//
//*****************************************************************************
void
AESKey1Set(uint32_t ui32Base, uint8_t *pui8Key, uint32_t ui32Keysize)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);
    ASSERT((ui32Keysize == AES_CFG_KEY_SIZE_128BIT) ||
           (ui32Keysize == AES_CFG_KEY_SIZE_192BIT) ||
           (ui32Keysize == AES_CFG_KEY_SIZE_256BIT));

    //
    // With all key sizes, the first 4 words are written.
    //
    HWREG(ui32Base + AES_O_KEY1_0) = * ((uint32_t *)(pui8Key + 0));
    HWREG(ui32Base + AES_O_KEY1_1) = * ((uint32_t *)(pui8Key + 4));
    HWREG(ui32Base + AES_O_KEY1_2) = * ((uint32_t *)(pui8Key + 8));
    HWREG(ui32Base + AES_O_KEY1_3) = * ((uint32_t *)(pui8Key + 12));

    //
    // The key is 192 or 256 bits.  Write the next 2 words.
    //
    if(ui32Keysize != AES_CFG_KEY_SIZE_128BIT)
    {
        HWREG(ui32Base + AES_O_KEY1_4) = * ((uint32_t *)(pui8Key + 16));
        HWREG(ui32Base + AES_O_KEY1_5) = * ((uint32_t *)(pui8Key + 20));
    }

    //
    // The key is 256 bits.  Write the last 2 words.
    //
    if(ui32Keysize == AES_CFG_KEY_SIZE_256BIT)
    {
        HWREG(ui32Base + AES_O_KEY1_6) = * ((uint32_t *)(pui8Key + 24));
        HWREG(ui32Base + AES_O_KEY1_7) = * ((uint32_t *)(pui8Key + 28));
    }
}

//*****************************************************************************
//
//! Writes the key 2 configuration registers, which are used for encryption or
//! decryption.
//!
//! \param ui32Base is the base address for the AES module.
//! \param pui8Key is an array of bytes, containing the key to be
//! configured.  The least significant word in the 0th index.
//! \param ui32Keysize is the size of the key, which must be one of the
//! following values:  \b AES_CFG_KEY_SIZE_128, \b AES_CFG_KEY_SIZE_192, or
//! \b AES_CFG_KEY_SIZE_256.
//!
//! This function writes the key 2 configuration registers based on the key
//! size.  This function is used in the F8, F9, XTS, CCM, and CBC-MAC modes.
//!
//! \return None.
//
//*****************************************************************************
void
AESKey2Set(uint32_t ui32Base, uint8_t *pui8Key, uint32_t ui32Keysize)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);
    ASSERT((ui32Keysize == AES_CFG_KEY_SIZE_128BIT) ||
           (ui32Keysize == AES_CFG_KEY_SIZE_192BIT) ||
           (ui32Keysize == AES_CFG_KEY_SIZE_256BIT));

    //
    // With all key sizes, the first 4 words are written.
    //
    HWREG(ui32Base + AES_O_KEY2_0) = * ((uint32_t *)(pui8Key + 0));
    HWREG(ui32Base + AES_O_KEY2_1) = * ((uint32_t *)(pui8Key + 4));
    HWREG(ui32Base + AES_O_KEY2_2) = * ((uint32_t *)(pui8Key + 8));
    HWREG(ui32Base + AES_O_KEY2_3) = * ((uint32_t *)(pui8Key + 12));

    //
    // The key is 192 or 256 bits.  Write the next 2 words.
    //
    if(ui32Keysize != AES_CFG_KEY_SIZE_128BIT)
    {
        HWREG(ui32Base + AES_O_KEY2_4) = * ((uint32_t *)(pui8Key + 16));
        HWREG(ui32Base + AES_O_KEY2_5) = * ((uint32_t *)(pui8Key + 20));
    }

    //
    // The key is 256 bits.  Write the last 2 words.
    //
    if(ui32Keysize == AES_CFG_KEY_SIZE_256BIT)
    {
        HWREG(ui32Base + AES_O_KEY2_6) = * ((uint32_t *)(pui8Key + 24));
        HWREG(ui32Base + AES_O_KEY2_7) = * ((uint32_t *)(pui8Key + 28));
    }
}

//*****************************************************************************
//
//! Writes key 3 configuration registers, which are used for encryption or
//! decryption.
//!
//! \param ui32Base is the base address for the AES module.
//! \param pui8Key is a pointer to an array bytes, containing
//! the key to be configured. The least significant word is in the 0th index.
//!
//! This function writes the key 2 configuration registers with key 3 data
//! used in CBC-MAC and F8 modes.  This key is always 128 bits.
//!
//! \return None.
//
//*****************************************************************************
void
AESKey3Set(uint32_t ui32Base, uint8_t *pui8Key)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);

    //
    // Write the key into the upper 4 key registers
    //
    HWREG(ui32Base + AES_O_KEY2_4) = * ((uint32_t *)(pui8Key + 0));
    HWREG(ui32Base + AES_O_KEY2_5) = * ((uint32_t *)(pui8Key + 4));
    HWREG(ui32Base + AES_O_KEY2_6) = * ((uint32_t *)(pui8Key + 8));
    HWREG(ui32Base + AES_O_KEY2_7) = * ((uint32_t *)(pui8Key + 12));
}

//*****************************************************************************
//
//! Writes the Initial Vector (IV) register, needed in some of the AES Modes.
//!
//! \param ui32Base is the base address of the AES module.
//! \param pui8IVdata is an array of 16 bytes (128 bits), containing the IV
//! value to be configured. The least significant word is in the 0th index.
//!
//! This functions writes the initial vector registers in the AES module.
//!
//! \return None.
//
//*****************************************************************************
void
AESIVSet(uint32_t ui32Base, uint8_t *pui8IVdata)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);

    //
    // Write the initial vector registers.
    //
    HWREG(ui32Base + AES_O_IV_IN_0) = *((uint32_t *)(pui8IVdata+0));
    HWREG(ui32Base + AES_O_IV_IN_1) = *((uint32_t *)(pui8IVdata+4));
    HWREG(ui32Base + AES_O_IV_IN_2) = *((uint32_t *)(pui8IVdata+8));
    HWREG(ui32Base + AES_O_IV_IN_3) = *((uint32_t *)(pui8IVdata+12));
}


//*****************************************************************************
//
//! Reads the Initial Vector (IV) register, needed in some of the AES Modes.
//!
//! \param ui32Base is the base address of the AES module.
//! \param pui8IVdata is pointer to an array of 16 bytes.
//!
//! This functions reads the initial vector registers in the AES module.
//!
//! \return None.
//
//*****************************************************************************
void
AESIVGet(uint32_t ui32Base, uint8_t *pui8IVdata)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);

    //
    // Write the initial vector registers.
    //
    *((uint32_t *)(pui8IVdata+ 0)) = HWREG(ui32Base + AES_O_IV_IN_0);
    *((uint32_t *)(pui8IVdata+ 4)) = HWREG(ui32Base + AES_O_IV_IN_1);
    *((uint32_t *)(pui8IVdata+ 8)) = HWREG(ui32Base + AES_O_IV_IN_2);
    *((uint32_t *)(pui8IVdata+12)) = HWREG(ui32Base + AES_O_IV_IN_3);
}

//*****************************************************************************
//
//! Saves the tag registers to a user-defined location.
//!
//! \param ui32Base is the base address of the AES module.
//! \param pui8TagData is pointer to the location that stores the tag data.
//!
//! This function stores the tag data for use authenticated encryption and
//! decryption operations.
//!
//! \return None.
//
//*****************************************************************************
void
AESTagRead(uint32_t ui32Base, uint8_t *pui8TagData)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);

    //
    // Read the tag data.
    //
    *((uint32_t *)(pui8TagData+0)) = HWREG((ui32Base + AES_O_TAG_OUT_0));
    *((uint32_t *)(pui8TagData+4)) = HWREG((ui32Base + AES_O_TAG_OUT_1));
    *((uint32_t *)(pui8TagData+8)) = HWREG((ui32Base + AES_O_TAG_OUT_2));
    *((uint32_t *)(pui8TagData+12)) = HWREG((ui32Base + AES_O_TAG_OUT_3));
}

//*****************************************************************************
//
//! Used to set the write crypto data length in the AES module.
//!
//! \param ui32Base is the base address of the AES module.
//! \param ui64Length is the crypto data length in bytes.
//!
//! This function stores the cryptographic data length in blocks for all modes.
//! Data lengths up to (2^61 - 1) bytes are allowed.  For GCM, any value up
//! to (2^36 - 2) bytes are allowed because a 32-bit block counter is used.  For
//! basic modes (ECB/CBC/CTR/ICM/CFB128), zero can be programmed into the
//! length field, indicating that the length is infinite.
//!
//! When this function is called, the engine is triggered to start using
//! this context.
//!
//! \note This length does not include the authentication-only data used in
//! some modes.  Use the AESAuthLengthSet() function to specify the
//! authentication data length.
//!
//! \return None
//
//*****************************************************************************
void
AESDataLengthSet(uint32_t ui32Base, uint64_t ui64Length)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);

    //
    // Write the length register by shifting the 64-bit ui64Length.
    //
    HWREG(ui32Base + AES_O_C_LENGTH_0) = (uint32_t)(ui64Length);
    HWREG(ui32Base + AES_O_C_LENGTH_1) = (uint32_t)(ui64Length >> 32);
}

//*****************************************************************************
//
//! Sets the optional additional authentication data (AAD) length.
//!
//! \param ui32Base is the base address of the AES module.
//! \param ui32Length is the length in bytes.
//!
//! This function is only used to write the authentication data length in the
//! combined modes (GCM or CCM) and XTS mode.  Supported AAD lengths for CCM
//! are from 0 to (2^16 - 28) bytes.  For GCM, any value up to (2^32 - 1) can
//! be used.  For XTS mode, this register is used to load j.  Loading of j is
//! only required if j != 0.  j represents the sequential number of the 128-bit
//! blocks inside the data unit.  Consequently, j must be multiplied by 16
//! when passed to this function, thereby placing the block number in
//! bits [31:4] of the register.
//!
//! When this function is called, the engine is triggered to start using
//! this context for GCM and CCM.
//!
//! \return None
//
//*****************************************************************************
void
AESAuthDataLengthSet(uint32_t ui32Base, uint32_t ui32Length)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);

    //
    // Write the length into the register.
    //
    HWREG(ui32Base + AES_O_AUTH_LENGTH) = ui32Length;
}

//*****************************************************************************
//
//! Reads plaintext/ciphertext from data registers without blocking.
//! This api writes data in blocks
//!
//! \param ui32Base is the base address of the AES module.
//! \param pui8Dest is a pointer to an array of words of data.
//! \param ui8Length the length can be from 1 to 16
//!
//! This function reads a block of either plaintext or ciphertext out of the
//! AES module.  If the output data is not ready, the function returns
//! false.  If the read completed successfully, the function returns true.
//! A block is 16 bytes or 4 words.
//!
//! \return true or false.
//
//*****************************************************************************
bool
AESDataReadNonBlocking(uint32_t ui32Base, uint8_t *pui8Dest, uint8_t ui8Length)
{
	volatile uint32_t pui32Dest[4];
	uint8_t ui8BytCnt;
	uint8_t *pui8DestTemp;
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);
    if((ui8Length == 0)||(ui8Length>16))
    {
       	return(false);
    }

    //
    // Check if the output is ready before reading the data.  If it not ready,
    // return false.
    //
    if((AES_CTRL_OUTPUT_READY & (HWREG(ui32Base + AES_O_CTRL))) == 0)
    {
        return(false);
    }

    //
    // Read a block of data from the data registers
    //
    pui32Dest[0] = HWREG(ui32Base + AES_O_DATA_IN_3);
    pui32Dest[1] = HWREG(ui32Base + AES_O_DATA_IN_2);
    pui32Dest[2] = HWREG(ui32Base + AES_O_DATA_IN_1);
    pui32Dest[3] = HWREG(ui32Base + AES_O_DATA_IN_0);

    //
    //Copy the data to a block memory
    //
    pui8DestTemp = (uint8_t *)pui32Dest;
    for(ui8BytCnt = 0; ui8BytCnt < ui8Length ; ui8BytCnt++)
    {
     	*(pui8Dest+ui8BytCnt) = *(pui8DestTemp+ui8BytCnt);
    }
    //
    // Read successful, return true.
    //
    return(true);
}


//*****************************************************************************
//
//! Reads plaintext/ciphertext from data registers with blocking.
//! This api writes data in blocks
//!
//! \param ui32Base is the base address of the AES module.
//! \param pui8Dest is a pointer to an array of words.
//! \param ui8Length is the length of data in bytes to be read.
//!        ui8Length can be from 1 to 16
//!
//! This function reads a block of either plaintext or ciphertext out of the
//! AES module.  If the output is not ready, the function waits until it
//! is ready. A block is 16 bytes or 4 words.
//!
//! \return None.
//
//*****************************************************************************

void
AESDataRead(uint32_t ui32Base, uint8_t *pui8Dest, uint8_t ui8Length)
{
	volatile uint32_t pui32Dest[4];
	uint8_t ui8BytCnt;
	uint8_t *pui8DestTemp;

    //
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);
    if((ui8Length == 0)||(ui8Length>16))
   	{
       	return;
    }


    //
    // Wait for the output to be ready before reading the data.
    //
    while((AES_CTRL_OUTPUT_READY & (HWREG(ui32Base + AES_O_CTRL))) == 0)
    {
    }

    //
    // Read a block of data from the data registers
    //
    pui32Dest[0] = HWREG(ui32Base + AES_O_DATA_IN_3);
    pui32Dest[1] = HWREG(ui32Base + AES_O_DATA_IN_2);
    pui32Dest[2] = HWREG(ui32Base + AES_O_DATA_IN_1);
    pui32Dest[3] = HWREG(ui32Base + AES_O_DATA_IN_0);
    //
    //Copy the data to a block memory
    //
    pui8DestTemp = (uint8_t *)pui32Dest;
    for(ui8BytCnt = 0; ui8BytCnt < ui8Length ; ui8BytCnt++)
   	{
    	*(pui8Dest+ui8BytCnt) = *(pui8DestTemp+ui8BytCnt);
   	}

    return;
}

//*****************************************************************************
//
//! Writes plaintext/ciphertext to data registers without blocking.
//!
//! \param ui32Base is the base address of the AES module.
//! \param pui8Src is a pointer to an array of words of data.
//! \param ui8Length the length can be from 1 to 16
//!
//! This function writes a block of either plaintext or ciphertext into the
//! AES module.  If the input is not ready, the function returns false
//! If the write completed successfully, the function returns true.
//!
//! \return True or false.
//
//*****************************************************************************
bool
AESDataWriteNonBlocking(uint32_t ui32Base, uint8_t *pui8Src, uint8_t ui8Length)
{
	volatile uint32_t pui32Src[4]={0,0,0,0};
	uint8_t ui8BytCnt;
	uint8_t *pui8SrcTemp;

	//
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);
    if((ui8Length == 0)||(ui8Length>16))
   	{
      	return(false);
    }

    //
    // Check if the input is ready.  If not, then return false.
    //
    if(!(AES_CTRL_INPUT_READY & (HWREG(ui32Base + AES_O_CTRL))))
    {
        return(false);
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
    // Write a block of data into the data registers.
    //
    HWREG(ui32Base + AES_O_DATA_IN_3) = pui32Src[0];
    HWREG(ui32Base + AES_O_DATA_IN_2) = pui32Src[1];
    HWREG(ui32Base + AES_O_DATA_IN_1) = pui32Src[2];
    HWREG(ui32Base + AES_O_DATA_IN_0) = pui32Src[3];

    //
    // Write successful, return true.
    //
    return(true);
}


//*****************************************************************************
//
//! Writes plaintext/ciphertext to data registers with blocking.
//!
//! \param ui32Base is the base address of the AES module.
//! \param pui8Src is a pointer to an array of bytes.
//! \param ui8Length the length can be from 1 to 16
//!
//! This function writes a block of either plaintext or ciphertext into the
//! AES module.  If the input is not ready, the function waits until it is
//! ready before performing the write.
//!
//! \return None.
//
//*****************************************************************************

void
AESDataWrite(uint32_t ui32Base, uint8_t *pui8Src, uint8_t ui8Length)
{
	volatile uint32_t pui32Src[4]={0,0,0,0};
	uint8_t ui8BytCnt;
	uint8_t *pui8SrcTemp;
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);
    if((ui8Length == 0)||(ui8Length>16))
	{
    	return;
	}
    //
    // Wait for input ready.
    //
    while((AES_CTRL_INPUT_READY & (HWREG(ui32Base + AES_O_CTRL))) == 0)
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
    // Write a block of data into the data registers.
    //
    HWREG(ui32Base + AES_O_DATA_IN_3) = pui32Src[0];
    HWREG(ui32Base + AES_O_DATA_IN_2) = pui32Src[1];
    HWREG(ui32Base + AES_O_DATA_IN_1) = pui32Src[2];
    HWREG(ui32Base + AES_O_DATA_IN_0) = pui32Src[3];
}


//*****************************************************************************
//
//! Used to process(transform) blocks of data, either encrypt or decrypt it.
//!
//! \param ui32Base is the base address of the AES module.
//! \param pui8Src is a pointer to the memory location where the input data
//! is stored.
//! \param pui8Dest is a pointer to the memory location output is written.
//! \param ui32Length is the length of the cryptographic data in bytes.
//!
//! This function iterates the encryption or decryption mechanism number over
//! the data length.  Before calling this function, ensure that the AES
//! module is properly configured the key, data size, mode, etc.  Only ECB,
//! CBC, CTR, ICM, CFB, XTS and F8 operating modes should be used.  The data
//! is processed in 4-word (16-byte) blocks.
//!
//! \note This function only supports values of \e ui32Length less than 2^32,
//! because the memory size is restricted to between 0 to 2^32 bytes.
//!
//! \return Returns true if data was processed successfully.  Returns false
//! if data processing failed.
//
//*****************************************************************************
bool
AESDataProcess(uint32_t ui32Base, uint8_t *pui8Src, uint8_t *pui8Dest,
               uint32_t ui32Length)
{
    uint32_t ui32Count, ui32BlkCount, ui32ByteCount;

    //
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);

    //
    // Write the length register first, which triggers the engine to start
    // using this context.
    //
    AESDataLengthSet(AES_BASE, (uint64_t) ui32Length);

    //
    // Now loop until the blocks are written.
    //
    ui32BlkCount = ui32Length/16;
    for(ui32Count = 0; ui32Count < ui32BlkCount; ui32Count += 1)
    {
        //
        // Write the data registers.
        //
        AESDataWrite(ui32Base, pui8Src + (ui32Count*16) ,16);

        //
        // Read the data registers.
        //
        AESDataRead(ui32Base, pui8Dest + (ui32Count*16) ,16);

    }

    //
    //Now handle the residue bytes
    //
    ui32ByteCount = ui32Length%16;
    if(ui32ByteCount)
    {
    	//
    	// Write the data registers.
    	//
    	AESDataWrite(ui32Base, pui8Src + (16*ui32BlkCount) ,ui32ByteCount);

    	//
    	// Read the data registers.
    	//
        AESDataRead(ui32Base, pui8Dest + (16*ui32BlkCount) ,ui32ByteCount);
    }



    //
    // Return true to indicate successful completion of the function.
    //
    return(true);
}
//*****************************************************************************
//
//! Used to generate message authentication code (MAC) using CBC-MAC and F9 mode.
//!
//! \param ui32Base is the base address of the AES module.
//! \param pui8Src is a pointer to the memory location where the input data
//! is stored.
//! \param ui32Length  is the length of the cryptographic data in bytes.
//! \param pui8Tag is a pointer to a 4-word array where the hash tag is
//! written.
//!
//! This function processes data to produce a hash tag that can be used tor
//! authentication.   Before calling this function, ensure that the AES
//! module is properly configured the key, data size, mode, etc.  Only
//! CBC-MAC and F9 modes should be used.
//!
//! \return Returns true if data was processed successfully.  Returns false
//! if data processing failed.
//
//*****************************************************************************
bool
AESDataMAC(uint32_t ui32Base, uint8_t *pui8Src, uint32_t ui32Length,
            uint8_t *pui8Tag)
{
    uint32_t ui32Count, ui32BlkCount, ui32ByteCount;
	//
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);

    //
    // Write the length register first, which triggers the engine to start
    // using this context.
    //
    AESDataLengthSet(AES_BASE, (uint64_t) ui32Length);

    //
    // Write the data registers.
    //

    //
    // Now loop until the blocks are written.
    //
    ui32BlkCount = ui32Length/16;
    for(ui32Count = 0; ui32Count < ui32BlkCount; ui32Count += 1)
    {
	   //
	   // Write the data registers.
	   //
	   AESDataWrite(ui32Base, pui8Src + ui32Count*16 ,16);
    }

    //
    //Now handle the residue bytes
    //
    ui32ByteCount = ui32Length%16;
    if(ui32ByteCount)
    {
	   //
	   // Write the data registers.
	   //
	   AESDataWrite(ui32Base, pui8Src + (ui32Count*ui32BlkCount) ,ui32ByteCount);
    }

    //
    // Wait for the context data regsiters to be ready.
    //
    while((AES_CTRL_SVCTXTRDY & (HWREG(AES_BASE + AES_O_CTRL))) == 0)
    {
    }

    //
    // Read the hash tag value.
    //
    AESTagRead(AES_BASE, pui8Tag);

    //
    // Return true to indicate successful completion of the function.
    //
    return(true);
}

//*****************************************************************************
//
//! Used for Authenticated encryption (AE) of the data. Processes and authenticates blocks of data,
//! either encrypt the data or decrypt the data.
//!
//! \param ui32Base  is the base address of the AES module.
//! \param pui8Src is a pointer to the memory location where the input data
//! is stored. The data must be padded to the 16-byte boundary.
//! \param pui8Dest is a pointer to the memory location output is written.
//! The space for written data must be rounded up to the 16-byte boundary.
//! \param ui32Length is the length of the cryptographic data in bytes.
//! \param pui8AuthSrc is a pointer to the memory location where the
//! additional authentication data is stored.  The data must be padded to the
//! 16-byte boundary.
//! \param ui32AuthLength is the length of the additional authentication
//! data in bytes.
//! \param pui8Tag is a pointer to a 4-word array where the hash tag is
//! written.
//!
//! This function encrypts or decrypts blocks of data in addition to
//! authentication data.  A hash tag is also produced.  Before calling this
//! function, ensure that the AES module is properly configured the key,
//! data size, mode, etc.  Only CCM and GCM modes should be used.
//!
//! \return Returns true if data was processed successfully.  Returns false
//! if data processing failed.
//
//*****************************************************************************
bool
AESDataProcessAE(uint32_t ui32Base, uint8_t *pui8Src, uint8_t *pui8Dest,
                   uint32_t ui32Length, uint8_t *pui8AuthSrc,
                   uint32_t ui32AuthLength, uint8_t *pui8Tag)
{
    uint32_t ui32Count;

    //
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);

    //
    // Set the data length.
    //
    AESDataLengthSet(AES_BASE, (uint64_t) ui32Length);

    //
    // Set the additional authentication data length.
    //
    AESAuthDataLengthSet(AES_BASE, ui32AuthLength);

    //
    // Now loop until the authentication data blocks are written.
    //
    for(ui32Count = 0; ui32Count < ui32AuthLength; ui32Count += 16)
    {
        //
        // Write the data registers.
        //
        AESDataWrite(ui32Base, pui8AuthSrc + (ui32Count),16);
    }

    //
    // Now loop until the data blocks are written.
    //
    for(ui32Count = 0; ui32Count < ui32Length; ui32Count += 16)
    {
        //
        // Write the data registers.
        //
        AESDataWrite(ui32Base, pui8Src + (ui32Count),16);

        //
        //
        // Read the data registers.
        //
        AESDataRead(ui32Base, pui8Dest + (ui32Count),16);
    }

    //
    // Wait for the context data regsiters to be ready.
    //
    while((AES_CTRL_SVCTXTRDY & (HWREG(AES_BASE + AES_O_CTRL))) == 0)
    {
    }

    //
    // Read the hash tag value.
    //
    AESTagRead(AES_BASE, pui8Tag);

    //
    // Return true to indicate successful completion of the function.
    //
    return(true);
}

//*****************************************************************************
//
//! Returns the current AES module interrupt status.
//!
//! \param ui32Base is the base address of the AES module.
//! \param bMasked is \b false if the raw interrupt status is required and
//! \b true if the masked interrupt status is required.
//!
//! \return Returns a bit mask of the interrupt sources, which is a logical OR
//! of any of the following:
//!
//! - \b AES_INT_CONTEXT_IN - Context interrupt
//! - \b AES_INT_CONTEXT_OUT - Authentication tag (and IV) interrupt.
//! - \b AES_INT_DATA_IN - Data input interrupt
//! - \b AES_INT_DATA_OUT - Data output interrupt
//! - \b AES_INT_DMA_CONTEXT_IN - Context DMA done interrupt
//! - \b AES_INT_DMA_CONTEXT_OUT - Authentication tag (and IV) DMA done
//! interrupt
//! - \b AES_INT_DMA_DATA_IN - Data input DMA done interrupt
//! - \b AES_INT_DMA_DATA_OUT - Data output DMA done interrupt
//
//*****************************************************************************
uint32_t
AESIntStatus(uint32_t ui32Base, bool bMasked)
{
    uint32_t ui32Temp;
    uint32_t ui32IrqEnable;

    //
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);

    //
    // Read the IRQ status register and return the value.
    //
    if(bMasked)
    {
        ui32Temp = HWREG(DTHE_BASE + DTHE_O_AES_MIS);
        ui32IrqEnable = HWREG(ui32Base + AES_O_IRQENABLE);
        return((HWREG(ui32Base + AES_O_IRQSTATUS) &
                ui32IrqEnable) | ((ui32Temp & 0x0000000F) << 16));
    }
    else
    {
        ui32Temp = HWREG(DTHE_BASE + DTHE_O_AES_RIS);
        return(HWREG(ui32Base + AES_O_IRQSTATUS) |
               ((ui32Temp & 0x0000000F) << 16));
    }
}

//*****************************************************************************
//
//! Enables AES module interrupts.
//!
//! \param ui32Base is the base address of the AES module.
//! \param ui32IntFlags is a bit mask of the interrupt sources to enable.
//!
//! This function enables the interrupts in the AES module. The \e ui32IntFlags
//! parameter is the logical OR of any of the following:
//!
//! - \b AES_INT_CONTEXT_IN - Context interrupt
//! - \b AES_INT_CONTEXT_OUT - Authentication tag (and IV) interrupt
//! - \b AES_INT_DATA_IN - Data input interrupt
//! - \b AES_INT_DATA_OUT - Data output interrupt
//! - \b AES_INT_DMA_CONTEXT_IN - Context DMA done interrupt
//! - \b AES_INT_DMA_CONTEXT_OUT - Authentication tag (and IV) DMA done
//! interrupt
//! - \b AES_INT_DMA_DATA_IN - Data input DMA done interrupt
//! - \b AES_INT_DMA_DATA_OUT - Data output DMA done interrupt
//!
//! \note Interrupts that have been previously been enabled are not disabled
//! when this function is called.
//!
//! \return None.
//
//*****************************************************************************
void
AESIntEnable(uint32_t ui32Base, uint32_t ui32IntFlags)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);
    ASSERT((ui32IntFlags == AES_INT_CONTEXT_IN) ||
           (ui32IntFlags == AES_INT_CONTEXT_OUT) ||
           (ui32IntFlags == AES_INT_DATA_IN) ||
           (ui32IntFlags == AES_INT_DATA_OUT) ||
           (ui32IntFlags == AES_INT_DMA_CONTEXT_IN) ||
           (ui32IntFlags == AES_INT_DMA_CONTEXT_OUT) ||
           (ui32IntFlags == AES_INT_DMA_DATA_IN) ||
           (ui32IntFlags == AES_INT_DMA_DATA_OUT));

    //
    // Set the flags.
    //
    HWREG(DTHE_BASE + DTHE_O_AES_IM) &= ~((ui32IntFlags & 0x000F0000) >> 16);
    HWREG(ui32Base + AES_O_IRQENABLE) |= ui32IntFlags & 0x0000ffff;
}

//*****************************************************************************
//
//! Disables AES module interrupts.
//!
//! \param ui32Base is the base address of the AES module.
//! \param ui32IntFlags is a bit mask of the interrupt sources to disable.
//!
//! This function disables the interrupt sources in the AES module. The
//! \e ui32IntFlags parameter is the logical OR of any of the following:
//!
//! - \b AES_INT_CONTEXT_IN - Context interrupt
//! - \b AES_INT_CONTEXT_OUT - Authentication tag (and IV) interrupt
//! - \b AES_INT_DATA_IN - Data input interrupt
//! - \b AES_INT_DATA_OUT - Data output interrupt
//! - \b AES_INT_DMA_CONTEXT_IN - Context DMA done interrupt
//! - \b AES_INT_DMA_CONTEXT_OUT - Authentication tag (and IV) DMA done
//! interrupt
//! - \b AES_INT_DMA_DATA_IN - Data input DMA done interrupt
//! - \b AES_INT_DMA_DATA_OUT - Data output DMA done interrupt
//!
//! \note The DMA done interrupts are the only interrupts that can be cleared.
//! The remaining interrupts can be disabled instead using AESIntDisable().
//!
//! \return None.
//
//*****************************************************************************
void
AESIntDisable(uint32_t ui32Base, uint32_t ui32IntFlags)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);
    ASSERT((ui32IntFlags == AES_INT_CONTEXT_IN) ||
           (ui32IntFlags == AES_INT_CONTEXT_OUT) ||
           (ui32IntFlags == AES_INT_DATA_IN) ||
           (ui32IntFlags == AES_INT_DATA_OUT) ||
           (ui32IntFlags == AES_INT_DMA_CONTEXT_IN) ||
           (ui32IntFlags == AES_INT_DMA_CONTEXT_OUT) ||
           (ui32IntFlags == AES_INT_DMA_DATA_IN) ||
           (ui32IntFlags == AES_INT_DMA_DATA_OUT));

    //
    // Clear the flags.
    //
    HWREG(DTHE_BASE + DTHE_O_AES_IM) |= ((ui32IntFlags & 0x000F0000) >> 16);
    HWREG(ui32Base + AES_O_IRQENABLE) &= ~(ui32IntFlags & 0x0000ffff);
}

//*****************************************************************************
//
//! Clears AES module interrupts.
//!
//! \param ui32Base is the base address of the AES module.
//! \param ui32IntFlags is a bit mask of the interrupt sources to disable.
//!
//! This function clears the interrupt sources in the AES module.  The
//! \e ui32IntFlags parameter is the logical OR of any of the following:
//!
//! - \b AES_INT_DMA_CONTEXT_IN - Context DMA done interrupt
//! - \b AES_INT_DMA_CONTEXT_OUT - Authentication tag (and IV) DMA done
//! interrupt
//! - \b AES_INT_DMA_DATA_IN - Data input DMA done interrupt
//! - \b AES_INT_DMA_DATA_OUT - Data output DMA done interrupt
//!
//! \note Only the DMA done interrupts can be cleared.  The remaining
//! interrupts should be disabled with AESIntDisable().
//!
//! \return None.
//
//*****************************************************************************
void
AESIntClear(uint32_t ui32Base, uint32_t ui32IntFlags)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);
    ASSERT((ui32IntFlags == AES_INT_DMA_CONTEXT_IN) ||
           (ui32IntFlags == AES_INT_DMA_CONTEXT_OUT) ||
           (ui32IntFlags == AES_INT_DMA_DATA_IN) ||
           (ui32IntFlags == AES_INT_DMA_DATA_OUT));

    HWREG(DTHE_BASE + DTHE_O_AES_IC) = ((ui32IntFlags >> 16) & 0x0000000F);
}

//*****************************************************************************
//
//! Registers an interrupt handler for the AES module.
//!
//! \param ui32Base is the base address of the AES module.
//! \param pfnHandler is a pointer to the function to be called when the
//! enabled AES interrupts occur.
//!
//! This function registers the interrupt handler in the interrupt vector
//! table, and enables AES interrupts on the interrupt controller; specific AES
//! interrupt sources must be enabled using AESIntEnable().  The interrupt
//! handler being registered must clear the source of the interrupt using
//! AESIntClear().
//!
//! If the application is using a static interrupt vector table stored in
//! flash, then it is not necessary to register the interrupt handler this way.
//! Instead, IntEnable() is used to enable AES interrupts on the
//! interrupt controller.
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \return None.
//
//*****************************************************************************
void
AESIntRegister(uint32_t ui32Base, void(*pfnHandler)(void))
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);

    //
    // Register the interrupt handler.
    //
    IntRegister(INT_AES, pfnHandler);

    //
    // Enable the interrupt
    //
    IntEnable(INT_AES);
}

//*****************************************************************************
//
//! Unregisters an interrupt handler for the AES module.
//!
//! \param ui32Base is the base address of the AES module.
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
AESIntUnregister(uint32_t ui32Base)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);

    //
    // Disable the interrupt.
    //
    IntDisable(INT_AES);

    //
    // Unregister the interrupt handler.
    //
    IntUnregister(INT_AES);
}

//*****************************************************************************
//
//! Enables uDMA requests for the AES module.
//!
//! \param ui32Base is the base address of the AES module.
//! \param ui32Flags is a bit mask of the uDMA requests to be enabled.
//!
//! This function enables the uDMA request sources in the AES module.
//! The \e ui32Flags parameter is the logical OR of any of the following:
//!
//! - \b AES_DMA_DATA_IN
//! - \b AES_DMA_DATA_OUT
//! - \b AES_DMA_CONTEXT_IN
//! - \b AES_DMA_CONTEXT_OUT
//!
//! \return None.
//
//*****************************************************************************
void
AESDMAEnable(uint32_t ui32Base, uint32_t ui32Flags)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);
    ASSERT((ui32Flags == AES_DMA_DATA_IN) ||
           (ui32Flags == AES_DMA_DATA_OUT) ||
           (ui32Flags == AES_DMA_CONTEXT_IN) ||
           (ui32Flags == AES_DMA_CONTEXT_OUT));

    //
    // Set the flags in the current register value.
    //
    HWREG(ui32Base + AES_O_SYSCONFIG) |= ui32Flags;
}

//*****************************************************************************
//
//! Disables uDMA requests for the AES module.
//!
//! \param ui32Base is the base address of the AES module.
//! \param ui32Flags is a bit mask of the uDMA requests to be disabled.
//!
//! This function disables the uDMA request sources in the AES module.
//! The \e ui32Flags parameter is the logical OR of any of the
//! following:
//!
//! - \b AES_DMA_DATA_IN
//! - \b AES_DMA_DATA_OUT
//! - \b AES_DMA_CONTEXT_IN
//! - \b AES_DMA_CONTEXT_OUT
//!
//! \return None.
//
//*****************************************************************************
void
AESDMADisable(uint32_t ui32Base, uint32_t ui32Flags)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Base == AES_BASE);
    ASSERT((ui32Flags == AES_DMA_DATA_IN) ||
           (ui32Flags == AES_DMA_DATA_OUT) ||
           (ui32Flags == AES_DMA_CONTEXT_IN) ||
           (ui32Flags == AES_DMA_CONTEXT_OUT));

    //
    // Clear the flags in the current register value.
    //
    HWREG(ui32Base + AES_O_SYSCONFIG) &= ~ui32Flags;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
