/**
 * \file
 *
 * \brief Component description for CAN
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

#ifndef _SAMD51_CAN_COMPONENT_
#define _SAMD51_CAN_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR CAN */
/* ========================================================================== */
/** \addtogroup SAMD51_CAN Control Area Network */
/*@{*/

#define CAN_U2003
#define REV_CAN                     0x321

/* -------- CAN_CREL : (CAN Offset: 0x00) (R/  32) Core Release -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :20;              /*!< bit:  0..19  Reserved                           */
    uint32_t SUBSTEP:4;        /*!< bit: 20..23  Sub-step of Core Release           */
    uint32_t STEP:4;           /*!< bit: 24..27  Step of Core Release               */
    uint32_t REL:4;            /*!< bit: 28..31  Core Release                       */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_CREL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_CREL_OFFSET             0x00         /**< \brief (CAN_CREL offset) Core Release */
#define CAN_CREL_RESETVALUE         0x32100000u  /**< \brief (CAN_CREL reset_value) Core Release */

#define CAN_CREL_SUBSTEP_Pos        20           /**< \brief (CAN_CREL) Sub-step of Core Release */
#define CAN_CREL_SUBSTEP_Msk        (0xFu << CAN_CREL_SUBSTEP_Pos)
#define CAN_CREL_SUBSTEP(value)     (CAN_CREL_SUBSTEP_Msk & ((value) << CAN_CREL_SUBSTEP_Pos))
#define CAN_CREL_STEP_Pos           24           /**< \brief (CAN_CREL) Step of Core Release */
#define CAN_CREL_STEP_Msk           (0xFu << CAN_CREL_STEP_Pos)
#define CAN_CREL_STEP(value)        (CAN_CREL_STEP_Msk & ((value) << CAN_CREL_STEP_Pos))
#define CAN_CREL_REL_Pos            28           /**< \brief (CAN_CREL) Core Release */
#define CAN_CREL_REL_Msk            (0xFu << CAN_CREL_REL_Pos)
#define CAN_CREL_REL(value)         (CAN_CREL_REL_Msk & ((value) << CAN_CREL_REL_Pos))
#define CAN_CREL_MASK               0xFFF00000u  /**< \brief (CAN_CREL) MASK Register */

/* -------- CAN_ENDN : (CAN Offset: 0x04) (R/  32) Endian -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ETV:32;           /*!< bit:  0..31  Endianness Test Value              */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_ENDN_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_ENDN_OFFSET             0x04         /**< \brief (CAN_ENDN offset) Endian */
#define CAN_ENDN_RESETVALUE         0x87654321u  /**< \brief (CAN_ENDN reset_value) Endian */

#define CAN_ENDN_ETV_Pos            0            /**< \brief (CAN_ENDN) Endianness Test Value */
#define CAN_ENDN_ETV_Msk            (0xFFFFFFFFu << CAN_ENDN_ETV_Pos)
#define CAN_ENDN_ETV(value)         (CAN_ENDN_ETV_Msk & ((value) << CAN_ENDN_ETV_Pos))
#define CAN_ENDN_MASK               0xFFFFFFFFu  /**< \brief (CAN_ENDN) MASK Register */

/* -------- CAN_MRCFG : (CAN Offset: 0x08) (R/W 32) Message RAM Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t QOS:2;            /*!< bit:  0.. 1  Quality of Service                 */
    uint32_t :30;              /*!< bit:  2..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_MRCFG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_MRCFG_OFFSET            0x08         /**< \brief (CAN_MRCFG offset) Message RAM Configuration */
#define CAN_MRCFG_RESETVALUE        0x00000002u  /**< \brief (CAN_MRCFG reset_value) Message RAM Configuration */

#define CAN_MRCFG_QOS_Pos           0            /**< \brief (CAN_MRCFG) Quality of Service */
#define CAN_MRCFG_QOS_Msk           (0x3u << CAN_MRCFG_QOS_Pos)
#define CAN_MRCFG_QOS(value)        (CAN_MRCFG_QOS_Msk & ((value) << CAN_MRCFG_QOS_Pos))
#define   CAN_MRCFG_QOS_DISABLE_Val       0x0u   /**< \brief (CAN_MRCFG) Background (no sensitive operation) */
#define   CAN_MRCFG_QOS_LOW_Val           0x1u   /**< \brief (CAN_MRCFG) Sensitive Bandwidth */
#define   CAN_MRCFG_QOS_MEDIUM_Val        0x2u   /**< \brief (CAN_MRCFG) Sensitive Latency */
#define   CAN_MRCFG_QOS_HIGH_Val          0x3u   /**< \brief (CAN_MRCFG) Critical Latency */
#define CAN_MRCFG_QOS_DISABLE       (CAN_MRCFG_QOS_DISABLE_Val     << CAN_MRCFG_QOS_Pos)
#define CAN_MRCFG_QOS_LOW           (CAN_MRCFG_QOS_LOW_Val         << CAN_MRCFG_QOS_Pos)
#define CAN_MRCFG_QOS_MEDIUM        (CAN_MRCFG_QOS_MEDIUM_Val      << CAN_MRCFG_QOS_Pos)
#define CAN_MRCFG_QOS_HIGH          (CAN_MRCFG_QOS_HIGH_Val        << CAN_MRCFG_QOS_Pos)
#define CAN_MRCFG_MASK              0x00000003u  /**< \brief (CAN_MRCFG) MASK Register */

/* -------- CAN_DBTP : (CAN Offset: 0x0C) (R/W 32) Fast Bit Timing and Prescaler -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DSJW:4;           /*!< bit:  0.. 3  Data (Re)Synchronization Jump Width */
    uint32_t DTSEG2:4;         /*!< bit:  4.. 7  Data time segment after sample point */
    uint32_t DTSEG1:5;         /*!< bit:  8..12  Data time segment before sample point */
    uint32_t :3;               /*!< bit: 13..15  Reserved                           */
    uint32_t DBRP:5;           /*!< bit: 16..20  Data Baud Rate Prescaler           */
    uint32_t :2;               /*!< bit: 21..22  Reserved                           */
    uint32_t TDC:1;            /*!< bit:     23  Tranceiver Delay Compensation      */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_DBTP_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_DBTP_OFFSET             0x0C         /**< \brief (CAN_DBTP offset) Fast Bit Timing and Prescaler */
#define CAN_DBTP_RESETVALUE         0x00000A33u  /**< \brief (CAN_DBTP reset_value) Fast Bit Timing and Prescaler */

#define CAN_DBTP_DSJW_Pos           0            /**< \brief (CAN_DBTP) Data (Re)Synchronization Jump Width */
#define CAN_DBTP_DSJW_Msk           (0xFu << CAN_DBTP_DSJW_Pos)
#define CAN_DBTP_DSJW(value)        (CAN_DBTP_DSJW_Msk & ((value) << CAN_DBTP_DSJW_Pos))
#define CAN_DBTP_DTSEG2_Pos         4            /**< \brief (CAN_DBTP) Data time segment after sample point */
#define CAN_DBTP_DTSEG2_Msk         (0xFu << CAN_DBTP_DTSEG2_Pos)
#define CAN_DBTP_DTSEG2(value)      (CAN_DBTP_DTSEG2_Msk & ((value) << CAN_DBTP_DTSEG2_Pos))
#define CAN_DBTP_DTSEG1_Pos         8            /**< \brief (CAN_DBTP) Data time segment before sample point */
#define CAN_DBTP_DTSEG1_Msk         (0x1Fu << CAN_DBTP_DTSEG1_Pos)
#define CAN_DBTP_DTSEG1(value)      (CAN_DBTP_DTSEG1_Msk & ((value) << CAN_DBTP_DTSEG1_Pos))
#define CAN_DBTP_DBRP_Pos           16           /**< \brief (CAN_DBTP) Data Baud Rate Prescaler */
#define CAN_DBTP_DBRP_Msk           (0x1Fu << CAN_DBTP_DBRP_Pos)
#define CAN_DBTP_DBRP(value)        (CAN_DBTP_DBRP_Msk & ((value) << CAN_DBTP_DBRP_Pos))
#define CAN_DBTP_TDC_Pos            23           /**< \brief (CAN_DBTP) Tranceiver Delay Compensation */
#define CAN_DBTP_TDC                (0x1u << CAN_DBTP_TDC_Pos)
#define CAN_DBTP_MASK               0x009F1FFFu  /**< \brief (CAN_DBTP) MASK Register */

/* -------- CAN_TEST : (CAN Offset: 0x10) (R/W 32) Test -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :4;               /*!< bit:  0.. 3  Reserved                           */
    uint32_t LBCK:1;           /*!< bit:      4  Loop Back Mode                     */
    uint32_t TX:2;             /*!< bit:  5.. 6  Control of Transmit Pin            */
    uint32_t RX:1;             /*!< bit:      7  Receive Pin                        */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TEST_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TEST_OFFSET             0x10         /**< \brief (CAN_TEST offset) Test */
#define CAN_TEST_RESETVALUE         0x00000000u  /**< \brief (CAN_TEST reset_value) Test */

#define CAN_TEST_LBCK_Pos           4            /**< \brief (CAN_TEST) Loop Back Mode */
#define CAN_TEST_LBCK               (0x1u << CAN_TEST_LBCK_Pos)
#define CAN_TEST_TX_Pos             5            /**< \brief (CAN_TEST) Control of Transmit Pin */
#define CAN_TEST_TX_Msk             (0x3u << CAN_TEST_TX_Pos)
#define CAN_TEST_TX(value)          (CAN_TEST_TX_Msk & ((value) << CAN_TEST_TX_Pos))
#define   CAN_TEST_TX_CORE_Val            0x0u   /**< \brief (CAN_TEST) TX controlled by CAN core */
#define   CAN_TEST_TX_SAMPLE_Val          0x1u   /**< \brief (CAN_TEST) TX monitoring sample point */
#define   CAN_TEST_TX_DOMINANT_Val        0x2u   /**< \brief (CAN_TEST) Dominant (0) level at pin CAN_TX */
#define   CAN_TEST_TX_RECESSIVE_Val       0x3u   /**< \brief (CAN_TEST) Recessive (1) level at pin CAN_TX */
#define CAN_TEST_TX_CORE            (CAN_TEST_TX_CORE_Val          << CAN_TEST_TX_Pos)
#define CAN_TEST_TX_SAMPLE          (CAN_TEST_TX_SAMPLE_Val        << CAN_TEST_TX_Pos)
#define CAN_TEST_TX_DOMINANT        (CAN_TEST_TX_DOMINANT_Val      << CAN_TEST_TX_Pos)
#define CAN_TEST_TX_RECESSIVE       (CAN_TEST_TX_RECESSIVE_Val     << CAN_TEST_TX_Pos)
#define CAN_TEST_RX_Pos             7            /**< \brief (CAN_TEST) Receive Pin */
#define CAN_TEST_RX                 (0x1u << CAN_TEST_RX_Pos)
#define CAN_TEST_MASK               0x000000F0u  /**< \brief (CAN_TEST) MASK Register */

/* -------- CAN_RWD : (CAN Offset: 0x14) (R/W 32) RAM Watchdog -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t WDC:8;            /*!< bit:  0.. 7  Watchdog Configuration             */
    uint32_t WDV:8;            /*!< bit:  8..15  Watchdog Value                     */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_RWD_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_RWD_OFFSET              0x14         /**< \brief (CAN_RWD offset) RAM Watchdog */
#define CAN_RWD_RESETVALUE          0x00000000u  /**< \brief (CAN_RWD reset_value) RAM Watchdog */

#define CAN_RWD_WDC_Pos             0            /**< \brief (CAN_RWD) Watchdog Configuration */
#define CAN_RWD_WDC_Msk             (0xFFu << CAN_RWD_WDC_Pos)
#define CAN_RWD_WDC(value)          (CAN_RWD_WDC_Msk & ((value) << CAN_RWD_WDC_Pos))
#define CAN_RWD_WDV_Pos             8            /**< \brief (CAN_RWD) Watchdog Value */
#define CAN_RWD_WDV_Msk             (0xFFu << CAN_RWD_WDV_Pos)
#define CAN_RWD_WDV(value)          (CAN_RWD_WDV_Msk & ((value) << CAN_RWD_WDV_Pos))
#define CAN_RWD_MASK                0x0000FFFFu  /**< \brief (CAN_RWD) MASK Register */

/* -------- CAN_CCCR : (CAN Offset: 0x18) (R/W 32) CC Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t INIT:1;           /*!< bit:      0  Initialization                     */
    uint32_t CCE:1;            /*!< bit:      1  Configuration Change Enable        */
    uint32_t ASM:1;            /*!< bit:      2  ASM Restricted Operation Mode      */
    uint32_t CSA:1;            /*!< bit:      3  Clock Stop Acknowledge             */
    uint32_t CSR:1;            /*!< bit:      4  Clock Stop Request                 */
    uint32_t MON:1;            /*!< bit:      5  Bus Monitoring Mode                */
    uint32_t DAR:1;            /*!< bit:      6  Disable Automatic Retransmission   */
    uint32_t TEST:1;           /*!< bit:      7  Test Mode Enable                   */
    uint32_t FDOE:1;           /*!< bit:      8  FD Operation Enable                */
    uint32_t BRSE:1;           /*!< bit:      9  Bit Rate Switch Enable             */
    uint32_t :2;               /*!< bit: 10..11  Reserved                           */
    uint32_t PXHD:1;           /*!< bit:     12  Protocol Exception Handling Disable */
    uint32_t EFBI:1;           /*!< bit:     13  Edge Filtering during Bus Integration */
    uint32_t TXP:1;            /*!< bit:     14  Transmit Pause                     */
    uint32_t NISO:1;           /*!< bit:     15  Non ISO Operation                  */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_CCCR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_CCCR_OFFSET             0x18         /**< \brief (CAN_CCCR offset) CC Control */
#define CAN_CCCR_RESETVALUE         0x00000001u  /**< \brief (CAN_CCCR reset_value) CC Control */

#define CAN_CCCR_INIT_Pos           0            /**< \brief (CAN_CCCR) Initialization */
#define CAN_CCCR_INIT               (0x1u << CAN_CCCR_INIT_Pos)
#define CAN_CCCR_CCE_Pos            1            /**< \brief (CAN_CCCR) Configuration Change Enable */
#define CAN_CCCR_CCE                (0x1u << CAN_CCCR_CCE_Pos)
#define CAN_CCCR_ASM_Pos            2            /**< \brief (CAN_CCCR) ASM Restricted Operation Mode */
#define CAN_CCCR_ASM                (0x1u << CAN_CCCR_ASM_Pos)
#define CAN_CCCR_CSA_Pos            3            /**< \brief (CAN_CCCR) Clock Stop Acknowledge */
#define CAN_CCCR_CSA                (0x1u << CAN_CCCR_CSA_Pos)
#define CAN_CCCR_CSR_Pos            4            /**< \brief (CAN_CCCR) Clock Stop Request */
#define CAN_CCCR_CSR                (0x1u << CAN_CCCR_CSR_Pos)
#define CAN_CCCR_MON_Pos            5            /**< \brief (CAN_CCCR) Bus Monitoring Mode */
#define CAN_CCCR_MON                (0x1u << CAN_CCCR_MON_Pos)
#define CAN_CCCR_DAR_Pos            6            /**< \brief (CAN_CCCR) Disable Automatic Retransmission */
#define CAN_CCCR_DAR                (0x1u << CAN_CCCR_DAR_Pos)
#define CAN_CCCR_TEST_Pos           7            /**< \brief (CAN_CCCR) Test Mode Enable */
#define CAN_CCCR_TEST               (0x1u << CAN_CCCR_TEST_Pos)
#define CAN_CCCR_FDOE_Pos           8            /**< \brief (CAN_CCCR) FD Operation Enable */
#define CAN_CCCR_FDOE               (0x1u << CAN_CCCR_FDOE_Pos)
#define CAN_CCCR_BRSE_Pos           9            /**< \brief (CAN_CCCR) Bit Rate Switch Enable */
#define CAN_CCCR_BRSE               (0x1u << CAN_CCCR_BRSE_Pos)
#define CAN_CCCR_PXHD_Pos           12           /**< \brief (CAN_CCCR) Protocol Exception Handling Disable */
#define CAN_CCCR_PXHD               (0x1u << CAN_CCCR_PXHD_Pos)
#define CAN_CCCR_EFBI_Pos           13           /**< \brief (CAN_CCCR) Edge Filtering during Bus Integration */
#define CAN_CCCR_EFBI               (0x1u << CAN_CCCR_EFBI_Pos)
#define CAN_CCCR_TXP_Pos            14           /**< \brief (CAN_CCCR) Transmit Pause */
#define CAN_CCCR_TXP                (0x1u << CAN_CCCR_TXP_Pos)
#define CAN_CCCR_NISO_Pos           15           /**< \brief (CAN_CCCR) Non ISO Operation */
#define CAN_CCCR_NISO               (0x1u << CAN_CCCR_NISO_Pos)
#define CAN_CCCR_MASK               0x0000F3FFu  /**< \brief (CAN_CCCR) MASK Register */

/* -------- CAN_NBTP : (CAN Offset: 0x1C) (R/W 32) Nominal Bit Timing and Prescaler -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t NTSEG2:7;         /*!< bit:  0.. 6  Nominal Time segment after sample point */
    uint32_t :1;               /*!< bit:      7  Reserved                           */
    uint32_t NTSEG1:8;         /*!< bit:  8..15  Nominal Time segment before sample point */
    uint32_t NBRP:9;           /*!< bit: 16..24  Nominal Baud Rate Prescaler        */
    uint32_t NSJW:7;           /*!< bit: 25..31  Nominal (Re)Synchronization Jump Width */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_NBTP_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_NBTP_OFFSET             0x1C         /**< \brief (CAN_NBTP offset) Nominal Bit Timing and Prescaler */
#define CAN_NBTP_RESETVALUE         0x06000A03u  /**< \brief (CAN_NBTP reset_value) Nominal Bit Timing and Prescaler */

#define CAN_NBTP_NTSEG2_Pos         0            /**< \brief (CAN_NBTP) Nominal Time segment after sample point */
#define CAN_NBTP_NTSEG2_Msk         (0x7Fu << CAN_NBTP_NTSEG2_Pos)
#define CAN_NBTP_NTSEG2(value)      (CAN_NBTP_NTSEG2_Msk & ((value) << CAN_NBTP_NTSEG2_Pos))
#define CAN_NBTP_NTSEG1_Pos         8            /**< \brief (CAN_NBTP) Nominal Time segment before sample point */
#define CAN_NBTP_NTSEG1_Msk         (0xFFu << CAN_NBTP_NTSEG1_Pos)
#define CAN_NBTP_NTSEG1(value)      (CAN_NBTP_NTSEG1_Msk & ((value) << CAN_NBTP_NTSEG1_Pos))
#define CAN_NBTP_NBRP_Pos           16           /**< \brief (CAN_NBTP) Nominal Baud Rate Prescaler */
#define CAN_NBTP_NBRP_Msk           (0x1FFu << CAN_NBTP_NBRP_Pos)
#define CAN_NBTP_NBRP(value)        (CAN_NBTP_NBRP_Msk & ((value) << CAN_NBTP_NBRP_Pos))
#define CAN_NBTP_NSJW_Pos           25           /**< \brief (CAN_NBTP) Nominal (Re)Synchronization Jump Width */
#define CAN_NBTP_NSJW_Msk           (0x7Fu << CAN_NBTP_NSJW_Pos)
#define CAN_NBTP_NSJW(value)        (CAN_NBTP_NSJW_Msk & ((value) << CAN_NBTP_NSJW_Pos))
#define CAN_NBTP_MASK               0xFFFFFF7Fu  /**< \brief (CAN_NBTP) MASK Register */

/* -------- CAN_TSCC : (CAN Offset: 0x20) (R/W 32) Timestamp Counter Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t TSS:2;            /*!< bit:  0.. 1  Timestamp Select                   */
    uint32_t :14;              /*!< bit:  2..15  Reserved                           */
    uint32_t TCP:4;            /*!< bit: 16..19  Timestamp Counter Prescaler        */
    uint32_t :12;              /*!< bit: 20..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TSCC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TSCC_OFFSET             0x20         /**< \brief (CAN_TSCC offset) Timestamp Counter Configuration */
#define CAN_TSCC_RESETVALUE         0x00000000u  /**< \brief (CAN_TSCC reset_value) Timestamp Counter Configuration */

#define CAN_TSCC_TSS_Pos            0            /**< \brief (CAN_TSCC) Timestamp Select */
#define CAN_TSCC_TSS_Msk            (0x3u << CAN_TSCC_TSS_Pos)
#define CAN_TSCC_TSS(value)         (CAN_TSCC_TSS_Msk & ((value) << CAN_TSCC_TSS_Pos))
#define   CAN_TSCC_TSS_ZERO_Val           0x0u   /**< \brief (CAN_TSCC) Timestamp counter value always 0x0000 */
#define   CAN_TSCC_TSS_INC_Val            0x1u   /**< \brief (CAN_TSCC) Timestamp counter value incremented by TCP */
#define   CAN_TSCC_TSS_EXT_Val            0x2u   /**< \brief (CAN_TSCC) External timestamp counter value used */
#define CAN_TSCC_TSS_ZERO           (CAN_TSCC_TSS_ZERO_Val         << CAN_TSCC_TSS_Pos)
#define CAN_TSCC_TSS_INC            (CAN_TSCC_TSS_INC_Val          << CAN_TSCC_TSS_Pos)
#define CAN_TSCC_TSS_EXT            (CAN_TSCC_TSS_EXT_Val          << CAN_TSCC_TSS_Pos)
#define CAN_TSCC_TCP_Pos            16           /**< \brief (CAN_TSCC) Timestamp Counter Prescaler */
#define CAN_TSCC_TCP_Msk            (0xFu << CAN_TSCC_TCP_Pos)
#define CAN_TSCC_TCP(value)         (CAN_TSCC_TCP_Msk & ((value) << CAN_TSCC_TCP_Pos))
#define CAN_TSCC_MASK               0x000F0003u  /**< \brief (CAN_TSCC) MASK Register */

/* -------- CAN_TSCV : (CAN Offset: 0x24) (R/  32) Timestamp Counter Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t TSC:16;           /*!< bit:  0..15  Timestamp Counter                  */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TSCV_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TSCV_OFFSET             0x24         /**< \brief (CAN_TSCV offset) Timestamp Counter Value */
#define CAN_TSCV_RESETVALUE         0x00000000u  /**< \brief (CAN_TSCV reset_value) Timestamp Counter Value */

#define CAN_TSCV_TSC_Pos            0            /**< \brief (CAN_TSCV) Timestamp Counter */
#define CAN_TSCV_TSC_Msk            (0xFFFFu << CAN_TSCV_TSC_Pos)
#define CAN_TSCV_TSC(value)         (CAN_TSCV_TSC_Msk & ((value) << CAN_TSCV_TSC_Pos))
#define CAN_TSCV_MASK               0x0000FFFFu  /**< \brief (CAN_TSCV) MASK Register */

/* -------- CAN_TOCC : (CAN Offset: 0x28) (R/W 32) Timeout Counter Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ETOC:1;           /*!< bit:      0  Enable Timeout Counter             */
    uint32_t TOS:2;            /*!< bit:  1.. 2  Timeout Select                     */
    uint32_t :13;              /*!< bit:  3..15  Reserved                           */
    uint32_t TOP:16;           /*!< bit: 16..31  Timeout Period                     */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TOCC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TOCC_OFFSET             0x28         /**< \brief (CAN_TOCC offset) Timeout Counter Configuration */
#define CAN_TOCC_RESETVALUE         0xFFFF0000u  /**< \brief (CAN_TOCC reset_value) Timeout Counter Configuration */

#define CAN_TOCC_ETOC_Pos           0            /**< \brief (CAN_TOCC) Enable Timeout Counter */
#define CAN_TOCC_ETOC               (0x1u << CAN_TOCC_ETOC_Pos)
#define CAN_TOCC_TOS_Pos            1            /**< \brief (CAN_TOCC) Timeout Select */
#define CAN_TOCC_TOS_Msk            (0x3u << CAN_TOCC_TOS_Pos)
#define CAN_TOCC_TOS(value)         (CAN_TOCC_TOS_Msk & ((value) << CAN_TOCC_TOS_Pos))
#define   CAN_TOCC_TOS_CONT_Val           0x0u   /**< \brief (CAN_TOCC) Continuout operation */
#define   CAN_TOCC_TOS_TXEF_Val           0x1u   /**< \brief (CAN_TOCC) Timeout controlled by TX Event FIFO */
#define   CAN_TOCC_TOS_RXF0_Val           0x2u   /**< \brief (CAN_TOCC) Timeout controlled by Rx FIFO 0 */
#define   CAN_TOCC_TOS_RXF1_Val           0x3u   /**< \brief (CAN_TOCC) Timeout controlled by Rx FIFO 1 */
#define CAN_TOCC_TOS_CONT           (CAN_TOCC_TOS_CONT_Val         << CAN_TOCC_TOS_Pos)
#define CAN_TOCC_TOS_TXEF           (CAN_TOCC_TOS_TXEF_Val         << CAN_TOCC_TOS_Pos)
#define CAN_TOCC_TOS_RXF0           (CAN_TOCC_TOS_RXF0_Val         << CAN_TOCC_TOS_Pos)
#define CAN_TOCC_TOS_RXF1           (CAN_TOCC_TOS_RXF1_Val         << CAN_TOCC_TOS_Pos)
#define CAN_TOCC_TOP_Pos            16           /**< \brief (CAN_TOCC) Timeout Period */
#define CAN_TOCC_TOP_Msk            (0xFFFFu << CAN_TOCC_TOP_Pos)
#define CAN_TOCC_TOP(value)         (CAN_TOCC_TOP_Msk & ((value) << CAN_TOCC_TOP_Pos))
#define CAN_TOCC_MASK               0xFFFF0007u  /**< \brief (CAN_TOCC) MASK Register */

/* -------- CAN_TOCV : (CAN Offset: 0x2C) (R/W 32) Timeout Counter Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t TOC:16;           /*!< bit:  0..15  Timeout Counter                    */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TOCV_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TOCV_OFFSET             0x2C         /**< \brief (CAN_TOCV offset) Timeout Counter Value */
#define CAN_TOCV_RESETVALUE         0x0000FFFFu  /**< \brief (CAN_TOCV reset_value) Timeout Counter Value */

#define CAN_TOCV_TOC_Pos            0            /**< \brief (CAN_TOCV) Timeout Counter */
#define CAN_TOCV_TOC_Msk            (0xFFFFu << CAN_TOCV_TOC_Pos)
#define CAN_TOCV_TOC(value)         (CAN_TOCV_TOC_Msk & ((value) << CAN_TOCV_TOC_Pos))
#define CAN_TOCV_MASK               0x0000FFFFu  /**< \brief (CAN_TOCV) MASK Register */

/* -------- CAN_ECR : (CAN Offset: 0x40) (R/  32) Error Counter -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t TEC:8;            /*!< bit:  0.. 7  Transmit Error Counter             */
    uint32_t REC:7;            /*!< bit:  8..14  Receive Error Counter              */
    uint32_t RP:1;             /*!< bit:     15  Receive Error Passive              */
    uint32_t CEL:8;            /*!< bit: 16..23  CAN Error Logging                  */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_ECR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_ECR_OFFSET              0x40         /**< \brief (CAN_ECR offset) Error Counter */
#define CAN_ECR_RESETVALUE          0x00000000u  /**< \brief (CAN_ECR reset_value) Error Counter */

#define CAN_ECR_TEC_Pos             0            /**< \brief (CAN_ECR) Transmit Error Counter */
#define CAN_ECR_TEC_Msk             (0xFFu << CAN_ECR_TEC_Pos)
#define CAN_ECR_TEC(value)          (CAN_ECR_TEC_Msk & ((value) << CAN_ECR_TEC_Pos))
#define CAN_ECR_REC_Pos             8            /**< \brief (CAN_ECR) Receive Error Counter */
#define CAN_ECR_REC_Msk             (0x7Fu << CAN_ECR_REC_Pos)
#define CAN_ECR_REC(value)          (CAN_ECR_REC_Msk & ((value) << CAN_ECR_REC_Pos))
#define CAN_ECR_RP_Pos              15           /**< \brief (CAN_ECR) Receive Error Passive */
#define CAN_ECR_RP                  (0x1u << CAN_ECR_RP_Pos)
#define CAN_ECR_CEL_Pos             16           /**< \brief (CAN_ECR) CAN Error Logging */
#define CAN_ECR_CEL_Msk             (0xFFu << CAN_ECR_CEL_Pos)
#define CAN_ECR_CEL(value)          (CAN_ECR_CEL_Msk & ((value) << CAN_ECR_CEL_Pos))
#define CAN_ECR_MASK                0x00FFFFFFu  /**< \brief (CAN_ECR) MASK Register */

/* -------- CAN_PSR : (CAN Offset: 0x44) (R/  32) Protocol Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t LEC:3;            /*!< bit:  0.. 2  Last Error Code                    */
    uint32_t ACT:2;            /*!< bit:  3.. 4  Activity                           */
    uint32_t EP:1;             /*!< bit:      5  Error Passive                      */
    uint32_t EW:1;             /*!< bit:      6  Warning Status                     */
    uint32_t BO:1;             /*!< bit:      7  Bus_Off Status                     */
    uint32_t DLEC:3;           /*!< bit:  8..10  Data Phase Last Error Code         */
    uint32_t RESI:1;           /*!< bit:     11  ESI flag of last received CAN FD Message */
    uint32_t RBRS:1;           /*!< bit:     12  BRS flag of last received CAN FD Message */
    uint32_t RFDF:1;           /*!< bit:     13  Received a CAN FD Message          */
    uint32_t PXE:1;            /*!< bit:     14  Protocol Exception Event           */
    uint32_t :1;               /*!< bit:     15  Reserved                           */
    uint32_t TDCV:7;           /*!< bit: 16..22  Transmitter Delay Compensation Value */
    uint32_t :9;               /*!< bit: 23..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_PSR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_PSR_OFFSET              0x44         /**< \brief (CAN_PSR offset) Protocol Status */
#define CAN_PSR_RESETVALUE          0x00000707u  /**< \brief (CAN_PSR reset_value) Protocol Status */

#define CAN_PSR_LEC_Pos             0            /**< \brief (CAN_PSR) Last Error Code */
#define CAN_PSR_LEC_Msk             (0x7u << CAN_PSR_LEC_Pos)
#define CAN_PSR_LEC(value)          (CAN_PSR_LEC_Msk & ((value) << CAN_PSR_LEC_Pos))
#define   CAN_PSR_LEC_NONE_Val            0x0u   /**< \brief (CAN_PSR) No Error */
#define   CAN_PSR_LEC_STUFF_Val           0x1u   /**< \brief (CAN_PSR) Stuff Error */
#define   CAN_PSR_LEC_FORM_Val            0x2u   /**< \brief (CAN_PSR) Form Error */
#define   CAN_PSR_LEC_ACK_Val             0x3u   /**< \brief (CAN_PSR) Ack Error */
#define   CAN_PSR_LEC_BIT1_Val            0x4u   /**< \brief (CAN_PSR) Bit1 Error */
#define   CAN_PSR_LEC_BIT0_Val            0x5u   /**< \brief (CAN_PSR) Bit0 Error */
#define   CAN_PSR_LEC_CRC_Val             0x6u   /**< \brief (CAN_PSR) CRC Error */
#define   CAN_PSR_LEC_NC_Val              0x7u   /**< \brief (CAN_PSR) No Change */
#define CAN_PSR_LEC_NONE            (CAN_PSR_LEC_NONE_Val          << CAN_PSR_LEC_Pos)
#define CAN_PSR_LEC_STUFF           (CAN_PSR_LEC_STUFF_Val         << CAN_PSR_LEC_Pos)
#define CAN_PSR_LEC_FORM            (CAN_PSR_LEC_FORM_Val          << CAN_PSR_LEC_Pos)
#define CAN_PSR_LEC_ACK             (CAN_PSR_LEC_ACK_Val           << CAN_PSR_LEC_Pos)
#define CAN_PSR_LEC_BIT1            (CAN_PSR_LEC_BIT1_Val          << CAN_PSR_LEC_Pos)
#define CAN_PSR_LEC_BIT0            (CAN_PSR_LEC_BIT0_Val          << CAN_PSR_LEC_Pos)
#define CAN_PSR_LEC_CRC             (CAN_PSR_LEC_CRC_Val           << CAN_PSR_LEC_Pos)
#define CAN_PSR_LEC_NC              (CAN_PSR_LEC_NC_Val            << CAN_PSR_LEC_Pos)
#define CAN_PSR_ACT_Pos             3            /**< \brief (CAN_PSR) Activity */
#define CAN_PSR_ACT_Msk             (0x3u << CAN_PSR_ACT_Pos)
#define CAN_PSR_ACT(value)          (CAN_PSR_ACT_Msk & ((value) << CAN_PSR_ACT_Pos))
#define   CAN_PSR_ACT_SYNC_Val            0x0u   /**< \brief (CAN_PSR) Node is synchronizing on CAN communication */
#define   CAN_PSR_ACT_IDLE_Val            0x1u   /**< \brief (CAN_PSR) Node is neither receiver nor transmitter */
#define   CAN_PSR_ACT_RX_Val              0x2u   /**< \brief (CAN_PSR) Node is operating as receiver */
#define   CAN_PSR_ACT_TX_Val              0x3u   /**< \brief (CAN_PSR) Node is operating as transmitter */
#define CAN_PSR_ACT_SYNC            (CAN_PSR_ACT_SYNC_Val          << CAN_PSR_ACT_Pos)
#define CAN_PSR_ACT_IDLE            (CAN_PSR_ACT_IDLE_Val          << CAN_PSR_ACT_Pos)
#define CAN_PSR_ACT_RX              (CAN_PSR_ACT_RX_Val            << CAN_PSR_ACT_Pos)
#define CAN_PSR_ACT_TX              (CAN_PSR_ACT_TX_Val            << CAN_PSR_ACT_Pos)
#define CAN_PSR_EP_Pos              5            /**< \brief (CAN_PSR) Error Passive */
#define CAN_PSR_EP                  (0x1u << CAN_PSR_EP_Pos)
#define CAN_PSR_EW_Pos              6            /**< \brief (CAN_PSR) Warning Status */
#define CAN_PSR_EW                  (0x1u << CAN_PSR_EW_Pos)
#define CAN_PSR_BO_Pos              7            /**< \brief (CAN_PSR) Bus_Off Status */
#define CAN_PSR_BO                  (0x1u << CAN_PSR_BO_Pos)
#define CAN_PSR_DLEC_Pos            8            /**< \brief (CAN_PSR) Data Phase Last Error Code */
#define CAN_PSR_DLEC_Msk            (0x7u << CAN_PSR_DLEC_Pos)
#define CAN_PSR_DLEC(value)         (CAN_PSR_DLEC_Msk & ((value) << CAN_PSR_DLEC_Pos))
#define   CAN_PSR_DLEC_NONE_Val           0x0u   /**< \brief (CAN_PSR) No Error */
#define   CAN_PSR_DLEC_STUFF_Val          0x1u   /**< \brief (CAN_PSR) Stuff Error */
#define   CAN_PSR_DLEC_FORM_Val           0x2u   /**< \brief (CAN_PSR) Form Error */
#define   CAN_PSR_DLEC_ACK_Val            0x3u   /**< \brief (CAN_PSR) Ack Error */
#define   CAN_PSR_DLEC_BIT1_Val           0x4u   /**< \brief (CAN_PSR) Bit1 Error */
#define   CAN_PSR_DLEC_BIT0_Val           0x5u   /**< \brief (CAN_PSR) Bit0 Error */
#define   CAN_PSR_DLEC_CRC_Val            0x6u   /**< \brief (CAN_PSR) CRC Error */
#define   CAN_PSR_DLEC_NC_Val             0x7u   /**< \brief (CAN_PSR) No Change */
#define CAN_PSR_DLEC_NONE           (CAN_PSR_DLEC_NONE_Val         << CAN_PSR_DLEC_Pos)
#define CAN_PSR_DLEC_STUFF          (CAN_PSR_DLEC_STUFF_Val        << CAN_PSR_DLEC_Pos)
#define CAN_PSR_DLEC_FORM           (CAN_PSR_DLEC_FORM_Val         << CAN_PSR_DLEC_Pos)
#define CAN_PSR_DLEC_ACK            (CAN_PSR_DLEC_ACK_Val          << CAN_PSR_DLEC_Pos)
#define CAN_PSR_DLEC_BIT1           (CAN_PSR_DLEC_BIT1_Val         << CAN_PSR_DLEC_Pos)
#define CAN_PSR_DLEC_BIT0           (CAN_PSR_DLEC_BIT0_Val         << CAN_PSR_DLEC_Pos)
#define CAN_PSR_DLEC_CRC            (CAN_PSR_DLEC_CRC_Val          << CAN_PSR_DLEC_Pos)
#define CAN_PSR_DLEC_NC             (CAN_PSR_DLEC_NC_Val           << CAN_PSR_DLEC_Pos)
#define CAN_PSR_RESI_Pos            11           /**< \brief (CAN_PSR) ESI flag of last received CAN FD Message */
#define CAN_PSR_RESI                (0x1u << CAN_PSR_RESI_Pos)
#define CAN_PSR_RBRS_Pos            12           /**< \brief (CAN_PSR) BRS flag of last received CAN FD Message */
#define CAN_PSR_RBRS                (0x1u << CAN_PSR_RBRS_Pos)
#define CAN_PSR_RFDF_Pos            13           /**< \brief (CAN_PSR) Received a CAN FD Message */
#define CAN_PSR_RFDF                (0x1u << CAN_PSR_RFDF_Pos)
#define CAN_PSR_PXE_Pos             14           /**< \brief (CAN_PSR) Protocol Exception Event */
#define CAN_PSR_PXE                 (0x1u << CAN_PSR_PXE_Pos)
#define CAN_PSR_TDCV_Pos            16           /**< \brief (CAN_PSR) Transmitter Delay Compensation Value */
#define CAN_PSR_TDCV_Msk            (0x7Fu << CAN_PSR_TDCV_Pos)
#define CAN_PSR_TDCV(value)         (CAN_PSR_TDCV_Msk & ((value) << CAN_PSR_TDCV_Pos))
#define CAN_PSR_MASK                0x007F7FFFu  /**< \brief (CAN_PSR) MASK Register */

/* -------- CAN_TDCR : (CAN Offset: 0x48) (R/W 32) Extended ID Filter Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t TDCF:7;           /*!< bit:  0.. 6  Transmitter Delay Compensation Filter Length */
    uint32_t :1;               /*!< bit:      7  Reserved                           */
    uint32_t TDCO:7;           /*!< bit:  8..14  Transmitter Delay Compensation Offset */
    uint32_t :17;              /*!< bit: 15..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TDCR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TDCR_OFFSET             0x48         /**< \brief (CAN_TDCR offset) Extended ID Filter Configuration */
#define CAN_TDCR_RESETVALUE         0x00000000u  /**< \brief (CAN_TDCR reset_value) Extended ID Filter Configuration */

#define CAN_TDCR_TDCF_Pos           0            /**< \brief (CAN_TDCR) Transmitter Delay Compensation Filter Length */
#define CAN_TDCR_TDCF_Msk           (0x7Fu << CAN_TDCR_TDCF_Pos)
#define CAN_TDCR_TDCF(value)        (CAN_TDCR_TDCF_Msk & ((value) << CAN_TDCR_TDCF_Pos))
#define CAN_TDCR_TDCO_Pos           8            /**< \brief (CAN_TDCR) Transmitter Delay Compensation Offset */
#define CAN_TDCR_TDCO_Msk           (0x7Fu << CAN_TDCR_TDCO_Pos)
#define CAN_TDCR_TDCO(value)        (CAN_TDCR_TDCO_Msk & ((value) << CAN_TDCR_TDCO_Pos))
#define CAN_TDCR_MASK               0x00007F7Fu  /**< \brief (CAN_TDCR) MASK Register */

/* -------- CAN_IR : (CAN Offset: 0x50) (R/W 32) Interrupt -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t RF0N:1;           /*!< bit:      0  Rx FIFO 0 New Message              */
    uint32_t RF0W:1;           /*!< bit:      1  Rx FIFO 0 Watermark Reached        */
    uint32_t RF0F:1;           /*!< bit:      2  Rx FIFO 0 Full                     */
    uint32_t RF0L:1;           /*!< bit:      3  Rx FIFO 0 Message Lost             */
    uint32_t RF1N:1;           /*!< bit:      4  Rx FIFO 1 New Message              */
    uint32_t RF1W:1;           /*!< bit:      5  Rx FIFO 1 Watermark Reached        */
    uint32_t RF1F:1;           /*!< bit:      6  Rx FIFO 1 FIFO Full                */
    uint32_t RF1L:1;           /*!< bit:      7  Rx FIFO 1 Message Lost             */
    uint32_t HPM:1;            /*!< bit:      8  High Priority Message              */
    uint32_t TC:1;             /*!< bit:      9  Timestamp Completed                */
    uint32_t TCF:1;            /*!< bit:     10  Transmission Cancellation Finished */
    uint32_t TFE:1;            /*!< bit:     11  Tx FIFO Empty                      */
    uint32_t TEFN:1;           /*!< bit:     12  Tx Event FIFO New Entry            */
    uint32_t TEFW:1;           /*!< bit:     13  Tx Event FIFO Watermark Reached    */
    uint32_t TEFF:1;           /*!< bit:     14  Tx Event FIFO Full                 */
    uint32_t TEFL:1;           /*!< bit:     15  Tx Event FIFO Element Lost         */
    uint32_t TSW:1;            /*!< bit:     16  Timestamp Wraparound               */
    uint32_t MRAF:1;           /*!< bit:     17  Message RAM Access Failure         */
    uint32_t TOO:1;            /*!< bit:     18  Timeout Occurred                   */
    uint32_t DRX:1;            /*!< bit:     19  Message stored to Dedicated Rx Buffer */
    uint32_t BEC:1;            /*!< bit:     20  Bit Error Corrected                */
    uint32_t BEU:1;            /*!< bit:     21  Bit Error Uncorrected              */
    uint32_t ELO:1;            /*!< bit:     22  Error Logging Overflow             */
    uint32_t EP:1;             /*!< bit:     23  Error Passive                      */
    uint32_t EW:1;             /*!< bit:     24  Warning Status                     */
    uint32_t BO:1;             /*!< bit:     25  Bus_Off Status                     */
    uint32_t WDI:1;            /*!< bit:     26  Watchdog Interrupt                 */
    uint32_t PEA:1;            /*!< bit:     27  Protocol Error in Arbitration Phase */
    uint32_t PED:1;            /*!< bit:     28  Protocol Error in Data Phase       */
    uint32_t ARA:1;            /*!< bit:     29  Access to Reserved Address         */
    uint32_t :2;               /*!< bit: 30..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_IR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_IR_OFFSET               0x50         /**< \brief (CAN_IR offset) Interrupt */
#define CAN_IR_RESETVALUE           0x00000000u  /**< \brief (CAN_IR reset_value) Interrupt */

#define CAN_IR_RF0N_Pos             0            /**< \brief (CAN_IR) Rx FIFO 0 New Message */
#define CAN_IR_RF0N                 (0x1u << CAN_IR_RF0N_Pos)
#define CAN_IR_RF0W_Pos             1            /**< \brief (CAN_IR) Rx FIFO 0 Watermark Reached */
#define CAN_IR_RF0W                 (0x1u << CAN_IR_RF0W_Pos)
#define CAN_IR_RF0F_Pos             2            /**< \brief (CAN_IR) Rx FIFO 0 Full */
#define CAN_IR_RF0F                 (0x1u << CAN_IR_RF0F_Pos)
#define CAN_IR_RF0L_Pos             3            /**< \brief (CAN_IR) Rx FIFO 0 Message Lost */
#define CAN_IR_RF0L                 (0x1u << CAN_IR_RF0L_Pos)
#define CAN_IR_RF1N_Pos             4            /**< \brief (CAN_IR) Rx FIFO 1 New Message */
#define CAN_IR_RF1N                 (0x1u << CAN_IR_RF1N_Pos)
#define CAN_IR_RF1W_Pos             5            /**< \brief (CAN_IR) Rx FIFO 1 Watermark Reached */
#define CAN_IR_RF1W                 (0x1u << CAN_IR_RF1W_Pos)
#define CAN_IR_RF1F_Pos             6            /**< \brief (CAN_IR) Rx FIFO 1 FIFO Full */
#define CAN_IR_RF1F                 (0x1u << CAN_IR_RF1F_Pos)
#define CAN_IR_RF1L_Pos             7            /**< \brief (CAN_IR) Rx FIFO 1 Message Lost */
#define CAN_IR_RF1L                 (0x1u << CAN_IR_RF1L_Pos)
#define CAN_IR_HPM_Pos              8            /**< \brief (CAN_IR) High Priority Message */
#define CAN_IR_HPM                  (0x1u << CAN_IR_HPM_Pos)
#define CAN_IR_TC_Pos               9            /**< \brief (CAN_IR) Timestamp Completed */
#define CAN_IR_TC                   (0x1u << CAN_IR_TC_Pos)
#define CAN_IR_TCF_Pos              10           /**< \brief (CAN_IR) Transmission Cancellation Finished */
#define CAN_IR_TCF                  (0x1u << CAN_IR_TCF_Pos)
#define CAN_IR_TFE_Pos              11           /**< \brief (CAN_IR) Tx FIFO Empty */
#define CAN_IR_TFE                  (0x1u << CAN_IR_TFE_Pos)
#define CAN_IR_TEFN_Pos             12           /**< \brief (CAN_IR) Tx Event FIFO New Entry */
#define CAN_IR_TEFN                 (0x1u << CAN_IR_TEFN_Pos)
#define CAN_IR_TEFW_Pos             13           /**< \brief (CAN_IR) Tx Event FIFO Watermark Reached */
#define CAN_IR_TEFW                 (0x1u << CAN_IR_TEFW_Pos)
#define CAN_IR_TEFF_Pos             14           /**< \brief (CAN_IR) Tx Event FIFO Full */
#define CAN_IR_TEFF                 (0x1u << CAN_IR_TEFF_Pos)
#define CAN_IR_TEFL_Pos             15           /**< \brief (CAN_IR) Tx Event FIFO Element Lost */
#define CAN_IR_TEFL                 (0x1u << CAN_IR_TEFL_Pos)
#define CAN_IR_TSW_Pos              16           /**< \brief (CAN_IR) Timestamp Wraparound */
#define CAN_IR_TSW                  (0x1u << CAN_IR_TSW_Pos)
#define CAN_IR_MRAF_Pos             17           /**< \brief (CAN_IR) Message RAM Access Failure */
#define CAN_IR_MRAF                 (0x1u << CAN_IR_MRAF_Pos)
#define CAN_IR_TOO_Pos              18           /**< \brief (CAN_IR) Timeout Occurred */
#define CAN_IR_TOO                  (0x1u << CAN_IR_TOO_Pos)
#define CAN_IR_DRX_Pos              19           /**< \brief (CAN_IR) Message stored to Dedicated Rx Buffer */
#define CAN_IR_DRX                  (0x1u << CAN_IR_DRX_Pos)
#define CAN_IR_BEC_Pos              20           /**< \brief (CAN_IR) Bit Error Corrected */
#define CAN_IR_BEC                  (0x1u << CAN_IR_BEC_Pos)
#define CAN_IR_BEU_Pos              21           /**< \brief (CAN_IR) Bit Error Uncorrected */
#define CAN_IR_BEU                  (0x1u << CAN_IR_BEU_Pos)
#define CAN_IR_ELO_Pos              22           /**< \brief (CAN_IR) Error Logging Overflow */
#define CAN_IR_ELO                  (0x1u << CAN_IR_ELO_Pos)
#define CAN_IR_EP_Pos               23           /**< \brief (CAN_IR) Error Passive */
#define CAN_IR_EP                   (0x1u << CAN_IR_EP_Pos)
#define CAN_IR_EW_Pos               24           /**< \brief (CAN_IR) Warning Status */
#define CAN_IR_EW                   (0x1u << CAN_IR_EW_Pos)
#define CAN_IR_BO_Pos               25           /**< \brief (CAN_IR) Bus_Off Status */
#define CAN_IR_BO                   (0x1u << CAN_IR_BO_Pos)
#define CAN_IR_WDI_Pos              26           /**< \brief (CAN_IR) Watchdog Interrupt */
#define CAN_IR_WDI                  (0x1u << CAN_IR_WDI_Pos)
#define CAN_IR_PEA_Pos              27           /**< \brief (CAN_IR) Protocol Error in Arbitration Phase */
#define CAN_IR_PEA                  (0x1u << CAN_IR_PEA_Pos)
#define CAN_IR_PED_Pos              28           /**< \brief (CAN_IR) Protocol Error in Data Phase */
#define CAN_IR_PED                  (0x1u << CAN_IR_PED_Pos)
#define CAN_IR_ARA_Pos              29           /**< \brief (CAN_IR) Access to Reserved Address */
#define CAN_IR_ARA                  (0x1u << CAN_IR_ARA_Pos)
#define CAN_IR_MASK                 0x3FFFFFFFu  /**< \brief (CAN_IR) MASK Register */

/* -------- CAN_IE : (CAN Offset: 0x54) (R/W 32) Interrupt Enable -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t RF0NE:1;          /*!< bit:      0  Rx FIFO 0 New Message Interrupt Enable */
    uint32_t RF0WE:1;          /*!< bit:      1  Rx FIFO 0 Watermark Reached Interrupt Enable */
    uint32_t RF0FE:1;          /*!< bit:      2  Rx FIFO 0 Full Interrupt Enable    */
    uint32_t RF0LE:1;          /*!< bit:      3  Rx FIFO 0 Message Lost Interrupt Enable */
    uint32_t RF1NE:1;          /*!< bit:      4  Rx FIFO 1 New Message Interrupt Enable */
    uint32_t RF1WE:1;          /*!< bit:      5  Rx FIFO 1 Watermark Reached Interrupt Enable */
    uint32_t RF1FE:1;          /*!< bit:      6  Rx FIFO 1 FIFO Full Interrupt Enable */
    uint32_t RF1LE:1;          /*!< bit:      7  Rx FIFO 1 Message Lost Interrupt Enable */
    uint32_t HPME:1;           /*!< bit:      8  High Priority Message Interrupt Enable */
    uint32_t TCE:1;            /*!< bit:      9  Timestamp Completed Interrupt Enable */
    uint32_t TCFE:1;           /*!< bit:     10  Transmission Cancellation Finished Interrupt Enable */
    uint32_t TFEE:1;           /*!< bit:     11  Tx FIFO Empty Interrupt Enable     */
    uint32_t TEFNE:1;          /*!< bit:     12  Tx Event FIFO New Entry Interrupt Enable */
    uint32_t TEFWE:1;          /*!< bit:     13  Tx Event FIFO Watermark Reached Interrupt Enable */
    uint32_t TEFFE:1;          /*!< bit:     14  Tx Event FIFO Full Interrupt Enable */
    uint32_t TEFLE:1;          /*!< bit:     15  Tx Event FIFO Element Lost Interrupt Enable */
    uint32_t TSWE:1;           /*!< bit:     16  Timestamp Wraparound Interrupt Enable */
    uint32_t MRAFE:1;          /*!< bit:     17  Message RAM Access Failure Interrupt Enable */
    uint32_t TOOE:1;           /*!< bit:     18  Timeout Occurred Interrupt Enable  */
    uint32_t DRXE:1;           /*!< bit:     19  Message stored to Dedicated Rx Buffer Interrupt Enable */
    uint32_t BECE:1;           /*!< bit:     20  Bit Error Corrected Interrupt Enable */
    uint32_t BEUE:1;           /*!< bit:     21  Bit Error Uncorrected Interrupt Enable */
    uint32_t ELOE:1;           /*!< bit:     22  Error Logging Overflow Interrupt Enable */
    uint32_t EPE:1;            /*!< bit:     23  Error Passive Interrupt Enable     */
    uint32_t EWE:1;            /*!< bit:     24  Warning Status Interrupt Enable    */
    uint32_t BOE:1;            /*!< bit:     25  Bus_Off Status Interrupt Enable    */
    uint32_t WDIE:1;           /*!< bit:     26  Watchdog Interrupt Interrupt Enable */
    uint32_t PEAE:1;           /*!< bit:     27  Protocol Error in Arbitration Phase Enable */
    uint32_t PEDE:1;           /*!< bit:     28  Protocol Error in Data Phase Enable */
    uint32_t ARAE:1;           /*!< bit:     29  Access to Reserved Address Enable  */
    uint32_t :2;               /*!< bit: 30..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_IE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_IE_OFFSET               0x54         /**< \brief (CAN_IE offset) Interrupt Enable */
#define CAN_IE_RESETVALUE           0x00000000u  /**< \brief (CAN_IE reset_value) Interrupt Enable */

#define CAN_IE_RF0NE_Pos            0            /**< \brief (CAN_IE) Rx FIFO 0 New Message Interrupt Enable */
#define CAN_IE_RF0NE                (0x1u << CAN_IE_RF0NE_Pos)
#define CAN_IE_RF0WE_Pos            1            /**< \brief (CAN_IE) Rx FIFO 0 Watermark Reached Interrupt Enable */
#define CAN_IE_RF0WE                (0x1u << CAN_IE_RF0WE_Pos)
#define CAN_IE_RF0FE_Pos            2            /**< \brief (CAN_IE) Rx FIFO 0 Full Interrupt Enable */
#define CAN_IE_RF0FE                (0x1u << CAN_IE_RF0FE_Pos)
#define CAN_IE_RF0LE_Pos            3            /**< \brief (CAN_IE) Rx FIFO 0 Message Lost Interrupt Enable */
#define CAN_IE_RF0LE                (0x1u << CAN_IE_RF0LE_Pos)
#define CAN_IE_RF1NE_Pos            4            /**< \brief (CAN_IE) Rx FIFO 1 New Message Interrupt Enable */
#define CAN_IE_RF1NE                (0x1u << CAN_IE_RF1NE_Pos)
#define CAN_IE_RF1WE_Pos            5            /**< \brief (CAN_IE) Rx FIFO 1 Watermark Reached Interrupt Enable */
#define CAN_IE_RF1WE                (0x1u << CAN_IE_RF1WE_Pos)
#define CAN_IE_RF1FE_Pos            6            /**< \brief (CAN_IE) Rx FIFO 1 FIFO Full Interrupt Enable */
#define CAN_IE_RF1FE                (0x1u << CAN_IE_RF1FE_Pos)
#define CAN_IE_RF1LE_Pos            7            /**< \brief (CAN_IE) Rx FIFO 1 Message Lost Interrupt Enable */
#define CAN_IE_RF1LE                (0x1u << CAN_IE_RF1LE_Pos)
#define CAN_IE_HPME_Pos             8            /**< \brief (CAN_IE) High Priority Message Interrupt Enable */
#define CAN_IE_HPME                 (0x1u << CAN_IE_HPME_Pos)
#define CAN_IE_TCE_Pos              9            /**< \brief (CAN_IE) Timestamp Completed Interrupt Enable */
#define CAN_IE_TCE                  (0x1u << CAN_IE_TCE_Pos)
#define CAN_IE_TCFE_Pos             10           /**< \brief (CAN_IE) Transmission Cancellation Finished Interrupt Enable */
#define CAN_IE_TCFE                 (0x1u << CAN_IE_TCFE_Pos)
#define CAN_IE_TFEE_Pos             11           /**< \brief (CAN_IE) Tx FIFO Empty Interrupt Enable */
#define CAN_IE_TFEE                 (0x1u << CAN_IE_TFEE_Pos)
#define CAN_IE_TEFNE_Pos            12           /**< \brief (CAN_IE) Tx Event FIFO New Entry Interrupt Enable */
#define CAN_IE_TEFNE                (0x1u << CAN_IE_TEFNE_Pos)
#define CAN_IE_TEFWE_Pos            13           /**< \brief (CAN_IE) Tx Event FIFO Watermark Reached Interrupt Enable */
#define CAN_IE_TEFWE                (0x1u << CAN_IE_TEFWE_Pos)
#define CAN_IE_TEFFE_Pos            14           /**< \brief (CAN_IE) Tx Event FIFO Full Interrupt Enable */
#define CAN_IE_TEFFE                (0x1u << CAN_IE_TEFFE_Pos)
#define CAN_IE_TEFLE_Pos            15           /**< \brief (CAN_IE) Tx Event FIFO Element Lost Interrupt Enable */
#define CAN_IE_TEFLE                (0x1u << CAN_IE_TEFLE_Pos)
#define CAN_IE_TSWE_Pos             16           /**< \brief (CAN_IE) Timestamp Wraparound Interrupt Enable */
#define CAN_IE_TSWE                 (0x1u << CAN_IE_TSWE_Pos)
#define CAN_IE_MRAFE_Pos            17           /**< \brief (CAN_IE) Message RAM Access Failure Interrupt Enable */
#define CAN_IE_MRAFE                (0x1u << CAN_IE_MRAFE_Pos)
#define CAN_IE_TOOE_Pos             18           /**< \brief (CAN_IE) Timeout Occurred Interrupt Enable */
#define CAN_IE_TOOE                 (0x1u << CAN_IE_TOOE_Pos)
#define CAN_IE_DRXE_Pos             19           /**< \brief (CAN_IE) Message stored to Dedicated Rx Buffer Interrupt Enable */
#define CAN_IE_DRXE                 (0x1u << CAN_IE_DRXE_Pos)
#define CAN_IE_BECE_Pos             20           /**< \brief (CAN_IE) Bit Error Corrected Interrupt Enable */
#define CAN_IE_BECE                 (0x1u << CAN_IE_BECE_Pos)
#define CAN_IE_BEUE_Pos             21           /**< \brief (CAN_IE) Bit Error Uncorrected Interrupt Enable */
#define CAN_IE_BEUE                 (0x1u << CAN_IE_BEUE_Pos)
#define CAN_IE_ELOE_Pos             22           /**< \brief (CAN_IE) Error Logging Overflow Interrupt Enable */
#define CAN_IE_ELOE                 (0x1u << CAN_IE_ELOE_Pos)
#define CAN_IE_EPE_Pos              23           /**< \brief (CAN_IE) Error Passive Interrupt Enable */
#define CAN_IE_EPE                  (0x1u << CAN_IE_EPE_Pos)
#define CAN_IE_EWE_Pos              24           /**< \brief (CAN_IE) Warning Status Interrupt Enable */
#define CAN_IE_EWE                  (0x1u << CAN_IE_EWE_Pos)
#define CAN_IE_BOE_Pos              25           /**< \brief (CAN_IE) Bus_Off Status Interrupt Enable */
#define CAN_IE_BOE                  (0x1u << CAN_IE_BOE_Pos)
#define CAN_IE_WDIE_Pos             26           /**< \brief (CAN_IE) Watchdog Interrupt Interrupt Enable */
#define CAN_IE_WDIE                 (0x1u << CAN_IE_WDIE_Pos)
#define CAN_IE_PEAE_Pos             27           /**< \brief (CAN_IE) Protocol Error in Arbitration Phase Enable */
#define CAN_IE_PEAE                 (0x1u << CAN_IE_PEAE_Pos)
#define CAN_IE_PEDE_Pos             28           /**< \brief (CAN_IE) Protocol Error in Data Phase Enable */
#define CAN_IE_PEDE                 (0x1u << CAN_IE_PEDE_Pos)
#define CAN_IE_ARAE_Pos             29           /**< \brief (CAN_IE) Access to Reserved Address Enable */
#define CAN_IE_ARAE                 (0x1u << CAN_IE_ARAE_Pos)
#define CAN_IE_MASK                 0x3FFFFFFFu  /**< \brief (CAN_IE) MASK Register */

/* -------- CAN_ILS : (CAN Offset: 0x58) (R/W 32) Interrupt Line Select -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t RF0NL:1;          /*!< bit:      0  Rx FIFO 0 New Message Interrupt Line */
    uint32_t RF0WL:1;          /*!< bit:      1  Rx FIFO 0 Watermark Reached Interrupt Line */
    uint32_t RF0FL:1;          /*!< bit:      2  Rx FIFO 0 Full Interrupt Line      */
    uint32_t RF0LL:1;          /*!< bit:      3  Rx FIFO 0 Message Lost Interrupt Line */
    uint32_t RF1NL:1;          /*!< bit:      4  Rx FIFO 1 New Message Interrupt Line */
    uint32_t RF1WL:1;          /*!< bit:      5  Rx FIFO 1 Watermark Reached Interrupt Line */
    uint32_t RF1FL:1;          /*!< bit:      6  Rx FIFO 1 FIFO Full Interrupt Line */
    uint32_t RF1LL:1;          /*!< bit:      7  Rx FIFO 1 Message Lost Interrupt Line */
    uint32_t HPML:1;           /*!< bit:      8  High Priority Message Interrupt Line */
    uint32_t TCL:1;            /*!< bit:      9  Timestamp Completed Interrupt Line */
    uint32_t TCFL:1;           /*!< bit:     10  Transmission Cancellation Finished Interrupt Line */
    uint32_t TFEL:1;           /*!< bit:     11  Tx FIFO Empty Interrupt Line       */
    uint32_t TEFNL:1;          /*!< bit:     12  Tx Event FIFO New Entry Interrupt Line */
    uint32_t TEFWL:1;          /*!< bit:     13  Tx Event FIFO Watermark Reached Interrupt Line */
    uint32_t TEFFL:1;          /*!< bit:     14  Tx Event FIFO Full Interrupt Line  */
    uint32_t TEFLL:1;          /*!< bit:     15  Tx Event FIFO Element Lost Interrupt Line */
    uint32_t TSWL:1;           /*!< bit:     16  Timestamp Wraparound Interrupt Line */
    uint32_t MRAFL:1;          /*!< bit:     17  Message RAM Access Failure Interrupt Line */
    uint32_t TOOL:1;           /*!< bit:     18  Timeout Occurred Interrupt Line    */
    uint32_t DRXL:1;           /*!< bit:     19  Message stored to Dedicated Rx Buffer Interrupt Line */
    uint32_t BECL:1;           /*!< bit:     20  Bit Error Corrected Interrupt Line */
    uint32_t BEUL:1;           /*!< bit:     21  Bit Error Uncorrected Interrupt Line */
    uint32_t ELOL:1;           /*!< bit:     22  Error Logging Overflow Interrupt Line */
    uint32_t EPL:1;            /*!< bit:     23  Error Passive Interrupt Line       */
    uint32_t EWL:1;            /*!< bit:     24  Warning Status Interrupt Line      */
    uint32_t BOL:1;            /*!< bit:     25  Bus_Off Status Interrupt Line      */
    uint32_t WDIL:1;           /*!< bit:     26  Watchdog Interrupt Interrupt Line  */
    uint32_t PEAL:1;           /*!< bit:     27  Protocol Error in Arbitration Phase Line */
    uint32_t PEDL:1;           /*!< bit:     28  Protocol Error in Data Phase Line  */
    uint32_t ARAL:1;           /*!< bit:     29  Access to Reserved Address Line    */
    uint32_t :2;               /*!< bit: 30..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_ILS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_ILS_OFFSET              0x58         /**< \brief (CAN_ILS offset) Interrupt Line Select */
#define CAN_ILS_RESETVALUE          0x00000000u  /**< \brief (CAN_ILS reset_value) Interrupt Line Select */

#define CAN_ILS_RF0NL_Pos           0            /**< \brief (CAN_ILS) Rx FIFO 0 New Message Interrupt Line */
#define CAN_ILS_RF0NL               (0x1u << CAN_ILS_RF0NL_Pos)
#define CAN_ILS_RF0WL_Pos           1            /**< \brief (CAN_ILS) Rx FIFO 0 Watermark Reached Interrupt Line */
#define CAN_ILS_RF0WL               (0x1u << CAN_ILS_RF0WL_Pos)
#define CAN_ILS_RF0FL_Pos           2            /**< \brief (CAN_ILS) Rx FIFO 0 Full Interrupt Line */
#define CAN_ILS_RF0FL               (0x1u << CAN_ILS_RF0FL_Pos)
#define CAN_ILS_RF0LL_Pos           3            /**< \brief (CAN_ILS) Rx FIFO 0 Message Lost Interrupt Line */
#define CAN_ILS_RF0LL               (0x1u << CAN_ILS_RF0LL_Pos)
#define CAN_ILS_RF1NL_Pos           4            /**< \brief (CAN_ILS) Rx FIFO 1 New Message Interrupt Line */
#define CAN_ILS_RF1NL               (0x1u << CAN_ILS_RF1NL_Pos)
#define CAN_ILS_RF1WL_Pos           5            /**< \brief (CAN_ILS) Rx FIFO 1 Watermark Reached Interrupt Line */
#define CAN_ILS_RF1WL               (0x1u << CAN_ILS_RF1WL_Pos)
#define CAN_ILS_RF1FL_Pos           6            /**< \brief (CAN_ILS) Rx FIFO 1 FIFO Full Interrupt Line */
#define CAN_ILS_RF1FL               (0x1u << CAN_ILS_RF1FL_Pos)
#define CAN_ILS_RF1LL_Pos           7            /**< \brief (CAN_ILS) Rx FIFO 1 Message Lost Interrupt Line */
#define CAN_ILS_RF1LL               (0x1u << CAN_ILS_RF1LL_Pos)
#define CAN_ILS_HPML_Pos            8            /**< \brief (CAN_ILS) High Priority Message Interrupt Line */
#define CAN_ILS_HPML                (0x1u << CAN_ILS_HPML_Pos)
#define CAN_ILS_TCL_Pos             9            /**< \brief (CAN_ILS) Timestamp Completed Interrupt Line */
#define CAN_ILS_TCL                 (0x1u << CAN_ILS_TCL_Pos)
#define CAN_ILS_TCFL_Pos            10           /**< \brief (CAN_ILS) Transmission Cancellation Finished Interrupt Line */
#define CAN_ILS_TCFL                (0x1u << CAN_ILS_TCFL_Pos)
#define CAN_ILS_TFEL_Pos            11           /**< \brief (CAN_ILS) Tx FIFO Empty Interrupt Line */
#define CAN_ILS_TFEL                (0x1u << CAN_ILS_TFEL_Pos)
#define CAN_ILS_TEFNL_Pos           12           /**< \brief (CAN_ILS) Tx Event FIFO New Entry Interrupt Line */
#define CAN_ILS_TEFNL               (0x1u << CAN_ILS_TEFNL_Pos)
#define CAN_ILS_TEFWL_Pos           13           /**< \brief (CAN_ILS) Tx Event FIFO Watermark Reached Interrupt Line */
#define CAN_ILS_TEFWL               (0x1u << CAN_ILS_TEFWL_Pos)
#define CAN_ILS_TEFFL_Pos           14           /**< \brief (CAN_ILS) Tx Event FIFO Full Interrupt Line */
#define CAN_ILS_TEFFL               (0x1u << CAN_ILS_TEFFL_Pos)
#define CAN_ILS_TEFLL_Pos           15           /**< \brief (CAN_ILS) Tx Event FIFO Element Lost Interrupt Line */
#define CAN_ILS_TEFLL               (0x1u << CAN_ILS_TEFLL_Pos)
#define CAN_ILS_TSWL_Pos            16           /**< \brief (CAN_ILS) Timestamp Wraparound Interrupt Line */
#define CAN_ILS_TSWL                (0x1u << CAN_ILS_TSWL_Pos)
#define CAN_ILS_MRAFL_Pos           17           /**< \brief (CAN_ILS) Message RAM Access Failure Interrupt Line */
#define CAN_ILS_MRAFL               (0x1u << CAN_ILS_MRAFL_Pos)
#define CAN_ILS_TOOL_Pos            18           /**< \brief (CAN_ILS) Timeout Occurred Interrupt Line */
#define CAN_ILS_TOOL                (0x1u << CAN_ILS_TOOL_Pos)
#define CAN_ILS_DRXL_Pos            19           /**< \brief (CAN_ILS) Message stored to Dedicated Rx Buffer Interrupt Line */
#define CAN_ILS_DRXL                (0x1u << CAN_ILS_DRXL_Pos)
#define CAN_ILS_BECL_Pos            20           /**< \brief (CAN_ILS) Bit Error Corrected Interrupt Line */
#define CAN_ILS_BECL                (0x1u << CAN_ILS_BECL_Pos)
#define CAN_ILS_BEUL_Pos            21           /**< \brief (CAN_ILS) Bit Error Uncorrected Interrupt Line */
#define CAN_ILS_BEUL                (0x1u << CAN_ILS_BEUL_Pos)
#define CAN_ILS_ELOL_Pos            22           /**< \brief (CAN_ILS) Error Logging Overflow Interrupt Line */
#define CAN_ILS_ELOL                (0x1u << CAN_ILS_ELOL_Pos)
#define CAN_ILS_EPL_Pos             23           /**< \brief (CAN_ILS) Error Passive Interrupt Line */
#define CAN_ILS_EPL                 (0x1u << CAN_ILS_EPL_Pos)
#define CAN_ILS_EWL_Pos             24           /**< \brief (CAN_ILS) Warning Status Interrupt Line */
#define CAN_ILS_EWL                 (0x1u << CAN_ILS_EWL_Pos)
#define CAN_ILS_BOL_Pos             25           /**< \brief (CAN_ILS) Bus_Off Status Interrupt Line */
#define CAN_ILS_BOL                 (0x1u << CAN_ILS_BOL_Pos)
#define CAN_ILS_WDIL_Pos            26           /**< \brief (CAN_ILS) Watchdog Interrupt Interrupt Line */
#define CAN_ILS_WDIL                (0x1u << CAN_ILS_WDIL_Pos)
#define CAN_ILS_PEAL_Pos            27           /**< \brief (CAN_ILS) Protocol Error in Arbitration Phase Line */
#define CAN_ILS_PEAL                (0x1u << CAN_ILS_PEAL_Pos)
#define CAN_ILS_PEDL_Pos            28           /**< \brief (CAN_ILS) Protocol Error in Data Phase Line */
#define CAN_ILS_PEDL                (0x1u << CAN_ILS_PEDL_Pos)
#define CAN_ILS_ARAL_Pos            29           /**< \brief (CAN_ILS) Access to Reserved Address Line */
#define CAN_ILS_ARAL                (0x1u << CAN_ILS_ARAL_Pos)
#define CAN_ILS_MASK                0x3FFFFFFFu  /**< \brief (CAN_ILS) MASK Register */

/* -------- CAN_ILE : (CAN Offset: 0x5C) (R/W 32) Interrupt Line Enable -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EINT0:1;          /*!< bit:      0  Enable Interrupt Line 0            */
    uint32_t EINT1:1;          /*!< bit:      1  Enable Interrupt Line 1            */
    uint32_t :30;              /*!< bit:  2..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_ILE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_ILE_OFFSET              0x5C         /**< \brief (CAN_ILE offset) Interrupt Line Enable */
#define CAN_ILE_RESETVALUE          0x00000000u  /**< \brief (CAN_ILE reset_value) Interrupt Line Enable */

#define CAN_ILE_EINT0_Pos           0            /**< \brief (CAN_ILE) Enable Interrupt Line 0 */
#define CAN_ILE_EINT0               (0x1u << CAN_ILE_EINT0_Pos)
#define CAN_ILE_EINT1_Pos           1            /**< \brief (CAN_ILE) Enable Interrupt Line 1 */
#define CAN_ILE_EINT1               (0x1u << CAN_ILE_EINT1_Pos)
#define CAN_ILE_MASK                0x00000003u  /**< \brief (CAN_ILE) MASK Register */

/* -------- CAN_GFC : (CAN Offset: 0x80) (R/W 32) Global Filter Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t RRFE:1;           /*!< bit:      0  Reject Remote Frames Extended      */
    uint32_t RRFS:1;           /*!< bit:      1  Reject Remote Frames Standard      */
    uint32_t ANFE:2;           /*!< bit:  2.. 3  Accept Non-matching Frames Extended */
    uint32_t ANFS:2;           /*!< bit:  4.. 5  Accept Non-matching Frames Standard */
    uint32_t :26;              /*!< bit:  6..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_GFC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_GFC_OFFSET              0x80         /**< \brief (CAN_GFC offset) Global Filter Configuration */
#define CAN_GFC_RESETVALUE          0x00000000u  /**< \brief (CAN_GFC reset_value) Global Filter Configuration */

#define CAN_GFC_RRFE_Pos            0            /**< \brief (CAN_GFC) Reject Remote Frames Extended */
#define CAN_GFC_RRFE                (0x1u << CAN_GFC_RRFE_Pos)
#define CAN_GFC_RRFS_Pos            1            /**< \brief (CAN_GFC) Reject Remote Frames Standard */
#define CAN_GFC_RRFS                (0x1u << CAN_GFC_RRFS_Pos)
#define CAN_GFC_ANFE_Pos            2            /**< \brief (CAN_GFC) Accept Non-matching Frames Extended */
#define CAN_GFC_ANFE_Msk            (0x3u << CAN_GFC_ANFE_Pos)
#define CAN_GFC_ANFE(value)         (CAN_GFC_ANFE_Msk & ((value) << CAN_GFC_ANFE_Pos))
#define   CAN_GFC_ANFE_RXF0_Val           0x0u   /**< \brief (CAN_GFC) Accept in Rx FIFO 0 */
#define   CAN_GFC_ANFE_RXF1_Val           0x1u   /**< \brief (CAN_GFC) Accept in Rx FIFO 1 */
#define   CAN_GFC_ANFE_REJECT_Val         0x2u   /**< \brief (CAN_GFC) Reject */
#define CAN_GFC_ANFE_RXF0           (CAN_GFC_ANFE_RXF0_Val         << CAN_GFC_ANFE_Pos)
#define CAN_GFC_ANFE_RXF1           (CAN_GFC_ANFE_RXF1_Val         << CAN_GFC_ANFE_Pos)
#define CAN_GFC_ANFE_REJECT         (CAN_GFC_ANFE_REJECT_Val       << CAN_GFC_ANFE_Pos)
#define CAN_GFC_ANFS_Pos            4            /**< \brief (CAN_GFC) Accept Non-matching Frames Standard */
#define CAN_GFC_ANFS_Msk            (0x3u << CAN_GFC_ANFS_Pos)
#define CAN_GFC_ANFS(value)         (CAN_GFC_ANFS_Msk & ((value) << CAN_GFC_ANFS_Pos))
#define   CAN_GFC_ANFS_RXF0_Val           0x0u   /**< \brief (CAN_GFC) Accept in Rx FIFO 0 */
#define   CAN_GFC_ANFS_RXF1_Val           0x1u   /**< \brief (CAN_GFC) Accept in Rx FIFO 1 */
#define   CAN_GFC_ANFS_REJECT_Val         0x2u   /**< \brief (CAN_GFC) Reject */
#define CAN_GFC_ANFS_RXF0           (CAN_GFC_ANFS_RXF0_Val         << CAN_GFC_ANFS_Pos)
#define CAN_GFC_ANFS_RXF1           (CAN_GFC_ANFS_RXF1_Val         << CAN_GFC_ANFS_Pos)
#define CAN_GFC_ANFS_REJECT         (CAN_GFC_ANFS_REJECT_Val       << CAN_GFC_ANFS_Pos)
#define CAN_GFC_MASK                0x0000003Fu  /**< \brief (CAN_GFC) MASK Register */

/* -------- CAN_SIDFC : (CAN Offset: 0x84) (R/W 32) Standard ID Filter Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t FLSSA:16;         /*!< bit:  0..15  Filter List Standard Start Address */
    uint32_t LSS:8;            /*!< bit: 16..23  List Size Standard                 */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_SIDFC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_SIDFC_OFFSET            0x84         /**< \brief (CAN_SIDFC offset) Standard ID Filter Configuration */
#define CAN_SIDFC_RESETVALUE        0x00000000u  /**< \brief (CAN_SIDFC reset_value) Standard ID Filter Configuration */

#define CAN_SIDFC_FLSSA_Pos         0            /**< \brief (CAN_SIDFC) Filter List Standard Start Address */
#define CAN_SIDFC_FLSSA_Msk         (0xFFFFu << CAN_SIDFC_FLSSA_Pos)
#define CAN_SIDFC_FLSSA(value)      (CAN_SIDFC_FLSSA_Msk & ((value) << CAN_SIDFC_FLSSA_Pos))
#define CAN_SIDFC_LSS_Pos           16           /**< \brief (CAN_SIDFC) List Size Standard */
#define CAN_SIDFC_LSS_Msk           (0xFFu << CAN_SIDFC_LSS_Pos)
#define CAN_SIDFC_LSS(value)        (CAN_SIDFC_LSS_Msk & ((value) << CAN_SIDFC_LSS_Pos))
#define CAN_SIDFC_MASK              0x00FFFFFFu  /**< \brief (CAN_SIDFC) MASK Register */

/* -------- CAN_XIDFC : (CAN Offset: 0x88) (R/W 32) Extended ID Filter Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t FLESA:16;         /*!< bit:  0..15  Filter List Extended Start Address */
    uint32_t LSE:7;            /*!< bit: 16..22  List Size Extended                 */
    uint32_t :9;               /*!< bit: 23..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_XIDFC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_XIDFC_OFFSET            0x88         /**< \brief (CAN_XIDFC offset) Extended ID Filter Configuration */
#define CAN_XIDFC_RESETVALUE        0x00000000u  /**< \brief (CAN_XIDFC reset_value) Extended ID Filter Configuration */

#define CAN_XIDFC_FLESA_Pos         0            /**< \brief (CAN_XIDFC) Filter List Extended Start Address */
#define CAN_XIDFC_FLESA_Msk         (0xFFFFu << CAN_XIDFC_FLESA_Pos)
#define CAN_XIDFC_FLESA(value)      (CAN_XIDFC_FLESA_Msk & ((value) << CAN_XIDFC_FLESA_Pos))
#define CAN_XIDFC_LSE_Pos           16           /**< \brief (CAN_XIDFC) List Size Extended */
#define CAN_XIDFC_LSE_Msk           (0x7Fu << CAN_XIDFC_LSE_Pos)
#define CAN_XIDFC_LSE(value)        (CAN_XIDFC_LSE_Msk & ((value) << CAN_XIDFC_LSE_Pos))
#define CAN_XIDFC_MASK              0x007FFFFFu  /**< \brief (CAN_XIDFC) MASK Register */

/* -------- CAN_XIDAM : (CAN Offset: 0x90) (R/W 32) Extended ID AND Mask -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EIDM:29;          /*!< bit:  0..28  Extended ID Mask                   */
    uint32_t :3;               /*!< bit: 29..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_XIDAM_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_XIDAM_OFFSET            0x90         /**< \brief (CAN_XIDAM offset) Extended ID AND Mask */
#define CAN_XIDAM_RESETVALUE        0x1FFFFFFFu  /**< \brief (CAN_XIDAM reset_value) Extended ID AND Mask */

#define CAN_XIDAM_EIDM_Pos          0            /**< \brief (CAN_XIDAM) Extended ID Mask */
#define CAN_XIDAM_EIDM_Msk          (0x1FFFFFFFu << CAN_XIDAM_EIDM_Pos)
#define CAN_XIDAM_EIDM(value)       (CAN_XIDAM_EIDM_Msk & ((value) << CAN_XIDAM_EIDM_Pos))
#define CAN_XIDAM_MASK              0x1FFFFFFFu  /**< \brief (CAN_XIDAM) MASK Register */

/* -------- CAN_HPMS : (CAN Offset: 0x94) (R/  32) High Priority Message Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t BIDX:6;           /*!< bit:  0.. 5  Buffer Index                       */
    uint32_t MSI:2;            /*!< bit:  6.. 7  Message Storage Indicator          */
    uint32_t FIDX:7;           /*!< bit:  8..14  Filter Index                       */
    uint32_t FLST:1;           /*!< bit:     15  Filter List                        */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_HPMS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_HPMS_OFFSET             0x94         /**< \brief (CAN_HPMS offset) High Priority Message Status */
#define CAN_HPMS_RESETVALUE         0x00000000u  /**< \brief (CAN_HPMS reset_value) High Priority Message Status */

#define CAN_HPMS_BIDX_Pos           0            /**< \brief (CAN_HPMS) Buffer Index */
#define CAN_HPMS_BIDX_Msk           (0x3Fu << CAN_HPMS_BIDX_Pos)
#define CAN_HPMS_BIDX(value)        (CAN_HPMS_BIDX_Msk & ((value) << CAN_HPMS_BIDX_Pos))
#define CAN_HPMS_MSI_Pos            6            /**< \brief (CAN_HPMS) Message Storage Indicator */
#define CAN_HPMS_MSI_Msk            (0x3u << CAN_HPMS_MSI_Pos)
#define CAN_HPMS_MSI(value)         (CAN_HPMS_MSI_Msk & ((value) << CAN_HPMS_MSI_Pos))
#define   CAN_HPMS_MSI_NONE_Val           0x0u   /**< \brief (CAN_HPMS) No FIFO selected */
#define   CAN_HPMS_MSI_LOST_Val           0x1u   /**< \brief (CAN_HPMS) FIFO message lost */
#define   CAN_HPMS_MSI_FIFO0_Val          0x2u   /**< \brief (CAN_HPMS) Message stored in FIFO 0 */
#define   CAN_HPMS_MSI_FIFO1_Val          0x3u   /**< \brief (CAN_HPMS) Message stored in FIFO 1 */
#define CAN_HPMS_MSI_NONE           (CAN_HPMS_MSI_NONE_Val         << CAN_HPMS_MSI_Pos)
#define CAN_HPMS_MSI_LOST           (CAN_HPMS_MSI_LOST_Val         << CAN_HPMS_MSI_Pos)
#define CAN_HPMS_MSI_FIFO0          (CAN_HPMS_MSI_FIFO0_Val        << CAN_HPMS_MSI_Pos)
#define CAN_HPMS_MSI_FIFO1          (CAN_HPMS_MSI_FIFO1_Val        << CAN_HPMS_MSI_Pos)
#define CAN_HPMS_FIDX_Pos           8            /**< \brief (CAN_HPMS) Filter Index */
#define CAN_HPMS_FIDX_Msk           (0x7Fu << CAN_HPMS_FIDX_Pos)
#define CAN_HPMS_FIDX(value)        (CAN_HPMS_FIDX_Msk & ((value) << CAN_HPMS_FIDX_Pos))
#define CAN_HPMS_FLST_Pos           15           /**< \brief (CAN_HPMS) Filter List */
#define CAN_HPMS_FLST               (0x1u << CAN_HPMS_FLST_Pos)
#define CAN_HPMS_MASK               0x0000FFFFu  /**< \brief (CAN_HPMS) MASK Register */

/* -------- CAN_NDAT1 : (CAN Offset: 0x98) (R/W 32) New Data 1 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ND0:1;            /*!< bit:      0  New Data 0                         */
    uint32_t ND1:1;            /*!< bit:      1  New Data 1                         */
    uint32_t ND2:1;            /*!< bit:      2  New Data 2                         */
    uint32_t ND3:1;            /*!< bit:      3  New Data 3                         */
    uint32_t ND4:1;            /*!< bit:      4  New Data 4                         */
    uint32_t ND5:1;            /*!< bit:      5  New Data 5                         */
    uint32_t ND6:1;            /*!< bit:      6  New Data 6                         */
    uint32_t ND7:1;            /*!< bit:      7  New Data 7                         */
    uint32_t ND8:1;            /*!< bit:      8  New Data 8                         */
    uint32_t ND9:1;            /*!< bit:      9  New Data 9                         */
    uint32_t ND10:1;           /*!< bit:     10  New Data 10                        */
    uint32_t ND11:1;           /*!< bit:     11  New Data 11                        */
    uint32_t ND12:1;           /*!< bit:     12  New Data 12                        */
    uint32_t ND13:1;           /*!< bit:     13  New Data 13                        */
    uint32_t ND14:1;           /*!< bit:     14  New Data 14                        */
    uint32_t ND15:1;           /*!< bit:     15  New Data 15                        */
    uint32_t ND16:1;           /*!< bit:     16  New Data 16                        */
    uint32_t ND17:1;           /*!< bit:     17  New Data 17                        */
    uint32_t ND18:1;           /*!< bit:     18  New Data 18                        */
    uint32_t ND19:1;           /*!< bit:     19  New Data 19                        */
    uint32_t ND20:1;           /*!< bit:     20  New Data 20                        */
    uint32_t ND21:1;           /*!< bit:     21  New Data 21                        */
    uint32_t ND22:1;           /*!< bit:     22  New Data 22                        */
    uint32_t ND23:1;           /*!< bit:     23  New Data 23                        */
    uint32_t ND24:1;           /*!< bit:     24  New Data 24                        */
    uint32_t ND25:1;           /*!< bit:     25  New Data 25                        */
    uint32_t ND26:1;           /*!< bit:     26  New Data 26                        */
    uint32_t ND27:1;           /*!< bit:     27  New Data 27                        */
    uint32_t ND28:1;           /*!< bit:     28  New Data 28                        */
    uint32_t ND29:1;           /*!< bit:     29  New Data 29                        */
    uint32_t ND30:1;           /*!< bit:     30  New Data 30                        */
    uint32_t ND31:1;           /*!< bit:     31  New Data 31                        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_NDAT1_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_NDAT1_OFFSET            0x98         /**< \brief (CAN_NDAT1 offset) New Data 1 */
#define CAN_NDAT1_RESETVALUE        0x00000000u  /**< \brief (CAN_NDAT1 reset_value) New Data 1 */

#define CAN_NDAT1_ND0_Pos           0            /**< \brief (CAN_NDAT1) New Data 0 */
#define CAN_NDAT1_ND0               (0x1u << CAN_NDAT1_ND0_Pos)
#define CAN_NDAT1_ND1_Pos           1            /**< \brief (CAN_NDAT1) New Data 1 */
#define CAN_NDAT1_ND1               (0x1u << CAN_NDAT1_ND1_Pos)
#define CAN_NDAT1_ND2_Pos           2            /**< \brief (CAN_NDAT1) New Data 2 */
#define CAN_NDAT1_ND2               (0x1u << CAN_NDAT1_ND2_Pos)
#define CAN_NDAT1_ND3_Pos           3            /**< \brief (CAN_NDAT1) New Data 3 */
#define CAN_NDAT1_ND3               (0x1u << CAN_NDAT1_ND3_Pos)
#define CAN_NDAT1_ND4_Pos           4            /**< \brief (CAN_NDAT1) New Data 4 */
#define CAN_NDAT1_ND4               (0x1u << CAN_NDAT1_ND4_Pos)
#define CAN_NDAT1_ND5_Pos           5            /**< \brief (CAN_NDAT1) New Data 5 */
#define CAN_NDAT1_ND5               (0x1u << CAN_NDAT1_ND5_Pos)
#define CAN_NDAT1_ND6_Pos           6            /**< \brief (CAN_NDAT1) New Data 6 */
#define CAN_NDAT1_ND6               (0x1u << CAN_NDAT1_ND6_Pos)
#define CAN_NDAT1_ND7_Pos           7            /**< \brief (CAN_NDAT1) New Data 7 */
#define CAN_NDAT1_ND7               (0x1u << CAN_NDAT1_ND7_Pos)
#define CAN_NDAT1_ND8_Pos           8            /**< \brief (CAN_NDAT1) New Data 8 */
#define CAN_NDAT1_ND8               (0x1u << CAN_NDAT1_ND8_Pos)
#define CAN_NDAT1_ND9_Pos           9            /**< \brief (CAN_NDAT1) New Data 9 */
#define CAN_NDAT1_ND9               (0x1u << CAN_NDAT1_ND9_Pos)
#define CAN_NDAT1_ND10_Pos          10           /**< \brief (CAN_NDAT1) New Data 10 */
#define CAN_NDAT1_ND10              (0x1u << CAN_NDAT1_ND10_Pos)
#define CAN_NDAT1_ND11_Pos          11           /**< \brief (CAN_NDAT1) New Data 11 */
#define CAN_NDAT1_ND11              (0x1u << CAN_NDAT1_ND11_Pos)
#define CAN_NDAT1_ND12_Pos          12           /**< \brief (CAN_NDAT1) New Data 12 */
#define CAN_NDAT1_ND12              (0x1u << CAN_NDAT1_ND12_Pos)
#define CAN_NDAT1_ND13_Pos          13           /**< \brief (CAN_NDAT1) New Data 13 */
#define CAN_NDAT1_ND13              (0x1u << CAN_NDAT1_ND13_Pos)
#define CAN_NDAT1_ND14_Pos          14           /**< \brief (CAN_NDAT1) New Data 14 */
#define CAN_NDAT1_ND14              (0x1u << CAN_NDAT1_ND14_Pos)
#define CAN_NDAT1_ND15_Pos          15           /**< \brief (CAN_NDAT1) New Data 15 */
#define CAN_NDAT1_ND15              (0x1u << CAN_NDAT1_ND15_Pos)
#define CAN_NDAT1_ND16_Pos          16           /**< \brief (CAN_NDAT1) New Data 16 */
#define CAN_NDAT1_ND16              (0x1u << CAN_NDAT1_ND16_Pos)
#define CAN_NDAT1_ND17_Pos          17           /**< \brief (CAN_NDAT1) New Data 17 */
#define CAN_NDAT1_ND17              (0x1u << CAN_NDAT1_ND17_Pos)
#define CAN_NDAT1_ND18_Pos          18           /**< \brief (CAN_NDAT1) New Data 18 */
#define CAN_NDAT1_ND18              (0x1u << CAN_NDAT1_ND18_Pos)
#define CAN_NDAT1_ND19_Pos          19           /**< \brief (CAN_NDAT1) New Data 19 */
#define CAN_NDAT1_ND19              (0x1u << CAN_NDAT1_ND19_Pos)
#define CAN_NDAT1_ND20_Pos          20           /**< \brief (CAN_NDAT1) New Data 20 */
#define CAN_NDAT1_ND20              (0x1u << CAN_NDAT1_ND20_Pos)
#define CAN_NDAT1_ND21_Pos          21           /**< \brief (CAN_NDAT1) New Data 21 */
#define CAN_NDAT1_ND21              (0x1u << CAN_NDAT1_ND21_Pos)
#define CAN_NDAT1_ND22_Pos          22           /**< \brief (CAN_NDAT1) New Data 22 */
#define CAN_NDAT1_ND22              (0x1u << CAN_NDAT1_ND22_Pos)
#define CAN_NDAT1_ND23_Pos          23           /**< \brief (CAN_NDAT1) New Data 23 */
#define CAN_NDAT1_ND23              (0x1u << CAN_NDAT1_ND23_Pos)
#define CAN_NDAT1_ND24_Pos          24           /**< \brief (CAN_NDAT1) New Data 24 */
#define CAN_NDAT1_ND24              (0x1u << CAN_NDAT1_ND24_Pos)
#define CAN_NDAT1_ND25_Pos          25           /**< \brief (CAN_NDAT1) New Data 25 */
#define CAN_NDAT1_ND25              (0x1u << CAN_NDAT1_ND25_Pos)
#define CAN_NDAT1_ND26_Pos          26           /**< \brief (CAN_NDAT1) New Data 26 */
#define CAN_NDAT1_ND26              (0x1u << CAN_NDAT1_ND26_Pos)
#define CAN_NDAT1_ND27_Pos          27           /**< \brief (CAN_NDAT1) New Data 27 */
#define CAN_NDAT1_ND27              (0x1u << CAN_NDAT1_ND27_Pos)
#define CAN_NDAT1_ND28_Pos          28           /**< \brief (CAN_NDAT1) New Data 28 */
#define CAN_NDAT1_ND28              (0x1u << CAN_NDAT1_ND28_Pos)
#define CAN_NDAT1_ND29_Pos          29           /**< \brief (CAN_NDAT1) New Data 29 */
#define CAN_NDAT1_ND29              (0x1u << CAN_NDAT1_ND29_Pos)
#define CAN_NDAT1_ND30_Pos          30           /**< \brief (CAN_NDAT1) New Data 30 */
#define CAN_NDAT1_ND30              (0x1u << CAN_NDAT1_ND30_Pos)
#define CAN_NDAT1_ND31_Pos          31           /**< \brief (CAN_NDAT1) New Data 31 */
#define CAN_NDAT1_ND31              (0x1u << CAN_NDAT1_ND31_Pos)
#define CAN_NDAT1_MASK              0xFFFFFFFFu  /**< \brief (CAN_NDAT1) MASK Register */

/* -------- CAN_NDAT2 : (CAN Offset: 0x9C) (R/W 32) New Data 2 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ND32:1;           /*!< bit:      0  New Data 32                        */
    uint32_t ND33:1;           /*!< bit:      1  New Data 33                        */
    uint32_t ND34:1;           /*!< bit:      2  New Data 34                        */
    uint32_t ND35:1;           /*!< bit:      3  New Data 35                        */
    uint32_t ND36:1;           /*!< bit:      4  New Data 36                        */
    uint32_t ND37:1;           /*!< bit:      5  New Data 37                        */
    uint32_t ND38:1;           /*!< bit:      6  New Data 38                        */
    uint32_t ND39:1;           /*!< bit:      7  New Data 39                        */
    uint32_t ND40:1;           /*!< bit:      8  New Data 40                        */
    uint32_t ND41:1;           /*!< bit:      9  New Data 41                        */
    uint32_t ND42:1;           /*!< bit:     10  New Data 42                        */
    uint32_t ND43:1;           /*!< bit:     11  New Data 43                        */
    uint32_t ND44:1;           /*!< bit:     12  New Data 44                        */
    uint32_t ND45:1;           /*!< bit:     13  New Data 45                        */
    uint32_t ND46:1;           /*!< bit:     14  New Data 46                        */
    uint32_t ND47:1;           /*!< bit:     15  New Data 47                        */
    uint32_t ND48:1;           /*!< bit:     16  New Data 48                        */
    uint32_t ND49:1;           /*!< bit:     17  New Data 49                        */
    uint32_t ND50:1;           /*!< bit:     18  New Data 50                        */
    uint32_t ND51:1;           /*!< bit:     19  New Data 51                        */
    uint32_t ND52:1;           /*!< bit:     20  New Data 52                        */
    uint32_t ND53:1;           /*!< bit:     21  New Data 53                        */
    uint32_t ND54:1;           /*!< bit:     22  New Data 54                        */
    uint32_t ND55:1;           /*!< bit:     23  New Data 55                        */
    uint32_t ND56:1;           /*!< bit:     24  New Data 56                        */
    uint32_t ND57:1;           /*!< bit:     25  New Data 57                        */
    uint32_t ND58:1;           /*!< bit:     26  New Data 58                        */
    uint32_t ND59:1;           /*!< bit:     27  New Data 59                        */
    uint32_t ND60:1;           /*!< bit:     28  New Data 60                        */
    uint32_t ND61:1;           /*!< bit:     29  New Data 61                        */
    uint32_t ND62:1;           /*!< bit:     30  New Data 62                        */
    uint32_t ND63:1;           /*!< bit:     31  New Data 63                        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_NDAT2_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_NDAT2_OFFSET            0x9C         /**< \brief (CAN_NDAT2 offset) New Data 2 */
#define CAN_NDAT2_RESETVALUE        0x00000000u  /**< \brief (CAN_NDAT2 reset_value) New Data 2 */

#define CAN_NDAT2_ND32_Pos          0            /**< \brief (CAN_NDAT2) New Data 32 */
#define CAN_NDAT2_ND32              (0x1u << CAN_NDAT2_ND32_Pos)
#define CAN_NDAT2_ND33_Pos          1            /**< \brief (CAN_NDAT2) New Data 33 */
#define CAN_NDAT2_ND33              (0x1u << CAN_NDAT2_ND33_Pos)
#define CAN_NDAT2_ND34_Pos          2            /**< \brief (CAN_NDAT2) New Data 34 */
#define CAN_NDAT2_ND34              (0x1u << CAN_NDAT2_ND34_Pos)
#define CAN_NDAT2_ND35_Pos          3            /**< \brief (CAN_NDAT2) New Data 35 */
#define CAN_NDAT2_ND35              (0x1u << CAN_NDAT2_ND35_Pos)
#define CAN_NDAT2_ND36_Pos          4            /**< \brief (CAN_NDAT2) New Data 36 */
#define CAN_NDAT2_ND36              (0x1u << CAN_NDAT2_ND36_Pos)
#define CAN_NDAT2_ND37_Pos          5            /**< \brief (CAN_NDAT2) New Data 37 */
#define CAN_NDAT2_ND37              (0x1u << CAN_NDAT2_ND37_Pos)
#define CAN_NDAT2_ND38_Pos          6            /**< \brief (CAN_NDAT2) New Data 38 */
#define CAN_NDAT2_ND38              (0x1u << CAN_NDAT2_ND38_Pos)
#define CAN_NDAT2_ND39_Pos          7            /**< \brief (CAN_NDAT2) New Data 39 */
#define CAN_NDAT2_ND39              (0x1u << CAN_NDAT2_ND39_Pos)
#define CAN_NDAT2_ND40_Pos          8            /**< \brief (CAN_NDAT2) New Data 40 */
#define CAN_NDAT2_ND40              (0x1u << CAN_NDAT2_ND40_Pos)
#define CAN_NDAT2_ND41_Pos          9            /**< \brief (CAN_NDAT2) New Data 41 */
#define CAN_NDAT2_ND41              (0x1u << CAN_NDAT2_ND41_Pos)
#define CAN_NDAT2_ND42_Pos          10           /**< \brief (CAN_NDAT2) New Data 42 */
#define CAN_NDAT2_ND42              (0x1u << CAN_NDAT2_ND42_Pos)
#define CAN_NDAT2_ND43_Pos          11           /**< \brief (CAN_NDAT2) New Data 43 */
#define CAN_NDAT2_ND43              (0x1u << CAN_NDAT2_ND43_Pos)
#define CAN_NDAT2_ND44_Pos          12           /**< \brief (CAN_NDAT2) New Data 44 */
#define CAN_NDAT2_ND44              (0x1u << CAN_NDAT2_ND44_Pos)
#define CAN_NDAT2_ND45_Pos          13           /**< \brief (CAN_NDAT2) New Data 45 */
#define CAN_NDAT2_ND45              (0x1u << CAN_NDAT2_ND45_Pos)
#define CAN_NDAT2_ND46_Pos          14           /**< \brief (CAN_NDAT2) New Data 46 */
#define CAN_NDAT2_ND46              (0x1u << CAN_NDAT2_ND46_Pos)
#define CAN_NDAT2_ND47_Pos          15           /**< \brief (CAN_NDAT2) New Data 47 */
#define CAN_NDAT2_ND47              (0x1u << CAN_NDAT2_ND47_Pos)
#define CAN_NDAT2_ND48_Pos          16           /**< \brief (CAN_NDAT2) New Data 48 */
#define CAN_NDAT2_ND48              (0x1u << CAN_NDAT2_ND48_Pos)
#define CAN_NDAT2_ND49_Pos          17           /**< \brief (CAN_NDAT2) New Data 49 */
#define CAN_NDAT2_ND49              (0x1u << CAN_NDAT2_ND49_Pos)
#define CAN_NDAT2_ND50_Pos          18           /**< \brief (CAN_NDAT2) New Data 50 */
#define CAN_NDAT2_ND50              (0x1u << CAN_NDAT2_ND50_Pos)
#define CAN_NDAT2_ND51_Pos          19           /**< \brief (CAN_NDAT2) New Data 51 */
#define CAN_NDAT2_ND51              (0x1u << CAN_NDAT2_ND51_Pos)
#define CAN_NDAT2_ND52_Pos          20           /**< \brief (CAN_NDAT2) New Data 52 */
#define CAN_NDAT2_ND52              (0x1u << CAN_NDAT2_ND52_Pos)
#define CAN_NDAT2_ND53_Pos          21           /**< \brief (CAN_NDAT2) New Data 53 */
#define CAN_NDAT2_ND53              (0x1u << CAN_NDAT2_ND53_Pos)
#define CAN_NDAT2_ND54_Pos          22           /**< \brief (CAN_NDAT2) New Data 54 */
#define CAN_NDAT2_ND54              (0x1u << CAN_NDAT2_ND54_Pos)
#define CAN_NDAT2_ND55_Pos          23           /**< \brief (CAN_NDAT2) New Data 55 */
#define CAN_NDAT2_ND55              (0x1u << CAN_NDAT2_ND55_Pos)
#define CAN_NDAT2_ND56_Pos          24           /**< \brief (CAN_NDAT2) New Data 56 */
#define CAN_NDAT2_ND56              (0x1u << CAN_NDAT2_ND56_Pos)
#define CAN_NDAT2_ND57_Pos          25           /**< \brief (CAN_NDAT2) New Data 57 */
#define CAN_NDAT2_ND57              (0x1u << CAN_NDAT2_ND57_Pos)
#define CAN_NDAT2_ND58_Pos          26           /**< \brief (CAN_NDAT2) New Data 58 */
#define CAN_NDAT2_ND58              (0x1u << CAN_NDAT2_ND58_Pos)
#define CAN_NDAT2_ND59_Pos          27           /**< \brief (CAN_NDAT2) New Data 59 */
#define CAN_NDAT2_ND59              (0x1u << CAN_NDAT2_ND59_Pos)
#define CAN_NDAT2_ND60_Pos          28           /**< \brief (CAN_NDAT2) New Data 60 */
#define CAN_NDAT2_ND60              (0x1u << CAN_NDAT2_ND60_Pos)
#define CAN_NDAT2_ND61_Pos          29           /**< \brief (CAN_NDAT2) New Data 61 */
#define CAN_NDAT2_ND61              (0x1u << CAN_NDAT2_ND61_Pos)
#define CAN_NDAT2_ND62_Pos          30           /**< \brief (CAN_NDAT2) New Data 62 */
#define CAN_NDAT2_ND62              (0x1u << CAN_NDAT2_ND62_Pos)
#define CAN_NDAT2_ND63_Pos          31           /**< \brief (CAN_NDAT2) New Data 63 */
#define CAN_NDAT2_ND63              (0x1u << CAN_NDAT2_ND63_Pos)
#define CAN_NDAT2_MASK              0xFFFFFFFFu  /**< \brief (CAN_NDAT2) MASK Register */

/* -------- CAN_RXF0C : (CAN Offset: 0xA0) (R/W 32) Rx FIFO 0 Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t F0SA:16;          /*!< bit:  0..15  Rx FIFO 0 Start Address            */
    uint32_t F0S:7;            /*!< bit: 16..22  Rx FIFO 0 Size                     */
    uint32_t :1;               /*!< bit:     23  Reserved                           */
    uint32_t F0WM:7;           /*!< bit: 24..30  Rx FIFO 0 Watermark                */
    uint32_t F0OM:1;           /*!< bit:     31  FIFO 0 Operation Mode              */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_RXF0C_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_RXF0C_OFFSET            0xA0         /**< \brief (CAN_RXF0C offset) Rx FIFO 0 Configuration */
#define CAN_RXF0C_RESETVALUE        0x00000000u  /**< \brief (CAN_RXF0C reset_value) Rx FIFO 0 Configuration */

#define CAN_RXF0C_F0SA_Pos          0            /**< \brief (CAN_RXF0C) Rx FIFO 0 Start Address */
#define CAN_RXF0C_F0SA_Msk          (0xFFFFu << CAN_RXF0C_F0SA_Pos)
#define CAN_RXF0C_F0SA(value)       (CAN_RXF0C_F0SA_Msk & ((value) << CAN_RXF0C_F0SA_Pos))
#define CAN_RXF0C_F0S_Pos           16           /**< \brief (CAN_RXF0C) Rx FIFO 0 Size */
#define CAN_RXF0C_F0S_Msk           (0x7Fu << CAN_RXF0C_F0S_Pos)
#define CAN_RXF0C_F0S(value)        (CAN_RXF0C_F0S_Msk & ((value) << CAN_RXF0C_F0S_Pos))
#define CAN_RXF0C_F0WM_Pos          24           /**< \brief (CAN_RXF0C) Rx FIFO 0 Watermark */
#define CAN_RXF0C_F0WM_Msk          (0x7Fu << CAN_RXF0C_F0WM_Pos)
#define CAN_RXF0C_F0WM(value)       (CAN_RXF0C_F0WM_Msk & ((value) << CAN_RXF0C_F0WM_Pos))
#define CAN_RXF0C_F0OM_Pos          31           /**< \brief (CAN_RXF0C) FIFO 0 Operation Mode */
#define CAN_RXF0C_F0OM              (0x1u << CAN_RXF0C_F0OM_Pos)
#define CAN_RXF0C_MASK              0xFF7FFFFFu  /**< \brief (CAN_RXF0C) MASK Register */

/* -------- CAN_RXF0S : (CAN Offset: 0xA4) (R/  32) Rx FIFO 0 Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t F0FL:7;           /*!< bit:  0.. 6  Rx FIFO 0 Fill Level               */
    uint32_t :1;               /*!< bit:      7  Reserved                           */
    uint32_t F0GI:6;           /*!< bit:  8..13  Rx FIFO 0 Get Index                */
    uint32_t :2;               /*!< bit: 14..15  Reserved                           */
    uint32_t F0PI:6;           /*!< bit: 16..21  Rx FIFO 0 Put Index                */
    uint32_t :2;               /*!< bit: 22..23  Reserved                           */
    uint32_t F0F:1;            /*!< bit:     24  Rx FIFO 0 Full                     */
    uint32_t RF0L:1;           /*!< bit:     25  Rx FIFO 0 Message Lost             */
    uint32_t :6;               /*!< bit: 26..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_RXF0S_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_RXF0S_OFFSET            0xA4         /**< \brief (CAN_RXF0S offset) Rx FIFO 0 Status */
#define CAN_RXF0S_RESETVALUE        0x00000000u  /**< \brief (CAN_RXF0S reset_value) Rx FIFO 0 Status */

#define CAN_RXF0S_F0FL_Pos          0            /**< \brief (CAN_RXF0S) Rx FIFO 0 Fill Level */
#define CAN_RXF0S_F0FL_Msk          (0x7Fu << CAN_RXF0S_F0FL_Pos)
#define CAN_RXF0S_F0FL(value)       (CAN_RXF0S_F0FL_Msk & ((value) << CAN_RXF0S_F0FL_Pos))
#define CAN_RXF0S_F0GI_Pos          8            /**< \brief (CAN_RXF0S) Rx FIFO 0 Get Index */
#define CAN_RXF0S_F0GI_Msk          (0x3Fu << CAN_RXF0S_F0GI_Pos)
#define CAN_RXF0S_F0GI(value)       (CAN_RXF0S_F0GI_Msk & ((value) << CAN_RXF0S_F0GI_Pos))
#define CAN_RXF0S_F0PI_Pos          16           /**< \brief (CAN_RXF0S) Rx FIFO 0 Put Index */
#define CAN_RXF0S_F0PI_Msk          (0x3Fu << CAN_RXF0S_F0PI_Pos)
#define CAN_RXF0S_F0PI(value)       (CAN_RXF0S_F0PI_Msk & ((value) << CAN_RXF0S_F0PI_Pos))
#define CAN_RXF0S_F0F_Pos           24           /**< \brief (CAN_RXF0S) Rx FIFO 0 Full */
#define CAN_RXF0S_F0F               (0x1u << CAN_RXF0S_F0F_Pos)
#define CAN_RXF0S_RF0L_Pos          25           /**< \brief (CAN_RXF0S) Rx FIFO 0 Message Lost */
#define CAN_RXF0S_RF0L              (0x1u << CAN_RXF0S_RF0L_Pos)
#define CAN_RXF0S_MASK              0x033F3F7Fu  /**< \brief (CAN_RXF0S) MASK Register */

/* -------- CAN_RXF0A : (CAN Offset: 0xA8) (R/W 32) Rx FIFO 0 Acknowledge -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t F0AI:6;           /*!< bit:  0.. 5  Rx FIFO 0 Acknowledge Index        */
    uint32_t :26;              /*!< bit:  6..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_RXF0A_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_RXF0A_OFFSET            0xA8         /**< \brief (CAN_RXF0A offset) Rx FIFO 0 Acknowledge */
#define CAN_RXF0A_RESETVALUE        0x00000000u  /**< \brief (CAN_RXF0A reset_value) Rx FIFO 0 Acknowledge */

#define CAN_RXF0A_F0AI_Pos          0            /**< \brief (CAN_RXF0A) Rx FIFO 0 Acknowledge Index */
#define CAN_RXF0A_F0AI_Msk          (0x3Fu << CAN_RXF0A_F0AI_Pos)
#define CAN_RXF0A_F0AI(value)       (CAN_RXF0A_F0AI_Msk & ((value) << CAN_RXF0A_F0AI_Pos))
#define CAN_RXF0A_MASK              0x0000003Fu  /**< \brief (CAN_RXF0A) MASK Register */

/* -------- CAN_RXBC : (CAN Offset: 0xAC) (R/W 32) Rx Buffer Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t RBSA:16;          /*!< bit:  0..15  Rx Buffer Start Address            */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_RXBC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_RXBC_OFFSET             0xAC         /**< \brief (CAN_RXBC offset) Rx Buffer Configuration */
#define CAN_RXBC_RESETVALUE         0x00000000u  /**< \brief (CAN_RXBC reset_value) Rx Buffer Configuration */

#define CAN_RXBC_RBSA_Pos           0            /**< \brief (CAN_RXBC) Rx Buffer Start Address */
#define CAN_RXBC_RBSA_Msk           (0xFFFFu << CAN_RXBC_RBSA_Pos)
#define CAN_RXBC_RBSA(value)        (CAN_RXBC_RBSA_Msk & ((value) << CAN_RXBC_RBSA_Pos))
#define CAN_RXBC_MASK               0x0000FFFFu  /**< \brief (CAN_RXBC) MASK Register */

/* -------- CAN_RXF1C : (CAN Offset: 0xB0) (R/W 32) Rx FIFO 1 Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t F1SA:16;          /*!< bit:  0..15  Rx FIFO 1 Start Address            */
    uint32_t F1S:7;            /*!< bit: 16..22  Rx FIFO 1 Size                     */
    uint32_t :1;               /*!< bit:     23  Reserved                           */
    uint32_t F1WM:7;           /*!< bit: 24..30  Rx FIFO 1 Watermark                */
    uint32_t F1OM:1;           /*!< bit:     31  FIFO 1 Operation Mode              */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_RXF1C_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_RXF1C_OFFSET            0xB0         /**< \brief (CAN_RXF1C offset) Rx FIFO 1 Configuration */
#define CAN_RXF1C_RESETVALUE        0x00000000u  /**< \brief (CAN_RXF1C reset_value) Rx FIFO 1 Configuration */

#define CAN_RXF1C_F1SA_Pos          0            /**< \brief (CAN_RXF1C) Rx FIFO 1 Start Address */
#define CAN_RXF1C_F1SA_Msk          (0xFFFFu << CAN_RXF1C_F1SA_Pos)
#define CAN_RXF1C_F1SA(value)       (CAN_RXF1C_F1SA_Msk & ((value) << CAN_RXF1C_F1SA_Pos))
#define CAN_RXF1C_F1S_Pos           16           /**< \brief (CAN_RXF1C) Rx FIFO 1 Size */
#define CAN_RXF1C_F1S_Msk           (0x7Fu << CAN_RXF1C_F1S_Pos)
#define CAN_RXF1C_F1S(value)        (CAN_RXF1C_F1S_Msk & ((value) << CAN_RXF1C_F1S_Pos))
#define CAN_RXF1C_F1WM_Pos          24           /**< \brief (CAN_RXF1C) Rx FIFO 1 Watermark */
#define CAN_RXF1C_F1WM_Msk          (0x7Fu << CAN_RXF1C_F1WM_Pos)
#define CAN_RXF1C_F1WM(value)       (CAN_RXF1C_F1WM_Msk & ((value) << CAN_RXF1C_F1WM_Pos))
#define CAN_RXF1C_F1OM_Pos          31           /**< \brief (CAN_RXF1C) FIFO 1 Operation Mode */
#define CAN_RXF1C_F1OM              (0x1u << CAN_RXF1C_F1OM_Pos)
#define CAN_RXF1C_MASK              0xFF7FFFFFu  /**< \brief (CAN_RXF1C) MASK Register */

/* -------- CAN_RXF1S : (CAN Offset: 0xB4) (R/  32) Rx FIFO 1 Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t F1FL:7;           /*!< bit:  0.. 6  Rx FIFO 1 Fill Level               */
    uint32_t :1;               /*!< bit:      7  Reserved                           */
    uint32_t F1GI:6;           /*!< bit:  8..13  Rx FIFO 1 Get Index                */
    uint32_t :2;               /*!< bit: 14..15  Reserved                           */
    uint32_t F1PI:6;           /*!< bit: 16..21  Rx FIFO 1 Put Index                */
    uint32_t :2;               /*!< bit: 22..23  Reserved                           */
    uint32_t F1F:1;            /*!< bit:     24  Rx FIFO 1 Full                     */
    uint32_t RF1L:1;           /*!< bit:     25  Rx FIFO 1 Message Lost             */
    uint32_t :4;               /*!< bit: 26..29  Reserved                           */
    uint32_t DMS:2;            /*!< bit: 30..31  Debug Message Status               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_RXF1S_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_RXF1S_OFFSET            0xB4         /**< \brief (CAN_RXF1S offset) Rx FIFO 1 Status */
#define CAN_RXF1S_RESETVALUE        0x00000000u  /**< \brief (CAN_RXF1S reset_value) Rx FIFO 1 Status */

#define CAN_RXF1S_F1FL_Pos          0            /**< \brief (CAN_RXF1S) Rx FIFO 1 Fill Level */
#define CAN_RXF1S_F1FL_Msk          (0x7Fu << CAN_RXF1S_F1FL_Pos)
#define CAN_RXF1S_F1FL(value)       (CAN_RXF1S_F1FL_Msk & ((value) << CAN_RXF1S_F1FL_Pos))
#define CAN_RXF1S_F1GI_Pos          8            /**< \brief (CAN_RXF1S) Rx FIFO 1 Get Index */
#define CAN_RXF1S_F1GI_Msk          (0x3Fu << CAN_RXF1S_F1GI_Pos)
#define CAN_RXF1S_F1GI(value)       (CAN_RXF1S_F1GI_Msk & ((value) << CAN_RXF1S_F1GI_Pos))
#define CAN_RXF1S_F1PI_Pos          16           /**< \brief (CAN_RXF1S) Rx FIFO 1 Put Index */
#define CAN_RXF1S_F1PI_Msk          (0x3Fu << CAN_RXF1S_F1PI_Pos)
#define CAN_RXF1S_F1PI(value)       (CAN_RXF1S_F1PI_Msk & ((value) << CAN_RXF1S_F1PI_Pos))
#define CAN_RXF1S_F1F_Pos           24           /**< \brief (CAN_RXF1S) Rx FIFO 1 Full */
#define CAN_RXF1S_F1F               (0x1u << CAN_RXF1S_F1F_Pos)
#define CAN_RXF1S_RF1L_Pos          25           /**< \brief (CAN_RXF1S) Rx FIFO 1 Message Lost */
#define CAN_RXF1S_RF1L              (0x1u << CAN_RXF1S_RF1L_Pos)
#define CAN_RXF1S_DMS_Pos           30           /**< \brief (CAN_RXF1S) Debug Message Status */
#define CAN_RXF1S_DMS_Msk           (0x3u << CAN_RXF1S_DMS_Pos)
#define CAN_RXF1S_DMS(value)        (CAN_RXF1S_DMS_Msk & ((value) << CAN_RXF1S_DMS_Pos))
#define   CAN_RXF1S_DMS_IDLE_Val          0x0u   /**< \brief (CAN_RXF1S) Idle state */
#define   CAN_RXF1S_DMS_DBGA_Val          0x1u   /**< \brief (CAN_RXF1S) Debug message A received */
#define   CAN_RXF1S_DMS_DBGB_Val          0x2u   /**< \brief (CAN_RXF1S) Debug message A/B received */
#define   CAN_RXF1S_DMS_DBGC_Val          0x3u   /**< \brief (CAN_RXF1S) Debug message A/B/C received, DMA request set */
#define CAN_RXF1S_DMS_IDLE          (CAN_RXF1S_DMS_IDLE_Val        << CAN_RXF1S_DMS_Pos)
#define CAN_RXF1S_DMS_DBGA          (CAN_RXF1S_DMS_DBGA_Val        << CAN_RXF1S_DMS_Pos)
#define CAN_RXF1S_DMS_DBGB          (CAN_RXF1S_DMS_DBGB_Val        << CAN_RXF1S_DMS_Pos)
#define CAN_RXF1S_DMS_DBGC          (CAN_RXF1S_DMS_DBGC_Val        << CAN_RXF1S_DMS_Pos)
#define CAN_RXF1S_MASK              0xC33F3F7Fu  /**< \brief (CAN_RXF1S) MASK Register */

/* -------- CAN_RXF1A : (CAN Offset: 0xB8) (R/W 32) Rx FIFO 1 Acknowledge -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t F1AI:6;           /*!< bit:  0.. 5  Rx FIFO 1 Acknowledge Index        */
    uint32_t :26;              /*!< bit:  6..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_RXF1A_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_RXF1A_OFFSET            0xB8         /**< \brief (CAN_RXF1A offset) Rx FIFO 1 Acknowledge */
#define CAN_RXF1A_RESETVALUE        0x00000000u  /**< \brief (CAN_RXF1A reset_value) Rx FIFO 1 Acknowledge */

#define CAN_RXF1A_F1AI_Pos          0            /**< \brief (CAN_RXF1A) Rx FIFO 1 Acknowledge Index */
#define CAN_RXF1A_F1AI_Msk          (0x3Fu << CAN_RXF1A_F1AI_Pos)
#define CAN_RXF1A_F1AI(value)       (CAN_RXF1A_F1AI_Msk & ((value) << CAN_RXF1A_F1AI_Pos))
#define CAN_RXF1A_MASK              0x0000003Fu  /**< \brief (CAN_RXF1A) MASK Register */

/* -------- CAN_RXESC : (CAN Offset: 0xBC) (R/W 32) Rx Buffer / FIFO Element Size Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t F0DS:3;           /*!< bit:  0.. 2  Rx FIFO 0 Data Field Size          */
    uint32_t :1;               /*!< bit:      3  Reserved                           */
    uint32_t F1DS:3;           /*!< bit:  4.. 6  Rx FIFO 1 Data Field Size          */
    uint32_t :1;               /*!< bit:      7  Reserved                           */
    uint32_t RBDS:3;           /*!< bit:  8..10  Rx Buffer Data Field Size          */
    uint32_t :21;              /*!< bit: 11..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_RXESC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_RXESC_OFFSET            0xBC         /**< \brief (CAN_RXESC offset) Rx Buffer / FIFO Element Size Configuration */
#define CAN_RXESC_RESETVALUE        0x00000000u  /**< \brief (CAN_RXESC reset_value) Rx Buffer / FIFO Element Size Configuration */

#define CAN_RXESC_F0DS_Pos          0            /**< \brief (CAN_RXESC) Rx FIFO 0 Data Field Size */
#define CAN_RXESC_F0DS_Msk          (0x7u << CAN_RXESC_F0DS_Pos)
#define CAN_RXESC_F0DS(value)       (CAN_RXESC_F0DS_Msk & ((value) << CAN_RXESC_F0DS_Pos))
#define   CAN_RXESC_F0DS_DATA8_Val        0x0u   /**< \brief (CAN_RXESC) 8 byte data field */
#define   CAN_RXESC_F0DS_DATA12_Val       0x1u   /**< \brief (CAN_RXESC) 12 byte data field */
#define   CAN_RXESC_F0DS_DATA16_Val       0x2u   /**< \brief (CAN_RXESC) 16 byte data field */
#define   CAN_RXESC_F0DS_DATA20_Val       0x3u   /**< \brief (CAN_RXESC) 20 byte data field */
#define   CAN_RXESC_F0DS_DATA24_Val       0x4u   /**< \brief (CAN_RXESC) 24 byte data field */
#define   CAN_RXESC_F0DS_DATA32_Val       0x5u   /**< \brief (CAN_RXESC) 32 byte data field */
#define   CAN_RXESC_F0DS_DATA48_Val       0x6u   /**< \brief (CAN_RXESC) 48 byte data field */
#define   CAN_RXESC_F0DS_DATA64_Val       0x7u   /**< \brief (CAN_RXESC) 64 byte data field */
#define CAN_RXESC_F0DS_DATA8        (CAN_RXESC_F0DS_DATA8_Val      << CAN_RXESC_F0DS_Pos)
#define CAN_RXESC_F0DS_DATA12       (CAN_RXESC_F0DS_DATA12_Val     << CAN_RXESC_F0DS_Pos)
#define CAN_RXESC_F0DS_DATA16       (CAN_RXESC_F0DS_DATA16_Val     << CAN_RXESC_F0DS_Pos)
#define CAN_RXESC_F0DS_DATA20       (CAN_RXESC_F0DS_DATA20_Val     << CAN_RXESC_F0DS_Pos)
#define CAN_RXESC_F0DS_DATA24       (CAN_RXESC_F0DS_DATA24_Val     << CAN_RXESC_F0DS_Pos)
#define CAN_RXESC_F0DS_DATA32       (CAN_RXESC_F0DS_DATA32_Val     << CAN_RXESC_F0DS_Pos)
#define CAN_RXESC_F0DS_DATA48       (CAN_RXESC_F0DS_DATA48_Val     << CAN_RXESC_F0DS_Pos)
#define CAN_RXESC_F0DS_DATA64       (CAN_RXESC_F0DS_DATA64_Val     << CAN_RXESC_F0DS_Pos)
#define CAN_RXESC_F1DS_Pos          4            /**< \brief (CAN_RXESC) Rx FIFO 1 Data Field Size */
#define CAN_RXESC_F1DS_Msk          (0x7u << CAN_RXESC_F1DS_Pos)
#define CAN_RXESC_F1DS(value)       (CAN_RXESC_F1DS_Msk & ((value) << CAN_RXESC_F1DS_Pos))
#define   CAN_RXESC_F1DS_DATA8_Val        0x0u   /**< \brief (CAN_RXESC) 8 byte data field */
#define   CAN_RXESC_F1DS_DATA12_Val       0x1u   /**< \brief (CAN_RXESC) 12 byte data field */
#define   CAN_RXESC_F1DS_DATA16_Val       0x2u   /**< \brief (CAN_RXESC) 16 byte data field */
#define   CAN_RXESC_F1DS_DATA20_Val       0x3u   /**< \brief (CAN_RXESC) 20 byte data field */
#define   CAN_RXESC_F1DS_DATA24_Val       0x4u   /**< \brief (CAN_RXESC) 24 byte data field */
#define   CAN_RXESC_F1DS_DATA32_Val       0x5u   /**< \brief (CAN_RXESC) 32 byte data field */
#define   CAN_RXESC_F1DS_DATA48_Val       0x6u   /**< \brief (CAN_RXESC) 48 byte data field */
#define   CAN_RXESC_F1DS_DATA64_Val       0x7u   /**< \brief (CAN_RXESC) 64 byte data field */
#define CAN_RXESC_F1DS_DATA8        (CAN_RXESC_F1DS_DATA8_Val      << CAN_RXESC_F1DS_Pos)
#define CAN_RXESC_F1DS_DATA12       (CAN_RXESC_F1DS_DATA12_Val     << CAN_RXESC_F1DS_Pos)
#define CAN_RXESC_F1DS_DATA16       (CAN_RXESC_F1DS_DATA16_Val     << CAN_RXESC_F1DS_Pos)
#define CAN_RXESC_F1DS_DATA20       (CAN_RXESC_F1DS_DATA20_Val     << CAN_RXESC_F1DS_Pos)
#define CAN_RXESC_F1DS_DATA24       (CAN_RXESC_F1DS_DATA24_Val     << CAN_RXESC_F1DS_Pos)
#define CAN_RXESC_F1DS_DATA32       (CAN_RXESC_F1DS_DATA32_Val     << CAN_RXESC_F1DS_Pos)
#define CAN_RXESC_F1DS_DATA48       (CAN_RXESC_F1DS_DATA48_Val     << CAN_RXESC_F1DS_Pos)
#define CAN_RXESC_F1DS_DATA64       (CAN_RXESC_F1DS_DATA64_Val     << CAN_RXESC_F1DS_Pos)
#define CAN_RXESC_RBDS_Pos          8            /**< \brief (CAN_RXESC) Rx Buffer Data Field Size */
#define CAN_RXESC_RBDS_Msk          (0x7u << CAN_RXESC_RBDS_Pos)
#define CAN_RXESC_RBDS(value)       (CAN_RXESC_RBDS_Msk & ((value) << CAN_RXESC_RBDS_Pos))
#define   CAN_RXESC_RBDS_DATA8_Val        0x0u   /**< \brief (CAN_RXESC) 8 byte data field */
#define   CAN_RXESC_RBDS_DATA12_Val       0x1u   /**< \brief (CAN_RXESC) 12 byte data field */
#define   CAN_RXESC_RBDS_DATA16_Val       0x2u   /**< \brief (CAN_RXESC) 16 byte data field */
#define   CAN_RXESC_RBDS_DATA20_Val       0x3u   /**< \brief (CAN_RXESC) 20 byte data field */
#define   CAN_RXESC_RBDS_DATA24_Val       0x4u   /**< \brief (CAN_RXESC) 24 byte data field */
#define   CAN_RXESC_RBDS_DATA32_Val       0x5u   /**< \brief (CAN_RXESC) 32 byte data field */
#define   CAN_RXESC_RBDS_DATA48_Val       0x6u   /**< \brief (CAN_RXESC) 48 byte data field */
#define   CAN_RXESC_RBDS_DATA64_Val       0x7u   /**< \brief (CAN_RXESC) 64 byte data field */
#define CAN_RXESC_RBDS_DATA8        (CAN_RXESC_RBDS_DATA8_Val      << CAN_RXESC_RBDS_Pos)
#define CAN_RXESC_RBDS_DATA12       (CAN_RXESC_RBDS_DATA12_Val     << CAN_RXESC_RBDS_Pos)
#define CAN_RXESC_RBDS_DATA16       (CAN_RXESC_RBDS_DATA16_Val     << CAN_RXESC_RBDS_Pos)
#define CAN_RXESC_RBDS_DATA20       (CAN_RXESC_RBDS_DATA20_Val     << CAN_RXESC_RBDS_Pos)
#define CAN_RXESC_RBDS_DATA24       (CAN_RXESC_RBDS_DATA24_Val     << CAN_RXESC_RBDS_Pos)
#define CAN_RXESC_RBDS_DATA32       (CAN_RXESC_RBDS_DATA32_Val     << CAN_RXESC_RBDS_Pos)
#define CAN_RXESC_RBDS_DATA48       (CAN_RXESC_RBDS_DATA48_Val     << CAN_RXESC_RBDS_Pos)
#define CAN_RXESC_RBDS_DATA64       (CAN_RXESC_RBDS_DATA64_Val     << CAN_RXESC_RBDS_Pos)
#define CAN_RXESC_MASK              0x00000777u  /**< \brief (CAN_RXESC) MASK Register */

/* -------- CAN_TXBC : (CAN Offset: 0xC0) (R/W 32) Tx Buffer Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t TBSA:16;          /*!< bit:  0..15  Tx Buffers Start Address           */
    uint32_t NDTB:6;           /*!< bit: 16..21  Number of Dedicated Transmit Buffers */
    uint32_t :2;               /*!< bit: 22..23  Reserved                           */
    uint32_t TFQS:6;           /*!< bit: 24..29  Transmit FIFO/Queue Size           */
    uint32_t TFQM:1;           /*!< bit:     30  Tx FIFO/Queue Mode                 */
    uint32_t :1;               /*!< bit:     31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TXBC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TXBC_OFFSET             0xC0         /**< \brief (CAN_TXBC offset) Tx Buffer Configuration */
#define CAN_TXBC_RESETVALUE         0x00000000u  /**< \brief (CAN_TXBC reset_value) Tx Buffer Configuration */

#define CAN_TXBC_TBSA_Pos           0            /**< \brief (CAN_TXBC) Tx Buffers Start Address */
#define CAN_TXBC_TBSA_Msk           (0xFFFFu << CAN_TXBC_TBSA_Pos)
#define CAN_TXBC_TBSA(value)        (CAN_TXBC_TBSA_Msk & ((value) << CAN_TXBC_TBSA_Pos))
#define CAN_TXBC_NDTB_Pos           16           /**< \brief (CAN_TXBC) Number of Dedicated Transmit Buffers */
#define CAN_TXBC_NDTB_Msk           (0x3Fu << CAN_TXBC_NDTB_Pos)
#define CAN_TXBC_NDTB(value)        (CAN_TXBC_NDTB_Msk & ((value) << CAN_TXBC_NDTB_Pos))
#define CAN_TXBC_TFQS_Pos           24           /**< \brief (CAN_TXBC) Transmit FIFO/Queue Size */
#define CAN_TXBC_TFQS_Msk           (0x3Fu << CAN_TXBC_TFQS_Pos)
#define CAN_TXBC_TFQS(value)        (CAN_TXBC_TFQS_Msk & ((value) << CAN_TXBC_TFQS_Pos))
#define CAN_TXBC_TFQM_Pos           30           /**< \brief (CAN_TXBC) Tx FIFO/Queue Mode */
#define CAN_TXBC_TFQM               (0x1u << CAN_TXBC_TFQM_Pos)
#define CAN_TXBC_MASK               0x7F3FFFFFu  /**< \brief (CAN_TXBC) MASK Register */

/* -------- CAN_TXFQS : (CAN Offset: 0xC4) (R/  32) Tx FIFO / Queue Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t TFFL:6;           /*!< bit:  0.. 5  Tx FIFO Free Level                 */
    uint32_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint32_t TFGI:5;           /*!< bit:  8..12  Tx FIFO Get Index                  */
    uint32_t :3;               /*!< bit: 13..15  Reserved                           */
    uint32_t TFQPI:5;          /*!< bit: 16..20  Tx FIFO/Queue Put Index            */
    uint32_t TFQF:1;           /*!< bit:     21  Tx FIFO/Queue Full                 */
    uint32_t :10;              /*!< bit: 22..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TXFQS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TXFQS_OFFSET            0xC4         /**< \brief (CAN_TXFQS offset) Tx FIFO / Queue Status */
#define CAN_TXFQS_RESETVALUE        0x00000000u  /**< \brief (CAN_TXFQS reset_value) Tx FIFO / Queue Status */

#define CAN_TXFQS_TFFL_Pos          0            /**< \brief (CAN_TXFQS) Tx FIFO Free Level */
#define CAN_TXFQS_TFFL_Msk          (0x3Fu << CAN_TXFQS_TFFL_Pos)
#define CAN_TXFQS_TFFL(value)       (CAN_TXFQS_TFFL_Msk & ((value) << CAN_TXFQS_TFFL_Pos))
#define CAN_TXFQS_TFGI_Pos          8            /**< \brief (CAN_TXFQS) Tx FIFO Get Index */
#define CAN_TXFQS_TFGI_Msk          (0x1Fu << CAN_TXFQS_TFGI_Pos)
#define CAN_TXFQS_TFGI(value)       (CAN_TXFQS_TFGI_Msk & ((value) << CAN_TXFQS_TFGI_Pos))
#define CAN_TXFQS_TFQPI_Pos         16           /**< \brief (CAN_TXFQS) Tx FIFO/Queue Put Index */
#define CAN_TXFQS_TFQPI_Msk         (0x1Fu << CAN_TXFQS_TFQPI_Pos)
#define CAN_TXFQS_TFQPI(value)      (CAN_TXFQS_TFQPI_Msk & ((value) << CAN_TXFQS_TFQPI_Pos))
#define CAN_TXFQS_TFQF_Pos          21           /**< \brief (CAN_TXFQS) Tx FIFO/Queue Full */
#define CAN_TXFQS_TFQF              (0x1u << CAN_TXFQS_TFQF_Pos)
#define CAN_TXFQS_MASK              0x003F1F3Fu  /**< \brief (CAN_TXFQS) MASK Register */

/* -------- CAN_TXESC : (CAN Offset: 0xC8) (R/W 32) Tx Buffer Element Size Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t TBDS:3;           /*!< bit:  0.. 2  Tx Buffer Data Field Size          */
    uint32_t :29;              /*!< bit:  3..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TXESC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TXESC_OFFSET            0xC8         /**< \brief (CAN_TXESC offset) Tx Buffer Element Size Configuration */
#define CAN_TXESC_RESETVALUE        0x00000000u  /**< \brief (CAN_TXESC reset_value) Tx Buffer Element Size Configuration */

#define CAN_TXESC_TBDS_Pos          0            /**< \brief (CAN_TXESC) Tx Buffer Data Field Size */
#define CAN_TXESC_TBDS_Msk          (0x7u << CAN_TXESC_TBDS_Pos)
#define CAN_TXESC_TBDS(value)       (CAN_TXESC_TBDS_Msk & ((value) << CAN_TXESC_TBDS_Pos))
#define   CAN_TXESC_TBDS_DATA8_Val        0x0u   /**< \brief (CAN_TXESC) 8 byte data field */
#define   CAN_TXESC_TBDS_DATA12_Val       0x1u   /**< \brief (CAN_TXESC) 12 byte data field */
#define   CAN_TXESC_TBDS_DATA16_Val       0x2u   /**< \brief (CAN_TXESC) 16 byte data field */
#define   CAN_TXESC_TBDS_DATA20_Val       0x3u   /**< \brief (CAN_TXESC) 20 byte data field */
#define   CAN_TXESC_TBDS_DATA24_Val       0x4u   /**< \brief (CAN_TXESC) 24 byte data field */
#define   CAN_TXESC_TBDS_DATA32_Val       0x5u   /**< \brief (CAN_TXESC) 32 byte data field */
#define   CAN_TXESC_TBDS_DATA48_Val       0x6u   /**< \brief (CAN_TXESC) 48 byte data field */
#define   CAN_TXESC_TBDS_DATA64_Val       0x7u   /**< \brief (CAN_TXESC) 64 byte data field */
#define CAN_TXESC_TBDS_DATA8        (CAN_TXESC_TBDS_DATA8_Val      << CAN_TXESC_TBDS_Pos)
#define CAN_TXESC_TBDS_DATA12       (CAN_TXESC_TBDS_DATA12_Val     << CAN_TXESC_TBDS_Pos)
#define CAN_TXESC_TBDS_DATA16       (CAN_TXESC_TBDS_DATA16_Val     << CAN_TXESC_TBDS_Pos)
#define CAN_TXESC_TBDS_DATA20       (CAN_TXESC_TBDS_DATA20_Val     << CAN_TXESC_TBDS_Pos)
#define CAN_TXESC_TBDS_DATA24       (CAN_TXESC_TBDS_DATA24_Val     << CAN_TXESC_TBDS_Pos)
#define CAN_TXESC_TBDS_DATA32       (CAN_TXESC_TBDS_DATA32_Val     << CAN_TXESC_TBDS_Pos)
#define CAN_TXESC_TBDS_DATA48       (CAN_TXESC_TBDS_DATA48_Val     << CAN_TXESC_TBDS_Pos)
#define CAN_TXESC_TBDS_DATA64       (CAN_TXESC_TBDS_DATA64_Val     << CAN_TXESC_TBDS_Pos)
#define CAN_TXESC_MASK              0x00000007u  /**< \brief (CAN_TXESC) MASK Register */

/* -------- CAN_TXBRP : (CAN Offset: 0xCC) (R/  32) Tx Buffer Request Pending -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t TRP0:1;           /*!< bit:      0  Transmission Request Pending 0     */
    uint32_t TRP1:1;           /*!< bit:      1  Transmission Request Pending 1     */
    uint32_t TRP2:1;           /*!< bit:      2  Transmission Request Pending 2     */
    uint32_t TRP3:1;           /*!< bit:      3  Transmission Request Pending 3     */
    uint32_t TRP4:1;           /*!< bit:      4  Transmission Request Pending 4     */
    uint32_t TRP5:1;           /*!< bit:      5  Transmission Request Pending 5     */
    uint32_t TRP6:1;           /*!< bit:      6  Transmission Request Pending 6     */
    uint32_t TRP7:1;           /*!< bit:      7  Transmission Request Pending 7     */
    uint32_t TRP8:1;           /*!< bit:      8  Transmission Request Pending 8     */
    uint32_t TRP9:1;           /*!< bit:      9  Transmission Request Pending 9     */
    uint32_t TRP10:1;          /*!< bit:     10  Transmission Request Pending 10    */
    uint32_t TRP11:1;          /*!< bit:     11  Transmission Request Pending 11    */
    uint32_t TRP12:1;          /*!< bit:     12  Transmission Request Pending 12    */
    uint32_t TRP13:1;          /*!< bit:     13  Transmission Request Pending 13    */
    uint32_t TRP14:1;          /*!< bit:     14  Transmission Request Pending 14    */
    uint32_t TRP15:1;          /*!< bit:     15  Transmission Request Pending 15    */
    uint32_t TRP16:1;          /*!< bit:     16  Transmission Request Pending 16    */
    uint32_t TRP17:1;          /*!< bit:     17  Transmission Request Pending 17    */
    uint32_t TRP18:1;          /*!< bit:     18  Transmission Request Pending 18    */
    uint32_t TRP19:1;          /*!< bit:     19  Transmission Request Pending 19    */
    uint32_t TRP20:1;          /*!< bit:     20  Transmission Request Pending 20    */
    uint32_t TRP21:1;          /*!< bit:     21  Transmission Request Pending 21    */
    uint32_t TRP22:1;          /*!< bit:     22  Transmission Request Pending 22    */
    uint32_t TRP23:1;          /*!< bit:     23  Transmission Request Pending 23    */
    uint32_t TRP24:1;          /*!< bit:     24  Transmission Request Pending 24    */
    uint32_t TRP25:1;          /*!< bit:     25  Transmission Request Pending 25    */
    uint32_t TRP26:1;          /*!< bit:     26  Transmission Request Pending 26    */
    uint32_t TRP27:1;          /*!< bit:     27  Transmission Request Pending 27    */
    uint32_t TRP28:1;          /*!< bit:     28  Transmission Request Pending 28    */
    uint32_t TRP29:1;          /*!< bit:     29  Transmission Request Pending 29    */
    uint32_t TRP30:1;          /*!< bit:     30  Transmission Request Pending 30    */
    uint32_t TRP31:1;          /*!< bit:     31  Transmission Request Pending 31    */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TXBRP_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TXBRP_OFFSET            0xCC         /**< \brief (CAN_TXBRP offset) Tx Buffer Request Pending */
#define CAN_TXBRP_RESETVALUE        0x00000000u  /**< \brief (CAN_TXBRP reset_value) Tx Buffer Request Pending */

#define CAN_TXBRP_TRP0_Pos          0            /**< \brief (CAN_TXBRP) Transmission Request Pending 0 */
#define CAN_TXBRP_TRP0              (0x1u << CAN_TXBRP_TRP0_Pos)
#define CAN_TXBRP_TRP1_Pos          1            /**< \brief (CAN_TXBRP) Transmission Request Pending 1 */
#define CAN_TXBRP_TRP1              (0x1u << CAN_TXBRP_TRP1_Pos)
#define CAN_TXBRP_TRP2_Pos          2            /**< \brief (CAN_TXBRP) Transmission Request Pending 2 */
#define CAN_TXBRP_TRP2              (0x1u << CAN_TXBRP_TRP2_Pos)
#define CAN_TXBRP_TRP3_Pos          3            /**< \brief (CAN_TXBRP) Transmission Request Pending 3 */
#define CAN_TXBRP_TRP3              (0x1u << CAN_TXBRP_TRP3_Pos)
#define CAN_TXBRP_TRP4_Pos          4            /**< \brief (CAN_TXBRP) Transmission Request Pending 4 */
#define CAN_TXBRP_TRP4              (0x1u << CAN_TXBRP_TRP4_Pos)
#define CAN_TXBRP_TRP5_Pos          5            /**< \brief (CAN_TXBRP) Transmission Request Pending 5 */
#define CAN_TXBRP_TRP5              (0x1u << CAN_TXBRP_TRP5_Pos)
#define CAN_TXBRP_TRP6_Pos          6            /**< \brief (CAN_TXBRP) Transmission Request Pending 6 */
#define CAN_TXBRP_TRP6              (0x1u << CAN_TXBRP_TRP6_Pos)
#define CAN_TXBRP_TRP7_Pos          7            /**< \brief (CAN_TXBRP) Transmission Request Pending 7 */
#define CAN_TXBRP_TRP7              (0x1u << CAN_TXBRP_TRP7_Pos)
#define CAN_TXBRP_TRP8_Pos          8            /**< \brief (CAN_TXBRP) Transmission Request Pending 8 */
#define CAN_TXBRP_TRP8              (0x1u << CAN_TXBRP_TRP8_Pos)
#define CAN_TXBRP_TRP9_Pos          9            /**< \brief (CAN_TXBRP) Transmission Request Pending 9 */
#define CAN_TXBRP_TRP9              (0x1u << CAN_TXBRP_TRP9_Pos)
#define CAN_TXBRP_TRP10_Pos         10           /**< \brief (CAN_TXBRP) Transmission Request Pending 10 */
#define CAN_TXBRP_TRP10             (0x1u << CAN_TXBRP_TRP10_Pos)
#define CAN_TXBRP_TRP11_Pos         11           /**< \brief (CAN_TXBRP) Transmission Request Pending 11 */
#define CAN_TXBRP_TRP11             (0x1u << CAN_TXBRP_TRP11_Pos)
#define CAN_TXBRP_TRP12_Pos         12           /**< \brief (CAN_TXBRP) Transmission Request Pending 12 */
#define CAN_TXBRP_TRP12             (0x1u << CAN_TXBRP_TRP12_Pos)
#define CAN_TXBRP_TRP13_Pos         13           /**< \brief (CAN_TXBRP) Transmission Request Pending 13 */
#define CAN_TXBRP_TRP13             (0x1u << CAN_TXBRP_TRP13_Pos)
#define CAN_TXBRP_TRP14_Pos         14           /**< \brief (CAN_TXBRP) Transmission Request Pending 14 */
#define CAN_TXBRP_TRP14             (0x1u << CAN_TXBRP_TRP14_Pos)
#define CAN_TXBRP_TRP15_Pos         15           /**< \brief (CAN_TXBRP) Transmission Request Pending 15 */
#define CAN_TXBRP_TRP15             (0x1u << CAN_TXBRP_TRP15_Pos)
#define CAN_TXBRP_TRP16_Pos         16           /**< \brief (CAN_TXBRP) Transmission Request Pending 16 */
#define CAN_TXBRP_TRP16             (0x1u << CAN_TXBRP_TRP16_Pos)
#define CAN_TXBRP_TRP17_Pos         17           /**< \brief (CAN_TXBRP) Transmission Request Pending 17 */
#define CAN_TXBRP_TRP17             (0x1u << CAN_TXBRP_TRP17_Pos)
#define CAN_TXBRP_TRP18_Pos         18           /**< \brief (CAN_TXBRP) Transmission Request Pending 18 */
#define CAN_TXBRP_TRP18             (0x1u << CAN_TXBRP_TRP18_Pos)
#define CAN_TXBRP_TRP19_Pos         19           /**< \brief (CAN_TXBRP) Transmission Request Pending 19 */
#define CAN_TXBRP_TRP19             (0x1u << CAN_TXBRP_TRP19_Pos)
#define CAN_TXBRP_TRP20_Pos         20           /**< \brief (CAN_TXBRP) Transmission Request Pending 20 */
#define CAN_TXBRP_TRP20             (0x1u << CAN_TXBRP_TRP20_Pos)
#define CAN_TXBRP_TRP21_Pos         21           /**< \brief (CAN_TXBRP) Transmission Request Pending 21 */
#define CAN_TXBRP_TRP21             (0x1u << CAN_TXBRP_TRP21_Pos)
#define CAN_TXBRP_TRP22_Pos         22           /**< \brief (CAN_TXBRP) Transmission Request Pending 22 */
#define CAN_TXBRP_TRP22             (0x1u << CAN_TXBRP_TRP22_Pos)
#define CAN_TXBRP_TRP23_Pos         23           /**< \brief (CAN_TXBRP) Transmission Request Pending 23 */
#define CAN_TXBRP_TRP23             (0x1u << CAN_TXBRP_TRP23_Pos)
#define CAN_TXBRP_TRP24_Pos         24           /**< \brief (CAN_TXBRP) Transmission Request Pending 24 */
#define CAN_TXBRP_TRP24             (0x1u << CAN_TXBRP_TRP24_Pos)
#define CAN_TXBRP_TRP25_Pos         25           /**< \brief (CAN_TXBRP) Transmission Request Pending 25 */
#define CAN_TXBRP_TRP25             (0x1u << CAN_TXBRP_TRP25_Pos)
#define CAN_TXBRP_TRP26_Pos         26           /**< \brief (CAN_TXBRP) Transmission Request Pending 26 */
#define CAN_TXBRP_TRP26             (0x1u << CAN_TXBRP_TRP26_Pos)
#define CAN_TXBRP_TRP27_Pos         27           /**< \brief (CAN_TXBRP) Transmission Request Pending 27 */
#define CAN_TXBRP_TRP27             (0x1u << CAN_TXBRP_TRP27_Pos)
#define CAN_TXBRP_TRP28_Pos         28           /**< \brief (CAN_TXBRP) Transmission Request Pending 28 */
#define CAN_TXBRP_TRP28             (0x1u << CAN_TXBRP_TRP28_Pos)
#define CAN_TXBRP_TRP29_Pos         29           /**< \brief (CAN_TXBRP) Transmission Request Pending 29 */
#define CAN_TXBRP_TRP29             (0x1u << CAN_TXBRP_TRP29_Pos)
#define CAN_TXBRP_TRP30_Pos         30           /**< \brief (CAN_TXBRP) Transmission Request Pending 30 */
#define CAN_TXBRP_TRP30             (0x1u << CAN_TXBRP_TRP30_Pos)
#define CAN_TXBRP_TRP31_Pos         31           /**< \brief (CAN_TXBRP) Transmission Request Pending 31 */
#define CAN_TXBRP_TRP31             (0x1u << CAN_TXBRP_TRP31_Pos)
#define CAN_TXBRP_MASK              0xFFFFFFFFu  /**< \brief (CAN_TXBRP) MASK Register */

/* -------- CAN_TXBAR : (CAN Offset: 0xD0) (R/W 32) Tx Buffer Add Request -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t AR0:1;            /*!< bit:      0  Add Request 0                      */
    uint32_t AR1:1;            /*!< bit:      1  Add Request 1                      */
    uint32_t AR2:1;            /*!< bit:      2  Add Request 2                      */
    uint32_t AR3:1;            /*!< bit:      3  Add Request 3                      */
    uint32_t AR4:1;            /*!< bit:      4  Add Request 4                      */
    uint32_t AR5:1;            /*!< bit:      5  Add Request 5                      */
    uint32_t AR6:1;            /*!< bit:      6  Add Request 6                      */
    uint32_t AR7:1;            /*!< bit:      7  Add Request 7                      */
    uint32_t AR8:1;            /*!< bit:      8  Add Request 8                      */
    uint32_t AR9:1;            /*!< bit:      9  Add Request 9                      */
    uint32_t AR10:1;           /*!< bit:     10  Add Request 10                     */
    uint32_t AR11:1;           /*!< bit:     11  Add Request 11                     */
    uint32_t AR12:1;           /*!< bit:     12  Add Request 12                     */
    uint32_t AR13:1;           /*!< bit:     13  Add Request 13                     */
    uint32_t AR14:1;           /*!< bit:     14  Add Request 14                     */
    uint32_t AR15:1;           /*!< bit:     15  Add Request 15                     */
    uint32_t AR16:1;           /*!< bit:     16  Add Request 16                     */
    uint32_t AR17:1;           /*!< bit:     17  Add Request 17                     */
    uint32_t AR18:1;           /*!< bit:     18  Add Request 18                     */
    uint32_t AR19:1;           /*!< bit:     19  Add Request 19                     */
    uint32_t AR20:1;           /*!< bit:     20  Add Request 20                     */
    uint32_t AR21:1;           /*!< bit:     21  Add Request 21                     */
    uint32_t AR22:1;           /*!< bit:     22  Add Request 22                     */
    uint32_t AR23:1;           /*!< bit:     23  Add Request 23                     */
    uint32_t AR24:1;           /*!< bit:     24  Add Request 24                     */
    uint32_t AR25:1;           /*!< bit:     25  Add Request 25                     */
    uint32_t AR26:1;           /*!< bit:     26  Add Request 26                     */
    uint32_t AR27:1;           /*!< bit:     27  Add Request 27                     */
    uint32_t AR28:1;           /*!< bit:     28  Add Request 28                     */
    uint32_t AR29:1;           /*!< bit:     29  Add Request 29                     */
    uint32_t AR30:1;           /*!< bit:     30  Add Request 30                     */
    uint32_t AR31:1;           /*!< bit:     31  Add Request 31                     */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TXBAR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TXBAR_OFFSET            0xD0         /**< \brief (CAN_TXBAR offset) Tx Buffer Add Request */
#define CAN_TXBAR_RESETVALUE        0x00000000u  /**< \brief (CAN_TXBAR reset_value) Tx Buffer Add Request */

#define CAN_TXBAR_AR0_Pos           0            /**< \brief (CAN_TXBAR) Add Request 0 */
#define CAN_TXBAR_AR0               (0x1u << CAN_TXBAR_AR0_Pos)
#define CAN_TXBAR_AR1_Pos           1            /**< \brief (CAN_TXBAR) Add Request 1 */
#define CAN_TXBAR_AR1               (0x1u << CAN_TXBAR_AR1_Pos)
#define CAN_TXBAR_AR2_Pos           2            /**< \brief (CAN_TXBAR) Add Request 2 */
#define CAN_TXBAR_AR2               (0x1u << CAN_TXBAR_AR2_Pos)
#define CAN_TXBAR_AR3_Pos           3            /**< \brief (CAN_TXBAR) Add Request 3 */
#define CAN_TXBAR_AR3               (0x1u << CAN_TXBAR_AR3_Pos)
#define CAN_TXBAR_AR4_Pos           4            /**< \brief (CAN_TXBAR) Add Request 4 */
#define CAN_TXBAR_AR4               (0x1u << CAN_TXBAR_AR4_Pos)
#define CAN_TXBAR_AR5_Pos           5            /**< \brief (CAN_TXBAR) Add Request 5 */
#define CAN_TXBAR_AR5               (0x1u << CAN_TXBAR_AR5_Pos)
#define CAN_TXBAR_AR6_Pos           6            /**< \brief (CAN_TXBAR) Add Request 6 */
#define CAN_TXBAR_AR6               (0x1u << CAN_TXBAR_AR6_Pos)
#define CAN_TXBAR_AR7_Pos           7            /**< \brief (CAN_TXBAR) Add Request 7 */
#define CAN_TXBAR_AR7               (0x1u << CAN_TXBAR_AR7_Pos)
#define CAN_TXBAR_AR8_Pos           8            /**< \brief (CAN_TXBAR) Add Request 8 */
#define CAN_TXBAR_AR8               (0x1u << CAN_TXBAR_AR8_Pos)
#define CAN_TXBAR_AR9_Pos           9            /**< \brief (CAN_TXBAR) Add Request 9 */
#define CAN_TXBAR_AR9               (0x1u << CAN_TXBAR_AR9_Pos)
#define CAN_TXBAR_AR10_Pos          10           /**< \brief (CAN_TXBAR) Add Request 10 */
#define CAN_TXBAR_AR10              (0x1u << CAN_TXBAR_AR10_Pos)
#define CAN_TXBAR_AR11_Pos          11           /**< \brief (CAN_TXBAR) Add Request 11 */
#define CAN_TXBAR_AR11              (0x1u << CAN_TXBAR_AR11_Pos)
#define CAN_TXBAR_AR12_Pos          12           /**< \brief (CAN_TXBAR) Add Request 12 */
#define CAN_TXBAR_AR12              (0x1u << CAN_TXBAR_AR12_Pos)
#define CAN_TXBAR_AR13_Pos          13           /**< \brief (CAN_TXBAR) Add Request 13 */
#define CAN_TXBAR_AR13              (0x1u << CAN_TXBAR_AR13_Pos)
#define CAN_TXBAR_AR14_Pos          14           /**< \brief (CAN_TXBAR) Add Request 14 */
#define CAN_TXBAR_AR14              (0x1u << CAN_TXBAR_AR14_Pos)
#define CAN_TXBAR_AR15_Pos          15           /**< \brief (CAN_TXBAR) Add Request 15 */
#define CAN_TXBAR_AR15              (0x1u << CAN_TXBAR_AR15_Pos)
#define CAN_TXBAR_AR16_Pos          16           /**< \brief (CAN_TXBAR) Add Request 16 */
#define CAN_TXBAR_AR16              (0x1u << CAN_TXBAR_AR16_Pos)
#define CAN_TXBAR_AR17_Pos          17           /**< \brief (CAN_TXBAR) Add Request 17 */
#define CAN_TXBAR_AR17              (0x1u << CAN_TXBAR_AR17_Pos)
#define CAN_TXBAR_AR18_Pos          18           /**< \brief (CAN_TXBAR) Add Request 18 */
#define CAN_TXBAR_AR18              (0x1u << CAN_TXBAR_AR18_Pos)
#define CAN_TXBAR_AR19_Pos          19           /**< \brief (CAN_TXBAR) Add Request 19 */
#define CAN_TXBAR_AR19              (0x1u << CAN_TXBAR_AR19_Pos)
#define CAN_TXBAR_AR20_Pos          20           /**< \brief (CAN_TXBAR) Add Request 20 */
#define CAN_TXBAR_AR20              (0x1u << CAN_TXBAR_AR20_Pos)
#define CAN_TXBAR_AR21_Pos          21           /**< \brief (CAN_TXBAR) Add Request 21 */
#define CAN_TXBAR_AR21              (0x1u << CAN_TXBAR_AR21_Pos)
#define CAN_TXBAR_AR22_Pos          22           /**< \brief (CAN_TXBAR) Add Request 22 */
#define CAN_TXBAR_AR22              (0x1u << CAN_TXBAR_AR22_Pos)
#define CAN_TXBAR_AR23_Pos          23           /**< \brief (CAN_TXBAR) Add Request 23 */
#define CAN_TXBAR_AR23              (0x1u << CAN_TXBAR_AR23_Pos)
#define CAN_TXBAR_AR24_Pos          24           /**< \brief (CAN_TXBAR) Add Request 24 */
#define CAN_TXBAR_AR24              (0x1u << CAN_TXBAR_AR24_Pos)
#define CAN_TXBAR_AR25_Pos          25           /**< \brief (CAN_TXBAR) Add Request 25 */
#define CAN_TXBAR_AR25              (0x1u << CAN_TXBAR_AR25_Pos)
#define CAN_TXBAR_AR26_Pos          26           /**< \brief (CAN_TXBAR) Add Request 26 */
#define CAN_TXBAR_AR26              (0x1u << CAN_TXBAR_AR26_Pos)
#define CAN_TXBAR_AR27_Pos          27           /**< \brief (CAN_TXBAR) Add Request 27 */
#define CAN_TXBAR_AR27              (0x1u << CAN_TXBAR_AR27_Pos)
#define CAN_TXBAR_AR28_Pos          28           /**< \brief (CAN_TXBAR) Add Request 28 */
#define CAN_TXBAR_AR28              (0x1u << CAN_TXBAR_AR28_Pos)
#define CAN_TXBAR_AR29_Pos          29           /**< \brief (CAN_TXBAR) Add Request 29 */
#define CAN_TXBAR_AR29              (0x1u << CAN_TXBAR_AR29_Pos)
#define CAN_TXBAR_AR30_Pos          30           /**< \brief (CAN_TXBAR) Add Request 30 */
#define CAN_TXBAR_AR30              (0x1u << CAN_TXBAR_AR30_Pos)
#define CAN_TXBAR_AR31_Pos          31           /**< \brief (CAN_TXBAR) Add Request 31 */
#define CAN_TXBAR_AR31              (0x1u << CAN_TXBAR_AR31_Pos)
#define CAN_TXBAR_MASK              0xFFFFFFFFu  /**< \brief (CAN_TXBAR) MASK Register */

/* -------- CAN_TXBCR : (CAN Offset: 0xD4) (R/W 32) Tx Buffer Cancellation Request -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CR0:1;            /*!< bit:      0  Cancellation Request 0             */
    uint32_t CR1:1;            /*!< bit:      1  Cancellation Request 1             */
    uint32_t CR2:1;            /*!< bit:      2  Cancellation Request 2             */
    uint32_t CR3:1;            /*!< bit:      3  Cancellation Request 3             */
    uint32_t CR4:1;            /*!< bit:      4  Cancellation Request 4             */
    uint32_t CR5:1;            /*!< bit:      5  Cancellation Request 5             */
    uint32_t CR6:1;            /*!< bit:      6  Cancellation Request 6             */
    uint32_t CR7:1;            /*!< bit:      7  Cancellation Request 7             */
    uint32_t CR8:1;            /*!< bit:      8  Cancellation Request 8             */
    uint32_t CR9:1;            /*!< bit:      9  Cancellation Request 9             */
    uint32_t CR10:1;           /*!< bit:     10  Cancellation Request 10            */
    uint32_t CR11:1;           /*!< bit:     11  Cancellation Request 11            */
    uint32_t CR12:1;           /*!< bit:     12  Cancellation Request 12            */
    uint32_t CR13:1;           /*!< bit:     13  Cancellation Request 13            */
    uint32_t CR14:1;           /*!< bit:     14  Cancellation Request 14            */
    uint32_t CR15:1;           /*!< bit:     15  Cancellation Request 15            */
    uint32_t CR16:1;           /*!< bit:     16  Cancellation Request 16            */
    uint32_t CR17:1;           /*!< bit:     17  Cancellation Request 17            */
    uint32_t CR18:1;           /*!< bit:     18  Cancellation Request 18            */
    uint32_t CR19:1;           /*!< bit:     19  Cancellation Request 19            */
    uint32_t CR20:1;           /*!< bit:     20  Cancellation Request 20            */
    uint32_t CR21:1;           /*!< bit:     21  Cancellation Request 21            */
    uint32_t CR22:1;           /*!< bit:     22  Cancellation Request 22            */
    uint32_t CR23:1;           /*!< bit:     23  Cancellation Request 23            */
    uint32_t CR24:1;           /*!< bit:     24  Cancellation Request 24            */
    uint32_t CR25:1;           /*!< bit:     25  Cancellation Request 25            */
    uint32_t CR26:1;           /*!< bit:     26  Cancellation Request 26            */
    uint32_t CR27:1;           /*!< bit:     27  Cancellation Request 27            */
    uint32_t CR28:1;           /*!< bit:     28  Cancellation Request 28            */
    uint32_t CR29:1;           /*!< bit:     29  Cancellation Request 29            */
    uint32_t CR30:1;           /*!< bit:     30  Cancellation Request 30            */
    uint32_t CR31:1;           /*!< bit:     31  Cancellation Request 31            */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TXBCR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TXBCR_OFFSET            0xD4         /**< \brief (CAN_TXBCR offset) Tx Buffer Cancellation Request */
#define CAN_TXBCR_RESETVALUE        0x00000000u  /**< \brief (CAN_TXBCR reset_value) Tx Buffer Cancellation Request */

#define CAN_TXBCR_CR0_Pos           0            /**< \brief (CAN_TXBCR) Cancellation Request 0 */
#define CAN_TXBCR_CR0               (0x1u << CAN_TXBCR_CR0_Pos)
#define CAN_TXBCR_CR1_Pos           1            /**< \brief (CAN_TXBCR) Cancellation Request 1 */
#define CAN_TXBCR_CR1               (0x1u << CAN_TXBCR_CR1_Pos)
#define CAN_TXBCR_CR2_Pos           2            /**< \brief (CAN_TXBCR) Cancellation Request 2 */
#define CAN_TXBCR_CR2               (0x1u << CAN_TXBCR_CR2_Pos)
#define CAN_TXBCR_CR3_Pos           3            /**< \brief (CAN_TXBCR) Cancellation Request 3 */
#define CAN_TXBCR_CR3               (0x1u << CAN_TXBCR_CR3_Pos)
#define CAN_TXBCR_CR4_Pos           4            /**< \brief (CAN_TXBCR) Cancellation Request 4 */
#define CAN_TXBCR_CR4               (0x1u << CAN_TXBCR_CR4_Pos)
#define CAN_TXBCR_CR5_Pos           5            /**< \brief (CAN_TXBCR) Cancellation Request 5 */
#define CAN_TXBCR_CR5               (0x1u << CAN_TXBCR_CR5_Pos)
#define CAN_TXBCR_CR6_Pos           6            /**< \brief (CAN_TXBCR) Cancellation Request 6 */
#define CAN_TXBCR_CR6               (0x1u << CAN_TXBCR_CR6_Pos)
#define CAN_TXBCR_CR7_Pos           7            /**< \brief (CAN_TXBCR) Cancellation Request 7 */
#define CAN_TXBCR_CR7               (0x1u << CAN_TXBCR_CR7_Pos)
#define CAN_TXBCR_CR8_Pos           8            /**< \brief (CAN_TXBCR) Cancellation Request 8 */
#define CAN_TXBCR_CR8               (0x1u << CAN_TXBCR_CR8_Pos)
#define CAN_TXBCR_CR9_Pos           9            /**< \brief (CAN_TXBCR) Cancellation Request 9 */
#define CAN_TXBCR_CR9               (0x1u << CAN_TXBCR_CR9_Pos)
#define CAN_TXBCR_CR10_Pos          10           /**< \brief (CAN_TXBCR) Cancellation Request 10 */
#define CAN_TXBCR_CR10              (0x1u << CAN_TXBCR_CR10_Pos)
#define CAN_TXBCR_CR11_Pos          11           /**< \brief (CAN_TXBCR) Cancellation Request 11 */
#define CAN_TXBCR_CR11              (0x1u << CAN_TXBCR_CR11_Pos)
#define CAN_TXBCR_CR12_Pos          12           /**< \brief (CAN_TXBCR) Cancellation Request 12 */
#define CAN_TXBCR_CR12              (0x1u << CAN_TXBCR_CR12_Pos)
#define CAN_TXBCR_CR13_Pos          13           /**< \brief (CAN_TXBCR) Cancellation Request 13 */
#define CAN_TXBCR_CR13              (0x1u << CAN_TXBCR_CR13_Pos)
#define CAN_TXBCR_CR14_Pos          14           /**< \brief (CAN_TXBCR) Cancellation Request 14 */
#define CAN_TXBCR_CR14              (0x1u << CAN_TXBCR_CR14_Pos)
#define CAN_TXBCR_CR15_Pos          15           /**< \brief (CAN_TXBCR) Cancellation Request 15 */
#define CAN_TXBCR_CR15              (0x1u << CAN_TXBCR_CR15_Pos)
#define CAN_TXBCR_CR16_Pos          16           /**< \brief (CAN_TXBCR) Cancellation Request 16 */
#define CAN_TXBCR_CR16              (0x1u << CAN_TXBCR_CR16_Pos)
#define CAN_TXBCR_CR17_Pos          17           /**< \brief (CAN_TXBCR) Cancellation Request 17 */
#define CAN_TXBCR_CR17              (0x1u << CAN_TXBCR_CR17_Pos)
#define CAN_TXBCR_CR18_Pos          18           /**< \brief (CAN_TXBCR) Cancellation Request 18 */
#define CAN_TXBCR_CR18              (0x1u << CAN_TXBCR_CR18_Pos)
#define CAN_TXBCR_CR19_Pos          19           /**< \brief (CAN_TXBCR) Cancellation Request 19 */
#define CAN_TXBCR_CR19              (0x1u << CAN_TXBCR_CR19_Pos)
#define CAN_TXBCR_CR20_Pos          20           /**< \brief (CAN_TXBCR) Cancellation Request 20 */
#define CAN_TXBCR_CR20              (0x1u << CAN_TXBCR_CR20_Pos)
#define CAN_TXBCR_CR21_Pos          21           /**< \brief (CAN_TXBCR) Cancellation Request 21 */
#define CAN_TXBCR_CR21              (0x1u << CAN_TXBCR_CR21_Pos)
#define CAN_TXBCR_CR22_Pos          22           /**< \brief (CAN_TXBCR) Cancellation Request 22 */
#define CAN_TXBCR_CR22              (0x1u << CAN_TXBCR_CR22_Pos)
#define CAN_TXBCR_CR23_Pos          23           /**< \brief (CAN_TXBCR) Cancellation Request 23 */
#define CAN_TXBCR_CR23              (0x1u << CAN_TXBCR_CR23_Pos)
#define CAN_TXBCR_CR24_Pos          24           /**< \brief (CAN_TXBCR) Cancellation Request 24 */
#define CAN_TXBCR_CR24              (0x1u << CAN_TXBCR_CR24_Pos)
#define CAN_TXBCR_CR25_Pos          25           /**< \brief (CAN_TXBCR) Cancellation Request 25 */
#define CAN_TXBCR_CR25              (0x1u << CAN_TXBCR_CR25_Pos)
#define CAN_TXBCR_CR26_Pos          26           /**< \brief (CAN_TXBCR) Cancellation Request 26 */
#define CAN_TXBCR_CR26              (0x1u << CAN_TXBCR_CR26_Pos)
#define CAN_TXBCR_CR27_Pos          27           /**< \brief (CAN_TXBCR) Cancellation Request 27 */
#define CAN_TXBCR_CR27              (0x1u << CAN_TXBCR_CR27_Pos)
#define CAN_TXBCR_CR28_Pos          28           /**< \brief (CAN_TXBCR) Cancellation Request 28 */
#define CAN_TXBCR_CR28              (0x1u << CAN_TXBCR_CR28_Pos)
#define CAN_TXBCR_CR29_Pos          29           /**< \brief (CAN_TXBCR) Cancellation Request 29 */
#define CAN_TXBCR_CR29              (0x1u << CAN_TXBCR_CR29_Pos)
#define CAN_TXBCR_CR30_Pos          30           /**< \brief (CAN_TXBCR) Cancellation Request 30 */
#define CAN_TXBCR_CR30              (0x1u << CAN_TXBCR_CR30_Pos)
#define CAN_TXBCR_CR31_Pos          31           /**< \brief (CAN_TXBCR) Cancellation Request 31 */
#define CAN_TXBCR_CR31              (0x1u << CAN_TXBCR_CR31_Pos)
#define CAN_TXBCR_MASK              0xFFFFFFFFu  /**< \brief (CAN_TXBCR) MASK Register */

/* -------- CAN_TXBTO : (CAN Offset: 0xD8) (R/  32) Tx Buffer Transmission Occurred -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t TO0:1;            /*!< bit:      0  Transmission Occurred 0            */
    uint32_t TO1:1;            /*!< bit:      1  Transmission Occurred 1            */
    uint32_t TO2:1;            /*!< bit:      2  Transmission Occurred 2            */
    uint32_t TO3:1;            /*!< bit:      3  Transmission Occurred 3            */
    uint32_t TO4:1;            /*!< bit:      4  Transmission Occurred 4            */
    uint32_t TO5:1;            /*!< bit:      5  Transmission Occurred 5            */
    uint32_t TO6:1;            /*!< bit:      6  Transmission Occurred 6            */
    uint32_t TO7:1;            /*!< bit:      7  Transmission Occurred 7            */
    uint32_t TO8:1;            /*!< bit:      8  Transmission Occurred 8            */
    uint32_t TO9:1;            /*!< bit:      9  Transmission Occurred 9            */
    uint32_t TO10:1;           /*!< bit:     10  Transmission Occurred 10           */
    uint32_t TO11:1;           /*!< bit:     11  Transmission Occurred 11           */
    uint32_t TO12:1;           /*!< bit:     12  Transmission Occurred 12           */
    uint32_t TO13:1;           /*!< bit:     13  Transmission Occurred 13           */
    uint32_t TO14:1;           /*!< bit:     14  Transmission Occurred 14           */
    uint32_t TO15:1;           /*!< bit:     15  Transmission Occurred 15           */
    uint32_t TO16:1;           /*!< bit:     16  Transmission Occurred 16           */
    uint32_t TO17:1;           /*!< bit:     17  Transmission Occurred 17           */
    uint32_t TO18:1;           /*!< bit:     18  Transmission Occurred 18           */
    uint32_t TO19:1;           /*!< bit:     19  Transmission Occurred 19           */
    uint32_t TO20:1;           /*!< bit:     20  Transmission Occurred 20           */
    uint32_t TO21:1;           /*!< bit:     21  Transmission Occurred 21           */
    uint32_t TO22:1;           /*!< bit:     22  Transmission Occurred 22           */
    uint32_t TO23:1;           /*!< bit:     23  Transmission Occurred 23           */
    uint32_t TO24:1;           /*!< bit:     24  Transmission Occurred 24           */
    uint32_t TO25:1;           /*!< bit:     25  Transmission Occurred 25           */
    uint32_t TO26:1;           /*!< bit:     26  Transmission Occurred 26           */
    uint32_t TO27:1;           /*!< bit:     27  Transmission Occurred 27           */
    uint32_t TO28:1;           /*!< bit:     28  Transmission Occurred 28           */
    uint32_t TO29:1;           /*!< bit:     29  Transmission Occurred 29           */
    uint32_t TO30:1;           /*!< bit:     30  Transmission Occurred 30           */
    uint32_t TO31:1;           /*!< bit:     31  Transmission Occurred 31           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TXBTO_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TXBTO_OFFSET            0xD8         /**< \brief (CAN_TXBTO offset) Tx Buffer Transmission Occurred */
#define CAN_TXBTO_RESETVALUE        0x00000000u  /**< \brief (CAN_TXBTO reset_value) Tx Buffer Transmission Occurred */

#define CAN_TXBTO_TO0_Pos           0            /**< \brief (CAN_TXBTO) Transmission Occurred 0 */
#define CAN_TXBTO_TO0               (0x1u << CAN_TXBTO_TO0_Pos)
#define CAN_TXBTO_TO1_Pos           1            /**< \brief (CAN_TXBTO) Transmission Occurred 1 */
#define CAN_TXBTO_TO1               (0x1u << CAN_TXBTO_TO1_Pos)
#define CAN_TXBTO_TO2_Pos           2            /**< \brief (CAN_TXBTO) Transmission Occurred 2 */
#define CAN_TXBTO_TO2               (0x1u << CAN_TXBTO_TO2_Pos)
#define CAN_TXBTO_TO3_Pos           3            /**< \brief (CAN_TXBTO) Transmission Occurred 3 */
#define CAN_TXBTO_TO3               (0x1u << CAN_TXBTO_TO3_Pos)
#define CAN_TXBTO_TO4_Pos           4            /**< \brief (CAN_TXBTO) Transmission Occurred 4 */
#define CAN_TXBTO_TO4               (0x1u << CAN_TXBTO_TO4_Pos)
#define CAN_TXBTO_TO5_Pos           5            /**< \brief (CAN_TXBTO) Transmission Occurred 5 */
#define CAN_TXBTO_TO5               (0x1u << CAN_TXBTO_TO5_Pos)
#define CAN_TXBTO_TO6_Pos           6            /**< \brief (CAN_TXBTO) Transmission Occurred 6 */
#define CAN_TXBTO_TO6               (0x1u << CAN_TXBTO_TO6_Pos)
#define CAN_TXBTO_TO7_Pos           7            /**< \brief (CAN_TXBTO) Transmission Occurred 7 */
#define CAN_TXBTO_TO7               (0x1u << CAN_TXBTO_TO7_Pos)
#define CAN_TXBTO_TO8_Pos           8            /**< \brief (CAN_TXBTO) Transmission Occurred 8 */
#define CAN_TXBTO_TO8               (0x1u << CAN_TXBTO_TO8_Pos)
#define CAN_TXBTO_TO9_Pos           9            /**< \brief (CAN_TXBTO) Transmission Occurred 9 */
#define CAN_TXBTO_TO9               (0x1u << CAN_TXBTO_TO9_Pos)
#define CAN_TXBTO_TO10_Pos          10           /**< \brief (CAN_TXBTO) Transmission Occurred 10 */
#define CAN_TXBTO_TO10              (0x1u << CAN_TXBTO_TO10_Pos)
#define CAN_TXBTO_TO11_Pos          11           /**< \brief (CAN_TXBTO) Transmission Occurred 11 */
#define CAN_TXBTO_TO11              (0x1u << CAN_TXBTO_TO11_Pos)
#define CAN_TXBTO_TO12_Pos          12           /**< \brief (CAN_TXBTO) Transmission Occurred 12 */
#define CAN_TXBTO_TO12              (0x1u << CAN_TXBTO_TO12_Pos)
#define CAN_TXBTO_TO13_Pos          13           /**< \brief (CAN_TXBTO) Transmission Occurred 13 */
#define CAN_TXBTO_TO13              (0x1u << CAN_TXBTO_TO13_Pos)
#define CAN_TXBTO_TO14_Pos          14           /**< \brief (CAN_TXBTO) Transmission Occurred 14 */
#define CAN_TXBTO_TO14              (0x1u << CAN_TXBTO_TO14_Pos)
#define CAN_TXBTO_TO15_Pos          15           /**< \brief (CAN_TXBTO) Transmission Occurred 15 */
#define CAN_TXBTO_TO15              (0x1u << CAN_TXBTO_TO15_Pos)
#define CAN_TXBTO_TO16_Pos          16           /**< \brief (CAN_TXBTO) Transmission Occurred 16 */
#define CAN_TXBTO_TO16              (0x1u << CAN_TXBTO_TO16_Pos)
#define CAN_TXBTO_TO17_Pos          17           /**< \brief (CAN_TXBTO) Transmission Occurred 17 */
#define CAN_TXBTO_TO17              (0x1u << CAN_TXBTO_TO17_Pos)
#define CAN_TXBTO_TO18_Pos          18           /**< \brief (CAN_TXBTO) Transmission Occurred 18 */
#define CAN_TXBTO_TO18              (0x1u << CAN_TXBTO_TO18_Pos)
#define CAN_TXBTO_TO19_Pos          19           /**< \brief (CAN_TXBTO) Transmission Occurred 19 */
#define CAN_TXBTO_TO19              (0x1u << CAN_TXBTO_TO19_Pos)
#define CAN_TXBTO_TO20_Pos          20           /**< \brief (CAN_TXBTO) Transmission Occurred 20 */
#define CAN_TXBTO_TO20              (0x1u << CAN_TXBTO_TO20_Pos)
#define CAN_TXBTO_TO21_Pos          21           /**< \brief (CAN_TXBTO) Transmission Occurred 21 */
#define CAN_TXBTO_TO21              (0x1u << CAN_TXBTO_TO21_Pos)
#define CAN_TXBTO_TO22_Pos          22           /**< \brief (CAN_TXBTO) Transmission Occurred 22 */
#define CAN_TXBTO_TO22              (0x1u << CAN_TXBTO_TO22_Pos)
#define CAN_TXBTO_TO23_Pos          23           /**< \brief (CAN_TXBTO) Transmission Occurred 23 */
#define CAN_TXBTO_TO23              (0x1u << CAN_TXBTO_TO23_Pos)
#define CAN_TXBTO_TO24_Pos          24           /**< \brief (CAN_TXBTO) Transmission Occurred 24 */
#define CAN_TXBTO_TO24              (0x1u << CAN_TXBTO_TO24_Pos)
#define CAN_TXBTO_TO25_Pos          25           /**< \brief (CAN_TXBTO) Transmission Occurred 25 */
#define CAN_TXBTO_TO25              (0x1u << CAN_TXBTO_TO25_Pos)
#define CAN_TXBTO_TO26_Pos          26           /**< \brief (CAN_TXBTO) Transmission Occurred 26 */
#define CAN_TXBTO_TO26              (0x1u << CAN_TXBTO_TO26_Pos)
#define CAN_TXBTO_TO27_Pos          27           /**< \brief (CAN_TXBTO) Transmission Occurred 27 */
#define CAN_TXBTO_TO27              (0x1u << CAN_TXBTO_TO27_Pos)
#define CAN_TXBTO_TO28_Pos          28           /**< \brief (CAN_TXBTO) Transmission Occurred 28 */
#define CAN_TXBTO_TO28              (0x1u << CAN_TXBTO_TO28_Pos)
#define CAN_TXBTO_TO29_Pos          29           /**< \brief (CAN_TXBTO) Transmission Occurred 29 */
#define CAN_TXBTO_TO29              (0x1u << CAN_TXBTO_TO29_Pos)
#define CAN_TXBTO_TO30_Pos          30           /**< \brief (CAN_TXBTO) Transmission Occurred 30 */
#define CAN_TXBTO_TO30              (0x1u << CAN_TXBTO_TO30_Pos)
#define CAN_TXBTO_TO31_Pos          31           /**< \brief (CAN_TXBTO) Transmission Occurred 31 */
#define CAN_TXBTO_TO31              (0x1u << CAN_TXBTO_TO31_Pos)
#define CAN_TXBTO_MASK              0xFFFFFFFFu  /**< \brief (CAN_TXBTO) MASK Register */

/* -------- CAN_TXBCF : (CAN Offset: 0xDC) (R/  32) Tx Buffer Cancellation Finished -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CF0:1;            /*!< bit:      0  Tx Buffer Cancellation Finished 0  */
    uint32_t CF1:1;            /*!< bit:      1  Tx Buffer Cancellation Finished 1  */
    uint32_t CF2:1;            /*!< bit:      2  Tx Buffer Cancellation Finished 2  */
    uint32_t CF3:1;            /*!< bit:      3  Tx Buffer Cancellation Finished 3  */
    uint32_t CF4:1;            /*!< bit:      4  Tx Buffer Cancellation Finished 4  */
    uint32_t CF5:1;            /*!< bit:      5  Tx Buffer Cancellation Finished 5  */
    uint32_t CF6:1;            /*!< bit:      6  Tx Buffer Cancellation Finished 6  */
    uint32_t CF7:1;            /*!< bit:      7  Tx Buffer Cancellation Finished 7  */
    uint32_t CF8:1;            /*!< bit:      8  Tx Buffer Cancellation Finished 8  */
    uint32_t CF9:1;            /*!< bit:      9  Tx Buffer Cancellation Finished 9  */
    uint32_t CF10:1;           /*!< bit:     10  Tx Buffer Cancellation Finished 10 */
    uint32_t CF11:1;           /*!< bit:     11  Tx Buffer Cancellation Finished 11 */
    uint32_t CF12:1;           /*!< bit:     12  Tx Buffer Cancellation Finished 12 */
    uint32_t CF13:1;           /*!< bit:     13  Tx Buffer Cancellation Finished 13 */
    uint32_t CF14:1;           /*!< bit:     14  Tx Buffer Cancellation Finished 14 */
    uint32_t CF15:1;           /*!< bit:     15  Tx Buffer Cancellation Finished 15 */
    uint32_t CF16:1;           /*!< bit:     16  Tx Buffer Cancellation Finished 16 */
    uint32_t CF17:1;           /*!< bit:     17  Tx Buffer Cancellation Finished 17 */
    uint32_t CF18:1;           /*!< bit:     18  Tx Buffer Cancellation Finished 18 */
    uint32_t CF19:1;           /*!< bit:     19  Tx Buffer Cancellation Finished 19 */
    uint32_t CF20:1;           /*!< bit:     20  Tx Buffer Cancellation Finished 20 */
    uint32_t CF21:1;           /*!< bit:     21  Tx Buffer Cancellation Finished 21 */
    uint32_t CF22:1;           /*!< bit:     22  Tx Buffer Cancellation Finished 22 */
    uint32_t CF23:1;           /*!< bit:     23  Tx Buffer Cancellation Finished 23 */
    uint32_t CF24:1;           /*!< bit:     24  Tx Buffer Cancellation Finished 24 */
    uint32_t CF25:1;           /*!< bit:     25  Tx Buffer Cancellation Finished 25 */
    uint32_t CF26:1;           /*!< bit:     26  Tx Buffer Cancellation Finished 26 */
    uint32_t CF27:1;           /*!< bit:     27  Tx Buffer Cancellation Finished 27 */
    uint32_t CF28:1;           /*!< bit:     28  Tx Buffer Cancellation Finished 28 */
    uint32_t CF29:1;           /*!< bit:     29  Tx Buffer Cancellation Finished 29 */
    uint32_t CF30:1;           /*!< bit:     30  Tx Buffer Cancellation Finished 30 */
    uint32_t CF31:1;           /*!< bit:     31  Tx Buffer Cancellation Finished 31 */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TXBCF_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TXBCF_OFFSET            0xDC         /**< \brief (CAN_TXBCF offset) Tx Buffer Cancellation Finished */
#define CAN_TXBCF_RESETVALUE        0x00000000u  /**< \brief (CAN_TXBCF reset_value) Tx Buffer Cancellation Finished */

#define CAN_TXBCF_CF0_Pos           0            /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 0 */
#define CAN_TXBCF_CF0               (0x1u << CAN_TXBCF_CF0_Pos)
#define CAN_TXBCF_CF1_Pos           1            /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 1 */
#define CAN_TXBCF_CF1               (0x1u << CAN_TXBCF_CF1_Pos)
#define CAN_TXBCF_CF2_Pos           2            /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 2 */
#define CAN_TXBCF_CF2               (0x1u << CAN_TXBCF_CF2_Pos)
#define CAN_TXBCF_CF3_Pos           3            /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 3 */
#define CAN_TXBCF_CF3               (0x1u << CAN_TXBCF_CF3_Pos)
#define CAN_TXBCF_CF4_Pos           4            /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 4 */
#define CAN_TXBCF_CF4               (0x1u << CAN_TXBCF_CF4_Pos)
#define CAN_TXBCF_CF5_Pos           5            /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 5 */
#define CAN_TXBCF_CF5               (0x1u << CAN_TXBCF_CF5_Pos)
#define CAN_TXBCF_CF6_Pos           6            /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 6 */
#define CAN_TXBCF_CF6               (0x1u << CAN_TXBCF_CF6_Pos)
#define CAN_TXBCF_CF7_Pos           7            /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 7 */
#define CAN_TXBCF_CF7               (0x1u << CAN_TXBCF_CF7_Pos)
#define CAN_TXBCF_CF8_Pos           8            /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 8 */
#define CAN_TXBCF_CF8               (0x1u << CAN_TXBCF_CF8_Pos)
#define CAN_TXBCF_CF9_Pos           9            /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 9 */
#define CAN_TXBCF_CF9               (0x1u << CAN_TXBCF_CF9_Pos)
#define CAN_TXBCF_CF10_Pos          10           /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 10 */
#define CAN_TXBCF_CF10              (0x1u << CAN_TXBCF_CF10_Pos)
#define CAN_TXBCF_CF11_Pos          11           /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 11 */
#define CAN_TXBCF_CF11              (0x1u << CAN_TXBCF_CF11_Pos)
#define CAN_TXBCF_CF12_Pos          12           /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 12 */
#define CAN_TXBCF_CF12              (0x1u << CAN_TXBCF_CF12_Pos)
#define CAN_TXBCF_CF13_Pos          13           /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 13 */
#define CAN_TXBCF_CF13              (0x1u << CAN_TXBCF_CF13_Pos)
#define CAN_TXBCF_CF14_Pos          14           /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 14 */
#define CAN_TXBCF_CF14              (0x1u << CAN_TXBCF_CF14_Pos)
#define CAN_TXBCF_CF15_Pos          15           /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 15 */
#define CAN_TXBCF_CF15              (0x1u << CAN_TXBCF_CF15_Pos)
#define CAN_TXBCF_CF16_Pos          16           /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 16 */
#define CAN_TXBCF_CF16              (0x1u << CAN_TXBCF_CF16_Pos)
#define CAN_TXBCF_CF17_Pos          17           /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 17 */
#define CAN_TXBCF_CF17              (0x1u << CAN_TXBCF_CF17_Pos)
#define CAN_TXBCF_CF18_Pos          18           /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 18 */
#define CAN_TXBCF_CF18              (0x1u << CAN_TXBCF_CF18_Pos)
#define CAN_TXBCF_CF19_Pos          19           /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 19 */
#define CAN_TXBCF_CF19              (0x1u << CAN_TXBCF_CF19_Pos)
#define CAN_TXBCF_CF20_Pos          20           /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 20 */
#define CAN_TXBCF_CF20              (0x1u << CAN_TXBCF_CF20_Pos)
#define CAN_TXBCF_CF21_Pos          21           /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 21 */
#define CAN_TXBCF_CF21              (0x1u << CAN_TXBCF_CF21_Pos)
#define CAN_TXBCF_CF22_Pos          22           /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 22 */
#define CAN_TXBCF_CF22              (0x1u << CAN_TXBCF_CF22_Pos)
#define CAN_TXBCF_CF23_Pos          23           /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 23 */
#define CAN_TXBCF_CF23              (0x1u << CAN_TXBCF_CF23_Pos)
#define CAN_TXBCF_CF24_Pos          24           /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 24 */
#define CAN_TXBCF_CF24              (0x1u << CAN_TXBCF_CF24_Pos)
#define CAN_TXBCF_CF25_Pos          25           /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 25 */
#define CAN_TXBCF_CF25              (0x1u << CAN_TXBCF_CF25_Pos)
#define CAN_TXBCF_CF26_Pos          26           /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 26 */
#define CAN_TXBCF_CF26              (0x1u << CAN_TXBCF_CF26_Pos)
#define CAN_TXBCF_CF27_Pos          27           /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 27 */
#define CAN_TXBCF_CF27              (0x1u << CAN_TXBCF_CF27_Pos)
#define CAN_TXBCF_CF28_Pos          28           /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 28 */
#define CAN_TXBCF_CF28              (0x1u << CAN_TXBCF_CF28_Pos)
#define CAN_TXBCF_CF29_Pos          29           /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 29 */
#define CAN_TXBCF_CF29              (0x1u << CAN_TXBCF_CF29_Pos)
#define CAN_TXBCF_CF30_Pos          30           /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 30 */
#define CAN_TXBCF_CF30              (0x1u << CAN_TXBCF_CF30_Pos)
#define CAN_TXBCF_CF31_Pos          31           /**< \brief (CAN_TXBCF) Tx Buffer Cancellation Finished 31 */
#define CAN_TXBCF_CF31              (0x1u << CAN_TXBCF_CF31_Pos)
#define CAN_TXBCF_MASK              0xFFFFFFFFu  /**< \brief (CAN_TXBCF) MASK Register */

/* -------- CAN_TXBTIE : (CAN Offset: 0xE0) (R/W 32) Tx Buffer Transmission Interrupt Enable -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t TIE0:1;           /*!< bit:      0  Transmission Interrupt Enable 0    */
    uint32_t TIE1:1;           /*!< bit:      1  Transmission Interrupt Enable 1    */
    uint32_t TIE2:1;           /*!< bit:      2  Transmission Interrupt Enable 2    */
    uint32_t TIE3:1;           /*!< bit:      3  Transmission Interrupt Enable 3    */
    uint32_t TIE4:1;           /*!< bit:      4  Transmission Interrupt Enable 4    */
    uint32_t TIE5:1;           /*!< bit:      5  Transmission Interrupt Enable 5    */
    uint32_t TIE6:1;           /*!< bit:      6  Transmission Interrupt Enable 6    */
    uint32_t TIE7:1;           /*!< bit:      7  Transmission Interrupt Enable 7    */
    uint32_t TIE8:1;           /*!< bit:      8  Transmission Interrupt Enable 8    */
    uint32_t TIE9:1;           /*!< bit:      9  Transmission Interrupt Enable 9    */
    uint32_t TIE10:1;          /*!< bit:     10  Transmission Interrupt Enable 10   */
    uint32_t TIE11:1;          /*!< bit:     11  Transmission Interrupt Enable 11   */
    uint32_t TIE12:1;          /*!< bit:     12  Transmission Interrupt Enable 12   */
    uint32_t TIE13:1;          /*!< bit:     13  Transmission Interrupt Enable 13   */
    uint32_t TIE14:1;          /*!< bit:     14  Transmission Interrupt Enable 14   */
    uint32_t TIE15:1;          /*!< bit:     15  Transmission Interrupt Enable 15   */
    uint32_t TIE16:1;          /*!< bit:     16  Transmission Interrupt Enable 16   */
    uint32_t TIE17:1;          /*!< bit:     17  Transmission Interrupt Enable 17   */
    uint32_t TIE18:1;          /*!< bit:     18  Transmission Interrupt Enable 18   */
    uint32_t TIE19:1;          /*!< bit:     19  Transmission Interrupt Enable 19   */
    uint32_t TIE20:1;          /*!< bit:     20  Transmission Interrupt Enable 20   */
    uint32_t TIE21:1;          /*!< bit:     21  Transmission Interrupt Enable 21   */
    uint32_t TIE22:1;          /*!< bit:     22  Transmission Interrupt Enable 22   */
    uint32_t TIE23:1;          /*!< bit:     23  Transmission Interrupt Enable 23   */
    uint32_t TIE24:1;          /*!< bit:     24  Transmission Interrupt Enable 24   */
    uint32_t TIE25:1;          /*!< bit:     25  Transmission Interrupt Enable 25   */
    uint32_t TIE26:1;          /*!< bit:     26  Transmission Interrupt Enable 26   */
    uint32_t TIE27:1;          /*!< bit:     27  Transmission Interrupt Enable 27   */
    uint32_t TIE28:1;          /*!< bit:     28  Transmission Interrupt Enable 28   */
    uint32_t TIE29:1;          /*!< bit:     29  Transmission Interrupt Enable 29   */
    uint32_t TIE30:1;          /*!< bit:     30  Transmission Interrupt Enable 30   */
    uint32_t TIE31:1;          /*!< bit:     31  Transmission Interrupt Enable 31   */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TXBTIE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TXBTIE_OFFSET           0xE0         /**< \brief (CAN_TXBTIE offset) Tx Buffer Transmission Interrupt Enable */
#define CAN_TXBTIE_RESETVALUE       0x00000000u  /**< \brief (CAN_TXBTIE reset_value) Tx Buffer Transmission Interrupt Enable */

#define CAN_TXBTIE_TIE0_Pos         0            /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 0 */
#define CAN_TXBTIE_TIE0             (0x1u << CAN_TXBTIE_TIE0_Pos)
#define CAN_TXBTIE_TIE1_Pos         1            /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 1 */
#define CAN_TXBTIE_TIE1             (0x1u << CAN_TXBTIE_TIE1_Pos)
#define CAN_TXBTIE_TIE2_Pos         2            /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 2 */
#define CAN_TXBTIE_TIE2             (0x1u << CAN_TXBTIE_TIE2_Pos)
#define CAN_TXBTIE_TIE3_Pos         3            /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 3 */
#define CAN_TXBTIE_TIE3             (0x1u << CAN_TXBTIE_TIE3_Pos)
#define CAN_TXBTIE_TIE4_Pos         4            /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 4 */
#define CAN_TXBTIE_TIE4             (0x1u << CAN_TXBTIE_TIE4_Pos)
#define CAN_TXBTIE_TIE5_Pos         5            /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 5 */
#define CAN_TXBTIE_TIE5             (0x1u << CAN_TXBTIE_TIE5_Pos)
#define CAN_TXBTIE_TIE6_Pos         6            /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 6 */
#define CAN_TXBTIE_TIE6             (0x1u << CAN_TXBTIE_TIE6_Pos)
#define CAN_TXBTIE_TIE7_Pos         7            /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 7 */
#define CAN_TXBTIE_TIE7             (0x1u << CAN_TXBTIE_TIE7_Pos)
#define CAN_TXBTIE_TIE8_Pos         8            /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 8 */
#define CAN_TXBTIE_TIE8             (0x1u << CAN_TXBTIE_TIE8_Pos)
#define CAN_TXBTIE_TIE9_Pos         9            /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 9 */
#define CAN_TXBTIE_TIE9             (0x1u << CAN_TXBTIE_TIE9_Pos)
#define CAN_TXBTIE_TIE10_Pos        10           /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 10 */
#define CAN_TXBTIE_TIE10            (0x1u << CAN_TXBTIE_TIE10_Pos)
#define CAN_TXBTIE_TIE11_Pos        11           /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 11 */
#define CAN_TXBTIE_TIE11            (0x1u << CAN_TXBTIE_TIE11_Pos)
#define CAN_TXBTIE_TIE12_Pos        12           /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 12 */
#define CAN_TXBTIE_TIE12            (0x1u << CAN_TXBTIE_TIE12_Pos)
#define CAN_TXBTIE_TIE13_Pos        13           /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 13 */
#define CAN_TXBTIE_TIE13            (0x1u << CAN_TXBTIE_TIE13_Pos)
#define CAN_TXBTIE_TIE14_Pos        14           /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 14 */
#define CAN_TXBTIE_TIE14            (0x1u << CAN_TXBTIE_TIE14_Pos)
#define CAN_TXBTIE_TIE15_Pos        15           /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 15 */
#define CAN_TXBTIE_TIE15            (0x1u << CAN_TXBTIE_TIE15_Pos)
#define CAN_TXBTIE_TIE16_Pos        16           /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 16 */
#define CAN_TXBTIE_TIE16            (0x1u << CAN_TXBTIE_TIE16_Pos)
#define CAN_TXBTIE_TIE17_Pos        17           /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 17 */
#define CAN_TXBTIE_TIE17            (0x1u << CAN_TXBTIE_TIE17_Pos)
#define CAN_TXBTIE_TIE18_Pos        18           /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 18 */
#define CAN_TXBTIE_TIE18            (0x1u << CAN_TXBTIE_TIE18_Pos)
#define CAN_TXBTIE_TIE19_Pos        19           /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 19 */
#define CAN_TXBTIE_TIE19            (0x1u << CAN_TXBTIE_TIE19_Pos)
#define CAN_TXBTIE_TIE20_Pos        20           /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 20 */
#define CAN_TXBTIE_TIE20            (0x1u << CAN_TXBTIE_TIE20_Pos)
#define CAN_TXBTIE_TIE21_Pos        21           /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 21 */
#define CAN_TXBTIE_TIE21            (0x1u << CAN_TXBTIE_TIE21_Pos)
#define CAN_TXBTIE_TIE22_Pos        22           /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 22 */
#define CAN_TXBTIE_TIE22            (0x1u << CAN_TXBTIE_TIE22_Pos)
#define CAN_TXBTIE_TIE23_Pos        23           /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 23 */
#define CAN_TXBTIE_TIE23            (0x1u << CAN_TXBTIE_TIE23_Pos)
#define CAN_TXBTIE_TIE24_Pos        24           /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 24 */
#define CAN_TXBTIE_TIE24            (0x1u << CAN_TXBTIE_TIE24_Pos)
#define CAN_TXBTIE_TIE25_Pos        25           /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 25 */
#define CAN_TXBTIE_TIE25            (0x1u << CAN_TXBTIE_TIE25_Pos)
#define CAN_TXBTIE_TIE26_Pos        26           /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 26 */
#define CAN_TXBTIE_TIE26            (0x1u << CAN_TXBTIE_TIE26_Pos)
#define CAN_TXBTIE_TIE27_Pos        27           /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 27 */
#define CAN_TXBTIE_TIE27            (0x1u << CAN_TXBTIE_TIE27_Pos)
#define CAN_TXBTIE_TIE28_Pos        28           /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 28 */
#define CAN_TXBTIE_TIE28            (0x1u << CAN_TXBTIE_TIE28_Pos)
#define CAN_TXBTIE_TIE29_Pos        29           /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 29 */
#define CAN_TXBTIE_TIE29            (0x1u << CAN_TXBTIE_TIE29_Pos)
#define CAN_TXBTIE_TIE30_Pos        30           /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 30 */
#define CAN_TXBTIE_TIE30            (0x1u << CAN_TXBTIE_TIE30_Pos)
#define CAN_TXBTIE_TIE31_Pos        31           /**< \brief (CAN_TXBTIE) Transmission Interrupt Enable 31 */
#define CAN_TXBTIE_TIE31            (0x1u << CAN_TXBTIE_TIE31_Pos)
#define CAN_TXBTIE_MASK             0xFFFFFFFFu  /**< \brief (CAN_TXBTIE) MASK Register */

/* -------- CAN_TXBCIE : (CAN Offset: 0xE4) (R/W 32) Tx Buffer Cancellation Finished Interrupt Enable -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CFIE0:1;          /*!< bit:      0  Cancellation Finished Interrupt Enable 0 */
    uint32_t CFIE1:1;          /*!< bit:      1  Cancellation Finished Interrupt Enable 1 */
    uint32_t CFIE2:1;          /*!< bit:      2  Cancellation Finished Interrupt Enable 2 */
    uint32_t CFIE3:1;          /*!< bit:      3  Cancellation Finished Interrupt Enable 3 */
    uint32_t CFIE4:1;          /*!< bit:      4  Cancellation Finished Interrupt Enable 4 */
    uint32_t CFIE5:1;          /*!< bit:      5  Cancellation Finished Interrupt Enable 5 */
    uint32_t CFIE6:1;          /*!< bit:      6  Cancellation Finished Interrupt Enable 6 */
    uint32_t CFIE7:1;          /*!< bit:      7  Cancellation Finished Interrupt Enable 7 */
    uint32_t CFIE8:1;          /*!< bit:      8  Cancellation Finished Interrupt Enable 8 */
    uint32_t CFIE9:1;          /*!< bit:      9  Cancellation Finished Interrupt Enable 9 */
    uint32_t CFIE10:1;         /*!< bit:     10  Cancellation Finished Interrupt Enable 10 */
    uint32_t CFIE11:1;         /*!< bit:     11  Cancellation Finished Interrupt Enable 11 */
    uint32_t CFIE12:1;         /*!< bit:     12  Cancellation Finished Interrupt Enable 12 */
    uint32_t CFIE13:1;         /*!< bit:     13  Cancellation Finished Interrupt Enable 13 */
    uint32_t CFIE14:1;         /*!< bit:     14  Cancellation Finished Interrupt Enable 14 */
    uint32_t CFIE15:1;         /*!< bit:     15  Cancellation Finished Interrupt Enable 15 */
    uint32_t CFIE16:1;         /*!< bit:     16  Cancellation Finished Interrupt Enable 16 */
    uint32_t CFIE17:1;         /*!< bit:     17  Cancellation Finished Interrupt Enable 17 */
    uint32_t CFIE18:1;         /*!< bit:     18  Cancellation Finished Interrupt Enable 18 */
    uint32_t CFIE19:1;         /*!< bit:     19  Cancellation Finished Interrupt Enable 19 */
    uint32_t CFIE20:1;         /*!< bit:     20  Cancellation Finished Interrupt Enable 20 */
    uint32_t CFIE21:1;         /*!< bit:     21  Cancellation Finished Interrupt Enable 21 */
    uint32_t CFIE22:1;         /*!< bit:     22  Cancellation Finished Interrupt Enable 22 */
    uint32_t CFIE23:1;         /*!< bit:     23  Cancellation Finished Interrupt Enable 23 */
    uint32_t CFIE24:1;         /*!< bit:     24  Cancellation Finished Interrupt Enable 24 */
    uint32_t CFIE25:1;         /*!< bit:     25  Cancellation Finished Interrupt Enable 25 */
    uint32_t CFIE26:1;         /*!< bit:     26  Cancellation Finished Interrupt Enable 26 */
    uint32_t CFIE27:1;         /*!< bit:     27  Cancellation Finished Interrupt Enable 27 */
    uint32_t CFIE28:1;         /*!< bit:     28  Cancellation Finished Interrupt Enable 28 */
    uint32_t CFIE29:1;         /*!< bit:     29  Cancellation Finished Interrupt Enable 29 */
    uint32_t CFIE30:1;         /*!< bit:     30  Cancellation Finished Interrupt Enable 30 */
    uint32_t CFIE31:1;         /*!< bit:     31  Cancellation Finished Interrupt Enable 31 */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TXBCIE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TXBCIE_OFFSET           0xE4         /**< \brief (CAN_TXBCIE offset) Tx Buffer Cancellation Finished Interrupt Enable */
#define CAN_TXBCIE_RESETVALUE       0x00000000u  /**< \brief (CAN_TXBCIE reset_value) Tx Buffer Cancellation Finished Interrupt Enable */

#define CAN_TXBCIE_CFIE0_Pos        0            /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 0 */
#define CAN_TXBCIE_CFIE0            (0x1u << CAN_TXBCIE_CFIE0_Pos)
#define CAN_TXBCIE_CFIE1_Pos        1            /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 1 */
#define CAN_TXBCIE_CFIE1            (0x1u << CAN_TXBCIE_CFIE1_Pos)
#define CAN_TXBCIE_CFIE2_Pos        2            /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 2 */
#define CAN_TXBCIE_CFIE2            (0x1u << CAN_TXBCIE_CFIE2_Pos)
#define CAN_TXBCIE_CFIE3_Pos        3            /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 3 */
#define CAN_TXBCIE_CFIE3            (0x1u << CAN_TXBCIE_CFIE3_Pos)
#define CAN_TXBCIE_CFIE4_Pos        4            /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 4 */
#define CAN_TXBCIE_CFIE4            (0x1u << CAN_TXBCIE_CFIE4_Pos)
#define CAN_TXBCIE_CFIE5_Pos        5            /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 5 */
#define CAN_TXBCIE_CFIE5            (0x1u << CAN_TXBCIE_CFIE5_Pos)
#define CAN_TXBCIE_CFIE6_Pos        6            /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 6 */
#define CAN_TXBCIE_CFIE6            (0x1u << CAN_TXBCIE_CFIE6_Pos)
#define CAN_TXBCIE_CFIE7_Pos        7            /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 7 */
#define CAN_TXBCIE_CFIE7            (0x1u << CAN_TXBCIE_CFIE7_Pos)
#define CAN_TXBCIE_CFIE8_Pos        8            /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 8 */
#define CAN_TXBCIE_CFIE8            (0x1u << CAN_TXBCIE_CFIE8_Pos)
#define CAN_TXBCIE_CFIE9_Pos        9            /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 9 */
#define CAN_TXBCIE_CFIE9            (0x1u << CAN_TXBCIE_CFIE9_Pos)
#define CAN_TXBCIE_CFIE10_Pos       10           /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 10 */
#define CAN_TXBCIE_CFIE10           (0x1u << CAN_TXBCIE_CFIE10_Pos)
#define CAN_TXBCIE_CFIE11_Pos       11           /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 11 */
#define CAN_TXBCIE_CFIE11           (0x1u << CAN_TXBCIE_CFIE11_Pos)
#define CAN_TXBCIE_CFIE12_Pos       12           /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 12 */
#define CAN_TXBCIE_CFIE12           (0x1u << CAN_TXBCIE_CFIE12_Pos)
#define CAN_TXBCIE_CFIE13_Pos       13           /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 13 */
#define CAN_TXBCIE_CFIE13           (0x1u << CAN_TXBCIE_CFIE13_Pos)
#define CAN_TXBCIE_CFIE14_Pos       14           /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 14 */
#define CAN_TXBCIE_CFIE14           (0x1u << CAN_TXBCIE_CFIE14_Pos)
#define CAN_TXBCIE_CFIE15_Pos       15           /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 15 */
#define CAN_TXBCIE_CFIE15           (0x1u << CAN_TXBCIE_CFIE15_Pos)
#define CAN_TXBCIE_CFIE16_Pos       16           /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 16 */
#define CAN_TXBCIE_CFIE16           (0x1u << CAN_TXBCIE_CFIE16_Pos)
#define CAN_TXBCIE_CFIE17_Pos       17           /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 17 */
#define CAN_TXBCIE_CFIE17           (0x1u << CAN_TXBCIE_CFIE17_Pos)
#define CAN_TXBCIE_CFIE18_Pos       18           /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 18 */
#define CAN_TXBCIE_CFIE18           (0x1u << CAN_TXBCIE_CFIE18_Pos)
#define CAN_TXBCIE_CFIE19_Pos       19           /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 19 */
#define CAN_TXBCIE_CFIE19           (0x1u << CAN_TXBCIE_CFIE19_Pos)
#define CAN_TXBCIE_CFIE20_Pos       20           /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 20 */
#define CAN_TXBCIE_CFIE20           (0x1u << CAN_TXBCIE_CFIE20_Pos)
#define CAN_TXBCIE_CFIE21_Pos       21           /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 21 */
#define CAN_TXBCIE_CFIE21           (0x1u << CAN_TXBCIE_CFIE21_Pos)
#define CAN_TXBCIE_CFIE22_Pos       22           /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 22 */
#define CAN_TXBCIE_CFIE22           (0x1u << CAN_TXBCIE_CFIE22_Pos)
#define CAN_TXBCIE_CFIE23_Pos       23           /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 23 */
#define CAN_TXBCIE_CFIE23           (0x1u << CAN_TXBCIE_CFIE23_Pos)
#define CAN_TXBCIE_CFIE24_Pos       24           /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 24 */
#define CAN_TXBCIE_CFIE24           (0x1u << CAN_TXBCIE_CFIE24_Pos)
#define CAN_TXBCIE_CFIE25_Pos       25           /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 25 */
#define CAN_TXBCIE_CFIE25           (0x1u << CAN_TXBCIE_CFIE25_Pos)
#define CAN_TXBCIE_CFIE26_Pos       26           /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 26 */
#define CAN_TXBCIE_CFIE26           (0x1u << CAN_TXBCIE_CFIE26_Pos)
#define CAN_TXBCIE_CFIE27_Pos       27           /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 27 */
#define CAN_TXBCIE_CFIE27           (0x1u << CAN_TXBCIE_CFIE27_Pos)
#define CAN_TXBCIE_CFIE28_Pos       28           /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 28 */
#define CAN_TXBCIE_CFIE28           (0x1u << CAN_TXBCIE_CFIE28_Pos)
#define CAN_TXBCIE_CFIE29_Pos       29           /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 29 */
#define CAN_TXBCIE_CFIE29           (0x1u << CAN_TXBCIE_CFIE29_Pos)
#define CAN_TXBCIE_CFIE30_Pos       30           /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 30 */
#define CAN_TXBCIE_CFIE30           (0x1u << CAN_TXBCIE_CFIE30_Pos)
#define CAN_TXBCIE_CFIE31_Pos       31           /**< \brief (CAN_TXBCIE) Cancellation Finished Interrupt Enable 31 */
#define CAN_TXBCIE_CFIE31           (0x1u << CAN_TXBCIE_CFIE31_Pos)
#define CAN_TXBCIE_MASK             0xFFFFFFFFu  /**< \brief (CAN_TXBCIE) MASK Register */

/* -------- CAN_TXEFC : (CAN Offset: 0xF0) (R/W 32) Tx Event FIFO Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EFSA:16;          /*!< bit:  0..15  Event FIFO Start Address           */
    uint32_t EFS:6;            /*!< bit: 16..21  Event FIFO Size                    */
    uint32_t :2;               /*!< bit: 22..23  Reserved                           */
    uint32_t EFWM:6;           /*!< bit: 24..29  Event FIFO Watermark               */
    uint32_t :2;               /*!< bit: 30..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TXEFC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TXEFC_OFFSET            0xF0         /**< \brief (CAN_TXEFC offset) Tx Event FIFO Configuration */
#define CAN_TXEFC_RESETVALUE        0x00000000u  /**< \brief (CAN_TXEFC reset_value) Tx Event FIFO Configuration */

#define CAN_TXEFC_EFSA_Pos          0            /**< \brief (CAN_TXEFC) Event FIFO Start Address */
#define CAN_TXEFC_EFSA_Msk          (0xFFFFu << CAN_TXEFC_EFSA_Pos)
#define CAN_TXEFC_EFSA(value)       (CAN_TXEFC_EFSA_Msk & ((value) << CAN_TXEFC_EFSA_Pos))
#define CAN_TXEFC_EFS_Pos           16           /**< \brief (CAN_TXEFC) Event FIFO Size */
#define CAN_TXEFC_EFS_Msk           (0x3Fu << CAN_TXEFC_EFS_Pos)
#define CAN_TXEFC_EFS(value)        (CAN_TXEFC_EFS_Msk & ((value) << CAN_TXEFC_EFS_Pos))
#define CAN_TXEFC_EFWM_Pos          24           /**< \brief (CAN_TXEFC) Event FIFO Watermark */
#define CAN_TXEFC_EFWM_Msk          (0x3Fu << CAN_TXEFC_EFWM_Pos)
#define CAN_TXEFC_EFWM(value)       (CAN_TXEFC_EFWM_Msk & ((value) << CAN_TXEFC_EFWM_Pos))
#define CAN_TXEFC_MASK              0x3F3FFFFFu  /**< \brief (CAN_TXEFC) MASK Register */

/* -------- CAN_TXEFS : (CAN Offset: 0xF4) (R/  32) Tx Event FIFO Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EFFL:6;           /*!< bit:  0.. 5  Event FIFO Fill Level              */
    uint32_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint32_t EFGI:5;           /*!< bit:  8..12  Event FIFO Get Index               */
    uint32_t :3;               /*!< bit: 13..15  Reserved                           */
    uint32_t EFPI:5;           /*!< bit: 16..20  Event FIFO Put Index               */
    uint32_t :3;               /*!< bit: 21..23  Reserved                           */
    uint32_t EFF:1;            /*!< bit:     24  Event FIFO Full                    */
    uint32_t TEFL:1;           /*!< bit:     25  Tx Event FIFO Element Lost         */
    uint32_t :6;               /*!< bit: 26..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TXEFS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TXEFS_OFFSET            0xF4         /**< \brief (CAN_TXEFS offset) Tx Event FIFO Status */
#define CAN_TXEFS_RESETVALUE        0x00000000u  /**< \brief (CAN_TXEFS reset_value) Tx Event FIFO Status */

#define CAN_TXEFS_EFFL_Pos          0            /**< \brief (CAN_TXEFS) Event FIFO Fill Level */
#define CAN_TXEFS_EFFL_Msk          (0x3Fu << CAN_TXEFS_EFFL_Pos)
#define CAN_TXEFS_EFFL(value)       (CAN_TXEFS_EFFL_Msk & ((value) << CAN_TXEFS_EFFL_Pos))
#define CAN_TXEFS_EFGI_Pos          8            /**< \brief (CAN_TXEFS) Event FIFO Get Index */
#define CAN_TXEFS_EFGI_Msk          (0x1Fu << CAN_TXEFS_EFGI_Pos)
#define CAN_TXEFS_EFGI(value)       (CAN_TXEFS_EFGI_Msk & ((value) << CAN_TXEFS_EFGI_Pos))
#define CAN_TXEFS_EFPI_Pos          16           /**< \brief (CAN_TXEFS) Event FIFO Put Index */
#define CAN_TXEFS_EFPI_Msk          (0x1Fu << CAN_TXEFS_EFPI_Pos)
#define CAN_TXEFS_EFPI(value)       (CAN_TXEFS_EFPI_Msk & ((value) << CAN_TXEFS_EFPI_Pos))
#define CAN_TXEFS_EFF_Pos           24           /**< \brief (CAN_TXEFS) Event FIFO Full */
#define CAN_TXEFS_EFF               (0x1u << CAN_TXEFS_EFF_Pos)
#define CAN_TXEFS_TEFL_Pos          25           /**< \brief (CAN_TXEFS) Tx Event FIFO Element Lost */
#define CAN_TXEFS_TEFL              (0x1u << CAN_TXEFS_TEFL_Pos)
#define CAN_TXEFS_MASK              0x031F1F3Fu  /**< \brief (CAN_TXEFS) MASK Register */

/* -------- CAN_TXEFA : (CAN Offset: 0xF8) (R/W 32) Tx Event FIFO Acknowledge -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EFAI:5;           /*!< bit:  0.. 4  Event FIFO Acknowledge Index       */
    uint32_t :27;              /*!< bit:  5..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TXEFA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TXEFA_OFFSET            0xF8         /**< \brief (CAN_TXEFA offset) Tx Event FIFO Acknowledge */
#define CAN_TXEFA_RESETVALUE        0x00000000u  /**< \brief (CAN_TXEFA reset_value) Tx Event FIFO Acknowledge */

#define CAN_TXEFA_EFAI_Pos          0            /**< \brief (CAN_TXEFA) Event FIFO Acknowledge Index */
#define CAN_TXEFA_EFAI_Msk          (0x1Fu << CAN_TXEFA_EFAI_Pos)
#define CAN_TXEFA_EFAI(value)       (CAN_TXEFA_EFAI_Msk & ((value) << CAN_TXEFA_EFAI_Pos))
#define CAN_TXEFA_MASK              0x0000001Fu  /**< \brief (CAN_TXEFA) MASK Register */

/* -------- CAN_RXBE_0 : (CAN Offset: 0x00) (R/W 32) Rx Buffer Element 0 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ID:29;            /*!< bit:  0..28  Identifier                         */
    uint32_t RTR:1;            /*!< bit:     29  Remote Transmission Request        */
    uint32_t XTD:1;            /*!< bit:     30  Extended Identifier                */
    uint32_t ESI:1;            /*!< bit:     31  Error State Indicator              */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_RXBE_0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_RXBE_0_OFFSET           0x00         /**< \brief (CAN_RXBE_0 offset) Rx Buffer Element 0 */
#define CAN_RXBE_0_RESETVALUE       0x00000000u  /**< \brief (CAN_RXBE_0 reset_value) Rx Buffer Element 0 */

#define CAN_RXBE_0_ID_Pos           0            /**< \brief (CAN_RXBE_0) Identifier */
#define CAN_RXBE_0_ID_Msk           (0x1FFFFFFFu << CAN_RXBE_0_ID_Pos)
#define CAN_RXBE_0_ID(value)        (CAN_RXBE_0_ID_Msk & ((value) << CAN_RXBE_0_ID_Pos))
#define CAN_RXBE_0_RTR_Pos          29           /**< \brief (CAN_RXBE_0) Remote Transmission Request */
#define CAN_RXBE_0_RTR              (0x1u << CAN_RXBE_0_RTR_Pos)
#define CAN_RXBE_0_XTD_Pos          30           /**< \brief (CAN_RXBE_0) Extended Identifier */
#define CAN_RXBE_0_XTD              (0x1u << CAN_RXBE_0_XTD_Pos)
#define CAN_RXBE_0_ESI_Pos          31           /**< \brief (CAN_RXBE_0) Error State Indicator */
#define CAN_RXBE_0_ESI              (0x1u << CAN_RXBE_0_ESI_Pos)
#define CAN_RXBE_0_MASK             0xFFFFFFFFu  /**< \brief (CAN_RXBE_0) MASK Register */

/* -------- CAN_RXBE_1 : (CAN Offset: 0x04) (R/W 32) Rx Buffer Element 1 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t RXTS:16;          /*!< bit:  0..15  Rx Timestamp                       */
    uint32_t DLC:4;            /*!< bit: 16..19  Data Length Code                   */
    uint32_t BRS:1;            /*!< bit:     20  Bit Rate Search                    */
    uint32_t FDF:1;            /*!< bit:     21  FD Format                          */
    uint32_t :2;               /*!< bit: 22..23  Reserved                           */
    uint32_t FIDX:7;           /*!< bit: 24..30  Filter Index                       */
    uint32_t ANMF:1;           /*!< bit:     31  Accepted Non-matching Frame        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_RXBE_1_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_RXBE_1_OFFSET           0x04         /**< \brief (CAN_RXBE_1 offset) Rx Buffer Element 1 */
#define CAN_RXBE_1_RESETVALUE       0x00000000u  /**< \brief (CAN_RXBE_1 reset_value) Rx Buffer Element 1 */

#define CAN_RXBE_1_RXTS_Pos         0            /**< \brief (CAN_RXBE_1) Rx Timestamp */
#define CAN_RXBE_1_RXTS_Msk         (0xFFFFu << CAN_RXBE_1_RXTS_Pos)
#define CAN_RXBE_1_RXTS(value)      (CAN_RXBE_1_RXTS_Msk & ((value) << CAN_RXBE_1_RXTS_Pos))
#define CAN_RXBE_1_DLC_Pos          16           /**< \brief (CAN_RXBE_1) Data Length Code */
#define CAN_RXBE_1_DLC_Msk          (0xFu << CAN_RXBE_1_DLC_Pos)
#define CAN_RXBE_1_DLC(value)       (CAN_RXBE_1_DLC_Msk & ((value) << CAN_RXBE_1_DLC_Pos))
#define CAN_RXBE_1_BRS_Pos          20           /**< \brief (CAN_RXBE_1) Bit Rate Search */
#define CAN_RXBE_1_BRS              (0x1u << CAN_RXBE_1_BRS_Pos)
#define CAN_RXBE_1_FDF_Pos          21           /**< \brief (CAN_RXBE_1) FD Format */
#define CAN_RXBE_1_FDF              (0x1u << CAN_RXBE_1_FDF_Pos)
#define CAN_RXBE_1_FIDX_Pos         24           /**< \brief (CAN_RXBE_1) Filter Index */
#define CAN_RXBE_1_FIDX_Msk         (0x7Fu << CAN_RXBE_1_FIDX_Pos)
#define CAN_RXBE_1_FIDX(value)      (CAN_RXBE_1_FIDX_Msk & ((value) << CAN_RXBE_1_FIDX_Pos))
#define CAN_RXBE_1_ANMF_Pos         31           /**< \brief (CAN_RXBE_1) Accepted Non-matching Frame */
#define CAN_RXBE_1_ANMF             (0x1u << CAN_RXBE_1_ANMF_Pos)
#define CAN_RXBE_1_MASK             0xFF3FFFFFu  /**< \brief (CAN_RXBE_1) MASK Register */

/* -------- CAN_RXBE_DATA : (CAN Offset: 0x08) (R/W 32) Rx Buffer Element Data -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DB0:8;            /*!< bit:  0.. 7  Data Byte 0                        */
    uint32_t DB1:8;            /*!< bit:  8..15  Data Byte 1                        */
    uint32_t DB2:8;            /*!< bit: 16..23  Data Byte 2                        */
    uint32_t DB3:8;            /*!< bit: 24..31  Data Byte 3                        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_RXBE_DATA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_RXBE_DATA_OFFSET        0x08         /**< \brief (CAN_RXBE_DATA offset) Rx Buffer Element Data */
#define CAN_RXBE_DATA_RESETVALUE    0x00000000u  /**< \brief (CAN_RXBE_DATA reset_value) Rx Buffer Element Data */

#define CAN_RXBE_DATA_DB0_Pos       0            /**< \brief (CAN_RXBE_DATA) Data Byte 0 */
#define CAN_RXBE_DATA_DB0_Msk       (0xFFu << CAN_RXBE_DATA_DB0_Pos)
#define CAN_RXBE_DATA_DB0(value)    (CAN_RXBE_DATA_DB0_Msk & ((value) << CAN_RXBE_DATA_DB0_Pos))
#define CAN_RXBE_DATA_DB1_Pos       8            /**< \brief (CAN_RXBE_DATA) Data Byte 1 */
#define CAN_RXBE_DATA_DB1_Msk       (0xFFu << CAN_RXBE_DATA_DB1_Pos)
#define CAN_RXBE_DATA_DB1(value)    (CAN_RXBE_DATA_DB1_Msk & ((value) << CAN_RXBE_DATA_DB1_Pos))
#define CAN_RXBE_DATA_DB2_Pos       16           /**< \brief (CAN_RXBE_DATA) Data Byte 2 */
#define CAN_RXBE_DATA_DB2_Msk       (0xFFu << CAN_RXBE_DATA_DB2_Pos)
#define CAN_RXBE_DATA_DB2(value)    (CAN_RXBE_DATA_DB2_Msk & ((value) << CAN_RXBE_DATA_DB2_Pos))
#define CAN_RXBE_DATA_DB3_Pos       24           /**< \brief (CAN_RXBE_DATA) Data Byte 3 */
#define CAN_RXBE_DATA_DB3_Msk       (0xFFu << CAN_RXBE_DATA_DB3_Pos)
#define CAN_RXBE_DATA_DB3(value)    (CAN_RXBE_DATA_DB3_Msk & ((value) << CAN_RXBE_DATA_DB3_Pos))
#define CAN_RXBE_DATA_MASK          0xFFFFFFFFu  /**< \brief (CAN_RXBE_DATA) MASK Register */

/* -------- CAN_RXF0E_0 : (CAN Offset: 0x00) (R/W 32) Rx FIFO 0 Element 0 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ID:29;            /*!< bit:  0..28  Identifier                         */
    uint32_t RTR:1;            /*!< bit:     29  Remote Transmission Request        */
    uint32_t XTD:1;            /*!< bit:     30  Extended Identifier                */
    uint32_t ESI:1;            /*!< bit:     31  Error State Indicator              */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_RXF0E_0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_RXF0E_0_OFFSET          0x00         /**< \brief (CAN_RXF0E_0 offset) Rx FIFO 0 Element 0 */
#define CAN_RXF0E_0_RESETVALUE      0x00000000u  /**< \brief (CAN_RXF0E_0 reset_value) Rx FIFO 0 Element 0 */

#define CAN_RXF0E_0_ID_Pos          0            /**< \brief (CAN_RXF0E_0) Identifier */
#define CAN_RXF0E_0_ID_Msk          (0x1FFFFFFFu << CAN_RXF0E_0_ID_Pos)
#define CAN_RXF0E_0_ID(value)       (CAN_RXF0E_0_ID_Msk & ((value) << CAN_RXF0E_0_ID_Pos))
#define CAN_RXF0E_0_RTR_Pos         29           /**< \brief (CAN_RXF0E_0) Remote Transmission Request */
#define CAN_RXF0E_0_RTR             (0x1u << CAN_RXF0E_0_RTR_Pos)
#define CAN_RXF0E_0_XTD_Pos         30           /**< \brief (CAN_RXF0E_0) Extended Identifier */
#define CAN_RXF0E_0_XTD             (0x1u << CAN_RXF0E_0_XTD_Pos)
#define CAN_RXF0E_0_ESI_Pos         31           /**< \brief (CAN_RXF0E_0) Error State Indicator */
#define CAN_RXF0E_0_ESI             (0x1u << CAN_RXF0E_0_ESI_Pos)
#define CAN_RXF0E_0_MASK            0xFFFFFFFFu  /**< \brief (CAN_RXF0E_0) MASK Register */

/* -------- CAN_RXF0E_1 : (CAN Offset: 0x04) (R/W 32) Rx FIFO 0 Element 1 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t RXTS:16;          /*!< bit:  0..15  Rx Timestamp                       */
    uint32_t DLC:4;            /*!< bit: 16..19  Data Length Code                   */
    uint32_t BRS:1;            /*!< bit:     20  Bit Rate Search                    */
    uint32_t FDF:1;            /*!< bit:     21  FD Format                          */
    uint32_t :2;               /*!< bit: 22..23  Reserved                           */
    uint32_t FIDX:7;           /*!< bit: 24..30  Filter Index                       */
    uint32_t ANMF:1;           /*!< bit:     31  Accepted Non-matching Frame        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_RXF0E_1_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_RXF0E_1_OFFSET          0x04         /**< \brief (CAN_RXF0E_1 offset) Rx FIFO 0 Element 1 */
#define CAN_RXF0E_1_RESETVALUE      0x00000000u  /**< \brief (CAN_RXF0E_1 reset_value) Rx FIFO 0 Element 1 */

#define CAN_RXF0E_1_RXTS_Pos        0            /**< \brief (CAN_RXF0E_1) Rx Timestamp */
#define CAN_RXF0E_1_RXTS_Msk        (0xFFFFu << CAN_RXF0E_1_RXTS_Pos)
#define CAN_RXF0E_1_RXTS(value)     (CAN_RXF0E_1_RXTS_Msk & ((value) << CAN_RXF0E_1_RXTS_Pos))
#define CAN_RXF0E_1_DLC_Pos         16           /**< \brief (CAN_RXF0E_1) Data Length Code */
#define CAN_RXF0E_1_DLC_Msk         (0xFu << CAN_RXF0E_1_DLC_Pos)
#define CAN_RXF0E_1_DLC(value)      (CAN_RXF0E_1_DLC_Msk & ((value) << CAN_RXF0E_1_DLC_Pos))
#define CAN_RXF0E_1_BRS_Pos         20           /**< \brief (CAN_RXF0E_1) Bit Rate Search */
#define CAN_RXF0E_1_BRS             (0x1u << CAN_RXF0E_1_BRS_Pos)
#define CAN_RXF0E_1_FDF_Pos         21           /**< \brief (CAN_RXF0E_1) FD Format */
#define CAN_RXF0E_1_FDF             (0x1u << CAN_RXF0E_1_FDF_Pos)
#define CAN_RXF0E_1_FIDX_Pos        24           /**< \brief (CAN_RXF0E_1) Filter Index */
#define CAN_RXF0E_1_FIDX_Msk        (0x7Fu << CAN_RXF0E_1_FIDX_Pos)
#define CAN_RXF0E_1_FIDX(value)     (CAN_RXF0E_1_FIDX_Msk & ((value) << CAN_RXF0E_1_FIDX_Pos))
#define CAN_RXF0E_1_ANMF_Pos        31           /**< \brief (CAN_RXF0E_1) Accepted Non-matching Frame */
#define CAN_RXF0E_1_ANMF            (0x1u << CAN_RXF0E_1_ANMF_Pos)
#define CAN_RXF0E_1_MASK            0xFF3FFFFFu  /**< \brief (CAN_RXF0E_1) MASK Register */

/* -------- CAN_RXF0E_DATA : (CAN Offset: 0x08) (R/W 32) Rx FIFO 0 Element Data -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DB0:8;            /*!< bit:  0.. 7  Data Byte 0                        */
    uint32_t DB1:8;            /*!< bit:  8..15  Data Byte 1                        */
    uint32_t DB2:8;            /*!< bit: 16..23  Data Byte 2                        */
    uint32_t DB3:8;            /*!< bit: 24..31  Data Byte 3                        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_RXF0E_DATA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_RXF0E_DATA_OFFSET       0x08         /**< \brief (CAN_RXF0E_DATA offset) Rx FIFO 0 Element Data */
#define CAN_RXF0E_DATA_RESETVALUE   0x00000000u  /**< \brief (CAN_RXF0E_DATA reset_value) Rx FIFO 0 Element Data */

#define CAN_RXF0E_DATA_DB0_Pos      0            /**< \brief (CAN_RXF0E_DATA) Data Byte 0 */
#define CAN_RXF0E_DATA_DB0_Msk      (0xFFu << CAN_RXF0E_DATA_DB0_Pos)
#define CAN_RXF0E_DATA_DB0(value)   (CAN_RXF0E_DATA_DB0_Msk & ((value) << CAN_RXF0E_DATA_DB0_Pos))
#define CAN_RXF0E_DATA_DB1_Pos      8            /**< \brief (CAN_RXF0E_DATA) Data Byte 1 */
#define CAN_RXF0E_DATA_DB1_Msk      (0xFFu << CAN_RXF0E_DATA_DB1_Pos)
#define CAN_RXF0E_DATA_DB1(value)   (CAN_RXF0E_DATA_DB1_Msk & ((value) << CAN_RXF0E_DATA_DB1_Pos))
#define CAN_RXF0E_DATA_DB2_Pos      16           /**< \brief (CAN_RXF0E_DATA) Data Byte 2 */
#define CAN_RXF0E_DATA_DB2_Msk      (0xFFu << CAN_RXF0E_DATA_DB2_Pos)
#define CAN_RXF0E_DATA_DB2(value)   (CAN_RXF0E_DATA_DB2_Msk & ((value) << CAN_RXF0E_DATA_DB2_Pos))
#define CAN_RXF0E_DATA_DB3_Pos      24           /**< \brief (CAN_RXF0E_DATA) Data Byte 3 */
#define CAN_RXF0E_DATA_DB3_Msk      (0xFFu << CAN_RXF0E_DATA_DB3_Pos)
#define CAN_RXF0E_DATA_DB3(value)   (CAN_RXF0E_DATA_DB3_Msk & ((value) << CAN_RXF0E_DATA_DB3_Pos))
#define CAN_RXF0E_DATA_MASK         0xFFFFFFFFu  /**< \brief (CAN_RXF0E_DATA) MASK Register */

/* -------- CAN_RXF1E_0 : (CAN Offset: 0x00) (R/W 32) Rx FIFO 1 Element 0 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ID:29;            /*!< bit:  0..28  Identifier                         */
    uint32_t RTR:1;            /*!< bit:     29  Remote Transmission Request        */
    uint32_t XTD:1;            /*!< bit:     30  Extended Identifier                */
    uint32_t ESI:1;            /*!< bit:     31  Error State Indicator              */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_RXF1E_0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_RXF1E_0_OFFSET          0x00         /**< \brief (CAN_RXF1E_0 offset) Rx FIFO 1 Element 0 */
#define CAN_RXF1E_0_RESETVALUE      0x00000000u  /**< \brief (CAN_RXF1E_0 reset_value) Rx FIFO 1 Element 0 */

#define CAN_RXF1E_0_ID_Pos          0            /**< \brief (CAN_RXF1E_0) Identifier */
#define CAN_RXF1E_0_ID_Msk          (0x1FFFFFFFu << CAN_RXF1E_0_ID_Pos)
#define CAN_RXF1E_0_ID(value)       (CAN_RXF1E_0_ID_Msk & ((value) << CAN_RXF1E_0_ID_Pos))
#define CAN_RXF1E_0_RTR_Pos         29           /**< \brief (CAN_RXF1E_0) Remote Transmission Request */
#define CAN_RXF1E_0_RTR             (0x1u << CAN_RXF1E_0_RTR_Pos)
#define CAN_RXF1E_0_XTD_Pos         30           /**< \brief (CAN_RXF1E_0) Extended Identifier */
#define CAN_RXF1E_0_XTD             (0x1u << CAN_RXF1E_0_XTD_Pos)
#define CAN_RXF1E_0_ESI_Pos         31           /**< \brief (CAN_RXF1E_0) Error State Indicator */
#define CAN_RXF1E_0_ESI             (0x1u << CAN_RXF1E_0_ESI_Pos)
#define CAN_RXF1E_0_MASK            0xFFFFFFFFu  /**< \brief (CAN_RXF1E_0) MASK Register */

/* -------- CAN_RXF1E_1 : (CAN Offset: 0x04) (R/W 32) Rx FIFO 1 Element 1 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t RXTS:16;          /*!< bit:  0..15  Rx Timestamp                       */
    uint32_t DLC:4;            /*!< bit: 16..19  Data Length Code                   */
    uint32_t BRS:1;            /*!< bit:     20  Bit Rate Search                    */
    uint32_t FDF:1;            /*!< bit:     21  FD Format                          */
    uint32_t :2;               /*!< bit: 22..23  Reserved                           */
    uint32_t FIDX:7;           /*!< bit: 24..30  Filter Index                       */
    uint32_t ANMF:1;           /*!< bit:     31  Accepted Non-matching Frame        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_RXF1E_1_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_RXF1E_1_OFFSET          0x04         /**< \brief (CAN_RXF1E_1 offset) Rx FIFO 1 Element 1 */
#define CAN_RXF1E_1_RESETVALUE      0x00000000u  /**< \brief (CAN_RXF1E_1 reset_value) Rx FIFO 1 Element 1 */

#define CAN_RXF1E_1_RXTS_Pos        0            /**< \brief (CAN_RXF1E_1) Rx Timestamp */
#define CAN_RXF1E_1_RXTS_Msk        (0xFFFFu << CAN_RXF1E_1_RXTS_Pos)
#define CAN_RXF1E_1_RXTS(value)     (CAN_RXF1E_1_RXTS_Msk & ((value) << CAN_RXF1E_1_RXTS_Pos))
#define CAN_RXF1E_1_DLC_Pos         16           /**< \brief (CAN_RXF1E_1) Data Length Code */
#define CAN_RXF1E_1_DLC_Msk         (0xFu << CAN_RXF1E_1_DLC_Pos)
#define CAN_RXF1E_1_DLC(value)      (CAN_RXF1E_1_DLC_Msk & ((value) << CAN_RXF1E_1_DLC_Pos))
#define CAN_RXF1E_1_BRS_Pos         20           /**< \brief (CAN_RXF1E_1) Bit Rate Search */
#define CAN_RXF1E_1_BRS             (0x1u << CAN_RXF1E_1_BRS_Pos)
#define CAN_RXF1E_1_FDF_Pos         21           /**< \brief (CAN_RXF1E_1) FD Format */
#define CAN_RXF1E_1_FDF             (0x1u << CAN_RXF1E_1_FDF_Pos)
#define CAN_RXF1E_1_FIDX_Pos        24           /**< \brief (CAN_RXF1E_1) Filter Index */
#define CAN_RXF1E_1_FIDX_Msk        (0x7Fu << CAN_RXF1E_1_FIDX_Pos)
#define CAN_RXF1E_1_FIDX(value)     (CAN_RXF1E_1_FIDX_Msk & ((value) << CAN_RXF1E_1_FIDX_Pos))
#define CAN_RXF1E_1_ANMF_Pos        31           /**< \brief (CAN_RXF1E_1) Accepted Non-matching Frame */
#define CAN_RXF1E_1_ANMF            (0x1u << CAN_RXF1E_1_ANMF_Pos)
#define CAN_RXF1E_1_MASK            0xFF3FFFFFu  /**< \brief (CAN_RXF1E_1) MASK Register */

/* -------- CAN_RXF1E_DATA : (CAN Offset: 0x08) (R/W 32) Rx FIFO 1 Element Data -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DB0:8;            /*!< bit:  0.. 7  Data Byte 0                        */
    uint32_t DB1:8;            /*!< bit:  8..15  Data Byte 1                        */
    uint32_t DB2:8;            /*!< bit: 16..23  Data Byte 2                        */
    uint32_t DB3:8;            /*!< bit: 24..31  Data Byte 3                        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_RXF1E_DATA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_RXF1E_DATA_OFFSET       0x08         /**< \brief (CAN_RXF1E_DATA offset) Rx FIFO 1 Element Data */
#define CAN_RXF1E_DATA_RESETVALUE   0x00000000u  /**< \brief (CAN_RXF1E_DATA reset_value) Rx FIFO 1 Element Data */

#define CAN_RXF1E_DATA_DB0_Pos      0            /**< \brief (CAN_RXF1E_DATA) Data Byte 0 */
#define CAN_RXF1E_DATA_DB0_Msk      (0xFFu << CAN_RXF1E_DATA_DB0_Pos)
#define CAN_RXF1E_DATA_DB0(value)   (CAN_RXF1E_DATA_DB0_Msk & ((value) << CAN_RXF1E_DATA_DB0_Pos))
#define CAN_RXF1E_DATA_DB1_Pos      8            /**< \brief (CAN_RXF1E_DATA) Data Byte 1 */
#define CAN_RXF1E_DATA_DB1_Msk      (0xFFu << CAN_RXF1E_DATA_DB1_Pos)
#define CAN_RXF1E_DATA_DB1(value)   (CAN_RXF1E_DATA_DB1_Msk & ((value) << CAN_RXF1E_DATA_DB1_Pos))
#define CAN_RXF1E_DATA_DB2_Pos      16           /**< \brief (CAN_RXF1E_DATA) Data Byte 2 */
#define CAN_RXF1E_DATA_DB2_Msk      (0xFFu << CAN_RXF1E_DATA_DB2_Pos)
#define CAN_RXF1E_DATA_DB2(value)   (CAN_RXF1E_DATA_DB2_Msk & ((value) << CAN_RXF1E_DATA_DB2_Pos))
#define CAN_RXF1E_DATA_DB3_Pos      24           /**< \brief (CAN_RXF1E_DATA) Data Byte 3 */
#define CAN_RXF1E_DATA_DB3_Msk      (0xFFu << CAN_RXF1E_DATA_DB3_Pos)
#define CAN_RXF1E_DATA_DB3(value)   (CAN_RXF1E_DATA_DB3_Msk & ((value) << CAN_RXF1E_DATA_DB3_Pos))
#define CAN_RXF1E_DATA_MASK         0xFFFFFFFFu  /**< \brief (CAN_RXF1E_DATA) MASK Register */

/* -------- CAN_SIDFE_0 : (CAN Offset: 0x00) (R/W 32) Standard Message ID Filter Element -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SFID2:11;         /*!< bit:  0..10  Standard Filter ID 2               */
    uint32_t :5;               /*!< bit: 11..15  Reserved                           */
    uint32_t SFID1:11;         /*!< bit: 16..26  Standard Filter ID 1               */
    uint32_t SFEC:3;           /*!< bit: 27..29  Standard Filter Element Configuration */
    uint32_t SFT:2;            /*!< bit: 30..31  Standard Filter Type               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_SIDFE_0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_SIDFE_0_OFFSET          0x00         /**< \brief (CAN_SIDFE_0 offset) Standard Message ID Filter Element */
#define CAN_SIDFE_0_RESETVALUE      0x00000000u  /**< \brief (CAN_SIDFE_0 reset_value) Standard Message ID Filter Element */

#define CAN_SIDFE_0_SFID2_Pos       0            /**< \brief (CAN_SIDFE_0) Standard Filter ID 2 */
#define CAN_SIDFE_0_SFID2_Msk       (0x7FFu << CAN_SIDFE_0_SFID2_Pos)
#define CAN_SIDFE_0_SFID2(value)    (CAN_SIDFE_0_SFID2_Msk & ((value) << CAN_SIDFE_0_SFID2_Pos))
#define CAN_SIDFE_0_SFID1_Pos       16           /**< \brief (CAN_SIDFE_0) Standard Filter ID 1 */
#define CAN_SIDFE_0_SFID1_Msk       (0x7FFu << CAN_SIDFE_0_SFID1_Pos)
#define CAN_SIDFE_0_SFID1(value)    (CAN_SIDFE_0_SFID1_Msk & ((value) << CAN_SIDFE_0_SFID1_Pos))
#define CAN_SIDFE_0_SFEC_Pos        27           /**< \brief (CAN_SIDFE_0) Standard Filter Element Configuration */
#define CAN_SIDFE_0_SFEC_Msk        (0x7u << CAN_SIDFE_0_SFEC_Pos)
#define CAN_SIDFE_0_SFEC(value)     (CAN_SIDFE_0_SFEC_Msk & ((value) << CAN_SIDFE_0_SFEC_Pos))
#define   CAN_SIDFE_0_SFEC_DISABLE_Val    0x0u   /**< \brief (CAN_SIDFE_0) Disable filter element */
#define   CAN_SIDFE_0_SFEC_STF0M_Val      0x1u   /**< \brief (CAN_SIDFE_0) Store in Rx FIFO 0 if filter match */
#define   CAN_SIDFE_0_SFEC_STF1M_Val      0x2u   /**< \brief (CAN_SIDFE_0) Store in Rx FIFO 1 if filter match */
#define   CAN_SIDFE_0_SFEC_REJECT_Val     0x3u   /**< \brief (CAN_SIDFE_0) Reject ID if filter match */
#define   CAN_SIDFE_0_SFEC_PRIORITY_Val   0x4u   /**< \brief (CAN_SIDFE_0) Set priority if filter match */
#define   CAN_SIDFE_0_SFEC_PRIF0M_Val     0x5u   /**< \brief (CAN_SIDFE_0) Set priority and store in FIFO 0 if filter match */
#define   CAN_SIDFE_0_SFEC_PRIF1M_Val     0x6u   /**< \brief (CAN_SIDFE_0) Set priority and store in FIFO 1 if filter match */
#define   CAN_SIDFE_0_SFEC_STRXBUF_Val    0x7u   /**< \brief (CAN_SIDFE_0) Store into Rx Buffer */
#define CAN_SIDFE_0_SFEC_DISABLE    (CAN_SIDFE_0_SFEC_DISABLE_Val  << CAN_SIDFE_0_SFEC_Pos)
#define CAN_SIDFE_0_SFEC_STF0M      (CAN_SIDFE_0_SFEC_STF0M_Val    << CAN_SIDFE_0_SFEC_Pos)
#define CAN_SIDFE_0_SFEC_STF1M      (CAN_SIDFE_0_SFEC_STF1M_Val    << CAN_SIDFE_0_SFEC_Pos)
#define CAN_SIDFE_0_SFEC_REJECT     (CAN_SIDFE_0_SFEC_REJECT_Val   << CAN_SIDFE_0_SFEC_Pos)
#define CAN_SIDFE_0_SFEC_PRIORITY   (CAN_SIDFE_0_SFEC_PRIORITY_Val << CAN_SIDFE_0_SFEC_Pos)
#define CAN_SIDFE_0_SFEC_PRIF0M     (CAN_SIDFE_0_SFEC_PRIF0M_Val   << CAN_SIDFE_0_SFEC_Pos)
#define CAN_SIDFE_0_SFEC_PRIF1M     (CAN_SIDFE_0_SFEC_PRIF1M_Val   << CAN_SIDFE_0_SFEC_Pos)
#define CAN_SIDFE_0_SFEC_STRXBUF    (CAN_SIDFE_0_SFEC_STRXBUF_Val  << CAN_SIDFE_0_SFEC_Pos)
#define CAN_SIDFE_0_SFT_Pos         30           /**< \brief (CAN_SIDFE_0) Standard Filter Type */
#define CAN_SIDFE_0_SFT_Msk         (0x3u << CAN_SIDFE_0_SFT_Pos)
#define CAN_SIDFE_0_SFT(value)      (CAN_SIDFE_0_SFT_Msk & ((value) << CAN_SIDFE_0_SFT_Pos))
#define   CAN_SIDFE_0_SFT_RANGE_Val       0x0u   /**< \brief (CAN_SIDFE_0) Range filter from SFID1 to SFID2 */
#define   CAN_SIDFE_0_SFT_DUAL_Val        0x1u   /**< \brief (CAN_SIDFE_0) Dual ID filter for SFID1 or SFID2 */
#define   CAN_SIDFE_0_SFT_CLASSIC_Val     0x2u   /**< \brief (CAN_SIDFE_0) Classic filter */
#define CAN_SIDFE_0_SFT_RANGE       (CAN_SIDFE_0_SFT_RANGE_Val     << CAN_SIDFE_0_SFT_Pos)
#define CAN_SIDFE_0_SFT_DUAL        (CAN_SIDFE_0_SFT_DUAL_Val      << CAN_SIDFE_0_SFT_Pos)
#define CAN_SIDFE_0_SFT_CLASSIC     (CAN_SIDFE_0_SFT_CLASSIC_Val   << CAN_SIDFE_0_SFT_Pos)
#define CAN_SIDFE_0_MASK            0xFFFF07FFu  /**< \brief (CAN_SIDFE_0) MASK Register */

/* -------- CAN_TXBE_0 : (CAN Offset: 0x00) (R/W 32) Tx Buffer Element 0 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ID:29;            /*!< bit:  0..28  Identifier                         */
    uint32_t RTR:1;            /*!< bit:     29  Remote Transmission Request        */
    uint32_t XTD:1;            /*!< bit:     30  Extended Identifier                */
    uint32_t ESI:1;            /*!< bit:     31  Error State Indicator              */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TXBE_0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TXBE_0_OFFSET           0x00         /**< \brief (CAN_TXBE_0 offset) Tx Buffer Element 0 */
#define CAN_TXBE_0_RESETVALUE       0x00000000u  /**< \brief (CAN_TXBE_0 reset_value) Tx Buffer Element 0 */

#define CAN_TXBE_0_ID_Pos           0            /**< \brief (CAN_TXBE_0) Identifier */
#define CAN_TXBE_0_ID_Msk           (0x1FFFFFFFu << CAN_TXBE_0_ID_Pos)
#define CAN_TXBE_0_ID(value)        (CAN_TXBE_0_ID_Msk & ((value) << CAN_TXBE_0_ID_Pos))
#define CAN_TXBE_0_RTR_Pos          29           /**< \brief (CAN_TXBE_0) Remote Transmission Request */
#define CAN_TXBE_0_RTR              (0x1u << CAN_TXBE_0_RTR_Pos)
#define CAN_TXBE_0_XTD_Pos          30           /**< \brief (CAN_TXBE_0) Extended Identifier */
#define CAN_TXBE_0_XTD              (0x1u << CAN_TXBE_0_XTD_Pos)
#define CAN_TXBE_0_ESI_Pos          31           /**< \brief (CAN_TXBE_0) Error State Indicator */
#define CAN_TXBE_0_ESI              (0x1u << CAN_TXBE_0_ESI_Pos)
#define CAN_TXBE_0_MASK             0xFFFFFFFFu  /**< \brief (CAN_TXBE_0) MASK Register */

/* -------- CAN_TXBE_1 : (CAN Offset: 0x04) (R/W 32) Tx Buffer Element 1 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :16;              /*!< bit:  0..15  Reserved                           */
    uint32_t DLC:4;            /*!< bit: 16..19  Identifier                         */
    uint32_t BRS:1;            /*!< bit:     20  Bit Rate Search                    */
    uint32_t FDF:1;            /*!< bit:     21  FD Format                          */
    uint32_t :1;               /*!< bit:     22  Reserved                           */
    uint32_t EFC:1;            /*!< bit:     23  Event FIFO Control                 */
    uint32_t MM:8;             /*!< bit: 24..31  Message Marker                     */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TXBE_1_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TXBE_1_OFFSET           0x04         /**< \brief (CAN_TXBE_1 offset) Tx Buffer Element 1 */
#define CAN_TXBE_1_RESETVALUE       0x00000000u  /**< \brief (CAN_TXBE_1 reset_value) Tx Buffer Element 1 */

#define CAN_TXBE_1_DLC_Pos          16           /**< \brief (CAN_TXBE_1) Identifier */
#define CAN_TXBE_1_DLC_Msk          (0xFu << CAN_TXBE_1_DLC_Pos)
#define CAN_TXBE_1_DLC(value)       (CAN_TXBE_1_DLC_Msk & ((value) << CAN_TXBE_1_DLC_Pos))
#define CAN_TXBE_1_BRS_Pos          20           /**< \brief (CAN_TXBE_1) Bit Rate Search */
#define CAN_TXBE_1_BRS              (0x1u << CAN_TXBE_1_BRS_Pos)
#define CAN_TXBE_1_FDF_Pos          21           /**< \brief (CAN_TXBE_1) FD Format */
#define CAN_TXBE_1_FDF              (0x1u << CAN_TXBE_1_FDF_Pos)
#define CAN_TXBE_1_EFC_Pos          23           /**< \brief (CAN_TXBE_1) Event FIFO Control */
#define CAN_TXBE_1_EFC              (0x1u << CAN_TXBE_1_EFC_Pos)
#define CAN_TXBE_1_MM_Pos           24           /**< \brief (CAN_TXBE_1) Message Marker */
#define CAN_TXBE_1_MM_Msk           (0xFFu << CAN_TXBE_1_MM_Pos)
#define CAN_TXBE_1_MM(value)        (CAN_TXBE_1_MM_Msk & ((value) << CAN_TXBE_1_MM_Pos))
#define CAN_TXBE_1_MASK             0xFFBF0000u  /**< \brief (CAN_TXBE_1) MASK Register */

/* -------- CAN_TXBE_DATA : (CAN Offset: 0x08) (R/W 32) Tx Buffer Element Data -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DB0:8;            /*!< bit:  0.. 7  Data Byte 0                        */
    uint32_t DB1:8;            /*!< bit:  8..15  Data Byte 1                        */
    uint32_t DB2:8;            /*!< bit: 16..23  Data Byte 2                        */
    uint32_t DB3:8;            /*!< bit: 24..31  Data Byte 3                        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TXBE_DATA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TXBE_DATA_OFFSET        0x08         /**< \brief (CAN_TXBE_DATA offset) Tx Buffer Element Data */
#define CAN_TXBE_DATA_RESETVALUE    0x00000000u  /**< \brief (CAN_TXBE_DATA reset_value) Tx Buffer Element Data */

#define CAN_TXBE_DATA_DB0_Pos       0            /**< \brief (CAN_TXBE_DATA) Data Byte 0 */
#define CAN_TXBE_DATA_DB0_Msk       (0xFFu << CAN_TXBE_DATA_DB0_Pos)
#define CAN_TXBE_DATA_DB0(value)    (CAN_TXBE_DATA_DB0_Msk & ((value) << CAN_TXBE_DATA_DB0_Pos))
#define CAN_TXBE_DATA_DB1_Pos       8            /**< \brief (CAN_TXBE_DATA) Data Byte 1 */
#define CAN_TXBE_DATA_DB1_Msk       (0xFFu << CAN_TXBE_DATA_DB1_Pos)
#define CAN_TXBE_DATA_DB1(value)    (CAN_TXBE_DATA_DB1_Msk & ((value) << CAN_TXBE_DATA_DB1_Pos))
#define CAN_TXBE_DATA_DB2_Pos       16           /**< \brief (CAN_TXBE_DATA) Data Byte 2 */
#define CAN_TXBE_DATA_DB2_Msk       (0xFFu << CAN_TXBE_DATA_DB2_Pos)
#define CAN_TXBE_DATA_DB2(value)    (CAN_TXBE_DATA_DB2_Msk & ((value) << CAN_TXBE_DATA_DB2_Pos))
#define CAN_TXBE_DATA_DB3_Pos       24           /**< \brief (CAN_TXBE_DATA) Data Byte 3 */
#define CAN_TXBE_DATA_DB3_Msk       (0xFFu << CAN_TXBE_DATA_DB3_Pos)
#define CAN_TXBE_DATA_DB3(value)    (CAN_TXBE_DATA_DB3_Msk & ((value) << CAN_TXBE_DATA_DB3_Pos))
#define CAN_TXBE_DATA_MASK          0xFFFFFFFFu  /**< \brief (CAN_TXBE_DATA) MASK Register */

/* -------- CAN_TXEFE_0 : (CAN Offset: 0x00) (R/W 32) Tx Event FIFO Element 0 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ID:29;            /*!< bit:  0..28  Identifier                         */
    uint32_t RTR:1;            /*!< bit:     29  Remote Transmission Request        */
    uint32_t XTD:1;            /*!< bit:     30  Extended Indentifier               */
    uint32_t ESI:1;            /*!< bit:     31  Error State Indicator              */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TXEFE_0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TXEFE_0_OFFSET          0x00         /**< \brief (CAN_TXEFE_0 offset) Tx Event FIFO Element 0 */
#define CAN_TXEFE_0_RESETVALUE      0x00000000u  /**< \brief (CAN_TXEFE_0 reset_value) Tx Event FIFO Element 0 */

#define CAN_TXEFE_0_ID_Pos          0            /**< \brief (CAN_TXEFE_0) Identifier */
#define CAN_TXEFE_0_ID_Msk          (0x1FFFFFFFu << CAN_TXEFE_0_ID_Pos)
#define CAN_TXEFE_0_ID(value)       (CAN_TXEFE_0_ID_Msk & ((value) << CAN_TXEFE_0_ID_Pos))
#define CAN_TXEFE_0_RTR_Pos         29           /**< \brief (CAN_TXEFE_0) Remote Transmission Request */
#define CAN_TXEFE_0_RTR             (0x1u << CAN_TXEFE_0_RTR_Pos)
#define CAN_TXEFE_0_XTD_Pos         30           /**< \brief (CAN_TXEFE_0) Extended Indentifier */
#define CAN_TXEFE_0_XTD             (0x1u << CAN_TXEFE_0_XTD_Pos)
#define CAN_TXEFE_0_ESI_Pos         31           /**< \brief (CAN_TXEFE_0) Error State Indicator */
#define CAN_TXEFE_0_ESI             (0x1u << CAN_TXEFE_0_ESI_Pos)
#define CAN_TXEFE_0_MASK            0xFFFFFFFFu  /**< \brief (CAN_TXEFE_0) MASK Register */

/* -------- CAN_TXEFE_1 : (CAN Offset: 0x04) (R/W 32) Tx Event FIFO Element 1 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t TXTS:16;          /*!< bit:  0..15  Tx Timestamp                       */
    uint32_t DLC:4;            /*!< bit: 16..19  Data Length Code                   */
    uint32_t BRS:1;            /*!< bit:     20  Bit Rate Search                    */
    uint32_t FDF:1;            /*!< bit:     21  FD Format                          */
    uint32_t ET:2;             /*!< bit: 22..23  Event Type                         */
    uint32_t MM:8;             /*!< bit: 24..31  Message Marker                     */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_TXEFE_1_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_TXEFE_1_OFFSET          0x04         /**< \brief (CAN_TXEFE_1 offset) Tx Event FIFO Element 1 */
#define CAN_TXEFE_1_RESETVALUE      0x00000000u  /**< \brief (CAN_TXEFE_1 reset_value) Tx Event FIFO Element 1 */

#define CAN_TXEFE_1_TXTS_Pos        0            /**< \brief (CAN_TXEFE_1) Tx Timestamp */
#define CAN_TXEFE_1_TXTS_Msk        (0xFFFFu << CAN_TXEFE_1_TXTS_Pos)
#define CAN_TXEFE_1_TXTS(value)     (CAN_TXEFE_1_TXTS_Msk & ((value) << CAN_TXEFE_1_TXTS_Pos))
#define CAN_TXEFE_1_DLC_Pos         16           /**< \brief (CAN_TXEFE_1) Data Length Code */
#define CAN_TXEFE_1_DLC_Msk         (0xFu << CAN_TXEFE_1_DLC_Pos)
#define CAN_TXEFE_1_DLC(value)      (CAN_TXEFE_1_DLC_Msk & ((value) << CAN_TXEFE_1_DLC_Pos))
#define CAN_TXEFE_1_BRS_Pos         20           /**< \brief (CAN_TXEFE_1) Bit Rate Search */
#define CAN_TXEFE_1_BRS             (0x1u << CAN_TXEFE_1_BRS_Pos)
#define CAN_TXEFE_1_FDF_Pos         21           /**< \brief (CAN_TXEFE_1) FD Format */
#define CAN_TXEFE_1_FDF             (0x1u << CAN_TXEFE_1_FDF_Pos)
#define CAN_TXEFE_1_ET_Pos          22           /**< \brief (CAN_TXEFE_1) Event Type */
#define CAN_TXEFE_1_ET_Msk          (0x3u << CAN_TXEFE_1_ET_Pos)
#define CAN_TXEFE_1_ET(value)       (CAN_TXEFE_1_ET_Msk & ((value) << CAN_TXEFE_1_ET_Pos))
#define   CAN_TXEFE_1_ET_TXE_Val          0x1u   /**< \brief (CAN_TXEFE_1) Tx event */
#define   CAN_TXEFE_1_ET_TXC_Val          0x2u   /**< \brief (CAN_TXEFE_1) Transmission in spite of cancellation */
#define CAN_TXEFE_1_ET_TXE          (CAN_TXEFE_1_ET_TXE_Val        << CAN_TXEFE_1_ET_Pos)
#define CAN_TXEFE_1_ET_TXC          (CAN_TXEFE_1_ET_TXC_Val        << CAN_TXEFE_1_ET_Pos)
#define CAN_TXEFE_1_MM_Pos          24           /**< \brief (CAN_TXEFE_1) Message Marker */
#define CAN_TXEFE_1_MM_Msk          (0xFFu << CAN_TXEFE_1_MM_Pos)
#define CAN_TXEFE_1_MM(value)       (CAN_TXEFE_1_MM_Msk & ((value) << CAN_TXEFE_1_MM_Pos))
#define CAN_TXEFE_1_MASK            0xFFFFFFFFu  /**< \brief (CAN_TXEFE_1) MASK Register */

/* -------- CAN_XIDFE_0 : (CAN Offset: 0x00) (R/W 32) Extended Message ID Filter Element 0 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EFID1:29;         /*!< bit:  0..28  Extended Filter ID 1               */
    uint32_t EFEC:3;           /*!< bit: 29..31  Extended Filter Element Configuration */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_XIDFE_0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_XIDFE_0_OFFSET          0x00         /**< \brief (CAN_XIDFE_0 offset) Extended Message ID Filter Element 0 */
#define CAN_XIDFE_0_RESETVALUE      0x00000000u  /**< \brief (CAN_XIDFE_0 reset_value) Extended Message ID Filter Element 0 */

#define CAN_XIDFE_0_EFID1_Pos       0            /**< \brief (CAN_XIDFE_0) Extended Filter ID 1 */
#define CAN_XIDFE_0_EFID1_Msk       (0x1FFFFFFFu << CAN_XIDFE_0_EFID1_Pos)
#define CAN_XIDFE_0_EFID1(value)    (CAN_XIDFE_0_EFID1_Msk & ((value) << CAN_XIDFE_0_EFID1_Pos))
#define CAN_XIDFE_0_EFEC_Pos        29           /**< \brief (CAN_XIDFE_0) Extended Filter Element Configuration */
#define CAN_XIDFE_0_EFEC_Msk        (0x7u << CAN_XIDFE_0_EFEC_Pos)
#define CAN_XIDFE_0_EFEC(value)     (CAN_XIDFE_0_EFEC_Msk & ((value) << CAN_XIDFE_0_EFEC_Pos))
#define   CAN_XIDFE_0_EFEC_DISABLE_Val    0x0u   /**< \brief (CAN_XIDFE_0) Disable filter element */
#define   CAN_XIDFE_0_EFEC_STF0M_Val      0x1u   /**< \brief (CAN_XIDFE_0) Store in Rx FIFO 0 if filter match */
#define   CAN_XIDFE_0_EFEC_STF1M_Val      0x2u   /**< \brief (CAN_XIDFE_0) Store in Rx FIFO 1 if filter match */
#define   CAN_XIDFE_0_EFEC_REJECT_Val     0x3u   /**< \brief (CAN_XIDFE_0) Reject ID if filter match */
#define   CAN_XIDFE_0_EFEC_PRIORITY_Val   0x4u   /**< \brief (CAN_XIDFE_0) Set priority if filter match */
#define   CAN_XIDFE_0_EFEC_PRIF0M_Val     0x5u   /**< \brief (CAN_XIDFE_0) Set priority and store in FIFO 0 if filter match */
#define   CAN_XIDFE_0_EFEC_PRIF1M_Val     0x6u   /**< \brief (CAN_XIDFE_0) Set priority and store in FIFO 1 if filter match */
#define   CAN_XIDFE_0_EFEC_STRXBUF_Val    0x7u   /**< \brief (CAN_XIDFE_0) Store into Rx Buffer */
#define CAN_XIDFE_0_EFEC_DISABLE    (CAN_XIDFE_0_EFEC_DISABLE_Val  << CAN_XIDFE_0_EFEC_Pos)
#define CAN_XIDFE_0_EFEC_STF0M      (CAN_XIDFE_0_EFEC_STF0M_Val    << CAN_XIDFE_0_EFEC_Pos)
#define CAN_XIDFE_0_EFEC_STF1M      (CAN_XIDFE_0_EFEC_STF1M_Val    << CAN_XIDFE_0_EFEC_Pos)
#define CAN_XIDFE_0_EFEC_REJECT     (CAN_XIDFE_0_EFEC_REJECT_Val   << CAN_XIDFE_0_EFEC_Pos)
#define CAN_XIDFE_0_EFEC_PRIORITY   (CAN_XIDFE_0_EFEC_PRIORITY_Val << CAN_XIDFE_0_EFEC_Pos)
#define CAN_XIDFE_0_EFEC_PRIF0M     (CAN_XIDFE_0_EFEC_PRIF0M_Val   << CAN_XIDFE_0_EFEC_Pos)
#define CAN_XIDFE_0_EFEC_PRIF1M     (CAN_XIDFE_0_EFEC_PRIF1M_Val   << CAN_XIDFE_0_EFEC_Pos)
#define CAN_XIDFE_0_EFEC_STRXBUF    (CAN_XIDFE_0_EFEC_STRXBUF_Val  << CAN_XIDFE_0_EFEC_Pos)
#define CAN_XIDFE_0_MASK            0xFFFFFFFFu  /**< \brief (CAN_XIDFE_0) MASK Register */

/* -------- CAN_XIDFE_1 : (CAN Offset: 0x04) (R/W 32) Extended Message ID Filter Element 1 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EFID2:29;         /*!< bit:  0..28  Extended Filter ID 2               */
    uint32_t :1;               /*!< bit:     29  Reserved                           */
    uint32_t EFT:2;            /*!< bit: 30..31  Extended Filter Type               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CAN_XIDFE_1_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CAN_XIDFE_1_OFFSET          0x04         /**< \brief (CAN_XIDFE_1 offset) Extended Message ID Filter Element 1 */
#define CAN_XIDFE_1_RESETVALUE      0x00000000u  /**< \brief (CAN_XIDFE_1 reset_value) Extended Message ID Filter Element 1 */

#define CAN_XIDFE_1_EFID2_Pos       0            /**< \brief (CAN_XIDFE_1) Extended Filter ID 2 */
#define CAN_XIDFE_1_EFID2_Msk       (0x1FFFFFFFu << CAN_XIDFE_1_EFID2_Pos)
#define CAN_XIDFE_1_EFID2(value)    (CAN_XIDFE_1_EFID2_Msk & ((value) << CAN_XIDFE_1_EFID2_Pos))
#define CAN_XIDFE_1_EFT_Pos         30           /**< \brief (CAN_XIDFE_1) Extended Filter Type */
#define CAN_XIDFE_1_EFT_Msk         (0x3u << CAN_XIDFE_1_EFT_Pos)
#define CAN_XIDFE_1_EFT(value)      (CAN_XIDFE_1_EFT_Msk & ((value) << CAN_XIDFE_1_EFT_Pos))
#define   CAN_XIDFE_1_EFT_RANGEM_Val      0x0u   /**< \brief (CAN_XIDFE_1) Range filter from EFID1 to EFID2 */
#define   CAN_XIDFE_1_EFT_DUAL_Val        0x1u   /**< \brief (CAN_XIDFE_1) Dual ID filter for EFID1 or EFID2 */
#define   CAN_XIDFE_1_EFT_CLASSIC_Val     0x2u   /**< \brief (CAN_XIDFE_1) Classic filter */
#define   CAN_XIDFE_1_EFT_RANGE_Val       0x3u   /**< \brief (CAN_XIDFE_1) Range filter from EFID1 to EFID2 with no XIDAM mask */
#define CAN_XIDFE_1_EFT_RANGEM      (CAN_XIDFE_1_EFT_RANGEM_Val    << CAN_XIDFE_1_EFT_Pos)
#define CAN_XIDFE_1_EFT_DUAL        (CAN_XIDFE_1_EFT_DUAL_Val      << CAN_XIDFE_1_EFT_Pos)
#define CAN_XIDFE_1_EFT_CLASSIC     (CAN_XIDFE_1_EFT_CLASSIC_Val   << CAN_XIDFE_1_EFT_Pos)
#define CAN_XIDFE_1_EFT_RANGE       (CAN_XIDFE_1_EFT_RANGE_Val     << CAN_XIDFE_1_EFT_Pos)
#define CAN_XIDFE_1_MASK            0xDFFFFFFFu  /**< \brief (CAN_XIDFE_1) MASK Register */

/** \brief CAN APB hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __I  CAN_CREL_Type             CREL;        /**< \brief Offset: 0x00 (R/  32) Core Release */
  __I  CAN_ENDN_Type             ENDN;        /**< \brief Offset: 0x04 (R/  32) Endian */
  __IO CAN_MRCFG_Type            MRCFG;       /**< \brief Offset: 0x08 (R/W 32) Message RAM Configuration */
  __IO CAN_DBTP_Type             DBTP;        /**< \brief Offset: 0x0C (R/W 32) Fast Bit Timing and Prescaler */
  __IO CAN_TEST_Type             TEST;        /**< \brief Offset: 0x10 (R/W 32) Test */
  __IO CAN_RWD_Type              RWD;         /**< \brief Offset: 0x14 (R/W 32) RAM Watchdog */
  __IO CAN_CCCR_Type             CCCR;        /**< \brief Offset: 0x18 (R/W 32) CC Control */
  __IO CAN_NBTP_Type             NBTP;        /**< \brief Offset: 0x1C (R/W 32) Nominal Bit Timing and Prescaler */
  __IO CAN_TSCC_Type             TSCC;        /**< \brief Offset: 0x20 (R/W 32) Timestamp Counter Configuration */
  __I  CAN_TSCV_Type             TSCV;        /**< \brief Offset: 0x24 (R/  32) Timestamp Counter Value */
  __IO CAN_TOCC_Type             TOCC;        /**< \brief Offset: 0x28 (R/W 32) Timeout Counter Configuration */
  __IO CAN_TOCV_Type             TOCV;        /**< \brief Offset: 0x2C (R/W 32) Timeout Counter Value */
       RoReg8                    Reserved1[0x10];
  __I  CAN_ECR_Type              ECR;         /**< \brief Offset: 0x40 (R/  32) Error Counter */
  __I  CAN_PSR_Type              PSR;         /**< \brief Offset: 0x44 (R/  32) Protocol Status */
  __IO CAN_TDCR_Type             TDCR;        /**< \brief Offset: 0x48 (R/W 32) Extended ID Filter Configuration */
       RoReg8                    Reserved2[0x4];
  __IO CAN_IR_Type               IR;          /**< \brief Offset: 0x50 (R/W 32) Interrupt */
  __IO CAN_IE_Type               IE;          /**< \brief Offset: 0x54 (R/W 32) Interrupt Enable */
  __IO CAN_ILS_Type              ILS;         /**< \brief Offset: 0x58 (R/W 32) Interrupt Line Select */
  __IO CAN_ILE_Type              ILE;         /**< \brief Offset: 0x5C (R/W 32) Interrupt Line Enable */
       RoReg8                    Reserved3[0x20];
  __IO CAN_GFC_Type              GFC;         /**< \brief Offset: 0x80 (R/W 32) Global Filter Configuration */
  __IO CAN_SIDFC_Type            SIDFC;       /**< \brief Offset: 0x84 (R/W 32) Standard ID Filter Configuration */
  __IO CAN_XIDFC_Type            XIDFC;       /**< \brief Offset: 0x88 (R/W 32) Extended ID Filter Configuration */
       RoReg8                    Reserved4[0x4];
  __IO CAN_XIDAM_Type            XIDAM;       /**< \brief Offset: 0x90 (R/W 32) Extended ID AND Mask */
  __I  CAN_HPMS_Type             HPMS;        /**< \brief Offset: 0x94 (R/  32) High Priority Message Status */
  __IO CAN_NDAT1_Type            NDAT1;       /**< \brief Offset: 0x98 (R/W 32) New Data 1 */
  __IO CAN_NDAT2_Type            NDAT2;       /**< \brief Offset: 0x9C (R/W 32) New Data 2 */
  __IO CAN_RXF0C_Type            RXF0C;       /**< \brief Offset: 0xA0 (R/W 32) Rx FIFO 0 Configuration */
  __I  CAN_RXF0S_Type            RXF0S;       /**< \brief Offset: 0xA4 (R/  32) Rx FIFO 0 Status */
  __IO CAN_RXF0A_Type            RXF0A;       /**< \brief Offset: 0xA8 (R/W 32) Rx FIFO 0 Acknowledge */
  __IO CAN_RXBC_Type             RXBC;        /**< \brief Offset: 0xAC (R/W 32) Rx Buffer Configuration */
  __IO CAN_RXF1C_Type            RXF1C;       /**< \brief Offset: 0xB0 (R/W 32) Rx FIFO 1 Configuration */
  __I  CAN_RXF1S_Type            RXF1S;       /**< \brief Offset: 0xB4 (R/  32) Rx FIFO 1 Status */
  __IO CAN_RXF1A_Type            RXF1A;       /**< \brief Offset: 0xB8 (R/W 32) Rx FIFO 1 Acknowledge */
  __IO CAN_RXESC_Type            RXESC;       /**< \brief Offset: 0xBC (R/W 32) Rx Buffer / FIFO Element Size Configuration */
  __IO CAN_TXBC_Type             TXBC;        /**< \brief Offset: 0xC0 (R/W 32) Tx Buffer Configuration */
  __I  CAN_TXFQS_Type            TXFQS;       /**< \brief Offset: 0xC4 (R/  32) Tx FIFO / Queue Status */
  __IO CAN_TXESC_Type            TXESC;       /**< \brief Offset: 0xC8 (R/W 32) Tx Buffer Element Size Configuration */
  __I  CAN_TXBRP_Type            TXBRP;       /**< \brief Offset: 0xCC (R/  32) Tx Buffer Request Pending */
  __IO CAN_TXBAR_Type            TXBAR;       /**< \brief Offset: 0xD0 (R/W 32) Tx Buffer Add Request */
  __IO CAN_TXBCR_Type            TXBCR;       /**< \brief Offset: 0xD4 (R/W 32) Tx Buffer Cancellation Request */
  __I  CAN_TXBTO_Type            TXBTO;       /**< \brief Offset: 0xD8 (R/  32) Tx Buffer Transmission Occurred */
  __I  CAN_TXBCF_Type            TXBCF;       /**< \brief Offset: 0xDC (R/  32) Tx Buffer Cancellation Finished */
  __IO CAN_TXBTIE_Type           TXBTIE;      /**< \brief Offset: 0xE0 (R/W 32) Tx Buffer Transmission Interrupt Enable */
  __IO CAN_TXBCIE_Type           TXBCIE;      /**< \brief Offset: 0xE4 (R/W 32) Tx Buffer Cancellation Finished Interrupt Enable */
       RoReg8                    Reserved5[0x8];
  __IO CAN_TXEFC_Type            TXEFC;       /**< \brief Offset: 0xF0 (R/W 32) Tx Event FIFO Configuration */
  __I  CAN_TXEFS_Type            TXEFS;       /**< \brief Offset: 0xF4 (R/  32) Tx Event FIFO Status */
  __IO CAN_TXEFA_Type            TXEFA;       /**< \brief Offset: 0xF8 (R/W 32) Tx Event FIFO Acknowledge */
} Can;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief CAN Mram_rxbe hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO CAN_RXBE_0_Type           RXBE_0;      /**< \brief Offset: 0x00 (R/W 32) Rx Buffer Element 0 */
  __IO CAN_RXBE_1_Type           RXBE_1;      /**< \brief Offset: 0x04 (R/W 32) Rx Buffer Element 1 */
  __IO CAN_RXBE_DATA_Type        RXBE_DATA[16]; /**< \brief Offset: 0x08 (R/W 32) Rx Buffer Element Data */
} CanMramRxbe
#ifdef __GNUC__
  __attribute__ ((aligned (4)))
#endif
;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief CAN Mram_rxf0e hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO CAN_RXF0E_0_Type          RXF0E_0;     /**< \brief Offset: 0x00 (R/W 32) Rx FIFO 0 Element 0 */
  __IO CAN_RXF0E_1_Type          RXF0E_1;     /**< \brief Offset: 0x04 (R/W 32) Rx FIFO 0 Element 1 */
  __IO CAN_RXF0E_DATA_Type       RXF0E_DATA[16]; /**< \brief Offset: 0x08 (R/W 32) Rx FIFO 0 Element Data */
} CanMramRxf0e
#ifdef __GNUC__
  __attribute__ ((aligned (4)))
#endif
;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief CAN Mram_rxf1e hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO CAN_RXF1E_0_Type          RXF1E_0;     /**< \brief Offset: 0x00 (R/W 32) Rx FIFO 1 Element 0 */
  __IO CAN_RXF1E_1_Type          RXF1E_1;     /**< \brief Offset: 0x04 (R/W 32) Rx FIFO 1 Element 1 */
  __IO CAN_RXF1E_DATA_Type       RXF1E_DATA[16]; /**< \brief Offset: 0x08 (R/W 32) Rx FIFO 1 Element Data */
} CanMramRxf1e
#ifdef __GNUC__
  __attribute__ ((aligned (4)))
#endif
;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief CAN Mram_sidfe hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO CAN_SIDFE_0_Type          SIDFE_0;     /**< \brief Offset: 0x00 (R/W 32) Standard Message ID Filter Element */
} CanMramSidfe
#ifdef __GNUC__
  __attribute__ ((aligned (4)))
#endif
;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief CAN Mram_txbe hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO CAN_TXBE_0_Type           TXBE_0;      /**< \brief Offset: 0x00 (R/W 32) Tx Buffer Element 0 */
  __IO CAN_TXBE_1_Type           TXBE_1;      /**< \brief Offset: 0x04 (R/W 32) Tx Buffer Element 1 */
  __IO CAN_TXBE_DATA_Type        TXBE_DATA[16]; /**< \brief Offset: 0x08 (R/W 32) Tx Buffer Element Data */
} CanMramTxbe
#ifdef __GNUC__
  __attribute__ ((aligned (4)))
#endif
;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief CAN Mram_txefe hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO CAN_TXEFE_0_Type          TXEFE_0;     /**< \brief Offset: 0x00 (R/W 32) Tx Event FIFO Element 0 */
  __IO CAN_TXEFE_1_Type          TXEFE_1;     /**< \brief Offset: 0x04 (R/W 32) Tx Event FIFO Element 1 */
} CanMramTxefe
#ifdef __GNUC__
  __attribute__ ((aligned (4)))
#endif
;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief CAN Mram_xifde hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO CAN_XIDFE_0_Type          XIDFE_0;     /**< \brief Offset: 0x00 (R/W 32) Extended Message ID Filter Element 0 */
  __IO CAN_XIDFE_1_Type          XIDFE_1;     /**< \brief Offset: 0x04 (R/W 32) Extended Message ID Filter Element 1 */
} CanMramXifde
#ifdef __GNUC__
  __attribute__ ((aligned (4)))
#endif
;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SECTION_CAN_MRAM_RXBE

#define SECTION_CAN_MRAM_RXF0E

#define SECTION_CAN_MRAM_RXF1E

#define SECTION_CAN_MRAM_SIDFE

#define SECTION_CAN_MRAM_TXBE

#define SECTION_CAN_MRAM_TXEFE

#define SECTION_CAN_MRAM_XIFDE

/*@}*/

#endif /* _SAMD51_CAN_COMPONENT_ */
