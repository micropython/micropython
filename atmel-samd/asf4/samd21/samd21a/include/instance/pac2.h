/**
 * \file
 *
 * \brief Instance description for PAC2
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

#ifndef _SAMD21_PAC2_INSTANCE_
#define _SAMD21_PAC2_INSTANCE_

/* ========== Register definition for PAC2 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_PAC2_WPCLR             (0x42000000) /**< \brief (PAC2) Write Protection Clear */
#define REG_PAC2_WPSET             (0x42000004) /**< \brief (PAC2) Write Protection Set */
#else
#define REG_PAC2_WPCLR             (*(RwReg  *)0x42000000UL) /**< \brief (PAC2) Write Protection Clear */
#define REG_PAC2_WPSET             (*(RwReg  *)0x42000004UL) /**< \brief (PAC2) Write Protection Set */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for PAC2 peripheral ========== */
#define PAC2_WPROT_DEFAULT_VAL      0x00800000 // PAC protection mask at reset

#endif /* _SAMD21_PAC2_INSTANCE_ */
