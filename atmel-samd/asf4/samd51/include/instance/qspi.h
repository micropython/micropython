/**
 * \file
 *
 * \brief Instance description for QSPI
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

#ifndef _SAMD51_QSPI_INSTANCE_
#define _SAMD51_QSPI_INSTANCE_

/* ========== Register definition for QSPI peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_QSPI_CTRLA             (0x42003400) /**< \brief (QSPI) Control A */
#define REG_QSPI_CTRLB             (0x42003404) /**< \brief (QSPI) Control B */
#define REG_QSPI_BAUD              (0x42003408) /**< \brief (QSPI) Baud Rate */
#define REG_QSPI_RXDATA            (0x4200340C) /**< \brief (QSPI) Receive Data */
#define REG_QSPI_TXDATA            (0x42003410) /**< \brief (QSPI) Transmit Data */
#define REG_QSPI_INTENCLR          (0x42003414) /**< \brief (QSPI) Interrupt Enable Clear */
#define REG_QSPI_INTENSET          (0x42003418) /**< \brief (QSPI) Interrupt Enable Set */
#define REG_QSPI_INTFLAG           (0x4200341C) /**< \brief (QSPI) Interrupt Flag Status and Clear */
#define REG_QSPI_STATUS            (0x42003420) /**< \brief (QSPI) Status Register */
#define REG_QSPI_INSTRADDR         (0x42003430) /**< \brief (QSPI) Instruction Address */
#define REG_QSPI_INSTRCTRL         (0x42003434) /**< \brief (QSPI) Instruction Code */
#define REG_QSPI_INSTRFRAME        (0x42003438) /**< \brief (QSPI) Instruction Frame */
#define REG_QSPI_SCRAMBCTRL        (0x42003440) /**< \brief (QSPI) Scrambling Mode */
#define REG_QSPI_SCRAMBKEY         (0x42003444) /**< \brief (QSPI) Scrambling Key */
#else
#define REG_QSPI_CTRLA             (*(RwReg  *)0x42003400UL) /**< \brief (QSPI) Control A */
#define REG_QSPI_CTRLB             (*(RwReg  *)0x42003404UL) /**< \brief (QSPI) Control B */
#define REG_QSPI_BAUD              (*(RwReg  *)0x42003408UL) /**< \brief (QSPI) Baud Rate */
#define REG_QSPI_RXDATA            (*(RoReg  *)0x4200340CUL) /**< \brief (QSPI) Receive Data */
#define REG_QSPI_TXDATA            (*(WoReg  *)0x42003410UL) /**< \brief (QSPI) Transmit Data */
#define REG_QSPI_INTENCLR          (*(RwReg  *)0x42003414UL) /**< \brief (QSPI) Interrupt Enable Clear */
#define REG_QSPI_INTENSET          (*(RwReg  *)0x42003418UL) /**< \brief (QSPI) Interrupt Enable Set */
#define REG_QSPI_INTFLAG           (*(RwReg  *)0x4200341CUL) /**< \brief (QSPI) Interrupt Flag Status and Clear */
#define REG_QSPI_STATUS            (*(RoReg  *)0x42003420UL) /**< \brief (QSPI) Status Register */
#define REG_QSPI_INSTRADDR         (*(RwReg  *)0x42003430UL) /**< \brief (QSPI) Instruction Address */
#define REG_QSPI_INSTRCTRL         (*(RwReg  *)0x42003434UL) /**< \brief (QSPI) Instruction Code */
#define REG_QSPI_INSTRFRAME        (*(RwReg  *)0x42003438UL) /**< \brief (QSPI) Instruction Frame */
#define REG_QSPI_SCRAMBCTRL        (*(RwReg  *)0x42003440UL) /**< \brief (QSPI) Scrambling Mode */
#define REG_QSPI_SCRAMBKEY         (*(WoReg  *)0x42003444UL) /**< \brief (QSPI) Scrambling Key */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for QSPI peripheral ========== */
#define QSPI_DMAC_ID_RX             83      
#define QSPI_DMAC_ID_TX             84      
#define QSPI_HADDR_MSB              23      
#define QSPI_OCMS                   1       

#endif /* _SAMD51_QSPI_INSTANCE_ */
