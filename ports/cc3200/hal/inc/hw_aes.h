//*****************************************************************************
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

#ifndef __HW_AES_H__
#define __HW_AES_H__

//*****************************************************************************
//
// The following are defines for the AES_P register offsets.
//
//*****************************************************************************
#define AES_O_KEY2_6          0x00000000  // XTS second key / CBC-MAC third
                                            // key
#define AES_O_KEY2_7          0x00000004  // XTS second key (MSW for 256-bit
                                            // key) / CBC-MAC third key (MSW)
#define AES_O_KEY2_4          0x00000008  // XTS / CCM second key / CBC-MAC
                                            // third key (LSW)
#define AES_O_KEY2_5          0x0000000C  // XTS second key (MSW for 192-bit
                                            // key) / CBC-MAC third key
#define AES_O_KEY2_2          0x00000010  // XTS / CCM / CBC-MAC second key /
                                            // Hash Key input
#define AES_O_KEY2_3          0x00000014  // XTS second key (MSW for 128-bit
                                            // key) + CCM/CBC-MAC second key
                                            // (MSW) / Hash Key input (MSW)
#define AES_O_KEY2_0          0x00000018  // XTS / CCM / CBC-MAC second key
                                            // (LSW) / Hash Key input (LSW)
#define AES_O_KEY2_1          0x0000001C  // XTS / CCM / CBC-MAC second key /
                                            // Hash Key input
#define AES_O_KEY1_6          0x00000020  // Key (LSW for 256-bit key)
#define AES_O_KEY1_7          0x00000024  // Key (MSW for 256-bit key)
#define AES_O_KEY1_4          0x00000028  // Key (LSW for 192-bit key)
#define AES_O_KEY1_5          0x0000002C  // Key (MSW for 192-bit key)
#define AES_O_KEY1_2          0x00000030  // Key
#define AES_O_KEY1_3          0x00000034  // Key (MSW for 128-bit key)
#define AES_O_KEY1_0          0x00000038  // Key (LSW for 128-bit key)
#define AES_O_KEY1_1          0x0000003C  // Key
#define AES_O_IV_IN_0         0x00000040  // Initialization Vector input
                                            // (LSW)
#define AES_O_IV_IN_1         0x00000044  // Initialization vector input
#define AES_O_IV_IN_2         0x00000048  // Initialization vector input
#define AES_O_IV_IN_3         0x0000004C  // Initialization Vector input
                                            // (MSW)
#define AES_O_CTRL            0x00000050  // register determines the mode of
                                            // operation of the AES Engine
#define AES_O_C_LENGTH_0      0x00000054  // Crypto data length registers
                                            // (LSW and MSW) store the
                                            // cryptographic data length in
                                            // bytes for all modes. Once
                                            // processing with this context is
                                            // started@@ this length decrements
                                            // to zero. Data lengths up to (2^61
                                            // – 1) bytes are allowed. For GCM@@
                                            // any value up to 2^36 - 32 bytes
                                            // can be used. This is because a
                                            // 32-bit counter mode is used; the
                                            // maximum number of 128-bit blocks
                                            // is 2^32 – 2@@ resulting in a
                                            // maximum number of bytes of 2^36 -
                                            // 32. A write to this register
                                            // triggers the engine to start
                                            // using this context. This is valid
                                            // for all modes except GCM and CCM.
                                            // Note that for the combined
                                            // modes@@ this length does not
                                            // include the authentication only
                                            // data; the authentication length
                                            // is specified in the
                                            // AES_AUTH_LENGTH register below.
                                            // All modes must have a length > 0.
                                            // For the combined modes@@ it is
                                            // allowed to have one of the
                                            // lengths equal to zero. For the
                                            // basic encryption modes
                                            // (ECB/CBC/CTR/ICM/CFB128) it is
                                            // allowed to program zero to the
                                            // length field; in that case the
                                            // length is assumed infinite. All
                                            // data must be byte (8-bit)
                                            // aligned; bit aligned data streams
                                            // are not supported by the AES
                                            // Engine. For a Host read
                                            // operation@@ these registers
                                            // return all-zeroes.
#define AES_O_C_LENGTH_1      0x00000058  // Crypto data length registers
                                            // (LSW and MSW) store the
                                            // cryptographic data length in
                                            // bytes for all modes. Once
                                            // processing with this context is
                                            // started@@ this length decrements
                                            // to zero. Data lengths up to (2^61
                                            // – 1) bytes are allowed. For GCM@@
                                            // any value up to 2^36 - 32 bytes
                                            // can be used. This is because a
                                            // 32-bit counter mode is used; the
                                            // maximum number of 128-bit blocks
                                            // is 2^32 – 2@@ resulting in a
                                            // maximum number of bytes of 2^36 -
                                            // 32. A write to this register
                                            // triggers the engine to start
                                            // using this context. This is valid
                                            // for all modes except GCM and CCM.
                                            // Note that for the combined
                                            // modes@@ this length does not
                                            // include the authentication only
                                            // data; the authentication length
                                            // is specified in the
                                            // AES_AUTH_LENGTH register below.
                                            // All modes must have a length > 0.
                                            // For the combined modes@@ it is
                                            // allowed to have one of the
                                            // lengths equal to zero. For the
                                            // basic encryption modes
                                            // (ECB/CBC/CTR/ICM/CFB128) it is
                                            // allowed to program zero to the
                                            // length field; in that case the
                                            // length is assumed infinite. All
                                            // data must be byte (8-bit)
                                            // aligned; bit aligned data streams
                                            // are not supported by the AES
                                            // Engine. For a Host read
                                            // operation@@ these registers
                                            // return all-zeroes.
#define AES_O_AUTH_LENGTH     0x0000005C  // AAD data length. The
                                            // authentication length register
                                            // store the authentication data
                                            // length in bytes for combined
                                            // modes only (GCM or CCM) Supported
                                            // AAD-lengths for CCM are from 0 to
                                            // (2^16 - 2^8) bytes. For GCM any
                                            // value up to (2^32 - 1) bytes can
                                            // be used. Once processing with
                                            // this context is started@@ this
                                            // length decrements to zero. A
                                            // write to this register triggers
                                            // the engine to start using this
                                            // context for GCM and CCM. For XTS
                                            // this register is optionally used
                                            // to load ‘j’. Loading of ‘j’ is
                                            // only required if ‘j’ != 0. ‘j’ is
                                            // a 28-bit value and must be
                                            // written to bits [31-4] of this
                                            // register. ‘j’ represents the
                                            // sequential number of the 128-bit
                                            // block inside the data unit. For
                                            // the first block in a unit@@ this
                                            // value is zero. It is not required
                                            // to provide a ‘j’ for each new
                                            // data block within a unit. Note
                                            // that it is possible to start with
                                            // a ‘j’ unequal to zero; refer to
                                            // Table 4 for more details. For a
                                            // Host read operation@@ these
                                            // registers return all-zeroes.
#define AES_O_DATA_IN_0       0x00000060  // Data register to read and write
                                            // plaintext/ciphertext (MSW)
#define AES_O_DATA_IN_1       0x00000064  // Data register to read and write
                                            // plaintext/ciphertext
#define AES_O_DATA_IN_2       0x00000068  // Data register to read and write
                                            // plaintext/ciphertext
#define AES_O_DATA_IN_3       0x0000006C  // Data register to read and write
                                            // plaintext/ciphertext (LSW)
#define AES_O_TAG_OUT_0       0x00000070
#define AES_O_TAG_OUT_1       0x00000074
#define AES_O_TAG_OUT_2       0x00000078
#define AES_O_TAG_OUT_3       0x0000007C
#define AES_O_REVISION        0x00000080  // Register AES_REVISION
#define AES_O_SYSCONFIG       0x00000084  // Register AES_SYSCONFIG.This
                                            // register configures the DMA
                                            // signals and controls the IDLE and
                                            // reset logic
#define AES_O_SYSSTATUS       0x00000088
#define AES_O_IRQSTATUS       0x0000008C  // This register indicates the
                                            // interrupt status. If one of the
                                            // interrupt bits is set the
                                            // interrupt output will be asserted
#define AES_O_IRQENABLE       0x00000090  // This register contains an enable
                                            // bit for each unique interrupt
                                            // generated by the module. It
                                            // matches the layout of
                                            // AES_IRQSTATUS register. An
                                            // interrupt is enabled when the bit
                                            // in this register is set to ‘1’.
                                            // An interrupt that is enabled is
                                            // propagated to the SINTREQUEST_x
                                            // output. All interrupts need to be
                                            // enabled explicitly by writing
                                            // this register.



//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_KEY2_6 register.
//
//******************************************************************************
#define AES_KEY2_6_KEY_M      0xFFFFFFFF  // key data
#define AES_KEY2_6_KEY_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_KEY2_7 register.
//
//******************************************************************************
#define AES_KEY2_7_KEY_M      0xFFFFFFFF  // key data
#define AES_KEY2_7_KEY_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_KEY2_4 register.
//
//******************************************************************************
#define AES_KEY2_4_KEY_M      0xFFFFFFFF  // key data
#define AES_KEY2_4_KEY_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_KEY2_5 register.
//
//******************************************************************************
#define AES_KEY2_5_KEY_M      0xFFFFFFFF  // key data
#define AES_KEY2_5_KEY_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_KEY2_2 register.
//
//******************************************************************************
#define AES_KEY2_2_KEY_M      0xFFFFFFFF  // key data
#define AES_KEY2_2_KEY_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_KEY2_3 register.
//
//******************************************************************************
#define AES_KEY2_3_KEY_M      0xFFFFFFFF  // key data
#define AES_KEY2_3_KEY_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_KEY2_0 register.
//
//******************************************************************************
#define AES_KEY2_0_KEY_M      0xFFFFFFFF  // key data
#define AES_KEY2_0_KEY_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_KEY2_1 register.
//
//******************************************************************************
#define AES_KEY2_1_KEY_M      0xFFFFFFFF  // key data
#define AES_KEY2_1_KEY_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_KEY1_6 register.
//
//******************************************************************************
#define AES_KEY1_6_KEY_M      0xFFFFFFFF  // key data
#define AES_KEY1_6_KEY_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_KEY1_7 register.
//
//******************************************************************************
#define AES_KEY1_7_KEY_M      0xFFFFFFFF  // key data
#define AES_KEY1_7_KEY_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_KEY1_4 register.
//
//******************************************************************************
#define AES_KEY1_4_KEY_M      0xFFFFFFFF  // key data
#define AES_KEY1_4_KEY_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_KEY1_5 register.
//
//******************************************************************************
#define AES_KEY1_5_KEY_M      0xFFFFFFFF  // key data
#define AES_KEY1_5_KEY_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_KEY1_2 register.
//
//******************************************************************************
#define AES_KEY1_2_KEY_M      0xFFFFFFFF  // key data
#define AES_KEY1_2_KEY_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_KEY1_3 register.
//
//******************************************************************************
#define AES_KEY1_3_KEY_M      0xFFFFFFFF  // key data
#define AES_KEY1_3_KEY_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_KEY1_0 register.
//
//******************************************************************************
#define AES_KEY1_0_KEY_M      0xFFFFFFFF  // key data
#define AES_KEY1_0_KEY_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_KEY1_1 register.
//
//******************************************************************************
#define AES_KEY1_1_KEY_M      0xFFFFFFFF  // key data
#define AES_KEY1_1_KEY_S      0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_IV_IN_0 register.
//
//******************************************************************************
#define AES_IV_IN_0_DATA_M    0xFFFFFFFF  // IV data
#define AES_IV_IN_0_DATA_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_IV_IN_1 register.
//
//******************************************************************************
#define AES_IV_IN_1_DATA_M    0xFFFFFFFF  // IV data
#define AES_IV_IN_1_DATA_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_IV_IN_2 register.
//
//******************************************************************************
#define AES_IV_IN_2_DATA_M    0xFFFFFFFF  // IV data
#define AES_IV_IN_2_DATA_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_IV_IN_3 register.
//
//******************************************************************************
#define AES_IV_IN_3_DATA_M    0xFFFFFFFF  // IV data
#define AES_IV_IN_3_DATA_S    0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_CTRL register.
//
//******************************************************************************
#define AES_CTRL_CONTEXT_READY \
                                0x80000000  // If ‘1’@@ this read-only status
                                            // bit indicates that the context
                                            // data registers can be overwritten
                                            // and the host is permitted to
                                            // write the next context.

#define AES_CTRL_SVCTXTRDY \
                                0x40000000  // If ‘1’@@ this read-only status
                                            // bit indicates that an AES
                                            // authentication TAG and/or IV
                                            // block(s) is/are available for the
                                            // host to retrieve. This bit is
                                            // only asserted if the
                                            // ‘save_context’ bit is set to ‘1’.
                                            // The bit is mutual exclusive with
                                            // the ‘context_ready’ bit.

#define AES_CTRL_SAVE_CONTEXT 0x20000000  // This bit is used to indicate
                                            // that an authentication TAG or
                                            // result IV needs to be stored as a
                                            // result context. If this bit is
                                            // set@@ context output DMA and/or
                                            // interrupt will be asserted if the
                                            // operation is finished and related
                                            // signals are enabled.
#define AES_CTRL_CCM_M      0x01C00000  // Defines “M” that indicated the
                                            // length of the authentication
                                            // field for CCM operations; the
                                            // authentication field length
                                            // equals two times (the value of
                                            // CCM-M plus one). Note that the
                                            // AES Engine always returns a
                                            // 128-bit authentication field@@ of
                                            // which the M least significant
                                            // bytes are valid. All values are
                                            // supported.
#define AES_CTRL_CCM_S      22
#define AES_CTRL_CCM_L_M      0x00380000  // Defines “L” that indicated the
                                            // width of the length field for CCM
                                            // operations; the length field in
                                            // bytes equals the value of CMM-L
                                            // plus one. Supported values for L
                                            // are (programmed value): 2 (1)@@ 4
                                            // (3) and 8 (7).
#define AES_CTRL_CCM_L_S      19
#define AES_CTRL_CCM          0x00040000  // AES-CCM is selected@@ this is a
                                            // combined mode@@ using AES for
                                            // both authentication and
                                            // encryption. No additional mode
                                            // selection is required. 0 Other
                                            // mode selected 1 ccm mode selected
#define AES_CTRL_GCM_M        0x00030000  // AES-GCM mode is selected.this is
                                            // a combined mode@@ using the
                                            // Galois field multiplier GF(2^128)
                                            // for authentication and AES-CTR
                                            // mode for encryption@@ the bits
                                            // specify the GCM mode. 0x0 No
                                            // operation 0x1 GHASH with H loaded
                                            // and Y0-encrypted forced to zero
                                            // 0x2 GHASH with H loaded and
                                            // Y0-encrypted calculated
                                            // internally 0x3 Autonomous GHASH
                                            // (both H and Y0-encrypted
                                            // calculated internally)
#define AES_CTRL_GCM_S        16
#define AES_CTRL_CBCMAC       0x00008000  // AES-CBC MAC is selected@@ the
                                            // Direction bit must be set to ‘1’
                                            // for this mode. 0 Other mode
                                            // selected 1 cbcmac mode selected
#define AES_CTRL_F9           0x00004000  // AES f9 mode is selected@@ the
                                            // AES key size must be set to
                                            // 128-bit for this mode. 0 Other
                                            // mode selected 1 f9 selected
#define AES_CTRL_F8           0x00002000  // AES f8 mode is selected@@ the
                                            // AES key size must be set to
                                            // 128-bit for this mode. 0 Other
                                            // mode selected 1 f8 selected
#define AES_CTRL_XTS_M        0x00001800  // AES-XTS operation is selected;
                                            // the bits specify the XTS mode.01
                                            // = Previous/intermediate tweak
                                            // value and ‘j’ loaded (value is
                                            // loaded via IV@@ j is loaded via
                                            // the AAD length register) 0x0 No
                                            // operation 0x1
                                            // Previous/intermediate tweak value
                                            // and ‘j’ loaded (value is loaded
                                            // via IV@@ j is loaded via the AAD
                                            // length register) 0x2 Key2@@ i and
                                            // j loaded (i is loaded via IV@@ j
                                            // is loaded via the AAD length
                                            // register) 0x3 Key2 and i loaded@@
                                            // j=0 (i is loaded via IV)
#define AES_CTRL_XTS_S        11
#define AES_CTRL_CFB          0x00000400  // full block AES cipher feedback
                                            // mode (CFB128) is selected. 0
                                            // other mode selected 1 cfb
                                            // selected
#define AES_CTRL_ICM          0x00000200  // AES integer counter mode (ICM)
                                            // is selected@@ this is a counter
                                            // mode with a 16-bit wide counter.
                                            // 0 Other mode selected. 1 ICM mode
                                            // selected
#define AES_CTRL_CTR_WIDTH_M  0x00000180  // Specifies the counter width for
                                            // AES-CTR mode 0x0 Counter is 32
                                            // bits 0x1 Counter is 64 bits 0x2
                                            // Counter is 128 bits 0x3 Counter
                                            // is 192 bits
#define AES_CTRL_CTR_WIDTH_S  7
#define AES_CTRL_CTR          0x00000040  // Tthis bit must also be set for
                                            // GCM and CCM@@ when
                                            // encryption/decryption is
                                            // required. 0 Other mode selected 1
                                            // Counter mode
#define AES_CTRL_MODE         0x00000020  // ecb/cbc mode 0 ecb mode 1 cbc
                                            // mode
#define AES_CTRL_KEY_SIZE_M   0x00000018  // key size 0x0 reserved 0x1 Key is
                                            // 128 bits. 0x2 Key is 192 bits 0x3
                                            // Key is 256
#define AES_CTRL_KEY_SIZE_S   3
#define AES_CTRL_DIRECTION    0x00000004  // If set to ‘1’ an encrypt
                                            // operation is performed. If set to
                                            // ‘0’ a decrypt operation is
                                            // performed. Read 0 decryption is
                                            // selected Read 1 Encryption is
                                            // selected
#define AES_CTRL_INPUT_READY  0x00000002  // If ‘1’@@ this read-only status
                                            // bit indicates that the 16-byte
                                            // input buffer is empty@@ and the
                                            // host is permitted to write the
                                            // next block of data.
#define AES_CTRL_OUTPUT_READY 0x00000001  // If ‘1’@@ this read-only status
                                            // bit indicates that an AES output
                                            // block is available for the host
                                            // to retrieve.
//******************************************************************************
//
// The following are defines for the bit fields in the
// AES_O_C_LENGTH_0 register.
//
//******************************************************************************
//******************************************************************************
//
// The following are defines for the bit fields in the
// AES_O_C_LENGTH_1 register.
//
//******************************************************************************
#define AES_C_LENGTH_1_LENGTH_M \
                                0x1FFFFFFF  // Data length (MSW) length
                                            // registers (LSW and MSW) store the
                                            // cryptographic data length in
                                            // bytes for all modes. Once
                                            // processing with this context is
                                            // started@@ this length decrements
                                            // to zero. Data lengths up to (2^61
                                            // – 1) bytes are allowed. For GCM@@
                                            // any value up to 2^36 - 32 bytes
                                            // can be used. This is because a
                                            // 32-bit counter mode is used; the
                                            // maximum number of 128-bit blocks
                                            // is 2^32 – 2@@ resulting in a
                                            // maximum number of bytes of 2^36 -
                                            // 32. A write to this register
                                            // triggers the engine to start
                                            // using this context. This is valid
                                            // for all modes except GCM and CCM.
                                            // Note that for the combined
                                            // modes@@ this length does not
                                            // include the authentication only
                                            // data; the authentication length
                                            // is specified in the
                                            // AES_AUTH_LENGTH register below.
                                            // All modes must have a length > 0.
                                            // For the combined modes@@ it is
                                            // allowed to have one of the
                                            // lengths equal to zero. For the
                                            // basic encryption modes
                                            // (ECB/CBC/CTR/ICM/CFB128) it is
                                            // allowed to program zero to the
                                            // length field; in that case the
                                            // length is assumed infinite. All
                                            // data must be byte (8-bit)
                                            // aligned; bit aligned data streams
                                            // are not supported by the AES
                                            // Engine. For a Host read
                                            // operation@@ these registers
                                            // return all-zeroes.

#define AES_C_LENGTH_1_LENGTH_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// AES_O_AUTH_LENGTH register.
//
//******************************************************************************
#define AES_AUTH_LENGTH_AUTH_M \
                                0xFFFFFFFF  // data

#define AES_AUTH_LENGTH_AUTH_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_DATA_IN_0 register.
//
//******************************************************************************
#define AES_DATA_IN_0_DATA_M  0xFFFFFFFF  // Data to encrypt/decrypt
#define AES_DATA_IN_0_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_DATA_IN_1 register.
//
//******************************************************************************
#define AES_DATA_IN_1_DATA_M  0xFFFFFFFF  // Data to encrypt/decrypt
#define AES_DATA_IN_1_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_DATA_IN_2 register.
//
//******************************************************************************
#define AES_DATA_IN_2_DATA_M  0xFFFFFFFF  // Data to encrypt/decrypt
#define AES_DATA_IN_2_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_DATA_IN_3 register.
//
//******************************************************************************
#define AES_DATA_IN_3_DATA_M  0xFFFFFFFF  // Data to encrypt/decrypt
#define AES_DATA_IN_3_DATA_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_TAG_OUT_0 register.
//
//******************************************************************************
#define AES_TAG_OUT_0_HASH_M  0xFFFFFFFF  // Hash result (MSW)
#define AES_TAG_OUT_0_HASH_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_TAG_OUT_1 register.
//
//******************************************************************************
#define AES_TAG_OUT_1_HASH_M  0xFFFFFFFF  // Hash result (MSW)
#define AES_TAG_OUT_1_HASH_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_TAG_OUT_2 register.
//
//******************************************************************************
#define AES_TAG_OUT_2_HASH_M  0xFFFFFFFF  // Hash result (MSW)
#define AES_TAG_OUT_2_HASH_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_TAG_OUT_3 register.
//
//******************************************************************************
#define AES_TAG_OUT_3_HASH_M  0xFFFFFFFF  // Hash result (LSW)
#define AES_TAG_OUT_3_HASH_S  0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_REVISION register.
//
//******************************************************************************
#define AES_REVISION_SCHEME_M 0xC0000000
#define AES_REVISION_SCHEME_S 30
#define AES_REVISION_FUNC_M   0x0FFF0000  // Function indicates a software
                                            // compatible module family. If
                                            // there is no level of software
                                            // compatibility a new Func number
                                            // (and hence REVISION) should be
                                            // assigned.
#define AES_REVISION_FUNC_S   16
#define AES_REVISION_R_RTL_M  0x0000F800  // RTL Version (R)@@ maintained by
                                            // IP design owner. RTL follows a
                                            // numbering such as X.Y.R.Z which
                                            // are explained in this table. R
                                            // changes ONLY when: (1) PDS
                                            // uploads occur which may have been
                                            // due to spec changes (2) Bug fixes
                                            // occur (3) Resets to '0' when X or
                                            // Y changes. Design team has an
                                            // internal 'Z' (customer invisible)
                                            // number which increments on every
                                            // drop that happens due to DV and
                                            // RTL updates. Z resets to 0 when R
                                            // increments.
#define AES_REVISION_R_RTL_S  11
#define AES_REVISION_X_MAJOR_M \
                                0x00000700  // Major Revision (X)@@ maintained
                                            // by IP specification owner. X
                                            // changes ONLY when: (1) There is a
                                            // major feature addition. An
                                            // example would be adding Master
                                            // Mode to Utopia Level2. The Func
                                            // field (or Class/Type in old PID
                                            // format) will remain the same. X
                                            // does NOT change due to: (1) Bug
                                            // fixes (2) Change in feature
                                            // parameters.

#define AES_REVISION_X_MAJOR_S 8
#define AES_REVISION_CUSTOM_M 0x000000C0
#define AES_REVISION_CUSTOM_S 6
#define AES_REVISION_Y_MINOR_M \
                                0x0000003F  // Minor Revision (Y)@@ maintained
                                            // by IP specification owner. Y
                                            // changes ONLY when: (1) Features
                                            // are scaled (up or down).
                                            // Flexibility exists in that this
                                            // feature scalability may either be
                                            // represented in the Y change or a
                                            // specific register in the IP that
                                            // indicates which features are
                                            // exactly available. (2) When
                                            // feature creeps from Is-Not list
                                            // to Is list. But this may not be
                                            // the case once it sees silicon; in
                                            // which case X will change. Y does
                                            // NOT change due to: (1) Bug fixes
                                            // (2) Typos or clarifications (3)
                                            // major functional/feature
                                            // change/addition/deletion. Instead
                                            // these changes may be reflected
                                            // via R@@ S@@ X as applicable. Spec
                                            // owner maintains a
                                            // customer-invisible number 'S'
                                            // which changes due to: (1)
                                            // Typos/clarifications (2) Bug
                                            // documentation. Note that this bug
                                            // is not due to a spec change but
                                            // due to implementation.
                                            // Nevertheless@@ the spec tracks
                                            // the IP bugs. An RTL release (say
                                            // for silicon PG1.1) that occurs
                                            // due to bug fix should document
                                            // the corresponding spec number
                                            // (X.Y.S) in its release notes.

#define AES_REVISION_Y_MINOR_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_SYSCONFIG register.
//
//******************************************************************************
#define AES_SYSCONFIG_MACONTEXT_OUT_ON_DATA_OUT \
                                0x00000200  // If set to '1' the two context
                                            // out requests
                                            // (dma_req_context_out_en@@ Bit [8]
                                            // above@@ and context_out interrupt
                                            // enable@@ Bit [3] of AES_IRQENABLE
                                            // register) are mapped on the
                                            // corresponding data output request
                                            // bit. In this case@@ the original
                                            // ‘context out’ bit values are
                                            // ignored.

#define AES_SYSCONFIG_DMA_REQ_CONTEXT_OUT_EN \
                                0x00000100  // If set to ‘1’@@ the DMA context
                                            // output request is enabled (for
                                            // context data out@@ e.g. TAG for
                                            // authentication modes). 0 Dma
                                            // disabled 1 Dma enabled

#define AES_SYSCONFIG_DMA_REQ_CONTEXT_IN_EN \
                                0x00000080  // If set to ‘1’@@ the DMA context
                                            // request is enabled. 0 Dma
                                            // disabled 1 Dma enabled

#define AES_SYSCONFIG_DMA_REQ_DATA_OUT_EN \
                                0x00000040  // If set to ‘1’@@ the DMA output
                                            // request is enabled. 0 Dma
                                            // disabled 1 Dma enabled

#define AES_SYSCONFIG_DMA_REQ_DATA_IN_EN \
                                0x00000020  // If set to ‘1’@@ the DMA input
                                            // request is enabled. 0 Dma
                                            // disabled 1 Dma enabled

//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_SYSSTATUS register.
//
//******************************************************************************
#define AES_SYSSTATUS_RESETDONE \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_IRQSTATUS register.
//
//******************************************************************************
#define AES_IRQSTATUS_CONTEXT_OUT \
                                0x00000008  // This bit indicates
                                            // authentication tag (and IV)
                                            // interrupt(s) is/are active and
                                            // triggers the interrupt output.

#define AES_IRQSTATUS_DATA_OUT \
                                0x00000004  // This bit indicates data output
                                            // interrupt is active and triggers
                                            // the interrupt output.

#define AES_IRQSTATUS_DATA_IN 0x00000002  // This bit indicates data input
                                            // interrupt is active and triggers
                                            // the interrupt output.
#define AES_IRQSTATUS_CONTEX_IN \
                                0x00000001  // This bit indicates context
                                            // interrupt is active and triggers
                                            // the interrupt output.

//******************************************************************************
//
// The following are defines for the bit fields in the AES_O_IRQENABLE register.
//
//******************************************************************************
#define AES_IRQENABLE_CONTEXT_OUT \
                                0x00000008  // This bit indicates
                                            // authentication tag (and IV)
                                            // interrupt(s) is/are active and
                                            // triggers the interrupt output.

#define AES_IRQENABLE_DATA_OUT \
                                0x00000004  // This bit indicates data output
                                            // interrupt is active and triggers
                                            // the interrupt output.

#define AES_IRQENABLE_DATA_IN 0x00000002  // This bit indicates data input
                                            // interrupt is active and triggers
                                            // the interrupt output.
#define AES_IRQENABLE_CONTEX_IN \
                                0x00000001  // This bit indicates context
                                            // interrupt is active and triggers
                                            // the interrupt output.




#endif // __HW_AES_H__
