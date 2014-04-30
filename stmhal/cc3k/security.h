/*****************************************************************************
*
*  security.h  - CC3000 Host Driver Implementation.
*  Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/
#ifndef __SECURITY__
#define __SECURITY__

#include "nvmem.h"

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef  __cplusplus
extern "C" {
#endif


#define AES128_KEY_SIZE		16

#ifndef CC3000_UNENCRYPTED_SMART_CONFIG


//*****************************************************************************
//
//!  aes_encrypt
//!
//!  @param[in]  key   AES128 key of size 16 bytes
//!  @param[in\out] state   16 bytes of plain text and cipher text
//!
//!  @return  none
//!
//!  @brief   AES128 encryption:
//!           Given AES128 key and  16 bytes plain text, cipher text of 16 bytes
//!           is computed. The AES implementation is in mode ECB (Electronic 
//!           Code Book). 
//!	 
//!
//*****************************************************************************
extern void aes_encrypt(unsigned char *state, unsigned char *key);

//*****************************************************************************
//
//!  aes_decrypt
//!
//!  @param[in]  key   AES128 key of size 16 bytes
//!  @param[in\out] state   16 bytes of cipher text and plain text
//!
//!  @return  none
//!
//!  @brief   AES128 decryption:
//!           Given AES128 key and  16 bytes cipher text, plain text of 16 bytes
//!           is computed The AES implementation is in mode ECB 
//!           (Electronic Code Book).
//!	 
//!
//*****************************************************************************
extern void aes_decrypt(unsigned char *state, unsigned char *key);


//*****************************************************************************
//
//!  aes_read_key
//!
//!  @param[out]  key   AES128 key of size 16 bytes
//!
//!  @return  on success 0, error otherwise.
//!
//!  @brief   Reads AES128 key from EEPROM
//!           Reads the AES128 key from fileID #12 in EEPROM
//!           returns an error if the key does not exist. 
//!	 
//!
//*****************************************************************************
extern signed long aes_read_key(unsigned char *key);

//*****************************************************************************
//
//!  aes_write_key
//!
//!  @param[out]  key   AES128 key of size 16 bytes
//!
//!  @return  on success 0, error otherwise.
//!
//!  @brief   writes AES128 key from EEPROM
//!           Writes the AES128 key to fileID #12 in EEPROM
//!	 
//!
//*****************************************************************************
extern signed long aes_write_key(unsigned char *key);

#endif //CC3000_UNENCRYPTED_SMART_CONFIG

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef  __cplusplus
}
#endif // __cplusplus

#endif
