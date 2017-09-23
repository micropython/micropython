/**
 * \file
 *
 * \brief Init related functionality declaration.
 *
 * Copyright (C) 2014 Atmel Corporation. All rights reserved.
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

#ifndef _HPL_INIT_H_INCLUDED
#define _HPL_INIT_H_INCLUDED

/**
 * \addtogroup HPL Init
 *
 * \section hpl_init_rev Revision History
 * - v1.0.0 Initial Release
 *
 *@{
 */

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \name HPL functions
 */
//@{
/**
 * \brief Initializes clock sources
 */
void _sysctrl_init_sources(void);

/**
 * \brief Initializes Power Manager
 */
void _pm_init(void);

/**
 * \brief Initialize generators
 */
void _gclk_init_generators(void);

/**
 * \brief Initialize 32 kHz clock sources
 */
void _osc32kctrl_init_sources(void);

/**
 * \brief Initialize clock sources
 */
void _oscctrl_init_sources(void);

/**
 * \brief Initialize clock sources that need input reference clocks
 */
void _sysctrl_init_referenced_generators(void);

/**
 * \brief Initialize clock sources that need input reference clocks
 */
void _oscctrl_init_referenced_generators(void);

/**
 * \brief Initialize master clock generator
 */
void _mclk_init(void);

/**
 * \brief Initialize clock generator
 */
void _lpmcu_misc_regs_init(void);

/**
 * \brief Initialize clock generator
 */
void _pmc_init(void);

/**
 * \brief Set performance level
 *
 * \param[in] level The performance level to set
 */
void _set_performance_level(const uint8_t level);

/**
 * \brief Initialize the chip
 */
void _init_chip(void);

//@}

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* _HPL_INIT_H_INCLUDED */
