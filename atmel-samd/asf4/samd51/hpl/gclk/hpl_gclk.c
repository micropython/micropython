
/**
 * \file
 *
 * \brief Generic Clock Controller related functionality.
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
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

#include <hpl_gclk_config.h>
#include <hpl_init.h>
#include <utils_assert.h>

/**
 * \brief Initializes generators

 */
void _gclk_init_generators(void)
{

#if CONF_GCLK_GEN_0_GENEN == 1
	hri_gclk_write_GENCTRL_reg(GCLK,
	                           0,
	                           GCLK_GENCTRL_DIV(CONF_GCLK_GEN_0_DIV)
	                               | (CONF_GCLK_GEN_0_RUNSTDBY << GCLK_GENCTRL_RUNSTDBY_Pos)
	                               | (CONF_GCLK_GEN_0_DIVSEL << GCLK_GENCTRL_DIVSEL_Pos)
	                               | (CONF_GCLK_GEN_0_OE << GCLK_GENCTRL_OE_Pos)
	                               | (CONF_GCLK_GEN_0_OOV << GCLK_GENCTRL_OOV_Pos)
	                               | (CONF_GCLK_GEN_0_IDC << GCLK_GENCTRL_IDC_Pos)
	                               | (CONF_GCLK_GEN_0_GENEN << GCLK_GENCTRL_GENEN_Pos)
	                               | CONF_GCLK_GEN_0_SOURCE);
#endif
#if CONF_GCLK_GEN_1_GENEN == 1
	hri_gclk_write_GENCTRL_reg(GCLK,
	                           1,
	                           GCLK_GENCTRL_DIV(CONF_GCLK_GEN_1_DIV)
	                               | (CONF_GCLK_GEN_1_RUNSTDBY << GCLK_GENCTRL_RUNSTDBY_Pos)
	                               | (CONF_GCLK_GEN_1_DIVSEL << GCLK_GENCTRL_DIVSEL_Pos)
	                               | (CONF_GCLK_GEN_1_OE << GCLK_GENCTRL_OE_Pos)
	                               | (CONF_GCLK_GEN_1_OOV << GCLK_GENCTRL_OOV_Pos)
	                               | (CONF_GCLK_GEN_1_IDC << GCLK_GENCTRL_IDC_Pos)
	                               | (CONF_GCLK_GEN_1_GENEN << GCLK_GENCTRL_GENEN_Pos)
	                               | CONF_GCLK_GEN_1_SOURCE);
#endif
#if CONF_GCLK_GEN_2_GENEN == 1
	hri_gclk_write_GENCTRL_reg(GCLK,
	                           2,
	                           GCLK_GENCTRL_DIV(CONF_GCLK_GEN_2_DIV)
	                               | (CONF_GCLK_GEN_2_RUNSTDBY << GCLK_GENCTRL_RUNSTDBY_Pos)
	                               | (CONF_GCLK_GEN_2_DIVSEL << GCLK_GENCTRL_DIVSEL_Pos)
	                               | (CONF_GCLK_GEN_2_OE << GCLK_GENCTRL_OE_Pos)
	                               | (CONF_GCLK_GEN_2_OOV << GCLK_GENCTRL_OOV_Pos)
	                               | (CONF_GCLK_GEN_2_IDC << GCLK_GENCTRL_IDC_Pos)
	                               | (CONF_GCLK_GEN_2_GENEN << GCLK_GENCTRL_GENEN_Pos)
	                               | CONF_GCLK_GEN_2_SOURCE);
#endif
#if CONF_GCLK_GEN_3_GENEN == 1
	hri_gclk_write_GENCTRL_reg(GCLK,
	                           3,
	                           GCLK_GENCTRL_DIV(CONF_GCLK_GEN_3_DIV)
	                               | (CONF_GCLK_GEN_3_RUNSTDBY << GCLK_GENCTRL_RUNSTDBY_Pos)
	                               | (CONF_GCLK_GEN_3_DIVSEL << GCLK_GENCTRL_DIVSEL_Pos)
	                               | (CONF_GCLK_GEN_3_OE << GCLK_GENCTRL_OE_Pos)
	                               | (CONF_GCLK_GEN_3_OOV << GCLK_GENCTRL_OOV_Pos)
	                               | (CONF_GCLK_GEN_3_IDC << GCLK_GENCTRL_IDC_Pos)
	                               | (CONF_GCLK_GEN_3_GENEN << GCLK_GENCTRL_GENEN_Pos)
	                               | CONF_GCLK_GEN_3_SOURCE);
#endif
#if CONF_GCLK_GEN_4_GENEN == 1
	hri_gclk_write_GENCTRL_reg(GCLK,
	                           4,
	                           GCLK_GENCTRL_DIV(CONF_GCLK_GEN_4_DIV)
	                               | (CONF_GCLK_GEN_4_RUNSTDBY << GCLK_GENCTRL_RUNSTDBY_Pos)
	                               | (CONF_GCLK_GEN_4_DIVSEL << GCLK_GENCTRL_DIVSEL_Pos)
	                               | (CONF_GCLK_GEN_4_OE << GCLK_GENCTRL_OE_Pos)
	                               | (CONF_GCLK_GEN_4_OOV << GCLK_GENCTRL_OOV_Pos)
	                               | (CONF_GCLK_GEN_4_IDC << GCLK_GENCTRL_IDC_Pos)
	                               | (CONF_GCLK_GEN_4_GENEN << GCLK_GENCTRL_GENEN_Pos)
	                               | CONF_GCLK_GEN_4_SOURCE);
#endif
#if CONF_GCLK_GEN_5_GENEN == 1
	hri_gclk_write_GENCTRL_reg(GCLK,
	                           5,
	                           GCLK_GENCTRL_DIV(CONF_GCLK_GEN_5_DIV)
	                               | (CONF_GCLK_GEN_5_RUNSTDBY << GCLK_GENCTRL_RUNSTDBY_Pos)
	                               | (CONF_GCLK_GEN_5_DIVSEL << GCLK_GENCTRL_DIVSEL_Pos)
	                               | (CONF_GCLK_GEN_5_OE << GCLK_GENCTRL_OE_Pos)
	                               | (CONF_GCLK_GEN_5_OOV << GCLK_GENCTRL_OOV_Pos)
	                               | (CONF_GCLK_GEN_5_IDC << GCLK_GENCTRL_IDC_Pos)
	                               | (CONF_GCLK_GEN_5_GENEN << GCLK_GENCTRL_GENEN_Pos)
	                               | CONF_GCLK_GEN_5_SOURCE);
#endif
#if CONF_GCLK_GEN_6_GENEN == 1
	hri_gclk_write_GENCTRL_reg(GCLK,
	                           6,
	                           GCLK_GENCTRL_DIV(CONF_GCLK_GEN_6_DIV)
	                               | (CONF_GCLK_GEN_6_RUNSTDBY << GCLK_GENCTRL_RUNSTDBY_Pos)
	                               | (CONF_GCLK_GEN_6_DIVSEL << GCLK_GENCTRL_DIVSEL_Pos)
	                               | (CONF_GCLK_GEN_6_OE << GCLK_GENCTRL_OE_Pos)
	                               | (CONF_GCLK_GEN_6_OOV << GCLK_GENCTRL_OOV_Pos)
	                               | (CONF_GCLK_GEN_6_IDC << GCLK_GENCTRL_IDC_Pos)
	                               | (CONF_GCLK_GEN_6_GENEN << GCLK_GENCTRL_GENEN_Pos)
	                               | CONF_GCLK_GEN_6_SOURCE);
#endif
#if CONF_GCLK_GEN_7_GENEN == 1
	hri_gclk_write_GENCTRL_reg(GCLK,
	                           7,
	                           GCLK_GENCTRL_DIV(CONF_GCLK_GEN_7_DIV)
	                               | (CONF_GCLK_GEN_7_RUNSTDBY << GCLK_GENCTRL_RUNSTDBY_Pos)
	                               | (CONF_GCLK_GEN_7_DIVSEL << GCLK_GENCTRL_DIVSEL_Pos)
	                               | (CONF_GCLK_GEN_7_OE << GCLK_GENCTRL_OE_Pos)
	                               | (CONF_GCLK_GEN_7_OOV << GCLK_GENCTRL_OOV_Pos)
	                               | (CONF_GCLK_GEN_7_IDC << GCLK_GENCTRL_IDC_Pos)
	                               | (CONF_GCLK_GEN_7_GENEN << GCLK_GENCTRL_GENEN_Pos)
	                               | CONF_GCLK_GEN_7_SOURCE);
#endif
#if CONF_GCLK_GEN_8_GENEN == 1
	hri_gclk_write_GENCTRL_reg(GCLK,
	                           8,
	                           GCLK_GENCTRL_DIV(CONF_GCLK_GEN_8_DIV)
	                               | (CONF_GCLK_GEN_8_RUNSTDBY << GCLK_GENCTRL_RUNSTDBY_Pos)
	                               | (CONF_GCLK_GEN_8_DIVSEL << GCLK_GENCTRL_DIVSEL_Pos)
	                               | (CONF_GCLK_GEN_8_OE << GCLK_GENCTRL_OE_Pos)
	                               | (CONF_GCLK_GEN_8_OOV << GCLK_GENCTRL_OOV_Pos)
	                               | (CONF_GCLK_GEN_8_IDC << GCLK_GENCTRL_IDC_Pos)
	                               | (CONF_GCLK_GEN_8_GENEN << GCLK_GENCTRL_GENEN_Pos)
	                               | CONF_GCLK_GEN_8_SOURCE);
#endif
#if CONF_GCLK_GEN_9_GENEN == 1
	hri_gclk_write_GENCTRL_reg(GCLK,
	                           9,
	                           GCLK_GENCTRL_DIV(CONF_GCLK_GEN_9_DIV)
	                               | (CONF_GCLK_GEN_9_RUNSTDBY << GCLK_GENCTRL_RUNSTDBY_Pos)
	                               | (CONF_GCLK_GEN_9_DIVSEL << GCLK_GENCTRL_DIVSEL_Pos)
	                               | (CONF_GCLK_GEN_9_OE << GCLK_GENCTRL_OE_Pos)
	                               | (CONF_GCLK_GEN_9_OOV << GCLK_GENCTRL_OOV_Pos)
	                               | (CONF_GCLK_GEN_9_IDC << GCLK_GENCTRL_IDC_Pos)
	                               | (CONF_GCLK_GEN_9_GENEN << GCLK_GENCTRL_GENEN_Pos)
	                               | CONF_GCLK_GEN_9_SOURCE);
#endif
#if CONF_GCLK_GEN_10_GENEN == 1
	hri_gclk_write_GENCTRL_reg(GCLK,
	                           10,
	                           GCLK_GENCTRL_DIV(CONF_GCLK_GEN_10_DIV)
	                               | (CONF_GCLK_GEN_10_RUNSTDBY << GCLK_GENCTRL_RUNSTDBY_Pos)
	                               | (CONF_GCLK_GEN_10_DIVSEL << GCLK_GENCTRL_DIVSEL_Pos)
	                               | (CONF_GCLK_GEN_10_OE << GCLK_GENCTRL_OE_Pos)
	                               | (CONF_GCLK_GEN_10_OOV << GCLK_GENCTRL_OOV_Pos)
	                               | (CONF_GCLK_GEN_10_IDC << GCLK_GENCTRL_IDC_Pos)
	                               | (CONF_GCLK_GEN_10_GENEN << GCLK_GENCTRL_GENEN_Pos)
	                               | CONF_GCLK_GEN_10_SOURCE);
#endif
#if CONF_GCLK_GEN_11_GENEN == 1
	hri_gclk_write_GENCTRL_reg(GCLK,
	                           11,
	                           GCLK_GENCTRL_DIV(CONF_GCLK_GEN_11_DIV)
	                               | (CONF_GCLK_GEN_11_RUNSTDBY << GCLK_GENCTRL_RUNSTDBY_Pos)
	                               | (CONF_GCLK_GEN_11_DIVSEL << GCLK_GENCTRL_DIVSEL_Pos)
	                               | (CONF_GCLK_GEN_11_OE << GCLK_GENCTRL_OE_Pos)
	                               | (CONF_GCLK_GEN_11_OOV << GCLK_GENCTRL_OOV_Pos)
	                               | (CONF_GCLK_GEN_11_IDC << GCLK_GENCTRL_IDC_Pos)
	                               | (CONF_GCLK_GEN_11_GENEN << GCLK_GENCTRL_GENEN_Pos)
	                               | CONF_GCLK_GEN_11_SOURCE);
#endif
}
