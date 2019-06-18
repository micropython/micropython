//   Copyright (C) 2018 TASChain
//
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <stdio.h>
#include "tas_tx.h"
#include "py/runtime.h"
#include "py/obj.h"
#include "py/objtuple.h"
#include "py/objstr.h"
#include "py/objint.h"
#include "py/lexer.h"
#include "py/compile.h"
#ifndef _MYWINDOWS_
#include "py/tvm.h"
#endif
#include <stdlib.h>
#include <string.h>

#include "py/bc0.h"
#include "py/gas.h"



STATIC const mp_rom_map_elem_t mp_module_tx_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_block) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_tx_globals, mp_module_tx_globals_table);

const mp_obj_module_t mp_module_tx = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t*)&mp_module_tx_globals,
};




