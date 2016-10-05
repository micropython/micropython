/**
 * \file
 *
 * \brief Instance description for DMAC
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD21_DMAC_INSTANCE_
#define _SAMD21_DMAC_INSTANCE_

/* ========== Register definition for DMAC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_DMAC_CTRL              (0x41004800U) /**< \brief (DMAC) Control */
#define REG_DMAC_CRCCTRL           (0x41004802U) /**< \brief (DMAC) CRC Control */
#define REG_DMAC_CRCDATAIN         (0x41004804U) /**< \brief (DMAC) CRC Data Input */
#define REG_DMAC_CRCCHKSUM         (0x41004808U) /**< \brief (DMAC) CRC Checksum */
#define REG_DMAC_CRCSTATUS         (0x4100480CU) /**< \brief (DMAC) CRC Status */
#define REG_DMAC_DBGCTRL           (0x4100480DU) /**< \brief (DMAC) Debug Control */
#define REG_DMAC_QOSCTRL           (0x4100480EU) /**< \brief (DMAC) QOS Control */
#define REG_DMAC_SWTRIGCTRL        (0x41004810U) /**< \brief (DMAC) Software Trigger Control */
#define REG_DMAC_PRICTRL0          (0x41004814U) /**< \brief (DMAC) Priority Control 0 */
#define REG_DMAC_INTPEND           (0x41004820U) /**< \brief (DMAC) Interrupt Pending */
#define REG_DMAC_INTSTATUS         (0x41004824U) /**< \brief (DMAC) Interrupt Status */
#define REG_DMAC_BUSYCH            (0x41004828U) /**< \brief (DMAC) Busy Channels */
#define REG_DMAC_PENDCH            (0x4100482CU) /**< \brief (DMAC) Pending Channels */
#define REG_DMAC_ACTIVE            (0x41004830U) /**< \brief (DMAC) Active Channel and Levels */
#define REG_DMAC_BASEADDR          (0x41004834U) /**< \brief (DMAC) Descriptor Memory Section Base Address */
#define REG_DMAC_WRBADDR           (0x41004838U) /**< \brief (DMAC) Write-Back Memory Section Base Address */
#define REG_DMAC_CHID              (0x4100483FU) /**< \brief (DMAC) Channel ID */
#define REG_DMAC_CHCTRLA           (0x41004840U) /**< \brief (DMAC) Channel Control A */
#define REG_DMAC_CHCTRLB           (0x41004844U) /**< \brief (DMAC) Channel Control B */
#define REG_DMAC_CHINTENCLR        (0x4100484CU) /**< \brief (DMAC) Channel Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET        (0x4100484DU) /**< \brief (DMAC) Channel Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG         (0x4100484EU) /**< \brief (DMAC) Channel Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS          (0x4100484FU) /**< \brief (DMAC) Channel Status */
#else
#define REG_DMAC_CTRL              (*(RwReg16*)0x41004800U) /**< \brief (DMAC) Control */
#define REG_DMAC_CRCCTRL           (*(RwReg16*)0x41004802U) /**< \brief (DMAC) CRC Control */
#define REG_DMAC_CRCDATAIN         (*(RwReg  *)0x41004804U) /**< \brief (DMAC) CRC Data Input */
#define REG_DMAC_CRCCHKSUM         (*(RwReg  *)0x41004808U) /**< \brief (DMAC) CRC Checksum */
#define REG_DMAC_CRCSTATUS         (*(RwReg8 *)0x4100480CU) /**< \brief (DMAC) CRC Status */
#define REG_DMAC_DBGCTRL           (*(RwReg8 *)0x4100480DU) /**< \brief (DMAC) Debug Control */
#define REG_DMAC_QOSCTRL           (*(RwReg8 *)0x4100480EU) /**< \brief (DMAC) QOS Control */
#define REG_DMAC_SWTRIGCTRL        (*(RwReg  *)0x41004810U) /**< \brief (DMAC) Software Trigger Control */
#define REG_DMAC_PRICTRL0          (*(RwReg  *)0x41004814U) /**< \brief (DMAC) Priority Control 0 */
#define REG_DMAC_INTPEND           (*(RwReg16*)0x41004820U) /**< \brief (DMAC) Interrupt Pending */
#define REG_DMAC_INTSTATUS         (*(RoReg  *)0x41004824U) /**< \brief (DMAC) Interrupt Status */
#define REG_DMAC_BUSYCH            (*(RoReg  *)0x41004828U) /**< \brief (DMAC) Busy Channels */
#define REG_DMAC_PENDCH            (*(RoReg  *)0x4100482CU) /**< \brief (DMAC) Pending Channels */
#define REG_DMAC_ACTIVE            (*(RoReg  *)0x41004830U) /**< \brief (DMAC) Active Channel and Levels */
#define REG_DMAC_BASEADDR          (*(RwReg  *)0x41004834U) /**< \brief (DMAC) Descriptor Memory Section Base Address */
#define REG_DMAC_WRBADDR           (*(RwReg  *)0x41004838U) /**< \brief (DMAC) Write-Back Memory Section Base Address */
#define REG_DMAC_CHID              (*(RwReg8 *)0x4100483FU) /**< \brief (DMAC) Channel ID */
#define REG_DMAC_CHCTRLA           (*(RwReg8 *)0x41004840U) /**< \brief (DMAC) Channel Control A */
#define REG_DMAC_CHCTRLB           (*(RwReg  *)0x41004844U) /**< \brief (DMAC) Channel Control B */
#define REG_DMAC_CHINTENCLR        (*(RwReg8 *)0x4100484CU) /**< \brief (DMAC) Channel Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET        (*(RwReg8 *)0x4100484DU) /**< \brief (DMAC) Channel Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG         (*(RwReg8 *)0x4100484EU) /**< \brief (DMAC) Channel Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS          (*(RoReg8 *)0x4100484FU) /**< \brief (DMAC) Channel Status */
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
