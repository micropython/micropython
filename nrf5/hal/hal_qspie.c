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
#include "hal_qspie.h"

#ifdef HAL_QSPIE_MODULE_ENABLED

#define QSPI_IRQ_NUM QSPI_IRQn
#define QSPI_BASE ((NRF_QSPI_Type *)NRF_QSPI_BASE)

// frequency, 32 MHz / (SCKFREQ + 1)
static const uint32_t hal_qspi_frequency_lookup[] = {
    QSPI_FREQUENCY_FREQUENCY_M2  = (15 << QSPI_IFCONFIG1_SCKFREQ_Pos), // 2 Mbps
    QSPI_FREQUENCY_FREQUENCY_M4  = (7  << QSPI_IFCONFIG1_SCKFREQ_Pos), // 4 Mbps
    QSPI_FREQUENCY_FREQUENCY_M8  = (3  << QSPI_IFCONFIG1_SCKFREQ_Pos), // 8 Mbps
    QSPI_FREQUENCY_FREQUENCY_M16 = (1  << QSPI_IFCONFIG1_SCKFREQ_Pos), // 16 Mbps
    QSPI_FREQUENCY_FREQUENCY_M32 = (0  << QSPI_IFCONFIG1_SCKFREQ_Pos), // 32 Mbps
};

#endif // HAL_QSPIE_MODULE_ENABLED
