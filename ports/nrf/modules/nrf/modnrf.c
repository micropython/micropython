/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Ayke van Laethem
 * Copyright (c) 2019 Glenn Ruben Bakke
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

#include "py/runtime.h"

#if MICROPY_PY_NRF

#if MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE
#include "flashbdev.h"
#include "flash.h"

#define FLASH_PAGE_ALIGN_UP(addr)   (((addr) - 1) + (FLASH_PAGESIZE)-(((addr) - 1) % (FLASH_PAGESIZE)))

extern uint32_t _unused_flash_start;
extern uint32_t _unused_flash_len;

mp_obj_t nrf_modnrf_freeflash_start_aligned(void) {
    return mp_obj_new_int_from_uint(FLASH_PAGE_ALIGN_UP((uint32_t)&_unused_flash_start));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(nrf_modnrf_freeflash_start_aligned_obj, nrf_modnrf_freeflash_start_aligned);

mp_obj_t nrf_modnrf_freeflash_length_aligned(void) {
    uint32_t align_diff = FLASH_PAGE_ALIGN_UP((uint32_t)&_unused_flash_start) - ((uint32_t)&_unused_flash_start);
    uint32_t temp_len = ((uint32_t)&_unused_flash_len) - align_diff;
    uint32_t len_page_aligned = (temp_len / FLASH_PAGESIZE) * FLASH_PAGESIZE;
    return mp_obj_new_int_from_uint(len_page_aligned);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(nrf_modnrf_freeflash_length_aligned_obj, nrf_modnrf_freeflash_length_aligned);
#endif

STATIC const mp_rom_map_elem_t nrf_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_nrf) },
    #if MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE
    { MP_ROM_QSTR(MP_QSTR_Flash), MP_ROM_PTR(&nrf_flashbdev_type) },
    { MP_ROM_QSTR(MP_QSTR_unused_flash_start), MP_ROM_PTR(&nrf_modnrf_freeflash_start_aligned_obj) },
    { MP_ROM_QSTR(MP_QSTR_unused_flash_length), MP_ROM_PTR(&nrf_modnrf_freeflash_length_aligned_obj) },
    #endif
};
STATIC MP_DEFINE_CONST_DICT(nrf_module_globals, nrf_module_globals_table);

const mp_obj_module_t nrf_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&nrf_module_globals,
};

#endif // MICROPY_PY_NRF
