/**
 * \file
 *
 * \brief Component description for PAC
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

#ifndef _SAMD51_PAC_COMPONENT_
#define _SAMD51_PAC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR PAC */
/* ========================================================================== */
/** \addtogroup SAMD51_PAC Peripheral Access Controller */
/*@{*/

#define PAC_U2120
#define REV_PAC                     0x120

/* -------- PAC_WRCTRL : (PAC Offset: 0x00) (R/W 32) Write control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PERID:16;         /*!< bit:  0..15  Peripheral identifier              */
    uint32_t KEY:8;            /*!< bit: 16..23  Peripheral access control key      */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_WRCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_WRCTRL_OFFSET           0x00         /**< \brief (PAC_WRCTRL offset) Write control */
#define PAC_WRCTRL_RESETVALUE       _U_(0x00000000) /**< \brief (PAC_WRCTRL reset_value) Write control */

#define PAC_WRCTRL_PERID_Pos        0            /**< \brief (PAC_WRCTRL) Peripheral identifier */
#define PAC_WRCTRL_PERID_Msk        (_U_(0xFFFF) << PAC_WRCTRL_PERID_Pos)
#define PAC_WRCTRL_PERID(value)     (PAC_WRCTRL_PERID_Msk & ((value) << PAC_WRCTRL_PERID_Pos))
#define PAC_WRCTRL_KEY_Pos          16           /**< \brief (PAC_WRCTRL) Peripheral access control key */
#define PAC_WRCTRL_KEY_Msk          (_U_(0xFF) << PAC_WRCTRL_KEY_Pos)
#define PAC_WRCTRL_KEY(value)       (PAC_WRCTRL_KEY_Msk & ((value) << PAC_WRCTRL_KEY_Pos))
#define   PAC_WRCTRL_KEY_OFF_Val          _U_(0x0)   /**< \brief (PAC_WRCTRL) No action */
#define   PAC_WRCTRL_KEY_CLR_Val          _U_(0x1)   /**< \brief (PAC_WRCTRL) Clear protection */
#define   PAC_WRCTRL_KEY_SET_Val          _U_(0x2)   /**< \brief (PAC_WRCTRL) Set protection */
#define   PAC_WRCTRL_KEY_SETLCK_Val       _U_(0x3)   /**< \brief (PAC_WRCTRL) Set and lock protection */
#define PAC_WRCTRL_KEY_OFF          (PAC_WRCTRL_KEY_OFF_Val        << PAC_WRCTRL_KEY_Pos)
#define PAC_WRCTRL_KEY_CLR          (PAC_WRCTRL_KEY_CLR_Val        << PAC_WRCTRL_KEY_Pos)
#define PAC_WRCTRL_KEY_SET          (PAC_WRCTRL_KEY_SET_Val        << PAC_WRCTRL_KEY_Pos)
#define PAC_WRCTRL_KEY_SETLCK       (PAC_WRCTRL_KEY_SETLCK_Val     << PAC_WRCTRL_KEY_Pos)
#define PAC_WRCTRL_MASK             _U_(0x00FFFFFF) /**< \brief (PAC_WRCTRL) MASK Register */

/* -------- PAC_EVCTRL : (PAC Offset: 0x04) (R/W  8) Event control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  ERREO:1;          /*!< bit:      0  Peripheral acess error event output */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PAC_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_EVCTRL_OFFSET           0x04         /**< \brief (PAC_EVCTRL offset) Event control */
#define PAC_EVCTRL_RESETVALUE       _U_(0x00)    /**< \brief (PAC_EVCTRL reset_value) Event control */

#define PAC_EVCTRL_ERREO_Pos        0            /**< \brief (PAC_EVCTRL) Peripheral acess error event output */
#define PAC_EVCTRL_ERREO            (_U_(0x1) << PAC_EVCTRL_ERREO_Pos)
#define PAC_EVCTRL_MASK             _U_(0x01)    /**< \brief (PAC_EVCTRL) MASK Register */

/* -------- PAC_INTENCLR : (PAC Offset: 0x08) (R/W  8) Interrupt enable clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  ERR:1;            /*!< bit:      0  Peripheral access error interrupt disable */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PAC_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_INTENCLR_OFFSET         0x08         /**< \brief (PAC_INTENCLR offset) Interrupt enable clear */
#define PAC_INTENCLR_RESETVALUE     _U_(0x00)    /**< \brief (PAC_INTENCLR reset_value) Interrupt enable clear */

#define PAC_INTENCLR_ERR_Pos        0            /**< \brief (PAC_INTENCLR) Peripheral access error interrupt disable */
#define PAC_INTENCLR_ERR            (_U_(0x1) << PAC_INTENCLR_ERR_Pos)
#define PAC_INTENCLR_MASK           _U_(0x01)    /**< \brief (PAC_INTENCLR) MASK Register */

/* -------- PAC_INTENSET : (PAC Offset: 0x09) (R/W  8) Interrupt enable set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  ERR:1;            /*!< bit:      0  Peripheral access error interrupt enable */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PAC_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_INTENSET_OFFSET         0x09         /**< \brief (PAC_INTENSET offset) Interrupt enable set */
#define PAC_INTENSET_RESETVALUE     _U_(0x00)    /**< \brief (PAC_INTENSET reset_value) Interrupt enable set */

#define PAC_INTENSET_ERR_Pos        0            /**< \brief (PAC_INTENSET) Peripheral access error interrupt enable */
#define PAC_INTENSET_ERR            (_U_(0x1) << PAC_INTENSET_ERR_Pos)
#define PAC_INTENSET_MASK           _U_(0x01)    /**< \brief (PAC_INTENSET) MASK Register */

/* -------- PAC_INTFLAGAHB : (PAC Offset: 0x10) (R/W 32) Bridge interrupt flag status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t FLASH_:1;         /*!< bit:      0  FLASH                              */
    __I uint32_t FLASH_ALT_:1;     /*!< bit:      1  FLASH_ALT                          */
    __I uint32_t SEEPROM_:1;       /*!< bit:      2  SEEPROM                            */
    __I uint32_t RAMCM4S_:1;       /*!< bit:      3  RAMCM4S                            */
    __I uint32_t RAMPPPDSU_:1;     /*!< bit:      4  RAMPPPDSU                          */
    __I uint32_t RAMDMAWR_:1;      /*!< bit:      5  RAMDMAWR                           */
    __I uint32_t RAMDMACICM_:1;    /*!< bit:      6  RAMDMACICM                         */
    __I uint32_t HPB0_:1;          /*!< bit:      7  HPB0                               */
    __I uint32_t HPB1_:1;          /*!< bit:      8  HPB1                               */
    __I uint32_t HPB2_:1;          /*!< bit:      9  HPB2                               */
    __I uint32_t HPB3_:1;          /*!< bit:     10  HPB3                               */
    __I uint32_t PUKCC_:1;         /*!< bit:     11  PUKCC                              */
    __I uint32_t SDHC0_:1;         /*!< bit:     12  SDHC0                              */
    __I uint32_t SDHC1_:1;         /*!< bit:     13  SDHC1                              */
    __I uint32_t QSPI_:1;          /*!< bit:     14  QSPI                               */
    __I uint32_t BKUPRAM_:1;       /*!< bit:     15  BKUPRAM                            */
    __I uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_INTFLAGAHB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_INTFLAGAHB_OFFSET       0x10         /**< \brief (PAC_INTFLAGAHB offset) Bridge interrupt flag status */
#define PAC_INTFLAGAHB_RESETVALUE   _U_(0x00000000) /**< \brief (PAC_INTFLAGAHB reset_value) Bridge interrupt flag status */

#define PAC_INTFLAGAHB_FLASH_Pos    0            /**< \brief (PAC_INTFLAGAHB) FLASH */
#define PAC_INTFLAGAHB_FLASH        (_U_(0x1) << PAC_INTFLAGAHB_FLASH_Pos)
#define PAC_INTFLAGAHB_FLASH_ALT_Pos 1            /**< \brief (PAC_INTFLAGAHB) FLASH_ALT */
#define PAC_INTFLAGAHB_FLASH_ALT    (_U_(0x1) << PAC_INTFLAGAHB_FLASH_ALT_Pos)
#define PAC_INTFLAGAHB_SEEPROM_Pos  2            /**< \brief (PAC_INTFLAGAHB) SEEPROM */
#define PAC_INTFLAGAHB_SEEPROM      (_U_(0x1) << PAC_INTFLAGAHB_SEEPROM_Pos)
#define PAC_INTFLAGAHB_RAMCM4S_Pos  3            /**< \brief (PAC_INTFLAGAHB) RAMCM4S */
#define PAC_INTFLAGAHB_RAMCM4S      (_U_(0x1) << PAC_INTFLAGAHB_RAMCM4S_Pos)
#define PAC_INTFLAGAHB_RAMPPPDSU_Pos 4            /**< \brief (PAC_INTFLAGAHB) RAMPPPDSU */
#define PAC_INTFLAGAHB_RAMPPPDSU    (_U_(0x1) << PAC_INTFLAGAHB_RAMPPPDSU_Pos)
#define PAC_INTFLAGAHB_RAMDMAWR_Pos 5            /**< \brief (PAC_INTFLAGAHB) RAMDMAWR */
#define PAC_INTFLAGAHB_RAMDMAWR     (_U_(0x1) << PAC_INTFLAGAHB_RAMDMAWR_Pos)
#define PAC_INTFLAGAHB_RAMDMACICM_Pos 6            /**< \brief (PAC_INTFLAGAHB) RAMDMACICM */
#define PAC_INTFLAGAHB_RAMDMACICM   (_U_(0x1) << PAC_INTFLAGAHB_RAMDMACICM_Pos)
#define PAC_INTFLAGAHB_HPB0_Pos     7            /**< \brief (PAC_INTFLAGAHB) HPB0 */
#define PAC_INTFLAGAHB_HPB0         (_U_(0x1) << PAC_INTFLAGAHB_HPB0_Pos)
#define PAC_INTFLAGAHB_HPB1_Pos     8            /**< \brief (PAC_INTFLAGAHB) HPB1 */
#define PAC_INTFLAGAHB_HPB1         (_U_(0x1) << PAC_INTFLAGAHB_HPB1_Pos)
#define PAC_INTFLAGAHB_HPB2_Pos     9            /**< \brief (PAC_INTFLAGAHB) HPB2 */
#define PAC_INTFLAGAHB_HPB2         (_U_(0x1) << PAC_INTFLAGAHB_HPB2_Pos)
#define PAC_INTFLAGAHB_HPB3_Pos     10           /**< \brief (PAC_INTFLAGAHB) HPB3 */
#define PAC_INTFLAGAHB_HPB3         (_U_(0x1) << PAC_INTFLAGAHB_HPB3_Pos)
#define PAC_INTFLAGAHB_PUKCC_Pos    11           /**< \brief (PAC_INTFLAGAHB) PUKCC */
#define PAC_INTFLAGAHB_PUKCC        (_U_(0x1) << PAC_INTFLAGAHB_PUKCC_Pos)
#define PAC_INTFLAGAHB_SDHC0_Pos    12           /**< \brief (PAC_INTFLAGAHB) SDHC0 */
#define PAC_INTFLAGAHB_SDHC0        (_U_(0x1) << PAC_INTFLAGAHB_SDHC0_Pos)
#define PAC_INTFLAGAHB_SDHC1_Pos    13           /**< \brief (PAC_INTFLAGAHB) SDHC1 */
#define PAC_INTFLAGAHB_SDHC1        (_U_(0x1) << PAC_INTFLAGAHB_SDHC1_Pos)
#define PAC_INTFLAGAHB_QSPI_Pos     14           /**< \brief (PAC_INTFLAGAHB) QSPI */
#define PAC_INTFLAGAHB_QSPI         (_U_(0x1) << PAC_INTFLAGAHB_QSPI_Pos)
#define PAC_INTFLAGAHB_BKUPRAM_Pos  15           /**< \brief (PAC_INTFLAGAHB) BKUPRAM */
#define PAC_INTFLAGAHB_BKUPRAM      (_U_(0x1) << PAC_INTFLAGAHB_BKUPRAM_Pos)
#define PAC_INTFLAGAHB_MASK         _U_(0x0000FFFF) /**< \brief (PAC_INTFLAGAHB) MASK Register */

/* -------- PAC_INTFLAGA : (PAC Offset: 0x14) (R/W 32) Peripheral interrupt flag status - Bridge A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t PAC_:1;           /*!< bit:      0  PAC                                */
    __I uint32_t PM_:1;            /*!< bit:      1  PM                                 */
    __I uint32_t MCLK_:1;          /*!< bit:      2  MCLK                               */
    __I uint32_t RSTC_:1;          /*!< bit:      3  RSTC                               */
    __I uint32_t OSCCTRL_:1;       /*!< bit:      4  OSCCTRL                            */
    __I uint32_t OSC32KCTRL_:1;    /*!< bit:      5  OSC32KCTRL                         */
    __I uint32_t SUPC_:1;          /*!< bit:      6  SUPC                               */
    __I uint32_t GCLK_:1;          /*!< bit:      7  GCLK                               */
    __I uint32_t WDT_:1;           /*!< bit:      8  WDT                                */
    __I uint32_t RTC_:1;           /*!< bit:      9  RTC                                */
    __I uint32_t EIC_:1;           /*!< bit:     10  EIC                                */
    __I uint32_t FREQM_:1;         /*!< bit:     11  FREQM                              */
    __I uint32_t SERCOM0_:1;       /*!< bit:     12  SERCOM0                            */
    __I uint32_t SERCOM1_:1;       /*!< bit:     13  SERCOM1                            */
    __I uint32_t TC0_:1;           /*!< bit:     14  TC0                                */
    __I uint32_t TC1_:1;           /*!< bit:     15  TC1                                */
    __I uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_INTFLAGA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_INTFLAGA_OFFSET         0x14         /**< \brief (PAC_INTFLAGA offset) Peripheral interrupt flag status - Bridge A */
#define PAC_INTFLAGA_RESETVALUE     _U_(0x00000000) /**< \brief (PAC_INTFLAGA reset_value) Peripheral interrupt flag status - Bridge A */

#define PAC_INTFLAGA_PAC_Pos        0            /**< \brief (PAC_INTFLAGA) PAC */
#define PAC_INTFLAGA_PAC            (_U_(0x1) << PAC_INTFLAGA_PAC_Pos)
#define PAC_INTFLAGA_PM_Pos         1            /**< \brief (PAC_INTFLAGA) PM */
#define PAC_INTFLAGA_PM             (_U_(0x1) << PAC_INTFLAGA_PM_Pos)
#define PAC_INTFLAGA_MCLK_Pos       2            /**< \brief (PAC_INTFLAGA) MCLK */
#define PAC_INTFLAGA_MCLK           (_U_(0x1) << PAC_INTFLAGA_MCLK_Pos)
#define PAC_INTFLAGA_RSTC_Pos       3            /**< \brief (PAC_INTFLAGA) RSTC */
#define PAC_INTFLAGA_RSTC           (_U_(0x1) << PAC_INTFLAGA_RSTC_Pos)
#define PAC_INTFLAGA_OSCCTRL_Pos    4            /**< \brief (PAC_INTFLAGA) OSCCTRL */
#define PAC_INTFLAGA_OSCCTRL        (_U_(0x1) << PAC_INTFLAGA_OSCCTRL_Pos)
#define PAC_INTFLAGA_OSC32KCTRL_Pos 5            /**< \brief (PAC_INTFLAGA) OSC32KCTRL */
#define PAC_INTFLAGA_OSC32KCTRL     (_U_(0x1) << PAC_INTFLAGA_OSC32KCTRL_Pos)
#define PAC_INTFLAGA_SUPC_Pos       6            /**< \brief (PAC_INTFLAGA) SUPC */
#define PAC_INTFLAGA_SUPC           (_U_(0x1) << PAC_INTFLAGA_SUPC_Pos)
#define PAC_INTFLAGA_GCLK_Pos       7            /**< \brief (PAC_INTFLAGA) GCLK */
#define PAC_INTFLAGA_GCLK           (_U_(0x1) << PAC_INTFLAGA_GCLK_Pos)
#define PAC_INTFLAGA_WDT_Pos        8            /**< \brief (PAC_INTFLAGA) WDT */
#define PAC_INTFLAGA_WDT            (_U_(0x1) << PAC_INTFLAGA_WDT_Pos)
#define PAC_INTFLAGA_RTC_Pos        9            /**< \brief (PAC_INTFLAGA) RTC */
#define PAC_INTFLAGA_RTC            (_U_(0x1) << PAC_INTFLAGA_RTC_Pos)
#define PAC_INTFLAGA_EIC_Pos        10           /**< \brief (PAC_INTFLAGA) EIC */
#define PAC_INTFLAGA_EIC            (_U_(0x1) << PAC_INTFLAGA_EIC_Pos)
#define PAC_INTFLAGA_FREQM_Pos      11           /**< \brief (PAC_INTFLAGA) FREQM */
#define PAC_INTFLAGA_FREQM          (_U_(0x1) << PAC_INTFLAGA_FREQM_Pos)
#define PAC_INTFLAGA_SERCOM0_Pos    12           /**< \brief (PAC_INTFLAGA) SERCOM0 */
#define PAC_INTFLAGA_SERCOM0        (_U_(0x1) << PAC_INTFLAGA_SERCOM0_Pos)
#define PAC_INTFLAGA_SERCOM1_Pos    13           /**< \brief (PAC_INTFLAGA) SERCOM1 */
#define PAC_INTFLAGA_SERCOM1        (_U_(0x1) << PAC_INTFLAGA_SERCOM1_Pos)
#define PAC_INTFLAGA_TC0_Pos        14           /**< \brief (PAC_INTFLAGA) TC0 */
#define PAC_INTFLAGA_TC0            (_U_(0x1) << PAC_INTFLAGA_TC0_Pos)
#define PAC_INTFLAGA_TC1_Pos        15           /**< \brief (PAC_INTFLAGA) TC1 */
#define PAC_INTFLAGA_TC1            (_U_(0x1) << PAC_INTFLAGA_TC1_Pos)
#define PAC_INTFLAGA_MASK           _U_(0x0000FFFF) /**< \brief (PAC_INTFLAGA) MASK Register */

/* -------- PAC_INTFLAGB : (PAC Offset: 0x18) (R/W 32) Peripheral interrupt flag status - Bridge B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t USB_:1;           /*!< bit:      0  USB                                */
    __I uint32_t DSU_:1;           /*!< bit:      1  DSU                                */
    __I uint32_t NVMCTRL_:1;       /*!< bit:      2  NVMCTRL                            */
    __I uint32_t CMCC_:1;          /*!< bit:      3  CMCC                               */
    __I uint32_t PORT_:1;          /*!< bit:      4  PORT                               */
    __I uint32_t DMAC_:1;          /*!< bit:      5  DMAC                               */
    __I uint32_t HMATRIX_:1;       /*!< bit:      6  HMATRIX                            */
    __I uint32_t EVSYS_:1;         /*!< bit:      7  EVSYS                              */
    __I uint32_t :1;               /*!< bit:      8  Reserved                           */
    __I uint32_t SERCOM2_:1;       /*!< bit:      9  SERCOM2                            */
    __I uint32_t SERCOM3_:1;       /*!< bit:     10  SERCOM3                            */
    __I uint32_t TCC0_:1;          /*!< bit:     11  TCC0                               */
    __I uint32_t TCC1_:1;          /*!< bit:     12  TCC1                               */
    __I uint32_t TC2_:1;           /*!< bit:     13  TC2                                */
    __I uint32_t TC3_:1;           /*!< bit:     14  TC3                                */
    __I uint32_t TAL_:1;           /*!< bit:     15  TAL                                */
    __I uint32_t RAMECC_:1;        /*!< bit:     16  RAMECC                             */
    __I uint32_t :15;              /*!< bit: 17..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_INTFLAGB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_INTFLAGB_OFFSET         0x18         /**< \brief (PAC_INTFLAGB offset) Peripheral interrupt flag status - Bridge B */
#define PAC_INTFLAGB_RESETVALUE     _U_(0x00000000) /**< \brief (PAC_INTFLAGB reset_value) Peripheral interrupt flag status - Bridge B */

#define PAC_INTFLAGB_USB_Pos        0            /**< \brief (PAC_INTFLAGB) USB */
#define PAC_INTFLAGB_USB            (_U_(0x1) << PAC_INTFLAGB_USB_Pos)
#define PAC_INTFLAGB_DSU_Pos        1            /**< \brief (PAC_INTFLAGB) DSU */
#define PAC_INTFLAGB_DSU            (_U_(0x1) << PAC_INTFLAGB_DSU_Pos)
#define PAC_INTFLAGB_NVMCTRL_Pos    2            /**< \brief (PAC_INTFLAGB) NVMCTRL */
#define PAC_INTFLAGB_NVMCTRL        (_U_(0x1) << PAC_INTFLAGB_NVMCTRL_Pos)
#define PAC_INTFLAGB_CMCC_Pos       3            /**< \brief (PAC_INTFLAGB) CMCC */
#define PAC_INTFLAGB_CMCC           (_U_(0x1) << PAC_INTFLAGB_CMCC_Pos)
#define PAC_INTFLAGB_PORT_Pos       4            /**< \brief (PAC_INTFLAGB) PORT */
#define PAC_INTFLAGB_PORT           (_U_(0x1) << PAC_INTFLAGB_PORT_Pos)
#define PAC_INTFLAGB_DMAC_Pos       5            /**< \brief (PAC_INTFLAGB) DMAC */
#define PAC_INTFLAGB_DMAC           (_U_(0x1) << PAC_INTFLAGB_DMAC_Pos)
#define PAC_INTFLAGB_HMATRIX_Pos    6            /**< \brief (PAC_INTFLAGB) HMATRIX */
#define PAC_INTFLAGB_HMATRIX        (_U_(0x1) << PAC_INTFLAGB_HMATRIX_Pos)
#define PAC_INTFLAGB_EVSYS_Pos      7            /**< \brief (PAC_INTFLAGB) EVSYS */
#define PAC_INTFLAGB_EVSYS          (_U_(0x1) << PAC_INTFLAGB_EVSYS_Pos)
#define PAC_INTFLAGB_SERCOM2_Pos    9            /**< \brief (PAC_INTFLAGB) SERCOM2 */
#define PAC_INTFLAGB_SERCOM2        (_U_(0x1) << PAC_INTFLAGB_SERCOM2_Pos)
#define PAC_INTFLAGB_SERCOM3_Pos    10           /**< \brief (PAC_INTFLAGB) SERCOM3 */
#define PAC_INTFLAGB_SERCOM3        (_U_(0x1) << PAC_INTFLAGB_SERCOM3_Pos)
#define PAC_INTFLAGB_TCC0_Pos       11           /**< \brief (PAC_INTFLAGB) TCC0 */
#define PAC_INTFLAGB_TCC0           (_U_(0x1) << PAC_INTFLAGB_TCC0_Pos)
#define PAC_INTFLAGB_TCC1_Pos       12           /**< \brief (PAC_INTFLAGB) TCC1 */
#define PAC_INTFLAGB_TCC1           (_U_(0x1) << PAC_INTFLAGB_TCC1_Pos)
#define PAC_INTFLAGB_TC2_Pos        13           /**< \brief (PAC_INTFLAGB) TC2 */
#define PAC_INTFLAGB_TC2            (_U_(0x1) << PAC_INTFLAGB_TC2_Pos)
#define PAC_INTFLAGB_TC3_Pos        14           /**< \brief (PAC_INTFLAGB) TC3 */
#define PAC_INTFLAGB_TC3            (_U_(0x1) << PAC_INTFLAGB_TC3_Pos)
#define PAC_INTFLAGB_TAL_Pos        15           /**< \brief (PAC_INTFLAGB) TAL */
#define PAC_INTFLAGB_TAL            (_U_(0x1) << PAC_INTFLAGB_TAL_Pos)
#define PAC_INTFLAGB_RAMECC_Pos     16           /**< \brief (PAC_INTFLAGB) RAMECC */
#define PAC_INTFLAGB_RAMECC         (_U_(0x1) << PAC_INTFLAGB_RAMECC_Pos)
#define PAC_INTFLAGB_MASK           _U_(0x0001FEFF) /**< \brief (PAC_INTFLAGB) MASK Register */

/* -------- PAC_INTFLAGC : (PAC Offset: 0x1C) (R/W 32) Peripheral interrupt flag status - Bridge C -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t :3;               /*!< bit:  0.. 2  Reserved                           */
    __I uint32_t TCC2_:1;          /*!< bit:      3  TCC2                               */
    __I uint32_t TCC3_:1;          /*!< bit:      4  TCC3                               */
    __I uint32_t TC4_:1;           /*!< bit:      5  TC4                                */
    __I uint32_t TC5_:1;           /*!< bit:      6  TC5                                */
    __I uint32_t PDEC_:1;          /*!< bit:      7  PDEC                               */
    __I uint32_t AC_:1;            /*!< bit:      8  AC                                 */
    __I uint32_t AES_:1;           /*!< bit:      9  AES                                */
    __I uint32_t TRNG_:1;          /*!< bit:     10  TRNG                               */
    __I uint32_t ICM_:1;           /*!< bit:     11  ICM                                */
    __I uint32_t PUKCC_:1;         /*!< bit:     12  PUKCC                              */
    __I uint32_t QSPI_:1;          /*!< bit:     13  QSPI                               */
    __I uint32_t CCL_:1;           /*!< bit:     14  CCL                                */
    __I uint32_t :17;              /*!< bit: 15..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_INTFLAGC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_INTFLAGC_OFFSET         0x1C         /**< \brief (PAC_INTFLAGC offset) Peripheral interrupt flag status - Bridge C */
#define PAC_INTFLAGC_RESETVALUE     _U_(0x00000000) /**< \brief (PAC_INTFLAGC reset_value) Peripheral interrupt flag status - Bridge C */

#define PAC_INTFLAGC_TCC2_Pos       3            /**< \brief (PAC_INTFLAGC) TCC2 */
#define PAC_INTFLAGC_TCC2           (_U_(0x1) << PAC_INTFLAGC_TCC2_Pos)
#define PAC_INTFLAGC_TCC3_Pos       4            /**< \brief (PAC_INTFLAGC) TCC3 */
#define PAC_INTFLAGC_TCC3           (_U_(0x1) << PAC_INTFLAGC_TCC3_Pos)
#define PAC_INTFLAGC_TC4_Pos        5            /**< \brief (PAC_INTFLAGC) TC4 */
#define PAC_INTFLAGC_TC4            (_U_(0x1) << PAC_INTFLAGC_TC4_Pos)
#define PAC_INTFLAGC_TC5_Pos        6            /**< \brief (PAC_INTFLAGC) TC5 */
#define PAC_INTFLAGC_TC5            (_U_(0x1) << PAC_INTFLAGC_TC5_Pos)
#define PAC_INTFLAGC_PDEC_Pos       7            /**< \brief (PAC_INTFLAGC) PDEC */
#define PAC_INTFLAGC_PDEC           (_U_(0x1) << PAC_INTFLAGC_PDEC_Pos)
#define PAC_INTFLAGC_AC_Pos         8            /**< \brief (PAC_INTFLAGC) AC */
#define PAC_INTFLAGC_AC             (_U_(0x1) << PAC_INTFLAGC_AC_Pos)
#define PAC_INTFLAGC_AES_Pos        9            /**< \brief (PAC_INTFLAGC) AES */
#define PAC_INTFLAGC_AES            (_U_(0x1) << PAC_INTFLAGC_AES_Pos)
#define PAC_INTFLAGC_TRNG_Pos       10           /**< \brief (PAC_INTFLAGC) TRNG */
#define PAC_INTFLAGC_TRNG           (_U_(0x1) << PAC_INTFLAGC_TRNG_Pos)
#define PAC_INTFLAGC_ICM_Pos        11           /**< \brief (PAC_INTFLAGC) ICM */
#define PAC_INTFLAGC_ICM            (_U_(0x1) << PAC_INTFLAGC_ICM_Pos)
#define PAC_INTFLAGC_PUKCC_Pos      12           /**< \brief (PAC_INTFLAGC) PUKCC */
#define PAC_INTFLAGC_PUKCC          (_U_(0x1) << PAC_INTFLAGC_PUKCC_Pos)
#define PAC_INTFLAGC_QSPI_Pos       13           /**< \brief (PAC_INTFLAGC) QSPI */
#define PAC_INTFLAGC_QSPI           (_U_(0x1) << PAC_INTFLAGC_QSPI_Pos)
#define PAC_INTFLAGC_CCL_Pos        14           /**< \brief (PAC_INTFLAGC) CCL */
#define PAC_INTFLAGC_CCL            (_U_(0x1) << PAC_INTFLAGC_CCL_Pos)
#define PAC_INTFLAGC_MASK           _U_(0x00007FF8) /**< \brief (PAC_INTFLAGC) MASK Register */

/* -------- PAC_INTFLAGD : (PAC Offset: 0x20) (R/W 32) Peripheral interrupt flag status - Bridge D -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t SERCOM4_:1;       /*!< bit:      0  SERCOM4                            */
    __I uint32_t SERCOM5_:1;       /*!< bit:      1  SERCOM5                            */
    __I uint32_t SERCOM6_:1;       /*!< bit:      2  SERCOM6                            */
    __I uint32_t SERCOM7_:1;       /*!< bit:      3  SERCOM7                            */
    __I uint32_t TCC4_:1;          /*!< bit:      4  TCC4                               */
    __I uint32_t TC6_:1;           /*!< bit:      5  TC6                                */
    __I uint32_t TC7_:1;           /*!< bit:      6  TC7                                */
    __I uint32_t ADC0_:1;          /*!< bit:      7  ADC0                               */
    __I uint32_t ADC1_:1;          /*!< bit:      8  ADC1                               */
    __I uint32_t DAC_:1;           /*!< bit:      9  DAC                                */
    __I uint32_t I2S_:1;           /*!< bit:     10  I2S                                */
    __I uint32_t PCC_:1;           /*!< bit:     11  PCC                                */
    __I uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_INTFLAGD_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_INTFLAGD_OFFSET         0x20         /**< \brief (PAC_INTFLAGD offset) Peripheral interrupt flag status - Bridge D */
#define PAC_INTFLAGD_RESETVALUE     _U_(0x00000000) /**< \brief (PAC_INTFLAGD reset_value) Peripheral interrupt flag status - Bridge D */

#define PAC_INTFLAGD_SERCOM4_Pos    0            /**< \brief (PAC_INTFLAGD) SERCOM4 */
#define PAC_INTFLAGD_SERCOM4        (_U_(0x1) << PAC_INTFLAGD_SERCOM4_Pos)
#define PAC_INTFLAGD_SERCOM5_Pos    1            /**< \brief (PAC_INTFLAGD) SERCOM5 */
#define PAC_INTFLAGD_SERCOM5        (_U_(0x1) << PAC_INTFLAGD_SERCOM5_Pos)
#define PAC_INTFLAGD_SERCOM6_Pos    2            /**< \brief (PAC_INTFLAGD) SERCOM6 */
#define PAC_INTFLAGD_SERCOM6        (_U_(0x1) << PAC_INTFLAGD_SERCOM6_Pos)
#define PAC_INTFLAGD_SERCOM7_Pos    3            /**< \brief (PAC_INTFLAGD) SERCOM7 */
#define PAC_INTFLAGD_SERCOM7        (_U_(0x1) << PAC_INTFLAGD_SERCOM7_Pos)
#define PAC_INTFLAGD_TCC4_Pos       4            /**< \brief (PAC_INTFLAGD) TCC4 */
#define PAC_INTFLAGD_TCC4           (_U_(0x1) << PAC_INTFLAGD_TCC4_Pos)
#define PAC_INTFLAGD_TC6_Pos        5            /**< \brief (PAC_INTFLAGD) TC6 */
#define PAC_INTFLAGD_TC6            (_U_(0x1) << PAC_INTFLAGD_TC6_Pos)
#define PAC_INTFLAGD_TC7_Pos        6            /**< \brief (PAC_INTFLAGD) TC7 */
#define PAC_INTFLAGD_TC7            (_U_(0x1) << PAC_INTFLAGD_TC7_Pos)
#define PAC_INTFLAGD_ADC0_Pos       7            /**< \brief (PAC_INTFLAGD) ADC0 */
#define PAC_INTFLAGD_ADC0           (_U_(0x1) << PAC_INTFLAGD_ADC0_Pos)
#define PAC_INTFLAGD_ADC1_Pos       8            /**< \brief (PAC_INTFLAGD) ADC1 */
#define PAC_INTFLAGD_ADC1           (_U_(0x1) << PAC_INTFLAGD_ADC1_Pos)
#define PAC_INTFLAGD_DAC_Pos        9            /**< \brief (PAC_INTFLAGD) DAC */
#define PAC_INTFLAGD_DAC            (_U_(0x1) << PAC_INTFLAGD_DAC_Pos)
#define PAC_INTFLAGD_I2S_Pos        10           /**< \brief (PAC_INTFLAGD) I2S */
#define PAC_INTFLAGD_I2S            (_U_(0x1) << PAC_INTFLAGD_I2S_Pos)
#define PAC_INTFLAGD_PCC_Pos        11           /**< \brief (PAC_INTFLAGD) PCC */
#define PAC_INTFLAGD_PCC            (_U_(0x1) << PAC_INTFLAGD_PCC_Pos)
#define PAC_INTFLAGD_MASK           _U_(0x00000FFF) /**< \brief (PAC_INTFLAGD) MASK Register */

/* -------- PAC_STATUSA : (PAC Offset: 0x34) (R/  32) Peripheral write protection status - Bridge A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PAC_:1;           /*!< bit:      0  PAC APB Protect Enable             */
    uint32_t PM_:1;            /*!< bit:      1  PM APB Protect Enable              */
    uint32_t MCLK_:1;          /*!< bit:      2  MCLK APB Protect Enable            */
    uint32_t RSTC_:1;          /*!< bit:      3  RSTC APB Protect Enable            */
    uint32_t OSCCTRL_:1;       /*!< bit:      4  OSCCTRL APB Protect Enable         */
    uint32_t OSC32KCTRL_:1;    /*!< bit:      5  OSC32KCTRL APB Protect Enable      */
    uint32_t SUPC_:1;          /*!< bit:      6  SUPC APB Protect Enable            */
    uint32_t GCLK_:1;          /*!< bit:      7  GCLK APB Protect Enable            */
    uint32_t WDT_:1;           /*!< bit:      8  WDT APB Protect Enable             */
    uint32_t RTC_:1;           /*!< bit:      9  RTC APB Protect Enable             */
    uint32_t EIC_:1;           /*!< bit:     10  EIC APB Protect Enable             */
    uint32_t FREQM_:1;         /*!< bit:     11  FREQM APB Protect Enable           */
    uint32_t SERCOM0_:1;       /*!< bit:     12  SERCOM0 APB Protect Enable         */
    uint32_t SERCOM1_:1;       /*!< bit:     13  SERCOM1 APB Protect Enable         */
    uint32_t TC0_:1;           /*!< bit:     14  TC0 APB Protect Enable             */
    uint32_t TC1_:1;           /*!< bit:     15  TC1 APB Protect Enable             */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_STATUSA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_STATUSA_OFFSET          0x34         /**< \brief (PAC_STATUSA offset) Peripheral write protection status - Bridge A */
#define PAC_STATUSA_RESETVALUE      _U_(0x00010000) /**< \brief (PAC_STATUSA reset_value) Peripheral write protection status - Bridge A */

#define PAC_STATUSA_PAC_Pos         0            /**< \brief (PAC_STATUSA) PAC APB Protect Enable */
#define PAC_STATUSA_PAC             (_U_(0x1) << PAC_STATUSA_PAC_Pos)
#define PAC_STATUSA_PM_Pos          1            /**< \brief (PAC_STATUSA) PM APB Protect Enable */
#define PAC_STATUSA_PM              (_U_(0x1) << PAC_STATUSA_PM_Pos)
#define PAC_STATUSA_MCLK_Pos        2            /**< \brief (PAC_STATUSA) MCLK APB Protect Enable */
#define PAC_STATUSA_MCLK            (_U_(0x1) << PAC_STATUSA_MCLK_Pos)
#define PAC_STATUSA_RSTC_Pos        3            /**< \brief (PAC_STATUSA) RSTC APB Protect Enable */
#define PAC_STATUSA_RSTC            (_U_(0x1) << PAC_STATUSA_RSTC_Pos)
#define PAC_STATUSA_OSCCTRL_Pos     4            /**< \brief (PAC_STATUSA) OSCCTRL APB Protect Enable */
#define PAC_STATUSA_OSCCTRL         (_U_(0x1) << PAC_STATUSA_OSCCTRL_Pos)
#define PAC_STATUSA_OSC32KCTRL_Pos  5            /**< \brief (PAC_STATUSA) OSC32KCTRL APB Protect Enable */
#define PAC_STATUSA_OSC32KCTRL      (_U_(0x1) << PAC_STATUSA_OSC32KCTRL_Pos)
#define PAC_STATUSA_SUPC_Pos        6            /**< \brief (PAC_STATUSA) SUPC APB Protect Enable */
#define PAC_STATUSA_SUPC            (_U_(0x1) << PAC_STATUSA_SUPC_Pos)
#define PAC_STATUSA_GCLK_Pos        7            /**< \brief (PAC_STATUSA) GCLK APB Protect Enable */
#define PAC_STATUSA_GCLK            (_U_(0x1) << PAC_STATUSA_GCLK_Pos)
#define PAC_STATUSA_WDT_Pos         8            /**< \brief (PAC_STATUSA) WDT APB Protect Enable */
#define PAC_STATUSA_WDT             (_U_(0x1) << PAC_STATUSA_WDT_Pos)
#define PAC_STATUSA_RTC_Pos         9            /**< \brief (PAC_STATUSA) RTC APB Protect Enable */
#define PAC_STATUSA_RTC             (_U_(0x1) << PAC_STATUSA_RTC_Pos)
#define PAC_STATUSA_EIC_Pos         10           /**< \brief (PAC_STATUSA) EIC APB Protect Enable */
#define PAC_STATUSA_EIC             (_U_(0x1) << PAC_STATUSA_EIC_Pos)
#define PAC_STATUSA_FREQM_Pos       11           /**< \brief (PAC_STATUSA) FREQM APB Protect Enable */
#define PAC_STATUSA_FREQM           (_U_(0x1) << PAC_STATUSA_FREQM_Pos)
#define PAC_STATUSA_SERCOM0_Pos     12           /**< \brief (PAC_STATUSA) SERCOM0 APB Protect Enable */
#define PAC_STATUSA_SERCOM0         (_U_(0x1) << PAC_STATUSA_SERCOM0_Pos)
#define PAC_STATUSA_SERCOM1_Pos     13           /**< \brief (PAC_STATUSA) SERCOM1 APB Protect Enable */
#define PAC_STATUSA_SERCOM1         (_U_(0x1) << PAC_STATUSA_SERCOM1_Pos)
#define PAC_STATUSA_TC0_Pos         14           /**< \brief (PAC_STATUSA) TC0 APB Protect Enable */
#define PAC_STATUSA_TC0             (_U_(0x1) << PAC_STATUSA_TC0_Pos)
#define PAC_STATUSA_TC1_Pos         15           /**< \brief (PAC_STATUSA) TC1 APB Protect Enable */
#define PAC_STATUSA_TC1             (_U_(0x1) << PAC_STATUSA_TC1_Pos)
#define PAC_STATUSA_MASK            _U_(0x0000FFFF) /**< \brief (PAC_STATUSA) MASK Register */

/* -------- PAC_STATUSB : (PAC Offset: 0x38) (R/  32) Peripheral write protection status - Bridge B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t USB_:1;           /*!< bit:      0  USB APB Protect Enable             */
    uint32_t DSU_:1;           /*!< bit:      1  DSU APB Protect Enable             */
    uint32_t NVMCTRL_:1;       /*!< bit:      2  NVMCTRL APB Protect Enable         */
    uint32_t CMCC_:1;          /*!< bit:      3  CMCC APB Protect Enable            */
    uint32_t PORT_:1;          /*!< bit:      4  PORT APB Protect Enable            */
    uint32_t DMAC_:1;          /*!< bit:      5  DMAC APB Protect Enable            */
    uint32_t HMATRIX_:1;       /*!< bit:      6  HMATRIX APB Protect Enable         */
    uint32_t EVSYS_:1;         /*!< bit:      7  EVSYS APB Protect Enable           */
    uint32_t :1;               /*!< bit:      8  Reserved                           */
    uint32_t SERCOM2_:1;       /*!< bit:      9  SERCOM2 APB Protect Enable         */
    uint32_t SERCOM3_:1;       /*!< bit:     10  SERCOM3 APB Protect Enable         */
    uint32_t TCC0_:1;          /*!< bit:     11  TCC0 APB Protect Enable            */
    uint32_t TCC1_:1;          /*!< bit:     12  TCC1 APB Protect Enable            */
    uint32_t TC2_:1;           /*!< bit:     13  TC2 APB Protect Enable             */
    uint32_t TC3_:1;           /*!< bit:     14  TC3 APB Protect Enable             */
    uint32_t TAL_:1;           /*!< bit:     15  TAL APB Protect Enable             */
    uint32_t RAMECC_:1;        /*!< bit:     16  RAMECC APB Protect Enable          */
    uint32_t :15;              /*!< bit: 17..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_STATUSB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_STATUSB_OFFSET          0x38         /**< \brief (PAC_STATUSB offset) Peripheral write protection status - Bridge B */
#define PAC_STATUSB_RESETVALUE      _U_(0x00000002) /**< \brief (PAC_STATUSB reset_value) Peripheral write protection status - Bridge B */

#define PAC_STATUSB_USB_Pos         0            /**< \brief (PAC_STATUSB) USB APB Protect Enable */
#define PAC_STATUSB_USB             (_U_(0x1) << PAC_STATUSB_USB_Pos)
#define PAC_STATUSB_DSU_Pos         1            /**< \brief (PAC_STATUSB) DSU APB Protect Enable */
#define PAC_STATUSB_DSU             (_U_(0x1) << PAC_STATUSB_DSU_Pos)
#define PAC_STATUSB_NVMCTRL_Pos     2            /**< \brief (PAC_STATUSB) NVMCTRL APB Protect Enable */
#define PAC_STATUSB_NVMCTRL         (_U_(0x1) << PAC_STATUSB_NVMCTRL_Pos)
#define PAC_STATUSB_CMCC_Pos        3            /**< \brief (PAC_STATUSB) CMCC APB Protect Enable */
#define PAC_STATUSB_CMCC            (_U_(0x1) << PAC_STATUSB_CMCC_Pos)
#define PAC_STATUSB_PORT_Pos        4            /**< \brief (PAC_STATUSB) PORT APB Protect Enable */
#define PAC_STATUSB_PORT            (_U_(0x1) << PAC_STATUSB_PORT_Pos)
#define PAC_STATUSB_DMAC_Pos        5            /**< \brief (PAC_STATUSB) DMAC APB Protect Enable */
#define PAC_STATUSB_DMAC            (_U_(0x1) << PAC_STATUSB_DMAC_Pos)
#define PAC_STATUSB_HMATRIX_Pos     6            /**< \brief (PAC_STATUSB) HMATRIX APB Protect Enable */
#define PAC_STATUSB_HMATRIX         (_U_(0x1) << PAC_STATUSB_HMATRIX_Pos)
#define PAC_STATUSB_EVSYS_Pos       7            /**< \brief (PAC_STATUSB) EVSYS APB Protect Enable */
#define PAC_STATUSB_EVSYS           (_U_(0x1) << PAC_STATUSB_EVSYS_Pos)
#define PAC_STATUSB_SERCOM2_Pos     9            /**< \brief (PAC_STATUSB) SERCOM2 APB Protect Enable */
#define PAC_STATUSB_SERCOM2         (_U_(0x1) << PAC_STATUSB_SERCOM2_Pos)
#define PAC_STATUSB_SERCOM3_Pos     10           /**< \brief (PAC_STATUSB) SERCOM3 APB Protect Enable */
#define PAC_STATUSB_SERCOM3         (_U_(0x1) << PAC_STATUSB_SERCOM3_Pos)
#define PAC_STATUSB_TCC0_Pos        11           /**< \brief (PAC_STATUSB) TCC0 APB Protect Enable */
#define PAC_STATUSB_TCC0            (_U_(0x1) << PAC_STATUSB_TCC0_Pos)
#define PAC_STATUSB_TCC1_Pos        12           /**< \brief (PAC_STATUSB) TCC1 APB Protect Enable */
#define PAC_STATUSB_TCC1            (_U_(0x1) << PAC_STATUSB_TCC1_Pos)
#define PAC_STATUSB_TC2_Pos         13           /**< \brief (PAC_STATUSB) TC2 APB Protect Enable */
#define PAC_STATUSB_TC2             (_U_(0x1) << PAC_STATUSB_TC2_Pos)
#define PAC_STATUSB_TC3_Pos         14           /**< \brief (PAC_STATUSB) TC3 APB Protect Enable */
#define PAC_STATUSB_TC3             (_U_(0x1) << PAC_STATUSB_TC3_Pos)
#define PAC_STATUSB_TAL_Pos         15           /**< \brief (PAC_STATUSB) TAL APB Protect Enable */
#define PAC_STATUSB_TAL             (_U_(0x1) << PAC_STATUSB_TAL_Pos)
#define PAC_STATUSB_RAMECC_Pos      16           /**< \brief (PAC_STATUSB) RAMECC APB Protect Enable */
#define PAC_STATUSB_RAMECC          (_U_(0x1) << PAC_STATUSB_RAMECC_Pos)
#define PAC_STATUSB_MASK            _U_(0x0001FEFF) /**< \brief (PAC_STATUSB) MASK Register */

/* -------- PAC_STATUSC : (PAC Offset: 0x3C) (R/  32) Peripheral write protection status - Bridge C -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :3;               /*!< bit:  0.. 2  Reserved                           */
    uint32_t TCC2_:1;          /*!< bit:      3  TCC2 APB Protect Enable            */
    uint32_t TCC3_:1;          /*!< bit:      4  TCC3 APB Protect Enable            */
    uint32_t TC4_:1;           /*!< bit:      5  TC4 APB Protect Enable             */
    uint32_t TC5_:1;           /*!< bit:      6  TC5 APB Protect Enable             */
    uint32_t PDEC_:1;          /*!< bit:      7  PDEC APB Protect Enable            */
    uint32_t AC_:1;            /*!< bit:      8  AC APB Protect Enable              */
    uint32_t AES_:1;           /*!< bit:      9  AES APB Protect Enable             */
    uint32_t TRNG_:1;          /*!< bit:     10  TRNG APB Protect Enable            */
    uint32_t ICM_:1;           /*!< bit:     11  ICM APB Protect Enable             */
    uint32_t PUKCC_:1;         /*!< bit:     12  PUKCC APB Protect Enable           */
    uint32_t QSPI_:1;          /*!< bit:     13  QSPI APB Protect Enable            */
    uint32_t CCL_:1;           /*!< bit:     14  CCL APB Protect Enable             */
    uint32_t :17;              /*!< bit: 15..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_STATUSC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_STATUSC_OFFSET          0x3C         /**< \brief (PAC_STATUSC offset) Peripheral write protection status - Bridge C */
#define PAC_STATUSC_RESETVALUE      _U_(0x00000000) /**< \brief (PAC_STATUSC reset_value) Peripheral write protection status - Bridge C */

#define PAC_STATUSC_TCC2_Pos        3            /**< \brief (PAC_STATUSC) TCC2 APB Protect Enable */
#define PAC_STATUSC_TCC2            (_U_(0x1) << PAC_STATUSC_TCC2_Pos)
#define PAC_STATUSC_TCC3_Pos        4            /**< \brief (PAC_STATUSC) TCC3 APB Protect Enable */
#define PAC_STATUSC_TCC3            (_U_(0x1) << PAC_STATUSC_TCC3_Pos)
#define PAC_STATUSC_TC4_Pos         5            /**< \brief (PAC_STATUSC) TC4 APB Protect Enable */
#define PAC_STATUSC_TC4             (_U_(0x1) << PAC_STATUSC_TC4_Pos)
#define PAC_STATUSC_TC5_Pos         6            /**< \brief (PAC_STATUSC) TC5 APB Protect Enable */
#define PAC_STATUSC_TC5             (_U_(0x1) << PAC_STATUSC_TC5_Pos)
#define PAC_STATUSC_PDEC_Pos        7            /**< \brief (PAC_STATUSC) PDEC APB Protect Enable */
#define PAC_STATUSC_PDEC            (_U_(0x1) << PAC_STATUSC_PDEC_Pos)
#define PAC_STATUSC_AC_Pos          8            /**< \brief (PAC_STATUSC) AC APB Protect Enable */
#define PAC_STATUSC_AC              (_U_(0x1) << PAC_STATUSC_AC_Pos)
#define PAC_STATUSC_AES_Pos         9            /**< \brief (PAC_STATUSC) AES APB Protect Enable */
#define PAC_STATUSC_AES             (_U_(0x1) << PAC_STATUSC_AES_Pos)
#define PAC_STATUSC_TRNG_Pos        10           /**< \brief (PAC_STATUSC) TRNG APB Protect Enable */
#define PAC_STATUSC_TRNG            (_U_(0x1) << PAC_STATUSC_TRNG_Pos)
#define PAC_STATUSC_ICM_Pos         11           /**< \brief (PAC_STATUSC) ICM APB Protect Enable */
#define PAC_STATUSC_ICM             (_U_(0x1) << PAC_STATUSC_ICM_Pos)
#define PAC_STATUSC_PUKCC_Pos       12           /**< \brief (PAC_STATUSC) PUKCC APB Protect Enable */
#define PAC_STATUSC_PUKCC           (_U_(0x1) << PAC_STATUSC_PUKCC_Pos)
#define PAC_STATUSC_QSPI_Pos        13           /**< \brief (PAC_STATUSC) QSPI APB Protect Enable */
#define PAC_STATUSC_QSPI            (_U_(0x1) << PAC_STATUSC_QSPI_Pos)
#define PAC_STATUSC_CCL_Pos         14           /**< \brief (PAC_STATUSC) CCL APB Protect Enable */
#define PAC_STATUSC_CCL             (_U_(0x1) << PAC_STATUSC_CCL_Pos)
#define PAC_STATUSC_MASK            _U_(0x00007FF8) /**< \brief (PAC_STATUSC) MASK Register */

/* -------- PAC_STATUSD : (PAC Offset: 0x40) (R/  32) Peripheral write protection status - Bridge D -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SERCOM4_:1;       /*!< bit:      0  SERCOM4 APB Protect Enable         */
    uint32_t SERCOM5_:1;       /*!< bit:      1  SERCOM5 APB Protect Enable         */
    uint32_t SERCOM6_:1;       /*!< bit:      2  SERCOM6 APB Protect Enable         */
    uint32_t SERCOM7_:1;       /*!< bit:      3  SERCOM7 APB Protect Enable         */
    uint32_t TCC4_:1;          /*!< bit:      4  TCC4 APB Protect Enable            */
    uint32_t TC6_:1;           /*!< bit:      5  TC6 APB Protect Enable             */
    uint32_t TC7_:1;           /*!< bit:      6  TC7 APB Protect Enable             */
    uint32_t ADC0_:1;          /*!< bit:      7  ADC0 APB Protect Enable            */
    uint32_t ADC1_:1;          /*!< bit:      8  ADC1 APB Protect Enable            */
    uint32_t DAC_:1;           /*!< bit:      9  DAC APB Protect Enable             */
    uint32_t I2S_:1;           /*!< bit:     10  I2S APB Protect Enable             */
    uint32_t PCC_:1;           /*!< bit:     11  PCC APB Protect Enable             */
    uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_STATUSD_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_STATUSD_OFFSET          0x40         /**< \brief (PAC_STATUSD offset) Peripheral write protection status - Bridge D */
#define PAC_STATUSD_RESETVALUE      _U_(0x00000000) /**< \brief (PAC_STATUSD reset_value) Peripheral write protection status - Bridge D */

#define PAC_STATUSD_SERCOM4_Pos     0            /**< \brief (PAC_STATUSD) SERCOM4 APB Protect Enable */
#define PAC_STATUSD_SERCOM4         (_U_(0x1) << PAC_STATUSD_SERCOM4_Pos)
#define PAC_STATUSD_SERCOM5_Pos     1            /**< \brief (PAC_STATUSD) SERCOM5 APB Protect Enable */
#define PAC_STATUSD_SERCOM5         (_U_(0x1) << PAC_STATUSD_SERCOM5_Pos)
#define PAC_STATUSD_SERCOM6_Pos     2            /**< \brief (PAC_STATUSD) SERCOM6 APB Protect Enable */
#define PAC_STATUSD_SERCOM6         (_U_(0x1) << PAC_STATUSD_SERCOM6_Pos)
#define PAC_STATUSD_SERCOM7_Pos     3            /**< \brief (PAC_STATUSD) SERCOM7 APB Protect Enable */
#define PAC_STATUSD_SERCOM7         (_U_(0x1) << PAC_STATUSD_SERCOM7_Pos)
#define PAC_STATUSD_TCC4_Pos        4            /**< \brief (PAC_STATUSD) TCC4 APB Protect Enable */
#define PAC_STATUSD_TCC4            (_U_(0x1) << PAC_STATUSD_TCC4_Pos)
#define PAC_STATUSD_TC6_Pos         5            /**< \brief (PAC_STATUSD) TC6 APB Protect Enable */
#define PAC_STATUSD_TC6             (_U_(0x1) << PAC_STATUSD_TC6_Pos)
#define PAC_STATUSD_TC7_Pos         6            /**< \brief (PAC_STATUSD) TC7 APB Protect Enable */
#define PAC_STATUSD_TC7             (_U_(0x1) << PAC_STATUSD_TC7_Pos)
#define PAC_STATUSD_ADC0_Pos        7            /**< \brief (PAC_STATUSD) ADC0 APB Protect Enable */
#define PAC_STATUSD_ADC0            (_U_(0x1) << PAC_STATUSD_ADC0_Pos)
#define PAC_STATUSD_ADC1_Pos        8            /**< \brief (PAC_STATUSD) ADC1 APB Protect Enable */
#define PAC_STATUSD_ADC1            (_U_(0x1) << PAC_STATUSD_ADC1_Pos)
#define PAC_STATUSD_DAC_Pos         9            /**< \brief (PAC_STATUSD) DAC APB Protect Enable */
#define PAC_STATUSD_DAC             (_U_(0x1) << PAC_STATUSD_DAC_Pos)
#define PAC_STATUSD_I2S_Pos         10           /**< \brief (PAC_STATUSD) I2S APB Protect Enable */
#define PAC_STATUSD_I2S             (_U_(0x1) << PAC_STATUSD_I2S_Pos)
#define PAC_STATUSD_PCC_Pos         11           /**< \brief (PAC_STATUSD) PCC APB Protect Enable */
#define PAC_STATUSD_PCC             (_U_(0x1) << PAC_STATUSD_PCC_Pos)
#define PAC_STATUSD_MASK            _U_(0x00000FFF) /**< \brief (PAC_STATUSD) MASK Register */

/** \brief PAC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO PAC_WRCTRL_Type           WRCTRL;      /**< \brief Offset: 0x00 (R/W 32) Write control */
  __IO PAC_EVCTRL_Type           EVCTRL;      /**< \brief Offset: 0x04 (R/W  8) Event control */
       RoReg8                    Reserved1[0x3];
  __IO PAC_INTENCLR_Type         INTENCLR;    /**< \brief Offset: 0x08 (R/W  8) Interrupt enable clear */
  __IO PAC_INTENSET_Type         INTENSET;    /**< \brief Offset: 0x09 (R/W  8) Interrupt enable set */
       RoReg8                    Reserved2[0x6];
  __IO PAC_INTFLAGAHB_Type       INTFLAGAHB;  /**< \brief Offset: 0x10 (R/W 32) Bridge interrupt flag status */
  __IO PAC_INTFLAGA_Type         INTFLAGA;    /**< \brief Offset: 0x14 (R/W 32) Peripheral interrupt flag status - Bridge A */
  __IO PAC_INTFLAGB_Type         INTFLAGB;    /**< \brief Offset: 0x18 (R/W 32) Peripheral interrupt flag status - Bridge B */
  __IO PAC_INTFLAGC_Type         INTFLAGC;    /**< \brief Offset: 0x1C (R/W 32) Peripheral interrupt flag status - Bridge C */
  __IO PAC_INTFLAGD_Type         INTFLAGD;    /**< \brief Offset: 0x20 (R/W 32) Peripheral interrupt flag status - Bridge D */
       RoReg8                    Reserved3[0x10];
  __I  PAC_STATUSA_Type          STATUSA;     /**< \brief Offset: 0x34 (R/  32) Peripheral write protection status - Bridge A */
  __I  PAC_STATUSB_Type          STATUSB;     /**< \brief Offset: 0x38 (R/  32) Peripheral write protection status - Bridge B */
  __I  PAC_STATUSC_Type          STATUSC;     /**< \brief Offset: 0x3C (R/  32) Peripheral write protection status - Bridge C */
  __I  PAC_STATUSD_Type          STATUSD;     /**< \brief Offset: 0x40 (R/  32) Peripheral write protection status - Bridge D */
} Pac;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_PAC_COMPONENT_ */
