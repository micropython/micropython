/**
 * \file
 *
 * \brief Instance description for CAN0
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

#ifndef _SAMD51_CAN0_INSTANCE_
#define _SAMD51_CAN0_INSTANCE_

/* ========== Register definition for CAN0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_CAN0_CREL              (0x42000000U) /**< \brief (CAN0) Core Release */
#define REG_CAN0_ENDN              (0x42000004U) /**< \brief (CAN0) Endian */
#define REG_CAN0_MRCFG             (0x42000008U) /**< \brief (CAN0) Message RAM Configuration */
#define REG_CAN0_DBTP              (0x4200000CU) /**< \brief (CAN0) Fast Bit Timing and Prescaler */
#define REG_CAN0_TEST              (0x42000010U) /**< \brief (CAN0) Test */
#define REG_CAN0_RWD               (0x42000014U) /**< \brief (CAN0) RAM Watchdog */
#define REG_CAN0_CCCR              (0x42000018U) /**< \brief (CAN0) CC Control */
#define REG_CAN0_NBTP              (0x4200001CU) /**< \brief (CAN0) Nominal Bit Timing and Prescaler */
#define REG_CAN0_TSCC              (0x42000020U) /**< \brief (CAN0) Timestamp Counter Configuration */
#define REG_CAN0_TSCV              (0x42000024U) /**< \brief (CAN0) Timestamp Counter Value */
#define REG_CAN0_TOCC              (0x42000028U) /**< \brief (CAN0) Timeout Counter Configuration */
#define REG_CAN0_TOCV              (0x4200002CU) /**< \brief (CAN0) Timeout Counter Value */
#define REG_CAN0_ECR               (0x42000040U) /**< \brief (CAN0) Error Counter */
#define REG_CAN0_PSR               (0x42000044U) /**< \brief (CAN0) Protocol Status */
#define REG_CAN0_TDCR              (0x42000048U) /**< \brief (CAN0) Extended ID Filter Configuration */
#define REG_CAN0_IR                (0x42000050U) /**< \brief (CAN0) Interrupt */
#define REG_CAN0_IE                (0x42000054U) /**< \brief (CAN0) Interrupt Enable */
#define REG_CAN0_ILS               (0x42000058U) /**< \brief (CAN0) Interrupt Line Select */
#define REG_CAN0_ILE               (0x4200005CU) /**< \brief (CAN0) Interrupt Line Enable */
#define REG_CAN0_GFC               (0x42000080U) /**< \brief (CAN0) Global Filter Configuration */
#define REG_CAN0_SIDFC             (0x42000084U) /**< \brief (CAN0) Standard ID Filter Configuration */
#define REG_CAN0_XIDFC             (0x42000088U) /**< \brief (CAN0) Extended ID Filter Configuration */
#define REG_CAN0_XIDAM             (0x42000090U) /**< \brief (CAN0) Extended ID AND Mask */
#define REG_CAN0_HPMS              (0x42000094U) /**< \brief (CAN0) High Priority Message Status */
#define REG_CAN0_NDAT1             (0x42000098U) /**< \brief (CAN0) New Data 1 */
#define REG_CAN0_NDAT2             (0x4200009CU) /**< \brief (CAN0) New Data 2 */
#define REG_CAN0_RXF0C             (0x420000A0U) /**< \brief (CAN0) Rx FIFO 0 Configuration */
#define REG_CAN0_RXF0S             (0x420000A4U) /**< \brief (CAN0) Rx FIFO 0 Status */
#define REG_CAN0_RXF0A             (0x420000A8U) /**< \brief (CAN0) Rx FIFO 0 Acknowledge */
#define REG_CAN0_RXBC              (0x420000ACU) /**< \brief (CAN0) Rx Buffer Configuration */
#define REG_CAN0_RXF1C             (0x420000B0U) /**< \brief (CAN0) Rx FIFO 1 Configuration */
#define REG_CAN0_RXF1S             (0x420000B4U) /**< \brief (CAN0) Rx FIFO 1 Status */
#define REG_CAN0_RXF1A             (0x420000B8U) /**< \brief (CAN0) Rx FIFO 1 Acknowledge */
#define REG_CAN0_RXESC             (0x420000BCU) /**< \brief (CAN0) Rx Buffer / FIFO Element Size Configuration */
#define REG_CAN0_TXBC              (0x420000C0U) /**< \brief (CAN0) Tx Buffer Configuration */
#define REG_CAN0_TXFQS             (0x420000C4U) /**< \brief (CAN0) Tx FIFO / Queue Status */
#define REG_CAN0_TXESC             (0x420000C8U) /**< \brief (CAN0) Tx Buffer Element Size Configuration */
#define REG_CAN0_TXBRP             (0x420000CCU) /**< \brief (CAN0) Tx Buffer Request Pending */
#define REG_CAN0_TXBAR             (0x420000D0U) /**< \brief (CAN0) Tx Buffer Add Request */
#define REG_CAN0_TXBCR             (0x420000D4U) /**< \brief (CAN0) Tx Buffer Cancellation Request */
#define REG_CAN0_TXBTO             (0x420000D8U) /**< \brief (CAN0) Tx Buffer Transmission Occurred */
#define REG_CAN0_TXBCF             (0x420000DCU) /**< \brief (CAN0) Tx Buffer Cancellation Finished */
#define REG_CAN0_TXBTIE            (0x420000E0U) /**< \brief (CAN0) Tx Buffer Transmission Interrupt Enable */
#define REG_CAN0_TXBCIE            (0x420000E4U) /**< \brief (CAN0) Tx Buffer Cancellation Finished Interrupt Enable */
#define REG_CAN0_TXEFC             (0x420000F0U) /**< \brief (CAN0) Tx Event FIFO Configuration */
#define REG_CAN0_TXEFS             (0x420000F4U) /**< \brief (CAN0) Tx Event FIFO Status */
#define REG_CAN0_TXEFA             (0x420000F8U) /**< \brief (CAN0) Tx Event FIFO Acknowledge */
#else
#define REG_CAN0_CREL              (*(RoReg  *)0x42000000U) /**< \brief (CAN0) Core Release */
#define REG_CAN0_ENDN              (*(RoReg  *)0x42000004U) /**< \brief (CAN0) Endian */
#define REG_CAN0_MRCFG             (*(RwReg  *)0x42000008U) /**< \brief (CAN0) Message RAM Configuration */
#define REG_CAN0_DBTP              (*(RwReg  *)0x4200000CU) /**< \brief (CAN0) Fast Bit Timing and Prescaler */
#define REG_CAN0_TEST              (*(RwReg  *)0x42000010U) /**< \brief (CAN0) Test */
#define REG_CAN0_RWD               (*(RwReg  *)0x42000014U) /**< \brief (CAN0) RAM Watchdog */
#define REG_CAN0_CCCR              (*(RwReg  *)0x42000018U) /**< \brief (CAN0) CC Control */
#define REG_CAN0_NBTP              (*(RwReg  *)0x4200001CU) /**< \brief (CAN0) Nominal Bit Timing and Prescaler */
#define REG_CAN0_TSCC              (*(RwReg  *)0x42000020U) /**< \brief (CAN0) Timestamp Counter Configuration */
#define REG_CAN0_TSCV              (*(RoReg  *)0x42000024U) /**< \brief (CAN0) Timestamp Counter Value */
#define REG_CAN0_TOCC              (*(RwReg  *)0x42000028U) /**< \brief (CAN0) Timeout Counter Configuration */
#define REG_CAN0_TOCV              (*(RwReg  *)0x4200002CU) /**< \brief (CAN0) Timeout Counter Value */
#define REG_CAN0_ECR               (*(RoReg  *)0x42000040U) /**< \brief (CAN0) Error Counter */
#define REG_CAN0_PSR               (*(RoReg  *)0x42000044U) /**< \brief (CAN0) Protocol Status */
#define REG_CAN0_TDCR              (*(RwReg  *)0x42000048U) /**< \brief (CAN0) Extended ID Filter Configuration */
#define REG_CAN0_IR                (*(RwReg  *)0x42000050U) /**< \brief (CAN0) Interrupt */
#define REG_CAN0_IE                (*(RwReg  *)0x42000054U) /**< \brief (CAN0) Interrupt Enable */
#define REG_CAN0_ILS               (*(RwReg  *)0x42000058U) /**< \brief (CAN0) Interrupt Line Select */
#define REG_CAN0_ILE               (*(RwReg  *)0x4200005CU) /**< \brief (CAN0) Interrupt Line Enable */
#define REG_CAN0_GFC               (*(RwReg  *)0x42000080U) /**< \brief (CAN0) Global Filter Configuration */
#define REG_CAN0_SIDFC             (*(RwReg  *)0x42000084U) /**< \brief (CAN0) Standard ID Filter Configuration */
#define REG_CAN0_XIDFC             (*(RwReg  *)0x42000088U) /**< \brief (CAN0) Extended ID Filter Configuration */
#define REG_CAN0_XIDAM             (*(RwReg  *)0x42000090U) /**< \brief (CAN0) Extended ID AND Mask */
#define REG_CAN0_HPMS              (*(RoReg  *)0x42000094U) /**< \brief (CAN0) High Priority Message Status */
#define REG_CAN0_NDAT1             (*(RwReg  *)0x42000098U) /**< \brief (CAN0) New Data 1 */
#define REG_CAN0_NDAT2             (*(RwReg  *)0x4200009CU) /**< \brief (CAN0) New Data 2 */
#define REG_CAN0_RXF0C             (*(RwReg  *)0x420000A0U) /**< \brief (CAN0) Rx FIFO 0 Configuration */
#define REG_CAN0_RXF0S             (*(RoReg  *)0x420000A4U) /**< \brief (CAN0) Rx FIFO 0 Status */
#define REG_CAN0_RXF0A             (*(RwReg  *)0x420000A8U) /**< \brief (CAN0) Rx FIFO 0 Acknowledge */
#define REG_CAN0_RXBC              (*(RwReg  *)0x420000ACU) /**< \brief (CAN0) Rx Buffer Configuration */
#define REG_CAN0_RXF1C             (*(RwReg  *)0x420000B0U) /**< \brief (CAN0) Rx FIFO 1 Configuration */
#define REG_CAN0_RXF1S             (*(RoReg  *)0x420000B4U) /**< \brief (CAN0) Rx FIFO 1 Status */
#define REG_CAN0_RXF1A             (*(RwReg  *)0x420000B8U) /**< \brief (CAN0) Rx FIFO 1 Acknowledge */
#define REG_CAN0_RXESC             (*(RwReg  *)0x420000BCU) /**< \brief (CAN0) Rx Buffer / FIFO Element Size Configuration */
#define REG_CAN0_TXBC              (*(RwReg  *)0x420000C0U) /**< \brief (CAN0) Tx Buffer Configuration */
#define REG_CAN0_TXFQS             (*(RoReg  *)0x420000C4U) /**< \brief (CAN0) Tx FIFO / Queue Status */
#define REG_CAN0_TXESC             (*(RwReg  *)0x420000C8U) /**< \brief (CAN0) Tx Buffer Element Size Configuration */
#define REG_CAN0_TXBRP             (*(RoReg  *)0x420000CCU) /**< \brief (CAN0) Tx Buffer Request Pending */
#define REG_CAN0_TXBAR             (*(RwReg  *)0x420000D0U) /**< \brief (CAN0) Tx Buffer Add Request */
#define REG_CAN0_TXBCR             (*(RwReg  *)0x420000D4U) /**< \brief (CAN0) Tx Buffer Cancellation Request */
#define REG_CAN0_TXBTO             (*(RoReg  *)0x420000D8U) /**< \brief (CAN0) Tx Buffer Transmission Occurred */
#define REG_CAN0_TXBCF             (*(RoReg  *)0x420000DCU) /**< \brief (CAN0) Tx Buffer Cancellation Finished */
#define REG_CAN0_TXBTIE            (*(RwReg  *)0x420000E0U) /**< \brief (CAN0) Tx Buffer Transmission Interrupt Enable */
#define REG_CAN0_TXBCIE            (*(RwReg  *)0x420000E4U) /**< \brief (CAN0) Tx Buffer Cancellation Finished Interrupt Enable */
#define REG_CAN0_TXEFC             (*(RwReg  *)0x420000F0U) /**< \brief (CAN0) Tx Event FIFO Configuration */
#define REG_CAN0_TXEFS             (*(RoReg  *)0x420000F4U) /**< \brief (CAN0) Tx Event FIFO Status */
#define REG_CAN0_TXEFA             (*(RwReg  *)0x420000F8U) /**< \brief (CAN0) Tx Event FIFO Acknowledge */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for CAN0 peripheral ========== */
#define CAN0_CLK_AHB_ID             17       // Index of AHB clock
#define CAN0_DMAC_ID_DEBUG          20       // DMA CAN Debug Req
#define CAN0_GCLK_ID                27       // Index of Generic Clock
#define CAN0_MSG_RAM_ADDR           0x20000000
#define CAN0_QOS_RESET_VAL          1        // QOS reset value

#endif /* _SAMD51_CAN0_INSTANCE_ */
