/**
 * \file
 *
 * \brief Component description for TAL
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

#ifndef _SAMD51_TAL_COMPONENT_
#define _SAMD51_TAL_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR TAL */
/* ========================================================================== */
/** \addtogroup SAMD51_TAL Trigger Allocator */
/*@{*/

#define TAL_U2253
#define REV_TAL                     0x200

/* -------- TAL_CTRLA : (TAL Offset: 0x000) (R/W  8) Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  ENABLE:1;         /*!< bit:      1  Enable                             */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TAL_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_CTRLA_OFFSET            0x000        /**< \brief (TAL_CTRLA offset) Control A */
#define TAL_CTRLA_RESETVALUE        _U_(0x00)    /**< \brief (TAL_CTRLA reset_value) Control A */

#define TAL_CTRLA_SWRST_Pos         0            /**< \brief (TAL_CTRLA) Software Reset */
#define TAL_CTRLA_SWRST             (_U_(0x1) << TAL_CTRLA_SWRST_Pos)
#define TAL_CTRLA_ENABLE_Pos        1            /**< \brief (TAL_CTRLA) Enable */
#define TAL_CTRLA_ENABLE            (_U_(0x1) << TAL_CTRLA_ENABLE_Pos)
#define TAL_CTRLA_MASK              _U_(0x03)    /**< \brief (TAL_CTRLA) MASK Register */

/* -------- TAL_EXTCTRL : (TAL Offset: 0x001) (R/W  8) External Break Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  ENABLE:1;         /*!< bit:      0  Enable BRK Pin                     */
    uint8_t  INV:1;            /*!< bit:      1  Invert BRK Pin                     */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TAL_EXTCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_EXTCTRL_OFFSET          0x001        /**< \brief (TAL_EXTCTRL offset) External Break Control */
#define TAL_EXTCTRL_RESETVALUE      _U_(0x00)    /**< \brief (TAL_EXTCTRL reset_value) External Break Control */

#define TAL_EXTCTRL_ENABLE_Pos      0            /**< \brief (TAL_EXTCTRL) Enable BRK Pin */
#define TAL_EXTCTRL_ENABLE          (_U_(0x1) << TAL_EXTCTRL_ENABLE_Pos)
#define TAL_EXTCTRL_INV_Pos         1            /**< \brief (TAL_EXTCTRL) Invert BRK Pin */
#define TAL_EXTCTRL_INV             (_U_(0x1) << TAL_EXTCTRL_INV_Pos)
#define TAL_EXTCTRL_MASK            _U_(0x03)    /**< \brief (TAL_EXTCTRL) MASK Register */

/* -------- TAL_EVCTRL : (TAL Offset: 0x004) (R/W 16) Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t BRKEI:1;          /*!< bit:      0  Break Input Event Enable           */
    uint16_t BRKEO:1;          /*!< bit:      1  Break Output Event Enable          */
    uint16_t IRQMONEO0:1;      /*!< bit:      2  Interrupt Request Monitor 0 Output Event Enable */
    uint16_t :13;              /*!< bit:  3..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t :2;               /*!< bit:  0.. 1  Reserved                           */
    uint16_t IRQMONEO:1;       /*!< bit:      2  Interrupt Request Monitor x Output Event Enable */
    uint16_t :13;              /*!< bit:  3..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} TAL_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_EVCTRL_OFFSET           0x004        /**< \brief (TAL_EVCTRL offset) Event Control */
#define TAL_EVCTRL_RESETVALUE       _U_(0x0000)  /**< \brief (TAL_EVCTRL reset_value) Event Control */

#define TAL_EVCTRL_BRKEI_Pos        0            /**< \brief (TAL_EVCTRL) Break Input Event Enable */
#define TAL_EVCTRL_BRKEI            (_U_(0x1) << TAL_EVCTRL_BRKEI_Pos)
#define TAL_EVCTRL_BRKEO_Pos        1            /**< \brief (TAL_EVCTRL) Break Output Event Enable */
#define TAL_EVCTRL_BRKEO            (_U_(0x1) << TAL_EVCTRL_BRKEO_Pos)
#define TAL_EVCTRL_IRQMONEO0_Pos    2            /**< \brief (TAL_EVCTRL) Interrupt Request Monitor 0 Output Event Enable */
#define TAL_EVCTRL_IRQMONEO0        (_U_(1) << TAL_EVCTRL_IRQMONEO0_Pos)
#define TAL_EVCTRL_IRQMONEO_Pos     2            /**< \brief (TAL_EVCTRL) Interrupt Request Monitor x Output Event Enable */
#define TAL_EVCTRL_IRQMONEO_Msk     (_U_(0x1) << TAL_EVCTRL_IRQMONEO_Pos)
#define TAL_EVCTRL_IRQMONEO(value)  (TAL_EVCTRL_IRQMONEO_Msk & ((value) << TAL_EVCTRL_IRQMONEO_Pos))
#define TAL_EVCTRL_MASK             _U_(0x0007)  /**< \brief (TAL_EVCTRL) MASK Register */

/* -------- TAL_INTENCLR : (TAL Offset: 0x008) (R/W  8) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  BRK:1;            /*!< bit:      0  Break Interrupt Enable             */
    uint8_t  IPS0:1;           /*!< bit:      1  Inter-Processor Signal Interrupt Enable for CPU 0 */
    uint8_t  IPS1:1;           /*!< bit:      2  Inter-Processor Signal Interrupt Enable for CPU 1 */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  :1;               /*!< bit:      0  Reserved                           */
    uint8_t  IPS:2;            /*!< bit:  1.. 2  Inter-Processor Signal Interrupt Enable for CPU x */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TAL_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_INTENCLR_OFFSET         0x008        /**< \brief (TAL_INTENCLR offset) Interrupt Enable Clear */
#define TAL_INTENCLR_RESETVALUE     _U_(0x00)    /**< \brief (TAL_INTENCLR reset_value) Interrupt Enable Clear */

#define TAL_INTENCLR_BRK_Pos        0            /**< \brief (TAL_INTENCLR) Break Interrupt Enable */
#define TAL_INTENCLR_BRK            (_U_(0x1) << TAL_INTENCLR_BRK_Pos)
#define TAL_INTENCLR_IPS0_Pos       1            /**< \brief (TAL_INTENCLR) Inter-Processor Signal Interrupt Enable for CPU 0 */
#define TAL_INTENCLR_IPS0           (_U_(1) << TAL_INTENCLR_IPS0_Pos)
#define TAL_INTENCLR_IPS1_Pos       2            /**< \brief (TAL_INTENCLR) Inter-Processor Signal Interrupt Enable for CPU 1 */
#define TAL_INTENCLR_IPS1           (_U_(1) << TAL_INTENCLR_IPS1_Pos)
#define TAL_INTENCLR_IPS_Pos        1            /**< \brief (TAL_INTENCLR) Inter-Processor Signal Interrupt Enable for CPU x */
#define TAL_INTENCLR_IPS_Msk        (_U_(0x3) << TAL_INTENCLR_IPS_Pos)
#define TAL_INTENCLR_IPS(value)     (TAL_INTENCLR_IPS_Msk & ((value) << TAL_INTENCLR_IPS_Pos))
#define TAL_INTENCLR_MASK           _U_(0x07)    /**< \brief (TAL_INTENCLR) MASK Register */

/* -------- TAL_INTENSET : (TAL Offset: 0x009) (R/W  8) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  BRK:1;            /*!< bit:      0  Break Interrupt Enable             */
    uint8_t  IPS0:1;           /*!< bit:      1  Inter-Processor Signal Interrupt Enable for CPU 0 */
    uint8_t  IPS1:1;           /*!< bit:      2  Inter-Processor Signal Interrupt Enable for CPU 1 */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  :1;               /*!< bit:      0  Reserved                           */
    uint8_t  IPS:2;            /*!< bit:  1.. 2  Inter-Processor Signal Interrupt Enable for CPU x */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TAL_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_INTENSET_OFFSET         0x009        /**< \brief (TAL_INTENSET offset) Interrupt Enable Set */
#define TAL_INTENSET_RESETVALUE     _U_(0x00)    /**< \brief (TAL_INTENSET reset_value) Interrupt Enable Set */

#define TAL_INTENSET_BRK_Pos        0            /**< \brief (TAL_INTENSET) Break Interrupt Enable */
#define TAL_INTENSET_BRK            (_U_(0x1) << TAL_INTENSET_BRK_Pos)
#define TAL_INTENSET_IPS0_Pos       1            /**< \brief (TAL_INTENSET) Inter-Processor Signal Interrupt Enable for CPU 0 */
#define TAL_INTENSET_IPS0           (_U_(1) << TAL_INTENSET_IPS0_Pos)
#define TAL_INTENSET_IPS1_Pos       2            /**< \brief (TAL_INTENSET) Inter-Processor Signal Interrupt Enable for CPU 1 */
#define TAL_INTENSET_IPS1           (_U_(1) << TAL_INTENSET_IPS1_Pos)
#define TAL_INTENSET_IPS_Pos        1            /**< \brief (TAL_INTENSET) Inter-Processor Signal Interrupt Enable for CPU x */
#define TAL_INTENSET_IPS_Msk        (_U_(0x3) << TAL_INTENSET_IPS_Pos)
#define TAL_INTENSET_IPS(value)     (TAL_INTENSET_IPS_Msk & ((value) << TAL_INTENSET_IPS_Pos))
#define TAL_INTENSET_MASK           _U_(0x07)    /**< \brief (TAL_INTENSET) MASK Register */

/* -------- TAL_INTFLAG : (TAL Offset: 0x00A) (R/W  8) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint8_t  BRK:1;            /*!< bit:      0  Break                              */
    __I uint8_t  IPS0:1;           /*!< bit:      1  Inter-Processor Signal for CPU 0   */
    __I uint8_t  IPS1:1;           /*!< bit:      2  Inter-Processor Signal for CPU 1   */
    __I uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    __I uint8_t  :1;               /*!< bit:      0  Reserved                           */
    __I uint8_t  IPS:2;            /*!< bit:  1.. 2  Inter-Processor Signal for CPU x   */
    __I uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TAL_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_INTFLAG_OFFSET          0x00A        /**< \brief (TAL_INTFLAG offset) Interrupt Flag Status and Clear */
#define TAL_INTFLAG_RESETVALUE      _U_(0x00)    /**< \brief (TAL_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define TAL_INTFLAG_BRK_Pos         0            /**< \brief (TAL_INTFLAG) Break */
#define TAL_INTFLAG_BRK             (_U_(0x1) << TAL_INTFLAG_BRK_Pos)
#define TAL_INTFLAG_IPS0_Pos        1            /**< \brief (TAL_INTFLAG) Inter-Processor Signal for CPU 0 */
#define TAL_INTFLAG_IPS0            (_U_(1) << TAL_INTFLAG_IPS0_Pos)
#define TAL_INTFLAG_IPS1_Pos        2            /**< \brief (TAL_INTFLAG) Inter-Processor Signal for CPU 1 */
#define TAL_INTFLAG_IPS1            (_U_(1) << TAL_INTFLAG_IPS1_Pos)
#define TAL_INTFLAG_IPS_Pos         1            /**< \brief (TAL_INTFLAG) Inter-Processor Signal for CPU x */
#define TAL_INTFLAG_IPS_Msk         (_U_(0x3) << TAL_INTFLAG_IPS_Pos)
#define TAL_INTFLAG_IPS(value)      (TAL_INTFLAG_IPS_Msk & ((value) << TAL_INTFLAG_IPS_Pos))
#define TAL_INTFLAG_MASK            _U_(0x07)    /**< \brief (TAL_INTFLAG) MASK Register */

/* -------- TAL_GLOBMASK : (TAL Offset: 0x00B) (R/W  8) Global Break Requests Mask -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CPU0:1;           /*!< bit:      0  CPU 0 Break Master                 */
    uint8_t  CPU1:1;           /*!< bit:      1  CPU 1 Break Master                 */
    uint8_t  :4;               /*!< bit:  2.. 5  Reserved                           */
    uint8_t  EVBRK:1;          /*!< bit:      6  Event Break Master                 */
    uint8_t  EXTBRK:1;         /*!< bit:      7  External Break Master              */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  CPU:2;            /*!< bit:  0.. 1  CPU x Break Master                 */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TAL_GLOBMASK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_GLOBMASK_OFFSET         0x00B        /**< \brief (TAL_GLOBMASK offset) Global Break Requests Mask */
#define TAL_GLOBMASK_RESETVALUE     _U_(0x00)    /**< \brief (TAL_GLOBMASK reset_value) Global Break Requests Mask */

#define TAL_GLOBMASK_CPU0_Pos       0            /**< \brief (TAL_GLOBMASK) CPU 0 Break Master */
#define TAL_GLOBMASK_CPU0           (_U_(1) << TAL_GLOBMASK_CPU0_Pos)
#define TAL_GLOBMASK_CPU1_Pos       1            /**< \brief (TAL_GLOBMASK) CPU 1 Break Master */
#define TAL_GLOBMASK_CPU1           (_U_(1) << TAL_GLOBMASK_CPU1_Pos)
#define TAL_GLOBMASK_CPU_Pos        0            /**< \brief (TAL_GLOBMASK) CPU x Break Master */
#define TAL_GLOBMASK_CPU_Msk        (_U_(0x3) << TAL_GLOBMASK_CPU_Pos)
#define TAL_GLOBMASK_CPU(value)     (TAL_GLOBMASK_CPU_Msk & ((value) << TAL_GLOBMASK_CPU_Pos))
#define TAL_GLOBMASK_EVBRK_Pos      6            /**< \brief (TAL_GLOBMASK) Event Break Master */
#define TAL_GLOBMASK_EVBRK          (_U_(0x1) << TAL_GLOBMASK_EVBRK_Pos)
#define TAL_GLOBMASK_EXTBRK_Pos     7            /**< \brief (TAL_GLOBMASK) External Break Master */
#define TAL_GLOBMASK_EXTBRK         (_U_(0x1) << TAL_GLOBMASK_EXTBRK_Pos)
#define TAL_GLOBMASK_MASK           _U_(0xC3)    /**< \brief (TAL_GLOBMASK) MASK Register */

/* -------- TAL_HALT : (TAL Offset: 0x00C) ( /W  8) Debug Halt Request -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CPU0:1;           /*!< bit:      0  CPU 0 Break Master                 */
    uint8_t  CPU1:1;           /*!< bit:      1  CPU 1 Break Master                 */
    uint8_t  :4;               /*!< bit:  2.. 5  Reserved                           */
    uint8_t  EVBRK:1;          /*!< bit:      6  Event Break Master                 */
    uint8_t  EXTBRK:1;         /*!< bit:      7  External Break Master              */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  CPU:2;            /*!< bit:  0.. 1  CPU x Break Master                 */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TAL_HALT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_HALT_OFFSET             0x00C        /**< \brief (TAL_HALT offset) Debug Halt Request */
#define TAL_HALT_RESETVALUE         _U_(0x00)    /**< \brief (TAL_HALT reset_value) Debug Halt Request */

#define TAL_HALT_CPU0_Pos           0            /**< \brief (TAL_HALT) CPU 0 Break Master */
#define TAL_HALT_CPU0               (_U_(1) << TAL_HALT_CPU0_Pos)
#define TAL_HALT_CPU1_Pos           1            /**< \brief (TAL_HALT) CPU 1 Break Master */
#define TAL_HALT_CPU1               (_U_(1) << TAL_HALT_CPU1_Pos)
#define TAL_HALT_CPU_Pos            0            /**< \brief (TAL_HALT) CPU x Break Master */
#define TAL_HALT_CPU_Msk            (_U_(0x3) << TAL_HALT_CPU_Pos)
#define TAL_HALT_CPU(value)         (TAL_HALT_CPU_Msk & ((value) << TAL_HALT_CPU_Pos))
#define TAL_HALT_EVBRK_Pos          6            /**< \brief (TAL_HALT) Event Break Master */
#define TAL_HALT_EVBRK              (_U_(0x1) << TAL_HALT_EVBRK_Pos)
#define TAL_HALT_EXTBRK_Pos         7            /**< \brief (TAL_HALT) External Break Master */
#define TAL_HALT_EXTBRK             (_U_(0x1) << TAL_HALT_EXTBRK_Pos)
#define TAL_HALT_MASK               _U_(0xC3)    /**< \brief (TAL_HALT) MASK Register */

/* -------- TAL_RESTART : (TAL Offset: 0x00D) ( /W  8) Debug Restart Request -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CPU0:1;           /*!< bit:      0  CPU 0 Break Master                 */
    uint8_t  CPU1:1;           /*!< bit:      1  CPU 1 Break Master                 */
    uint8_t  :5;               /*!< bit:  2.. 6  Reserved                           */
    uint8_t  EXTBRK:1;         /*!< bit:      7  External Break Master              */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  CPU:2;            /*!< bit:  0.. 1  CPU x Break Master                 */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TAL_RESTART_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_RESTART_OFFSET          0x00D        /**< \brief (TAL_RESTART offset) Debug Restart Request */
#define TAL_RESTART_RESETVALUE      _U_(0x00)    /**< \brief (TAL_RESTART reset_value) Debug Restart Request */

#define TAL_RESTART_CPU0_Pos        0            /**< \brief (TAL_RESTART) CPU 0 Break Master */
#define TAL_RESTART_CPU0            (_U_(1) << TAL_RESTART_CPU0_Pos)
#define TAL_RESTART_CPU1_Pos        1            /**< \brief (TAL_RESTART) CPU 1 Break Master */
#define TAL_RESTART_CPU1            (_U_(1) << TAL_RESTART_CPU1_Pos)
#define TAL_RESTART_CPU_Pos         0            /**< \brief (TAL_RESTART) CPU x Break Master */
#define TAL_RESTART_CPU_Msk         (_U_(0x3) << TAL_RESTART_CPU_Pos)
#define TAL_RESTART_CPU(value)      (TAL_RESTART_CPU_Msk & ((value) << TAL_RESTART_CPU_Pos))
#define TAL_RESTART_EXTBRK_Pos      7            /**< \brief (TAL_RESTART) External Break Master */
#define TAL_RESTART_EXTBRK          (_U_(0x1) << TAL_RESTART_EXTBRK_Pos)
#define TAL_RESTART_MASK            _U_(0x83)    /**< \brief (TAL_RESTART) MASK Register */

/* -------- TAL_BRKSTATUS : (TAL Offset: 0x00E) (R/  16) Break Request Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t CPU0:2;           /*!< bit:  0.. 1  CPU 0 Break Request                */
    uint16_t CPU1:2;           /*!< bit:  2.. 3  CPU 1 Break Request                */
    uint16_t :8;               /*!< bit:  4..11  Reserved                           */
    uint16_t EVBRK:2;          /*!< bit: 12..13  Event Break Request                */
    uint16_t EXTBRK:2;         /*!< bit: 14..15  External Break Request             */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} TAL_BRKSTATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_BRKSTATUS_OFFSET        0x00E        /**< \brief (TAL_BRKSTATUS offset) Break Request Status */
#define TAL_BRKSTATUS_RESETVALUE    _U_(0x0000)  /**< \brief (TAL_BRKSTATUS reset_value) Break Request Status */

#define TAL_BRKSTATUS_CPU0_Pos      0            /**< \brief (TAL_BRKSTATUS) CPU 0 Break Request */
#define TAL_BRKSTATUS_CPU0_Msk      (_U_(0x3) << TAL_BRKSTATUS_CPU0_Pos)
#define TAL_BRKSTATUS_CPU0(value)   (TAL_BRKSTATUS_CPU0_Msk & ((value) << TAL_BRKSTATUS_CPU0_Pos))
#define TAL_BRKSTATUS_CPU1_Pos      2            /**< \brief (TAL_BRKSTATUS) CPU 1 Break Request */
#define TAL_BRKSTATUS_CPU1_Msk      (_U_(0x3) << TAL_BRKSTATUS_CPU1_Pos)
#define TAL_BRKSTATUS_CPU1(value)   (TAL_BRKSTATUS_CPU1_Msk & ((value) << TAL_BRKSTATUS_CPU1_Pos))
#define TAL_BRKSTATUS_EVBRK_Pos     12           /**< \brief (TAL_BRKSTATUS) Event Break Request */
#define TAL_BRKSTATUS_EVBRK_Msk     (_U_(0x3) << TAL_BRKSTATUS_EVBRK_Pos)
#define TAL_BRKSTATUS_EVBRK(value)  (TAL_BRKSTATUS_EVBRK_Msk & ((value) << TAL_BRKSTATUS_EVBRK_Pos))
#define TAL_BRKSTATUS_EXTBRK_Pos    14           /**< \brief (TAL_BRKSTATUS) External Break Request */
#define TAL_BRKSTATUS_EXTBRK_Msk    (_U_(0x3) << TAL_BRKSTATUS_EXTBRK_Pos)
#define TAL_BRKSTATUS_EXTBRK(value) (TAL_BRKSTATUS_EXTBRK_Msk & ((value) << TAL_BRKSTATUS_EXTBRK_Pos))
#define TAL_BRKSTATUS_MASK          _U_(0xF00F)  /**< \brief (TAL_BRKSTATUS) MASK Register */

/* -------- TAL_CTICTRLA : (TAL Offset: 0x010) (R/W  8) CTIS Cross-Trigger Interface n Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  BRK:2;            /*!< bit:  0.. 1  Action when global break issued    */
    uint8_t  RESTART:1;        /*!< bit:      2  Action when global restart issued  */
    uint8_t  IPS:1;            /*!< bit:      3  Action when inter-process resource freed */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TAL_CTICTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_CTICTRLA_OFFSET         0x010        /**< \brief (TAL_CTICTRLA offset) Cross-Trigger Interface n Control A */
#define TAL_CTICTRLA_RESETVALUE     _U_(0x00)    /**< \brief (TAL_CTICTRLA reset_value) Cross-Trigger Interface n Control A */

#define TAL_CTICTRLA_BRK_Pos        0            /**< \brief (TAL_CTICTRLA) Action when global break issued */
#define TAL_CTICTRLA_BRK_Msk        (_U_(0x3) << TAL_CTICTRLA_BRK_Pos)
#define TAL_CTICTRLA_BRK(value)     (TAL_CTICTRLA_BRK_Msk & ((value) << TAL_CTICTRLA_BRK_Pos))
#define   TAL_CTICTRLA_BRK_BREAK_Val      _U_(0x0)   /**< \brief (TAL_CTICTRLA) Break when requested */
#define   TAL_CTICTRLA_BRK_INTERRUPT_Val  _U_(0x1)   /**< \brief (TAL_CTICTRLA) Trigger DBG interrupt instead of break */
#define   TAL_CTICTRLA_BRK_IGNORE_Val     _U_(0x2)   /**< \brief (TAL_CTICTRLA) Ignore break request */
#define TAL_CTICTRLA_BRK_BREAK      (TAL_CTICTRLA_BRK_BREAK_Val    << TAL_CTICTRLA_BRK_Pos)
#define TAL_CTICTRLA_BRK_INTERRUPT  (TAL_CTICTRLA_BRK_INTERRUPT_Val << TAL_CTICTRLA_BRK_Pos)
#define TAL_CTICTRLA_BRK_IGNORE     (TAL_CTICTRLA_BRK_IGNORE_Val   << TAL_CTICTRLA_BRK_Pos)
#define TAL_CTICTRLA_RESTART_Pos    2            /**< \brief (TAL_CTICTRLA) Action when global restart issued */
#define TAL_CTICTRLA_RESTART        (_U_(0x1) << TAL_CTICTRLA_RESTART_Pos)
#define   TAL_CTICTRLA_RESTART_RESTART_Val _U_(0x0)   /**< \brief (TAL_CTICTRLA) Restart when requested */
#define   TAL_CTICTRLA_RESTART_IGNORE_Val _U_(0x1)   /**< \brief (TAL_CTICTRLA) Ignore restart request */
#define TAL_CTICTRLA_RESTART_RESTART (TAL_CTICTRLA_RESTART_RESTART_Val << TAL_CTICTRLA_RESTART_Pos)
#define TAL_CTICTRLA_RESTART_IGNORE (TAL_CTICTRLA_RESTART_IGNORE_Val << TAL_CTICTRLA_RESTART_Pos)
#define TAL_CTICTRLA_IPS_Pos        3            /**< \brief (TAL_CTICTRLA) Action when inter-process resource freed */
#define TAL_CTICTRLA_IPS            (_U_(0x1) << TAL_CTICTRLA_IPS_Pos)
#define   TAL_CTICTRLA_IPS_EVENT_Val      _U_(0x0)   /**< \brief (TAL_CTICTRLA) Generate CPU Event when awaited resource is freed. */
#define   TAL_CTICTRLA_IPS_INTERRUPT_Val  _U_(0x1)   /**< \brief (TAL_CTICTRLA) Generate Interrupt when awaited resource is freed. */
#define TAL_CTICTRLA_IPS_EVENT      (TAL_CTICTRLA_IPS_EVENT_Val    << TAL_CTICTRLA_IPS_Pos)
#define TAL_CTICTRLA_IPS_INTERRUPT  (TAL_CTICTRLA_IPS_INTERRUPT_Val << TAL_CTICTRLA_IPS_Pos)
#define TAL_CTICTRLA_MASK           _U_(0x0F)    /**< \brief (TAL_CTICTRLA) MASK Register */

/* -------- TAL_CTIMASK : (TAL Offset: 0x011) (R/W  8) CTIS Cross-Trigger Interface n Mask -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CPU0:1;           /*!< bit:      0  CPU 0 Break Master                 */
    uint8_t  CPU1:1;           /*!< bit:      1  CPU 1 Break Master                 */
    uint8_t  :4;               /*!< bit:  2.. 5  Reserved                           */
    uint8_t  EVBRK:1;          /*!< bit:      6  Event Break Master                 */
    uint8_t  EXTBRK:1;         /*!< bit:      7  External Break Master              */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  CPU:2;            /*!< bit:  0.. 1  CPU x Break Master                 */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TAL_CTIMASK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_CTIMASK_OFFSET          0x011        /**< \brief (TAL_CTIMASK offset) Cross-Trigger Interface n Mask */
#define TAL_CTIMASK_RESETVALUE      _U_(0x00)    /**< \brief (TAL_CTIMASK reset_value) Cross-Trigger Interface n Mask */

#define TAL_CTIMASK_CPU0_Pos        0            /**< \brief (TAL_CTIMASK) CPU 0 Break Master */
#define TAL_CTIMASK_CPU0            (_U_(1) << TAL_CTIMASK_CPU0_Pos)
#define TAL_CTIMASK_CPU1_Pos        1            /**< \brief (TAL_CTIMASK) CPU 1 Break Master */
#define TAL_CTIMASK_CPU1            (_U_(1) << TAL_CTIMASK_CPU1_Pos)
#define TAL_CTIMASK_CPU_Pos         0            /**< \brief (TAL_CTIMASK) CPU x Break Master */
#define TAL_CTIMASK_CPU_Msk         (_U_(0x3) << TAL_CTIMASK_CPU_Pos)
#define TAL_CTIMASK_CPU(value)      (TAL_CTIMASK_CPU_Msk & ((value) << TAL_CTIMASK_CPU_Pos))
#define TAL_CTIMASK_EVBRK_Pos       6            /**< \brief (TAL_CTIMASK) Event Break Master */
#define TAL_CTIMASK_EVBRK           (_U_(0x1) << TAL_CTIMASK_EVBRK_Pos)
#define TAL_CTIMASK_EXTBRK_Pos      7            /**< \brief (TAL_CTIMASK) External Break Master */
#define TAL_CTIMASK_EXTBRK          (_U_(0x1) << TAL_CTIMASK_EXTBRK_Pos)
#define TAL_CTIMASK_MASK            _U_(0xC3)    /**< \brief (TAL_CTIMASK) MASK Register */

/* -------- TAL_INTSTATUS : (TAL Offset: 0x020) (R/   8) Interrupt n Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  IRQ0:1;           /*!< bit:      0  Interrupt Status for Interrupt Request 0 within Interrupt n */
    uint8_t  IRQ1:1;           /*!< bit:      1  Interrupt Status for Interrupt Request 1 within Interrupt n */
    uint8_t  IRQ2:1;           /*!< bit:      2  Interrupt Status for Interrupt Request 2 within Interrupt n */
    uint8_t  IRQ3:1;           /*!< bit:      3  Interrupt Status for Interrupt Request 3 within Interrupt n */
    uint8_t  IRQ4:1;           /*!< bit:      4  Interrupt Status for Interrupt Request 4 within Interrupt n */
    uint8_t  IRQ5:1;           /*!< bit:      5  Interrupt Status for Interrupt Request 5 within Interrupt n */
    uint8_t  IRQ6:1;           /*!< bit:      6  Interrupt Status for Interrupt Request 6 within Interrupt n */
    uint8_t  IRQ7:1;           /*!< bit:      7  Interrupt Status for Interrupt Request 7 within Interrupt n */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  IRQ:8;            /*!< bit:  0.. 7  Interrupt Status for Interrupt Request x within Interrupt n */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TAL_INTSTATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_INTSTATUS_OFFSET        0x020        /**< \brief (TAL_INTSTATUS offset) Interrupt n Status */
#define TAL_INTSTATUS_RESETVALUE    _U_(0x00)    /**< \brief (TAL_INTSTATUS reset_value) Interrupt n Status */

#define TAL_INTSTATUS_IRQ0_Pos      0            /**< \brief (TAL_INTSTATUS) Interrupt Status for Interrupt Request 0 within Interrupt n */
#define TAL_INTSTATUS_IRQ0          (_U_(1) << TAL_INTSTATUS_IRQ0_Pos)
#define TAL_INTSTATUS_IRQ1_Pos      1            /**< \brief (TAL_INTSTATUS) Interrupt Status for Interrupt Request 1 within Interrupt n */
#define TAL_INTSTATUS_IRQ1          (_U_(1) << TAL_INTSTATUS_IRQ1_Pos)
#define TAL_INTSTATUS_IRQ2_Pos      2            /**< \brief (TAL_INTSTATUS) Interrupt Status for Interrupt Request 2 within Interrupt n */
#define TAL_INTSTATUS_IRQ2          (_U_(1) << TAL_INTSTATUS_IRQ2_Pos)
#define TAL_INTSTATUS_IRQ3_Pos      3            /**< \brief (TAL_INTSTATUS) Interrupt Status for Interrupt Request 3 within Interrupt n */
#define TAL_INTSTATUS_IRQ3          (_U_(1) << TAL_INTSTATUS_IRQ3_Pos)
#define TAL_INTSTATUS_IRQ4_Pos      4            /**< \brief (TAL_INTSTATUS) Interrupt Status for Interrupt Request 4 within Interrupt n */
#define TAL_INTSTATUS_IRQ4          (_U_(1) << TAL_INTSTATUS_IRQ4_Pos)
#define TAL_INTSTATUS_IRQ5_Pos      5            /**< \brief (TAL_INTSTATUS) Interrupt Status for Interrupt Request 5 within Interrupt n */
#define TAL_INTSTATUS_IRQ5          (_U_(1) << TAL_INTSTATUS_IRQ5_Pos)
#define TAL_INTSTATUS_IRQ6_Pos      6            /**< \brief (TAL_INTSTATUS) Interrupt Status for Interrupt Request 6 within Interrupt n */
#define TAL_INTSTATUS_IRQ6          (_U_(1) << TAL_INTSTATUS_IRQ6_Pos)
#define TAL_INTSTATUS_IRQ7_Pos      7            /**< \brief (TAL_INTSTATUS) Interrupt Status for Interrupt Request 7 within Interrupt n */
#define TAL_INTSTATUS_IRQ7          (_U_(1) << TAL_INTSTATUS_IRQ7_Pos)
#define TAL_INTSTATUS_IRQ_Pos       0            /**< \brief (TAL_INTSTATUS) Interrupt Status for Interrupt Request x within Interrupt n */
#define TAL_INTSTATUS_IRQ_Msk       (_U_(0xFF) << TAL_INTSTATUS_IRQ_Pos)
#define TAL_INTSTATUS_IRQ(value)    (TAL_INTSTATUS_IRQ_Msk & ((value) << TAL_INTSTATUS_IRQ_Pos))
#define TAL_INTSTATUS_MASK          _U_(0xFF)    /**< \brief (TAL_INTSTATUS) MASK Register */

/* -------- TAL_DMACPUSEL0 : (TAL Offset: 0x110) (R/W 32) DMA Channel Interrupts CPU Select 0 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CH0:1;            /*!< bit:      0  DMA Channel 0 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:      1  Reserved                           */
    uint32_t CH1:1;            /*!< bit:      2  DMA Channel 1 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:      3  Reserved                           */
    uint32_t CH2:1;            /*!< bit:      4  DMA Channel 2 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:      5  Reserved                           */
    uint32_t CH3:1;            /*!< bit:      6  DMA Channel 3 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:      7  Reserved                           */
    uint32_t CH4:1;            /*!< bit:      8  DMA Channel 4 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:      9  Reserved                           */
    uint32_t CH5:1;            /*!< bit:     10  DMA Channel 5 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     11  Reserved                           */
    uint32_t CH6:1;            /*!< bit:     12  DMA Channel 6 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     13  Reserved                           */
    uint32_t CH7:1;            /*!< bit:     14  DMA Channel 7 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     15  Reserved                           */
    uint32_t CH8:1;            /*!< bit:     16  DMA Channel 8 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     17  Reserved                           */
    uint32_t CH9:1;            /*!< bit:     18  DMA Channel 9 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     19  Reserved                           */
    uint32_t CH10:1;           /*!< bit:     20  DMA Channel 10 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     21  Reserved                           */
    uint32_t CH11:1;           /*!< bit:     22  DMA Channel 11 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     23  Reserved                           */
    uint32_t CH12:1;           /*!< bit:     24  DMA Channel 12 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     25  Reserved                           */
    uint32_t CH13:1;           /*!< bit:     26  DMA Channel 13 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     27  Reserved                           */
    uint32_t CH14:1;           /*!< bit:     28  DMA Channel 14 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     29  Reserved                           */
    uint32_t CH15:1;           /*!< bit:     30  DMA Channel 15 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TAL_DMACPUSEL0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_DMACPUSEL0_OFFSET       0x110        /**< \brief (TAL_DMACPUSEL0 offset) DMA Channel Interrupts CPU Select 0 */
#define TAL_DMACPUSEL0_RESETVALUE   _U_(0x00000000) /**< \brief (TAL_DMACPUSEL0 reset_value) DMA Channel Interrupts CPU Select 0 */

#define TAL_DMACPUSEL0_CH0_Pos      0            /**< \brief (TAL_DMACPUSEL0) DMA Channel 0 Interrupt CPU Select */
#define TAL_DMACPUSEL0_CH0_Msk      (_U_(0x1) << TAL_DMACPUSEL0_CH0_Pos)
#define TAL_DMACPUSEL0_CH0(value)   (TAL_DMACPUSEL0_CH0_Msk & ((value) << TAL_DMACPUSEL0_CH0_Pos))
#define TAL_DMACPUSEL0_CH1_Pos      2            /**< \brief (TAL_DMACPUSEL0) DMA Channel 1 Interrupt CPU Select */
#define TAL_DMACPUSEL0_CH1_Msk      (_U_(0x1) << TAL_DMACPUSEL0_CH1_Pos)
#define TAL_DMACPUSEL0_CH1(value)   (TAL_DMACPUSEL0_CH1_Msk & ((value) << TAL_DMACPUSEL0_CH1_Pos))
#define TAL_DMACPUSEL0_CH2_Pos      4            /**< \brief (TAL_DMACPUSEL0) DMA Channel 2 Interrupt CPU Select */
#define TAL_DMACPUSEL0_CH2_Msk      (_U_(0x1) << TAL_DMACPUSEL0_CH2_Pos)
#define TAL_DMACPUSEL0_CH2(value)   (TAL_DMACPUSEL0_CH2_Msk & ((value) << TAL_DMACPUSEL0_CH2_Pos))
#define TAL_DMACPUSEL0_CH3_Pos      6            /**< \brief (TAL_DMACPUSEL0) DMA Channel 3 Interrupt CPU Select */
#define TAL_DMACPUSEL0_CH3_Msk      (_U_(0x1) << TAL_DMACPUSEL0_CH3_Pos)
#define TAL_DMACPUSEL0_CH3(value)   (TAL_DMACPUSEL0_CH3_Msk & ((value) << TAL_DMACPUSEL0_CH3_Pos))
#define TAL_DMACPUSEL0_CH4_Pos      8            /**< \brief (TAL_DMACPUSEL0) DMA Channel 4 Interrupt CPU Select */
#define TAL_DMACPUSEL0_CH4_Msk      (_U_(0x1) << TAL_DMACPUSEL0_CH4_Pos)
#define TAL_DMACPUSEL0_CH4(value)   (TAL_DMACPUSEL0_CH4_Msk & ((value) << TAL_DMACPUSEL0_CH4_Pos))
#define TAL_DMACPUSEL0_CH5_Pos      10           /**< \brief (TAL_DMACPUSEL0) DMA Channel 5 Interrupt CPU Select */
#define TAL_DMACPUSEL0_CH5_Msk      (_U_(0x1) << TAL_DMACPUSEL0_CH5_Pos)
#define TAL_DMACPUSEL0_CH5(value)   (TAL_DMACPUSEL0_CH5_Msk & ((value) << TAL_DMACPUSEL0_CH5_Pos))
#define TAL_DMACPUSEL0_CH6_Pos      12           /**< \brief (TAL_DMACPUSEL0) DMA Channel 6 Interrupt CPU Select */
#define TAL_DMACPUSEL0_CH6_Msk      (_U_(0x1) << TAL_DMACPUSEL0_CH6_Pos)
#define TAL_DMACPUSEL0_CH6(value)   (TAL_DMACPUSEL0_CH6_Msk & ((value) << TAL_DMACPUSEL0_CH6_Pos))
#define TAL_DMACPUSEL0_CH7_Pos      14           /**< \brief (TAL_DMACPUSEL0) DMA Channel 7 Interrupt CPU Select */
#define TAL_DMACPUSEL0_CH7_Msk      (_U_(0x1) << TAL_DMACPUSEL0_CH7_Pos)
#define TAL_DMACPUSEL0_CH7(value)   (TAL_DMACPUSEL0_CH7_Msk & ((value) << TAL_DMACPUSEL0_CH7_Pos))
#define TAL_DMACPUSEL0_CH8_Pos      16           /**< \brief (TAL_DMACPUSEL0) DMA Channel 8 Interrupt CPU Select */
#define TAL_DMACPUSEL0_CH8_Msk      (_U_(0x1) << TAL_DMACPUSEL0_CH8_Pos)
#define TAL_DMACPUSEL0_CH8(value)   (TAL_DMACPUSEL0_CH8_Msk & ((value) << TAL_DMACPUSEL0_CH8_Pos))
#define TAL_DMACPUSEL0_CH9_Pos      18           /**< \brief (TAL_DMACPUSEL0) DMA Channel 9 Interrupt CPU Select */
#define TAL_DMACPUSEL0_CH9_Msk      (_U_(0x1) << TAL_DMACPUSEL0_CH9_Pos)
#define TAL_DMACPUSEL0_CH9(value)   (TAL_DMACPUSEL0_CH9_Msk & ((value) << TAL_DMACPUSEL0_CH9_Pos))
#define TAL_DMACPUSEL0_CH10_Pos     20           /**< \brief (TAL_DMACPUSEL0) DMA Channel 10 Interrupt CPU Select */
#define TAL_DMACPUSEL0_CH10_Msk     (_U_(0x1) << TAL_DMACPUSEL0_CH10_Pos)
#define TAL_DMACPUSEL0_CH10(value)  (TAL_DMACPUSEL0_CH10_Msk & ((value) << TAL_DMACPUSEL0_CH10_Pos))
#define TAL_DMACPUSEL0_CH11_Pos     22           /**< \brief (TAL_DMACPUSEL0) DMA Channel 11 Interrupt CPU Select */
#define TAL_DMACPUSEL0_CH11_Msk     (_U_(0x1) << TAL_DMACPUSEL0_CH11_Pos)
#define TAL_DMACPUSEL0_CH11(value)  (TAL_DMACPUSEL0_CH11_Msk & ((value) << TAL_DMACPUSEL0_CH11_Pos))
#define TAL_DMACPUSEL0_CH12_Pos     24           /**< \brief (TAL_DMACPUSEL0) DMA Channel 12 Interrupt CPU Select */
#define TAL_DMACPUSEL0_CH12_Msk     (_U_(0x1) << TAL_DMACPUSEL0_CH12_Pos)
#define TAL_DMACPUSEL0_CH12(value)  (TAL_DMACPUSEL0_CH12_Msk & ((value) << TAL_DMACPUSEL0_CH12_Pos))
#define TAL_DMACPUSEL0_CH13_Pos     26           /**< \brief (TAL_DMACPUSEL0) DMA Channel 13 Interrupt CPU Select */
#define TAL_DMACPUSEL0_CH13_Msk     (_U_(0x1) << TAL_DMACPUSEL0_CH13_Pos)
#define TAL_DMACPUSEL0_CH13(value)  (TAL_DMACPUSEL0_CH13_Msk & ((value) << TAL_DMACPUSEL0_CH13_Pos))
#define TAL_DMACPUSEL0_CH14_Pos     28           /**< \brief (TAL_DMACPUSEL0) DMA Channel 14 Interrupt CPU Select */
#define TAL_DMACPUSEL0_CH14_Msk     (_U_(0x1) << TAL_DMACPUSEL0_CH14_Pos)
#define TAL_DMACPUSEL0_CH14(value)  (TAL_DMACPUSEL0_CH14_Msk & ((value) << TAL_DMACPUSEL0_CH14_Pos))
#define TAL_DMACPUSEL0_CH15_Pos     30           /**< \brief (TAL_DMACPUSEL0) DMA Channel 15 Interrupt CPU Select */
#define TAL_DMACPUSEL0_CH15_Msk     (_U_(0x1) << TAL_DMACPUSEL0_CH15_Pos)
#define TAL_DMACPUSEL0_CH15(value)  (TAL_DMACPUSEL0_CH15_Msk & ((value) << TAL_DMACPUSEL0_CH15_Pos))
#define TAL_DMACPUSEL0_MASK         _U_(0x55555555) /**< \brief (TAL_DMACPUSEL0) MASK Register */

/* -------- TAL_DMACPUSEL1 : (TAL Offset: 0x114) (R/W 32) DMA Channel Interrupts CPU Select 1 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CH16:1;           /*!< bit:      0  DMA Channel 16 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:      1  Reserved                           */
    uint32_t CH17:1;           /*!< bit:      2  DMA Channel 17 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:      3  Reserved                           */
    uint32_t CH18:1;           /*!< bit:      4  DMA Channel 18 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:      5  Reserved                           */
    uint32_t CH19:1;           /*!< bit:      6  DMA Channel 19 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:      7  Reserved                           */
    uint32_t CH20:1;           /*!< bit:      8  DMA Channel 20 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:      9  Reserved                           */
    uint32_t CH21:1;           /*!< bit:     10  DMA Channel 21 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     11  Reserved                           */
    uint32_t CH22:1;           /*!< bit:     12  DMA Channel 22 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     13  Reserved                           */
    uint32_t CH23:1;           /*!< bit:     14  DMA Channel 23 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     15  Reserved                           */
    uint32_t CH24:1;           /*!< bit:     16  DMA Channel 24 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     17  Reserved                           */
    uint32_t CH25:1;           /*!< bit:     18  DMA Channel 25 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     19  Reserved                           */
    uint32_t CH26:1;           /*!< bit:     20  DMA Channel 26 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     21  Reserved                           */
    uint32_t CH27:1;           /*!< bit:     22  DMA Channel 27 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     23  Reserved                           */
    uint32_t CH28:1;           /*!< bit:     24  DMA Channel 28 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     25  Reserved                           */
    uint32_t CH29:1;           /*!< bit:     26  DMA Channel 29 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     27  Reserved                           */
    uint32_t CH30:1;           /*!< bit:     28  DMA Channel 30 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     29  Reserved                           */
    uint32_t CH31:1;           /*!< bit:     30  DMA Channel 31 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TAL_DMACPUSEL1_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_DMACPUSEL1_OFFSET       0x114        /**< \brief (TAL_DMACPUSEL1 offset) DMA Channel Interrupts CPU Select 1 */
#define TAL_DMACPUSEL1_RESETVALUE   _U_(0x00000000) /**< \brief (TAL_DMACPUSEL1 reset_value) DMA Channel Interrupts CPU Select 1 */

#define TAL_DMACPUSEL1_CH16_Pos     0            /**< \brief (TAL_DMACPUSEL1) DMA Channel 16 Interrupt CPU Select */
#define TAL_DMACPUSEL1_CH16_Msk     (_U_(0x1) << TAL_DMACPUSEL1_CH16_Pos)
#define TAL_DMACPUSEL1_CH16(value)  (TAL_DMACPUSEL1_CH16_Msk & ((value) << TAL_DMACPUSEL1_CH16_Pos))
#define TAL_DMACPUSEL1_CH17_Pos     2            /**< \brief (TAL_DMACPUSEL1) DMA Channel 17 Interrupt CPU Select */
#define TAL_DMACPUSEL1_CH17_Msk     (_U_(0x1) << TAL_DMACPUSEL1_CH17_Pos)
#define TAL_DMACPUSEL1_CH17(value)  (TAL_DMACPUSEL1_CH17_Msk & ((value) << TAL_DMACPUSEL1_CH17_Pos))
#define TAL_DMACPUSEL1_CH18_Pos     4            /**< \brief (TAL_DMACPUSEL1) DMA Channel 18 Interrupt CPU Select */
#define TAL_DMACPUSEL1_CH18_Msk     (_U_(0x1) << TAL_DMACPUSEL1_CH18_Pos)
#define TAL_DMACPUSEL1_CH18(value)  (TAL_DMACPUSEL1_CH18_Msk & ((value) << TAL_DMACPUSEL1_CH18_Pos))
#define TAL_DMACPUSEL1_CH19_Pos     6            /**< \brief (TAL_DMACPUSEL1) DMA Channel 19 Interrupt CPU Select */
#define TAL_DMACPUSEL1_CH19_Msk     (_U_(0x1) << TAL_DMACPUSEL1_CH19_Pos)
#define TAL_DMACPUSEL1_CH19(value)  (TAL_DMACPUSEL1_CH19_Msk & ((value) << TAL_DMACPUSEL1_CH19_Pos))
#define TAL_DMACPUSEL1_CH20_Pos     8            /**< \brief (TAL_DMACPUSEL1) DMA Channel 20 Interrupt CPU Select */
#define TAL_DMACPUSEL1_CH20_Msk     (_U_(0x1) << TAL_DMACPUSEL1_CH20_Pos)
#define TAL_DMACPUSEL1_CH20(value)  (TAL_DMACPUSEL1_CH20_Msk & ((value) << TAL_DMACPUSEL1_CH20_Pos))
#define TAL_DMACPUSEL1_CH21_Pos     10           /**< \brief (TAL_DMACPUSEL1) DMA Channel 21 Interrupt CPU Select */
#define TAL_DMACPUSEL1_CH21_Msk     (_U_(0x1) << TAL_DMACPUSEL1_CH21_Pos)
#define TAL_DMACPUSEL1_CH21(value)  (TAL_DMACPUSEL1_CH21_Msk & ((value) << TAL_DMACPUSEL1_CH21_Pos))
#define TAL_DMACPUSEL1_CH22_Pos     12           /**< \brief (TAL_DMACPUSEL1) DMA Channel 22 Interrupt CPU Select */
#define TAL_DMACPUSEL1_CH22_Msk     (_U_(0x1) << TAL_DMACPUSEL1_CH22_Pos)
#define TAL_DMACPUSEL1_CH22(value)  (TAL_DMACPUSEL1_CH22_Msk & ((value) << TAL_DMACPUSEL1_CH22_Pos))
#define TAL_DMACPUSEL1_CH23_Pos     14           /**< \brief (TAL_DMACPUSEL1) DMA Channel 23 Interrupt CPU Select */
#define TAL_DMACPUSEL1_CH23_Msk     (_U_(0x1) << TAL_DMACPUSEL1_CH23_Pos)
#define TAL_DMACPUSEL1_CH23(value)  (TAL_DMACPUSEL1_CH23_Msk & ((value) << TAL_DMACPUSEL1_CH23_Pos))
#define TAL_DMACPUSEL1_CH24_Pos     16           /**< \brief (TAL_DMACPUSEL1) DMA Channel 24 Interrupt CPU Select */
#define TAL_DMACPUSEL1_CH24_Msk     (_U_(0x1) << TAL_DMACPUSEL1_CH24_Pos)
#define TAL_DMACPUSEL1_CH24(value)  (TAL_DMACPUSEL1_CH24_Msk & ((value) << TAL_DMACPUSEL1_CH24_Pos))
#define TAL_DMACPUSEL1_CH25_Pos     18           /**< \brief (TAL_DMACPUSEL1) DMA Channel 25 Interrupt CPU Select */
#define TAL_DMACPUSEL1_CH25_Msk     (_U_(0x1) << TAL_DMACPUSEL1_CH25_Pos)
#define TAL_DMACPUSEL1_CH25(value)  (TAL_DMACPUSEL1_CH25_Msk & ((value) << TAL_DMACPUSEL1_CH25_Pos))
#define TAL_DMACPUSEL1_CH26_Pos     20           /**< \brief (TAL_DMACPUSEL1) DMA Channel 26 Interrupt CPU Select */
#define TAL_DMACPUSEL1_CH26_Msk     (_U_(0x1) << TAL_DMACPUSEL1_CH26_Pos)
#define TAL_DMACPUSEL1_CH26(value)  (TAL_DMACPUSEL1_CH26_Msk & ((value) << TAL_DMACPUSEL1_CH26_Pos))
#define TAL_DMACPUSEL1_CH27_Pos     22           /**< \brief (TAL_DMACPUSEL1) DMA Channel 27 Interrupt CPU Select */
#define TAL_DMACPUSEL1_CH27_Msk     (_U_(0x1) << TAL_DMACPUSEL1_CH27_Pos)
#define TAL_DMACPUSEL1_CH27(value)  (TAL_DMACPUSEL1_CH27_Msk & ((value) << TAL_DMACPUSEL1_CH27_Pos))
#define TAL_DMACPUSEL1_CH28_Pos     24           /**< \brief (TAL_DMACPUSEL1) DMA Channel 28 Interrupt CPU Select */
#define TAL_DMACPUSEL1_CH28_Msk     (_U_(0x1) << TAL_DMACPUSEL1_CH28_Pos)
#define TAL_DMACPUSEL1_CH28(value)  (TAL_DMACPUSEL1_CH28_Msk & ((value) << TAL_DMACPUSEL1_CH28_Pos))
#define TAL_DMACPUSEL1_CH29_Pos     26           /**< \brief (TAL_DMACPUSEL1) DMA Channel 29 Interrupt CPU Select */
#define TAL_DMACPUSEL1_CH29_Msk     (_U_(0x1) << TAL_DMACPUSEL1_CH29_Pos)
#define TAL_DMACPUSEL1_CH29(value)  (TAL_DMACPUSEL1_CH29_Msk & ((value) << TAL_DMACPUSEL1_CH29_Pos))
#define TAL_DMACPUSEL1_CH30_Pos     28           /**< \brief (TAL_DMACPUSEL1) DMA Channel 30 Interrupt CPU Select */
#define TAL_DMACPUSEL1_CH30_Msk     (_U_(0x1) << TAL_DMACPUSEL1_CH30_Pos)
#define TAL_DMACPUSEL1_CH30(value)  (TAL_DMACPUSEL1_CH30_Msk & ((value) << TAL_DMACPUSEL1_CH30_Pos))
#define TAL_DMACPUSEL1_CH31_Pos     30           /**< \brief (TAL_DMACPUSEL1) DMA Channel 31 Interrupt CPU Select */
#define TAL_DMACPUSEL1_CH31_Msk     (_U_(0x1) << TAL_DMACPUSEL1_CH31_Pos)
#define TAL_DMACPUSEL1_CH31(value)  (TAL_DMACPUSEL1_CH31_Msk & ((value) << TAL_DMACPUSEL1_CH31_Pos))
#define TAL_DMACPUSEL1_MASK         _U_(0x55555555) /**< \brief (TAL_DMACPUSEL1) MASK Register */

/* -------- TAL_EVCPUSEL0 : (TAL Offset: 0x118) (R/W 32) EVSYS Channel Interrupts CPU Select 0 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CH0:1;            /*!< bit:      0  Event Channel 0 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:      1  Reserved                           */
    uint32_t CH1:1;            /*!< bit:      2  Event Channel 1 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:      3  Reserved                           */
    uint32_t CH2:1;            /*!< bit:      4  Event Channel 2 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:      5  Reserved                           */
    uint32_t CH3:1;            /*!< bit:      6  Event Channel 3 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:      7  Reserved                           */
    uint32_t CH4:1;            /*!< bit:      8  Event Channel 4 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:      9  Reserved                           */
    uint32_t CH5:1;            /*!< bit:     10  Event Channel 5 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     11  Reserved                           */
    uint32_t CH6:1;            /*!< bit:     12  Event Channel 6 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     13  Reserved                           */
    uint32_t CH7:1;            /*!< bit:     14  Event Channel 7 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     15  Reserved                           */
    uint32_t CH8:1;            /*!< bit:     16  Event Channel 8 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     17  Reserved                           */
    uint32_t CH9:1;            /*!< bit:     18  Event Channel 9 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     19  Reserved                           */
    uint32_t CH10:1;           /*!< bit:     20  Event Channel 10 Interrupt CPU Select */
    uint32_t :1;               /*!< bit:     21  Reserved                           */
    uint32_t CH11:1;           /*!< bit:     22  Event Channel 11 Interrupt CPU Select */
    uint32_t :9;               /*!< bit: 23..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TAL_EVCPUSEL0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_EVCPUSEL0_OFFSET        0x118        /**< \brief (TAL_EVCPUSEL0 offset) EVSYS Channel Interrupts CPU Select 0 */
#define TAL_EVCPUSEL0_RESETVALUE    _U_(0x00000000) /**< \brief (TAL_EVCPUSEL0 reset_value) EVSYS Channel Interrupts CPU Select 0 */

#define TAL_EVCPUSEL0_CH0_Pos       0            /**< \brief (TAL_EVCPUSEL0) Event Channel 0 Interrupt CPU Select */
#define TAL_EVCPUSEL0_CH0_Msk       (_U_(0x1) << TAL_EVCPUSEL0_CH0_Pos)
#define TAL_EVCPUSEL0_CH0(value)    (TAL_EVCPUSEL0_CH0_Msk & ((value) << TAL_EVCPUSEL0_CH0_Pos))
#define TAL_EVCPUSEL0_CH1_Pos       2            /**< \brief (TAL_EVCPUSEL0) Event Channel 1 Interrupt CPU Select */
#define TAL_EVCPUSEL0_CH1_Msk       (_U_(0x1) << TAL_EVCPUSEL0_CH1_Pos)
#define TAL_EVCPUSEL0_CH1(value)    (TAL_EVCPUSEL0_CH1_Msk & ((value) << TAL_EVCPUSEL0_CH1_Pos))
#define TAL_EVCPUSEL0_CH2_Pos       4            /**< \brief (TAL_EVCPUSEL0) Event Channel 2 Interrupt CPU Select */
#define TAL_EVCPUSEL0_CH2_Msk       (_U_(0x1) << TAL_EVCPUSEL0_CH2_Pos)
#define TAL_EVCPUSEL0_CH2(value)    (TAL_EVCPUSEL0_CH2_Msk & ((value) << TAL_EVCPUSEL0_CH2_Pos))
#define TAL_EVCPUSEL0_CH3_Pos       6            /**< \brief (TAL_EVCPUSEL0) Event Channel 3 Interrupt CPU Select */
#define TAL_EVCPUSEL0_CH3_Msk       (_U_(0x1) << TAL_EVCPUSEL0_CH3_Pos)
#define TAL_EVCPUSEL0_CH3(value)    (TAL_EVCPUSEL0_CH3_Msk & ((value) << TAL_EVCPUSEL0_CH3_Pos))
#define TAL_EVCPUSEL0_CH4_Pos       8            /**< \brief (TAL_EVCPUSEL0) Event Channel 4 Interrupt CPU Select */
#define TAL_EVCPUSEL0_CH4_Msk       (_U_(0x1) << TAL_EVCPUSEL0_CH4_Pos)
#define TAL_EVCPUSEL0_CH4(value)    (TAL_EVCPUSEL0_CH4_Msk & ((value) << TAL_EVCPUSEL0_CH4_Pos))
#define TAL_EVCPUSEL0_CH5_Pos       10           /**< \brief (TAL_EVCPUSEL0) Event Channel 5 Interrupt CPU Select */
#define TAL_EVCPUSEL0_CH5_Msk       (_U_(0x1) << TAL_EVCPUSEL0_CH5_Pos)
#define TAL_EVCPUSEL0_CH5(value)    (TAL_EVCPUSEL0_CH5_Msk & ((value) << TAL_EVCPUSEL0_CH5_Pos))
#define TAL_EVCPUSEL0_CH6_Pos       12           /**< \brief (TAL_EVCPUSEL0) Event Channel 6 Interrupt CPU Select */
#define TAL_EVCPUSEL0_CH6_Msk       (_U_(0x1) << TAL_EVCPUSEL0_CH6_Pos)
#define TAL_EVCPUSEL0_CH6(value)    (TAL_EVCPUSEL0_CH6_Msk & ((value) << TAL_EVCPUSEL0_CH6_Pos))
#define TAL_EVCPUSEL0_CH7_Pos       14           /**< \brief (TAL_EVCPUSEL0) Event Channel 7 Interrupt CPU Select */
#define TAL_EVCPUSEL0_CH7_Msk       (_U_(0x1) << TAL_EVCPUSEL0_CH7_Pos)
#define TAL_EVCPUSEL0_CH7(value)    (TAL_EVCPUSEL0_CH7_Msk & ((value) << TAL_EVCPUSEL0_CH7_Pos))
#define TAL_EVCPUSEL0_CH8_Pos       16           /**< \brief (TAL_EVCPUSEL0) Event Channel 8 Interrupt CPU Select */
#define TAL_EVCPUSEL0_CH8_Msk       (_U_(0x1) << TAL_EVCPUSEL0_CH8_Pos)
#define TAL_EVCPUSEL0_CH8(value)    (TAL_EVCPUSEL0_CH8_Msk & ((value) << TAL_EVCPUSEL0_CH8_Pos))
#define TAL_EVCPUSEL0_CH9_Pos       18           /**< \brief (TAL_EVCPUSEL0) Event Channel 9 Interrupt CPU Select */
#define TAL_EVCPUSEL0_CH9_Msk       (_U_(0x1) << TAL_EVCPUSEL0_CH9_Pos)
#define TAL_EVCPUSEL0_CH9(value)    (TAL_EVCPUSEL0_CH9_Msk & ((value) << TAL_EVCPUSEL0_CH9_Pos))
#define TAL_EVCPUSEL0_CH10_Pos      20           /**< \brief (TAL_EVCPUSEL0) Event Channel 10 Interrupt CPU Select */
#define TAL_EVCPUSEL0_CH10_Msk      (_U_(0x1) << TAL_EVCPUSEL0_CH10_Pos)
#define TAL_EVCPUSEL0_CH10(value)   (TAL_EVCPUSEL0_CH10_Msk & ((value) << TAL_EVCPUSEL0_CH10_Pos))
#define TAL_EVCPUSEL0_CH11_Pos      22           /**< \brief (TAL_EVCPUSEL0) Event Channel 11 Interrupt CPU Select */
#define TAL_EVCPUSEL0_CH11_Msk      (_U_(0x1) << TAL_EVCPUSEL0_CH11_Pos)
#define TAL_EVCPUSEL0_CH11(value)   (TAL_EVCPUSEL0_CH11_Msk & ((value) << TAL_EVCPUSEL0_CH11_Pos))
#define TAL_EVCPUSEL0_MASK          _U_(0x00555555) /**< \brief (TAL_EVCPUSEL0) MASK Register */

/* -------- TAL_EICCPUSEL0 : (TAL Offset: 0x120) (R/W 32) EIC External Interrupts CPU Select 0 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EXTINT0:1;        /*!< bit:      0  External Interrupt 0 CPU Select    */
    uint32_t :1;               /*!< bit:      1  Reserved                           */
    uint32_t EXTINT1:1;        /*!< bit:      2  External Interrupt 1 CPU Select    */
    uint32_t :1;               /*!< bit:      3  Reserved                           */
    uint32_t EXTINT2:1;        /*!< bit:      4  External Interrupt 2 CPU Select    */
    uint32_t :1;               /*!< bit:      5  Reserved                           */
    uint32_t EXTINT3:1;        /*!< bit:      6  External Interrupt 3 CPU Select    */
    uint32_t :1;               /*!< bit:      7  Reserved                           */
    uint32_t EXTINT4:1;        /*!< bit:      8  External Interrupt 4 CPU Select    */
    uint32_t :1;               /*!< bit:      9  Reserved                           */
    uint32_t EXTINT5:1;        /*!< bit:     10  External Interrupt 5 CPU Select    */
    uint32_t :1;               /*!< bit:     11  Reserved                           */
    uint32_t EXTINT6:1;        /*!< bit:     12  External Interrupt 6 CPU Select    */
    uint32_t :1;               /*!< bit:     13  Reserved                           */
    uint32_t EXTINT7:1;        /*!< bit:     14  External Interrupt 7 CPU Select    */
    uint32_t :1;               /*!< bit:     15  Reserved                           */
    uint32_t EXTINT8:1;        /*!< bit:     16  External Interrupt 8 CPU Select    */
    uint32_t :1;               /*!< bit:     17  Reserved                           */
    uint32_t EXTINT9:1;        /*!< bit:     18  External Interrupt 9 CPU Select    */
    uint32_t :1;               /*!< bit:     19  Reserved                           */
    uint32_t EXTINT10:1;       /*!< bit:     20  External Interrupt 10 CPU Select   */
    uint32_t :1;               /*!< bit:     21  Reserved                           */
    uint32_t EXTINT11:1;       /*!< bit:     22  External Interrupt 11 CPU Select   */
    uint32_t :1;               /*!< bit:     23  Reserved                           */
    uint32_t EXTINT12:1;       /*!< bit:     24  External Interrupt 12 CPU Select   */
    uint32_t :1;               /*!< bit:     25  Reserved                           */
    uint32_t EXTINT13:1;       /*!< bit:     26  External Interrupt 13 CPU Select   */
    uint32_t :1;               /*!< bit:     27  Reserved                           */
    uint32_t EXTINT14:1;       /*!< bit:     28  External Interrupt 14 CPU Select   */
    uint32_t :1;               /*!< bit:     29  Reserved                           */
    uint32_t EXTINT15:1;       /*!< bit:     30  External Interrupt 15 CPU Select   */
    uint32_t :1;               /*!< bit:     31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TAL_EICCPUSEL0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_EICCPUSEL0_OFFSET       0x120        /**< \brief (TAL_EICCPUSEL0 offset) EIC External Interrupts CPU Select 0 */
#define TAL_EICCPUSEL0_RESETVALUE   _U_(0x00000000) /**< \brief (TAL_EICCPUSEL0 reset_value) EIC External Interrupts CPU Select 0 */

#define TAL_EICCPUSEL0_EXTINT0_Pos  0            /**< \brief (TAL_EICCPUSEL0) External Interrupt 0 CPU Select */
#define TAL_EICCPUSEL0_EXTINT0_Msk  (_U_(0x1) << TAL_EICCPUSEL0_EXTINT0_Pos)
#define TAL_EICCPUSEL0_EXTINT0(value) (TAL_EICCPUSEL0_EXTINT0_Msk & ((value) << TAL_EICCPUSEL0_EXTINT0_Pos))
#define TAL_EICCPUSEL0_EXTINT1_Pos  2            /**< \brief (TAL_EICCPUSEL0) External Interrupt 1 CPU Select */
#define TAL_EICCPUSEL0_EXTINT1_Msk  (_U_(0x1) << TAL_EICCPUSEL0_EXTINT1_Pos)
#define TAL_EICCPUSEL0_EXTINT1(value) (TAL_EICCPUSEL0_EXTINT1_Msk & ((value) << TAL_EICCPUSEL0_EXTINT1_Pos))
#define TAL_EICCPUSEL0_EXTINT2_Pos  4            /**< \brief (TAL_EICCPUSEL0) External Interrupt 2 CPU Select */
#define TAL_EICCPUSEL0_EXTINT2_Msk  (_U_(0x1) << TAL_EICCPUSEL0_EXTINT2_Pos)
#define TAL_EICCPUSEL0_EXTINT2(value) (TAL_EICCPUSEL0_EXTINT2_Msk & ((value) << TAL_EICCPUSEL0_EXTINT2_Pos))
#define TAL_EICCPUSEL0_EXTINT3_Pos  6            /**< \brief (TAL_EICCPUSEL0) External Interrupt 3 CPU Select */
#define TAL_EICCPUSEL0_EXTINT3_Msk  (_U_(0x1) << TAL_EICCPUSEL0_EXTINT3_Pos)
#define TAL_EICCPUSEL0_EXTINT3(value) (TAL_EICCPUSEL0_EXTINT3_Msk & ((value) << TAL_EICCPUSEL0_EXTINT3_Pos))
#define TAL_EICCPUSEL0_EXTINT4_Pos  8            /**< \brief (TAL_EICCPUSEL0) External Interrupt 4 CPU Select */
#define TAL_EICCPUSEL0_EXTINT4_Msk  (_U_(0x1) << TAL_EICCPUSEL0_EXTINT4_Pos)
#define TAL_EICCPUSEL0_EXTINT4(value) (TAL_EICCPUSEL0_EXTINT4_Msk & ((value) << TAL_EICCPUSEL0_EXTINT4_Pos))
#define TAL_EICCPUSEL0_EXTINT5_Pos  10           /**< \brief (TAL_EICCPUSEL0) External Interrupt 5 CPU Select */
#define TAL_EICCPUSEL0_EXTINT5_Msk  (_U_(0x1) << TAL_EICCPUSEL0_EXTINT5_Pos)
#define TAL_EICCPUSEL0_EXTINT5(value) (TAL_EICCPUSEL0_EXTINT5_Msk & ((value) << TAL_EICCPUSEL0_EXTINT5_Pos))
#define TAL_EICCPUSEL0_EXTINT6_Pos  12           /**< \brief (TAL_EICCPUSEL0) External Interrupt 6 CPU Select */
#define TAL_EICCPUSEL0_EXTINT6_Msk  (_U_(0x1) << TAL_EICCPUSEL0_EXTINT6_Pos)
#define TAL_EICCPUSEL0_EXTINT6(value) (TAL_EICCPUSEL0_EXTINT6_Msk & ((value) << TAL_EICCPUSEL0_EXTINT6_Pos))
#define TAL_EICCPUSEL0_EXTINT7_Pos  14           /**< \brief (TAL_EICCPUSEL0) External Interrupt 7 CPU Select */
#define TAL_EICCPUSEL0_EXTINT7_Msk  (_U_(0x1) << TAL_EICCPUSEL0_EXTINT7_Pos)
#define TAL_EICCPUSEL0_EXTINT7(value) (TAL_EICCPUSEL0_EXTINT7_Msk & ((value) << TAL_EICCPUSEL0_EXTINT7_Pos))
#define TAL_EICCPUSEL0_EXTINT8_Pos  16           /**< \brief (TAL_EICCPUSEL0) External Interrupt 8 CPU Select */
#define TAL_EICCPUSEL0_EXTINT8_Msk  (_U_(0x1) << TAL_EICCPUSEL0_EXTINT8_Pos)
#define TAL_EICCPUSEL0_EXTINT8(value) (TAL_EICCPUSEL0_EXTINT8_Msk & ((value) << TAL_EICCPUSEL0_EXTINT8_Pos))
#define TAL_EICCPUSEL0_EXTINT9_Pos  18           /**< \brief (TAL_EICCPUSEL0) External Interrupt 9 CPU Select */
#define TAL_EICCPUSEL0_EXTINT9_Msk  (_U_(0x1) << TAL_EICCPUSEL0_EXTINT9_Pos)
#define TAL_EICCPUSEL0_EXTINT9(value) (TAL_EICCPUSEL0_EXTINT9_Msk & ((value) << TAL_EICCPUSEL0_EXTINT9_Pos))
#define TAL_EICCPUSEL0_EXTINT10_Pos 20           /**< \brief (TAL_EICCPUSEL0) External Interrupt 10 CPU Select */
#define TAL_EICCPUSEL0_EXTINT10_Msk (_U_(0x1) << TAL_EICCPUSEL0_EXTINT10_Pos)
#define TAL_EICCPUSEL0_EXTINT10(value) (TAL_EICCPUSEL0_EXTINT10_Msk & ((value) << TAL_EICCPUSEL0_EXTINT10_Pos))
#define TAL_EICCPUSEL0_EXTINT11_Pos 22           /**< \brief (TAL_EICCPUSEL0) External Interrupt 11 CPU Select */
#define TAL_EICCPUSEL0_EXTINT11_Msk (_U_(0x1) << TAL_EICCPUSEL0_EXTINT11_Pos)
#define TAL_EICCPUSEL0_EXTINT11(value) (TAL_EICCPUSEL0_EXTINT11_Msk & ((value) << TAL_EICCPUSEL0_EXTINT11_Pos))
#define TAL_EICCPUSEL0_EXTINT12_Pos 24           /**< \brief (TAL_EICCPUSEL0) External Interrupt 12 CPU Select */
#define TAL_EICCPUSEL0_EXTINT12_Msk (_U_(0x1) << TAL_EICCPUSEL0_EXTINT12_Pos)
#define TAL_EICCPUSEL0_EXTINT12(value) (TAL_EICCPUSEL0_EXTINT12_Msk & ((value) << TAL_EICCPUSEL0_EXTINT12_Pos))
#define TAL_EICCPUSEL0_EXTINT13_Pos 26           /**< \brief (TAL_EICCPUSEL0) External Interrupt 13 CPU Select */
#define TAL_EICCPUSEL0_EXTINT13_Msk (_U_(0x1) << TAL_EICCPUSEL0_EXTINT13_Pos)
#define TAL_EICCPUSEL0_EXTINT13(value) (TAL_EICCPUSEL0_EXTINT13_Msk & ((value) << TAL_EICCPUSEL0_EXTINT13_Pos))
#define TAL_EICCPUSEL0_EXTINT14_Pos 28           /**< \brief (TAL_EICCPUSEL0) External Interrupt 14 CPU Select */
#define TAL_EICCPUSEL0_EXTINT14_Msk (_U_(0x1) << TAL_EICCPUSEL0_EXTINT14_Pos)
#define TAL_EICCPUSEL0_EXTINT14(value) (TAL_EICCPUSEL0_EXTINT14_Msk & ((value) << TAL_EICCPUSEL0_EXTINT14_Pos))
#define TAL_EICCPUSEL0_EXTINT15_Pos 30           /**< \brief (TAL_EICCPUSEL0) External Interrupt 15 CPU Select */
#define TAL_EICCPUSEL0_EXTINT15_Msk (_U_(0x1) << TAL_EICCPUSEL0_EXTINT15_Pos)
#define TAL_EICCPUSEL0_EXTINT15(value) (TAL_EICCPUSEL0_EXTINT15_Msk & ((value) << TAL_EICCPUSEL0_EXTINT15_Pos))
#define TAL_EICCPUSEL0_MASK         _U_(0x55555555) /**< \brief (TAL_EICCPUSEL0) MASK Register */

/* -------- TAL_INTCPUSEL0 : (TAL Offset: 0x128) (R/W 32) Interrupts CPU Select 0 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PAC:1;            /*!< bit:      0  PAC Interrupt CPU Select           */
    uint32_t :1;               /*!< bit:      1  Reserved                           */
    uint32_t PM:1;             /*!< bit:      2  PM Interrupt CPU Select            */
    uint32_t :1;               /*!< bit:      3  Reserved                           */
    uint32_t MCLK:1;           /*!< bit:      4  MCLK Interrupt CPU Select          */
    uint32_t :3;               /*!< bit:  5.. 7  Reserved                           */
    uint32_t OSCCTRL:1;        /*!< bit:      8  OSCCTRL Interrupt CPU Select       */
    uint32_t :1;               /*!< bit:      9  Reserved                           */
    uint32_t OSC32KCTRL:1;     /*!< bit:     10  OSC32KCTRL Interrupt CPU Select    */
    uint32_t :1;               /*!< bit:     11  Reserved                           */
    uint32_t SUPC:1;           /*!< bit:     12  SUPC Interrupt CPU Select          */
    uint32_t :3;               /*!< bit: 13..15  Reserved                           */
    uint32_t WDT:1;            /*!< bit:     16  WDT Interrupt CPU Select           */
    uint32_t :1;               /*!< bit:     17  Reserved                           */
    uint32_t RTC:1;            /*!< bit:     18  RTC Interrupt CPU Select           */
    uint32_t :1;               /*!< bit:     19  Reserved                           */
    uint32_t EIC:1;            /*!< bit:     20  EIC Interrupt CPU Select           */
    uint32_t :1;               /*!< bit:     21  Reserved                           */
    uint32_t FREQM:1;          /*!< bit:     22  FREQM Interrupt CPU Select         */
    uint32_t :1;               /*!< bit:     23  Reserved                           */
    uint32_t SERCOM0:1;        /*!< bit:     24  SERCOM0 Interrupt CPU Select       */
    uint32_t :1;               /*!< bit:     25  Reserved                           */
    uint32_t SERCOM1:1;        /*!< bit:     26  SERCOM1 Interrupt CPU Select       */
    uint32_t :1;               /*!< bit:     27  Reserved                           */
    uint32_t TC0:1;            /*!< bit:     28  TC0 Interrupt CPU Select           */
    uint32_t :1;               /*!< bit:     29  Reserved                           */
    uint32_t TC1:1;            /*!< bit:     30  TC1 Interrupt CPU Select           */
    uint32_t :1;               /*!< bit:     31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TAL_INTCPUSEL0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_INTCPUSEL0_OFFSET       0x128        /**< \brief (TAL_INTCPUSEL0 offset) Interrupts CPU Select 0 */
#define TAL_INTCPUSEL0_RESETVALUE   _U_(0x00000000) /**< \brief (TAL_INTCPUSEL0 reset_value) Interrupts CPU Select 0 */

#define TAL_INTCPUSEL0_PAC_Pos      0            /**< \brief (TAL_INTCPUSEL0) PAC Interrupt CPU Select */
#define TAL_INTCPUSEL0_PAC_Msk      (_U_(0x1) << TAL_INTCPUSEL0_PAC_Pos)
#define TAL_INTCPUSEL0_PAC(value)   (TAL_INTCPUSEL0_PAC_Msk & ((value) << TAL_INTCPUSEL0_PAC_Pos))
#define TAL_INTCPUSEL0_PM_Pos       2            /**< \brief (TAL_INTCPUSEL0) PM Interrupt CPU Select */
#define TAL_INTCPUSEL0_PM_Msk       (_U_(0x1) << TAL_INTCPUSEL0_PM_Pos)
#define TAL_INTCPUSEL0_PM(value)    (TAL_INTCPUSEL0_PM_Msk & ((value) << TAL_INTCPUSEL0_PM_Pos))
#define TAL_INTCPUSEL0_MCLK_Pos     4            /**< \brief (TAL_INTCPUSEL0) MCLK Interrupt CPU Select */
#define TAL_INTCPUSEL0_MCLK_Msk     (_U_(0x1) << TAL_INTCPUSEL0_MCLK_Pos)
#define TAL_INTCPUSEL0_MCLK(value)  (TAL_INTCPUSEL0_MCLK_Msk & ((value) << TAL_INTCPUSEL0_MCLK_Pos))
#define TAL_INTCPUSEL0_OSCCTRL_Pos  8            /**< \brief (TAL_INTCPUSEL0) OSCCTRL Interrupt CPU Select */
#define TAL_INTCPUSEL0_OSCCTRL_Msk  (_U_(0x1) << TAL_INTCPUSEL0_OSCCTRL_Pos)
#define TAL_INTCPUSEL0_OSCCTRL(value) (TAL_INTCPUSEL0_OSCCTRL_Msk & ((value) << TAL_INTCPUSEL0_OSCCTRL_Pos))
#define TAL_INTCPUSEL0_OSC32KCTRL_Pos 10           /**< \brief (TAL_INTCPUSEL0) OSC32KCTRL Interrupt CPU Select */
#define TAL_INTCPUSEL0_OSC32KCTRL_Msk (_U_(0x1) << TAL_INTCPUSEL0_OSC32KCTRL_Pos)
#define TAL_INTCPUSEL0_OSC32KCTRL(value) (TAL_INTCPUSEL0_OSC32KCTRL_Msk & ((value) << TAL_INTCPUSEL0_OSC32KCTRL_Pos))
#define TAL_INTCPUSEL0_SUPC_Pos     12           /**< \brief (TAL_INTCPUSEL0) SUPC Interrupt CPU Select */
#define TAL_INTCPUSEL0_SUPC_Msk     (_U_(0x1) << TAL_INTCPUSEL0_SUPC_Pos)
#define TAL_INTCPUSEL0_SUPC(value)  (TAL_INTCPUSEL0_SUPC_Msk & ((value) << TAL_INTCPUSEL0_SUPC_Pos))
#define TAL_INTCPUSEL0_WDT_Pos      16           /**< \brief (TAL_INTCPUSEL0) WDT Interrupt CPU Select */
#define TAL_INTCPUSEL0_WDT_Msk      (_U_(0x1) << TAL_INTCPUSEL0_WDT_Pos)
#define TAL_INTCPUSEL0_WDT(value)   (TAL_INTCPUSEL0_WDT_Msk & ((value) << TAL_INTCPUSEL0_WDT_Pos))
#define TAL_INTCPUSEL0_RTC_Pos      18           /**< \brief (TAL_INTCPUSEL0) RTC Interrupt CPU Select */
#define TAL_INTCPUSEL0_RTC_Msk      (_U_(0x1) << TAL_INTCPUSEL0_RTC_Pos)
#define TAL_INTCPUSEL0_RTC(value)   (TAL_INTCPUSEL0_RTC_Msk & ((value) << TAL_INTCPUSEL0_RTC_Pos))
#define TAL_INTCPUSEL0_EIC_Pos      20           /**< \brief (TAL_INTCPUSEL0) EIC Interrupt CPU Select */
#define TAL_INTCPUSEL0_EIC_Msk      (_U_(0x1) << TAL_INTCPUSEL0_EIC_Pos)
#define TAL_INTCPUSEL0_EIC(value)   (TAL_INTCPUSEL0_EIC_Msk & ((value) << TAL_INTCPUSEL0_EIC_Pos))
#define TAL_INTCPUSEL0_FREQM_Pos    22           /**< \brief (TAL_INTCPUSEL0) FREQM Interrupt CPU Select */
#define TAL_INTCPUSEL0_FREQM_Msk    (_U_(0x1) << TAL_INTCPUSEL0_FREQM_Pos)
#define TAL_INTCPUSEL0_FREQM(value) (TAL_INTCPUSEL0_FREQM_Msk & ((value) << TAL_INTCPUSEL0_FREQM_Pos))
#define TAL_INTCPUSEL0_SERCOM0_Pos  24           /**< \brief (TAL_INTCPUSEL0) SERCOM0 Interrupt CPU Select */
#define TAL_INTCPUSEL0_SERCOM0_Msk  (_U_(0x1) << TAL_INTCPUSEL0_SERCOM0_Pos)
#define TAL_INTCPUSEL0_SERCOM0(value) (TAL_INTCPUSEL0_SERCOM0_Msk & ((value) << TAL_INTCPUSEL0_SERCOM0_Pos))
#define TAL_INTCPUSEL0_SERCOM1_Pos  26           /**< \brief (TAL_INTCPUSEL0) SERCOM1 Interrupt CPU Select */
#define TAL_INTCPUSEL0_SERCOM1_Msk  (_U_(0x1) << TAL_INTCPUSEL0_SERCOM1_Pos)
#define TAL_INTCPUSEL0_SERCOM1(value) (TAL_INTCPUSEL0_SERCOM1_Msk & ((value) << TAL_INTCPUSEL0_SERCOM1_Pos))
#define TAL_INTCPUSEL0_TC0_Pos      28           /**< \brief (TAL_INTCPUSEL0) TC0 Interrupt CPU Select */
#define TAL_INTCPUSEL0_TC0_Msk      (_U_(0x1) << TAL_INTCPUSEL0_TC0_Pos)
#define TAL_INTCPUSEL0_TC0(value)   (TAL_INTCPUSEL0_TC0_Msk & ((value) << TAL_INTCPUSEL0_TC0_Pos))
#define TAL_INTCPUSEL0_TC1_Pos      30           /**< \brief (TAL_INTCPUSEL0) TC1 Interrupt CPU Select */
#define TAL_INTCPUSEL0_TC1_Msk      (_U_(0x1) << TAL_INTCPUSEL0_TC1_Pos)
#define TAL_INTCPUSEL0_TC1(value)   (TAL_INTCPUSEL0_TC1_Msk & ((value) << TAL_INTCPUSEL0_TC1_Pos))
#define TAL_INTCPUSEL0_MASK         _U_(0x55551515) /**< \brief (TAL_INTCPUSEL0) MASK Register */

/* -------- TAL_INTCPUSEL1 : (TAL Offset: 0x12C) (R/W 32) Interrupts CPU Select 1 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} TAL_INTCPUSEL1_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_INTCPUSEL1_OFFSET       0x12C        /**< \brief (TAL_INTCPUSEL1 offset) Interrupts CPU Select 1 */
#define TAL_INTCPUSEL1_RESETVALUE   _U_(0x00000000) /**< \brief (TAL_INTCPUSEL1 reset_value) Interrupts CPU Select 1 */
#define TAL_INTCPUSEL1_MASK         _U_(0x00000000) /**< \brief (TAL_INTCPUSEL1) MASK Register */

/* -------- TAL_INTCPUSEL2 : (TAL Offset: 0x130) (R/W 32) Interrupts CPU Select 2 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t USB:1;            /*!< bit:      0  USB Interrupt CPU Select           */
    uint32_t :3;               /*!< bit:  1.. 3  Reserved                           */
    uint32_t NVMCTRL:1;        /*!< bit:      4  NVMCTRL Interrupt CPU Select       */
    uint32_t :5;               /*!< bit:  5.. 9  Reserved                           */
    uint32_t DMAC:1;           /*!< bit:     10  DMAC Interrupt CPU Select          */
    uint32_t :3;               /*!< bit: 11..13  Reserved                           */
    uint32_t EVSYS:1;          /*!< bit:     14  EVSYS Interrupt CPU Select         */
    uint32_t :1;               /*!< bit:     15  Reserved                           */
    uint32_t PICOP:1;          /*!< bit:     16  PICOP Interrupt CPU Select         */
    uint32_t :1;               /*!< bit:     17  Reserved                           */
    uint32_t SERCOM2:1;        /*!< bit:     18  SERCOM2 Interrupt CPU Select       */
    uint32_t :1;               /*!< bit:     19  Reserved                           */
    uint32_t SERCOM3:1;        /*!< bit:     20  SERCOM3 Interrupt CPU Select       */
    uint32_t :1;               /*!< bit:     21  Reserved                           */
    uint32_t TCC0:1;           /*!< bit:     22  TCC0 Interrupt CPU Select          */
    uint32_t :1;               /*!< bit:     23  Reserved                           */
    uint32_t TCC1:1;           /*!< bit:     24  TCC1 Interrupt CPU Select          */
    uint32_t :1;               /*!< bit:     25  Reserved                           */
    uint32_t TC2:1;            /*!< bit:     26  TC2 Interrupt CPU Select           */
    uint32_t :1;               /*!< bit:     27  Reserved                           */
    uint32_t TC3:1;            /*!< bit:     28  TC3 Interrupt CPU Select           */
    uint32_t :1;               /*!< bit:     29  Reserved                           */
    uint32_t TAL:1;            /*!< bit:     30  TAL Interrupt CPU Select           */
    uint32_t :1;               /*!< bit:     31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TAL_INTCPUSEL2_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_INTCPUSEL2_OFFSET       0x130        /**< \brief (TAL_INTCPUSEL2 offset) Interrupts CPU Select 2 */
#define TAL_INTCPUSEL2_RESETVALUE   _U_(0x00000000) /**< \brief (TAL_INTCPUSEL2 reset_value) Interrupts CPU Select 2 */

#define TAL_INTCPUSEL2_USB_Pos      0            /**< \brief (TAL_INTCPUSEL2) USB Interrupt CPU Select */
#define TAL_INTCPUSEL2_USB_Msk      (_U_(0x1) << TAL_INTCPUSEL2_USB_Pos)
#define TAL_INTCPUSEL2_USB(value)   (TAL_INTCPUSEL2_USB_Msk & ((value) << TAL_INTCPUSEL2_USB_Pos))
#define TAL_INTCPUSEL2_NVMCTRL_Pos  4            /**< \brief (TAL_INTCPUSEL2) NVMCTRL Interrupt CPU Select */
#define TAL_INTCPUSEL2_NVMCTRL_Msk  (_U_(0x1) << TAL_INTCPUSEL2_NVMCTRL_Pos)
#define TAL_INTCPUSEL2_NVMCTRL(value) (TAL_INTCPUSEL2_NVMCTRL_Msk & ((value) << TAL_INTCPUSEL2_NVMCTRL_Pos))
#define TAL_INTCPUSEL2_DMAC_Pos     10           /**< \brief (TAL_INTCPUSEL2) DMAC Interrupt CPU Select */
#define TAL_INTCPUSEL2_DMAC_Msk     (_U_(0x1) << TAL_INTCPUSEL2_DMAC_Pos)
#define TAL_INTCPUSEL2_DMAC(value)  (TAL_INTCPUSEL2_DMAC_Msk & ((value) << TAL_INTCPUSEL2_DMAC_Pos))
#define TAL_INTCPUSEL2_EVSYS_Pos    14           /**< \brief (TAL_INTCPUSEL2) EVSYS Interrupt CPU Select */
#define TAL_INTCPUSEL2_EVSYS_Msk    (_U_(0x1) << TAL_INTCPUSEL2_EVSYS_Pos)
#define TAL_INTCPUSEL2_EVSYS(value) (TAL_INTCPUSEL2_EVSYS_Msk & ((value) << TAL_INTCPUSEL2_EVSYS_Pos))
#define TAL_INTCPUSEL2_PICOP_Pos    16           /**< \brief (TAL_INTCPUSEL2) PICOP Interrupt CPU Select */
#define TAL_INTCPUSEL2_PICOP_Msk    (_U_(0x1) << TAL_INTCPUSEL2_PICOP_Pos)
#define TAL_INTCPUSEL2_PICOP(value) (TAL_INTCPUSEL2_PICOP_Msk & ((value) << TAL_INTCPUSEL2_PICOP_Pos))
#define TAL_INTCPUSEL2_SERCOM2_Pos  18           /**< \brief (TAL_INTCPUSEL2) SERCOM2 Interrupt CPU Select */
#define TAL_INTCPUSEL2_SERCOM2_Msk  (_U_(0x1) << TAL_INTCPUSEL2_SERCOM2_Pos)
#define TAL_INTCPUSEL2_SERCOM2(value) (TAL_INTCPUSEL2_SERCOM2_Msk & ((value) << TAL_INTCPUSEL2_SERCOM2_Pos))
#define TAL_INTCPUSEL2_SERCOM3_Pos  20           /**< \brief (TAL_INTCPUSEL2) SERCOM3 Interrupt CPU Select */
#define TAL_INTCPUSEL2_SERCOM3_Msk  (_U_(0x1) << TAL_INTCPUSEL2_SERCOM3_Pos)
#define TAL_INTCPUSEL2_SERCOM3(value) (TAL_INTCPUSEL2_SERCOM3_Msk & ((value) << TAL_INTCPUSEL2_SERCOM3_Pos))
#define TAL_INTCPUSEL2_TCC0_Pos     22           /**< \brief (TAL_INTCPUSEL2) TCC0 Interrupt CPU Select */
#define TAL_INTCPUSEL2_TCC0_Msk     (_U_(0x1) << TAL_INTCPUSEL2_TCC0_Pos)
#define TAL_INTCPUSEL2_TCC0(value)  (TAL_INTCPUSEL2_TCC0_Msk & ((value) << TAL_INTCPUSEL2_TCC0_Pos))
#define TAL_INTCPUSEL2_TCC1_Pos     24           /**< \brief (TAL_INTCPUSEL2) TCC1 Interrupt CPU Select */
#define TAL_INTCPUSEL2_TCC1_Msk     (_U_(0x1) << TAL_INTCPUSEL2_TCC1_Pos)
#define TAL_INTCPUSEL2_TCC1(value)  (TAL_INTCPUSEL2_TCC1_Msk & ((value) << TAL_INTCPUSEL2_TCC1_Pos))
#define TAL_INTCPUSEL2_TC2_Pos      26           /**< \brief (TAL_INTCPUSEL2) TC2 Interrupt CPU Select */
#define TAL_INTCPUSEL2_TC2_Msk      (_U_(0x1) << TAL_INTCPUSEL2_TC2_Pos)
#define TAL_INTCPUSEL2_TC2(value)   (TAL_INTCPUSEL2_TC2_Msk & ((value) << TAL_INTCPUSEL2_TC2_Pos))
#define TAL_INTCPUSEL2_TC3_Pos      28           /**< \brief (TAL_INTCPUSEL2) TC3 Interrupt CPU Select */
#define TAL_INTCPUSEL2_TC3_Msk      (_U_(0x1) << TAL_INTCPUSEL2_TC3_Pos)
#define TAL_INTCPUSEL2_TC3(value)   (TAL_INTCPUSEL2_TC3_Msk & ((value) << TAL_INTCPUSEL2_TC3_Pos))
#define TAL_INTCPUSEL2_TAL_Pos      30           /**< \brief (TAL_INTCPUSEL2) TAL Interrupt CPU Select */
#define TAL_INTCPUSEL2_TAL_Msk      (_U_(0x1) << TAL_INTCPUSEL2_TAL_Pos)
#define TAL_INTCPUSEL2_TAL(value)   (TAL_INTCPUSEL2_TAL_Msk & ((value) << TAL_INTCPUSEL2_TAL_Pos))
#define TAL_INTCPUSEL2_MASK         _U_(0x55554411) /**< \brief (TAL_INTCPUSEL2) MASK Register */

/* -------- TAL_INTCPUSEL3 : (TAL Offset: 0x134) (R/W 32) Interrupts CPU Select 3 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t RAMECC:1;         /*!< bit:      0  RAMECC Interrupt CPU Select        */
    uint32_t :31;              /*!< bit:  1..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TAL_INTCPUSEL3_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_INTCPUSEL3_OFFSET       0x134        /**< \brief (TAL_INTCPUSEL3 offset) Interrupts CPU Select 3 */
#define TAL_INTCPUSEL3_RESETVALUE   _U_(0x00000000) /**< \brief (TAL_INTCPUSEL3 reset_value) Interrupts CPU Select 3 */

#define TAL_INTCPUSEL3_RAMECC_Pos   0            /**< \brief (TAL_INTCPUSEL3) RAMECC Interrupt CPU Select */
#define TAL_INTCPUSEL3_RAMECC_Msk   (_U_(0x1) << TAL_INTCPUSEL3_RAMECC_Pos)
#define TAL_INTCPUSEL3_RAMECC(value) (TAL_INTCPUSEL3_RAMECC_Msk & ((value) << TAL_INTCPUSEL3_RAMECC_Pos))
#define TAL_INTCPUSEL3_MASK         _U_(0x00000001) /**< \brief (TAL_INTCPUSEL3) MASK Register */

/* -------- TAL_INTCPUSEL4 : (TAL Offset: 0x138) (R/W 32) Interrupts CPU Select 4 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CAN0:1;           /*!< bit:      0  CAN0 Interrupt CPU Select          */
    uint32_t :1;               /*!< bit:      1  Reserved                           */
    uint32_t CAN1:1;           /*!< bit:      2  CAN1 Interrupt CPU Select          */
    uint32_t :1;               /*!< bit:      3  Reserved                           */
    uint32_t GMAC:1;           /*!< bit:      4  GMAC Interrupt CPU Select          */
    uint32_t :1;               /*!< bit:      5  Reserved                           */
    uint32_t TCC2:1;           /*!< bit:      6  TCC2 Interrupt CPU Select          */
    uint32_t :1;               /*!< bit:      7  Reserved                           */
    uint32_t TCC3:1;           /*!< bit:      8  TCC3 Interrupt CPU Select          */
    uint32_t :1;               /*!< bit:      9  Reserved                           */
    uint32_t TC4:1;            /*!< bit:     10  TC4 Interrupt CPU Select           */
    uint32_t :1;               /*!< bit:     11  Reserved                           */
    uint32_t TC5:1;            /*!< bit:     12  TC5 Interrupt CPU Select           */
    uint32_t :1;               /*!< bit:     13  Reserved                           */
    uint32_t PDEC:1;           /*!< bit:     14  PDEC Interrupt CPU Select          */
    uint32_t :1;               /*!< bit:     15  Reserved                           */
    uint32_t AC:1;             /*!< bit:     16  AC Interrupt CPU Select            */
    uint32_t :1;               /*!< bit:     17  Reserved                           */
    uint32_t AES:1;            /*!< bit:     18  AES Interrupt CPU Select           */
    uint32_t :1;               /*!< bit:     19  Reserved                           */
    uint32_t TRNG:1;           /*!< bit:     20  TRNG Interrupt CPU Select          */
    uint32_t :1;               /*!< bit:     21  Reserved                           */
    uint32_t ICM:1;            /*!< bit:     22  ICM Interrupt CPU Select           */
    uint32_t :1;               /*!< bit:     23  Reserved                           */
    uint32_t PUKCC:1;          /*!< bit:     24  PUKCC Interrupt CPU Select         */
    uint32_t :1;               /*!< bit:     25  Reserved                           */
    uint32_t QSPI:1;           /*!< bit:     26  QSPI Interrupt CPU Select          */
    uint32_t :5;               /*!< bit: 27..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TAL_INTCPUSEL4_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_INTCPUSEL4_OFFSET       0x138        /**< \brief (TAL_INTCPUSEL4 offset) Interrupts CPU Select 4 */
#define TAL_INTCPUSEL4_RESETVALUE   _U_(0x00000000) /**< \brief (TAL_INTCPUSEL4 reset_value) Interrupts CPU Select 4 */

#define TAL_INTCPUSEL4_CAN0_Pos     0            /**< \brief (TAL_INTCPUSEL4) CAN0 Interrupt CPU Select */
#define TAL_INTCPUSEL4_CAN0_Msk     (_U_(0x1) << TAL_INTCPUSEL4_CAN0_Pos)
#define TAL_INTCPUSEL4_CAN0(value)  (TAL_INTCPUSEL4_CAN0_Msk & ((value) << TAL_INTCPUSEL4_CAN0_Pos))
#define TAL_INTCPUSEL4_CAN1_Pos     2            /**< \brief (TAL_INTCPUSEL4) CAN1 Interrupt CPU Select */
#define TAL_INTCPUSEL4_CAN1_Msk     (_U_(0x1) << TAL_INTCPUSEL4_CAN1_Pos)
#define TAL_INTCPUSEL4_CAN1(value)  (TAL_INTCPUSEL4_CAN1_Msk & ((value) << TAL_INTCPUSEL4_CAN1_Pos))
#define TAL_INTCPUSEL4_GMAC_Pos     4            /**< \brief (TAL_INTCPUSEL4) GMAC Interrupt CPU Select */
#define TAL_INTCPUSEL4_GMAC_Msk     (_U_(0x1) << TAL_INTCPUSEL4_GMAC_Pos)
#define TAL_INTCPUSEL4_GMAC(value)  (TAL_INTCPUSEL4_GMAC_Msk & ((value) << TAL_INTCPUSEL4_GMAC_Pos))
#define TAL_INTCPUSEL4_TCC2_Pos     6            /**< \brief (TAL_INTCPUSEL4) TCC2 Interrupt CPU Select */
#define TAL_INTCPUSEL4_TCC2_Msk     (_U_(0x1) << TAL_INTCPUSEL4_TCC2_Pos)
#define TAL_INTCPUSEL4_TCC2(value)  (TAL_INTCPUSEL4_TCC2_Msk & ((value) << TAL_INTCPUSEL4_TCC2_Pos))
#define TAL_INTCPUSEL4_TCC3_Pos     8            /**< \brief (TAL_INTCPUSEL4) TCC3 Interrupt CPU Select */
#define TAL_INTCPUSEL4_TCC3_Msk     (_U_(0x1) << TAL_INTCPUSEL4_TCC3_Pos)
#define TAL_INTCPUSEL4_TCC3(value)  (TAL_INTCPUSEL4_TCC3_Msk & ((value) << TAL_INTCPUSEL4_TCC3_Pos))
#define TAL_INTCPUSEL4_TC4_Pos      10           /**< \brief (TAL_INTCPUSEL4) TC4 Interrupt CPU Select */
#define TAL_INTCPUSEL4_TC4_Msk      (_U_(0x1) << TAL_INTCPUSEL4_TC4_Pos)
#define TAL_INTCPUSEL4_TC4(value)   (TAL_INTCPUSEL4_TC4_Msk & ((value) << TAL_INTCPUSEL4_TC4_Pos))
#define TAL_INTCPUSEL4_TC5_Pos      12           /**< \brief (TAL_INTCPUSEL4) TC5 Interrupt CPU Select */
#define TAL_INTCPUSEL4_TC5_Msk      (_U_(0x1) << TAL_INTCPUSEL4_TC5_Pos)
#define TAL_INTCPUSEL4_TC5(value)   (TAL_INTCPUSEL4_TC5_Msk & ((value) << TAL_INTCPUSEL4_TC5_Pos))
#define TAL_INTCPUSEL4_PDEC_Pos     14           /**< \brief (TAL_INTCPUSEL4) PDEC Interrupt CPU Select */
#define TAL_INTCPUSEL4_PDEC_Msk     (_U_(0x1) << TAL_INTCPUSEL4_PDEC_Pos)
#define TAL_INTCPUSEL4_PDEC(value)  (TAL_INTCPUSEL4_PDEC_Msk & ((value) << TAL_INTCPUSEL4_PDEC_Pos))
#define TAL_INTCPUSEL4_AC_Pos       16           /**< \brief (TAL_INTCPUSEL4) AC Interrupt CPU Select */
#define TAL_INTCPUSEL4_AC_Msk       (_U_(0x1) << TAL_INTCPUSEL4_AC_Pos)
#define TAL_INTCPUSEL4_AC(value)    (TAL_INTCPUSEL4_AC_Msk & ((value) << TAL_INTCPUSEL4_AC_Pos))
#define TAL_INTCPUSEL4_AES_Pos      18           /**< \brief (TAL_INTCPUSEL4) AES Interrupt CPU Select */
#define TAL_INTCPUSEL4_AES_Msk      (_U_(0x1) << TAL_INTCPUSEL4_AES_Pos)
#define TAL_INTCPUSEL4_AES(value)   (TAL_INTCPUSEL4_AES_Msk & ((value) << TAL_INTCPUSEL4_AES_Pos))
#define TAL_INTCPUSEL4_TRNG_Pos     20           /**< \brief (TAL_INTCPUSEL4) TRNG Interrupt CPU Select */
#define TAL_INTCPUSEL4_TRNG_Msk     (_U_(0x1) << TAL_INTCPUSEL4_TRNG_Pos)
#define TAL_INTCPUSEL4_TRNG(value)  (TAL_INTCPUSEL4_TRNG_Msk & ((value) << TAL_INTCPUSEL4_TRNG_Pos))
#define TAL_INTCPUSEL4_ICM_Pos      22           /**< \brief (TAL_INTCPUSEL4) ICM Interrupt CPU Select */
#define TAL_INTCPUSEL4_ICM_Msk      (_U_(0x1) << TAL_INTCPUSEL4_ICM_Pos)
#define TAL_INTCPUSEL4_ICM(value)   (TAL_INTCPUSEL4_ICM_Msk & ((value) << TAL_INTCPUSEL4_ICM_Pos))
#define TAL_INTCPUSEL4_PUKCC_Pos    24           /**< \brief (TAL_INTCPUSEL4) PUKCC Interrupt CPU Select */
#define TAL_INTCPUSEL4_PUKCC_Msk    (_U_(0x1) << TAL_INTCPUSEL4_PUKCC_Pos)
#define TAL_INTCPUSEL4_PUKCC(value) (TAL_INTCPUSEL4_PUKCC_Msk & ((value) << TAL_INTCPUSEL4_PUKCC_Pos))
#define TAL_INTCPUSEL4_QSPI_Pos     26           /**< \brief (TAL_INTCPUSEL4) QSPI Interrupt CPU Select */
#define TAL_INTCPUSEL4_QSPI_Msk     (_U_(0x1) << TAL_INTCPUSEL4_QSPI_Pos)
#define TAL_INTCPUSEL4_QSPI(value)  (TAL_INTCPUSEL4_QSPI_Msk & ((value) << TAL_INTCPUSEL4_QSPI_Pos))
#define TAL_INTCPUSEL4_MASK         _U_(0x05555555) /**< \brief (TAL_INTCPUSEL4) MASK Register */

/* -------- TAL_INTCPUSEL5 : (TAL Offset: 0x13C) (R/W 32) Interrupts CPU Select 5 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} TAL_INTCPUSEL5_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_INTCPUSEL5_OFFSET       0x13C        /**< \brief (TAL_INTCPUSEL5 offset) Interrupts CPU Select 5 */
#define TAL_INTCPUSEL5_RESETVALUE   _U_(0x00000000) /**< \brief (TAL_INTCPUSEL5 reset_value) Interrupts CPU Select 5 */
#define TAL_INTCPUSEL5_MASK         _U_(0x00000000) /**< \brief (TAL_INTCPUSEL5) MASK Register */

/* -------- TAL_INTCPUSEL6 : (TAL Offset: 0x140) (R/W 32) Interrupts CPU Select 6 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SERCOM4:1;        /*!< bit:      0  SERCOM4 Interrupt CPU Select       */
    uint32_t :1;               /*!< bit:      1  Reserved                           */
    uint32_t SERCOM5:1;        /*!< bit:      2  SERCOM5 Interrupt CPU Select       */
    uint32_t :1;               /*!< bit:      3  Reserved                           */
    uint32_t SERCOM6:1;        /*!< bit:      4  SERCOM6 Interrupt CPU Select       */
    uint32_t :1;               /*!< bit:      5  Reserved                           */
    uint32_t SERCOM7:1;        /*!< bit:      6  SERCOM7 Interrupt CPU Select       */
    uint32_t :1;               /*!< bit:      7  Reserved                           */
    uint32_t TCC4:1;           /*!< bit:      8  TCC4 Interrupt CPU Select          */
    uint32_t :1;               /*!< bit:      9  Reserved                           */
    uint32_t TC6:1;            /*!< bit:     10  TC6 Interrupt CPU Select           */
    uint32_t :1;               /*!< bit:     11  Reserved                           */
    uint32_t TC7:1;            /*!< bit:     12  TC7 Interrupt CPU Select           */
    uint32_t :1;               /*!< bit:     13  Reserved                           */
    uint32_t ADC0:1;           /*!< bit:     14  ADC0 Interrupt CPU Select          */
    uint32_t :1;               /*!< bit:     15  Reserved                           */
    uint32_t ADC1:1;           /*!< bit:     16  ADC1 Interrupt CPU Select          */
    uint32_t :1;               /*!< bit:     17  Reserved                           */
    uint32_t DAC:1;            /*!< bit:     18  DAC Interrupt CPU Select           */
    uint32_t :1;               /*!< bit:     19  Reserved                           */
    uint32_t I2S:1;            /*!< bit:     20  I2S Interrupt CPU Select           */
    uint32_t :1;               /*!< bit:     21  Reserved                           */
    uint32_t PCC:1;            /*!< bit:     22  PCC Interrupt CPU Select           */
    uint32_t :9;               /*!< bit: 23..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TAL_INTCPUSEL6_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_INTCPUSEL6_OFFSET       0x140        /**< \brief (TAL_INTCPUSEL6 offset) Interrupts CPU Select 6 */
#define TAL_INTCPUSEL6_RESETVALUE   _U_(0x00000000) /**< \brief (TAL_INTCPUSEL6 reset_value) Interrupts CPU Select 6 */

#define TAL_INTCPUSEL6_SERCOM4_Pos  0            /**< \brief (TAL_INTCPUSEL6) SERCOM4 Interrupt CPU Select */
#define TAL_INTCPUSEL6_SERCOM4_Msk  (_U_(0x1) << TAL_INTCPUSEL6_SERCOM4_Pos)
#define TAL_INTCPUSEL6_SERCOM4(value) (TAL_INTCPUSEL6_SERCOM4_Msk & ((value) << TAL_INTCPUSEL6_SERCOM4_Pos))
#define TAL_INTCPUSEL6_SERCOM5_Pos  2            /**< \brief (TAL_INTCPUSEL6) SERCOM5 Interrupt CPU Select */
#define TAL_INTCPUSEL6_SERCOM5_Msk  (_U_(0x1) << TAL_INTCPUSEL6_SERCOM5_Pos)
#define TAL_INTCPUSEL6_SERCOM5(value) (TAL_INTCPUSEL6_SERCOM5_Msk & ((value) << TAL_INTCPUSEL6_SERCOM5_Pos))
#define TAL_INTCPUSEL6_SERCOM6_Pos  4            /**< \brief (TAL_INTCPUSEL6) SERCOM6 Interrupt CPU Select */
#define TAL_INTCPUSEL6_SERCOM6_Msk  (_U_(0x1) << TAL_INTCPUSEL6_SERCOM6_Pos)
#define TAL_INTCPUSEL6_SERCOM6(value) (TAL_INTCPUSEL6_SERCOM6_Msk & ((value) << TAL_INTCPUSEL6_SERCOM6_Pos))
#define TAL_INTCPUSEL6_SERCOM7_Pos  6            /**< \brief (TAL_INTCPUSEL6) SERCOM7 Interrupt CPU Select */
#define TAL_INTCPUSEL6_SERCOM7_Msk  (_U_(0x1) << TAL_INTCPUSEL6_SERCOM7_Pos)
#define TAL_INTCPUSEL6_SERCOM7(value) (TAL_INTCPUSEL6_SERCOM7_Msk & ((value) << TAL_INTCPUSEL6_SERCOM7_Pos))
#define TAL_INTCPUSEL6_TCC4_Pos     8            /**< \brief (TAL_INTCPUSEL6) TCC4 Interrupt CPU Select */
#define TAL_INTCPUSEL6_TCC4_Msk     (_U_(0x1) << TAL_INTCPUSEL6_TCC4_Pos)
#define TAL_INTCPUSEL6_TCC4(value)  (TAL_INTCPUSEL6_TCC4_Msk & ((value) << TAL_INTCPUSEL6_TCC4_Pos))
#define TAL_INTCPUSEL6_TC6_Pos      10           /**< \brief (TAL_INTCPUSEL6) TC6 Interrupt CPU Select */
#define TAL_INTCPUSEL6_TC6_Msk      (_U_(0x1) << TAL_INTCPUSEL6_TC6_Pos)
#define TAL_INTCPUSEL6_TC6(value)   (TAL_INTCPUSEL6_TC6_Msk & ((value) << TAL_INTCPUSEL6_TC6_Pos))
#define TAL_INTCPUSEL6_TC7_Pos      12           /**< \brief (TAL_INTCPUSEL6) TC7 Interrupt CPU Select */
#define TAL_INTCPUSEL6_TC7_Msk      (_U_(0x1) << TAL_INTCPUSEL6_TC7_Pos)
#define TAL_INTCPUSEL6_TC7(value)   (TAL_INTCPUSEL6_TC7_Msk & ((value) << TAL_INTCPUSEL6_TC7_Pos))
#define TAL_INTCPUSEL6_ADC0_Pos     14           /**< \brief (TAL_INTCPUSEL6) ADC0 Interrupt CPU Select */
#define TAL_INTCPUSEL6_ADC0_Msk     (_U_(0x1) << TAL_INTCPUSEL6_ADC0_Pos)
#define TAL_INTCPUSEL6_ADC0(value)  (TAL_INTCPUSEL6_ADC0_Msk & ((value) << TAL_INTCPUSEL6_ADC0_Pos))
#define TAL_INTCPUSEL6_ADC1_Pos     16           /**< \brief (TAL_INTCPUSEL6) ADC1 Interrupt CPU Select */
#define TAL_INTCPUSEL6_ADC1_Msk     (_U_(0x1) << TAL_INTCPUSEL6_ADC1_Pos)
#define TAL_INTCPUSEL6_ADC1(value)  (TAL_INTCPUSEL6_ADC1_Msk & ((value) << TAL_INTCPUSEL6_ADC1_Pos))
#define TAL_INTCPUSEL6_DAC_Pos      18           /**< \brief (TAL_INTCPUSEL6) DAC Interrupt CPU Select */
#define TAL_INTCPUSEL6_DAC_Msk      (_U_(0x1) << TAL_INTCPUSEL6_DAC_Pos)
#define TAL_INTCPUSEL6_DAC(value)   (TAL_INTCPUSEL6_DAC_Msk & ((value) << TAL_INTCPUSEL6_DAC_Pos))
#define TAL_INTCPUSEL6_I2S_Pos      20           /**< \brief (TAL_INTCPUSEL6) I2S Interrupt CPU Select */
#define TAL_INTCPUSEL6_I2S_Msk      (_U_(0x1) << TAL_INTCPUSEL6_I2S_Pos)
#define TAL_INTCPUSEL6_I2S(value)   (TAL_INTCPUSEL6_I2S_Msk & ((value) << TAL_INTCPUSEL6_I2S_Pos))
#define TAL_INTCPUSEL6_PCC_Pos      22           /**< \brief (TAL_INTCPUSEL6) PCC Interrupt CPU Select */
#define TAL_INTCPUSEL6_PCC_Msk      (_U_(0x1) << TAL_INTCPUSEL6_PCC_Pos)
#define TAL_INTCPUSEL6_PCC(value)   (TAL_INTCPUSEL6_PCC_Msk & ((value) << TAL_INTCPUSEL6_PCC_Pos))
#define TAL_INTCPUSEL6_MASK         _U_(0x00555555) /**< \brief (TAL_INTCPUSEL6) MASK Register */

/* -------- TAL_INTCPUSEL7 : (TAL Offset: 0x144) (R/W 32) Interrupts CPU Select 7 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} TAL_INTCPUSEL7_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_INTCPUSEL7_OFFSET       0x144        /**< \brief (TAL_INTCPUSEL7 offset) Interrupts CPU Select 7 */
#define TAL_INTCPUSEL7_RESETVALUE   _U_(0x00000000) /**< \brief (TAL_INTCPUSEL7 reset_value) Interrupts CPU Select 7 */
#define TAL_INTCPUSEL7_MASK         _U_(0x00000000) /**< \brief (TAL_INTCPUSEL7) MASK Register */

/* -------- TAL_INTCPUSEL8 : (TAL Offset: 0x148) (R/W 32) Interrupts CPU Select 8 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SDHC0:1;          /*!< bit:      0  SDHC0 Interrupt CPU Select         */
    uint32_t :1;               /*!< bit:      1  Reserved                           */
    uint32_t SDHC1:1;          /*!< bit:      2  SDHC1 Interrupt CPU Select         */
    uint32_t :29;              /*!< bit:  3..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TAL_INTCPUSEL8_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_INTCPUSEL8_OFFSET       0x148        /**< \brief (TAL_INTCPUSEL8 offset) Interrupts CPU Select 8 */
#define TAL_INTCPUSEL8_RESETVALUE   _U_(0x00000000) /**< \brief (TAL_INTCPUSEL8 reset_value) Interrupts CPU Select 8 */

#define TAL_INTCPUSEL8_SDHC0_Pos    0            /**< \brief (TAL_INTCPUSEL8) SDHC0 Interrupt CPU Select */
#define TAL_INTCPUSEL8_SDHC0_Msk    (_U_(0x1) << TAL_INTCPUSEL8_SDHC0_Pos)
#define TAL_INTCPUSEL8_SDHC0(value) (TAL_INTCPUSEL8_SDHC0_Msk & ((value) << TAL_INTCPUSEL8_SDHC0_Pos))
#define TAL_INTCPUSEL8_SDHC1_Pos    2            /**< \brief (TAL_INTCPUSEL8) SDHC1 Interrupt CPU Select */
#define TAL_INTCPUSEL8_SDHC1_Msk    (_U_(0x1) << TAL_INTCPUSEL8_SDHC1_Pos)
#define TAL_INTCPUSEL8_SDHC1(value) (TAL_INTCPUSEL8_SDHC1_Msk & ((value) << TAL_INTCPUSEL8_SDHC1_Pos))
#define TAL_INTCPUSEL8_MASK         _U_(0x00000005) /**< \brief (TAL_INTCPUSEL8) MASK Register */

/* -------- TAL_IRQTRIG : (TAL Offset: 0x164) (R/W 32) Interrupt Trigger -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ENABLE:1;         /*!< bit:      0  Trigger Enable                     */
    uint32_t :7;               /*!< bit:  1.. 7  Reserved                           */
    uint32_t IRQNUM:8;         /*!< bit:  8..15  Interrupt Request Number           */
    uint32_t OVERRIDE:8;       /*!< bit: 16..23  Interrupt Request Override Value   */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TAL_IRQTRIG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_IRQTRIG_OFFSET          0x164        /**< \brief (TAL_IRQTRIG offset) Interrupt Trigger */
#define TAL_IRQTRIG_RESETVALUE      _U_(0x00000000) /**< \brief (TAL_IRQTRIG reset_value) Interrupt Trigger */

#define TAL_IRQTRIG_ENABLE_Pos      0            /**< \brief (TAL_IRQTRIG) Trigger Enable */
#define TAL_IRQTRIG_ENABLE          (_U_(0x1) << TAL_IRQTRIG_ENABLE_Pos)
#define TAL_IRQTRIG_IRQNUM_Pos      8            /**< \brief (TAL_IRQTRIG) Interrupt Request Number */
#define TAL_IRQTRIG_IRQNUM_Msk      (_U_(0xFF) << TAL_IRQTRIG_IRQNUM_Pos)
#define TAL_IRQTRIG_IRQNUM(value)   (TAL_IRQTRIG_IRQNUM_Msk & ((value) << TAL_IRQTRIG_IRQNUM_Pos))
#define TAL_IRQTRIG_OVERRIDE_Pos    16           /**< \brief (TAL_IRQTRIG) Interrupt Request Override Value */
#define TAL_IRQTRIG_OVERRIDE_Msk    (_U_(0xFF) << TAL_IRQTRIG_OVERRIDE_Pos)
#define TAL_IRQTRIG_OVERRIDE(value) (TAL_IRQTRIG_OVERRIDE_Msk & ((value) << TAL_IRQTRIG_OVERRIDE_Pos))
#define TAL_IRQTRIG_MASK            _U_(0x00FFFF01) /**< \brief (TAL_IRQTRIG) MASK Register */

/* -------- TAL_IRQMON : (TAL Offset: 0x168) (R/W 16) Interrupt Monitor Select -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t EXTEND:1;         /*!< bit:      0  Extended Interrupt Request         */
    uint16_t DROP:1;           /*!< bit:      1  Drop Shortened Events              */
    uint16_t CPUID:1;          /*!< bit:      2  ID of CPU currently servicing this IRQ */
    uint16_t :5;               /*!< bit:  3.. 7  Reserved                           */
    uint16_t IRQNUM:8;         /*!< bit:  8..15  Interrupt Request Number           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} TAL_IRQMON_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_IRQMON_OFFSET           0x168        /**< \brief (TAL_IRQMON offset) Interrupt Monitor Select */
#define TAL_IRQMON_RESETVALUE       _U_(0x0000)  /**< \brief (TAL_IRQMON reset_value) Interrupt Monitor Select */

#define TAL_IRQMON_EXTEND_Pos       0            /**< \brief (TAL_IRQMON) Extended Interrupt Request */
#define TAL_IRQMON_EXTEND           (_U_(0x1) << TAL_IRQMON_EXTEND_Pos)
#define   TAL_IRQMON_EXTEND_NO_Val        _U_(0x0)   /**< \brief (TAL_IRQMON) Event is Interrupt Request signal */
#define   TAL_IRQMON_EXTEND_YES_Val       _U_(0x1)   /**< \brief (TAL_IRQMON) Event is Interrupt Request signal extended until end of Interrupt Handler */
#define TAL_IRQMON_EXTEND_NO        (TAL_IRQMON_EXTEND_NO_Val      << TAL_IRQMON_EXTEND_Pos)
#define TAL_IRQMON_EXTEND_YES       (TAL_IRQMON_EXTEND_YES_Val     << TAL_IRQMON_EXTEND_Pos)
#define TAL_IRQMON_DROP_Pos         1            /**< \brief (TAL_IRQMON) Drop Shortened Events */
#define TAL_IRQMON_DROP             (_U_(0x1) << TAL_IRQMON_DROP_Pos)
#define TAL_IRQMON_CPUID_Pos        2            /**< \brief (TAL_IRQMON) ID of CPU currently servicing this IRQ */
#define TAL_IRQMON_CPUID_Msk        (_U_(0x1) << TAL_IRQMON_CPUID_Pos)
#define TAL_IRQMON_CPUID(value)     (TAL_IRQMON_CPUID_Msk & ((value) << TAL_IRQMON_CPUID_Pos))
#define TAL_IRQMON_IRQNUM_Pos       8            /**< \brief (TAL_IRQMON) Interrupt Request Number */
#define TAL_IRQMON_IRQNUM_Msk       (_U_(0xFF) << TAL_IRQMON_IRQNUM_Pos)
#define TAL_IRQMON_IRQNUM(value)    (TAL_IRQMON_IRQNUM_Msk & ((value) << TAL_IRQMON_IRQNUM_Pos))
#define TAL_IRQMON_MASK             _U_(0xFF07)  /**< \brief (TAL_IRQMON) MASK Register */

/* -------- TAL_CPUIRQS : (TAL Offset: 0x180) (R/  32) CPUIRQS Interrupt Status m for CPU n -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CPUIRQS:32;       /*!< bit:  0..31  Interrupt Requests for CPU n       */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TAL_CPUIRQS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_CPUIRQS_OFFSET          0x180        /**< \brief (TAL_CPUIRQS offset) Interrupt Status m for CPU n */
#define TAL_CPUIRQS_RESETVALUE      _U_(0x00000000) /**< \brief (TAL_CPUIRQS reset_value) Interrupt Status m for CPU n */

#define TAL_CPUIRQS_CPUIRQS_Pos     0            /**< \brief (TAL_CPUIRQS) Interrupt Requests for CPU n */
#define TAL_CPUIRQS_CPUIRQS_Msk     (_U_(0xFFFFFFFF) << TAL_CPUIRQS_CPUIRQS_Pos)
#define TAL_CPUIRQS_CPUIRQS(value)  (TAL_CPUIRQS_CPUIRQS_Msk & ((value) << TAL_CPUIRQS_CPUIRQS_Pos))
#define TAL_CPUIRQS_MASK            _U_(0xFFFFFFFF) /**< \brief (TAL_CPUIRQS) MASK Register */

/* -------- TAL_SMASK : (TAL Offset: 0x200) (R/W 32) SMASKS Inter-Process Signal Mask m for CPU n -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t IPS0:1;           /*!< bit:      0  Inter-Process Signal 0             */
    uint32_t IPS1:1;           /*!< bit:      1  Inter-Process Signal 1             */
    uint32_t IPS2:1;           /*!< bit:      2  Inter-Process Signal 2             */
    uint32_t IPS3:1;           /*!< bit:      3  Inter-Process Signal 3             */
    uint32_t IPS4:1;           /*!< bit:      4  Inter-Process Signal 4             */
    uint32_t IPS5:1;           /*!< bit:      5  Inter-Process Signal 5             */
    uint32_t IPS6:1;           /*!< bit:      6  Inter-Process Signal 6             */
    uint32_t IPS7:1;           /*!< bit:      7  Inter-Process Signal 7             */
    uint32_t IPS8:1;           /*!< bit:      8  Inter-Process Signal 8             */
    uint32_t IPS9:1;           /*!< bit:      9  Inter-Process Signal 9             */
    uint32_t IPS10:1;          /*!< bit:     10  Inter-Process Signal 10            */
    uint32_t IPS11:1;          /*!< bit:     11  Inter-Process Signal 11            */
    uint32_t IPS12:1;          /*!< bit:     12  Inter-Process Signal 12            */
    uint32_t IPS13:1;          /*!< bit:     13  Inter-Process Signal 13            */
    uint32_t IPS14:1;          /*!< bit:     14  Inter-Process Signal 14            */
    uint32_t IPS15:1;          /*!< bit:     15  Inter-Process Signal 15            */
    uint32_t IPS16:1;          /*!< bit:     16  Inter-Process Signal 16            */
    uint32_t IPS17:1;          /*!< bit:     17  Inter-Process Signal 17            */
    uint32_t IPS18:1;          /*!< bit:     18  Inter-Process Signal 18            */
    uint32_t IPS19:1;          /*!< bit:     19  Inter-Process Signal 19            */
    uint32_t IPS20:1;          /*!< bit:     20  Inter-Process Signal 20            */
    uint32_t IPS21:1;          /*!< bit:     21  Inter-Process Signal 21            */
    uint32_t IPS22:1;          /*!< bit:     22  Inter-Process Signal 22            */
    uint32_t IPS23:1;          /*!< bit:     23  Inter-Process Signal 23            */
    uint32_t IPS24:1;          /*!< bit:     24  Inter-Process Signal 24            */
    uint32_t IPS25:1;          /*!< bit:     25  Inter-Process Signal 25            */
    uint32_t IPS26:1;          /*!< bit:     26  Inter-Process Signal 26            */
    uint32_t IPS27:1;          /*!< bit:     27  Inter-Process Signal 27            */
    uint32_t IPS28:1;          /*!< bit:     28  Inter-Process Signal 28            */
    uint32_t IPS29:1;          /*!< bit:     29  Inter-Process Signal 29            */
    uint32_t IPS30:1;          /*!< bit:     30  Inter-Process Signal 30            */
    uint32_t IPS31:1;          /*!< bit:     31  Inter-Process Signal 31            */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t IPS:32;           /*!< bit:  0..31  Inter-Process Signal x             */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TAL_SMASK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_SMASK_OFFSET            0x200        /**< \brief (TAL_SMASK offset) Inter-Process Signal Mask m for CPU n */
#define TAL_SMASK_RESETVALUE        _U_(0x00000000) /**< \brief (TAL_SMASK reset_value) Inter-Process Signal Mask m for CPU n */

#define TAL_SMASK_IPS0_Pos          0            /**< \brief (TAL_SMASK) Inter-Process Signal 0 */
#define TAL_SMASK_IPS0              (_U_(1) << TAL_SMASK_IPS0_Pos)
#define TAL_SMASK_IPS1_Pos          1            /**< \brief (TAL_SMASK) Inter-Process Signal 1 */
#define TAL_SMASK_IPS1              (_U_(1) << TAL_SMASK_IPS1_Pos)
#define TAL_SMASK_IPS2_Pos          2            /**< \brief (TAL_SMASK) Inter-Process Signal 2 */
#define TAL_SMASK_IPS2              (_U_(1) << TAL_SMASK_IPS2_Pos)
#define TAL_SMASK_IPS3_Pos          3            /**< \brief (TAL_SMASK) Inter-Process Signal 3 */
#define TAL_SMASK_IPS3              (_U_(1) << TAL_SMASK_IPS3_Pos)
#define TAL_SMASK_IPS4_Pos          4            /**< \brief (TAL_SMASK) Inter-Process Signal 4 */
#define TAL_SMASK_IPS4              (_U_(1) << TAL_SMASK_IPS4_Pos)
#define TAL_SMASK_IPS5_Pos          5            /**< \brief (TAL_SMASK) Inter-Process Signal 5 */
#define TAL_SMASK_IPS5              (_U_(1) << TAL_SMASK_IPS5_Pos)
#define TAL_SMASK_IPS6_Pos          6            /**< \brief (TAL_SMASK) Inter-Process Signal 6 */
#define TAL_SMASK_IPS6              (_U_(1) << TAL_SMASK_IPS6_Pos)
#define TAL_SMASK_IPS7_Pos          7            /**< \brief (TAL_SMASK) Inter-Process Signal 7 */
#define TAL_SMASK_IPS7              (_U_(1) << TAL_SMASK_IPS7_Pos)
#define TAL_SMASK_IPS8_Pos          8            /**< \brief (TAL_SMASK) Inter-Process Signal 8 */
#define TAL_SMASK_IPS8              (_U_(1) << TAL_SMASK_IPS8_Pos)
#define TAL_SMASK_IPS9_Pos          9            /**< \brief (TAL_SMASK) Inter-Process Signal 9 */
#define TAL_SMASK_IPS9              (_U_(1) << TAL_SMASK_IPS9_Pos)
#define TAL_SMASK_IPS10_Pos         10           /**< \brief (TAL_SMASK) Inter-Process Signal 10 */
#define TAL_SMASK_IPS10             (_U_(1) << TAL_SMASK_IPS10_Pos)
#define TAL_SMASK_IPS11_Pos         11           /**< \brief (TAL_SMASK) Inter-Process Signal 11 */
#define TAL_SMASK_IPS11             (_U_(1) << TAL_SMASK_IPS11_Pos)
#define TAL_SMASK_IPS12_Pos         12           /**< \brief (TAL_SMASK) Inter-Process Signal 12 */
#define TAL_SMASK_IPS12             (_U_(1) << TAL_SMASK_IPS12_Pos)
#define TAL_SMASK_IPS13_Pos         13           /**< \brief (TAL_SMASK) Inter-Process Signal 13 */
#define TAL_SMASK_IPS13             (_U_(1) << TAL_SMASK_IPS13_Pos)
#define TAL_SMASK_IPS14_Pos         14           /**< \brief (TAL_SMASK) Inter-Process Signal 14 */
#define TAL_SMASK_IPS14             (_U_(1) << TAL_SMASK_IPS14_Pos)
#define TAL_SMASK_IPS15_Pos         15           /**< \brief (TAL_SMASK) Inter-Process Signal 15 */
#define TAL_SMASK_IPS15             (_U_(1) << TAL_SMASK_IPS15_Pos)
#define TAL_SMASK_IPS16_Pos         16           /**< \brief (TAL_SMASK) Inter-Process Signal 16 */
#define TAL_SMASK_IPS16             (_U_(1) << TAL_SMASK_IPS16_Pos)
#define TAL_SMASK_IPS17_Pos         17           /**< \brief (TAL_SMASK) Inter-Process Signal 17 */
#define TAL_SMASK_IPS17             (_U_(1) << TAL_SMASK_IPS17_Pos)
#define TAL_SMASK_IPS18_Pos         18           /**< \brief (TAL_SMASK) Inter-Process Signal 18 */
#define TAL_SMASK_IPS18             (_U_(1) << TAL_SMASK_IPS18_Pos)
#define TAL_SMASK_IPS19_Pos         19           /**< \brief (TAL_SMASK) Inter-Process Signal 19 */
#define TAL_SMASK_IPS19             (_U_(1) << TAL_SMASK_IPS19_Pos)
#define TAL_SMASK_IPS20_Pos         20           /**< \brief (TAL_SMASK) Inter-Process Signal 20 */
#define TAL_SMASK_IPS20             (_U_(1) << TAL_SMASK_IPS20_Pos)
#define TAL_SMASK_IPS21_Pos         21           /**< \brief (TAL_SMASK) Inter-Process Signal 21 */
#define TAL_SMASK_IPS21             (_U_(1) << TAL_SMASK_IPS21_Pos)
#define TAL_SMASK_IPS22_Pos         22           /**< \brief (TAL_SMASK) Inter-Process Signal 22 */
#define TAL_SMASK_IPS22             (_U_(1) << TAL_SMASK_IPS22_Pos)
#define TAL_SMASK_IPS23_Pos         23           /**< \brief (TAL_SMASK) Inter-Process Signal 23 */
#define TAL_SMASK_IPS23             (_U_(1) << TAL_SMASK_IPS23_Pos)
#define TAL_SMASK_IPS24_Pos         24           /**< \brief (TAL_SMASK) Inter-Process Signal 24 */
#define TAL_SMASK_IPS24             (_U_(1) << TAL_SMASK_IPS24_Pos)
#define TAL_SMASK_IPS25_Pos         25           /**< \brief (TAL_SMASK) Inter-Process Signal 25 */
#define TAL_SMASK_IPS25             (_U_(1) << TAL_SMASK_IPS25_Pos)
#define TAL_SMASK_IPS26_Pos         26           /**< \brief (TAL_SMASK) Inter-Process Signal 26 */
#define TAL_SMASK_IPS26             (_U_(1) << TAL_SMASK_IPS26_Pos)
#define TAL_SMASK_IPS27_Pos         27           /**< \brief (TAL_SMASK) Inter-Process Signal 27 */
#define TAL_SMASK_IPS27             (_U_(1) << TAL_SMASK_IPS27_Pos)
#define TAL_SMASK_IPS28_Pos         28           /**< \brief (TAL_SMASK) Inter-Process Signal 28 */
#define TAL_SMASK_IPS28             (_U_(1) << TAL_SMASK_IPS28_Pos)
#define TAL_SMASK_IPS29_Pos         29           /**< \brief (TAL_SMASK) Inter-Process Signal 29 */
#define TAL_SMASK_IPS29             (_U_(1) << TAL_SMASK_IPS29_Pos)
#define TAL_SMASK_IPS30_Pos         30           /**< \brief (TAL_SMASK) Inter-Process Signal 30 */
#define TAL_SMASK_IPS30             (_U_(1) << TAL_SMASK_IPS30_Pos)
#define TAL_SMASK_IPS31_Pos         31           /**< \brief (TAL_SMASK) Inter-Process Signal 31 */
#define TAL_SMASK_IPS31             (_U_(1) << TAL_SMASK_IPS31_Pos)
#define TAL_SMASK_IPS_Pos           0            /**< \brief (TAL_SMASK) Inter-Process Signal x */
#define TAL_SMASK_IPS_Msk           (_U_(0xFFFFFFFF) << TAL_SMASK_IPS_Pos)
#define TAL_SMASK_IPS(value)        (TAL_SMASK_IPS_Msk & ((value) << TAL_SMASK_IPS_Pos))
#define TAL_SMASK_MASK              _U_(0xFFFFFFFF) /**< \brief (TAL_SMASK) MASK Register */

/* -------- TAL_SFLAGCLR : (TAL Offset: 0x220) ( /W 32) Inter-Process Signal Flag Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t IPS0:1;           /*!< bit:      0  Inter-Process Signal 0             */
    uint32_t IPS1:1;           /*!< bit:      1  Inter-Process Signal 1             */
    uint32_t IPS2:1;           /*!< bit:      2  Inter-Process Signal 2             */
    uint32_t IPS3:1;           /*!< bit:      3  Inter-Process Signal 3             */
    uint32_t IPS4:1;           /*!< bit:      4  Inter-Process Signal 4             */
    uint32_t IPS5:1;           /*!< bit:      5  Inter-Process Signal 5             */
    uint32_t IPS6:1;           /*!< bit:      6  Inter-Process Signal 6             */
    uint32_t IPS7:1;           /*!< bit:      7  Inter-Process Signal 7             */
    uint32_t IPS8:1;           /*!< bit:      8  Inter-Process Signal 8             */
    uint32_t IPS9:1;           /*!< bit:      9  Inter-Process Signal 9             */
    uint32_t IPS10:1;          /*!< bit:     10  Inter-Process Signal 10            */
    uint32_t IPS11:1;          /*!< bit:     11  Inter-Process Signal 11            */
    uint32_t IPS12:1;          /*!< bit:     12  Inter-Process Signal 12            */
    uint32_t IPS13:1;          /*!< bit:     13  Inter-Process Signal 13            */
    uint32_t IPS14:1;          /*!< bit:     14  Inter-Process Signal 14            */
    uint32_t IPS15:1;          /*!< bit:     15  Inter-Process Signal 15            */
    uint32_t IPS16:1;          /*!< bit:     16  Inter-Process Signal 16            */
    uint32_t IPS17:1;          /*!< bit:     17  Inter-Process Signal 17            */
    uint32_t IPS18:1;          /*!< bit:     18  Inter-Process Signal 18            */
    uint32_t IPS19:1;          /*!< bit:     19  Inter-Process Signal 19            */
    uint32_t IPS20:1;          /*!< bit:     20  Inter-Process Signal 20            */
    uint32_t IPS21:1;          /*!< bit:     21  Inter-Process Signal 21            */
    uint32_t IPS22:1;          /*!< bit:     22  Inter-Process Signal 22            */
    uint32_t IPS23:1;          /*!< bit:     23  Inter-Process Signal 23            */
    uint32_t IPS24:1;          /*!< bit:     24  Inter-Process Signal 24            */
    uint32_t IPS25:1;          /*!< bit:     25  Inter-Process Signal 25            */
    uint32_t IPS26:1;          /*!< bit:     26  Inter-Process Signal 26            */
    uint32_t IPS27:1;          /*!< bit:     27  Inter-Process Signal 27            */
    uint32_t IPS28:1;          /*!< bit:     28  Inter-Process Signal 28            */
    uint32_t IPS29:1;          /*!< bit:     29  Inter-Process Signal 29            */
    uint32_t IPS30:1;          /*!< bit:     30  Inter-Process Signal 30            */
    uint32_t IPS31:1;          /*!< bit:     31  Inter-Process Signal 31            */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t IPS:32;           /*!< bit:  0..31  Inter-Process Signal x             */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TAL_SFLAGCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_SFLAGCLR_OFFSET         0x220        /**< \brief (TAL_SFLAGCLR offset) Inter-Process Signal Flag Clear */
#define TAL_SFLAGCLR_RESETVALUE     _U_(0x00000000) /**< \brief (TAL_SFLAGCLR reset_value) Inter-Process Signal Flag Clear */

#define TAL_SFLAGCLR_IPS0_Pos       0            /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 0 */
#define TAL_SFLAGCLR_IPS0           (_U_(1) << TAL_SFLAGCLR_IPS0_Pos)
#define TAL_SFLAGCLR_IPS1_Pos       1            /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 1 */
#define TAL_SFLAGCLR_IPS1           (_U_(1) << TAL_SFLAGCLR_IPS1_Pos)
#define TAL_SFLAGCLR_IPS2_Pos       2            /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 2 */
#define TAL_SFLAGCLR_IPS2           (_U_(1) << TAL_SFLAGCLR_IPS2_Pos)
#define TAL_SFLAGCLR_IPS3_Pos       3            /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 3 */
#define TAL_SFLAGCLR_IPS3           (_U_(1) << TAL_SFLAGCLR_IPS3_Pos)
#define TAL_SFLAGCLR_IPS4_Pos       4            /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 4 */
#define TAL_SFLAGCLR_IPS4           (_U_(1) << TAL_SFLAGCLR_IPS4_Pos)
#define TAL_SFLAGCLR_IPS5_Pos       5            /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 5 */
#define TAL_SFLAGCLR_IPS5           (_U_(1) << TAL_SFLAGCLR_IPS5_Pos)
#define TAL_SFLAGCLR_IPS6_Pos       6            /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 6 */
#define TAL_SFLAGCLR_IPS6           (_U_(1) << TAL_SFLAGCLR_IPS6_Pos)
#define TAL_SFLAGCLR_IPS7_Pos       7            /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 7 */
#define TAL_SFLAGCLR_IPS7           (_U_(1) << TAL_SFLAGCLR_IPS7_Pos)
#define TAL_SFLAGCLR_IPS8_Pos       8            /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 8 */
#define TAL_SFLAGCLR_IPS8           (_U_(1) << TAL_SFLAGCLR_IPS8_Pos)
#define TAL_SFLAGCLR_IPS9_Pos       9            /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 9 */
#define TAL_SFLAGCLR_IPS9           (_U_(1) << TAL_SFLAGCLR_IPS9_Pos)
#define TAL_SFLAGCLR_IPS10_Pos      10           /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 10 */
#define TAL_SFLAGCLR_IPS10          (_U_(1) << TAL_SFLAGCLR_IPS10_Pos)
#define TAL_SFLAGCLR_IPS11_Pos      11           /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 11 */
#define TAL_SFLAGCLR_IPS11          (_U_(1) << TAL_SFLAGCLR_IPS11_Pos)
#define TAL_SFLAGCLR_IPS12_Pos      12           /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 12 */
#define TAL_SFLAGCLR_IPS12          (_U_(1) << TAL_SFLAGCLR_IPS12_Pos)
#define TAL_SFLAGCLR_IPS13_Pos      13           /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 13 */
#define TAL_SFLAGCLR_IPS13          (_U_(1) << TAL_SFLAGCLR_IPS13_Pos)
#define TAL_SFLAGCLR_IPS14_Pos      14           /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 14 */
#define TAL_SFLAGCLR_IPS14          (_U_(1) << TAL_SFLAGCLR_IPS14_Pos)
#define TAL_SFLAGCLR_IPS15_Pos      15           /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 15 */
#define TAL_SFLAGCLR_IPS15          (_U_(1) << TAL_SFLAGCLR_IPS15_Pos)
#define TAL_SFLAGCLR_IPS16_Pos      16           /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 16 */
#define TAL_SFLAGCLR_IPS16          (_U_(1) << TAL_SFLAGCLR_IPS16_Pos)
#define TAL_SFLAGCLR_IPS17_Pos      17           /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 17 */
#define TAL_SFLAGCLR_IPS17          (_U_(1) << TAL_SFLAGCLR_IPS17_Pos)
#define TAL_SFLAGCLR_IPS18_Pos      18           /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 18 */
#define TAL_SFLAGCLR_IPS18          (_U_(1) << TAL_SFLAGCLR_IPS18_Pos)
#define TAL_SFLAGCLR_IPS19_Pos      19           /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 19 */
#define TAL_SFLAGCLR_IPS19          (_U_(1) << TAL_SFLAGCLR_IPS19_Pos)
#define TAL_SFLAGCLR_IPS20_Pos      20           /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 20 */
#define TAL_SFLAGCLR_IPS20          (_U_(1) << TAL_SFLAGCLR_IPS20_Pos)
#define TAL_SFLAGCLR_IPS21_Pos      21           /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 21 */
#define TAL_SFLAGCLR_IPS21          (_U_(1) << TAL_SFLAGCLR_IPS21_Pos)
#define TAL_SFLAGCLR_IPS22_Pos      22           /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 22 */
#define TAL_SFLAGCLR_IPS22          (_U_(1) << TAL_SFLAGCLR_IPS22_Pos)
#define TAL_SFLAGCLR_IPS23_Pos      23           /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 23 */
#define TAL_SFLAGCLR_IPS23          (_U_(1) << TAL_SFLAGCLR_IPS23_Pos)
#define TAL_SFLAGCLR_IPS24_Pos      24           /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 24 */
#define TAL_SFLAGCLR_IPS24          (_U_(1) << TAL_SFLAGCLR_IPS24_Pos)
#define TAL_SFLAGCLR_IPS25_Pos      25           /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 25 */
#define TAL_SFLAGCLR_IPS25          (_U_(1) << TAL_SFLAGCLR_IPS25_Pos)
#define TAL_SFLAGCLR_IPS26_Pos      26           /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 26 */
#define TAL_SFLAGCLR_IPS26          (_U_(1) << TAL_SFLAGCLR_IPS26_Pos)
#define TAL_SFLAGCLR_IPS27_Pos      27           /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 27 */
#define TAL_SFLAGCLR_IPS27          (_U_(1) << TAL_SFLAGCLR_IPS27_Pos)
#define TAL_SFLAGCLR_IPS28_Pos      28           /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 28 */
#define TAL_SFLAGCLR_IPS28          (_U_(1) << TAL_SFLAGCLR_IPS28_Pos)
#define TAL_SFLAGCLR_IPS29_Pos      29           /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 29 */
#define TAL_SFLAGCLR_IPS29          (_U_(1) << TAL_SFLAGCLR_IPS29_Pos)
#define TAL_SFLAGCLR_IPS30_Pos      30           /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 30 */
#define TAL_SFLAGCLR_IPS30          (_U_(1) << TAL_SFLAGCLR_IPS30_Pos)
#define TAL_SFLAGCLR_IPS31_Pos      31           /**< \brief (TAL_SFLAGCLR) Inter-Process Signal 31 */
#define TAL_SFLAGCLR_IPS31          (_U_(1) << TAL_SFLAGCLR_IPS31_Pos)
#define TAL_SFLAGCLR_IPS_Pos        0            /**< \brief (TAL_SFLAGCLR) Inter-Process Signal x */
#define TAL_SFLAGCLR_IPS_Msk        (_U_(0xFFFFFFFF) << TAL_SFLAGCLR_IPS_Pos)
#define TAL_SFLAGCLR_IPS(value)     (TAL_SFLAGCLR_IPS_Msk & ((value) << TAL_SFLAGCLR_IPS_Pos))
#define TAL_SFLAGCLR_MASK           _U_(0xFFFFFFFF) /**< \brief (TAL_SFLAGCLR) MASK Register */

/* -------- TAL_SFLAGSET : (TAL Offset: 0x228) ( /W 32) Inter-Process Signal Flag Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t IPS0:1;           /*!< bit:      0  Inter-Process Signal 0             */
    uint32_t IPS1:1;           /*!< bit:      1  Inter-Process Signal 1             */
    uint32_t IPS2:1;           /*!< bit:      2  Inter-Process Signal 2             */
    uint32_t IPS3:1;           /*!< bit:      3  Inter-Process Signal 3             */
    uint32_t IPS4:1;           /*!< bit:      4  Inter-Process Signal 4             */
    uint32_t IPS5:1;           /*!< bit:      5  Inter-Process Signal 5             */
    uint32_t IPS6:1;           /*!< bit:      6  Inter-Process Signal 6             */
    uint32_t IPS7:1;           /*!< bit:      7  Inter-Process Signal 7             */
    uint32_t IPS8:1;           /*!< bit:      8  Inter-Process Signal 8             */
    uint32_t IPS9:1;           /*!< bit:      9  Inter-Process Signal 9             */
    uint32_t IPS10:1;          /*!< bit:     10  Inter-Process Signal 10            */
    uint32_t IPS11:1;          /*!< bit:     11  Inter-Process Signal 11            */
    uint32_t IPS12:1;          /*!< bit:     12  Inter-Process Signal 12            */
    uint32_t IPS13:1;          /*!< bit:     13  Inter-Process Signal 13            */
    uint32_t IPS14:1;          /*!< bit:     14  Inter-Process Signal 14            */
    uint32_t IPS15:1;          /*!< bit:     15  Inter-Process Signal 15            */
    uint32_t IPS16:1;          /*!< bit:     16  Inter-Process Signal 16            */
    uint32_t IPS17:1;          /*!< bit:     17  Inter-Process Signal 17            */
    uint32_t IPS18:1;          /*!< bit:     18  Inter-Process Signal 18            */
    uint32_t IPS19:1;          /*!< bit:     19  Inter-Process Signal 19            */
    uint32_t IPS20:1;          /*!< bit:     20  Inter-Process Signal 20            */
    uint32_t IPS21:1;          /*!< bit:     21  Inter-Process Signal 21            */
    uint32_t IPS22:1;          /*!< bit:     22  Inter-Process Signal 22            */
    uint32_t IPS23:1;          /*!< bit:     23  Inter-Process Signal 23            */
    uint32_t IPS24:1;          /*!< bit:     24  Inter-Process Signal 24            */
    uint32_t IPS25:1;          /*!< bit:     25  Inter-Process Signal 25            */
    uint32_t IPS26:1;          /*!< bit:     26  Inter-Process Signal 26            */
    uint32_t IPS27:1;          /*!< bit:     27  Inter-Process Signal 27            */
    uint32_t IPS28:1;          /*!< bit:     28  Inter-Process Signal 28            */
    uint32_t IPS29:1;          /*!< bit:     29  Inter-Process Signal 29            */
    uint32_t IPS30:1;          /*!< bit:     30  Inter-Process Signal 30            */
    uint32_t IPS31:1;          /*!< bit:     31  Inter-Process Signal 31            */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t IPS:32;           /*!< bit:  0..31  Inter-Process Signal x             */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TAL_SFLAGSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_SFLAGSET_OFFSET         0x228        /**< \brief (TAL_SFLAGSET offset) Inter-Process Signal Flag Set */
#define TAL_SFLAGSET_RESETVALUE     _U_(0x00000000) /**< \brief (TAL_SFLAGSET reset_value) Inter-Process Signal Flag Set */

#define TAL_SFLAGSET_IPS0_Pos       0            /**< \brief (TAL_SFLAGSET) Inter-Process Signal 0 */
#define TAL_SFLAGSET_IPS0           (_U_(1) << TAL_SFLAGSET_IPS0_Pos)
#define TAL_SFLAGSET_IPS1_Pos       1            /**< \brief (TAL_SFLAGSET) Inter-Process Signal 1 */
#define TAL_SFLAGSET_IPS1           (_U_(1) << TAL_SFLAGSET_IPS1_Pos)
#define TAL_SFLAGSET_IPS2_Pos       2            /**< \brief (TAL_SFLAGSET) Inter-Process Signal 2 */
#define TAL_SFLAGSET_IPS2           (_U_(1) << TAL_SFLAGSET_IPS2_Pos)
#define TAL_SFLAGSET_IPS3_Pos       3            /**< \brief (TAL_SFLAGSET) Inter-Process Signal 3 */
#define TAL_SFLAGSET_IPS3           (_U_(1) << TAL_SFLAGSET_IPS3_Pos)
#define TAL_SFLAGSET_IPS4_Pos       4            /**< \brief (TAL_SFLAGSET) Inter-Process Signal 4 */
#define TAL_SFLAGSET_IPS4           (_U_(1) << TAL_SFLAGSET_IPS4_Pos)
#define TAL_SFLAGSET_IPS5_Pos       5            /**< \brief (TAL_SFLAGSET) Inter-Process Signal 5 */
#define TAL_SFLAGSET_IPS5           (_U_(1) << TAL_SFLAGSET_IPS5_Pos)
#define TAL_SFLAGSET_IPS6_Pos       6            /**< \brief (TAL_SFLAGSET) Inter-Process Signal 6 */
#define TAL_SFLAGSET_IPS6           (_U_(1) << TAL_SFLAGSET_IPS6_Pos)
#define TAL_SFLAGSET_IPS7_Pos       7            /**< \brief (TAL_SFLAGSET) Inter-Process Signal 7 */
#define TAL_SFLAGSET_IPS7           (_U_(1) << TAL_SFLAGSET_IPS7_Pos)
#define TAL_SFLAGSET_IPS8_Pos       8            /**< \brief (TAL_SFLAGSET) Inter-Process Signal 8 */
#define TAL_SFLAGSET_IPS8           (_U_(1) << TAL_SFLAGSET_IPS8_Pos)
#define TAL_SFLAGSET_IPS9_Pos       9            /**< \brief (TAL_SFLAGSET) Inter-Process Signal 9 */
#define TAL_SFLAGSET_IPS9           (_U_(1) << TAL_SFLAGSET_IPS9_Pos)
#define TAL_SFLAGSET_IPS10_Pos      10           /**< \brief (TAL_SFLAGSET) Inter-Process Signal 10 */
#define TAL_SFLAGSET_IPS10          (_U_(1) << TAL_SFLAGSET_IPS10_Pos)
#define TAL_SFLAGSET_IPS11_Pos      11           /**< \brief (TAL_SFLAGSET) Inter-Process Signal 11 */
#define TAL_SFLAGSET_IPS11          (_U_(1) << TAL_SFLAGSET_IPS11_Pos)
#define TAL_SFLAGSET_IPS12_Pos      12           /**< \brief (TAL_SFLAGSET) Inter-Process Signal 12 */
#define TAL_SFLAGSET_IPS12          (_U_(1) << TAL_SFLAGSET_IPS12_Pos)
#define TAL_SFLAGSET_IPS13_Pos      13           /**< \brief (TAL_SFLAGSET) Inter-Process Signal 13 */
#define TAL_SFLAGSET_IPS13          (_U_(1) << TAL_SFLAGSET_IPS13_Pos)
#define TAL_SFLAGSET_IPS14_Pos      14           /**< \brief (TAL_SFLAGSET) Inter-Process Signal 14 */
#define TAL_SFLAGSET_IPS14          (_U_(1) << TAL_SFLAGSET_IPS14_Pos)
#define TAL_SFLAGSET_IPS15_Pos      15           /**< \brief (TAL_SFLAGSET) Inter-Process Signal 15 */
#define TAL_SFLAGSET_IPS15          (_U_(1) << TAL_SFLAGSET_IPS15_Pos)
#define TAL_SFLAGSET_IPS16_Pos      16           /**< \brief (TAL_SFLAGSET) Inter-Process Signal 16 */
#define TAL_SFLAGSET_IPS16          (_U_(1) << TAL_SFLAGSET_IPS16_Pos)
#define TAL_SFLAGSET_IPS17_Pos      17           /**< \brief (TAL_SFLAGSET) Inter-Process Signal 17 */
#define TAL_SFLAGSET_IPS17          (_U_(1) << TAL_SFLAGSET_IPS17_Pos)
#define TAL_SFLAGSET_IPS18_Pos      18           /**< \brief (TAL_SFLAGSET) Inter-Process Signal 18 */
#define TAL_SFLAGSET_IPS18          (_U_(1) << TAL_SFLAGSET_IPS18_Pos)
#define TAL_SFLAGSET_IPS19_Pos      19           /**< \brief (TAL_SFLAGSET) Inter-Process Signal 19 */
#define TAL_SFLAGSET_IPS19          (_U_(1) << TAL_SFLAGSET_IPS19_Pos)
#define TAL_SFLAGSET_IPS20_Pos      20           /**< \brief (TAL_SFLAGSET) Inter-Process Signal 20 */
#define TAL_SFLAGSET_IPS20          (_U_(1) << TAL_SFLAGSET_IPS20_Pos)
#define TAL_SFLAGSET_IPS21_Pos      21           /**< \brief (TAL_SFLAGSET) Inter-Process Signal 21 */
#define TAL_SFLAGSET_IPS21          (_U_(1) << TAL_SFLAGSET_IPS21_Pos)
#define TAL_SFLAGSET_IPS22_Pos      22           /**< \brief (TAL_SFLAGSET) Inter-Process Signal 22 */
#define TAL_SFLAGSET_IPS22          (_U_(1) << TAL_SFLAGSET_IPS22_Pos)
#define TAL_SFLAGSET_IPS23_Pos      23           /**< \brief (TAL_SFLAGSET) Inter-Process Signal 23 */
#define TAL_SFLAGSET_IPS23          (_U_(1) << TAL_SFLAGSET_IPS23_Pos)
#define TAL_SFLAGSET_IPS24_Pos      24           /**< \brief (TAL_SFLAGSET) Inter-Process Signal 24 */
#define TAL_SFLAGSET_IPS24          (_U_(1) << TAL_SFLAGSET_IPS24_Pos)
#define TAL_SFLAGSET_IPS25_Pos      25           /**< \brief (TAL_SFLAGSET) Inter-Process Signal 25 */
#define TAL_SFLAGSET_IPS25          (_U_(1) << TAL_SFLAGSET_IPS25_Pos)
#define TAL_SFLAGSET_IPS26_Pos      26           /**< \brief (TAL_SFLAGSET) Inter-Process Signal 26 */
#define TAL_SFLAGSET_IPS26          (_U_(1) << TAL_SFLAGSET_IPS26_Pos)
#define TAL_SFLAGSET_IPS27_Pos      27           /**< \brief (TAL_SFLAGSET) Inter-Process Signal 27 */
#define TAL_SFLAGSET_IPS27          (_U_(1) << TAL_SFLAGSET_IPS27_Pos)
#define TAL_SFLAGSET_IPS28_Pos      28           /**< \brief (TAL_SFLAGSET) Inter-Process Signal 28 */
#define TAL_SFLAGSET_IPS28          (_U_(1) << TAL_SFLAGSET_IPS28_Pos)
#define TAL_SFLAGSET_IPS29_Pos      29           /**< \brief (TAL_SFLAGSET) Inter-Process Signal 29 */
#define TAL_SFLAGSET_IPS29          (_U_(1) << TAL_SFLAGSET_IPS29_Pos)
#define TAL_SFLAGSET_IPS30_Pos      30           /**< \brief (TAL_SFLAGSET) Inter-Process Signal 30 */
#define TAL_SFLAGSET_IPS30          (_U_(1) << TAL_SFLAGSET_IPS30_Pos)
#define TAL_SFLAGSET_IPS31_Pos      31           /**< \brief (TAL_SFLAGSET) Inter-Process Signal 31 */
#define TAL_SFLAGSET_IPS31          (_U_(1) << TAL_SFLAGSET_IPS31_Pos)
#define TAL_SFLAGSET_IPS_Pos        0            /**< \brief (TAL_SFLAGSET) Inter-Process Signal x */
#define TAL_SFLAGSET_IPS_Msk        (_U_(0xFFFFFFFF) << TAL_SFLAGSET_IPS_Pos)
#define TAL_SFLAGSET_IPS(value)     (TAL_SFLAGSET_IPS_Msk & ((value) << TAL_SFLAGSET_IPS_Pos))
#define TAL_SFLAGSET_MASK           _U_(0xFFFFFFFF) /**< \brief (TAL_SFLAGSET) MASK Register */

/* -------- TAL_SFLAG : (TAL Offset: 0x230) (R/  32) Inter-Process Signal Flag -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t IPS0:1;           /*!< bit:      0  Inter-Process Signal 0             */
    uint32_t IPS1:1;           /*!< bit:      1  Inter-Process Signal 1             */
    uint32_t IPS2:1;           /*!< bit:      2  Inter-Process Signal 2             */
    uint32_t IPS3:1;           /*!< bit:      3  Inter-Process Signal 3             */
    uint32_t IPS4:1;           /*!< bit:      4  Inter-Process Signal 4             */
    uint32_t IPS5:1;           /*!< bit:      5  Inter-Process Signal 5             */
    uint32_t IPS6:1;           /*!< bit:      6  Inter-Process Signal 6             */
    uint32_t IPS7:1;           /*!< bit:      7  Inter-Process Signal 7             */
    uint32_t IPS8:1;           /*!< bit:      8  Inter-Process Signal 8             */
    uint32_t IPS9:1;           /*!< bit:      9  Inter-Process Signal 9             */
    uint32_t IPS10:1;          /*!< bit:     10  Inter-Process Signal 10            */
    uint32_t IPS11:1;          /*!< bit:     11  Inter-Process Signal 11            */
    uint32_t IPS12:1;          /*!< bit:     12  Inter-Process Signal 12            */
    uint32_t IPS13:1;          /*!< bit:     13  Inter-Process Signal 13            */
    uint32_t IPS14:1;          /*!< bit:     14  Inter-Process Signal 14            */
    uint32_t IPS15:1;          /*!< bit:     15  Inter-Process Signal 15            */
    uint32_t IPS16:1;          /*!< bit:     16  Inter-Process Signal 16            */
    uint32_t IPS17:1;          /*!< bit:     17  Inter-Process Signal 17            */
    uint32_t IPS18:1;          /*!< bit:     18  Inter-Process Signal 18            */
    uint32_t IPS19:1;          /*!< bit:     19  Inter-Process Signal 19            */
    uint32_t IPS20:1;          /*!< bit:     20  Inter-Process Signal 20            */
    uint32_t IPS21:1;          /*!< bit:     21  Inter-Process Signal 21            */
    uint32_t IPS22:1;          /*!< bit:     22  Inter-Process Signal 22            */
    uint32_t IPS23:1;          /*!< bit:     23  Inter-Process Signal 23            */
    uint32_t IPS24:1;          /*!< bit:     24  Inter-Process Signal 24            */
    uint32_t IPS25:1;          /*!< bit:     25  Inter-Process Signal 25            */
    uint32_t IPS26:1;          /*!< bit:     26  Inter-Process Signal 26            */
    uint32_t IPS27:1;          /*!< bit:     27  Inter-Process Signal 27            */
    uint32_t IPS28:1;          /*!< bit:     28  Inter-Process Signal 28            */
    uint32_t IPS29:1;          /*!< bit:     29  Inter-Process Signal 29            */
    uint32_t IPS30:1;          /*!< bit:     30  Inter-Process Signal 30            */
    uint32_t IPS31:1;          /*!< bit:     31  Inter-Process Signal 31            */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t IPS:32;           /*!< bit:  0..31  Inter-Process Signal x             */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TAL_SFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_SFLAG_OFFSET            0x230        /**< \brief (TAL_SFLAG offset) Inter-Process Signal Flag */
#define TAL_SFLAG_RESETVALUE        _U_(0x00000000) /**< \brief (TAL_SFLAG reset_value) Inter-Process Signal Flag */

#define TAL_SFLAG_IPS0_Pos          0            /**< \brief (TAL_SFLAG) Inter-Process Signal 0 */
#define TAL_SFLAG_IPS0              (_U_(1) << TAL_SFLAG_IPS0_Pos)
#define TAL_SFLAG_IPS1_Pos          1            /**< \brief (TAL_SFLAG) Inter-Process Signal 1 */
#define TAL_SFLAG_IPS1              (_U_(1) << TAL_SFLAG_IPS1_Pos)
#define TAL_SFLAG_IPS2_Pos          2            /**< \brief (TAL_SFLAG) Inter-Process Signal 2 */
#define TAL_SFLAG_IPS2              (_U_(1) << TAL_SFLAG_IPS2_Pos)
#define TAL_SFLAG_IPS3_Pos          3            /**< \brief (TAL_SFLAG) Inter-Process Signal 3 */
#define TAL_SFLAG_IPS3              (_U_(1) << TAL_SFLAG_IPS3_Pos)
#define TAL_SFLAG_IPS4_Pos          4            /**< \brief (TAL_SFLAG) Inter-Process Signal 4 */
#define TAL_SFLAG_IPS4              (_U_(1) << TAL_SFLAG_IPS4_Pos)
#define TAL_SFLAG_IPS5_Pos          5            /**< \brief (TAL_SFLAG) Inter-Process Signal 5 */
#define TAL_SFLAG_IPS5              (_U_(1) << TAL_SFLAG_IPS5_Pos)
#define TAL_SFLAG_IPS6_Pos          6            /**< \brief (TAL_SFLAG) Inter-Process Signal 6 */
#define TAL_SFLAG_IPS6              (_U_(1) << TAL_SFLAG_IPS6_Pos)
#define TAL_SFLAG_IPS7_Pos          7            /**< \brief (TAL_SFLAG) Inter-Process Signal 7 */
#define TAL_SFLAG_IPS7              (_U_(1) << TAL_SFLAG_IPS7_Pos)
#define TAL_SFLAG_IPS8_Pos          8            /**< \brief (TAL_SFLAG) Inter-Process Signal 8 */
#define TAL_SFLAG_IPS8              (_U_(1) << TAL_SFLAG_IPS8_Pos)
#define TAL_SFLAG_IPS9_Pos          9            /**< \brief (TAL_SFLAG) Inter-Process Signal 9 */
#define TAL_SFLAG_IPS9              (_U_(1) << TAL_SFLAG_IPS9_Pos)
#define TAL_SFLAG_IPS10_Pos         10           /**< \brief (TAL_SFLAG) Inter-Process Signal 10 */
#define TAL_SFLAG_IPS10             (_U_(1) << TAL_SFLAG_IPS10_Pos)
#define TAL_SFLAG_IPS11_Pos         11           /**< \brief (TAL_SFLAG) Inter-Process Signal 11 */
#define TAL_SFLAG_IPS11             (_U_(1) << TAL_SFLAG_IPS11_Pos)
#define TAL_SFLAG_IPS12_Pos         12           /**< \brief (TAL_SFLAG) Inter-Process Signal 12 */
#define TAL_SFLAG_IPS12             (_U_(1) << TAL_SFLAG_IPS12_Pos)
#define TAL_SFLAG_IPS13_Pos         13           /**< \brief (TAL_SFLAG) Inter-Process Signal 13 */
#define TAL_SFLAG_IPS13             (_U_(1) << TAL_SFLAG_IPS13_Pos)
#define TAL_SFLAG_IPS14_Pos         14           /**< \brief (TAL_SFLAG) Inter-Process Signal 14 */
#define TAL_SFLAG_IPS14             (_U_(1) << TAL_SFLAG_IPS14_Pos)
#define TAL_SFLAG_IPS15_Pos         15           /**< \brief (TAL_SFLAG) Inter-Process Signal 15 */
#define TAL_SFLAG_IPS15             (_U_(1) << TAL_SFLAG_IPS15_Pos)
#define TAL_SFLAG_IPS16_Pos         16           /**< \brief (TAL_SFLAG) Inter-Process Signal 16 */
#define TAL_SFLAG_IPS16             (_U_(1) << TAL_SFLAG_IPS16_Pos)
#define TAL_SFLAG_IPS17_Pos         17           /**< \brief (TAL_SFLAG) Inter-Process Signal 17 */
#define TAL_SFLAG_IPS17             (_U_(1) << TAL_SFLAG_IPS17_Pos)
#define TAL_SFLAG_IPS18_Pos         18           /**< \brief (TAL_SFLAG) Inter-Process Signal 18 */
#define TAL_SFLAG_IPS18             (_U_(1) << TAL_SFLAG_IPS18_Pos)
#define TAL_SFLAG_IPS19_Pos         19           /**< \brief (TAL_SFLAG) Inter-Process Signal 19 */
#define TAL_SFLAG_IPS19             (_U_(1) << TAL_SFLAG_IPS19_Pos)
#define TAL_SFLAG_IPS20_Pos         20           /**< \brief (TAL_SFLAG) Inter-Process Signal 20 */
#define TAL_SFLAG_IPS20             (_U_(1) << TAL_SFLAG_IPS20_Pos)
#define TAL_SFLAG_IPS21_Pos         21           /**< \brief (TAL_SFLAG) Inter-Process Signal 21 */
#define TAL_SFLAG_IPS21             (_U_(1) << TAL_SFLAG_IPS21_Pos)
#define TAL_SFLAG_IPS22_Pos         22           /**< \brief (TAL_SFLAG) Inter-Process Signal 22 */
#define TAL_SFLAG_IPS22             (_U_(1) << TAL_SFLAG_IPS22_Pos)
#define TAL_SFLAG_IPS23_Pos         23           /**< \brief (TAL_SFLAG) Inter-Process Signal 23 */
#define TAL_SFLAG_IPS23             (_U_(1) << TAL_SFLAG_IPS23_Pos)
#define TAL_SFLAG_IPS24_Pos         24           /**< \brief (TAL_SFLAG) Inter-Process Signal 24 */
#define TAL_SFLAG_IPS24             (_U_(1) << TAL_SFLAG_IPS24_Pos)
#define TAL_SFLAG_IPS25_Pos         25           /**< \brief (TAL_SFLAG) Inter-Process Signal 25 */
#define TAL_SFLAG_IPS25             (_U_(1) << TAL_SFLAG_IPS25_Pos)
#define TAL_SFLAG_IPS26_Pos         26           /**< \brief (TAL_SFLAG) Inter-Process Signal 26 */
#define TAL_SFLAG_IPS26             (_U_(1) << TAL_SFLAG_IPS26_Pos)
#define TAL_SFLAG_IPS27_Pos         27           /**< \brief (TAL_SFLAG) Inter-Process Signal 27 */
#define TAL_SFLAG_IPS27             (_U_(1) << TAL_SFLAG_IPS27_Pos)
#define TAL_SFLAG_IPS28_Pos         28           /**< \brief (TAL_SFLAG) Inter-Process Signal 28 */
#define TAL_SFLAG_IPS28             (_U_(1) << TAL_SFLAG_IPS28_Pos)
#define TAL_SFLAG_IPS29_Pos         29           /**< \brief (TAL_SFLAG) Inter-Process Signal 29 */
#define TAL_SFLAG_IPS29             (_U_(1) << TAL_SFLAG_IPS29_Pos)
#define TAL_SFLAG_IPS30_Pos         30           /**< \brief (TAL_SFLAG) Inter-Process Signal 30 */
#define TAL_SFLAG_IPS30             (_U_(1) << TAL_SFLAG_IPS30_Pos)
#define TAL_SFLAG_IPS31_Pos         31           /**< \brief (TAL_SFLAG) Inter-Process Signal 31 */
#define TAL_SFLAG_IPS31             (_U_(1) << TAL_SFLAG_IPS31_Pos)
#define TAL_SFLAG_IPS_Pos           0            /**< \brief (TAL_SFLAG) Inter-Process Signal x */
#define TAL_SFLAG_IPS_Msk           (_U_(0xFFFFFFFF) << TAL_SFLAG_IPS_Pos)
#define TAL_SFLAG_IPS(value)        (TAL_SFLAG_IPS_Msk & ((value) << TAL_SFLAG_IPS_Pos))
#define TAL_SFLAG_MASK              _U_(0xFFFFFFFF) /**< \brief (TAL_SFLAG) MASK Register */

/* -------- TAL_SFLAGCLRR : (TAL Offset: 0x300) (R/W  8) Inter-Process Signal Flag Bit n -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  IPS:1;            /*!< bit:      0  Inter-Process Signal n             */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TAL_SFLAGCLRR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TAL_SFLAGCLRR_OFFSET        0x300        /**< \brief (TAL_SFLAGCLRR offset) Inter-Process Signal Flag Bit n */
#define TAL_SFLAGCLRR_RESETVALUE    _U_(0x00)    /**< \brief (TAL_SFLAGCLRR reset_value) Inter-Process Signal Flag Bit n */

#define TAL_SFLAGCLRR_IPS_Pos       0            /**< \brief (TAL_SFLAGCLRR) Inter-Process Signal n */
#define TAL_SFLAGCLRR_IPS           (_U_(0x1) << TAL_SFLAGCLRR_IPS_Pos)
#define TAL_SFLAGCLRR_MASK          _U_(0x01)    /**< \brief (TAL_SFLAGCLRR) MASK Register */

/** \brief TalCpuirqs hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __I  TAL_CPUIRQS_Type          CPUIRQS[5];  /**< \brief Offset: 0x000 (R/  32) Interrupt Status m for CPU n */
       RoReg8                    Reserved1[0xC];
} TalCpuirqs;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief TalCtis hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO TAL_CTICTRLA_Type         CTICTRLA;    /**< \brief Offset: 0x000 (R/W  8) Cross-Trigger Interface n Control A */
  __IO TAL_CTIMASK_Type          CTIMASK;     /**< \brief Offset: 0x001 (R/W  8) Cross-Trigger Interface n Mask */
} TalCtis;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief TalSmasks hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO TAL_SMASK_Type            SMASK[2];    /**< \brief Offset: 0x000 (R/W 32) Inter-Process Signal Mask m for CPU n */
} TalSmasks;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief TAL hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO TAL_CTRLA_Type            CTRLA;       /**< \brief Offset: 0x000 (R/W  8) Control A */
  __IO TAL_EXTCTRL_Type          EXTCTRL;     /**< \brief Offset: 0x001 (R/W  8) External Break Control */
       RoReg8                    Reserved1[0x2];
  __IO TAL_EVCTRL_Type           EVCTRL;      /**< \brief Offset: 0x004 (R/W 16) Event Control */
       RoReg8                    Reserved2[0x2];
  __IO TAL_INTENCLR_Type         INTENCLR;    /**< \brief Offset: 0x008 (R/W  8) Interrupt Enable Clear */
  __IO TAL_INTENSET_Type         INTENSET;    /**< \brief Offset: 0x009 (R/W  8) Interrupt Enable Set */
  __IO TAL_INTFLAG_Type          INTFLAG;     /**< \brief Offset: 0x00A (R/W  8) Interrupt Flag Status and Clear */
  __IO TAL_GLOBMASK_Type         GLOBMASK;    /**< \brief Offset: 0x00B (R/W  8) Global Break Requests Mask */
  __O  TAL_HALT_Type             HALT;        /**< \brief Offset: 0x00C ( /W  8) Debug Halt Request */
  __O  TAL_RESTART_Type          RESTART;     /**< \brief Offset: 0x00D ( /W  8) Debug Restart Request */
  __I  TAL_BRKSTATUS_Type        BRKSTATUS;   /**< \brief Offset: 0x00E (R/  16) Break Request Status */
       TalCtis                   Ctis[4];     /**< \brief Offset: 0x010 TalCtis groups [CTI_NUM] */
       RoReg8                    Reserved3[0x8];
  __I  TAL_INTSTATUS_Type        INTSTATUS[137]; /**< \brief Offset: 0x020 (R/   8) Interrupt n Status */
       RoReg8                    Reserved4[0x67];
  __IO TAL_DMACPUSEL0_Type       DMACPUSEL0;  /**< \brief Offset: 0x110 (R/W 32) DMA Channel Interrupts CPU Select 0 */
  __IO TAL_DMACPUSEL1_Type       DMACPUSEL1;  /**< \brief Offset: 0x114 (R/W 32) DMA Channel Interrupts CPU Select 1 */
  __IO TAL_EVCPUSEL0_Type        EVCPUSEL0;   /**< \brief Offset: 0x118 (R/W 32) EVSYS Channel Interrupts CPU Select 0 */
       RoReg8                    Reserved5[0x4];
  __IO TAL_EICCPUSEL0_Type       EICCPUSEL0;  /**< \brief Offset: 0x120 (R/W 32) EIC External Interrupts CPU Select 0 */
       RoReg8                    Reserved6[0x4];
  __IO TAL_INTCPUSEL0_Type       INTCPUSEL0;  /**< \brief Offset: 0x128 (R/W 32) Interrupts CPU Select 0 */
  __IO TAL_INTCPUSEL1_Type       INTCPUSEL1;  /**< \brief Offset: 0x12C (R/W 32) Interrupts CPU Select 1 */
  __IO TAL_INTCPUSEL2_Type       INTCPUSEL2;  /**< \brief Offset: 0x130 (R/W 32) Interrupts CPU Select 2 */
  __IO TAL_INTCPUSEL3_Type       INTCPUSEL3;  /**< \brief Offset: 0x134 (R/W 32) Interrupts CPU Select 3 */
  __IO TAL_INTCPUSEL4_Type       INTCPUSEL4;  /**< \brief Offset: 0x138 (R/W 32) Interrupts CPU Select 4 */
  __IO TAL_INTCPUSEL5_Type       INTCPUSEL5;  /**< \brief Offset: 0x13C (R/W 32) Interrupts CPU Select 5 */
  __IO TAL_INTCPUSEL6_Type       INTCPUSEL6;  /**< \brief Offset: 0x140 (R/W 32) Interrupts CPU Select 6 */
  __IO TAL_INTCPUSEL7_Type       INTCPUSEL7;  /**< \brief Offset: 0x144 (R/W 32) Interrupts CPU Select 7 */
  __IO TAL_INTCPUSEL8_Type       INTCPUSEL8;  /**< \brief Offset: 0x148 (R/W 32) Interrupts CPU Select 8 */
       RoReg8                    Reserved7[0x18];
  __IO TAL_IRQTRIG_Type          IRQTRIG;     /**< \brief Offset: 0x164 (R/W 32) Interrupt Trigger */
  __IO TAL_IRQMON_Type           IRQMON[1];   /**< \brief Offset: 0x168 (R/W 16) Interrupt Monitor Select */
       RoReg8                    Reserved8[0x16];
       TalCpuirqs                Cpuirqs[2];  /**< \brief Offset: 0x180 TalCpuirqs groups [CPU_NUM] */
       RoReg8                    Reserved9[0x40];
       TalSmasks                 Smasks[2];   /**< \brief Offset: 0x200 TalSmasks groups [CPU_NUM] */
       RoReg8                    Reserved10[0x10];
  __O  TAL_SFLAGCLR_Type         SFLAGCLR[2]; /**< \brief Offset: 0x220 ( /W 32) Inter-Process Signal Flag Clear */
  __O  TAL_SFLAGSET_Type         SFLAGSET[2]; /**< \brief Offset: 0x228 ( /W 32) Inter-Process Signal Flag Set */
  __I  TAL_SFLAG_Type            SFLAG[2];    /**< \brief Offset: 0x230 (R/  32) Inter-Process Signal Flag */
       RoReg8                    Reserved11[0xC8];
  __IO TAL_SFLAGCLRR_Type        SFLAGCLRR[64]; /**< \brief Offset: 0x300 (R/W  8) Inter-Process Signal Flag Bit n */
} Tal;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_TAL_COMPONENT_ */
