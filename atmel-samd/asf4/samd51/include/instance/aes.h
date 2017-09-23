/**
 * \file
 *
 * \brief Instance description for AES
 *
 * Copyright (c) 2017 Atmel Corporation,
 *                    a wholly owned subsidiary of Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD51_AES_INSTANCE_
#define _SAMD51_AES_INSTANCE_

/* ========== Register definition for AES peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_AES_CTRLA              (0x42002400) /**< \brief (AES) Control A */
#define REG_AES_CTRLB              (0x42002404) /**< \brief (AES) Control B */
#define REG_AES_INTENCLR           (0x42002405) /**< \brief (AES) Interrupt Enable Clear */
#define REG_AES_INTENSET           (0x42002406) /**< \brief (AES) Interrupt Enable Set */
#define REG_AES_INTFLAG            (0x42002407) /**< \brief (AES) Interrupt Flag Status */
#define REG_AES_DATABUFPTR         (0x42002408) /**< \brief (AES) Data buffer pointer */
#define REG_AES_DBGCTRL            (0x42002409) /**< \brief (AES) Debug control */
#define REG_AES_KEYWORD0           (0x4200240C) /**< \brief (AES) Keyword 0 */
#define REG_AES_KEYWORD1           (0x42002410) /**< \brief (AES) Keyword 1 */
#define REG_AES_KEYWORD2           (0x42002414) /**< \brief (AES) Keyword 2 */
#define REG_AES_KEYWORD3           (0x42002418) /**< \brief (AES) Keyword 3 */
#define REG_AES_KEYWORD4           (0x4200241C) /**< \brief (AES) Keyword 4 */
#define REG_AES_KEYWORD5           (0x42002420) /**< \brief (AES) Keyword 5 */
#define REG_AES_KEYWORD6           (0x42002424) /**< \brief (AES) Keyword 6 */
#define REG_AES_KEYWORD7           (0x42002428) /**< \brief (AES) Keyword 7 */
#define REG_AES_INDATA             (0x42002438) /**< \brief (AES) Indata */
#define REG_AES_INTVECTV0          (0x4200243C) /**< \brief (AES) Initialisation Vector 0 */
#define REG_AES_INTVECTV1          (0x42002440) /**< \brief (AES) Initialisation Vector 1 */
#define REG_AES_INTVECTV2          (0x42002444) /**< \brief (AES) Initialisation Vector 2 */
#define REG_AES_INTVECTV3          (0x42002448) /**< \brief (AES) Initialisation Vector 3 */
#define REG_AES_HASHKEY0           (0x4200245C) /**< \brief (AES) Hash key 0 */
#define REG_AES_HASHKEY1           (0x42002460) /**< \brief (AES) Hash key 1 */
#define REG_AES_HASHKEY2           (0x42002464) /**< \brief (AES) Hash key 2 */
#define REG_AES_HASHKEY3           (0x42002468) /**< \brief (AES) Hash key 3 */
#define REG_AES_GHASH0             (0x4200246C) /**< \brief (AES) Galois Hash 0 */
#define REG_AES_GHASH1             (0x42002470) /**< \brief (AES) Galois Hash 1 */
#define REG_AES_GHASH2             (0x42002474) /**< \brief (AES) Galois Hash 2 */
#define REG_AES_GHASH3             (0x42002478) /**< \brief (AES) Galois Hash 3 */
#define REG_AES_CIPLEN             (0x42002480) /**< \brief (AES) Cipher Length */
#define REG_AES_RANDSEED           (0x42002484) /**< \brief (AES) Random Seed */
#else
#define REG_AES_CTRLA              (*(RwReg  *)0x42002400UL) /**< \brief (AES) Control A */
#define REG_AES_CTRLB              (*(RwReg8 *)0x42002404UL) /**< \brief (AES) Control B */
#define REG_AES_INTENCLR           (*(RwReg8 *)0x42002405UL) /**< \brief (AES) Interrupt Enable Clear */
#define REG_AES_INTENSET           (*(RwReg8 *)0x42002406UL) /**< \brief (AES) Interrupt Enable Set */
#define REG_AES_INTFLAG            (*(RwReg8 *)0x42002407UL) /**< \brief (AES) Interrupt Flag Status */
#define REG_AES_DATABUFPTR         (*(RwReg8 *)0x42002408UL) /**< \brief (AES) Data buffer pointer */
#define REG_AES_DBGCTRL            (*(RwReg8 *)0x42002409UL) /**< \brief (AES) Debug control */
#define REG_AES_KEYWORD0           (*(WoReg  *)0x4200240CUL) /**< \brief (AES) Keyword 0 */
#define REG_AES_KEYWORD1           (*(WoReg  *)0x42002410UL) /**< \brief (AES) Keyword 1 */
#define REG_AES_KEYWORD2           (*(WoReg  *)0x42002414UL) /**< \brief (AES) Keyword 2 */
#define REG_AES_KEYWORD3           (*(WoReg  *)0x42002418UL) /**< \brief (AES) Keyword 3 */
#define REG_AES_KEYWORD4           (*(WoReg  *)0x4200241CUL) /**< \brief (AES) Keyword 4 */
#define REG_AES_KEYWORD5           (*(WoReg  *)0x42002420UL) /**< \brief (AES) Keyword 5 */
#define REG_AES_KEYWORD6           (*(WoReg  *)0x42002424UL) /**< \brief (AES) Keyword 6 */
#define REG_AES_KEYWORD7           (*(WoReg  *)0x42002428UL) /**< \brief (AES) Keyword 7 */
#define REG_AES_INDATA             (*(RwReg  *)0x42002438UL) /**< \brief (AES) Indata */
#define REG_AES_INTVECTV0          (*(WoReg  *)0x4200243CUL) /**< \brief (AES) Initialisation Vector 0 */
#define REG_AES_INTVECTV1          (*(WoReg  *)0x42002440UL) /**< \brief (AES) Initialisation Vector 1 */
#define REG_AES_INTVECTV2          (*(WoReg  *)0x42002444UL) /**< \brief (AES) Initialisation Vector 2 */
#define REG_AES_INTVECTV3          (*(WoReg  *)0x42002448UL) /**< \brief (AES) Initialisation Vector 3 */
#define REG_AES_HASHKEY0           (*(RwReg  *)0x4200245CUL) /**< \brief (AES) Hash key 0 */
#define REG_AES_HASHKEY1           (*(RwReg  *)0x42002460UL) /**< \brief (AES) Hash key 1 */
#define REG_AES_HASHKEY2           (*(RwReg  *)0x42002464UL) /**< \brief (AES) Hash key 2 */
#define REG_AES_HASHKEY3           (*(RwReg  *)0x42002468UL) /**< \brief (AES) Hash key 3 */
#define REG_AES_GHASH0             (*(RwReg  *)0x4200246CUL) /**< \brief (AES) Galois Hash 0 */
#define REG_AES_GHASH1             (*(RwReg  *)0x42002470UL) /**< \brief (AES) Galois Hash 1 */
#define REG_AES_GHASH2             (*(RwReg  *)0x42002474UL) /**< \brief (AES) Galois Hash 2 */
#define REG_AES_GHASH3             (*(RwReg  *)0x42002478UL) /**< \brief (AES) Galois Hash 3 */
#define REG_AES_CIPLEN             (*(RwReg  *)0x42002480UL) /**< \brief (AES) Cipher Length */
#define REG_AES_RANDSEED           (*(RwReg  *)0x42002484UL) /**< \brief (AES) Random Seed */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for AES peripheral ========== */
#define AES_DMAC_ID_RD              82       // DMA DATA Read trigger
#define AES_DMAC_ID_WR              81       // DMA DATA Write trigger
#define AES_FOUR_BYTE_OPERATION     1        // Byte Operation
#define AES_GCM                     1        // GCM
#define AES_KEYLEN                  2        // Key Length

#endif /* _SAMD51_AES_INSTANCE_ */
