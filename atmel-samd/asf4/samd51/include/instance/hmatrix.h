/**
 * \file
 *
 * \brief Instance description for HMATRIX
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

#ifndef _SAMD51_HMATRIX_INSTANCE_
#define _SAMD51_HMATRIX_INSTANCE_

/* ========== Register definition for HMATRIX peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_HMATRIX_PRAS0          (0x4100C080) /**< \brief (HMATRIX) Priority A for Slave 0 */
#define REG_HMATRIX_PRBS0          (0x4100C084) /**< \brief (HMATRIX) Priority B for Slave 0 */
#define REG_HMATRIX_PRAS1          (0x4100C088) /**< \brief (HMATRIX) Priority A for Slave 1 */
#define REG_HMATRIX_PRBS1          (0x4100C08C) /**< \brief (HMATRIX) Priority B for Slave 1 */
#define REG_HMATRIX_PRAS2          (0x4100C090) /**< \brief (HMATRIX) Priority A for Slave 2 */
#define REG_HMATRIX_PRBS2          (0x4100C094) /**< \brief (HMATRIX) Priority B for Slave 2 */
#define REG_HMATRIX_PRAS3          (0x4100C098) /**< \brief (HMATRIX) Priority A for Slave 3 */
#define REG_HMATRIX_PRBS3          (0x4100C09C) /**< \brief (HMATRIX) Priority B for Slave 3 */
#define REG_HMATRIX_PRAS4          (0x4100C0A0) /**< \brief (HMATRIX) Priority A for Slave 4 */
#define REG_HMATRIX_PRBS4          (0x4100C0A4) /**< \brief (HMATRIX) Priority B for Slave 4 */
#else
#define REG_HMATRIX_PRAS0          (*(RwReg  *)0x4100C080UL) /**< \brief (HMATRIX) Priority A for Slave 0 */
#define REG_HMATRIX_PRBS0          (*(RwReg  *)0x4100C084UL) /**< \brief (HMATRIX) Priority B for Slave 0 */
#define REG_HMATRIX_PRAS1          (*(RwReg  *)0x4100C088UL) /**< \brief (HMATRIX) Priority A for Slave 1 */
#define REG_HMATRIX_PRBS1          (*(RwReg  *)0x4100C08CUL) /**< \brief (HMATRIX) Priority B for Slave 1 */
#define REG_HMATRIX_PRAS2          (*(RwReg  *)0x4100C090UL) /**< \brief (HMATRIX) Priority A for Slave 2 */
#define REG_HMATRIX_PRBS2          (*(RwReg  *)0x4100C094UL) /**< \brief (HMATRIX) Priority B for Slave 2 */
#define REG_HMATRIX_PRAS3          (*(RwReg  *)0x4100C098UL) /**< \brief (HMATRIX) Priority A for Slave 3 */
#define REG_HMATRIX_PRBS3          (*(RwReg  *)0x4100C09CUL) /**< \brief (HMATRIX) Priority B for Slave 3 */
#define REG_HMATRIX_PRAS4          (*(RwReg  *)0x4100C0A0UL) /**< \brief (HMATRIX) Priority A for Slave 4 */
#define REG_HMATRIX_PRBS4          (*(RwReg  *)0x4100C0A4UL) /**< \brief (HMATRIX) Priority B for Slave 4 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for HMATRIX peripheral ========== */
#define HMATRIX_CLK_AHB_ID          5        // Index of AHB Clock in MCLK.AHBMASK register (MASK may be tied to 1 depending on chip integration)
#define HMATRIX_DEFINED                     
/* ========== Instance parameters for HMATRIX ========== */
#define HMATRIX_SLAVE_FLASH         0
#define HMATRIX_SLAVE_FLASH_ALT     1
#define HMATRIX_SLAVE_SEEPROM       2
#define HMATRIX_SLAVE_RAMCM4S       3
#define HMATRIX_SLAVE_RAMPPPDSU     4
#define HMATRIX_SLAVE_RAMDMAWR      5
#define HMATRIX_SLAVE_RAMDMACICM    6
#define HMATRIX_SLAVE_HPB0          7
#define HMATRIX_SLAVE_HPB1          8
#define HMATRIX_SLAVE_HPB2          9
#define HMATRIX_SLAVE_HPB3          10
#define HMATRIX_SLAVE_SDHC0         12
#define HMATRIX_SLAVE_SDHC1         13
#define HMATRIX_SLAVE_QSPI          14
#define HMATRIX_SLAVE_BKUPRAM       15
#define HMATRIX_SLAVE_NUM           16

#define HMATRIX_MASTER_CM4_S        0
#define HMATRIX_MASTER_CMCC         1
#define HMATRIX_MASTER_PICOP_MEM    2
#define HMATRIX_MASTER_PICOP_IO     3
#define HMATRIX_MASTER_DMAC_DTWR    4
#define HMATRIX_MASTER_DMAC_DTRD    5
#define HMATRIX_MASTER_ICM          6
#define HMATRIX_MASTER_DSU          7
#define HMATRIX_MASTER_NUM          8

#endif /* _SAMD51_HMATRIX_INSTANCE_ */
