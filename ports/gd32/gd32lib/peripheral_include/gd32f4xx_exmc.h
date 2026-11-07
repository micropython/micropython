/*!
    \file    gd32f4xx_exmc.h
    \brief   definitions for the EXMC
    
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

#ifndef GD32F4XX_EXMC_H
#define GD32F4XX_EXMC_H

#include "gd32f4xx.h"

/* EXMC definitions */
#define EXMC                                (EXMC_BASE)                   /*!< EXMC register base address */
#define EXMC_NOR_PSRAM                      (EXMC_BASE - 0x40000000)      /*!< EXMC NOR/PSRAM base address */
#define EXMC_NAND                           (EXMC_BASE - 0x30000000)      /*!< EXMC NAND base address */
#define EXMC_PCCARD                         (EXMC_BASE - 0x10000000)      /*!< EXMC PC card base address */
#define EXMC_SDRAM                          (EXMC_BASE + 0x20000000)      /*!< EXMC SDRAM base address */

/* registers definitions */
/* NOR/PSRAM */
#define EXMC_SNCTL0                         REG32(EXMC + 0x00U)           /*!< EXMC SRAM/NOR flash control register for region0 */
#define EXMC_SNTCFG0                        REG32(EXMC + 0x04U)           /*!< EXMC SRAM/NOR flash timing configuration register for region0 */
#define EXMC_SNWTCFG0                       REG32(EXMC + 0x104U)          /*!< EXMC SRAM/NOR flash write timing configuration register for region0 */

#define EXMC_SNCTL1                         REG32(EXMC + 0x08U)           /*!< EXMC SRAM/NOR flash control register for region1 */
#define EXMC_SNTCFG1                        REG32(EXMC + 0x0CU)           /*!< EXMC SRAM/NOR flash timing configuration register for region1 */
#define EXMC_SNWTCFG1                       REG32(EXMC + 0x10CU)          /*!< EXMC SRAM/NOR flash write timing configuration register for region1 */

#define EXMC_SNCTL2                         REG32(EXMC + 0x10U)           /*!< EXMC SRAM/NOR flash control register for region2 */
#define EXMC_SNTCFG2                        REG32(EXMC + 0x14U)           /*!< EXMC SRAM/NOR flash timing configuration register for region2 */
#define EXMC_SNWTCFG2                       REG32(EXMC + 0x114U)          /*!< EXMC SRAM/NOR flash write timing configuration register for region2 */

#define EXMC_SNCTL3                         REG32(EXMC + 0x18U)           /*!< EXMC SRAM/NOR flash control register for region3 */
#define EXMC_SNTCFG3                        REG32(EXMC + 0x1CU)           /*!< EXMC SRAM/NOR flash timing configuration register for region3 */
#define EXMC_SNWTCFG3                       REG32(EXMC + 0x11CU)          /*!< EXMC SRAM/NOR flash write timing configuration register for region3 */

/* NAND/PC card */
#define EXMC_NPCTL1                         REG32(EXMC + 0x60U)           /*!< EXMC NAND/PC card control register for bank1 */
#define EXMC_NPINTEN1                       REG32(EXMC + 0x64U)           /*!< EXMC NAND/PC card interrupt enable register for bank1 */
#define EXMC_NPCTCFG1                       REG32(EXMC + 0x68U)           /*!< EXMC NAND/PC card common space timing configuration register for bank1 */
#define EXMC_NPATCFG1                       REG32(EXMC + 0x6CU)           /*!< EXMC NAND/PC card attribute space timing configuration register for bank1 */
#define EXMC_NECC1                          REG32(EXMC + 0x74U)           /*!< EXMC NAND ECC register */

#define EXMC_NPCTL2                         REG32(EXMC + 0x80U)           /*!< EXMC NAND/PC card control register for bank2 */
#define EXMC_NPINTEN2                       REG32(EXMC + 0x84U)           /*!< EXMC NAND/PC card interrupt enable register for bank2 */
#define EXMC_NPCTCFG2                       REG32(EXMC + 0x88U)           /*!< EXMC NAND/PC card common space timing configuration register for bank2 */
#define EXMC_NPATCFG2                       REG32(EXMC + 0x8CU)           /*!< EXMC NAND/PC card attribute space timing configuration register for bank2 */
#define EXMC_NECC2                          REG32(EXMC + 0x94U)           /*!< EXMC NAND ECC register */

#define EXMC_NPCTL3                         REG32(EXMC + 0xA0U)           /*!< EXMC NAND/PC card control register for bank3 */
#define EXMC_NPINTEN3                       REG32(EXMC + 0xA4U)           /*!< EXMC NAND/PC card interrupt enable register for bank3 */
#define EXMC_NPCTCFG3                       REG32(EXMC + 0xA8U)           /*!< EXMC NAND/PC card common space timing configuration register for bank3 */
#define EXMC_NPATCFG3                       REG32(EXMC + 0xACU)           /*!< EXMC NAND/PC card attribute space timing configuration register for bank3 */
#define EXMC_PIOTCFG3                       REG32(EXMC + 0xB0U)           /*!< EXMC PC card I/O space timing configuration register for bank3 */

/* SDRAM */
#define EXMC_SDCTL0                         REG32(EXMC + 0x140U)          /*!< EXMC SDRAM control register for device0 */
#define EXMC_SDTCFG0                        REG32(EXMC + 0x148U)          /*!< EXMC SDRAM timing configuration register register for device0 */

#define EXMC_SDCTL1                         REG32(EXMC + 0x144U)          /*!< EXMC SDRAM control register for device1 */
#define EXMC_SDTCFG1                        REG32(EXMC + 0x14CU)          /*!< EXMC SDRAM timing configuration register register for device1 */

#define EXMC_SDCMD                          REG32(EXMC + 0x150U)          /*!< EXMC SDRAM command register */
#define EXMC_SDARI                          REG32(EXMC + 0x154U)          /*!< EXMC SDRAM auto-refresh interval register */
#define EXMC_SDSTAT                         REG32(EXMC + 0x158U)          /*!< EXMC SDRAM status register */
#define EXMC_SDRSCTL                        REG32(EXMC + 0x180U)          /*!< EXMC SDRAM read sample control register */

/* SQPI PSRAM */
#define EXMC_SINIT                          REG32(EXMC + 0x310U)          /*!< EXMC SPI initialization register */
#define EXMC_SRCMD                          REG32(EXMC + 0x320U)          /*!< EXMC SPI read command register */
#define EXMC_SWCMD                          REG32(EXMC + 0x330U)          /*!< EXMC SPI write command register */
#define EXMC_SIDL                           REG32(EXMC + 0x340U)          /*!< EXMC SPI ID low register */
#define EXMC_SIDH                           REG32(EXMC + 0x350U)          /*!< EXMC SPI ID high register */

/* bits definitions */
/* EXMC_SNCTLx,x=0..3 */
#define EXMC_SNCTL_NRBKEN                   BIT(0)                        /*!< NOR bank enable */
#define EXMC_SNCTL_NRMUX                    BIT(1)                        /*!< NOR bank memory address/data multiplexing enable */
#define EXMC_SNCTL_NRTP                     BITS(2,3)                     /*!< NOR bank memory type */
#define EXMC_SNCTL_NRW                      BITS(4,5)                     /*!< NOR bank memory data bus width */
#define EXMC_SNCTL_NREN                     BIT(6)                        /*!< NOR flash access enable */
#define EXMC_SNCTL_SBRSTEN                  BIT(8)                        /*!< synchronous burst enable */
#define EXMC_SNCTL_NRWTPOL                  BIT(9)                        /*!< NWAIT signal polarity */
#define EXMC_SNCTL_WRAPEN                   BIT(10)                       /*!< wrapped burst mode enable */
#define EXMC_SNCTL_NRWTCFG                  BIT(11)                       /*!< NWAIT signal configuration, only work in synchronous mode */
#define EXMC_SNCTL_WEN                      BIT(12)                       /*!< write enable */
#define EXMC_SNCTL_NRWTEN                   BIT(13)                       /*!< NWAIT signal enable */
#define EXMC_SNCTL_EXMODEN                  BIT(14)                       /*!< extended mode enable */
#define EXMC_SNCTL_ASYNCWTEN                BIT(15)                       /*!< asynchronous wait enable */
#define EXMC_SNCTL_CPS                      BITS(16,18)                   /*!< CRAM page size */
#define EXMC_SNCTL_SYNCWR                   BIT(19)                       /*!< synchronous write configuration */
#define EXMC_SNCTL_CCK                      BIT(20)                       /*!< consecutive clock configuration */

/* EXMC_SNTCFGx,x=0..3 */
#define EXMC_SNTCFG_ASET                    BITS(0,3)                     /*!< asynchronous address setup time */
#define EXMC_SNTCFG_AHLD                    BITS(4,7)                     /*!< asynchronous address hold time */
#define EXMC_SNTCFG_DSET                    BITS(8,15)                    /*!< asynchronous data setup time */
#define EXMC_SNTCFG_BUSLAT                  BITS(16,19)                   /*!< bus latency */
#define EXMC_SNTCFG_CKDIV                   BITS(20,23)                   /*!< synchronous clock divide ratio */
#define EXMC_SNTCFG_DLAT                    BITS(24,27)                   /*!< synchronous data latency for NOR flash */
#define EXMC_SNTCFG_ASYNCMOD                BITS(28,29)                   /*!< asynchronous access mode */

/* EXMC_SNWTCFGx,x=0..3 */
#define EXMC_SNWTCFG_WASET                  BITS(0,3)                     /*!< asynchronous address setup time */
#define EXMC_SNWTCFG_WAHLD                  BITS(4,7)                     /*!< asynchronous address hold time */
#define EXMC_SNWTCFG_WDSET                  BITS(8,15)                    /*!< asynchronous data setup time */
#define EXMC_SNWTCFG_WBUSLAT                BITS(16,19)                   /*!< bus latency */
#define EXMC_SNWTCFG_WASYNCMOD              BITS(28,29)                   /*!< asynchronous access mode */

/* EXMC_NPCTLx,x=1..3 */
#define EXMC_NPCTL_NDWTEN                   BIT(1)                        /*!< wait feature enable */
#define EXMC_NPCTL_NDBKEN                   BIT(2)                        /*!< NAND bank enable */
#define EXMC_NPCTL_NDTP                     BIT(3)                        /*!< NAND bank memory type */
#define EXMC_NPCTL_NDW                      BITS(4,5)                     /*!< NAND bank memory data bus width */
#define EXMC_NPCTL_ECCEN                    BIT(6)                        /*!< ECC enable */
#define EXMC_NPCTL_CTR                      BITS(9,12)                    /*!< CLE to RE delay */
#define EXMC_NPCTL_ATR                      BITS(13,16)                   /*!< ALE to RE delay */
#define EXMC_NPCTL_ECCSZ                    BITS(17,19)                   /*!< ECC size */

/* EXMC_NPINTENx,x=1..3 */
#define EXMC_NPINTEN_INTRS                  BIT(0)                        /*!< interrupt rising edge status */
#define EXMC_NPINTEN_INTHS                  BIT(1)                        /*!< interrupt high-level status */
#define EXMC_NPINTEN_INTFS                  BIT(2)                        /*!< interrupt falling edge status */
#define EXMC_NPINTEN_INTREN                 BIT(3)                        /*!< interrupt rising edge detection enable */
#define EXMC_NPINTEN_INTHEN                 BIT(4)                        /*!< interrupt high-level detection enable */
#define EXMC_NPINTEN_INTFEN                 BIT(5)                        /*!< interrupt falling edge detection enable */
#define EXMC_NPINTEN_FFEPT                  BIT(6)                        /*!< FIFO empty flag */

/* EXMC_NPCTCFGx,x=1..3 */
#define EXMC_NPCTCFG_COMSET                 BITS(0,7)                     /*!< common memory setup time */
#define EXMC_NPCTCFG_COMWAIT                BITS(8,15)                    /*!< common memory wait time */
#define EXMC_NPCTCFG_COMHLD                 BITS(16,23)                   /*!< common memory hold time */
#define EXMC_NPCTCFG_COMHIZ                 BITS(24,31)                   /*!< common memory data bus HiZ time */

/* EXMC_NPATCFGx,x=1..3 */
#define EXMC_NPATCFG_ATTSET                 BITS(0,7)                     /*!< attribute memory setup time */
#define EXMC_NPATCFG_ATTWAIT                BITS(8,15)                    /*!< attribute memory wait time */
#define EXMC_NPATCFG_ATTHLD                 BITS(16,23)                   /*!< attribute memory hold time */
#define EXMC_NPATCFG_ATTHIZ                 BITS(24,31)                   /*!< attribute memory data bus HiZ time */

/* EXMC_PIOTCFG3 */
#define EXMC_PIOTCFG3_IOSET                 BITS(0,7)                     /*!< IO space setup time */
#define EXMC_PIOTCFG3_IOWAIT                BITS(8,15)                    /*!< IO space wait time */
#define EXMC_PIOTCFG3_IOHLD                 BITS(16,23)                   /*!< IO space hold time */
#define EXMC_PIOTCFG3_IOHIZ                 BITS(24,31)                   /*!< IO space data bus HiZ time */

/* EXMC_NECCx,x=1..2 */
#define EXMC_NECC_ECC                       BITS(0,31)                    /*!< ECC result */

/* EXMC_SDCTLx,x=0..1 */
#define EXMC_SDCTL_CAW                      BITS(0,1)                     /*!< column address bit width */
#define EXMC_SDCTL_RAW                      BITS(2,3)                     /*!< row address bit width */
#define EXMC_SDCTL_SDW                      BITS(4,5)                     /*!< SDRAM data bus width */
#define EXMC_SDCTL_NBK                      BIT(6)                        /*!< number of banks */
#define EXMC_SDCTL_CL                       BIT(7,8)                      /*!< CAS Latency */
#define EXMC_SDCTL_WPEN                     BIT(9)                        /*!< write protection enable */
#define EXMC_SDCTL_SDCLK                    BITS(10,11)                   /*!< SDRAM clock configuration */
#define EXMC_SDCTL_BRSTRD                   BIT(12)                       /*!< burst read enable */
#define EXMC_SDCTL_PIPED                    BITS(13,14)                   /*!< pipeline delay */

/* EXMC_SDTCFGx,x=0..1 */
#define EXMC_SDTCFG_LMRD                    BITS(0,3)                     /*!< load mode register delay */
#define EXMC_SDTCFG_XSRD                    BITS(4,7)                     /*!< exit self-refresh delay */
#define EXMC_SDTCFG_RASD                    BITS(8,11)                    /*!< row address select delay */
#define EXMC_SDTCFG_ARFD                    BITS(12,15)                   /*!< auto refresh delay */
#define EXMC_SDTCFG_WRD                     BITS(16,19)                   /*!< write recovery delay */
#define EXMC_SDTCFG_RPD                     BITS(20,23)                   /*!< row precharge delay */
#define EXMC_SDTCFG_RCD                     BITS(24,27)                   /*!< row to column delay */

/* EXMC_SDCMD */
#define EXMC_SDCMD_CMD                      BITS(0,2)                     /*!< command */
#define EXMC_SDCMD_DS1                      BIT(3)                        /*!< select device1 */
#define EXMC_SDCMD_DS0                      BIT(4)                        /*!< select device0 */
#define EXMC_SDCMD_NARF                     BITS(5,8)                     /*!< number of successive auto-refresh */
#define EXMC_SDCMD_MRC                      BITS(9,21)                    /*!< mode register content */

/* EXMC_SDARI */
#define EXMC_SDARI_REC                      BIT(0)                        /*!< refresh error flag clear */
#define EXMC_SDARI_ARINTV                   BITS(1,13)                    /*!< auto-refresh interval */
#define EXMC_SDARI_REIE                     BIT(14)                       /*!< refresh error interrupt enable */

/* EXMC_SDSTAT */
#define EXMC_SDSDAT_REIF                    BIT(0)                        /*!< refresh error interrupt flag */
#define EXMC_SDSDAT_STA0                    BITS(1,2)                     /*!< device0 status */
#define EXMC_SDSDAT_STA1                    BITS(3,4)                     /*!< device1 status */
#define EXMC_SDSDAT_NRDY                    BIT(5)                        /*!< not ready status */

/* EXMC_SDRSCTL */
#define EXMC_SDRSCTL_RSEN                   BIT(0)                        /*!< read sample enable */
#define EXMC_SDRSCTL_SSCR                   BIT(1)                        /*!< select sample cycle of read data */
#define EXMC_SDRSCTL_SDSC                   BITS(4,7)                     /*!< select the delayed sample clock of read data */

/* EXMC_SINIT */
#define EXMC_SINIT_CMDBIT                   BITS(16,17)                   /*!< bit number of SPI PSRAM command phase */
#define EXMC_SINIT_ARDBIT                   BITS(24,28)                   /*!< bit number of SPI PSRAM address phase */
#define EXMC_SINIT_IDL                      BITS(29,30)                   /*!< SPI PSRAM ID length */
#define EXMC_SINIT_POL                      BIT(31)                       /*!< read data sample polarity */

/* EXMC_SRCMD */
#define EXMC_SRCMD_RCMD                     BITS(0,15)                    /*!< SPI read command for AHB read transfer */
#define EXMC_SRCMD_RWAITCYCLE               BITS(16,19)                   /*!< SPI read wait cycle number after address phase */
#define EXMC_SRCMD_RMODE                    BITS(20,21)                   /*!< SPI PSRAM read command mode */
#define EXMC_SRCMD_RDID                     BIT(31)                       /*!< send SPI read ID command */

/* EXMC_SWCMD */
#define EXMC_SWCMD_WCMD                     BITS(0,15)                    /*!< SPI write command for AHB write transfer */
#define EXMC_SWCMD_WWAITCYCLE               BITS(16,19)                   /*!< SPI write wait cycle number after address phase */
#define EXMC_SWCMD_WMODE                    BITS(20,21)                   /*!< SPI PSRAM write command mode */
#define EXMC_SWCMD_SC                       BIT(31)                       /*!< send SPI special command */

/* EXMC_SIDL */
#define EXMC_SIDL_SIDL                      BITS(0,31)                    /*!< ID low data saved for SPI read ID command */

/* EXMC_SIDH */
#define EXMC_SIDL_SIDH                      BITS(0,31)                    /*!< ID high Data saved for SPI read ID command */

/* constants definitions */
/* EXMC NOR/SRAM timing initialize structure */
typedef struct
{
    uint32_t asyn_access_mode;                                          /*!< asynchronous access mode */
    uint32_t syn_data_latency;                                          /*!< configure the data latency */
    uint32_t syn_clk_division;                                          /*!< configure the clock divide ratio */
    uint32_t bus_latency;                                               /*!< configure the bus latency */
    uint32_t asyn_data_setuptime;                                       /*!< configure the data setup time, asynchronous access mode valid */
    uint32_t asyn_address_holdtime;                                     /*!< configure the address hold time, asynchronous access mode valid */
    uint32_t asyn_address_setuptime;                                    /*!< configure the address setup time, asynchronous access mode valid */
}exmc_norsram_timing_parameter_struct;

/* EXMC NOR/SRAM initialize structure */
typedef struct
{
    uint32_t norsram_region;                                            /*!< select the region of EXMC NOR/SRAM bank */
    uint32_t write_mode;                                                /*!< the write mode, synchronous mode or asynchronous mode */
    uint32_t extended_mode;                                             /*!< enable or disable the extended mode */
    uint32_t asyn_wait;                                                 /*!< enable or disable the asynchronous wait function */
    uint32_t nwait_signal;                                              /*!< enable or disable the NWAIT signal while in synchronous bust mode */
    uint32_t memory_write;                                              /*!< enable or disable the write operation */
    uint32_t nwait_config;                                              /*!< NWAIT signal configuration */
    uint32_t wrap_burst_mode;                                           /*!< enable or disable the wrap burst mode */
    uint32_t nwait_polarity;                                            /*!< specifies the polarity of NWAIT signal from memory */
    uint32_t burst_mode;                                                /*!< enable or disable the burst mode */
    uint32_t databus_width;                                             /*!< specifies the databus width of external memory */
    uint32_t memory_type;                                               /*!< specifies the type of external memory */
    uint32_t address_data_mux;                                          /*!< specifies whether the data bus and address bus are multiplexed */
    exmc_norsram_timing_parameter_struct* read_write_timing;            /*!< timing parameters for read and write if the extendedmode is not used or the timing 
                                                                             parameters for read if the extendedmode is used. */
    exmc_norsram_timing_parameter_struct* write_timing;                 /*!< timing parameters for write when the extendedmode is used. */
}exmc_norsram_parameter_struct;

/* EXMC NAND/PC card timing initialize structure */
typedef struct
{
    uint32_t databus_hiztime;                                           /*!< configure the dadtabus HiZ time for write operation */
    uint32_t holdtime;                                                  /*!< configure the address hold time(or the data hold time for write operation) */
    uint32_t waittime;                                                  /*!< configure the minimum wait time */
    uint32_t setuptime;                                                 /*!< configure the address setup time */
}exmc_nand_pccard_timing_parameter_struct;

/* EXMC NAND initialize structure */
typedef struct
{
    uint32_t nand_bank;                                                 /*!< select the bank of NAND */ 
    uint32_t ecc_size;                                                  /*!< the page size for the ECC calculation */
    uint32_t atr_latency;                                               /*!< configure the latency of ALE low to RB low */
    uint32_t ctr_latency;                                               /*!< configure the latency of CLE low to RB low */
    uint32_t ecc_logic;                                                 /*!< enable or disable the ECC calculation logic */
    uint32_t databus_width;                                             /*!< the NAND flash databus width */
    uint32_t wait_feature;                                              /*!< enable or disable the wait feature */
    exmc_nand_pccard_timing_parameter_struct* common_space_timing;      /*!< the timing parameters for NAND flash common space */
    exmc_nand_pccard_timing_parameter_struct* attribute_space_timing;   /*!< the timing parameters for NAND flash attribute space */
}exmc_nand_parameter_struct;

/* EXMC PC card initialize structure */
typedef struct
{
    uint32_t atr_latency;                                               /*!< configure the latency of ALE low to RB low */
    uint32_t ctr_latency;                                               /*!< configure the latency of CLE low to RB low */
    uint32_t wait_feature;                                              /*!< enable or disable the wait feature */
    exmc_nand_pccard_timing_parameter_struct*  common_space_timing;     /*!< the timing parameters for PC card common space */
    exmc_nand_pccard_timing_parameter_struct*  attribute_space_timing;  /*!< the timing parameters for PC card attribute space */
    exmc_nand_pccard_timing_parameter_struct*  io_space_timing;         /*!< the timing parameters for PC card IO space */
}exmc_pccard_parameter_struct;

/* EXMC SDRAM timing initialize structure */
typedef struct
{
    uint32_t row_to_column_delay;                                       /*!< configure the row to column delay */
    uint32_t row_precharge_delay;                                       /*!< configure the row precharge delay */
    uint32_t write_recovery_delay;                                      /*!< configure the write recovery delay */
    uint32_t auto_refresh_delay;                                        /*!< configure the auto refresh delay */
    uint32_t row_address_select_delay;                                  /*!< configure the row address select delay */
    uint32_t exit_selfrefresh_delay;                                    /*!< configure the exit self-refresh delay */
    uint32_t load_mode_register_delay;                                  /*!< configure the load mode register delay */
}exmc_sdram_timing_parameter_struct;

/* EXMC SDRAM initialize structure */
typedef struct
{
    uint32_t sdram_device;                                              /*!< device of SDRAM */
    uint32_t pipeline_read_delay;                                       /*!< the delay for reading data after CAS latency in HCLK clock cycles */
    uint32_t burst_read_switch;                                         /*!< enable or disable the burst read */
    uint32_t sdclock_config;                                            /*!< the SDCLK memory clock for both SDRAM banks */
    uint32_t write_protection;                                          /*!< enable or disable SDRAM bank write protection function */
    uint32_t cas_latency;                                               /*!< configure the SDRAM CAS latency */
    uint32_t internal_bank_number;                                      /*!< the number of internal bank */
    uint32_t data_width;                                                /*!< the databus width of SDRAM memory */
    uint32_t row_address_width;                                         /*!< the bit width of a row address */
    uint32_t column_address_width;                                      /*!< the bit width of a column address */
    exmc_sdram_timing_parameter_struct* timing;                         /*!< the timing parameters for write and read SDRAM */
}exmc_sdram_parameter_struct;

/* EXMC SDRAM command initialize structure */
typedef struct
{
    uint32_t mode_register_content;                                     /*!< the SDRAM mode register content */
    uint32_t auto_refresh_number;                                       /*!< the number of successive auto-refresh cycles will be send when CMD = 011 */
    uint32_t bank_select;                                               /*!< the bank which command will be sent to */
    uint32_t command;                                                   /*!< the commands that will be sent to SDRAM */
}exmc_sdram_command_parameter_struct;

/* EXMC SQPISRAM initialize structure */
typedef struct{
    uint32_t sample_polarity;                                           /*!< read data sample polarity */
    uint32_t id_length;                                                 /*!< SPI PSRAM ID length */
    uint32_t address_bits;                                              /*!< bit number of SPI PSRAM address phase */
    uint32_t command_bits;                                              /*!< bit number of SPI PSRAM command phase */
}exmc_sqpipsram_parameter_struct;

/* EXMC register address */
#define EXMC_SNCTL(region)                    REG32(EXMC + 0x08U*((uint32_t)(region)))                      /*!< EXMC SRAM/NOR flash control registers, region = 0,1,2,3 */
#define EXMC_SNTCFG(region)                   REG32(EXMC + 0x04U + 0x08U*((uint32_t)(region)))              /*!< EXMC SRAM/NOR flash timing configuration registers, region = 0,1,2,3 */
#define EXMC_SNWTCFG(region)                  REG32(EXMC + 0x104U + 0x08U*((uint32_t)(region)))             /*!< EXMC SRAM/NOR flash write timing configuration registers, region = 0,1,2,3 */

#define EXMC_NPCTL(bank)                      REG32(EXMC + 0x40U + 0x20U*((uint32_t)(bank)))                /*!< EXMC NAND/PC card control registers, bank = 1,2,3 */
#define EXMC_NPINTEN(bank)                    REG32(EXMC + 0x44U + 0x20U*((uint32_t)(bank)))                /*!< EXMC NAND/PC card interrupt enable registers, bank = 1,2,3 */
#define EXMC_NPCTCFG(bank)                    REG32(EXMC + 0x48U + 0x20U*((uint32_t)(bank)))                /*!< EXMC NAND/PC card common space timing configuration registers, bank = 1,2,3 */
#define EXMC_NPATCFG(bank)                    REG32(EXMC + 0x4CU + 0x20U*((uint32_t)(bank)))                /*!< EXMC NAND/PC card attribute space timing configuration registers, bank = 1,2,3 */
#define EXMC_NECC(bank)                       REG32(EXMC + 0x54U + 0x20U*((uint32_t)(bank)))                /*!< EXMC NAND ECC registers, bank = 1,2 */

#define EXMC_SDCTL(device)                    REG32(EXMC + 0x140U + 0x4U*(((uint32_t)(device)) - 0x4U))     /*!< EXMC SDRAM control registers,device = 0,1 */
#define EXMC_SDTCFG(device)                   REG32(EXMC + 0x148U + 0x4U*(((uint32_t)(device)) - 0x4U))     /*!< EXMC SDRAM timing configuration registers,device = 0,1 */

/* CRAM page size */
#define SNCTL_CPS(regval)                   (BITS(16,18) & ((uint32_t)(regval) << 16))
#define EXMC_CRAM_AUTO_SPLIT                SNCTL_CPS(0)                  /*!< automatic burst split on page boundary crossing */
#define EXMC_CRAM_PAGE_SIZE_128_BYTES       SNCTL_CPS(1)                  /*!< page size is 128 bytes */
#define EXMC_CRAM_PAGE_SIZE_256_BYTES       SNCTL_CPS(2)                  /*!< page size is 256 bytes */
#define EXMC_CRAM_PAGE_SIZE_512_BYTES       SNCTL_CPS(3)                  /*!< page size is 512 bytes */
#define EXMC_CRAM_PAGE_SIZE_1024_BYTES      SNCTL_CPS(4)                  /*!< page size is 1024 bytes */

/* NOR bank memory data bus width */
#define SNCTL_NRW(regval)                   (BITS(4,5) & ((uint32_t)(regval) << 4))
#define EXMC_NOR_DATABUS_WIDTH_8B           SNCTL_NRW(0)                  /*!< NOR data width is 8 bits */
#define EXMC_NOR_DATABUS_WIDTH_16B          SNCTL_NRW(1)                  /*!< NOR data width is 16 bits */

/* NOR bank memory type */
#define SNCTL_NRTP(regval)                  (BITS(2,3) & ((uint32_t)(regval) << 2))
#define EXMC_MEMORY_TYPE_SRAM               SNCTL_NRTP(0)                 /*!< SRAM,ROM */
#define EXMC_MEMORY_TYPE_PSRAM              SNCTL_NRTP(1)                 /*!< PSRAM,CRAM */
#define EXMC_MEMORY_TYPE_NOR                SNCTL_NRTP(2)                 /*!< NOR flash */

/* asynchronous access mode */
#define SNTCFG_ASYNCMOD(regval)             (BITS(28,29) & ((uint32_t)(regval) << 28))
#define EXMC_ACCESS_MODE_A                  SNTCFG_ASYNCMOD(0)            /*!< mode A access */
#define EXMC_ACCESS_MODE_B                  SNTCFG_ASYNCMOD(1)            /*!< mode B access */
#define EXMC_ACCESS_MODE_C                  SNTCFG_ASYNCMOD(2)            /*!< mode C access */
#define EXMC_ACCESS_MODE_D                  SNTCFG_ASYNCMOD(3)            /*!< mode D access */

/* data latency for NOR flash */
#define SNTCFG_DLAT(regval)                 (BITS(24,27) & ((uint32_t)(regval) << 24))
#define EXMC_DATALAT_2_CLK                  SNTCFG_DLAT(0)                /*!< data latency of first burst access is 2 EXMC_CLK */
#define EXMC_DATALAT_3_CLK                  SNTCFG_DLAT(1)                /*!< data latency of first burst access is 3 EXMC_CLK */
#define EXMC_DATALAT_4_CLK                  SNTCFG_DLAT(2)                /*!< data latency of first burst access is 4 EXMC_CLK */
#define EXMC_DATALAT_5_CLK                  SNTCFG_DLAT(3)                /*!< data latency of first burst access is 5 EXMC_CLK */
#define EXMC_DATALAT_6_CLK                  SNTCFG_DLAT(4)                /*!< data latency of first burst access is 6 EXMC_CLK */
#define EXMC_DATALAT_7_CLK                  SNTCFG_DLAT(5)                /*!< data latency of first burst access is 7 EXMC_CLK */
#define EXMC_DATALAT_8_CLK                  SNTCFG_DLAT(6)                /*!< data latency of first burst access is 8 EXMC_CLK */
#define EXMC_DATALAT_9_CLK                  SNTCFG_DLAT(7)                /*!< data latency of first burst access is 9 EXMC_CLK */
#define EXMC_DATALAT_10_CLK                 SNTCFG_DLAT(8)                /*!< data latency of first burst access is 10 EXMC_CLK */
#define EXMC_DATALAT_11_CLK                 SNTCFG_DLAT(9)                /*!< data latency of first burst access is 11 EXMC_CLK */
#define EXMC_DATALAT_12_CLK                 SNTCFG_DLAT(10)               /*!< data latency of first burst access is 12 EXMC_CLK */
#define EXMC_DATALAT_13_CLK                 SNTCFG_DLAT(11)               /*!< data latency of first burst access is 13 EXMC_CLK */
#define EXMC_DATALAT_14_CLK                 SNTCFG_DLAT(12)               /*!< data latency of first burst access is 14 EXMC_CLK */
#define EXMC_DATALAT_15_CLK                 SNTCFG_DLAT(13)               /*!< data latency of first burst access is 15 EXMC_CLK */
#define EXMC_DATALAT_16_CLK                 SNTCFG_DLAT(14)               /*!< data latency of first burst access is 16 EXMC_CLK */
#define EXMC_DATALAT_17_CLK                 SNTCFG_DLAT(15)               /*!< data latency of first burst access is 17 EXMC_CLK */

/* synchronous clock divide ratio */
#define SNTCFG_CKDIV(regval)                (BITS(20,23) & ((uint32_t)(regval) << 20))
#define EXMC_SYN_CLOCK_RATIO_DISABLE        SNTCFG_CKDIV(0)               /*!< EXMC_CLK disable */
#define EXMC_SYN_CLOCK_RATIO_2_CLK          SNTCFG_CKDIV(1)               /*!< EXMC_CLK = 2*HCLK */
#define EXMC_SYN_CLOCK_RATIO_3_CLK          SNTCFG_CKDIV(2)               /*!< EXMC_CLK = 3*HCLK */
#define EXMC_SYN_CLOCK_RATIO_4_CLK          SNTCFG_CKDIV(3)               /*!< EXMC_CLK = 4*HCLK */
#define EXMC_SYN_CLOCK_RATIO_5_CLK          SNTCFG_CKDIV(4)               /*!< EXMC_CLK = 5*HCLK */
#define EXMC_SYN_CLOCK_RATIO_6_CLK          SNTCFG_CKDIV(5)               /*!< EXMC_CLK = 6*HCLK */
#define EXMC_SYN_CLOCK_RATIO_7_CLK          SNTCFG_CKDIV(6)               /*!< EXMC_CLK = 7*HCLK */
#define EXMC_SYN_CLOCK_RATIO_8_CLK          SNTCFG_CKDIV(7)               /*!< EXMC_CLK = 8*HCLK */
#define EXMC_SYN_CLOCK_RATIO_9_CLK          SNTCFG_CKDIV(8)               /*!< EXMC_CLK = 9*HCLK */
#define EXMC_SYN_CLOCK_RATIO_10_CLK         SNTCFG_CKDIV(9)               /*!< EXMC_CLK = 10*HCLK */
#define EXMC_SYN_CLOCK_RATIO_11_CLK         SNTCFG_CKDIV(10)              /*!< EXMC_CLK = 11*HCLK */
#define EXMC_SYN_CLOCK_RATIO_12_CLK         SNTCFG_CKDIV(11)              /*!< EXMC_CLK = 12*HCLK */
#define EXMC_SYN_CLOCK_RATIO_13_CLK         SNTCFG_CKDIV(12)              /*!< EXMC_CLK = 13*HCLK */
#define EXMC_SYN_CLOCK_RATIO_14_CLK         SNTCFG_CKDIV(13)              /*!< EXMC_CLK = 14*HCLK*/
#define EXMC_SYN_CLOCK_RATIO_15_CLK         SNTCFG_CKDIV(14)              /*!< EXMC_CLK = 15*HCLK */
#define EXMC_SYN_CLOCK_RATIO_16_CLK         SNTCFG_CKDIV(15)              /*!< EXMC_CLK = 16*HCLK */

/* ECC size */
#define NPCTL_ECCSZ(regval)                 (BITS(17,19) & ((uint32_t)(regval) << 17))
#define EXMC_ECC_SIZE_256BYTES              NPCTL_ECCSZ(0)                /* ECC size is 256 bytes */
#define EXMC_ECC_SIZE_512BYTES              NPCTL_ECCSZ(1)                /* ECC size is 512 bytes */
#define EXMC_ECC_SIZE_1024BYTES             NPCTL_ECCSZ(2)                /* ECC size is 1024 bytes */
#define EXMC_ECC_SIZE_2048BYTES             NPCTL_ECCSZ(3)                /* ECC size is 2048 bytes */
#define EXMC_ECC_SIZE_4096BYTES             NPCTL_ECCSZ(4)                /* ECC size is 4096 bytes */
#define EXMC_ECC_SIZE_8192BYTES             NPCTL_ECCSZ(5)                /* ECC size is 8192 bytes */

/* ALE to RE delay */
#define NPCTL_ATR(regval)                   (BITS(13,16) & ((uint32_t)(regval) << 13))
#define EXMC_ALE_RE_DELAY_1_HCLK            NPCTL_ATR(0)                  /* ALE to RE delay = 1*HCLK */
#define EXMC_ALE_RE_DELAY_2_HCLK            NPCTL_ATR(1)                  /* ALE to RE delay = 2*HCLK */
#define EXMC_ALE_RE_DELAY_3_HCLK            NPCTL_ATR(2)                  /* ALE to RE delay = 3*HCLK */
#define EXMC_ALE_RE_DELAY_4_HCLK            NPCTL_ATR(3)                  /* ALE to RE delay = 4*HCLK */
#define EXMC_ALE_RE_DELAY_5_HCLK            NPCTL_ATR(4)                  /* ALE to RE delay = 5*HCLK */
#define EXMC_ALE_RE_DELAY_6_HCLK            NPCTL_ATR(5)                  /* ALE to RE delay = 6*HCLK */
#define EXMC_ALE_RE_DELAY_7_HCLK            NPCTL_ATR(6)                  /* ALE to RE delay = 7*HCLK */
#define EXMC_ALE_RE_DELAY_8_HCLK            NPCTL_ATR(7)                  /* ALE to RE delay = 8*HCLK */
#define EXMC_ALE_RE_DELAY_9_HCLK            NPCTL_ATR(8)                  /* ALE to RE delay = 9*HCLK */
#define EXMC_ALE_RE_DELAY_10_HCLK           NPCTL_ATR(9)                  /* ALE to RE delay = 10*HCLK */
#define EXMC_ALE_RE_DELAY_11_HCLK           NPCTL_ATR(10)                 /* ALE to RE delay = 11*HCLK */
#define EXMC_ALE_RE_DELAY_12_HCLK           NPCTL_ATR(11)                 /* ALE to RE delay = 12*HCLK */
#define EXMC_ALE_RE_DELAY_13_HCLK           NPCTL_ATR(12)                 /* ALE to RE delay = 13*HCLK */
#define EXMC_ALE_RE_DELAY_14_HCLK           NPCTL_ATR(13)                 /* ALE to RE delay = 14*HCLK */
#define EXMC_ALE_RE_DELAY_15_HCLK           NPCTL_ATR(14)                 /* ALE to RE delay = 15*HCLK */
#define EXMC_ALE_RE_DELAY_16_HCLK           NPCTL_ATR(15)                 /* ALE to RE delay = 16*HCLK */

/* CLE to RE delay */
#define NPCTL_CTR(regval)                   (BITS(9,12) & ((uint32_t)(regval) << 9))
#define EXMC_CLE_RE_DELAY_1_HCLK            NPCTL_CTR(0)                  /* CLE to RE delay = 1*HCLK */
#define EXMC_CLE_RE_DELAY_2_HCLK            NPCTL_CTR(1)                  /* CLE to RE delay = 2*HCLK */
#define EXMC_CLE_RE_DELAY_3_HCLK            NPCTL_CTR(2)                  /* CLE to RE delay = 3*HCLK */
#define EXMC_CLE_RE_DELAY_4_HCLK            NPCTL_CTR(3)                  /* CLE to RE delay = 4*HCLK */
#define EXMC_CLE_RE_DELAY_5_HCLK            NPCTL_CTR(4)                  /* CLE to RE delay = 5*HCLK */
#define EXMC_CLE_RE_DELAY_6_HCLK            NPCTL_CTR(5)                  /* CLE to RE delay = 6*HCLK */
#define EXMC_CLE_RE_DELAY_7_HCLK            NPCTL_CTR(6)                  /* CLE to RE delay = 7*HCLK */
#define EXMC_CLE_RE_DELAY_8_HCLK            NPCTL_CTR(7)                  /* CLE to RE delay = 8*HCLK */
#define EXMC_CLE_RE_DELAY_9_HCLK            NPCTL_CTR(8)                  /* CLE to RE delay = 9*HCLK */
#define EXMC_CLE_RE_DELAY_10_HCLK           NPCTL_CTR(9)                  /* CLE to RE delay = 10*HCLK */
#define EXMC_CLE_RE_DELAY_11_HCLK           NPCTL_CTR(10)                 /* CLE to RE delay = 11*HCLK */
#define EXMC_CLE_RE_DELAY_12_HCLK           NPCTL_CTR(11)                 /* CLE to RE delay = 12*HCLK */
#define EXMC_CLE_RE_DELAY_13_HCLK           NPCTL_CTR(12)                 /* CLE to RE delay = 13*HCLK */
#define EXMC_CLE_RE_DELAY_14_HCLK           NPCTL_CTR(13)                 /* CLE to RE delay = 14*HCLK */
#define EXMC_CLE_RE_DELAY_15_HCLK           NPCTL_CTR(14)                 /* CLE to RE delay = 15*HCLK */
#define EXMC_CLE_RE_DELAY_16_HCLK           NPCTL_CTR(15)                 /* CLE to RE delay = 16*HCLK */

/* NAND bank memory data bus width */
#define NPCTL_NDW(regval)                   (BITS(4,5) & ((uint32_t)(regval) << 4))
#define EXMC_NAND_DATABUS_WIDTH_8B          NPCTL_NDW(0)                  /*!< NAND data width is 8 bits */
#define EXMC_NAND_DATABUS_WIDTH_16B         NPCTL_NDW(1)                  /*!< NAND data width is 16 bits */

/* SDRAM pipeline delay */
#define SDCTL_PIPED(regval)                 (BITS(13,14) & ((uint32_t)(regval) << 13))
#define EXMC_PIPELINE_DELAY_0_HCLK          SDCTL_PIPED(0)                /*!< 0 HCLK clock cycle delay */
#define EXMC_PIPELINE_DELAY_1_HCLK          SDCTL_PIPED(1)                /*!< 1 HCLK clock cycle delay */
#define EXMC_PIPELINE_DELAY_2_HCLK          SDCTL_PIPED(2)                /*!< 2 HCLK clock cycle delay */

/* SDRAM clock configuration */
#define SDCTL_SDCLK(regval)                 (BITS(10,11) & ((uint32_t)(regval) << 10))
#define EXMC_SDCLK_DISABLE                  SDCTL_SDCLK(0)                /*!< SDCLK memory clock disabled */
#define EXMC_SDCLK_PERIODS_2_HCLK           SDCTL_SDCLK(2)                /*!< SDCLK memory period = 2*HCLK */
#define EXMC_SDCLK_PERIODS_3_HCLK           SDCTL_SDCLK(3)                /*!< SDCLK memory period = 3*HCLK */

/* CAS latency */
#define SDCTL_CL(regval)                    (BITS(7,8) & ((uint32_t)(regval) << 7))
#define EXMC_CAS_LATENCY_1_SDCLK            SDCTL_CL(1)                   /*!< CAS latency is 1 memory clock cycle */
#define EXMC_CAS_LATENCY_2_SDCLK            SDCTL_CL(2)                   /*!< CAS latency is 2 memory clock cycle */
#define EXMC_CAS_LATENCY_3_SDCLK            SDCTL_CL(3)                   /*!< CAS latency is 3 memory clock cycle */

/* SDRAM data bus width */
#define SDCTL_SDW(regval)                   (BITS(4,5) & ((uint32_t)(regval) << 4))
#define EXMC_SDRAM_DATABUS_WIDTH_8B         SDCTL_SDW(0)                  /*!< SDRAM data width 8 bits */
#define EXMC_SDRAM_DATABUS_WIDTH_16B        SDCTL_SDW(1)                  /*!< SDRAM data width 16 bits */
#define EXMC_SDRAM_DATABUS_WIDTH_32B        SDCTL_SDW(2)                  /*!< SDRAM data width 32 bits */

/* SDRAM row address bit width */
#define SDCTL_RAW(regval)                   (BITS(2,3) & ((uint32_t)(regval) << 2))
#define EXMC_SDRAM_ROW_ADDRESS_11           SDCTL_RAW(0)                  /*!< row address bit width is 11 bits */
#define EXMC_SDRAM_ROW_ADDRESS_12           SDCTL_RAW(1)                  /*!< row address bit width is 12 bits */
#define EXMC_SDRAM_ROW_ADDRESS_13           SDCTL_RAW(2)                  /*!< row address bit width is 13 bits */

/* SDRAM column address bit width */
#define SDCTL_CAW(regval)                   (BITS(0,1) & ((uint32_t)(regval) << 0))
#define EXMC_SDRAM_COW_ADDRESS_8            SDCTL_CAW(0)                  /*!< column address bit width is 8 bits */
#define EXMC_SDRAM_COW_ADDRESS_9            SDCTL_CAW(1)                  /*!< column address bit width is 9 bits */
#define EXMC_SDRAM_COW_ADDRESS_10           SDCTL_CAW(2)                  /*!< column address bit width is 10 bits */
#define EXMC_SDRAM_COW_ADDRESS_11           SDCTL_CAW(3)                  /*!< column address bit width is 11 bits */

/* SDRAM number of successive auto-refresh */
#define SDCMD_NARF(regval)                  (BITS(5,8) & ((uint32_t)(regval) << 5))
#define EXMC_SDRAM_AUTO_REFLESH_1_SDCLK     SDCMD_NARF(0)                 /*!< 1 auto-refresh cycle */
#define EXMC_SDRAM_AUTO_REFLESH_2_SDCLK     SDCMD_NARF(1)                 /*!< 2 auto-refresh cycles */
#define EXMC_SDRAM_AUTO_REFLESH_3_SDCLK     SDCMD_NARF(2)                 /*!< 3 auto-refresh cycles */
#define EXMC_SDRAM_AUTO_REFLESH_4_SDCLK     SDCMD_NARF(3)                 /*!< 4 auto-refresh cycles */
#define EXMC_SDRAM_AUTO_REFLESH_5_SDCLK     SDCMD_NARF(4)                 /*!< 5 auto-refresh cycles */
#define EXMC_SDRAM_AUTO_REFLESH_6_SDCLK     SDCMD_NARF(5)                 /*!< 6 auto-refresh cycles */
#define EXMC_SDRAM_AUTO_REFLESH_7_SDCLK     SDCMD_NARF(6)                 /*!< 7 auto-refresh cycles */
#define EXMC_SDRAM_AUTO_REFLESH_8_SDCLK     SDCMD_NARF(7)                 /*!< 8 auto-refresh cycles */
#define EXMC_SDRAM_AUTO_REFLESH_9_SDCLK     SDCMD_NARF(8)                 /*!< 9 auto-refresh cycles */
#define EXMC_SDRAM_AUTO_REFLESH_10_SDCLK    SDCMD_NARF(9)                 /*!< 10 auto-refresh cycles */
#define EXMC_SDRAM_AUTO_REFLESH_11_SDCLK    SDCMD_NARF(10)                /*!< 11 auto-refresh cycles */
#define EXMC_SDRAM_AUTO_REFLESH_12_SDCLK    SDCMD_NARF(11)                /*!< 12 auto-refresh cycles */
#define EXMC_SDRAM_AUTO_REFLESH_13_SDCLK    SDCMD_NARF(12)                /*!< 13 auto-refresh cycles */
#define EXMC_SDRAM_AUTO_REFLESH_14_SDCLK    SDCMD_NARF(13)                /*!< 14 auto-refresh cycles */
#define EXMC_SDRAM_AUTO_REFLESH_15_SDCLK    SDCMD_NARF(14)                /*!< 15 auto-refresh cycles */

/* SDRAM command selection */
#define SDCMD_CMD(regval)                   (BITS(0,2) & ((uint32_t)(regval) << 0))
#define EXMC_SDRAM_NORMAL_OPERATION         SDCMD_CMD(0)                  /*!< normal operation command */
#define EXMC_SDRAM_CLOCK_ENABLE             SDCMD_CMD(1)                  /*!< clock enable command */
#define EXMC_SDRAM_PRECHARGE_ALL            SDCMD_CMD(2)                  /*!< precharge all command */
#define EXMC_SDRAM_AUTO_REFRESH             SDCMD_CMD(3)                  /*!< auto-refresh command */
#define EXMC_SDRAM_LOAD_MODE_REGISTER       SDCMD_CMD(4)                  /*!< load mode register command */
#define EXMC_SDRAM_SELF_REFRESH             SDCMD_CMD(5)                  /*!< self-refresh command */
#define EXMC_SDRAM_POWERDOWN_ENTRY          SDCMD_CMD(6)                  /*!< power-down entry command */

/* SDRAM the delayed sample clock of read data */
#define SDRSCTL_SDSC(regval)                (BITS(4,7) & ((uint32_t)(regval) << 4))
#define EXMC_SDRAM_0_DELAY_CELL             SDRSCTL_SDSC(0)               /*!< select the clock after 0 delay cell */
#define EXMC_SDRAM_1_DELAY_CELL             SDRSCTL_SDSC(1)               /*!< select the clock after 1 delay cell */
#define EXMC_SDRAM_2_DELAY_CELL             SDRSCTL_SDSC(2)               /*!< select the clock after 2 delay cell */
#define EXMC_SDRAM_3_DELAY_CELL             SDRSCTL_SDSC(3)               /*!< select the clock after 3 delay cell */
#define EXMC_SDRAM_4_DELAY_CELL             SDRSCTL_SDSC(4)               /*!< select the clock after 4 delay cell */
#define EXMC_SDRAM_5_DELAY_CELL             SDRSCTL_SDSC(5)               /*!< select the clock after 5 delay cell */
#define EXMC_SDRAM_6_DELAY_CELL             SDRSCTL_SDSC(6)               /*!< select the clock after 6 delay cell */
#define EXMC_SDRAM_7_DELAY_CELL             SDRSCTL_SDSC(7)               /*!< select the clock after 7 delay cell */
#define EXMC_SDRAM_8_DELAY_CELL             SDRSCTL_SDSC(8)               /*!< select the clock after 8 delay cell */
#define EXMC_SDRAM_9_DELAY_CELL             SDRSCTL_SDSC(9)               /*!< select the clock after 9 delay cell */
#define EXMC_SDRAM_10_DELAY_CELL            SDRSCTL_SDSC(10)              /*!< select the clock after 10 delay cell */
#define EXMC_SDRAM_11_DELAY_CELL            SDRSCTL_SDSC(11)              /*!< select the clock after 11 delay cell */
#define EXMC_SDRAM_12_DELAY_CELL            SDRSCTL_SDSC(12)              /*!< select the clock after 12 delay cell */
#define EXMC_SDRAM_13_DELAY_CELL            SDRSCTL_SDSC(13)              /*!< select the clock after 13 delay cell */
#define EXMC_SDRAM_14_DELAY_CELL            SDRSCTL_SDSC(14)              /*!< select the clock after 14 delay cell */
#define EXMC_SDRAM_15_DELAY_CELL            SDRSCTL_SDSC(15)              /*!< select the clock after 15 delay cell */

/* SPI PSRAM ID length */
#define SINIT_IDL(regval)                   (BITS(29,30) & ((uint32_t)(regval) << 29))
#define EXMC_SQPIPSRAM_ID_LENGTH_64B        SINIT_IDL(0)                  /*!< SPI PSRAM ID length is 64 bits */
#define EXMC_SQPIPSRAM_ID_LENGTH_32B        SINIT_IDL(1)                  /*!< SPI PSRAM ID length is 32 bits */
#define EXMC_SQPIPSRAM_ID_LENGTH_16B        SINIT_IDL(2)                  /*!< SPI PSRAM ID length is 16 bits */
#define EXMC_SQPIPSRAM_ID_LENGTH_8B         SINIT_IDL(3)                  /*!< SPI PSRAM ID length is 8 bits */

/* SPI PSRAM bit number of address phase */
#define SINIT_ADRBIT(regval)                (BITS(24,28) & ((uint32_t)(regval) << 24))
#define EXMC_SQPIPSRAM_ADDR_LENGTH_1B       SINIT_ADRBIT(1)               /*!< SPI PSRAM address is 1 bit */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_2B       SINIT_ADRBIT(2)               /*!< SPI PSRAM address is 2 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_3B       SINIT_ADRBIT(3)               /*!< SPI PSRAM address is 3 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_4B       SINIT_ADRBIT(4)               /*!< SPI PSRAM address is 4 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_5B       SINIT_ADRBIT(5)               /*!< SPI PSRAM address is 5 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_6B       SINIT_ADRBIT(6)               /*!< SPI PSRAM address is 6 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_7B       SINIT_ADRBIT(7)               /*!< SPI PSRAM address is 7 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_8B       SINIT_ADRBIT(8)               /*!< SPI PSRAM address is 8 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_9B       SINIT_ADRBIT(9)               /*!< SPI PSRAM address is 9 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_10B      SINIT_ADRBIT(10)              /*!< SPI PSRAM address is 10 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_11B      SINIT_ADRBIT(11)              /*!< SPI PSRAM address is 11 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_12B      SINIT_ADRBIT(12)              /*!< SPI PSRAM address is 12 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_13B      SINIT_ADRBIT(13)              /*!< SPI PSRAM address is 13 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_14B      SINIT_ADRBIT(14)              /*!< SPI PSRAM address is 14 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_15B      SINIT_ADRBIT(15)              /*!< SPI PSRAM address is 15 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_16B      SINIT_ADRBIT(16)              /*!< SPI PSRAM address is 16 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_17B      SINIT_ADRBIT(17)              /*!< SPI PSRAM address is 17 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_18B      SINIT_ADRBIT(18)              /*!< SPI PSRAM address is 18 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_19B      SINIT_ADRBIT(19)              /*!< SPI PSRAM address is 19 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_20B      SINIT_ADRBIT(20)              /*!< SPI PSRAM address is 20 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_21B      SINIT_ADRBIT(21)              /*!< SPI PSRAM address is 21 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_22B      SINIT_ADRBIT(22)              /*!< SPI PSRAM address is 22 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_23B      SINIT_ADRBIT(23)              /*!< SPI PSRAM address is 23 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_24B      SINIT_ADRBIT(24)              /*!< SPI PSRAM address is 24 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_25B      SINIT_ADRBIT(25)              /*!< SPI PSRAM address is 25 bits */
#define EXMC_SQPIPSRAM_ADDR_LENGTH_26B      SINIT_ADRBIT(26)              /*!< SPI PSRAM address is 26 bits */

/* SPI PSRAM bit number of command phase */
#define SINIT_CMDBIT(regval)                (BITS(16,17) & ((uint32_t)(regval) << 16))
#define EXMC_SQPIPSRAM_COMMAND_LENGTH_4B    SINIT_CMDBIT(0)               /*!< SPI PSRAM command is 4 bits */
#define EXMC_SQPIPSRAM_COMMAND_LENGTH_8B    SINIT_CMDBIT(1)               /*!< SPI PSRAM command is 8 bits */
#define EXMC_SQPIPSRAM_COMMAND_LENGTH_16B   SINIT_CMDBIT(2)               /*!< SPI PSRAM command is 16 bits */

/* SPI PSRAM read command mode */
#define SRCMD_RMODE(regval)                 (BITS(20,21) & ((uint32_t)(regval) << 20))
#define EXMC_SQPIPSRAM_READ_MODE_DISABLE    SRCMD_RMODE(0)                /*!< not SPI mode */
#define EXMC_SQPIPSRAM_READ_MODE_SPI        SRCMD_RMODE(1)                /*!< SPI mode */
#define EXMC_SQPIPSRAM_READ_MODE_SQPI       SRCMD_RMODE(2)                /*!< SQPI mode */
#define EXMC_SQPIPSRAM_READ_MODE_QPI        SRCMD_RMODE(3)                /*!< QPI mode */

/* SPI PSRAM write command mode */
#define SRCMD_WMODE(regval)                 (BITS(20,21) & ((uint32_t)(regval) << 20))
#define EXMC_SQPIPSRAM_WRITE_MODE_DISABLE   SRCMD_WMODE(0)                /*!< not SPI mode */
#define EXMC_SQPIPSRAM_WRITE_MODE_SPI       SRCMD_WMODE(1)                /*!< SPI mode */
#define EXMC_SQPIPSRAM_WRITE_MODE_SQPI      SRCMD_WMODE(2)                /*!< SQPI mode */
#define EXMC_SQPIPSRAM_WRITE_MODE_QPI       SRCMD_WMODE(3)                /*!< QPI mode */

/* EXMC NOR/SRAM bank region definition */
#define EXMC_BANK0_NORSRAM_REGION0          ((uint32_t)0x00000000U)       /*!< bank0 NOR/SRAM region0 */
#define EXMC_BANK0_NORSRAM_REGION1          ((uint32_t)0x00000001U)       /*!< bank0 NOR/SRAM region1 */
#define EXMC_BANK0_NORSRAM_REGION2          ((uint32_t)0x00000002U)       /*!< bank0 NOR/SRAM region2 */
#define EXMC_BANK0_NORSRAM_REGION3          ((uint32_t)0x00000003U)       /*!< bank0 NOR/SRAM region3 */

/* EXMC consecutive clock */
#define EXMC_CLOCK_SYN_MODE                 ((uint32_t)0x00000000U)       /*!< EXMC_CLK is generated only during synchronous access */
#define EXMC_CLOCK_UNCONDITIONALLY          EXMC_SNCTL_CCK                /*!< EXMC_CLK is generated unconditionally */

/* EXMC NOR/SRAM write mode */
#define EXMC_ASYN_WRITE                     ((uint32_t)0x00000000U)       /*!< asynchronous write mode */
#define EXMC_SYN_WRITE                      EXMC_SNCTL_SYNCWR             /*!< synchronous write mode */

/* EXMC NWAIT signal configuration */
#define EXMC_NWAIT_CONFIG_BEFORE            ((uint32_t)0x00000000U)       /*!< NWAIT signal is active one data cycle before wait state */
#define EXMC_NWAIT_CONFIG_DURING            EXMC_SNCTL_NRWTCFG            /*!< NWAIT signal is active during wait state */

/* EXMC NWAIT signal polarity configuration */
#define EXMC_NWAIT_POLARITY_LOW             ((uint32_t)0x00000000U)       /*!< low level is active of NWAIT */
#define EXMC_NWAIT_POLARITY_HIGH            EXMC_SNCTL_NRWTPOL            /*!< high level is active of NWAIT */

/* EXMC NAND/PC card bank definition */
#define EXMC_BANK1_NAND                     ((uint32_t)0x00000001U)       /*!< NAND flash bank1 */
#define EXMC_BANK2_NAND                     ((uint32_t)0x00000002U)       /*!< NAND flash bank2 */
#define EXMC_BANK3_PCCARD                   ((uint32_t)0x00000003U)       /*!< PC card bank3 */

/* EXMC SDRAM bank definition */
#define EXMC_SDRAM_DEVICE0                  ((uint32_t)0x00000004U)       /*!< SDRAM device0 */
#define EXMC_SDRAM_DEVICE1                  ((uint32_t)0x00000005U)       /*!< SDRAM device1 */

/* EXMC SDRAM internal banks */
#define EXMC_SDRAM_2_INTER_BANK             ((uint32_t)0x00000000U)       /*!< 2 internal banks */
#define EXMC_SDRAM_4_INTER_BANK             EXMC_SDCTL_NBK                /*!< 4 internal banks */

/* SDRAM device0 selection */
#define EXMC_SDRAM_DEVICE0_UNSELECT         ((uint32_t)0x00000000U)       /*!< unselect SDRAM device0 */
#define EXMC_SDRAM_DEVICE0_SELECT           EXMC_SDCMD_DS0                /*!< select SDRAM device0 */

/* SDRAM device1 selection */
#define EXMC_SDRAM_DEVICE1_UNSELECT         ((uint32_t)0x00000000U)       /*!< unselect SDRAM device1 */
#define EXMC_SDRAM_DEVICE1_SELECT           EXMC_SDCMD_DS1                /*!< select SDRAM device1 */

/* SDRAM device status */
#define EXMC_SDRAM_DEVICE_NORMAL            ((uint32_t)0x00000000U)       /*!< normal status */
#define EXMC_SDRAM_DEVICE_SELF_REFRESH      ((uint32_t)0x00000001U)       /*!< self refresh status */
#define EXMC_SDRAM_DEVICE_POWER_DOWN        ((uint32_t)0x00000002U)       /*!< power down status */

/* sample cycle of read data */
#define EXMC_SDRAM_READSAMPLE_0_EXTRAHCLK   ((uint32_t)0x00000000U)       /*!< add 0 extra HCLK cycle to the read data sample clock besides the delay chain */
#define EXMC_SDRAM_READSAMPLE_1_EXTRAHCLK   EXMC_SDRSCTL_SSCR             /*!< add 1 extra HCLK cycle to the read data sample clock besides the delay chain */

/* read data sample polarity */
#define EXMC_SQPIPSRAM_SAMPLE_RISING_EDGE   ((uint32_t)0x00000000U)       /*!< sample data at rising edge */
#define EXMC_SQPIPSRAM_SAMPLE_FALLING_EDGE  EXMC_SINIT_POL                /*!< sample data at falling edge */

/* SQPI SRAM command flag */
#define EXMC_SEND_COMMAND_FLAG_RDID         EXMC_SRCMD_RDID               /*!< EXMC_SRCMD_RDID flag bit */
#define EXMC_SEND_COMMAND_FLAG_SC           EXMC_SWCMD_SC                 /*!< EXMC_SWCMD_SC flag bit */

/* EXMC flag bits */
#define EXMC_NAND_PCCARD_FLAG_RISE          EXMC_NPINTEN_INTRS            /*!< interrupt rising edge status */
#define EXMC_NAND_PCCARD_FLAG_LEVEL         EXMC_NPINTEN_INTHS            /*!< interrupt high-level status */
#define EXMC_NAND_PCCARD_FLAG_FALL          EXMC_NPINTEN_INTFS            /*!< interrupt falling edge status */
#define EXMC_NAND_PCCARD_FLAG_FIFOE         EXMC_NPINTEN_FFEPT            /*!< FIFO empty flag */
#define EXMC_SDRAM_FLAG_REFRESH             EXMC_SDSDAT_REIF              /*!< refresh error interrupt flag */
#define EXMC_SDRAM_FLAG_NREADY              EXMC_SDSDAT_NRDY              /*!< not ready status  */

/* EXMC interrupt flag bits */
#define EXMC_NAND_PCCARD_INT_FLAG_RISE      EXMC_NPINTEN_INTREN           /*!< rising edge interrupt and flag */
#define EXMC_NAND_PCCARD_INT_FLAG_LEVEL     EXMC_NPINTEN_INTHEN           /*!< high-level interrupt and flag  */
#define EXMC_NAND_PCCARD_INT_FLAG_FALL      EXMC_NPINTEN_INTFEN           /*!< falling edge interrupt and flag */
#define EXMC_SDRAM_INT_FLAG_REFRESH         EXMC_SDARI_REIE               /*!< refresh error interrupt and flag  */

/* function declarations */
/* initialization functions */
/* NOR/SRAM */
/* deinitialize EXMC NOR/SRAM region */
void exmc_norsram_deinit(uint32_t exmc_norsram_region);
/* initialize exmc_norsram_parameter_struct with the default values */
void exmc_norsram_struct_para_init(exmc_norsram_parameter_struct* exmc_norsram_init_struct);
/* initialize EXMC NOR/SRAM region */
void exmc_norsram_init(exmc_norsram_parameter_struct* exmc_norsram_init_struct);
/* enable EXMC NOR/SRAM region */
void exmc_norsram_enable(uint32_t exmc_norsram_region);
/* disable EXMC NOR/SRAM region */
void exmc_norsram_disable(uint32_t exmc_norsram_region);
/* NAND */
/* deinitialize EXMC NAND bank */
void exmc_nand_deinit(uint32_t exmc_nand_bank);
/* initialize exmc_nand_parameter_struct with the default values */
void exmc_nand_struct_para_init(exmc_nand_parameter_struct* exmc_nand_init_struct);
/* initialize EXMC NAND bank */
void exmc_nand_init(exmc_nand_parameter_struct* exmc_nand_init_struct);
/* enable EXMC NAND bank */
void exmc_nand_enable(uint32_t exmc_nand_bank);
/* disable EXMC NAND bank */
void exmc_nand_disable(uint32_t exmc_nand_bank);
/* PC card */
/* deinitialize EXMC PC card bank */
void exmc_pccard_deinit(void);
/* initialize exmc_pccard_parameter_struct with the default values */
void exmc_pccard_struct_para_init(exmc_pccard_parameter_struct* exmc_pccard_init_struct);
/* initialize EXMC PC card bank */
void exmc_pccard_init(exmc_pccard_parameter_struct* exmc_pccard_init_struct);
/* enable EXMC PC card bank */
void exmc_pccard_enable(void);
/* disable EXMC PC card bank */
void exmc_pccard_disable(void);
/* SDRAM */
/* deinitialize EXMC SDRAM device */
void exmc_sdram_deinit(uint32_t exmc_sdram_device);
/* initialize exmc_sdram_parameter_struct with the default values */
void exmc_sdram_struct_para_init(exmc_sdram_parameter_struct* exmc_sdram_init_struct);
/* initialize EXMC SDRAM device */
void exmc_sdram_init(exmc_sdram_parameter_struct* exmc_sdram_init_struct);
/* initialize exmc_sdram_command_parameter_struct with the default values */
void exmc_sdram_struct_command_para_init(exmc_sdram_command_parameter_struct *exmc_sdram_command_init_struct);
/* SQPIPSRAM */
/* deinitialize EXMC SQPIPSRAM */
void exmc_sqpipsram_deinit(void);
/* initialize exmc_sqpipsram_parameter_struct with the default values */
void exmc_sqpipsram_struct_para_init(exmc_sqpipsram_parameter_struct* exmc_sqpipsram_init_struct);
/* initialize EXMC SQPIPSRAM */
void exmc_sqpipsram_init(exmc_sqpipsram_parameter_struct* exmc_sqpipsram_init_struct);

/* function configuration */
/* NOR/SRAM */
/* configure consecutive clock */
void exmc_norsram_consecutive_clock_config(uint32_t clock_mode);
/* configure CRAM page size */
void exmc_norsram_page_size_config(uint32_t exmc_norsram_region, uint32_t page_size);
/* NAND */
/* enable or disable the EXMC NAND ECC function */
void exmc_nand_ecc_config(uint32_t exmc_nand_bank, ControlStatus newvalue);
/* get the EXMC ECC value */
uint32_t exmc_ecc_get(uint32_t exmc_nand_bank);
/* SDRAM */
/* enable or disable read sample */
void exmc_sdram_readsample_enable(ControlStatus newvalue);
/* configure the delayed sample clock of read data */
void exmc_sdram_readsample_config(uint32_t delay_cell, uint32_t extra_hclk);
/* configure the SDRAM memory command */
void exmc_sdram_command_config(exmc_sdram_command_parameter_struct* exmc_sdram_command_init_struct);
/* set auto-refresh interval */
void exmc_sdram_refresh_count_set(uint32_t exmc_count);
/* set the number of successive auto-refresh command */
void exmc_sdram_autorefresh_number_set(uint32_t exmc_number);
/* configure the write protection function */
void exmc_sdram_write_protection_config(uint32_t exmc_sdram_device, ControlStatus newvalue);
/* get the status of SDRAM device0 or device1 */
uint32_t exmc_sdram_bankstatus_get(uint32_t exmc_sdram_device);
/* SQPIPSRAM */
/* set the read command */
void exmc_sqpipsram_read_command_set(uint32_t read_command_mode,uint32_t read_wait_cycle,uint32_t read_command_code);
/* set the write command */
void exmc_sqpipsram_write_command_set(uint32_t write_command_mode,uint32_t write_wait_cycle,uint32_t write_command_code);
/* send SPI read ID command */
void exmc_sqpipsram_read_id_command_send(void);
/* send SPI special command which does not have address and data phase */
void exmc_sqpipsram_write_cmd_send(void);
/* get the EXMC SPI ID low data */
uint32_t exmc_sqpipsram_low_id_get(void);
/* get the EXMC SPI ID high data */
uint32_t exmc_sqpipsram_high_id_get(void);
/* get the bit value of EXMC send write command bit or read ID command */
FlagStatus exmc_sqpipsram_send_command_state_get(uint32_t send_command_flag);

/* interrupt & flag functions */
/* enable EXMC interrupt */
void exmc_interrupt_enable(uint32_t exmc_bank,uint32_t interrupt);
/* disable EXMC interrupt */
void exmc_interrupt_disable(uint32_t exmc_bank,uint32_t interrupt);
/* get EXMC flag status */
FlagStatus exmc_flag_get(uint32_t exmc_bank,uint32_t flag);
/* clear EXMC flag status */
void exmc_flag_clear(uint32_t exmc_bank,uint32_t flag);
/* get EXMC interrupt flag */
FlagStatus exmc_interrupt_flag_get(uint32_t exmc_bank,uint32_t interrupt);
/* clear EXMC interrupt flag */
void exmc_interrupt_flag_clear(uint32_t exmc_bank,uint32_t interrupt);

#endif /* GD32F4XX_EXMC_H */
