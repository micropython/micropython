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

// Helper function to minimise code size of read/write functions
// note the n_args argument is moved to the end for further code size reduction (args keep same position in caller and callee).
static int mp_vfs_blockdev_call_rw(mp_obj_t *args, size_t block_num, size_t block_off, size_t len, void *buf, size_t n_args) {
    mp_obj_array_t ar = {{&mp_type_bytearray}, BYTEARRAY_TYPECODE, 0, len, buf};
    args[2] = MP_OBJ_NEW_SMALL_INT(block_num);
    args[3] = MP_OBJ_FROM_PTR(&ar);
    args[4] = MP_OBJ_NEW_SMALL_INT(block_off); // ignored for n_args == 2
    mp_obj_t ret = mp_call_method_n_kw(n_args, 0, args);

    if (ret == mp_const_none) {
        return 0;
    } else {
        // Some block devices return a bool indicating success, so
        // convert those to an errno integer code.
        if (ret == mp_const_true) {
            return 0;
        } else if (ret == mp_const_false) {
            return -MP_EIO;
        }
        // Block device functions are expected to return 0 on success
        // and negative integer on errors. Check for positive integer
        // results as some callers (i.e. littlefs) will produce corrupt
        // results from these.
        int i = mp_obj_get_int(ret);
        return i > 0 ? (-MP_EINVAL) : i;
    }
}

int mp_vfs_blockdev_read(mp_vfs_blockdev_t *self, size_t block_num, size_t num_blocks, uint8_t *buf) {
    if (self->flags & MP_BLOCKDEV_FLAG_NATIVE) {
        mp_uint_t (*f)(uint8_t *, uint32_t, uint32_t) = (void *)(uintptr_t)self->readblocks[2];
        return f(buf, block_num, num_blocks);
    } else {
        return mp_vfs_blockdev_call_rw(self->readblocks, block_num, 0, num_blocks * self->block_size, buf, 2);
    }
}

int mp_vfs_blockdev_read_ext(mp_vfs_blockdev_t *self, size_t block_num, size_t block_off, size_t len, uint8_t *buf) {
    return mp_vfs_blockdev_call_rw(self->readblocks, block_num, block_off, len, buf, 3);
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
        return mp_vfs_blockdev_call_rw(self->writeblocks, block_num, 0, num_blocks * self->block_size, (void *)buf, 2);
    }
}

int mp_vfs_blockdev_write_ext(mp_vfs_blockdev_t *self, size_t block_num, size_t block_off, size_t len, const uint8_t *buf) {
    if (self->writeblocks[0] == MP_OBJ_NULL) {
        // read-only block device
        return -MP_EROFS;
    }
    return mp_vfs_blockdev_call_rw(self->writeblocks, block_num, block_off, len, (void *)buf, 3);
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
