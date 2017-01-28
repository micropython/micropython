/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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

#include "mphalport.h"
#include "hal_twi.h"

#ifdef HAL_TWIE_MODULE_ENABLED

// EasyDMA variants
#define TWI_MASTER_BASE(x) ((NRF_TWIM_Type *)TWI_BASE_POINTERS[x])
#define TWI_SLAVE_BASE(x) ((NRF_TWIS_Type *)TWI_BASE_POINTERS[x])

static const uint32_t hal_twi_frequency_lookup[] = {
    TWIM_FREQUENCY_FREQUENCY_K100, // 100 kbps
    TWIM_FREQUENCY_FREQUENCY_K250, // 250 kbps
    TWIM_FREQUENCY_FREQUENCY_K400, // 400 kbps
};

void hal_twi_init(NRF_TWI_Type * p_instance, hal_twi_init_t const * p_twi_init) {
}

#endif // HAL_TWIE_MODULE_ENABLED

