/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include <string.h>

#include "stm32f4xx_hal.h"

#include "mpconfig.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "crc.h"

#if MICROPY_HW_ENABLE_CRC

/// \moduleref pyb

STATIC CRC_HandleTypeDef CRCHandle = {.Instance = NULL};

void crc_init0(void) {
    // reset the crc handle
    memset(&CRCHandle, 0, sizeof(CRC_HandleTypeDef));
    CRCHandle.Instance = CRC;
}

void crc_init(void) {
    __CRC_CLK_ENABLE();
    HAL_CRC_Init(&CRCHandle);
}

static uint32_t crc32_acc(uint32_t v) {
    CRCHandle.Instance->DR = v;
    return CRCHandle.Instance->DR;

}

uint32_t crc32(const char *buf, uint32_t len) {
    uint32_t crc, crc_reg=~0UL;
    uint32_t *p = (uint32_t*) buf;

    if (CRCHandle.State == HAL_CRC_STATE_RESET) {
        crc_init();
    }

    // reset DR
    __HAL_CRC_DR_RESET(&CRCHandle);

    // compute first words
    for (int i=0; i<(len>>2); i++) {
        crc_reg = crc32_acc(__RBIT(*p++));
    }

    crc = __RBIT(crc_reg);

    // compute remaining bytes
    if(len & 3) {
        uint32_t w = 0;
        for (int i=1; i<=(len&3); i++) {
            w <<=  8;
            w |= buf[len-i];
        }

        crc32_acc(crc_reg);
        switch(len & 3) {
            case 1:
                crc_reg = crc32_acc(__RBIT(w ^ crc) >> 24);
                crc = ( crc >> 8 ) ^ __RBIT(crc_reg);
                break;
            case 2:
                crc_reg = crc32_acc(__RBIT(w ^ crc) >> 16);
                crc = ( crc >> 16 ) ^ __RBIT(crc_reg);
                break;
            case 3:
                crc_reg = crc32_acc(__RBIT(w ^ crc) >> 8);
                crc = ( crc >> 24 ) ^ __RBIT(crc_reg);
                break;
        }
    }
    return ~crc;
}

STATIC mp_obj_t pyb_crc32(mp_obj_t buffer) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buffer, &bufinfo, MP_BUFFER_READ);
    return mp_obj_new_int(crc32(bufinfo.buf, bufinfo.len));
}

MP_DEFINE_CONST_FUN_OBJ_1(pyb_crc32_obj, pyb_crc32);

#endif // MICROPY_HW_ENABLE_CRC
