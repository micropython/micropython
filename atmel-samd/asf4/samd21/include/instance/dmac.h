/**
 * \file
 *
 * \brief Instance description for DMAC
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

#ifndef _SAMD21_DMAC_INSTANCE_
#define _SAMD21_DMAC_INSTANCE_

/* ========== Register definition for DMAC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_DMAC_CTRL              (0x41004800) /**< \brief (DMAC) Control */
#define REG_DMAC_CRCCTRL           (0x41004802) /**< \brief (DMAC) CRC Control */
#define REG_DMAC_CRCDATAIN         (0x41004804) /**< \brief (DMAC) CRC Data Input */
#define REG_DMAC_CRCCHKSUM         (0x41004808) /**< \brief (DMAC) CRC Checksum */
#define REG_DMAC_CRCSTATUS         (0x4100480C) /**< \brief (DMAC) CRC Status */
#define REG_DMAC_DBGCTRL           (0x4100480D) /**< \brief (DMAC) Debug Control */
#define REG_DMAC_QOSCTRL           (0x4100480E) /**< \brief (DMAC) QOS Control */
#define REG_DMAC_SWTRIGCTRL        (0x41004810) /**< \brief (DMAC) Software Trigger Control */
#define REG_DMAC_PRICTRL0          (0x41004814) /**< \brief (DMAC) Priority Control 0 */
#define REG_DMAC_INTPEND           (0x41004820) /**< \brief (DMAC) Interrupt Pending */
#define REG_DMAC_INTSTATUS         (0x41004824) /**< \brief (DMAC) Interrupt Status */
#define REG_DMAC_BUSYCH            (0x41004828) /**< \brief (DMAC) Busy Channels */
#define REG_DMAC_PENDCH            (0x4100482C) /**< \brief (DMAC) Pending Channels */
#define REG_DMAC_ACTIVE            (0x41004830) /**< \brief (DMAC) Active Channel and Levels */
#define REG_DMAC_BASEADDR          (0x41004834) /**< \brief (DMAC) Descriptor Memory Section Base Address */
#define REG_DMAC_WRBADDR           (0x41004838) /**< \brief (DMAC) Write-Back Memory Section Base Address */
#define REG_DMAC_CHID              (0x4100483F) /**< \brief (DMAC) Channel ID */
#define REG_DMAC_CHCTRLA           (0x41004840) /**< \brief (DMAC) Channel Control A */
#define REG_DMAC_CHCTRLB           (0x41004844) /**< \brief (DMAC) Channel Control B */
#define REG_DMAC_CHINTENCLR        (0x4100484C) /**< \brief (DMAC) Channel Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET        (0x4100484D) /**< \brief (DMAC) Channel Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG         (0x4100484E) /**< \brief (DMAC) Channel Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS          (0x4100484F) /**< \brief (DMAC) Channel Status */
#else
#define REG_DMAC_CTRL              (*(RwReg16*)0x41004800UL) /**< \brief (DMAC) Control */
#define REG_DMAC_CRCCTRL           (*(RwReg16*)0x41004802UL) /**< \brief (DMAC) CRC Control */
#define REG_DMAC_CRCDATAIN         (*(RwReg  *)0x41004804UL) /**< \brief (DMAC) CRC Data Input */
#define REG_DMAC_CRCCHKSUM         (*(RwReg  *)0x41004808UL) /**< \brief (DMAC) CRC Checksum */
#define REG_DMAC_CRCSTATUS         (*(RwReg8 *)0x4100480CUL) /**< \brief (DMAC) CRC Status */
#define REG_DMAC_DBGCTRL           (*(RwReg8 *)0x4100480DUL) /**< \brief (DMAC) Debug Control */
#define REG_DMAC_QOSCTRL           (*(RwReg8 *)0x4100480EUL) /**< \brief (DMAC) QOS Control */
#define REG_DMAC_SWTRIGCTRL        (*(RwReg  *)0x41004810UL) /**< \brief (DMAC) Software Trigger Control */
#define REG_DMAC_PRICTRL0          (*(RwReg  *)0x41004814UL) /**< \brief (DMAC) Priority Control 0 */
#define REG_DMAC_INTPEND           (*(RwReg16*)0x41004820UL) /**< \brief (DMAC) Interrupt Pending */
#define REG_DMAC_INTSTATUS         (*(RoReg  *)0x41004824UL) /**< \brief (DMAC) Interrupt Status */
#define REG_DMAC_BUSYCH            (*(RoReg  *)0x41004828UL) /**< \brief (DMAC) Busy Channels */
#define REG_DMAC_PENDCH            (*(RoReg  *)0x4100482CUL) /**< \brief (DMAC) Pending Channels */
#define REG_DMAC_ACTIVE            (*(RoReg  *)0x41004830UL) /**< \brief (DMAC) Active Channel and Levels */
#define REG_DMAC_BASEADDR          (*(RwReg  *)0x41004834UL) /**< \brief (DMAC) Descriptor Memory Section Base Address */
#define REG_DMAC_WRBADDR           (*(RwReg  *)0x41004838UL) /**< \brief (DMAC) Write-Back Memory Section Base Address */
#define REG_DMAC_CHID              (*(RwReg8 *)0x4100483FUL) /**< \brief (DMAC) Channel ID */
#define REG_DMAC_CHCTRLA           (*(RwReg8 *)0x41004840UL) /**< \brief (DMAC) Channel Control A */
#define REG_DMAC_CHCTRLB           (*(RwReg  *)0x41004844UL) /**< \brief (DMAC) Channel Control B */
#define REG_DMAC_CHINTENCLR        (*(RwReg8 *)0x4100484CUL) /**< \brief (DMAC) Channel Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET        (*(RwReg8 *)0x4100484DUL) /**< \brief (DMAC) Channel Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG         (*(RwReg8 *)0x4100484EUL) /**< \brief (DMAC) Channel Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS          (*(RoReg8 *)0x4100484FUL) /**< \brief (DMAC) Channel Status */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for DMAC peripheral ========== */
#define DMAC_CH_BITS                4        // Number of bits to select channel
#define DMAC_CH_NUM                 12       // Number of channels
#define DMAC_CLK_AHB_ID             5        // AHB clock index
#define DMAC_EVIN_NUM               4        // Number of input events
#define DMAC_EVOUT_NUM              4        // Number of output events
#define DMAC_LVL_BITS               2        // Number of bit to select level priority
#define DMAC_LVL_NUM                4        // Enable priority level number
#define DMAC_TRIG_BITS              6        // Number of bits to select trigger source
#define DMAC_TRIG_NUM               45       // Number of peripheral triggers

#endif /* _SAMD21_DMAC_INSTANCE_ */
