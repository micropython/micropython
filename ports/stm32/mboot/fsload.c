/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Damien P. George
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

#include "py/mphal.h"
#include "mboot.h"
#include "vfs.h"

#if MBOOT_FSLOAD

#if !(MBOOT_VFS_FAT || MBOOT_VFS_LFS1 || MBOOT_VFS_LFS2)
#error Must enable at least one VFS component
#endif

static int fsload_program_file(bool write_to_flash) {
    // Parse DFU
    uint8_t buf[512];
    size_t file_offset;

    // Read file header, <5sBIB
    int res = gz_stream_read(11, buf);
    if (res != 11) {
        return -1;
    }
    file_offset = 11;

    // Validate header, version 1
    if (memcmp(buf, "DfuSe\x01", 6) != 0) {
        return -1;
    }

    // Must have only 1 target
    if (buf[10] != 1) {
        return -2;
    }

    // Get total size
    uint32_t total_size = get_le32(buf + 6);

    // Read target header, <6sBi255sII
    res = gz_stream_read(274, buf);
    if (res != 274) {
        return -1;
    }
    file_offset += 274;

    // Validate target header, with alt being 0
    if (memcmp(buf, "Target\x00", 7) != 0) {
        return -1;
    }

    // Get target size and number of elements
    uint32_t target_size = get_le32(buf + 266);
    uint32_t num_elems = get_le32(buf + 270);

    size_t file_offset_target = file_offset;

    // Parse each element
    for (size_t elem = 0; elem < num_elems; ++elem) {
        // Read element header, <II
        res = gz_stream_read(8, buf);
        if (res != 8) {
            return -1;
        }
        file_offset += 8;

        // Get element destination address and size
        uint32_t elem_addr = get_le32(buf);
        uint32_t elem_size = get_le32(buf + 4);

        // Erase flash before writing
        if (write_to_flash) {
            uint32_t addr = elem_addr;
            while (addr < elem_addr + elem_size) {
                res = do_page_erase(addr, &addr);
                if (res != 0) {
                    return res;
                }
            }
        }

        // Read element data and possibly write to flash
        for (uint32_t s = elem_size; s;) {
            uint32_t l = s;
            if (l > sizeof(buf)) {
                l = sizeof(buf);
            }
            res = gz_stream_read(l, buf);
            if (res != l) {
                return -1;
            }
            if (write_to_flash) {
                res = do_write(elem_addr, buf, l);
                if (res != 0) {
                    return -1;
                }
                elem_addr += l;
            }
            s -= l;
        }

        file_offset += elem_size;
    }

    if (target_size != file_offset - file_offset_target) {
        return -1;
    }

    if (total_size != file_offset) {
        return -1;
    }

    // Read trailing info
    res = gz_stream_read(16, buf);
    if (res != 16) {
        return -1;
    }

    // TODO validate CRC32

    return 0;
}

static int fsload_validate_and_program_file(void *stream, const stream_methods_t *meth, const char *fname) {
    // First pass verifies the file, second pass programs it
    for (unsigned int pass = 0; pass <= 1; ++pass) {
        led_state_all(pass == 0 ? 2 : 4);
        int res = meth->open(stream, fname);
        if (res == 0) {
            res = gz_stream_init(stream, meth->read);
            if (res == 0) {
                res = fsload_program_file(pass == 0 ? false : true);
            }
        }
        meth->close(stream);
        if (res != 0) {
            return res;
        }
    }
    return 0;
}

int fsload_process(void) {
    const uint8_t *elem = elem_search(ELEM_DATA_START, ELEM_TYPE_FSLOAD);
    if (elem == NULL || elem[-1] < 2) {
        return -1;
    }

    // Get mount point id and create null-terminated filename
    uint8_t mount_point = elem[0];
    uint8_t fname_len = elem[-1] - 1;
    char fname[256];
    memcpy(fname, &elem[1], fname_len);
    fname[fname_len] = '\0';

    elem = ELEM_DATA_START;
    for (;;) {
        elem = elem_search(elem, ELEM_TYPE_MOUNT);
        if (elem == NULL || elem[-1] != 10) {
            // End of elements, or invalid MOUNT element
            return -1;
        }
        if (elem[0] == mount_point) {
            uint32_t base_addr = get_le32(&elem[2]);
            uint32_t byte_len = get_le32(&elem[6]);
            int ret;
            union {
                #if MBOOT_VFS_FAT
                vfs_fat_context_t fat;
                #endif
                #if MBOOT_VFS_LFS1
                vfs_lfs1_context_t lfs1;
                #endif
                #if MBOOT_VFS_LFS2
                vfs_lfs2_context_t lfs2;
                #endif
            } ctx;
            const stream_methods_t *methods;
            #if MBOOT_VFS_FAT
            if (elem[1] == ELEM_MOUNT_FAT) {
                ret = vfs_fat_mount(&ctx.fat, base_addr, byte_len);
                methods = &vfs_fat_stream_methods;
            } else
            #endif
            #if MBOOT_VFS_LFS1
            if (elem[1] == ELEM_MOUNT_LFS1) {
                ret = vfs_lfs1_mount(&ctx.lfs1, base_addr, byte_len);
                methods = &vfs_lfs1_stream_methods;
            } else
            #endif
            #if MBOOT_VFS_LFS2
            if (elem[1] == ELEM_MOUNT_LFS2) {
                ret = vfs_lfs2_mount(&ctx.lfs2, base_addr, byte_len);
                methods = &vfs_lfs2_stream_methods;
            } else
            #endif
            {
                // Unknown filesystem type
                return -1;
            }

            if (ret == 0) {
                ret = fsload_validate_and_program_file(&ctx, methods, fname);
            }

            // Flash LEDs based on success/failure of update
            for (int i = 0; i < 4; ++i) {
                if (ret == 0) {
                    led_state_all(7);
                } else {
                    led_state_all(1);
                }
                mp_hal_delay_ms(100);
                led_state_all(0);
                mp_hal_delay_ms(100);
            }
            return ret;
        }
        elem += elem[-1];
    }
}

#endif // MBOOT_FSLOAD
