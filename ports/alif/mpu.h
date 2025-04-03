/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
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

#include <stdbool.h>

#define MP_MPU_ATTR_NORMAL_WT_RA_TRANSIENT  (0)
#define MP_MPU_ATTR_DEVICE_nGnRE            (1)
#define MP_MPU_ATTR_NORMAL_WB_RA_WA         (2)
#define MP_MPU_ATTR_NORMAL_WT_RA            (3)
#define MP_MPU_ATTR_NORMAL_NON_CACHEABLE    (4)

#define MP_MPU_REGION_SRAM0                 (0)
#define MP_MPU_REGION_SRAM1                 (1)
#define MP_MPU_REGION_HOST_PERIPHERALS      (2)
#define MP_MPU_REGION_MRAM                  (3)
#define MP_MPU_REGION_OSPI_REGISTERS        (4)
#define MP_MPU_REGION_OSPI0_XIP             (5)
#define MP_MPU_REGION_OSPI1_XIP             (6)
#define MP_MPU_REGION_OPENAMP               (7)

void mpu_config_mram(bool read_only);
