/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
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
#include "py/binary.h"
#include "py/objarray.h"
#include "py/mperrno.h"
#include "extmod/vfs.h"

#if MICROPY_VFS

void mp_vfs_blockdev_init(mp_vfs_blockdev_t *self, mp_obj_t bdev) {
    mp_load_method(bdev, MP_QSTR_readblocks, self->readblocks);
    mp_load_method_maybe(bdev, MP_QSTR_writeblocks, self->writeblocks);
    mp_load_method_maybe(bdev, MP_QSTR_ioctl, self->u.ioctl);
    if (self->u.ioctl[0] != MP_OBJ_NULL) {
        // Device supports new block protocol, so indicate it
        self->flags |= MP_BLOCKDEV_FLAG_HAVE_IOCTL;
    } else {
        // No ioctl method, so assume the device uses the old block protocol
        mp_load_method_maybe(bdev, MP_QSTR_sync, self->u.old.sync);
        mp_load_method(bdev, MP_QSTR_count, self->u.old.count);
    }
}

int mp_vfs_blockdev_read(mp_vfs_blockdev_t *self, size_t block_num, size_t num_blocks, uint8_t *buf) {
    if (self->flags & MP_BLOCKDEV_FLAG_NATIVE) {
        mp_uint_t (*f)(uint8_t *, uint32_t, uint32_t) = (void *)(uintptr_t)self->readblocks[2];
        return f(buf, block_num, num_blocks);
    } else {
        mp_obj_array_t ar = {{&mp_type_bytearray}, BYTEARRAY_TYPECODE, 0, num_blocks *self->block_size, buf};
        self->readblocks[2] = MP_OBJ_NEW_SMALL_INT(block_num);
        self->readblocks[3] = MP_OBJ_FROM_PTR(&ar);
        mp_call_method_n_kw(2, 0, self->readblocks);
        // TODO handle error return
        return 0;
    }
}

int mp_vfs_blockdev_read_ext(mp_vfs_blockdev_t *self, size_t block_num, size_t block_off, size_t len, uint8_t *buf) {
    mp_obj_array_t ar = {{&mp_type_bytearray}, BYTEARRAY_TYPECODE, 0, len, buf};
    self->readblocks[2] = MP_OBJ_NEW_SMALL_INT(block_num);
    self->readblocks[3] = MP_OBJ_FROM_PTR(&ar);
    self->readblocks[4] = MP_OBJ_NEW_SMALL_INT(block_off);
    mp_obj_t ret = mp_call_method_n_kw(3, 0, self->readblocks);
    if (ret == mp_const_none) {
        return 0;
    } else {
        return MP_OBJ_SMALL_INT_VALUE(ret);
    }
}

int mp_vfs_blockdev_write(mp_vfs_blockdev_t *self, size_t block_num, size_t num_blocks, const uint8_t *buf) {
    if (self->writeblocks[0] == MP_OBJ_NULL) {
        // read-only block device
        return -MP_EROFS;
    }

    if (self->flags & MP_BLOCKDEV_FLAG_NATIVE) {
        mp_uint_t (*f)(const uint8_t *, uint32_t, uint32_t) = (void *)(uintptr_t)self->writeblocks[2];
        return f(buf, block_num, num_blocks);
    } else {
        mp_obj_array_t ar = {{&mp_type_bytearray}, BYTEARRAY_TYPECODE, 0, num_blocks *self->block_size, (void *)buf};
        self->writeblocks[2] = MP_OBJ_NEW_SMALL_INT(block_num);
        self->writeblocks[3] = MP_OBJ_FROM_PTR(&ar);
        mp_call_method_n_kw(2, 0, self->writeblocks);
        // TODO handle error return
        return 0;
    }
}

int mp_vfs_blockdev_write_ext(mp_vfs_blockdev_t *self, size_t block_num, size_t block_off, size_t len, const uint8_t *buf) {
    if (self->writeblocks[0] == MP_OBJ_NULL) {
        // read-only block device
        return -MP_EROFS;
    }

    mp_obj_array_t ar = {{&mp_type_bytearray}, BYTEARRAY_TYPECODE, 0, len, (void *)buf};
    self->writeblocks[2] = MP_OBJ_NEW_SMALL_INT(block_num);
    self->writeblocks[3] = MP_OBJ_FROM_PTR(&ar);
    self->writeblocks[4] = MP_OBJ_NEW_SMALL_INT(block_off);
    mp_obj_t ret = mp_call_method_n_kw(3, 0, self->writeblocks);
    if (ret == mp_const_none) {
        return 0;
    } else {
        return MP_OBJ_SMALL_INT_VALUE(ret);
    }
}

mp_obj_t mp_vfs_blockdev_ioctl(mp_vfs_blockdev_t *self, uintptr_t cmd, uintptr_t arg) {
    if (self->flags & MP_BLOCKDEV_FLAG_HAVE_IOCTL) {
        // New protocol with ioctl
        self->u.ioctl[2] = MP_OBJ_NEW_SMALL_INT(cmd);
        self->u.ioctl[3] = MP_OBJ_NEW_SMALL_INT(arg);
        return mp_call_method_n_kw(2, 0, self->u.ioctl);
    } else {
        // Old protocol with sync and count
        switch (cmd) {
            case MP_BLOCKDEV_IOCTL_SYNC:
                if (self->u.old.sync[0] != MP_OBJ_NULL) {
                    mp_call_method_n_kw(0, 0, self->u.old.sync);
                }
                break;

            case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
                return mp_call_method_n_kw(0, 0, self->u.old.count);

            case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
                // Old protocol has fixed sector size of 512 bytes
                break;

            case MP_BLOCKDEV_IOCTL_INIT:
                // Old protocol doesn't have init
                break;
        }
        return mp_const_none;
    }
}

#endif // MICROPY_VFS
