/**
 * \file
 *
 * \brief Instance description for DSU
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

#ifndef _SAMD21_DSU_INSTANCE_
#define _SAMD21_DSU_INSTANCE_

/* ========== Register definition for DSU peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_DSU_CTRL               (0x41002000U) /**< \brief (DSU) Control */
#define REG_DSU_STATUSA            (0x41002001U) /**< \brief (DSU) Status A */
#define REG_DSU_STATUSB            (0x41002002U) /**< \brief (DSU) Status B */
#define REG_DSU_ADDR               (0x41002004U) /**< \brief (DSU) Address */
#define REG_DSU_LENGTH             (0x41002008U) /**< \brief (DSU) Length */
#define REG_DSU_DATA               (0x4100200CU) /**< \brief (DSU) Data */
#define REG_DSU_DCC0               (0x41002010U) /**< \brief (DSU) Debug Communication Channel 0 */
#define REG_DSU_DCC1               (0x41002014U) /**< \brief (DSU) Debug Communication Channel 1 */
#define REG_DSU_DID                (0x41002018U) /**< \brief (DSU) Device Identification */
#define REG_DSU_ENTRY0             (0x41003000U) /**< \brief (DSU) Coresight ROM Table Entry 0 */
#define REG_DSU_ENTRY1             (0x41003004U) /**< \brief (DSU) Coresight ROM Table Entry 1 */
#define REG_DSU_END                (0x41003008U) /**< \brief (DSU) Coresight ROM Table End */
#define REG_DSU_MEMTYPE            (0x41003FCCU) /**< \brief (DSU) Coresight ROM Table Memory Type */
#define REG_DSU_PID4               (0x41003FD0U) /**< \brief (DSU) Peripheral Identification 4 */
#define REG_DSU_PID0               (0x41003FE0U) /**< \brief (DSU) Peripheral Identification 0 */
#define REG_DSU_PID1               (0x41003FE4U) /**< \brief (DSU) Peripheral Identification 1 */
#define REG_DSU_PID2               (0x41003FE8U) /**< \brief (DSU) Peripheral Identification 2 */
#define REG_DSU_PID3               (0x41003FECU) /**< \brief (DSU) Peripheral Identification 3 */
#define REG_DSU_CID0               (0x41003FF0U) /**< \brief (DSU) Component Identification 0 */
#define REG_DSU_CID1               (0x41003FF4U) /**< \brief (DSU) Component Identification 1 */
#define REG_DSU_CID2               (0x41003FF8U) /**< \brief (DSU) Component Identification 2 */
#define REG_DSU_CID3               (0x41003FFCU) /**< \brief (DSU) Component Identification 3 */
#else
#define REG_DSU_CTRL               (*(WoReg8 *)0x41002000U) /**< \brief (DSU) Control */
#define REG_DSU_STATUSA            (*(RwReg8 *)0x41002001U) /**< \brief (DSU) Status A */
#define REG_DSU_STATUSB            (*(RoReg8 *)0x41002002U) /**< \brief (DSU) Status B */
#define REG_DSU_ADDR               (*(RwReg  *)0x41002004U) /**< \brief (DSU) Address */
#define REG_DSU_LENGTH             (*(RwReg  *)0x41002008U) /**< \brief (DSU) Length */
#define REG_DSU_DATA               (*(RwReg  *)0x4100200CU) /**< \brief (DSU) Data */
#define REG_DSU_DCC0               (*(RwReg  *)0x41002010U) /**< \brief (DSU) Debug Communication Channel 0 */
#define REG_DSU_DCC1               (*(RwReg  *)0x41002014U) /**< \brief (DSU) Debug Communication Channel 1 */
#define REG_DSU_DID                (*(RoReg  *)0x41002018U) /**< \brief (DSU) Device Identification */
#define REG_DSU_ENTRY0             (*(RoReg  *)0x41003000U) /**< \brief (DSU) Coresight ROM Table Entry 0 */
#define REG_DSU_ENTRY1             (*(RoReg  *)0x41003004U) /**< \brief (DSU) Coresight ROM Table Entry 1 */
#define REG_DSU_END                (*(RoReg  *)0x41003008U) /**< \brief (DSU) Coresight ROM Table End */
#define REG_DSU_MEMTYPE            (*(RoReg  *)0x41003FCCU) /**< \brief (DSU) Coresight ROM Table Memory Type */
#define REG_DSU_PID4               (*(RoReg  *)0x41003FD0U) /**< \brief (DSU) Peripheral Identification 4 */
#define REG_DSU_PID0               (*(RoReg  *)0x41003FE0U) /**< \brief (DSU) Peripheral Identification 0 */
#define REG_DSU_PID1               (*(RoReg  *)0x41003FE4U) /**< \brief (DSU) Peripheral Identification 1 */
#define REG_DSU_PID2               (*(RoReg  *)0x41003FE8U) /**< \brief (DSU) Peripheral Identification 2 */
#define REG_DSU_PID3               (*(RoReg  *)0x41003FECU) /**< \brief (DSU) Peripheral Identification 3 */
#define REG_DSU_CID0               (*(RoReg  *)0x41003FF0U) /**< \brief (DSU) Component Identification 0 */
#define REG_DSU_CID1               (*(RoReg  *)0x41003FF4U) /**< \brief (DSU) Component Identification 1 */
#define REG_DSU_CID2               (*(RoReg  *)0x41003FF8U) /**< \brief (DSU) Component Identification 2 */
#define REG_DSU_CID3               (*(RoReg  *)0x41003FFCU) /**< \brief (DSU) Component Identification 3 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for DSU peripheral ========== */
#define DSU_CLK_HSB_ID              3        // Index of AHB clock in PM.AHBMASK register

#endif /* _SAMD21_DSU_INSTANCE_ */
