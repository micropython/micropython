/**
 * \file
 *
 * \brief Instance description for TAL
 *
 * Copyright (c) 2017 Atmel Corporation,
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

#ifndef _SAMD51_TAL_INSTANCE_
#define _SAMD51_TAL_INSTANCE_

/* ========== Register definition for TAL peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TAL_CTRLA              (0x4101E000) /**< \brief (TAL) Control A */
#define REG_TAL_EXTCTRL            (0x4101E001) /**< \brief (TAL) External Break Control */
#define REG_TAL_EVCTRL             (0x4101E004) /**< \brief (TAL) Event Control */
#define REG_TAL_INTENCLR           (0x4101E008) /**< \brief (TAL) Interrupt Enable Clear */
#define REG_TAL_INTENSET           (0x4101E009) /**< \brief (TAL) Interrupt Enable Set */
#define REG_TAL_INTFLAG            (0x4101E00A) /**< \brief (TAL) Interrupt Flag Status and Clear */
#define REG_TAL_GLOBMASK           (0x4101E00B) /**< \brief (TAL) Global Break Requests Mask */
#define REG_TAL_HALT               (0x4101E00C) /**< \brief (TAL) Debug Halt Request */
#define REG_TAL_RESTART            (0x4101E00D) /**< \brief (TAL) Debug Restart Request */
#define REG_TAL_BRKSTATUS          (0x4101E00E) /**< \brief (TAL) Break Request Status */
#define REG_TAL_CTICTRLA0          (0x4101E010) /**< \brief (TAL) Cross-Trigger Interface 0 Control A */
#define REG_TAL_CTIMASK0           (0x4101E011) /**< \brief (TAL) Cross-Trigger Interface 0 Mask */
#define REG_TAL_CTICTRLA1          (0x4101E012) /**< \brief (TAL) Cross-Trigger Interface 1 Control A */
#define REG_TAL_CTIMASK1           (0x4101E013) /**< \brief (TAL) Cross-Trigger Interface 1 Mask */
#define REG_TAL_CTICTRLA2          (0x4101E014) /**< \brief (TAL) Cross-Trigger Interface 2 Control A */
#define REG_TAL_CTIMASK2           (0x4101E015) /**< \brief (TAL) Cross-Trigger Interface 2 Mask */
#define REG_TAL_CTICTRLA3          (0x4101E016) /**< \brief (TAL) Cross-Trigger Interface 3 Control A */
#define REG_TAL_CTIMASK3           (0x4101E017) /**< \brief (TAL) Cross-Trigger Interface 3 Mask */
#define REG_TAL_INTSTATUS0         (0x4101E020) /**< \brief (TAL) Interrupt 0 Status */
#define REG_TAL_INTSTATUS1         (0x4101E021) /**< \brief (TAL) Interrupt 1 Status */
#define REG_TAL_INTSTATUS2         (0x4101E022) /**< \brief (TAL) Interrupt 2 Status */
#define REG_TAL_INTSTATUS3         (0x4101E023) /**< \brief (TAL) Interrupt 3 Status */
#define REG_TAL_INTSTATUS4         (0x4101E024) /**< \brief (TAL) Interrupt 4 Status */
#define REG_TAL_INTSTATUS5         (0x4101E025) /**< \brief (TAL) Interrupt 5 Status */
#define REG_TAL_INTSTATUS6         (0x4101E026) /**< \brief (TAL) Interrupt 6 Status */
#define REG_TAL_INTSTATUS7         (0x4101E027) /**< \brief (TAL) Interrupt 7 Status */
#define REG_TAL_INTSTATUS8         (0x4101E028) /**< \brief (TAL) Interrupt 8 Status */
#define REG_TAL_INTSTATUS9         (0x4101E029) /**< \brief (TAL) Interrupt 9 Status */
#define REG_TAL_INTSTATUS10        (0x4101E02A) /**< \brief (TAL) Interrupt 10 Status */
#define REG_TAL_INTSTATUS11        (0x4101E02B) /**< \brief (TAL) Interrupt 11 Status */
#define REG_TAL_INTSTATUS12        (0x4101E02C) /**< \brief (TAL) Interrupt 12 Status */
#define REG_TAL_INTSTATUS13        (0x4101E02D) /**< \brief (TAL) Interrupt 13 Status */
#define REG_TAL_INTSTATUS14        (0x4101E02E) /**< \brief (TAL) Interrupt 14 Status */
#define REG_TAL_INTSTATUS15        (0x4101E02F) /**< \brief (TAL) Interrupt 15 Status */
#define REG_TAL_INTSTATUS16        (0x4101E030) /**< \brief (TAL) Interrupt 16 Status */
#define REG_TAL_INTSTATUS17        (0x4101E031) /**< \brief (TAL) Interrupt 17 Status */
#define REG_TAL_INTSTATUS18        (0x4101E032) /**< \brief (TAL) Interrupt 18 Status */
#define REG_TAL_INTSTATUS19        (0x4101E033) /**< \brief (TAL) Interrupt 19 Status */
#define REG_TAL_INTSTATUS20        (0x4101E034) /**< \brief (TAL) Interrupt 20 Status */
#define REG_TAL_INTSTATUS21        (0x4101E035) /**< \brief (TAL) Interrupt 21 Status */
#define REG_TAL_INTSTATUS22        (0x4101E036) /**< \brief (TAL) Interrupt 22 Status */
#define REG_TAL_INTSTATUS23        (0x4101E037) /**< \brief (TAL) Interrupt 23 Status */
#define REG_TAL_INTSTATUS24        (0x4101E038) /**< \brief (TAL) Interrupt 24 Status */
#define REG_TAL_INTSTATUS25        (0x4101E039) /**< \brief (TAL) Interrupt 25 Status */
#define REG_TAL_INTSTATUS26        (0x4101E03A) /**< \brief (TAL) Interrupt 26 Status */
#define REG_TAL_INTSTATUS27        (0x4101E03B) /**< \brief (TAL) Interrupt 27 Status */
#define REG_TAL_INTSTATUS28        (0x4101E03C) /**< \brief (TAL) Interrupt 28 Status */
#define REG_TAL_INTSTATUS29        (0x4101E03D) /**< \brief (TAL) Interrupt 29 Status */
#define REG_TAL_INTSTATUS30        (0x4101E03E) /**< \brief (TAL) Interrupt 30 Status */
#define REG_TAL_INTSTATUS31        (0x4101E03F) /**< \brief (TAL) Interrupt 31 Status */
#define REG_TAL_INTSTATUS32        (0x4101E040) /**< \brief (TAL) Interrupt 32 Status */
#define REG_TAL_INTSTATUS33        (0x4101E041) /**< \brief (TAL) Interrupt 33 Status */
#define REG_TAL_INTSTATUS34        (0x4101E042) /**< \brief (TAL) Interrupt 34 Status */
#define REG_TAL_INTSTATUS35        (0x4101E043) /**< \brief (TAL) Interrupt 35 Status */
#define REG_TAL_INTSTATUS36        (0x4101E044) /**< \brief (TAL) Interrupt 36 Status */
#define REG_TAL_INTSTATUS37        (0x4101E045) /**< \brief (TAL) Interrupt 37 Status */
#define REG_TAL_INTSTATUS38        (0x4101E046) /**< \brief (TAL) Interrupt 38 Status */
#define REG_TAL_INTSTATUS39        (0x4101E047) /**< \brief (TAL) Interrupt 39 Status */
#define REG_TAL_INTSTATUS40        (0x4101E048) /**< \brief (TAL) Interrupt 40 Status */
#define REG_TAL_INTSTATUS41        (0x4101E049) /**< \brief (TAL) Interrupt 41 Status */
#define REG_TAL_INTSTATUS42        (0x4101E04A) /**< \brief (TAL) Interrupt 42 Status */
#define REG_TAL_INTSTATUS43        (0x4101E04B) /**< \brief (TAL) Interrupt 43 Status */
#define REG_TAL_INTSTATUS44        (0x4101E04C) /**< \brief (TAL) Interrupt 44 Status */
#define REG_TAL_INTSTATUS45        (0x4101E04D) /**< \brief (TAL) Interrupt 45 Status */
#define REG_TAL_INTSTATUS46        (0x4101E04E) /**< \brief (TAL) Interrupt 46 Status */
#define REG_TAL_INTSTATUS47        (0x4101E04F) /**< \brief (TAL) Interrupt 47 Status */
#define REG_TAL_INTSTATUS48        (0x4101E050) /**< \brief (TAL) Interrupt 48 Status */
#define REG_TAL_INTSTATUS49        (0x4101E051) /**< \brief (TAL) Interrupt 49 Status */
#define REG_TAL_INTSTATUS50        (0x4101E052) /**< \brief (TAL) Interrupt 50 Status */
#define REG_TAL_INTSTATUS51        (0x4101E053) /**< \brief (TAL) Interrupt 51 Status */
#define REG_TAL_INTSTATUS52        (0x4101E054) /**< \brief (TAL) Interrupt 52 Status */
#define REG_TAL_INTSTATUS53        (0x4101E055) /**< \brief (TAL) Interrupt 53 Status */
#define REG_TAL_INTSTATUS54        (0x4101E056) /**< \brief (TAL) Interrupt 54 Status */
#define REG_TAL_INTSTATUS55        (0x4101E057) /**< \brief (TAL) Interrupt 55 Status */
#define REG_TAL_INTSTATUS56        (0x4101E058) /**< \brief (TAL) Interrupt 56 Status */
#define REG_TAL_INTSTATUS57        (0x4101E059) /**< \brief (TAL) Interrupt 57 Status */
#define REG_TAL_INTSTATUS58        (0x4101E05A) /**< \brief (TAL) Interrupt 58 Status */
#define REG_TAL_INTSTATUS59        (0x4101E05B) /**< \brief (TAL) Interrupt 59 Status */
#define REG_TAL_INTSTATUS60        (0x4101E05C) /**< \brief (TAL) Interrupt 60 Status */
#define REG_TAL_INTSTATUS61        (0x4101E05D) /**< \brief (TAL) Interrupt 61 Status */
#define REG_TAL_INTSTATUS62        (0x4101E05E) /**< \brief (TAL) Interrupt 62 Status */
#define REG_TAL_INTSTATUS63        (0x4101E05F) /**< \brief (TAL) Interrupt 63 Status */
#define REG_TAL_INTSTATUS64        (0x4101E060) /**< \brief (TAL) Interrupt 64 Status */
#define REG_TAL_INTSTATUS65        (0x4101E061) /**< \brief (TAL) Interrupt 65 Status */
#define REG_TAL_INTSTATUS66        (0x4101E062) /**< \brief (TAL) Interrupt 66 Status */
#define REG_TAL_INTSTATUS67        (0x4101E063) /**< \brief (TAL) Interrupt 67 Status */
#define REG_TAL_INTSTATUS68        (0x4101E064) /**< \brief (TAL) Interrupt 68 Status */
#define REG_TAL_INTSTATUS69        (0x4101E065) /**< \brief (TAL) Interrupt 69 Status */
#define REG_TAL_INTSTATUS70        (0x4101E066) /**< \brief (TAL) Interrupt 70 Status */
#define REG_TAL_INTSTATUS71        (0x4101E067) /**< \brief (TAL) Interrupt 71 Status */
#define REG_TAL_INTSTATUS72        (0x4101E068) /**< \brief (TAL) Interrupt 72 Status */
#define REG_TAL_INTSTATUS73        (0x4101E069) /**< \brief (TAL) Interrupt 73 Status */
#define REG_TAL_INTSTATUS74        (0x4101E06A) /**< \brief (TAL) Interrupt 74 Status */
#define REG_TAL_INTSTATUS75        (0x4101E06B) /**< \brief (TAL) Interrupt 75 Status */
#define REG_TAL_INTSTATUS76        (0x4101E06C) /**< \brief (TAL) Interrupt 76 Status */
#define REG_TAL_INTSTATUS77        (0x4101E06D) /**< \brief (TAL) Interrupt 77 Status */
#define REG_TAL_INTSTATUS78        (0x4101E06E) /**< \brief (TAL) Interrupt 78 Status */
#define REG_TAL_INTSTATUS79        (0x4101E06F) /**< \brief (TAL) Interrupt 79 Status */
#define REG_TAL_INTSTATUS80        (0x4101E070) /**< \brief (TAL) Interrupt 80 Status */
#define REG_TAL_INTSTATUS81        (0x4101E071) /**< \brief (TAL) Interrupt 81 Status */
#define REG_TAL_INTSTATUS82        (0x4101E072) /**< \brief (TAL) Interrupt 82 Status */
#define REG_TAL_INTSTATUS83        (0x4101E073) /**< \brief (TAL) Interrupt 83 Status */
#define REG_TAL_INTSTATUS84        (0x4101E074) /**< \brief (TAL) Interrupt 84 Status */
#define REG_TAL_INTSTATUS85        (0x4101E075) /**< \brief (TAL) Interrupt 85 Status */
#define REG_TAL_INTSTATUS86        (0x4101E076) /**< \brief (TAL) Interrupt 86 Status */
#define REG_TAL_INTSTATUS87        (0x4101E077) /**< \brief (TAL) Interrupt 87 Status */
#define REG_TAL_INTSTATUS88        (0x4101E078) /**< \brief (TAL) Interrupt 88 Status */
#define REG_TAL_INTSTATUS89        (0x4101E079) /**< \brief (TAL) Interrupt 89 Status */
#define REG_TAL_INTSTATUS90        (0x4101E07A) /**< \brief (TAL) Interrupt 90 Status */
#define REG_TAL_INTSTATUS91        (0x4101E07B) /**< \brief (TAL) Interrupt 91 Status */
#define REG_TAL_INTSTATUS92        (0x4101E07C) /**< \brief (TAL) Interrupt 92 Status */
#define REG_TAL_INTSTATUS93        (0x4101E07D) /**< \brief (TAL) Interrupt 93 Status */
#define REG_TAL_INTSTATUS94        (0x4101E07E) /**< \brief (TAL) Interrupt 94 Status */
#define REG_TAL_INTSTATUS95        (0x4101E07F) /**< \brief (TAL) Interrupt 95 Status */
#define REG_TAL_INTSTATUS96        (0x4101E080) /**< \brief (TAL) Interrupt 96 Status */
#define REG_TAL_INTSTATUS97        (0x4101E081) /**< \brief (TAL) Interrupt 97 Status */
#define REG_TAL_INTSTATUS98        (0x4101E082) /**< \brief (TAL) Interrupt 98 Status */
#define REG_TAL_INTSTATUS99        (0x4101E083) /**< \brief (TAL) Interrupt 99 Status */
#define REG_TAL_INTSTATUS100       (0x4101E084) /**< \brief (TAL) Interrupt 100 Status */
#define REG_TAL_INTSTATUS101       (0x4101E085) /**< \brief (TAL) Interrupt 101 Status */
#define REG_TAL_INTSTATUS102       (0x4101E086) /**< \brief (TAL) Interrupt 102 Status */
#define REG_TAL_INTSTATUS103       (0x4101E087) /**< \brief (TAL) Interrupt 103 Status */
#define REG_TAL_INTSTATUS104       (0x4101E088) /**< \brief (TAL) Interrupt 104 Status */
#define REG_TAL_INTSTATUS105       (0x4101E089) /**< \brief (TAL) Interrupt 105 Status */
#define REG_TAL_INTSTATUS106       (0x4101E08A) /**< \brief (TAL) Interrupt 106 Status */
#define REG_TAL_INTSTATUS107       (0x4101E08B) /**< \brief (TAL) Interrupt 107 Status */
#define REG_TAL_INTSTATUS108       (0x4101E08C) /**< \brief (TAL) Interrupt 108 Status */
#define REG_TAL_INTSTATUS109       (0x4101E08D) /**< \brief (TAL) Interrupt 109 Status */
#define REG_TAL_INTSTATUS110       (0x4101E08E) /**< \brief (TAL) Interrupt 110 Status */
#define REG_TAL_INTSTATUS111       (0x4101E08F) /**< \brief (TAL) Interrupt 111 Status */
#define REG_TAL_INTSTATUS112       (0x4101E090) /**< \brief (TAL) Interrupt 112 Status */
#define REG_TAL_INTSTATUS113       (0x4101E091) /**< \brief (TAL) Interrupt 113 Status */
#define REG_TAL_INTSTATUS114       (0x4101E092) /**< \brief (TAL) Interrupt 114 Status */
#define REG_TAL_INTSTATUS115       (0x4101E093) /**< \brief (TAL) Interrupt 115 Status */
#define REG_TAL_INTSTATUS116       (0x4101E094) /**< \brief (TAL) Interrupt 116 Status */
#define REG_TAL_INTSTATUS117       (0x4101E095) /**< \brief (TAL) Interrupt 117 Status */
#define REG_TAL_INTSTATUS118       (0x4101E096) /**< \brief (TAL) Interrupt 118 Status */
#define REG_TAL_INTSTATUS119       (0x4101E097) /**< \brief (TAL) Interrupt 119 Status */
#define REG_TAL_INTSTATUS120       (0x4101E098) /**< \brief (TAL) Interrupt 120 Status */
#define REG_TAL_INTSTATUS121       (0x4101E099) /**< \brief (TAL) Interrupt 121 Status */
#define REG_TAL_INTSTATUS122       (0x4101E09A) /**< \brief (TAL) Interrupt 122 Status */
#define REG_TAL_INTSTATUS123       (0x4101E09B) /**< \brief (TAL) Interrupt 123 Status */
#define REG_TAL_INTSTATUS124       (0x4101E09C) /**< \brief (TAL) Interrupt 124 Status */
#define REG_TAL_INTSTATUS125       (0x4101E09D) /**< \brief (TAL) Interrupt 125 Status */
#define REG_TAL_INTSTATUS126       (0x4101E09E) /**< \brief (TAL) Interrupt 126 Status */
#define REG_TAL_INTSTATUS127       (0x4101E09F) /**< \brief (TAL) Interrupt 127 Status */
#define REG_TAL_INTSTATUS128       (0x4101E0A0) /**< \brief (TAL) Interrupt 128 Status */
#define REG_TAL_INTSTATUS129       (0x4101E0A1) /**< \brief (TAL) Interrupt 129 Status */
#define REG_TAL_INTSTATUS130       (0x4101E0A2) /**< \brief (TAL) Interrupt 130 Status */
#define REG_TAL_INTSTATUS131       (0x4101E0A3) /**< \brief (TAL) Interrupt 131 Status */
#define REG_TAL_INTSTATUS132       (0x4101E0A4) /**< \brief (TAL) Interrupt 132 Status */
#define REG_TAL_INTSTATUS133       (0x4101E0A5) /**< \brief (TAL) Interrupt 133 Status */
#define REG_TAL_INTSTATUS134       (0x4101E0A6) /**< \brief (TAL) Interrupt 134 Status */
#define REG_TAL_INTSTATUS135       (0x4101E0A7) /**< \brief (TAL) Interrupt 135 Status */
#define REG_TAL_INTSTATUS136       (0x4101E0A8) /**< \brief (TAL) Interrupt 136 Status */
#define REG_TAL_DMACPUSEL0         (0x4101E110) /**< \brief (TAL) DMA Channel Interrupts CPU Select 0 */
#define REG_TAL_DMACPUSEL1         (0x4101E114) /**< \brief (TAL) DMA Channel Interrupts CPU Select 1 */
#define REG_TAL_EVCPUSEL0          (0x4101E118) /**< \brief (TAL) EVSYS Channel Interrupts CPU Select 0 */
#define REG_TAL_EICCPUSEL0         (0x4101E120) /**< \brief (TAL) EIC External Interrupts CPU Select 0 */
#define REG_TAL_INTCPUSEL0         (0x4101E128) /**< \brief (TAL) Interrupts CPU Select 0 */
#define REG_TAL_INTCPUSEL1         (0x4101E12C) /**< \brief (TAL) Interrupts CPU Select 1 */
#define REG_TAL_INTCPUSEL2         (0x4101E130) /**< \brief (TAL) Interrupts CPU Select 2 */
#define REG_TAL_INTCPUSEL3         (0x4101E134) /**< \brief (TAL) Interrupts CPU Select 3 */
#define REG_TAL_INTCPUSEL4         (0x4101E138) /**< \brief (TAL) Interrupts CPU Select 4 */
#define REG_TAL_INTCPUSEL5         (0x4101E13C) /**< \brief (TAL) Interrupts CPU Select 5 */
#define REG_TAL_INTCPUSEL6         (0x4101E140) /**< \brief (TAL) Interrupts CPU Select 6 */
#define REG_TAL_INTCPUSEL7         (0x4101E144) /**< \brief (TAL) Interrupts CPU Select 7 */
#define REG_TAL_INTCPUSEL8         (0x4101E148) /**< \brief (TAL) Interrupts CPU Select 8 */
#define REG_TAL_IRQTRIG            (0x4101E164) /**< \brief (TAL) Interrupt Trigger */
#define REG_TAL_IRQMON0            (0x4101E168) /**< \brief (TAL) Interrupt Monitor Select 0 */
#define REG_TAL_CPUIRQS0           (0x4101E180) /**< \brief (TAL) Interrupt Status m for CPU 0 */
#define REG_TAL_CPUIRQS1           (0x4101E1A0) /**< \brief (TAL) Interrupt Status m for CPU 1 */
#define REG_TAL_SMASK0             (0x4101E200) /**< \brief (TAL) Inter-Process Signal Mask m for CPU 0 */
#define REG_TAL_SMASK1             (0x4101E208) /**< \brief (TAL) Inter-Process Signal Mask m for CPU 1 */
#define REG_TAL_SFLAGCLR0          (0x4101E220) /**< \brief (TAL) Inter-Process Signal Flag Clear 0 */
#define REG_TAL_SFLAGCLR1          (0x4101E224) /**< \brief (TAL) Inter-Process Signal Flag Clear 1 */
#define REG_TAL_SFLAGSET0          (0x4101E228) /**< \brief (TAL) Inter-Process Signal Flag Set 0 */
#define REG_TAL_SFLAGSET1          (0x4101E22C) /**< \brief (TAL) Inter-Process Signal Flag Set 1 */
#define REG_TAL_SFLAG0             (0x4101E230) /**< \brief (TAL) Inter-Process Signal Flag 0 */
#define REG_TAL_SFLAG1             (0x4101E234) /**< \brief (TAL) Inter-Process Signal Flag 1 */
#define REG_TAL_SFLAGCLRR0         (0x4101E300) /**< \brief (TAL) Inter-Process Signal Flag Bit 0 */
#define REG_TAL_SFLAGCLRR1         (0x4101E301) /**< \brief (TAL) Inter-Process Signal Flag Bit 1 */
#define REG_TAL_SFLAGCLRR2         (0x4101E302) /**< \brief (TAL) Inter-Process Signal Flag Bit 2 */
#define REG_TAL_SFLAGCLRR3         (0x4101E303) /**< \brief (TAL) Inter-Process Signal Flag Bit 3 */
#define REG_TAL_SFLAGCLRR4         (0x4101E304) /**< \brief (TAL) Inter-Process Signal Flag Bit 4 */
#define REG_TAL_SFLAGCLRR5         (0x4101E305) /**< \brief (TAL) Inter-Process Signal Flag Bit 5 */
#define REG_TAL_SFLAGCLRR6         (0x4101E306) /**< \brief (TAL) Inter-Process Signal Flag Bit 6 */
#define REG_TAL_SFLAGCLRR7         (0x4101E307) /**< \brief (TAL) Inter-Process Signal Flag Bit 7 */
#define REG_TAL_SFLAGCLRR8         (0x4101E308) /**< \brief (TAL) Inter-Process Signal Flag Bit 8 */
#define REG_TAL_SFLAGCLRR9         (0x4101E309) /**< \brief (TAL) Inter-Process Signal Flag Bit 9 */
#define REG_TAL_SFLAGCLRR10        (0x4101E30A) /**< \brief (TAL) Inter-Process Signal Flag Bit 10 */
#define REG_TAL_SFLAGCLRR11        (0x4101E30B) /**< \brief (TAL) Inter-Process Signal Flag Bit 11 */
#define REG_TAL_SFLAGCLRR12        (0x4101E30C) /**< \brief (TAL) Inter-Process Signal Flag Bit 12 */
#define REG_TAL_SFLAGCLRR13        (0x4101E30D) /**< \brief (TAL) Inter-Process Signal Flag Bit 13 */
#define REG_TAL_SFLAGCLRR14        (0x4101E30E) /**< \brief (TAL) Inter-Process Signal Flag Bit 14 */
#define REG_TAL_SFLAGCLRR15        (0x4101E30F) /**< \brief (TAL) Inter-Process Signal Flag Bit 15 */
#define REG_TAL_SFLAGCLRR16        (0x4101E310) /**< \brief (TAL) Inter-Process Signal Flag Bit 16 */
#define REG_TAL_SFLAGCLRR17        (0x4101E311) /**< \brief (TAL) Inter-Process Signal Flag Bit 17 */
#define REG_TAL_SFLAGCLRR18        (0x4101E312) /**< \brief (TAL) Inter-Process Signal Flag Bit 18 */
#define REG_TAL_SFLAGCLRR19        (0x4101E313) /**< \brief (TAL) Inter-Process Signal Flag Bit 19 */
#define REG_TAL_SFLAGCLRR20        (0x4101E314) /**< \brief (TAL) Inter-Process Signal Flag Bit 20 */
#define REG_TAL_SFLAGCLRR21        (0x4101E315) /**< \brief (TAL) Inter-Process Signal Flag Bit 21 */
#define REG_TAL_SFLAGCLRR22        (0x4101E316) /**< \brief (TAL) Inter-Process Signal Flag Bit 22 */
#define REG_TAL_SFLAGCLRR23        (0x4101E317) /**< \brief (TAL) Inter-Process Signal Flag Bit 23 */
#define REG_TAL_SFLAGCLRR24        (0x4101E318) /**< \brief (TAL) Inter-Process Signal Flag Bit 24 */
#define REG_TAL_SFLAGCLRR25        (0x4101E319) /**< \brief (TAL) Inter-Process Signal Flag Bit 25 */
#define REG_TAL_SFLAGCLRR26        (0x4101E31A) /**< \brief (TAL) Inter-Process Signal Flag Bit 26 */
#define REG_TAL_SFLAGCLRR27        (0x4101E31B) /**< \brief (TAL) Inter-Process Signal Flag Bit 27 */
#define REG_TAL_SFLAGCLRR28        (0x4101E31C) /**< \brief (TAL) Inter-Process Signal Flag Bit 28 */
#define REG_TAL_SFLAGCLRR29        (0x4101E31D) /**< \brief (TAL) Inter-Process Signal Flag Bit 29 */
#define REG_TAL_SFLAGCLRR30        (0x4101E31E) /**< \brief (TAL) Inter-Process Signal Flag Bit 30 */
#define REG_TAL_SFLAGCLRR31        (0x4101E31F) /**< \brief (TAL) Inter-Process Signal Flag Bit 31 */
#define REG_TAL_SFLAGCLRR32        (0x4101E320) /**< \brief (TAL) Inter-Process Signal Flag Bit 32 */
#define REG_TAL_SFLAGCLRR33        (0x4101E321) /**< \brief (TAL) Inter-Process Signal Flag Bit 33 */
#define REG_TAL_SFLAGCLRR34        (0x4101E322) /**< \brief (TAL) Inter-Process Signal Flag Bit 34 */
#define REG_TAL_SFLAGCLRR35        (0x4101E323) /**< \brief (TAL) Inter-Process Signal Flag Bit 35 */
#define REG_TAL_SFLAGCLRR36        (0x4101E324) /**< \brief (TAL) Inter-Process Signal Flag Bit 36 */
#define REG_TAL_SFLAGCLRR37        (0x4101E325) /**< \brief (TAL) Inter-Process Signal Flag Bit 37 */
#define REG_TAL_SFLAGCLRR38        (0x4101E326) /**< \brief (TAL) Inter-Process Signal Flag Bit 38 */
#define REG_TAL_SFLAGCLRR39        (0x4101E327) /**< \brief (TAL) Inter-Process Signal Flag Bit 39 */
#define REG_TAL_SFLAGCLRR40        (0x4101E328) /**< \brief (TAL) Inter-Process Signal Flag Bit 40 */
#define REG_TAL_SFLAGCLRR41        (0x4101E329) /**< \brief (TAL) Inter-Process Signal Flag Bit 41 */
#define REG_TAL_SFLAGCLRR42        (0x4101E32A) /**< \brief (TAL) Inter-Process Signal Flag Bit 42 */
#define REG_TAL_SFLAGCLRR43        (0x4101E32B) /**< \brief (TAL) Inter-Process Signal Flag Bit 43 */
#define REG_TAL_SFLAGCLRR44        (0x4101E32C) /**< \brief (TAL) Inter-Process Signal Flag Bit 44 */
#define REG_TAL_SFLAGCLRR45        (0x4101E32D) /**< \brief (TAL) Inter-Process Signal Flag Bit 45 */
#define REG_TAL_SFLAGCLRR46        (0x4101E32E) /**< \brief (TAL) Inter-Process Signal Flag Bit 46 */
#define REG_TAL_SFLAGCLRR47        (0x4101E32F) /**< \brief (TAL) Inter-Process Signal Flag Bit 47 */
#define REG_TAL_SFLAGCLRR48        (0x4101E330) /**< \brief (TAL) Inter-Process Signal Flag Bit 48 */
#define REG_TAL_SFLAGCLRR49        (0x4101E331) /**< \brief (TAL) Inter-Process Signal Flag Bit 49 */
#define REG_TAL_SFLAGCLRR50        (0x4101E332) /**< \brief (TAL) Inter-Process Signal Flag Bit 50 */
#define REG_TAL_SFLAGCLRR51        (0x4101E333) /**< \brief (TAL) Inter-Process Signal Flag Bit 51 */
#define REG_TAL_SFLAGCLRR52        (0x4101E334) /**< \brief (TAL) Inter-Process Signal Flag Bit 52 */
#define REG_TAL_SFLAGCLRR53        (0x4101E335) /**< \brief (TAL) Inter-Process Signal Flag Bit 53 */
#define REG_TAL_SFLAGCLRR54        (0x4101E336) /**< \brief (TAL) Inter-Process Signal Flag Bit 54 */
#define REG_TAL_SFLAGCLRR55        (0x4101E337) /**< \brief (TAL) Inter-Process Signal Flag Bit 55 */
#define REG_TAL_SFLAGCLRR56        (0x4101E338) /**< \brief (TAL) Inter-Process Signal Flag Bit 56 */
#define REG_TAL_SFLAGCLRR57        (0x4101E339) /**< \brief (TAL) Inter-Process Signal Flag Bit 57 */
#define REG_TAL_SFLAGCLRR58        (0x4101E33A) /**< \brief (TAL) Inter-Process Signal Flag Bit 58 */
#define REG_TAL_SFLAGCLRR59        (0x4101E33B) /**< \brief (TAL) Inter-Process Signal Flag Bit 59 */
#define REG_TAL_SFLAGCLRR60        (0x4101E33C) /**< \brief (TAL) Inter-Process Signal Flag Bit 60 */
#define REG_TAL_SFLAGCLRR61        (0x4101E33D) /**< \brief (TAL) Inter-Process Signal Flag Bit 61 */
#define REG_TAL_SFLAGCLRR62        (0x4101E33E) /**< \brief (TAL) Inter-Process Signal Flag Bit 62 */
#define REG_TAL_SFLAGCLRR63        (0x4101E33F) /**< \brief (TAL) Inter-Process Signal Flag Bit 63 */
#else
#define REG_TAL_CTRLA              (*(RwReg8 *)0x4101E000UL) /**< \brief (TAL) Control A */
#define REG_TAL_EXTCTRL            (*(RwReg8 *)0x4101E001UL) /**< \brief (TAL) External Break Control */
#define REG_TAL_EVCTRL             (*(RwReg16*)0x4101E004UL) /**< \brief (TAL) Event Control */
#define REG_TAL_INTENCLR           (*(RwReg8 *)0x4101E008UL) /**< \brief (TAL) Interrupt Enable Clear */
#define REG_TAL_INTENSET           (*(RwReg8 *)0x4101E009UL) /**< \brief (TAL) Interrupt Enable Set */
#define REG_TAL_INTFLAG            (*(RwReg8 *)0x4101E00AUL) /**< \brief (TAL) Interrupt Flag Status and Clear */
#define REG_TAL_GLOBMASK           (*(RwReg8 *)0x4101E00BUL) /**< \brief (TAL) Global Break Requests Mask */
#define REG_TAL_HALT               (*(WoReg8 *)0x4101E00CUL) /**< \brief (TAL) Debug Halt Request */
#define REG_TAL_RESTART            (*(WoReg8 *)0x4101E00DUL) /**< \brief (TAL) Debug Restart Request */
#define REG_TAL_BRKSTATUS          (*(RoReg16*)0x4101E00EUL) /**< \brief (TAL) Break Request Status */
#define REG_TAL_CTICTRLA0          (*(RwReg8 *)0x4101E010UL) /**< \brief (TAL) Cross-Trigger Interface 0 Control A */
#define REG_TAL_CTIMASK0           (*(RwReg8 *)0x4101E011UL) /**< \brief (TAL) Cross-Trigger Interface 0 Mask */
#define REG_TAL_CTICTRLA1          (*(RwReg8 *)0x4101E012UL) /**< \brief (TAL) Cross-Trigger Interface 1 Control A */
#define REG_TAL_CTIMASK1           (*(RwReg8 *)0x4101E013UL) /**< \brief (TAL) Cross-Trigger Interface 1 Mask */
#define REG_TAL_CTICTRLA2          (*(RwReg8 *)0x4101E014UL) /**< \brief (TAL) Cross-Trigger Interface 2 Control A */
#define REG_TAL_CTIMASK2           (*(RwReg8 *)0x4101E015UL) /**< \brief (TAL) Cross-Trigger Interface 2 Mask */
#define REG_TAL_CTICTRLA3          (*(RwReg8 *)0x4101E016UL) /**< \brief (TAL) Cross-Trigger Interface 3 Control A */
#define REG_TAL_CTIMASK3           (*(RwReg8 *)0x4101E017UL) /**< \brief (TAL) Cross-Trigger Interface 3 Mask */
#define REG_TAL_INTSTATUS0         (*(RoReg8 *)0x4101E020UL) /**< \brief (TAL) Interrupt 0 Status */
#define REG_TAL_INTSTATUS1         (*(RoReg8 *)0x4101E021UL) /**< \brief (TAL) Interrupt 1 Status */
#define REG_TAL_INTSTATUS2         (*(RoReg8 *)0x4101E022UL) /**< \brief (TAL) Interrupt 2 Status */
#define REG_TAL_INTSTATUS3         (*(RoReg8 *)0x4101E023UL) /**< \brief (TAL) Interrupt 3 Status */
#define REG_TAL_INTSTATUS4         (*(RoReg8 *)0x4101E024UL) /**< \brief (TAL) Interrupt 4 Status */
#define REG_TAL_INTSTATUS5         (*(RoReg8 *)0x4101E025UL) /**< \brief (TAL) Interrupt 5 Status */
#define REG_TAL_INTSTATUS6         (*(RoReg8 *)0x4101E026UL) /**< \brief (TAL) Interrupt 6 Status */
#define REG_TAL_INTSTATUS7         (*(RoReg8 *)0x4101E027UL) /**< \brief (TAL) Interrupt 7 Status */
#define REG_TAL_INTSTATUS8         (*(RoReg8 *)0x4101E028UL) /**< \brief (TAL) Interrupt 8 Status */
#define REG_TAL_INTSTATUS9         (*(RoReg8 *)0x4101E029UL) /**< \brief (TAL) Interrupt 9 Status */
#define REG_TAL_INTSTATUS10        (*(RoReg8 *)0x4101E02AUL) /**< \brief (TAL) Interrupt 10 Status */
#define REG_TAL_INTSTATUS11        (*(RoReg8 *)0x4101E02BUL) /**< \brief (TAL) Interrupt 11 Status */
#define REG_TAL_INTSTATUS12        (*(RoReg8 *)0x4101E02CUL) /**< \brief (TAL) Interrupt 12 Status */
#define REG_TAL_INTSTATUS13        (*(RoReg8 *)0x4101E02DUL) /**< \brief (TAL) Interrupt 13 Status */
#define REG_TAL_INTSTATUS14        (*(RoReg8 *)0x4101E02EUL) /**< \brief (TAL) Interrupt 14 Status */
#define REG_TAL_INTSTATUS15        (*(RoReg8 *)0x4101E02FUL) /**< \brief (TAL) Interrupt 15 Status */
#define REG_TAL_INTSTATUS16        (*(RoReg8 *)0x4101E030UL) /**< \brief (TAL) Interrupt 16 Status */
#define REG_TAL_INTSTATUS17        (*(RoReg8 *)0x4101E031UL) /**< \brief (TAL) Interrupt 17 Status */
#define REG_TAL_INTSTATUS18        (*(RoReg8 *)0x4101E032UL) /**< \brief (TAL) Interrupt 18 Status */
#define REG_TAL_INTSTATUS19        (*(RoReg8 *)0x4101E033UL) /**< \brief (TAL) Interrupt 19 Status */
#define REG_TAL_INTSTATUS20        (*(RoReg8 *)0x4101E034UL) /**< \brief (TAL) Interrupt 20 Status */
#define REG_TAL_INTSTATUS21        (*(RoReg8 *)0x4101E035UL) /**< \brief (TAL) Interrupt 21 Status */
#define REG_TAL_INTSTATUS22        (*(RoReg8 *)0x4101E036UL) /**< \brief (TAL) Interrupt 22 Status */
#define REG_TAL_INTSTATUS23        (*(RoReg8 *)0x4101E037UL) /**< \brief (TAL) Interrupt 23 Status */
#define REG_TAL_INTSTATUS24        (*(RoReg8 *)0x4101E038UL) /**< \brief (TAL) Interrupt 24 Status */
#define REG_TAL_INTSTATUS25        (*(RoReg8 *)0x4101E039UL) /**< \brief (TAL) Interrupt 25 Status */
#define REG_TAL_INTSTATUS26        (*(RoReg8 *)0x4101E03AUL) /**< \brief (TAL) Interrupt 26 Status */
#define REG_TAL_INTSTATUS27        (*(RoReg8 *)0x4101E03BUL) /**< \brief (TAL) Interrupt 27 Status */
#define REG_TAL_INTSTATUS28        (*(RoReg8 *)0x4101E03CUL) /**< \brief (TAL) Interrupt 28 Status */
#define REG_TAL_INTSTATUS29        (*(RoReg8 *)0x4101E03DUL) /**< \brief (TAL) Interrupt 29 Status */
#define REG_TAL_INTSTATUS30        (*(RoReg8 *)0x4101E03EUL) /**< \brief (TAL) Interrupt 30 Status */
#define REG_TAL_INTSTATUS31        (*(RoReg8 *)0x4101E03FUL) /**< \brief (TAL) Interrupt 31 Status */
#define REG_TAL_INTSTATUS32        (*(RoReg8 *)0x4101E040UL) /**< \brief (TAL) Interrupt 32 Status */
#define REG_TAL_INTSTATUS33        (*(RoReg8 *)0x4101E041UL) /**< \brief (TAL) Interrupt 33 Status */
#define REG_TAL_INTSTATUS34        (*(RoReg8 *)0x4101E042UL) /**< \brief (TAL) Interrupt 34 Status */
#define REG_TAL_INTSTATUS35        (*(RoReg8 *)0x4101E043UL) /**< \brief (TAL) Interrupt 35 Status */
#define REG_TAL_INTSTATUS36        (*(RoReg8 *)0x4101E044UL) /**< \brief (TAL) Interrupt 36 Status */
#define REG_TAL_INTSTATUS37        (*(RoReg8 *)0x4101E045UL) /**< \brief (TAL) Interrupt 37 Status */
#define REG_TAL_INTSTATUS38        (*(RoReg8 *)0x4101E046UL) /**< \brief (TAL) Interrupt 38 Status */
#define REG_TAL_INTSTATUS39        (*(RoReg8 *)0x4101E047UL) /**< \brief (TAL) Interrupt 39 Status */
#define REG_TAL_INTSTATUS40        (*(RoReg8 *)0x4101E048UL) /**< \brief (TAL) Interrupt 40 Status */
#define REG_TAL_INTSTATUS41        (*(RoReg8 *)0x4101E049UL) /**< \brief (TAL) Interrupt 41 Status */
#define REG_TAL_INTSTATUS42        (*(RoReg8 *)0x4101E04AUL) /**< \brief (TAL) Interrupt 42 Status */
#define REG_TAL_INTSTATUS43        (*(RoReg8 *)0x4101E04BUL) /**< \brief (TAL) Interrupt 43 Status */
#define REG_TAL_INTSTATUS44        (*(RoReg8 *)0x4101E04CUL) /**< \brief (TAL) Interrupt 44 Status */
#define REG_TAL_INTSTATUS45        (*(RoReg8 *)0x4101E04DUL) /**< \brief (TAL) Interrupt 45 Status */
#define REG_TAL_INTSTATUS46        (*(RoReg8 *)0x4101E04EUL) /**< \brief (TAL) Interrupt 46 Status */
#define REG_TAL_INTSTATUS47        (*(RoReg8 *)0x4101E04FUL) /**< \brief (TAL) Interrupt 47 Status */
#define REG_TAL_INTSTATUS48        (*(RoReg8 *)0x4101E050UL) /**< \brief (TAL) Interrupt 48 Status */
#define REG_TAL_INTSTATUS49        (*(RoReg8 *)0x4101E051UL) /**< \brief (TAL) Interrupt 49 Status */
#define REG_TAL_INTSTATUS50        (*(RoReg8 *)0x4101E052UL) /**< \brief (TAL) Interrupt 50 Status */
#define REG_TAL_INTSTATUS51        (*(RoReg8 *)0x4101E053UL) /**< \brief (TAL) Interrupt 51 Status */
#define REG_TAL_INTSTATUS52        (*(RoReg8 *)0x4101E054UL) /**< \brief (TAL) Interrupt 52 Status */
#define REG_TAL_INTSTATUS53        (*(RoReg8 *)0x4101E055UL) /**< \brief (TAL) Interrupt 53 Status */
#define REG_TAL_INTSTATUS54        (*(RoReg8 *)0x4101E056UL) /**< \brief (TAL) Interrupt 54 Status */
#define REG_TAL_INTSTATUS55        (*(RoReg8 *)0x4101E057UL) /**< \brief (TAL) Interrupt 55 Status */
#define REG_TAL_INTSTATUS56        (*(RoReg8 *)0x4101E058UL) /**< \brief (TAL) Interrupt 56 Status */
#define REG_TAL_INTSTATUS57        (*(RoReg8 *)0x4101E059UL) /**< \brief (TAL) Interrupt 57 Status */
#define REG_TAL_INTSTATUS58        (*(RoReg8 *)0x4101E05AUL) /**< \brief (TAL) Interrupt 58 Status */
#define REG_TAL_INTSTATUS59        (*(RoReg8 *)0x4101E05BUL) /**< \brief (TAL) Interrupt 59 Status */
#define REG_TAL_INTSTATUS60        (*(RoReg8 *)0x4101E05CUL) /**< \brief (TAL) Interrupt 60 Status */
#define REG_TAL_INTSTATUS61        (*(RoReg8 *)0x4101E05DUL) /**< \brief (TAL) Interrupt 61 Status */
#define REG_TAL_INTSTATUS62        (*(RoReg8 *)0x4101E05EUL) /**< \brief (TAL) Interrupt 62 Status */
#define REG_TAL_INTSTATUS63        (*(RoReg8 *)0x4101E05FUL) /**< \brief (TAL) Interrupt 63 Status */
#define REG_TAL_INTSTATUS64        (*(RoReg8 *)0x4101E060UL) /**< \brief (TAL) Interrupt 64 Status */
#define REG_TAL_INTSTATUS65        (*(RoReg8 *)0x4101E061UL) /**< \brief (TAL) Interrupt 65 Status */
#define REG_TAL_INTSTATUS66        (*(RoReg8 *)0x4101E062UL) /**< \brief (TAL) Interrupt 66 Status */
#define REG_TAL_INTSTATUS67        (*(RoReg8 *)0x4101E063UL) /**< \brief (TAL) Interrupt 67 Status */
#define REG_TAL_INTSTATUS68        (*(RoReg8 *)0x4101E064UL) /**< \brief (TAL) Interrupt 68 Status */
#define REG_TAL_INTSTATUS69        (*(RoReg8 *)0x4101E065UL) /**< \brief (TAL) Interrupt 69 Status */
#define REG_TAL_INTSTATUS70        (*(RoReg8 *)0x4101E066UL) /**< \brief (TAL) Interrupt 70 Status */
#define REG_TAL_INTSTATUS71        (*(RoReg8 *)0x4101E067UL) /**< \brief (TAL) Interrupt 71 Status */
#define REG_TAL_INTSTATUS72        (*(RoReg8 *)0x4101E068UL) /**< \brief (TAL) Interrupt 72 Status */
#define REG_TAL_INTSTATUS73        (*(RoReg8 *)0x4101E069UL) /**< \brief (TAL) Interrupt 73 Status */
#define REG_TAL_INTSTATUS74        (*(RoReg8 *)0x4101E06AUL) /**< \brief (TAL) Interrupt 74 Status */
#define REG_TAL_INTSTATUS75        (*(RoReg8 *)0x4101E06BUL) /**< \brief (TAL) Interrupt 75 Status */
#define REG_TAL_INTSTATUS76        (*(RoReg8 *)0x4101E06CUL) /**< \brief (TAL) Interrupt 76 Status */
#define REG_TAL_INTSTATUS77        (*(RoReg8 *)0x4101E06DUL) /**< \brief (TAL) Interrupt 77 Status */
#define REG_TAL_INTSTATUS78        (*(RoReg8 *)0x4101E06EUL) /**< \brief (TAL) Interrupt 78 Status */
#define REG_TAL_INTSTATUS79        (*(RoReg8 *)0x4101E06FUL) /**< \brief (TAL) Interrupt 79 Status */
#define REG_TAL_INTSTATUS80        (*(RoReg8 *)0x4101E070UL) /**< \brief (TAL) Interrupt 80 Status */
#define REG_TAL_INTSTATUS81        (*(RoReg8 *)0x4101E071UL) /**< \brief (TAL) Interrupt 81 Status */
#define REG_TAL_INTSTATUS82        (*(RoReg8 *)0x4101E072UL) /**< \brief (TAL) Interrupt 82 Status */
#define REG_TAL_INTSTATUS83        (*(RoReg8 *)0x4101E073UL) /**< \brief (TAL) Interrupt 83 Status */
#define REG_TAL_INTSTATUS84        (*(RoReg8 *)0x4101E074UL) /**< \brief (TAL) Interrupt 84 Status */
#define REG_TAL_INTSTATUS85        (*(RoReg8 *)0x4101E075UL) /**< \brief (TAL) Interrupt 85 Status */
#define REG_TAL_INTSTATUS86        (*(RoReg8 *)0x4101E076UL) /**< \brief (TAL) Interrupt 86 Status */
#define REG_TAL_INTSTATUS87        (*(RoReg8 *)0x4101E077UL) /**< \brief (TAL) Interrupt 87 Status */
#define REG_TAL_INTSTATUS88        (*(RoReg8 *)0x4101E078UL) /**< \brief (TAL) Interrupt 88 Status */
#define REG_TAL_INTSTATUS89        (*(RoReg8 *)0x4101E079UL) /**< \brief (TAL) Interrupt 89 Status */
#define REG_TAL_INTSTATUS90        (*(RoReg8 *)0x4101E07AUL) /**< \brief (TAL) Interrupt 90 Status */
#define REG_TAL_INTSTATUS91        (*(RoReg8 *)0x4101E07BUL) /**< \brief (TAL) Interrupt 91 Status */
#define REG_TAL_INTSTATUS92        (*(RoReg8 *)0x4101E07CUL) /**< \brief (TAL) Interrupt 92 Status */
#define REG_TAL_INTSTATUS93        (*(RoReg8 *)0x4101E07DUL) /**< \brief (TAL) Interrupt 93 Status */
#define REG_TAL_INTSTATUS94        (*(RoReg8 *)0x4101E07EUL) /**< \brief (TAL) Interrupt 94 Status */
#define REG_TAL_INTSTATUS95        (*(RoReg8 *)0x4101E07FUL) /**< \brief (TAL) Interrupt 95 Status */
#define REG_TAL_INTSTATUS96        (*(RoReg8 *)0x4101E080UL) /**< \brief (TAL) Interrupt 96 Status */
#define REG_TAL_INTSTATUS97        (*(RoReg8 *)0x4101E081UL) /**< \brief (TAL) Interrupt 97 Status */
#define REG_TAL_INTSTATUS98        (*(RoReg8 *)0x4101E082UL) /**< \brief (TAL) Interrupt 98 Status */
#define REG_TAL_INTSTATUS99        (*(RoReg8 *)0x4101E083UL) /**< \brief (TAL) Interrupt 99 Status */
#define REG_TAL_INTSTATUS100       (*(RoReg8 *)0x4101E084UL) /**< \brief (TAL) Interrupt 100 Status */
#define REG_TAL_INTSTATUS101       (*(RoReg8 *)0x4101E085UL) /**< \brief (TAL) Interrupt 101 Status */
#define REG_TAL_INTSTATUS102       (*(RoReg8 *)0x4101E086UL) /**< \brief (TAL) Interrupt 102 Status */
#define REG_TAL_INTSTATUS103       (*(RoReg8 *)0x4101E087UL) /**< \brief (TAL) Interrupt 103 Status */
#define REG_TAL_INTSTATUS104       (*(RoReg8 *)0x4101E088UL) /**< \brief (TAL) Interrupt 104 Status */
#define REG_TAL_INTSTATUS105       (*(RoReg8 *)0x4101E089UL) /**< \brief (TAL) Interrupt 105 Status */
#define REG_TAL_INTSTATUS106       (*(RoReg8 *)0x4101E08AUL) /**< \brief (TAL) Interrupt 106 Status */
#define REG_TAL_INTSTATUS107       (*(RoReg8 *)0x4101E08BUL) /**< \brief (TAL) Interrupt 107 Status */
#define REG_TAL_INTSTATUS108       (*(RoReg8 *)0x4101E08CUL) /**< \brief (TAL) Interrupt 108 Status */
#define REG_TAL_INTSTATUS109       (*(RoReg8 *)0x4101E08DUL) /**< \brief (TAL) Interrupt 109 Status */
#define REG_TAL_INTSTATUS110       (*(RoReg8 *)0x4101E08EUL) /**< \brief (TAL) Interrupt 110 Status */
#define REG_TAL_INTSTATUS111       (*(RoReg8 *)0x4101E08FUL) /**< \brief (TAL) Interrupt 111 Status */
#define REG_TAL_INTSTATUS112       (*(RoReg8 *)0x4101E090UL) /**< \brief (TAL) Interrupt 112 Status */
#define REG_TAL_INTSTATUS113       (*(RoReg8 *)0x4101E091UL) /**< \brief (TAL) Interrupt 113 Status */
#define REG_TAL_INTSTATUS114       (*(RoReg8 *)0x4101E092UL) /**< \brief (TAL) Interrupt 114 Status */
#define REG_TAL_INTSTATUS115       (*(RoReg8 *)0x4101E093UL) /**< \brief (TAL) Interrupt 115 Status */
#define REG_TAL_INTSTATUS116       (*(RoReg8 *)0x4101E094UL) /**< \brief (TAL) Interrupt 116 Status */
#define REG_TAL_INTSTATUS117       (*(RoReg8 *)0x4101E095UL) /**< \brief (TAL) Interrupt 117 Status */
#define REG_TAL_INTSTATUS118       (*(RoReg8 *)0x4101E096UL) /**< \brief (TAL) Interrupt 118 Status */
#define REG_TAL_INTSTATUS119       (*(RoReg8 *)0x4101E097UL) /**< \brief (TAL) Interrupt 119 Status */
#define REG_TAL_INTSTATUS120       (*(RoReg8 *)0x4101E098UL) /**< \brief (TAL) Interrupt 120 Status */
#define REG_TAL_INTSTATUS121       (*(RoReg8 *)0x4101E099UL) /**< \brief (TAL) Interrupt 121 Status */
#define REG_TAL_INTSTATUS122       (*(RoReg8 *)0x4101E09AUL) /**< \brief (TAL) Interrupt 122 Status */
#define REG_TAL_INTSTATUS123       (*(RoReg8 *)0x4101E09BUL) /**< \brief (TAL) Interrupt 123 Status */
#define REG_TAL_INTSTATUS124       (*(RoReg8 *)0x4101E09CUL) /**< \brief (TAL) Interrupt 124 Status */
#define REG_TAL_INTSTATUS125       (*(RoReg8 *)0x4101E09DUL) /**< \brief (TAL) Interrupt 125 Status */
#define REG_TAL_INTSTATUS126       (*(RoReg8 *)0x4101E09EUL) /**< \brief (TAL) Interrupt 126 Status */
#define REG_TAL_INTSTATUS127       (*(RoReg8 *)0x4101E09FUL) /**< \brief (TAL) Interrupt 127 Status */
#define REG_TAL_INTSTATUS128       (*(RoReg8 *)0x4101E0A0UL) /**< \brief (TAL) Interrupt 128 Status */
#define REG_TAL_INTSTATUS129       (*(RoReg8 *)0x4101E0A1UL) /**< \brief (TAL) Interrupt 129 Status */
#define REG_TAL_INTSTATUS130       (*(RoReg8 *)0x4101E0A2UL) /**< \brief (TAL) Interrupt 130 Status */
#define REG_TAL_INTSTATUS131       (*(RoReg8 *)0x4101E0A3UL) /**< \brief (TAL) Interrupt 131 Status */
#define REG_TAL_INTSTATUS132       (*(RoReg8 *)0x4101E0A4UL) /**< \brief (TAL) Interrupt 132 Status */
#define REG_TAL_INTSTATUS133       (*(RoReg8 *)0x4101E0A5UL) /**< \brief (TAL) Interrupt 133 Status */
#define REG_TAL_INTSTATUS134       (*(RoReg8 *)0x4101E0A6UL) /**< \brief (TAL) Interrupt 134 Status */
#define REG_TAL_INTSTATUS135       (*(RoReg8 *)0x4101E0A7UL) /**< \brief (TAL) Interrupt 135 Status */
#define REG_TAL_INTSTATUS136       (*(RoReg8 *)0x4101E0A8UL) /**< \brief (TAL) Interrupt 136 Status */
#define REG_TAL_DMACPUSEL0         (*(RwReg  *)0x4101E110UL) /**< \brief (TAL) DMA Channel Interrupts CPU Select 0 */
#define REG_TAL_DMACPUSEL1         (*(RwReg  *)0x4101E114UL) /**< \brief (TAL) DMA Channel Interrupts CPU Select 1 */
#define REG_TAL_EVCPUSEL0          (*(RwReg  *)0x4101E118UL) /**< \brief (TAL) EVSYS Channel Interrupts CPU Select 0 */
#define REG_TAL_EICCPUSEL0         (*(RwReg  *)0x4101E120UL) /**< \brief (TAL) EIC External Interrupts CPU Select 0 */
#define REG_TAL_INTCPUSEL0         (*(RwReg  *)0x4101E128UL) /**< \brief (TAL) Interrupts CPU Select 0 */
#define REG_TAL_INTCPUSEL1         (*(RwReg  *)0x4101E12CUL) /**< \brief (TAL) Interrupts CPU Select 1 */
#define REG_TAL_INTCPUSEL2         (*(RwReg  *)0x4101E130UL) /**< \brief (TAL) Interrupts CPU Select 2 */
#define REG_TAL_INTCPUSEL3         (*(RwReg  *)0x4101E134UL) /**< \brief (TAL) Interrupts CPU Select 3 */
#define REG_TAL_INTCPUSEL4         (*(RwReg  *)0x4101E138UL) /**< \brief (TAL) Interrupts CPU Select 4 */
#define REG_TAL_INTCPUSEL5         (*(RwReg  *)0x4101E13CUL) /**< \brief (TAL) Interrupts CPU Select 5 */
#define REG_TAL_INTCPUSEL6         (*(RwReg  *)0x4101E140UL) /**< \brief (TAL) Interrupts CPU Select 6 */
#define REG_TAL_INTCPUSEL7         (*(RwReg  *)0x4101E144UL) /**< \brief (TAL) Interrupts CPU Select 7 */
#define REG_TAL_INTCPUSEL8         (*(RwReg  *)0x4101E148UL) /**< \brief (TAL) Interrupts CPU Select 8 */
#define REG_TAL_IRQTRIG            (*(RwReg  *)0x4101E164UL) /**< \brief (TAL) Interrupt Trigger */
#define REG_TAL_IRQMON0            (*(RwReg16*)0x4101E168UL) /**< \brief (TAL) Interrupt Monitor Select 0 */
#define REG_TAL_CPUIRQS0           (*(RoReg  *)0x4101E180UL) /**< \brief (TAL) Interrupt Status m for CPU 0 */
#define REG_TAL_CPUIRQS1           (*(RoReg  *)0x4101E1A0UL) /**< \brief (TAL) Interrupt Status m for CPU 1 */
#define REG_TAL_SMASK0             (*(RwReg  *)0x4101E200UL) /**< \brief (TAL) Inter-Process Signal Mask m for CPU 0 */
#define REG_TAL_SMASK1             (*(RwReg  *)0x4101E208UL) /**< \brief (TAL) Inter-Process Signal Mask m for CPU 1 */
#define REG_TAL_SFLAGCLR0          (*(WoReg  *)0x4101E220UL) /**< \brief (TAL) Inter-Process Signal Flag Clear 0 */
#define REG_TAL_SFLAGCLR1          (*(WoReg  *)0x4101E224UL) /**< \brief (TAL) Inter-Process Signal Flag Clear 1 */
#define REG_TAL_SFLAGSET0          (*(WoReg  *)0x4101E228UL) /**< \brief (TAL) Inter-Process Signal Flag Set 0 */
#define REG_TAL_SFLAGSET1          (*(WoReg  *)0x4101E22CUL) /**< \brief (TAL) Inter-Process Signal Flag Set 1 */
#define REG_TAL_SFLAG0             (*(RoReg  *)0x4101E230UL) /**< \brief (TAL) Inter-Process Signal Flag 0 */
#define REG_TAL_SFLAG1             (*(RoReg  *)0x4101E234UL) /**< \brief (TAL) Inter-Process Signal Flag 1 */
#define REG_TAL_SFLAGCLRR0         (*(RwReg8 *)0x4101E300UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 0 */
#define REG_TAL_SFLAGCLRR1         (*(RwReg8 *)0x4101E301UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 1 */
#define REG_TAL_SFLAGCLRR2         (*(RwReg8 *)0x4101E302UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 2 */
#define REG_TAL_SFLAGCLRR3         (*(RwReg8 *)0x4101E303UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 3 */
#define REG_TAL_SFLAGCLRR4         (*(RwReg8 *)0x4101E304UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 4 */
#define REG_TAL_SFLAGCLRR5         (*(RwReg8 *)0x4101E305UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 5 */
#define REG_TAL_SFLAGCLRR6         (*(RwReg8 *)0x4101E306UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 6 */
#define REG_TAL_SFLAGCLRR7         (*(RwReg8 *)0x4101E307UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 7 */
#define REG_TAL_SFLAGCLRR8         (*(RwReg8 *)0x4101E308UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 8 */
#define REG_TAL_SFLAGCLRR9         (*(RwReg8 *)0x4101E309UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 9 */
#define REG_TAL_SFLAGCLRR10        (*(RwReg8 *)0x4101E30AUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 10 */
#define REG_TAL_SFLAGCLRR11        (*(RwReg8 *)0x4101E30BUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 11 */
#define REG_TAL_SFLAGCLRR12        (*(RwReg8 *)0x4101E30CUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 12 */
#define REG_TAL_SFLAGCLRR13        (*(RwReg8 *)0x4101E30DUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 13 */
#define REG_TAL_SFLAGCLRR14        (*(RwReg8 *)0x4101E30EUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 14 */
#define REG_TAL_SFLAGCLRR15        (*(RwReg8 *)0x4101E30FUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 15 */
#define REG_TAL_SFLAGCLRR16        (*(RwReg8 *)0x4101E310UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 16 */
#define REG_TAL_SFLAGCLRR17        (*(RwReg8 *)0x4101E311UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 17 */
#define REG_TAL_SFLAGCLRR18        (*(RwReg8 *)0x4101E312UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 18 */
#define REG_TAL_SFLAGCLRR19        (*(RwReg8 *)0x4101E313UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 19 */
#define REG_TAL_SFLAGCLRR20        (*(RwReg8 *)0x4101E314UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 20 */
#define REG_TAL_SFLAGCLRR21        (*(RwReg8 *)0x4101E315UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 21 */
#define REG_TAL_SFLAGCLRR22        (*(RwReg8 *)0x4101E316UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 22 */
#define REG_TAL_SFLAGCLRR23        (*(RwReg8 *)0x4101E317UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 23 */
#define REG_TAL_SFLAGCLRR24        (*(RwReg8 *)0x4101E318UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 24 */
#define REG_TAL_SFLAGCLRR25        (*(RwReg8 *)0x4101E319UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 25 */
#define REG_TAL_SFLAGCLRR26        (*(RwReg8 *)0x4101E31AUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 26 */
#define REG_TAL_SFLAGCLRR27        (*(RwReg8 *)0x4101E31BUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 27 */
#define REG_TAL_SFLAGCLRR28        (*(RwReg8 *)0x4101E31CUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 28 */
#define REG_TAL_SFLAGCLRR29        (*(RwReg8 *)0x4101E31DUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 29 */
#define REG_TAL_SFLAGCLRR30        (*(RwReg8 *)0x4101E31EUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 30 */
#define REG_TAL_SFLAGCLRR31        (*(RwReg8 *)0x4101E31FUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 31 */
#define REG_TAL_SFLAGCLRR32        (*(RwReg8 *)0x4101E320UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 32 */
#define REG_TAL_SFLAGCLRR33        (*(RwReg8 *)0x4101E321UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 33 */
#define REG_TAL_SFLAGCLRR34        (*(RwReg8 *)0x4101E322UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 34 */
#define REG_TAL_SFLAGCLRR35        (*(RwReg8 *)0x4101E323UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 35 */
#define REG_TAL_SFLAGCLRR36        (*(RwReg8 *)0x4101E324UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 36 */
#define REG_TAL_SFLAGCLRR37        (*(RwReg8 *)0x4101E325UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 37 */
#define REG_TAL_SFLAGCLRR38        (*(RwReg8 *)0x4101E326UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 38 */
#define REG_TAL_SFLAGCLRR39        (*(RwReg8 *)0x4101E327UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 39 */
#define REG_TAL_SFLAGCLRR40        (*(RwReg8 *)0x4101E328UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 40 */
#define REG_TAL_SFLAGCLRR41        (*(RwReg8 *)0x4101E329UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 41 */
#define REG_TAL_SFLAGCLRR42        (*(RwReg8 *)0x4101E32AUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 42 */
#define REG_TAL_SFLAGCLRR43        (*(RwReg8 *)0x4101E32BUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 43 */
#define REG_TAL_SFLAGCLRR44        (*(RwReg8 *)0x4101E32CUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 44 */
#define REG_TAL_SFLAGCLRR45        (*(RwReg8 *)0x4101E32DUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 45 */
#define REG_TAL_SFLAGCLRR46        (*(RwReg8 *)0x4101E32EUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 46 */
#define REG_TAL_SFLAGCLRR47        (*(RwReg8 *)0x4101E32FUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 47 */
#define REG_TAL_SFLAGCLRR48        (*(RwReg8 *)0x4101E330UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 48 */
#define REG_TAL_SFLAGCLRR49        (*(RwReg8 *)0x4101E331UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 49 */
#define REG_TAL_SFLAGCLRR50        (*(RwReg8 *)0x4101E332UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 50 */
#define REG_TAL_SFLAGCLRR51        (*(RwReg8 *)0x4101E333UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 51 */
#define REG_TAL_SFLAGCLRR52        (*(RwReg8 *)0x4101E334UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 52 */
#define REG_TAL_SFLAGCLRR53        (*(RwReg8 *)0x4101E335UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 53 */
#define REG_TAL_SFLAGCLRR54        (*(RwReg8 *)0x4101E336UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 54 */
#define REG_TAL_SFLAGCLRR55        (*(RwReg8 *)0x4101E337UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 55 */
#define REG_TAL_SFLAGCLRR56        (*(RwReg8 *)0x4101E338UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 56 */
#define REG_TAL_SFLAGCLRR57        (*(RwReg8 *)0x4101E339UL) /**< \brief (TAL) Inter-Process Signal Flag Bit 57 */
#define REG_TAL_SFLAGCLRR58        (*(RwReg8 *)0x4101E33AUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 58 */
#define REG_TAL_SFLAGCLRR59        (*(RwReg8 *)0x4101E33BUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 59 */
#define REG_TAL_SFLAGCLRR60        (*(RwReg8 *)0x4101E33CUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 60 */
#define REG_TAL_SFLAGCLRR61        (*(RwReg8 *)0x4101E33DUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 61 */
#define REG_TAL_SFLAGCLRR62        (*(RwReg8 *)0x4101E33EUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 62 */
#define REG_TAL_SFLAGCLRR63        (*(RwReg8 *)0x4101E33FUL) /**< \brief (TAL) Inter-Process Signal Flag Bit 63 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TAL peripheral ========== */
#define TAL_CPU_NUM                 2        // Number of CPUs
#define TAL_CTI_NUM                 4        // Number of Cross-Trigger Interfaces
#define TAL_DMA_CH_NUM              32       // Number of DMAC Channels
#define TAL_EV_CH_NUM               12       // Number of EVSYS Channels
#define TAL_EXTINT_NUM              16       // Number of EIC External Interrrupts
#define TAL_ID_IN_INTCPUSEL         1        // Use ID of IP instances in INTCPUSEL registers
#define TAL_ID_NUM                  130      // Number of IDs for IP instance numbers
#define TAL_INT_GRP_NUM             5        // Number of 32-IRQ Groups
#define TAL_INT_NUM                 137      // Number of Interrupt Requests
#define TAL_INT_NUM_BITS            8        // Number of bits for INT_NUM
#define TAL_IPS_GRP_NUM             2        // Number of 32-IPS Groups
#define TAL_IPS_NUM                 64       // Number of Inter-Process Signals
#define TAL_IRQMON_NUM              1        // Number of IRQ Monitors

#endif /* _SAMD51_TAL_INSTANCE_ */
