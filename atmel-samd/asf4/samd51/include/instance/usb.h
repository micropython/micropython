/**
 * \file
 *
 * \brief Instance description for USB
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

#ifndef _SAMD51_USB_INSTANCE_
#define _SAMD51_USB_INSTANCE_

/* ========== Register definition for USB peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_USB_CTRLA              (0x41000000) /**< \brief (USB) Control A */
#define REG_USB_SYNCBUSY           (0x41000002) /**< \brief (USB) Synchronization Busy */
#define REG_USB_QOSCTRL            (0x41000003) /**< \brief (USB) USB Quality Of Service */
#define REG_USB_FSMSTATUS          (0x4100000D) /**< \brief (USB) Finite State Machine Status */
#define REG_USB_DESCADD            (0x41000024) /**< \brief (USB) Descriptor Address */
#define REG_USB_PADCAL             (0x41000028) /**< \brief (USB) USB PAD Calibration */
#define REG_USB_DEVICE_CTRLB       (0x41000008) /**< \brief (USB) DEVICE Control B */
#define REG_USB_DEVICE_DADD        (0x4100000A) /**< \brief (USB) DEVICE Device Address */
#define REG_USB_DEVICE_STATUS      (0x4100000C) /**< \brief (USB) DEVICE Status */
#define REG_USB_DEVICE_FNUM        (0x41000010) /**< \brief (USB) DEVICE Device Frame Number */
#define REG_USB_DEVICE_INTENCLR    (0x41000014) /**< \brief (USB) DEVICE Device Interrupt Enable Clear */
#define REG_USB_DEVICE_INTENSET    (0x41000018) /**< \brief (USB) DEVICE Device Interrupt Enable Set */
#define REG_USB_DEVICE_INTFLAG     (0x4100001C) /**< \brief (USB) DEVICE Device Interrupt Flag */
#define REG_USB_DEVICE_EPINTSMRY   (0x41000020) /**< \brief (USB) DEVICE End Point Interrupt Summary */
#define REG_USB_DEVICE_ENDPOINT_EPCFG0 (0x41000100) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 0 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR0 (0x41000104) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 0 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET0 (0x41000105) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 0 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS0 (0x41000106) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 0 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG0 (0x41000107) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 0 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR0 (0x41000108) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 0 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET0 (0x41000109) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 0 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG1 (0x41000120) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 1 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR1 (0x41000124) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 1 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET1 (0x41000125) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 1 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS1 (0x41000126) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 1 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG1 (0x41000127) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 1 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR1 (0x41000128) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 1 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET1 (0x41000129) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 1 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG2 (0x41000140) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 2 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR2 (0x41000144) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 2 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET2 (0x41000145) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 2 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS2 (0x41000146) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 2 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG2 (0x41000147) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 2 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR2 (0x41000148) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 2 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET2 (0x41000149) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 2 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG3 (0x41000160) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 3 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR3 (0x41000164) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 3 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET3 (0x41000165) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 3 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS3 (0x41000166) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 3 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG3 (0x41000167) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 3 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR3 (0x41000168) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 3 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET3 (0x41000169) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 3 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG4 (0x41000180) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 4 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR4 (0x41000184) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 4 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET4 (0x41000185) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 4 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS4 (0x41000186) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 4 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG4 (0x41000187) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 4 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR4 (0x41000188) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 4 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET4 (0x41000189) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 4 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG5 (0x410001A0) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 5 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR5 (0x410001A4) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 5 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET5 (0x410001A5) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 5 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS5 (0x410001A6) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 5 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG5 (0x410001A7) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 5 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR5 (0x410001A8) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 5 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET5 (0x410001A9) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 5 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG6 (0x410001C0) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 6 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR6 (0x410001C4) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 6 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET6 (0x410001C5) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 6 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS6 (0x410001C6) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 6 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG6 (0x410001C7) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 6 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR6 (0x410001C8) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 6 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET6 (0x410001C9) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 6 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG7 (0x410001E0) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 7 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR7 (0x410001E4) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 7 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET7 (0x410001E5) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 7 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS7 (0x410001E6) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 7 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG7 (0x410001E7) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 7 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR7 (0x410001E8) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 7 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET7 (0x410001E9) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 7 */
#define REG_USB_HOST_CTRLB         (0x41000008) /**< \brief (USB) HOST Control B */
#define REG_USB_HOST_HSOFC         (0x4100000A) /**< \brief (USB) HOST Host Start Of Frame Control */
#define REG_USB_HOST_STATUS        (0x4100000C) /**< \brief (USB) HOST Status */
#define REG_USB_HOST_FNUM          (0x41000010) /**< \brief (USB) HOST Host Frame Number */
#define REG_USB_HOST_FLENHIGH      (0x41000012) /**< \brief (USB) HOST Host Frame Length */
#define REG_USB_HOST_INTENCLR      (0x41000014) /**< \brief (USB) HOST Host Interrupt Enable Clear */
#define REG_USB_HOST_INTENSET      (0x41000018) /**< \brief (USB) HOST Host Interrupt Enable Set */
#define REG_USB_HOST_INTFLAG       (0x4100001C) /**< \brief (USB) HOST Host Interrupt Flag */
#define REG_USB_HOST_PINTSMRY      (0x41000020) /**< \brief (USB) HOST Pipe Interrupt Summary */
#define REG_USB_HOST_PIPE_PCFG0    (0x41000100) /**< \brief (USB) HOST_PIPE End Point Configuration 0 */
#define REG_USB_HOST_PIPE_BINTERVAL0 (0x41000103) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 0 */
#define REG_USB_HOST_PIPE_PSTATUSCLR0 (0x41000104) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 0 */
#define REG_USB_HOST_PIPE_PSTATUSSET0 (0x41000105) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 0 */
#define REG_USB_HOST_PIPE_PSTATUS0 (0x41000106) /**< \brief (USB) HOST_PIPE End Point Pipe Status 0 */
#define REG_USB_HOST_PIPE_PINTFLAG0 (0x41000107) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 0 */
#define REG_USB_HOST_PIPE_PINTENCLR0 (0x41000108) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 0 */
#define REG_USB_HOST_PIPE_PINTENSET0 (0x41000109) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 0 */
#define REG_USB_HOST_PIPE_PCFG1    (0x41000120) /**< \brief (USB) HOST_PIPE End Point Configuration 1 */
#define REG_USB_HOST_PIPE_BINTERVAL1 (0x41000123) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 1 */
#define REG_USB_HOST_PIPE_PSTATUSCLR1 (0x41000124) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 1 */
#define REG_USB_HOST_PIPE_PSTATUSSET1 (0x41000125) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 1 */
#define REG_USB_HOST_PIPE_PSTATUS1 (0x41000126) /**< \brief (USB) HOST_PIPE End Point Pipe Status 1 */
#define REG_USB_HOST_PIPE_PINTFLAG1 (0x41000127) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 1 */
#define REG_USB_HOST_PIPE_PINTENCLR1 (0x41000128) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 1 */
#define REG_USB_HOST_PIPE_PINTENSET1 (0x41000129) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 1 */
#define REG_USB_HOST_PIPE_PCFG2    (0x41000140) /**< \brief (USB) HOST_PIPE End Point Configuration 2 */
#define REG_USB_HOST_PIPE_BINTERVAL2 (0x41000143) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 2 */
#define REG_USB_HOST_PIPE_PSTATUSCLR2 (0x41000144) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 2 */
#define REG_USB_HOST_PIPE_PSTATUSSET2 (0x41000145) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 2 */
#define REG_USB_HOST_PIPE_PSTATUS2 (0x41000146) /**< \brief (USB) HOST_PIPE End Point Pipe Status 2 */
#define REG_USB_HOST_PIPE_PINTFLAG2 (0x41000147) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 2 */
#define REG_USB_HOST_PIPE_PINTENCLR2 (0x41000148) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 2 */
#define REG_USB_HOST_PIPE_PINTENSET2 (0x41000149) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 2 */
#define REG_USB_HOST_PIPE_PCFG3    (0x41000160) /**< \brief (USB) HOST_PIPE End Point Configuration 3 */
#define REG_USB_HOST_PIPE_BINTERVAL3 (0x41000163) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 3 */
#define REG_USB_HOST_PIPE_PSTATUSCLR3 (0x41000164) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 3 */
#define REG_USB_HOST_PIPE_PSTATUSSET3 (0x41000165) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 3 */
#define REG_USB_HOST_PIPE_PSTATUS3 (0x41000166) /**< \brief (USB) HOST_PIPE End Point Pipe Status 3 */
#define REG_USB_HOST_PIPE_PINTFLAG3 (0x41000167) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 3 */
#define REG_USB_HOST_PIPE_PINTENCLR3 (0x41000168) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 3 */
#define REG_USB_HOST_PIPE_PINTENSET3 (0x41000169) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 3 */
#define REG_USB_HOST_PIPE_PCFG4    (0x41000180) /**< \brief (USB) HOST_PIPE End Point Configuration 4 */
#define REG_USB_HOST_PIPE_BINTERVAL4 (0x41000183) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 4 */
#define REG_USB_HOST_PIPE_PSTATUSCLR4 (0x41000184) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 4 */
#define REG_USB_HOST_PIPE_PSTATUSSET4 (0x41000185) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 4 */
#define REG_USB_HOST_PIPE_PSTATUS4 (0x41000186) /**< \brief (USB) HOST_PIPE End Point Pipe Status 4 */
#define REG_USB_HOST_PIPE_PINTFLAG4 (0x41000187) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 4 */
#define REG_USB_HOST_PIPE_PINTENCLR4 (0x41000188) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 4 */
#define REG_USB_HOST_PIPE_PINTENSET4 (0x41000189) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 4 */
#define REG_USB_HOST_PIPE_PCFG5    (0x410001A0) /**< \brief (USB) HOST_PIPE End Point Configuration 5 */
#define REG_USB_HOST_PIPE_BINTERVAL5 (0x410001A3) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 5 */
#define REG_USB_HOST_PIPE_PSTATUSCLR5 (0x410001A4) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 5 */
#define REG_USB_HOST_PIPE_PSTATUSSET5 (0x410001A5) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 5 */
#define REG_USB_HOST_PIPE_PSTATUS5 (0x410001A6) /**< \brief (USB) HOST_PIPE End Point Pipe Status 5 */
#define REG_USB_HOST_PIPE_PINTFLAG5 (0x410001A7) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 5 */
#define REG_USB_HOST_PIPE_PINTENCLR5 (0x410001A8) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 5 */
#define REG_USB_HOST_PIPE_PINTENSET5 (0x410001A9) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 5 */
#define REG_USB_HOST_PIPE_PCFG6    (0x410001C0) /**< \brief (USB) HOST_PIPE End Point Configuration 6 */
#define REG_USB_HOST_PIPE_BINTERVAL6 (0x410001C3) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 6 */
#define REG_USB_HOST_PIPE_PSTATUSCLR6 (0x410001C4) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 6 */
#define REG_USB_HOST_PIPE_PSTATUSSET6 (0x410001C5) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 6 */
#define REG_USB_HOST_PIPE_PSTATUS6 (0x410001C6) /**< \brief (USB) HOST_PIPE End Point Pipe Status 6 */
#define REG_USB_HOST_PIPE_PINTFLAG6 (0x410001C7) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 6 */
#define REG_USB_HOST_PIPE_PINTENCLR6 (0x410001C8) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 6 */
#define REG_USB_HOST_PIPE_PINTENSET6 (0x410001C9) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 6 */
#define REG_USB_HOST_PIPE_PCFG7    (0x410001E0) /**< \brief (USB) HOST_PIPE End Point Configuration 7 */
#define REG_USB_HOST_PIPE_BINTERVAL7 (0x410001E3) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 7 */
#define REG_USB_HOST_PIPE_PSTATUSCLR7 (0x410001E4) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 7 */
#define REG_USB_HOST_PIPE_PSTATUSSET7 (0x410001E5) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 7 */
#define REG_USB_HOST_PIPE_PSTATUS7 (0x410001E6) /**< \brief (USB) HOST_PIPE End Point Pipe Status 7 */
#define REG_USB_HOST_PIPE_PINTFLAG7 (0x410001E7) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 7 */
#define REG_USB_HOST_PIPE_PINTENCLR7 (0x410001E8) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 7 */
#define REG_USB_HOST_PIPE_PINTENSET7 (0x410001E9) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 7 */
#else
#define REG_USB_CTRLA              (*(RwReg8 *)0x41000000UL) /**< \brief (USB) Control A */
#define REG_USB_SYNCBUSY           (*(RoReg8 *)0x41000002UL) /**< \brief (USB) Synchronization Busy */
#define REG_USB_QOSCTRL            (*(RwReg8 *)0x41000003UL) /**< \brief (USB) USB Quality Of Service */
#define REG_USB_FSMSTATUS          (*(RoReg8 *)0x4100000DUL) /**< \brief (USB) Finite State Machine Status */
#define REG_USB_DESCADD            (*(RwReg  *)0x41000024UL) /**< \brief (USB) Descriptor Address */
#define REG_USB_PADCAL             (*(RwReg16*)0x41000028UL) /**< \brief (USB) USB PAD Calibration */
#define REG_USB_DEVICE_CTRLB       (*(RwReg16*)0x41000008UL) /**< \brief (USB) DEVICE Control B */
#define REG_USB_DEVICE_DADD        (*(RwReg8 *)0x4100000AUL) /**< \brief (USB) DEVICE Device Address */
#define REG_USB_DEVICE_STATUS      (*(RoReg8 *)0x4100000CUL) /**< \brief (USB) DEVICE Status */
#define REG_USB_DEVICE_FNUM        (*(RoReg16*)0x41000010UL) /**< \brief (USB) DEVICE Device Frame Number */
#define REG_USB_DEVICE_INTENCLR    (*(RwReg16*)0x41000014UL) /**< \brief (USB) DEVICE Device Interrupt Enable Clear */
#define REG_USB_DEVICE_INTENSET    (*(RwReg16*)0x41000018UL) /**< \brief (USB) DEVICE Device Interrupt Enable Set */
#define REG_USB_DEVICE_INTFLAG     (*(RwReg16*)0x4100001CUL) /**< \brief (USB) DEVICE Device Interrupt Flag */
#define REG_USB_DEVICE_EPINTSMRY   (*(RoReg16*)0x41000020UL) /**< \brief (USB) DEVICE End Point Interrupt Summary */
#define REG_USB_DEVICE_ENDPOINT_EPCFG0 (*(RwReg8 *)0x41000100UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 0 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR0 (*(WoReg8 *)0x41000104UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 0 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET0 (*(WoReg8 *)0x41000105UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 0 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS0 (*(RoReg8 *)0x41000106UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 0 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG0 (*(RwReg8 *)0x41000107UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 0 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR0 (*(RwReg8 *)0x41000108UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 0 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET0 (*(RwReg8 *)0x41000109UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 0 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG1 (*(RwReg8 *)0x41000120UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 1 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR1 (*(WoReg8 *)0x41000124UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 1 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET1 (*(WoReg8 *)0x41000125UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 1 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS1 (*(RoReg8 *)0x41000126UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 1 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG1 (*(RwReg8 *)0x41000127UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 1 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR1 (*(RwReg8 *)0x41000128UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 1 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET1 (*(RwReg8 *)0x41000129UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 1 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG2 (*(RwReg8 *)0x41000140UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 2 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR2 (*(WoReg8 *)0x41000144UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 2 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET2 (*(WoReg8 *)0x41000145UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 2 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS2 (*(RoReg8 *)0x41000146UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 2 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG2 (*(RwReg8 *)0x41000147UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 2 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR2 (*(RwReg8 *)0x41000148UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 2 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET2 (*(RwReg8 *)0x41000149UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 2 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG3 (*(RwReg8 *)0x41000160UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 3 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR3 (*(WoReg8 *)0x41000164UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 3 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET3 (*(WoReg8 *)0x41000165UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 3 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS3 (*(RoReg8 *)0x41000166UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 3 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG3 (*(RwReg8 *)0x41000167UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 3 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR3 (*(RwReg8 *)0x41000168UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 3 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET3 (*(RwReg8 *)0x41000169UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 3 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG4 (*(RwReg8 *)0x41000180UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 4 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR4 (*(WoReg8 *)0x41000184UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 4 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET4 (*(WoReg8 *)0x41000185UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 4 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS4 (*(RoReg8 *)0x41000186UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 4 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG4 (*(RwReg8 *)0x41000187UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 4 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR4 (*(RwReg8 *)0x41000188UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 4 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET4 (*(RwReg8 *)0x41000189UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 4 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG5 (*(RwReg8 *)0x410001A0UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 5 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR5 (*(WoReg8 *)0x410001A4UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 5 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET5 (*(WoReg8 *)0x410001A5UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 5 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS5 (*(RoReg8 *)0x410001A6UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 5 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG5 (*(RwReg8 *)0x410001A7UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 5 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR5 (*(RwReg8 *)0x410001A8UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 5 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET5 (*(RwReg8 *)0x410001A9UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 5 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG6 (*(RwReg8 *)0x410001C0UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 6 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR6 (*(WoReg8 *)0x410001C4UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 6 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET6 (*(WoReg8 *)0x410001C5UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 6 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS6 (*(RoReg8 *)0x410001C6UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 6 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG6 (*(RwReg8 *)0x410001C7UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 6 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR6 (*(RwReg8 *)0x410001C8UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 6 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET6 (*(RwReg8 *)0x410001C9UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 6 */
#define REG_USB_DEVICE_ENDPOINT_EPCFG7 (*(RwReg8 *)0x410001E0UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Configuration 7 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSCLR7 (*(WoReg8 *)0x410001E4UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Clear 7 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUSSET7 (*(WoReg8 *)0x410001E5UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status Set 7 */
#define REG_USB_DEVICE_ENDPOINT_EPSTATUS7 (*(RoReg8 *)0x410001E6UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Pipe Status 7 */
#define REG_USB_DEVICE_ENDPOINT_EPINTFLAG7 (*(RwReg8 *)0x410001E7UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Flag 7 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENCLR7 (*(RwReg8 *)0x410001E8UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Clear Flag 7 */
#define REG_USB_DEVICE_ENDPOINT_EPINTENSET7 (*(RwReg8 *)0x410001E9UL) /**< \brief (USB) DEVICE_ENDPOINT End Point Interrupt Set Flag 7 */
#define REG_USB_HOST_CTRLB         (*(RwReg16*)0x41000008UL) /**< \brief (USB) HOST Control B */
#define REG_USB_HOST_HSOFC         (*(RwReg8 *)0x4100000AUL) /**< \brief (USB) HOST Host Start Of Frame Control */
#define REG_USB_HOST_STATUS        (*(RwReg8 *)0x4100000CUL) /**< \brief (USB) HOST Status */
#define REG_USB_HOST_FNUM          (*(RwReg16*)0x41000010UL) /**< \brief (USB) HOST Host Frame Number */
#define REG_USB_HOST_FLENHIGH      (*(RoReg8 *)0x41000012UL) /**< \brief (USB) HOST Host Frame Length */
#define REG_USB_HOST_INTENCLR      (*(RwReg16*)0x41000014UL) /**< \brief (USB) HOST Host Interrupt Enable Clear */
#define REG_USB_HOST_INTENSET      (*(RwReg16*)0x41000018UL) /**< \brief (USB) HOST Host Interrupt Enable Set */
#define REG_USB_HOST_INTFLAG       (*(RwReg16*)0x4100001CUL) /**< \brief (USB) HOST Host Interrupt Flag */
#define REG_USB_HOST_PINTSMRY      (*(RoReg16*)0x41000020UL) /**< \brief (USB) HOST Pipe Interrupt Summary */
#define REG_USB_HOST_PIPE_PCFG0    (*(RwReg8 *)0x41000100UL) /**< \brief (USB) HOST_PIPE End Point Configuration 0 */
#define REG_USB_HOST_PIPE_BINTERVAL0 (*(RwReg8 *)0x41000103UL) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 0 */
#define REG_USB_HOST_PIPE_PSTATUSCLR0 (*(WoReg8 *)0x41000104UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 0 */
#define REG_USB_HOST_PIPE_PSTATUSSET0 (*(WoReg8 *)0x41000105UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 0 */
#define REG_USB_HOST_PIPE_PSTATUS0 (*(RoReg8 *)0x41000106UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status 0 */
#define REG_USB_HOST_PIPE_PINTFLAG0 (*(RwReg8 *)0x41000107UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 0 */
#define REG_USB_HOST_PIPE_PINTENCLR0 (*(RwReg8 *)0x41000108UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 0 */
#define REG_USB_HOST_PIPE_PINTENSET0 (*(RwReg8 *)0x41000109UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 0 */
#define REG_USB_HOST_PIPE_PCFG1    (*(RwReg8 *)0x41000120UL) /**< \brief (USB) HOST_PIPE End Point Configuration 1 */
#define REG_USB_HOST_PIPE_BINTERVAL1 (*(RwReg8 *)0x41000123UL) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 1 */
#define REG_USB_HOST_PIPE_PSTATUSCLR1 (*(WoReg8 *)0x41000124UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 1 */
#define REG_USB_HOST_PIPE_PSTATUSSET1 (*(WoReg8 *)0x41000125UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 1 */
#define REG_USB_HOST_PIPE_PSTATUS1 (*(RoReg8 *)0x41000126UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status 1 */
#define REG_USB_HOST_PIPE_PINTFLAG1 (*(RwReg8 *)0x41000127UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 1 */
#define REG_USB_HOST_PIPE_PINTENCLR1 (*(RwReg8 *)0x41000128UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 1 */
#define REG_USB_HOST_PIPE_PINTENSET1 (*(RwReg8 *)0x41000129UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 1 */
#define REG_USB_HOST_PIPE_PCFG2    (*(RwReg8 *)0x41000140UL) /**< \brief (USB) HOST_PIPE End Point Configuration 2 */
#define REG_USB_HOST_PIPE_BINTERVAL2 (*(RwReg8 *)0x41000143UL) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 2 */
#define REG_USB_HOST_PIPE_PSTATUSCLR2 (*(WoReg8 *)0x41000144UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 2 */
#define REG_USB_HOST_PIPE_PSTATUSSET2 (*(WoReg8 *)0x41000145UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 2 */
#define REG_USB_HOST_PIPE_PSTATUS2 (*(RoReg8 *)0x41000146UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status 2 */
#define REG_USB_HOST_PIPE_PINTFLAG2 (*(RwReg8 *)0x41000147UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 2 */
#define REG_USB_HOST_PIPE_PINTENCLR2 (*(RwReg8 *)0x41000148UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 2 */
#define REG_USB_HOST_PIPE_PINTENSET2 (*(RwReg8 *)0x41000149UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 2 */
#define REG_USB_HOST_PIPE_PCFG3    (*(RwReg8 *)0x41000160UL) /**< \brief (USB) HOST_PIPE End Point Configuration 3 */
#define REG_USB_HOST_PIPE_BINTERVAL3 (*(RwReg8 *)0x41000163UL) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 3 */
#define REG_USB_HOST_PIPE_PSTATUSCLR3 (*(WoReg8 *)0x41000164UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 3 */
#define REG_USB_HOST_PIPE_PSTATUSSET3 (*(WoReg8 *)0x41000165UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 3 */
#define REG_USB_HOST_PIPE_PSTATUS3 (*(RoReg8 *)0x41000166UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status 3 */
#define REG_USB_HOST_PIPE_PINTFLAG3 (*(RwReg8 *)0x41000167UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 3 */
#define REG_USB_HOST_PIPE_PINTENCLR3 (*(RwReg8 *)0x41000168UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 3 */
#define REG_USB_HOST_PIPE_PINTENSET3 (*(RwReg8 *)0x41000169UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 3 */
#define REG_USB_HOST_PIPE_PCFG4    (*(RwReg8 *)0x41000180UL) /**< \brief (USB) HOST_PIPE End Point Configuration 4 */
#define REG_USB_HOST_PIPE_BINTERVAL4 (*(RwReg8 *)0x41000183UL) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 4 */
#define REG_USB_HOST_PIPE_PSTATUSCLR4 (*(WoReg8 *)0x41000184UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 4 */
#define REG_USB_HOST_PIPE_PSTATUSSET4 (*(WoReg8 *)0x41000185UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 4 */
#define REG_USB_HOST_PIPE_PSTATUS4 (*(RoReg8 *)0x41000186UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status 4 */
#define REG_USB_HOST_PIPE_PINTFLAG4 (*(RwReg8 *)0x41000187UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 4 */
#define REG_USB_HOST_PIPE_PINTENCLR4 (*(RwReg8 *)0x41000188UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 4 */
#define REG_USB_HOST_PIPE_PINTENSET4 (*(RwReg8 *)0x41000189UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 4 */
#define REG_USB_HOST_PIPE_PCFG5    (*(RwReg8 *)0x410001A0UL) /**< \brief (USB) HOST_PIPE End Point Configuration 5 */
#define REG_USB_HOST_PIPE_BINTERVAL5 (*(RwReg8 *)0x410001A3UL) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 5 */
#define REG_USB_HOST_PIPE_PSTATUSCLR5 (*(WoReg8 *)0x410001A4UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 5 */
#define REG_USB_HOST_PIPE_PSTATUSSET5 (*(WoReg8 *)0x410001A5UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 5 */
#define REG_USB_HOST_PIPE_PSTATUS5 (*(RoReg8 *)0x410001A6UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status 5 */
#define REG_USB_HOST_PIPE_PINTFLAG5 (*(RwReg8 *)0x410001A7UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 5 */
#define REG_USB_HOST_PIPE_PINTENCLR5 (*(RwReg8 *)0x410001A8UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 5 */
#define REG_USB_HOST_PIPE_PINTENSET5 (*(RwReg8 *)0x410001A9UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 5 */
#define REG_USB_HOST_PIPE_PCFG6    (*(RwReg8 *)0x410001C0UL) /**< \brief (USB) HOST_PIPE End Point Configuration 6 */
#define REG_USB_HOST_PIPE_BINTERVAL6 (*(RwReg8 *)0x410001C3UL) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 6 */
#define REG_USB_HOST_PIPE_PSTATUSCLR6 (*(WoReg8 *)0x410001C4UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 6 */
#define REG_USB_HOST_PIPE_PSTATUSSET6 (*(WoReg8 *)0x410001C5UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 6 */
#define REG_USB_HOST_PIPE_PSTATUS6 (*(RoReg8 *)0x410001C6UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status 6 */
#define REG_USB_HOST_PIPE_PINTFLAG6 (*(RwReg8 *)0x410001C7UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 6 */
#define REG_USB_HOST_PIPE_PINTENCLR6 (*(RwReg8 *)0x410001C8UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 6 */
#define REG_USB_HOST_PIPE_PINTENSET6 (*(RwReg8 *)0x410001C9UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 6 */
#define REG_USB_HOST_PIPE_PCFG7    (*(RwReg8 *)0x410001E0UL) /**< \brief (USB) HOST_PIPE End Point Configuration 7 */
#define REG_USB_HOST_PIPE_BINTERVAL7 (*(RwReg8 *)0x410001E3UL) /**< \brief (USB) HOST_PIPE Bus Access Period of Pipe 7 */
#define REG_USB_HOST_PIPE_PSTATUSCLR7 (*(WoReg8 *)0x410001E4UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status Clear 7 */
#define REG_USB_HOST_PIPE_PSTATUSSET7 (*(WoReg8 *)0x410001E5UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status Set 7 */
#define REG_USB_HOST_PIPE_PSTATUS7 (*(RoReg8 *)0x410001E6UL) /**< \brief (USB) HOST_PIPE End Point Pipe Status 7 */
#define REG_USB_HOST_PIPE_PINTFLAG7 (*(RwReg8 *)0x410001E7UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag 7 */
#define REG_USB_HOST_PIPE_PINTENCLR7 (*(RwReg8 *)0x410001E8UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Clear 7 */
#define REG_USB_HOST_PIPE_PINTENSET7 (*(RwReg8 *)0x410001E9UL) /**< \brief (USB) HOST_PIPE Pipe Interrupt Flag Set 7 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for USB peripheral ========== */
#define USB_AHB_2_USB_FIFO_DEPTH    4        // bytes number, should be at least 2, and 2^n (4,8,16 ...)
#define USB_AHB_2_USB_RD_DATA_BITS  8        // 8, 16 or 32, here : 8-bits is required as UTMI interface should work in 8-bits mode
#define USB_AHB_2_USB_WR_DATA_BITS  32       // 8, 16 or 32 : here, AHB transfer is made in word mode
#define USB_AHB_2_USB_WR_THRESHOLD  2        // as soon as there are N bytes-free inside the fifo, ahb read transfer is requested
#define USB_DATA_BUS_16_8           0        // UTMI/SIE data bus size : 0 -> 8 bits, 1 -> 16 bits
#define USB_EPNUM                   8        // parameter for rtl : max of ENDPOINT and PIPE NUM
#define USB_EPT_NUM                 8        // Number of USB end points
#define USB_GCLK_ID                 10       // Index of Generic Clock
#define USB_INITIAL_CONTROL_QOS     3        // CONTROL QOS RESET value
#define USB_INITIAL_DATA_QOS        3        // DATA QOS RESET value
#define USB_MISSING_SOF_DET_IMPLEMENTED 1        // 48 mHz xPLL feature implemented
#define USB_PIPE_NUM                8        // Number of USB pipes
#define USB_SYSTEM_CLOCK_IS_CKUSB   0        // Dual (1'b0) or Single (1'b1) clock system
#define USB_USB_2_AHB_FIFO_DEPTH    4        // bytes number, should be at least 2, and 2^n (4,8,16 ...)
#define USB_USB_2_AHB_RD_DATA_BITS  16       // 8, 16 or 32, here : 8-bits is required as UTMI interface should work in 8-bits mode
#define USB_USB_2_AHB_RD_THRESHOLD  2        // as soon as there are 16 bytes-free inside the fifo, ahb read transfer is requested
#define USB_USB_2_AHB_WR_DATA_BITS  8        // 8, 16 or 32 : here : 8-bits is required as UTMI interface should work in 8-bits mode

#endif /* _SAMD51_USB_INSTANCE_ */
