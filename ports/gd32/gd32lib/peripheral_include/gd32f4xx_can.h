/*!
    \file    gd32f4xx_can.h
    \brief   definitions for the CAN
    
    \version 2026-02-05, V3.3.3, firmware for GD32F4xx
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/


#ifndef GD32F4XX_CAN_H
#define GD32F4XX_CAN_H

#include "gd32f4xx.h"

/* CAN definitions */
#define CAN0                               CAN_BASE                           /*!< CAN0 base address */
#define CAN1                               (CAN0 + 0x00000400U)               /*!< CAN1 base address */

/* registers definitions */
#define CAN_CTL(canx)                      REG32((canx) + 0x00000000U)        /*!< CAN control register */
#define CAN_STAT(canx)                     REG32((canx) + 0x00000004U)        /*!< CAN status register */
#define CAN_TSTAT(canx)                    REG32((canx) + 0x00000008U)        /*!< CAN transmit status register*/
#define CAN_RFIFO0(canx)                   REG32((canx) + 0x0000000CU)        /*!< CAN receive FIFO0 register */
#define CAN_RFIFO1(canx)                   REG32((canx) + 0x00000010U)        /*!< CAN receive FIFO1 register */
#define CAN_INTEN(canx)                    REG32((canx) + 0x00000014U)        /*!< CAN interrupt enable register */
#define CAN_ERR(canx)                      REG32((canx) + 0x00000018U)        /*!< CAN error register */
#define CAN_BT(canx)                       REG32((canx) + 0x0000001CU)        /*!< CAN bit timing register */
#define CAN_TMI0(canx)                     REG32((canx) + 0x00000180U)        /*!< CAN transmit mailbox0 identifier register */
#define CAN_TMP0(canx)                     REG32((canx) + 0x00000184U)        /*!< CAN transmit mailbox0 property register */
#define CAN_TMDATA00(canx)                 REG32((canx) + 0x00000188U)        /*!< CAN transmit mailbox0 data0 register */
#define CAN_TMDATA10(canx)                 REG32((canx) + 0x0000018CU)        /*!< CAN transmit mailbox0 data1 register */
#define CAN_TMI1(canx)                     REG32((canx) + 0x00000190U)        /*!< CAN transmit mailbox1 identifier register */
#define CAN_TMP1(canx)                     REG32((canx) + 0x00000194U)        /*!< CAN transmit mailbox1 property register */
#define CAN_TMDATA01(canx)                 REG32((canx) + 0x00000198U)        /*!< CAN transmit mailbox1 data0 register */
#define CAN_TMDATA11(canx)                 REG32((canx) + 0x0000019CU)        /*!< CAN transmit mailbox1 data1 register */
#define CAN_TMI2(canx)                     REG32((canx) + 0x000001A0U)        /*!< CAN transmit mailbox2 identifier register */
#define CAN_TMP2(canx)                     REG32((canx) + 0x000001A4U)        /*!< CAN transmit mailbox2 property register */
#define CAN_TMDATA02(canx)                 REG32((canx) + 0x000001A8U)        /*!< CAN transmit mailbox2 data0 register */
#define CAN_TMDATA12(canx)                 REG32((canx) + 0x000001ACU)        /*!< CAN transmit mailbox2 data1 register */
#define CAN_RFIFOMI0(canx)                 REG32((canx) + 0x000001B0U)        /*!< CAN receive FIFO0 mailbox identifier register */
#define CAN_RFIFOMP0(canx)                 REG32((canx) + 0x000001B4U)        /*!< CAN receive FIFO0 mailbox property register */
#define CAN_RFIFOMDATA00(canx)             REG32((canx) + 0x000001B8U)        /*!< CAN receive FIFO0 mailbox data0 register */
#define CAN_RFIFOMDATA10(canx)             REG32((canx) + 0x000001BCU)        /*!< CAN receive FIFO0 mailbox data1 register */
#define CAN_RFIFOMI1(canx)                 REG32((canx) + 0x000001C0U)        /*!< CAN receive FIFO1 mailbox identifier register */
#define CAN_RFIFOMP1(canx)                 REG32((canx) + 0x000001C4U)        /*!< CAN receive FIFO1 mailbox property register */
#define CAN_RFIFOMDATA01(canx)             REG32((canx) + 0x000001C8U)        /*!< CAN receive FIFO1 mailbox data0 register */
#define CAN_RFIFOMDATA11(canx)             REG32((canx) + 0x000001CCU)        /*!< CAN receive FIFO1 mailbox data1 register */
#define CAN_FCTL(canx)                     REG32((canx) + 0x00000200U)        /*!< CAN filter control register */
#define CAN_FMCFG(canx)                    REG32((canx) + 0x00000204U)        /*!< CAN filter mode register */
#define CAN_FSCFG(canx)                    REG32((canx) + 0x0000020CU)        /*!< CAN filter scale register */
#define CAN_FAFIFO(canx)                   REG32((canx) + 0x00000214U)        /*!< CAN filter associated FIFO register */
#define CAN_FW(canx)                       REG32((canx) + 0x0000021CU)        /*!< CAN filter working register */
#define CAN_F0DATA0(canx)                  REG32((canx) + 0x00000240U)        /*!< CAN filter 0 data 0 register */
#define CAN_F1DATA0(canx)                  REG32((canx) + 0x00000248U)        /*!< CAN filter 1 data 0 register */
#define CAN_F2DATA0(canx)                  REG32((canx) + 0x00000250U)        /*!< CAN filter 2 data 0 register */
#define CAN_F3DATA0(canx)                  REG32((canx) + 0x00000258U)        /*!< CAN filter 3 data 0 register */
#define CAN_F4DATA0(canx)                  REG32((canx) + 0x00000260U)        /*!< CAN filter 4 data 0 register */
#define CAN_F5DATA0(canx)                  REG32((canx) + 0x00000268U)        /*!< CAN filter 5 data 0 register */
#define CAN_F6DATA0(canx)                  REG32((canx) + 0x00000270U)        /*!< CAN filter 6 data 0 register */
#define CAN_F7DATA0(canx)                  REG32((canx) + 0x00000278U)        /*!< CAN filter 7 data 0 register */
#define CAN_F8DATA0(canx)                  REG32((canx) + 0x00000280U)        /*!< CAN filter 8 data 0 register */
#define CAN_F9DATA0(canx)                  REG32((canx) + 0x00000288U)        /*!< CAN filter 9 data 0 register */
#define CAN_F10DATA0(canx)                 REG32((canx) + 0x00000290U)        /*!< CAN filter 10 data 0 register */
#define CAN_F11DATA0(canx)                 REG32((canx) + 0x00000298U)        /*!< CAN filter 11 data 0 register */
#define CAN_F12DATA0(canx)                 REG32((canx) + 0x000002A0U)        /*!< CAN filter 12 data 0 register */
#define CAN_F13DATA0(canx)                 REG32((canx) + 0x000002A8U)        /*!< CAN filter 13 data 0 register */
#define CAN_F14DATA0(canx)                 REG32((canx) + 0x000002B0U)        /*!< CAN filter 14 data 0 register */
#define CAN_F15DATA0(canx)                 REG32((canx) + 0x000002B8U)        /*!< CAN filter 15 data 0 register */
#define CAN_F16DATA0(canx)                 REG32((canx) + 0x000002C0U)        /*!< CAN filter 16 data 0 register */
#define CAN_F17DATA0(canx)                 REG32((canx) + 0x000002C8U)        /*!< CAN filter 17 data 0 register */
#define CAN_F18DATA0(canx)                 REG32((canx) + 0x000002D0U)        /*!< CAN filter 18 data 0 register */
#define CAN_F19DATA0(canx)                 REG32((canx) + 0x000002D8U)        /*!< CAN filter 19 data 0 register */
#define CAN_F20DATA0(canx)                 REG32((canx) + 0x000002E0U)        /*!< CAN filter 20 data 0 register */
#define CAN_F21DATA0(canx)                 REG32((canx) + 0x000002E8U)        /*!< CAN filter 21 data 0 register */
#define CAN_F22DATA0(canx)                 REG32((canx) + 0x000002F0U)        /*!< CAN filter 22 data 0 register */
#define CAN_F23DATA0(canx)                 REG32((canx) + 0x000002F8U)        /*!< CAN filter 23 data 0 register */
#define CAN_F24DATA0(canx)                 REG32((canx) + 0x00000300U)        /*!< CAN filter 24 data 0 register */
#define CAN_F25DATA0(canx)                 REG32((canx) + 0x00000308U)        /*!< CAN filter 25 data 0 register */
#define CAN_F26DATA0(canx)                 REG32((canx) + 0x00000310U)        /*!< CAN filter 26 data 0 register */
#define CAN_F27DATA0(canx)                 REG32((canx) + 0x00000318U)        /*!< CAN filter 27 data 0 register */
#define CAN_F0DATA1(canx)                  REG32((canx) + 0x00000244U)        /*!< CAN filter 0 data 1 register */
#define CAN_F1DATA1(canx)                  REG32((canx) + 0x0000024CU)        /*!< CAN filter 1 data 1 register */
#define CAN_F2DATA1(canx)                  REG32((canx) + 0x00000254U)        /*!< CAN filter 2 data 1 register */
#define CAN_F3DATA1(canx)                  REG32((canx) + 0x0000025CU)        /*!< CAN filter 3 data 1 register */
#define CAN_F4DATA1(canx)                  REG32((canx) + 0x00000264U)        /*!< CAN filter 4 data 1 register */
#define CAN_F5DATA1(canx)                  REG32((canx) + 0x0000026CU)        /*!< CAN filter 5 data 1 register */
#define CAN_F6DATA1(canx)                  REG32((canx) + 0x00000274U)        /*!< CAN filter 6 data 1 register */
#define CAN_F7DATA1(canx)                  REG32((canx) + 0x0000027CU)        /*!< CAN filter 7 data 1 register */
#define CAN_F8DATA1(canx)                  REG32((canx) + 0x00000284U)        /*!< CAN filter 8 data 1 register */
#define CAN_F9DATA1(canx)                  REG32((canx) + 0x0000028CU)        /*!< CAN filter 9 data 1 register */
#define CAN_F10DATA1(canx)                 REG32((canx) + 0x00000294U)        /*!< CAN filter 10 data 1 register */
#define CAN_F11DATA1(canx)                 REG32((canx) + 0x0000029CU)        /*!< CAN filter 11 data 1 register */
#define CAN_F12DATA1(canx)                 REG32((canx) + 0x000002A4U)        /*!< CAN filter 12 data 1 register */
#define CAN_F13DATA1(canx)                 REG32((canx) + 0x000002ACU)        /*!< CAN filter 13 data 1 register */
#define CAN_F14DATA1(canx)                 REG32((canx) + 0x000002B4U)        /*!< CAN filter 14 data 1 register */
#define CAN_F15DATA1(canx)                 REG32((canx) + 0x000002BCU)        /*!< CAN filter 15 data 1 register */
#define CAN_F16DATA1(canx)                 REG32((canx) + 0x000002C4U)        /*!< CAN filter 16 data 1 register */
#define CAN_F17DATA1(canx)                 REG32((canx) + 0x000002CCU)        /*!< CAN filter 17 data 1 register */
#define CAN_F18DATA1(canx)                 REG32((canx) + 0x000002D4U)        /*!< CAN filter 18 data 1 register */
#define CAN_F19DATA1(canx)                 REG32((canx) + 0x000002DCU)        /*!< CAN filter 19 data 1 register */
#define CAN_F20DATA1(canx)                 REG32((canx) + 0x000002E4U)        /*!< CAN filter 20 data 1 register */
#define CAN_F21DATA1(canx)                 REG32((canx) + 0x000002ECU)        /*!< CAN filter 21 data 1 register */
#define CAN_F22DATA1(canx)                 REG32((canx) + 0x000002F4U)        /*!< CAN filter 22 data 1 register */
#define CAN_F23DATA1(canx)                 REG32((canx) + 0x000002FCU)        /*!< CAN filter 23 data 1 register */
#define CAN_F24DATA1(canx)                 REG32((canx) + 0x00000304U)        /*!< CAN filter 24 data 1 register */
#define CAN_F25DATA1(canx)                 REG32((canx) + 0x0000030CU)        /*!< CAN filter 25 data 1 register */
#define CAN_F26DATA1(canx)                 REG32((canx) + 0x00000314U)        /*!< CAN filter 26 data 1 register */
#define CAN_F27DATA1(canx)                 REG32((canx) + 0x0000031CU)        /*!< CAN filter 27 data 1 register */

/* CAN transmit mailbox bank */
#define CAN_TMI(canx, bank)                REG32((canx) + 0x180U + ((bank) * 0x10U))        /*!< CAN transmit mailbox identifier register */
#define CAN_TMP(canx, bank)                REG32((canx) + 0x184U + ((bank) * 0x10U))        /*!< CAN transmit mailbox property register */
#define CAN_TMDATA0(canx, bank)            REG32((canx) + 0x188U + ((bank) * 0x10U))        /*!< CAN transmit mailbox data0 register */
#define CAN_TMDATA1(canx, bank)            REG32((canx) + 0x18CU + ((bank) * 0x10U))        /*!< CAN transmit mailbox data1 register */

/* CAN filter bank */
#define CAN_FDATA0(canx, bank)             REG32((canx) + 0x240U + ((bank) * 0x8U) + 0x0U)  /*!< CAN filter data 0 register */
#define CAN_FDATA1(canx, bank)             REG32((canx) + 0x240U + ((bank) * 0x8U) + 0x4U)  /*!< CAN filter data 1 register */

/* CAN receive FIFO mailbox bank */
#define CAN_RFIFOMI(canx, bank)            REG32((canx) + 0x1B0U + ((bank) * 0x10U))        /*!< CAN receive FIFO mailbox identifier register */
#define CAN_RFIFOMP(canx, bank)            REG32((canx) + 0x1B4U + ((bank) * 0x10U))        /*!< CAN receive FIFO mailbox property register */
#define CAN_RFIFOMDATA0(canx, bank)        REG32((canx) + 0x1B8U + ((bank) * 0x10U))        /*!< CAN receive FIFO mailbox data0 register */
#define CAN_RFIFOMDATA1(canx, bank)        REG32((canx) + 0x1BCU + ((bank) * 0x10U))        /*!< CAN receive FIFO mailbox data1 register */

/* bits definitions */
/* CAN_CTL */
#define CAN_CTL_IWMOD                      BIT(0)                       /*!< initial working mode */
#define CAN_CTL_SLPWMOD                    BIT(1)                       /*!< sleep working mode */
#define CAN_CTL_TFO                        BIT(2)                       /*!< transmit FIFO order */
#define CAN_CTL_RFOD                       BIT(3)                       /*!< receive FIFO overwrite disable */
#define CAN_CTL_ARD                        BIT(4)                       /*!< automatic retransmission disable */
#define CAN_CTL_AWU                        BIT(5)                       /*!< automatic wakeup */
#define CAN_CTL_ABOR                       BIT(6)                       /*!< automatic bus-off recovery */
#define CAN_CTL_TTC                        BIT(7)                       /*!< time triggered communication */
#define CAN_CTL_SWRST                      BIT(15)                      /*!< CAN software reset */
#define CAN_CTL_DFZ                        BIT(16)                      /*!< CAN debug freeze */

/* CAN_STAT */
#define CAN_STAT_IWS                       BIT(0)                       /*!< initial working state */
#define CAN_STAT_SLPWS                     BIT(1)                       /*!< sleep working state */
#define CAN_STAT_ERRIF                     BIT(2)                       /*!< error interrupt flag*/
#define CAN_STAT_WUIF                      BIT(3)                       /*!< status change interrupt flag of wakeup from sleep working mode */
#define CAN_STAT_SLPIF                     BIT(4)                       /*!< status change interrupt flag of sleep working mode entering */
#define CAN_STAT_TS                        BIT(8)                       /*!< transmitting state */
#define CAN_STAT_RS                        BIT(9)                       /*!< receiving state */
#define CAN_STAT_LASTRX                    BIT(10)                      /*!< last sample value of rx pin */
#define CAN_STAT_RXL                       BIT(11)                      /*!< CAN rx signal */

/* CAN_TSTAT */
#define CAN_TSTAT_MTF0                     BIT(0)                       /*!< mailbox0 transmit finished */
#define CAN_TSTAT_MTFNERR0                 BIT(1)                       /*!< mailbox0 transmit finished and no error */
#define CAN_TSTAT_MAL0                     BIT(2)                       /*!< mailbox0 arbitration lost */
#define CAN_TSTAT_MTE0                     BIT(3)                       /*!< mailbox0 transmit error */
#define CAN_TSTAT_MST0                     BIT(7)                       /*!< mailbox0 stop transmitting */
#define CAN_TSTAT_MTF1                     BIT(8)                       /*!< mailbox1 transmit finished */
#define CAN_TSTAT_MTFNERR1                 BIT(9)                       /*!< mailbox1 transmit finished and no error */
#define CAN_TSTAT_MAL1                     BIT(10)                      /*!< mailbox1 arbitration lost */
#define CAN_TSTAT_MTE1                     BIT(11)                      /*!< mailbox1 transmit error */
#define CAN_TSTAT_MST1                     BIT(15)                      /*!< mailbox1 stop transmitting */
#define CAN_TSTAT_MTF2                     BIT(16)                      /*!< mailbox2 transmit finished */
#define CAN_TSTAT_MTFNERR2                 BIT(17)                      /*!< mailbox2 transmit finished and no error */
#define CAN_TSTAT_MAL2                     BIT(18)                      /*!< mailbox2 arbitration lost */
#define CAN_TSTAT_MTE2                     BIT(19)                      /*!< mailbox2 transmit error */
#define CAN_TSTAT_MST2                     BIT(23)                      /*!< mailbox2 stop transmitting */
#define CAN_TSTAT_NUM                      BITS(24,25)                  /*!< mailbox number */
#define CAN_TSTAT_TME0                     BIT(26)                      /*!< transmit mailbox0 empty */
#define CAN_TSTAT_TME1                     BIT(27)                      /*!< transmit mailbox1 empty */
#define CAN_TSTAT_TME2                     BIT(28)                      /*!< transmit mailbox2 empty */
#define CAN_TSTAT_TMLS0                    BIT(29)                      /*!< last sending priority flag for mailbox0 */
#define CAN_TSTAT_TMLS1                    BIT(30)                      /*!< last sending priority flag for mailbox1 */
#define CAN_TSTAT_TMLS2                    BIT(31)                      /*!< last sending priority flag for mailbox2 */

/* CAN_RFIFO0 */
#define CAN_RFIFO0_RFL0                    BITS(0,1)                    /*!< receive FIFO0 length */
#define CAN_RFIFO0_RFF0                    BIT(3)                       /*!< receive FIFO0 full */
#define CAN_RFIFO0_RFO0                    BIT(4)                       /*!< receive FIFO0 overfull */
#define CAN_RFIFO0_RFD0                    BIT(5)                       /*!< receive FIFO0 dequeue */

/* CAN_RFIFO1 */
#define CAN_RFIFO1_RFL1                    BITS(0,1)                    /*!< receive FIFO1 length */
#define CAN_RFIFO1_RFF1                    BIT(3)                       /*!< receive FIFO1 full */
#define CAN_RFIFO1_RFO1                    BIT(4)                       /*!< receive FIFO1 overfull */
#define CAN_RFIFO1_RFD1                    BIT(5)                       /*!< receive FIFO1 dequeue */

/* CAN_INTEN */
#define CAN_INTEN_TMEIE                    BIT(0)                       /*!< transmit mailbox empty interrupt enable */
#define CAN_INTEN_RFNEIE0                  BIT(1)                       /*!< receive FIFO0 not empty interrupt enable */
#define CAN_INTEN_RFFIE0                   BIT(2)                       /*!< receive FIFO0 full interrupt enable */
#define CAN_INTEN_RFOIE0                   BIT(3)                       /*!< receive FIFO0 overfull interrupt enable */
#define CAN_INTEN_RFNEIE1                  BIT(4)                       /*!< receive FIFO1 not empty interrupt enable */
#define CAN_INTEN_RFFIE1                   BIT(5)                       /*!< receive FIFO1 full interrupt enable */
#define CAN_INTEN_RFOIE1                   BIT(6)                       /*!< receive FIFO1 overfull interrupt enable */
#define CAN_INTEN_WERRIE                   BIT(8)                       /*!< warning error interrupt enable */
#define CAN_INTEN_PERRIE                   BIT(9)                       /*!< passive error interrupt enable */
#define CAN_INTEN_BOIE                     BIT(10)                      /*!< bus-off interrupt enable */
#define CAN_INTEN_ERRNIE                   BIT(11)                      /*!< error number interrupt enable */
#define CAN_INTEN_ERRIE                    BIT(15)                      /*!< error interrupt enable */
#define CAN_INTEN_WIE                      BIT(16)                      /*!< wakeup interrupt enable */
#define CAN_INTEN_SLPWIE                   BIT(17)                      /*!< sleep working interrupt enable */

/* CAN_ERR */
#define CAN_ERR_WERR                       BIT(0)                       /*!< warning error */
#define CAN_ERR_PERR                       BIT(1)                       /*!< passive error */
#define CAN_ERR_BOERR                      BIT(2)                       /*!< bus-off error */
#define CAN_ERR_ERRN                       BITS(4,6)                    /*!< error number */
#define CAN_ERR_TECNT                      BITS(16,23)                  /*!< transmit error count */
#define CAN_ERR_RECNT                      BITS(24,31)                  /*!< receive error count */

/* CAN_BT */
#define CAN_BT_BAUDPSC                     BITS(0,9)                    /*!< baudrate prescaler */
#define CAN_BT_BS1                         BITS(16,19)                  /*!< bit segment 1 */
#define CAN_BT_BS2                         BITS(20,22)                  /*!< bit segment 2 */
#define CAN_BT_SJW                         BITS(24,25)                  /*!< resynchronization jump width */
#define CAN_BT_LCMOD                       BIT(30)                      /*!< loopback communication mode */
#define CAN_BT_SCMOD                       BIT(31)                      /*!< silent communication mode */

/* CAN_TMIx */
#define CAN_TMI_TEN                        BIT(0)                       /*!< transmit enable */
#define CAN_TMI_FT                         BIT(1)                       /*!< frame type */
#define CAN_TMI_FF                         BIT(2)                       /*!< frame format */
#define CAN_TMI_EFID                       BITS(3,31)                   /*!< the frame identifier */
#define CAN_TMI_SFID                       BITS(21,31)                  /*!< the frame identifier */

/* CAN_TMPx */
#define CAN_TMP_DLENC                      BITS(0,3)                    /*!< data length code */
#define CAN_TMP_TSEN                       BIT(8)                       /*!< time stamp enable */
#define CAN_TMP_TS                         BITS(16,31)                  /*!< time stamp */

/* CAN_TMDATA0x */
#define CAN_TMDATA0_DB0                    BITS(0,7)                    /*!< transmit data byte 0 */
#define CAN_TMDATA0_DB1                    BITS(8,15)                   /*!< transmit data byte 1 */
#define CAN_TMDATA0_DB2                    BITS(16,23)                  /*!< transmit data byte 2 */
#define CAN_TMDATA0_DB3                    BITS(24,31)                  /*!< transmit data byte 3 */

/* CAN_TMDATA1x */
#define CAN_TMDATA1_DB4                    BITS(0,7)                    /*!< transmit data byte 4 */
#define CAN_TMDATA1_DB5                    BITS(8,15)                   /*!< transmit data byte 5 */
#define CAN_TMDATA1_DB6                    BITS(16,23)                  /*!< transmit data byte 6 */
#define CAN_TMDATA1_DB7                    BITS(24,31)                  /*!< transmit data byte 7 */

/* CAN_RFIFOMIx */
#define CAN_RFIFOMI_FT                     BIT(1)                       /*!< frame type */
#define CAN_RFIFOMI_FF                     BIT(2)                       /*!< frame format */
#define CAN_RFIFOMI_EFID                   BITS(3,31)                   /*!< the frame identifier */
#define CAN_RFIFOMI_SFID                   BITS(21,31)                  /*!< the frame identifier */

/* CAN_RFIFOMPx */
#define CAN_RFIFOMP_DLENC                  BITS(0,3)                    /*!< receive data length code */
#define CAN_RFIFOMP_FI                     BITS(8,15)                   /*!< filter index */
#define CAN_RFIFOMP_TS                     BITS(16,31)                  /*!< time stamp */

/* CAN_RFIFOMDATA0x */
#define CAN_RFIFOMDATA0_DB0                BITS(0,7)                    /*!< receive data byte 0 */
#define CAN_RFIFOMDATA0_DB1                BITS(8,15)                   /*!< receive data byte 1 */
#define CAN_RFIFOMDATA0_DB2                BITS(16,23)                  /*!< receive data byte 2 */
#define CAN_RFIFOMDATA0_DB3                BITS(24,31)                  /*!< receive data byte 3 */

/* CAN_RFIFOMDATA1x */
#define CAN_RFIFOMDATA1_DB4                BITS(0,7)                    /*!< receive data byte 4 */
#define CAN_RFIFOMDATA1_DB5                BITS(8,15)                   /*!< receive data byte 5 */
#define CAN_RFIFOMDATA1_DB6                BITS(16,23)                  /*!< receive data byte 6 */
#define CAN_RFIFOMDATA1_DB7                BITS(24,31)                  /*!< receive data byte 7 */

/* CAN_FCTL */
#define CAN_FCTL_FLD                       BIT(0)                       /*!< filter lock disable */
#define CAN_FCTL_HBC1F                     BITS(8,13)                   /*!< header bank of CAN1 filter */

/* CAN_FMCFG */
#define CAN_FMCFG_FMOD(regval)             BIT(regval)                  /*!< filter mode, list or mask */

/* CAN_FSCFG */
#define CAN_FSCFG_FS(regval)               BIT(regval)                  /*!< filter scale, 32 bits or 16 bits */

/* CAN_FAFIFO */
#define CAN_FAFIFOR_FAF(regval)            BIT(regval)                  /*!< filter associated with FIFO */

/* CAN_FW */
#define CAN_FW_FW(regval)                  BIT(regval)                  /*!< filter working */

/* CAN_FxDATAy */
#define CAN_FDATA_FD(regval)               BIT(regval)                  /*!< filter data */

/* constants definitions */
/* define the CAN bit position and its register index offset */
#define CAN_REGIDX_BIT(regidx, bitpos)              (((uint32_t)(regidx) << 6) | (uint32_t)(bitpos))
#define CAN_REG_VAL(canx, offset)                   (REG32((canx) + ((uint32_t)(offset) >> 6)))
#define CAN_BIT_POS(val)                            ((uint32_t)(val) & 0x1FU)

#define CAN_REGIDX_BITS(regidx, bitpos0, bitpos1)   (((uint32_t)(regidx) << 12) | ((uint32_t)(bitpos0) << 6) | (uint32_t)(bitpos1))
#define CAN_REG_VALS(canx, offset)                  (REG32((canx) + ((uint32_t)(offset) >> 12)))
#define CAN_BIT_POS0(val)                           (((uint32_t)(val) >> 6) & 0x1FU)
#define CAN_BIT_POS1(val)                           ((uint32_t)(val) & 0x1FU)

/* register offset */
#define STAT_REG_OFFSET                    ((uint8_t)0x04U)             /*!< STAT register offset */
#define TSTAT_REG_OFFSET                   ((uint8_t)0x08U)             /*!< TSTAT register offset */
#define RFIFO0_REG_OFFSET                  ((uint8_t)0x0CU)             /*!< RFIFO0 register offset */
#define RFIFO1_REG_OFFSET                  ((uint8_t)0x10U)             /*!< RFIFO1 register offset */
#define ERR_REG_OFFSET                     ((uint8_t)0x18U)             /*!< ERR register offset */

/* CAN flags */
typedef enum {
    /* flags in STAT register */
    CAN_FLAG_RXL      = CAN_REGIDX_BIT(STAT_REG_OFFSET, 11U),           /*!< RX level */
    CAN_FLAG_LASTRX   = CAN_REGIDX_BIT(STAT_REG_OFFSET, 10U),           /*!< last sample value of RX pin */
    CAN_FLAG_RS       = CAN_REGIDX_BIT(STAT_REG_OFFSET, 9U),            /*!< receiving state */
    CAN_FLAG_TS       = CAN_REGIDX_BIT(STAT_REG_OFFSET, 8U),            /*!< transmitting state */
    CAN_FLAG_SLPIF    = CAN_REGIDX_BIT(STAT_REG_OFFSET, 4U),            /*!< status change flag of entering sleep working mode */
    CAN_FLAG_WUIF     = CAN_REGIDX_BIT(STAT_REG_OFFSET, 3U),            /*!< status change flag of wakeup from sleep working mode */
    CAN_FLAG_ERRIF    = CAN_REGIDX_BIT(STAT_REG_OFFSET, 2U),            /*!< error flag */
    CAN_FLAG_SLPWS    = CAN_REGIDX_BIT(STAT_REG_OFFSET, 1U),            /*!< sleep working state */
    CAN_FLAG_IWS      = CAN_REGIDX_BIT(STAT_REG_OFFSET, 0U),            /*!< initial working state */
    /* flags in TSTAT register */
    CAN_FLAG_TMLS2    = CAN_REGIDX_BIT(TSTAT_REG_OFFSET, 31U),          /*!< transmit mailbox 2 last sending in TX FIFO */
    CAN_FLAG_TMLS1    = CAN_REGIDX_BIT(TSTAT_REG_OFFSET, 30U),          /*!< transmit mailbox 1 last sending in TX FIFO */
    CAN_FLAG_TMLS0    = CAN_REGIDX_BIT(TSTAT_REG_OFFSET, 29U),          /*!< transmit mailbox 0 last sending in TX FIFO */
    CAN_FLAG_TME2     = CAN_REGIDX_BIT(TSTAT_REG_OFFSET, 28U),          /*!< transmit mailbox 2 empty */
    CAN_FLAG_TME1     = CAN_REGIDX_BIT(TSTAT_REG_OFFSET, 27U),          /*!< transmit mailbox 1 empty */
    CAN_FLAG_TME0     = CAN_REGIDX_BIT(TSTAT_REG_OFFSET, 26U),          /*!< transmit mailbox 0 empty */
    CAN_FLAG_MTE2     = CAN_REGIDX_BIT(TSTAT_REG_OFFSET, 19U),          /*!< mailbox 2 transmit error */
    CAN_FLAG_MTE1     = CAN_REGIDX_BIT(TSTAT_REG_OFFSET, 11U),          /*!< mailbox 1 transmit error */
    CAN_FLAG_MTE0     = CAN_REGIDX_BIT(TSTAT_REG_OFFSET, 3U),           /*!< mailbox 0 transmit error */
    CAN_FLAG_MAL2     = CAN_REGIDX_BIT(TSTAT_REG_OFFSET, 18U),          /*!< mailbox 2 arbitration lost */
    CAN_FLAG_MAL1     = CAN_REGIDX_BIT(TSTAT_REG_OFFSET, 10U),          /*!< mailbox 1 arbitration lost */
    CAN_FLAG_MAL0     = CAN_REGIDX_BIT(TSTAT_REG_OFFSET, 2U),           /*!< mailbox 0 arbitration lost */
    CAN_FLAG_MTFNERR2 = CAN_REGIDX_BIT(TSTAT_REG_OFFSET, 17U),          /*!< mailbox 2 transmit finished with no error */
    CAN_FLAG_MTFNERR1 = CAN_REGIDX_BIT(TSTAT_REG_OFFSET, 9U),           /*!< mailbox 1 transmit finished with no error */
    CAN_FLAG_MTFNERR0 = CAN_REGIDX_BIT(TSTAT_REG_OFFSET, 1U),           /*!< mailbox 0 transmit finished with no error */
    CAN_FLAG_MTF2     = CAN_REGIDX_BIT(TSTAT_REG_OFFSET, 16U),          /*!< mailbox 2 transmit finished */
    CAN_FLAG_MTF1     = CAN_REGIDX_BIT(TSTAT_REG_OFFSET, 8U),           /*!< mailbox 1 transmit finished */
    CAN_FLAG_MTF0     = CAN_REGIDX_BIT(TSTAT_REG_OFFSET, 0U),           /*!< mailbox 0 transmit finished */
    /* flags in RFIFO0 register */
    CAN_FLAG_RFO0     = CAN_REGIDX_BIT(RFIFO0_REG_OFFSET, 4U),          /*!< receive FIFO0 overfull */
    CAN_FLAG_RFF0     = CAN_REGIDX_BIT(RFIFO0_REG_OFFSET, 3U),          /*!< receive FIFO0 full */
    /* flags in RFIFO1 register */
    CAN_FLAG_RFO1     = CAN_REGIDX_BIT(RFIFO1_REG_OFFSET, 4U),          /*!< receive FIFO1 overfull */
    CAN_FLAG_RFF1     = CAN_REGIDX_BIT(RFIFO1_REG_OFFSET, 3U),          /*!< receive FIFO1 full */
    /* flags in ERR register */
    CAN_FLAG_BOERR    = CAN_REGIDX_BIT(ERR_REG_OFFSET, 2U),             /*!< bus-off error */
    CAN_FLAG_PERR     = CAN_REGIDX_BIT(ERR_REG_OFFSET, 1U),             /*!< passive error */
    CAN_FLAG_WERR     = CAN_REGIDX_BIT(ERR_REG_OFFSET, 0U),             /*!< warning error */
} can_flag_enum;

/* CAN interrupt flags */
typedef enum {
    /* interrupt flags in STAT register */
    CAN_INT_FLAG_SLPIF = CAN_REGIDX_BITS(STAT_REG_OFFSET, 4U, 17U),     /*!< status change interrupt flag of sleep working mode entering */
    CAN_INT_FLAG_WUIF  = CAN_REGIDX_BITS(STAT_REG_OFFSET, 3U, 16),      /*!< status change interrupt flag of wakeup from sleep working mode */
    CAN_INT_FLAG_ERRIF = CAN_REGIDX_BITS(STAT_REG_OFFSET, 2U, 15),      /*!< error interrupt flag */
    /* interrupt flags in TSTAT register */
    CAN_INT_FLAG_MTF2  = CAN_REGIDX_BITS(TSTAT_REG_OFFSET, 16U, 0U),    /*!< mailbox 2 transmit finished interrupt flag */
    CAN_INT_FLAG_MTF1  = CAN_REGIDX_BITS(TSTAT_REG_OFFSET, 8U, 0U),     /*!< mailbox 1 transmit finished interrupt flag */
    CAN_INT_FLAG_MTF0  = CAN_REGIDX_BITS(TSTAT_REG_OFFSET, 0U, 0U),     /*!< mailbox 0 transmit finished interrupt flag */
    /* interrupt flags in RFIFO0 register */
    CAN_INT_FLAG_RFO0  = CAN_REGIDX_BITS(RFIFO0_REG_OFFSET, 4U, 3U),    /*!< receive FIFO0 overfull interrupt flag */
    CAN_INT_FLAG_RFF0  = CAN_REGIDX_BITS(RFIFO0_REG_OFFSET, 3U, 2U),    /*!< receive FIFO0 full interrupt flag */
    CAN_INT_FLAG_RFL0  = CAN_REGIDX_BITS(RFIFO0_REG_OFFSET, 2U, 1U),    /*!< receive FIFO0 not empty interrupt flag */
    /* interrupt flags in RFIFO0 register */
    CAN_INT_FLAG_RFO1  = CAN_REGIDX_BITS(RFIFO1_REG_OFFSET, 4U, 6U),    /*!< receive FIFO1 overfull interrupt flag */
    CAN_INT_FLAG_RFF1  = CAN_REGIDX_BITS(RFIFO1_REG_OFFSET, 3U, 5U),    /*!< receive FIFO1 full interrupt flag */
    CAN_INT_FLAG_RFL1  = CAN_REGIDX_BITS(RFIFO1_REG_OFFSET, 2U, 4U),    /*!< receive FIFO1 not empty interrupt flag */
    /* interrupt flags in ERR register */
    CAN_INT_FLAG_ERRN  = CAN_REGIDX_BITS(ERR_REG_OFFSET, 3U, 11U),      /*!< error number interrupt flag */
    CAN_INT_FLAG_BOERR = CAN_REGIDX_BITS(ERR_REG_OFFSET, 2U, 10U),      /*!< bus-off error interrupt flag */
    CAN_INT_FLAG_PERR  = CAN_REGIDX_BITS(ERR_REG_OFFSET, 1U, 9U),       /*!< passive error interrupt flag */
    CAN_INT_FLAG_WERR  = CAN_REGIDX_BITS(ERR_REG_OFFSET, 0U, 8U),       /*!< warning error interrupt flag */
} can_interrupt_flag_enum;

/* CAN initialize parameters structure */
typedef struct {
    uint8_t working_mode;                                               /*!< CAN working mode */
    uint8_t resync_jump_width;                                          /*!< CAN resynchronization jump width */
    uint8_t time_segment_1;                                             /*!< time segment 1 */
    uint8_t time_segment_2;                                             /*!< time segment 2 */
    ControlStatus time_triggered;                                       /*!< time triggered communication mode */
    ControlStatus auto_bus_off_recovery;                                /*!< automatic bus-off recovery */
    ControlStatus auto_wake_up;                                         /*!< automatic wake-up mode */
    ControlStatus auto_retrans;                                         /*!< automatic retransmission mode */
    ControlStatus rec_fifo_overwrite;                                   /*!< receive FIFO overwrite mode */
    ControlStatus trans_fifo_order;                                     /*!< transmit FIFO order */
    uint16_t prescaler;                                                 /*!< baudrate prescaler */
} can_parameter_struct;

/* CAN transmit message structure */
typedef struct {
    uint32_t tx_sfid;                                                   /*!< standard format frame identifier */
    uint32_t tx_efid;                                                   /*!< extended format frame identifier */
    uint8_t tx_ff;                                                      /*!< format of frame, standard or extended format */
    uint8_t tx_ft;                                                      /*!< type of frame, data or remote */
    uint8_t tx_dlen;                                                    /*!< data length */
    uint8_t tx_data[8];                                                 /*!< transmit data */
} can_transmit_message_struct;

/* CAN receive message structure */
typedef struct {
    uint32_t rx_sfid;                                                   /*!< standard format frame identifier */
    uint32_t rx_efid;                                                   /*!< extended format frame identifier */
    uint8_t rx_ff;                                                      /*!< format of frame, standard or extended format */
    uint8_t rx_ft;                                                      /*!< type of frame, data or remote */
    uint8_t rx_dlen;                                                    /*!< data length */
    uint8_t rx_data[8];                                                 /*!< receive data */
    uint8_t rx_fi;                                                      /*!< filtering index */
} can_receive_message_struct;

/* CAN filter parameters structure */
typedef struct {
    uint16_t filter_list_high;                                          /*!< filter list number high bits */
    uint16_t filter_list_low;                                           /*!< filter list number low bits */
    uint16_t filter_mask_high;                                          /*!< filter mask number high bits */
    uint16_t filter_mask_low;                                           /*!< filter mask number low bits */
    uint16_t filter_fifo_number;                                        /*!< receive FIFO associated with the filter */
    uint16_t filter_number;                                             /*!< filter number */
    uint16_t filter_mode;                                               /*!< filter mode, list or mask */
    uint16_t filter_bits;                                               /*!< filter scale */
    ControlStatus filter_enable;                                        /*!< filter work or not */
} can_filter_parameter_struct;

/* CAN errors */
typedef enum {
    CAN_ERROR_NONE = 0,                                                 /*!< no error */
    CAN_ERROR_FILL,                                                     /*!< fill error */
    CAN_ERROR_FORMAT,                                                   /*!< format error */
    CAN_ERROR_ACK,                                                      /*!< ACK error */
    CAN_ERROR_BITRECESSIVE,                                             /*!< bit recessive error */
    CAN_ERROR_BITDOMINANT,                                              /*!< bit dominant error */
    CAN_ERROR_CRC,                                                      /*!< CRC error */
    CAN_ERROR_SOFTWARECFG,                                              /*!< software configuration */
} can_error_enum;

/* transmit states */
typedef enum {
    CAN_TRANSMIT_FAILED = 0U,                                           /*!< CAN transmitted failure */
    CAN_TRANSMIT_OK = 1U,                                               /*!< CAN transmitted success */
    CAN_TRANSMIT_PENDING = 2U,                                          /*!< CAN transmitted pending */
    CAN_TRANSMIT_NOMAILBOX = 4U,                                        /*!< no empty mailbox to be used for CAN */
} can_transmit_state_enum;

typedef enum {
    CAN_INIT_STRUCT = 0U,                                                /* CAN initialize parameters struct */
    CAN_FILTER_STRUCT,                                                  /* CAN filter parameters struct */
    CAN_TX_MESSAGE_STRUCT,                                              /* CAN transmit message struct */
    CAN_RX_MESSAGE_STRUCT,                                              /* CAN receive message struct */
} can_struct_type_enum;

/* CAN baudrate prescaler */
#define BT_BAUDPSC(regval)                 (BITS(0,9) & ((uint32_t)(regval) << 0))

/* CAN bit segment 1 */
#define BT_BS1(regval)                     (BITS(16,19) & ((uint32_t)(regval) << 16))

/* CAN bit segment 2 */
#define BT_BS2(regval)                     (BITS(20,22) & ((uint32_t)(regval) << 20))

/* CAN resynchronization jump width */
#define BT_SJW(regval)                     (BITS(24,25) & ((uint32_t)(regval) << 24))

/* CAN communication mode */
#define BT_MODE(regval)                    (BITS(30,31) & ((uint32_t)(regval) << 30))

/* CAN FDATA high 16 bits */
#define FDATA_MASK_HIGH(regval)            (BITS(16,31) & ((uint32_t)(regval) << 16))

/* CAN FDATA low 16 bits */
#define FDATA_MASK_LOW(regval)             (BITS(0,15) & ((uint32_t)(regval) << 0))

/* CAN1 filter start bank_number */
#define FCTL_HBC1F(regval)                 (BITS(8,13) & ((uint32_t)(regval) << 8))

/* CAN transmit mailbox extended identifier */
#define TMI_EFID(regval)                   (BITS(3,31) & ((uint32_t)(regval) << 3))

/* CAN transmit mailbox standard identifier */
#define TMI_SFID(regval)                   (BITS(21,31) & ((uint32_t)(regval) << 21))

/* transmit data byte 0 */
#define TMDATA0_DB0(regval)                (BITS(0,7) & ((uint32_t)(regval) << 0))

/* transmit data byte 1 */
#define TMDATA0_DB1(regval)                (BITS(8,15) & ((uint32_t)(regval) << 8))

/* transmit data byte 2 */
#define TMDATA0_DB2(regval)                (BITS(16,23) & ((uint32_t)(regval) << 16))

/* transmit data byte 3 */
#define TMDATA0_DB3(regval)                (BITS(24,31) & ((uint32_t)(regval) << 24))

/* transmit data byte 4 */
#define TMDATA1_DB4(regval)                (BITS(0,7) & ((uint32_t)(regval) << 0))

/* transmit data byte 5 */
#define TMDATA1_DB5(regval)                (BITS(8,15) & ((uint32_t)(regval) << 8))

/* transmit data byte 6 */
#define TMDATA1_DB6(regval)                (BITS(16,23) & ((uint32_t)(regval) << 16))

/* transmit data byte 7 */
#define TMDATA1_DB7(regval)                (BITS(24,31) & ((uint32_t)(regval) << 24))

/* receive mailbox extended identifier */
#define GET_RFIFOMI_EFID(regval)           GET_BITS((uint32_t)(regval), 3U, 31U)

/* receive mailbox standard identifier */
#define GET_RFIFOMI_SFID(regval)           GET_BITS((uint32_t)(regval), 21U, 31U)

/* receive data length */
#define GET_RFIFOMP_DLENC(regval)          GET_BITS((uint32_t)(regval), 0U, 3U)

/* the index of the filter by which the frame is passed */
#define GET_RFIFOMP_FI(regval)             GET_BITS((uint32_t)(regval), 8U, 15U)

/* receive data byte 0 */
#define GET_RFIFOMDATA0_DB0(regval)        GET_BITS((uint32_t)(regval), 0U, 7U)

/* receive data byte 1 */
#define GET_RFIFOMDATA0_DB1(regval)        GET_BITS((uint32_t)(regval), 8U, 15U)

/* receive data byte 2 */
#define GET_RFIFOMDATA0_DB2(regval)        GET_BITS((uint32_t)(regval), 16U, 23U)

/* receive data byte 3 */
#define GET_RFIFOMDATA0_DB3(regval)        GET_BITS((uint32_t)(regval), 24U, 31U)

/* receive data byte 4 */
#define GET_RFIFOMDATA1_DB4(regval)        GET_BITS((uint32_t)(regval), 0U, 7U)

/* receive data byte 5 */
#define GET_RFIFOMDATA1_DB5(regval)        GET_BITS((uint32_t)(regval), 8U, 15U)

/* receive data byte 6 */
#define GET_RFIFOMDATA1_DB6(regval)        GET_BITS((uint32_t)(regval), 16U, 23U)

/* receive data byte 7 */
#define GET_RFIFOMDATA1_DB7(regval)        GET_BITS((uint32_t)(regval), 24U, 31U)

/* error number */
#define GET_ERR_ERRN(regval)               GET_BITS((uint32_t)(regval), 4U, 6U)

/* transmit error count */
#define GET_ERR_TECNT(regval)              GET_BITS((uint32_t)(regval), 16U, 23U)

/* receive  error count */
#define GET_ERR_RECNT(regval)              GET_BITS((uint32_t)(regval), 24U, 31U)

/* CAN errors */
#define ERR_ERRN(regval)                   (BITS(4,6) & ((uint32_t)(regval) << 4))
#define CAN_ERRN_0                         ERR_ERRN(0U)                 /*!< no error */
#define CAN_ERRN_1                         ERR_ERRN(1U)                 /*!< fill error */
#define CAN_ERRN_2                         ERR_ERRN(2U)                 /*!< format error */
#define CAN_ERRN_3                         ERR_ERRN(3U)                 /*!< ACK error */
#define CAN_ERRN_4                         ERR_ERRN(4U)                 /*!< bit recessive error */
#define CAN_ERRN_5                         ERR_ERRN(5U)                 /*!< bit dominant error */
#define CAN_ERRN_6                         ERR_ERRN(6U)                 /*!< CRC error */
#define CAN_ERRN_7                         ERR_ERRN(7U)                 /*!< software error */

#define CAN_STATE_PENDING                  ((uint32_t)0x00000000U)      /*!< CAN pending */

/* CAN communication mode */
#define CAN_NORMAL_MODE                    ((uint8_t)0x00U)             /*!< normal communication mode */
#define CAN_LOOPBACK_MODE                  ((uint8_t)0x01U)             /*!< loopback communication mode */
#define CAN_SILENT_MODE                    ((uint8_t)0x02U)             /*!< silent communication mode */
#define CAN_SILENT_LOOPBACK_MODE           ((uint8_t)0x03U)             /*!< loopback and silent communication mode */

/* CAN resynchronization jump width */
#define CAN_BT_SJW_1TQ                     ((uint8_t)0x00U)             /*!< 1 time quanta */
#define CAN_BT_SJW_2TQ                     ((uint8_t)0x01U)             /*!< 2 time quanta */
#define CAN_BT_SJW_3TQ                     ((uint8_t)0x02U)             /*!< 3 time quanta */
#define CAN_BT_SJW_4TQ                     ((uint8_t)0x03U)             /*!< 4 time quanta */

/* CAN time segment 1 */
#define CAN_BT_BS1_1TQ                     ((uint8_t)0x00U)             /*!< 1 time quanta */
#define CAN_BT_BS1_2TQ                     ((uint8_t)0x01U)             /*!< 2 time quanta */
#define CAN_BT_BS1_3TQ                     ((uint8_t)0x02U)             /*!< 3 time quanta */
#define CAN_BT_BS1_4TQ                     ((uint8_t)0x03U)             /*!< 4 time quanta */
#define CAN_BT_BS1_5TQ                     ((uint8_t)0x04U)             /*!< 5 time quanta */
#define CAN_BT_BS1_6TQ                     ((uint8_t)0x05U)             /*!< 6 time quanta */
#define CAN_BT_BS1_7TQ                     ((uint8_t)0x06U)             /*!< 7 time quanta */
#define CAN_BT_BS1_8TQ                     ((uint8_t)0x07U)             /*!< 8 time quanta */
#define CAN_BT_BS1_9TQ                     ((uint8_t)0x08U)             /*!< 9 time quanta */
#define CAN_BT_BS1_10TQ                    ((uint8_t)0x09U)             /*!< 10 time quanta */
#define CAN_BT_BS1_11TQ                    ((uint8_t)0x0AU)             /*!< 11 time quanta */
#define CAN_BT_BS1_12TQ                    ((uint8_t)0x0BU)             /*!< 12 time quanta */
#define CAN_BT_BS1_13TQ                    ((uint8_t)0x0CU)             /*!< 13 time quanta */
#define CAN_BT_BS1_14TQ                    ((uint8_t)0x0DU)             /*!< 14 time quanta */
#define CAN_BT_BS1_15TQ                    ((uint8_t)0x0EU)             /*!< 15 time quanta */
#define CAN_BT_BS1_16TQ                    ((uint8_t)0x0FU)             /*!< 16 time quanta */

/* CAN time segment 2 */
#define CAN_BT_BS2_1TQ                     ((uint8_t)0x00U)             /*!< 1 time quanta */
#define CAN_BT_BS2_2TQ                     ((uint8_t)0x01U)             /*!< 2 time quanta */
#define CAN_BT_BS2_3TQ                     ((uint8_t)0x02U)             /*!< 3 time quanta */
#define CAN_BT_BS2_4TQ                     ((uint8_t)0x03U)             /*!< 4 time quanta */
#define CAN_BT_BS2_5TQ                     ((uint8_t)0x04U)             /*!< 5 time quanta */
#define CAN_BT_BS2_6TQ                     ((uint8_t)0x05U)             /*!< 6 time quanta */
#define CAN_BT_BS2_7TQ                     ((uint8_t)0x06U)             /*!< 7 time quanta */
#define CAN_BT_BS2_8TQ                     ((uint8_t)0x07U)             /*!< 8 time quanta */

/* CAN mailbox number */
#define CAN_MAILBOX0                       ((uint8_t)0x00U)             /*!< mailbox0 */
#define CAN_MAILBOX1                       ((uint8_t)0x01U)             /*!< mailbox1 */
#define CAN_MAILBOX2                       ((uint8_t)0x02U)             /*!< mailbox2 */
#define CAN_NOMAILBOX                      ((uint8_t)0x03U)             /*!< no mailbox empty */

/* CAN frame format */
#define CAN_FF_STANDARD                    ((uint32_t)0x00000000U)      /*!< standard frame */
#define CAN_FF_EXTENDED                    ((uint32_t)0x00000004U)      /*!< extended frame */

/* CAN receive FIFO */
#define CAN_FIFO0                          ((uint8_t)0x00U)             /*!< receive FIFO0 */
#define CAN_FIFO1                          ((uint8_t)0x01U)             /*!< receive FIFO1 */

/* frame number of receive FIFO */
#define CAN_RFIF_RFL_MASK                  ((uint32_t)0x00000003U)      /*!< mask for frame number in receive FIFOx */

#define CAN_SFID_MASK                      ((uint32_t)0x000007FFU)      /*!< mask of standard identifier */
#define CAN_EFID_MASK                      ((uint32_t)0x1FFFFFFFU)      /*!< mask of extended identifier */

/* CAN working mode */
#define CAN_MODE_INITIALIZE                ((uint8_t)0x01U)             /*!< CAN initialize mode */
#define CAN_MODE_NORMAL                    ((uint8_t)0x02U)             /*!< CAN normal mode */
#define CAN_MODE_SLEEP                     ((uint8_t)0x04U)             /*!< CAN sleep mode */

/* filter bits */
#define CAN_FILTERBITS_16BIT               ((uint8_t)0x00U)             /*!< CAN filter 16 bits */
#define CAN_FILTERBITS_32BIT               ((uint8_t)0x01U)             /*!< CAN filter 32 bits */

/* filter mode */
#define CAN_FILTERMODE_MASK                ((uint8_t)0x00U)             /*!< mask mode */
#define CAN_FILTERMODE_LIST                ((uint8_t)0x01U)             /*!< list mode */

/* filter 16 bits mask */
#define CAN_FILTER_MASK_16BITS             ((uint32_t)0x0000FFFFU)      /*!< can filter 16 bits mask */

/* frame type */
#define CAN_FT_DATA                        ((uint32_t)0x00000000U)      /*!< data frame */
#define CAN_FT_REMOTE                      ((uint32_t)0x00000002U)      /*!< remote frame */

/* CAN timeout */
#define CAN_TIMEOUT                        ((uint32_t)0x0000FFFFU)      /*!< timeout value */

/* CAN mailbox empty status mask */
#define CAN_ALL_MAILBOX_EMPTY              ((uint32_t)0x1C000000U)      /*!< CAN mailbox empty status mask */

/* interrupt enable bits */
#define CAN_INT_TME                        CAN_INTEN_TMEIE              /*!< transmit mailbox empty interrupt enable */
#define CAN_INT_RFNE0                      CAN_INTEN_RFNEIE0            /*!< receive FIFO0 not empty interrupt enable */
#define CAN_INT_RFF0                       CAN_INTEN_RFFIE0             /*!< receive FIFO0 full interrupt enable */
#define CAN_INT_RFO0                       CAN_INTEN_RFOIE0             /*!< receive FIFO0 overfull interrupt enable */
#define CAN_INT_RFNE1                      CAN_INTEN_RFNEIE1            /*!< receive FIFO1 not empty interrupt enable */
#define CAN_INT_RFF1                       CAN_INTEN_RFFIE1             /*!< receive FIFO1 full interrupt enable */
#define CAN_INT_RFO1                       CAN_INTEN_RFOIE1             /*!< receive FIFO1 overfull interrupt enable */
#define CAN_INT_WERR                       CAN_INTEN_WERRIE             /*!< warning error interrupt enable */
#define CAN_INT_PERR                       CAN_INTEN_PERRIE             /*!< passive error interrupt enable */
#define CAN_INT_BO                         CAN_INTEN_BOIE               /*!< bus-off interrupt enable */
#define CAN_INT_ERRN                       CAN_INTEN_ERRNIE             /*!< error number interrupt enable */
#define CAN_INT_ERR                        CAN_INTEN_ERRIE              /*!< error interrupt enable */
#define CAN_INT_WAKEUP                     CAN_INTEN_WIE                /*!< wakeup interrupt enable */
#define CAN_INT_SLPW                       CAN_INTEN_SLPWIE             /*!< sleep working interrupt enable */

/* function declarations */
/* initialization functions */
/* deinitialize CAN */
void can_deinit(uint32_t can_periph);
/* initialize CAN structure */
void can_struct_para_init(can_struct_type_enum type, void *p_struct);
/* initialize CAN */
ErrStatus can_init(uint32_t can_periph, can_parameter_struct *can_parameter_init);
/* CAN filter initialization */
void can_filter_init(can_filter_parameter_struct *can_filter_parameter_init);

/* function configuration */
/* set can1 filter start bank number */
void can1_filter_start_bank(uint8_t start_bank);
/* enable functions */
/* CAN debug freeze enable */
void can_debug_freeze_enable(uint32_t can_periph);
/* CAN debug freeze disable */
void can_debug_freeze_disable(uint32_t can_periph);
/* CAN time trigger mode enable */
void can_time_trigger_mode_enable(uint32_t can_periph);
/* CAN time trigger mode disable */
void can_time_trigger_mode_disable(uint32_t can_periph);

/* transmit functions */
/* transmit CAN message */
uint8_t can_message_transmit(uint32_t can_periph, can_transmit_message_struct *transmit_message);
/* get CAN transmit state */
can_transmit_state_enum can_transmit_states(uint32_t can_periph, uint8_t mailbox_number);
/* stop CAN transmission */
ErrStatus can_transmission_stop(uint32_t can_periph, uint8_t mailbox_number);
/* CAN receive message */
void can_message_receive(uint32_t can_periph, uint8_t fifo_number, can_receive_message_struct *receive_message);
/* CAN release FIFO */
void can_fifo_release(uint32_t can_periph, uint8_t fifo_number);
/* CAN receive message length */
uint8_t can_receive_message_length_get(uint32_t can_periph, uint8_t fifo_number);
/* CAN working mode */
ErrStatus can_working_mode_set(uint32_t can_periph, uint8_t working_mode);
/* CAN wakeup from sleep mode */
ErrStatus can_wakeup(uint32_t can_periph);

/* CAN get error type */
can_error_enum can_error_get(uint32_t can_periph);
/* get CAN receive error number */
uint8_t can_receive_error_number_get(uint32_t can_periph);
/* get CAN transmit error number */
uint8_t can_transmit_error_number_get(uint32_t can_periph);

/* interrupt & flag functions */
/* CAN get flag state */
FlagStatus can_flag_get(uint32_t can_periph, can_flag_enum flag);
/* CAN clear flag state */
void can_flag_clear(uint32_t can_periph, can_flag_enum flag);
/* CAN interrupt enable */
void can_interrupt_enable(uint32_t can_periph, uint32_t interrupt);
/* CAN interrupt disable */
void can_interrupt_disable(uint32_t can_periph, uint32_t interrupt);
/* CAN get interrupt flag state */
FlagStatus can_interrupt_flag_get(uint32_t can_periph, can_interrupt_flag_enum flag);
/* CAN clear interrupt flag state */
void can_interrupt_flag_clear(uint32_t can_periph, can_interrupt_flag_enum flag);

#endif /* GD32F4XX_CAN_H */
