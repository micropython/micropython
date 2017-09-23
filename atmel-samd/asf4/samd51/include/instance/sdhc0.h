/**
 * \file
 *
 * \brief Instance description for SDHC0
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

#ifndef _SAMD51_SDHC0_INSTANCE_
#define _SAMD51_SDHC0_INSTANCE_

/* ========== Register definition for SDHC0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_SDHC0_SSAR             (0x45000000) /**< \brief (SDHC0) SDMA System Address / Argument 2 */
#define REG_SDHC0_BSR              (0x45000004) /**< \brief (SDHC0) Block Size */
#define REG_SDHC0_BCR              (0x45000006) /**< \brief (SDHC0) Block Count */
#define REG_SDHC0_ARG1R            (0x45000008) /**< \brief (SDHC0) Argument 1 */
#define REG_SDHC0_TMR              (0x4500000C) /**< \brief (SDHC0) Transfer Mode */
#define REG_SDHC0_CR               (0x4500000E) /**< \brief (SDHC0) Command */
#define REG_SDHC0_RR0              (0x45000010) /**< \brief (SDHC0) Response 0 */
#define REG_SDHC0_RR1              (0x45000014) /**< \brief (SDHC0) Response 1 */
#define REG_SDHC0_RR2              (0x45000018) /**< \brief (SDHC0) Response 2 */
#define REG_SDHC0_RR3              (0x4500001C) /**< \brief (SDHC0) Response 3 */
#define REG_SDHC0_BDPR             (0x45000020) /**< \brief (SDHC0) Buffer Data Port */
#define REG_SDHC0_PSR              (0x45000024) /**< \brief (SDHC0) Present State */
#define REG_SDHC0_HC1R             (0x45000028) /**< \brief (SDHC0) Host Control 1 */
#define REG_SDHC0_PCR              (0x45000029) /**< \brief (SDHC0) Power Control */
#define REG_SDHC0_BGCR             (0x4500002A) /**< \brief (SDHC0) Block Gap Control */
#define REG_SDHC0_WCR              (0x4500002B) /**< \brief (SDHC0) Wakeup Control */
#define REG_SDHC0_CCR              (0x4500002C) /**< \brief (SDHC0) Clock Control */
#define REG_SDHC0_TCR              (0x4500002E) /**< \brief (SDHC0) Timeout Control */
#define REG_SDHC0_SRR              (0x4500002F) /**< \brief (SDHC0) Software Reset */
#define REG_SDHC0_NISTR            (0x45000030) /**< \brief (SDHC0) Normal Interrupt Status */
#define REG_SDHC0_EISTR            (0x45000032) /**< \brief (SDHC0) Error Interrupt Status */
#define REG_SDHC0_NISTER           (0x45000034) /**< \brief (SDHC0) Normal Interrupt Status Enable */
#define REG_SDHC0_EISTER           (0x45000036) /**< \brief (SDHC0) Error Interrupt Status Enable */
#define REG_SDHC0_NISIER           (0x45000038) /**< \brief (SDHC0) Normal Interrupt Signal Enable */
#define REG_SDHC0_EISIER           (0x4500003A) /**< \brief (SDHC0) Error Interrupt Signal Enable */
#define REG_SDHC0_ACESR            (0x4500003C) /**< \brief (SDHC0) Auto CMD Error Status */
#define REG_SDHC0_HC2R             (0x4500003E) /**< \brief (SDHC0) Host Control 2 */
#define REG_SDHC0_CA0R             (0x45000040) /**< \brief (SDHC0) Capabilities 0 */
#define REG_SDHC0_CA1R             (0x45000044) /**< \brief (SDHC0) Capabilities 1 */
#define REG_SDHC0_MCCAR            (0x45000048) /**< \brief (SDHC0) Maximum Current Capabilities */
#define REG_SDHC0_FERACES          (0x45000050) /**< \brief (SDHC0) Force Event for Auto CMD Error Status */
#define REG_SDHC0_FEREIS           (0x45000052) /**< \brief (SDHC0) Force Event for Error Interrupt Status */
#define REG_SDHC0_AESR             (0x45000054) /**< \brief (SDHC0) ADMA Error Status */
#define REG_SDHC0_ASAR0            (0x45000058) /**< \brief (SDHC0) ADMA System Address 0 */
#define REG_SDHC0_PVR0             (0x45000060) /**< \brief (SDHC0) Preset Value 0 */
#define REG_SDHC0_PVR1             (0x45000062) /**< \brief (SDHC0) Preset Value 1 */
#define REG_SDHC0_PVR2             (0x45000064) /**< \brief (SDHC0) Preset Value 2 */
#define REG_SDHC0_PVR3             (0x45000066) /**< \brief (SDHC0) Preset Value 3 */
#define REG_SDHC0_PVR4             (0x45000068) /**< \brief (SDHC0) Preset Value 4 */
#define REG_SDHC0_PVR5             (0x4500006A) /**< \brief (SDHC0) Preset Value 5 */
#define REG_SDHC0_PVR6             (0x4500006C) /**< \brief (SDHC0) Preset Value 6 */
#define REG_SDHC0_PVR7             (0x4500006E) /**< \brief (SDHC0) Preset Value 7 */
#define REG_SDHC0_SISR             (0x450000FC) /**< \brief (SDHC0) Slot Interrupt Status */
#define REG_SDHC0_HCVR             (0x450000FE) /**< \brief (SDHC0) Host Controller Version */
#define REG_SDHC0_MC1R             (0x45000204) /**< \brief (SDHC0) MMC Control 1 */
#define REG_SDHC0_MC2R             (0x45000205) /**< \brief (SDHC0) MMC Control 2 */
#define REG_SDHC0_ACR              (0x45000208) /**< \brief (SDHC0) AHB Control */
#define REG_SDHC0_CC2R             (0x4500020C) /**< \brief (SDHC0) Clock Control 2 */
#define REG_SDHC0_CACR             (0x45000230) /**< \brief (SDHC0) Capabilities Control */
#define REG_SDHC0_DBGR             (0x45000234) /**< \brief (SDHC0) Debug */
#else
#define REG_SDHC0_SSAR             (*(RwReg  *)0x45000000UL) /**< \brief (SDHC0) SDMA System Address / Argument 2 */
#define REG_SDHC0_BSR              (*(RwReg16*)0x45000004UL) /**< \brief (SDHC0) Block Size */
#define REG_SDHC0_BCR              (*(RwReg16*)0x45000006UL) /**< \brief (SDHC0) Block Count */
#define REG_SDHC0_ARG1R            (*(RwReg  *)0x45000008UL) /**< \brief (SDHC0) Argument 1 */
#define REG_SDHC0_TMR              (*(RwReg16*)0x4500000CUL) /**< \brief (SDHC0) Transfer Mode */
#define REG_SDHC0_CR               (*(RwReg16*)0x4500000EUL) /**< \brief (SDHC0) Command */
#define REG_SDHC0_RR0              (*(RoReg  *)0x45000010UL) /**< \brief (SDHC0) Response 0 */
#define REG_SDHC0_RR1              (*(RoReg  *)0x45000014UL) /**< \brief (SDHC0) Response 1 */
#define REG_SDHC0_RR2              (*(RoReg  *)0x45000018UL) /**< \brief (SDHC0) Response 2 */
#define REG_SDHC0_RR3              (*(RoReg  *)0x4500001CUL) /**< \brief (SDHC0) Response 3 */
#define REG_SDHC0_BDPR             (*(RwReg  *)0x45000020UL) /**< \brief (SDHC0) Buffer Data Port */
#define REG_SDHC0_PSR              (*(RoReg  *)0x45000024UL) /**< \brief (SDHC0) Present State */
#define REG_SDHC0_HC1R             (*(RwReg8 *)0x45000028UL) /**< \brief (SDHC0) Host Control 1 */
#define REG_SDHC0_PCR              (*(RwReg8 *)0x45000029UL) /**< \brief (SDHC0) Power Control */
#define REG_SDHC0_BGCR             (*(RwReg8 *)0x4500002AUL) /**< \brief (SDHC0) Block Gap Control */
#define REG_SDHC0_WCR              (*(RwReg8 *)0x4500002BUL) /**< \brief (SDHC0) Wakeup Control */
#define REG_SDHC0_CCR              (*(RwReg16*)0x4500002CUL) /**< \brief (SDHC0) Clock Control */
#define REG_SDHC0_TCR              (*(RwReg8 *)0x4500002EUL) /**< \brief (SDHC0) Timeout Control */
#define REG_SDHC0_SRR              (*(RwReg8 *)0x4500002FUL) /**< \brief (SDHC0) Software Reset */
#define REG_SDHC0_NISTR            (*(RwReg16*)0x45000030UL) /**< \brief (SDHC0) Normal Interrupt Status */
#define REG_SDHC0_EISTR            (*(RwReg16*)0x45000032UL) /**< \brief (SDHC0) Error Interrupt Status */
#define REG_SDHC0_NISTER           (*(RwReg16*)0x45000034UL) /**< \brief (SDHC0) Normal Interrupt Status Enable */
#define REG_SDHC0_EISTER           (*(RwReg16*)0x45000036UL) /**< \brief (SDHC0) Error Interrupt Status Enable */
#define REG_SDHC0_NISIER           (*(RwReg16*)0x45000038UL) /**< \brief (SDHC0) Normal Interrupt Signal Enable */
#define REG_SDHC0_EISIER           (*(RwReg16*)0x4500003AUL) /**< \brief (SDHC0) Error Interrupt Signal Enable */
#define REG_SDHC0_ACESR            (*(RoReg16*)0x4500003CUL) /**< \brief (SDHC0) Auto CMD Error Status */
#define REG_SDHC0_HC2R             (*(RwReg16*)0x4500003EUL) /**< \brief (SDHC0) Host Control 2 */
#define REG_SDHC0_CA0R             (*(RoReg  *)0x45000040UL) /**< \brief (SDHC0) Capabilities 0 */
#define REG_SDHC0_CA1R             (*(RoReg  *)0x45000044UL) /**< \brief (SDHC0) Capabilities 1 */
#define REG_SDHC0_MCCAR            (*(RoReg  *)0x45000048UL) /**< \brief (SDHC0) Maximum Current Capabilities */
#define REG_SDHC0_FERACES          (*(WoReg16*)0x45000050UL) /**< \brief (SDHC0) Force Event for Auto CMD Error Status */
#define REG_SDHC0_FEREIS           (*(WoReg16*)0x45000052UL) /**< \brief (SDHC0) Force Event for Error Interrupt Status */
#define REG_SDHC0_AESR             (*(RoReg8 *)0x45000054UL) /**< \brief (SDHC0) ADMA Error Status */
#define REG_SDHC0_ASAR0            (*(RwReg  *)0x45000058UL) /**< \brief (SDHC0) ADMA System Address 0 */
#define REG_SDHC0_PVR0             (*(RwReg16*)0x45000060UL) /**< \brief (SDHC0) Preset Value 0 */
#define REG_SDHC0_PVR1             (*(RwReg16*)0x45000062UL) /**< \brief (SDHC0) Preset Value 1 */
#define REG_SDHC0_PVR2             (*(RwReg16*)0x45000064UL) /**< \brief (SDHC0) Preset Value 2 */
#define REG_SDHC0_PVR3             (*(RwReg16*)0x45000066UL) /**< \brief (SDHC0) Preset Value 3 */
#define REG_SDHC0_PVR4             (*(RwReg16*)0x45000068UL) /**< \brief (SDHC0) Preset Value 4 */
#define REG_SDHC0_PVR5             (*(RwReg16*)0x4500006AUL) /**< \brief (SDHC0) Preset Value 5 */
#define REG_SDHC0_PVR6             (*(RwReg16*)0x4500006CUL) /**< \brief (SDHC0) Preset Value 6 */
#define REG_SDHC0_PVR7             (*(RwReg16*)0x4500006EUL) /**< \brief (SDHC0) Preset Value 7 */
#define REG_SDHC0_SISR             (*(RoReg16*)0x450000FCUL) /**< \brief (SDHC0) Slot Interrupt Status */
#define REG_SDHC0_HCVR             (*(RoReg16*)0x450000FEUL) /**< \brief (SDHC0) Host Controller Version */
#define REG_SDHC0_MC1R             (*(RwReg8 *)0x45000204UL) /**< \brief (SDHC0) MMC Control 1 */
#define REG_SDHC0_MC2R             (*(WoReg8 *)0x45000205UL) /**< \brief (SDHC0) MMC Control 2 */
#define REG_SDHC0_ACR              (*(RwReg  *)0x45000208UL) /**< \brief (SDHC0) AHB Control */
#define REG_SDHC0_CC2R             (*(RwReg  *)0x4500020CUL) /**< \brief (SDHC0) Clock Control 2 */
#define REG_SDHC0_CACR             (*(RwReg  *)0x45000230UL) /**< \brief (SDHC0) Capabilities Control */
#define REG_SDHC0_DBGR             (*(RwReg8 *)0x45000234UL) /**< \brief (SDHC0) Debug */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for SDHC0 peripheral ========== */
#define SDHC0_CARD_DATA_SIZE        4       
#define SDHC0_CLK_AHB_ID            15      
#define SDHC0_GCLK_ID               45      
#define SDHC0_GCLK_ID_SLOW          3       
#define SDHC0_NB_OF_DEVICES         1       
#define SDHC0_NB_REG_PVR            8       
#define SDHC0_NB_REG_RR             4       

#endif /* _SAMD51_SDHC0_INSTANCE_ */
