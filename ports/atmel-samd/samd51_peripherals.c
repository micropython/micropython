/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Dan Halbert for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "hpl/gclk/hpl_gclk_base.h"
#include "hri/hri_mclk_d51.h"

// The clock initializer values are rather random, so we need to put them in
// tables for lookup. We can't compute them.

static const uint8_t SERCOMx_GCLK_ID_CORE[] = {
    SERCOM0_GCLK_ID_CORE,
    SERCOM1_GCLK_ID_CORE,
    SERCOM2_GCLK_ID_CORE,
    SERCOM3_GCLK_ID_CORE,
    SERCOM4_GCLK_ID_CORE,
    SERCOM5_GCLK_ID_CORE,
#ifdef SERCOM6
    SERCOM6_GCLK_ID_CORE,
#endif
#ifdef SERCOM7
    SERCOM7_GCLK_ID_CORE,
#endif
};

static const uint8_t SERCOMx_GCLK_ID_SLOW[] = {
    SERCOM0_GCLK_ID_SLOW,
    SERCOM1_GCLK_ID_SLOW,
    SERCOM2_GCLK_ID_SLOW,
    SERCOM3_GCLK_ID_SLOW,
    SERCOM4_GCLK_ID_SLOW,
    SERCOM5_GCLK_ID_SLOW,
#ifdef SERCOM6
    SERCOM6_GCLK_ID_SLOW,
#endif
#ifdef SERCOM7
    SERCOM7_GCLK_ID_SLOW,
#endif
};

    
// Clock initialization as done in Atmel START.
void sercom_clock_init(Sercom* sercom, uint8_t sercom_index) {
	hri_gclk_write_PCHCTRL_reg(GCLK,
                                   SERCOMx_GCLK_ID_CORE[sercom_index],
                                   GCLK_PCHCTRL_GEN_GCLK1_Val | (1 << GCLK_PCHCTRL_CHEN_Pos));
	hri_gclk_write_PCHCTRL_reg(GCLK,
                                   SERCOMx_GCLK_ID_SLOW[sercom_index],
                                   GCLK_PCHCTRL_GEN_GCLK3_Val | (1 << GCLK_PCHCTRL_CHEN_Pos));

        // hri_mclk_set_APBAMASK_SERCOMx_bit is an inline, so let's use a switch, not a table.
        switch (sercom_index) {
        case 0:
            hri_mclk_set_APBAMASK_SERCOM0_bit(MCLK);
            break;
        case 1:
            hri_mclk_set_APBAMASK_SERCOM1_bit(MCLK);
            break;
        case 2:
            hri_mclk_set_APBBMASK_SERCOM2_bit(MCLK);
            break;
        case 3:
            hri_mclk_set_APBBMASK_SERCOM3_bit(MCLK);
            break;
        case 4:
            hri_mclk_set_APBDMASK_SERCOM4_bit(MCLK);
            break;
        case 5:
            hri_mclk_set_APBDMASK_SERCOM5_bit(MCLK);
            break;
#ifdef SERCOM6
        case 6:
            hri_mclk_set_APBDMASK_SERCOM6_bit(MCLK);
            break;
#endif
#ifdef SERCOM7
        case 7:
            hri_mclk_set_APBDMASK_SERCOM7_bit(MCLK);
            break;
#endif
        }
}
