/**
 * \file
 *
 * \brief Component description for AES
 *
 * Copyright (c) 2017 Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD51_AES_COMPONENT_
#define _SAMD51_AES_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR AES */
/* ========================================================================== */
/** \addtogroup SAMD51_AES Advanced Encryption Standard */
/*@{*/

#define AES_U2238
#define REV_AES                     0x220

/* -------- AES_CTRLA : (AES Offset: 0x00) (R/W 32) Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable                             */
    uint32_t AESMODE:3;        /*!< bit:  2.. 4  AES Modes of operation             */
    uint32_t CFBS:3;           /*!< bit:  5.. 7  Cipher Feedback Block Size         */
    uint32_t KEYSIZE:2;        /*!< bit:  8.. 9  Encryption Key Size                */
    uint32_t CIPHER:1;         /*!< bit:     10  Cipher Mode                        */
    uint32_t STARTMODE:1;      /*!< bit:     11  Start Mode Select                  */
    uint32_t LOD:1;            /*!< bit:     12  Last Output Data Mode              */
    uint32_t KEYGEN:1;         /*!< bit:     13  Last Key Generation                */
    uint32_t XORKEY:1;         /*!< bit:     14  XOR Key Operation                  */
    uint32_t :1;               /*!< bit:     15  Reserved                           */
    uint32_t CTYPE:4;          /*!< bit: 16..19  Counter Measure Type               */
    uint32_t :12;              /*!< bit: 20..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} AES_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AES_CTRLA_OFFSET            0x00         /**< \brief (AES_CTRLA offset) Control A */
#define AES_CTRLA_RESETVALUE        _U_(0x00000000) /**< \brief (AES_CTRLA reset_value) Control A */

#define AES_CTRLA_SWRST_Pos         0            /**< \brief (AES_CTRLA) Software Reset */
#define AES_CTRLA_SWRST             (_U_(0x1) << AES_CTRLA_SWRST_Pos)
#define AES_CTRLA_ENABLE_Pos        1            /**< \brief (AES_CTRLA) Enable */
#define AES_CTRLA_ENABLE            (_U_(0x1) << AES_CTRLA_ENABLE_Pos)
#define AES_CTRLA_AESMODE_Pos       2            /**< \brief (AES_CTRLA) AES Modes of operation */
#define AES_CTRLA_AESMODE_Msk       (_U_(0x7) << AES_CTRLA_AESMODE_Pos)
#define AES_CTRLA_AESMODE(value)    (AES_CTRLA_AESMODE_Msk & ((value) << AES_CTRLA_AESMODE_Pos))
#define   AES_CTRLA_AESMODE_ECB_Val       _U_(0x0)   /**< \brief (AES_CTRLA) Electronic code book mode */
#define   AES_CTRLA_AESMODE_CBC_Val       _U_(0x1)   /**< \brief (AES_CTRLA) Cipher block chaining mode */
#define   AES_CTRLA_AESMODE_OFB_Val       _U_(0x2)   /**< \brief (AES_CTRLA) Output feedback mode */
#define   AES_CTRLA_AESMODE_CFB_Val       _U_(0x3)   /**< \brief (AES_CTRLA) Cipher feedback mode */
#define   AES_CTRLA_AESMODE_COUNTER_Val   _U_(0x4)   /**< \brief (AES_CTRLA) Counter mode */
#define   AES_CTRLA_AESMODE_CCM_Val       _U_(0x5)   /**< \brief (AES_CTRLA) CCM mode */
#define   AES_CTRLA_AESMODE_GCM_Val       _U_(0x6)   /**< \brief (AES_CTRLA) Galois counter mode */
#define AES_CTRLA_AESMODE_ECB       (AES_CTRLA_AESMODE_ECB_Val     << AES_CTRLA_AESMODE_Pos)
#define AES_CTRLA_AESMODE_CBC       (AES_CTRLA_AESMODE_CBC_Val     << AES_CTRLA_AESMODE_Pos)
#define AES_CTRLA_AESMODE_OFB       (AES_CTRLA_AESMODE_OFB_Val     << AES_CTRLA_AESMODE_Pos)
#define AES_CTRLA_AESMODE_CFB       (AES_CTRLA_AESMODE_CFB_Val     << AES_CTRLA_AESMODE_Pos)
#define AES_CTRLA_AESMODE_COUNTER   (AES_CTRLA_AESMODE_COUNTER_Val << AES_CTRLA_AESMODE_Pos)
#define AES_CTRLA_AESMODE_CCM       (AES_CTRLA_AESMODE_CCM_Val     << AES_CTRLA_AESMODE_Pos)
#define AES_CTRLA_AESMODE_GCM       (AES_CTRLA_AESMODE_GCM_Val     << AES_CTRLA_AESMODE_Pos)
#define AES_CTRLA_CFBS_Pos          5            /**< \brief (AES_CTRLA) Cipher Feedback Block Size */
#define AES_CTRLA_CFBS_Msk          (_U_(0x7) << AES_CTRLA_CFBS_Pos)
#define AES_CTRLA_CFBS(value)       (AES_CTRLA_CFBS_Msk & ((value) << AES_CTRLA_CFBS_Pos))
#define   AES_CTRLA_CFBS_128BIT_Val       _U_(0x0)   /**< \brief (AES_CTRLA) 128-bit Input data block for Encryption/Decryption in Cipher Feedback mode */
#define   AES_CTRLA_CFBS_64BIT_Val        _U_(0x1)   /**< \brief (AES_CTRLA) 64-bit Input data block for Encryption/Decryption in Cipher Feedback mode */
#define   AES_CTRLA_CFBS_32BIT_Val        _U_(0x2)   /**< \brief (AES_CTRLA) 32-bit Input data block for Encryption/Decryption in Cipher Feedback mode */
#define   AES_CTRLA_CFBS_16BIT_Val        _U_(0x3)   /**< \brief (AES_CTRLA) 16-bit Input data block for Encryption/Decryption in Cipher Feedback mode */
#define   AES_CTRLA_CFBS_8BIT_Val         _U_(0x4)   /**< \brief (AES_CTRLA) 8-bit Input data block for Encryption/Decryption in Cipher Feedback mode */
#define AES_CTRLA_CFBS_128BIT       (AES_CTRLA_CFBS_128BIT_Val     << AES_CTRLA_CFBS_Pos)
#define AES_CTRLA_CFBS_64BIT        (AES_CTRLA_CFBS_64BIT_Val      << AES_CTRLA_CFBS_Pos)
#define AES_CTRLA_CFBS_32BIT        (AES_CTRLA_CFBS_32BIT_Val      << AES_CTRLA_CFBS_Pos)
#define AES_CTRLA_CFBS_16BIT        (AES_CTRLA_CFBS_16BIT_Val      << AES_CTRLA_CFBS_Pos)
#define AES_CTRLA_CFBS_8BIT         (AES_CTRLA_CFBS_8BIT_Val       << AES_CTRLA_CFBS_Pos)
#define AES_CTRLA_KEYSIZE_Pos       8            /**< \brief (AES_CTRLA) Encryption Key Size */
#define AES_CTRLA_KEYSIZE_Msk       (_U_(0x3) << AES_CTRLA_KEYSIZE_Pos)
#define AES_CTRLA_KEYSIZE(value)    (AES_CTRLA_KEYSIZE_Msk & ((value) << AES_CTRLA_KEYSIZE_Pos))
#define   AES_CTRLA_KEYSIZE_128BIT_Val    _U_(0x0)   /**< \brief (AES_CTRLA) 128-bit Key for Encryption / Decryption */
#define   AES_CTRLA_KEYSIZE_192BIT_Val    _U_(0x1)   /**< \brief (AES_CTRLA) 192-bit Key for Encryption / Decryption */
#define   AES_CTRLA_KEYSIZE_256BIT_Val    _U_(0x2)   /**< \brief (AES_CTRLA) 256-bit Key for Encryption / Decryption */
#define AES_CTRLA_KEYSIZE_128BIT    (AES_CTRLA_KEYSIZE_128BIT_Val  << AES_CTRLA_KEYSIZE_Pos)
#define AES_CTRLA_KEYSIZE_192BIT    (AES_CTRLA_KEYSIZE_192BIT_Val  << AES_CTRLA_KEYSIZE_Pos)
#define AES_CTRLA_KEYSIZE_256BIT    (AES_CTRLA_KEYSIZE_256BIT_Val  << AES_CTRLA_KEYSIZE_Pos)
#define AES_CTRLA_CIPHER_Pos        10           /**< \brief (AES_CTRLA) Cipher Mode */
#define AES_CTRLA_CIPHER            (_U_(0x1) << AES_CTRLA_CIPHER_Pos)
#define   AES_CTRLA_CIPHER_DEC_Val        _U_(0x0)   /**< \brief (AES_CTRLA) Decryption */
#define   AES_CTRLA_CIPHER_ENC_Val        _U_(0x1)   /**< \brief (AES_CTRLA) Encryption */
#define AES_CTRLA_CIPHER_DEC        (AES_CTRLA_CIPHER_DEC_Val      << AES_CTRLA_CIPHER_Pos)
#define AES_CTRLA_CIPHER_ENC        (AES_CTRLA_CIPHER_ENC_Val      << AES_CTRLA_CIPHER_Pos)
#define AES_CTRLA_STARTMODE_Pos     11           /**< \brief (AES_CTRLA) Start Mode Select */
#define AES_CTRLA_STARTMODE         (_U_(0x1) << AES_CTRLA_STARTMODE_Pos)
#define   AES_CTRLA_STARTMODE_MANUAL_Val  _U_(0x0)   /**< \brief (AES_CTRLA) Start Encryption / Decryption in Manual mode */
#define   AES_CTRLA_STARTMODE_AUTO_Val    _U_(0x1)   /**< \brief (AES_CTRLA) Start Encryption / Decryption in Auto mode */
#define AES_CTRLA_STARTMODE_MANUAL  (AES_CTRLA_STARTMODE_MANUAL_Val << AES_CTRLA_STARTMODE_Pos)
#define AES_CTRLA_STARTMODE_AUTO    (AES_CTRLA_STARTMODE_AUTO_Val  << AES_CTRLA_STARTMODE_Pos)
#define AES_CTRLA_LOD_Pos           12           /**< \brief (AES_CTRLA) Last Output Data Mode */
#define AES_CTRLA_LOD               (_U_(0x1) << AES_CTRLA_LOD_Pos)
#define   AES_CTRLA_LOD_NONE_Val          _U_(0x0)   /**< \brief (AES_CTRLA) No effect */
#define   AES_CTRLA_LOD_LAST_Val          _U_(0x1)   /**< \brief (AES_CTRLA) Start encryption in Last Output Data mode */
#define AES_CTRLA_LOD_NONE          (AES_CTRLA_LOD_NONE_Val        << AES_CTRLA_LOD_Pos)
#define AES_CTRLA_LOD_LAST          (AES_CTRLA_LOD_LAST_Val        << AES_CTRLA_LOD_Pos)
#define AES_CTRLA_KEYGEN_Pos        13           /**< \brief (AES_CTRLA) Last Key Generation */
#define AES_CTRLA_KEYGEN            (_U_(0x1) << AES_CTRLA_KEYGEN_Pos)
#define   AES_CTRLA_KEYGEN_NONE_Val       _U_(0x0)   /**< \brief (AES_CTRLA) No effect */
#define   AES_CTRLA_KEYGEN_LAST_Val       _U_(0x1)   /**< \brief (AES_CTRLA) Start Computation of the last NK words of the expanded key */
#define AES_CTRLA_KEYGEN_NONE       (AES_CTRLA_KEYGEN_NONE_Val     << AES_CTRLA_KEYGEN_Pos)
#define AES_CTRLA_KEYGEN_LAST       (AES_CTRLA_KEYGEN_LAST_Val     << AES_CTRLA_KEYGEN_Pos)
#define AES_CTRLA_XORKEY_Pos        14           /**< \brief (AES_CTRLA) XOR Key Operation */
#define AES_CTRLA_XORKEY            (_U_(0x1) << AES_CTRLA_XORKEY_Pos)
#define   AES_CTRLA_XORKEY_NONE_Val       _U_(0x0)   /**< \brief (AES_CTRLA) No effect */
#define   AES_CTRLA_XORKEY_XOR_Val        _U_(0x1)   /**< \brief (AES_CTRLA) The user keyword gets XORed with the previous keyword register content. */
#define AES_CTRLA_XORKEY_NONE       (AES_CTRLA_XORKEY_NONE_Val     << AES_CTRLA_XORKEY_Pos)
#define AES_CTRLA_XORKEY_XOR        (AES_CTRLA_XORKEY_XOR_Val      << AES_CTRLA_XORKEY_Pos)
#define AES_CTRLA_CTYPE_Pos         16           /**< \brief (AES_CTRLA) Counter Measure Type */
#define AES_CTRLA_CTYPE_Msk         (_U_(0xF) << AES_CTRLA_CTYPE_Pos)
#define AES_CTRLA_CTYPE(value)      (AES_CTRLA_CTYPE_Msk & ((value) << AES_CTRLA_CTYPE_Pos))
#define AES_CTRLA_MASK              _U_(0x000F7FFF) /**< \brief (AES_CTRLA) MASK Register */

/* -------- AES_CTRLB : (AES Offset: 0x04) (R/W  8) Control B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  START:1;          /*!< bit:      0  Start Encryption/Decryption        */
    uint8_t  NEWMSG:1;         /*!< bit:      1  New message                        */
    uint8_t  EOM:1;            /*!< bit:      2  End of message                     */
    uint8_t  GFMUL:1;          /*!< bit:      3  GF Multiplication                  */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} AES_CTRLB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AES_CTRLB_OFFSET            0x04         /**< \brief (AES_CTRLB offset) Control B */
#define AES_CTRLB_RESETVALUE        _U_(0x00)    /**< \brief (AES_CTRLB reset_value) Control B */

#define AES_CTRLB_START_Pos         0            /**< \brief (AES_CTRLB) Start Encryption/Decryption */
#define AES_CTRLB_START             (_U_(0x1) << AES_CTRLB_START_Pos)
#define AES_CTRLB_NEWMSG_Pos        1            /**< \brief (AES_CTRLB) New message */
#define AES_CTRLB_NEWMSG            (_U_(0x1) << AES_CTRLB_NEWMSG_Pos)
#define AES_CTRLB_EOM_Pos           2            /**< \brief (AES_CTRLB) End of message */
#define AES_CTRLB_EOM               (_U_(0x1) << AES_CTRLB_EOM_Pos)
#define AES_CTRLB_GFMUL_Pos         3            /**< \brief (AES_CTRLB) GF Multiplication */
#define AES_CTRLB_GFMUL             (_U_(0x1) << AES_CTRLB_GFMUL_Pos)
#define AES_CTRLB_MASK              _U_(0x0F)    /**< \brief (AES_CTRLB) MASK Register */

/* -------- AES_INTENCLR : (AES Offset: 0x05) (R/W  8) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  ENCCMP:1;         /*!< bit:      0  Encryption Complete Interrupt Enable */
    uint8_t  GFMCMP:1;         /*!< bit:      1  GF Multiplication Complete Interrupt Enable */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} AES_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AES_INTENCLR_OFFSET         0x05         /**< \brief (AES_INTENCLR offset) Interrupt Enable Clear */
#define AES_INTENCLR_RESETVALUE     _U_(0x00)    /**< \brief (AES_INTENCLR reset_value) Interrupt Enable Clear */

#define AES_INTENCLR_ENCCMP_Pos     0            /**< \brief (AES_INTENCLR) Encryption Complete Interrupt Enable */
#define AES_INTENCLR_ENCCMP         (_U_(0x1) << AES_INTENCLR_ENCCMP_Pos)
#define AES_INTENCLR_GFMCMP_Pos     1            /**< \brief (AES_INTENCLR) GF Multiplication Complete Interrupt Enable */
#define AES_INTENCLR_GFMCMP         (_U_(0x1) << AES_INTENCLR_GFMCMP_Pos)
#define AES_INTENCLR_MASK           _U_(0x03)    /**< \brief (AES_INTENCLR) MASK Register */

/* -------- AES_INTENSET : (AES Offset: 0x06) (R/W  8) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  ENCCMP:1;         /*!< bit:      0  Encryption Complete Interrupt Enable */
    uint8_t  GFMCMP:1;         /*!< bit:      1  GF Multiplication Complete Interrupt Enable */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} AES_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AES_INTENSET_OFFSET         0x06         /**< \brief (AES_INTENSET offset) Interrupt Enable Set */
#define AES_INTENSET_RESETVALUE     _U_(0x00)    /**< \brief (AES_INTENSET reset_value) Interrupt Enable Set */

#define AES_INTENSET_ENCCMP_Pos     0            /**< \brief (AES_INTENSET) Encryption Complete Interrupt Enable */
#define AES_INTENSET_ENCCMP         (_U_(0x1) << AES_INTENSET_ENCCMP_Pos)
#define AES_INTENSET_GFMCMP_Pos     1            /**< \brief (AES_INTENSET) GF Multiplication Complete Interrupt Enable */
#define AES_INTENSET_GFMCMP         (_U_(0x1) << AES_INTENSET_GFMCMP_Pos)
#define AES_INTENSET_MASK           _U_(0x03)    /**< \brief (AES_INTENSET) MASK Register */

/* -------- AES_INTFLAG : (AES Offset: 0x07) (R/W  8) Interrupt Flag Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint8_t  ENCCMP:1;         /*!< bit:      0  Encryption Complete                */
    __I uint8_t  GFMCMP:1;         /*!< bit:      1  GF Multiplication Complete         */
    __I uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} AES_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AES_INTFLAG_OFFSET          0x07         /**< \brief (AES_INTFLAG offset) Interrupt Flag Status */
#define AES_INTFLAG_RESETVALUE      _U_(0x00)    /**< \brief (AES_INTFLAG reset_value) Interrupt Flag Status */

#define AES_INTFLAG_ENCCMP_Pos      0            /**< \brief (AES_INTFLAG) Encryption Complete */
#define AES_INTFLAG_ENCCMP          (_U_(0x1) << AES_INTFLAG_ENCCMP_Pos)
#define AES_INTFLAG_GFMCMP_Pos      1            /**< \brief (AES_INTFLAG) GF Multiplication Complete */
#define AES_INTFLAG_GFMCMP          (_U_(0x1) << AES_INTFLAG_GFMCMP_Pos)
#define AES_INTFLAG_MASK            _U_(0x03)    /**< \brief (AES_INTFLAG) MASK Register */

/* -------- AES_DATABUFPTR : (AES Offset: 0x08) (R/W  8) Data buffer pointer -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  INDATAPTR:2;      /*!< bit:  0.. 1  Input Data Pointer                 */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} AES_DATABUFPTR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AES_DATABUFPTR_OFFSET       0x08         /**< \brief (AES_DATABUFPTR offset) Data buffer pointer */
#define AES_DATABUFPTR_RESETVALUE   _U_(0x00)    /**< \brief (AES_DATABUFPTR reset_value) Data buffer pointer */

#define AES_DATABUFPTR_INDATAPTR_Pos 0            /**< \brief (AES_DATABUFPTR) Input Data Pointer */
#define AES_DATABUFPTR_INDATAPTR_Msk (_U_(0x3) << AES_DATABUFPTR_INDATAPTR_Pos)
#define AES_DATABUFPTR_INDATAPTR(value) (AES_DATABUFPTR_INDATAPTR_Msk & ((value) << AES_DATABUFPTR_INDATAPTR_Pos))
#define AES_DATABUFPTR_MASK         _U_(0x03)    /**< \brief (AES_DATABUFPTR) MASK Register */

/* -------- AES_DBGCTRL : (AES Offset: 0x09) (R/W  8) Debug control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DBGRUN:1;         /*!< bit:      0  Debug Run                          */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} AES_DBGCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AES_DBGCTRL_OFFSET          0x09         /**< \brief (AES_DBGCTRL offset) Debug control */
#define AES_DBGCTRL_RESETVALUE      _U_(0x00)    /**< \brief (AES_DBGCTRL reset_value) Debug control */

#define AES_DBGCTRL_DBGRUN_Pos      0            /**< \brief (AES_DBGCTRL) Debug Run */
#define AES_DBGCTRL_DBGRUN          (_U_(0x1) << AES_DBGCTRL_DBGRUN_Pos)
#define AES_DBGCTRL_MASK            _U_(0x01)    /**< \brief (AES_DBGCTRL) MASK Register */

/* -------- AES_KEYWORD : (AES Offset: 0x0C) ( /W 32) Keyword n -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} AES_KEYWORD_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AES_KEYWORD_OFFSET          0x0C         /**< \brief (AES_KEYWORD offset) Keyword n */
#define AES_KEYWORD_RESETVALUE      _U_(0x00000000) /**< \brief (AES_KEYWORD reset_value) Keyword n */
#define AES_KEYWORD_MASK            _U_(0xFFFFFFFF) /**< \brief (AES_KEYWORD) MASK Register */

/* -------- AES_INDATA : (AES Offset: 0x38) (R/W 32) Indata -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} AES_INDATA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AES_INDATA_OFFSET           0x38         /**< \brief (AES_INDATA offset) Indata */
#define AES_INDATA_RESETVALUE       _U_(0x00000000) /**< \brief (AES_INDATA reset_value) Indata */
#define AES_INDATA_MASK             _U_(0xFFFFFFFF) /**< \brief (AES_INDATA) MASK Register */

/* -------- AES_INTVECTV : (AES Offset: 0x3C) ( /W 32) Initialisation Vector n -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} AES_INTVECTV_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AES_INTVECTV_OFFSET         0x3C         /**< \brief (AES_INTVECTV offset) Initialisation Vector n */
#define AES_INTVECTV_RESETVALUE     _U_(0x00000000) /**< \brief (AES_INTVECTV reset_value) Initialisation Vector n */
#define AES_INTVECTV_MASK           _U_(0xFFFFFFFF) /**< \brief (AES_INTVECTV) MASK Register */

/* -------- AES_HASHKEY : (AES Offset: 0x5C) (R/W 32) Hash key n -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} AES_HASHKEY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AES_HASHKEY_OFFSET          0x5C         /**< \brief (AES_HASHKEY offset) Hash key n */
#define AES_HASHKEY_RESETVALUE      _U_(0x00000000) /**< \brief (AES_HASHKEY reset_value) Hash key n */
#define AES_HASHKEY_MASK            _U_(0xFFFFFFFF) /**< \brief (AES_HASHKEY) MASK Register */

/* -------- AES_GHASH : (AES Offset: 0x6C) (R/W 32) Galois Hash n -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} AES_GHASH_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AES_GHASH_OFFSET            0x6C         /**< \brief (AES_GHASH offset) Galois Hash n */
#define AES_GHASH_RESETVALUE        _U_(0x00000000) /**< \brief (AES_GHASH reset_value) Galois Hash n */
#define AES_GHASH_MASK              _U_(0xFFFFFFFF) /**< \brief (AES_GHASH) MASK Register */

/* -------- AES_CIPLEN : (AES Offset: 0x80) (R/W 32) Cipher Length -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} AES_CIPLEN_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AES_CIPLEN_OFFSET           0x80         /**< \brief (AES_CIPLEN offset) Cipher Length */
#define AES_CIPLEN_RESETVALUE       _U_(0x00000000) /**< \brief (AES_CIPLEN reset_value) Cipher Length */
#define AES_CIPLEN_MASK             _U_(0xFFFFFFFF) /**< \brief (AES_CIPLEN) MASK Register */

/* -------- AES_RANDSEED : (AES Offset: 0x84) (R/W 32) Random Seed -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} AES_RANDSEED_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AES_RANDSEED_OFFSET         0x84         /**< \brief (AES_RANDSEED offset) Random Seed */
#define AES_RANDSEED_RESETVALUE     _U_(0x00000000) /**< \brief (AES_RANDSEED reset_value) Random Seed */
#define AES_RANDSEED_MASK           _U_(0xFFFFFFFF) /**< \brief (AES_RANDSEED) MASK Register */

/** \brief AES hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO AES_CTRLA_Type            CTRLA;       /**< \brief Offset: 0x00 (R/W 32) Control A */
  __IO AES_CTRLB_Type            CTRLB;       /**< \brief Offset: 0x04 (R/W  8) Control B */
  __IO AES_INTENCLR_Type         INTENCLR;    /**< \brief Offset: 0x05 (R/W  8) Interrupt Enable Clear */
  __IO AES_INTENSET_Type         INTENSET;    /**< \brief Offset: 0x06 (R/W  8) Interrupt Enable Set */
  __IO AES_INTFLAG_Type          INTFLAG;     /**< \brief Offset: 0x07 (R/W  8) Interrupt Flag Status */
  __IO AES_DATABUFPTR_Type       DATABUFPTR;  /**< \brief Offset: 0x08 (R/W  8) Data buffer pointer */
  __IO AES_DBGCTRL_Type          DBGCTRL;     /**< \brief Offset: 0x09 (R/W  8) Debug control */
       RoReg8                    Reserved1[0x2];
  __O  AES_KEYWORD_Type          KEYWORD[8];  /**< \brief Offset: 0x0C ( /W 32) Keyword n */
       RoReg8                    Reserved2[0xC];
  __IO AES_INDATA_Type           INDATA;      /**< \brief Offset: 0x38 (R/W 32) Indata */
  __O  AES_INTVECTV_Type         INTVECTV[4]; /**< \brief Offset: 0x3C ( /W 32) Initialisation Vector n */
       RoReg8                    Reserved3[0x10];
  __IO AES_HASHKEY_Type          HASHKEY[4];  /**< \brief Offset: 0x5C (R/W 32) Hash key n */
  __IO AES_GHASH_Type            GHASH[4];    /**< \brief Offset: 0x6C (R/W 32) Galois Hash n */
       RoReg8                    Reserved4[0x4];
  __IO AES_CIPLEN_Type           CIPLEN;      /**< \brief Offset: 0x80 (R/W 32) Cipher Length */
  __IO AES_RANDSEED_Type         RANDSEED;    /**< \brief Offset: 0x84 (R/W 32) Random Seed */
} Aes;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_AES_COMPONENT_ */
