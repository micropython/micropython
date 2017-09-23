/**
 * \file
 *
 * \brief Instance description for SBMATRIX
 *
 * Copyright (c) 2016 Atmel Corporation,
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

#ifndef _SAMD21_SBMATRIX_INSTANCE_
#define _SAMD21_SBMATRIX_INSTANCE_

/* ========== Register definition for SBMATRIX peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_SBMATRIX_PRAS0         (0x41007080) /**< \brief (SBMATRIX) Priority A for Slave 0 */
#define REG_SBMATRIX_PRBS0         (0x41007084) /**< \brief (SBMATRIX) Priority B for Slave 0 */
#define REG_SBMATRIX_PRAS1         (0x41007088) /**< \brief (SBMATRIX) Priority A for Slave 1 */
#define REG_SBMATRIX_PRBS1         (0x4100708C) /**< \brief (SBMATRIX) Priority B for Slave 1 */
#define REG_SBMATRIX_PRAS2         (0x41007090) /**< \brief (SBMATRIX) Priority A for Slave 2 */
#define REG_SBMATRIX_PRBS2         (0x41007094) /**< \brief (SBMATRIX) Priority B for Slave 2 */
#define REG_SBMATRIX_PRAS3         (0x41007098) /**< \brief (SBMATRIX) Priority A for Slave 3 */
#define REG_SBMATRIX_PRBS3         (0x4100709C) /**< \brief (SBMATRIX) Priority B for Slave 3 */
#define REG_SBMATRIX_PRAS4         (0x410070A0) /**< \brief (SBMATRIX) Priority A for Slave 4 */
#define REG_SBMATRIX_PRBS4         (0x410070A4) /**< \brief (SBMATRIX) Priority B for Slave 4 */
#define REG_SBMATRIX_PRAS5         (0x410070A8) /**< \brief (SBMATRIX) Priority A for Slave 5 */
#define REG_SBMATRIX_PRBS5         (0x410070AC) /**< \brief (SBMATRIX) Priority B for Slave 5 */
#define REG_SBMATRIX_PRAS6         (0x410070B0) /**< \brief (SBMATRIX) Priority A for Slave 6 */
#define REG_SBMATRIX_PRBS6         (0x410070B4) /**< \brief (SBMATRIX) Priority B for Slave 6 */
#define REG_SBMATRIX_PRAS7         (0x410070B8) /**< \brief (SBMATRIX) Priority A for Slave 7 */
#define REG_SBMATRIX_PRBS7         (0x410070BC) /**< \brief (SBMATRIX) Priority B for Slave 7 */
#define REG_SBMATRIX_PRAS8         (0x410070C0) /**< \brief (SBMATRIX) Priority A for Slave 8 */
#define REG_SBMATRIX_PRBS8         (0x410070C4) /**< \brief (SBMATRIX) Priority B for Slave 8 */
#define REG_SBMATRIX_PRAS9         (0x410070C8) /**< \brief (SBMATRIX) Priority A for Slave 9 */
#define REG_SBMATRIX_PRBS9         (0x410070CC) /**< \brief (SBMATRIX) Priority B for Slave 9 */
#define REG_SBMATRIX_PRAS10        (0x410070D0) /**< \brief (SBMATRIX) Priority A for Slave 10 */
#define REG_SBMATRIX_PRBS10        (0x410070D4) /**< \brief (SBMATRIX) Priority B for Slave 10 */
#define REG_SBMATRIX_PRAS11        (0x410070D8) /**< \brief (SBMATRIX) Priority A for Slave 11 */
#define REG_SBMATRIX_PRBS11        (0x410070DC) /**< \brief (SBMATRIX) Priority B for Slave 11 */
#define REG_SBMATRIX_PRAS12        (0x410070E0) /**< \brief (SBMATRIX) Priority A for Slave 12 */
#define REG_SBMATRIX_PRBS12        (0x410070E4) /**< \brief (SBMATRIX) Priority B for Slave 12 */
#define REG_SBMATRIX_PRAS13        (0x410070E8) /**< \brief (SBMATRIX) Priority A for Slave 13 */
#define REG_SBMATRIX_PRBS13        (0x410070EC) /**< \brief (SBMATRIX) Priority B for Slave 13 */
#define REG_SBMATRIX_PRAS14        (0x410070F0) /**< \brief (SBMATRIX) Priority A for Slave 14 */
#define REG_SBMATRIX_PRBS14        (0x410070F4) /**< \brief (SBMATRIX) Priority B for Slave 14 */
#define REG_SBMATRIX_PRAS15        (0x410070F8) /**< \brief (SBMATRIX) Priority A for Slave 15 */
#define REG_SBMATRIX_PRBS15        (0x410070FC) /**< \brief (SBMATRIX) Priority B for Slave 15 */
#define REG_SBMATRIX_SFR0          (0x41007110) /**< \brief (SBMATRIX) Special Function 0 */
#define REG_SBMATRIX_SFR1          (0x41007114) /**< \brief (SBMATRIX) Special Function 1 */
#define REG_SBMATRIX_SFR2          (0x41007118) /**< \brief (SBMATRIX) Special Function 2 */
#define REG_SBMATRIX_SFR3          (0x4100711C) /**< \brief (SBMATRIX) Special Function 3 */
#define REG_SBMATRIX_SFR4          (0x41007120) /**< \brief (SBMATRIX) Special Function 4 */
#define REG_SBMATRIX_SFR5          (0x41007124) /**< \brief (SBMATRIX) Special Function 5 */
#define REG_SBMATRIX_SFR6          (0x41007128) /**< \brief (SBMATRIX) Special Function 6 */
#define REG_SBMATRIX_SFR7          (0x4100712C) /**< \brief (SBMATRIX) Special Function 7 */
#define REG_SBMATRIX_SFR8          (0x41007130) /**< \brief (SBMATRIX) Special Function 8 */
#define REG_SBMATRIX_SFR9          (0x41007134) /**< \brief (SBMATRIX) Special Function 9 */
#define REG_SBMATRIX_SFR10         (0x41007138) /**< \brief (SBMATRIX) Special Function 10 */
#define REG_SBMATRIX_SFR11         (0x4100713C) /**< \brief (SBMATRIX) Special Function 11 */
#define REG_SBMATRIX_SFR12         (0x41007140) /**< \brief (SBMATRIX) Special Function 12 */
#define REG_SBMATRIX_SFR13         (0x41007144) /**< \brief (SBMATRIX) Special Function 13 */
#define REG_SBMATRIX_SFR14         (0x41007148) /**< \brief (SBMATRIX) Special Function 14 */
#define REG_SBMATRIX_SFR15         (0x4100714C) /**< \brief (SBMATRIX) Special Function 15 */
#else
#define REG_SBMATRIX_PRAS0         (*(RwReg  *)0x41007080UL) /**< \brief (SBMATRIX) Priority A for Slave 0 */
#define REG_SBMATRIX_PRBS0         (*(RwReg  *)0x41007084UL) /**< \brief (SBMATRIX) Priority B for Slave 0 */
#define REG_SBMATRIX_PRAS1         (*(RwReg  *)0x41007088UL) /**< \brief (SBMATRIX) Priority A for Slave 1 */
#define REG_SBMATRIX_PRBS1         (*(RwReg  *)0x4100708CUL) /**< \brief (SBMATRIX) Priority B for Slave 1 */
#define REG_SBMATRIX_PRAS2         (*(RwReg  *)0x41007090UL) /**< \brief (SBMATRIX) Priority A for Slave 2 */
#define REG_SBMATRIX_PRBS2         (*(RwReg  *)0x41007094UL) /**< \brief (SBMATRIX) Priority B for Slave 2 */
#define REG_SBMATRIX_PRAS3         (*(RwReg  *)0x41007098UL) /**< \brief (SBMATRIX) Priority A for Slave 3 */
#define REG_SBMATRIX_PRBS3         (*(RwReg  *)0x4100709CUL) /**< \brief (SBMATRIX) Priority B for Slave 3 */
#define REG_SBMATRIX_PRAS4         (*(RwReg  *)0x410070A0UL) /**< \brief (SBMATRIX) Priority A for Slave 4 */
#define REG_SBMATRIX_PRBS4         (*(RwReg  *)0x410070A4UL) /**< \brief (SBMATRIX) Priority B for Slave 4 */
#define REG_SBMATRIX_PRAS5         (*(RwReg  *)0x410070A8UL) /**< \brief (SBMATRIX) Priority A for Slave 5 */
#define REG_SBMATRIX_PRBS5         (*(RwReg  *)0x410070ACUL) /**< \brief (SBMATRIX) Priority B for Slave 5 */
#define REG_SBMATRIX_PRAS6         (*(RwReg  *)0x410070B0UL) /**< \brief (SBMATRIX) Priority A for Slave 6 */
#define REG_SBMATRIX_PRBS6         (*(RwReg  *)0x410070B4UL) /**< \brief (SBMATRIX) Priority B for Slave 6 */
#define REG_SBMATRIX_PRAS7         (*(RwReg  *)0x410070B8UL) /**< \brief (SBMATRIX) Priority A for Slave 7 */
#define REG_SBMATRIX_PRBS7         (*(RwReg  *)0x410070BCUL) /**< \brief (SBMATRIX) Priority B for Slave 7 */
#define REG_SBMATRIX_PRAS8         (*(RwReg  *)0x410070C0UL) /**< \brief (SBMATRIX) Priority A for Slave 8 */
#define REG_SBMATRIX_PRBS8         (*(RwReg  *)0x410070C4UL) /**< \brief (SBMATRIX) Priority B for Slave 8 */
#define REG_SBMATRIX_PRAS9         (*(RwReg  *)0x410070C8UL) /**< \brief (SBMATRIX) Priority A for Slave 9 */
#define REG_SBMATRIX_PRBS9         (*(RwReg  *)0x410070CCUL) /**< \brief (SBMATRIX) Priority B for Slave 9 */
#define REG_SBMATRIX_PRAS10        (*(RwReg  *)0x410070D0UL) /**< \brief (SBMATRIX) Priority A for Slave 10 */
#define REG_SBMATRIX_PRBS10        (*(RwReg  *)0x410070D4UL) /**< \brief (SBMATRIX) Priority B for Slave 10 */
#define REG_SBMATRIX_PRAS11        (*(RwReg  *)0x410070D8UL) /**< \brief (SBMATRIX) Priority A for Slave 11 */
#define REG_SBMATRIX_PRBS11        (*(RwReg  *)0x410070DCUL) /**< \brief (SBMATRIX) Priority B for Slave 11 */
#define REG_SBMATRIX_PRAS12        (*(RwReg  *)0x410070E0UL) /**< \brief (SBMATRIX) Priority A for Slave 12 */
#define REG_SBMATRIX_PRBS12        (*(RwReg  *)0x410070E4UL) /**< \brief (SBMATRIX) Priority B for Slave 12 */
#define REG_SBMATRIX_PRAS13        (*(RwReg  *)0x410070E8UL) /**< \brief (SBMATRIX) Priority A for Slave 13 */
#define REG_SBMATRIX_PRBS13        (*(RwReg  *)0x410070ECUL) /**< \brief (SBMATRIX) Priority B for Slave 13 */
#define REG_SBMATRIX_PRAS14        (*(RwReg  *)0x410070F0UL) /**< \brief (SBMATRIX) Priority A for Slave 14 */
#define REG_SBMATRIX_PRBS14        (*(RwReg  *)0x410070F4UL) /**< \brief (SBMATRIX) Priority B for Slave 14 */
#define REG_SBMATRIX_PRAS15        (*(RwReg  *)0x410070F8UL) /**< \brief (SBMATRIX) Priority A for Slave 15 */
#define REG_SBMATRIX_PRBS15        (*(RwReg  *)0x410070FCUL) /**< \brief (SBMATRIX) Priority B for Slave 15 */
#define REG_SBMATRIX_SFR0          (*(RwReg  *)0x41007110UL) /**< \brief (SBMATRIX) Special Function 0 */
#define REG_SBMATRIX_SFR1          (*(RwReg  *)0x41007114UL) /**< \brief (SBMATRIX) Special Function 1 */
#define REG_SBMATRIX_SFR2          (*(RwReg  *)0x41007118UL) /**< \brief (SBMATRIX) Special Function 2 */
#define REG_SBMATRIX_SFR3          (*(RwReg  *)0x4100711CUL) /**< \brief (SBMATRIX) Special Function 3 */
#define REG_SBMATRIX_SFR4          (*(RwReg  *)0x41007120UL) /**< \brief (SBMATRIX) Special Function 4 */
#define REG_SBMATRIX_SFR5          (*(RwReg  *)0x41007124UL) /**< \brief (SBMATRIX) Special Function 5 */
#define REG_SBMATRIX_SFR6          (*(RwReg  *)0x41007128UL) /**< \brief (SBMATRIX) Special Function 6 */
#define REG_SBMATRIX_SFR7          (*(RwReg  *)0x4100712CUL) /**< \brief (SBMATRIX) Special Function 7 */
#define REG_SBMATRIX_SFR8          (*(RwReg  *)0x41007130UL) /**< \brief (SBMATRIX) Special Function 8 */
#define REG_SBMATRIX_SFR9          (*(RwReg  *)0x41007134UL) /**< \brief (SBMATRIX) Special Function 9 */
#define REG_SBMATRIX_SFR10         (*(RwReg  *)0x41007138UL) /**< \brief (SBMATRIX) Special Function 10 */
#define REG_SBMATRIX_SFR11         (*(RwReg  *)0x4100713CUL) /**< \brief (SBMATRIX) Special Function 11 */
#define REG_SBMATRIX_SFR12         (*(RwReg  *)0x41007140UL) /**< \brief (SBMATRIX) Special Function 12 */
#define REG_SBMATRIX_SFR13         (*(RwReg  *)0x41007144UL) /**< \brief (SBMATRIX) Special Function 13 */
#define REG_SBMATRIX_SFR14         (*(RwReg  *)0x41007148UL) /**< \brief (SBMATRIX) Special Function 14 */
#define REG_SBMATRIX_SFR15         (*(RwReg  *)0x4100714CUL) /**< \brief (SBMATRIX) Special Function 15 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for SBMATRIX peripheral ========== */
#define SBMATRIX_DEFINED                    
/* ========== Instance parameters for SBMATRIX ========== */
#define SBMATRIX_SLAVE_FLASH        0
#define SBMATRIX_SLAVE_HPB0         1
#define SBMATRIX_SLAVE_HPB1         2
#define SBMATRIX_SLAVE_HPB2         3
#define SBMATRIX_SLAVE_HMCRAMC0     4
#define SBMATRIX_SLAVE_HMCRAMC0_ALT0 5
#define SBMATRIX_SLAVE_HMCRAMC0_ALT1 6
#define SBMATRIX_SLAVE_NUM          7

#define SBMATRIX_MASTER_CM0PLUS     0
#define SBMATRIX_MASTER_DSU         1
#define SBMATRIX_MASTER_DMAC        2
#define SBMATRIX_MASTER_NUM         3

#endif /* _SAMD21_SBMATRIX_INSTANCE_ */
