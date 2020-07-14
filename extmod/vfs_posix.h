// SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
// SPDX-FileCopyrightText: Copyright (c) 2018 Damien P. George
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_EXTMOD_VFS_POSIX_H
#define MICROPY_INCLUDED_EXTMOD_VFS_POSIX_H

#include "py/lexer.h"
#include "py/mpconfig.h"
#include "py/obj.h"

extern const mp_obj_type_t mp_type_vfs_posix;
extern const mp_obj_type_t mp_type_vfs_posix_fileio;
extern const mp_obj_type_t mp_type_vfs_posix_textio;

mp_obj_t mp_vfs_posix_file_open(const mp_obj_type_t *type, mp_obj_t file_in, mp_obj_t mode_in);

#endif // MICROPY_INCLUDED_EXTMOD_VFS_POSIX_H
