/**
 * \file
 *
 * \brief Instance description for DSU
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

#ifndef _SAMD51_DSU_INSTANCE_
#define _SAMD51_DSU_INSTANCE_

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
#define REG_DSU_CFG                (0x4100201C) /**< \brief (DSU) Configuration */
#define REG_DSU_MBCTRL             (0x41002040) /**< \brief (DSU) MBIST Control */
#define REG_DSU_MBCONFIG           (0x41002044) /**< \brief (DSU) MBIST Configuration */
#define REG_DSU_MBWORD             (0x41002048) /**< \brief (DSU) MBIST Background Word */
#define REG_DSU_MBGSTAT            (0x4100204C) /**< \brief (DSU) MBIST Global Status */
#define REG_DSU_MBDFAIL            (0x41002050) /**< \brief (DSU) MBIST Fail Data */
#define REG_DSU_MBDEXP             (0x41002054) /**< \brief (DSU) MBIST Expected Data */
#define REG_DSU_MBAFAIL            (0x41002058) /**< \brief (DSU) MBIST Fail Address */
#define REG_DSU_MBCONTEXT          (0x4100205C) /**< \brief (DSU) MBIST Fail Context */
#define REG_DSU_MBENABLE0          (0x41002060) /**< \brief (DSU) MBIST Memory Enable 0 */
#define REG_DSU_MBBUSY0            (0x41002068) /**< \brief (DSU) MBIST Memory Busy 0 */
#define REG_DSU_MBSTATUS0          (0x41002070) /**< \brief (DSU) MBIST Memory Status 0 */
#define REG_DSU_DCFG0              (0x410020F0) /**< \brief (DSU) Device Configuration 0 */
#define REG_DSU_DCFG1              (0x410020F4) /**< \brief (DSU) Device Configuration 1 */
#define REG_DSU_ENTRY0             (0x41003000) /**< \brief (DSU) CoreSight ROM Table Entry 0 */
#define REG_DSU_ENTRY1             (0x41003004) /**< \brief (DSU) CoreSight ROM Table Entry 1 */
#define REG_DSU_END                (0x41003008) /**< \brief (DSU) CoreSight ROM Table End */
#define REG_DSU_MEMTYPE            (0x41003FCC) /**< \brief (DSU) CoreSight ROM Table Memory Type */
#define REG_DSU_PID4               (0x41003FD0) /**< \brief (DSU) Peripheral Identification 4 */
#define REG_DSU_PID5               (0x41003FD4) /**< \brief (DSU) Peripheral Identification 5 */
#define REG_DSU_PID6               (0x41003FD8) /**< \brief (DSU) Peripheral Identification 6 */
#define REG_DSU_PID7               (0x41003FDC) /**< \brief (DSU) Peripheral Identification 7 */
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
#define REG_DSU_CFG                (*(RwReg  *)0x4100201CUL) /**< \brief (DSU) Configuration */
#define REG_DSU_MBCTRL             (*(RwReg  *)0x41002040UL) /**< \brief (DSU) MBIST Control */
#define REG_DSU_MBCONFIG           (*(RwReg  *)0x41002044UL) /**< \brief (DSU) MBIST Configuration */
#define REG_DSU_MBWORD             (*(RwReg  *)0x41002048UL) /**< \brief (DSU) MBIST Background Word */
#define REG_DSU_MBGSTAT            (*(RwReg  *)0x4100204CUL) /**< \brief (DSU) MBIST Global Status */
#define REG_DSU_MBDFAIL            (*(RoReg  *)0x41002050UL) /**< \brief (DSU) MBIST Fail Data */
#define REG_DSU_MBDEXP             (*(RoReg  *)0x41002054UL) /**< \brief (DSU) MBIST Expected Data */
#define REG_DSU_MBAFAIL            (*(RoReg  *)0x41002058UL) /**< \brief (DSU) MBIST Fail Address */
#define REG_DSU_MBCONTEXT          (*(RoReg  *)0x4100205CUL) /**< \brief (DSU) MBIST Fail Context */
#define REG_DSU_MBENABLE0          (*(RwReg  *)0x41002060UL) /**< \brief (DSU) MBIST Memory Enable 0 */
#define REG_DSU_MBBUSY0            (*(RoReg  *)0x41002068UL) /**< \brief (DSU) MBIST Memory Busy 0 */
#define REG_DSU_MBSTATUS0          (*(RwReg  *)0x41002070UL) /**< \brief (DSU) MBIST Memory Status 0 */
#define REG_DSU_DCFG0              (*(RwReg  *)0x410020F0UL) /**< \brief (DSU) Device Configuration 0 */
#define REG_DSU_DCFG1              (*(RwReg  *)0x410020F4UL) /**< \brief (DSU) Device Configuration 1 */
#define REG_DSU_ENTRY0             (*(RoReg  *)0x41003000UL) /**< \brief (DSU) CoreSight ROM Table Entry 0 */
#define REG_DSU_ENTRY1             (*(RoReg  *)0x41003004UL) /**< \brief (DSU) CoreSight ROM Table Entry 1 */
#define REG_DSU_END                (*(RoReg  *)0x41003008UL) /**< \brief (DSU) CoreSight ROM Table End */
#define REG_DSU_MEMTYPE            (*(RoReg  *)0x41003FCCUL) /**< \brief (DSU) CoreSight ROM Table Memory Type */
#define REG_DSU_PID4               (*(RoReg  *)0x41003FD0UL) /**< \brief (DSU) Peripheral Identification 4 */
#define REG_DSU_PID5               (*(RoReg  *)0x41003FD4UL) /**< \brief (DSU) Peripheral Identification 5 */
#define REG_DSU_PID6               (*(RoReg  *)0x41003FD8UL) /**< \brief (DSU) Peripheral Identification 6 */
#define REG_DSU_PID7               (*(RoReg  *)0x41003FDCUL) /**< \brief (DSU) Peripheral Identification 7 */
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
#define DSU_CLK_AHB_ID              4       
#define DSU_DMAC_ID_DCC0            2        // DMAC ID for DCC0 register
#define DSU_DMAC_ID_DCC1            3        // DMAC ID for DCC1 register

#endif /* _SAMD51_DSU_INSTANCE_ */
