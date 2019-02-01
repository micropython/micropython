/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Analog Devices, Inc.
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

#include <stdint.h>
#include <string.h>

#include "py/runtime.h"
#include "py/objtuple.h"
#include "py/objstr.h"
#include "lib/timeutils/timeutils.h"

#include <sys/platform.h>
#include <sys/adi_core.h>
#include <cdefSC58x_rom.h>

static ADI_ROM_BOOT_CONFIG bootConfig;
static ADI_ROM_BOOT_HEADER blockHeaderC0;

void mboot_load(ADI_ROM_BOOT_CONFIG * pBootConfig)
{
  /* main part of runtime load from memory */
  int i;
  uint8_t *src, *dst;

  src = pBootConfig->pSource;
  dst = pBootConfig->pDestination;

  /* just byte-copy from boot source to memory */
  for (i=0; i<pBootConfig->dByteCount; i++)
    *dst++ = *src++;

  /* Update pSource pointer */
  pBootConfig->pSource = (void *) ((uint32_t) pBootConfig->pSource + (pBootConfig->uwSrcModifyMult * pBootConfig->dByteCount));
}

/// \module sharc - functions related to the SHARC+ processor

/// \function boot()
/// Boot the SHARC core using given bootstream.
STATIC mp_obj_t sharc_boot(mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);

    bootConfig.errorReturn   = 0;
    bootConfig.pSource       = bufinfo.buf;
    bootConfig.dBootCommand  = 0;  /*command; */
    bootConfig.dFlags        = 0;  /* no need to pass any global flags; //flags; */
    bootConfig.ulBlockCount  = 0;  /* Have always passed this as 0 ; //blockCount; */
    bootConfig.pHeader       = &blockHeaderC0;
    bootConfig.pLogBuffer    = 0;
    bootConfig.pLogCurrent   = 0;
    bootConfig.dLogByteCount = 0;
    bootConfig.pForwardFunction = 0;
    bootConfig.pCrcFunction  = 0;
    bootConfig.pErrorFunction = 0;
    bootConfig.uwDataWidth   = 0;
    bootConfig.uwUserShort   = 0;
    bootConfig.dUserLong     = 0;
    bootConfig.dReserved     = 0;
    bootConfig.pControlRegister = 0;
    bootConfig.uwDstModifyMult = 1;
    bootConfig.uwSrcModifyMult = 1;
    bootConfig.pDmaBaseRegister =0;
    bootConfig.uwDataWidth   = 0;
    bootConfig.pDestination  = 0;                        /*! Destination Pointer used for receive operations*/
    bootConfig.dByteCount    = 0;                        /*! Byte Count */
    bootConfig.loadType=ROM_DMA_DONE_POLL_IRQDONE;       /*! DMA Done Detection Method */

    /*Register the load function. */
    bootConfig.bootRegistry.pLoadFunction = mboot_load;

    adi_rom_BootKernel(&bootConfig);

    adi_core_enable(ADI_CORE_SHARC0);
    adi_core_enable(ADI_CORE_SHARC1);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_sharc_boot_obj, sharc_boot);

STATIC const mp_rom_map_elem_t sharc_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_sharc) },
    { MP_ROM_QSTR(MP_QSTR_boot), MP_ROM_PTR(&mod_sharc_boot_obj) },
};

STATIC MP_DEFINE_CONST_DICT(sharc_module_globals, sharc_module_globals_table);

const mp_obj_module_t mp_module_sharc = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&sharc_module_globals,
};
