/**
 * \file
 *
 * \brief Instance description for SDHC1
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

#ifndef _SAMD51_SDHC1_INSTANCE_
#define _SAMD51_SDHC1_INSTANCE_

/* ========== Register definition for SDHC1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_SDHC1_SSAR             (0x46000000) /**< \brief (SDHC1) SDMA System Address / Argument 2 */
#define REG_SDHC1_BSR              (0x46000004) /**< \brief (SDHC1) Block Size */
#define REG_SDHC1_BCR              (0x46000006) /**< \brief (SDHC1) Block Count */
#define REG_SDHC1_ARG1R            (0x46000008) /**< \brief (SDHC1) Argument 1 */
#define REG_SDHC1_TMR              (0x4600000C) /**< \brief (SDHC1) Transfer Mode */
#define REG_SDHC1_CR               (0x4600000E) /**< \brief (SDHC1) Command */
#define REG_SDHC1_RR0              (0x46000010) /**< \brief (SDHC1) Response 0 */
#define REG_SDHC1_RR1              (0x46000014) /**< \brief (SDHC1) Response 1 */
#define REG_SDHC1_RR2              (0x46000018) /**< \brief (SDHC1) Response 2 */
#define REG_SDHC1_RR3              (0x4600001C) /**< \brief (SDHC1) Response 3 */
#define REG_SDHC1_BDPR             (0x46000020) /**< \brief (SDHC1) Buffer Data Port */
#define REG_SDHC1_PSR              (0x46000024) /**< \brief (SDHC1) Present State */
#define REG_SDHC1_HC1R             (0x46000028) /**< \brief (SDHC1) Host Control 1 */
#define REG_SDHC1_PCR              (0x46000029) /**< \brief (SDHC1) Power Control */
#define REG_SDHC1_BGCR             (0x4600002A) /**< \brief (SDHC1) Block Gap Control */
#define REG_SDHC1_WCR              (0x4600002B) /**< \brief (SDHC1) Wakeup Control */
#define REG_SDHC1_CCR              (0x4600002C) /**< \brief (SDHC1) Clock Control */
#define REG_SDHC1_TCR              (0x4600002E) /**< \brief (SDHC1) Timeout Control */
#define REG_SDHC1_SRR              (0x4600002F) /**< \brief (SDHC1) Software Reset */
#define REG_SDHC1_NISTR            (0x46000030) /**< \brief (SDHC1) Normal Interrupt Status */
#define REG_SDHC1_EISTR            (0x46000032) /**< \brief (SDHC1) Error Interrupt Status */
#define REG_SDHC1_NISTER           (0x46000034) /**< \brief (SDHC1) Normal Interrupt Status Enable */
#define REG_SDHC1_EISTER           (0x46000036) /**< \brief (SDHC1) Error Interrupt Status Enable */
#define REG_SDHC1_NISIER           (0x46000038) /**< \brief (SDHC1) Normal Interrupt Signal Enable */
#define REG_SDHC1_EISIER           (0x4600003A) /**< \brief (SDHC1) Error Interrupt Signal Enable */
#define REG_SDHC1_ACESR            (0x4600003C) /**< \brief (SDHC1) Auto CMD Error Status */
#define REG_SDHC1_HC2R             (0x4600003E) /**< \brief (SDHC1) Host Control 2 */
#define REG_SDHC1_CA0R             (0x46000040) /**< \brief (SDHC1) Capabilities 0 */
#define REG_SDHC1_CA1R             (0x46000044) /**< \brief (SDHC1) Capabilities 1 */
#define REG_SDHC1_MCCAR            (0x46000048) /**< \brief (SDHC1) Maximum Current Capabilities */
#define REG_SDHC1_FERACES          (0x46000050) /**< \brief (SDHC1) Force Event for Auto CMD Error Status */
#define REG_SDHC1_FEREIS           (0x46000052) /**< \brief (SDHC1) Force Event for Error Interrupt Status */
#define REG_SDHC1_AESR             (0x46000054) /**< \brief (SDHC1) ADMA Error Status */
#define REG_SDHC1_ASAR0            (0x46000058) /**< \brief (SDHC1) ADMA System Address 0 */
#define REG_SDHC1_PVR0             (0x46000060) /**< \brief (SDHC1) Preset Value 0 */
#define REG_SDHC1_PVR1             (0x46000062) /**< \brief (SDHC1) Preset Value 1 */
#define REG_SDHC1_PVR2             (0x46000064) /**< \brief (SDHC1) Preset Value 2 */
#define REG_SDHC1_PVR3             (0x46000066) /**< \brief (SDHC1) Preset Value 3 */
#define REG_SDHC1_PVR4             (0x46000068) /**< \brief (SDHC1) Preset Value 4 */
#define REG_SDHC1_PVR5             (0x4600006A) /**< \brief (SDHC1) Preset Value 5 */
#define REG_SDHC1_PVR6             (0x4600006C) /**< \brief (SDHC1) Preset Value 6 */
#define REG_SDHC1_PVR7             (0x4600006E) /**< \brief (SDHC1) Preset Value 7 */
#define REG_SDHC1_SISR             (0x460000FC) /**< \brief (SDHC1) Slot Interrupt Status */
#define REG_SDHC1_HCVR             (0x460000FE) /**< \brief (SDHC1) Host Controller Version */
#define REG_SDHC1_MC1R             (0x46000204) /**< \brief (SDHC1) MMC Control 1 */
#define REG_SDHC1_MC2R             (0x46000205) /**< \brief (SDHC1) MMC Control 2 */
#define REG_SDHC1_ACR              (0x46000208) /**< \brief (SDHC1) AHB Control */
#define REG_SDHC1_CC2R             (0x4600020C) /**< \brief (SDHC1) Clock Control 2 */
#define REG_SDHC1_CACR             (0x46000230) /**< \brief (SDHC1) Capabilities Control */
#define REG_SDHC1_DBGR             (0x46000234) /**< \brief (SDHC1) Debug */
#else
#define REG_SDHC1_SSAR             (*(RwReg  *)0x46000000UL) /**< \brief (SDHC1) SDMA System Address / Argument 2 */
#define REG_SDHC1_BSR              (*(RwReg16*)0x46000004UL) /**< \brief (SDHC1) Block Size */
#define REG_SDHC1_BCR              (*(RwReg16*)0x46000006UL) /**< \brief (SDHC1) Block Count */
#define REG_SDHC1_ARG1R            (*(RwReg  *)0x46000008UL) /**< \brief (SDHC1) Argument 1 */
#define REG_SDHC1_TMR              (*(RwReg16*)0x4600000CUL) /**< \brief (SDHC1) Transfer Mode */
#define REG_SDHC1_CR               (*(RwReg16*)0x4600000EUL) /**< \brief (SDHC1) Command */
#define REG_SDHC1_RR0              (*(RoReg  *)0x46000010UL) /**< \brief (SDHC1) Response 0 */
#define REG_SDHC1_RR1              (*(RoReg  *)0x46000014UL) /**< \brief (SDHC1) Response 1 */
#define REG_SDHC1_RR2              (*(RoReg  *)0x46000018UL) /**< \brief (SDHC1) Response 2 */
#define REG_SDHC1_RR3              (*(RoReg  *)0x4600001CUL) /**< \brief (SDHC1) Response 3 */
#define REG_SDHC1_BDPR             (*(RwReg  *)0x46000020UL) /**< \brief (SDHC1) Buffer Data Port */
#define REG_SDHC1_PSR              (*(RoReg  *)0x46000024UL) /**< \brief (SDHC1) Present State */
#define REG_SDHC1_HC1R             (*(RwReg8 *)0x46000028UL) /**< \brief (SDHC1) Host Control 1 */
#define REG_SDHC1_PCR              (*(RwReg8 *)0x46000029UL) /**< \brief (SDHC1) Power Control */
#define REG_SDHC1_BGCR             (*(RwReg8 *)0x4600002AUL) /**< \brief (SDHC1) Block Gap Control */
#define REG_SDHC1_WCR              (*(RwReg8 *)0x4600002BUL) /**< \brief (SDHC1) Wakeup Control */
#define REG_SDHC1_CCR              (*(RwReg16*)0x4600002CUL) /**< \brief (SDHC1) Clock Control */
#define REG_SDHC1_TCR              (*(RwReg8 *)0x4600002EUL) /**< \brief (SDHC1) Timeout Control */
#define REG_SDHC1_SRR              (*(RwReg8 *)0x4600002FUL) /**< \brief (SDHC1) Software Reset */
#define REG_SDHC1_NISTR            (*(RwReg16*)0x46000030UL) /**< \brief (SDHC1) Normal Interrupt Status */
#define REG_SDHC1_EISTR            (*(RwReg16*)0x46000032UL) /**< \brief (SDHC1) Error Interrupt Status */
#define REG_SDHC1_NISTER           (*(RwReg16*)0x46000034UL) /**< \brief (SDHC1) Normal Interrupt Status Enable */
#define REG_SDHC1_EISTER           (*(RwReg16*)0x46000036UL) /**< \brief (SDHC1) Error Interrupt Status Enable */
#define REG_SDHC1_NISIER           (*(RwReg16*)0x46000038UL) /**< \brief (SDHC1) Normal Interrupt Signal Enable */
#define REG_SDHC1_EISIER           (*(RwReg16*)0x4600003AUL) /**< \brief (SDHC1) Error Interrupt Signal Enable */
#define REG_SDHC1_ACESR            (*(RoReg16*)0x4600003CUL) /**< \brief (SDHC1) Auto CMD Error Status */
#define REG_SDHC1_HC2R             (*(RwReg16*)0x4600003EUL) /**< \brief (SDHC1) Host Control 2 */
#define REG_SDHC1_CA0R             (*(RoReg  *)0x46000040UL) /**< \brief (SDHC1) Capabilities 0 */
#define REG_SDHC1_CA1R             (*(RoReg  *)0x46000044UL) /**< \brief (SDHC1) Capabilities 1 */
#define REG_SDHC1_MCCAR            (*(RoReg  *)0x46000048UL) /**< \brief (SDHC1) Maximum Current Capabilities */
#define REG_SDHC1_FERACES          (*(WoReg16*)0x46000050UL) /**< \brief (SDHC1) Force Event for Auto CMD Error Status */
#define REG_SDHC1_FEREIS           (*(WoReg16*)0x46000052UL) /**< \brief (SDHC1) Force Event for Error Interrupt Status */
#define REG_SDHC1_AESR             (*(RoReg8 *)0x46000054UL) /**< \brief (SDHC1) ADMA Error Status */
#define REG_SDHC1_ASAR0            (*(RwReg  *)0x46000058UL) /**< \brief (SDHC1) ADMA System Address 0 */
#define REG_SDHC1_PVR0             (*(RwReg16*)0x46000060UL) /**< \brief (SDHC1) Preset Value 0 */
#define REG_SDHC1_PVR1             (*(RwReg16*)0x46000062UL) /**< \brief (SDHC1) Preset Value 1 */
#define REG_SDHC1_PVR2             (*(RwReg16*)0x46000064UL) /**< \brief (SDHC1) Preset Value 2 */
#define REG_SDHC1_PVR3             (*(RwReg16*)0x46000066UL) /**< \brief (SDHC1) Preset Value 3 */
#define REG_SDHC1_PVR4             (*(RwReg16*)0x46000068UL) /**< \brief (SDHC1) Preset Value 4 */
#define REG_SDHC1_PVR5             (*(RwReg16*)0x4600006AUL) /**< \brief (SDHC1) Preset Value 5 */
#define REG_SDHC1_PVR6             (*(RwReg16*)0x4600006CUL) /**< \brief (SDHC1) Preset Value 6 */
#define REG_SDHC1_PVR7             (*(RwReg16*)0x4600006EUL) /**< \brief (SDHC1) Preset Value 7 */
#define REG_SDHC1_SISR             (*(RoReg16*)0x460000FCUL) /**< \brief (SDHC1) Slot Interrupt Status */
#define REG_SDHC1_HCVR             (*(RoReg16*)0x460000FEUL) /**< \brief (SDHC1) Host Controller Version */
#define REG_SDHC1_MC1R             (*(RwReg8 *)0x46000204UL) /**< \brief (SDHC1) MMC Control 1 */
#define REG_SDHC1_MC2R             (*(WoReg8 *)0x46000205UL) /**< \brief (SDHC1) MMC Control 2 */
#define REG_SDHC1_ACR              (*(RwReg  *)0x46000208UL) /**< \brief (SDHC1) AHB Control */
#define REG_SDHC1_CC2R             (*(RwReg  *)0x4600020CUL) /**< \brief (SDHC1) Clock Control 2 */
#define REG_SDHC1_CACR             (*(RwReg  *)0x46000230UL) /**< \brief (SDHC1) Capabilities Control */
#define REG_SDHC1_DBGR             (*(RwReg8 *)0x46000234UL) /**< \brief (SDHC1) Debug */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for SDHC1 peripheral ========== */
#define SDHC1_CARD_DATA_SIZE        4       
#define SDHC1_CLK_AHB_ID            16      
#define SDHC1_GCLK_ID               46      
#define SDHC1_GCLK_ID_SLOW          3       
#define SDHC1_NB_OF_DEVICES         1       
#define SDHC1_NB_REG_PVR            8       
#define SDHC1_NB_REG_RR             4       

#endif /* _SAMD51_SDHC1_INSTANCE_ */
