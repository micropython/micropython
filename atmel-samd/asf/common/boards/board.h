/**
 * \file
 *
 * \brief Standard board header file.
 *
 * This file includes the appropriate board header file according to the
 * defined board (parameter BOARD).
 *
 * Copyright (c) 2009-2016 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _BOARD_H_
#define _BOARD_H_

/**
 * \defgroup group_common_boards Generic board support
 *
 * The generic board support module includes board-specific definitions
 * and function prototypes, such as the board initialization function.
 *
 * \{
 */

#include "compiler.h"

#ifdef __cplusplus
extern "C" {
#endif


/*! \name Base Boards
 */
//! @{
#define EVK1100                     1  //!< AT32UC3A EVK1100 board.
#define EVK1101                     2  //!< AT32UC3B EVK1101 board.
#define UC3C_EK                     3  //!< AT32UC3C UC3C-EK board.
#define EVK1104                     4  //!< AT32UC3A3 EVK1104 board.
#define EVK1105                     5  //!< AT32UC3A EVK1105 board.
#define STK600_RCUC3L0              6  //!< STK600 RCUC3L0 board.
#define UC3L_EK                     7  //!< AT32UC3L-EK board.
#define XPLAIN                      8  //!< ATxmega128A1 Xplain board.
#define STK600_RC064X              10  //!< ATxmega256A3 STK600 board.
#define STK600_RC100X              11  //!< ATxmega128A1 STK600 board.
#define UC3_A3_XPLAINED            13  //!< ATUC3A3 UC3-A3 Xplained board.
#define UC3_L0_XPLAINED            15  //!< ATUC3L0 UC3-L0 Xplained board.
#define STK600_RCUC3D              16  //!< STK600 RCUC3D board.
#define STK600_RCUC3C0             17  //!< STK600 RCUC3C board.
#define XMEGA_B1_XPLAINED          18  //!< ATxmega128B1 Xplained board.
#define XMEGA_A1_XPLAINED          19  //!< ATxmega128A1 Xplain-A1 board.
#define XMEGA_A1U_XPLAINED_PRO     20  //!< ATxmega128A1U XMEGA-A1U Xplained Pro board.
#define STK600_RCUC3L4             21  //!< ATUCL4 STK600 board.
#define UC3_L0_XPLAINED_BC         22  //!< ATUC3L0 UC3-L0 Xplained board controller board.
#define MEGA1284P_XPLAINED_BC      23  //!< ATmega1284P-Xplained board controller board.
#define STK600_RC044X              24  //!< STK600 with RC044X routing card board.
#define STK600_RCUC3B0             25  //!< STK600 RCUC3B0 board.
#define UC3_L0_QT600               26  //!< QT600 UC3L0 MCU board.
#define XMEGA_A3BU_XPLAINED        27  //!< ATxmega256A3BU Xplained board.
#define STK600_RC064X_LCDX         28  //!< XMEGAB3 STK600 RC064X LCDX board.
#define STK600_RC100X_LCDX         29  //!< XMEGAB1 STK600 RC100X LCDX board.
#define UC3B_BOARD_CONTROLLER      30  //!< AT32UC3B1 board controller for Atmel boards.
#define RZ600                      31  //!< AT32UC3A RZ600 MCU board.
#define SAM3S_EK                   32  //!< SAM3S-EK board.
#define SAM3U_EK                   33  //!< SAM3U-EK board.
#define SAM3X_EK                   34  //!< SAM3X-EK board.
#define SAM3N_EK                   35  //!< SAM3N-EK board.
#define SAM3S_EK2                  36  //!< SAM3S-EK2 board.
#define SAM4S_EK                   37  //!< SAM4S-EK board.
#define STK600_RCUC3A0             38  //!< STK600 RCUC3A0 board.
#define STK600_MEGA                39  //!< STK600 MEGA board.
#define MEGA_1284P_XPLAINED        40  //!< ATmega1284P Xplained board.
#define SAM4S_XPLAINED             41  //!< SAM4S Xplained board.
#define ATXMEGA128A1_QT600         42  //!< QT600 ATXMEGA128A1 MCU board.
#define ARDUINO_DUE_X              43  //!< Arduino Due/X board.
#define STK600_RCUC3L3             44  //!< ATUCL3 STK600 board.
#define SAM4L_EK                   45  //!< SAM4L-EK board.
#define STK600_MEGA_RF             46  //!< STK600 MEGA RF EVK board.
#define XMEGA_C3_XPLAINED          47  //!< ATxmega384C3 Xplained board.
#define STK600_RC032X              48  //!< STK600 with RC032X routing card board.
#define SAM4S_EK2                  49  //!< SAM4S-EK2 board.
#define XMEGA_E5_XPLAINED          50  //!< ATxmega32E5 Xplained board.
#define SAM4E_EK                   51  //!< SAM4E-EK board.
#define ATMEGA256RFR2_XPLAINED_PRO 52  //!< ATmega256RFR2 Xplained Pro board.
#define SAM4S_XPLAINED_PRO         53  //!< SAM4S Xplained Pro board.
#define SAM4L_XPLAINED_PRO         54  //!< SAM4L Xplained Pro board.
#define ATMEGA256RFR2_ZIGBIT       55  //!< ATmega256RFR2 zigbit.
#define XMEGA_RF233_ZIGBIT         56  //!< ATxmega256A3U with AT86RF233 Zigbit.
#define XMEGA_RF212B_ZIGBIT        57  //!< ATxmega256A3U with AT86RF212B Zigbit.
#define SAM4S_WPIR_RD              58  //!< SAM4S-WPIR-RD board.
#define SAMD20_XPLAINED_PRO        59  //!< SAM D20 Xplained Pro board.
#define SAM4L8_XPLAINED_PRO        60  //!< SAM4L8 Xplained Pro board.
#define SAM4N_XPLAINED_PRO         61  //!< SAM4N Xplained Pro board.
#define XMEGA_A3_REB_CBB           62  //!< XMEGA REB Controller Base board.
#define ATMEGARFX_RCB              63  //!< RFR2 & RFA1 RCB.
#define SAM4C_EK                   64  //!< SAM4C-EK board.
#define RCB256RFR2_XPRO            65  //!< RFR2 RCB Xplained Pro board.
#define SAMG53_XPLAINED_PRO        66  //!< SAMG53 Xplained Pro board.
#define SAM4CP16BMB                67  //!< SAM4CP16BMB board.
#define SAM4E_XPLAINED_PRO         68  //!< SAM4E Xplained Pro board.
#define SAMD21_XPLAINED_PRO        69  //!< SAM D21 Xplained Pro board.
#define SAMR21_XPLAINED_PRO        70  //!< SAM R21 Xplained Pro board.
#define SAM4CMP_DB                 71  //!< SAM4CMP demo board.
#define SAM4CMS_DB                 72  //!< SAM4CMS demo board.
#define ATPL230AMB                 73  //!< ATPL230AMB board.
#define SAMD11_XPLAINED_PRO        74  //!< SAM D11 Xplained Pro board.
#define SAMG55_XPLAINED_PRO        75  //!< SAMG55 Xplained Pro board.
#define SAML21_XPLAINED_PRO        76  //!< SAM L21 Xplained Pro board.
#define SAMD10_XPLAINED_MINI       77  //!< SAM D10 Xplained Mini board.
#define SAMDA1_XPLAINED_PRO        78  //!< SAM DA1 Xplained Pro board.
#define SAMW25_XPLAINED_PRO        79  //!< SAMW25 Xplained Pro board.
#define SAMC21_XPLAINED_PRO        80  //!< SAM C21 Xplained Pro board.
#define SAMV71_XPLAINED_ULTRA      81  //!< SAMV71 Xplained Ultra board.
#define ATMEGA328P_XPLAINED_MINI   82  //!< ATMEGA328P Xplained MINI board.
#define ATMEGA328PB_XPLAINED_MINI  83  //!< ATMEGA328PB Xplained MINI board.
#define SAMB11_XPLAINED_PRO        84  //!< SAM B11 Xplained Pro board.
#define SAME70_XPLAINED            85  //!< SAME70 Xplained board.
#define SAML22_XPLAINED_PRO        86  //!< SAM L22 Xplained Pro board.
#define SAML22_XPLAINED_PRO_B      87  //!< SAM L22 Xplained Pro board.
#define SAMR21ZLL_EK               88  //!< SAMR21ZLL-EK board.
#define ATMEGA168PB_XPLAINED_MINI  89  //!< ATMEGA168PB Xplained MINI board.
#define ATMEGA324PB_XPLAINED_PRO   90  //!< ATMEGA324PB Xplained Pro board.
#define SAMB11CSP_XPLAINED_PRO     91  //!< SAM B11 CSP Xplained Pro board.
#define SAMB11ZR_XPLAINED_PRO      92  //!< SAM B11 ZR Xplained Pro board.
#define SAMR30_XPLAINED_PRO        93  //!< SAM R30 Xplained Pro board.
#define SIMULATOR_XMEGA_A1         97  //!< Simulator for XMEGA A1 devices.
#define AVR_SIMULATOR_UC3          98  //!< Simulator for the AVR UC3 device family.
#define USER_BOARD                 99  //!< User-reserved board (if any).
#define DUMMY_BOARD               100  //!< Dummy board to support board-independent applications (e.g. bootloader).
//! @}

/*! \name Extension Boards
 */
//! @{
#define EXT1102                      1  //!< AT32UC3B EXT1102 board
#define MC300                        2  //!< AT32UC3 MC300 board
#define SENSORS_XPLAINED_INERTIAL_1  3  //!< Xplained inertial sensor board 1
#define SENSORS_XPLAINED_INERTIAL_2  4  //!< Xplained inertial sensor board 2
#define SENSORS_XPLAINED_PRESSURE_1  5  //!< Xplained pressure sensor board
#define SENSORS_XPLAINED_LIGHTPROX_1 6  //!< Xplained light & proximity sensor board
#define SENSORS_XPLAINED_INERTIAL_A1 7  //!< Xplained inertial sensor board "A"
#define RZ600_AT86RF231              8  //!< AT86RF231 RF board in RZ600
#define RZ600_AT86RF230B             9  //!< AT86RF230B RF board in RZ600
#define RZ600_AT86RF212             10  //!< AT86RF212 RF board in RZ600
#define SENSORS_XPLAINED_BREADBOARD 11  //!< Xplained sensor development breadboard
#define SECURITY_XPLAINED           12  //!< Xplained ATSHA204 board
#define USER_EXT_BOARD              99  //!< User-reserved extension board (if any).
//! @}

#if BOARD == EVK1100
#  include "evk1100/evk1100.h"
#elif BOARD == EVK1101
#  include "evk1101/evk1101.h"
#elif BOARD == UC3C_EK
#  include "uc3c_ek/uc3c_ek.h"
#elif BOARD == EVK1104
#  include "evk1104/evk1104.h"
#elif BOARD == EVK1105
#  include "evk1105/evk1105.h"
#elif BOARD == STK600_RCUC3L0
#  include "stk600/rcuc3l0/stk600_rcuc3l0.h"
#elif BOARD == UC3L_EK
#  include "uc3l_ek/uc3l_ek.h"
#elif BOARD == STK600_RCUC3L4
#  include "stk600/rcuc3l4/stk600_rcuc3l4.h"
#elif BOARD == XPLAIN
#  include "xplain/xplain.h"
#elif BOARD == STK600_MEGA
  /*No header-file to include*/
#elif BOARD == STK600_MEGA_RF
#  include "stk600.h"
#elif BOARD == ATMEGA256RFR2_XPLAINED_PRO
#  include "atmega256rfr2_xplained_pro/atmega256rfr2_xplained_pro.h"
#elif BOARD == ATMEGA256RFR2_ZIGBIT
#  include "atmega256rfr2_zigbit/atmega256rfr2_zigbit.h"
#elif BOARD == STK600_RC032X
#  include "stk600/rc032x/stk600_rc032x.h"
#elif BOARD == STK600_RC044X
#  include "stk600/rc044x/stk600_rc044x.h"
#elif BOARD == STK600_RC064X
#  include "stk600/rc064x/stk600_rc064x.h"
#elif BOARD == STK600_RC100X
#  include "stk600/rc100x/stk600_rc100x.h"
#elif BOARD == UC3_A3_XPLAINED
#  include "uc3_a3_xplained/uc3_a3_xplained.h"
#elif BOARD == UC3_L0_XPLAINED
#  include "uc3_l0_xplained/uc3_l0_xplained.h"
#elif BOARD == STK600_RCUC3B0
#  include "stk600/rcuc3b0/stk600_rcuc3b0.h"
#elif BOARD == STK600_RCUC3D
#  include "stk600/rcuc3d/stk600_rcuc3d.h"
#elif BOARD == STK600_RCUC3C0
#  include "stk600/rcuc3c0/stk600_rcuc3c0.h"
#elif BOARD == SAMG53_XPLAINED_PRO
#  include "samg53_xplained_pro/samg53_xplained_pro.h"
#elif BOARD == SAMG55_XPLAINED_PRO
#  include "samg55_xplained_pro/samg55_xplained_pro.h"
#elif BOARD == XMEGA_B1_XPLAINED
#  include "xmega_b1_xplained/xmega_b1_xplained.h"
#elif BOARD == STK600_RC064X_LCDX
#  include "stk600/rc064x_lcdx/stk600_rc064x_lcdx.h"
#elif BOARD == STK600_RC100X_LCDX
#  include "stk600/rc100x_lcdx/stk600_rc100x_lcdx.h"
#elif BOARD == XMEGA_A1_XPLAINED
#  include "xmega_a1_xplained/xmega_a1_xplained.h"
#elif BOARD == XMEGA_A1U_XPLAINED_PRO
#  include "xmega_a1u_xplained_pro/xmega_a1u_xplained_pro.h"
#elif BOARD == UC3_L0_XPLAINED_BC
#  include "uc3_l0_xplained_bc/uc3_l0_xplained_bc.h"
#elif BOARD == SAM3S_EK
#  include "sam3s_ek/sam3s_ek.h"
#  include "system_sam3s.h"
#elif BOARD == SAM3S_EK2
#  include "sam3s_ek2/sam3s_ek2.h"
#  include "system_sam3sd8.h"
#elif BOARD == SAM3U_EK
#  include "sam3u_ek/sam3u_ek.h"
#  include "system_sam3u.h"
#elif BOARD == SAM3X_EK
#  include "sam3x_ek/sam3x_ek.h"
#  include "system_sam3x.h"
#elif BOARD == SAM3N_EK
#  include "sam3n_ek/sam3n_ek.h"
#  include "system_sam3n.h"
#elif BOARD == SAM4S_EK
#  include "sam4s_ek/sam4s_ek.h"
#  include "system_sam4s.h"
#elif BOARD == SAM4S_WPIR_RD
#  include "sam4s_wpir_rd/sam4s_wpir_rd.h"
#  include "system_sam4s.h"
#elif BOARD == SAM4S_XPLAINED
#  include "sam4s_xplained/sam4s_xplained.h"
#  include "system_sam4s.h"
#elif BOARD == SAM4S_EK2
#  include "sam4s_ek2/sam4s_ek2.h"
#  include "system_sam4s.h"
#elif BOARD == MEGA_1284P_XPLAINED
  /*No header-file to include*/
#elif BOARD == ARDUINO_DUE_X
#  include "arduino_due_x/arduino_due_x.h"
#  include "system_sam3x.h"
#elif BOARD == SAM4L_EK
#  include "sam4l_ek/sam4l_ek.h"
#elif BOARD == SAM4E_EK
#  include "sam4e_ek/sam4e_ek.h"
#elif BOARD == SAMD20_XPLAINED_PRO
#  include "samd20_xplained_pro/samd20_xplained_pro.h"
#elif BOARD == SAMD21_XPLAINED_PRO
#  include "samd21_xplained_pro/samd21_xplained_pro.h"
#elif BOARD == SAMR21_XPLAINED_PRO
#  include "samr21_xplained_pro/samr21_xplained_pro.h"
#elif BOARD == SAMR30_XPLAINED_PRO
#  include "samr30_xplained_pro/samr30_xplained_pro.h"
#elif BOARD == SAMR21ZLL_EK
#  include "samr21zll_ek/samr21zll_ek.h"
#elif BOARD == SAMD11_XPLAINED_PRO
#  include "samd11_xplained_pro/samd11_xplained_pro.h"
#elif BOARD == SAML21_XPLAINED_PRO && defined(__SAML21J18A__)
#  include "saml21_xplained_pro/saml21_xplained_pro.h"
#elif BOARD == SAML22_XPLAINED_PRO
#  include "saml22_xplained_pro/saml22_xplained_pro.h"
#elif BOARD == SAML22_XPLAINED_PRO_B
#  include "saml22_xplained_pro_b/saml22_xplained_pro_b.h"
#elif BOARD == SAML21_XPLAINED_PRO && defined(__SAML21J18B__)
#  include "saml21_xplained_pro_b/saml21_xplained_pro.h"
#elif BOARD == SAMD10_XPLAINED_MINI
#  include "samd10_xplained_mini/samd10_xplained_mini.h"
#elif BOARD == SAMDA1_XPLAINED_PRO
#  include "samda1_xplained_pro/samda1_xplained_pro.h"
#elif BOARD == SAMC21_XPLAINED_PRO
#  include "samc21_xplained_pro/samc21_xplained_pro.h"
#elif BOARD == SAM4N_XPLAINED_PRO
#  include "sam4n_xplained_pro/sam4n_xplained_pro.h"
#elif BOARD == SAMW25_XPLAINED_PRO
#  include "samw25_xplained_pro/samw25_xplained_pro.h"
#elif BOARD == SAMV71_XPLAINED_ULTRA
#  include "samv71_xplained_ultra/samv71_xplained_ultra.h"
#elif BOARD == MEGA1284P_XPLAINED_BC
#  include "mega1284p_xplained_bc/mega1284p_xplained_bc.h"
#elif BOARD == UC3_L0_QT600
#  include "uc3_l0_qt600/uc3_l0_qt600.h"
#elif BOARD == XMEGA_A3BU_XPLAINED
#  include "xmega_a3bu_xplained/xmega_a3bu_xplained.h"
#elif BOARD == XMEGA_E5_XPLAINED
#  include "xmega_e5_xplained/xmega_e5_xplained.h"
#elif BOARD == UC3B_BOARD_CONTROLLER
#  include "uc3b_board_controller/uc3b_board_controller.h"
#elif BOARD == RZ600
#  include "rz600/rz600.h"
#elif BOARD == STK600_RCUC3A0
#  include "stk600/rcuc3a0/stk600_rcuc3a0.h"
#elif BOARD == ATXMEGA128A1_QT600
#  include "atxmega128a1_qt600/atxmega128a1_qt600.h"
#elif BOARD == STK600_RCUC3L3
#  include "stk600/rcuc3l3/stk600_rcuc3l3.h"
#elif BOARD == SAM4S_XPLAINED_PRO
#  include "sam4s_xplained_pro/sam4s_xplained_pro.h"
#elif BOARD == SAM4L_XPLAINED_PRO
#  include "sam4l_xplained_pro/sam4l_xplained_pro.h"
#elif BOARD == SAM4L8_XPLAINED_PRO
#  include "sam4l8_xplained_pro/sam4l8_xplained_pro.h"
#elif BOARD == SAM4C_EK
#  include "sam4c_ek/sam4c_ek.h"
#elif BOARD == SAM4CMP_DB
#  include "sam4cmp_db/sam4cmp_db.h"
#elif BOARD == SAM4CMS_DB
#  include "sam4cms_db/sam4cms_db.h"
#elif BOARD == SAM4CP16BMB
#  include "sam4cp16bmb/sam4cp16bmb.h"
#elif BOARD == ATPL230AMB
#  include "atpl230amb/atpl230amb.h"
#elif BOARD == XMEGA_C3_XPLAINED
#  include "xmega_c3_xplained/xmega_c3_xplained.h"
#elif BOARD == XMEGA_RF233_ZIGBIT
#  include "xmega_rf233_zigbit/xmega_rf233_zigbit.h"
#elif BOARD == XMEGA_A3_REB_CBB
#  include "xmega_a3_reb_cbb/xmega_a3_reb_cbb.h"
#elif BOARD == ATMEGARFX_RCB
#  include "atmegarfx_rcb/atmegarfx_rcb.h"
#elif BOARD == RCB256RFR2_XPRO
#  include "atmega256rfr2_rcb_xpro/atmega256rfr2_rcb_xpro.h"
#elif BOARD == XMEGA_RF212B_ZIGBIT
#  include "xmega_rf212b_zigbit/xmega_rf212b_zigbit.h"
#elif BOARD == SAM4E_XPLAINED_PRO
#  include "sam4e_xplained_pro/sam4e_xplained_pro.h"
#elif BOARD == ATMEGA328P_XPLAINED_MINI
#  include "atmega328p_xplained_mini/atmega328p_xplained_mini.h"
#elif BOARD == ATMEGA328PB_XPLAINED_MINI
#  include "atmega328pb_xplained_mini/atmega328pb_xplained_mini.h"
#elif BOARD == SAMB11_XPLAINED_PRO
#  include "samb11_xplained_pro/samb11_xplained_pro.h"
#elif BOARD == SAME70_XPLAINED
#  include "same70_xplained/same70_xplained.h"
#elif BOARD == ATMEGA168PB_XPLAINED_MINI
#  include "atmega168pb_xplained_mini/atmega168pb_xplained_mini.h"
#elif BOARD == ATMEGA324PB_XPLAINED_PRO
#  include "atmega324pb_xplained_pro/atmega324pb_xplained_pro.h"
#elif BOARD == SAMB11CSP_XPLAINED_PRO
#  include "samb11csp_xplained_pro/samb11csp_xplained_pro.h"
#elif BOARD == SAMB11ZR_XPLAINED_PRO
#  include "samb11zr_xplained_pro/samb11zr_xplained_pro.h"
#elif BOARD == SIMULATOR_XMEGA_A1
#  include "simulator/xmega_a1/simulator_xmega_a1.h"
#elif BOARD == AVR_SIMULATOR_UC3
#  include "avr_simulator_uc3/avr_simulator_uc3.h"
#elif BOARD == USER_BOARD
  // User-reserved area: #include the header file of your board here (if any).
#  include "user_board.h"
#elif BOARD == DUMMY_BOARD
#  include "dummy/dummy_board.h"
#else
#  error No known Atmel board defined
#endif

#if (defined EXT_BOARD)
#  if EXT_BOARD == MC300
#    include "mc300/mc300.h"
#  elif (EXT_BOARD == SENSORS_XPLAINED_INERTIAL_1)  || \
        (EXT_BOARD == SENSORS_XPLAINED_INERTIAL_2)  || \
        (EXT_BOARD == SENSORS_XPLAINED_INERTIAL_A1) || \
        (EXT_BOARD == SENSORS_XPLAINED_PRESSURE_1)  || \
        (EXT_BOARD == SENSORS_XPLAINED_LIGHTPROX_1) || \
        (EXT_BOARD == SENSORS_XPLAINED_BREADBOARD)
#    include "sensors_xplained/sensors_xplained.h"
#  elif EXT_BOARD == RZ600_AT86RF231
#     include "at86rf231/at86rf231.h"
#  elif EXT_BOARD == RZ600_AT86RF230B
#    include "at86rf230b/at86rf230b.h"
#  elif EXT_BOARD == RZ600_AT86RF212
#    include "at86rf212/at86rf212.h"
#  elif EXT_BOARD == SECURITY_XPLAINED
#    include "security_xplained.h"
#  elif EXT_BOARD == USER_EXT_BOARD
    // User-reserved area: #include the header file of your extension board here
    // (if any).
#  endif
#endif


#if (defined(__GNUC__) && defined(__AVR32__)) || (defined(__ICCAVR32__) || defined(__AAVR32__))
#ifdef __AVR32_ABI_COMPILER__ // Automatically defined when compiling for AVR32, not when assembling.

/*! \brief This function initializes the board target resources
 *
 * This function should be called to ensure proper initialization of the target
 * board hardware connected to the part.
 */
extern void board_init(void);

#endif  // #ifdef __AVR32_ABI_COMPILER__
#else
/*! \brief This function initializes the board target resources
 *
 * This function should be called to ensure proper initialization of the target
 * board hardware connected to the part.
 */
extern void board_init(void);
#endif


#ifdef __cplusplus
}
#endif

/**
 * \}
 */

#endif  // _BOARD_H_
