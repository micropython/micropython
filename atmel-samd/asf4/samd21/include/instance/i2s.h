/**
 * \file
 *
 * \brief Instance description for I2S
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

#ifndef _SAMD21_I2S_INSTANCE_
#define _SAMD21_I2S_INSTANCE_

/* ========== Register definition for I2S peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_I2S_CTRLA              (0x42005000) /**< \brief (I2S) Control A */
#define REG_I2S_CLKCTRL0           (0x42005004) /**< \brief (I2S) Clock Unit 0 Control */
#define REG_I2S_CLKCTRL1           (0x42005008) /**< \brief (I2S) Clock Unit 1 Control */
#define REG_I2S_INTENCLR           (0x4200500C) /**< \brief (I2S) Interrupt Enable Clear */
#define REG_I2S_INTENSET           (0x42005010) /**< \brief (I2S) Interrupt Enable Set */
#define REG_I2S_INTFLAG            (0x42005014) /**< \brief (I2S) Interrupt Flag Status and Clear */
#define REG_I2S_SYNCBUSY           (0x42005018) /**< \brief (I2S) Synchronization Status */
#define REG_I2S_SERCTRL0           (0x42005020) /**< \brief (I2S) Serializer 0 Control */
#define REG_I2S_SERCTRL1           (0x42005024) /**< \brief (I2S) Serializer 1 Control */
#define REG_I2S_DATA0              (0x42005030) /**< \brief (I2S) Data 0 */
#define REG_I2S_DATA1              (0x42005034) /**< \brief (I2S) Data 1 */
#else
#define REG_I2S_CTRLA              (*(RwReg8 *)0x42005000UL) /**< \brief (I2S) Control A */
#define REG_I2S_CLKCTRL0           (*(RwReg  *)0x42005004UL) /**< \brief (I2S) Clock Unit 0 Control */
#define REG_I2S_CLKCTRL1           (*(RwReg  *)0x42005008UL) /**< \brief (I2S) Clock Unit 1 Control */
#define REG_I2S_INTENCLR           (*(RwReg16*)0x4200500CUL) /**< \brief (I2S) Interrupt Enable Clear */
#define REG_I2S_INTENSET           (*(RwReg16*)0x42005010UL) /**< \brief (I2S) Interrupt Enable Set */
#define REG_I2S_INTFLAG            (*(RwReg16*)0x42005014UL) /**< \brief (I2S) Interrupt Flag Status and Clear */
#define REG_I2S_SYNCBUSY           (*(RoReg16*)0x42005018UL) /**< \brief (I2S) Synchronization Status */
#define REG_I2S_SERCTRL0           (*(RwReg  *)0x42005020UL) /**< \brief (I2S) Serializer 0 Control */
#define REG_I2S_SERCTRL1           (*(RwReg  *)0x42005024UL) /**< \brief (I2S) Serializer 1 Control */
#define REG_I2S_DATA0              (*(RwReg  *)0x42005030UL) /**< \brief (I2S) Data 0 */
#define REG_I2S_DATA1              (*(RwReg  *)0x42005034UL) /**< \brief (I2S) Data 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for I2S peripheral ========== */
#define I2S_CLK_NUM                 2        // Number of clock units
#define I2S_DMAC_ID_RX_0            41
#define I2S_DMAC_ID_RX_1            42
#define I2S_DMAC_ID_RX_LSB          41
#define I2S_DMAC_ID_RX_MSB          42
#define I2S_DMAC_ID_RX_SIZE         2
#define I2S_DMAC_ID_TX_0            43
#define I2S_DMAC_ID_TX_1            44
#define I2S_DMAC_ID_TX_LSB          43
#define I2S_DMAC_ID_TX_MSB          44
#define I2S_DMAC_ID_TX_SIZE         2
#define I2S_GCLK_ID_0               35
#define I2S_GCLK_ID_1               36
#define I2S_GCLK_ID_LSB             35
#define I2S_GCLK_ID_MSB             36
#define I2S_GCLK_ID_SIZE            2
#define I2S_MAX_SLOTS               8        // Max number of data slots in frame
#define I2S_SER_NUM                 2        // Number of serializers

#endif /* _SAMD21_I2S_INSTANCE_ */
