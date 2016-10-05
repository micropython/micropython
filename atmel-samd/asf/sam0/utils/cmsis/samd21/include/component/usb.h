/**
 * \file
 *
 * \brief Component description for USB
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

#ifndef _SAMD21_USB_COMPONENT_
#define _SAMD21_USB_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR USB */
/* ========================================================================== */
/** \addtogroup SAMD21_USB Universal Serial Bus */
/*@{*/

#define USB_U2222
#define REV_USB                     0x103

/* -------- USB_CTRLA : (USB Offset: 0x000) (R/W  8) Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  ENABLE:1;         /*!< bit:      1  Enable                             */
    uint8_t  RUNSTDBY:1;       /*!< bit:      2  Run in Standby Mode                */
    uint8_t  :4;               /*!< bit:  3.. 6  Reserved                           */
    uint8_t  MODE:1;           /*!< bit:      7  Operating Mode                     */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_CTRLA_OFFSET            0x000        /**< \brief (USB_CTRLA offset) Control A */
#define USB_CTRLA_RESETVALUE        0x00ul       /**< \brief (USB_CTRLA reset_value) Control A */

#define USB_CTRLA_SWRST_Pos         0            /**< \brief (USB_CTRLA) Software Reset */
#define USB_CTRLA_SWRST             (0x1ul << USB_CTRLA_SWRST_Pos)
#define USB_CTRLA_ENABLE_Pos        1            /**< \brief (USB_CTRLA) Enable */
#define USB_CTRLA_ENABLE            (0x1ul << USB_CTRLA_ENABLE_Pos)
#define USB_CTRLA_RUNSTDBY_Pos      2            /**< \brief (USB_CTRLA) Run in Standby Mode */
#define USB_CTRLA_RUNSTDBY          (0x1ul << USB_CTRLA_RUNSTDBY_Pos)
#define USB_CTRLA_MODE_Pos          7            /**< \brief (USB_CTRLA) Operating Mode */
#define USB_CTRLA_MODE              (0x1ul << USB_CTRLA_MODE_Pos)
#define   USB_CTRLA_MODE_DEVICE_Val       0x0ul  /**< \brief (USB_CTRLA) Device Mode */
#define   USB_CTRLA_MODE_HOST_Val         0x1ul  /**< \brief (USB_CTRLA) Host Mode */
#define USB_CTRLA_MODE_DEVICE       (USB_CTRLA_MODE_DEVICE_Val     << USB_CTRLA_MODE_Pos)
#define USB_CTRLA_MODE_HOST         (USB_CTRLA_MODE_HOST_Val       << USB_CTRLA_MODE_Pos)
#define USB_CTRLA_MASK              0x87ul       /**< \brief (USB_CTRLA) MASK Register */

/* -------- USB_SYNCBUSY : (USB Offset: 0x002) (R/   8) Synchronization Busy -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset Synchronization Busy */
    uint8_t  ENABLE:1;         /*!< bit:      1  Enable Synchronization Busy        */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_SYNCBUSY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_SYNCBUSY_OFFSET         0x002        /**< \brief (USB_SYNCBUSY offset) Synchronization Busy */
#define USB_SYNCBUSY_RESETVALUE     0x00ul       /**< \brief (USB_SYNCBUSY reset_value) Synchronization Busy */

#define USB_SYNCBUSY_SWRST_Pos      0            /**< \brief (USB_SYNCBUSY) Software Reset Synchronization Busy */
#define USB_SYNCBUSY_SWRST          (0x1ul << USB_SYNCBUSY_SWRST_Pos)
#define USB_SYNCBUSY_ENABLE_Pos     1            /**< \brief (USB_SYNCBUSY) Enable Synchronization Busy */
#define USB_SYNCBUSY_ENABLE         (0x1ul << USB_SYNCBUSY_ENABLE_Pos)
#define USB_SYNCBUSY_MASK           0x03ul       /**< \brief (USB_SYNCBUSY) MASK Register */

/* -------- USB_QOSCTRL : (USB Offset: 0x003) (R/W  8) USB Quality Of Service -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CQOS:2;           /*!< bit:  0.. 1  Configuration Quality of Service   */
    uint8_t  DQOS:2;           /*!< bit:  2.. 3  Data Quality of Service            */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_QOSCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_QOSCTRL_OFFSET          0x003        /**< \brief (USB_QOSCTRL offset) USB Quality Of Service */
#define USB_QOSCTRL_RESETVALUE      0x05ul       /**< \brief (USB_QOSCTRL reset_value) USB Quality Of Service */

#define USB_QOSCTRL_CQOS_Pos        0            /**< \brief (USB_QOSCTRL) Configuration Quality of Service */
#define USB_QOSCTRL_CQOS_Msk        (0x3ul << USB_QOSCTRL_CQOS_Pos)
#define USB_QOSCTRL_CQOS(value)     (USB_QOSCTRL_CQOS_Msk & ((value) << USB_QOSCTRL_CQOS_Pos))
#define   USB_QOSCTRL_CQOS_DISABLE_Val    0x0ul  /**< \brief (USB_QOSCTRL) Background (no sensitive operation) */
#define   USB_QOSCTRL_CQOS_LOW_Val        0x1ul  /**< \brief (USB_QOSCTRL) Sensitive Bandwidth */
#define   USB_QOSCTRL_CQOS_MEDIUM_Val     0x2ul  /**< \brief (USB_QOSCTRL) Sensitive Latency */
#define   USB_QOSCTRL_CQOS_HIGH_Val       0x3ul  /**< \brief (USB_QOSCTRL) Critical Latency */
#define USB_QOSCTRL_CQOS_DISABLE    (USB_QOSCTRL_CQOS_DISABLE_Val  << USB_QOSCTRL_CQOS_Pos)
#define USB_QOSCTRL_CQOS_LOW        (USB_QOSCTRL_CQOS_LOW_Val      << USB_QOSCTRL_CQOS_Pos)
#define USB_QOSCTRL_CQOS_MEDIUM     (USB_QOSCTRL_CQOS_MEDIUM_Val   << USB_QOSCTRL_CQOS_Pos)
#define USB_QOSCTRL_CQOS_HIGH       (USB_QOSCTRL_CQOS_HIGH_Val     << USB_QOSCTRL_CQOS_Pos)
#define USB_QOSCTRL_DQOS_Pos        2            /**< \brief (USB_QOSCTRL) Data Quality of Service */
#define USB_QOSCTRL_DQOS_Msk        (0x3ul << USB_QOSCTRL_DQOS_Pos)
#define USB_QOSCTRL_DQOS(value)     (USB_QOSCTRL_DQOS_Msk & ((value) << USB_QOSCTRL_DQOS_Pos))
#define   USB_QOSCTRL_DQOS_DISABLE_Val    0x0ul  /**< \brief (USB_QOSCTRL) Background (no sensitive operation) */
#define   USB_QOSCTRL_DQOS_LOW_Val        0x1ul  /**< \brief (USB_QOSCTRL) Sensitive Bandwidth */
#define   USB_QOSCTRL_DQOS_MEDIUM_Val     0x2ul  /**< \brief (USB_QOSCTRL) Sensitive Latency */
#define   USB_QOSCTRL_DQOS_HIGH_Val       0x3ul  /**< \brief (USB_QOSCTRL) Critical Latency */
#define USB_QOSCTRL_DQOS_DISABLE    (USB_QOSCTRL_DQOS_DISABLE_Val  << USB_QOSCTRL_DQOS_Pos)
#define USB_QOSCTRL_DQOS_LOW        (USB_QOSCTRL_DQOS_LOW_Val      << USB_QOSCTRL_DQOS_Pos)
#define USB_QOSCTRL_DQOS_MEDIUM     (USB_QOSCTRL_DQOS_MEDIUM_Val   << USB_QOSCTRL_DQOS_Pos)
#define USB_QOSCTRL_DQOS_HIGH       (USB_QOSCTRL_DQOS_HIGH_Val     << USB_QOSCTRL_DQOS_Pos)
#define USB_QOSCTRL_MASK            0x0Ful       /**< \brief (USB_QOSCTRL) MASK Register */

/* -------- USB_DEVICE_CTRLB : (USB Offset: 0x008) (R/W 16) DEVICE DEVICE Control B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t DETACH:1;         /*!< bit:      0  Detach                             */
    uint16_t UPRSM:1;          /*!< bit:      1  Upstream Resume                    */
    uint16_t SPDCONF:2;        /*!< bit:  2.. 3  Speed Configuration                */
    uint16_t NREPLY:1;         /*!< bit:      4  No Reply                           */
    uint16_t TSTJ:1;           /*!< bit:      5  Test mode J                        */
    uint16_t TSTK:1;           /*!< bit:      6  Test mode K                        */
    uint16_t TSTPCKT:1;        /*!< bit:      7  Test packet mode                   */
    uint16_t OPMODE2:1;        /*!< bit:      8  Specific Operational Mode          */
    uint16_t GNAK:1;           /*!< bit:      9  Global NAK                         */
    uint16_t LPMHDSK:2;        /*!< bit: 10..11  Link Power Management Handshake    */
    uint16_t :4;               /*!< bit: 12..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} USB_DEVICE_CTRLB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_DEVICE_CTRLB_OFFSET     0x008        /**< \brief (USB_DEVICE_CTRLB offset) DEVICE Control B */
#define USB_DEVICE_CTRLB_RESETVALUE 0x0001ul     /**< \brief (USB_DEVICE_CTRLB reset_value) DEVICE Control B */

#define USB_DEVICE_CTRLB_DETACH_Pos 0            /**< \brief (USB_DEVICE_CTRLB) Detach */
#define USB_DEVICE_CTRLB_DETACH     (0x1ul << USB_DEVICE_CTRLB_DETACH_Pos)
#define USB_DEVICE_CTRLB_UPRSM_Pos  1            /**< \brief (USB_DEVICE_CTRLB) Upstream Resume */
#define USB_DEVICE_CTRLB_UPRSM      (0x1ul << USB_DEVICE_CTRLB_UPRSM_Pos)
#define USB_DEVICE_CTRLB_SPDCONF_Pos 2            /**< \brief (USB_DEVICE_CTRLB) Speed Configuration */
#define USB_DEVICE_CTRLB_SPDCONF_Msk (0x3ul << USB_DEVICE_CTRLB_SPDCONF_Pos)
#define USB_DEVICE_CTRLB_SPDCONF(value) (USB_DEVICE_CTRLB_SPDCONF_Msk & ((value) << USB_DEVICE_CTRLB_SPDCONF_Pos))
#define   USB_DEVICE_CTRLB_SPDCONF_FS_Val 0x0ul  /**< \brief (USB_DEVICE_CTRLB) FS : Full Speed */
#define   USB_DEVICE_CTRLB_SPDCONF_LS_Val 0x1ul  /**< \brief (USB_DEVICE_CTRLB) LS : Low Speed */
#define   USB_DEVICE_CTRLB_SPDCONF_HS_Val 0x2ul  /**< \brief (USB_DEVICE_CTRLB) HS : High Speed capable */
#define   USB_DEVICE_CTRLB_SPDCONF_HSTM_Val 0x3ul  /**< \brief (USB_DEVICE_CTRLB) HSTM: High Speed Test Mode (force high-speed mode for test mode) */
#define USB_DEVICE_CTRLB_SPDCONF_FS (USB_DEVICE_CTRLB_SPDCONF_FS_Val << USB_DEVICE_CTRLB_SPDCONF_Pos)
#define USB_DEVICE_CTRLB_SPDCONF_LS (USB_DEVICE_CTRLB_SPDCONF_LS_Val << USB_DEVICE_CTRLB_SPDCONF_Pos)
#define USB_DEVICE_CTRLB_SPDCONF_HS (USB_DEVICE_CTRLB_SPDCONF_HS_Val << USB_DEVICE_CTRLB_SPDCONF_Pos)
#define USB_DEVICE_CTRLB_SPDCONF_HSTM (USB_DEVICE_CTRLB_SPDCONF_HSTM_Val << USB_DEVICE_CTRLB_SPDCONF_Pos)
#define USB_DEVICE_CTRLB_NREPLY_Pos 4            /**< \brief (USB_DEVICE_CTRLB) No Reply */
#define USB_DEVICE_CTRLB_NREPLY     (0x1ul << USB_DEVICE_CTRLB_NREPLY_Pos)
#define USB_DEVICE_CTRLB_TSTJ_Pos   5            /**< \brief (USB_DEVICE_CTRLB) Test mode J */
#define USB_DEVICE_CTRLB_TSTJ       (0x1ul << USB_DEVICE_CTRLB_TSTJ_Pos)
#define USB_DEVICE_CTRLB_TSTK_Pos   6            /**< \brief (USB_DEVICE_CTRLB) Test mode K */
#define USB_DEVICE_CTRLB_TSTK       (0x1ul << USB_DEVICE_CTRLB_TSTK_Pos)
#define USB_DEVICE_CTRLB_TSTPCKT_Pos 7            /**< \brief (USB_DEVICE_CTRLB) Test packet mode */
#define USB_DEVICE_CTRLB_TSTPCKT    (0x1ul << USB_DEVICE_CTRLB_TSTPCKT_Pos)
#define USB_DEVICE_CTRLB_OPMODE2_Pos 8            /**< \brief (USB_DEVICE_CTRLB) Specific Operational Mode */
#define USB_DEVICE_CTRLB_OPMODE2    (0x1ul << USB_DEVICE_CTRLB_OPMODE2_Pos)
#define USB_DEVICE_CTRLB_GNAK_Pos   9            /**< \brief (USB_DEVICE_CTRLB) Global NAK */
#define USB_DEVICE_CTRLB_GNAK       (0x1ul << USB_DEVICE_CTRLB_GNAK_Pos)
#define USB_DEVICE_CTRLB_LPMHDSK_Pos 10           /**< \brief (USB_DEVICE_CTRLB) Link Power Management Handshake */
#define USB_DEVICE_CTRLB_LPMHDSK_Msk (0x3ul << USB_DEVICE_CTRLB_LPMHDSK_Pos)
#define USB_DEVICE_CTRLB_LPMHDSK(value) (USB_DEVICE_CTRLB_LPMHDSK_Msk & ((value) << USB_DEVICE_CTRLB_LPMHDSK_Pos))
#define   USB_DEVICE_CTRLB_LPMHDSK_NO_Val 0x0ul  /**< \brief (USB_DEVICE_CTRLB) No handshake. LPM is not supported */
#define   USB_DEVICE_CTRLB_LPMHDSK_ACK_Val 0x1ul  /**< \brief (USB_DEVICE_CTRLB) ACK */
#define   USB_DEVICE_CTRLB_LPMHDSK_NYET_Val 0x2ul  /**< \brief (USB_DEVICE_CTRLB) NYET */
#define   USB_DEVICE_CTRLB_LPMHDSK_STALL_Val 0x3ul  /**< \brief (USB_DEVICE_CTRLB) STALL */
#define USB_DEVICE_CTRLB_LPMHDSK_NO (USB_DEVICE_CTRLB_LPMHDSK_NO_Val << USB_DEVICE_CTRLB_LPMHDSK_Pos)
#define USB_DEVICE_CTRLB_LPMHDSK_ACK (USB_DEVICE_CTRLB_LPMHDSK_ACK_Val << USB_DEVICE_CTRLB_LPMHDSK_Pos)
#define USB_DEVICE_CTRLB_LPMHDSK_NYET (USB_DEVICE_CTRLB_LPMHDSK_NYET_Val << USB_DEVICE_CTRLB_LPMHDSK_Pos)
#define USB_DEVICE_CTRLB_LPMHDSK_STALL (USB_DEVICE_CTRLB_LPMHDSK_STALL_Val << USB_DEVICE_CTRLB_LPMHDSK_Pos)
#define USB_DEVICE_CTRLB_MASK       0x0FFFul     /**< \brief (USB_DEVICE_CTRLB) MASK Register */

/* -------- USB_HOST_CTRLB : (USB Offset: 0x008) (R/W 16) HOST HOST Control B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t :1;               /*!< bit:      0  Reserved                           */
    uint16_t RESUME:1;         /*!< bit:      1  Send USB Resume                    */
    uint16_t SPDCONF:2;        /*!< bit:  2.. 3  Speed Configuration for Host       */
    uint16_t :1;               /*!< bit:      4  Reserved                           */
    uint16_t TSTJ:1;           /*!< bit:      5  Test mode J                        */
    uint16_t TSTK:1;           /*!< bit:      6  Test mode K                        */
    uint16_t :1;               /*!< bit:      7  Reserved                           */
    uint16_t SOFE:1;           /*!< bit:      8  Start of Frame Generation Enable   */
    uint16_t BUSRESET:1;       /*!< bit:      9  Send USB Reset                     */
    uint16_t VBUSOK:1;         /*!< bit:     10  VBUS is OK                         */
    uint16_t L1RESUME:1;       /*!< bit:     11  Send L1 Resume                     */
    uint16_t :4;               /*!< bit: 12..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} USB_HOST_CTRLB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_CTRLB_OFFSET       0x008        /**< \brief (USB_HOST_CTRLB offset) HOST Control B */
#define USB_HOST_CTRLB_RESETVALUE   0x0000ul     /**< \brief (USB_HOST_CTRLB reset_value) HOST Control B */

#define USB_HOST_CTRLB_RESUME_Pos   1            /**< \brief (USB_HOST_CTRLB) Send USB Resume */
#define USB_HOST_CTRLB_RESUME       (0x1ul << USB_HOST_CTRLB_RESUME_Pos)
#define USB_HOST_CTRLB_SPDCONF_Pos  2            /**< \brief (USB_HOST_CTRLB) Speed Configuration for Host */
#define USB_HOST_CTRLB_SPDCONF_Msk  (0x3ul << USB_HOST_CTRLB_SPDCONF_Pos)
#define USB_HOST_CTRLB_SPDCONF(value) (USB_HOST_CTRLB_SPDCONF_Msk & ((value) << USB_HOST_CTRLB_SPDCONF_Pos))
#define   USB_HOST_CTRLB_SPDCONF_NORMAL_Val 0x0ul  /**< \brief (USB_HOST_CTRLB) Normal mode: the host starts in full-speed mode and performs a high-speed reset to switch to the high speed mode if the downstream peripheral is high-speed capable. */
#define   USB_HOST_CTRLB_SPDCONF_FS_Val   0x3ul  /**< \brief (USB_HOST_CTRLB) Full-speed: the host remains in full-speed mode whatever is the peripheral speed capability. Relevant in UTMI mode only. */
#define USB_HOST_CTRLB_SPDCONF_NORMAL (USB_HOST_CTRLB_SPDCONF_NORMAL_Val << USB_HOST_CTRLB_SPDCONF_Pos)
#define USB_HOST_CTRLB_SPDCONF_FS   (USB_HOST_CTRLB_SPDCONF_FS_Val << USB_HOST_CTRLB_SPDCONF_Pos)
#define USB_HOST_CTRLB_TSTJ_Pos     5            /**< \brief (USB_HOST_CTRLB) Test mode J */
#define USB_HOST_CTRLB_TSTJ         (0x1ul << USB_HOST_CTRLB_TSTJ_Pos)
#define USB_HOST_CTRLB_TSTK_Pos     6            /**< \brief (USB_HOST_CTRLB) Test mode K */
#define USB_HOST_CTRLB_TSTK         (0x1ul << USB_HOST_CTRLB_TSTK_Pos)
#define USB_HOST_CTRLB_SOFE_Pos     8            /**< \brief (USB_HOST_CTRLB) Start of Frame Generation Enable */
#define USB_HOST_CTRLB_SOFE         (0x1ul << USB_HOST_CTRLB_SOFE_Pos)
#define USB_HOST_CTRLB_BUSRESET_Pos 9            /**< \brief (USB_HOST_CTRLB) Send USB Reset */
#define USB_HOST_CTRLB_BUSRESET     (0x1ul << USB_HOST_CTRLB_BUSRESET_Pos)
#define USB_HOST_CTRLB_VBUSOK_Pos   10           /**< \brief (USB_HOST_CTRLB) VBUS is OK */
#define USB_HOST_CTRLB_VBUSOK       (0x1ul << USB_HOST_CTRLB_VBUSOK_Pos)
#define USB_HOST_CTRLB_L1RESUME_Pos 11           /**< \brief (USB_HOST_CTRLB) Send L1 Resume */
#define USB_HOST_CTRLB_L1RESUME     (0x1ul << USB_HOST_CTRLB_L1RESUME_Pos)
#define USB_HOST_CTRLB_MASK         0x0F6Eul     /**< \brief (USB_HOST_CTRLB) MASK Register */

/* -------- USB_DEVICE_DADD : (USB Offset: 0x00A) (R/W  8) DEVICE DEVICE Device Address -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DADD:7;           /*!< bit:  0.. 6  Device Address                     */
    uint8_t  ADDEN:1;          /*!< bit:      7  Device Address Enable              */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_DEVICE_DADD_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_DEVICE_DADD_OFFSET      0x00A        /**< \brief (USB_DEVICE_DADD offset) DEVICE Device Address */
#define USB_DEVICE_DADD_RESETVALUE  0x00ul       /**< \brief (USB_DEVICE_DADD reset_value) DEVICE Device Address */

#define USB_DEVICE_DADD_DADD_Pos    0            /**< \brief (USB_DEVICE_DADD) Device Address */
#define USB_DEVICE_DADD_DADD_Msk    (0x7Ful << USB_DEVICE_DADD_DADD_Pos)
#define USB_DEVICE_DADD_DADD(value) (USB_DEVICE_DADD_DADD_Msk & ((value) << USB_DEVICE_DADD_DADD_Pos))
#define USB_DEVICE_DADD_ADDEN_Pos   7            /**< \brief (USB_DEVICE_DADD) Device Address Enable */
#define USB_DEVICE_DADD_ADDEN       (0x1ul << USB_DEVICE_DADD_ADDEN_Pos)
#define USB_DEVICE_DADD_MASK        0xFFul       /**< \brief (USB_DEVICE_DADD) MASK Register */

/* -------- USB_HOST_HSOFC : (USB Offset: 0x00A) (R/W  8) HOST HOST Host Start Of Frame Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  FLENC:4;          /*!< bit:  0.. 3  Frame Length Control               */
    uint8_t  :3;               /*!< bit:  4.. 6  Reserved                           */
    uint8_t  FLENCE:1;         /*!< bit:      7  Frame Length Control Enable        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_HOST_HSOFC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_HSOFC_OFFSET       0x00A        /**< \brief (USB_HOST_HSOFC offset) HOST Host Start Of Frame Control */
#define USB_HOST_HSOFC_RESETVALUE   0x00ul       /**< \brief (USB_HOST_HSOFC reset_value) HOST Host Start Of Frame Control */

#define USB_HOST_HSOFC_FLENC_Pos    0            /**< \brief (USB_HOST_HSOFC) Frame Length Control */
#define USB_HOST_HSOFC_FLENC_Msk    (0xFul << USB_HOST_HSOFC_FLENC_Pos)
#define USB_HOST_HSOFC_FLENC(value) (USB_HOST_HSOFC_FLENC_Msk & ((value) << USB_HOST_HSOFC_FLENC_Pos))
#define USB_HOST_HSOFC_FLENCE_Pos   7            /**< \brief (USB_HOST_HSOFC) Frame Length Control Enable */
#define USB_HOST_HSOFC_FLENCE       (0x1ul << USB_HOST_HSOFC_FLENCE_Pos)
#define USB_HOST_HSOFC_MASK         0x8Ful       /**< \brief (USB_HOST_HSOFC) MASK Register */

/* -------- USB_DEVICE_STATUS : (USB Offset: 0x00C) (R/   8) DEVICE DEVICE Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :2;               /*!< bit:  0.. 1  Reserved                           */
    uint8_t  SPEED:2;          /*!< bit:  2.. 3  Speed Status                       */
    uint8_t  :2;               /*!< bit:  4.. 5  Reserved                           */
    uint8_t  LINESTATE:2;      /*!< bit:  6.. 7  USB Line State Status              */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_DEVICE_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_DEVICE_STATUS_OFFSET    0x00C        /**< \brief (USB_DEVICE_STATUS offset) DEVICE Status */
#define USB_DEVICE_STATUS_RESETVALUE 0x40ul       /**< \brief (USB_DEVICE_STATUS reset_value) DEVICE Status */

#define USB_DEVICE_STATUS_SPEED_Pos 2            /**< \brief (USB_DEVICE_STATUS) Speed Status */
#define USB_DEVICE_STATUS_SPEED_Msk (0x3ul << USB_DEVICE_STATUS_SPEED_Pos)
#define USB_DEVICE_STATUS_SPEED(value) (USB_DEVICE_STATUS_SPEED_Msk & ((value) << USB_DEVICE_STATUS_SPEED_Pos))
#define   USB_DEVICE_STATUS_SPEED_FS_Val  0x0ul  /**< \brief (USB_DEVICE_STATUS) Full-speed mode */
#define   USB_DEVICE_STATUS_SPEED_HS_Val  0x1ul  /**< \brief (USB_DEVICE_STATUS) High-speed mode */
#define   USB_DEVICE_STATUS_SPEED_LS_Val  0x2ul  /**< \brief (USB_DEVICE_STATUS) Low-speed mode */
#define USB_DEVICE_STATUS_SPEED_FS  (USB_DEVICE_STATUS_SPEED_FS_Val << USB_DEVICE_STATUS_SPEED_Pos)
#define USB_DEVICE_STATUS_SPEED_HS  (USB_DEVICE_STATUS_SPEED_HS_Val << USB_DEVICE_STATUS_SPEED_Pos)
#define USB_DEVICE_STATUS_SPEED_LS  (USB_DEVICE_STATUS_SPEED_LS_Val << USB_DEVICE_STATUS_SPEED_Pos)
#define USB_DEVICE_STATUS_LINESTATE_Pos 6            /**< \brief (USB_DEVICE_STATUS) USB Line State Status */
#define USB_DEVICE_STATUS_LINESTATE_Msk (0x3ul << USB_DEVICE_STATUS_LINESTATE_Pos)
#define USB_DEVICE_STATUS_LINESTATE(value) (USB_DEVICE_STATUS_LINESTATE_Msk & ((value) << USB_DEVICE_STATUS_LINESTATE_Pos))
#define   USB_DEVICE_STATUS_LINESTATE_0_Val 0x0ul  /**< \brief (USB_DEVICE_STATUS) SE0/RESET */
#define   USB_DEVICE_STATUS_LINESTATE_1_Val 0x1ul  /**< \brief (USB_DEVICE_STATUS) FS-J or LS-K State */
#define   USB_DEVICE_STATUS_LINESTATE_2_Val 0x2ul  /**< \brief (USB_DEVICE_STATUS) FS-K or LS-J State */
#define USB_DEVICE_STATUS_LINESTATE_0 (USB_DEVICE_STATUS_LINESTATE_0_Val << USB_DEVICE_STATUS_LINESTATE_Pos)
#define USB_DEVICE_STATUS_LINESTATE_1 (USB_DEVICE_STATUS_LINESTATE_1_Val << USB_DEVICE_STATUS_LINESTATE_Pos)
#define USB_DEVICE_STATUS_LINESTATE_2 (USB_DEVICE_STATUS_LINESTATE_2_Val << USB_DEVICE_STATUS_LINESTATE_Pos)
#define USB_DEVICE_STATUS_MASK      0xCCul       /**< \brief (USB_DEVICE_STATUS) MASK Register */

/* -------- USB_HOST_STATUS : (USB Offset: 0x00C) (R/W  8) HOST HOST Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :2;               /*!< bit:  0.. 1  Reserved                           */
    uint8_t  SPEED:2;          /*!< bit:  2.. 3  Speed Status                       */
    uint8_t  :2;               /*!< bit:  4.. 5  Reserved                           */
    uint8_t  LINESTATE:2;      /*!< bit:  6.. 7  USB Line State Status              */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_HOST_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_STATUS_OFFSET      0x00C        /**< \brief (USB_HOST_STATUS offset) HOST Status */
#define USB_HOST_STATUS_RESETVALUE  0x00ul       /**< \brief (USB_HOST_STATUS reset_value) HOST Status */

#define USB_HOST_STATUS_SPEED_Pos   2            /**< \brief (USB_HOST_STATUS) Speed Status */
#define USB_HOST_STATUS_SPEED_Msk   (0x3ul << USB_HOST_STATUS_SPEED_Pos)
#define USB_HOST_STATUS_SPEED(value) (USB_HOST_STATUS_SPEED_Msk & ((value) << USB_HOST_STATUS_SPEED_Pos))
#define USB_HOST_STATUS_LINESTATE_Pos 6            /**< \brief (USB_HOST_STATUS) USB Line State Status */
#define USB_HOST_STATUS_LINESTATE_Msk (0x3ul << USB_HOST_STATUS_LINESTATE_Pos)
#define USB_HOST_STATUS_LINESTATE(value) (USB_HOST_STATUS_LINESTATE_Msk & ((value) << USB_HOST_STATUS_LINESTATE_Pos))
#define USB_HOST_STATUS_MASK        0xCCul       /**< \brief (USB_HOST_STATUS) MASK Register */

/* -------- USB_FSMSTATUS : (USB Offset: 0x00D) (R/   8) Finite State Machine Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  FSMSTATE:7;       /*!< bit:  0.. 6  Fine State Machine Status          */
    uint8_t  :1;               /*!< bit:      7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_FSMSTATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_FSMSTATUS_OFFSET        0x00D        /**< \brief (USB_FSMSTATUS offset) Finite State Machine Status */
#define USB_FSMSTATUS_RESETVALUE    0x01ul       /**< \brief (USB_FSMSTATUS reset_value) Finite State Machine Status */

#define USB_FSMSTATUS_FSMSTATE_Pos  0            /**< \brief (USB_FSMSTATUS) Fine State Machine Status */
#define USB_FSMSTATUS_FSMSTATE_Msk  (0x7Ful << USB_FSMSTATUS_FSMSTATE_Pos)
#define USB_FSMSTATUS_FSMSTATE(value) (USB_FSMSTATUS_FSMSTATE_Msk & ((value) << USB_FSMSTATUS_FSMSTATE_Pos))
#define   USB_FSMSTATUS_FSMSTATE_OFF_Val  0x1ul  /**< \brief (USB_FSMSTATUS) OFF (L3). It corresponds to the powered-off, disconnected, and disabled state */
#define   USB_FSMSTATUS_FSMSTATE_ON_Val   0x2ul  /**< \brief (USB_FSMSTATUS) ON (L0). It corresponds to the Idle and Active states */
#define   USB_FSMSTATUS_FSMSTATE_SUSPEND_Val 0x4ul  /**< \brief (USB_FSMSTATUS) SUSPEND (L2) */
#define   USB_FSMSTATUS_FSMSTATE_SLEEP_Val 0x8ul  /**< \brief (USB_FSMSTATUS) SLEEP (L1) */
#define   USB_FSMSTATUS_FSMSTATE_DNRESUME_Val 0x10ul  /**< \brief (USB_FSMSTATUS) DNRESUME. Down Stream Resume. */
#define   USB_FSMSTATUS_FSMSTATE_UPRESUME_Val 0x20ul  /**< \brief (USB_FSMSTATUS) UPRESUME. Up Stream Resume. */
#define   USB_FSMSTATUS_FSMSTATE_RESET_Val 0x40ul  /**< \brief (USB_FSMSTATUS) RESET. USB lines Reset. */
#define USB_FSMSTATUS_FSMSTATE_OFF  (USB_FSMSTATUS_FSMSTATE_OFF_Val << USB_FSMSTATUS_FSMSTATE_Pos)
#define USB_FSMSTATUS_FSMSTATE_ON   (USB_FSMSTATUS_FSMSTATE_ON_Val << USB_FSMSTATUS_FSMSTATE_Pos)
#define USB_FSMSTATUS_FSMSTATE_SUSPEND (USB_FSMSTATUS_FSMSTATE_SUSPEND_Val << USB_FSMSTATUS_FSMSTATE_Pos)
#define USB_FSMSTATUS_FSMSTATE_SLEEP (USB_FSMSTATUS_FSMSTATE_SLEEP_Val << USB_FSMSTATUS_FSMSTATE_Pos)
#define USB_FSMSTATUS_FSMSTATE_DNRESUME (USB_FSMSTATUS_FSMSTATE_DNRESUME_Val << USB_FSMSTATUS_FSMSTATE_Pos)
#define USB_FSMSTATUS_FSMSTATE_UPRESUME (USB_FSMSTATUS_FSMSTATE_UPRESUME_Val << USB_FSMSTATUS_FSMSTATE_Pos)
#define USB_FSMSTATUS_FSMSTATE_RESET (USB_FSMSTATUS_FSMSTATE_RESET_Val << USB_FSMSTATUS_FSMSTATE_Pos)
#define USB_FSMSTATUS_MASK          0x7Ful       /**< \brief (USB_FSMSTATUS) MASK Register */

/* -------- USB_DEVICE_FNUM : (USB Offset: 0x010) (R/  16) DEVICE DEVICE Device Frame Number -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t MFNUM:3;          /*!< bit:  0.. 2  Micro Frame Number                 */
    uint16_t FNUM:11;          /*!< bit:  3..13  Frame Number                       */
    uint16_t :1;               /*!< bit:     14  Reserved                           */
    uint16_t FNCERR:1;         /*!< bit:     15  Frame Number CRC Error             */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} USB_DEVICE_FNUM_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_DEVICE_FNUM_OFFSET      0x010        /**< \brief (USB_DEVICE_FNUM offset) DEVICE Device Frame Number */
#define USB_DEVICE_FNUM_RESETVALUE  0x0000ul     /**< \brief (USB_DEVICE_FNUM reset_value) DEVICE Device Frame Number */

#define USB_DEVICE_FNUM_MFNUM_Pos   0            /**< \brief (USB_DEVICE_FNUM) Micro Frame Number */
#define USB_DEVICE_FNUM_MFNUM_Msk   (0x7ul << USB_DEVICE_FNUM_MFNUM_Pos)
#define USB_DEVICE_FNUM_MFNUM(value) (USB_DEVICE_FNUM_MFNUM_Msk & ((value) << USB_DEVICE_FNUM_MFNUM_Pos))
#define USB_DEVICE_FNUM_FNUM_Pos    3            /**< \brief (USB_DEVICE_FNUM) Frame Number */
#define USB_DEVICE_FNUM_FNUM_Msk    (0x7FFul << USB_DEVICE_FNUM_FNUM_Pos)
#define USB_DEVICE_FNUM_FNUM(value) (USB_DEVICE_FNUM_FNUM_Msk & ((value) << USB_DEVICE_FNUM_FNUM_Pos))
#define USB_DEVICE_FNUM_FNCERR_Pos  15           /**< \brief (USB_DEVICE_FNUM) Frame Number CRC Error */
#define USB_DEVICE_FNUM_FNCERR      (0x1ul << USB_DEVICE_FNUM_FNCERR_Pos)
#define USB_DEVICE_FNUM_MASK        0xBFFFul     /**< \brief (USB_DEVICE_FNUM) MASK Register */

/* -------- USB_HOST_FNUM : (USB Offset: 0x010) (R/W 16) HOST HOST Host Frame Number -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t MFNUM:3;          /*!< bit:  0.. 2  Micro Frame Number                 */
    uint16_t FNUM:11;          /*!< bit:  3..13  Frame Number                       */
    uint16_t :2;               /*!< bit: 14..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} USB_HOST_FNUM_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_FNUM_OFFSET        0x010        /**< \brief (USB_HOST_FNUM offset) HOST Host Frame Number */
#define USB_HOST_FNUM_RESETVALUE    0x0000ul     /**< \brief (USB_HOST_FNUM reset_value) HOST Host Frame Number */

#define USB_HOST_FNUM_MFNUM_Pos     0            /**< \brief (USB_HOST_FNUM) Micro Frame Number */
#define USB_HOST_FNUM_MFNUM_Msk     (0x7ul << USB_HOST_FNUM_MFNUM_Pos)
#define USB_HOST_FNUM_MFNUM(value)  (USB_HOST_FNUM_MFNUM_Msk & ((value) << USB_HOST_FNUM_MFNUM_Pos))
#define USB_HOST_FNUM_FNUM_Pos      3            /**< \brief (USB_HOST_FNUM) Frame Number */
#define USB_HOST_FNUM_FNUM_Msk      (0x7FFul << USB_HOST_FNUM_FNUM_Pos)
#define USB_HOST_FNUM_FNUM(value)   (USB_HOST_FNUM_FNUM_Msk & ((value) << USB_HOST_FNUM_FNUM_Pos))
#define USB_HOST_FNUM_MASK          0x3FFFul     /**< \brief (USB_HOST_FNUM) MASK Register */

/* -------- USB_HOST_FLENHIGH : (USB Offset: 0x012) (R/   8) HOST HOST Host Frame Length -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  FLENHIGH:8;       /*!< bit:  0.. 7  Frame Length                       */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_HOST_FLENHIGH_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_FLENHIGH_OFFSET    0x012        /**< \brief (USB_HOST_FLENHIGH offset) HOST Host Frame Length */
#define USB_HOST_FLENHIGH_RESETVALUE 0x00ul       /**< \brief (USB_HOST_FLENHIGH reset_value) HOST Host Frame Length */

#define USB_HOST_FLENHIGH_FLENHIGH_Pos 0            /**< \brief (USB_HOST_FLENHIGH) Frame Length */
#define USB_HOST_FLENHIGH_FLENHIGH_Msk (0xFFul << USB_HOST_FLENHIGH_FLENHIGH_Pos)
#define USB_HOST_FLENHIGH_FLENHIGH(value) (USB_HOST_FLENHIGH_FLENHIGH_Msk & ((value) << USB_HOST_FLENHIGH_FLENHIGH_Pos))
#define USB_HOST_FLENHIGH_MASK      0xFFul       /**< \brief (USB_HOST_FLENHIGH) MASK Register */

/* -------- USB_DEVICE_INTENCLR : (USB Offset: 0x014) (R/W 16) DEVICE DEVICE Device Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t SUSPEND:1;        /*!< bit:      0  Suspend Interrupt Enable           */
    uint16_t MSOF:1;           /*!< bit:      1  Micro Start of Frame Interrupt Enable in High Speed Mode */
    uint16_t SOF:1;            /*!< bit:      2  Start Of Frame Interrupt Enable    */
    uint16_t EORST:1;          /*!< bit:      3  End of Reset Interrupt Enable      */
    uint16_t WAKEUP:1;         /*!< bit:      4  Wake Up Interrupt Enable           */
    uint16_t EORSM:1;          /*!< bit:      5  End Of Resume Interrupt Enable     */
    uint16_t UPRSM:1;          /*!< bit:      6  Upstream Resume Interrupt Enable   */
    uint16_t RAMACER:1;        /*!< bit:      7  Ram Access Interrupt Enable        */
    uint16_t LPMNYET:1;        /*!< bit:      8  Link Power Management Not Yet Interrupt Enable */
    uint16_t LPMSUSP:1;        /*!< bit:      9  Link Power Management Suspend Interrupt Enable */
    uint16_t :6;               /*!< bit: 10..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} USB_DEVICE_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_DEVICE_INTENCLR_OFFSET  0x014        /**< \brief (USB_DEVICE_INTENCLR offset) DEVICE Device Interrupt Enable Clear */
#define USB_DEVICE_INTENCLR_RESETVALUE 0x0000ul     /**< \brief (USB_DEVICE_INTENCLR reset_value) DEVICE Device Interrupt Enable Clear */

#define USB_DEVICE_INTENCLR_SUSPEND_Pos 0            /**< \brief (USB_DEVICE_INTENCLR) Suspend Interrupt Enable */
#define USB_DEVICE_INTENCLR_SUSPEND (0x1ul << USB_DEVICE_INTENCLR_SUSPEND_Pos)
#define USB_DEVICE_INTENCLR_MSOF_Pos 1            /**< \brief (USB_DEVICE_INTENCLR) Micro Start of Frame Interrupt Enable in High Speed Mode */
#define USB_DEVICE_INTENCLR_MSOF    (0x1ul << USB_DEVICE_INTENCLR_MSOF_Pos)
#define USB_DEVICE_INTENCLR_SOF_Pos 2            /**< \brief (USB_DEVICE_INTENCLR) Start Of Frame Interrupt Enable */
#define USB_DEVICE_INTENCLR_SOF     (0x1ul << USB_DEVICE_INTENCLR_SOF_Pos)
#define USB_DEVICE_INTENCLR_EORST_Pos 3            /**< \brief (USB_DEVICE_INTENCLR) End of Reset Interrupt Enable */
#define USB_DEVICE_INTENCLR_EORST   (0x1ul << USB_DEVICE_INTENCLR_EORST_Pos)
#define USB_DEVICE_INTENCLR_WAKEUP_Pos 4            /**< \brief (USB_DEVICE_INTENCLR) Wake Up Interrupt Enable */
#define USB_DEVICE_INTENCLR_WAKEUP  (0x1ul << USB_DEVICE_INTENCLR_WAKEUP_Pos)
#define USB_DEVICE_INTENCLR_EORSM_Pos 5            /**< \brief (USB_DEVICE_INTENCLR) End Of Resume Interrupt Enable */
#define USB_DEVICE_INTENCLR_EORSM   (0x1ul << USB_DEVICE_INTENCLR_EORSM_Pos)
#define USB_DEVICE_INTENCLR_UPRSM_Pos 6            /**< \brief (USB_DEVICE_INTENCLR) Upstream Resume Interrupt Enable */
#define USB_DEVICE_INTENCLR_UPRSM   (0x1ul << USB_DEVICE_INTENCLR_UPRSM_Pos)
#define USB_DEVICE_INTENCLR_RAMACER_Pos 7            /**< \brief (USB_DEVICE_INTENCLR) Ram Access Interrupt Enable */
#define USB_DEVICE_INTENCLR_RAMACER (0x1ul << USB_DEVICE_INTENCLR_RAMACER_Pos)
#define USB_DEVICE_INTENCLR_LPMNYET_Pos 8            /**< \brief (USB_DEVICE_INTENCLR) Link Power Management Not Yet Interrupt Enable */
#define USB_DEVICE_INTENCLR_LPMNYET (0x1ul << USB_DEVICE_INTENCLR_LPMNYET_Pos)
#define USB_DEVICE_INTENCLR_LPMSUSP_Pos 9            /**< \brief (USB_DEVICE_INTENCLR) Link Power Management Suspend Interrupt Enable */
#define USB_DEVICE_INTENCLR_LPMSUSP (0x1ul << USB_DEVICE_INTENCLR_LPMSUSP_Pos)
#define USB_DEVICE_INTENCLR_MASK    0x03FFul     /**< \brief (USB_DEVICE_INTENCLR) MASK Register */

/* -------- USB_HOST_INTENCLR : (USB Offset: 0x014) (R/W 16) HOST HOST Host Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t :2;               /*!< bit:  0.. 1  Reserved                           */
    uint16_t HSOF:1;           /*!< bit:      2  Host Start Of Frame Interrupt Disable */
    uint16_t RST:1;            /*!< bit:      3  BUS Reset Interrupt Disable        */
    uint16_t WAKEUP:1;         /*!< bit:      4  Wake Up Interrupt Disable          */
    uint16_t DNRSM:1;          /*!< bit:      5  DownStream to Device Interrupt Disable */
    uint16_t UPRSM:1;          /*!< bit:      6  Upstream Resume from Device Interrupt Disable */
    uint16_t RAMACER:1;        /*!< bit:      7  Ram Access Interrupt Disable       */
    uint16_t DCONN:1;          /*!< bit:      8  Device Connection Interrupt Disable */
    uint16_t DDISC:1;          /*!< bit:      9  Device Disconnection Interrupt Disable */
    uint16_t :6;               /*!< bit: 10..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} USB_HOST_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_INTENCLR_OFFSET    0x014        /**< \brief (USB_HOST_INTENCLR offset) HOST Host Interrupt Enable Clear */
#define USB_HOST_INTENCLR_RESETVALUE 0x0000ul     /**< \brief (USB_HOST_INTENCLR reset_value) HOST Host Interrupt Enable Clear */

#define USB_HOST_INTENCLR_HSOF_Pos  2            /**< \brief (USB_HOST_INTENCLR) Host Start Of Frame Interrupt Disable */
#define USB_HOST_INTENCLR_HSOF      (0x1ul << USB_HOST_INTENCLR_HSOF_Pos)
#define USB_HOST_INTENCLR_RST_Pos   3            /**< \brief (USB_HOST_INTENCLR) BUS Reset Interrupt Disable */
#define USB_HOST_INTENCLR_RST       (0x1ul << USB_HOST_INTENCLR_RST_Pos)
#define USB_HOST_INTENCLR_WAKEUP_Pos 4            /**< \brief (USB_HOST_INTENCLR) Wake Up Interrupt Disable */
#define USB_HOST_INTENCLR_WAKEUP    (0x1ul << USB_HOST_INTENCLR_WAKEUP_Pos)
#define USB_HOST_INTENCLR_DNRSM_Pos 5            /**< \brief (USB_HOST_INTENCLR) DownStream to Device Interrupt Disable */
#define USB_HOST_INTENCLR_DNRSM     (0x1ul << USB_HOST_INTENCLR_DNRSM_Pos)
#define USB_HOST_INTENCLR_UPRSM_Pos 6            /**< \brief (USB_HOST_INTENCLR) Upstream Resume from Device Interrupt Disable */
#define USB_HOST_INTENCLR_UPRSM     (0x1ul << USB_HOST_INTENCLR_UPRSM_Pos)
#define USB_HOST_INTENCLR_RAMACER_Pos 7            /**< \brief (USB_HOST_INTENCLR) Ram Access Interrupt Disable */
#define USB_HOST_INTENCLR_RAMACER   (0x1ul << USB_HOST_INTENCLR_RAMACER_Pos)
#define USB_HOST_INTENCLR_DCONN_Pos 8            /**< \brief (USB_HOST_INTENCLR) Device Connection Interrupt Disable */
#define USB_HOST_INTENCLR_DCONN     (0x1ul << USB_HOST_INTENCLR_DCONN_Pos)
#define USB_HOST_INTENCLR_DDISC_Pos 9            /**< \brief (USB_HOST_INTENCLR) Device Disconnection Interrupt Disable */
#define USB_HOST_INTENCLR_DDISC     (0x1ul << USB_HOST_INTENCLR_DDISC_Pos)
#define USB_HOST_INTENCLR_MASK      0x03FCul     /**< \brief (USB_HOST_INTENCLR) MASK Register */

/* -------- USB_DEVICE_INTENSET : (USB Offset: 0x018) (R/W 16) DEVICE DEVICE Device Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t SUSPEND:1;        /*!< bit:      0  Suspend Interrupt Enable           */
    uint16_t MSOF:1;           /*!< bit:      1  Micro Start of Frame Interrupt Enable in High Speed Mode */
    uint16_t SOF:1;            /*!< bit:      2  Start Of Frame Interrupt Enable    */
    uint16_t EORST:1;          /*!< bit:      3  End of Reset Interrupt Enable      */
    uint16_t WAKEUP:1;         /*!< bit:      4  Wake Up Interrupt Enable           */
    uint16_t EORSM:1;          /*!< bit:      5  End Of Resume Interrupt Enable     */
    uint16_t UPRSM:1;          /*!< bit:      6  Upstream Resume Interrupt Enable   */
    uint16_t RAMACER:1;        /*!< bit:      7  Ram Access Interrupt Enable        */
    uint16_t LPMNYET:1;        /*!< bit:      8  Link Power Management Not Yet Interrupt Enable */
    uint16_t LPMSUSP:1;        /*!< bit:      9  Link Power Management Suspend Interrupt Enable */
    uint16_t :6;               /*!< bit: 10..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} USB_DEVICE_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_DEVICE_INTENSET_OFFSET  0x018        /**< \brief (USB_DEVICE_INTENSET offset) DEVICE Device Interrupt Enable Set */
#define USB_DEVICE_INTENSET_RESETVALUE 0x0000ul     /**< \brief (USB_DEVICE_INTENSET reset_value) DEVICE Device Interrupt Enable Set */

#define USB_DEVICE_INTENSET_SUSPEND_Pos 0            /**< \brief (USB_DEVICE_INTENSET) Suspend Interrupt Enable */
#define USB_DEVICE_INTENSET_SUSPEND (0x1ul << USB_DEVICE_INTENSET_SUSPEND_Pos)
#define USB_DEVICE_INTENSET_MSOF_Pos 1            /**< \brief (USB_DEVICE_INTENSET) Micro Start of Frame Interrupt Enable in High Speed Mode */
#define USB_DEVICE_INTENSET_MSOF    (0x1ul << USB_DEVICE_INTENSET_MSOF_Pos)
#define USB_DEVICE_INTENSET_SOF_Pos 2            /**< \brief (USB_DEVICE_INTENSET) Start Of Frame Interrupt Enable */
#define USB_DEVICE_INTENSET_SOF     (0x1ul << USB_DEVICE_INTENSET_SOF_Pos)
#define USB_DEVICE_INTENSET_EORST_Pos 3            /**< \brief (USB_DEVICE_INTENSET) End of Reset Interrupt Enable */
#define USB_DEVICE_INTENSET_EORST   (0x1ul << USB_DEVICE_INTENSET_EORST_Pos)
#define USB_DEVICE_INTENSET_WAKEUP_Pos 4            /**< \brief (USB_DEVICE_INTENSET) Wake Up Interrupt Enable */
#define USB_DEVICE_INTENSET_WAKEUP  (0x1ul << USB_DEVICE_INTENSET_WAKEUP_Pos)
#define USB_DEVICE_INTENSET_EORSM_Pos 5            /**< \brief (USB_DEVICE_INTENSET) End Of Resume Interrupt Enable */
#define USB_DEVICE_INTENSET_EORSM   (0x1ul << USB_DEVICE_INTENSET_EORSM_Pos)
#define USB_DEVICE_INTENSET_UPRSM_Pos 6            /**< \brief (USB_DEVICE_INTENSET) Upstream Resume Interrupt Enable */
#define USB_DEVICE_INTENSET_UPRSM   (0x1ul << USB_DEVICE_INTENSET_UPRSM_Pos)
#define USB_DEVICE_INTENSET_RAMACER_Pos 7            /**< \brief (USB_DEVICE_INTENSET) Ram Access Interrupt Enable */
#define USB_DEVICE_INTENSET_RAMACER (0x1ul << USB_DEVICE_INTENSET_RAMACER_Pos)
#define USB_DEVICE_INTENSET_LPMNYET_Pos 8            /**< \brief (USB_DEVICE_INTENSET) Link Power Management Not Yet Interrupt Enable */
#define USB_DEVICE_INTENSET_LPMNYET (0x1ul << USB_DEVICE_INTENSET_LPMNYET_Pos)
#define USB_DEVICE_INTENSET_LPMSUSP_Pos 9            /**< \brief (USB_DEVICE_INTENSET) Link Power Management Suspend Interrupt Enable */
#define USB_DEVICE_INTENSET_LPMSUSP (0x1ul << USB_DEVICE_INTENSET_LPMSUSP_Pos)
#define USB_DEVICE_INTENSET_MASK    0x03FFul     /**< \brief (USB_DEVICE_INTENSET) MASK Register */

/* -------- USB_HOST_INTENSET : (USB Offset: 0x018) (R/W 16) HOST HOST Host Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t :2;               /*!< bit:  0.. 1  Reserved                           */
    uint16_t HSOF:1;           /*!< bit:      2  Host Start Of Frame Interrupt Enable */
    uint16_t RST:1;            /*!< bit:      3  Bus Reset Interrupt Enable         */
    uint16_t WAKEUP:1;         /*!< bit:      4  Wake Up Interrupt Enable           */
    uint16_t DNRSM:1;          /*!< bit:      5  DownStream to the Device Interrupt Enable */
    uint16_t UPRSM:1;          /*!< bit:      6  Upstream Resume fromthe device Interrupt Enable */
    uint16_t RAMACER:1;        /*!< bit:      7  Ram Access Interrupt Enable        */
    uint16_t DCONN:1;          /*!< bit:      8  Link Power Management Interrupt Enable */
    uint16_t DDISC:1;          /*!< bit:      9  Device Disconnection Interrupt Enable */
    uint16_t :6;               /*!< bit: 10..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} USB_HOST_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_INTENSET_OFFSET    0x018        /**< \brief (USB_HOST_INTENSET offset) HOST Host Interrupt Enable Set */
#define USB_HOST_INTENSET_RESETVALUE 0x0000ul     /**< \brief (USB_HOST_INTENSET reset_value) HOST Host Interrupt Enable Set */

#define USB_HOST_INTENSET_HSOF_Pos  2            /**< \brief (USB_HOST_INTENSET) Host Start Of Frame Interrupt Enable */
#define USB_HOST_INTENSET_HSOF      (0x1ul << USB_HOST_INTENSET_HSOF_Pos)
#define USB_HOST_INTENSET_RST_Pos   3            /**< \brief (USB_HOST_INTENSET) Bus Reset Interrupt Enable */
#define USB_HOST_INTENSET_RST       (0x1ul << USB_HOST_INTENSET_RST_Pos)
#define USB_HOST_INTENSET_WAKEUP_Pos 4            /**< \brief (USB_HOST_INTENSET) Wake Up Interrupt Enable */
#define USB_HOST_INTENSET_WAKEUP    (0x1ul << USB_HOST_INTENSET_WAKEUP_Pos)
#define USB_HOST_INTENSET_DNRSM_Pos 5            /**< \brief (USB_HOST_INTENSET) DownStream to the Device Interrupt Enable */
#define USB_HOST_INTENSET_DNRSM     (0x1ul << USB_HOST_INTENSET_DNRSM_Pos)
#define USB_HOST_INTENSET_UPRSM_Pos 6            /**< \brief (USB_HOST_INTENSET) Upstream Resume fromthe device Interrupt Enable */
#define USB_HOST_INTENSET_UPRSM     (0x1ul << USB_HOST_INTENSET_UPRSM_Pos)
#define USB_HOST_INTENSET_RAMACER_Pos 7            /**< \brief (USB_HOST_INTENSET) Ram Access Interrupt Enable */
#define USB_HOST_INTENSET_RAMACER   (0x1ul << USB_HOST_INTENSET_RAMACER_Pos)
#define USB_HOST_INTENSET_DCONN_Pos 8            /**< \brief (USB_HOST_INTENSET) Link Power Management Interrupt Enable */
#define USB_HOST_INTENSET_DCONN     (0x1ul << USB_HOST_INTENSET_DCONN_Pos)
#define USB_HOST_INTENSET_DDISC_Pos 9            /**< \brief (USB_HOST_INTENSET) Device Disconnection Interrupt Enable */
#define USB_HOST_INTENSET_DDISC     (0x1ul << USB_HOST_INTENSET_DDISC_Pos)
#define USB_HOST_INTENSET_MASK      0x03FCul     /**< \brief (USB_HOST_INTENSET) MASK Register */

/* -------- USB_DEVICE_INTFLAG : (USB Offset: 0x01C) (R/W 16) DEVICE DEVICE Device Interrupt Flag -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint16_t SUSPEND:1;        /*!< bit:      0  Suspend                            */
    __I uint16_t MSOF:1;           /*!< bit:      1  Micro Start of Frame in High Speed Mode */
    __I uint16_t SOF:1;            /*!< bit:      2  Start Of Frame                     */
    __I uint16_t EORST:1;          /*!< bit:      3  End of Reset                       */
    __I uint16_t WAKEUP:1;         /*!< bit:      4  Wake Up                            */
    __I uint16_t EORSM:1;          /*!< bit:      5  End Of Resume                      */
    __I uint16_t UPRSM:1;          /*!< bit:      6  Upstream Resume                    */
    __I uint16_t RAMACER:1;        /*!< bit:      7  Ram Access                         */
    __I uint16_t LPMNYET:1;        /*!< bit:      8  Link Power Management Not Yet      */
    __I uint16_t LPMSUSP:1;        /*!< bit:      9  Link Power Management Suspend      */
    __I uint16_t :6;               /*!< bit: 10..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} USB_DEVICE_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_DEVICE_INTFLAG_OFFSET   0x01C        /**< \brief (USB_DEVICE_INTFLAG offset) DEVICE Device Interrupt Flag */
#define USB_DEVICE_INTFLAG_RESETVALUE 0x0000ul     /**< \brief (USB_DEVICE_INTFLAG reset_value) DEVICE Device Interrupt Flag */

#define USB_DEVICE_INTFLAG_SUSPEND_Pos 0            /**< \brief (USB_DEVICE_INTFLAG) Suspend */
#define USB_DEVICE_INTFLAG_SUSPEND  (0x1ul << USB_DEVICE_INTFLAG_SUSPEND_Pos)
#define USB_DEVICE_INTFLAG_MSOF_Pos 1            /**< \brief (USB_DEVICE_INTFLAG) Micro Start of Frame in High Speed Mode */
#define USB_DEVICE_INTFLAG_MSOF     (0x1ul << USB_DEVICE_INTFLAG_MSOF_Pos)
#define USB_DEVICE_INTFLAG_SOF_Pos  2            /**< \brief (USB_DEVICE_INTFLAG) Start Of Frame */
#define USB_DEVICE_INTFLAG_SOF      (0x1ul << USB_DEVICE_INTFLAG_SOF_Pos)
#define USB_DEVICE_INTFLAG_EORST_Pos 3            /**< \brief (USB_DEVICE_INTFLAG) End of Reset */
#define USB_DEVICE_INTFLAG_EORST    (0x1ul << USB_DEVICE_INTFLAG_EORST_Pos)
#define USB_DEVICE_INTFLAG_WAKEUP_Pos 4            /**< \brief (USB_DEVICE_INTFLAG) Wake Up */
#define USB_DEVICE_INTFLAG_WAKEUP   (0x1ul << USB_DEVICE_INTFLAG_WAKEUP_Pos)
#define USB_DEVICE_INTFLAG_EORSM_Pos 5            /**< \brief (USB_DEVICE_INTFLAG) End Of Resume */
#define USB_DEVICE_INTFLAG_EORSM    (0x1ul << USB_DEVICE_INTFLAG_EORSM_Pos)
#define USB_DEVICE_INTFLAG_UPRSM_Pos 6            /**< \brief (USB_DEVICE_INTFLAG) Upstream Resume */
#define USB_DEVICE_INTFLAG_UPRSM    (0x1ul << USB_DEVICE_INTFLAG_UPRSM_Pos)
#define USB_DEVICE_INTFLAG_RAMACER_Pos 7            /**< \brief (USB_DEVICE_INTFLAG) Ram Access */
#define USB_DEVICE_INTFLAG_RAMACER  (0x1ul << USB_DEVICE_INTFLAG_RAMACER_Pos)
#define USB_DEVICE_INTFLAG_LPMNYET_Pos 8            /**< \brief (USB_DEVICE_INTFLAG) Link Power Management Not Yet */
#define USB_DEVICE_INTFLAG_LPMNYET  (0x1ul << USB_DEVICE_INTFLAG_LPMNYET_Pos)
#define USB_DEVICE_INTFLAG_LPMSUSP_Pos 9            /**< \brief (USB_DEVICE_INTFLAG) Link Power Management Suspend */
#define USB_DEVICE_INTFLAG_LPMSUSP  (0x1ul << USB_DEVICE_INTFLAG_LPMSUSP_Pos)
#define USB_DEVICE_INTFLAG_MASK     0x03FFul     /**< \brief (USB_DEVICE_INTFLAG) MASK Register */

/* -------- USB_HOST_INTFLAG : (USB Offset: 0x01C) (R/W 16) HOST HOST Host Interrupt Flag -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint16_t :2;               /*!< bit:  0.. 1  Reserved                           */
    __I uint16_t HSOF:1;           /*!< bit:      2  Host Start Of Frame                */
    __I uint16_t RST:1;            /*!< bit:      3  Bus Reset                          */
    __I uint16_t WAKEUP:1;         /*!< bit:      4  Wake Up                            */
    __I uint16_t DNRSM:1;          /*!< bit:      5  Downstream                         */
    __I uint16_t UPRSM:1;          /*!< bit:      6  Upstream Resume from the Device    */
    __I uint16_t RAMACER:1;        /*!< bit:      7  Ram Access                         */
    __I uint16_t DCONN:1;          /*!< bit:      8  Device Connection                  */
    __I uint16_t DDISC:1;          /*!< bit:      9  Device Disconnection               */
    __I uint16_t :6;               /*!< bit: 10..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} USB_HOST_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_INTFLAG_OFFSET     0x01C        /**< \brief (USB_HOST_INTFLAG offset) HOST Host Interrupt Flag */
#define USB_HOST_INTFLAG_RESETVALUE 0x0000ul     /**< \brief (USB_HOST_INTFLAG reset_value) HOST Host Interrupt Flag */

#define USB_HOST_INTFLAG_HSOF_Pos   2            /**< \brief (USB_HOST_INTFLAG) Host Start Of Frame */
#define USB_HOST_INTFLAG_HSOF       (0x1ul << USB_HOST_INTFLAG_HSOF_Pos)
#define USB_HOST_INTFLAG_RST_Pos    3            /**< \brief (USB_HOST_INTFLAG) Bus Reset */
#define USB_HOST_INTFLAG_RST        (0x1ul << USB_HOST_INTFLAG_RST_Pos)
#define USB_HOST_INTFLAG_WAKEUP_Pos 4            /**< \brief (USB_HOST_INTFLAG) Wake Up */
#define USB_HOST_INTFLAG_WAKEUP     (0x1ul << USB_HOST_INTFLAG_WAKEUP_Pos)
#define USB_HOST_INTFLAG_DNRSM_Pos  5            /**< \brief (USB_HOST_INTFLAG) Downstream */
#define USB_HOST_INTFLAG_DNRSM      (0x1ul << USB_HOST_INTFLAG_DNRSM_Pos)
#define USB_HOST_INTFLAG_UPRSM_Pos  6            /**< \brief (USB_HOST_INTFLAG) Upstream Resume from the Device */
#define USB_HOST_INTFLAG_UPRSM      (0x1ul << USB_HOST_INTFLAG_UPRSM_Pos)
#define USB_HOST_INTFLAG_RAMACER_Pos 7            /**< \brief (USB_HOST_INTFLAG) Ram Access */
#define USB_HOST_INTFLAG_RAMACER    (0x1ul << USB_HOST_INTFLAG_RAMACER_Pos)
#define USB_HOST_INTFLAG_DCONN_Pos  8            /**< \brief (USB_HOST_INTFLAG) Device Connection */
#define USB_HOST_INTFLAG_DCONN      (0x1ul << USB_HOST_INTFLAG_DCONN_Pos)
#define USB_HOST_INTFLAG_DDISC_Pos  9            /**< \brief (USB_HOST_INTFLAG) Device Disconnection */
#define USB_HOST_INTFLAG_DDISC      (0x1ul << USB_HOST_INTFLAG_DDISC_Pos)
#define USB_HOST_INTFLAG_MASK       0x03FCul     /**< \brief (USB_HOST_INTFLAG) MASK Register */

/* -------- USB_DEVICE_EPINTSMRY : (USB Offset: 0x020) (R/  16) DEVICE DEVICE End Point Interrupt Summary -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t EPINT0:1;         /*!< bit:      0  End Point 0 Interrupt              */
    uint16_t EPINT1:1;         /*!< bit:      1  End Point 1 Interrupt              */
    uint16_t EPINT2:1;         /*!< bit:      2  End Point 2 Interrupt              */
    uint16_t EPINT3:1;         /*!< bit:      3  End Point 3 Interrupt              */
    uint16_t EPINT4:1;         /*!< bit:      4  End Point 4 Interrupt              */
    uint16_t EPINT5:1;         /*!< bit:      5  End Point 5 Interrupt              */
    uint16_t EPINT6:1;         /*!< bit:      6  End Point 6 Interrupt              */
    uint16_t EPINT7:1;         /*!< bit:      7  End Point 7 Interrupt              */
    uint16_t :8;               /*!< bit:  8..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t EPINT:8;          /*!< bit:  0.. 7  End Point x Interrupt              */
    uint16_t :8;               /*!< bit:  8..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} USB_DEVICE_EPINTSMRY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_DEVICE_EPINTSMRY_OFFSET 0x020        /**< \brief (USB_DEVICE_EPINTSMRY offset) DEVICE End Point Interrupt Summary */
#define USB_DEVICE_EPINTSMRY_RESETVALUE 0x0000ul     /**< \brief (USB_DEVICE_EPINTSMRY reset_value) DEVICE End Point Interrupt Summary */

#define USB_DEVICE_EPINTSMRY_EPINT0_Pos 0            /**< \brief (USB_DEVICE_EPINTSMRY) End Point 0 Interrupt */
#define USB_DEVICE_EPINTSMRY_EPINT0 (1 << USB_DEVICE_EPINTSMRY_EPINT0_Pos)
#define USB_DEVICE_EPINTSMRY_EPINT1_Pos 1            /**< \brief (USB_DEVICE_EPINTSMRY) End Point 1 Interrupt */
#define USB_DEVICE_EPINTSMRY_EPINT1 (1 << USB_DEVICE_EPINTSMRY_EPINT1_Pos)
#define USB_DEVICE_EPINTSMRY_EPINT2_Pos 2            /**< \brief (USB_DEVICE_EPINTSMRY) End Point 2 Interrupt */
#define USB_DEVICE_EPINTSMRY_EPINT2 (1 << USB_DEVICE_EPINTSMRY_EPINT2_Pos)
#define USB_DEVICE_EPINTSMRY_EPINT3_Pos 3            /**< \brief (USB_DEVICE_EPINTSMRY) End Point 3 Interrupt */
#define USB_DEVICE_EPINTSMRY_EPINT3 (1 << USB_DEVICE_EPINTSMRY_EPINT3_Pos)
#define USB_DEVICE_EPINTSMRY_EPINT4_Pos 4            /**< \brief (USB_DEVICE_EPINTSMRY) End Point 4 Interrupt */
#define USB_DEVICE_EPINTSMRY_EPINT4 (1 << USB_DEVICE_EPINTSMRY_EPINT4_Pos)
#define USB_DEVICE_EPINTSMRY_EPINT5_Pos 5            /**< \brief (USB_DEVICE_EPINTSMRY) End Point 5 Interrupt */
#define USB_DEVICE_EPINTSMRY_EPINT5 (1 << USB_DEVICE_EPINTSMRY_EPINT5_Pos)
#define USB_DEVICE_EPINTSMRY_EPINT6_Pos 6            /**< \brief (USB_DEVICE_EPINTSMRY) End Point 6 Interrupt */
#define USB_DEVICE_EPINTSMRY_EPINT6 (1 << USB_DEVICE_EPINTSMRY_EPINT6_Pos)
#define USB_DEVICE_EPINTSMRY_EPINT7_Pos 7            /**< \brief (USB_DEVICE_EPINTSMRY) End Point 7 Interrupt */
#define USB_DEVICE_EPINTSMRY_EPINT7 (1 << USB_DEVICE_EPINTSMRY_EPINT7_Pos)
#define USB_DEVICE_EPINTSMRY_EPINT_Pos 0            /**< \brief (USB_DEVICE_EPINTSMRY) End Point x Interrupt */
#define USB_DEVICE_EPINTSMRY_EPINT_Msk (0xFFul << USB_DEVICE_EPINTSMRY_EPINT_Pos)
#define USB_DEVICE_EPINTSMRY_EPINT(value) (USB_DEVICE_EPINTSMRY_EPINT_Msk & ((value) << USB_DEVICE_EPINTSMRY_EPINT_Pos))
#define USB_DEVICE_EPINTSMRY_MASK   0x00FFul     /**< \brief (USB_DEVICE_EPINTSMRY) MASK Register */

/* -------- USB_HOST_PINTSMRY : (USB Offset: 0x020) (R/  16) HOST HOST Pipe Interrupt Summary -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t EPINT0:1;         /*!< bit:      0  Pipe 0 Interrupt                   */
    uint16_t EPINT1:1;         /*!< bit:      1  Pipe 1 Interrupt                   */
    uint16_t EPINT2:1;         /*!< bit:      2  Pipe 2 Interrupt                   */
    uint16_t EPINT3:1;         /*!< bit:      3  Pipe 3 Interrupt                   */
    uint16_t EPINT4:1;         /*!< bit:      4  Pipe 4 Interrupt                   */
    uint16_t EPINT5:1;         /*!< bit:      5  Pipe 5 Interrupt                   */
    uint16_t EPINT6:1;         /*!< bit:      6  Pipe 6 Interrupt                   */
    uint16_t EPINT7:1;         /*!< bit:      7  Pipe 7 Interrupt                   */
    uint16_t :8;               /*!< bit:  8..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t EPINT:8;          /*!< bit:  0.. 7  Pipe x Interrupt                   */
    uint16_t :8;               /*!< bit:  8..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} USB_HOST_PINTSMRY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_PINTSMRY_OFFSET    0x020        /**< \brief (USB_HOST_PINTSMRY offset) HOST Pipe Interrupt Summary */
#define USB_HOST_PINTSMRY_RESETVALUE 0x0000ul     /**< \brief (USB_HOST_PINTSMRY reset_value) HOST Pipe Interrupt Summary */

#define USB_HOST_PINTSMRY_EPINT0_Pos 0            /**< \brief (USB_HOST_PINTSMRY) Pipe 0 Interrupt */
#define USB_HOST_PINTSMRY_EPINT0    (1 << USB_HOST_PINTSMRY_EPINT0_Pos)
#define USB_HOST_PINTSMRY_EPINT1_Pos 1            /**< \brief (USB_HOST_PINTSMRY) Pipe 1 Interrupt */
#define USB_HOST_PINTSMRY_EPINT1    (1 << USB_HOST_PINTSMRY_EPINT1_Pos)
#define USB_HOST_PINTSMRY_EPINT2_Pos 2            /**< \brief (USB_HOST_PINTSMRY) Pipe 2 Interrupt */
#define USB_HOST_PINTSMRY_EPINT2    (1 << USB_HOST_PINTSMRY_EPINT2_Pos)
#define USB_HOST_PINTSMRY_EPINT3_Pos 3            /**< \brief (USB_HOST_PINTSMRY) Pipe 3 Interrupt */
#define USB_HOST_PINTSMRY_EPINT3    (1 << USB_HOST_PINTSMRY_EPINT3_Pos)
#define USB_HOST_PINTSMRY_EPINT4_Pos 4            /**< \brief (USB_HOST_PINTSMRY) Pipe 4 Interrupt */
#define USB_HOST_PINTSMRY_EPINT4    (1 << USB_HOST_PINTSMRY_EPINT4_Pos)
#define USB_HOST_PINTSMRY_EPINT5_Pos 5            /**< \brief (USB_HOST_PINTSMRY) Pipe 5 Interrupt */
#define USB_HOST_PINTSMRY_EPINT5    (1 << USB_HOST_PINTSMRY_EPINT5_Pos)
#define USB_HOST_PINTSMRY_EPINT6_Pos 6            /**< \brief (USB_HOST_PINTSMRY) Pipe 6 Interrupt */
#define USB_HOST_PINTSMRY_EPINT6    (1 << USB_HOST_PINTSMRY_EPINT6_Pos)
#define USB_HOST_PINTSMRY_EPINT7_Pos 7            /**< \brief (USB_HOST_PINTSMRY) Pipe 7 Interrupt */
#define USB_HOST_PINTSMRY_EPINT7    (1 << USB_HOST_PINTSMRY_EPINT7_Pos)
#define USB_HOST_PINTSMRY_EPINT_Pos 0            /**< \brief (USB_HOST_PINTSMRY) Pipe x Interrupt */
#define USB_HOST_PINTSMRY_EPINT_Msk (0xFFul << USB_HOST_PINTSMRY_EPINT_Pos)
#define USB_HOST_PINTSMRY_EPINT(value) (USB_HOST_PINTSMRY_EPINT_Msk & ((value) << USB_HOST_PINTSMRY_EPINT_Pos))
#define USB_HOST_PINTSMRY_MASK      0x00FFul     /**< \brief (USB_HOST_PINTSMRY) MASK Register */

/* -------- USB_DESCADD : (USB Offset: 0x024) (R/W 32) Descriptor Address -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DESCADD:32;       /*!< bit:  0..31  Descriptor Address Value           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} USB_DESCADD_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_DESCADD_OFFSET          0x024        /**< \brief (USB_DESCADD offset) Descriptor Address */
#define USB_DESCADD_RESETVALUE      0x00000000ul /**< \brief (USB_DESCADD reset_value) Descriptor Address */

#define USB_DESCADD_DESCADD_Pos     0            /**< \brief (USB_DESCADD) Descriptor Address Value */
#define USB_DESCADD_DESCADD_Msk     (0xFFFFFFFFul << USB_DESCADD_DESCADD_Pos)
#define USB_DESCADD_DESCADD(value)  (USB_DESCADD_DESCADD_Msk & ((value) << USB_DESCADD_DESCADD_Pos))
#define USB_DESCADD_MASK            0xFFFFFFFFul /**< \brief (USB_DESCADD) MASK Register */

/* -------- USB_PADCAL : (USB Offset: 0x028) (R/W 16) USB PAD Calibration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t TRANSP:5;         /*!< bit:  0.. 4  USB Pad Transp calibration         */
    uint16_t :1;               /*!< bit:      5  Reserved                           */
    uint16_t TRANSN:5;         /*!< bit:  6..10  USB Pad Transn calibration         */
    uint16_t :1;               /*!< bit:     11  Reserved                           */
    uint16_t TRIM:3;           /*!< bit: 12..14  USB Pad Trim calibration           */
    uint16_t :1;               /*!< bit:     15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} USB_PADCAL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_PADCAL_OFFSET           0x028        /**< \brief (USB_PADCAL offset) USB PAD Calibration */
#define USB_PADCAL_RESETVALUE       0x0000ul     /**< \brief (USB_PADCAL reset_value) USB PAD Calibration */

#define USB_PADCAL_TRANSP_Pos       0            /**< \brief (USB_PADCAL) USB Pad Transp calibration */
#define USB_PADCAL_TRANSP_Msk       (0x1Ful << USB_PADCAL_TRANSP_Pos)
#define USB_PADCAL_TRANSP(value)    (USB_PADCAL_TRANSP_Msk & ((value) << USB_PADCAL_TRANSP_Pos))
#define USB_PADCAL_TRANSN_Pos       6            /**< \brief (USB_PADCAL) USB Pad Transn calibration */
#define USB_PADCAL_TRANSN_Msk       (0x1Ful << USB_PADCAL_TRANSN_Pos)
#define USB_PADCAL_TRANSN(value)    (USB_PADCAL_TRANSN_Msk & ((value) << USB_PADCAL_TRANSN_Pos))
#define USB_PADCAL_TRIM_Pos         12           /**< \brief (USB_PADCAL) USB Pad Trim calibration */
#define USB_PADCAL_TRIM_Msk         (0x7ul << USB_PADCAL_TRIM_Pos)
#define USB_PADCAL_TRIM(value)      (USB_PADCAL_TRIM_Msk & ((value) << USB_PADCAL_TRIM_Pos))
#define USB_PADCAL_MASK             0x77DFul     /**< \brief (USB_PADCAL) MASK Register */

/* -------- USB_DEVICE_EPCFG : (USB Offset: 0x100) (R/W  8) DEVICE DEVICE_ENDPOINT End Point Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  EPTYPE0:3;        /*!< bit:  0.. 2  End Point Type0                    */
    uint8_t  :1;               /*!< bit:      3  Reserved                           */
    uint8_t  EPTYPE1:3;        /*!< bit:  4.. 6  End Point Type1                    */
    uint8_t  NYETDIS:1;        /*!< bit:      7  NYET Token Disable                 */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_DEVICE_EPCFG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_DEVICE_EPCFG_OFFSET     0x100        /**< \brief (USB_DEVICE_EPCFG offset) DEVICE_ENDPOINT End Point Configuration */
#define USB_DEVICE_EPCFG_RESETVALUE 0x00ul       /**< \brief (USB_DEVICE_EPCFG reset_value) DEVICE_ENDPOINT End Point Configuration */

#define USB_DEVICE_EPCFG_EPTYPE0_Pos 0            /**< \brief (USB_DEVICE_EPCFG) End Point Type0 */
#define USB_DEVICE_EPCFG_EPTYPE0_Msk (0x7ul << USB_DEVICE_EPCFG_EPTYPE0_Pos)
#define USB_DEVICE_EPCFG_EPTYPE0(value) (USB_DEVICE_EPCFG_EPTYPE0_Msk & ((value) << USB_DEVICE_EPCFG_EPTYPE0_Pos))
#define USB_DEVICE_EPCFG_EPTYPE1_Pos 4            /**< \brief (USB_DEVICE_EPCFG) End Point Type1 */
#define USB_DEVICE_EPCFG_EPTYPE1_Msk (0x7ul << USB_DEVICE_EPCFG_EPTYPE1_Pos)
#define USB_DEVICE_EPCFG_EPTYPE1(value) (USB_DEVICE_EPCFG_EPTYPE1_Msk & ((value) << USB_DEVICE_EPCFG_EPTYPE1_Pos))
#define USB_DEVICE_EPCFG_NYETDIS_Pos 7            /**< \brief (USB_DEVICE_EPCFG) NYET Token Disable */
#define USB_DEVICE_EPCFG_NYETDIS    (0x1ul << USB_DEVICE_EPCFG_NYETDIS_Pos)
#define USB_DEVICE_EPCFG_MASK       0xF7ul       /**< \brief (USB_DEVICE_EPCFG) MASK Register */

/* -------- USB_HOST_PCFG : (USB Offset: 0x100) (R/W  8) HOST HOST_PIPE End Point Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  PTOKEN:2;         /*!< bit:  0.. 1  Pipe Token                         */
    uint8_t  BK:1;             /*!< bit:      2  Pipe Bank                          */
    uint8_t  PTYPE:3;          /*!< bit:  3.. 5  Pipe Type                          */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_HOST_PCFG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_PCFG_OFFSET        0x100        /**< \brief (USB_HOST_PCFG offset) HOST_PIPE End Point Configuration */
#define USB_HOST_PCFG_RESETVALUE    0x00ul       /**< \brief (USB_HOST_PCFG reset_value) HOST_PIPE End Point Configuration */

#define USB_HOST_PCFG_PTOKEN_Pos    0            /**< \brief (USB_HOST_PCFG) Pipe Token */
#define USB_HOST_PCFG_PTOKEN_Msk    (0x3ul << USB_HOST_PCFG_PTOKEN_Pos)
#define USB_HOST_PCFG_PTOKEN(value) (USB_HOST_PCFG_PTOKEN_Msk & ((value) << USB_HOST_PCFG_PTOKEN_Pos))
#define USB_HOST_PCFG_BK_Pos        2            /**< \brief (USB_HOST_PCFG) Pipe Bank */
#define USB_HOST_PCFG_BK            (0x1ul << USB_HOST_PCFG_BK_Pos)
#define USB_HOST_PCFG_PTYPE_Pos     3            /**< \brief (USB_HOST_PCFG) Pipe Type */
#define USB_HOST_PCFG_PTYPE_Msk     (0x7ul << USB_HOST_PCFG_PTYPE_Pos)
#define USB_HOST_PCFG_PTYPE(value)  (USB_HOST_PCFG_PTYPE_Msk & ((value) << USB_HOST_PCFG_PTYPE_Pos))
#define USB_HOST_PCFG_MASK          0x3Ful       /**< \brief (USB_HOST_PCFG) MASK Register */

/* -------- USB_HOST_BINTERVAL : (USB Offset: 0x103) (R/W  8) HOST HOST_PIPE Bus Access Period of Pipe -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  BITINTERVAL:8;    /*!< bit:  0.. 7  Bit Interval                       */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_HOST_BINTERVAL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_BINTERVAL_OFFSET   0x103        /**< \brief (USB_HOST_BINTERVAL offset) HOST_PIPE Bus Access Period of Pipe */
#define USB_HOST_BINTERVAL_RESETVALUE 0x00ul       /**< \brief (USB_HOST_BINTERVAL reset_value) HOST_PIPE Bus Access Period of Pipe */

#define USB_HOST_BINTERVAL_BITINTERVAL_Pos 0            /**< \brief (USB_HOST_BINTERVAL) Bit Interval */
#define USB_HOST_BINTERVAL_BITINTERVAL_Msk (0xFFul << USB_HOST_BINTERVAL_BITINTERVAL_Pos)
#define USB_HOST_BINTERVAL_BITINTERVAL(value) (USB_HOST_BINTERVAL_BITINTERVAL_Msk & ((value) << USB_HOST_BINTERVAL_BITINTERVAL_Pos))
#define USB_HOST_BINTERVAL_MASK     0xFFul       /**< \brief (USB_HOST_BINTERVAL) MASK Register */

/* -------- USB_DEVICE_EPSTATUSCLR : (USB Offset: 0x104) ( /W  8) DEVICE DEVICE_ENDPOINT End Point Pipe Status Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DTGLOUT:1;        /*!< bit:      0  Data Toggle OUT Clear              */
    uint8_t  DTGLIN:1;         /*!< bit:      1  Data Toggle IN Clear               */
    uint8_t  CURBK:1;          /*!< bit:      2  Curren Bank Clear                  */
    uint8_t  :1;               /*!< bit:      3  Reserved                           */
    uint8_t  STALLRQ0:1;       /*!< bit:      4  Stall 0 Request Clear              */
    uint8_t  STALLRQ1:1;       /*!< bit:      5  Stall 1 Request Clear              */
    uint8_t  BK0RDY:1;         /*!< bit:      6  Bank 0 Ready Clear                 */
    uint8_t  BK1RDY:1;         /*!< bit:      7  Bank 1 Ready Clear                 */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  :4;               /*!< bit:  0.. 3  Reserved                           */
    uint8_t  STALLRQ:2;        /*!< bit:  4.. 5  Stall x Request Clear              */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_DEVICE_EPSTATUSCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_DEVICE_EPSTATUSCLR_OFFSET 0x104        /**< \brief (USB_DEVICE_EPSTATUSCLR offset) DEVICE_ENDPOINT End Point Pipe Status Clear */
#define USB_DEVICE_EPSTATUSCLR_RESETVALUE 0x00ul       /**< \brief (USB_DEVICE_EPSTATUSCLR reset_value) DEVICE_ENDPOINT End Point Pipe Status Clear */

#define USB_DEVICE_EPSTATUSCLR_DTGLOUT_Pos 0            /**< \brief (USB_DEVICE_EPSTATUSCLR) Data Toggle OUT Clear */
#define USB_DEVICE_EPSTATUSCLR_DTGLOUT (0x1ul << USB_DEVICE_EPSTATUSCLR_DTGLOUT_Pos)
#define USB_DEVICE_EPSTATUSCLR_DTGLIN_Pos 1            /**< \brief (USB_DEVICE_EPSTATUSCLR) Data Toggle IN Clear */
#define USB_DEVICE_EPSTATUSCLR_DTGLIN (0x1ul << USB_DEVICE_EPSTATUSCLR_DTGLIN_Pos)
#define USB_DEVICE_EPSTATUSCLR_CURBK_Pos 2            /**< \brief (USB_DEVICE_EPSTATUSCLR) Curren Bank Clear */
#define USB_DEVICE_EPSTATUSCLR_CURBK (0x1ul << USB_DEVICE_EPSTATUSCLR_CURBK_Pos)
#define USB_DEVICE_EPSTATUSCLR_STALLRQ0_Pos 4            /**< \brief (USB_DEVICE_EPSTATUSCLR) Stall 0 Request Clear */
#define USB_DEVICE_EPSTATUSCLR_STALLRQ0 (1 << USB_DEVICE_EPSTATUSCLR_STALLRQ0_Pos)
#define USB_DEVICE_EPSTATUSCLR_STALLRQ1_Pos 5            /**< \brief (USB_DEVICE_EPSTATUSCLR) Stall 1 Request Clear */
#define USB_DEVICE_EPSTATUSCLR_STALLRQ1 (1 << USB_DEVICE_EPSTATUSCLR_STALLRQ1_Pos)
#define USB_DEVICE_EPSTATUSCLR_STALLRQ_Pos 4            /**< \brief (USB_DEVICE_EPSTATUSCLR) Stall x Request Clear */
#define USB_DEVICE_EPSTATUSCLR_STALLRQ_Msk (0x3ul << USB_DEVICE_EPSTATUSCLR_STALLRQ_Pos)
#define USB_DEVICE_EPSTATUSCLR_STALLRQ(value) (USB_DEVICE_EPSTATUSCLR_STALLRQ_Msk & ((value) << USB_DEVICE_EPSTATUSCLR_STALLRQ_Pos))
#define USB_DEVICE_EPSTATUSCLR_BK0RDY_Pos 6            /**< \brief (USB_DEVICE_EPSTATUSCLR) Bank 0 Ready Clear */
#define USB_DEVICE_EPSTATUSCLR_BK0RDY (0x1ul << USB_DEVICE_EPSTATUSCLR_BK0RDY_Pos)
#define USB_DEVICE_EPSTATUSCLR_BK1RDY_Pos 7            /**< \brief (USB_DEVICE_EPSTATUSCLR) Bank 1 Ready Clear */
#define USB_DEVICE_EPSTATUSCLR_BK1RDY (0x1ul << USB_DEVICE_EPSTATUSCLR_BK1RDY_Pos)
#define USB_DEVICE_EPSTATUSCLR_MASK 0xF7ul       /**< \brief (USB_DEVICE_EPSTATUSCLR) MASK Register */

/* -------- USB_HOST_PSTATUSCLR : (USB Offset: 0x104) ( /W  8) HOST HOST_PIPE End Point Pipe Status Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DTGL:1;           /*!< bit:      0  Data Toggle clear                  */
    uint8_t  :1;               /*!< bit:      1  Reserved                           */
    uint8_t  CURBK:1;          /*!< bit:      2  Curren Bank clear                  */
    uint8_t  :1;               /*!< bit:      3  Reserved                           */
    uint8_t  PFREEZE:1;        /*!< bit:      4  Pipe Freeze Clear                  */
    uint8_t  :1;               /*!< bit:      5  Reserved                           */
    uint8_t  BK0RDY:1;         /*!< bit:      6  Bank 0 Ready Clear                 */
    uint8_t  BK1RDY:1;         /*!< bit:      7  Bank 1 Ready Clear                 */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_HOST_PSTATUSCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_PSTATUSCLR_OFFSET  0x104        /**< \brief (USB_HOST_PSTATUSCLR offset) HOST_PIPE End Point Pipe Status Clear */
#define USB_HOST_PSTATUSCLR_RESETVALUE 0x00ul       /**< \brief (USB_HOST_PSTATUSCLR reset_value) HOST_PIPE End Point Pipe Status Clear */

#define USB_HOST_PSTATUSCLR_DTGL_Pos 0            /**< \brief (USB_HOST_PSTATUSCLR) Data Toggle clear */
#define USB_HOST_PSTATUSCLR_DTGL    (0x1ul << USB_HOST_PSTATUSCLR_DTGL_Pos)
#define USB_HOST_PSTATUSCLR_CURBK_Pos 2            /**< \brief (USB_HOST_PSTATUSCLR) Curren Bank clear */
#define USB_HOST_PSTATUSCLR_CURBK   (0x1ul << USB_HOST_PSTATUSCLR_CURBK_Pos)
#define USB_HOST_PSTATUSCLR_PFREEZE_Pos 4            /**< \brief (USB_HOST_PSTATUSCLR) Pipe Freeze Clear */
#define USB_HOST_PSTATUSCLR_PFREEZE (0x1ul << USB_HOST_PSTATUSCLR_PFREEZE_Pos)
#define USB_HOST_PSTATUSCLR_BK0RDY_Pos 6            /**< \brief (USB_HOST_PSTATUSCLR) Bank 0 Ready Clear */
#define USB_HOST_PSTATUSCLR_BK0RDY  (0x1ul << USB_HOST_PSTATUSCLR_BK0RDY_Pos)
#define USB_HOST_PSTATUSCLR_BK1RDY_Pos 7            /**< \brief (USB_HOST_PSTATUSCLR) Bank 1 Ready Clear */
#define USB_HOST_PSTATUSCLR_BK1RDY  (0x1ul << USB_HOST_PSTATUSCLR_BK1RDY_Pos)
#define USB_HOST_PSTATUSCLR_MASK    0xD5ul       /**< \brief (USB_HOST_PSTATUSCLR) MASK Register */

/* -------- USB_DEVICE_EPSTATUSSET : (USB Offset: 0x105) ( /W  8) DEVICE DEVICE_ENDPOINT End Point Pipe Status Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DTGLOUT:1;        /*!< bit:      0  Data Toggle OUT Set                */
    uint8_t  DTGLIN:1;         /*!< bit:      1  Data Toggle IN Set                 */
    uint8_t  CURBK:1;          /*!< bit:      2  Current Bank Set                   */
    uint8_t  :1;               /*!< bit:      3  Reserved                           */
    uint8_t  STALLRQ0:1;       /*!< bit:      4  Stall 0 Request Set                */
    uint8_t  STALLRQ1:1;       /*!< bit:      5  Stall 1 Request Set                */
    uint8_t  BK0RDY:1;         /*!< bit:      6  Bank 0 Ready Set                   */
    uint8_t  BK1RDY:1;         /*!< bit:      7  Bank 1 Ready Set                   */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  :4;               /*!< bit:  0.. 3  Reserved                           */
    uint8_t  STALLRQ:2;        /*!< bit:  4.. 5  Stall x Request Set                */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_DEVICE_EPSTATUSSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_DEVICE_EPSTATUSSET_OFFSET 0x105        /**< \brief (USB_DEVICE_EPSTATUSSET offset) DEVICE_ENDPOINT End Point Pipe Status Set */
#define USB_DEVICE_EPSTATUSSET_RESETVALUE 0x00ul       /**< \brief (USB_DEVICE_EPSTATUSSET reset_value) DEVICE_ENDPOINT End Point Pipe Status Set */

#define USB_DEVICE_EPSTATUSSET_DTGLOUT_Pos 0            /**< \brief (USB_DEVICE_EPSTATUSSET) Data Toggle OUT Set */
#define USB_DEVICE_EPSTATUSSET_DTGLOUT (0x1ul << USB_DEVICE_EPSTATUSSET_DTGLOUT_Pos)
#define USB_DEVICE_EPSTATUSSET_DTGLIN_Pos 1            /**< \brief (USB_DEVICE_EPSTATUSSET) Data Toggle IN Set */
#define USB_DEVICE_EPSTATUSSET_DTGLIN (0x1ul << USB_DEVICE_EPSTATUSSET_DTGLIN_Pos)
#define USB_DEVICE_EPSTATUSSET_CURBK_Pos 2            /**< \brief (USB_DEVICE_EPSTATUSSET) Current Bank Set */
#define USB_DEVICE_EPSTATUSSET_CURBK (0x1ul << USB_DEVICE_EPSTATUSSET_CURBK_Pos)
#define USB_DEVICE_EPSTATUSSET_STALLRQ0_Pos 4            /**< \brief (USB_DEVICE_EPSTATUSSET) Stall 0 Request Set */
#define USB_DEVICE_EPSTATUSSET_STALLRQ0 (1 << USB_DEVICE_EPSTATUSSET_STALLRQ0_Pos)
#define USB_DEVICE_EPSTATUSSET_STALLRQ1_Pos 5            /**< \brief (USB_DEVICE_EPSTATUSSET) Stall 1 Request Set */
#define USB_DEVICE_EPSTATUSSET_STALLRQ1 (1 << USB_DEVICE_EPSTATUSSET_STALLRQ1_Pos)
#define USB_DEVICE_EPSTATUSSET_STALLRQ_Pos 4            /**< \brief (USB_DEVICE_EPSTATUSSET) Stall x Request Set */
#define USB_DEVICE_EPSTATUSSET_STALLRQ_Msk (0x3ul << USB_DEVICE_EPSTATUSSET_STALLRQ_Pos)
#define USB_DEVICE_EPSTATUSSET_STALLRQ(value) (USB_DEVICE_EPSTATUSSET_STALLRQ_Msk & ((value) << USB_DEVICE_EPSTATUSSET_STALLRQ_Pos))
#define USB_DEVICE_EPSTATUSSET_BK0RDY_Pos 6            /**< \brief (USB_DEVICE_EPSTATUSSET) Bank 0 Ready Set */
#define USB_DEVICE_EPSTATUSSET_BK0RDY (0x1ul << USB_DEVICE_EPSTATUSSET_BK0RDY_Pos)
#define USB_DEVICE_EPSTATUSSET_BK1RDY_Pos 7            /**< \brief (USB_DEVICE_EPSTATUSSET) Bank 1 Ready Set */
#define USB_DEVICE_EPSTATUSSET_BK1RDY (0x1ul << USB_DEVICE_EPSTATUSSET_BK1RDY_Pos)
#define USB_DEVICE_EPSTATUSSET_MASK 0xF7ul       /**< \brief (USB_DEVICE_EPSTATUSSET) MASK Register */

/* -------- USB_HOST_PSTATUSSET : (USB Offset: 0x105) ( /W  8) HOST HOST_PIPE End Point Pipe Status Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DTGL:1;           /*!< bit:      0  Data Toggle Set                    */
    uint8_t  :1;               /*!< bit:      1  Reserved                           */
    uint8_t  CURBK:1;          /*!< bit:      2  Current Bank Set                   */
    uint8_t  :1;               /*!< bit:      3  Reserved                           */
    uint8_t  PFREEZE:1;        /*!< bit:      4  Pipe Freeze Set                    */
    uint8_t  :1;               /*!< bit:      5  Reserved                           */
    uint8_t  BK0RDY:1;         /*!< bit:      6  Bank 0 Ready Set                   */
    uint8_t  BK1RDY:1;         /*!< bit:      7  Bank 1 Ready Set                   */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_HOST_PSTATUSSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_PSTATUSSET_OFFSET  0x105        /**< \brief (USB_HOST_PSTATUSSET offset) HOST_PIPE End Point Pipe Status Set */
#define USB_HOST_PSTATUSSET_RESETVALUE 0x00ul       /**< \brief (USB_HOST_PSTATUSSET reset_value) HOST_PIPE End Point Pipe Status Set */

#define USB_HOST_PSTATUSSET_DTGL_Pos 0            /**< \brief (USB_HOST_PSTATUSSET) Data Toggle Set */
#define USB_HOST_PSTATUSSET_DTGL    (0x1ul << USB_HOST_PSTATUSSET_DTGL_Pos)
#define USB_HOST_PSTATUSSET_CURBK_Pos 2            /**< \brief (USB_HOST_PSTATUSSET) Current Bank Set */
#define USB_HOST_PSTATUSSET_CURBK   (0x1ul << USB_HOST_PSTATUSSET_CURBK_Pos)
#define USB_HOST_PSTATUSSET_PFREEZE_Pos 4            /**< \brief (USB_HOST_PSTATUSSET) Pipe Freeze Set */
#define USB_HOST_PSTATUSSET_PFREEZE (0x1ul << USB_HOST_PSTATUSSET_PFREEZE_Pos)
#define USB_HOST_PSTATUSSET_BK0RDY_Pos 6            /**< \brief (USB_HOST_PSTATUSSET) Bank 0 Ready Set */
#define USB_HOST_PSTATUSSET_BK0RDY  (0x1ul << USB_HOST_PSTATUSSET_BK0RDY_Pos)
#define USB_HOST_PSTATUSSET_BK1RDY_Pos 7            /**< \brief (USB_HOST_PSTATUSSET) Bank 1 Ready Set */
#define USB_HOST_PSTATUSSET_BK1RDY  (0x1ul << USB_HOST_PSTATUSSET_BK1RDY_Pos)
#define USB_HOST_PSTATUSSET_MASK    0xD5ul       /**< \brief (USB_HOST_PSTATUSSET) MASK Register */

/* -------- USB_DEVICE_EPSTATUS : (USB Offset: 0x106) (R/   8) DEVICE DEVICE_ENDPOINT End Point Pipe Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DTGLOUT:1;        /*!< bit:      0  Data Toggle Out                    */
    uint8_t  DTGLIN:1;         /*!< bit:      1  Data Toggle In                     */
    uint8_t  CURBK:1;          /*!< bit:      2  Current Bank                       */
    uint8_t  :1;               /*!< bit:      3  Reserved                           */
    uint8_t  STALLRQ0:1;       /*!< bit:      4  Stall 0 Request                    */
    uint8_t  STALLRQ1:1;       /*!< bit:      5  Stall 1 Request                    */
    uint8_t  BK0RDY:1;         /*!< bit:      6  Bank 0 ready                       */
    uint8_t  BK1RDY:1;         /*!< bit:      7  Bank 1 ready                       */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  :4;               /*!< bit:  0.. 3  Reserved                           */
    uint8_t  STALLRQ:2;        /*!< bit:  4.. 5  Stall x Request                    */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_DEVICE_EPSTATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_DEVICE_EPSTATUS_OFFSET  0x106        /**< \brief (USB_DEVICE_EPSTATUS offset) DEVICE_ENDPOINT End Point Pipe Status */
#define USB_DEVICE_EPSTATUS_RESETVALUE 0x00ul       /**< \brief (USB_DEVICE_EPSTATUS reset_value) DEVICE_ENDPOINT End Point Pipe Status */

#define USB_DEVICE_EPSTATUS_DTGLOUT_Pos 0            /**< \brief (USB_DEVICE_EPSTATUS) Data Toggle Out */
#define USB_DEVICE_EPSTATUS_DTGLOUT (0x1ul << USB_DEVICE_EPSTATUS_DTGLOUT_Pos)
#define USB_DEVICE_EPSTATUS_DTGLIN_Pos 1            /**< \brief (USB_DEVICE_EPSTATUS) Data Toggle In */
#define USB_DEVICE_EPSTATUS_DTGLIN  (0x1ul << USB_DEVICE_EPSTATUS_DTGLIN_Pos)
#define USB_DEVICE_EPSTATUS_CURBK_Pos 2            /**< \brief (USB_DEVICE_EPSTATUS) Current Bank */
#define USB_DEVICE_EPSTATUS_CURBK   (0x1ul << USB_DEVICE_EPSTATUS_CURBK_Pos)
#define USB_DEVICE_EPSTATUS_STALLRQ0_Pos 4            /**< \brief (USB_DEVICE_EPSTATUS) Stall 0 Request */
#define USB_DEVICE_EPSTATUS_STALLRQ0 (1 << USB_DEVICE_EPSTATUS_STALLRQ0_Pos)
#define USB_DEVICE_EPSTATUS_STALLRQ1_Pos 5            /**< \brief (USB_DEVICE_EPSTATUS) Stall 1 Request */
#define USB_DEVICE_EPSTATUS_STALLRQ1 (1 << USB_DEVICE_EPSTATUS_STALLRQ1_Pos)
#define USB_DEVICE_EPSTATUS_STALLRQ_Pos 4            /**< \brief (USB_DEVICE_EPSTATUS) Stall x Request */
#define USB_DEVICE_EPSTATUS_STALLRQ_Msk (0x3ul << USB_DEVICE_EPSTATUS_STALLRQ_Pos)
#define USB_DEVICE_EPSTATUS_STALLRQ(value) (USB_DEVICE_EPSTATUS_STALLRQ_Msk & ((value) << USB_DEVICE_EPSTATUS_STALLRQ_Pos))
#define USB_DEVICE_EPSTATUS_BK0RDY_Pos 6            /**< \brief (USB_DEVICE_EPSTATUS) Bank 0 ready */
#define USB_DEVICE_EPSTATUS_BK0RDY  (0x1ul << USB_DEVICE_EPSTATUS_BK0RDY_Pos)
#define USB_DEVICE_EPSTATUS_BK1RDY_Pos 7            /**< \brief (USB_DEVICE_EPSTATUS) Bank 1 ready */
#define USB_DEVICE_EPSTATUS_BK1RDY  (0x1ul << USB_DEVICE_EPSTATUS_BK1RDY_Pos)
#define USB_DEVICE_EPSTATUS_MASK    0xF7ul       /**< \brief (USB_DEVICE_EPSTATUS) MASK Register */

/* -------- USB_HOST_PSTATUS : (USB Offset: 0x106) (R/   8) HOST HOST_PIPE End Point Pipe Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DTGL:1;           /*!< bit:      0  Data Toggle                        */
    uint8_t  :1;               /*!< bit:      1  Reserved                           */
    uint8_t  CURBK:1;          /*!< bit:      2  Current Bank                       */
    uint8_t  :1;               /*!< bit:      3  Reserved                           */
    uint8_t  PFREEZE:1;        /*!< bit:      4  Pipe Freeze                        */
    uint8_t  :1;               /*!< bit:      5  Reserved                           */
    uint8_t  BK0RDY:1;         /*!< bit:      6  Bank 0 ready                       */
    uint8_t  BK1RDY:1;         /*!< bit:      7  Bank 1 ready                       */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_HOST_PSTATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_PSTATUS_OFFSET     0x106        /**< \brief (USB_HOST_PSTATUS offset) HOST_PIPE End Point Pipe Status */
#define USB_HOST_PSTATUS_RESETVALUE 0x00ul       /**< \brief (USB_HOST_PSTATUS reset_value) HOST_PIPE End Point Pipe Status */

#define USB_HOST_PSTATUS_DTGL_Pos   0            /**< \brief (USB_HOST_PSTATUS) Data Toggle */
#define USB_HOST_PSTATUS_DTGL       (0x1ul << USB_HOST_PSTATUS_DTGL_Pos)
#define USB_HOST_PSTATUS_CURBK_Pos  2            /**< \brief (USB_HOST_PSTATUS) Current Bank */
#define USB_HOST_PSTATUS_CURBK      (0x1ul << USB_HOST_PSTATUS_CURBK_Pos)
#define USB_HOST_PSTATUS_PFREEZE_Pos 4            /**< \brief (USB_HOST_PSTATUS) Pipe Freeze */
#define USB_HOST_PSTATUS_PFREEZE    (0x1ul << USB_HOST_PSTATUS_PFREEZE_Pos)
#define USB_HOST_PSTATUS_BK0RDY_Pos 6            /**< \brief (USB_HOST_PSTATUS) Bank 0 ready */
#define USB_HOST_PSTATUS_BK0RDY     (0x1ul << USB_HOST_PSTATUS_BK0RDY_Pos)
#define USB_HOST_PSTATUS_BK1RDY_Pos 7            /**< \brief (USB_HOST_PSTATUS) Bank 1 ready */
#define USB_HOST_PSTATUS_BK1RDY     (0x1ul << USB_HOST_PSTATUS_BK1RDY_Pos)
#define USB_HOST_PSTATUS_MASK       0xD5ul       /**< \brief (USB_HOST_PSTATUS) MASK Register */

/* -------- USB_DEVICE_EPINTFLAG : (USB Offset: 0x107) (R/W  8) DEVICE DEVICE_ENDPOINT End Point Interrupt Flag -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint8_t  TRCPT0:1;         /*!< bit:      0  Transfer Complete 0                */
    __I uint8_t  TRCPT1:1;         /*!< bit:      1  Transfer Complete 1                */
    __I uint8_t  TRFAIL0:1;        /*!< bit:      2  Error Flow 0                       */
    __I uint8_t  TRFAIL1:1;        /*!< bit:      3  Error Flow 1                       */
    __I uint8_t  RXSTP:1;          /*!< bit:      4  Received Setup                     */
    __I uint8_t  STALL0:1;         /*!< bit:      5  Stall 0 In/out                     */
    __I uint8_t  STALL1:1;         /*!< bit:      6  Stall 1 In/out                     */
    __I uint8_t  :1;               /*!< bit:      7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    __I uint8_t  TRCPT:2;          /*!< bit:  0.. 1  Transfer Complete x                */
    __I uint8_t  TRFAIL:2;         /*!< bit:  2.. 3  Error Flow x                       */
    __I uint8_t  :1;               /*!< bit:      4  Reserved                           */
    __I uint8_t  STALL:2;          /*!< bit:  5.. 6  Stall x In/out                     */
    __I uint8_t  :1;               /*!< bit:      7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_DEVICE_EPINTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_DEVICE_EPINTFLAG_OFFSET 0x107        /**< \brief (USB_DEVICE_EPINTFLAG offset) DEVICE_ENDPOINT End Point Interrupt Flag */
#define USB_DEVICE_EPINTFLAG_RESETVALUE 0x00ul       /**< \brief (USB_DEVICE_EPINTFLAG reset_value) DEVICE_ENDPOINT End Point Interrupt Flag */

#define USB_DEVICE_EPINTFLAG_TRCPT0_Pos 0            /**< \brief (USB_DEVICE_EPINTFLAG) Transfer Complete 0 */
#define USB_DEVICE_EPINTFLAG_TRCPT0 (1 << USB_DEVICE_EPINTFLAG_TRCPT0_Pos)
#define USB_DEVICE_EPINTFLAG_TRCPT1_Pos 1            /**< \brief (USB_DEVICE_EPINTFLAG) Transfer Complete 1 */
#define USB_DEVICE_EPINTFLAG_TRCPT1 (1 << USB_DEVICE_EPINTFLAG_TRCPT1_Pos)
#define USB_DEVICE_EPINTFLAG_TRCPT_Pos 0            /**< \brief (USB_DEVICE_EPINTFLAG) Transfer Complete x */
#define USB_DEVICE_EPINTFLAG_TRCPT_Msk (0x3ul << USB_DEVICE_EPINTFLAG_TRCPT_Pos)
#define USB_DEVICE_EPINTFLAG_TRCPT(value) (USB_DEVICE_EPINTFLAG_TRCPT_Msk & ((value) << USB_DEVICE_EPINTFLAG_TRCPT_Pos))
#define USB_DEVICE_EPINTFLAG_TRFAIL0_Pos 2            /**< \brief (USB_DEVICE_EPINTFLAG) Error Flow 0 */
#define USB_DEVICE_EPINTFLAG_TRFAIL0 (1 << USB_DEVICE_EPINTFLAG_TRFAIL0_Pos)
#define USB_DEVICE_EPINTFLAG_TRFAIL1_Pos 3            /**< \brief (USB_DEVICE_EPINTFLAG) Error Flow 1 */
#define USB_DEVICE_EPINTFLAG_TRFAIL1 (1 << USB_DEVICE_EPINTFLAG_TRFAIL1_Pos)
#define USB_DEVICE_EPINTFLAG_TRFAIL_Pos 2            /**< \brief (USB_DEVICE_EPINTFLAG) Error Flow x */
#define USB_DEVICE_EPINTFLAG_TRFAIL_Msk (0x3ul << USB_DEVICE_EPINTFLAG_TRFAIL_Pos)
#define USB_DEVICE_EPINTFLAG_TRFAIL(value) (USB_DEVICE_EPINTFLAG_TRFAIL_Msk & ((value) << USB_DEVICE_EPINTFLAG_TRFAIL_Pos))
#define USB_DEVICE_EPINTFLAG_RXSTP_Pos 4            /**< \brief (USB_DEVICE_EPINTFLAG) Received Setup */
#define USB_DEVICE_EPINTFLAG_RXSTP  (0x1ul << USB_DEVICE_EPINTFLAG_RXSTP_Pos)
#define USB_DEVICE_EPINTFLAG_STALL0_Pos 5            /**< \brief (USB_DEVICE_EPINTFLAG) Stall 0 In/out */
#define USB_DEVICE_EPINTFLAG_STALL0 (1 << USB_DEVICE_EPINTFLAG_STALL0_Pos)
#define USB_DEVICE_EPINTFLAG_STALL1_Pos 6            /**< \brief (USB_DEVICE_EPINTFLAG) Stall 1 In/out */
#define USB_DEVICE_EPINTFLAG_STALL1 (1 << USB_DEVICE_EPINTFLAG_STALL1_Pos)
#define USB_DEVICE_EPINTFLAG_STALL_Pos 5            /**< \brief (USB_DEVICE_EPINTFLAG) Stall x In/out */
#define USB_DEVICE_EPINTFLAG_STALL_Msk (0x3ul << USB_DEVICE_EPINTFLAG_STALL_Pos)
#define USB_DEVICE_EPINTFLAG_STALL(value) (USB_DEVICE_EPINTFLAG_STALL_Msk & ((value) << USB_DEVICE_EPINTFLAG_STALL_Pos))
#define USB_DEVICE_EPINTFLAG_MASK   0x7Ful       /**< \brief (USB_DEVICE_EPINTFLAG) MASK Register */

/* -------- USB_HOST_PINTFLAG : (USB Offset: 0x107) (R/W  8) HOST HOST_PIPE Pipe Interrupt Flag -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint8_t  TRCPT0:1;         /*!< bit:      0  Transfer Complete 0 Interrupt Flag */
    __I uint8_t  TRCPT1:1;         /*!< bit:      1  Transfer Complete 1 Interrupt Flag */
    __I uint8_t  TRFAIL:1;         /*!< bit:      2  Error Flow Interrupt Flag          */
    __I uint8_t  PERR:1;           /*!< bit:      3  Pipe Error Interrupt Flag          */
    __I uint8_t  TXSTP:1;          /*!< bit:      4  Transmit  Setup Interrupt Flag     */
    __I uint8_t  STALL:1;          /*!< bit:      5  Stall Interrupt Flag               */
    __I uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    __I uint8_t  TRCPT:2;          /*!< bit:  0.. 1  Transfer Complete x Interrupt Flag */
    __I uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_HOST_PINTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_PINTFLAG_OFFSET    0x107        /**< \brief (USB_HOST_PINTFLAG offset) HOST_PIPE Pipe Interrupt Flag */
#define USB_HOST_PINTFLAG_RESETVALUE 0x00ul       /**< \brief (USB_HOST_PINTFLAG reset_value) HOST_PIPE Pipe Interrupt Flag */

#define USB_HOST_PINTFLAG_TRCPT0_Pos 0            /**< \brief (USB_HOST_PINTFLAG) Transfer Complete 0 Interrupt Flag */
#define USB_HOST_PINTFLAG_TRCPT0    (1 << USB_HOST_PINTFLAG_TRCPT0_Pos)
#define USB_HOST_PINTFLAG_TRCPT1_Pos 1            /**< \brief (USB_HOST_PINTFLAG) Transfer Complete 1 Interrupt Flag */
#define USB_HOST_PINTFLAG_TRCPT1    (1 << USB_HOST_PINTFLAG_TRCPT1_Pos)
#define USB_HOST_PINTFLAG_TRCPT_Pos 0            /**< \brief (USB_HOST_PINTFLAG) Transfer Complete x Interrupt Flag */
#define USB_HOST_PINTFLAG_TRCPT_Msk (0x3ul << USB_HOST_PINTFLAG_TRCPT_Pos)
#define USB_HOST_PINTFLAG_TRCPT(value) (USB_HOST_PINTFLAG_TRCPT_Msk & ((value) << USB_HOST_PINTFLAG_TRCPT_Pos))
#define USB_HOST_PINTFLAG_TRFAIL_Pos 2            /**< \brief (USB_HOST_PINTFLAG) Error Flow Interrupt Flag */
#define USB_HOST_PINTFLAG_TRFAIL    (0x1ul << USB_HOST_PINTFLAG_TRFAIL_Pos)
#define USB_HOST_PINTFLAG_PERR_Pos  3            /**< \brief (USB_HOST_PINTFLAG) Pipe Error Interrupt Flag */
#define USB_HOST_PINTFLAG_PERR      (0x1ul << USB_HOST_PINTFLAG_PERR_Pos)
#define USB_HOST_PINTFLAG_TXSTP_Pos 4            /**< \brief (USB_HOST_PINTFLAG) Transmit  Setup Interrupt Flag */
#define USB_HOST_PINTFLAG_TXSTP     (0x1ul << USB_HOST_PINTFLAG_TXSTP_Pos)
#define USB_HOST_PINTFLAG_STALL_Pos 5            /**< \brief (USB_HOST_PINTFLAG) Stall Interrupt Flag */
#define USB_HOST_PINTFLAG_STALL     (0x1ul << USB_HOST_PINTFLAG_STALL_Pos)
#define USB_HOST_PINTFLAG_MASK      0x3Ful       /**< \brief (USB_HOST_PINTFLAG) MASK Register */

/* -------- USB_DEVICE_EPINTENCLR : (USB Offset: 0x108) (R/W  8) DEVICE DEVICE_ENDPOINT End Point Interrupt Clear Flag -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  TRCPT0:1;         /*!< bit:      0  Transfer Complete 0 Interrupt Disable */
    uint8_t  TRCPT1:1;         /*!< bit:      1  Transfer Complete 1 Interrupt Disable */
    uint8_t  TRFAIL0:1;        /*!< bit:      2  Error Flow 0 Interrupt Disable     */
    uint8_t  TRFAIL1:1;        /*!< bit:      3  Error Flow 1 Interrupt Disable     */
    uint8_t  RXSTP:1;          /*!< bit:      4  Received Setup Interrupt Disable   */
    uint8_t  STALL0:1;         /*!< bit:      5  Stall 0 In/Out Interrupt Disable   */
    uint8_t  STALL1:1;         /*!< bit:      6  Stall 1 In/Out Interrupt Disable   */
    uint8_t  :1;               /*!< bit:      7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  TRCPT:2;          /*!< bit:  0.. 1  Transfer Complete x Interrupt Disable */
    uint8_t  TRFAIL:2;         /*!< bit:  2.. 3  Error Flow x Interrupt Disable     */
    uint8_t  :1;               /*!< bit:      4  Reserved                           */
    uint8_t  STALL:2;          /*!< bit:  5.. 6  Stall x In/Out Interrupt Disable   */
    uint8_t  :1;               /*!< bit:      7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_DEVICE_EPINTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_DEVICE_EPINTENCLR_OFFSET 0x108        /**< \brief (USB_DEVICE_EPINTENCLR offset) DEVICE_ENDPOINT End Point Interrupt Clear Flag */
#define USB_DEVICE_EPINTENCLR_RESETVALUE 0x00ul       /**< \brief (USB_DEVICE_EPINTENCLR reset_value) DEVICE_ENDPOINT End Point Interrupt Clear Flag */

#define USB_DEVICE_EPINTENCLR_TRCPT0_Pos 0            /**< \brief (USB_DEVICE_EPINTENCLR) Transfer Complete 0 Interrupt Disable */
#define USB_DEVICE_EPINTENCLR_TRCPT0 (1 << USB_DEVICE_EPINTENCLR_TRCPT0_Pos)
#define USB_DEVICE_EPINTENCLR_TRCPT1_Pos 1            /**< \brief (USB_DEVICE_EPINTENCLR) Transfer Complete 1 Interrupt Disable */
#define USB_DEVICE_EPINTENCLR_TRCPT1 (1 << USB_DEVICE_EPINTENCLR_TRCPT1_Pos)
#define USB_DEVICE_EPINTENCLR_TRCPT_Pos 0            /**< \brief (USB_DEVICE_EPINTENCLR) Transfer Complete x Interrupt Disable */
#define USB_DEVICE_EPINTENCLR_TRCPT_Msk (0x3ul << USB_DEVICE_EPINTENCLR_TRCPT_Pos)
#define USB_DEVICE_EPINTENCLR_TRCPT(value) (USB_DEVICE_EPINTENCLR_TRCPT_Msk & ((value) << USB_DEVICE_EPINTENCLR_TRCPT_Pos))
#define USB_DEVICE_EPINTENCLR_TRFAIL0_Pos 2            /**< \brief (USB_DEVICE_EPINTENCLR) Error Flow 0 Interrupt Disable */
#define USB_DEVICE_EPINTENCLR_TRFAIL0 (1 << USB_DEVICE_EPINTENCLR_TRFAIL0_Pos)
#define USB_DEVICE_EPINTENCLR_TRFAIL1_Pos 3            /**< \brief (USB_DEVICE_EPINTENCLR) Error Flow 1 Interrupt Disable */
#define USB_DEVICE_EPINTENCLR_TRFAIL1 (1 << USB_DEVICE_EPINTENCLR_TRFAIL1_Pos)
#define USB_DEVICE_EPINTENCLR_TRFAIL_Pos 2            /**< \brief (USB_DEVICE_EPINTENCLR) Error Flow x Interrupt Disable */
#define USB_DEVICE_EPINTENCLR_TRFAIL_Msk (0x3ul << USB_DEVICE_EPINTENCLR_TRFAIL_Pos)
#define USB_DEVICE_EPINTENCLR_TRFAIL(value) (USB_DEVICE_EPINTENCLR_TRFAIL_Msk & ((value) << USB_DEVICE_EPINTENCLR_TRFAIL_Pos))
#define USB_DEVICE_EPINTENCLR_RXSTP_Pos 4            /**< \brief (USB_DEVICE_EPINTENCLR) Received Setup Interrupt Disable */
#define USB_DEVICE_EPINTENCLR_RXSTP (0x1ul << USB_DEVICE_EPINTENCLR_RXSTP_Pos)
#define USB_DEVICE_EPINTENCLR_STALL0_Pos 5            /**< \brief (USB_DEVICE_EPINTENCLR) Stall 0 In/Out Interrupt Disable */
#define USB_DEVICE_EPINTENCLR_STALL0 (1 << USB_DEVICE_EPINTENCLR_STALL0_Pos)
#define USB_DEVICE_EPINTENCLR_STALL1_Pos 6            /**< \brief (USB_DEVICE_EPINTENCLR) Stall 1 In/Out Interrupt Disable */
#define USB_DEVICE_EPINTENCLR_STALL1 (1 << USB_DEVICE_EPINTENCLR_STALL1_Pos)
#define USB_DEVICE_EPINTENCLR_STALL_Pos 5            /**< \brief (USB_DEVICE_EPINTENCLR) Stall x In/Out Interrupt Disable */
#define USB_DEVICE_EPINTENCLR_STALL_Msk (0x3ul << USB_DEVICE_EPINTENCLR_STALL_Pos)
#define USB_DEVICE_EPINTENCLR_STALL(value) (USB_DEVICE_EPINTENCLR_STALL_Msk & ((value) << USB_DEVICE_EPINTENCLR_STALL_Pos))
#define USB_DEVICE_EPINTENCLR_MASK  0x7Ful       /**< \brief (USB_DEVICE_EPINTENCLR) MASK Register */

/* -------- USB_HOST_PINTENCLR : (USB Offset: 0x108) (R/W  8) HOST HOST_PIPE Pipe Interrupt Flag Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  TRCPT0:1;         /*!< bit:      0  Transfer Complete 0 Disable        */
    uint8_t  TRCPT1:1;         /*!< bit:      1  Transfer Complete 1 Disable        */
    uint8_t  TRFAIL:1;         /*!< bit:      2  Error Flow Interrupt Disable       */
    uint8_t  PERR:1;           /*!< bit:      3  Pipe Error Interrupt Disable       */
    uint8_t  TXSTP:1;          /*!< bit:      4  Transmit  Setup Interrupt Disable  */
    uint8_t  STALL:1;          /*!< bit:      5  Stall Interrupt Disable            */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  TRCPT:2;          /*!< bit:  0.. 1  Transfer Complete x Disable        */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_HOST_PINTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_PINTENCLR_OFFSET   0x108        /**< \brief (USB_HOST_PINTENCLR offset) HOST_PIPE Pipe Interrupt Flag Clear */
#define USB_HOST_PINTENCLR_RESETVALUE 0x00ul       /**< \brief (USB_HOST_PINTENCLR reset_value) HOST_PIPE Pipe Interrupt Flag Clear */

#define USB_HOST_PINTENCLR_TRCPT0_Pos 0            /**< \brief (USB_HOST_PINTENCLR) Transfer Complete 0 Disable */
#define USB_HOST_PINTENCLR_TRCPT0   (1 << USB_HOST_PINTENCLR_TRCPT0_Pos)
#define USB_HOST_PINTENCLR_TRCPT1_Pos 1            /**< \brief (USB_HOST_PINTENCLR) Transfer Complete 1 Disable */
#define USB_HOST_PINTENCLR_TRCPT1   (1 << USB_HOST_PINTENCLR_TRCPT1_Pos)
#define USB_HOST_PINTENCLR_TRCPT_Pos 0            /**< \brief (USB_HOST_PINTENCLR) Transfer Complete x Disable */
#define USB_HOST_PINTENCLR_TRCPT_Msk (0x3ul << USB_HOST_PINTENCLR_TRCPT_Pos)
#define USB_HOST_PINTENCLR_TRCPT(value) (USB_HOST_PINTENCLR_TRCPT_Msk & ((value) << USB_HOST_PINTENCLR_TRCPT_Pos))
#define USB_HOST_PINTENCLR_TRFAIL_Pos 2            /**< \brief (USB_HOST_PINTENCLR) Error Flow Interrupt Disable */
#define USB_HOST_PINTENCLR_TRFAIL   (0x1ul << USB_HOST_PINTENCLR_TRFAIL_Pos)
#define USB_HOST_PINTENCLR_PERR_Pos 3            /**< \brief (USB_HOST_PINTENCLR) Pipe Error Interrupt Disable */
#define USB_HOST_PINTENCLR_PERR     (0x1ul << USB_HOST_PINTENCLR_PERR_Pos)
#define USB_HOST_PINTENCLR_TXSTP_Pos 4            /**< \brief (USB_HOST_PINTENCLR) Transmit  Setup Interrupt Disable */
#define USB_HOST_PINTENCLR_TXSTP    (0x1ul << USB_HOST_PINTENCLR_TXSTP_Pos)
#define USB_HOST_PINTENCLR_STALL_Pos 5            /**< \brief (USB_HOST_PINTENCLR) Stall Interrupt Disable */
#define USB_HOST_PINTENCLR_STALL    (0x1ul << USB_HOST_PINTENCLR_STALL_Pos)
#define USB_HOST_PINTENCLR_MASK     0x3Ful       /**< \brief (USB_HOST_PINTENCLR) MASK Register */

/* -------- USB_DEVICE_EPINTENSET : (USB Offset: 0x109) (R/W  8) DEVICE DEVICE_ENDPOINT End Point Interrupt Set Flag -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  TRCPT0:1;         /*!< bit:      0  Transfer Complete 0 Interrupt Enable */
    uint8_t  TRCPT1:1;         /*!< bit:      1  Transfer Complete 1 Interrupt Enable */
    uint8_t  TRFAIL0:1;        /*!< bit:      2  Error Flow 0 Interrupt Enable      */
    uint8_t  TRFAIL1:1;        /*!< bit:      3  Error Flow 1 Interrupt Enable      */
    uint8_t  RXSTP:1;          /*!< bit:      4  Received Setup Interrupt Enable    */
    uint8_t  STALL0:1;         /*!< bit:      5  Stall 0 In/out Interrupt enable    */
    uint8_t  STALL1:1;         /*!< bit:      6  Stall 1 In/out Interrupt enable    */
    uint8_t  :1;               /*!< bit:      7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  TRCPT:2;          /*!< bit:  0.. 1  Transfer Complete x Interrupt Enable */
    uint8_t  TRFAIL:2;         /*!< bit:  2.. 3  Error Flow x Interrupt Enable      */
    uint8_t  :1;               /*!< bit:      4  Reserved                           */
    uint8_t  STALL:2;          /*!< bit:  5.. 6  Stall x In/out Interrupt enable    */
    uint8_t  :1;               /*!< bit:      7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_DEVICE_EPINTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_DEVICE_EPINTENSET_OFFSET 0x109        /**< \brief (USB_DEVICE_EPINTENSET offset) DEVICE_ENDPOINT End Point Interrupt Set Flag */
#define USB_DEVICE_EPINTENSET_RESETVALUE 0x00ul       /**< \brief (USB_DEVICE_EPINTENSET reset_value) DEVICE_ENDPOINT End Point Interrupt Set Flag */

#define USB_DEVICE_EPINTENSET_TRCPT0_Pos 0            /**< \brief (USB_DEVICE_EPINTENSET) Transfer Complete 0 Interrupt Enable */
#define USB_DEVICE_EPINTENSET_TRCPT0 (1 << USB_DEVICE_EPINTENSET_TRCPT0_Pos)
#define USB_DEVICE_EPINTENSET_TRCPT1_Pos 1            /**< \brief (USB_DEVICE_EPINTENSET) Transfer Complete 1 Interrupt Enable */
#define USB_DEVICE_EPINTENSET_TRCPT1 (1 << USB_DEVICE_EPINTENSET_TRCPT1_Pos)
#define USB_DEVICE_EPINTENSET_TRCPT_Pos 0            /**< \brief (USB_DEVICE_EPINTENSET) Transfer Complete x Interrupt Enable */
#define USB_DEVICE_EPINTENSET_TRCPT_Msk (0x3ul << USB_DEVICE_EPINTENSET_TRCPT_Pos)
#define USB_DEVICE_EPINTENSET_TRCPT(value) (USB_DEVICE_EPINTENSET_TRCPT_Msk & ((value) << USB_DEVICE_EPINTENSET_TRCPT_Pos))
#define USB_DEVICE_EPINTENSET_TRFAIL0_Pos 2            /**< \brief (USB_DEVICE_EPINTENSET) Error Flow 0 Interrupt Enable */
#define USB_DEVICE_EPINTENSET_TRFAIL0 (1 << USB_DEVICE_EPINTENSET_TRFAIL0_Pos)
#define USB_DEVICE_EPINTENSET_TRFAIL1_Pos 3            /**< \brief (USB_DEVICE_EPINTENSET) Error Flow 1 Interrupt Enable */
#define USB_DEVICE_EPINTENSET_TRFAIL1 (1 << USB_DEVICE_EPINTENSET_TRFAIL1_Pos)
#define USB_DEVICE_EPINTENSET_TRFAIL_Pos 2            /**< \brief (USB_DEVICE_EPINTENSET) Error Flow x Interrupt Enable */
#define USB_DEVICE_EPINTENSET_TRFAIL_Msk (0x3ul << USB_DEVICE_EPINTENSET_TRFAIL_Pos)
#define USB_DEVICE_EPINTENSET_TRFAIL(value) (USB_DEVICE_EPINTENSET_TRFAIL_Msk & ((value) << USB_DEVICE_EPINTENSET_TRFAIL_Pos))
#define USB_DEVICE_EPINTENSET_RXSTP_Pos 4            /**< \brief (USB_DEVICE_EPINTENSET) Received Setup Interrupt Enable */
#define USB_DEVICE_EPINTENSET_RXSTP (0x1ul << USB_DEVICE_EPINTENSET_RXSTP_Pos)
#define USB_DEVICE_EPINTENSET_STALL0_Pos 5            /**< \brief (USB_DEVICE_EPINTENSET) Stall 0 In/out Interrupt enable */
#define USB_DEVICE_EPINTENSET_STALL0 (1 << USB_DEVICE_EPINTENSET_STALL0_Pos)
#define USB_DEVICE_EPINTENSET_STALL1_Pos 6            /**< \brief (USB_DEVICE_EPINTENSET) Stall 1 In/out Interrupt enable */
#define USB_DEVICE_EPINTENSET_STALL1 (1 << USB_DEVICE_EPINTENSET_STALL1_Pos)
#define USB_DEVICE_EPINTENSET_STALL_Pos 5            /**< \brief (USB_DEVICE_EPINTENSET) Stall x In/out Interrupt enable */
#define USB_DEVICE_EPINTENSET_STALL_Msk (0x3ul << USB_DEVICE_EPINTENSET_STALL_Pos)
#define USB_DEVICE_EPINTENSET_STALL(value) (USB_DEVICE_EPINTENSET_STALL_Msk & ((value) << USB_DEVICE_EPINTENSET_STALL_Pos))
#define USB_DEVICE_EPINTENSET_MASK  0x7Ful       /**< \brief (USB_DEVICE_EPINTENSET) MASK Register */

/* -------- USB_HOST_PINTENSET : (USB Offset: 0x109) (R/W  8) HOST HOST_PIPE Pipe Interrupt Flag Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  TRCPT0:1;         /*!< bit:      0  Transfer Complete 0 Interrupt Enable */
    uint8_t  TRCPT1:1;         /*!< bit:      1  Transfer Complete 1 Interrupt Enable */
    uint8_t  TRFAIL:1;         /*!< bit:      2  Error Flow Interrupt Enable        */
    uint8_t  PERR:1;           /*!< bit:      3  Pipe Error Interrupt Enable        */
    uint8_t  TXSTP:1;          /*!< bit:      4  Transmit  Setup Interrupt Enable   */
    uint8_t  STALL:1;          /*!< bit:      5  Stall Interrupt Enable             */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  TRCPT:2;          /*!< bit:  0.. 1  Transfer Complete x Interrupt Enable */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_HOST_PINTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_PINTENSET_OFFSET   0x109        /**< \brief (USB_HOST_PINTENSET offset) HOST_PIPE Pipe Interrupt Flag Set */
#define USB_HOST_PINTENSET_RESETVALUE 0x00ul       /**< \brief (USB_HOST_PINTENSET reset_value) HOST_PIPE Pipe Interrupt Flag Set */

#define USB_HOST_PINTENSET_TRCPT0_Pos 0            /**< \brief (USB_HOST_PINTENSET) Transfer Complete 0 Interrupt Enable */
#define USB_HOST_PINTENSET_TRCPT0   (1 << USB_HOST_PINTENSET_TRCPT0_Pos)
#define USB_HOST_PINTENSET_TRCPT1_Pos 1            /**< \brief (USB_HOST_PINTENSET) Transfer Complete 1 Interrupt Enable */
#define USB_HOST_PINTENSET_TRCPT1   (1 << USB_HOST_PINTENSET_TRCPT1_Pos)
#define USB_HOST_PINTENSET_TRCPT_Pos 0            /**< \brief (USB_HOST_PINTENSET) Transfer Complete x Interrupt Enable */
#define USB_HOST_PINTENSET_TRCPT_Msk (0x3ul << USB_HOST_PINTENSET_TRCPT_Pos)
#define USB_HOST_PINTENSET_TRCPT(value) (USB_HOST_PINTENSET_TRCPT_Msk & ((value) << USB_HOST_PINTENSET_TRCPT_Pos))
#define USB_HOST_PINTENSET_TRFAIL_Pos 2            /**< \brief (USB_HOST_PINTENSET) Error Flow Interrupt Enable */
#define USB_HOST_PINTENSET_TRFAIL   (0x1ul << USB_HOST_PINTENSET_TRFAIL_Pos)
#define USB_HOST_PINTENSET_PERR_Pos 3            /**< \brief (USB_HOST_PINTENSET) Pipe Error Interrupt Enable */
#define USB_HOST_PINTENSET_PERR     (0x1ul << USB_HOST_PINTENSET_PERR_Pos)
#define USB_HOST_PINTENSET_TXSTP_Pos 4            /**< \brief (USB_HOST_PINTENSET) Transmit  Setup Interrupt Enable */
#define USB_HOST_PINTENSET_TXSTP    (0x1ul << USB_HOST_PINTENSET_TXSTP_Pos)
#define USB_HOST_PINTENSET_STALL_Pos 5            /**< \brief (USB_HOST_PINTENSET) Stall Interrupt Enable */
#define USB_HOST_PINTENSET_STALL    (0x1ul << USB_HOST_PINTENSET_STALL_Pos)
#define USB_HOST_PINTENSET_MASK     0x3Ful       /**< \brief (USB_HOST_PINTENSET) MASK Register */

/* -------- USB_DEVICE_ADDR : (USB Offset: 0x000) (R/W 32) DEVICE DEVICE_DESC_BANK Endpoint Bank, Adress of Data Buffer -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ADDR:32;          /*!< bit:  0..31  Adress of data buffer              */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} USB_DEVICE_ADDR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_DEVICE_ADDR_OFFSET      0x000        /**< \brief (USB_DEVICE_ADDR offset) DEVICE_DESC_BANK Endpoint Bank, Adress of Data Buffer */

#define USB_DEVICE_ADDR_ADDR_Pos    0            /**< \brief (USB_DEVICE_ADDR) Adress of data buffer */
#define USB_DEVICE_ADDR_ADDR_Msk    (0xFFFFFFFFul << USB_DEVICE_ADDR_ADDR_Pos)
#define USB_DEVICE_ADDR_ADDR(value) (USB_DEVICE_ADDR_ADDR_Msk & ((value) << USB_DEVICE_ADDR_ADDR_Pos))
#define USB_DEVICE_ADDR_MASK        0xFFFFFFFFul /**< \brief (USB_DEVICE_ADDR) MASK Register */

/* -------- USB_HOST_ADDR : (USB Offset: 0x000) (R/W 32) HOST HOST_DESC_BANK Host Bank, Adress of Data Buffer -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ADDR:32;          /*!< bit:  0..31  Adress of data buffer              */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} USB_HOST_ADDR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_ADDR_OFFSET        0x000        /**< \brief (USB_HOST_ADDR offset) HOST_DESC_BANK Host Bank, Adress of Data Buffer */

#define USB_HOST_ADDR_ADDR_Pos      0            /**< \brief (USB_HOST_ADDR) Adress of data buffer */
#define USB_HOST_ADDR_ADDR_Msk      (0xFFFFFFFFul << USB_HOST_ADDR_ADDR_Pos)
#define USB_HOST_ADDR_ADDR(value)   (USB_HOST_ADDR_ADDR_Msk & ((value) << USB_HOST_ADDR_ADDR_Pos))
#define USB_HOST_ADDR_MASK          0xFFFFFFFFul /**< \brief (USB_HOST_ADDR) MASK Register */

/* -------- USB_DEVICE_PCKSIZE : (USB Offset: 0x004) (R/W 32) DEVICE DEVICE_DESC_BANK Endpoint Bank, Packet Size -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t BYTE_COUNT:14;    /*!< bit:  0..13  Byte Count                         */
    uint32_t MULTI_PACKET_SIZE:14; /*!< bit: 14..27  Multi Packet In or Out size        */
    uint32_t SIZE:3;           /*!< bit: 28..30  Enpoint size                       */
    uint32_t AUTO_ZLP:1;       /*!< bit:     31  Automatic Zero Length Packet       */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} USB_DEVICE_PCKSIZE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_DEVICE_PCKSIZE_OFFSET   0x004        /**< \brief (USB_DEVICE_PCKSIZE offset) DEVICE_DESC_BANK Endpoint Bank, Packet Size */

#define USB_DEVICE_PCKSIZE_BYTE_COUNT_Pos 0            /**< \brief (USB_DEVICE_PCKSIZE) Byte Count */
#define USB_DEVICE_PCKSIZE_BYTE_COUNT_Msk (0x3FFFul << USB_DEVICE_PCKSIZE_BYTE_COUNT_Pos)
#define USB_DEVICE_PCKSIZE_BYTE_COUNT(value) (USB_DEVICE_PCKSIZE_BYTE_COUNT_Msk & ((value) << USB_DEVICE_PCKSIZE_BYTE_COUNT_Pos))
#define USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE_Pos 14           /**< \brief (USB_DEVICE_PCKSIZE) Multi Packet In or Out size */
#define USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE_Msk (0x3FFFul << USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE_Pos)
#define USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE(value) (USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE_Msk & ((value) << USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE_Pos))
#define USB_DEVICE_PCKSIZE_SIZE_Pos 28           /**< \brief (USB_DEVICE_PCKSIZE) Enpoint size */
#define USB_DEVICE_PCKSIZE_SIZE_Msk (0x7ul << USB_DEVICE_PCKSIZE_SIZE_Pos)
#define USB_DEVICE_PCKSIZE_SIZE(value) (USB_DEVICE_PCKSIZE_SIZE_Msk & ((value) << USB_DEVICE_PCKSIZE_SIZE_Pos))
#define USB_DEVICE_PCKSIZE_AUTO_ZLP_Pos 31           /**< \brief (USB_DEVICE_PCKSIZE) Automatic Zero Length Packet */
#define USB_DEVICE_PCKSIZE_AUTO_ZLP (0x1ul << USB_DEVICE_PCKSIZE_AUTO_ZLP_Pos)
#define USB_DEVICE_PCKSIZE_MASK     0xFFFFFFFFul /**< \brief (USB_DEVICE_PCKSIZE) MASK Register */

/* -------- USB_HOST_PCKSIZE : (USB Offset: 0x004) (R/W 32) HOST HOST_DESC_BANK Host Bank, Packet Size -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t BYTE_COUNT:14;    /*!< bit:  0..13  Byte Count                         */
    uint32_t MULTI_PACKET_SIZE:14; /*!< bit: 14..27  Multi Packet In or Out size        */
    uint32_t SIZE:3;           /*!< bit: 28..30  Pipe size                          */
    uint32_t AUTO_ZLP:1;       /*!< bit:     31  Automatic Zero Length Packet       */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} USB_HOST_PCKSIZE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_PCKSIZE_OFFSET     0x004        /**< \brief (USB_HOST_PCKSIZE offset) HOST_DESC_BANK Host Bank, Packet Size */

#define USB_HOST_PCKSIZE_BYTE_COUNT_Pos 0            /**< \brief (USB_HOST_PCKSIZE) Byte Count */
#define USB_HOST_PCKSIZE_BYTE_COUNT_Msk (0x3FFFul << USB_HOST_PCKSIZE_BYTE_COUNT_Pos)
#define USB_HOST_PCKSIZE_BYTE_COUNT(value) (USB_HOST_PCKSIZE_BYTE_COUNT_Msk & ((value) << USB_HOST_PCKSIZE_BYTE_COUNT_Pos))
#define USB_HOST_PCKSIZE_MULTI_PACKET_SIZE_Pos 14           /**< \brief (USB_HOST_PCKSIZE) Multi Packet In or Out size */
#define USB_HOST_PCKSIZE_MULTI_PACKET_SIZE_Msk (0x3FFFul << USB_HOST_PCKSIZE_MULTI_PACKET_SIZE_Pos)
#define USB_HOST_PCKSIZE_MULTI_PACKET_SIZE(value) (USB_HOST_PCKSIZE_MULTI_PACKET_SIZE_Msk & ((value) << USB_HOST_PCKSIZE_MULTI_PACKET_SIZE_Pos))
#define USB_HOST_PCKSIZE_SIZE_Pos   28           /**< \brief (USB_HOST_PCKSIZE) Pipe size */
#define USB_HOST_PCKSIZE_SIZE_Msk   (0x7ul << USB_HOST_PCKSIZE_SIZE_Pos)
#define USB_HOST_PCKSIZE_SIZE(value) (USB_HOST_PCKSIZE_SIZE_Msk & ((value) << USB_HOST_PCKSIZE_SIZE_Pos))
#define USB_HOST_PCKSIZE_AUTO_ZLP_Pos 31           /**< \brief (USB_HOST_PCKSIZE) Automatic Zero Length Packet */
#define USB_HOST_PCKSIZE_AUTO_ZLP   (0x1ul << USB_HOST_PCKSIZE_AUTO_ZLP_Pos)
#define USB_HOST_PCKSIZE_MASK       0xFFFFFFFFul /**< \brief (USB_HOST_PCKSIZE) MASK Register */

/* -------- USB_DEVICE_EXTREG : (USB Offset: 0x008) (R/W 16) DEVICE DEVICE_DESC_BANK Endpoint Bank, Extended -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t SUBPID:4;         /*!< bit:  0.. 3  SUBPID field send with extended token */
    uint16_t VARIABLE:11;      /*!< bit:  4..14  Variable field send with extended token */
    uint16_t :1;               /*!< bit:     15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} USB_DEVICE_EXTREG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_DEVICE_EXTREG_OFFSET    0x008        /**< \brief (USB_DEVICE_EXTREG offset) DEVICE_DESC_BANK Endpoint Bank, Extended */

#define USB_DEVICE_EXTREG_SUBPID_Pos 0            /**< \brief (USB_DEVICE_EXTREG) SUBPID field send with extended token */
#define USB_DEVICE_EXTREG_SUBPID_Msk (0xFul << USB_DEVICE_EXTREG_SUBPID_Pos)
#define USB_DEVICE_EXTREG_SUBPID(value) (USB_DEVICE_EXTREG_SUBPID_Msk & ((value) << USB_DEVICE_EXTREG_SUBPID_Pos))
#define USB_DEVICE_EXTREG_VARIABLE_Pos 4            /**< \brief (USB_DEVICE_EXTREG) Variable field send with extended token */
#define USB_DEVICE_EXTREG_VARIABLE_Msk (0x7FFul << USB_DEVICE_EXTREG_VARIABLE_Pos)
#define USB_DEVICE_EXTREG_VARIABLE(value) (USB_DEVICE_EXTREG_VARIABLE_Msk & ((value) << USB_DEVICE_EXTREG_VARIABLE_Pos))
#define USB_DEVICE_EXTREG_MASK      0x7FFFul     /**< \brief (USB_DEVICE_EXTREG) MASK Register */

/* -------- USB_HOST_EXTREG : (USB Offset: 0x008) (R/W 16) HOST HOST_DESC_BANK Host Bank, Extended -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t SUBPID:4;         /*!< bit:  0.. 3  SUBPID field send with extended token */
    uint16_t VARIABLE:11;      /*!< bit:  4..14  Variable field send with extended token */
    uint16_t :1;               /*!< bit:     15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} USB_HOST_EXTREG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_EXTREG_OFFSET      0x008        /**< \brief (USB_HOST_EXTREG offset) HOST_DESC_BANK Host Bank, Extended */

#define USB_HOST_EXTREG_SUBPID_Pos  0            /**< \brief (USB_HOST_EXTREG) SUBPID field send with extended token */
#define USB_HOST_EXTREG_SUBPID_Msk  (0xFul << USB_HOST_EXTREG_SUBPID_Pos)
#define USB_HOST_EXTREG_SUBPID(value) (USB_HOST_EXTREG_SUBPID_Msk & ((value) << USB_HOST_EXTREG_SUBPID_Pos))
#define USB_HOST_EXTREG_VARIABLE_Pos 4            /**< \brief (USB_HOST_EXTREG) Variable field send with extended token */
#define USB_HOST_EXTREG_VARIABLE_Msk (0x7FFul << USB_HOST_EXTREG_VARIABLE_Pos)
#define USB_HOST_EXTREG_VARIABLE(value) (USB_HOST_EXTREG_VARIABLE_Msk & ((value) << USB_HOST_EXTREG_VARIABLE_Pos))
#define USB_HOST_EXTREG_MASK        0x7FFFul     /**< \brief (USB_HOST_EXTREG) MASK Register */

/* -------- USB_DEVICE_STATUS_BK : (USB Offset: 0x00A) (R/W  8) DEVICE DEVICE_DESC_BANK Enpoint Bank, Status of Bank -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CRCERR:1;         /*!< bit:      0  CRC Error Status                   */
    uint8_t  ERRORFLOW:1;      /*!< bit:      1  Error Flow Status                  */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_DEVICE_STATUS_BK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_DEVICE_STATUS_BK_OFFSET 0x00A        /**< \brief (USB_DEVICE_STATUS_BK offset) DEVICE_DESC_BANK Enpoint Bank, Status of Bank */

#define USB_DEVICE_STATUS_BK_CRCERR_Pos 0            /**< \brief (USB_DEVICE_STATUS_BK) CRC Error Status */
#define USB_DEVICE_STATUS_BK_CRCERR (0x1ul << USB_DEVICE_STATUS_BK_CRCERR_Pos)
#define USB_DEVICE_STATUS_BK_ERRORFLOW_Pos 1            /**< \brief (USB_DEVICE_STATUS_BK) Error Flow Status */
#define USB_DEVICE_STATUS_BK_ERRORFLOW (0x1ul << USB_DEVICE_STATUS_BK_ERRORFLOW_Pos)
#define USB_DEVICE_STATUS_BK_MASK   0x03ul       /**< \brief (USB_DEVICE_STATUS_BK) MASK Register */

/* -------- USB_HOST_STATUS_BK : (USB Offset: 0x00A) (R/W  8) HOST HOST_DESC_BANK Host Bank, Status of Bank -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CRCERR:1;         /*!< bit:      0  CRC Error Status                   */
    uint8_t  ERRORFLOW:1;      /*!< bit:      1  Error Flow Status                  */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} USB_HOST_STATUS_BK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_STATUS_BK_OFFSET   0x00A        /**< \brief (USB_HOST_STATUS_BK offset) HOST_DESC_BANK Host Bank, Status of Bank */

#define USB_HOST_STATUS_BK_CRCERR_Pos 0            /**< \brief (USB_HOST_STATUS_BK) CRC Error Status */
#define USB_HOST_STATUS_BK_CRCERR   (0x1ul << USB_HOST_STATUS_BK_CRCERR_Pos)
#define USB_HOST_STATUS_BK_ERRORFLOW_Pos 1            /**< \brief (USB_HOST_STATUS_BK) Error Flow Status */
#define USB_HOST_STATUS_BK_ERRORFLOW (0x1ul << USB_HOST_STATUS_BK_ERRORFLOW_Pos)
#define USB_HOST_STATUS_BK_MASK     0x03ul       /**< \brief (USB_HOST_STATUS_BK) MASK Register */

/* -------- USB_HOST_CTRL_PIPE : (USB Offset: 0x00C) (R/W 16) HOST HOST_DESC_BANK Host Bank, Host Control Pipe -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t PDADDR:7;         /*!< bit:  0.. 6  Pipe Device Adress                 */
    uint16_t :1;               /*!< bit:      7  Reserved                           */
    uint16_t PEPNUM:4;         /*!< bit:  8..11  Pipe Endpoint Number               */
    uint16_t PERMAX:4;         /*!< bit: 12..15  Pipe Error Max Number              */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} USB_HOST_CTRL_PIPE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_CTRL_PIPE_OFFSET   0x00C        /**< \brief (USB_HOST_CTRL_PIPE offset) HOST_DESC_BANK Host Bank, Host Control Pipe */
#define USB_HOST_CTRL_PIPE_RESETVALUE 0x0000ul     /**< \brief (USB_HOST_CTRL_PIPE reset_value) HOST_DESC_BANK Host Bank, Host Control Pipe */

#define USB_HOST_CTRL_PIPE_PDADDR_Pos 0            /**< \brief (USB_HOST_CTRL_PIPE) Pipe Device Adress */
#define USB_HOST_CTRL_PIPE_PDADDR_Msk (0x7Ful << USB_HOST_CTRL_PIPE_PDADDR_Pos)
#define USB_HOST_CTRL_PIPE_PDADDR(value) (USB_HOST_CTRL_PIPE_PDADDR_Msk & ((value) << USB_HOST_CTRL_PIPE_PDADDR_Pos))
#define USB_HOST_CTRL_PIPE_PEPNUM_Pos 8            /**< \brief (USB_HOST_CTRL_PIPE) Pipe Endpoint Number */
#define USB_HOST_CTRL_PIPE_PEPNUM_Msk (0xFul << USB_HOST_CTRL_PIPE_PEPNUM_Pos)
#define USB_HOST_CTRL_PIPE_PEPNUM(value) (USB_HOST_CTRL_PIPE_PEPNUM_Msk & ((value) << USB_HOST_CTRL_PIPE_PEPNUM_Pos))
#define USB_HOST_CTRL_PIPE_PERMAX_Pos 12           /**< \brief (USB_HOST_CTRL_PIPE) Pipe Error Max Number */
#define USB_HOST_CTRL_PIPE_PERMAX_Msk (0xFul << USB_HOST_CTRL_PIPE_PERMAX_Pos)
#define USB_HOST_CTRL_PIPE_PERMAX(value) (USB_HOST_CTRL_PIPE_PERMAX_Msk & ((value) << USB_HOST_CTRL_PIPE_PERMAX_Pos))
#define USB_HOST_CTRL_PIPE_MASK     0xFF7Ful     /**< \brief (USB_HOST_CTRL_PIPE) MASK Register */

/* -------- USB_HOST_STATUS_PIPE : (USB Offset: 0x00E) (R/W 16) HOST HOST_DESC_BANK Host Bank, Host Status Pipe -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t DTGLER:1;         /*!< bit:      0  Data Toggle Error                  */
    uint16_t DAPIDER:1;        /*!< bit:      1  Data PID Error                     */
    uint16_t PIDER:1;          /*!< bit:      2  PID Error                          */
    uint16_t TOUTER:1;         /*!< bit:      3  Time Out Error                     */
    uint16_t CRC16ER:1;        /*!< bit:      4  CRC16 Error                        */
    uint16_t ERCNT:3;          /*!< bit:  5.. 7  Pipe Error Count                   */
    uint16_t :8;               /*!< bit:  8..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} USB_HOST_STATUS_PIPE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define USB_HOST_STATUS_PIPE_OFFSET 0x00E        /**< \brief (USB_HOST_STATUS_PIPE offset) HOST_DESC_BANK Host Bank, Host Status Pipe */

#define USB_HOST_STATUS_PIPE_DTGLER_Pos 0            /**< \brief (USB_HOST_STATUS_PIPE) Data Toggle Error */
#define USB_HOST_STATUS_PIPE_DTGLER (0x1ul << USB_HOST_STATUS_PIPE_DTGLER_Pos)
#define USB_HOST_STATUS_PIPE_DAPIDER_Pos 1            /**< \brief (USB_HOST_STATUS_PIPE) Data PID Error */
#define USB_HOST_STATUS_PIPE_DAPIDER (0x1ul << USB_HOST_STATUS_PIPE_DAPIDER_Pos)
#define USB_HOST_STATUS_PIPE_PIDER_Pos 2            /**< \brief (USB_HOST_STATUS_PIPE) PID Error */
#define USB_HOST_STATUS_PIPE_PIDER  (0x1ul << USB_HOST_STATUS_PIPE_PIDER_Pos)
#define USB_HOST_STATUS_PIPE_TOUTER_Pos 3            /**< \brief (USB_HOST_STATUS_PIPE) Time Out Error */
#define USB_HOST_STATUS_PIPE_TOUTER (0x1ul << USB_HOST_STATUS_PIPE_TOUTER_Pos)
#define USB_HOST_STATUS_PIPE_CRC16ER_Pos 4            /**< \brief (USB_HOST_STATUS_PIPE) CRC16 Error */
#define USB_HOST_STATUS_PIPE_CRC16ER (0x1ul << USB_HOST_STATUS_PIPE_CRC16ER_Pos)
#define USB_HOST_STATUS_PIPE_ERCNT_Pos 5            /**< \brief (USB_HOST_STATUS_PIPE) Pipe Error Count */
#define USB_HOST_STATUS_PIPE_ERCNT_Msk (0x7ul << USB_HOST_STATUS_PIPE_ERCNT_Pos)
#define USB_HOST_STATUS_PIPE_ERCNT(value) (USB_HOST_STATUS_PIPE_ERCNT_Msk & ((value) << USB_HOST_STATUS_PIPE_ERCNT_Pos))
#define USB_HOST_STATUS_PIPE_MASK   0x00FFul     /**< \brief (USB_HOST_STATUS_PIPE) MASK Register */

/** \brief UsbDeviceDescBank SRAM registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO USB_DEVICE_ADDR_Type      ADDR;        /**< \brief Offset: 0x000 (R/W 32) DEVICE_DESC_BANK Endpoint Bank, Adress of Data Buffer */
  __IO USB_DEVICE_PCKSIZE_Type   PCKSIZE;     /**< \brief Offset: 0x004 (R/W 32) DEVICE_DESC_BANK Endpoint Bank, Packet Size */
  __IO USB_DEVICE_EXTREG_Type    EXTREG;      /**< \brief Offset: 0x008 (R/W 16) DEVICE_DESC_BANK Endpoint Bank, Extended */
  __IO USB_DEVICE_STATUS_BK_Type STATUS_BK;   /**< \brief Offset: 0x00A (R/W  8) DEVICE_DESC_BANK Enpoint Bank, Status of Bank */
       RoReg8                    Reserved1[0x5];
} UsbDeviceDescBank;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief UsbHostDescBank SRAM registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO USB_HOST_ADDR_Type        ADDR;        /**< \brief Offset: 0x000 (R/W 32) HOST_DESC_BANK Host Bank, Adress of Data Buffer */
  __IO USB_HOST_PCKSIZE_Type     PCKSIZE;     /**< \brief Offset: 0x004 (R/W 32) HOST_DESC_BANK Host Bank, Packet Size */
  __IO USB_HOST_EXTREG_Type      EXTREG;      /**< \brief Offset: 0x008 (R/W 16) HOST_DESC_BANK Host Bank, Extended */
  __IO USB_HOST_STATUS_BK_Type   STATUS_BK;   /**< \brief Offset: 0x00A (R/W  8) HOST_DESC_BANK Host Bank, Status of Bank */
       RoReg8                    Reserved1[0x1];
  __IO USB_HOST_CTRL_PIPE_Type   CTRL_PIPE;   /**< \brief Offset: 0x00C (R/W 16) HOST_DESC_BANK Host Bank, Host Control Pipe */
  __IO USB_HOST_STATUS_PIPE_Type STATUS_PIPE; /**< \brief Offset: 0x00E (R/W 16) HOST_DESC_BANK Host Bank, Host Status Pipe */
} UsbHostDescBank;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief UsbDeviceEndpoint hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO USB_DEVICE_EPCFG_Type     EPCFG;       /**< \brief Offset: 0x000 (R/W  8) DEVICE_ENDPOINT End Point Configuration */
       RoReg8                    Reserved1[0x3];
  __O  USB_DEVICE_EPSTATUSCLR_Type EPSTATUSCLR; /**< \brief Offset: 0x004 ( /W  8) DEVICE_ENDPOINT End Point Pipe Status Clear */
  __O  USB_DEVICE_EPSTATUSSET_Type EPSTATUSSET; /**< \brief Offset: 0x005 ( /W  8) DEVICE_ENDPOINT End Point Pipe Status Set */
  __I  USB_DEVICE_EPSTATUS_Type  EPSTATUS;    /**< \brief Offset: 0x006 (R/   8) DEVICE_ENDPOINT End Point Pipe Status */
  __IO USB_DEVICE_EPINTFLAG_Type EPINTFLAG;   /**< \brief Offset: 0x007 (R/W  8) DEVICE_ENDPOINT End Point Interrupt Flag */
  __IO USB_DEVICE_EPINTENCLR_Type EPINTENCLR;  /**< \brief Offset: 0x008 (R/W  8) DEVICE_ENDPOINT End Point Interrupt Clear Flag */
  __IO USB_DEVICE_EPINTENSET_Type EPINTENSET;  /**< \brief Offset: 0x009 (R/W  8) DEVICE_ENDPOINT End Point Interrupt Set Flag */
       RoReg8                    Reserved2[0x16];
} UsbDeviceEndpoint;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief UsbHostPipe hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO USB_HOST_PCFG_Type        PCFG;        /**< \brief Offset: 0x000 (R/W  8) HOST_PIPE End Point Configuration */
       RoReg8                    Reserved1[0x2];
  __IO USB_HOST_BINTERVAL_Type   BINTERVAL;   /**< \brief Offset: 0x003 (R/W  8) HOST_PIPE Bus Access Period of Pipe */
  __O  USB_HOST_PSTATUSCLR_Type  PSTATUSCLR;  /**< \brief Offset: 0x004 ( /W  8) HOST_PIPE End Point Pipe Status Clear */
  __O  USB_HOST_PSTATUSSET_Type  PSTATUSSET;  /**< \brief Offset: 0x005 ( /W  8) HOST_PIPE End Point Pipe Status Set */
  __I  USB_HOST_PSTATUS_Type     PSTATUS;     /**< \brief Offset: 0x006 (R/   8) HOST_PIPE End Point Pipe Status */
  __IO USB_HOST_PINTFLAG_Type    PINTFLAG;    /**< \brief Offset: 0x007 (R/W  8) HOST_PIPE Pipe Interrupt Flag */
  __IO USB_HOST_PINTENCLR_Type   PINTENCLR;   /**< \brief Offset: 0x008 (R/W  8) HOST_PIPE Pipe Interrupt Flag Clear */
  __IO USB_HOST_PINTENSET_Type   PINTENSET;   /**< \brief Offset: 0x009 (R/W  8) HOST_PIPE Pipe Interrupt Flag Set */
       RoReg8                    Reserved2[0x16];
} UsbHostPipe;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief USB_DEVICE APB hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct { /* USB is Device */
  __IO USB_CTRLA_Type            CTRLA;       /**< \brief Offset: 0x000 (R/W  8) Control A */
       RoReg8                    Reserved1[0x1];
  __I  USB_SYNCBUSY_Type         SYNCBUSY;    /**< \brief Offset: 0x002 (R/   8) Synchronization Busy */
  __IO USB_QOSCTRL_Type          QOSCTRL;     /**< \brief Offset: 0x003 (R/W  8) USB Quality Of Service */
       RoReg8                    Reserved2[0x4];
  __IO USB_DEVICE_CTRLB_Type     CTRLB;       /**< \brief Offset: 0x008 (R/W 16) DEVICE Control B */
  __IO USB_DEVICE_DADD_Type      DADD;        /**< \brief Offset: 0x00A (R/W  8) DEVICE Device Address */
       RoReg8                    Reserved3[0x1];
  __I  USB_DEVICE_STATUS_Type    STATUS;      /**< \brief Offset: 0x00C (R/   8) DEVICE Status */
  __I  USB_FSMSTATUS_Type        FSMSTATUS;   /**< \brief Offset: 0x00D (R/   8) Finite State Machine Status */
       RoReg8                    Reserved4[0x2];
  __I  USB_DEVICE_FNUM_Type      FNUM;        /**< \brief Offset: 0x010 (R/  16) DEVICE Device Frame Number */
       RoReg8                    Reserved5[0x2];
  __IO USB_DEVICE_INTENCLR_Type  INTENCLR;    /**< \brief Offset: 0x014 (R/W 16) DEVICE Device Interrupt Enable Clear */
       RoReg8                    Reserved6[0x2];
  __IO USB_DEVICE_INTENSET_Type  INTENSET;    /**< \brief Offset: 0x018 (R/W 16) DEVICE Device Interrupt Enable Set */
       RoReg8                    Reserved7[0x2];
  __IO USB_DEVICE_INTFLAG_Type   INTFLAG;     /**< \brief Offset: 0x01C (R/W 16) DEVICE Device Interrupt Flag */
       RoReg8                    Reserved8[0x2];
  __I  USB_DEVICE_EPINTSMRY_Type EPINTSMRY;   /**< \brief Offset: 0x020 (R/  16) DEVICE End Point Interrupt Summary */
       RoReg8                    Reserved9[0x2];
  __IO USB_DESCADD_Type          DESCADD;     /**< \brief Offset: 0x024 (R/W 32) Descriptor Address */
  __IO USB_PADCAL_Type           PADCAL;      /**< \brief Offset: 0x028 (R/W 16) USB PAD Calibration */
       RoReg8                    Reserved10[0xD6];
       UsbDeviceEndpoint         DeviceEndpoint[8]; /**< \brief Offset: 0x100 UsbDeviceEndpoint groups [EPT_NUM] */
} UsbDevice;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief USB_HOST hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct { /* USB is Host */
  __IO USB_CTRLA_Type            CTRLA;       /**< \brief Offset: 0x000 (R/W  8) Control A */
       RoReg8                    Reserved1[0x1];
  __I  USB_SYNCBUSY_Type         SYNCBUSY;    /**< \brief Offset: 0x002 (R/   8) Synchronization Busy */
  __IO USB_QOSCTRL_Type          QOSCTRL;     /**< \brief Offset: 0x003 (R/W  8) USB Quality Of Service */
       RoReg8                    Reserved2[0x4];
  __IO USB_HOST_CTRLB_Type       CTRLB;       /**< \brief Offset: 0x008 (R/W 16) HOST Control B */
  __IO USB_HOST_HSOFC_Type       HSOFC;       /**< \brief Offset: 0x00A (R/W  8) HOST Host Start Of Frame Control */
       RoReg8                    Reserved3[0x1];
  __IO USB_HOST_STATUS_Type      STATUS;      /**< \brief Offset: 0x00C (R/W  8) HOST Status */
  __I  USB_FSMSTATUS_Type        FSMSTATUS;   /**< \brief Offset: 0x00D (R/   8) Finite State Machine Status */
       RoReg8                    Reserved4[0x2];
  __IO USB_HOST_FNUM_Type        FNUM;        /**< \brief Offset: 0x010 (R/W 16) HOST Host Frame Number */
  __I  USB_HOST_FLENHIGH_Type    FLENHIGH;    /**< \brief Offset: 0x012 (R/   8) HOST Host Frame Length */
       RoReg8                    Reserved5[0x1];
  __IO USB_HOST_INTENCLR_Type    INTENCLR;    /**< \brief Offset: 0x014 (R/W 16) HOST Host Interrupt Enable Clear */
       RoReg8                    Reserved6[0x2];
  __IO USB_HOST_INTENSET_Type    INTENSET;    /**< \brief Offset: 0x018 (R/W 16) HOST Host Interrupt Enable Set */
       RoReg8                    Reserved7[0x2];
  __IO USB_HOST_INTFLAG_Type     INTFLAG;     /**< \brief Offset: 0x01C (R/W 16) HOST Host Interrupt Flag */
       RoReg8                    Reserved8[0x2];
  __I  USB_HOST_PINTSMRY_Type    PINTSMRY;    /**< \brief Offset: 0x020 (R/  16) HOST Pipe Interrupt Summary */
       RoReg8                    Reserved9[0x2];
  __IO USB_DESCADD_Type          DESCADD;     /**< \brief Offset: 0x024 (R/W 32) Descriptor Address */
  __IO USB_PADCAL_Type           PADCAL;      /**< \brief Offset: 0x028 (R/W 16) USB PAD Calibration */
       RoReg8                    Reserved10[0xD6];
       UsbHostPipe               HostPipe[8]; /**< \brief Offset: 0x100 UsbHostPipe groups [EPT_NUM*HOST_IMPLEMENTED] */
} UsbHost;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief USB_DEVICE Descriptor SRAM registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct { /* USB is Device */
       UsbDeviceDescBank         DeviceDescBank[2]; /**< \brief Offset: 0x000 UsbDeviceDescBank groups */
} UsbDeviceDescriptor;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief USB_HOST Descriptor SRAM registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct { /* USB is Host */
       UsbHostDescBank           HostDescBank[2]; /**< \brief Offset: 0x000 UsbHostDescBank groups [2*HOST_IMPLEMENTED] */
} UsbHostDescriptor;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SECTION_USB_DESCRIPTOR

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
       UsbDevice                 DEVICE;      /**< \brief Offset: 0x000 USB is Device */
       UsbHost                   HOST;        /**< \brief Offset: 0x000 USB is Host */
} Usb;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD21_USB_COMPONENT_ */
