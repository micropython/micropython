/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Paul Sokolovsky
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

#include <stdio.h>

#include "py/gc.h"
#include "py/runtime.h"
#include "py/persistentcode.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "uart.h"
#include "user_interface.h"
#include "mem.h"
#include "ets_alt_task.h"
#include "espapa102.h"
#include "modmachine.h"

#define MODESP_INCLUDE_CONSTANTS (1)

void error_check(bool status, mp_rom_error_text_t msg) {
    if (!status) {
        mp_raise_msg(&mp_type_OSError, msg);
    }
}

static mp_obj_t esp_osdebug(mp_obj_t val) {
    if (val == mp_const_none) {
        uart_os_config(-1);
    } else {
        uart_os_config(mp_obj_get_int(val));
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp_osdebug_obj, esp_osdebug);

static mp_obj_t esp_sleep_type(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return mp_obj_new_int(wifi_get_sleep_type());
    } else {
        wifi_set_sleep_type(mp_obj_get_int(args[0]));
        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_sleep_type_obj, 0, 1, esp_sleep_type);

static mp_obj_t esp_deepsleep(size_t n_args, const mp_obj_t *args) {
    uint32_t sleep_us = n_args > 0 ? mp_obj_get_int(args[0]) : 0;
    // prepare for RTC reset at wake up
    rtc_prepare_deepsleep(sleep_us);
    system_deep_sleep_set_option(n_args > 1 ? mp_obj_get_int(args[1]) : 0);
    system_deep_sleep(sleep_us);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_deepsleep_obj, 0, 2, esp_deepsleep);

static mp_obj_t esp_flash_id() {
    return mp_obj_new_int(spi_flash_get_id());
}
static MP_DEFINE_CONST_FUN_OBJ_0(esp_flash_id_obj, esp_flash_id);

static mp_obj_t esp_flash_read(mp_obj_t offset_in, mp_obj_t len_or_buf_in) {
    mp_int_t offset = mp_obj_get_int(offset_in);

    mp_int_t len;
    byte *buf;
    bool alloc_buf = mp_obj_is_int(len_or_buf_in);

    if (alloc_buf) {
        len = mp_obj_get_int(len_or_buf_in);
        buf = m_new(byte, len);
    } else {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(len_or_buf_in, &bufinfo, MP_BUFFER_WRITE);
        len = bufinfo.len;
        buf = bufinfo.buf;
    }

    // We know that allocation will be 4-byte aligned for sure
    SpiFlashOpResult res = spi_flash_read(offset, (uint32_t *)buf, len);
    if (res == SPI_FLASH_RESULT_OK) {
        if (alloc_buf) {
            return mp_obj_new_bytes(buf, len);
        }
        return mp_const_none;
    }
    if (alloc_buf) {
        m_del(byte, buf, len);
    }
    mp_raise_OSError(res == SPI_FLASH_RESULT_TIMEOUT ? MP_ETIMEDOUT : MP_EIO);
}
static MP_DEFINE_CONST_FUN_OBJ_2(esp_flash_read_obj, esp_flash_read);

static mp_obj_t esp_flash_write(mp_obj_t offset_in, const mp_obj_t buf_in) {
    mp_int_t offset = mp_obj_get_int(offset_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
    if (bufinfo.len & 0x3) {
        mp_raise_ValueError(MP_ERROR_TEXT("len must be multiple of 4"));
    }
    ets_loop_iter(); // flash access takes time so run any pending tasks
    SpiFlashOpResult res = spi_flash_write(offset, bufinfo.buf, bufinfo.len);
    ets_loop_iter();
    if (res == SPI_FLASH_RESULT_OK) {
        return mp_const_none;
    }
    mp_raise_OSError(res == SPI_FLASH_RESULT_TIMEOUT ? MP_ETIMEDOUT : MP_EIO);
}
static MP_DEFINE_CONST_FUN_OBJ_2(esp_flash_write_obj, esp_flash_write);

static mp_obj_t esp_flash_erase(mp_obj_t sector_in) {
    mp_int_t sector = mp_obj_get_int(sector_in);
    ets_loop_iter(); // flash access takes time so run any pending tasks
    SpiFlashOpResult res = spi_flash_erase_sector(sector);
    ets_loop_iter();
    if (res == SPI_FLASH_RESULT_OK) {
        return mp_const_none;
    }
    mp_raise_OSError(res == SPI_FLASH_RESULT_TIMEOUT ? MP_ETIMEDOUT : MP_EIO);
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp_flash_erase_obj, esp_flash_erase);

static mp_obj_t esp_flash_size(void) {
    extern char flashchip;
    // For SDK 1.5.2, either address has shifted and not mirrored in
    // eagle.rom.addr.v6.ld, or extra initial member was added.
    SpiFlashChip *flash = (SpiFlashChip *)(&flashchip + 4);
    #if 0
    printf("deviceId: %x\n", flash->deviceId);
    printf("chip_size: %u\n", flash->chip_size);
    printf("block_size: %u\n", flash->block_size);
    printf("sector_size: %u\n", flash->sector_size);
    printf("page_size: %u\n", flash->page_size);
    printf("status_mask: %u\n", flash->status_mask);
    #endif
    return mp_obj_new_int_from_uint(flash->chip_size);
}
static MP_DEFINE_CONST_FUN_OBJ_0(esp_flash_size_obj, esp_flash_size);

// If there's just 1 loadable segment at the start of flash,
// we assume there's a yaota8266 bootloader.
#define IS_OTA_FIRMWARE() ((*(uint32_t *)0x40200000 & 0xff00) == 0x100)

extern byte _firmware_size[];
#if MICROPY_VFS_ROM_IOCTL
extern uint8_t _micropy_hw_romfs_part0_size;
#endif

static mp_obj_t esp_flash_user_start(void) {
    uint32_t flash_user_start = (uint32_t)_firmware_size;
    #if MICROPY_VFS_ROM_IOCTL
    flash_user_start += (uint32_t)&_micropy_hw_romfs_part0_size;
    #endif
    return MP_OBJ_NEW_SMALL_INT(flash_user_start);
}
static MP_DEFINE_CONST_FUN_OBJ_0(esp_flash_user_start_obj, esp_flash_user_start);

static mp_obj_t esp_check_fw(void) {
    MD5_CTX ctx;
    char *fw_start = (char *)0x40200000;
    if (IS_OTA_FIRMWARE()) {
        // Skip yaota8266 bootloader
        fw_start += 0x3c000;
    }

    uint32_t size = *(uint32_t *)(fw_start + 0x8ffc);
    printf("size: %d\n", size);
    if (size > 1024 * 1024) {
        printf("Invalid size\n");
        return mp_const_false;
    }
    MD5Init(&ctx);
    MD5Update(&ctx, fw_start + 4, size - 4);
    unsigned char digest[16];
    MD5Final(digest, &ctx);
    printf("md5: ");
    for (int i = 0; i < 16; i++) {
        printf("%02x", digest[i]);
    }
    printf("\n");
    return mp_obj_new_bool(memcmp(digest, fw_start + size, sizeof(digest)) == 0);
}
static MP_DEFINE_CONST_FUN_OBJ_0(esp_check_fw_obj, esp_check_fw);

#if MICROPY_ESP8266_APA102
static mp_obj_t esp_apa102_write_(mp_obj_t clockPin, mp_obj_t dataPin, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    esp_apa102_write(mp_obj_get_pin(clockPin),
        mp_obj_get_pin(dataPin),
        (uint8_t *)bufinfo.buf, bufinfo.len);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(esp_apa102_write_obj, esp_apa102_write_);
#endif

static mp_obj_t esp_freemem() {
    return MP_OBJ_NEW_SMALL_INT(system_get_free_heap_size());
}
static MP_DEFINE_CONST_FUN_OBJ_0(esp_freemem_obj, esp_freemem);

static mp_obj_t esp_meminfo() {
    system_print_meminfo();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(esp_meminfo_obj, esp_meminfo);

static mp_obj_t esp_malloc(mp_obj_t size_in) {
    return MP_OBJ_NEW_SMALL_INT((mp_uint_t)os_malloc(mp_obj_get_int(size_in)));
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp_malloc_obj, esp_malloc);

static mp_obj_t esp_free(mp_obj_t addr_in) {
    os_free((void *)mp_obj_get_int(addr_in));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp_free_obj, esp_free);

static mp_obj_t esp_esf_free_bufs(mp_obj_t idx_in) {
    return MP_OBJ_NEW_SMALL_INT(ets_esf_free_bufs(mp_obj_get_int(idx_in)));
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp_esf_free_bufs_obj, esp_esf_free_bufs);

#if MICROPY_EMIT_XTENSA || MICROPY_EMIT_INLINE_XTENSA

// We provide here a way of committing executable data to a region from
// which it can be executed by the CPU.  There are 2 such writable regions:
//  - iram1, which may have some space left at the end of it
//  - memory-mapped flash rom
//
// By default the iram1 region (the space at the end of it) is used.  The
// user can select iram1 or a section of flash by calling the
// esp.set_native_code_location() function; see below.  If flash is selected
// then it is erased as needed.

#define IRAM1_END (0x40108000)
#define FLASH_START (0x40200000)
#define FLASH_END (0x40300000)
#define FLASH_SEC_SIZE (4096)

#define ESP_NATIVE_CODE_IRAM1 (0)
#define ESP_NATIVE_CODE_FLASH (1)

extern uint32_t _lit4_end;
static uint32_t esp_native_code_location;
static uint32_t esp_native_code_start;
static uint32_t esp_native_code_end;
static uint32_t esp_native_code_cur;
static uint32_t esp_native_code_erased;

void esp_native_code_init(void) {
    esp_native_code_location = ESP_NATIVE_CODE_IRAM1;
    esp_native_code_start = (uint32_t)&_lit4_end;
    esp_native_code_end = IRAM1_END;
    esp_native_code_cur = esp_native_code_start;
    esp_native_code_erased = 0;
}

void *esp_native_code_commit(void *buf, size_t len, void *reloc) {
    // printf("COMMIT(buf=%p, len=%u, start=%08x, cur=%08x, end=%08x, erased=%08x)\n", buf, len, esp_native_code_start, esp_native_code_cur, esp_native_code_end, esp_native_code_erased);

    len = (len + 3) & ~3;
    if (esp_native_code_cur + len > esp_native_code_end) {
        mp_raise_msg_varg(&mp_type_MemoryError,
            MP_ERROR_TEXT("memory allocation failed, allocating %u bytes for native code"), (uint)len);
    }

    void *dest;
    if (esp_native_code_location == ESP_NATIVE_CODE_IRAM1) {
        dest = (void *)esp_native_code_cur;
    } else {
        dest = (void *)(FLASH_START + esp_native_code_cur);
    }
    if (reloc) {
        mp_native_relocate(reloc, buf, (uintptr_t)dest);
    }

    if (esp_native_code_location == ESP_NATIVE_CODE_IRAM1) {
        memcpy(dest, buf, len);
    } else {
        SpiFlashOpResult res;
        while (esp_native_code_erased < esp_native_code_cur + len) {
            ets_loop_iter(); // flash access takes time so run any pending tasks
            res = spi_flash_erase_sector(esp_native_code_erased / FLASH_SEC_SIZE);
            if (res != SPI_FLASH_RESULT_OK) {
                break;
            }
            esp_native_code_erased += FLASH_SEC_SIZE;
        }
        ets_loop_iter();
        if (res == SPI_FLASH_RESULT_OK) {
            res = spi_flash_write(esp_native_code_cur, buf, len);
            ets_loop_iter();
        }
        if (res != SPI_FLASH_RESULT_OK) {
            mp_raise_OSError(res == SPI_FLASH_RESULT_TIMEOUT ? MP_ETIMEDOUT : MP_EIO);
        }
    }

    esp_native_code_cur += len;

    return dest;
}

static mp_obj_t esp_set_native_code_location(mp_obj_t start_in, mp_obj_t len_in) {
    if (start_in == mp_const_none && len_in == mp_const_none) {
        // use end of iram1 region
        esp_native_code_init();
    } else {
        // use flash; input params are byte offsets from start of flash
        esp_native_code_location = ESP_NATIVE_CODE_FLASH;
        esp_native_code_start = mp_obj_get_int(start_in);
        esp_native_code_end = esp_native_code_start + mp_obj_get_int(len_in);
        esp_native_code_cur = esp_native_code_start;
        esp_native_code_erased = esp_native_code_start;
        // memory-mapped flash is limited in extents to 1MByte
        if (esp_native_code_end > FLASH_END - FLASH_START) {
            mp_raise_ValueError(MP_ERROR_TEXT("flash location must be below 1MByte"));
        }
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(esp_set_native_code_location_obj, esp_set_native_code_location);

#endif

static const mp_rom_map_elem_t esp_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_esp) },

    { MP_ROM_QSTR(MP_QSTR_osdebug), MP_ROM_PTR(&esp_osdebug_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep_type), MP_ROM_PTR(&esp_sleep_type_obj) },
    { MP_ROM_QSTR(MP_QSTR_deepsleep), MP_ROM_PTR(&esp_deepsleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash_id), MP_ROM_PTR(&esp_flash_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash_read), MP_ROM_PTR(&esp_flash_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash_write), MP_ROM_PTR(&esp_flash_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash_erase), MP_ROM_PTR(&esp_flash_erase_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash_size), MP_ROM_PTR(&esp_flash_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash_user_start), MP_ROM_PTR(&esp_flash_user_start_obj) },
    #if MICROPY_ESP8266_APA102
    { MP_ROM_QSTR(MP_QSTR_apa102_write), MP_ROM_PTR(&esp_apa102_write_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_freemem), MP_ROM_PTR(&esp_freemem_obj) },
    { MP_ROM_QSTR(MP_QSTR_meminfo), MP_ROM_PTR(&esp_meminfo_obj) },
    { MP_ROM_QSTR(MP_QSTR_check_fw), MP_ROM_PTR(&esp_check_fw_obj) },
    { MP_ROM_QSTR(MP_QSTR_malloc), MP_ROM_PTR(&esp_malloc_obj) },
    { MP_ROM_QSTR(MP_QSTR_free), MP_ROM_PTR(&esp_free_obj) },
    { MP_ROM_QSTR(MP_QSTR_esf_free_bufs), MP_ROM_PTR(&esp_esf_free_bufs_obj) },
    #if MICROPY_EMIT_XTENSA || MICROPY_EMIT_INLINE_XTENSA
    { MP_ROM_QSTR(MP_QSTR_set_native_code_location), MP_ROM_PTR(&esp_set_native_code_location_obj) },
    #endif

    #if MODESP_INCLUDE_CONSTANTS
    { MP_ROM_QSTR(MP_QSTR_SLEEP_NONE), MP_ROM_INT(NONE_SLEEP_T) },
    { MP_ROM_QSTR(MP_QSTR_SLEEP_LIGHT), MP_ROM_INT(LIGHT_SLEEP_T) },
    { MP_ROM_QSTR(MP_QSTR_SLEEP_MODEM), MP_ROM_INT(MODEM_SLEEP_T) },
    #endif
};

static MP_DEFINE_CONST_DICT(esp_module_globals, esp_module_globals_table);

const mp_obj_module_t esp_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&esp_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_esp, esp_module);
