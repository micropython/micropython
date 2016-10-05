/**
 * \file
 *
 * \brief Instance description for I2S
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

#ifndef _SAMD21_I2S_INSTANCE_
#define _SAMD21_I2S_INSTANCE_

/* ========== Register definition for I2S peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_I2S_CTRLA              (0x42005000U) /**< \brief (I2S) Control A */
#define REG_I2S_CLKCTRL0           (0x42005004U) /**< \brief (I2S) Clock Unit 0 Control */
#define REG_I2S_CLKCTRL1           (0x42005008U) /**< \brief (I2S) Clock Unit 1 Control */
#define REG_I2S_INTENCLR           (0x4200500CU) /**< \brief (I2S) Interrupt Enable Clear */
#define REG_I2S_INTENSET           (0x42005010U) /**< \brief (I2S) Interrupt Enable Set */
#define REG_I2S_INTFLAG            (0x42005014U) /**< \brief (I2S) Interrupt Flag Status and Clear */
#define REG_I2S_SYNCBUSY           (0x42005018U) /**< \brief (I2S) Synchronization Status */
#define REG_I2S_SERCTRL0           (0x42005020U) /**< \brief (I2S) Serializer 0 Control */
#define REG_I2S_SERCTRL1           (0x42005024U) /**< \brief (I2S) Serializer 1 Control */
#define REG_I2S_DATA0              (0x42005030U) /**< \brief (I2S) Data 0 */
#define REG_I2S_DATA1              (0x42005034U) /**< \brief (I2S) Data 1 */
#else
#define REG_I2S_CTRLA              (*(RwReg8 *)0x42005000U) /**< \brief (I2S) Control A */
#define REG_I2S_CLKCTRL0           (*(RwReg  *)0x42005004U) /**< \brief (I2S) Clock Unit 0 Control */
#define REG_I2S_CLKCTRL1           (*(RwReg  *)0x42005008U) /**< \brief (I2S) Clock Unit 1 Control */
#define REG_I2S_INTENCLR           (*(RwReg16*)0x4200500CU) /**< \brief (I2S) Interrupt Enable Clear */
#define REG_I2S_INTENSET           (*(RwReg16*)0x42005010U) /**< \brief (I2S) Interrupt Enable Set */
#define REG_I2S_INTFLAG            (*(RwReg16*)0x42005014U) /**< \brief (I2S) Interrupt Flag Status and Clear */
#define REG_I2S_SYNCBUSY           (*(RoReg16*)0x42005018U) /**< \brief (I2S) Synchronization Status */
#define REG_I2S_SERCTRL0           (*(RwReg  *)0x42005020U) /**< \brief (I2S) Serializer 0 Control */
#define REG_I2S_SERCTRL1           (*(RwReg  *)0x42005024U) /**< \brief (I2S) Serializer 1 Control */
#define REG_I2S_DATA0              (*(RwReg  *)0x42005030U) /**< \brief (I2S) Data 0 */
#define REG_I2S_DATA1              (*(RwReg  *)0x42005034U) /**< \brief (I2S) Data 1 */
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
