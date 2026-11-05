/*!
    \file    gd32f4xx_crc.c
    \brief   CRC driver

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

#include "gd32f4xx_crc.h"

#define CRC_DATA_RESET_VALUE      ((uint32_t)0xFFFFFFFFU)
#define CRC_FDATA_RESET_VALUE     ((uint32_t)0x00000000U)

/*!
    \brief      deinit CRC calculation unit
    \param[in]  none
    \param[out] none
    \retval     none
*/
void crc_deinit(void)
{
    CRC_DATA  = CRC_DATA_RESET_VALUE;
    CRC_FDATA = CRC_FDATA_RESET_VALUE;
    CRC_CTL   = (uint32_t)CRC_CTL_RST;
}

/*!
    \brief      reset data register(CRC_DATA) to the value of 0xFFFFFFFF
    \param[in]  none
    \param[out] none
    \retval     none
*/
void crc_data_register_reset(void)
{
    CRC_CTL |= (uint32_t)CRC_CTL_RST;
}

/*!
    \brief      read the value of the data register
    \param[in]  none
    \param[out] none
    \retval     32-bit value of the data register
*/
uint32_t crc_data_register_read(void)
{
    uint32_t data;
    data = CRC_DATA;
    return (data);
}

/*!
    \brief      read the value of the free data register
    \param[in]  none
    \param[out] none
    \retval     8-bit value of the free data register
*/
uint8_t crc_free_data_register_read(void)
{
    uint8_t fdata;
    fdata = (uint8_t)CRC_FDATA;
    return (fdata);
}

/*!
    \brief      write data to the free data register
    \param[in]  free_data: specified 8-bit data
    \param[out] none
    \retval     none
*/
void crc_free_data_register_write(uint8_t free_data)
{
    CRC_FDATA = (uint32_t)free_data;
}

/*!
    \brief      calculate the CRC value of a 32-bit data
    \param[in]  sdata: specified 32-bit data
    \param[out] none
    \retval     32-bit value calculated by CRC
*/
uint32_t crc_single_data_calculate(uint32_t sdata)
{
    CRC_DATA = sdata;
    return (CRC_DATA);
}

/*!
    \brief      calculate the CRC value of an array of 32-bit values
    \param[in]  array: pointer to an array of 32-bit values
    \param[in]  size: size of the array
    \param[out] none
    \retval     32-bit value calculated by CRC
*/
uint32_t crc_block_data_calculate(uint32_t array[], uint32_t size)
{
    uint32_t index;
    for(index = 0U; index < size; index++) {
        CRC_DATA = array[index];
    }
    return (CRC_DATA);
}
