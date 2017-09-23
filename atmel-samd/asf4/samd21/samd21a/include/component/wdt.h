/**
 * \file
 *
 * \brief Component description for WDT
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

#ifndef _SAMD21_WDT_COMPONENT_
#define _SAMD21_WDT_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR WDT */
/* ========================================================================== */
/** \addtogroup SAMD21_WDT Watchdog Timer */
/*@{*/

#define WDT_U2203
#define REV_WDT                     0x200

/* -------- WDT_CTRL : (WDT Offset: 0x0) (R/W  8) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :1;               /*!< bit:      0  Reserved                           */
    uint8_t  ENABLE:1;         /*!< bit:      1  Enable                             */
    uint8_t  WEN:1;            /*!< bit:      2  Watchdog Timer Window Mode Enable  */
    uint8_t  :4;               /*!< bit:  3.. 6  Reserved                           */
    uint8_t  ALWAYSON:1;       /*!< bit:      7  Always-On                          */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} WDT_CTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define WDT_CTRL_OFFSET             0x0          /**< \brief (WDT_CTRL offset) Control */
#define WDT_CTRL_RESETVALUE         _U(0x00)     /**< \brief (WDT_CTRL reset_value) Control */

#define WDT_CTRL_ENABLE_Pos         1            /**< \brief (WDT_CTRL) Enable */
#define WDT_CTRL_ENABLE             (_U(0x1) << WDT_CTRL_ENABLE_Pos)
#define WDT_CTRL_WEN_Pos            2            /**< \brief (WDT_CTRL) Watchdog Timer Window Mode Enable */
#define WDT_CTRL_WEN                (_U(0x1) << WDT_CTRL_WEN_Pos)
#define WDT_CTRL_ALWAYSON_Pos       7            /**< \brief (WDT_CTRL) Always-On */
#define WDT_CTRL_ALWAYSON           (_U(0x1) << WDT_CTRL_ALWAYSON_Pos)
#define WDT_CTRL_MASK               _U(0x86)     /**< \brief (WDT_CTRL) MASK Register */

/* -------- WDT_CONFIG : (WDT Offset: 0x1) (R/W  8) Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  PER:4;            /*!< bit:  0.. 3  Time-Out Period                    */
    uint8_t  WINDOW:4;         /*!< bit:  4.. 7  Window Mode Time-Out Period        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} WDT_CONFIG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define WDT_CONFIG_OFFSET           0x1          /**< \brief (WDT_CONFIG offset) Configuration */
#define WDT_CONFIG_RESETVALUE       _U(0xBB)     /**< \brief (WDT_CONFIG reset_value) Configuration */

#define WDT_CONFIG_PER_Pos          0            /**< \brief (WDT_CONFIG) Time-Out Period */
#define WDT_CONFIG_PER_Msk          (_U(0xF) << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER(value)       (WDT_CONFIG_PER_Msk & ((value) << WDT_CONFIG_PER_Pos))
#define   WDT_CONFIG_PER_8_Val            _U(0x0)   /**< \brief (WDT_CONFIG) 8 clock cycles */
#define   WDT_CONFIG_PER_16_Val           _U(0x1)   /**< \brief (WDT_CONFIG) 16 clock cycles */
#define   WDT_CONFIG_PER_32_Val           _U(0x2)   /**< \brief (WDT_CONFIG) 32 clock cycles */
#define   WDT_CONFIG_PER_64_Val           _U(0x3)   /**< \brief (WDT_CONFIG) 64 clock cycles */
#define   WDT_CONFIG_PER_128_Val          _U(0x4)   /**< \brief (WDT_CONFIG) 128 clock cycles */
#define   WDT_CONFIG_PER_256_Val          _U(0x5)   /**< \brief (WDT_CONFIG) 256 clock cycles */
#define   WDT_CONFIG_PER_512_Val          _U(0x6)   /**< \brief (WDT_CONFIG) 512 clock cycles */
#define   WDT_CONFIG_PER_1K_Val           _U(0x7)   /**< \brief (WDT_CONFIG) 1024 clock cycles */
#define   WDT_CONFIG_PER_2K_Val           _U(0x8)   /**< \brief (WDT_CONFIG) 2048 clock cycles */
#define   WDT_CONFIG_PER_4K_Val           _U(0x9)   /**< \brief (WDT_CONFIG) 4096 clock cycles */
#define   WDT_CONFIG_PER_8K_Val           _U(0xA)   /**< \brief (WDT_CONFIG) 8192 clock cycles */
#define   WDT_CONFIG_PER_16K_Val          _U(0xB)   /**< \brief (WDT_CONFIG) 16384 clock cycles */
#define WDT_CONFIG_PER_8            (WDT_CONFIG_PER_8_Val          << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER_16           (WDT_CONFIG_PER_16_Val         << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER_32           (WDT_CONFIG_PER_32_Val         << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER_64           (WDT_CONFIG_PER_64_Val         << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER_128          (WDT_CONFIG_PER_128_Val        << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER_256          (WDT_CONFIG_PER_256_Val        << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER_512          (WDT_CONFIG_PER_512_Val        << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER_1K           (WDT_CONFIG_PER_1K_Val         << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER_2K           (WDT_CONFIG_PER_2K_Val         << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER_4K           (WDT_CONFIG_PER_4K_Val         << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER_8K           (WDT_CONFIG_PER_8K_Val         << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER_16K          (WDT_CONFIG_PER_16K_Val        << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_WINDOW_Pos       4            /**< \brief (WDT_CONFIG) Window Mode Time-Out Period */
#define WDT_CONFIG_WINDOW_Msk       (_U(0xF) << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW(value)    (WDT_CONFIG_WINDOW_Msk & ((value) << WDT_CONFIG_WINDOW_Pos))
#define   WDT_CONFIG_WINDOW_8_Val         _U(0x0)   /**< \brief (WDT_CONFIG) 8 clock cycles */
#define   WDT_CONFIG_WINDOW_16_Val        _U(0x1)   /**< \brief (WDT_CONFIG) 16 clock cycles */
#define   WDT_CONFIG_WINDOW_32_Val        _U(0x2)   /**< \brief (WDT_CONFIG) 32 clock cycles */
#define   WDT_CONFIG_WINDOW_64_Val        _U(0x3)   /**< \brief (WDT_CONFIG) 64 clock cycles */
#define   WDT_CONFIG_WINDOW_128_Val       _U(0x4)   /**< \brief (WDT_CONFIG) 128 clock cycles */
#define   WDT_CONFIG_WINDOW_256_Val       _U(0x5)   /**< \brief (WDT_CONFIG) 256 clock cycles */
#define   WDT_CONFIG_WINDOW_512_Val       _U(0x6)   /**< \brief (WDT_CONFIG) 512 clock cycles */
#define   WDT_CONFIG_WINDOW_1K_Val        _U(0x7)   /**< \brief (WDT_CONFIG) 1024 clock cycles */
#define   WDT_CONFIG_WINDOW_2K_Val        _U(0x8)   /**< \brief (WDT_CONFIG) 2048 clock cycles */
#define   WDT_CONFIG_WINDOW_4K_Val        _U(0x9)   /**< \brief (WDT_CONFIG) 4096 clock cycles */
#define   WDT_CONFIG_WINDOW_8K_Val        _U(0xA)   /**< \brief (WDT_CONFIG) 8192 clock cycles */
#define   WDT_CONFIG_WINDOW_16K_Val       _U(0xB)   /**< \brief (WDT_CONFIG) 16384 clock cycles */
#define WDT_CONFIG_WINDOW_8         (WDT_CONFIG_WINDOW_8_Val       << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW_16        (WDT_CONFIG_WINDOW_16_Val      << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW_32        (WDT_CONFIG_WINDOW_32_Val      << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW_64        (WDT_CONFIG_WINDOW_64_Val      << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW_128       (WDT_CONFIG_WINDOW_128_Val     << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW_256       (WDT_CONFIG_WINDOW_256_Val     << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW_512       (WDT_CONFIG_WINDOW_512_Val     << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW_1K        (WDT_CONFIG_WINDOW_1K_Val      << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW_2K        (WDT_CONFIG_WINDOW_2K_Val      << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW_4K        (WDT_CONFIG_WINDOW_4K_Val      << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW_8K        (WDT_CONFIG_WINDOW_8K_Val      << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW_16K       (WDT_CONFIG_WINDOW_16K_Val     << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_MASK             _U(0xFF)     /**< \brief (WDT_CONFIG) MASK Register */

/* -------- WDT_EWCTRL : (WDT Offset: 0x2) (R/W  8) Early Warning Interrupt Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  EWOFFSET:4;       /*!< bit:  0.. 3  Early Warning Interrupt Time Offset */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} WDT_EWCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define WDT_EWCTRL_OFFSET           0x2          /**< \brief (WDT_EWCTRL offset) Early Warning Interrupt Control */
#define WDT_EWCTRL_RESETVALUE       _U(0x0B)     /**< \brief (WDT_EWCTRL reset_value) Early Warning Interrupt Control */

#define WDT_EWCTRL_EWOFFSET_Pos     0            /**< \brief (WDT_EWCTRL) Early Warning Interrupt Time Offset */
#define WDT_EWCTRL_EWOFFSET_Msk     (_U(0xF) << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET(value)  (WDT_EWCTRL_EWOFFSET_Msk & ((value) << WDT_EWCTRL_EWOFFSET_Pos))
#define   WDT_EWCTRL_EWOFFSET_8_Val       _U(0x0)   /**< \brief (WDT_EWCTRL) 8 clock cycles */
#define   WDT_EWCTRL_EWOFFSET_16_Val      _U(0x1)   /**< \brief (WDT_EWCTRL) 16 clock cycles */
#define   WDT_EWCTRL_EWOFFSET_32_Val      _U(0x2)   /**< \brief (WDT_EWCTRL) 32 clock cycles */
#define   WDT_EWCTRL_EWOFFSET_64_Val      _U(0x3)   /**< \brief (WDT_EWCTRL) 64 clock cycles */
#define   WDT_EWCTRL_EWOFFSET_128_Val     _U(0x4)   /**< \brief (WDT_EWCTRL) 128 clock cycles */
#define   WDT_EWCTRL_EWOFFSET_256_Val     _U(0x5)   /**< \brief (WDT_EWCTRL) 256 clock cycles */
#define   WDT_EWCTRL_EWOFFSET_512_Val     _U(0x6)   /**< \brief (WDT_EWCTRL) 512 clock cycles */
#define   WDT_EWCTRL_EWOFFSET_1K_Val      _U(0x7)   /**< \brief (WDT_EWCTRL) 1024 clock cycles */
#define   WDT_EWCTRL_EWOFFSET_2K_Val      _U(0x8)   /**< \brief (WDT_EWCTRL) 2048 clock cycles */
#define   WDT_EWCTRL_EWOFFSET_4K_Val      _U(0x9)   /**< \brief (WDT_EWCTRL) 4096 clock cycles */
#define   WDT_EWCTRL_EWOFFSET_8K_Val      _U(0xA)   /**< \brief (WDT_EWCTRL) 8192 clock cycles */
#define   WDT_EWCTRL_EWOFFSET_16K_Val     _U(0xB)   /**< \brief (WDT_EWCTRL) 16384 clock cycles */
#define WDT_EWCTRL_EWOFFSET_8       (WDT_EWCTRL_EWOFFSET_8_Val     << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET_16      (WDT_EWCTRL_EWOFFSET_16_Val    << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET_32      (WDT_EWCTRL_EWOFFSET_32_Val    << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET_64      (WDT_EWCTRL_EWOFFSET_64_Val    << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET_128     (WDT_EWCTRL_EWOFFSET_128_Val   << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET_256     (WDT_EWCTRL_EWOFFSET_256_Val   << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET_512     (WDT_EWCTRL_EWOFFSET_512_Val   << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET_1K      (WDT_EWCTRL_EWOFFSET_1K_Val    << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET_2K      (WDT_EWCTRL_EWOFFSET_2K_Val    << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET_4K      (WDT_EWCTRL_EWOFFSET_4K_Val    << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET_8K      (WDT_EWCTRL_EWOFFSET_8K_Val    << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET_16K     (WDT_EWCTRL_EWOFFSET_16K_Val   << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_MASK             _U(0x0F)     /**< \brief (WDT_EWCTRL) MASK Register */

/* -------- WDT_INTENCLR : (WDT Offset: 0x4) (R/W  8) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  EW:1;             /*!< bit:      0  Early Warning Interrupt Enable     */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} WDT_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define WDT_INTENCLR_OFFSET         0x4          /**< \brief (WDT_INTENCLR offset) Interrupt Enable Clear */
#define WDT_INTENCLR_RESETVALUE     _U(0x00)     /**< \brief (WDT_INTENCLR reset_value) Interrupt Enable Clear */

#define WDT_INTENCLR_EW_Pos         0            /**< \brief (WDT_INTENCLR) Early Warning Interrupt Enable */
#define WDT_INTENCLR_EW             (_U(0x1) << WDT_INTENCLR_EW_Pos)
#define WDT_INTENCLR_MASK           _U(0x01)     /**< \brief (WDT_INTENCLR) MASK Register */

/* -------- WDT_INTENSET : (WDT Offset: 0x5) (R/W  8) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  EW:1;             /*!< bit:      0  Early Warning Interrupt Enable     */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} WDT_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define WDT_INTENSET_OFFSET         0x5          /**< \brief (WDT_INTENSET offset) Interrupt Enable Set */
#define WDT_INTENSET_RESETVALUE     _U(0x00)     /**< \brief (WDT_INTENSET reset_value) Interrupt Enable Set */

#define WDT_INTENSET_EW_Pos         0            /**< \brief (WDT_INTENSET) Early Warning Interrupt Enable */
#define WDT_INTENSET_EW             (_U(0x1) << WDT_INTENSET_EW_Pos)
#define WDT_INTENSET_MASK           _U(0x01)     /**< \brief (WDT_INTENSET) MASK Register */

/* -------- WDT_INTFLAG : (WDT Offset: 0x6) (R/W  8) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint8_t  EW:1;             /*!< bit:      0  Early Warning                      */
    __I uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} WDT_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define WDT_INTFLAG_OFFSET          0x6          /**< \brief (WDT_INTFLAG offset) Interrupt Flag Status and Clear */
#define WDT_INTFLAG_RESETVALUE      _U(0x00)     /**< \brief (WDT_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define WDT_INTFLAG_EW_Pos          0            /**< \brief (WDT_INTFLAG) Early Warning */
#define WDT_INTFLAG_EW              (_U(0x1) << WDT_INTFLAG_EW_Pos)
#define WDT_INTFLAG_MASK            _U(0x01)     /**< \brief (WDT_INTFLAG) MASK Register */

/* -------- WDT_STATUS : (WDT Offset: 0x7) (R/   8) Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :7;               /*!< bit:  0.. 6  Reserved                           */
    uint8_t  SYNCBUSY:1;       /*!< bit:      7  Synchronization Busy               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} WDT_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define WDT_STATUS_OFFSET           0x7          /**< \brief (WDT_STATUS offset) Status */
#define WDT_STATUS_RESETVALUE       _U(0x00)     /**< \brief (WDT_STATUS reset_value) Status */

#define WDT_STATUS_SYNCBUSY_Pos     7            /**< \brief (WDT_STATUS) Synchronization Busy */
#define WDT_STATUS_SYNCBUSY         (_U(0x1) << WDT_STATUS_SYNCBUSY_Pos)
#define WDT_STATUS_MASK             _U(0x80)     /**< \brief (WDT_STATUS) MASK Register */

/* -------- WDT_CLEAR : (WDT Offset: 0x8) ( /W  8) Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CLEAR:8;          /*!< bit:  0.. 7  Watchdog Clear                     */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} WDT_CLEAR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define WDT_CLEAR_OFFSET            0x8          /**< \brief (WDT_CLEAR offset) Clear */
#define WDT_CLEAR_RESETVALUE        _U(0x00)     /**< \brief (WDT_CLEAR reset_value) Clear */

#define WDT_CLEAR_CLEAR_Pos         0            /**< \brief (WDT_CLEAR) Watchdog Clear */
#define WDT_CLEAR_CLEAR_Msk         (_U(0xFF) << WDT_CLEAR_CLEAR_Pos)
#define WDT_CLEAR_CLEAR(value)      (WDT_CLEAR_CLEAR_Msk & ((value) << WDT_CLEAR_CLEAR_Pos))
#define   WDT_CLEAR_CLEAR_KEY_Val         _U(0xA5)   /**< \brief (WDT_CLEAR) Clear Key */
#define WDT_CLEAR_CLEAR_KEY         (WDT_CLEAR_CLEAR_KEY_Val       << WDT_CLEAR_CLEAR_Pos)
#define WDT_CLEAR_MASK              _U(0xFF)     /**< \brief (WDT_CLEAR) MASK Register */

/** \brief WDT hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO WDT_CTRL_Type             CTRL;        /**< \brief Offset: 0x0 (R/W  8) Control */
  __IO WDT_CONFIG_Type           CONFIG;      /**< \brief Offset: 0x1 (R/W  8) Configuration */
  __IO WDT_EWCTRL_Type           EWCTRL;      /**< \brief Offset: 0x2 (R/W  8) Early Warning Interrupt Control */
       RoReg8                    Reserved1[0x1];
  __IO WDT_INTENCLR_Type         INTENCLR;    /**< \brief Offset: 0x4 (R/W  8) Interrupt Enable Clear */
  __IO WDT_INTENSET_Type         INTENSET;    /**< \brief Offset: 0x5 (R/W  8) Interrupt Enable Set */
  __IO WDT_INTFLAG_Type          INTFLAG;     /**< \brief Offset: 0x6 (R/W  8) Interrupt Flag Status and Clear */
  __I  WDT_STATUS_Type           STATUS;      /**< \brief Offset: 0x7 (R/   8) Status */
  __O  WDT_CLEAR_Type            CLEAR;       /**< \brief Offset: 0x8 ( /W  8) Clear */
} Wdt;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD21_WDT_COMPONENT_ */
