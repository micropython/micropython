/**
 * \file
 *
 * \brief Instance description for DSU
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

#ifndef _SAMD21_DSU_INSTANCE_
#define _SAMD21_DSU_INSTANCE_

/* ========== Register definition for DSU peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_DSU_CTRL               (0x41002000) /**< \brief (DSU) Control */
#define REG_DSU_STATUSA            (0x41002001) /**< \brief (DSU) Status A */
#define REG_DSU_STATUSB            (0x41002002) /**< \brief (DSU) Status B */
#define REG_DSU_ADDR               (0x41002004) /**< \brief (DSU) Address */
#define REG_DSU_LENGTH             (0x41002008) /**< \brief (DSU) Length */
#define REG_DSU_DATA               (0x4100200C) /**< \brief (DSU) Data */
#define REG_DSU_DCC0               (0x41002010) /**< \brief (DSU) Debug Communication Channel 0 */
#define REG_DSU_DCC1               (0x41002014) /**< \brief (DSU) Debug Communication Channel 1 */
#define REG_DSU_DID                (0x41002018) /**< \brief (DSU) Device Identification */
#define REG_DSU_ENTRY0             (0x41003000) /**< \brief (DSU) Coresight ROM Table Entry 0 */
#define REG_DSU_ENTRY1             (0x41003004) /**< \brief (DSU) Coresight ROM Table Entry 1 */
#define REG_DSU_END                (0x41003008) /**< \brief (DSU) Coresight ROM Table End */
#define REG_DSU_MEMTYPE            (0x41003FCC) /**< \brief (DSU) Coresight ROM Table Memory Type */
#define REG_DSU_PID4               (0x41003FD0) /**< \brief (DSU) Peripheral Identification 4 */
#define REG_DSU_PID0               (0x41003FE0) /**< \brief (DSU) Peripheral Identification 0 */
#define REG_DSU_PID1               (0x41003FE4) /**< \brief (DSU) Peripheral Identification 1 */
#define REG_DSU_PID2               (0x41003FE8) /**< \brief (DSU) Peripheral Identification 2 */
#define REG_DSU_PID3               (0x41003FEC) /**< \brief (DSU) Peripheral Identification 3 */
#define REG_DSU_CID0               (0x41003FF0) /**< \brief (DSU) Component Identification 0 */
#define REG_DSU_CID1               (0x41003FF4) /**< \brief (DSU) Component Identification 1 */
#define REG_DSU_CID2               (0x41003FF8) /**< \brief (DSU) Component Identification 2 */
#define REG_DSU_CID3               (0x41003FFC) /**< \brief (DSU) Component Identification 3 */
#else
#define REG_DSU_CTRL               (*(WoReg8 *)0x41002000UL) /**< \brief (DSU) Control */
#define REG_DSU_STATUSA            (*(RwReg8 *)0x41002001UL) /**< \brief (DSU) Status A */
#define REG_DSU_STATUSB            (*(RoReg8 *)0x41002002UL) /**< \brief (DSU) Status B */
#define REG_DSU_ADDR               (*(RwReg  *)0x41002004UL) /**< \brief (DSU) Address */
#define REG_DSU_LENGTH             (*(RwReg  *)0x41002008UL) /**< \brief (DSU) Length */
#define REG_DSU_DATA               (*(RwReg  *)0x4100200CUL) /**< \brief (DSU) Data */
#define REG_DSU_DCC0               (*(RwReg  *)0x41002010UL) /**< \brief (DSU) Debug Communication Channel 0 */
#define REG_DSU_DCC1               (*(RwReg  *)0x41002014UL) /**< \brief (DSU) Debug Communication Channel 1 */
#define REG_DSU_DID                (*(RoReg  *)0x41002018UL) /**< \brief (DSU) Device Identification */
#define REG_DSU_ENTRY0             (*(RoReg  *)0x41003000UL) /**< \brief (DSU) Coresight ROM Table Entry 0 */
#define REG_DSU_ENTRY1             (*(RoReg  *)0x41003004UL) /**< \brief (DSU) Coresight ROM Table Entry 1 */
#define REG_DSU_END                (*(RoReg  *)0x41003008UL) /**< \brief (DSU) Coresight ROM Table End */
#define REG_DSU_MEMTYPE            (*(RoReg  *)0x41003FCCUL) /**< \brief (DSU) Coresight ROM Table Memory Type */
#define REG_DSU_PID4               (*(RoReg  *)0x41003FD0UL) /**< \brief (DSU) Peripheral Identification 4 */
#define REG_DSU_PID0               (*(RoReg  *)0x41003FE0UL) /**< \brief (DSU) Peripheral Identification 0 */
#define REG_DSU_PID1               (*(RoReg  *)0x41003FE4UL) /**< \brief (DSU) Peripheral Identification 1 */
#define REG_DSU_PID2               (*(RoReg  *)0x41003FE8UL) /**< \brief (DSU) Peripheral Identification 2 */
#define REG_DSU_PID3               (*(RoReg  *)0x41003FECUL) /**< \brief (DSU) Peripheral Identification 3 */
#define REG_DSU_CID0               (*(RoReg  *)0x41003FF0UL) /**< \brief (DSU) Component Identification 0 */
#define REG_DSU_CID1               (*(RoReg  *)0x41003FF4UL) /**< \brief (DSU) Component Identification 1 */
#define REG_DSU_CID2               (*(RoReg  *)0x41003FF8UL) /**< \brief (DSU) Component Identification 2 */
#define REG_DSU_CID3               (*(RoReg  *)0x41003FFCUL) /**< \brief (DSU) Component Identification 3 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for DSU peripheral ========== */
#define DSU_CLK_HSB_ID              3        // Index of AHB clock in PM.AHBMASK register

#endif /* _SAMD21_DSU_INSTANCE_ */
