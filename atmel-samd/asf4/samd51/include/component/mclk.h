/**
 * \file
 *
 * \brief Component description for MCLK
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

#ifndef _SAMD51_MCLK_COMPONENT_
#define _SAMD51_MCLK_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR MCLK */
/* ========================================================================== */
/** \addtogroup SAMD51_MCLK Main Clock */
/*@{*/

#define MCLK_U2408
#define REV_MCLK                    0x100

/* -------- MCLK_INTENCLR : (MCLK Offset: 0x01) (R/W  8) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CKRDY:1;          /*!< bit:      0  Clock Ready Interrupt Enable       */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} MCLK_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MCLK_INTENCLR_OFFSET        0x01         /**< \brief (MCLK_INTENCLR offset) Interrupt Enable Clear */
#define MCLK_INTENCLR_RESETVALUE    _U_(0x00)    /**< \brief (MCLK_INTENCLR reset_value) Interrupt Enable Clear */

#define MCLK_INTENCLR_CKRDY_Pos     0            /**< \brief (MCLK_INTENCLR) Clock Ready Interrupt Enable */
#define MCLK_INTENCLR_CKRDY         (_U_(0x1) << MCLK_INTENCLR_CKRDY_Pos)
#define MCLK_INTENCLR_MASK          _U_(0x01)    /**< \brief (MCLK_INTENCLR) MASK Register */

/* -------- MCLK_INTENSET : (MCLK Offset: 0x02) (R/W  8) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CKRDY:1;          /*!< bit:      0  Clock Ready Interrupt Enable       */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} MCLK_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MCLK_INTENSET_OFFSET        0x02         /**< \brief (MCLK_INTENSET offset) Interrupt Enable Set */
#define MCLK_INTENSET_RESETVALUE    _U_(0x00)    /**< \brief (MCLK_INTENSET reset_value) Interrupt Enable Set */

#define MCLK_INTENSET_CKRDY_Pos     0            /**< \brief (MCLK_INTENSET) Clock Ready Interrupt Enable */
#define MCLK_INTENSET_CKRDY         (_U_(0x1) << MCLK_INTENSET_CKRDY_Pos)
#define MCLK_INTENSET_MASK          _U_(0x01)    /**< \brief (MCLK_INTENSET) MASK Register */

/* -------- MCLK_INTFLAG : (MCLK Offset: 0x03) (R/W  8) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint8_t  CKRDY:1;          /*!< bit:      0  Clock Ready                        */
    __I uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} MCLK_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MCLK_INTFLAG_OFFSET         0x03         /**< \brief (MCLK_INTFLAG offset) Interrupt Flag Status and Clear */
#define MCLK_INTFLAG_RESETVALUE     _U_(0x01)    /**< \brief (MCLK_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define MCLK_INTFLAG_CKRDY_Pos      0            /**< \brief (MCLK_INTFLAG) Clock Ready */
#define MCLK_INTFLAG_CKRDY          (_U_(0x1) << MCLK_INTFLAG_CKRDY_Pos)
#define MCLK_INTFLAG_MASK           _U_(0x01)    /**< \brief (MCLK_INTFLAG) MASK Register */

/* -------- MCLK_HSDIV : (MCLK Offset: 0x04) (R/   8) HS Clock Division -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DIV:8;            /*!< bit:  0.. 7  CPU Clock Division Factor          */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} MCLK_HSDIV_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MCLK_HSDIV_OFFSET           0x04         /**< \brief (MCLK_HSDIV offset) HS Clock Division */
#define MCLK_HSDIV_RESETVALUE       _U_(0x01)    /**< \brief (MCLK_HSDIV reset_value) HS Clock Division */

#define MCLK_HSDIV_DIV_Pos          0            /**< \brief (MCLK_HSDIV) CPU Clock Division Factor */
#define MCLK_HSDIV_DIV_Msk          (_U_(0xFF) << MCLK_HSDIV_DIV_Pos)
#define MCLK_HSDIV_DIV(value)       (MCLK_HSDIV_DIV_Msk & ((value) << MCLK_HSDIV_DIV_Pos))
#define   MCLK_HSDIV_DIV_DIV1_Val         _U_(0x1)   /**< \brief (MCLK_HSDIV) Divide by 1 */
#define MCLK_HSDIV_DIV_DIV1         (MCLK_HSDIV_DIV_DIV1_Val       << MCLK_HSDIV_DIV_Pos)
#define MCLK_HSDIV_MASK             _U_(0xFF)    /**< \brief (MCLK_HSDIV) MASK Register */

/* -------- MCLK_CPUDIV : (MCLK Offset: 0x05) (R/W  8) CPU Clock Division -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DIV:8;            /*!< bit:  0.. 7  Low-Power Clock Division Factor    */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} MCLK_CPUDIV_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MCLK_CPUDIV_OFFSET          0x05         /**< \brief (MCLK_CPUDIV offset) CPU Clock Division */
#define MCLK_CPUDIV_RESETVALUE      _U_(0x01)    /**< \brief (MCLK_CPUDIV reset_value) CPU Clock Division */

#define MCLK_CPUDIV_DIV_Pos         0            /**< \brief (MCLK_CPUDIV) Low-Power Clock Division Factor */
#define MCLK_CPUDIV_DIV_Msk         (_U_(0xFF) << MCLK_CPUDIV_DIV_Pos)
#define MCLK_CPUDIV_DIV(value)      (MCLK_CPUDIV_DIV_Msk & ((value) << MCLK_CPUDIV_DIV_Pos))
#define   MCLK_CPUDIV_DIV_DIV1_Val        _U_(0x1)   /**< \brief (MCLK_CPUDIV) Divide by 1 */
#define   MCLK_CPUDIV_DIV_DIV2_Val        _U_(0x2)   /**< \brief (MCLK_CPUDIV) Divide by 2 */
#define   MCLK_CPUDIV_DIV_DIV4_Val        _U_(0x4)   /**< \brief (MCLK_CPUDIV) Divide by 4 */
#define   MCLK_CPUDIV_DIV_DIV8_Val        _U_(0x8)   /**< \brief (MCLK_CPUDIV) Divide by 8 */
#define   MCLK_CPUDIV_DIV_DIV16_Val       _U_(0x10)   /**< \brief (MCLK_CPUDIV) Divide by 16 */
#define   MCLK_CPUDIV_DIV_DIV32_Val       _U_(0x20)   /**< \brief (MCLK_CPUDIV) Divide by 32 */
#define   MCLK_CPUDIV_DIV_DIV64_Val       _U_(0x40)   /**< \brief (MCLK_CPUDIV) Divide by 64 */
#define   MCLK_CPUDIV_DIV_DIV128_Val      _U_(0x80)   /**< \brief (MCLK_CPUDIV) Divide by 128 */
#define MCLK_CPUDIV_DIV_DIV1        (MCLK_CPUDIV_DIV_DIV1_Val      << MCLK_CPUDIV_DIV_Pos)
#define MCLK_CPUDIV_DIV_DIV2        (MCLK_CPUDIV_DIV_DIV2_Val      << MCLK_CPUDIV_DIV_Pos)
#define MCLK_CPUDIV_DIV_DIV4        (MCLK_CPUDIV_DIV_DIV4_Val      << MCLK_CPUDIV_DIV_Pos)
#define MCLK_CPUDIV_DIV_DIV8        (MCLK_CPUDIV_DIV_DIV8_Val      << MCLK_CPUDIV_DIV_Pos)
#define MCLK_CPUDIV_DIV_DIV16       (MCLK_CPUDIV_DIV_DIV16_Val     << MCLK_CPUDIV_DIV_Pos)
#define MCLK_CPUDIV_DIV_DIV32       (MCLK_CPUDIV_DIV_DIV32_Val     << MCLK_CPUDIV_DIV_Pos)
#define MCLK_CPUDIV_DIV_DIV64       (MCLK_CPUDIV_DIV_DIV64_Val     << MCLK_CPUDIV_DIV_Pos)
#define MCLK_CPUDIV_DIV_DIV128      (MCLK_CPUDIV_DIV_DIV128_Val    << MCLK_CPUDIV_DIV_Pos)
#define MCLK_CPUDIV_MASK            _U_(0xFF)    /**< \brief (MCLK_CPUDIV) MASK Register */

/* -------- MCLK_AHBMASK : (MCLK Offset: 0x10) (R/W 32) AHB Mask -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t HPB0_:1;          /*!< bit:      0  HPB0 AHB Clock Mask                */
    uint32_t HPB1_:1;          /*!< bit:      1  HPB1 AHB Clock Mask                */
    uint32_t HPB2_:1;          /*!< bit:      2  HPB2 AHB Clock Mask                */
    uint32_t HPB3_:1;          /*!< bit:      3  HPB3 AHB Clock Mask                */
    uint32_t DSU_:1;           /*!< bit:      4  DSU AHB Clock Mask                 */
    uint32_t HMATRIX_:1;       /*!< bit:      5  HMATRIX AHB Clock Mask             */
    uint32_t NVMCTRL_:1;       /*!< bit:      6  NVMCTRL AHB Clock Mask             */
    uint32_t HSRAM_:1;         /*!< bit:      7  HSRAM AHB Clock Mask               */
    uint32_t CMCC_:1;          /*!< bit:      8  CMCC AHB Clock Mask                */
    uint32_t DMAC_:1;          /*!< bit:      9  DMAC AHB Clock Mask                */
    uint32_t USB_:1;           /*!< bit:     10  USB AHB Clock Mask                 */
    uint32_t BKUPRAM_:1;       /*!< bit:     11  BKUPRAM AHB Clock Mask             */
    uint32_t PAC_:1;           /*!< bit:     12  PAC AHB Clock Mask                 */
    uint32_t QSPI_:1;          /*!< bit:     13  QSPI AHB Clock Mask                */
    uint32_t :1;               /*!< bit:     14  Reserved                           */
    uint32_t SDHC0_:1;         /*!< bit:     15  SDHC0 AHB Clock Mask               */
    uint32_t SDHC1_:1;         /*!< bit:     16  SDHC1 AHB Clock Mask               */
    uint32_t :2;               /*!< bit: 17..18  Reserved                           */
    uint32_t ICM_:1;           /*!< bit:     19  ICM AHB Clock Mask                 */
    uint32_t PUKCC_:1;         /*!< bit:     20  PUKCC AHB Clock Mask               */
    uint32_t QSPI_2X_:1;       /*!< bit:     21  QSPI_2X AHB Clock Mask             */
    uint32_t NVMCTRL_SMEEPROM_:1; /*!< bit:     22  NVMCTRL_SMEEPROM AHB Clock Mask    */
    uint32_t NVMCTRL_CACHE_:1; /*!< bit:     23  NVMCTRL_CACHE AHB Clock Mask       */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} MCLK_AHBMASK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MCLK_AHBMASK_OFFSET         0x10         /**< \brief (MCLK_AHBMASK offset) AHB Mask */
#define MCLK_AHBMASK_RESETVALUE     _U_(0x00FFFFFF) /**< \brief (MCLK_AHBMASK reset_value) AHB Mask */

#define MCLK_AHBMASK_HPB0_Pos       0            /**< \brief (MCLK_AHBMASK) HPB0 AHB Clock Mask */
#define MCLK_AHBMASK_HPB0           (_U_(0x1) << MCLK_AHBMASK_HPB0_Pos)
#define MCLK_AHBMASK_HPB1_Pos       1            /**< \brief (MCLK_AHBMASK) HPB1 AHB Clock Mask */
#define MCLK_AHBMASK_HPB1           (_U_(0x1) << MCLK_AHBMASK_HPB1_Pos)
#define MCLK_AHBMASK_HPB2_Pos       2            /**< \brief (MCLK_AHBMASK) HPB2 AHB Clock Mask */
#define MCLK_AHBMASK_HPB2           (_U_(0x1) << MCLK_AHBMASK_HPB2_Pos)
#define MCLK_AHBMASK_HPB3_Pos       3            /**< \brief (MCLK_AHBMASK) HPB3 AHB Clock Mask */
#define MCLK_AHBMASK_HPB3           (_U_(0x1) << MCLK_AHBMASK_HPB3_Pos)
#define MCLK_AHBMASK_DSU_Pos        4            /**< \brief (MCLK_AHBMASK) DSU AHB Clock Mask */
#define MCLK_AHBMASK_DSU            (_U_(0x1) << MCLK_AHBMASK_DSU_Pos)
#define MCLK_AHBMASK_HMATRIX_Pos    5            /**< \brief (MCLK_AHBMASK) HMATRIX AHB Clock Mask */
#define MCLK_AHBMASK_HMATRIX        (_U_(0x1) << MCLK_AHBMASK_HMATRIX_Pos)
#define MCLK_AHBMASK_NVMCTRL_Pos    6            /**< \brief (MCLK_AHBMASK) NVMCTRL AHB Clock Mask */
#define MCLK_AHBMASK_NVMCTRL        (_U_(0x1) << MCLK_AHBMASK_NVMCTRL_Pos)
#define MCLK_AHBMASK_HSRAM_Pos      7            /**< \brief (MCLK_AHBMASK) HSRAM AHB Clock Mask */
#define MCLK_AHBMASK_HSRAM          (_U_(0x1) << MCLK_AHBMASK_HSRAM_Pos)
#define MCLK_AHBMASK_CMCC_Pos       8            /**< \brief (MCLK_AHBMASK) CMCC AHB Clock Mask */
#define MCLK_AHBMASK_CMCC           (_U_(0x1) << MCLK_AHBMASK_CMCC_Pos)
#define MCLK_AHBMASK_DMAC_Pos       9            /**< \brief (MCLK_AHBMASK) DMAC AHB Clock Mask */
#define MCLK_AHBMASK_DMAC           (_U_(0x1) << MCLK_AHBMASK_DMAC_Pos)
#define MCLK_AHBMASK_USB_Pos        10           /**< \brief (MCLK_AHBMASK) USB AHB Clock Mask */
#define MCLK_AHBMASK_USB            (_U_(0x1) << MCLK_AHBMASK_USB_Pos)
#define MCLK_AHBMASK_BKUPRAM_Pos    11           /**< \brief (MCLK_AHBMASK) BKUPRAM AHB Clock Mask */
#define MCLK_AHBMASK_BKUPRAM        (_U_(0x1) << MCLK_AHBMASK_BKUPRAM_Pos)
#define MCLK_AHBMASK_PAC_Pos        12           /**< \brief (MCLK_AHBMASK) PAC AHB Clock Mask */
#define MCLK_AHBMASK_PAC            (_U_(0x1) << MCLK_AHBMASK_PAC_Pos)
#define MCLK_AHBMASK_QSPI_Pos       13           /**< \brief (MCLK_AHBMASK) QSPI AHB Clock Mask */
#define MCLK_AHBMASK_QSPI           (_U_(0x1) << MCLK_AHBMASK_QSPI_Pos)
#define MCLK_AHBMASK_SDHC0_Pos      15           /**< \brief (MCLK_AHBMASK) SDHC0 AHB Clock Mask */
#define MCLK_AHBMASK_SDHC0          (_U_(0x1) << MCLK_AHBMASK_SDHC0_Pos)
#define MCLK_AHBMASK_SDHC1_Pos      16           /**< \brief (MCLK_AHBMASK) SDHC1 AHB Clock Mask */
#define MCLK_AHBMASK_SDHC1          (_U_(0x1) << MCLK_AHBMASK_SDHC1_Pos)
#define MCLK_AHBMASK_ICM_Pos        19           /**< \brief (MCLK_AHBMASK) ICM AHB Clock Mask */
#define MCLK_AHBMASK_ICM            (_U_(0x1) << MCLK_AHBMASK_ICM_Pos)
#define MCLK_AHBMASK_PUKCC_Pos      20           /**< \brief (MCLK_AHBMASK) PUKCC AHB Clock Mask */
#define MCLK_AHBMASK_PUKCC          (_U_(0x1) << MCLK_AHBMASK_PUKCC_Pos)
#define MCLK_AHBMASK_QSPI_2X_Pos    21           /**< \brief (MCLK_AHBMASK) QSPI_2X AHB Clock Mask */
#define MCLK_AHBMASK_QSPI_2X        (_U_(0x1) << MCLK_AHBMASK_QSPI_2X_Pos)
#define MCLK_AHBMASK_NVMCTRL_SMEEPROM_Pos 22           /**< \brief (MCLK_AHBMASK) NVMCTRL_SMEEPROM AHB Clock Mask */
#define MCLK_AHBMASK_NVMCTRL_SMEEPROM (_U_(0x1) << MCLK_AHBMASK_NVMCTRL_SMEEPROM_Pos)
#define MCLK_AHBMASK_NVMCTRL_CACHE_Pos 23           /**< \brief (MCLK_AHBMASK) NVMCTRL_CACHE AHB Clock Mask */
#define MCLK_AHBMASK_NVMCTRL_CACHE  (_U_(0x1) << MCLK_AHBMASK_NVMCTRL_CACHE_Pos)
#define MCLK_AHBMASK_MASK           _U_(0x00F9BFFF) /**< \brief (MCLK_AHBMASK) MASK Register */

/* -------- MCLK_APBAMASK : (MCLK Offset: 0x14) (R/W 32) APBA Mask -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PAC_:1;           /*!< bit:      0  PAC APB Clock Enable               */
    uint32_t PM_:1;            /*!< bit:      1  PM APB Clock Enable                */
    uint32_t MCLK_:1;          /*!< bit:      2  MCLK APB Clock Enable              */
    uint32_t RSTC_:1;          /*!< bit:      3  RSTC APB Clock Enable              */
    uint32_t OSCCTRL_:1;       /*!< bit:      4  OSCCTRL APB Clock Enable           */
    uint32_t OSC32KCTRL_:1;    /*!< bit:      5  OSC32KCTRL APB Clock Enable        */
    uint32_t SUPC_:1;          /*!< bit:      6  SUPC APB Clock Enable              */
    uint32_t GCLK_:1;          /*!< bit:      7  GCLK APB Clock Enable              */
    uint32_t WDT_:1;           /*!< bit:      8  WDT APB Clock Enable               */
    uint32_t RTC_:1;           /*!< bit:      9  RTC APB Clock Enable               */
    uint32_t EIC_:1;           /*!< bit:     10  EIC APB Clock Enable               */
    uint32_t FREQM_:1;         /*!< bit:     11  FREQM APB Clock Enable             */
    uint32_t SERCOM0_:1;       /*!< bit:     12  SERCOM0 APB Clock Enable           */
    uint32_t SERCOM1_:1;       /*!< bit:     13  SERCOM1 APB Clock Enable           */
    uint32_t TC0_:1;           /*!< bit:     14  TC0 APB Clock Enable               */
    uint32_t TC1_:1;           /*!< bit:     15  TC1 APB Clock Enable               */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} MCLK_APBAMASK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MCLK_APBAMASK_OFFSET        0x14         /**< \brief (MCLK_APBAMASK offset) APBA Mask */
#define MCLK_APBAMASK_RESETVALUE    _U_(0x000007FF) /**< \brief (MCLK_APBAMASK reset_value) APBA Mask */

#define MCLK_APBAMASK_PAC_Pos       0            /**< \brief (MCLK_APBAMASK) PAC APB Clock Enable */
#define MCLK_APBAMASK_PAC           (_U_(0x1) << MCLK_APBAMASK_PAC_Pos)
#define MCLK_APBAMASK_PM_Pos        1            /**< \brief (MCLK_APBAMASK) PM APB Clock Enable */
#define MCLK_APBAMASK_PM            (_U_(0x1) << MCLK_APBAMASK_PM_Pos)
#define MCLK_APBAMASK_MCLK_Pos      2            /**< \brief (MCLK_APBAMASK) MCLK APB Clock Enable */
#define MCLK_APBAMASK_MCLK          (_U_(0x1) << MCLK_APBAMASK_MCLK_Pos)
#define MCLK_APBAMASK_RSTC_Pos      3            /**< \brief (MCLK_APBAMASK) RSTC APB Clock Enable */
#define MCLK_APBAMASK_RSTC          (_U_(0x1) << MCLK_APBAMASK_RSTC_Pos)
#define MCLK_APBAMASK_OSCCTRL_Pos   4            /**< \brief (MCLK_APBAMASK) OSCCTRL APB Clock Enable */
#define MCLK_APBAMASK_OSCCTRL       (_U_(0x1) << MCLK_APBAMASK_OSCCTRL_Pos)
#define MCLK_APBAMASK_OSC32KCTRL_Pos 5            /**< \brief (MCLK_APBAMASK) OSC32KCTRL APB Clock Enable */
#define MCLK_APBAMASK_OSC32KCTRL    (_U_(0x1) << MCLK_APBAMASK_OSC32KCTRL_Pos)
#define MCLK_APBAMASK_SUPC_Pos      6            /**< \brief (MCLK_APBAMASK) SUPC APB Clock Enable */
#define MCLK_APBAMASK_SUPC          (_U_(0x1) << MCLK_APBAMASK_SUPC_Pos)
#define MCLK_APBAMASK_GCLK_Pos      7            /**< \brief (MCLK_APBAMASK) GCLK APB Clock Enable */
#define MCLK_APBAMASK_GCLK          (_U_(0x1) << MCLK_APBAMASK_GCLK_Pos)
#define MCLK_APBAMASK_WDT_Pos       8            /**< \brief (MCLK_APBAMASK) WDT APB Clock Enable */
#define MCLK_APBAMASK_WDT           (_U_(0x1) << MCLK_APBAMASK_WDT_Pos)
#define MCLK_APBAMASK_RTC_Pos       9            /**< \brief (MCLK_APBAMASK) RTC APB Clock Enable */
#define MCLK_APBAMASK_RTC           (_U_(0x1) << MCLK_APBAMASK_RTC_Pos)
#define MCLK_APBAMASK_EIC_Pos       10           /**< \brief (MCLK_APBAMASK) EIC APB Clock Enable */
#define MCLK_APBAMASK_EIC           (_U_(0x1) << MCLK_APBAMASK_EIC_Pos)
#define MCLK_APBAMASK_FREQM_Pos     11           /**< \brief (MCLK_APBAMASK) FREQM APB Clock Enable */
#define MCLK_APBAMASK_FREQM         (_U_(0x1) << MCLK_APBAMASK_FREQM_Pos)
#define MCLK_APBAMASK_SERCOM0_Pos   12           /**< \brief (MCLK_APBAMASK) SERCOM0 APB Clock Enable */
#define MCLK_APBAMASK_SERCOM0       (_U_(0x1) << MCLK_APBAMASK_SERCOM0_Pos)
#define MCLK_APBAMASK_SERCOM1_Pos   13           /**< \brief (MCLK_APBAMASK) SERCOM1 APB Clock Enable */
#define MCLK_APBAMASK_SERCOM1       (_U_(0x1) << MCLK_APBAMASK_SERCOM1_Pos)
#define MCLK_APBAMASK_TC0_Pos       14           /**< \brief (MCLK_APBAMASK) TC0 APB Clock Enable */
#define MCLK_APBAMASK_TC0           (_U_(0x1) << MCLK_APBAMASK_TC0_Pos)
#define MCLK_APBAMASK_TC1_Pos       15           /**< \brief (MCLK_APBAMASK) TC1 APB Clock Enable */
#define MCLK_APBAMASK_TC1           (_U_(0x1) << MCLK_APBAMASK_TC1_Pos)
#define MCLK_APBAMASK_MASK          _U_(0x0000FFFF) /**< \brief (MCLK_APBAMASK) MASK Register */

/* -------- MCLK_APBBMASK : (MCLK Offset: 0x18) (R/W 32) APBB Mask -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t USB_:1;           /*!< bit:      0  USB APB Clock Enable               */
    uint32_t DSU_:1;           /*!< bit:      1  DSU APB Clock Enable               */
    uint32_t NVMCTRL_:1;       /*!< bit:      2  NVMCTRL APB Clock Enable           */
    uint32_t :1;               /*!< bit:      3  Reserved                           */
    uint32_t PORT_:1;          /*!< bit:      4  PORT APB Clock Enable              */
    uint32_t :1;               /*!< bit:      5  Reserved                           */
    uint32_t HMATRIX_:1;       /*!< bit:      6  HMATRIX APB Clock Enable           */
    uint32_t EVSYS_:1;         /*!< bit:      7  EVSYS APB Clock Enable             */
    uint32_t :1;               /*!< bit:      8  Reserved                           */
    uint32_t SERCOM2_:1;       /*!< bit:      9  SERCOM2 APB Clock Enable           */
    uint32_t SERCOM3_:1;       /*!< bit:     10  SERCOM3 APB Clock Enable           */
    uint32_t TCC0_:1;          /*!< bit:     11  TCC0 APB Clock Enable              */
    uint32_t TCC1_:1;          /*!< bit:     12  TCC1 APB Clock Enable              */
    uint32_t TC2_:1;           /*!< bit:     13  TC2 APB Clock Enable               */
    uint32_t TC3_:1;           /*!< bit:     14  TC3 APB Clock Enable               */
    uint32_t TAL_:1;           /*!< bit:     15  TAL APB Clock Enable               */
    uint32_t RAMECC_:1;        /*!< bit:     16  RAMECC APB Clock Enable            */
    uint32_t :15;              /*!< bit: 17..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} MCLK_APBBMASK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MCLK_APBBMASK_OFFSET        0x18         /**< \brief (MCLK_APBBMASK offset) APBB Mask */
#define MCLK_APBBMASK_RESETVALUE    _U_(0x00018056) /**< \brief (MCLK_APBBMASK reset_value) APBB Mask */

#define MCLK_APBBMASK_USB_Pos       0            /**< \brief (MCLK_APBBMASK) USB APB Clock Enable */
#define MCLK_APBBMASK_USB           (_U_(0x1) << MCLK_APBBMASK_USB_Pos)
#define MCLK_APBBMASK_DSU_Pos       1            /**< \brief (MCLK_APBBMASK) DSU APB Clock Enable */
#define MCLK_APBBMASK_DSU           (_U_(0x1) << MCLK_APBBMASK_DSU_Pos)
#define MCLK_APBBMASK_NVMCTRL_Pos   2            /**< \brief (MCLK_APBBMASK) NVMCTRL APB Clock Enable */
#define MCLK_APBBMASK_NVMCTRL       (_U_(0x1) << MCLK_APBBMASK_NVMCTRL_Pos)
#define MCLK_APBBMASK_PORT_Pos      4            /**< \brief (MCLK_APBBMASK) PORT APB Clock Enable */
#define MCLK_APBBMASK_PORT          (_U_(0x1) << MCLK_APBBMASK_PORT_Pos)
#define MCLK_APBBMASK_HMATRIX_Pos   6            /**< \brief (MCLK_APBBMASK) HMATRIX APB Clock Enable */
#define MCLK_APBBMASK_HMATRIX       (_U_(0x1) << MCLK_APBBMASK_HMATRIX_Pos)
#define MCLK_APBBMASK_EVSYS_Pos     7            /**< \brief (MCLK_APBBMASK) EVSYS APB Clock Enable */
#define MCLK_APBBMASK_EVSYS         (_U_(0x1) << MCLK_APBBMASK_EVSYS_Pos)
#define MCLK_APBBMASK_SERCOM2_Pos   9            /**< \brief (MCLK_APBBMASK) SERCOM2 APB Clock Enable */
#define MCLK_APBBMASK_SERCOM2       (_U_(0x1) << MCLK_APBBMASK_SERCOM2_Pos)
#define MCLK_APBBMASK_SERCOM3_Pos   10           /**< \brief (MCLK_APBBMASK) SERCOM3 APB Clock Enable */
#define MCLK_APBBMASK_SERCOM3       (_U_(0x1) << MCLK_APBBMASK_SERCOM3_Pos)
#define MCLK_APBBMASK_TCC0_Pos      11           /**< \brief (MCLK_APBBMASK) TCC0 APB Clock Enable */
#define MCLK_APBBMASK_TCC0          (_U_(0x1) << MCLK_APBBMASK_TCC0_Pos)
#define MCLK_APBBMASK_TCC1_Pos      12           /**< \brief (MCLK_APBBMASK) TCC1 APB Clock Enable */
#define MCLK_APBBMASK_TCC1          (_U_(0x1) << MCLK_APBBMASK_TCC1_Pos)
#define MCLK_APBBMASK_TC2_Pos       13           /**< \brief (MCLK_APBBMASK) TC2 APB Clock Enable */
#define MCLK_APBBMASK_TC2           (_U_(0x1) << MCLK_APBBMASK_TC2_Pos)
#define MCLK_APBBMASK_TC3_Pos       14           /**< \brief (MCLK_APBBMASK) TC3 APB Clock Enable */
#define MCLK_APBBMASK_TC3           (_U_(0x1) << MCLK_APBBMASK_TC3_Pos)
#define MCLK_APBBMASK_TAL_Pos       15           /**< \brief (MCLK_APBBMASK) TAL APB Clock Enable */
#define MCLK_APBBMASK_TAL           (_U_(0x1) << MCLK_APBBMASK_TAL_Pos)
#define MCLK_APBBMASK_RAMECC_Pos    16           /**< \brief (MCLK_APBBMASK) RAMECC APB Clock Enable */
#define MCLK_APBBMASK_RAMECC        (_U_(0x1) << MCLK_APBBMASK_RAMECC_Pos)
#define MCLK_APBBMASK_MASK          _U_(0x0001FED7) /**< \brief (MCLK_APBBMASK) MASK Register */

/* -------- MCLK_APBCMASK : (MCLK Offset: 0x1C) (R/W 32) APBC Mask -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :3;               /*!< bit:  0.. 2  Reserved                           */
    uint32_t TCC2_:1;          /*!< bit:      3  TCC2 APB Clock Enable              */
    uint32_t TCC3_:1;          /*!< bit:      4  TCC3 APB Clock Enable              */
    uint32_t TC4_:1;           /*!< bit:      5  TC4 APB Clock Enable               */
    uint32_t TC5_:1;           /*!< bit:      6  TC5 APB Clock Enable               */
    uint32_t PDEC_:1;          /*!< bit:      7  PDEC APB Clock Enable              */
    uint32_t AC_:1;            /*!< bit:      8  AC APB Clock Enable                */
    uint32_t AES_:1;           /*!< bit:      9  AES APB Clock Enable               */
    uint32_t TRNG_:1;          /*!< bit:     10  TRNG APB Clock Enable              */
    uint32_t ICM_:1;           /*!< bit:     11  ICM APB Clock Enable               */
    uint32_t :1;               /*!< bit:     12  Reserved                           */
    uint32_t QSPI_:1;          /*!< bit:     13  QSPI APB Clock Enable              */
    uint32_t CCL_:1;           /*!< bit:     14  CCL APB Clock Enable               */
    uint32_t :17;              /*!< bit: 15..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} MCLK_APBCMASK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MCLK_APBCMASK_OFFSET        0x1C         /**< \brief (MCLK_APBCMASK offset) APBC Mask */
#define MCLK_APBCMASK_RESETVALUE    _U_(0x00002000) /**< \brief (MCLK_APBCMASK reset_value) APBC Mask */

#define MCLK_APBCMASK_TCC2_Pos      3            /**< \brief (MCLK_APBCMASK) TCC2 APB Clock Enable */
#define MCLK_APBCMASK_TCC2          (_U_(0x1) << MCLK_APBCMASK_TCC2_Pos)
#define MCLK_APBCMASK_TCC3_Pos      4            /**< \brief (MCLK_APBCMASK) TCC3 APB Clock Enable */
#define MCLK_APBCMASK_TCC3          (_U_(0x1) << MCLK_APBCMASK_TCC3_Pos)
#define MCLK_APBCMASK_TC4_Pos       5            /**< \brief (MCLK_APBCMASK) TC4 APB Clock Enable */
#define MCLK_APBCMASK_TC4           (_U_(0x1) << MCLK_APBCMASK_TC4_Pos)
#define MCLK_APBCMASK_TC5_Pos       6            /**< \brief (MCLK_APBCMASK) TC5 APB Clock Enable */
#define MCLK_APBCMASK_TC5           (_U_(0x1) << MCLK_APBCMASK_TC5_Pos)
#define MCLK_APBCMASK_PDEC_Pos      7            /**< \brief (MCLK_APBCMASK) PDEC APB Clock Enable */
#define MCLK_APBCMASK_PDEC          (_U_(0x1) << MCLK_APBCMASK_PDEC_Pos)
#define MCLK_APBCMASK_AC_Pos        8            /**< \brief (MCLK_APBCMASK) AC APB Clock Enable */
#define MCLK_APBCMASK_AC            (_U_(0x1) << MCLK_APBCMASK_AC_Pos)
#define MCLK_APBCMASK_AES_Pos       9            /**< \brief (MCLK_APBCMASK) AES APB Clock Enable */
#define MCLK_APBCMASK_AES           (_U_(0x1) << MCLK_APBCMASK_AES_Pos)
#define MCLK_APBCMASK_TRNG_Pos      10           /**< \brief (MCLK_APBCMASK) TRNG APB Clock Enable */
#define MCLK_APBCMASK_TRNG          (_U_(0x1) << MCLK_APBCMASK_TRNG_Pos)
#define MCLK_APBCMASK_ICM_Pos       11           /**< \brief (MCLK_APBCMASK) ICM APB Clock Enable */
#define MCLK_APBCMASK_ICM           (_U_(0x1) << MCLK_APBCMASK_ICM_Pos)
#define MCLK_APBCMASK_QSPI_Pos      13           /**< \brief (MCLK_APBCMASK) QSPI APB Clock Enable */
#define MCLK_APBCMASK_QSPI          (_U_(0x1) << MCLK_APBCMASK_QSPI_Pos)
#define MCLK_APBCMASK_CCL_Pos       14           /**< \brief (MCLK_APBCMASK) CCL APB Clock Enable */
#define MCLK_APBCMASK_CCL           (_U_(0x1) << MCLK_APBCMASK_CCL_Pos)
#define MCLK_APBCMASK_MASK          _U_(0x00006FF8) /**< \brief (MCLK_APBCMASK) MASK Register */

/* -------- MCLK_APBDMASK : (MCLK Offset: 0x20) (R/W 32) APBD Mask -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SERCOM4_:1;       /*!< bit:      0  SERCOM4 APB Clock Enable           */
    uint32_t SERCOM5_:1;       /*!< bit:      1  SERCOM5 APB Clock Enable           */
    uint32_t SERCOM6_:1;       /*!< bit:      2  SERCOM6 APB Clock Enable           */
    uint32_t SERCOM7_:1;       /*!< bit:      3  SERCOM7 APB Clock Enable           */
    uint32_t TCC4_:1;          /*!< bit:      4  TCC4 APB Clock Enable              */
    uint32_t TC6_:1;           /*!< bit:      5  TC6 APB Clock Enable               */
    uint32_t TC7_:1;           /*!< bit:      6  TC7 APB Clock Enable               */
    uint32_t ADC0_:1;          /*!< bit:      7  ADC0 APB Clock Enable              */
    uint32_t ADC1_:1;          /*!< bit:      8  ADC1 APB Clock Enable              */
    uint32_t DAC_:1;           /*!< bit:      9  DAC APB Clock Enable               */
    uint32_t I2S_:1;           /*!< bit:     10  I2S APB Clock Enable               */
    uint32_t PCC_:1;           /*!< bit:     11  PCC APB Clock Enable               */
    uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} MCLK_APBDMASK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MCLK_APBDMASK_OFFSET        0x20         /**< \brief (MCLK_APBDMASK offset) APBD Mask */
#define MCLK_APBDMASK_RESETVALUE    _U_(0x00000000) /**< \brief (MCLK_APBDMASK reset_value) APBD Mask */

#define MCLK_APBDMASK_SERCOM4_Pos   0            /**< \brief (MCLK_APBDMASK) SERCOM4 APB Clock Enable */
#define MCLK_APBDMASK_SERCOM4       (_U_(0x1) << MCLK_APBDMASK_SERCOM4_Pos)
#define MCLK_APBDMASK_SERCOM5_Pos   1            /**< \brief (MCLK_APBDMASK) SERCOM5 APB Clock Enable */
#define MCLK_APBDMASK_SERCOM5       (_U_(0x1) << MCLK_APBDMASK_SERCOM5_Pos)
#define MCLK_APBDMASK_SERCOM6_Pos   2            /**< \brief (MCLK_APBDMASK) SERCOM6 APB Clock Enable */
#define MCLK_APBDMASK_SERCOM6       (_U_(0x1) << MCLK_APBDMASK_SERCOM6_Pos)
#define MCLK_APBDMASK_SERCOM7_Pos   3            /**< \brief (MCLK_APBDMASK) SERCOM7 APB Clock Enable */
#define MCLK_APBDMASK_SERCOM7       (_U_(0x1) << MCLK_APBDMASK_SERCOM7_Pos)
#define MCLK_APBDMASK_TCC4_Pos      4            /**< \brief (MCLK_APBDMASK) TCC4 APB Clock Enable */
#define MCLK_APBDMASK_TCC4          (_U_(0x1) << MCLK_APBDMASK_TCC4_Pos)
#define MCLK_APBDMASK_TC6_Pos       5            /**< \brief (MCLK_APBDMASK) TC6 APB Clock Enable */
#define MCLK_APBDMASK_TC6           (_U_(0x1) << MCLK_APBDMASK_TC6_Pos)
#define MCLK_APBDMASK_TC7_Pos       6            /**< \brief (MCLK_APBDMASK) TC7 APB Clock Enable */
#define MCLK_APBDMASK_TC7           (_U_(0x1) << MCLK_APBDMASK_TC7_Pos)
#define MCLK_APBDMASK_ADC0_Pos      7            /**< \brief (MCLK_APBDMASK) ADC0 APB Clock Enable */
#define MCLK_APBDMASK_ADC0          (_U_(0x1) << MCLK_APBDMASK_ADC0_Pos)
#define MCLK_APBDMASK_ADC1_Pos      8            /**< \brief (MCLK_APBDMASK) ADC1 APB Clock Enable */
#define MCLK_APBDMASK_ADC1          (_U_(0x1) << MCLK_APBDMASK_ADC1_Pos)
#define MCLK_APBDMASK_DAC_Pos       9            /**< \brief (MCLK_APBDMASK) DAC APB Clock Enable */
#define MCLK_APBDMASK_DAC           (_U_(0x1) << MCLK_APBDMASK_DAC_Pos)
#define MCLK_APBDMASK_I2S_Pos       10           /**< \brief (MCLK_APBDMASK) I2S APB Clock Enable */
#define MCLK_APBDMASK_I2S           (_U_(0x1) << MCLK_APBDMASK_I2S_Pos)
#define MCLK_APBDMASK_PCC_Pos       11           /**< \brief (MCLK_APBDMASK) PCC APB Clock Enable */
#define MCLK_APBDMASK_PCC           (_U_(0x1) << MCLK_APBDMASK_PCC_Pos)
#define MCLK_APBDMASK_MASK          _U_(0x00000FFF) /**< \brief (MCLK_APBDMASK) MASK Register */

/** \brief MCLK hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
       RoReg8                    Reserved1[0x1];
  __IO MCLK_INTENCLR_Type        INTENCLR;    /**< \brief Offset: 0x01 (R/W  8) Interrupt Enable Clear */
  __IO MCLK_INTENSET_Type        INTENSET;    /**< \brief Offset: 0x02 (R/W  8) Interrupt Enable Set */
  __IO MCLK_INTFLAG_Type         INTFLAG;     /**< \brief Offset: 0x03 (R/W  8) Interrupt Flag Status and Clear */
  __I  MCLK_HSDIV_Type           HSDIV;       /**< \brief Offset: 0x04 (R/   8) HS Clock Division */
  __IO MCLK_CPUDIV_Type          CPUDIV;      /**< \brief Offset: 0x05 (R/W  8) CPU Clock Division */
       RoReg8                    Reserved2[0xA];
  __IO MCLK_AHBMASK_Type         AHBMASK;     /**< \brief Offset: 0x10 (R/W 32) AHB Mask */
  __IO MCLK_APBAMASK_Type        APBAMASK;    /**< \brief Offset: 0x14 (R/W 32) APBA Mask */
  __IO MCLK_APBBMASK_Type        APBBMASK;    /**< \brief Offset: 0x18 (R/W 32) APBB Mask */
  __IO MCLK_APBCMASK_Type        APBCMASK;    /**< \brief Offset: 0x1C (R/W 32) APBC Mask */
  __IO MCLK_APBDMASK_Type        APBDMASK;    /**< \brief Offset: 0x20 (R/W 32) APBD Mask */
} Mclk;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_MCLK_COMPONENT_ */
