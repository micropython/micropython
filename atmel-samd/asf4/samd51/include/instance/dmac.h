/**
 * \file
 *
 * \brief Instance description for DMAC
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

#ifndef _SAMD51_DMAC_INSTANCE_
#define _SAMD51_DMAC_INSTANCE_

/* ========== Register definition for DMAC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_DMAC_CTRL              (0x4100A000) /**< \brief (DMAC) Control */
#define REG_DMAC_CRCCTRL           (0x4100A002) /**< \brief (DMAC) CRC Control */
#define REG_DMAC_CRCDATAIN         (0x4100A004) /**< \brief (DMAC) CRC Data Input */
#define REG_DMAC_CRCCHKSUM         (0x4100A008) /**< \brief (DMAC) CRC Checksum */
#define REG_DMAC_CRCSTATUS         (0x4100A00C) /**< \brief (DMAC) CRC Status */
#define REG_DMAC_DBGCTRL           (0x4100A00D) /**< \brief (DMAC) Debug Control */
#define REG_DMAC_SWTRIGCTRL        (0x4100A010) /**< \brief (DMAC) Software Trigger Control */
#define REG_DMAC_PRICTRL0          (0x4100A014) /**< \brief (DMAC) Priority Control 0 */
#define REG_DMAC_INTPEND           (0x4100A020) /**< \brief (DMAC) Interrupt Pending */
#define REG_DMAC_INTSTATUS         (0x4100A024) /**< \brief (DMAC) Interrupt Status */
#define REG_DMAC_BUSYCH            (0x4100A028) /**< \brief (DMAC) Busy Channels */
#define REG_DMAC_PENDCH            (0x4100A02C) /**< \brief (DMAC) Pending Channels */
#define REG_DMAC_ACTIVE            (0x4100A030) /**< \brief (DMAC) Active Channel and Levels */
#define REG_DMAC_BASEADDR          (0x4100A034) /**< \brief (DMAC) Descriptor Memory Section Base Address */
#define REG_DMAC_WRBADDR           (0x4100A038) /**< \brief (DMAC) Write-Back Memory Section Base Address */
#define REG_DMAC_CHCTRLA0          (0x4100A040) /**< \brief (DMAC) Channel 0 Control A */
#define REG_DMAC_CHCTRLB0          (0x4100A044) /**< \brief (DMAC) Channel 0 Control B */
#define REG_DMAC_CHPRILVL0         (0x4100A045) /**< \brief (DMAC) Channel 0 Priority Level */
#define REG_DMAC_CHEVCTRL0         (0x4100A046) /**< \brief (DMAC) Channel 0 Event Control */
#define REG_DMAC_CHINTENCLR0       (0x4100A04C) /**< \brief (DMAC) Channel 0 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET0       (0x4100A04D) /**< \brief (DMAC) Channel 0 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG0        (0x4100A04E) /**< \brief (DMAC) Channel 0 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS0         (0x4100A04F) /**< \brief (DMAC) Channel 0 Status */
#define REG_DMAC_CHCTRLA1          (0x4100A050) /**< \brief (DMAC) Channel 1 Control A */
#define REG_DMAC_CHCTRLB1          (0x4100A054) /**< \brief (DMAC) Channel 1 Control B */
#define REG_DMAC_CHPRILVL1         (0x4100A055) /**< \brief (DMAC) Channel 1 Priority Level */
#define REG_DMAC_CHEVCTRL1         (0x4100A056) /**< \brief (DMAC) Channel 1 Event Control */
#define REG_DMAC_CHINTENCLR1       (0x4100A05C) /**< \brief (DMAC) Channel 1 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET1       (0x4100A05D) /**< \brief (DMAC) Channel 1 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG1        (0x4100A05E) /**< \brief (DMAC) Channel 1 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS1         (0x4100A05F) /**< \brief (DMAC) Channel 1 Status */
#define REG_DMAC_CHCTRLA2          (0x4100A060) /**< \brief (DMAC) Channel 2 Control A */
#define REG_DMAC_CHCTRLB2          (0x4100A064) /**< \brief (DMAC) Channel 2 Control B */
#define REG_DMAC_CHPRILVL2         (0x4100A065) /**< \brief (DMAC) Channel 2 Priority Level */
#define REG_DMAC_CHEVCTRL2         (0x4100A066) /**< \brief (DMAC) Channel 2 Event Control */
#define REG_DMAC_CHINTENCLR2       (0x4100A06C) /**< \brief (DMAC) Channel 2 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET2       (0x4100A06D) /**< \brief (DMAC) Channel 2 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG2        (0x4100A06E) /**< \brief (DMAC) Channel 2 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS2         (0x4100A06F) /**< \brief (DMAC) Channel 2 Status */
#define REG_DMAC_CHCTRLA3          (0x4100A070) /**< \brief (DMAC) Channel 3 Control A */
#define REG_DMAC_CHCTRLB3          (0x4100A074) /**< \brief (DMAC) Channel 3 Control B */
#define REG_DMAC_CHPRILVL3         (0x4100A075) /**< \brief (DMAC) Channel 3 Priority Level */
#define REG_DMAC_CHEVCTRL3         (0x4100A076) /**< \brief (DMAC) Channel 3 Event Control */
#define REG_DMAC_CHINTENCLR3       (0x4100A07C) /**< \brief (DMAC) Channel 3 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET3       (0x4100A07D) /**< \brief (DMAC) Channel 3 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG3        (0x4100A07E) /**< \brief (DMAC) Channel 3 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS3         (0x4100A07F) /**< \brief (DMAC) Channel 3 Status */
#define REG_DMAC_CHCTRLA4          (0x4100A080) /**< \brief (DMAC) Channel 4 Control A */
#define REG_DMAC_CHCTRLB4          (0x4100A084) /**< \brief (DMAC) Channel 4 Control B */
#define REG_DMAC_CHPRILVL4         (0x4100A085) /**< \brief (DMAC) Channel 4 Priority Level */
#define REG_DMAC_CHEVCTRL4         (0x4100A086) /**< \brief (DMAC) Channel 4 Event Control */
#define REG_DMAC_CHINTENCLR4       (0x4100A08C) /**< \brief (DMAC) Channel 4 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET4       (0x4100A08D) /**< \brief (DMAC) Channel 4 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG4        (0x4100A08E) /**< \brief (DMAC) Channel 4 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS4         (0x4100A08F) /**< \brief (DMAC) Channel 4 Status */
#define REG_DMAC_CHCTRLA5          (0x4100A090) /**< \brief (DMAC) Channel 5 Control A */
#define REG_DMAC_CHCTRLB5          (0x4100A094) /**< \brief (DMAC) Channel 5 Control B */
#define REG_DMAC_CHPRILVL5         (0x4100A095) /**< \brief (DMAC) Channel 5 Priority Level */
#define REG_DMAC_CHEVCTRL5         (0x4100A096) /**< \brief (DMAC) Channel 5 Event Control */
#define REG_DMAC_CHINTENCLR5       (0x4100A09C) /**< \brief (DMAC) Channel 5 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET5       (0x4100A09D) /**< \brief (DMAC) Channel 5 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG5        (0x4100A09E) /**< \brief (DMAC) Channel 5 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS5         (0x4100A09F) /**< \brief (DMAC) Channel 5 Status */
#define REG_DMAC_CHCTRLA6          (0x4100A0A0) /**< \brief (DMAC) Channel 6 Control A */
#define REG_DMAC_CHCTRLB6          (0x4100A0A4) /**< \brief (DMAC) Channel 6 Control B */
#define REG_DMAC_CHPRILVL6         (0x4100A0A5) /**< \brief (DMAC) Channel 6 Priority Level */
#define REG_DMAC_CHEVCTRL6         (0x4100A0A6) /**< \brief (DMAC) Channel 6 Event Control */
#define REG_DMAC_CHINTENCLR6       (0x4100A0AC) /**< \brief (DMAC) Channel 6 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET6       (0x4100A0AD) /**< \brief (DMAC) Channel 6 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG6        (0x4100A0AE) /**< \brief (DMAC) Channel 6 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS6         (0x4100A0AF) /**< \brief (DMAC) Channel 6 Status */
#define REG_DMAC_CHCTRLA7          (0x4100A0B0) /**< \brief (DMAC) Channel 7 Control A */
#define REG_DMAC_CHCTRLB7          (0x4100A0B4) /**< \brief (DMAC) Channel 7 Control B */
#define REG_DMAC_CHPRILVL7         (0x4100A0B5) /**< \brief (DMAC) Channel 7 Priority Level */
#define REG_DMAC_CHEVCTRL7         (0x4100A0B6) /**< \brief (DMAC) Channel 7 Event Control */
#define REG_DMAC_CHINTENCLR7       (0x4100A0BC) /**< \brief (DMAC) Channel 7 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET7       (0x4100A0BD) /**< \brief (DMAC) Channel 7 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG7        (0x4100A0BE) /**< \brief (DMAC) Channel 7 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS7         (0x4100A0BF) /**< \brief (DMAC) Channel 7 Status */
#define REG_DMAC_CHCTRLA8          (0x4100A0C0) /**< \brief (DMAC) Channel 8 Control A */
#define REG_DMAC_CHCTRLB8          (0x4100A0C4) /**< \brief (DMAC) Channel 8 Control B */
#define REG_DMAC_CHPRILVL8         (0x4100A0C5) /**< \brief (DMAC) Channel 8 Priority Level */
#define REG_DMAC_CHEVCTRL8         (0x4100A0C6) /**< \brief (DMAC) Channel 8 Event Control */
#define REG_DMAC_CHINTENCLR8       (0x4100A0CC) /**< \brief (DMAC) Channel 8 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET8       (0x4100A0CD) /**< \brief (DMAC) Channel 8 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG8        (0x4100A0CE) /**< \brief (DMAC) Channel 8 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS8         (0x4100A0CF) /**< \brief (DMAC) Channel 8 Status */
#define REG_DMAC_CHCTRLA9          (0x4100A0D0) /**< \brief (DMAC) Channel 9 Control A */
#define REG_DMAC_CHCTRLB9          (0x4100A0D4) /**< \brief (DMAC) Channel 9 Control B */
#define REG_DMAC_CHPRILVL9         (0x4100A0D5) /**< \brief (DMAC) Channel 9 Priority Level */
#define REG_DMAC_CHEVCTRL9         (0x4100A0D6) /**< \brief (DMAC) Channel 9 Event Control */
#define REG_DMAC_CHINTENCLR9       (0x4100A0DC) /**< \brief (DMAC) Channel 9 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET9       (0x4100A0DD) /**< \brief (DMAC) Channel 9 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG9        (0x4100A0DE) /**< \brief (DMAC) Channel 9 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS9         (0x4100A0DF) /**< \brief (DMAC) Channel 9 Status */
#define REG_DMAC_CHCTRLA10         (0x4100A0E0) /**< \brief (DMAC) Channel 10 Control A */
#define REG_DMAC_CHCTRLB10         (0x4100A0E4) /**< \brief (DMAC) Channel 10 Control B */
#define REG_DMAC_CHPRILVL10        (0x4100A0E5) /**< \brief (DMAC) Channel 10 Priority Level */
#define REG_DMAC_CHEVCTRL10        (0x4100A0E6) /**< \brief (DMAC) Channel 10 Event Control */
#define REG_DMAC_CHINTENCLR10      (0x4100A0EC) /**< \brief (DMAC) Channel 10 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET10      (0x4100A0ED) /**< \brief (DMAC) Channel 10 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG10       (0x4100A0EE) /**< \brief (DMAC) Channel 10 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS10        (0x4100A0EF) /**< \brief (DMAC) Channel 10 Status */
#define REG_DMAC_CHCTRLA11         (0x4100A0F0) /**< \brief (DMAC) Channel 11 Control A */
#define REG_DMAC_CHCTRLB11         (0x4100A0F4) /**< \brief (DMAC) Channel 11 Control B */
#define REG_DMAC_CHPRILVL11        (0x4100A0F5) /**< \brief (DMAC) Channel 11 Priority Level */
#define REG_DMAC_CHEVCTRL11        (0x4100A0F6) /**< \brief (DMAC) Channel 11 Event Control */
#define REG_DMAC_CHINTENCLR11      (0x4100A0FC) /**< \brief (DMAC) Channel 11 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET11      (0x4100A0FD) /**< \brief (DMAC) Channel 11 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG11       (0x4100A0FE) /**< \brief (DMAC) Channel 11 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS11        (0x4100A0FF) /**< \brief (DMAC) Channel 11 Status */
#define REG_DMAC_CHCTRLA12         (0x4100A100) /**< \brief (DMAC) Channel 12 Control A */
#define REG_DMAC_CHCTRLB12         (0x4100A104) /**< \brief (DMAC) Channel 12 Control B */
#define REG_DMAC_CHPRILVL12        (0x4100A105) /**< \brief (DMAC) Channel 12 Priority Level */
#define REG_DMAC_CHEVCTRL12        (0x4100A106) /**< \brief (DMAC) Channel 12 Event Control */
#define REG_DMAC_CHINTENCLR12      (0x4100A10C) /**< \brief (DMAC) Channel 12 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET12      (0x4100A10D) /**< \brief (DMAC) Channel 12 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG12       (0x4100A10E) /**< \brief (DMAC) Channel 12 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS12        (0x4100A10F) /**< \brief (DMAC) Channel 12 Status */
#define REG_DMAC_CHCTRLA13         (0x4100A110) /**< \brief (DMAC) Channel 13 Control A */
#define REG_DMAC_CHCTRLB13         (0x4100A114) /**< \brief (DMAC) Channel 13 Control B */
#define REG_DMAC_CHPRILVL13        (0x4100A115) /**< \brief (DMAC) Channel 13 Priority Level */
#define REG_DMAC_CHEVCTRL13        (0x4100A116) /**< \brief (DMAC) Channel 13 Event Control */
#define REG_DMAC_CHINTENCLR13      (0x4100A11C) /**< \brief (DMAC) Channel 13 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET13      (0x4100A11D) /**< \brief (DMAC) Channel 13 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG13       (0x4100A11E) /**< \brief (DMAC) Channel 13 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS13        (0x4100A11F) /**< \brief (DMAC) Channel 13 Status */
#define REG_DMAC_CHCTRLA14         (0x4100A120) /**< \brief (DMAC) Channel 14 Control A */
#define REG_DMAC_CHCTRLB14         (0x4100A124) /**< \brief (DMAC) Channel 14 Control B */
#define REG_DMAC_CHPRILVL14        (0x4100A125) /**< \brief (DMAC) Channel 14 Priority Level */
#define REG_DMAC_CHEVCTRL14        (0x4100A126) /**< \brief (DMAC) Channel 14 Event Control */
#define REG_DMAC_CHINTENCLR14      (0x4100A12C) /**< \brief (DMAC) Channel 14 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET14      (0x4100A12D) /**< \brief (DMAC) Channel 14 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG14       (0x4100A12E) /**< \brief (DMAC) Channel 14 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS14        (0x4100A12F) /**< \brief (DMAC) Channel 14 Status */
#define REG_DMAC_CHCTRLA15         (0x4100A130) /**< \brief (DMAC) Channel 15 Control A */
#define REG_DMAC_CHCTRLB15         (0x4100A134) /**< \brief (DMAC) Channel 15 Control B */
#define REG_DMAC_CHPRILVL15        (0x4100A135) /**< \brief (DMAC) Channel 15 Priority Level */
#define REG_DMAC_CHEVCTRL15        (0x4100A136) /**< \brief (DMAC) Channel 15 Event Control */
#define REG_DMAC_CHINTENCLR15      (0x4100A13C) /**< \brief (DMAC) Channel 15 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET15      (0x4100A13D) /**< \brief (DMAC) Channel 15 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG15       (0x4100A13E) /**< \brief (DMAC) Channel 15 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS15        (0x4100A13F) /**< \brief (DMAC) Channel 15 Status */
#define REG_DMAC_CHCTRLA16         (0x4100A140) /**< \brief (DMAC) Channel 16 Control A */
#define REG_DMAC_CHCTRLB16         (0x4100A144) /**< \brief (DMAC) Channel 16 Control B */
#define REG_DMAC_CHPRILVL16        (0x4100A145) /**< \brief (DMAC) Channel 16 Priority Level */
#define REG_DMAC_CHEVCTRL16        (0x4100A146) /**< \brief (DMAC) Channel 16 Event Control */
#define REG_DMAC_CHINTENCLR16      (0x4100A14C) /**< \brief (DMAC) Channel 16 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET16      (0x4100A14D) /**< \brief (DMAC) Channel 16 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG16       (0x4100A14E) /**< \brief (DMAC) Channel 16 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS16        (0x4100A14F) /**< \brief (DMAC) Channel 16 Status */
#define REG_DMAC_CHCTRLA17         (0x4100A150) /**< \brief (DMAC) Channel 17 Control A */
#define REG_DMAC_CHCTRLB17         (0x4100A154) /**< \brief (DMAC) Channel 17 Control B */
#define REG_DMAC_CHPRILVL17        (0x4100A155) /**< \brief (DMAC) Channel 17 Priority Level */
#define REG_DMAC_CHEVCTRL17        (0x4100A156) /**< \brief (DMAC) Channel 17 Event Control */
#define REG_DMAC_CHINTENCLR17      (0x4100A15C) /**< \brief (DMAC) Channel 17 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET17      (0x4100A15D) /**< \brief (DMAC) Channel 17 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG17       (0x4100A15E) /**< \brief (DMAC) Channel 17 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS17        (0x4100A15F) /**< \brief (DMAC) Channel 17 Status */
#define REG_DMAC_CHCTRLA18         (0x4100A160) /**< \brief (DMAC) Channel 18 Control A */
#define REG_DMAC_CHCTRLB18         (0x4100A164) /**< \brief (DMAC) Channel 18 Control B */
#define REG_DMAC_CHPRILVL18        (0x4100A165) /**< \brief (DMAC) Channel 18 Priority Level */
#define REG_DMAC_CHEVCTRL18        (0x4100A166) /**< \brief (DMAC) Channel 18 Event Control */
#define REG_DMAC_CHINTENCLR18      (0x4100A16C) /**< \brief (DMAC) Channel 18 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET18      (0x4100A16D) /**< \brief (DMAC) Channel 18 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG18       (0x4100A16E) /**< \brief (DMAC) Channel 18 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS18        (0x4100A16F) /**< \brief (DMAC) Channel 18 Status */
#define REG_DMAC_CHCTRLA19         (0x4100A170) /**< \brief (DMAC) Channel 19 Control A */
#define REG_DMAC_CHCTRLB19         (0x4100A174) /**< \brief (DMAC) Channel 19 Control B */
#define REG_DMAC_CHPRILVL19        (0x4100A175) /**< \brief (DMAC) Channel 19 Priority Level */
#define REG_DMAC_CHEVCTRL19        (0x4100A176) /**< \brief (DMAC) Channel 19 Event Control */
#define REG_DMAC_CHINTENCLR19      (0x4100A17C) /**< \brief (DMAC) Channel 19 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET19      (0x4100A17D) /**< \brief (DMAC) Channel 19 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG19       (0x4100A17E) /**< \brief (DMAC) Channel 19 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS19        (0x4100A17F) /**< \brief (DMAC) Channel 19 Status */
#define REG_DMAC_CHCTRLA20         (0x4100A180) /**< \brief (DMAC) Channel 20 Control A */
#define REG_DMAC_CHCTRLB20         (0x4100A184) /**< \brief (DMAC) Channel 20 Control B */
#define REG_DMAC_CHPRILVL20        (0x4100A185) /**< \brief (DMAC) Channel 20 Priority Level */
#define REG_DMAC_CHEVCTRL20        (0x4100A186) /**< \brief (DMAC) Channel 20 Event Control */
#define REG_DMAC_CHINTENCLR20      (0x4100A18C) /**< \brief (DMAC) Channel 20 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET20      (0x4100A18D) /**< \brief (DMAC) Channel 20 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG20       (0x4100A18E) /**< \brief (DMAC) Channel 20 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS20        (0x4100A18F) /**< \brief (DMAC) Channel 20 Status */
#define REG_DMAC_CHCTRLA21         (0x4100A190) /**< \brief (DMAC) Channel 21 Control A */
#define REG_DMAC_CHCTRLB21         (0x4100A194) /**< \brief (DMAC) Channel 21 Control B */
#define REG_DMAC_CHPRILVL21        (0x4100A195) /**< \brief (DMAC) Channel 21 Priority Level */
#define REG_DMAC_CHEVCTRL21        (0x4100A196) /**< \brief (DMAC) Channel 21 Event Control */
#define REG_DMAC_CHINTENCLR21      (0x4100A19C) /**< \brief (DMAC) Channel 21 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET21      (0x4100A19D) /**< \brief (DMAC) Channel 21 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG21       (0x4100A19E) /**< \brief (DMAC) Channel 21 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS21        (0x4100A19F) /**< \brief (DMAC) Channel 21 Status */
#define REG_DMAC_CHCTRLA22         (0x4100A1A0) /**< \brief (DMAC) Channel 22 Control A */
#define REG_DMAC_CHCTRLB22         (0x4100A1A4) /**< \brief (DMAC) Channel 22 Control B */
#define REG_DMAC_CHPRILVL22        (0x4100A1A5) /**< \brief (DMAC) Channel 22 Priority Level */
#define REG_DMAC_CHEVCTRL22        (0x4100A1A6) /**< \brief (DMAC) Channel 22 Event Control */
#define REG_DMAC_CHINTENCLR22      (0x4100A1AC) /**< \brief (DMAC) Channel 22 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET22      (0x4100A1AD) /**< \brief (DMAC) Channel 22 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG22       (0x4100A1AE) /**< \brief (DMAC) Channel 22 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS22        (0x4100A1AF) /**< \brief (DMAC) Channel 22 Status */
#define REG_DMAC_CHCTRLA23         (0x4100A1B0) /**< \brief (DMAC) Channel 23 Control A */
#define REG_DMAC_CHCTRLB23         (0x4100A1B4) /**< \brief (DMAC) Channel 23 Control B */
#define REG_DMAC_CHPRILVL23        (0x4100A1B5) /**< \brief (DMAC) Channel 23 Priority Level */
#define REG_DMAC_CHEVCTRL23        (0x4100A1B6) /**< \brief (DMAC) Channel 23 Event Control */
#define REG_DMAC_CHINTENCLR23      (0x4100A1BC) /**< \brief (DMAC) Channel 23 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET23      (0x4100A1BD) /**< \brief (DMAC) Channel 23 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG23       (0x4100A1BE) /**< \brief (DMAC) Channel 23 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS23        (0x4100A1BF) /**< \brief (DMAC) Channel 23 Status */
#define REG_DMAC_CHCTRLA24         (0x4100A1C0) /**< \brief (DMAC) Channel 24 Control A */
#define REG_DMAC_CHCTRLB24         (0x4100A1C4) /**< \brief (DMAC) Channel 24 Control B */
#define REG_DMAC_CHPRILVL24        (0x4100A1C5) /**< \brief (DMAC) Channel 24 Priority Level */
#define REG_DMAC_CHEVCTRL24        (0x4100A1C6) /**< \brief (DMAC) Channel 24 Event Control */
#define REG_DMAC_CHINTENCLR24      (0x4100A1CC) /**< \brief (DMAC) Channel 24 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET24      (0x4100A1CD) /**< \brief (DMAC) Channel 24 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG24       (0x4100A1CE) /**< \brief (DMAC) Channel 24 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS24        (0x4100A1CF) /**< \brief (DMAC) Channel 24 Status */
#define REG_DMAC_CHCTRLA25         (0x4100A1D0) /**< \brief (DMAC) Channel 25 Control A */
#define REG_DMAC_CHCTRLB25         (0x4100A1D4) /**< \brief (DMAC) Channel 25 Control B */
#define REG_DMAC_CHPRILVL25        (0x4100A1D5) /**< \brief (DMAC) Channel 25 Priority Level */
#define REG_DMAC_CHEVCTRL25        (0x4100A1D6) /**< \brief (DMAC) Channel 25 Event Control */
#define REG_DMAC_CHINTENCLR25      (0x4100A1DC) /**< \brief (DMAC) Channel 25 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET25      (0x4100A1DD) /**< \brief (DMAC) Channel 25 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG25       (0x4100A1DE) /**< \brief (DMAC) Channel 25 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS25        (0x4100A1DF) /**< \brief (DMAC) Channel 25 Status */
#define REG_DMAC_CHCTRLA26         (0x4100A1E0) /**< \brief (DMAC) Channel 26 Control A */
#define REG_DMAC_CHCTRLB26         (0x4100A1E4) /**< \brief (DMAC) Channel 26 Control B */
#define REG_DMAC_CHPRILVL26        (0x4100A1E5) /**< \brief (DMAC) Channel 26 Priority Level */
#define REG_DMAC_CHEVCTRL26        (0x4100A1E6) /**< \brief (DMAC) Channel 26 Event Control */
#define REG_DMAC_CHINTENCLR26      (0x4100A1EC) /**< \brief (DMAC) Channel 26 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET26      (0x4100A1ED) /**< \brief (DMAC) Channel 26 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG26       (0x4100A1EE) /**< \brief (DMAC) Channel 26 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS26        (0x4100A1EF) /**< \brief (DMAC) Channel 26 Status */
#define REG_DMAC_CHCTRLA27         (0x4100A1F0) /**< \brief (DMAC) Channel 27 Control A */
#define REG_DMAC_CHCTRLB27         (0x4100A1F4) /**< \brief (DMAC) Channel 27 Control B */
#define REG_DMAC_CHPRILVL27        (0x4100A1F5) /**< \brief (DMAC) Channel 27 Priority Level */
#define REG_DMAC_CHEVCTRL27        (0x4100A1F6) /**< \brief (DMAC) Channel 27 Event Control */
#define REG_DMAC_CHINTENCLR27      (0x4100A1FC) /**< \brief (DMAC) Channel 27 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET27      (0x4100A1FD) /**< \brief (DMAC) Channel 27 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG27       (0x4100A1FE) /**< \brief (DMAC) Channel 27 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS27        (0x4100A1FF) /**< \brief (DMAC) Channel 27 Status */
#define REG_DMAC_CHCTRLA28         (0x4100A200) /**< \brief (DMAC) Channel 28 Control A */
#define REG_DMAC_CHCTRLB28         (0x4100A204) /**< \brief (DMAC) Channel 28 Control B */
#define REG_DMAC_CHPRILVL28        (0x4100A205) /**< \brief (DMAC) Channel 28 Priority Level */
#define REG_DMAC_CHEVCTRL28        (0x4100A206) /**< \brief (DMAC) Channel 28 Event Control */
#define REG_DMAC_CHINTENCLR28      (0x4100A20C) /**< \brief (DMAC) Channel 28 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET28      (0x4100A20D) /**< \brief (DMAC) Channel 28 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG28       (0x4100A20E) /**< \brief (DMAC) Channel 28 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS28        (0x4100A20F) /**< \brief (DMAC) Channel 28 Status */
#define REG_DMAC_CHCTRLA29         (0x4100A210) /**< \brief (DMAC) Channel 29 Control A */
#define REG_DMAC_CHCTRLB29         (0x4100A214) /**< \brief (DMAC) Channel 29 Control B */
#define REG_DMAC_CHPRILVL29        (0x4100A215) /**< \brief (DMAC) Channel 29 Priority Level */
#define REG_DMAC_CHEVCTRL29        (0x4100A216) /**< \brief (DMAC) Channel 29 Event Control */
#define REG_DMAC_CHINTENCLR29      (0x4100A21C) /**< \brief (DMAC) Channel 29 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET29      (0x4100A21D) /**< \brief (DMAC) Channel 29 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG29       (0x4100A21E) /**< \brief (DMAC) Channel 29 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS29        (0x4100A21F) /**< \brief (DMAC) Channel 29 Status */
#define REG_DMAC_CHCTRLA30         (0x4100A220) /**< \brief (DMAC) Channel 30 Control A */
#define REG_DMAC_CHCTRLB30         (0x4100A224) /**< \brief (DMAC) Channel 30 Control B */
#define REG_DMAC_CHPRILVL30        (0x4100A225) /**< \brief (DMAC) Channel 30 Priority Level */
#define REG_DMAC_CHEVCTRL30        (0x4100A226) /**< \brief (DMAC) Channel 30 Event Control */
#define REG_DMAC_CHINTENCLR30      (0x4100A22C) /**< \brief (DMAC) Channel 30 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET30      (0x4100A22D) /**< \brief (DMAC) Channel 30 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG30       (0x4100A22E) /**< \brief (DMAC) Channel 30 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS30        (0x4100A22F) /**< \brief (DMAC) Channel 30 Status */
#define REG_DMAC_CHCTRLA31         (0x4100A230) /**< \brief (DMAC) Channel 31 Control A */
#define REG_DMAC_CHCTRLB31         (0x4100A234) /**< \brief (DMAC) Channel 31 Control B */
#define REG_DMAC_CHPRILVL31        (0x4100A235) /**< \brief (DMAC) Channel 31 Priority Level */
#define REG_DMAC_CHEVCTRL31        (0x4100A236) /**< \brief (DMAC) Channel 31 Event Control */
#define REG_DMAC_CHINTENCLR31      (0x4100A23C) /**< \brief (DMAC) Channel 31 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET31      (0x4100A23D) /**< \brief (DMAC) Channel 31 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG31       (0x4100A23E) /**< \brief (DMAC) Channel 31 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS31        (0x4100A23F) /**< \brief (DMAC) Channel 31 Status */
#else
#define REG_DMAC_CTRL              (*(RwReg16*)0x4100A000UL) /**< \brief (DMAC) Control */
#define REG_DMAC_CRCCTRL           (*(RwReg16*)0x4100A002UL) /**< \brief (DMAC) CRC Control */
#define REG_DMAC_CRCDATAIN         (*(RwReg  *)0x4100A004UL) /**< \brief (DMAC) CRC Data Input */
#define REG_DMAC_CRCCHKSUM         (*(RwReg  *)0x4100A008UL) /**< \brief (DMAC) CRC Checksum */
#define REG_DMAC_CRCSTATUS         (*(RwReg8 *)0x4100A00CUL) /**< \brief (DMAC) CRC Status */
#define REG_DMAC_DBGCTRL           (*(RwReg8 *)0x4100A00DUL) /**< \brief (DMAC) Debug Control */
#define REG_DMAC_SWTRIGCTRL        (*(RwReg  *)0x4100A010UL) /**< \brief (DMAC) Software Trigger Control */
#define REG_DMAC_PRICTRL0          (*(RwReg  *)0x4100A014UL) /**< \brief (DMAC) Priority Control 0 */
#define REG_DMAC_INTPEND           (*(RwReg16*)0x4100A020UL) /**< \brief (DMAC) Interrupt Pending */
#define REG_DMAC_INTSTATUS         (*(RoReg  *)0x4100A024UL) /**< \brief (DMAC) Interrupt Status */
#define REG_DMAC_BUSYCH            (*(RoReg  *)0x4100A028UL) /**< \brief (DMAC) Busy Channels */
#define REG_DMAC_PENDCH            (*(RoReg  *)0x4100A02CUL) /**< \brief (DMAC) Pending Channels */
#define REG_DMAC_ACTIVE            (*(RoReg  *)0x4100A030UL) /**< \brief (DMAC) Active Channel and Levels */
#define REG_DMAC_BASEADDR          (*(RwReg  *)0x4100A034UL) /**< \brief (DMAC) Descriptor Memory Section Base Address */
#define REG_DMAC_WRBADDR           (*(RwReg  *)0x4100A038UL) /**< \brief (DMAC) Write-Back Memory Section Base Address */
#define REG_DMAC_CHCTRLA0          (*(RwReg  *)0x4100A040UL) /**< \brief (DMAC) Channel 0 Control A */
#define REG_DMAC_CHCTRLB0          (*(RwReg  *)0x4100A044UL) /**< \brief (DMAC) Channel 0 Control B */
#define REG_DMAC_CHPRILVL0         (*(RwReg  *)0x4100A045UL) /**< \brief (DMAC) Channel 0 Priority Level */
#define REG_DMAC_CHEVCTRL0         (*(RwReg  *)0x4100A046UL) /**< \brief (DMAC) Channel 0 Event Control */
#define REG_DMAC_CHINTENCLR0       (*(RwReg  *)0x4100A04CUL) /**< \brief (DMAC) Channel 0 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET0       (*(RwReg  *)0x4100A04DUL) /**< \brief (DMAC) Channel 0 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG0        (*(RwReg  *)0x4100A04EUL) /**< \brief (DMAC) Channel 0 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS0         (*(RwReg  *)0x4100A04FUL) /**< \brief (DMAC) Channel 0 Status */
#define REG_DMAC_CHCTRLA1          (*(RwReg  *)0x4100A050UL) /**< \brief (DMAC) Channel 1 Control A */
#define REG_DMAC_CHCTRLB1          (*(RwReg  *)0x4100A054UL) /**< \brief (DMAC) Channel 1 Control B */
#define REG_DMAC_CHPRILVL1         (*(RwReg  *)0x4100A055UL) /**< \brief (DMAC) Channel 1 Priority Level */
#define REG_DMAC_CHEVCTRL1         (*(RwReg  *)0x4100A056UL) /**< \brief (DMAC) Channel 1 Event Control */
#define REG_DMAC_CHINTENCLR1       (*(RwReg  *)0x4100A05CUL) /**< \brief (DMAC) Channel 1 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET1       (*(RwReg  *)0x4100A05DUL) /**< \brief (DMAC) Channel 1 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG1        (*(RwReg  *)0x4100A05EUL) /**< \brief (DMAC) Channel 1 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS1         (*(RwReg  *)0x4100A05FUL) /**< \brief (DMAC) Channel 1 Status */
#define REG_DMAC_CHCTRLA2          (*(RwReg  *)0x4100A060UL) /**< \brief (DMAC) Channel 2 Control A */
#define REG_DMAC_CHCTRLB2          (*(RwReg  *)0x4100A064UL) /**< \brief (DMAC) Channel 2 Control B */
#define REG_DMAC_CHPRILVL2         (*(RwReg  *)0x4100A065UL) /**< \brief (DMAC) Channel 2 Priority Level */
#define REG_DMAC_CHEVCTRL2         (*(RwReg  *)0x4100A066UL) /**< \brief (DMAC) Channel 2 Event Control */
#define REG_DMAC_CHINTENCLR2       (*(RwReg  *)0x4100A06CUL) /**< \brief (DMAC) Channel 2 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET2       (*(RwReg  *)0x4100A06DUL) /**< \brief (DMAC) Channel 2 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG2        (*(RwReg  *)0x4100A06EUL) /**< \brief (DMAC) Channel 2 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS2         (*(RwReg  *)0x4100A06FUL) /**< \brief (DMAC) Channel 2 Status */
#define REG_DMAC_CHCTRLA3          (*(RwReg  *)0x4100A070UL) /**< \brief (DMAC) Channel 3 Control A */
#define REG_DMAC_CHCTRLB3          (*(RwReg  *)0x4100A074UL) /**< \brief (DMAC) Channel 3 Control B */
#define REG_DMAC_CHPRILVL3         (*(RwReg  *)0x4100A075UL) /**< \brief (DMAC) Channel 3 Priority Level */
#define REG_DMAC_CHEVCTRL3         (*(RwReg  *)0x4100A076UL) /**< \brief (DMAC) Channel 3 Event Control */
#define REG_DMAC_CHINTENCLR3       (*(RwReg  *)0x4100A07CUL) /**< \brief (DMAC) Channel 3 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET3       (*(RwReg  *)0x4100A07DUL) /**< \brief (DMAC) Channel 3 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG3        (*(RwReg  *)0x4100A07EUL) /**< \brief (DMAC) Channel 3 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS3         (*(RwReg  *)0x4100A07FUL) /**< \brief (DMAC) Channel 3 Status */
#define REG_DMAC_CHCTRLA4          (*(RwReg  *)0x4100A080UL) /**< \brief (DMAC) Channel 4 Control A */
#define REG_DMAC_CHCTRLB4          (*(RwReg  *)0x4100A084UL) /**< \brief (DMAC) Channel 4 Control B */
#define REG_DMAC_CHPRILVL4         (*(RwReg  *)0x4100A085UL) /**< \brief (DMAC) Channel 4 Priority Level */
#define REG_DMAC_CHEVCTRL4         (*(RwReg  *)0x4100A086UL) /**< \brief (DMAC) Channel 4 Event Control */
#define REG_DMAC_CHINTENCLR4       (*(RwReg  *)0x4100A08CUL) /**< \brief (DMAC) Channel 4 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET4       (*(RwReg  *)0x4100A08DUL) /**< \brief (DMAC) Channel 4 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG4        (*(RwReg  *)0x4100A08EUL) /**< \brief (DMAC) Channel 4 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS4         (*(RwReg  *)0x4100A08FUL) /**< \brief (DMAC) Channel 4 Status */
#define REG_DMAC_CHCTRLA5          (*(RwReg  *)0x4100A090UL) /**< \brief (DMAC) Channel 5 Control A */
#define REG_DMAC_CHCTRLB5          (*(RwReg  *)0x4100A094UL) /**< \brief (DMAC) Channel 5 Control B */
#define REG_DMAC_CHPRILVL5         (*(RwReg  *)0x4100A095UL) /**< \brief (DMAC) Channel 5 Priority Level */
#define REG_DMAC_CHEVCTRL5         (*(RwReg  *)0x4100A096UL) /**< \brief (DMAC) Channel 5 Event Control */
#define REG_DMAC_CHINTENCLR5       (*(RwReg  *)0x4100A09CUL) /**< \brief (DMAC) Channel 5 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET5       (*(RwReg  *)0x4100A09DUL) /**< \brief (DMAC) Channel 5 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG5        (*(RwReg  *)0x4100A09EUL) /**< \brief (DMAC) Channel 5 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS5         (*(RwReg  *)0x4100A09FUL) /**< \brief (DMAC) Channel 5 Status */
#define REG_DMAC_CHCTRLA6          (*(RwReg  *)0x4100A0A0UL) /**< \brief (DMAC) Channel 6 Control A */
#define REG_DMAC_CHCTRLB6          (*(RwReg  *)0x4100A0A4UL) /**< \brief (DMAC) Channel 6 Control B */
#define REG_DMAC_CHPRILVL6         (*(RwReg  *)0x4100A0A5UL) /**< \brief (DMAC) Channel 6 Priority Level */
#define REG_DMAC_CHEVCTRL6         (*(RwReg  *)0x4100A0A6UL) /**< \brief (DMAC) Channel 6 Event Control */
#define REG_DMAC_CHINTENCLR6       (*(RwReg  *)0x4100A0ACUL) /**< \brief (DMAC) Channel 6 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET6       (*(RwReg  *)0x4100A0ADUL) /**< \brief (DMAC) Channel 6 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG6        (*(RwReg  *)0x4100A0AEUL) /**< \brief (DMAC) Channel 6 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS6         (*(RwReg  *)0x4100A0AFUL) /**< \brief (DMAC) Channel 6 Status */
#define REG_DMAC_CHCTRLA7          (*(RwReg  *)0x4100A0B0UL) /**< \brief (DMAC) Channel 7 Control A */
#define REG_DMAC_CHCTRLB7          (*(RwReg  *)0x4100A0B4UL) /**< \brief (DMAC) Channel 7 Control B */
#define REG_DMAC_CHPRILVL7         (*(RwReg  *)0x4100A0B5UL) /**< \brief (DMAC) Channel 7 Priority Level */
#define REG_DMAC_CHEVCTRL7         (*(RwReg  *)0x4100A0B6UL) /**< \brief (DMAC) Channel 7 Event Control */
#define REG_DMAC_CHINTENCLR7       (*(RwReg  *)0x4100A0BCUL) /**< \brief (DMAC) Channel 7 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET7       (*(RwReg  *)0x4100A0BDUL) /**< \brief (DMAC) Channel 7 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG7        (*(RwReg  *)0x4100A0BEUL) /**< \brief (DMAC) Channel 7 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS7         (*(RwReg  *)0x4100A0BFUL) /**< \brief (DMAC) Channel 7 Status */
#define REG_DMAC_CHCTRLA8          (*(RwReg  *)0x4100A0C0UL) /**< \brief (DMAC) Channel 8 Control A */
#define REG_DMAC_CHCTRLB8          (*(RwReg  *)0x4100A0C4UL) /**< \brief (DMAC) Channel 8 Control B */
#define REG_DMAC_CHPRILVL8         (*(RwReg  *)0x4100A0C5UL) /**< \brief (DMAC) Channel 8 Priority Level */
#define REG_DMAC_CHEVCTRL8         (*(RwReg  *)0x4100A0C6UL) /**< \brief (DMAC) Channel 8 Event Control */
#define REG_DMAC_CHINTENCLR8       (*(RwReg  *)0x4100A0CCUL) /**< \brief (DMAC) Channel 8 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET8       (*(RwReg  *)0x4100A0CDUL) /**< \brief (DMAC) Channel 8 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG8        (*(RwReg  *)0x4100A0CEUL) /**< \brief (DMAC) Channel 8 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS8         (*(RwReg  *)0x4100A0CFUL) /**< \brief (DMAC) Channel 8 Status */
#define REG_DMAC_CHCTRLA9          (*(RwReg  *)0x4100A0D0UL) /**< \brief (DMAC) Channel 9 Control A */
#define REG_DMAC_CHCTRLB9          (*(RwReg  *)0x4100A0D4UL) /**< \brief (DMAC) Channel 9 Control B */
#define REG_DMAC_CHPRILVL9         (*(RwReg  *)0x4100A0D5UL) /**< \brief (DMAC) Channel 9 Priority Level */
#define REG_DMAC_CHEVCTRL9         (*(RwReg  *)0x4100A0D6UL) /**< \brief (DMAC) Channel 9 Event Control */
#define REG_DMAC_CHINTENCLR9       (*(RwReg  *)0x4100A0DCUL) /**< \brief (DMAC) Channel 9 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET9       (*(RwReg  *)0x4100A0DDUL) /**< \brief (DMAC) Channel 9 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG9        (*(RwReg  *)0x4100A0DEUL) /**< \brief (DMAC) Channel 9 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS9         (*(RwReg  *)0x4100A0DFUL) /**< \brief (DMAC) Channel 9 Status */
#define REG_DMAC_CHCTRLA10         (*(RwReg  *)0x4100A0E0UL) /**< \brief (DMAC) Channel 10 Control A */
#define REG_DMAC_CHCTRLB10         (*(RwReg  *)0x4100A0E4UL) /**< \brief (DMAC) Channel 10 Control B */
#define REG_DMAC_CHPRILVL10        (*(RwReg  *)0x4100A0E5UL) /**< \brief (DMAC) Channel 10 Priority Level */
#define REG_DMAC_CHEVCTRL10        (*(RwReg  *)0x4100A0E6UL) /**< \brief (DMAC) Channel 10 Event Control */
#define REG_DMAC_CHINTENCLR10      (*(RwReg  *)0x4100A0ECUL) /**< \brief (DMAC) Channel 10 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET10      (*(RwReg  *)0x4100A0EDUL) /**< \brief (DMAC) Channel 10 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG10       (*(RwReg  *)0x4100A0EEUL) /**< \brief (DMAC) Channel 10 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS10        (*(RwReg  *)0x4100A0EFUL) /**< \brief (DMAC) Channel 10 Status */
#define REG_DMAC_CHCTRLA11         (*(RwReg  *)0x4100A0F0UL) /**< \brief (DMAC) Channel 11 Control A */
#define REG_DMAC_CHCTRLB11         (*(RwReg  *)0x4100A0F4UL) /**< \brief (DMAC) Channel 11 Control B */
#define REG_DMAC_CHPRILVL11        (*(RwReg  *)0x4100A0F5UL) /**< \brief (DMAC) Channel 11 Priority Level */
#define REG_DMAC_CHEVCTRL11        (*(RwReg  *)0x4100A0F6UL) /**< \brief (DMAC) Channel 11 Event Control */
#define REG_DMAC_CHINTENCLR11      (*(RwReg  *)0x4100A0FCUL) /**< \brief (DMAC) Channel 11 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET11      (*(RwReg  *)0x4100A0FDUL) /**< \brief (DMAC) Channel 11 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG11       (*(RwReg  *)0x4100A0FEUL) /**< \brief (DMAC) Channel 11 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS11        (*(RwReg  *)0x4100A0FFUL) /**< \brief (DMAC) Channel 11 Status */
#define REG_DMAC_CHCTRLA12         (*(RwReg  *)0x4100A100UL) /**< \brief (DMAC) Channel 12 Control A */
#define REG_DMAC_CHCTRLB12         (*(RwReg  *)0x4100A104UL) /**< \brief (DMAC) Channel 12 Control B */
#define REG_DMAC_CHPRILVL12        (*(RwReg  *)0x4100A105UL) /**< \brief (DMAC) Channel 12 Priority Level */
#define REG_DMAC_CHEVCTRL12        (*(RwReg  *)0x4100A106UL) /**< \brief (DMAC) Channel 12 Event Control */
#define REG_DMAC_CHINTENCLR12      (*(RwReg  *)0x4100A10CUL) /**< \brief (DMAC) Channel 12 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET12      (*(RwReg  *)0x4100A10DUL) /**< \brief (DMAC) Channel 12 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG12       (*(RwReg  *)0x4100A10EUL) /**< \brief (DMAC) Channel 12 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS12        (*(RwReg  *)0x4100A10FUL) /**< \brief (DMAC) Channel 12 Status */
#define REG_DMAC_CHCTRLA13         (*(RwReg  *)0x4100A110UL) /**< \brief (DMAC) Channel 13 Control A */
#define REG_DMAC_CHCTRLB13         (*(RwReg  *)0x4100A114UL) /**< \brief (DMAC) Channel 13 Control B */
#define REG_DMAC_CHPRILVL13        (*(RwReg  *)0x4100A115UL) /**< \brief (DMAC) Channel 13 Priority Level */
#define REG_DMAC_CHEVCTRL13        (*(RwReg  *)0x4100A116UL) /**< \brief (DMAC) Channel 13 Event Control */
#define REG_DMAC_CHINTENCLR13      (*(RwReg  *)0x4100A11CUL) /**< \brief (DMAC) Channel 13 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET13      (*(RwReg  *)0x4100A11DUL) /**< \brief (DMAC) Channel 13 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG13       (*(RwReg  *)0x4100A11EUL) /**< \brief (DMAC) Channel 13 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS13        (*(RwReg  *)0x4100A11FUL) /**< \brief (DMAC) Channel 13 Status */
#define REG_DMAC_CHCTRLA14         (*(RwReg  *)0x4100A120UL) /**< \brief (DMAC) Channel 14 Control A */
#define REG_DMAC_CHCTRLB14         (*(RwReg  *)0x4100A124UL) /**< \brief (DMAC) Channel 14 Control B */
#define REG_DMAC_CHPRILVL14        (*(RwReg  *)0x4100A125UL) /**< \brief (DMAC) Channel 14 Priority Level */
#define REG_DMAC_CHEVCTRL14        (*(RwReg  *)0x4100A126UL) /**< \brief (DMAC) Channel 14 Event Control */
#define REG_DMAC_CHINTENCLR14      (*(RwReg  *)0x4100A12CUL) /**< \brief (DMAC) Channel 14 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET14      (*(RwReg  *)0x4100A12DUL) /**< \brief (DMAC) Channel 14 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG14       (*(RwReg  *)0x4100A12EUL) /**< \brief (DMAC) Channel 14 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS14        (*(RwReg  *)0x4100A12FUL) /**< \brief (DMAC) Channel 14 Status */
#define REG_DMAC_CHCTRLA15         (*(RwReg  *)0x4100A130UL) /**< \brief (DMAC) Channel 15 Control A */
#define REG_DMAC_CHCTRLB15         (*(RwReg  *)0x4100A134UL) /**< \brief (DMAC) Channel 15 Control B */
#define REG_DMAC_CHPRILVL15        (*(RwReg  *)0x4100A135UL) /**< \brief (DMAC) Channel 15 Priority Level */
#define REG_DMAC_CHEVCTRL15        (*(RwReg  *)0x4100A136UL) /**< \brief (DMAC) Channel 15 Event Control */
#define REG_DMAC_CHINTENCLR15      (*(RwReg  *)0x4100A13CUL) /**< \brief (DMAC) Channel 15 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET15      (*(RwReg  *)0x4100A13DUL) /**< \brief (DMAC) Channel 15 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG15       (*(RwReg  *)0x4100A13EUL) /**< \brief (DMAC) Channel 15 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS15        (*(RwReg  *)0x4100A13FUL) /**< \brief (DMAC) Channel 15 Status */
#define REG_DMAC_CHCTRLA16         (*(RwReg  *)0x4100A140UL) /**< \brief (DMAC) Channel 16 Control A */
#define REG_DMAC_CHCTRLB16         (*(RwReg  *)0x4100A144UL) /**< \brief (DMAC) Channel 16 Control B */
#define REG_DMAC_CHPRILVL16        (*(RwReg  *)0x4100A145UL) /**< \brief (DMAC) Channel 16 Priority Level */
#define REG_DMAC_CHEVCTRL16        (*(RwReg  *)0x4100A146UL) /**< \brief (DMAC) Channel 16 Event Control */
#define REG_DMAC_CHINTENCLR16      (*(RwReg  *)0x4100A14CUL) /**< \brief (DMAC) Channel 16 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET16      (*(RwReg  *)0x4100A14DUL) /**< \brief (DMAC) Channel 16 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG16       (*(RwReg  *)0x4100A14EUL) /**< \brief (DMAC) Channel 16 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS16        (*(RwReg  *)0x4100A14FUL) /**< \brief (DMAC) Channel 16 Status */
#define REG_DMAC_CHCTRLA17         (*(RwReg  *)0x4100A150UL) /**< \brief (DMAC) Channel 17 Control A */
#define REG_DMAC_CHCTRLB17         (*(RwReg  *)0x4100A154UL) /**< \brief (DMAC) Channel 17 Control B */
#define REG_DMAC_CHPRILVL17        (*(RwReg  *)0x4100A155UL) /**< \brief (DMAC) Channel 17 Priority Level */
#define REG_DMAC_CHEVCTRL17        (*(RwReg  *)0x4100A156UL) /**< \brief (DMAC) Channel 17 Event Control */
#define REG_DMAC_CHINTENCLR17      (*(RwReg  *)0x4100A15CUL) /**< \brief (DMAC) Channel 17 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET17      (*(RwReg  *)0x4100A15DUL) /**< \brief (DMAC) Channel 17 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG17       (*(RwReg  *)0x4100A15EUL) /**< \brief (DMAC) Channel 17 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS17        (*(RwReg  *)0x4100A15FUL) /**< \brief (DMAC) Channel 17 Status */
#define REG_DMAC_CHCTRLA18         (*(RwReg  *)0x4100A160UL) /**< \brief (DMAC) Channel 18 Control A */
#define REG_DMAC_CHCTRLB18         (*(RwReg  *)0x4100A164UL) /**< \brief (DMAC) Channel 18 Control B */
#define REG_DMAC_CHPRILVL18        (*(RwReg  *)0x4100A165UL) /**< \brief (DMAC) Channel 18 Priority Level */
#define REG_DMAC_CHEVCTRL18        (*(RwReg  *)0x4100A166UL) /**< \brief (DMAC) Channel 18 Event Control */
#define REG_DMAC_CHINTENCLR18      (*(RwReg  *)0x4100A16CUL) /**< \brief (DMAC) Channel 18 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET18      (*(RwReg  *)0x4100A16DUL) /**< \brief (DMAC) Channel 18 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG18       (*(RwReg  *)0x4100A16EUL) /**< \brief (DMAC) Channel 18 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS18        (*(RwReg  *)0x4100A16FUL) /**< \brief (DMAC) Channel 18 Status */
#define REG_DMAC_CHCTRLA19         (*(RwReg  *)0x4100A170UL) /**< \brief (DMAC) Channel 19 Control A */
#define REG_DMAC_CHCTRLB19         (*(RwReg  *)0x4100A174UL) /**< \brief (DMAC) Channel 19 Control B */
#define REG_DMAC_CHPRILVL19        (*(RwReg  *)0x4100A175UL) /**< \brief (DMAC) Channel 19 Priority Level */
#define REG_DMAC_CHEVCTRL19        (*(RwReg  *)0x4100A176UL) /**< \brief (DMAC) Channel 19 Event Control */
#define REG_DMAC_CHINTENCLR19      (*(RwReg  *)0x4100A17CUL) /**< \brief (DMAC) Channel 19 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET19      (*(RwReg  *)0x4100A17DUL) /**< \brief (DMAC) Channel 19 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG19       (*(RwReg  *)0x4100A17EUL) /**< \brief (DMAC) Channel 19 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS19        (*(RwReg  *)0x4100A17FUL) /**< \brief (DMAC) Channel 19 Status */
#define REG_DMAC_CHCTRLA20         (*(RwReg  *)0x4100A180UL) /**< \brief (DMAC) Channel 20 Control A */
#define REG_DMAC_CHCTRLB20         (*(RwReg  *)0x4100A184UL) /**< \brief (DMAC) Channel 20 Control B */
#define REG_DMAC_CHPRILVL20        (*(RwReg  *)0x4100A185UL) /**< \brief (DMAC) Channel 20 Priority Level */
#define REG_DMAC_CHEVCTRL20        (*(RwReg  *)0x4100A186UL) /**< \brief (DMAC) Channel 20 Event Control */
#define REG_DMAC_CHINTENCLR20      (*(RwReg  *)0x4100A18CUL) /**< \brief (DMAC) Channel 20 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET20      (*(RwReg  *)0x4100A18DUL) /**< \brief (DMAC) Channel 20 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG20       (*(RwReg  *)0x4100A18EUL) /**< \brief (DMAC) Channel 20 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS20        (*(RwReg  *)0x4100A18FUL) /**< \brief (DMAC) Channel 20 Status */
#define REG_DMAC_CHCTRLA21         (*(RwReg  *)0x4100A190UL) /**< \brief (DMAC) Channel 21 Control A */
#define REG_DMAC_CHCTRLB21         (*(RwReg  *)0x4100A194UL) /**< \brief (DMAC) Channel 21 Control B */
#define REG_DMAC_CHPRILVL21        (*(RwReg  *)0x4100A195UL) /**< \brief (DMAC) Channel 21 Priority Level */
#define REG_DMAC_CHEVCTRL21        (*(RwReg  *)0x4100A196UL) /**< \brief (DMAC) Channel 21 Event Control */
#define REG_DMAC_CHINTENCLR21      (*(RwReg  *)0x4100A19CUL) /**< \brief (DMAC) Channel 21 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET21      (*(RwReg  *)0x4100A19DUL) /**< \brief (DMAC) Channel 21 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG21       (*(RwReg  *)0x4100A19EUL) /**< \brief (DMAC) Channel 21 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS21        (*(RwReg  *)0x4100A19FUL) /**< \brief (DMAC) Channel 21 Status */
#define REG_DMAC_CHCTRLA22         (*(RwReg  *)0x4100A1A0UL) /**< \brief (DMAC) Channel 22 Control A */
#define REG_DMAC_CHCTRLB22         (*(RwReg  *)0x4100A1A4UL) /**< \brief (DMAC) Channel 22 Control B */
#define REG_DMAC_CHPRILVL22        (*(RwReg  *)0x4100A1A5UL) /**< \brief (DMAC) Channel 22 Priority Level */
#define REG_DMAC_CHEVCTRL22        (*(RwReg  *)0x4100A1A6UL) /**< \brief (DMAC) Channel 22 Event Control */
#define REG_DMAC_CHINTENCLR22      (*(RwReg  *)0x4100A1ACUL) /**< \brief (DMAC) Channel 22 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET22      (*(RwReg  *)0x4100A1ADUL) /**< \brief (DMAC) Channel 22 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG22       (*(RwReg  *)0x4100A1AEUL) /**< \brief (DMAC) Channel 22 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS22        (*(RwReg  *)0x4100A1AFUL) /**< \brief (DMAC) Channel 22 Status */
#define REG_DMAC_CHCTRLA23         (*(RwReg  *)0x4100A1B0UL) /**< \brief (DMAC) Channel 23 Control A */
#define REG_DMAC_CHCTRLB23         (*(RwReg  *)0x4100A1B4UL) /**< \brief (DMAC) Channel 23 Control B */
#define REG_DMAC_CHPRILVL23        (*(RwReg  *)0x4100A1B5UL) /**< \brief (DMAC) Channel 23 Priority Level */
#define REG_DMAC_CHEVCTRL23        (*(RwReg  *)0x4100A1B6UL) /**< \brief (DMAC) Channel 23 Event Control */
#define REG_DMAC_CHINTENCLR23      (*(RwReg  *)0x4100A1BCUL) /**< \brief (DMAC) Channel 23 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET23      (*(RwReg  *)0x4100A1BDUL) /**< \brief (DMAC) Channel 23 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG23       (*(RwReg  *)0x4100A1BEUL) /**< \brief (DMAC) Channel 23 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS23        (*(RwReg  *)0x4100A1BFUL) /**< \brief (DMAC) Channel 23 Status */
#define REG_DMAC_CHCTRLA24         (*(RwReg  *)0x4100A1C0UL) /**< \brief (DMAC) Channel 24 Control A */
#define REG_DMAC_CHCTRLB24         (*(RwReg  *)0x4100A1C4UL) /**< \brief (DMAC) Channel 24 Control B */
#define REG_DMAC_CHPRILVL24        (*(RwReg  *)0x4100A1C5UL) /**< \brief (DMAC) Channel 24 Priority Level */
#define REG_DMAC_CHEVCTRL24        (*(RwReg  *)0x4100A1C6UL) /**< \brief (DMAC) Channel 24 Event Control */
#define REG_DMAC_CHINTENCLR24      (*(RwReg  *)0x4100A1CCUL) /**< \brief (DMAC) Channel 24 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET24      (*(RwReg  *)0x4100A1CDUL) /**< \brief (DMAC) Channel 24 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG24       (*(RwReg  *)0x4100A1CEUL) /**< \brief (DMAC) Channel 24 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS24        (*(RwReg  *)0x4100A1CFUL) /**< \brief (DMAC) Channel 24 Status */
#define REG_DMAC_CHCTRLA25         (*(RwReg  *)0x4100A1D0UL) /**< \brief (DMAC) Channel 25 Control A */
#define REG_DMAC_CHCTRLB25         (*(RwReg  *)0x4100A1D4UL) /**< \brief (DMAC) Channel 25 Control B */
#define REG_DMAC_CHPRILVL25        (*(RwReg  *)0x4100A1D5UL) /**< \brief (DMAC) Channel 25 Priority Level */
#define REG_DMAC_CHEVCTRL25        (*(RwReg  *)0x4100A1D6UL) /**< \brief (DMAC) Channel 25 Event Control */
#define REG_DMAC_CHINTENCLR25      (*(RwReg  *)0x4100A1DCUL) /**< \brief (DMAC) Channel 25 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET25      (*(RwReg  *)0x4100A1DDUL) /**< \brief (DMAC) Channel 25 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG25       (*(RwReg  *)0x4100A1DEUL) /**< \brief (DMAC) Channel 25 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS25        (*(RwReg  *)0x4100A1DFUL) /**< \brief (DMAC) Channel 25 Status */
#define REG_DMAC_CHCTRLA26         (*(RwReg  *)0x4100A1E0UL) /**< \brief (DMAC) Channel 26 Control A */
#define REG_DMAC_CHCTRLB26         (*(RwReg  *)0x4100A1E4UL) /**< \brief (DMAC) Channel 26 Control B */
#define REG_DMAC_CHPRILVL26        (*(RwReg  *)0x4100A1E5UL) /**< \brief (DMAC) Channel 26 Priority Level */
#define REG_DMAC_CHEVCTRL26        (*(RwReg  *)0x4100A1E6UL) /**< \brief (DMAC) Channel 26 Event Control */
#define REG_DMAC_CHINTENCLR26      (*(RwReg  *)0x4100A1ECUL) /**< \brief (DMAC) Channel 26 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET26      (*(RwReg  *)0x4100A1EDUL) /**< \brief (DMAC) Channel 26 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG26       (*(RwReg  *)0x4100A1EEUL) /**< \brief (DMAC) Channel 26 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS26        (*(RwReg  *)0x4100A1EFUL) /**< \brief (DMAC) Channel 26 Status */
#define REG_DMAC_CHCTRLA27         (*(RwReg  *)0x4100A1F0UL) /**< \brief (DMAC) Channel 27 Control A */
#define REG_DMAC_CHCTRLB27         (*(RwReg  *)0x4100A1F4UL) /**< \brief (DMAC) Channel 27 Control B */
#define REG_DMAC_CHPRILVL27        (*(RwReg  *)0x4100A1F5UL) /**< \brief (DMAC) Channel 27 Priority Level */
#define REG_DMAC_CHEVCTRL27        (*(RwReg  *)0x4100A1F6UL) /**< \brief (DMAC) Channel 27 Event Control */
#define REG_DMAC_CHINTENCLR27      (*(RwReg  *)0x4100A1FCUL) /**< \brief (DMAC) Channel 27 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET27      (*(RwReg  *)0x4100A1FDUL) /**< \brief (DMAC) Channel 27 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG27       (*(RwReg  *)0x4100A1FEUL) /**< \brief (DMAC) Channel 27 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS27        (*(RwReg  *)0x4100A1FFUL) /**< \brief (DMAC) Channel 27 Status */
#define REG_DMAC_CHCTRLA28         (*(RwReg  *)0x4100A200UL) /**< \brief (DMAC) Channel 28 Control A */
#define REG_DMAC_CHCTRLB28         (*(RwReg  *)0x4100A204UL) /**< \brief (DMAC) Channel 28 Control B */
#define REG_DMAC_CHPRILVL28        (*(RwReg  *)0x4100A205UL) /**< \brief (DMAC) Channel 28 Priority Level */
#define REG_DMAC_CHEVCTRL28        (*(RwReg  *)0x4100A206UL) /**< \brief (DMAC) Channel 28 Event Control */
#define REG_DMAC_CHINTENCLR28      (*(RwReg  *)0x4100A20CUL) /**< \brief (DMAC) Channel 28 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET28      (*(RwReg  *)0x4100A20DUL) /**< \brief (DMAC) Channel 28 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG28       (*(RwReg  *)0x4100A20EUL) /**< \brief (DMAC) Channel 28 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS28        (*(RwReg  *)0x4100A20FUL) /**< \brief (DMAC) Channel 28 Status */
#define REG_DMAC_CHCTRLA29         (*(RwReg  *)0x4100A210UL) /**< \brief (DMAC) Channel 29 Control A */
#define REG_DMAC_CHCTRLB29         (*(RwReg  *)0x4100A214UL) /**< \brief (DMAC) Channel 29 Control B */
#define REG_DMAC_CHPRILVL29        (*(RwReg  *)0x4100A215UL) /**< \brief (DMAC) Channel 29 Priority Level */
#define REG_DMAC_CHEVCTRL29        (*(RwReg  *)0x4100A216UL) /**< \brief (DMAC) Channel 29 Event Control */
#define REG_DMAC_CHINTENCLR29      (*(RwReg  *)0x4100A21CUL) /**< \brief (DMAC) Channel 29 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET29      (*(RwReg  *)0x4100A21DUL) /**< \brief (DMAC) Channel 29 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG29       (*(RwReg  *)0x4100A21EUL) /**< \brief (DMAC) Channel 29 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS29        (*(RwReg  *)0x4100A21FUL) /**< \brief (DMAC) Channel 29 Status */
#define REG_DMAC_CHCTRLA30         (*(RwReg  *)0x4100A220UL) /**< \brief (DMAC) Channel 30 Control A */
#define REG_DMAC_CHCTRLB30         (*(RwReg  *)0x4100A224UL) /**< \brief (DMAC) Channel 30 Control B */
#define REG_DMAC_CHPRILVL30        (*(RwReg  *)0x4100A225UL) /**< \brief (DMAC) Channel 30 Priority Level */
#define REG_DMAC_CHEVCTRL30        (*(RwReg  *)0x4100A226UL) /**< \brief (DMAC) Channel 30 Event Control */
#define REG_DMAC_CHINTENCLR30      (*(RwReg  *)0x4100A22CUL) /**< \brief (DMAC) Channel 30 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET30      (*(RwReg  *)0x4100A22DUL) /**< \brief (DMAC) Channel 30 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG30       (*(RwReg  *)0x4100A22EUL) /**< \brief (DMAC) Channel 30 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS30        (*(RwReg  *)0x4100A22FUL) /**< \brief (DMAC) Channel 30 Status */
#define REG_DMAC_CHCTRLA31         (*(RwReg  *)0x4100A230UL) /**< \brief (DMAC) Channel 31 Control A */
#define REG_DMAC_CHCTRLB31         (*(RwReg  *)0x4100A234UL) /**< \brief (DMAC) Channel 31 Control B */
#define REG_DMAC_CHPRILVL31        (*(RwReg  *)0x4100A235UL) /**< \brief (DMAC) Channel 31 Priority Level */
#define REG_DMAC_CHEVCTRL31        (*(RwReg  *)0x4100A236UL) /**< \brief (DMAC) Channel 31 Event Control */
#define REG_DMAC_CHINTENCLR31      (*(RwReg  *)0x4100A23CUL) /**< \brief (DMAC) Channel 31 Interrupt Enable Clear */
#define REG_DMAC_CHINTENSET31      (*(RwReg  *)0x4100A23DUL) /**< \brief (DMAC) Channel 31 Interrupt Enable Set */
#define REG_DMAC_CHINTFLAG31       (*(RwReg  *)0x4100A23EUL) /**< \brief (DMAC) Channel 31 Interrupt Flag Status and Clear */
#define REG_DMAC_CHSTATUS31        (*(RwReg  *)0x4100A23FUL) /**< \brief (DMAC) Channel 31 Status */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for DMAC peripheral ========== */
#define DMAC_BURST                  1        // 0: no burst support; 1: burst support
#define DMAC_CH_BITS                5        // Number of bits to select channel
#define DMAC_CH_NUM                 32       // Number of channels
#define DMAC_CLK_AHB_ID             9        // AHB clock index
#define DMAC_EVIN_NUM               8        // Number of input events
#define DMAC_EVOUT_NUM              4        // Number of output events
#define DMAC_FIFO_SIZE              16       // FIFO size for burst mode.
#define DMAC_LVL_BITS               2        // Number of bits to select level priority
#define DMAC_LVL_NUM                4        // Enable priority level number
#define DMAC_QOSCTRL_D_RESETVALUE   2        // QOS dmac ahb interface reset value
#define DMAC_QOSCTRL_F_RESETVALUE   2        // QOS dmac fetch interface reset value
#define DMAC_QOSCTRL_WRB_RESETVALUE 2        // QOS dmac write back interface reset value
#define DMAC_TRIG_BITS              7        // Number of bits to select trigger source
#define DMAC_TRIG_NUM               85       // Number of peripheral triggers

#endif /* _SAMD51_DMAC_INSTANCE_ */
