/* Copyright (c) 2016, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   * Neither the name of Nordic Semiconductor ASA nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
 
#ifndef NRF51_TO_NRF52840_H
#define NRF51_TO_NRF52840_H

/*lint ++flb "Enter library region */

/* This file is given to prevent your SW from not compiling with the name changes between nRF51 and nRF52840 devices.
 * It redefines the old nRF51 names into the new ones as long as the functionality is still supported. If the
 * functionality is gone, there old names are not defined, so compilation will fail. Note that also includes macros
 * from the nrf51_deprecated.h file. */


/* IRQ */
/* Several peripherals have been added to several indexes. Names of IRQ handlers and IRQ numbers have changed. */
#define UART0_IRQHandler        UARTE0_UART0_IRQHandler
#define SPI0_TWI0_IRQHandler    SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler
#define SPI1_TWI1_IRQHandler    SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler
#define ADC_IRQHandler          SAADC_IRQHandler
#define LPCOMP_IRQHandler       COMP_LPCOMP_IRQHandler
#define SWI0_IRQHandler         SWI0_EGU0_IRQHandler
#define SWI1_IRQHandler         SWI1_EGU1_IRQHandler
#define SWI2_IRQHandler         SWI2_EGU2_IRQHandler
#define SWI3_IRQHandler         SWI3_EGU3_IRQHandler
#define SWI4_IRQHandler         SWI4_EGU4_IRQHandler
#define SWI5_IRQHandler         SWI5_EGU5_IRQHandler

#define UART0_IRQn              UARTE0_UART0_IRQn
#define SPI0_TWI0_IRQn          SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn
#define SPI1_TWI1_IRQn          SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn
#define ADC_IRQn                SAADC_IRQn
#define LPCOMP_IRQn             COMP_LPCOMP_IRQn
#define SWI0_IRQn               SWI0_EGU0_IRQn
#define SWI1_IRQn               SWI1_EGU1_IRQn
#define SWI2_IRQn               SWI2_EGU2_IRQn
#define SWI3_IRQn               SWI3_EGU3_IRQn
#define SWI4_IRQn               SWI4_EGU4_IRQn
#define SWI5_IRQn               SWI5_EGU5_IRQn


/* UICR */
/* Register RBPCONF was renamed to APPROTECT. */
#define RBPCONF     APPROTECT

#define UICR_RBPCONF_PALL_Pos           UICR_APPROTECT_PALL_Pos
#define UICR_RBPCONF_PALL_Msk           UICR_APPROTECT_PALL_Msk
#define UICR_RBPCONF_PALL_Enabled       UICR_APPROTECT_PALL_Enabled
#define UICR_RBPCONF_PALL_Disabled      UICR_APPROTECT_PALL_Disabled


/* GPIO */
/* GPIO port was renamed to P0. */
#define NRF_GPIO        NRF_P0
#define NRF_GPIO_BASE   NRF_P0_BASE


/* QDEC */
/* The registers PSELA, PSELB and PSELLED were restructured into a struct. */
#define PSELLED     PSEL.LED
#define PSELA       PSEL.A
#define PSELB       PSEL.B


/* SPIS */
/* The registers PSELSCK, PSELMISO, PSELMOSI, PSELCSN were restructured into a struct. */
#define PSELSCK       PSEL.SCK
#define PSELMISO      PSEL.MISO
#define PSELMOSI      PSEL.MOSI
#define PSELCSN       PSEL.CSN

/* The registers RXDPTR, MAXRX, AMOUNTRX were restructured into a struct */
#define RXDPTR        RXD.PTR
#define MAXRX         RXD.MAXCNT
#define AMOUNTRX      RXD.AMOUNT

#define SPIS_MAXRX_MAXRX_Pos        SPIS_RXD_MAXCNT_MAXCNT_Pos
#define SPIS_MAXRX_MAXRX_Msk        SPIS_RXD_MAXCNT_MAXCNT_Msk

#define SPIS_AMOUNTRX_AMOUNTRX_Pos  SPIS_RXD_AMOUNT_AMOUNT_Pos
#define SPIS_AMOUNTRX_AMOUNTRX_Msk  SPIS_RXD_AMOUNT_AMOUNT_Msk

/* The registers TXDPTR, MAXTX, AMOUNTTX were restructured into a struct */
#define TXDPTR        TXD.PTR
#define MAXTX         TXD.MAXCNT
#define AMOUNTTX      TXD.AMOUNT

#define SPIS_MAXTX_MAXTX_Pos        SPIS_TXD_MAXCNT_MAXCNT_Pos
#define SPIS_MAXTX_MAXTX_Msk        SPIS_TXD_MAXCNT_MAXCNT_Msk

#define SPIS_AMOUNTTX_AMOUNTTX_Pos  SPIS_TXD_AMOUNT_AMOUNT_Pos
#define SPIS_AMOUNTTX_AMOUNTTX_Msk  SPIS_TXD_AMOUNT_AMOUNT_Msk


/* UART */
/* The registers PSELRTS, PSELTXD, PSELCTS, PSELRXD were restructured into a struct. */
#define PSELRTS       PSEL.RTS
#define PSELTXD       PSEL.TXD
#define PSELCTS       PSEL.CTS
#define PSELRXD       PSEL.RXD

/* TWI */
/* The registers PSELSCL, PSELSDA were restructured into a struct. */
#define PSELSCL       PSEL.SCL
#define PSELSDA       PSEL.SDA



/* From nrf51_deprecated.h */

/* NVMC */
/* The register ERASEPROTECTEDPAGE changed name to ERASEPCR0 in the documentation. */
#define ERASEPROTECTEDPAGE      ERASEPCR0


/* IRQ */
/* COMP module was eliminated. Adapted to nrf52840 headers. */
#define LPCOMP_COMP_IRQHandler  COMP_LPCOMP_IRQHandler
#define LPCOMP_COMP_IRQn        COMP_LPCOMP_IRQn


/* REFSEL register redefined enumerated values and added some more. */
#define LPCOMP_REFSEL_REFSEL_SupplyOneEighthPrescaling          LPCOMP_REFSEL_REFSEL_Ref1_8Vdd
#define LPCOMP_REFSEL_REFSEL_SupplyTwoEighthsPrescaling         LPCOMP_REFSEL_REFSEL_Ref2_8Vdd
#define LPCOMP_REFSEL_REFSEL_SupplyThreeEighthsPrescaling       LPCOMP_REFSEL_REFSEL_Ref3_8Vdd
#define LPCOMP_REFSEL_REFSEL_SupplyFourEighthsPrescaling        LPCOMP_REFSEL_REFSEL_Ref4_8Vdd
#define LPCOMP_REFSEL_REFSEL_SupplyFiveEighthsPrescaling        LPCOMP_REFSEL_REFSEL_Ref5_8Vdd
#define LPCOMP_REFSEL_REFSEL_SupplySixEighthsPrescaling         LPCOMP_REFSEL_REFSEL_Ref6_8Vdd
#define LPCOMP_REFSEL_REFSEL_SupplySevenEighthsPrescaling       LPCOMP_REFSEL_REFSEL_Ref7_8Vdd


/* RADIO */
/* The name of the field SKIPADDR was corrected. Old macros added for compatibility. */
#define RADIO_CRCCNF_SKIP_ADDR_Pos      RADIO_CRCCNF_SKIPADDR_Pos
#define RADIO_CRCCNF_SKIP_ADDR_Msk      RADIO_CRCCNF_SKIPADDR_Msk
#define RADIO_CRCCNF_SKIP_ADDR_Include  RADIO_CRCCNF_SKIPADDR_Include
#define RADIO_CRCCNF_SKIP_ADDR_Skip     RADIO_CRCCNF_SKIPADDR_Skip


/* FICR */
/* The registers FICR.DEVICEID0 and FICR.DEVICEID1 were renamed into an array. */
#define DEVICEID0       DEVICEID[0]
#define DEVICEID1       DEVICEID[1]

/* The registers FICR.ER0, FICR.ER1, FICR.ER2 and FICR.ER3 were renamed into an array. */
#define ER0             ER[0]
#define ER1             ER[1]
#define ER2             ER[2]
#define ER3             ER[3]

/* The registers FICR.IR0, FICR.IR1, FICR.IR2 and FICR.IR3 were renamed into an array. */
#define IR0             IR[0]
#define IR1             IR[1]
#define IR2             IR[2]
#define IR3             IR[3]

/* The registers FICR.DEVICEADDR0 and FICR.DEVICEADDR1 were renamed into an array. */
#define DEVICEADDR0     DEVICEADDR[0]
#define DEVICEADDR1     DEVICEADDR[1]


/* PPI */
/* The tasks PPI.TASKS_CHGxEN and PPI.TASKS_CHGxDIS were renamed into an array of structs. */
#define TASKS_CHG0EN     TASKS_CHG[0].EN
#define TASKS_CHG0DIS    TASKS_CHG[0].DIS
#define TASKS_CHG1EN     TASKS_CHG[1].EN
#define TASKS_CHG1DIS    TASKS_CHG[1].DIS
#define TASKS_CHG2EN     TASKS_CHG[2].EN
#define TASKS_CHG2DIS    TASKS_CHG[2].DIS
#define TASKS_CHG3EN     TASKS_CHG[3].EN
#define TASKS_CHG3DIS    TASKS_CHG[3].DIS

/* The registers PPI.CHx_EEP and PPI.CHx_TEP were renamed into an array of structs. */
#define CH0_EEP          CH[0].EEP
#define CH0_TEP          CH[0].TEP
#define CH1_EEP          CH[1].EEP
#define CH1_TEP          CH[1].TEP
#define CH2_EEP          CH[2].EEP
#define CH2_TEP          CH[2].TEP
#define CH3_EEP          CH[3].EEP
#define CH3_TEP          CH[3].TEP
#define CH4_EEP          CH[4].EEP
#define CH4_TEP          CH[4].TEP
#define CH5_EEP          CH[5].EEP
#define CH5_TEP          CH[5].TEP
#define CH6_EEP          CH[6].EEP
#define CH6_TEP          CH[6].TEP
#define CH7_EEP          CH[7].EEP
#define CH7_TEP          CH[7].TEP
#define CH8_EEP          CH[8].EEP
#define CH8_TEP          CH[8].TEP
#define CH9_EEP          CH[9].EEP
#define CH9_TEP          CH[9].TEP
#define CH10_EEP         CH[10].EEP
#define CH10_TEP         CH[10].TEP
#define CH11_EEP         CH[11].EEP
#define CH11_TEP         CH[11].TEP
#define CH12_EEP         CH[12].EEP
#define CH12_TEP         CH[12].TEP
#define CH13_EEP         CH[13].EEP
#define CH13_TEP         CH[13].TEP
#define CH14_EEP         CH[14].EEP
#define CH14_TEP         CH[14].TEP
#define CH15_EEP         CH[15].EEP
#define CH15_TEP         CH[15].TEP

/* The registers PPI.CHG0, PPI.CHG1, PPI.CHG2 and PPI.CHG3 were renamed into an array. */
#define CHG0             CHG[0]
#define CHG1             CHG[1]
#define CHG2             CHG[2]
#define CHG3             CHG[3]

/* All bitfield macros for the CHGx registers therefore changed name. */
#define PPI_CHG0_CH15_Pos       PPI_CHG_CH15_Pos
#define PPI_CHG0_CH15_Msk       PPI_CHG_CH15_Msk
#define PPI_CHG0_CH15_Excluded  PPI_CHG_CH15_Excluded
#define PPI_CHG0_CH15_Included  PPI_CHG_CH15_Included

#define PPI_CHG0_CH14_Pos       PPI_CHG_CH14_Pos
#define PPI_CHG0_CH14_Msk       PPI_CHG_CH14_Msk
#define PPI_CHG0_CH14_Excluded  PPI_CHG_CH14_Excluded
#define PPI_CHG0_CH14_Included  PPI_CHG_CH14_Included

#define PPI_CHG0_CH13_Pos       PPI_CHG_CH13_Pos
#define PPI_CHG0_CH13_Msk       PPI_CHG_CH13_Msk
#define PPI_CHG0_CH13_Excluded  PPI_CHG_CH13_Excluded
#define PPI_CHG0_CH13_Included  PPI_CHG_CH13_Included

#define PPI_CHG0_CH12_Pos       PPI_CHG_CH12_Pos
#define PPI_CHG0_CH12_Msk       PPI_CHG_CH12_Msk
#define PPI_CHG0_CH12_Excluded  PPI_CHG_CH12_Excluded
#define PPI_CHG0_CH12_Included  PPI_CHG_CH12_Included

#define PPI_CHG0_CH11_Pos       PPI_CHG_CH11_Pos
#define PPI_CHG0_CH11_Msk       PPI_CHG_CH11_Msk
#define PPI_CHG0_CH11_Excluded  PPI_CHG_CH11_Excluded
#define PPI_CHG0_CH11_Included  PPI_CHG_CH11_Included

#define PPI_CHG0_CH10_Pos       PPI_CHG_CH10_Pos
#define PPI_CHG0_CH10_Msk       PPI_CHG_CH10_Msk
#define PPI_CHG0_CH10_Excluded  PPI_CHG_CH10_Excluded
#define PPI_CHG0_CH10_Included  PPI_CHG_CH10_Included

#define PPI_CHG0_CH9_Pos        PPI_CHG_CH9_Pos
#define PPI_CHG0_CH9_Msk        PPI_CHG_CH9_Msk
#define PPI_CHG0_CH9_Excluded   PPI_CHG_CH9_Excluded
#define PPI_CHG0_CH9_Included   PPI_CHG_CH9_Included

#define PPI_CHG0_CH8_Pos        PPI_CHG_CH8_Pos
#define PPI_CHG0_CH8_Msk        PPI_CHG_CH8_Msk
#define PPI_CHG0_CH8_Excluded   PPI_CHG_CH8_Excluded
#define PPI_CHG0_CH8_Included   PPI_CHG_CH8_Included

#define PPI_CHG0_CH7_Pos        PPI_CHG_CH7_Pos
#define PPI_CHG0_CH7_Msk        PPI_CHG_CH7_Msk
#define PPI_CHG0_CH7_Excluded   PPI_CHG_CH7_Excluded
#define PPI_CHG0_CH7_Included   PPI_CHG_CH7_Included

#define PPI_CHG0_CH6_Pos        PPI_CHG_CH6_Pos
#define PPI_CHG0_CH6_Msk        PPI_CHG_CH6_Msk
#define PPI_CHG0_CH6_Excluded   PPI_CHG_CH6_Excluded
#define PPI_CHG0_CH6_Included   PPI_CHG_CH6_Included

#define PPI_CHG0_CH5_Pos        PPI_CHG_CH5_Pos
#define PPI_CHG0_CH5_Msk        PPI_CHG_CH5_Msk
#define PPI_CHG0_CH5_Excluded   PPI_CHG_CH5_Excluded
#define PPI_CHG0_CH5_Included   PPI_CHG_CH5_Included

#define PPI_CHG0_CH4_Pos        PPI_CHG_CH4_Pos
#define PPI_CHG0_CH4_Msk        PPI_CHG_CH4_Msk
#define PPI_CHG0_CH4_Excluded   PPI_CHG_CH4_Excluded
#define PPI_CHG0_CH4_Included   PPI_CHG_CH4_Included

#define PPI_CHG0_CH3_Pos        PPI_CHG_CH3_Pos
#define PPI_CHG0_CH3_Msk        PPI_CHG_CH3_Msk
#define PPI_CHG0_CH3_Excluded   PPI_CHG_CH3_Excluded
#define PPI_CHG0_CH3_Included   PPI_CHG_CH3_Included

#define PPI_CHG0_CH2_Pos        PPI_CHG_CH2_Pos
#define PPI_CHG0_CH2_Msk        PPI_CHG_CH2_Msk
#define PPI_CHG0_CH2_Excluded   PPI_CHG_CH2_Excluded
#define PPI_CHG0_CH2_Included   PPI_CHG_CH2_Included

#define PPI_CHG0_CH1_Pos        PPI_CHG_CH1_Pos
#define PPI_CHG0_CH1_Msk        PPI_CHG_CH1_Msk
#define PPI_CHG0_CH1_Excluded   PPI_CHG_CH1_Excluded
#define PPI_CHG0_CH1_Included   PPI_CHG_CH1_Included

#define PPI_CHG0_CH0_Pos        PPI_CHG_CH0_Pos
#define PPI_CHG0_CH0_Msk        PPI_CHG_CH0_Msk
#define PPI_CHG0_CH0_Excluded   PPI_CHG_CH0_Excluded
#define PPI_CHG0_CH0_Included   PPI_CHG_CH0_Included

#define PPI_CHG1_CH15_Pos       PPI_CHG_CH15_Pos
#define PPI_CHG1_CH15_Msk       PPI_CHG_CH15_Msk
#define PPI_CHG1_CH15_Excluded  PPI_CHG_CH15_Excluded
#define PPI_CHG1_CH15_Included  PPI_CHG_CH15_Included

#define PPI_CHG1_CH14_Pos       PPI_CHG_CH14_Pos
#define PPI_CHG1_CH14_Msk       PPI_CHG_CH14_Msk
#define PPI_CHG1_CH14_Excluded  PPI_CHG_CH14_Excluded
#define PPI_CHG1_CH14_Included  PPI_CHG_CH14_Included

#define PPI_CHG1_CH13_Pos       PPI_CHG_CH13_Pos
#define PPI_CHG1_CH13_Msk       PPI_CHG_CH13_Msk
#define PPI_CHG1_CH13_Excluded  PPI_CHG_CH13_Excluded
#define PPI_CHG1_CH13_Included  PPI_CHG_CH13_Included

#define PPI_CHG1_CH12_Pos       PPI_CHG_CH12_Pos
#define PPI_CHG1_CH12_Msk       PPI_CHG_CH12_Msk
#define PPI_CHG1_CH12_Excluded  PPI_CHG_CH12_Excluded
#define PPI_CHG1_CH12_Included  PPI_CHG_CH12_Included

#define PPI_CHG1_CH11_Pos       PPI_CHG_CH11_Pos
#define PPI_CHG1_CH11_Msk       PPI_CHG_CH11_Msk
#define PPI_CHG1_CH11_Excluded  PPI_CHG_CH11_Excluded
#define PPI_CHG1_CH11_Included  PPI_CHG_CH11_Included

#define PPI_CHG1_CH10_Pos       PPI_CHG_CH10_Pos
#define PPI_CHG1_CH10_Msk       PPI_CHG_CH10_Msk
#define PPI_CHG1_CH10_Excluded  PPI_CHG_CH10_Excluded
#define PPI_CHG1_CH10_Included  PPI_CHG_CH10_Included

#define PPI_CHG1_CH9_Pos        PPI_CHG_CH9_Pos
#define PPI_CHG1_CH9_Msk        PPI_CHG_CH9_Msk
#define PPI_CHG1_CH9_Excluded   PPI_CHG_CH9_Excluded
#define PPI_CHG1_CH9_Included   PPI_CHG_CH9_Included

#define PPI_CHG1_CH8_Pos        PPI_CHG_CH8_Pos
#define PPI_CHG1_CH8_Msk        PPI_CHG_CH8_Msk
#define PPI_CHG1_CH8_Excluded   PPI_CHG_CH8_Excluded
#define PPI_CHG1_CH8_Included   PPI_CHG_CH8_Included

#define PPI_CHG1_CH7_Pos        PPI_CHG_CH7_Pos
#define PPI_CHG1_CH7_Msk        PPI_CHG_CH7_Msk
#define PPI_CHG1_CH7_Excluded   PPI_CHG_CH7_Excluded
#define PPI_CHG1_CH7_Included   PPI_CHG_CH7_Included

#define PPI_CHG1_CH6_Pos        PPI_CHG_CH6_Pos
#define PPI_CHG1_CH6_Msk        PPI_CHG_CH6_Msk
#define PPI_CHG1_CH6_Excluded   PPI_CHG_CH6_Excluded
#define PPI_CHG1_CH6_Included   PPI_CHG_CH6_Included

#define PPI_CHG1_CH5_Pos        PPI_CHG_CH5_Pos
#define PPI_CHG1_CH5_Msk        PPI_CHG_CH5_Msk
#define PPI_CHG1_CH5_Excluded   PPI_CHG_CH5_Excluded
#define PPI_CHG1_CH5_Included   PPI_CHG_CH5_Included

#define PPI_CHG1_CH4_Pos        PPI_CHG_CH4_Pos
#define PPI_CHG1_CH4_Msk        PPI_CHG_CH4_Msk
#define PPI_CHG1_CH4_Excluded   PPI_CHG_CH4_Excluded
#define PPI_CHG1_CH4_Included   PPI_CHG_CH4_Included

#define PPI_CHG1_CH3_Pos        PPI_CHG_CH3_Pos
#define PPI_CHG1_CH3_Msk        PPI_CHG_CH3_Msk
#define PPI_CHG1_CH3_Excluded   PPI_CHG_CH3_Excluded
#define PPI_CHG1_CH3_Included   PPI_CHG_CH3_Included

#define PPI_CHG1_CH2_Pos        PPI_CHG_CH2_Pos
#define PPI_CHG1_CH2_Msk        PPI_CHG_CH2_Msk
#define PPI_CHG1_CH2_Excluded   PPI_CHG_CH2_Excluded
#define PPI_CHG1_CH2_Included   PPI_CHG_CH2_Included

#define PPI_CHG1_CH1_Pos        PPI_CHG_CH1_Pos
#define PPI_CHG1_CH1_Msk        PPI_CHG_CH1_Msk
#define PPI_CHG1_CH1_Excluded   PPI_CHG_CH1_Excluded
#define PPI_CHG1_CH1_Included   PPI_CHG_CH1_Included

#define PPI_CHG1_CH0_Pos        PPI_CHG_CH0_Pos
#define PPI_CHG1_CH0_Msk        PPI_CHG_CH0_Msk
#define PPI_CHG1_CH0_Excluded   PPI_CHG_CH0_Excluded
#define PPI_CHG1_CH0_Included   PPI_CHG_CH0_Included

#define PPI_CHG2_CH15_Pos       PPI_CHG_CH15_Pos
#define PPI_CHG2_CH15_Msk       PPI_CHG_CH15_Msk
#define PPI_CHG2_CH15_Excluded  PPI_CHG_CH15_Excluded
#define PPI_CHG2_CH15_Included  PPI_CHG_CH15_Included

#define PPI_CHG2_CH14_Pos       PPI_CHG_CH14_Pos
#define PPI_CHG2_CH14_Msk       PPI_CHG_CH14_Msk
#define PPI_CHG2_CH14_Excluded  PPI_CHG_CH14_Excluded
#define PPI_CHG2_CH14_Included  PPI_CHG_CH14_Included

#define PPI_CHG2_CH13_Pos       PPI_CHG_CH13_Pos
#define PPI_CHG2_CH13_Msk       PPI_CHG_CH13_Msk
#define PPI_CHG2_CH13_Excluded  PPI_CHG_CH13_Excluded
#define PPI_CHG2_CH13_Included  PPI_CHG_CH13_Included

#define PPI_CHG2_CH12_Pos       PPI_CHG_CH12_Pos
#define PPI_CHG2_CH12_Msk       PPI_CHG_CH12_Msk
#define PPI_CHG2_CH12_Excluded  PPI_CHG_CH12_Excluded
#define PPI_CHG2_CH12_Included  PPI_CHG_CH12_Included

#define PPI_CHG2_CH11_Pos       PPI_CHG_CH11_Pos
#define PPI_CHG2_CH11_Msk       PPI_CHG_CH11_Msk
#define PPI_CHG2_CH11_Excluded  PPI_CHG_CH11_Excluded
#define PPI_CHG2_CH11_Included  PPI_CHG_CH11_Included

#define PPI_CHG2_CH10_Pos       PPI_CHG_CH10_Pos
#define PPI_CHG2_CH10_Msk       PPI_CHG_CH10_Msk
#define PPI_CHG2_CH10_Excluded  PPI_CHG_CH10_Excluded
#define PPI_CHG2_CH10_Included  PPI_CHG_CH10_Included

#define PPI_CHG2_CH9_Pos        PPI_CHG_CH9_Pos
#define PPI_CHG2_CH9_Msk        PPI_CHG_CH9_Msk
#define PPI_CHG2_CH9_Excluded   PPI_CHG_CH9_Excluded
#define PPI_CHG2_CH9_Included   PPI_CHG_CH9_Included

#define PPI_CHG2_CH8_Pos        PPI_CHG_CH8_Pos
#define PPI_CHG2_CH8_Msk        PPI_CHG_CH8_Msk
#define PPI_CHG2_CH8_Excluded   PPI_CHG_CH8_Excluded
#define PPI_CHG2_CH8_Included   PPI_CHG_CH8_Included

#define PPI_CHG2_CH7_Pos        PPI_CHG_CH7_Pos
#define PPI_CHG2_CH7_Msk        PPI_CHG_CH7_Msk
#define PPI_CHG2_CH7_Excluded   PPI_CHG_CH7_Excluded
#define PPI_CHG2_CH7_Included   PPI_CHG_CH7_Included

#define PPI_CHG2_CH6_Pos        PPI_CHG_CH6_Pos
#define PPI_CHG2_CH6_Msk        PPI_CHG_CH6_Msk
#define PPI_CHG2_CH6_Excluded   PPI_CHG_CH6_Excluded
#define PPI_CHG2_CH6_Included   PPI_CHG_CH6_Included

#define PPI_CHG2_CH5_Pos        PPI_CHG_CH5_Pos
#define PPI_CHG2_CH5_Msk        PPI_CHG_CH5_Msk
#define PPI_CHG2_CH5_Excluded   PPI_CHG_CH5_Excluded
#define PPI_CHG2_CH5_Included   PPI_CHG_CH5_Included

#define PPI_CHG2_CH4_Pos        PPI_CHG_CH4_Pos
#define PPI_CHG2_CH4_Msk        PPI_CHG_CH4_Msk
#define PPI_CHG2_CH4_Excluded   PPI_CHG_CH4_Excluded
#define PPI_CHG2_CH4_Included   PPI_CHG_CH4_Included

#define PPI_CHG2_CH3_Pos        PPI_CHG_CH3_Pos
#define PPI_CHG2_CH3_Msk        PPI_CHG_CH3_Msk
#define PPI_CHG2_CH3_Excluded   PPI_CHG_CH3_Excluded
#define PPI_CHG2_CH3_Included   PPI_CHG_CH3_Included

#define PPI_CHG2_CH2_Pos        PPI_CHG_CH2_Pos
#define PPI_CHG2_CH2_Msk        PPI_CHG_CH2_Msk
#define PPI_CHG2_CH2_Excluded   PPI_CHG_CH2_Excluded
#define PPI_CHG2_CH2_Included   PPI_CHG_CH2_Included

#define PPI_CHG2_CH1_Pos        PPI_CHG_CH1_Pos
#define PPI_CHG2_CH1_Msk        PPI_CHG_CH1_Msk
#define PPI_CHG2_CH1_Excluded   PPI_CHG_CH1_Excluded
#define PPI_CHG2_CH1_Included   PPI_CHG_CH1_Included

#define PPI_CHG2_CH0_Pos        PPI_CHG_CH0_Pos
#define PPI_CHG2_CH0_Msk        PPI_CHG_CH0_Msk
#define PPI_CHG2_CH0_Excluded   PPI_CHG_CH0_Excluded
#define PPI_CHG2_CH0_Included   PPI_CHG_CH0_Included

#define PPI_CHG3_CH15_Pos       PPI_CHG_CH15_Pos
#define PPI_CHG3_CH15_Msk       PPI_CHG_CH15_Msk
#define PPI_CHG3_CH15_Excluded  PPI_CHG_CH15_Excluded
#define PPI_CHG3_CH15_Included  PPI_CHG_CH15_Included

#define PPI_CHG3_CH14_Pos       PPI_CHG_CH14_Pos
#define PPI_CHG3_CH14_Msk       PPI_CHG_CH14_Msk
#define PPI_CHG3_CH14_Excluded  PPI_CHG_CH14_Excluded
#define PPI_CHG3_CH14_Included  PPI_CHG_CH14_Included

#define PPI_CHG3_CH13_Pos       PPI_CHG_CH13_Pos
#define PPI_CHG3_CH13_Msk       PPI_CHG_CH13_Msk
#define PPI_CHG3_CH13_Excluded  PPI_CHG_CH13_Excluded
#define PPI_CHG3_CH13_Included  PPI_CHG_CH13_Included

#define PPI_CHG3_CH12_Pos       PPI_CHG_CH12_Pos
#define PPI_CHG3_CH12_Msk       PPI_CHG_CH12_Msk
#define PPI_CHG3_CH12_Excluded  PPI_CHG_CH12_Excluded
#define PPI_CHG3_CH12_Included  PPI_CHG_CH12_Included

#define PPI_CHG3_CH11_Pos       PPI_CHG_CH11_Pos
#define PPI_CHG3_CH11_Msk       PPI_CHG_CH11_Msk
#define PPI_CHG3_CH11_Excluded  PPI_CHG_CH11_Excluded
#define PPI_CHG3_CH11_Included  PPI_CHG_CH11_Included

#define PPI_CHG3_CH10_Pos       PPI_CHG_CH10_Pos
#define PPI_CHG3_CH10_Msk       PPI_CHG_CH10_Msk
#define PPI_CHG3_CH10_Excluded  PPI_CHG_CH10_Excluded
#define PPI_CHG3_CH10_Included  PPI_CHG_CH10_Included

#define PPI_CHG3_CH9_Pos        PPI_CHG_CH9_Pos
#define PPI_CHG3_CH9_Msk        PPI_CHG_CH9_Msk
#define PPI_CHG3_CH9_Excluded   PPI_CHG_CH9_Excluded
#define PPI_CHG3_CH9_Included   PPI_CHG_CH9_Included

#define PPI_CHG3_CH8_Pos        PPI_CHG_CH8_Pos
#define PPI_CHG3_CH8_Msk        PPI_CHG_CH8_Msk
#define PPI_CHG3_CH8_Excluded   PPI_CHG_CH8_Excluded
#define PPI_CHG3_CH8_Included   PPI_CHG_CH8_Included

#define PPI_CHG3_CH7_Pos        PPI_CHG_CH7_Pos
#define PPI_CHG3_CH7_Msk        PPI_CHG_CH7_Msk
#define PPI_CHG3_CH7_Excluded   PPI_CHG_CH7_Excluded
#define PPI_CHG3_CH7_Included   PPI_CHG_CH7_Included

#define PPI_CHG3_CH6_Pos        PPI_CHG_CH6_Pos
#define PPI_CHG3_CH6_Msk        PPI_CHG_CH6_Msk
#define PPI_CHG3_CH6_Excluded   PPI_CHG_CH6_Excluded
#define PPI_CHG3_CH6_Included   PPI_CHG_CH6_Included

#define PPI_CHG3_CH5_Pos        PPI_CHG_CH5_Pos
#define PPI_CHG3_CH5_Msk        PPI_CHG_CH5_Msk
#define PPI_CHG3_CH5_Excluded   PPI_CHG_CH5_Excluded
#define PPI_CHG3_CH5_Included   PPI_CHG_CH5_Included

#define PPI_CHG3_CH4_Pos        PPI_CHG_CH4_Pos
#define PPI_CHG3_CH4_Msk        PPI_CHG_CH4_Msk
#define PPI_CHG3_CH4_Excluded   PPI_CHG_CH4_Excluded
#define PPI_CHG3_CH4_Included   PPI_CHG_CH4_Included

#define PPI_CHG3_CH3_Pos        PPI_CHG_CH3_Pos
#define PPI_CHG3_CH3_Msk        PPI_CHG_CH3_Msk
#define PPI_CHG3_CH3_Excluded   PPI_CHG_CH3_Excluded
#define PPI_CHG3_CH3_Included   PPI_CHG_CH3_Included

#define PPI_CHG3_CH2_Pos        PPI_CHG_CH2_Pos
#define PPI_CHG3_CH2_Msk        PPI_CHG_CH2_Msk
#define PPI_CHG3_CH2_Excluded   PPI_CHG_CH2_Excluded
#define PPI_CHG3_CH2_Included   PPI_CHG_CH2_Included

#define PPI_CHG3_CH1_Pos        PPI_CHG_CH1_Pos
#define PPI_CHG3_CH1_Msk        PPI_CHG_CH1_Msk
#define PPI_CHG3_CH1_Excluded   PPI_CHG_CH1_Excluded
#define PPI_CHG3_CH1_Included   PPI_CHG_CH1_Included

#define PPI_CHG3_CH0_Pos        PPI_CHG_CH0_Pos
#define PPI_CHG3_CH0_Msk        PPI_CHG_CH0_Msk
#define PPI_CHG3_CH0_Excluded   PPI_CHG_CH0_Excluded
#define PPI_CHG3_CH0_Included   PPI_CHG_CH0_Included




/*lint --flb "Leave library region" */

#endif /* NRF51_TO_NRF52840_H */

