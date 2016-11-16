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
 
#ifndef NRF52_NAME_CHANGE_H
#define NRF52_NAME_CHANGE_H

/*lint ++flb "Enter library region */

/* This file is given to prevent your SW from not compiling with the updates made to nrf52.h and 
 * nrf52_bitfields.h. The macros defined in this file were available previously. Do not use these
 * macros on purpose. Use the ones defined in nrf52.h and nrf52_bitfields.h instead.
 */

/* I2S */
/* Several enumerations changed case. Adding old macros to keep compilation compatibility. */
#define I2S_ENABLE_ENABLE_DISABLE           I2S_ENABLE_ENABLE_Disabled
#define I2S_ENABLE_ENABLE_ENABLE            I2S_ENABLE_ENABLE_Enabled
#define I2S_CONFIG_MODE_MODE_MASTER         I2S_CONFIG_MODE_MODE_Master
#define I2S_CONFIG_MODE_MODE_SLAVE          I2S_CONFIG_MODE_MODE_Slave
#define I2S_CONFIG_RXEN_RXEN_DISABLE        I2S_CONFIG_RXEN_RXEN_Disabled
#define I2S_CONFIG_RXEN_RXEN_ENABLE         I2S_CONFIG_RXEN_RXEN_Enabled
#define I2S_CONFIG_TXEN_TXEN_DISABLE        I2S_CONFIG_TXEN_TXEN_Disabled
#define I2S_CONFIG_TXEN_TXEN_ENABLE         I2S_CONFIG_TXEN_TXEN_Enabled
#define I2S_CONFIG_MCKEN_MCKEN_DISABLE      I2S_CONFIG_MCKEN_MCKEN_Disabled
#define I2S_CONFIG_MCKEN_MCKEN_ENABLE       I2S_CONFIG_MCKEN_MCKEN_Enabled
#define I2S_CONFIG_SWIDTH_SWIDTH_8BIT       I2S_CONFIG_SWIDTH_SWIDTH_8Bit
#define I2S_CONFIG_SWIDTH_SWIDTH_16BIT      I2S_CONFIG_SWIDTH_SWIDTH_16Bit
#define I2S_CONFIG_SWIDTH_SWIDTH_24BIT      I2S_CONFIG_SWIDTH_SWIDTH_24Bit
#define I2S_CONFIG_ALIGN_ALIGN_LEFT         I2S_CONFIG_ALIGN_ALIGN_Left
#define I2S_CONFIG_ALIGN_ALIGN_RIGHT        I2S_CONFIG_ALIGN_ALIGN_Right
#define I2S_CONFIG_FORMAT_FORMAT_ALIGNED    I2S_CONFIG_FORMAT_FORMAT_Aligned
#define I2S_CONFIG_CHANNELS_CHANNELS_STEREO I2S_CONFIG_CHANNELS_CHANNELS_Stereo
#define I2S_CONFIG_CHANNELS_CHANNELS_LEFT   I2S_CONFIG_CHANNELS_CHANNELS_Left
#define I2S_CONFIG_CHANNELS_CHANNELS_RIGHT  I2S_CONFIG_CHANNELS_CHANNELS_Right

/* LPCOMP */
/* Corrected typo in RESULT register. */
#define LPCOMP_RESULT_RESULT_Bellow         LPCOMP_RESULT_RESULT_Below

/* FICR */
/* Renamed name of the package. */
#define FICR_INFO_PACKAGE_PACKAGE_CH        FICR_INFO_PACKAGE_PACKAGE_CI


/*lint --flb "Leave library region" */

#endif /* NRF52_NAME_CHANGE_H */

