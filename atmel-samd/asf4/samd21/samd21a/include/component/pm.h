/**
 * \file
 *
 * \brief Component description for PM
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

#ifndef _SAMD21_PM_COMPONENT_
#define _SAMD21_PM_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR PM */
/* ========================================================================== */
/** \addtogroup SAMD21_PM Power Manager */
/*@{*/

#define PM_U2206
#define REV_PM                      0x201

/* -------- PM_CTRL : (PM Offset: 0x00) (R/W  8) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_CTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_CTRL_OFFSET              0x00         /**< \brief (PM_CTRL offset) Control */
#define PM_CTRL_RESETVALUE          _U(0x00)     /**< \brief (PM_CTRL reset_value) Control */

#define PM_CTRL_MASK                _U(0x00)     /**< \brief (PM_CTRL) MASK Register */

/* -------- PM_SLEEP : (PM Offset: 0x01) (R/W  8) Sleep Mode -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  IDLE:2;           /*!< bit:  0.. 1  Idle Mode Configuration            */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_SLEEP_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_SLEEP_OFFSET             0x01         /**< \brief (PM_SLEEP offset) Sleep Mode */
#define PM_SLEEP_RESETVALUE         _U(0x00)     /**< \brief (PM_SLEEP reset_value) Sleep Mode */

#define PM_SLEEP_IDLE_Pos           0            /**< \brief (PM_SLEEP) Idle Mode Configuration */
#define PM_SLEEP_IDLE_Msk           (_U(0x3) << PM_SLEEP_IDLE_Pos)
#define PM_SLEEP_IDLE(value)        (PM_SLEEP_IDLE_Msk & ((value) << PM_SLEEP_IDLE_Pos))
#define   PM_SLEEP_IDLE_CPU_Val           _U(0x0)   /**< \brief (PM_SLEEP) The CPU clock domain is stopped */
#define   PM_SLEEP_IDLE_AHB_Val           _U(0x1)   /**< \brief (PM_SLEEP) The CPU and AHB clock domains are stopped */
#define   PM_SLEEP_IDLE_APB_Val           _U(0x2)   /**< \brief (PM_SLEEP) The CPU, AHB and APB clock domains are stopped */
#define PM_SLEEP_IDLE_CPU           (PM_SLEEP_IDLE_CPU_Val         << PM_SLEEP_IDLE_Pos)
#define PM_SLEEP_IDLE_AHB           (PM_SLEEP_IDLE_AHB_Val         << PM_SLEEP_IDLE_Pos)
#define PM_SLEEP_IDLE_APB           (PM_SLEEP_IDLE_APB_Val         << PM_SLEEP_IDLE_Pos)
#define PM_SLEEP_MASK               _U(0x03)     /**< \brief (PM_SLEEP) MASK Register */

/* -------- PM_CPUSEL : (PM Offset: 0x08) (R/W  8) CPU Clock Select -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CPUDIV:3;         /*!< bit:  0.. 2  CPU Prescaler Selection            */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_CPUSEL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_CPUSEL_OFFSET            0x08         /**< \brief (PM_CPUSEL offset) CPU Clock Select */
#define PM_CPUSEL_RESETVALUE        _U(0x00)     /**< \brief (PM_CPUSEL reset_value) CPU Clock Select */

#define PM_CPUSEL_CPUDIV_Pos        0            /**< \brief (PM_CPUSEL) CPU Prescaler Selection */
#define PM_CPUSEL_CPUDIV_Msk        (_U(0x7) << PM_CPUSEL_CPUDIV_Pos)
#define PM_CPUSEL_CPUDIV(value)     (PM_CPUSEL_CPUDIV_Msk & ((value) << PM_CPUSEL_CPUDIV_Pos))
#define   PM_CPUSEL_CPUDIV_DIV1_Val       _U(0x0)   /**< \brief (PM_CPUSEL) Divide by 1 */
#define   PM_CPUSEL_CPUDIV_DIV2_Val       _U(0x1)   /**< \brief (PM_CPUSEL) Divide by 2 */
#define   PM_CPUSEL_CPUDIV_DIV4_Val       _U(0x2)   /**< \brief (PM_CPUSEL) Divide by 4 */
#define   PM_CPUSEL_CPUDIV_DIV8_Val       _U(0x3)   /**< \brief (PM_CPUSEL) Divide by 8 */
#define   PM_CPUSEL_CPUDIV_DIV16_Val      _U(0x4)   /**< \brief (PM_CPUSEL) Divide by 16 */
#define   PM_CPUSEL_CPUDIV_DIV32_Val      _U(0x5)   /**< \brief (PM_CPUSEL) Divide by 32 */
#define   PM_CPUSEL_CPUDIV_DIV64_Val      _U(0x6)   /**< \brief (PM_CPUSEL) Divide by 64 */
#define   PM_CPUSEL_CPUDIV_DIV128_Val     _U(0x7)   /**< \brief (PM_CPUSEL) Divide by 128 */
#define PM_CPUSEL_CPUDIV_DIV1       (PM_CPUSEL_CPUDIV_DIV1_Val     << PM_CPUSEL_CPUDIV_Pos)
#define PM_CPUSEL_CPUDIV_DIV2       (PM_CPUSEL_CPUDIV_DIV2_Val     << PM_CPUSEL_CPUDIV_Pos)
#define PM_CPUSEL_CPUDIV_DIV4       (PM_CPUSEL_CPUDIV_DIV4_Val     << PM_CPUSEL_CPUDIV_Pos)
#define PM_CPUSEL_CPUDIV_DIV8       (PM_CPUSEL_CPUDIV_DIV8_Val     << PM_CPUSEL_CPUDIV_Pos)
#define PM_CPUSEL_CPUDIV_DIV16      (PM_CPUSEL_CPUDIV_DIV16_Val    << PM_CPUSEL_CPUDIV_Pos)
#define PM_CPUSEL_CPUDIV_DIV32      (PM_CPUSEL_CPUDIV_DIV32_Val    << PM_CPUSEL_CPUDIV_Pos)
#define PM_CPUSEL_CPUDIV_DIV64      (PM_CPUSEL_CPUDIV_DIV64_Val    << PM_CPUSEL_CPUDIV_Pos)
#define PM_CPUSEL_CPUDIV_DIV128     (PM_CPUSEL_CPUDIV_DIV128_Val   << PM_CPUSEL_CPUDIV_Pos)
#define PM_CPUSEL_MASK              _U(0x07)     /**< \brief (PM_CPUSEL) MASK Register */

/* -------- PM_APBASEL : (PM Offset: 0x09) (R/W  8) APBA Clock Select -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  APBADIV:3;        /*!< bit:  0.. 2  APBA Prescaler Selection           */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_APBASEL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_APBASEL_OFFSET           0x09         /**< \brief (PM_APBASEL offset) APBA Clock Select */
#define PM_APBASEL_RESETVALUE       _U(0x00)     /**< \brief (PM_APBASEL reset_value) APBA Clock Select */

#define PM_APBASEL_APBADIV_Pos      0            /**< \brief (PM_APBASEL) APBA Prescaler Selection */
#define PM_APBASEL_APBADIV_Msk      (_U(0x7) << PM_APBASEL_APBADIV_Pos)
#define PM_APBASEL_APBADIV(value)   (PM_APBASEL_APBADIV_Msk & ((value) << PM_APBASEL_APBADIV_Pos))
#define   PM_APBASEL_APBADIV_DIV1_Val     _U(0x0)   /**< \brief (PM_APBASEL) Divide by 1 */
#define   PM_APBASEL_APBADIV_DIV2_Val     _U(0x1)   /**< \brief (PM_APBASEL) Divide by 2 */
#define   PM_APBASEL_APBADIV_DIV4_Val     _U(0x2)   /**< \brief (PM_APBASEL) Divide by 4 */
#define   PM_APBASEL_APBADIV_DIV8_Val     _U(0x3)   /**< \brief (PM_APBASEL) Divide by 8 */
#define   PM_APBASEL_APBADIV_DIV16_Val    _U(0x4)   /**< \brief (PM_APBASEL) Divide by 16 */
#define   PM_APBASEL_APBADIV_DIV32_Val    _U(0x5)   /**< \brief (PM_APBASEL) Divide by 32 */
#define   PM_APBASEL_APBADIV_DIV64_Val    _U(0x6)   /**< \brief (PM_APBASEL) Divide by 64 */
#define   PM_APBASEL_APBADIV_DIV128_Val   _U(0x7)   /**< \brief (PM_APBASEL) Divide by 128 */
#define PM_APBASEL_APBADIV_DIV1     (PM_APBASEL_APBADIV_DIV1_Val   << PM_APBASEL_APBADIV_Pos)
#define PM_APBASEL_APBADIV_DIV2     (PM_APBASEL_APBADIV_DIV2_Val   << PM_APBASEL_APBADIV_Pos)
#define PM_APBASEL_APBADIV_DIV4     (PM_APBASEL_APBADIV_DIV4_Val   << PM_APBASEL_APBADIV_Pos)
#define PM_APBASEL_APBADIV_DIV8     (PM_APBASEL_APBADIV_DIV8_Val   << PM_APBASEL_APBADIV_Pos)
#define PM_APBASEL_APBADIV_DIV16    (PM_APBASEL_APBADIV_DIV16_Val  << PM_APBASEL_APBADIV_Pos)
#define PM_APBASEL_APBADIV_DIV32    (PM_APBASEL_APBADIV_DIV32_Val  << PM_APBASEL_APBADIV_Pos)
#define PM_APBASEL_APBADIV_DIV64    (PM_APBASEL_APBADIV_DIV64_Val  << PM_APBASEL_APBADIV_Pos)
#define PM_APBASEL_APBADIV_DIV128   (PM_APBASEL_APBADIV_DIV128_Val << PM_APBASEL_APBADIV_Pos)
#define PM_APBASEL_MASK             _U(0x07)     /**< \brief (PM_APBASEL) MASK Register */

/* -------- PM_APBBSEL : (PM Offset: 0x0A) (R/W  8) APBB Clock Select -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  APBBDIV:3;        /*!< bit:  0.. 2  APBB Prescaler Selection           */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_APBBSEL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_APBBSEL_OFFSET           0x0A         /**< \brief (PM_APBBSEL offset) APBB Clock Select */
#define PM_APBBSEL_RESETVALUE       _U(0x00)     /**< \brief (PM_APBBSEL reset_value) APBB Clock Select */

#define PM_APBBSEL_APBBDIV_Pos      0            /**< \brief (PM_APBBSEL) APBB Prescaler Selection */
#define PM_APBBSEL_APBBDIV_Msk      (_U(0x7) << PM_APBBSEL_APBBDIV_Pos)
#define PM_APBBSEL_APBBDIV(value)   (PM_APBBSEL_APBBDIV_Msk & ((value) << PM_APBBSEL_APBBDIV_Pos))
#define   PM_APBBSEL_APBBDIV_DIV1_Val     _U(0x0)   /**< \brief (PM_APBBSEL) Divide by 1 */
#define   PM_APBBSEL_APBBDIV_DIV2_Val     _U(0x1)   /**< \brief (PM_APBBSEL) Divide by 2 */
#define   PM_APBBSEL_APBBDIV_DIV4_Val     _U(0x2)   /**< \brief (PM_APBBSEL) Divide by 4 */
#define   PM_APBBSEL_APBBDIV_DIV8_Val     _U(0x3)   /**< \brief (PM_APBBSEL) Divide by 8 */
#define   PM_APBBSEL_APBBDIV_DIV16_Val    _U(0x4)   /**< \brief (PM_APBBSEL) Divide by 16 */
#define   PM_APBBSEL_APBBDIV_DIV32_Val    _U(0x5)   /**< \brief (PM_APBBSEL) Divide by 32 */
#define   PM_APBBSEL_APBBDIV_DIV64_Val    _U(0x6)   /**< \brief (PM_APBBSEL) Divide by 64 */
#define   PM_APBBSEL_APBBDIV_DIV128_Val   _U(0x7)   /**< \brief (PM_APBBSEL) Divide by 128 */
#define PM_APBBSEL_APBBDIV_DIV1     (PM_APBBSEL_APBBDIV_DIV1_Val   << PM_APBBSEL_APBBDIV_Pos)
#define PM_APBBSEL_APBBDIV_DIV2     (PM_APBBSEL_APBBDIV_DIV2_Val   << PM_APBBSEL_APBBDIV_Pos)
#define PM_APBBSEL_APBBDIV_DIV4     (PM_APBBSEL_APBBDIV_DIV4_Val   << PM_APBBSEL_APBBDIV_Pos)
#define PM_APBBSEL_APBBDIV_DIV8     (PM_APBBSEL_APBBDIV_DIV8_Val   << PM_APBBSEL_APBBDIV_Pos)
#define PM_APBBSEL_APBBDIV_DIV16    (PM_APBBSEL_APBBDIV_DIV16_Val  << PM_APBBSEL_APBBDIV_Pos)
#define PM_APBBSEL_APBBDIV_DIV32    (PM_APBBSEL_APBBDIV_DIV32_Val  << PM_APBBSEL_APBBDIV_Pos)
#define PM_APBBSEL_APBBDIV_DIV64    (PM_APBBSEL_APBBDIV_DIV64_Val  << PM_APBBSEL_APBBDIV_Pos)
#define PM_APBBSEL_APBBDIV_DIV128   (PM_APBBSEL_APBBDIV_DIV128_Val << PM_APBBSEL_APBBDIV_Pos)
#define PM_APBBSEL_MASK             _U(0x07)     /**< \brief (PM_APBBSEL) MASK Register */

/* -------- PM_APBCSEL : (PM Offset: 0x0B) (R/W  8) APBC Clock Select -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  APBCDIV:3;        /*!< bit:  0.. 2  APBC Prescaler Selection           */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_APBCSEL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_APBCSEL_OFFSET           0x0B         /**< \brief (PM_APBCSEL offset) APBC Clock Select */
#define PM_APBCSEL_RESETVALUE       _U(0x00)     /**< \brief (PM_APBCSEL reset_value) APBC Clock Select */

#define PM_APBCSEL_APBCDIV_Pos      0            /**< \brief (PM_APBCSEL) APBC Prescaler Selection */
#define PM_APBCSEL_APBCDIV_Msk      (_U(0x7) << PM_APBCSEL_APBCDIV_Pos)
#define PM_APBCSEL_APBCDIV(value)   (PM_APBCSEL_APBCDIV_Msk & ((value) << PM_APBCSEL_APBCDIV_Pos))
#define   PM_APBCSEL_APBCDIV_DIV1_Val     _U(0x0)   /**< \brief (PM_APBCSEL) Divide by 1 */
#define   PM_APBCSEL_APBCDIV_DIV2_Val     _U(0x1)   /**< \brief (PM_APBCSEL) Divide by 2 */
#define   PM_APBCSEL_APBCDIV_DIV4_Val     _U(0x2)   /**< \brief (PM_APBCSEL) Divide by 4 */
#define   PM_APBCSEL_APBCDIV_DIV8_Val     _U(0x3)   /**< \brief (PM_APBCSEL) Divide by 8 */
#define   PM_APBCSEL_APBCDIV_DIV16_Val    _U(0x4)   /**< \brief (PM_APBCSEL) Divide by 16 */
#define   PM_APBCSEL_APBCDIV_DIV32_Val    _U(0x5)   /**< \brief (PM_APBCSEL) Divide by 32 */
#define   PM_APBCSEL_APBCDIV_DIV64_Val    _U(0x6)   /**< \brief (PM_APBCSEL) Divide by 64 */
#define   PM_APBCSEL_APBCDIV_DIV128_Val   _U(0x7)   /**< \brief (PM_APBCSEL) Divide by 128 */
#define PM_APBCSEL_APBCDIV_DIV1     (PM_APBCSEL_APBCDIV_DIV1_Val   << PM_APBCSEL_APBCDIV_Pos)
#define PM_APBCSEL_APBCDIV_DIV2     (PM_APBCSEL_APBCDIV_DIV2_Val   << PM_APBCSEL_APBCDIV_Pos)
#define PM_APBCSEL_APBCDIV_DIV4     (PM_APBCSEL_APBCDIV_DIV4_Val   << PM_APBCSEL_APBCDIV_Pos)
#define PM_APBCSEL_APBCDIV_DIV8     (PM_APBCSEL_APBCDIV_DIV8_Val   << PM_APBCSEL_APBCDIV_Pos)
#define PM_APBCSEL_APBCDIV_DIV16    (PM_APBCSEL_APBCDIV_DIV16_Val  << PM_APBCSEL_APBCDIV_Pos)
#define PM_APBCSEL_APBCDIV_DIV32    (PM_APBCSEL_APBCDIV_DIV32_Val  << PM_APBCSEL_APBCDIV_Pos)
#define PM_APBCSEL_APBCDIV_DIV64    (PM_APBCSEL_APBCDIV_DIV64_Val  << PM_APBCSEL_APBCDIV_Pos)
#define PM_APBCSEL_APBCDIV_DIV128   (PM_APBCSEL_APBCDIV_DIV128_Val << PM_APBCSEL_APBCDIV_Pos)
#define PM_APBCSEL_MASK             _U(0x07)     /**< \brief (PM_APBCSEL) MASK Register */

/* -------- PM_AHBMASK : (PM Offset: 0x14) (R/W 32) AHB Mask -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t HPB0_:1;          /*!< bit:      0  HPB0 AHB Clock Mask                */
    uint32_t HPB1_:1;          /*!< bit:      1  HPB1 AHB Clock Mask                */
    uint32_t HPB2_:1;          /*!< bit:      2  HPB2 AHB Clock Mask                */
    uint32_t DSU_:1;           /*!< bit:      3  DSU AHB Clock Mask                 */
    uint32_t NVMCTRL_:1;       /*!< bit:      4  NVMCTRL AHB Clock Mask             */
    uint32_t DMAC_:1;          /*!< bit:      5  DMAC AHB Clock Mask                */
    uint32_t USB_:1;           /*!< bit:      6  USB AHB Clock Mask                 */
    uint32_t :25;              /*!< bit:  7..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PM_AHBMASK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_AHBMASK_OFFSET           0x14         /**< \brief (PM_AHBMASK offset) AHB Mask */
#define PM_AHBMASK_RESETVALUE       _U(0x0000007F) /**< \brief (PM_AHBMASK reset_value) AHB Mask */

#define PM_AHBMASK_HPB0_Pos         0            /**< \brief (PM_AHBMASK) HPB0 AHB Clock Mask */
#define PM_AHBMASK_HPB0             (_U(0x1) << PM_AHBMASK_HPB0_Pos)
#define PM_AHBMASK_HPB1_Pos         1            /**< \brief (PM_AHBMASK) HPB1 AHB Clock Mask */
#define PM_AHBMASK_HPB1             (_U(0x1) << PM_AHBMASK_HPB1_Pos)
#define PM_AHBMASK_HPB2_Pos         2            /**< \brief (PM_AHBMASK) HPB2 AHB Clock Mask */
#define PM_AHBMASK_HPB2             (_U(0x1) << PM_AHBMASK_HPB2_Pos)
#define PM_AHBMASK_DSU_Pos          3            /**< \brief (PM_AHBMASK) DSU AHB Clock Mask */
#define PM_AHBMASK_DSU              (_U(0x1) << PM_AHBMASK_DSU_Pos)
#define PM_AHBMASK_NVMCTRL_Pos      4            /**< \brief (PM_AHBMASK) NVMCTRL AHB Clock Mask */
#define PM_AHBMASK_NVMCTRL          (_U(0x1) << PM_AHBMASK_NVMCTRL_Pos)
#define PM_AHBMASK_DMAC_Pos         5            /**< \brief (PM_AHBMASK) DMAC AHB Clock Mask */
#define PM_AHBMASK_DMAC             (_U(0x1) << PM_AHBMASK_DMAC_Pos)
#define PM_AHBMASK_USB_Pos          6            /**< \brief (PM_AHBMASK) USB AHB Clock Mask */
#define PM_AHBMASK_USB              (_U(0x1) << PM_AHBMASK_USB_Pos)
#define PM_AHBMASK_MASK             _U(0x0000007F) /**< \brief (PM_AHBMASK) MASK Register */

/* -------- PM_APBAMASK : (PM Offset: 0x18) (R/W 32) APBA Mask -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PAC0_:1;          /*!< bit:      0  PAC0 APB Clock Enable              */
    uint32_t PM_:1;            /*!< bit:      1  PM APB Clock Enable                */
    uint32_t SYSCTRL_:1;       /*!< bit:      2  SYSCTRL APB Clock Enable           */
    uint32_t GCLK_:1;          /*!< bit:      3  GCLK APB Clock Enable              */
    uint32_t WDT_:1;           /*!< bit:      4  WDT APB Clock Enable               */
    uint32_t RTC_:1;           /*!< bit:      5  RTC APB Clock Enable               */
    uint32_t EIC_:1;           /*!< bit:      6  EIC APB Clock Enable               */
    uint32_t :25;              /*!< bit:  7..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PM_APBAMASK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_APBAMASK_OFFSET          0x18         /**< \brief (PM_APBAMASK offset) APBA Mask */
#define PM_APBAMASK_RESETVALUE      _U(0x0000007F) /**< \brief (PM_APBAMASK reset_value) APBA Mask */

#define PM_APBAMASK_PAC0_Pos        0            /**< \brief (PM_APBAMASK) PAC0 APB Clock Enable */
#define PM_APBAMASK_PAC0            (_U(0x1) << PM_APBAMASK_PAC0_Pos)
#define PM_APBAMASK_PM_Pos          1            /**< \brief (PM_APBAMASK) PM APB Clock Enable */
#define PM_APBAMASK_PM              (_U(0x1) << PM_APBAMASK_PM_Pos)
#define PM_APBAMASK_SYSCTRL_Pos     2            /**< \brief (PM_APBAMASK) SYSCTRL APB Clock Enable */
#define PM_APBAMASK_SYSCTRL         (_U(0x1) << PM_APBAMASK_SYSCTRL_Pos)
#define PM_APBAMASK_GCLK_Pos        3            /**< \brief (PM_APBAMASK) GCLK APB Clock Enable */
#define PM_APBAMASK_GCLK            (_U(0x1) << PM_APBAMASK_GCLK_Pos)
#define PM_APBAMASK_WDT_Pos         4            /**< \brief (PM_APBAMASK) WDT APB Clock Enable */
#define PM_APBAMASK_WDT             (_U(0x1) << PM_APBAMASK_WDT_Pos)
#define PM_APBAMASK_RTC_Pos         5            /**< \brief (PM_APBAMASK) RTC APB Clock Enable */
#define PM_APBAMASK_RTC             (_U(0x1) << PM_APBAMASK_RTC_Pos)
#define PM_APBAMASK_EIC_Pos         6            /**< \brief (PM_APBAMASK) EIC APB Clock Enable */
#define PM_APBAMASK_EIC             (_U(0x1) << PM_APBAMASK_EIC_Pos)
#define PM_APBAMASK_MASK            _U(0x0000007F) /**< \brief (PM_APBAMASK) MASK Register */

/* -------- PM_APBBMASK : (PM Offset: 0x1C) (R/W 32) APBB Mask -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PAC1_:1;          /*!< bit:      0  PAC1 APB Clock Enable              */
    uint32_t DSU_:1;           /*!< bit:      1  DSU APB Clock Enable               */
    uint32_t NVMCTRL_:1;       /*!< bit:      2  NVMCTRL APB Clock Enable           */
    uint32_t PORT_:1;          /*!< bit:      3  PORT APB Clock Enable              */
    uint32_t DMAC_:1;          /*!< bit:      4  DMAC APB Clock Enable              */
    uint32_t USB_:1;           /*!< bit:      5  USB APB Clock Enable               */
    uint32_t HMATRIX_:1;       /*!< bit:      6  HMATRIX APB Clock Enable           */
    uint32_t :25;              /*!< bit:  7..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PM_APBBMASK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_APBBMASK_OFFSET          0x1C         /**< \brief (PM_APBBMASK offset) APBB Mask */
#define PM_APBBMASK_RESETVALUE      _U(0x0000007F) /**< \brief (PM_APBBMASK reset_value) APBB Mask */

#define PM_APBBMASK_PAC1_Pos        0            /**< \brief (PM_APBBMASK) PAC1 APB Clock Enable */
#define PM_APBBMASK_PAC1            (_U(0x1) << PM_APBBMASK_PAC1_Pos)
#define PM_APBBMASK_DSU_Pos         1            /**< \brief (PM_APBBMASK) DSU APB Clock Enable */
#define PM_APBBMASK_DSU             (_U(0x1) << PM_APBBMASK_DSU_Pos)
#define PM_APBBMASK_NVMCTRL_Pos     2            /**< \brief (PM_APBBMASK) NVMCTRL APB Clock Enable */
#define PM_APBBMASK_NVMCTRL         (_U(0x1) << PM_APBBMASK_NVMCTRL_Pos)
#define PM_APBBMASK_PORT_Pos        3            /**< \brief (PM_APBBMASK) PORT APB Clock Enable */
#define PM_APBBMASK_PORT            (_U(0x1) << PM_APBBMASK_PORT_Pos)
#define PM_APBBMASK_DMAC_Pos        4            /**< \brief (PM_APBBMASK) DMAC APB Clock Enable */
#define PM_APBBMASK_DMAC            (_U(0x1) << PM_APBBMASK_DMAC_Pos)
#define PM_APBBMASK_USB_Pos         5            /**< \brief (PM_APBBMASK) USB APB Clock Enable */
#define PM_APBBMASK_USB             (_U(0x1) << PM_APBBMASK_USB_Pos)
#define PM_APBBMASK_HMATRIX_Pos     6            /**< \brief (PM_APBBMASK) HMATRIX APB Clock Enable */
#define PM_APBBMASK_HMATRIX         (_U(0x1) << PM_APBBMASK_HMATRIX_Pos)
#define PM_APBBMASK_MASK            _U(0x0000007F) /**< \brief (PM_APBBMASK) MASK Register */

/* -------- PM_APBCMASK : (PM Offset: 0x20) (R/W 32) APBC Mask -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PAC2_:1;          /*!< bit:      0  PAC2 APB Clock Enable              */
    uint32_t EVSYS_:1;         /*!< bit:      1  EVSYS APB Clock Enable             */
    uint32_t SERCOM0_:1;       /*!< bit:      2  SERCOM0 APB Clock Enable           */
    uint32_t SERCOM1_:1;       /*!< bit:      3  SERCOM1 APB Clock Enable           */
    uint32_t SERCOM2_:1;       /*!< bit:      4  SERCOM2 APB Clock Enable           */
    uint32_t SERCOM3_:1;       /*!< bit:      5  SERCOM3 APB Clock Enable           */
    uint32_t SERCOM4_:1;       /*!< bit:      6  SERCOM4 APB Clock Enable           */
    uint32_t SERCOM5_:1;       /*!< bit:      7  SERCOM5 APB Clock Enable           */
    uint32_t TCC0_:1;          /*!< bit:      8  TCC0 APB Clock Enable              */
    uint32_t TCC1_:1;          /*!< bit:      9  TCC1 APB Clock Enable              */
    uint32_t TCC2_:1;          /*!< bit:     10  TCC2 APB Clock Enable              */
    uint32_t TC3_:1;           /*!< bit:     11  TC3 APB Clock Enable               */
    uint32_t TC4_:1;           /*!< bit:     12  TC4 APB Clock Enable               */
    uint32_t TC5_:1;           /*!< bit:     13  TC5 APB Clock Enable               */
    uint32_t TC6_:1;           /*!< bit:     14  TC6 APB Clock Enable               */
    uint32_t TC7_:1;           /*!< bit:     15  TC7 APB Clock Enable               */
    uint32_t ADC_:1;           /*!< bit:     16  ADC APB Clock Enable               */
    uint32_t AC_:1;            /*!< bit:     17  AC APB Clock Enable                */
    uint32_t DAC_:1;           /*!< bit:     18  DAC APB Clock Enable               */
    uint32_t PTC_:1;           /*!< bit:     19  PTC APB Clock Enable               */
    uint32_t I2S_:1;           /*!< bit:     20  I2S APB Clock Enable               */
    uint32_t :11;              /*!< bit: 21..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PM_APBCMASK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_APBCMASK_OFFSET          0x20         /**< \brief (PM_APBCMASK offset) APBC Mask */
#define PM_APBCMASK_RESETVALUE      _U(0x00010000) /**< \brief (PM_APBCMASK reset_value) APBC Mask */

#define PM_APBCMASK_PAC2_Pos        0            /**< \brief (PM_APBCMASK) PAC2 APB Clock Enable */
#define PM_APBCMASK_PAC2            (_U(0x1) << PM_APBCMASK_PAC2_Pos)
#define PM_APBCMASK_EVSYS_Pos       1            /**< \brief (PM_APBCMASK) EVSYS APB Clock Enable */
#define PM_APBCMASK_EVSYS           (_U(0x1) << PM_APBCMASK_EVSYS_Pos)
#define PM_APBCMASK_SERCOM0_Pos     2            /**< \brief (PM_APBCMASK) SERCOM0 APB Clock Enable */
#define PM_APBCMASK_SERCOM0         (_U(0x1) << PM_APBCMASK_SERCOM0_Pos)
#define PM_APBCMASK_SERCOM1_Pos     3            /**< \brief (PM_APBCMASK) SERCOM1 APB Clock Enable */
#define PM_APBCMASK_SERCOM1         (_U(0x1) << PM_APBCMASK_SERCOM1_Pos)
#define PM_APBCMASK_SERCOM2_Pos     4            /**< \brief (PM_APBCMASK) SERCOM2 APB Clock Enable */
#define PM_APBCMASK_SERCOM2         (_U(0x1) << PM_APBCMASK_SERCOM2_Pos)
#define PM_APBCMASK_SERCOM3_Pos     5            /**< \brief (PM_APBCMASK) SERCOM3 APB Clock Enable */
#define PM_APBCMASK_SERCOM3         (_U(0x1) << PM_APBCMASK_SERCOM3_Pos)
#define PM_APBCMASK_SERCOM4_Pos     6            /**< \brief (PM_APBCMASK) SERCOM4 APB Clock Enable */
#define PM_APBCMASK_SERCOM4         (_U(0x1) << PM_APBCMASK_SERCOM4_Pos)
#define PM_APBCMASK_SERCOM5_Pos     7            /**< \brief (PM_APBCMASK) SERCOM5 APB Clock Enable */
#define PM_APBCMASK_SERCOM5         (_U(0x1) << PM_APBCMASK_SERCOM5_Pos)
#define PM_APBCMASK_TCC0_Pos        8            /**< \brief (PM_APBCMASK) TCC0 APB Clock Enable */
#define PM_APBCMASK_TCC0            (_U(0x1) << PM_APBCMASK_TCC0_Pos)
#define PM_APBCMASK_TCC1_Pos        9            /**< \brief (PM_APBCMASK) TCC1 APB Clock Enable */
#define PM_APBCMASK_TCC1            (_U(0x1) << PM_APBCMASK_TCC1_Pos)
#define PM_APBCMASK_TCC2_Pos        10           /**< \brief (PM_APBCMASK) TCC2 APB Clock Enable */
#define PM_APBCMASK_TCC2            (_U(0x1) << PM_APBCMASK_TCC2_Pos)
#define PM_APBCMASK_TC3_Pos         11           /**< \brief (PM_APBCMASK) TC3 APB Clock Enable */
#define PM_APBCMASK_TC3             (_U(0x1) << PM_APBCMASK_TC3_Pos)
#define PM_APBCMASK_TC4_Pos         12           /**< \brief (PM_APBCMASK) TC4 APB Clock Enable */
#define PM_APBCMASK_TC4             (_U(0x1) << PM_APBCMASK_TC4_Pos)
#define PM_APBCMASK_TC5_Pos         13           /**< \brief (PM_APBCMASK) TC5 APB Clock Enable */
#define PM_APBCMASK_TC5             (_U(0x1) << PM_APBCMASK_TC5_Pos)
#define PM_APBCMASK_TC6_Pos         14           /**< \brief (PM_APBCMASK) TC6 APB Clock Enable */
#define PM_APBCMASK_TC6             (_U(0x1) << PM_APBCMASK_TC6_Pos)
#define PM_APBCMASK_TC7_Pos         15           /**< \brief (PM_APBCMASK) TC7 APB Clock Enable */
#define PM_APBCMASK_TC7             (_U(0x1) << PM_APBCMASK_TC7_Pos)
#define PM_APBCMASK_ADC_Pos         16           /**< \brief (PM_APBCMASK) ADC APB Clock Enable */
#define PM_APBCMASK_ADC             (_U(0x1) << PM_APBCMASK_ADC_Pos)
#define PM_APBCMASK_AC_Pos          17           /**< \brief (PM_APBCMASK) AC APB Clock Enable */
#define PM_APBCMASK_AC              (_U(0x1) << PM_APBCMASK_AC_Pos)
#define PM_APBCMASK_DAC_Pos         18           /**< \brief (PM_APBCMASK) DAC APB Clock Enable */
#define PM_APBCMASK_DAC             (_U(0x1) << PM_APBCMASK_DAC_Pos)
#define PM_APBCMASK_PTC_Pos         19           /**< \brief (PM_APBCMASK) PTC APB Clock Enable */
#define PM_APBCMASK_PTC             (_U(0x1) << PM_APBCMASK_PTC_Pos)
#define PM_APBCMASK_I2S_Pos         20           /**< \brief (PM_APBCMASK) I2S APB Clock Enable */
#define PM_APBCMASK_I2S             (_U(0x1) << PM_APBCMASK_I2S_Pos)
#define PM_APBCMASK_MASK            _U(0x001FFFFF) /**< \brief (PM_APBCMASK) MASK Register */

/* -------- PM_INTENCLR : (PM Offset: 0x34) (R/W  8) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CKRDY:1;          /*!< bit:      0  Clock Ready Interrupt Enable       */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_INTENCLR_OFFSET          0x34         /**< \brief (PM_INTENCLR offset) Interrupt Enable Clear */
#define PM_INTENCLR_RESETVALUE      _U(0x00)     /**< \brief (PM_INTENCLR reset_value) Interrupt Enable Clear */

#define PM_INTENCLR_CKRDY_Pos       0            /**< \brief (PM_INTENCLR) Clock Ready Interrupt Enable */
#define PM_INTENCLR_CKRDY           (_U(0x1) << PM_INTENCLR_CKRDY_Pos)
#define PM_INTENCLR_MASK            _U(0x01)     /**< \brief (PM_INTENCLR) MASK Register */

/* -------- PM_INTENSET : (PM Offset: 0x35) (R/W  8) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CKRDY:1;          /*!< bit:      0  Clock Ready Interrupt Enable       */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_INTENSET_OFFSET          0x35         /**< \brief (PM_INTENSET offset) Interrupt Enable Set */
#define PM_INTENSET_RESETVALUE      _U(0x00)     /**< \brief (PM_INTENSET reset_value) Interrupt Enable Set */

#define PM_INTENSET_CKRDY_Pos       0            /**< \brief (PM_INTENSET) Clock Ready Interrupt Enable */
#define PM_INTENSET_CKRDY           (_U(0x1) << PM_INTENSET_CKRDY_Pos)
#define PM_INTENSET_MASK            _U(0x01)     /**< \brief (PM_INTENSET) MASK Register */

/* -------- PM_INTFLAG : (PM Offset: 0x36) (R/W  8) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint8_t  CKRDY:1;          /*!< bit:      0  Clock Ready                        */
    __I uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_INTFLAG_OFFSET           0x36         /**< \brief (PM_INTFLAG offset) Interrupt Flag Status and Clear */
#define PM_INTFLAG_RESETVALUE       _U(0x00)     /**< \brief (PM_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define PM_INTFLAG_CKRDY_Pos        0            /**< \brief (PM_INTFLAG) Clock Ready */
#define PM_INTFLAG_CKRDY            (_U(0x1) << PM_INTFLAG_CKRDY_Pos)
#define PM_INTFLAG_MASK             _U(0x01)     /**< \brief (PM_INTFLAG) MASK Register */

/* -------- PM_RCAUSE : (PM Offset: 0x38) (R/   8) Reset Cause -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  POR:1;            /*!< bit:      0  Power On Reset                     */
    uint8_t  BOD12:1;          /*!< bit:      1  Brown Out 12 Detector Reset        */
    uint8_t  BOD33:1;          /*!< bit:      2  Brown Out 33 Detector Reset        */
    uint8_t  :1;               /*!< bit:      3  Reserved                           */
    uint8_t  EXT:1;            /*!< bit:      4  External Reset                     */
    uint8_t  WDT:1;            /*!< bit:      5  Watchdog Reset                     */
    uint8_t  SYST:1;           /*!< bit:      6  System Reset Request               */
    uint8_t  :1;               /*!< bit:      7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_RCAUSE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_RCAUSE_OFFSET            0x38         /**< \brief (PM_RCAUSE offset) Reset Cause */
#define PM_RCAUSE_RESETVALUE        _U(0x01)     /**< \brief (PM_RCAUSE reset_value) Reset Cause */

#define PM_RCAUSE_POR_Pos           0            /**< \brief (PM_RCAUSE) Power On Reset */
#define PM_RCAUSE_POR               (_U(0x1) << PM_RCAUSE_POR_Pos)
#define PM_RCAUSE_BOD12_Pos         1            /**< \brief (PM_RCAUSE) Brown Out 12 Detector Reset */
#define PM_RCAUSE_BOD12             (_U(0x1) << PM_RCAUSE_BOD12_Pos)
#define PM_RCAUSE_BOD33_Pos         2            /**< \brief (PM_RCAUSE) Brown Out 33 Detector Reset */
#define PM_RCAUSE_BOD33             (_U(0x1) << PM_RCAUSE_BOD33_Pos)
#define PM_RCAUSE_EXT_Pos           4            /**< \brief (PM_RCAUSE) External Reset */
#define PM_RCAUSE_EXT               (_U(0x1) << PM_RCAUSE_EXT_Pos)
#define PM_RCAUSE_WDT_Pos           5            /**< \brief (PM_RCAUSE) Watchdog Reset */
#define PM_RCAUSE_WDT               (_U(0x1) << PM_RCAUSE_WDT_Pos)
#define PM_RCAUSE_SYST_Pos          6            /**< \brief (PM_RCAUSE) System Reset Request */
#define PM_RCAUSE_SYST              (_U(0x1) << PM_RCAUSE_SYST_Pos)
#define PM_RCAUSE_MASK              _U(0x77)     /**< \brief (PM_RCAUSE) MASK Register */

/** \brief PM hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO PM_CTRL_Type              CTRL;        /**< \brief Offset: 0x00 (R/W  8) Control */
  __IO PM_SLEEP_Type             SLEEP;       /**< \brief Offset: 0x01 (R/W  8) Sleep Mode */
       RoReg8                    Reserved1[0x6];
  __IO PM_CPUSEL_Type            CPUSEL;      /**< \brief Offset: 0x08 (R/W  8) CPU Clock Select */
  __IO PM_APBASEL_Type           APBASEL;     /**< \brief Offset: 0x09 (R/W  8) APBA Clock Select */
  __IO PM_APBBSEL_Type           APBBSEL;     /**< \brief Offset: 0x0A (R/W  8) APBB Clock Select */
  __IO PM_APBCSEL_Type           APBCSEL;     /**< \brief Offset: 0x0B (R/W  8) APBC Clock Select */
       RoReg8                    Reserved2[0x8];
  __IO PM_AHBMASK_Type           AHBMASK;     /**< \brief Offset: 0x14 (R/W 32) AHB Mask */
  __IO PM_APBAMASK_Type          APBAMASK;    /**< \brief Offset: 0x18 (R/W 32) APBA Mask */
  __IO PM_APBBMASK_Type          APBBMASK;    /**< \brief Offset: 0x1C (R/W 32) APBB Mask */
  __IO PM_APBCMASK_Type          APBCMASK;    /**< \brief Offset: 0x20 (R/W 32) APBC Mask */
       RoReg8                    Reserved3[0x10];
  __IO PM_INTENCLR_Type          INTENCLR;    /**< \brief Offset: 0x34 (R/W  8) Interrupt Enable Clear */
  __IO PM_INTENSET_Type          INTENSET;    /**< \brief Offset: 0x35 (R/W  8) Interrupt Enable Set */
  __IO PM_INTFLAG_Type           INTFLAG;     /**< \brief Offset: 0x36 (R/W  8) Interrupt Flag Status and Clear */
       RoReg8                    Reserved4[0x1];
  __I  PM_RCAUSE_Type            RCAUSE;      /**< \brief Offset: 0x38 (R/   8) Reset Cause */
} Pm;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD21_PM_COMPONENT_ */
