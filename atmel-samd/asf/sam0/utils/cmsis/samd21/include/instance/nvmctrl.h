/**
 * \file
 *
 * \brief Instance description for NVMCTRL
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

#ifndef _SAMD21_NVMCTRL_INSTANCE_
#define _SAMD21_NVMCTRL_INSTANCE_

/* ========== Register definition for NVMCTRL peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_NVMCTRL_CTRLA          (0x41004000U) /**< \brief (NVMCTRL) Control A */
#define REG_NVMCTRL_CTRLB          (0x41004004U) /**< \brief (NVMCTRL) Control B */
#define REG_NVMCTRL_PARAM          (0x41004008U) /**< \brief (NVMCTRL) NVM Parameter */
#define REG_NVMCTRL_INTENCLR       (0x4100400CU) /**< \brief (NVMCTRL) Interrupt Enable Clear */
#define REG_NVMCTRL_INTENSET       (0x41004010U) /**< \brief (NVMCTRL) Interrupt Enable Set */
#define REG_NVMCTRL_INTFLAG        (0x41004014U) /**< \brief (NVMCTRL) Interrupt Flag Status and Clear */
#define REG_NVMCTRL_STATUS         (0x41004018U) /**< \brief (NVMCTRL) Status */
#define REG_NVMCTRL_ADDR           (0x4100401CU) /**< \brief (NVMCTRL) Address */
#define REG_NVMCTRL_LOCK           (0x41004020U) /**< \brief (NVMCTRL) Lock Section */
#else
#define REG_NVMCTRL_CTRLA          (*(RwReg16*)0x41004000U) /**< \brief (NVMCTRL) Control A */
#define REG_NVMCTRL_CTRLB          (*(RwReg  *)0x41004004U) /**< \brief (NVMCTRL) Control B */
#define REG_NVMCTRL_PARAM          (*(RwReg  *)0x41004008U) /**< \brief (NVMCTRL) NVM Parameter */
#define REG_NVMCTRL_INTENCLR       (*(RwReg8 *)0x4100400CU) /**< \brief (NVMCTRL) Interrupt Enable Clear */
#define REG_NVMCTRL_INTENSET       (*(RwReg8 *)0x41004010U) /**< \brief (NVMCTRL) Interrupt Enable Set */
#define REG_NVMCTRL_INTFLAG        (*(RwReg8 *)0x41004014U) /**< \brief (NVMCTRL) Interrupt Flag Status and Clear */
#define REG_NVMCTRL_STATUS         (*(RwReg16*)0x41004018U) /**< \brief (NVMCTRL) Status */
#define REG_NVMCTRL_ADDR           (*(RwReg  *)0x4100401CU) /**< \brief (NVMCTRL) Address */
#define REG_NVMCTRL_LOCK           (*(RwReg16*)0x41004020U) /**< \brief (NVMCTRL) Lock Section */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for NVMCTRL peripheral ========== */
#define NVMCTRL_AUX0_ADDRESS        0x00804000
#define NVMCTRL_AUX1_ADDRESS        0x00806000
#define NVMCTRL_AUX2_ADDRESS        0x00808000
#define NVMCTRL_AUX3_ADDRESS        0x0080A000
#define NVMCTRL_CLK_AHB_ID          4        // Index of AHB Clock in PM.AHBMASK register
#define NVMCTRL_FACTORY_WORD_IMPLEMENTED_MASK 0xC0000007FFFFFFFF
#define NVMCTRL_FLASH_SIZE          65536   
#define NVMCTRL_LOCKBIT_ADDRESS     0x00802000
#define NVMCTRL_PAGE_HW             32      
#define NVMCTRL_PAGE_SIZE           64      
#define NVMCTRL_PAGE_W              16      
#define NVMCTRL_PMSB                3       
#define NVMCTRL_PSZ_BITS            6       
#define NVMCTRL_ROW_PAGES           4       
#define NVMCTRL_ROW_SIZE            256     
#define NVMCTRL_USER_PAGE_ADDRESS   0x00800000
#define NVMCTRL_USER_PAGE_OFFSET    0x00800000
#define NVMCTRL_USER_WORD_IMPLEMENTED_MASK 0xC01FFFFFFFFFFFFF
#define NVMCTRL_RWWEE_PAGES         32       // Page size
#define NVMCTRL_RWW_EEPROM_ADDR     0x00400000 // Start address of the RWW EEPROM area

#endif /* _SAMD21_NVMCTRL_INSTANCE_ */
