/*!
    \file    gd32f4xx_exmc.c
    \brief   EXMC driver

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

#include "gd32f4xx_exmc.h"

/* EXMC bank0 register reset value */
#define BANK0_SNCTL_RESET                 ((uint32_t)0x000030DAU)
#define BANK0_SNTCFG_RESET                ((uint32_t)0x0FFFFFFFU)
#define BANK0_SNWTCFG_RESET               ((uint32_t)0x0FFFFFFFU)

/* EXMC bank1/2 register reset mask */
#define BANK1_2_NPCTL_RESET               ((uint32_t)0x00000008U)
#define BANK1_2_NPINTEN_RESET             ((uint32_t)0x00000042U)
#define BANK1_2_NPCTCFG_RESET             ((uint32_t)0xFFFFFFFFU)
#define BANK1_2_NPATCFG_RESET             ((uint32_t)0xFFFFFFFFU)

/* EXMC bank3 register reset mask */
#define BANK3_NPCTL_RESET                 ((uint32_t)0x00000008U)
#define BANK3_NPINTEN_RESET               ((uint32_t)0x00000040U)
#define BANK3_NPCTCFG_RESET               ((uint32_t)0xFFFFFFFFU)
#define BANK3_NPATCFG_RESET               ((uint32_t)0xFFFFFFFFU)
#define BANK3_PIOTCFG3_RESET              ((uint32_t)0xFFFFFFFFU)

/* EXMC SDRAM device register reset mask */
#define SDRAM_DEVICE_SDCTL_RESET          ((uint32_t)0x000002D0U)
#define SDRAM_DEVICE_SDTCFG_RESET         ((uint32_t)0x0FFFFFFFU)
#define SDRAM_DEVICE_SDCMD_RESET          ((uint32_t)0x00000000U)
#define SDRAM_DEVICE_SDARI_RESET          ((uint32_t)0x00000000U)
#define SDRAM_DEVICE_SDSTAT_RESET         ((uint32_t)0x00000000U)
#define SDRAM_DEVICE_SDRSCTL_RESET        ((uint32_t)0x00000000U)

/* EXMC bank0 SQPI-PSRAM register reset mask */
#define BANK0_SQPI_SINIT_RESET            ((uint32_t)0x18010000U)
#define BANK0_SQPI_SRCMD_RESET            ((uint32_t)0x00000000U)
#define BANK0_SQPI_SWCMD_RESET            ((uint32_t)0x00000000U)
#define BANK0_SQPI_SIDL_RESET             ((uint32_t)0x00000000U)
#define BANK0_SQPI_SIDH_RESET             ((uint32_t)0x00000000U)

/* EXMC register bit offset */
#define SNCTL_NRMUX_OFFSET                ((uint32_t)1U)
#define SNCTL_SBRSTEN_OFFSET              ((uint32_t)8U)
#define SNCTL_WRAPEN_OFFSET               ((uint32_t)10U)
#define SNCTL_WREN_OFFSET                 ((uint32_t)12U)
#define SNCTL_NRWTEN_OFFSET               ((uint32_t)13U)
#define SNCTL_EXMODEN_OFFSET              ((uint32_t)14U)
#define SNCTL_ASYNCWAIT_OFFSET            ((uint32_t)15U)

#define SNTCFG_AHLD_OFFSET                ((uint32_t)4U)
#define SNTCFG_DSET_OFFSET                ((uint32_t)8U)
#define SNTCFG_BUSLAT_OFFSET              ((uint32_t)16U)

#define NPCTL_NDWTEN_OFFSET               ((uint32_t)1U)
#define NPCTL_ECCEN_OFFSET                ((uint32_t)6U)

#define NPCTCFG_COMWAIT_OFFSET            ((uint32_t)8U)
#define NPCTCFG_COMHLD_OFFSET             ((uint32_t)16U)
#define NPCTCFG_COMHIZ_OFFSET             ((uint32_t)24U)

#define NPATCFG_ATTWAIT_OFFSET            ((uint32_t)8U)
#define NPATCFG_ATTHLD_OFFSET             ((uint32_t)16U)
#define NPATCFG_ATTHIZ_OFFSET             ((uint32_t)24U)

#define PIOTCFG_IOWAIT_OFFSET             ((uint32_t)8U)
#define PIOTCFG_IOHLD_OFFSET              ((uint32_t)16U)
#define PIOTCFG_IOHIZ_OFFSET              ((uint32_t)24U)

#define SDCTL_WPEN_OFFSET                 ((uint32_t)9U)
#define SDCTL_BRSTRD_OFFSET               ((uint32_t)12U)

#define SDTCFG_XSRD_OFFSET                ((uint32_t)4U)
#define SDTCFG_RASD_OFFSET                ((uint32_t)8U)
#define SDTCFG_ARFD_OFFSET                ((uint32_t)12U)
#define SDTCFG_WRD_OFFSET                 ((uint32_t)16U)
#define SDTCFG_RPD_OFFSET                 ((uint32_t)20U)
#define SDTCFG_RCD_OFFSET                 ((uint32_t)24U)

#define SDCMD_NARF_OFFSET                 ((uint32_t)5U)
#define SDCMD_MRC_OFFSET                  ((uint32_t)9U)

#define SDARI_ARINTV_OFFSET               ((uint32_t)1U)

#define SDSTAT_STA0_OFFSET                ((uint32_t)1U)
#define SDSTAT_STA1_OFFSET                ((uint32_t)3U)

#define SRCMD_RWAITCYCLE_OFFSET           ((uint32_t)16U)
#define SWCMD_WWAITCYCLE_OFFSET           ((uint32_t)16U)

#define INTEN_INTS_OFFSET                 ((uint32_t)3U)

/*!
    \brief      deinitialize EXMC NOR/SRAM region
    \param[in]  exmc_norsram_region: select the region of bank0
                only one parameter can be selected which is shown as below:
      \arg        EXMC_BANK0_NORSRAM_REGIONx(x=0..3)
    \param[out] none
    \retval     none
*/
void exmc_norsram_deinit(uint32_t exmc_norsram_region)
{
    /* reset the registers */
    EXMC_SNCTL(exmc_norsram_region) = BANK0_SNCTL_RESET;
    EXMC_SNTCFG(exmc_norsram_region) = BANK0_SNTCFG_RESET;
    EXMC_SNWTCFG(exmc_norsram_region) = BANK0_SNWTCFG_RESET;
}

/*!
    \brief      initialize exmc_norsram_parameter_struct with the default values
    \param[in]  none
    \param[out] exmc_norsram_init_struct: the initialized struct exmc_norsram_parameter_struct pointer
    \retval     none
*/
void exmc_norsram_struct_para_init(exmc_norsram_parameter_struct *exmc_norsram_init_struct)
{
    /* configure the structure with default values */
    exmc_norsram_init_struct->norsram_region = EXMC_BANK0_NORSRAM_REGION0;
    exmc_norsram_init_struct->address_data_mux = ENABLE;
    exmc_norsram_init_struct->memory_type = EXMC_MEMORY_TYPE_SRAM;
    exmc_norsram_init_struct->databus_width = EXMC_NOR_DATABUS_WIDTH_8B;
    exmc_norsram_init_struct->burst_mode = DISABLE;
    exmc_norsram_init_struct->nwait_polarity = EXMC_NWAIT_POLARITY_LOW;
    exmc_norsram_init_struct->wrap_burst_mode = DISABLE;
    exmc_norsram_init_struct->nwait_config = EXMC_NWAIT_CONFIG_BEFORE;
    exmc_norsram_init_struct->memory_write = ENABLE;
    exmc_norsram_init_struct->nwait_signal = ENABLE;
    exmc_norsram_init_struct->extended_mode = DISABLE;
    exmc_norsram_init_struct->asyn_wait = DISABLE;
    exmc_norsram_init_struct->write_mode = EXMC_ASYN_WRITE;

    /* configure read/write timing */
    exmc_norsram_init_struct->read_write_timing->asyn_address_setuptime = 0xFU;
    exmc_norsram_init_struct->read_write_timing->asyn_address_holdtime = 0xFU;
    exmc_norsram_init_struct->read_write_timing->asyn_data_setuptime = 0xFFU;
    exmc_norsram_init_struct->read_write_timing->bus_latency = 0xFU;
    exmc_norsram_init_struct->read_write_timing->syn_clk_division = EXMC_SYN_CLOCK_RATIO_16_CLK;
    exmc_norsram_init_struct->read_write_timing->syn_data_latency = EXMC_DATALAT_17_CLK;
    exmc_norsram_init_struct->read_write_timing->asyn_access_mode = EXMC_ACCESS_MODE_A;

    /* configure write timing, when extended mode is used */
    exmc_norsram_init_struct->write_timing->asyn_address_setuptime = 0xFU;
    exmc_norsram_init_struct->write_timing->asyn_address_holdtime = 0xFU;
    exmc_norsram_init_struct->write_timing->asyn_data_setuptime = 0xFFU;
    exmc_norsram_init_struct->write_timing->bus_latency = 0xFU;
    exmc_norsram_init_struct->write_timing->asyn_access_mode = EXMC_ACCESS_MODE_A;
}

/*!
    \brief      initialize EXMC NOR/SRAM region
    \param[in]  exmc_norsram_parameter_struct: configure the EXMC NOR/SRAM parameter
                  norsram_region: EXMC_BANK0_NORSRAM_REGIONx, x=0..3
                  write_mode: EXMC_ASYN_WRITE, EXMC_SYN_WRITE
                  extended_mode: ENABLE or DISABLE
                  asyn_wait: ENABLE or DISABLE
                  nwait_signal: ENABLE or DISABLE
                  memory_write: ENABLE or DISABLE
                  nwait_config: EXMC_NWAIT_CONFIG_BEFORE, EXMC_NWAIT_CONFIG_DURING
                  wrap_burst_mode: ENABLE or DISABLE
                  nwait_polarity: EXMC_NWAIT_POLARITY_LOW, EXMC_NWAIT_POLARITY_HIGH
                  burst_mode: ENABLE or DISABLE
                  databus_width: EXMC_NOR_DATABUS_WIDTH_8B, EXMC_NOR_DATABUS_WIDTH_16B
                  memory_type: EXMC_MEMORY_TYPE_SRAM, EXMC_MEMORY_TYPE_PSRAM, EXMC_MEMORY_TYPE_NOR
                  address_data_mux: ENABLE or DISABLE
                  read_write_timing: struct exmc_norsram_timing_parameter_struct set the time
                    asyn_access_mode: EXMC_ACCESS_MODE_A, EXMC_ACCESS_MODE_B, EXMC_ACCESS_MODE_C, EXMC_ACCESS_MODE_D
                    syn_data_latency: EXMC_DATALAT_x_CLK, x=2..17
                    syn_clk_division: EXMC_SYN_CLOCK_RATIO_DISABLE, EXMC_SYN_CLOCK_RATIO_x_CLK, x=2..16
                    bus_latency: 0x0U~0xFU
                    asyn_data_setuptime: 0x01U~0xFFU
                    asyn_address_holdtime: 0x1U~0xFU
                    asyn_address_setuptime: 0x0U~0xFU
                  write_timing: struct exmc_norsram_timing_parameter_struct set the time
                    asyn_access_mode: EXMC_ACCESS_MODE_A, EXMC_ACCESS_MODE_B, EXMC_ACCESS_MODE_C, EXMC_ACCESS_MODE_D
                    syn_data_latency: EXMC_DATALAT_x_CLK, x=2..17
                    syn_clk_division: EXMC_SYN_CLOCK_RATIO_x_CLK, x=2..16
                    bus_latency: 0x0U~0xFU
                    asyn_data_setuptime: 0x01U~0xFFU
                    asyn_address_holdtime: 0x1U~0xFU
                    asyn_address_setuptime: 0x0U~0xFU
    \param[out] none
    \retval     none
*/
void exmc_norsram_init(exmc_norsram_parameter_struct *exmc_norsram_init_struct)
{
    uint32_t snctl = 0x00000000U, sntcfg = 0x00000000U, snwtcfg = 0x00000000U;

    /* get the register value */
    snctl = EXMC_SNCTL(exmc_norsram_init_struct->norsram_region);

    /* clear relative bits */
    snctl &= ((uint32_t)~(EXMC_SNCTL_NREN | EXMC_SNCTL_NRTP | EXMC_SNCTL_NRW | EXMC_SNCTL_SBRSTEN |
                          EXMC_SNCTL_NRWTPOL | EXMC_SNCTL_WRAPEN | EXMC_SNCTL_NRWTCFG | EXMC_SNCTL_WEN |
                          EXMC_SNCTL_NRWTEN | EXMC_SNCTL_EXMODEN | EXMC_SNCTL_ASYNCWTEN | EXMC_SNCTL_SYNCWR |
                          EXMC_SNCTL_NRMUX));

    /* configure control bits */
    snctl |= (uint32_t)(exmc_norsram_init_struct->address_data_mux << SNCTL_NRMUX_OFFSET) |
             exmc_norsram_init_struct->memory_type |
             exmc_norsram_init_struct->databus_width |
             (exmc_norsram_init_struct->burst_mode << SNCTL_SBRSTEN_OFFSET) |
             exmc_norsram_init_struct->nwait_polarity |
             (exmc_norsram_init_struct->wrap_burst_mode << SNCTL_WRAPEN_OFFSET) |
             exmc_norsram_init_struct->nwait_config |
             (exmc_norsram_init_struct->memory_write << SNCTL_WREN_OFFSET) |
             (exmc_norsram_init_struct->nwait_signal << SNCTL_NRWTEN_OFFSET) |
             (exmc_norsram_init_struct->extended_mode << SNCTL_EXMODEN_OFFSET) |
             (exmc_norsram_init_struct->asyn_wait << SNCTL_ASYNCWAIT_OFFSET) |
             exmc_norsram_init_struct->write_mode;

    /* configure timing */
    sntcfg = (uint32_t)exmc_norsram_init_struct->read_write_timing->asyn_address_setuptime |
             (exmc_norsram_init_struct->read_write_timing->asyn_address_holdtime << SNTCFG_AHLD_OFFSET) |
             (exmc_norsram_init_struct->read_write_timing->asyn_data_setuptime << SNTCFG_DSET_OFFSET) |
             (exmc_norsram_init_struct->read_write_timing->bus_latency << SNTCFG_BUSLAT_OFFSET) |
             exmc_norsram_init_struct->read_write_timing->syn_clk_division |
             exmc_norsram_init_struct->read_write_timing->syn_data_latency |
             exmc_norsram_init_struct->read_write_timing->asyn_access_mode;

    /* enable nor flash access */
    if(EXMC_MEMORY_TYPE_NOR == exmc_norsram_init_struct->memory_type) {
        snctl |= (uint32_t)EXMC_SNCTL_NREN;
    }

    /* configure extended mode */
    if(ENABLE == exmc_norsram_init_struct->extended_mode) {
        snwtcfg = (uint32_t)exmc_norsram_init_struct->write_timing->asyn_address_setuptime |
                  (exmc_norsram_init_struct->write_timing->asyn_address_holdtime << SNTCFG_AHLD_OFFSET) |
                  (exmc_norsram_init_struct->write_timing->asyn_data_setuptime << SNTCFG_DSET_OFFSET) |
                  (exmc_norsram_init_struct->write_timing->bus_latency << SNTCFG_BUSLAT_OFFSET) |
                  exmc_norsram_init_struct->write_timing->asyn_access_mode;
    } else {
        snwtcfg = BANK0_SNWTCFG_RESET;
    }

    /* configure the registers */
    EXMC_SNCTL(exmc_norsram_init_struct->norsram_region) = snctl;
    EXMC_SNTCFG(exmc_norsram_init_struct->norsram_region) = sntcfg;
    EXMC_SNWTCFG(exmc_norsram_init_struct->norsram_region) = snwtcfg;
}

/*!
    \brief      enable EXMC NOR/PSRAM bank region
    \param[in]  exmc_norsram_region: specify the region of NOR/PSRAM bank
                only one parameter can be selected which is shown as below:
      \arg        EXMC_BANK0_NORSRAM_REGIONx(x=0..3)
    \param[out] none
    \retval     none
*/
void exmc_norsram_enable(uint32_t exmc_norsram_region)
{
    EXMC_SNCTL(exmc_norsram_region) |= (uint32_t)EXMC_SNCTL_NRBKEN;
}

/*!
    \brief      disable EXMC NOR/PSRAM bank region
    \param[in]  exmc_norsram_region: specify the region of NOR/PSRAM Bank
                only one parameter can be selected which is shown as below:
      \arg        EXMC_BANK0_NORSRAM_REGIONx(x=0..3)
    \param[out] none
    \retval     none
*/
void exmc_norsram_disable(uint32_t exmc_norsram_region)
{
    EXMC_SNCTL(exmc_norsram_region) &= ~(uint32_t)EXMC_SNCTL_NRBKEN;
}

/*!
    \brief      deinitialize EXMC NAND bank
    \param[in]  exmc_nand_bank: select the bank of NAND
                only one parameter can be selected which is shown as below:
      \arg        EXMC_BANKx_NAND(x=1..2)
    \param[out] none
    \retval     none
*/
void exmc_nand_deinit(uint32_t exmc_nand_bank)
{
    /* EXMC_BANK1_NAND or EXMC_BANK2_NAND */
    EXMC_NPCTL(exmc_nand_bank) = BANK1_2_NPCTL_RESET;
    EXMC_NPINTEN(exmc_nand_bank) = BANK1_2_NPINTEN_RESET;
    EXMC_NPCTCFG(exmc_nand_bank) = BANK1_2_NPCTCFG_RESET;
    EXMC_NPATCFG(exmc_nand_bank) = BANK1_2_NPATCFG_RESET;
}

/*!
    \brief      initialize exmc_norsram_parameter_struct with the default values
    \param[in]  none
    \param[out] the initialized struct exmc_norsram_parameter_struct pointer
    \retval     none
*/
void exmc_nand_struct_para_init(exmc_nand_parameter_struct *exmc_nand_init_struct)
{
    /* configure the structure with default values */
    exmc_nand_init_struct->nand_bank = EXMC_BANK1_NAND;
    exmc_nand_init_struct->wait_feature = DISABLE;
    exmc_nand_init_struct->databus_width = EXMC_NAND_DATABUS_WIDTH_8B;
    exmc_nand_init_struct->ecc_logic = DISABLE;
    exmc_nand_init_struct->ecc_size = EXMC_ECC_SIZE_256BYTES;
    exmc_nand_init_struct->ctr_latency = 0x0U;
    exmc_nand_init_struct->atr_latency = 0x0U;
    exmc_nand_init_struct->common_space_timing->setuptime = 0xFCU;
    exmc_nand_init_struct->common_space_timing->waittime = 0xFCU;
    exmc_nand_init_struct->common_space_timing->holdtime = 0xFCU;
    exmc_nand_init_struct->common_space_timing->databus_hiztime = 0xFCU;
    exmc_nand_init_struct->attribute_space_timing->setuptime = 0xFCU;
    exmc_nand_init_struct->attribute_space_timing->waittime = 0xFCU;
    exmc_nand_init_struct->attribute_space_timing->holdtime = 0xFCU;
    exmc_nand_init_struct->attribute_space_timing->databus_hiztime = 0xFCU;
}

/*!
    \brief      initialize EXMC NAND bank
    \param[in]  exmc_nand_parameter_struct: configure the EXMC NAND parameter
                  nand_bank: EXMC_BANK1_NAND,EXMC_BANK2_NAND
                  ecc_size: EXMC_ECC_SIZE_xBYTES,x=256,512,1024,2048,4096
                  atr_latency: EXMC_ALE_RE_DELAY_x_HCLK,x=1..16
                  ctr_latency: EXMC_CLE_RE_DELAY_x_HCLK,x=1..16
                  ecc_logic: ENABLE or DISABLE
                  databus_width: EXMC_NAND_DATABUS_WIDTH_8B,EXMC_NAND_DATABUS_WIDTH_16B
                  wait_feature: ENABLE or DISABLE
                  common_space_timing: struct exmc_nand_pccard_timing_parameter_struct set the time
                    databus_hiztime: 0x01U~0xFFU
                    holdtime: 0x01U~0xFEU
                    waittime: 0x02U~0xFFU
                    setuptime: 0x01U~0xFFU
                  attribute_space_timing: struct exmc_nand_pccard_timing_parameter_struct set the time
                    databus_hiztime: 0x00U~0xFEU
                    holdtime: 0x01U~0xFEU
                    waittime: 0x02U~0xFFU
                    setuptime: 0x01U~0xFFU
    \param[out] none
    \retval     none
*/
void exmc_nand_init(exmc_nand_parameter_struct *exmc_nand_init_struct)
{
    uint32_t npctl = 0x00000000U, npctcfg = 0x00000000U, npatcfg = 0x00000000U;

    npctl = (uint32_t)(exmc_nand_init_struct->wait_feature << NPCTL_NDWTEN_OFFSET) |
            EXMC_NPCTL_NDTP |
            exmc_nand_init_struct->databus_width |
            (exmc_nand_init_struct->ecc_logic << NPCTL_ECCEN_OFFSET) |
            exmc_nand_init_struct->ecc_size |
            exmc_nand_init_struct->ctr_latency |
            exmc_nand_init_struct->atr_latency;

    npctcfg = (uint32_t)((exmc_nand_init_struct->common_space_timing->setuptime - 1U) & EXMC_NPCTCFG_COMSET) |
              (((exmc_nand_init_struct->common_space_timing->waittime - 1U) << NPCTCFG_COMWAIT_OFFSET) & EXMC_NPCTCFG_COMWAIT) |
              ((exmc_nand_init_struct->common_space_timing->holdtime << NPCTCFG_COMHLD_OFFSET) & EXMC_NPCTCFG_COMHLD) |
              (((exmc_nand_init_struct->common_space_timing->databus_hiztime - 1U) << NPCTCFG_COMHIZ_OFFSET) & EXMC_NPCTCFG_COMHIZ);

    npatcfg = (uint32_t)((exmc_nand_init_struct->attribute_space_timing->setuptime - 1U) & EXMC_NPATCFG_ATTSET) |
              (((exmc_nand_init_struct->attribute_space_timing->waittime - 1U) << NPATCFG_ATTWAIT_OFFSET) & EXMC_NPATCFG_ATTWAIT) |
              ((exmc_nand_init_struct->attribute_space_timing->holdtime << NPATCFG_ATTHLD_OFFSET) & EXMC_NPATCFG_ATTHLD) |
              ((exmc_nand_init_struct->attribute_space_timing->databus_hiztime << NPATCFG_ATTHIZ_OFFSET) & EXMC_NPATCFG_ATTHIZ);

    /* initialize EXMC_BANK1_NAND or EXMC_BANK2_NAND */
    EXMC_NPCTL(exmc_nand_init_struct->nand_bank) = npctl;
    EXMC_NPCTCFG(exmc_nand_init_struct->nand_bank) = npctcfg;
    EXMC_NPATCFG(exmc_nand_init_struct->nand_bank) = npatcfg;
}

/*!
    \brief      enable NAND bank
    \param[in]  exmc_nand_bank: specify the NAND bank
                only one parameter can be selected which is shown as below:
      \arg        EXMC_BANKx_NAND(x=1,2)
    \param[out] none
    \retval     none
*/
void exmc_nand_enable(uint32_t exmc_nand_bank)
{
    EXMC_NPCTL(exmc_nand_bank) |= EXMC_NPCTL_NDBKEN;
}

/*!
    \brief      disable NAND bank
    \param[in]  exmc_nand_bank: specify the NAND bank
                only one parameter can be selected which is shown as below:
      \arg        EXMC_BANKx_NAND(x=1,2)
    \param[out] none
    \retval     none
*/
void exmc_nand_disable(uint32_t exmc_nand_bank)
{
    EXMC_NPCTL(exmc_nand_bank) &= ~EXMC_NPCTL_NDBKEN;
}

/*!
    \brief      deinitialize EXMC PC card bank
    \param[in]  none
    \param[out] none
    \retval     none
*/
void exmc_pccard_deinit(void)
{
    /* EXMC_BANK3_PCCARD */
    EXMC_NPCTL3 = BANK3_NPCTL_RESET;
    EXMC_NPINTEN3 = BANK3_NPINTEN_RESET;
    EXMC_NPCTCFG3 = BANK3_NPCTCFG_RESET;
    EXMC_NPATCFG3 = BANK3_NPATCFG_RESET;
    EXMC_PIOTCFG3 = BANK3_PIOTCFG3_RESET;
}

/*!
    \brief      initialize exmc_pccard_parameter_struct with the default values
    \param[in]  none
    \param[out] the initialized struct exmc_pccard_parameter_struct pointer
    \retval     none
*/
void exmc_pccard_struct_para_init(exmc_pccard_parameter_struct *exmc_pccard_init_struct)
{
    /* configure the structure with default values */
    exmc_pccard_init_struct->wait_feature = DISABLE;
    exmc_pccard_init_struct->ctr_latency = 0x0U;
    exmc_pccard_init_struct->atr_latency = 0x0U;
    exmc_pccard_init_struct->common_space_timing->setuptime = 0xFCU;
    exmc_pccard_init_struct->common_space_timing->waittime = 0xFCU;
    exmc_pccard_init_struct->common_space_timing->holdtime = 0xFCU;
    exmc_pccard_init_struct->common_space_timing->databus_hiztime = 0xFCU;
    exmc_pccard_init_struct->attribute_space_timing->setuptime = 0xFCU;
    exmc_pccard_init_struct->attribute_space_timing->waittime = 0xFCU;
    exmc_pccard_init_struct->attribute_space_timing->holdtime = 0xFCU;
    exmc_pccard_init_struct->attribute_space_timing->databus_hiztime = 0xFCU;
    exmc_pccard_init_struct->io_space_timing->setuptime = 0xFCU;
    exmc_pccard_init_struct->io_space_timing->waittime = 0xFCU;
    exmc_pccard_init_struct->io_space_timing->holdtime = 0xFCU;
    exmc_pccard_init_struct->io_space_timing->databus_hiztime = 0xFCU;
}

/*!
    \brief      initialize EXMC PC card bank
    \param[in]  exmc_pccard_parameter_struct: configure the EXMC NAND parameter
                  atr_latency: EXMC_ALE_RE_DELAY_x_HCLK,x=1..16
                  ctr_latency: EXMC_CLE_RE_DELAY_x_HCLK,x=1..16
                  wait_feature: ENABLE or DISABLE
                  common_space_timing: struct exmc_nand_pccard_timing_parameter_struct set the time
                    databus_hiztime: 0x01U~0xFFU
                    holdtime: 0x01U~0xFEU
                    waittime: 0x02U~0xFFU
                    setuptime: 0x01U~0xFFU
                  attribute_space_timing: struct exmc_nand_pccard_timing_parameter_struct set the time
                    databus_hiztime: 0x00U~0xFEU
                    holdtime: 0x01U~0xFEU
                    waittime: 0x02U~0xFFU
                    setuptime: 0x01U~0xFFU
                  io_space_timing: exmc_nand_pccard_timing_parameter_struct set the time
                    databus_hiztime: 0x00U~0xFFU
                    holdtime: 0x01U~0xFFU
                    waittime: 0x02U~0x100U
                    setuptime: 0x01U~0x100U
    \param[out] none
    \retval     none
*/
void exmc_pccard_init(exmc_pccard_parameter_struct *exmc_pccard_init_struct)
{
    /* configure the EXMC bank3 PC card control register */
    EXMC_NPCTL3 = (uint32_t)(exmc_pccard_init_struct->wait_feature << NPCTL_NDWTEN_OFFSET) |
                  EXMC_NAND_DATABUS_WIDTH_16B |
                  exmc_pccard_init_struct->ctr_latency |
                  exmc_pccard_init_struct->atr_latency ;

    /* configure the EXMC bank3 PC card common space timing configuration register */
    EXMC_NPCTCFG3 = (uint32_t)((exmc_pccard_init_struct->common_space_timing->setuptime - 1U) & EXMC_NPCTCFG_COMSET) |
                    (((exmc_pccard_init_struct->common_space_timing->waittime - 1U) << NPCTCFG_COMWAIT_OFFSET) & EXMC_NPCTCFG_COMWAIT) |
                    ((exmc_pccard_init_struct->common_space_timing->holdtime << NPCTCFG_COMHLD_OFFSET) & EXMC_NPCTCFG_COMHLD) |
                    (((exmc_pccard_init_struct->common_space_timing->databus_hiztime - 1U) << NPCTCFG_COMHIZ_OFFSET) & EXMC_NPCTCFG_COMHIZ);

    /* configure the EXMC bank3 PC card attribute space timing configuration register */
    EXMC_NPATCFG3 = (uint32_t)((exmc_pccard_init_struct->attribute_space_timing->setuptime - 1U) & EXMC_NPATCFG_ATTSET) |
                    (((exmc_pccard_init_struct->attribute_space_timing->waittime - 1U) << NPATCFG_ATTWAIT_OFFSET) & EXMC_NPATCFG_ATTWAIT) |
                    ((exmc_pccard_init_struct->attribute_space_timing->holdtime << NPATCFG_ATTHLD_OFFSET) & EXMC_NPATCFG_ATTHLD) |
                    ((exmc_pccard_init_struct->attribute_space_timing->databus_hiztime << NPATCFG_ATTHIZ_OFFSET) & EXMC_NPATCFG_ATTHIZ);

    /* configure the EXMC bank3 PC card io space timing configuration register */
    EXMC_PIOTCFG3 = (uint32_t)((exmc_pccard_init_struct->io_space_timing->setuptime - 1U) & EXMC_PIOTCFG3_IOSET) |
                    (((exmc_pccard_init_struct->io_space_timing->waittime - 1U) << PIOTCFG_IOWAIT_OFFSET) & EXMC_PIOTCFG3_IOWAIT) |
                    ((exmc_pccard_init_struct->io_space_timing->holdtime << PIOTCFG_IOHLD_OFFSET) & EXMC_PIOTCFG3_IOHLD) |
                    ((exmc_pccard_init_struct->io_space_timing->databus_hiztime << PIOTCFG_IOHIZ_OFFSET) & EXMC_PIOTCFG3_IOHIZ);
}

/*!
    \brief      enable PC Card Bank
    \param[in]  none
    \param[out] none
    \retval     none
*/
void exmc_pccard_enable(void)
{
    EXMC_NPCTL3 |= EXMC_NPCTL_NDBKEN;
}

/*!
    \brief      disable PC Card Bank
    \param[in]  none
    \param[out] none
    \retval     none
*/
void exmc_pccard_disable(void)
{
    EXMC_NPCTL3 &= ~EXMC_NPCTL_NDBKEN;
}

/*!
    \brief      deinitialize EXMC SDRAM device
   \param[in]   exmc_sdram_device: select the SRAM device
                only one parameter can be selected which is shown as below:
      \arg        EXMC_SDRAM_DEVICEx(x=0, 1)
    \param[in]  none
    \param[out] none
    \retval     none
*/
void exmc_sdram_deinit(uint32_t exmc_sdram_device)
{
    /* reset SDRAM registers */
    EXMC_SDCTL(exmc_sdram_device) = SDRAM_DEVICE_SDCTL_RESET;
    EXMC_SDTCFG(exmc_sdram_device) = SDRAM_DEVICE_SDTCFG_RESET;
    EXMC_SDCMD = SDRAM_DEVICE_SDCMD_RESET;
    EXMC_SDARI = SDRAM_DEVICE_SDARI_RESET;
    EXMC_SDRSCTL = SDRAM_DEVICE_SDRSCTL_RESET;
}

/*!
    \brief      initialize exmc_sdram_parameter_struct with the default values
    \param[in]  none
    \param[out] the initialized struct exmc_pccard_parameter_struct pointer
    \retval     none
*/
void exmc_sdram_struct_para_init(exmc_sdram_parameter_struct *exmc_sdram_init_struct)
{
    /* configure the structure with default values */
    exmc_sdram_init_struct->sdram_device = EXMC_SDRAM_DEVICE0;
    exmc_sdram_init_struct->column_address_width = EXMC_SDRAM_COW_ADDRESS_8;
    exmc_sdram_init_struct->row_address_width = EXMC_SDRAM_ROW_ADDRESS_11;
    exmc_sdram_init_struct->data_width = EXMC_SDRAM_DATABUS_WIDTH_16B;
    exmc_sdram_init_struct->internal_bank_number = EXMC_SDRAM_4_INTER_BANK;
    exmc_sdram_init_struct->cas_latency = EXMC_CAS_LATENCY_1_SDCLK;
    exmc_sdram_init_struct->write_protection = ENABLE;
    exmc_sdram_init_struct->sdclock_config = EXMC_SDCLK_DISABLE;
    exmc_sdram_init_struct->burst_read_switch = DISABLE;
    exmc_sdram_init_struct->pipeline_read_delay = EXMC_PIPELINE_DELAY_0_HCLK;

    exmc_sdram_init_struct->timing->load_mode_register_delay = 16U;
    exmc_sdram_init_struct->timing->exit_selfrefresh_delay = 16U;
    exmc_sdram_init_struct->timing->row_address_select_delay = 16U;
    exmc_sdram_init_struct->timing->auto_refresh_delay = 16U;
    exmc_sdram_init_struct->timing->write_recovery_delay = 16U;
    exmc_sdram_init_struct->timing->row_precharge_delay = 16U;
    exmc_sdram_init_struct->timing->row_to_column_delay = 16U;
}

/*!
    \brief      initialize EXMC SDRAM device
    \param[in]  exmc_sdram_parameter_struct: configure the EXMC SDRAM parameter
                  sdram_device: EXMC_SDRAM_DEVICE0,EXMC_SDRAM_DEVICE1
                  pipeline_read_delay: EXMC_PIPELINE_DELAY_x_HCLK,x=0..2
                  burst_read_switch: ENABLE or DISABLE
                  sdclock_config: EXMC_SDCLK_DISABLE,EXMC_SDCLK_PERIODS_2_HCLK,EXMC_SDCLK_PERIODS_3_HCLK
                  write_protection: ENABLE or DISABLE
                  cas_latency: EXMC_CAS_LATENCY_x_SDCLK,x=1..3
                  internal_bank_number: EXMC_SDRAM_2_INTER_BANK,EXMC_SDRAM_4_INTER_BANK
                  data_width: EXMC_SDRAM_DATABUS_WIDTH_8B,EXMC_SDRAM_DATABUS_WIDTH_16B,EXMC_SDRAM_DATABUS_WIDTH_32B
                  row_address_width: EXMC_SDRAM_ROW_ADDRESS_x,x=11..13
                  column_address_width: EXMC_SDRAM_COW_ADDRESS_x,x=8..11
                  timing: exmc_sdram_timing_parameter_struct set the time
                    row_to_column_delay: 1U~16U
                    row_precharge_delay: 1U~16U
                    write_recovery_delay: 1U~16U
                    auto_refresh_delay: 1U~16U
                    row_address_select_delay: 1U~16U
                    exit_selfrefresh_delay: 1U~16U
                    load_mode_register_delay: 1U~16U
    \param[out] none
    \retval     none
*/
void exmc_sdram_init(exmc_sdram_parameter_struct *exmc_sdram_init_struct)
{
    uint32_t sdctl0, sdctl1, sdtcfg0, sdtcfg1;

    /* configure EXMC_SDCTL0 or EXMC_SDCTL1 */
    if(EXMC_SDRAM_DEVICE0 == exmc_sdram_init_struct->sdram_device) {
        /* configure EXMC_SDCTL0 */
        EXMC_SDCTL(EXMC_SDRAM_DEVICE0)  = (uint32_t)(exmc_sdram_init_struct->column_address_width |
                                                     exmc_sdram_init_struct->row_address_width |
                                                     exmc_sdram_init_struct->data_width |
                                                     exmc_sdram_init_struct->internal_bank_number |
                                                     exmc_sdram_init_struct->cas_latency |
                                                    (exmc_sdram_init_struct->write_protection << SDCTL_WPEN_OFFSET) |
                                                     exmc_sdram_init_struct->sdclock_config |
                                                    (exmc_sdram_init_struct->burst_read_switch << SDCTL_BRSTRD_OFFSET) | 
                                                     exmc_sdram_init_struct->pipeline_read_delay);
        
        /* configure EXMC_SDTCFG0 */
        EXMC_SDTCFG(EXMC_SDRAM_DEVICE0) = (uint32_t)((exmc_sdram_init_struct->timing->load_mode_register_delay) - 1U) |
                                          (((exmc_sdram_init_struct->timing->exit_selfrefresh_delay) - 1U) << SDTCFG_XSRD_OFFSET) |
                                          (((exmc_sdram_init_struct->timing->row_address_select_delay) - 1U) << SDTCFG_RASD_OFFSET) |
                                          (((exmc_sdram_init_struct->timing->auto_refresh_delay) - 1U) << SDTCFG_ARFD_OFFSET) |
                                          (((exmc_sdram_init_struct->timing->write_recovery_delay) - 1U) << SDTCFG_WRD_OFFSET) |
                                          (((exmc_sdram_init_struct->timing->row_precharge_delay) - 1U) << SDTCFG_RPD_OFFSET) |
                                          (((exmc_sdram_init_struct->timing->row_to_column_delay) - 1U) << SDTCFG_RCD_OFFSET);
    } else {
        /* configure EXMC_SDCTL0 and EXMC_SDCTL1 */
        /* some bits in the EXMC_SDCTL1 register are reserved */
        sdctl0 = EXMC_SDCTL(EXMC_SDRAM_DEVICE0) & (~(EXMC_SDCTL_PIPED | EXMC_SDCTL_BRSTRD | EXMC_SDCTL_SDCLK));

        sdctl0 |= (uint32_t)(exmc_sdram_init_struct->sdclock_config |
                             (exmc_sdram_init_struct->burst_read_switch << SDCTL_BRSTRD_OFFSET) | 
                             exmc_sdram_init_struct->pipeline_read_delay);
        
        sdctl1 = (uint32_t)(exmc_sdram_init_struct->column_address_width |
                            exmc_sdram_init_struct->row_address_width |
                            exmc_sdram_init_struct->data_width |
                            exmc_sdram_init_struct->internal_bank_number |
                            exmc_sdram_init_struct->cas_latency |
                            (exmc_sdram_init_struct->write_protection << SDCTL_WPEN_OFFSET));

        EXMC_SDCTL(EXMC_SDRAM_DEVICE0) = sdctl0;
        EXMC_SDCTL(EXMC_SDRAM_DEVICE1) = sdctl1;

        /* configure EXMC_SDTCFG0 and EXMC_SDTCFG1 */
        /* some bits in the EXMC_SDTCFG1 register are reserved */
        sdtcfg0 = EXMC_SDTCFG(EXMC_SDRAM_DEVICE0) & (~(EXMC_SDTCFG_RPD | EXMC_SDTCFG_WRD | EXMC_SDTCFG_ARFD));

        sdtcfg0 |= (uint32_t)((((exmc_sdram_init_struct->timing->auto_refresh_delay) - 1U) << SDTCFG_ARFD_OFFSET) |
                              (((exmc_sdram_init_struct->timing->row_precharge_delay) - 1U) << SDTCFG_RPD_OFFSET) |
                              (((exmc_sdram_init_struct->timing->write_recovery_delay) - 1U) << SDTCFG_WRD_OFFSET));

        sdtcfg1 = (uint32_t)(((exmc_sdram_init_struct->timing->load_mode_register_delay) - 1U) |
                            (((exmc_sdram_init_struct->timing->exit_selfrefresh_delay) - 1U) << SDTCFG_XSRD_OFFSET) |
                            (((exmc_sdram_init_struct->timing->row_address_select_delay) - 1U) << SDTCFG_RASD_OFFSET) |
                            (((exmc_sdram_init_struct->timing->row_to_column_delay) - 1U) << SDTCFG_RCD_OFFSET));

        EXMC_SDTCFG(EXMC_SDRAM_DEVICE0) = sdtcfg0;
        EXMC_SDTCFG(EXMC_SDRAM_DEVICE1) = sdtcfg1;
    }
}

/*!
    \brief      initialize exmc_sdram_struct_command_para_init with the default values
    \param[in]  none
    \param[out] the initialized struct exmc_sdram_struct_command_para_init pointer
    \retval     none
*/
void exmc_sdram_struct_command_para_init(exmc_sdram_command_parameter_struct *exmc_sdram_command_init_struct)
{
    /* configure the structure with default value */
    exmc_sdram_command_init_struct->mode_register_content = 0U;
    exmc_sdram_command_init_struct->auto_refresh_number = EXMC_SDRAM_AUTO_REFLESH_1_SDCLK;
    exmc_sdram_command_init_struct->bank_select = EXMC_SDRAM_DEVICE0_SELECT;
    exmc_sdram_command_init_struct->command = EXMC_SDRAM_NORMAL_OPERATION;
}

/*!
    \brief      deinitialize exmc SQPIPSRAM
    \param[in]  none
    \param[out] none
    \retval     none
*/
void exmc_sqpipsram_deinit(void)
{
    /* reset the registers */
    EXMC_SINIT = BANK0_SQPI_SINIT_RESET;
    EXMC_SRCMD = BANK0_SQPI_SRCMD_RESET;
    EXMC_SWCMD = BANK0_SQPI_SWCMD_RESET;
    EXMC_SIDL = BANK0_SQPI_SIDL_RESET;
    EXMC_SIDH = BANK0_SQPI_SIDH_RESET;
}

/*!
    \brief      initialize exmc_sqpipsram_parameter_struct with the default values
    \param[in]  the struct exmc_sqpipsram_parameter_struct pointer
    \param[out] none
    \retval     none
*/
void exmc_sqpipsram_struct_para_init(exmc_sqpipsram_parameter_struct *exmc_sqpipsram_init_struct)
{
    /* configure the structure with default values */
    exmc_sqpipsram_init_struct->sample_polarity = EXMC_SQPIPSRAM_SAMPLE_RISING_EDGE;
    exmc_sqpipsram_init_struct->id_length = EXMC_SQPIPSRAM_ID_LENGTH_64B;
    exmc_sqpipsram_init_struct->address_bits = EXMC_SQPIPSRAM_ADDR_LENGTH_24B;
    exmc_sqpipsram_init_struct->command_bits = EXMC_SQPIPSRAM_COMMAND_LENGTH_8B;
}

/*!
    \brief      initialize EXMC SQPIPSRAM
    \param[in]  exmc_sqpipsram_parameter_struct: configure the EXMC SQPIPSRAM parameter
                  sample_polarity: EXMC_SQPIPSRAM_SAMPLE_RISING_EDGE,EXMC_SQPIPSRAM_SAMPLE_FALLING_EDGE
                  id_length: EXMC_SQPIPSRAM_ID_LENGTH_xB,x=8,16,32,64
                  address_bits: EXMC_SQPIPSRAM_ADDR_LENGTH_xB,x=1..26
                  command_bits: EXMC_SQPIPSRAM_COMMAND_LENGTH_xB,x=4,8,16
    \param[out] none
    \retval     none
*/
void exmc_sqpipsram_init(exmc_sqpipsram_parameter_struct *exmc_sqpipsram_init_struct)
{
    /* initialize SQPI controller */
    EXMC_SINIT = (uint32_t)exmc_sqpipsram_init_struct->sample_polarity |
                 exmc_sqpipsram_init_struct->id_length |
                 exmc_sqpipsram_init_struct->address_bits |
                 exmc_sqpipsram_init_struct->command_bits;
}

/*!
    \brief      configure consecutive clock
    \param[in]  clock_mode: specify when the clock is generated
                only one parameter can be selected which is shown as below:
      \arg        EXMC_CLOCK_SYN_MODE: the clock is generated only during synchronous access
      \arg        EXMC_CLOCK_UNCONDITIONALLY: the clock is generated unconditionally
    \param[out] none
    \retval     none
*/
void exmc_norsram_consecutive_clock_config(uint32_t clock_mode)
{
    if(EXMC_CLOCK_UNCONDITIONALLY == clock_mode) {
        EXMC_SNCTL(EXMC_BANK0_NORSRAM_REGION0) |= EXMC_CLOCK_UNCONDITIONALLY;
    } else {
        EXMC_SNCTL(EXMC_BANK0_NORSRAM_REGION0) &= ~EXMC_CLOCK_UNCONDITIONALLY;
    }
}

/*!
    \brief      configure CRAM page size
    \param[in]  exmc_norsram_region: select the region of bank0
                only one parameter can be selected which is shown as below:
      \arg        EXMC_BANK0_NORSRAM_REGIONx(x=0..3)
    \param[in]  page_size: CRAM page size
                only one parameter can be selected which is shown as below:
      \arg        EXMC_CRAM_AUTO_SPLIT: the clock is generated only during synchronous access
      \arg        EXMC_CRAM_PAGE_SIZE_128_BYTES: page size is 128 bytes
      \arg        EXMC_CRAM_PAGE_SIZE_256_BYTES: page size is 256 bytes
      \arg        EXMC_CRAM_PAGE_SIZE_512_BYTES: page size is 512 bytes
      \arg        EXMC_CRAM_PAGE_SIZE_1024_BYTES: page size is 1024 bytes
    \param[out] none
    \retval     none
*/
void exmc_norsram_page_size_config(uint32_t exmc_norsram_region, uint32_t page_size)
{
    /* reset the bits */
    EXMC_SNCTL(exmc_norsram_region) &= ~EXMC_SNCTL_CPS;

    /* set the CPS bits */
    EXMC_SNCTL(exmc_norsram_region) |= page_size;
}

/*!
    \brief      enable or disable the EXMC NAND ECC function
    \param[in]  exmc_nand_bank: specify the NAND bank
                only one parameter can be selected which is shown as below:
      \arg        EXMC_BANKx_NAND(x=1,2)
    \param[in]  newvalue: ENABLE or DISABLE
    \param[out] none
    \retval     none
*/
void exmc_nand_ecc_config(uint32_t exmc_nand_bank, ControlStatus newvalue)
{
    if(ENABLE == newvalue) {
        /* enable the selected NAND bank ECC function */
        EXMC_NPCTL(exmc_nand_bank) |= EXMC_NPCTL_ECCEN;
    } else {
        /* disable the selected NAND bank ECC function */
        EXMC_NPCTL(exmc_nand_bank) &= ~EXMC_NPCTL_ECCEN;
    }
}

/*!
    \brief      get the EXMC ECC value
    \param[in]  exmc_nand_bank: specify the NAND bank
                only one parameter can be selected which is shown as below:
      \arg        EXMC_BANKx_NAND(x=1,2)
    \param[out] none
    \retval     the error correction code(ECC) value
*/
uint32_t exmc_ecc_get(uint32_t exmc_nand_bank)
{
    return(EXMC_NECC(exmc_nand_bank));
}

/*!
    \brief      enable or disable read sample
    \param[in]  newvalue: ENABLE or DISABLE
    \param[out] none
    \retval     none
*/
void exmc_sdram_readsample_enable(ControlStatus newvalue)
{
    if(ENABLE == newvalue) {
        EXMC_SDRSCTL |=  EXMC_SDRSCTL_RSEN;
    } else {
        EXMC_SDRSCTL &= (uint32_t)(~EXMC_SDRSCTL_RSEN);
    }
}

/*!
    \brief      configure the delayed sample clock of read data
    \param[in]  delay_cell: SDRAM the delayed sample clock of read data
                only one parameter can be selected which is shown as below:
      \arg        EXMC_SDRAM_x_DELAY_CELL(x=0..15)
    \param[in]  extra_hclk: sample cycle of read data
                only one parameter can be selected which is shown as below:
      \arg        EXMC_SDRAM_READSAMPLE_x_EXTRAHCLK(x=0,1)
    \param[out] none
    \retval     none
*/
void exmc_sdram_readsample_config(uint32_t delay_cell, uint32_t extra_hclk)
{
    uint32_t sdrsctl = 0U;

    /* reset the bits */
    sdrsctl = EXMC_SDRSCTL & (~(EXMC_SDRSCTL_SDSC | EXMC_SDRSCTL_SSCR));
    /* set the bits */
    sdrsctl |= (uint32_t)(delay_cell | extra_hclk);
    EXMC_SDRSCTL = sdrsctl;
}

/*!
    \brief      configure the SDRAM memory command
    \param[in]  exmc_sdram_command_init_struct: initialize EXMC SDRAM command
                  mode_register_content:
                  auto_refresh_number: EXMC_SDRAM_AUTO_REFLESH_x_SDCLK, x=1..15
                  bank_select: EXMC_SDRAM_DEVICE0_SELECT, EXMC_SDRAM_DEVICE1_SELECT, EXMC_SDRAM_DEVICE0_1_SELECT
                  command: EXMC_SDRAM_NORMAL_OPERATION, EXMC_SDRAM_CLOCK_ENABLE, EXMC_SDRAM_PRECHARGE_ALL,
                           EXMC_SDRAM_AUTO_REFRESH, EXMC_SDRAM_LOAD_MODE_REGISTER, EXMC_SDRAM_SELF_REFRESH,
                           EXMC_SDRAM_POWERDOWN_ENTRY
    \param[out] none
    \retval     none
*/
void exmc_sdram_command_config(exmc_sdram_command_parameter_struct *exmc_sdram_command_init_struct)
{
    /* configure command register */
    EXMC_SDCMD = (uint32_t)((exmc_sdram_command_init_struct->command) |
                            (exmc_sdram_command_init_struct->bank_select) |
                            ((exmc_sdram_command_init_struct->auto_refresh_number)) |
                            ((exmc_sdram_command_init_struct->mode_register_content) << SDCMD_MRC_OFFSET));
}

/*!
    \brief      set auto-refresh interval
    \param[in]  exmc_count: the number SDRAM clock cycles unit between two successive auto-refresh commands, 0x0000~0x1FFF
    \param[out] none
    \retval     none
*/
void exmc_sdram_refresh_count_set(uint32_t exmc_count)
{
    uint32_t sdari;
    sdari = EXMC_SDARI & (~EXMC_SDARI_ARINTV);
    EXMC_SDARI = sdari | (uint32_t)((exmc_count << SDARI_ARINTV_OFFSET) & EXMC_SDARI_ARINTV);
}

/*!
    \brief      set the number of successive auto-refresh command
    \param[in]  exmc_number: the number of successive Auto-refresh cycles will be send, 1~15
    \param[out] none
    \retval     none
*/
void exmc_sdram_autorefresh_number_set(uint32_t exmc_number)
{
    uint32_t sdcmd;
    sdcmd = EXMC_SDCMD & (~EXMC_SDCMD_NARF);
    EXMC_SDCMD = sdcmd | (uint32_t)((exmc_number << SDCMD_NARF_OFFSET) & EXMC_SDCMD_NARF) ;
}

/*!
    \brief      configure the write protection function
    \param[in]  exmc_sdram_device: specify the SDRAM device
                only one parameter can be selected which is shown as below:
      \arg        EXMC_SDRAM_DEVICEx(x=0,1)
    \param[in]  newvalue: ENABLE or DISABLE
    \param[out] none
    \retval     none
*/
void exmc_sdram_write_protection_config(uint32_t exmc_sdram_device, ControlStatus newvalue)
{
    if(ENABLE == newvalue) {
        EXMC_SDCTL(exmc_sdram_device) |= (uint32_t)EXMC_SDCTL_WPEN;
    } else {
        EXMC_SDCTL(exmc_sdram_device) &= ~((uint32_t)EXMC_SDCTL_WPEN);
    }

}

/*!
    \brief      get the status of SDRAM device0 or device1
    \param[in]  exmc_sdram_device: specify the SDRAM device
                only one parameter can be selected which is shown as below:
      \arg        EXMC_SDRAM_DEVICEx(x=0,1)
    \param[out] none
    \retval     the status of SDRAM device
*/
uint32_t exmc_sdram_bankstatus_get(uint32_t exmc_sdram_device)
{
    uint32_t sdstat = 0U;

    if(EXMC_SDRAM_DEVICE0 == exmc_sdram_device) {
        sdstat = ((uint32_t)(EXMC_SDSTAT & EXMC_SDSDAT_STA0) >> SDSTAT_STA0_OFFSET);
    } else {
        sdstat = ((uint32_t)(EXMC_SDSTAT & EXMC_SDSDAT_STA1) >> SDSTAT_STA1_OFFSET);
    }

    return sdstat;
}

/*!
    \brief      set the read command
    \param[in]  read_command_mode: configure SPI PSRAM read command mode
                only one parameter can be selected which is shown as below:
      \arg        EXMC_SQPIPSRAM_READ_MODE_DISABLE: not SPI mode
      \arg        EXMC_SQPIPSRAM_READ_MODE_SPI: SPI mode
      \arg        EXMC_SQPIPSRAM_READ_MODE_SQPI: SQPI mode
      \arg        EXMC_SQPIPSRAM_READ_MODE_QPI: QPI mode
    \param[in]  read_wait_cycle: wait cycle number after address phase,0..15
    \param[in]  read_command_code: read command for AHB read transfer
    \param[out] none
    \retval     none
*/
void exmc_sqpipsram_read_command_set(uint32_t read_command_mode, uint32_t read_wait_cycle, uint32_t read_command_code)
{
    uint32_t srcmd;

    srcmd = (uint32_t) read_command_mode |
            ((read_wait_cycle << SRCMD_RWAITCYCLE_OFFSET) & EXMC_SRCMD_RWAITCYCLE) |
            ((read_command_code & EXMC_SRCMD_RCMD));
    EXMC_SRCMD = srcmd;
}

/*!
    \brief      set the write command
    \param[in]  write_command_mode: configure SPI PSRAM write command mode
                only one parameter can be selected which is shown as below:
      \arg        EXMC_SQPIPSRAM_WRITE_MODE_DISABLE: not SPI mode
      \arg        EXMC_SQPIPSRAM_WRITE_MODE_SPI: SPI mode
      \arg        EXMC_SQPIPSRAM_WRITE_MODE_SQPI: SQPI mode
      \arg        EXMC_SQPIPSRAM_WRITE_MODE_QPI: QPI mode
    \param[in]  write_wait_cycle: wait cycle number after address phase,0..15
    \param[in]  write_command_code: read command for AHB read transfer
    \param[out] none
    \retval     none
*/
void exmc_sqpipsram_write_command_set(uint32_t write_command_mode, uint32_t write_wait_cycle, uint32_t write_command_code)
{
    uint32_t swcmd;

    swcmd = (uint32_t) write_command_mode |
            ((write_wait_cycle << SWCMD_WWAITCYCLE_OFFSET) & EXMC_SWCMD_WWAITCYCLE) |
            ((write_command_code & EXMC_SWCMD_WCMD));
    EXMC_SWCMD = swcmd;
}

/*!
    \brief      send SPI read ID command
    \param[in]  none
    \param[out] none
    \retval     none
*/
void exmc_sqpipsram_read_id_command_send(void)
{
    EXMC_SRCMD |= EXMC_SRCMD_RDID;
}

/*!
    \brief      send SPI special command which does not have address and data phase
    \param[in]  none
    \param[out] none
    \retval     none
*/
void exmc_sqpipsram_write_cmd_send(void)
{
    EXMC_SWCMD |= EXMC_SWCMD_SC;
}

/*!
    \brief      get the EXMC SPI ID low data
    \param[in]  none
    \param[out] none
    \retval     the ID low data
*/
uint32_t exmc_sqpipsram_low_id_get(void)
{
    return (EXMC_SIDL);
}

/*!
    \brief      get the EXMC SPI ID high data
    \param[in]  none
    \param[out] none
    \retval     the ID high data
*/
uint32_t exmc_sqpipsram_high_id_get(void)
{
    return (EXMC_SIDH);
}

/*!
    \brief      get the bit value of EXMC send write command bit or read ID command
    \param[in]  send_command_flag: the send command flag
                only one parameter can be selected which is shown as below:
      \arg        EXMC_SEND_COMMAND_FLAG_RDID: EXMC_SRCMD_RDID flag bit
      \arg        EXMC_SEND_COMMAND_FLAG_SC: EXMC_SWCMD_SC flag bit
    \param[out] none
    \retval     the new value of send command flag
*/
FlagStatus exmc_sqpipsram_send_command_state_get(uint32_t send_command_flag)
{
    FlagStatus status;
    uint32_t flag = 0x00000000U;

    if(EXMC_SEND_COMMAND_FLAG_RDID == send_command_flag) {
        flag = EXMC_SRCMD;
    } else if(EXMC_SEND_COMMAND_FLAG_SC == send_command_flag) {
        flag = EXMC_SWCMD;
    } else {
    }

    if(flag & send_command_flag) {
        status = SET;
    } else {
        status = RESET;
    }
    return status;
}

/*!
    \brief      enable EXMC interrupt
    \param[in]  exmc_bank: specify the NAND bank,PC card bank or SDRAM device
                only one parameter can be selected which is shown as below:
      \arg        EXMC_BANK1_NAND: the NAND bank1
      \arg        EXMC_BANK2_NAND: the NAND bank2
      \arg        EXMC_BANK3_PCCARD: the PC card bank
      \arg        EXMC_SDRAM_DEVICE0: the SDRAM device0
      \arg        EXMC_SDRAM_DEVICE1: the SDRAM device1
    \param[in]  interrupt: specify EXMC interrupt flag
                only one parameter can be selected which is shown as below:
      \arg        EXMC_NAND_PCCARD_INT_FLAG_RISE: rising edge interrupt and flag
      \arg        EXMC_NAND_PCCARD_INT_FLAG_LEVEL: high-level interrupt and flag
      \arg        EXMC_NAND_PCCARD_INT_FLAG_FALL: falling edge interrupt and flag
      \arg        EXMC_SDRAM_INT_FLAG_REFRESH: refresh error interrupt and flag
    \param[out] none
    \retval     none
*/
void exmc_interrupt_enable(uint32_t exmc_bank, uint32_t interrupt)
{
    if((EXMC_BANK1_NAND == exmc_bank) || (EXMC_BANK2_NAND == exmc_bank) || (EXMC_BANK3_PCCARD == exmc_bank)) {
        /* NAND bank1,bank2 or PC card bank3 */
        EXMC_NPINTEN(exmc_bank) |= interrupt;
    } else {
        /* SDRAM device0 or device1 */
        EXMC_SDARI |= EXMC_SDARI_REIE;
    }
}

/*!
    \brief      disable EXMC interrupt
    \param[in]  exmc_bank: specify the NAND bank , PC card bank or SDRAM device
                only one parameter can be selected which is shown as below:
      \arg        EXMC_BANK1_NAND: the NAND bank1
      \arg        EXMC_BANK2_NAND: the NAND bank2
      \arg        EXMC_BANK3_PCCARD: the PC card bank
      \arg        EXMC_SDRAM_DEVICE0: the SDRAM device0
      \arg        EXMC_SDRAM_DEVICE1: the SDRAM device1
    \param[in]  interrupt: specify EXMC interrupt flag
                only one parameter can be selected which is shown as below:
      \arg        EXMC_NAND_PCCARD_INT_FLAG_RISE: rising edge interrupt and flag
      \arg        EXMC_NAND_PCCARD_INT_FLAG_LEVEL: high-level interrupt and flag
      \arg        EXMC_NAND_PCCARD_INT_FLAG_FALL: falling edge interrupt and flag
      \arg        EXMC_SDRAM_INT_FLAG_REFRESH: refresh error interrupt and flag
    \param[out] none
    \retval     none
*/
void exmc_interrupt_disable(uint32_t exmc_bank, uint32_t interrupt)
{
    if((EXMC_BANK1_NAND == exmc_bank) || (EXMC_BANK2_NAND == exmc_bank) || (EXMC_BANK3_PCCARD == exmc_bank)) {
        /* NAND bank1,bank2 or PC card bank3 */
        EXMC_NPINTEN(exmc_bank) &= ~interrupt;
    } else {
        /* SDRAM device0 or device1 */
        EXMC_SDARI &= ~EXMC_SDARI_REIE;
    }
}

/*!
    \brief      get EXMC flag status
    \param[in]  exmc_bank: specify the NAND bank , PC card bank or SDRAM device
                only one parameter can be selected which is shown as below:
      \arg        EXMC_BANK1_NAND: the NAND bank1
      \arg        EXMC_BANK2_NAND: the NAND bank2
      \arg        EXMC_BANK3_PCCARD: the PC Card bank
      \arg        EXMC_SDRAM_DEVICE0: the SDRAM device0
      \arg        EXMC_SDRAM_DEVICE1: the SDRAM device1
    \param[in]  flag: EXMC status and flag
                only one parameter can be selected which is shown as below:
      \arg        EXMC_NAND_PCCARD_FLAG_RISE: interrupt rising edge status
      \arg        EXMC_NAND_PCCARD_FLAG_LEVEL: interrupt high-level status
      \arg        EXMC_NAND_PCCARD_FLAG_FALL: interrupt falling edge status
      \arg        EXMC_NAND_PCCARD_FLAG_FIFOE: FIFO empty flag
      \arg        EXMC_SDRAM_FLAG_REFRESH: refresh error interrupt flag
      \arg        EXMC_SDRAM_FLAG_NREADY: not ready status
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus exmc_flag_get(uint32_t exmc_bank, uint32_t flag)
{
    FlagStatus reval;
    uint32_t status = 0x00000000U;

    if((EXMC_BANK1_NAND == exmc_bank) || (EXMC_BANK2_NAND == exmc_bank) || (EXMC_BANK3_PCCARD == exmc_bank)) {
        /* NAND bank1,bank2 or PC card bank3 */
        status = EXMC_NPINTEN(exmc_bank);
    } else {
        /* SDRAM device0 or device1 */
        status = EXMC_SDSTAT;
    }

    if((status & flag) != (uint32_t)flag) {
        reval = RESET;
    } else {
        reval = SET;
    }
    return reval;
}

/*!
    \brief      clear EXMC flag status
    \param[in]  exmc_bank: specify the NAND bank , PCCARD bank or SDRAM device
                only one parameter can be selected which is shown as below:
      \arg        EXMC_BANK1_NAND: the NAND bank1
      \arg        EXMC_BANK2_NAND: the NAND bank2
      \arg        EXMC_BANK3_PCCARD: the PC card bank
      \arg        EXMC_SDRAM_DEVICE0: the SDRAM device0
      \arg        EXMC_SDRAM_DEVICE1: the SDRAM device1
    \param[in]  flag: EXMC status and flag
                only one parameter can be selected which is shown as below:
      \arg        EXMC_NAND_PCCARD_FLAG_RISE: interrupt rising edge status
      \arg        EXMC_NAND_PCCARD_FLAG_LEVEL: interrupt high-level status
      \arg        EXMC_NAND_PCCARD_FLAG_FALL: interrupt falling edge status
      \arg        EXMC_NAND_PCCARD_FLAG_FIFOE: FIFO empty flag
      \arg        EXMC_SDRAM_FLAG_REFRESH: refresh error interrupt flag
      \arg        EXMC_SDRAM_FLAG_NREADY: not ready status
    \param[out] none
    \retval     none
*/
void exmc_flag_clear(uint32_t exmc_bank, uint32_t flag)
{
    if((EXMC_BANK1_NAND == exmc_bank) || (EXMC_BANK2_NAND == exmc_bank) || (EXMC_BANK3_PCCARD == exmc_bank)) {
        /* NAND bank1,bank2 or PC card bank3 */
        EXMC_NPINTEN(exmc_bank) &= ~flag;
    } else {
        /* SDRAM device0 or device1 */
        EXMC_SDSTAT &= ~flag;
    }
}

/*!
    \brief      get EXMC interrupt flag
    \param[in]  exmc_bank: specify the NAND bank , PC card bank or SDRAM device
                only one parameter can be selected which is shown as below:
      \arg        EXMC_BANK1_NAND: the NAND bank1
      \arg        EXMC_BANK2_NAND: the NAND bank2
      \arg        EXMC_BANK3_PCCARD: the PC card bank
      \arg        EXMC_SDRAM_DEVICE0: the SDRAM device0
      \arg        EXMC_SDRAM_DEVICE1: the SDRAM device1
    \param[in]  interrupt: EXMC interrupt flag
                only one parameter can be selected which is shown as below:
      \arg        EXMC_NAND_PCCARD_INT_FLAG_RISE: rising edge interrupt and flag
      \arg        EXMC_NAND_PCCARD_INT_FLAG_LEVEL: high-level interrupt and flag
      \arg        EXMC_NAND_PCCARD_INT_FLAG_FALL: falling edge interrupt and flag
      \arg        EXMC_SDRAM_INT_FLAG_REFRESH: refresh error interrupt and flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus exmc_interrupt_flag_get(uint32_t exmc_bank, uint32_t interrupt)
{
    FlagStatus reval;
    uint32_t status = 0x00000000U, interrupt_enable = 0x00000000U, interrupt_state = 0x00000000U;

    if((EXMC_BANK1_NAND == exmc_bank) || (EXMC_BANK2_NAND == exmc_bank) || (EXMC_BANK3_PCCARD == exmc_bank)) {
        /* NAND bank1,bank2 or PC card bank3 */
        status = EXMC_NPINTEN(exmc_bank);
        interrupt_state = (status & (interrupt >> INTEN_INTS_OFFSET));
    } else {
        /* SDRAM device0 or device1 */
        status = EXMC_SDARI;
        interrupt_state = (EXMC_SDSTAT & EXMC_SDSDAT_REIF);
    }

    interrupt_enable = (status & interrupt);

    if((interrupt_enable) && (interrupt_state)) {
        /* interrupt flag is set */
        reval = SET;
    } else {
        /* interrupt flag is reset */
        reval = RESET;
    }
    return reval;
}

/*!
    \brief      clear EXMC interrupt flag
    \param[in]  exmc_bank: specify the NAND bank , PC card bank or SDRAM device
                only one parameter can be selected which is shown as below:
      \arg        EXMC_BANK1_NAND: the NAND bank1
      \arg        EXMC_BANK2_NAND: the NAND bank2
      \arg        EXMC_BANK3_PCCARD: the PC card bank
      \arg        EXMC_SDRAM_DEVICE0: the SDRAM device0
      \arg        EXMC_SDRAM_DEVICE1: the SDRAM device1
    \param[in]  interrupt: EXMC interrupt flag
                only one parameter can be selected which is shown as below:
      \arg        EXMC_NAND_PCCARD_INT_FLAG_RISE: rising edge interrupt and flag
      \arg        EXMC_NAND_PCCARD_INT_FLAG_LEVEL: high-level interrupt and flag
      \arg        EXMC_NAND_PCCARD_INT_FLAG_FALL: falling edge interrupt and flag
      \arg        EXMC_SDRAM_INT_FLAG_REFRESH: refresh error interrupt and flag
    \param[out] none
    \retval     none
*/
void exmc_interrupt_flag_clear(uint32_t exmc_bank, uint32_t interrupt)
{
    if((EXMC_BANK1_NAND == exmc_bank) || (EXMC_BANK2_NAND == exmc_bank) || (EXMC_BANK3_PCCARD == exmc_bank)) {
        /* NAND bank1,bank2 or PC card bank3 */
        EXMC_NPINTEN(exmc_bank) &= ~(interrupt >> INTEN_INTS_OFFSET);
    } else {
        /* SDRAM device0 or device1 */
        EXMC_SDARI |= EXMC_SDARI_REC;
    }
}
