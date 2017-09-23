/**
 * \file
 *
 * \brief Instance description for CAN1
 *
 * Copyright (c) 2014 Atmel Corporation. All rights reserved.
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

#ifndef _SAMD51_CAN1_INSTANCE_
#define _SAMD51_CAN1_INSTANCE_

/* ========== Register definition for CAN1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_CAN1_CREL              (0x42003800U) /**< \brief (CAN1) Core Release */
#define REG_CAN1_ENDN              (0x42003804U) /**< \brief (CAN1) Endian */
#define REG_CAN1_MRCFG             (0x42003808U) /**< \brief (CAN1) Message RAM Configuration */
#define REG_CAN1_DBTP              (0x4200380CU) /**< \brief (CAN1) Fast Bit Timing and Prescaler */
#define REG_CAN1_TEST              (0x42003810U) /**< \brief (CAN1) Test */
#define REG_CAN1_RWD               (0x42003814U) /**< \brief (CAN1) RAM Watchdog */
#define REG_CAN1_CCCR              (0x42003818U) /**< \brief (CAN1) CC Control */
#define REG_CAN1_NBTP              (0x4200381CU) /**< \brief (CAN1) Nominal Bit Timing and Prescaler */
#define REG_CAN1_TSCC              (0x42003820U) /**< \brief (CAN1) Timestamp Counter Configuration */
#define REG_CAN1_TSCV              (0x42003824U) /**< \brief (CAN1) Timestamp Counter Value */
#define REG_CAN1_TOCC              (0x42003828U) /**< \brief (CAN1) Timeout Counter Configuration */
#define REG_CAN1_TOCV              (0x4200382CU) /**< \brief (CAN1) Timeout Counter Value */
#define REG_CAN1_ECR               (0x42003840U) /**< \brief (CAN1) Error Counter */
#define REG_CAN1_PSR               (0x42003844U) /**< \brief (CAN1) Protocol Status */
#define REG_CAN1_TDCR              (0x42003848U) /**< \brief (CAN1) Extended ID Filter Configuration */
#define REG_CAN1_IR                (0x42003850U) /**< \brief (CAN1) Interrupt */
#define REG_CAN1_IE                (0x42003854U) /**< \brief (CAN1) Interrupt Enable */
#define REG_CAN1_ILS               (0x42003858U) /**< \brief (CAN1) Interrupt Line Select */
#define REG_CAN1_ILE               (0x4200385CU) /**< \brief (CAN1) Interrupt Line Enable */
#define REG_CAN1_GFC               (0x42003880U) /**< \brief (CAN1) Global Filter Configuration */
#define REG_CAN1_SIDFC             (0x42003884U) /**< \brief (CAN1) Standard ID Filter Configuration */
#define REG_CAN1_XIDFC             (0x42003888U) /**< \brief (CAN1) Extended ID Filter Configuration */
#define REG_CAN1_XIDAM             (0x42003890U) /**< \brief (CAN1) Extended ID AND Mask */
#define REG_CAN1_HPMS              (0x42003894U) /**< \brief (CAN1) High Priority Message Status */
#define REG_CAN1_NDAT1             (0x42003898U) /**< \brief (CAN1) New Data 1 */
#define REG_CAN1_NDAT2             (0x4200389CU) /**< \brief (CAN1) New Data 2 */
#define REG_CAN1_RXF0C             (0x420038A0U) /**< \brief (CAN1) Rx FIFO 0 Configuration */
#define REG_CAN1_RXF0S             (0x420038A4U) /**< \brief (CAN1) Rx FIFO 0 Status */
#define REG_CAN1_RXF0A             (0x420038A8U) /**< \brief (CAN1) Rx FIFO 0 Acknowledge */
#define REG_CAN1_RXBC              (0x420038ACU) /**< \brief (CAN1) Rx Buffer Configuration */
#define REG_CAN1_RXF1C             (0x420038B0U) /**< \brief (CAN1) Rx FIFO 1 Configuration */
#define REG_CAN1_RXF1S             (0x420038B4U) /**< \brief (CAN1) Rx FIFO 1 Status */
#define REG_CAN1_RXF1A             (0x420038B8U) /**< \brief (CAN1) Rx FIFO 1 Acknowledge */
#define REG_CAN1_RXESC             (0x420038BCU) /**< \brief (CAN1) Rx Buffer / FIFO Element Size Configuration */
#define REG_CAN1_TXBC              (0x420038C0U) /**< \brief (CAN1) Tx Buffer Configuration */
#define REG_CAN1_TXFQS             (0x420038C4U) /**< \brief (CAN1) Tx FIFO / Queue Status */
#define REG_CAN1_TXESC             (0x420038C8U) /**< \brief (CAN1) Tx Buffer Element Size Configuration */
#define REG_CAN1_TXBRP             (0x420038CCU) /**< \brief (CAN1) Tx Buffer Request Pending */
#define REG_CAN1_TXBAR             (0x420038D0U) /**< \brief (CAN1) Tx Buffer Add Request */
#define REG_CAN1_TXBCR             (0x420038D4U) /**< \brief (CAN1) Tx Buffer Cancellation Request */
#define REG_CAN1_TXBTO             (0x420038D8U) /**< \brief (CAN1) Tx Buffer Transmission Occurred */
#define REG_CAN1_TXBCF             (0x420038DCU) /**< \brief (CAN1) Tx Buffer Cancellation Finished */
#define REG_CAN1_TXBTIE            (0x420038E0U) /**< \brief (CAN1) Tx Buffer Transmission Interrupt Enable */
#define REG_CAN1_TXBCIE            (0x420038E4U) /**< \brief (CAN1) Tx Buffer Cancellation Finished Interrupt Enable */
#define REG_CAN1_TXEFC             (0x420038F0U) /**< \brief (CAN1) Tx Event FIFO Configuration */
#define REG_CAN1_TXEFS             (0x420038F4U) /**< \brief (CAN1) Tx Event FIFO Status */
#define REG_CAN1_TXEFA             (0x420038F8U) /**< \brief (CAN1) Tx Event FIFO Acknowledge */
#else
#define REG_CAN1_CREL              (*(RoReg  *)0x42003800U) /**< \brief (CAN1) Core Release */
#define REG_CAN1_ENDN              (*(RoReg  *)0x42003804U) /**< \brief (CAN1) Endian */
#define REG_CAN1_MRCFG             (*(RwReg  *)0x42003808U) /**< \brief (CAN1) Message RAM Configuration */
#define REG_CAN1_DBTP              (*(RwReg  *)0x4200380CU) /**< \brief (CAN1) Fast Bit Timing and Prescaler */
#define REG_CAN1_TEST              (*(RwReg  *)0x42003810U) /**< \brief (CAN1) Test */
#define REG_CAN1_RWD               (*(RwReg  *)0x42003814U) /**< \brief (CAN1) RAM Watchdog */
#define REG_CAN1_CCCR              (*(RwReg  *)0x42003818U) /**< \brief (CAN1) CC Control */
#define REG_CAN1_NBTP              (*(RwReg  *)0x4200381CU) /**< \brief (CAN1) Nominal Bit Timing and Prescaler */
#define REG_CAN1_TSCC              (*(RwReg  *)0x42003820U) /**< \brief (CAN1) Timestamp Counter Configuration */
#define REG_CAN1_TSCV              (*(RoReg  *)0x42003824U) /**< \brief (CAN1) Timestamp Counter Value */
#define REG_CAN1_TOCC              (*(RwReg  *)0x42003828U) /**< \brief (CAN1) Timeout Counter Configuration */
#define REG_CAN1_TOCV              (*(RwReg  *)0x4200382CU) /**< \brief (CAN1) Timeout Counter Value */
#define REG_CAN1_ECR               (*(RoReg  *)0x42003840U) /**< \brief (CAN1) Error Counter */
#define REG_CAN1_PSR               (*(RoReg  *)0x42003844U) /**< \brief (CAN1) Protocol Status */
#define REG_CAN1_TDCR              (*(RwReg  *)0x42003848U) /**< \brief (CAN1) Extended ID Filter Configuration */
#define REG_CAN1_IR                (*(RwReg  *)0x42003850U) /**< \brief (CAN1) Interrupt */
#define REG_CAN1_IE                (*(RwReg  *)0x42003854U) /**< \brief (CAN1) Interrupt Enable */
#define REG_CAN1_ILS               (*(RwReg  *)0x42003858U) /**< \brief (CAN1) Interrupt Line Select */
#define REG_CAN1_ILE               (*(RwReg  *)0x4200385CU) /**< \brief (CAN1) Interrupt Line Enable */
#define REG_CAN1_GFC               (*(RwReg  *)0x42003880U) /**< \brief (CAN1) Global Filter Configuration */
#define REG_CAN1_SIDFC             (*(RwReg  *)0x42003884U) /**< \brief (CAN1) Standard ID Filter Configuration */
#define REG_CAN1_XIDFC             (*(RwReg  *)0x42003888U) /**< \brief (CAN1) Extended ID Filter Configuration */
#define REG_CAN1_XIDAM             (*(RwReg  *)0x42003890U) /**< \brief (CAN1) Extended ID AND Mask */
#define REG_CAN1_HPMS              (*(RoReg  *)0x42003894U) /**< \brief (CAN1) High Priority Message Status */
#define REG_CAN1_NDAT1             (*(RwReg  *)0x42003898U) /**< \brief (CAN1) New Data 1 */
#define REG_CAN1_NDAT2             (*(RwReg  *)0x4200389CU) /**< \brief (CAN1) New Data 2 */
#define REG_CAN1_RXF0C             (*(RwReg  *)0x420038A0U) /**< \brief (CAN1) Rx FIFO 0 Configuration */
#define REG_CAN1_RXF0S             (*(RoReg  *)0x420038A4U) /**< \brief (CAN1) Rx FIFO 0 Status */
#define REG_CAN1_RXF0A             (*(RwReg  *)0x420038A8U) /**< \brief (CAN1) Rx FIFO 0 Acknowledge */
#define REG_CAN1_RXBC              (*(RwReg  *)0x420038ACU) /**< \brief (CAN1) Rx Buffer Configuration */
#define REG_CAN1_RXF1C             (*(RwReg  *)0x420038B0U) /**< \brief (CAN1) Rx FIFO 1 Configuration */
#define REG_CAN1_RXF1S             (*(RoReg  *)0x420038B4U) /**< \brief (CAN1) Rx FIFO 1 Status */
#define REG_CAN1_RXF1A             (*(RwReg  *)0x420038B8U) /**< \brief (CAN1) Rx FIFO 1 Acknowledge */
#define REG_CAN1_RXESC             (*(RwReg  *)0x420038BCU) /**< \brief (CAN1) Rx Buffer / FIFO Element Size Configuration */
#define REG_CAN1_TXBC              (*(RwReg  *)0x420038C0U) /**< \brief (CAN1) Tx Buffer Configuration */
#define REG_CAN1_TXFQS             (*(RoReg  *)0x420038C4U) /**< \brief (CAN1) Tx FIFO / Queue Status */
#define REG_CAN1_TXESC             (*(RwReg  *)0x420038C8U) /**< \brief (CAN1) Tx Buffer Element Size Configuration */
#define REG_CAN1_TXBRP             (*(RoReg  *)0x420038CCU) /**< \brief (CAN1) Tx Buffer Request Pending */
#define REG_CAN1_TXBAR             (*(RwReg  *)0x420038D0U) /**< \brief (CAN1) Tx Buffer Add Request */
#define REG_CAN1_TXBCR             (*(RwReg  *)0x420038D4U) /**< \brief (CAN1) Tx Buffer Cancellation Request */
#define REG_CAN1_TXBTO             (*(RoReg  *)0x420038D8U) /**< \brief (CAN1) Tx Buffer Transmission Occurred */
#define REG_CAN1_TXBCF             (*(RoReg  *)0x420038DCU) /**< \brief (CAN1) Tx Buffer Cancellation Finished */
#define REG_CAN1_TXBTIE            (*(RwReg  *)0x420038E0U) /**< \brief (CAN1) Tx Buffer Transmission Interrupt Enable */
#define REG_CAN1_TXBCIE            (*(RwReg  *)0x420038E4U) /**< \brief (CAN1) Tx Buffer Cancellation Finished Interrupt Enable */
#define REG_CAN1_TXEFC             (*(RwReg  *)0x420038F0U) /**< \brief (CAN1) Tx Event FIFO Configuration */
#define REG_CAN1_TXEFS             (*(RoReg  *)0x420038F4U) /**< \brief (CAN1) Tx Event FIFO Status */
#define REG_CAN1_TXEFA             (*(RwReg  *)0x420038F8U) /**< \brief (CAN1) Tx Event FIFO Acknowledge */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for CAN1 peripheral ========== */
#define CAN1_CLK_AHB_ID             22       // Index of AHB clock
#define CAN1_DMAC_ID_DEBUG          38       // DMA CAN Debug Req
#define CAN1_GCLK_ID                36       // Index of Generic Clock

#endif /* _SAMD51_CAN1_INSTANCE_ */
