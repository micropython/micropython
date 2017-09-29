/**
 * \file
 *
 * \brief Instance description for I2S
 *
 * Copyright (c) 2017 Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD51_I2S_INSTANCE_
#define _SAMD51_I2S_INSTANCE_

/* ========== Register definition for I2S peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_I2S_CTRLA              (0x43002800) /**< \brief (I2S) Control A */
#define REG_I2S_CLKCTRL0           (0x43002804) /**< \brief (I2S) Clock Unit 0 Control */
#define REG_I2S_CLKCTRL1           (0x43002808) /**< \brief (I2S) Clock Unit 1 Control */
#define REG_I2S_INTENCLR           (0x4300280C) /**< \brief (I2S) Interrupt Enable Clear */
#define REG_I2S_INTENSET           (0x43002810) /**< \brief (I2S) Interrupt Enable Set */
#define REG_I2S_INTFLAG            (0x43002814) /**< \brief (I2S) Interrupt Flag Status and Clear */
#define REG_I2S_SYNCBUSY           (0x43002818) /**< \brief (I2S) Synchronization Status */
#define REG_I2S_TXCTRL             (0x43002820) /**< \brief (I2S) Tx Serializer Control */
#define REG_I2S_RXCTRL             (0x43002824) /**< \brief (I2S) Rx Serializer Control */
#define REG_I2S_TXDATA             (0x43002830) /**< \brief (I2S) Tx Data */
#define REG_I2S_RXDATA             (0x43002834) /**< \brief (I2S) Rx Data */
#else
#define REG_I2S_CTRLA              (*(RwReg8 *)0x43002800UL) /**< \brief (I2S) Control A */
#define REG_I2S_CLKCTRL0           (*(RwReg  *)0x43002804UL) /**< \brief (I2S) Clock Unit 0 Control */
#define REG_I2S_CLKCTRL1           (*(RwReg  *)0x43002808UL) /**< \brief (I2S) Clock Unit 1 Control */
#define REG_I2S_INTENCLR           (*(RwReg16*)0x4300280CUL) /**< \brief (I2S) Interrupt Enable Clear */
#define REG_I2S_INTENSET           (*(RwReg16*)0x43002810UL) /**< \brief (I2S) Interrupt Enable Set */
#define REG_I2S_INTFLAG            (*(RwReg16*)0x43002814UL) /**< \brief (I2S) Interrupt Flag Status and Clear */
#define REG_I2S_SYNCBUSY           (*(RoReg16*)0x43002818UL) /**< \brief (I2S) Synchronization Status */
#define REG_I2S_TXCTRL             (*(RwReg  *)0x43002820UL) /**< \brief (I2S) Tx Serializer Control */
#define REG_I2S_RXCTRL             (*(RwReg  *)0x43002824UL) /**< \brief (I2S) Rx Serializer Control */
#define REG_I2S_TXDATA             (*(WoReg  *)0x43002830UL) /**< \brief (I2S) Tx Data */
#define REG_I2S_RXDATA             (*(RoReg  *)0x43002834UL) /**< \brief (I2S) Rx Data */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for I2S peripheral ========== */
#define I2S_CLK_NUM                 2        // Number of clock units
#define I2S_DMAC_ID_RX_0            76
#define I2S_DMAC_ID_RX_1            77
#define I2S_DMAC_ID_RX_LSB          76
#define I2S_DMAC_ID_RX_MSB          77
#define I2S_DMAC_ID_RX_SIZE         2
#define I2S_DMAC_ID_TX_0            78
#define I2S_DMAC_ID_TX_1            79
#define I2S_DMAC_ID_TX_LSB          78
#define I2S_DMAC_ID_TX_MSB          79
#define I2S_DMAC_ID_TX_SIZE         2
#define I2S_GCLK_ID_0               43
#define I2S_GCLK_ID_1               44
#define I2S_GCLK_ID_LSB             43
#define I2S_GCLK_ID_MSB             44
#define I2S_GCLK_ID_SIZE            2
#define I2S_MAX_SLOTS               8        // Max number of data slots in frame
#define I2S_MAX_WL_BITS             32       // Max number of bits in data samples
#define I2S_SER_NUM                 2        // Number of serializers

#endif /* _SAMD51_I2S_INSTANCE_ */
