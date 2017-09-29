/**
 * \file
 *
 * \brief Component description for PM
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

#ifndef _SAMD51_PM_COMPONENT_
#define _SAMD51_PM_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR PM */
/* ========================================================================== */
/** \addtogroup SAMD51_PM Power Manager */
/*@{*/

#define PM_U2406
#define REV_PM                      0x100

/* -------- PM_CTRLA : (PM Offset: 0x00) (R/W  8) Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :2;               /*!< bit:  0.. 1  Reserved                           */
    uint8_t  IORET:1;          /*!< bit:      2  I/O Retention                      */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_CTRLA_OFFSET             0x00         /**< \brief (PM_CTRLA offset) Control A */
#define PM_CTRLA_RESETVALUE         _U_(0x00)    /**< \brief (PM_CTRLA reset_value) Control A */

#define PM_CTRLA_IORET_Pos          2            /**< \brief (PM_CTRLA) I/O Retention */
#define PM_CTRLA_IORET              (_U_(0x1) << PM_CTRLA_IORET_Pos)
#define PM_CTRLA_MASK               _U_(0x04)    /**< \brief (PM_CTRLA) MASK Register */

/* -------- PM_SLEEPCFG : (PM Offset: 0x01) (R/W  8) Sleep Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SLEEPMODE:3;      /*!< bit:  0.. 2  Sleep Mode                         */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_SLEEPCFG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_SLEEPCFG_OFFSET          0x01         /**< \brief (PM_SLEEPCFG offset) Sleep Configuration */
#define PM_SLEEPCFG_RESETVALUE      _U_(0x02)    /**< \brief (PM_SLEEPCFG reset_value) Sleep Configuration */

#define PM_SLEEPCFG_SLEEPMODE_Pos   0            /**< \brief (PM_SLEEPCFG) Sleep Mode */
#define PM_SLEEPCFG_SLEEPMODE_Msk   (_U_(0x7) << PM_SLEEPCFG_SLEEPMODE_Pos)
#define PM_SLEEPCFG_SLEEPMODE(value) (PM_SLEEPCFG_SLEEPMODE_Msk & ((value) << PM_SLEEPCFG_SLEEPMODE_Pos))
#define   PM_SLEEPCFG_SLEEPMODE_IDLE0_Val _U_(0x0)   /**< \brief (PM_SLEEPCFG) CPU clock is OFF */
#define   PM_SLEEPCFG_SLEEPMODE_IDLE1_Val _U_(0x1)   /**< \brief (PM_SLEEPCFG) AHB clock is OFF */
#define   PM_SLEEPCFG_SLEEPMODE_IDLE2_Val _U_(0x2)   /**< \brief (PM_SLEEPCFG) APB clock are OFF */
#define   PM_SLEEPCFG_SLEEPMODE_STANDBY_Val _U_(0x4)   /**< \brief (PM_SLEEPCFG) All Clocks are OFF */
#define   PM_SLEEPCFG_SLEEPMODE_HIBERNATE_Val _U_(0x5)   /**< \brief (PM_SLEEPCFG) Backup domain is ON as well as some PDRAMs */
#define   PM_SLEEPCFG_SLEEPMODE_BACKUP_Val _U_(0x6)   /**< \brief (PM_SLEEPCFG) Only Backup domain is powered ON */
#define   PM_SLEEPCFG_SLEEPMODE_OFF_Val   _U_(0x7)   /**< \brief (PM_SLEEPCFG) All power domains are powered OFF */
#define PM_SLEEPCFG_SLEEPMODE_IDLE0 (PM_SLEEPCFG_SLEEPMODE_IDLE0_Val << PM_SLEEPCFG_SLEEPMODE_Pos)
#define PM_SLEEPCFG_SLEEPMODE_IDLE1 (PM_SLEEPCFG_SLEEPMODE_IDLE1_Val << PM_SLEEPCFG_SLEEPMODE_Pos)
#define PM_SLEEPCFG_SLEEPMODE_IDLE2 (PM_SLEEPCFG_SLEEPMODE_IDLE2_Val << PM_SLEEPCFG_SLEEPMODE_Pos)
#define PM_SLEEPCFG_SLEEPMODE_STANDBY (PM_SLEEPCFG_SLEEPMODE_STANDBY_Val << PM_SLEEPCFG_SLEEPMODE_Pos)
#define PM_SLEEPCFG_SLEEPMODE_HIBERNATE (PM_SLEEPCFG_SLEEPMODE_HIBERNATE_Val << PM_SLEEPCFG_SLEEPMODE_Pos)
#define PM_SLEEPCFG_SLEEPMODE_BACKUP (PM_SLEEPCFG_SLEEPMODE_BACKUP_Val << PM_SLEEPCFG_SLEEPMODE_Pos)
#define PM_SLEEPCFG_SLEEPMODE_OFF   (PM_SLEEPCFG_SLEEPMODE_OFF_Val << PM_SLEEPCFG_SLEEPMODE_Pos)
#define PM_SLEEPCFG_MASK            _U_(0x07)    /**< \brief (PM_SLEEPCFG) MASK Register */

/* -------- PM_INTENCLR : (PM Offset: 0x04) (R/W  8) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SLEEPRDY:1;       /*!< bit:      0  Sleep Mode Entry Ready Enable      */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_INTENCLR_OFFSET          0x04         /**< \brief (PM_INTENCLR offset) Interrupt Enable Clear */
#define PM_INTENCLR_RESETVALUE      _U_(0x00)    /**< \brief (PM_INTENCLR reset_value) Interrupt Enable Clear */

#define PM_INTENCLR_SLEEPRDY_Pos    0            /**< \brief (PM_INTENCLR) Sleep Mode Entry Ready Enable */
#define PM_INTENCLR_SLEEPRDY        (_U_(0x1) << PM_INTENCLR_SLEEPRDY_Pos)
#define PM_INTENCLR_MASK            _U_(0x01)    /**< \brief (PM_INTENCLR) MASK Register */

/* -------- PM_INTENSET : (PM Offset: 0x05) (R/W  8) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SLEEPRDY:1;       /*!< bit:      0  Sleep Mode Entry Ready Enable      */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_INTENSET_OFFSET          0x05         /**< \brief (PM_INTENSET offset) Interrupt Enable Set */
#define PM_INTENSET_RESETVALUE      _U_(0x00)    /**< \brief (PM_INTENSET reset_value) Interrupt Enable Set */

#define PM_INTENSET_SLEEPRDY_Pos    0            /**< \brief (PM_INTENSET) Sleep Mode Entry Ready Enable */
#define PM_INTENSET_SLEEPRDY        (_U_(0x1) << PM_INTENSET_SLEEPRDY_Pos)
#define PM_INTENSET_MASK            _U_(0x01)    /**< \brief (PM_INTENSET) MASK Register */

/* -------- PM_INTFLAG : (PM Offset: 0x06) (R/W  8) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint8_t  SLEEPRDY:1;       /*!< bit:      0  Sleep Mode Entry Ready             */
    __I uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_INTFLAG_OFFSET           0x06         /**< \brief (PM_INTFLAG offset) Interrupt Flag Status and Clear */
#define PM_INTFLAG_RESETVALUE       _U_(0x00)    /**< \brief (PM_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define PM_INTFLAG_SLEEPRDY_Pos     0            /**< \brief (PM_INTFLAG) Sleep Mode Entry Ready */
#define PM_INTFLAG_SLEEPRDY         (_U_(0x1) << PM_INTFLAG_SLEEPRDY_Pos)
#define PM_INTFLAG_MASK             _U_(0x01)    /**< \brief (PM_INTFLAG) MASK Register */

/* -------- PM_STDBYCFG : (PM Offset: 0x08) (R/W  8) Standby Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  RAMCFG:2;         /*!< bit:  0.. 1  Ram Configuration                  */
    uint8_t  :2;               /*!< bit:  2.. 3  Reserved                           */
    uint8_t  FASTWKUP:2;       /*!< bit:  4.. 5  Fast Wakeup                        */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_STDBYCFG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_STDBYCFG_OFFSET          0x08         /**< \brief (PM_STDBYCFG offset) Standby Configuration */
#define PM_STDBYCFG_RESETVALUE      _U_(0x00)    /**< \brief (PM_STDBYCFG reset_value) Standby Configuration */

#define PM_STDBYCFG_RAMCFG_Pos      0            /**< \brief (PM_STDBYCFG) Ram Configuration */
#define PM_STDBYCFG_RAMCFG_Msk      (_U_(0x3) << PM_STDBYCFG_RAMCFG_Pos)
#define PM_STDBYCFG_RAMCFG(value)   (PM_STDBYCFG_RAMCFG_Msk & ((value) << PM_STDBYCFG_RAMCFG_Pos))
#define   PM_STDBYCFG_RAMCFG_RET_Val      _U_(0x0)   /**< \brief (PM_STDBYCFG) All the RAMs are retained */
#define   PM_STDBYCFG_RAMCFG_PARTIAL_Val  _U_(0x1)   /**< \brief (PM_STDBYCFG) Only the first 32K bytes are retained */
#define   PM_STDBYCFG_RAMCFG_OFF_Val      _U_(0x2)   /**< \brief (PM_STDBYCFG) All the RAMs are OFF */
#define PM_STDBYCFG_RAMCFG_RET      (PM_STDBYCFG_RAMCFG_RET_Val    << PM_STDBYCFG_RAMCFG_Pos)
#define PM_STDBYCFG_RAMCFG_PARTIAL  (PM_STDBYCFG_RAMCFG_PARTIAL_Val << PM_STDBYCFG_RAMCFG_Pos)
#define PM_STDBYCFG_RAMCFG_OFF      (PM_STDBYCFG_RAMCFG_OFF_Val    << PM_STDBYCFG_RAMCFG_Pos)
#define PM_STDBYCFG_FASTWKUP_Pos    4            /**< \brief (PM_STDBYCFG) Fast Wakeup */
#define PM_STDBYCFG_FASTWKUP_Msk    (_U_(0x3) << PM_STDBYCFG_FASTWKUP_Pos)
#define PM_STDBYCFG_FASTWKUP(value) (PM_STDBYCFG_FASTWKUP_Msk & ((value) << PM_STDBYCFG_FASTWKUP_Pos))
#define PM_STDBYCFG_MASK            _U_(0x33)    /**< \brief (PM_STDBYCFG) MASK Register */

/* -------- PM_HIBCFG : (PM Offset: 0x09) (R/W  8) Hibernate Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  RAMCFG:2;         /*!< bit:  0.. 1  Ram Configuration                  */
    uint8_t  BRAMCFG:2;        /*!< bit:  2.. 3  Backup Ram Configuration           */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_HIBCFG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_HIBCFG_OFFSET            0x09         /**< \brief (PM_HIBCFG offset) Hibernate Configuration */
#define PM_HIBCFG_RESETVALUE        _U_(0x00)    /**< \brief (PM_HIBCFG reset_value) Hibernate Configuration */

#define PM_HIBCFG_RAMCFG_Pos        0            /**< \brief (PM_HIBCFG) Ram Configuration */
#define PM_HIBCFG_RAMCFG_Msk        (_U_(0x3) << PM_HIBCFG_RAMCFG_Pos)
#define PM_HIBCFG_RAMCFG(value)     (PM_HIBCFG_RAMCFG_Msk & ((value) << PM_HIBCFG_RAMCFG_Pos))
#define PM_HIBCFG_BRAMCFG_Pos       2            /**< \brief (PM_HIBCFG) Backup Ram Configuration */
#define PM_HIBCFG_BRAMCFG_Msk       (_U_(0x3) << PM_HIBCFG_BRAMCFG_Pos)
#define PM_HIBCFG_BRAMCFG(value)    (PM_HIBCFG_BRAMCFG_Msk & ((value) << PM_HIBCFG_BRAMCFG_Pos))
#define PM_HIBCFG_MASK              _U_(0x0F)    /**< \brief (PM_HIBCFG) MASK Register */

/* -------- PM_BKUPCFG : (PM Offset: 0x0A) (R/W  8) Backup Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  BRAMCFG:2;        /*!< bit:  0.. 1  Ram Configuration                  */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_BKUPCFG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_BKUPCFG_OFFSET           0x0A         /**< \brief (PM_BKUPCFG offset) Backup Configuration */
#define PM_BKUPCFG_RESETVALUE       _U_(0x00)    /**< \brief (PM_BKUPCFG reset_value) Backup Configuration */

#define PM_BKUPCFG_BRAMCFG_Pos      0            /**< \brief (PM_BKUPCFG) Ram Configuration */
#define PM_BKUPCFG_BRAMCFG_Msk      (_U_(0x3) << PM_BKUPCFG_BRAMCFG_Pos)
#define PM_BKUPCFG_BRAMCFG(value)   (PM_BKUPCFG_BRAMCFG_Msk & ((value) << PM_BKUPCFG_BRAMCFG_Pos))
#define PM_BKUPCFG_MASK             _U_(0x03)    /**< \brief (PM_BKUPCFG) MASK Register */

/* -------- PM_PWSAKDLY : (PM Offset: 0x12) (R/W  8) Power Switch Acknowledge Delay -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DLYVAL:7;         /*!< bit:  0.. 6  Delay Value                        */
    uint8_t  IGNACK:1;         /*!< bit:      7  Ignore Acknowledge                 */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_PWSAKDLY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_PWSAKDLY_OFFSET          0x12         /**< \brief (PM_PWSAKDLY offset) Power Switch Acknowledge Delay */
#define PM_PWSAKDLY_RESETVALUE      _U_(0x00)    /**< \brief (PM_PWSAKDLY reset_value) Power Switch Acknowledge Delay */

#define PM_PWSAKDLY_DLYVAL_Pos      0            /**< \brief (PM_PWSAKDLY) Delay Value */
#define PM_PWSAKDLY_DLYVAL_Msk      (_U_(0x7F) << PM_PWSAKDLY_DLYVAL_Pos)
#define PM_PWSAKDLY_DLYVAL(value)   (PM_PWSAKDLY_DLYVAL_Msk & ((value) << PM_PWSAKDLY_DLYVAL_Pos))
#define PM_PWSAKDLY_IGNACK_Pos      7            /**< \brief (PM_PWSAKDLY) Ignore Acknowledge */
#define PM_PWSAKDLY_IGNACK          (_U_(0x1) << PM_PWSAKDLY_IGNACK_Pos)
#define PM_PWSAKDLY_MASK            _U_(0xFF)    /**< \brief (PM_PWSAKDLY) MASK Register */

/** \brief PM hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO PM_CTRLA_Type             CTRLA;       /**< \brief Offset: 0x00 (R/W  8) Control A */
  __IO PM_SLEEPCFG_Type          SLEEPCFG;    /**< \brief Offset: 0x01 (R/W  8) Sleep Configuration */
       RoReg8                    Reserved1[0x2];
  __IO PM_INTENCLR_Type          INTENCLR;    /**< \brief Offset: 0x04 (R/W  8) Interrupt Enable Clear */
  __IO PM_INTENSET_Type          INTENSET;    /**< \brief Offset: 0x05 (R/W  8) Interrupt Enable Set */
  __IO PM_INTFLAG_Type           INTFLAG;     /**< \brief Offset: 0x06 (R/W  8) Interrupt Flag Status and Clear */
       RoReg8                    Reserved2[0x1];
  __IO PM_STDBYCFG_Type          STDBYCFG;    /**< \brief Offset: 0x08 (R/W  8) Standby Configuration */
  __IO PM_HIBCFG_Type            HIBCFG;      /**< \brief Offset: 0x09 (R/W  8) Hibernate Configuration */
  __IO PM_BKUPCFG_Type           BKUPCFG;     /**< \brief Offset: 0x0A (R/W  8) Backup Configuration */
       RoReg8                    Reserved3[0x7];
  __IO PM_PWSAKDLY_Type          PWSAKDLY;    /**< \brief Offset: 0x12 (R/W  8) Power Switch Acknowledge Delay */
} Pm;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_PM_COMPONENT_ */
