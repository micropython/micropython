/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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
#include "lib/oofatfs/ff.h"
#include "extmod/uzlib/uzlib.h"
#include "mboot.h"

#if MBOOT_FSLOAD

#define DICT_SIZE (1 << 15)

typedef struct _gz_stream_t {
    FIL fp;
    TINF_DATA tinf;
    uint8_t buf[512];
    uint8_t dict[DICT_SIZE];
} gz_stream_t;

static gz_stream_t gz_stream SECTION_NOZERO_BSS;

static int gz_stream_read_src(TINF_DATA *tinf) {
    UINT n;
    FRESULT res = f_read(&gz_stream.fp, gz_stream.buf, sizeof(gz_stream.buf), &n);
    if (res != FR_OK) {
        return -1;
    }
    if (n == 0) {
        return -1;
    }
    tinf->source = gz_stream.buf + 1;
    tinf->source_limit = gz_stream.buf + n;
    return gz_stream.buf[0];
}

static int gz_stream_open(FATFS *fatfs, const char *filename) {
    FRESULT res = f_open(fatfs, &gz_stream.fp, filename, FA_READ);
    if (res != FR_OK) {
        return -1;
    }
    memset(&gz_stream.tinf, 0, sizeof(gz_stream.tinf));
    gz_stream.tinf.readSource = gz_stream_read_src;

    int st = uzlib_gzip_parse_header(&gz_stream.tinf);
    if (st != TINF_OK) {
        f_close(&gz_stream.fp);
        return -1;
    }

    uzlib_uncompress_init(&gz_stream.tinf, gz_stream.dict, DICT_SIZE);

    return 0;
}

static int gz_stream_read(size_t len, uint8_t *buf) {
    gz_stream.tinf.dest = buf;
    gz_stream.tinf.dest_limit = buf + len;
    int st = uzlib_uncompress_chksum(&gz_stream.tinf);
    if (st == TINF_DONE) {
        return 0;
    }
    if (st < 0) {
        return st;
    }
    return gz_stream.tinf.dest - buf;
}

static int fsload_program_file(FATFS *fatfs, const char *filename, bool write_to_flash) {
    int res = gz_stream_open(fatfs, filename);
    if (res != 0) {
        return res;
    }

    // Parse DFU
    uint8_t buf[512];
    size_t file_offset;

    // Read file header, <5sBIB
    res = gz_stream_read(11, buf);
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

static int fsload_process_fatfs(uint32_t base_addr, uint32_t byte_len, size_t fname_len, const char *fname) {
    fsload_bdev_t bdev = {base_addr, byte_len};
    FATFS fatfs;
    fatfs.drv = &bdev;
    FRESULT res = f_mount(&fatfs);
    if (res != FR_OK) {
        return -1;
    }

    FF_DIR dp;
    res = f_opendir(&fatfs, &dp, "/");
    if (res != FR_OK) {
        return -1;
    }

    // Search for firmware file with correct name
    int r;
    for (;;) {
        FILINFO fno;
        res = f_readdir(&dp, &fno);
        char *fn = fno.fname;
        if (res != FR_OK || fn[0] == 0) {
            // Finished listing dir, no firmware found
            r = -1;
            break;
        }
        if (memcmp(fn, fname, fname_len) == 0 && fn[fname_len] == '\0') {
            // Found firmware
            led_state_all(2);
            r = fsload_program_file(&fatfs, fn, false);
            if (r == 0) {
                // Firmware is valid, program it
                led_state_all(4);
                r = fsload_program_file(&fatfs, fn, true);
            }
            break;
        }
    }

    return r;
}

int fsload_process(void) {
    const uint8_t *elem = elem_search(ELEM_DATA_START, ELEM_TYPE_FSLOAD);
    if (elem == NULL || elem[-1] < 2) {
        return -1;
    }

    uint8_t mount_point = elem[0];
    uint8_t fname_len = elem[-1] - 1;
    const char *fname = (const char*)&elem[1];

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
            if (elem[1] == ELEM_MOUNT_FAT) {
                int ret = fsload_process_fatfs(base_addr, byte_len, fname_len, fname);
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
            // Unknown filesystem type
            return -1;
        }
        elem += elem[-1];
    }
}

#endif // MBOOT_FSLOAD
