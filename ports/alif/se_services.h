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
#ifndef MICROPY_INCLUDED_ALIF_SE_SERVICES_H
#define MICROPY_INCLUDED_ALIF_SE_SERVICES_H

#include "services_lib_api.h"

void se_services_init(void);
void se_services_deinit(void);
void se_services_dump_device_data(void);
void se_services_get_unique_id(uint8_t id[8]);
__attribute__((noreturn)) void se_services_reset_soc(void);
uint64_t se_services_rand64(void);
uint32_t se_services_notify(void);

uint32_t se_services_enable_clock(clock_enable_t clock, bool enable);
uint32_t se_services_select_pll_source(pll_source_t source, pll_target_t target);

uint32_t se_services_get_run_profile(run_profile_t *profile);
uint32_t se_services_set_run_profile(run_profile_t *profile);
uint32_t se_services_get_off_profile(off_profile_t *profile);
uint32_t se_services_set_off_profile(off_profile_t *profile);

uint32_t se_services_boot_process_toc_entry(const uint8_t *image_id);
uint32_t se_services_boot_cpu(uint32_t cpu_id, uint32_t address);
uint32_t se_services_boot_reset_cpu(uint32_t cpu_id);
uint32_t se_services_boot_release_cpu(uint32_t cpu_id);
#endif // MICROPY_INCLUDED_ALIF_SE_SERVICES_H
