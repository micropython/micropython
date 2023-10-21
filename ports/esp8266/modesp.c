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
#include "py/frozenmod.h"
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

STATIC mp_obj_t esp_osdebug(mp_obj_t val) {
    if (val == mp_const_none) {
        uart_os_config(-1);
    } else {
        uart_os_config(mp_obj_get_int(val));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_osdebug_obj, esp_osdebug);

STATIC mp_obj_t esp_sleep_type(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return mp_obj_new_int(wifi_get_sleep_type());
    } else {
        wifi_set_sleep_type(mp_obj_get_int(args[0]));
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_sleep_type_obj, 0, 1, esp_sleep_type);

STATIC mp_obj_t esp_deepsleep(size_t n_args, const mp_obj_t *args) {
    uint32_t sleep_us = n_args > 0 ? mp_obj_get_int(args[0]) : 0;
    // prepare for RTC reset at wake up
    rtc_prepare_deepsleep(sleep_us);
    system_deep_sleep_set_option(n_args > 1 ? mp_obj_get_int(args[1]) : 0);
    system_deep_sleep(sleep_us);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_deepsleep_obj, 0, 2, esp_deepsleep);

STATIC mp_obj_t esp_flash_id() {
    return mp_obj_new_int(spi_flash_get_id());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_flash_id_obj, esp_flash_id);

STATIC mp_obj_t esp_flash_read(mp_obj_t offset_in, mp_obj_t len_or_buf_in) {
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
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_flash_read_obj, esp_flash_read);

STATIC mp_obj_t esp_flash_write(mp_obj_t offset_in, const mp_obj_t buf_in) {
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
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_flash_write_obj, esp_flash_write);

STATIC mp_obj_t esp_flash_erase(mp_obj_t sector_in) {
    mp_int_t sector = mp_obj_get_int(sector_in);
    ets_loop_iter(); // flash access takes time so run any pending tasks
    SpiFlashOpResult res = spi_flash_erase_sector(sector);
    ets_loop_iter();
    if (res == SPI_FLASH_RESULT_OK) {
        return mp_const_none;
    }
    mp_raise_OSError(res == SPI_FLASH_RESULT_TIMEOUT ? MP_ETIMEDOUT : MP_EIO);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_flash_erase_obj, esp_flash_erase);

STATIC mp_obj_t esp_flash_size(void) {
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
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_flash_size_obj, esp_flash_size);

// If there's just 1 loadable segment at the start of flash,
// we assume there's a yaota8266 bootloader.
#define IS_OTA_FIRMWARE() ((*(uint32_t *)0x40200000 & 0xff00) == 0x100)

extern byte _firmware_size[];

STATIC mp_obj_t esp_flash_user_start(void) {
    return MP_OBJ_NEW_SMALL_INT((uint32_t)_firmware_size);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_flash_user_start_obj, esp_flash_user_start);

STATIC mp_obj_t esp_check_fw(void) {
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
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_check_fw_obj, esp_check_fw);

#if MICROPY_ESP8266_APA102
STATIC mp_obj_t esp_apa102_write_(mp_obj_t clockPin, mp_obj_t dataPin, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    esp_apa102_write(mp_obj_get_pin(clockPin),
        mp_obj_get_pin(dataPin),
        (uint8_t *)bufinfo.buf, bufinfo.len);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(esp_apa102_write_obj, esp_apa102_write_);
#endif

STATIC mp_obj_t esp_freemem() {
    return MP_OBJ_NEW_SMALL_INT(system_get_free_heap_size());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_freemem_obj, esp_freemem);

STATIC mp_obj_t esp_meminfo() {
    system_print_meminfo();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_meminfo_obj, esp_meminfo);

STATIC mp_obj_t esp_malloc(mp_obj_t size_in) {
    return MP_OBJ_NEW_SMALL_INT((mp_uint_t)os_malloc(mp_obj_get_int(size_in)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_malloc_obj, esp_malloc);

STATIC mp_obj_t esp_free(mp_obj_t addr_in) {
    os_free((void *)mp_obj_get_int(addr_in));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_free_obj, esp_free);

STATIC mp_obj_t esp_esf_free_bufs(mp_obj_t idx_in) {
    return MP_OBJ_NEW_SMALL_INT(ets_esf_free_bufs(mp_obj_get_int(idx_in)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_esf_free_bufs_obj, esp_esf_free_bufs);

#define IRAM1_END (0x40108000)
#define FLASH_START (0x40200000)
#define FLASH_END (0x40300000)
#define FLASH_SEC_SIZE (4096)

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

#define ESP_NATIVE_CODE_IRAM1 (0)
#define ESP_NATIVE_CODE_FLASH (1)

extern uint32_t _lit4_end;
STATIC uint32_t esp_native_code_location;
STATIC uint32_t esp_native_code_start;
STATIC uint32_t esp_native_code_end;
STATIC uint32_t esp_native_code_cur;
STATIC uint32_t esp_native_code_erased;

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

STATIC mp_obj_t esp_set_native_code_location(mp_obj_t start_in, mp_obj_t len_in) {
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
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_set_native_code_location_obj, esp_set_native_code_location);

#endif

STATIC void showMemory(char *ptr, int size) {
    for (int x = 0; x < size;) {
        printf("%02x ", ptr[x++]);
        if (x % 32 == 0) {
            printf("\r\n");
        } else if (x % 16 == 0) {
            printf("  ");
        } else if (x % 8 == 0) {
            printf(" ");
        }
    }
    printf("\r\n");
}
STATIC mp_obj_t esp_showMemory(mp_obj_t _start, mp_obj_t _size) {
    showMemory((char *)mp_obj_get_int(_start), mp_obj_get_int(_size));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_showMemory_obj, esp_showMemory);

#ifdef MICROPY_DFU
int dfu_nblks = -1;
uint32_t dfu_fwsize = 0;
uint16_t *dfu_blks = NULL;
uint16_t *dfu_offs = NULL;

/* Firmware update codes must be put into IRAM, or will crash while flashing halfway! */
void MP_FASTCODE(erase_sector)(int sector) {
    char *ptr = FLASH_START + sector * FLASH_SEC_SIZE;
    for (char *PTR = ptr + FLASH_SEC_SIZE; ptr < PTR; ptr++) {
        if (*ptr != 0xff) {
            SPIEraseSector(sector);
            break;
        }
    }
}
void MP_FASTCODE(DFU)(char *dfu_buf, int n_fwblks, int n_erase) {
    for (int x = 0; x < n_fwblks; x++) {
        erase_sector(x);
        // uart_tx_one_char(0, 'E');
    }

    int cur_write_pos = 0;
    int cur_write_sz;
    int remaining_sz = dfu_fwsize;

    for (int x = 0; x < dfu_nblks && remaining_sz > 0; x++) {
        cur_write_sz = FLASH_SEC_SIZE - dfu_offs[x];
        if (remaining_sz < cur_write_sz) {
            cur_write_sz = remaining_sz;
        }

        Cache_Read_Disable_2();
        SPIRead(1048576 + FLASH_SEC_SIZE * dfu_blks[x] + dfu_offs[x], (uint32_t *)dfu_buf, cur_write_sz);
        Cache_Read_Enable_2();
        // uart_tx_one_char(0, 'R');
        if (x == 0) {
            dfu_buf[3] = 64;
        }
        SPIWrite(cur_write_pos, (uint32_t *)dfu_buf, cur_write_sz);
        // uart_tx_one_char(0, 'W');

        cur_write_pos += cur_write_sz;
        remaining_sz -= cur_write_sz;
        wdt_feed();
    }

    for (int x = n_fwblks; x < n_erase; x++) {
        erase_sector(x);
        // uart_tx_one_char(0, 'E');
    }

    asm ("memw");
    asm ("isync");
    // rom_phy_reset_req();
    system_restart_core();
    // return cur_write_pos;
}
STATIC mp_obj_t esp_DFU(mp_obj_t erase_all) {
    extern char flashchip;
    SpiFlashChip *flash = (SpiFlashChip *)(&flashchip + 4);
    char *buf = malloc(FLASH_SEC_SIZE);
    if (buf == NULL) {
        mp_raise_OSError(MP_ENOMEM);
        return mp_const_none;
    }

    ets_wdt_disable();
    disable_irq();
    DFU(buf, dfu_fwsize / FLASH_SEC_SIZE + (dfu_fwsize % FLASH_SEC_SIZE?1:0), mp_obj_is_true(erase_all)?(flash->chip_size / FLASH_SEC_SIZE):0);

    // It will NEVER reach here even if flash failed
    // enable_irq(state);
    // ets_wdt_enable();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_DFU_obj, esp_DFU);

STATIC mp_obj_t esp_set_dfu(mp_obj_t nblks, mp_obj_t fsize) {
    dfu_nblks = mp_obj_get_int(nblks);
    dfu_fwsize = mp_obj_get_int(fsize);
    if (dfu_nblks > 0 && dfu_blks == NULL && dfu_offs == NULL) {
        size_t res = 0;
        mp_obj_t ret = mp_obj_new_bytes(FLASH_START, dfu_nblks * sizeof(uint16_t) * 2);
        dfu_blks = mp_obj_str_get_data(ret, &res);
        dfu_offs = &dfu_blks[dfu_nblks];
        return ret; // must return an object to keep the buffer, or will be GC collected
    }
    return (dfu_blks != NULL && dfu_offs != NULL)?mp_const_true:mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_set_dfu_obj, esp_set_dfu);

STATIC mp_obj_t esp_get_blks(mp_obj_t return_details) {
    if (!mp_obj_is_true(return_details)) {
        return mp_obj_new_int(dfu_nblks);
    }
    if (dfu_nblks <= 0) {
        return mp_const_none;
    }
    mp_obj_t *secs = m_new(mp_obj_t, dfu_nblks);
    mp_obj_t *offs = m_new(mp_obj_t, dfu_nblks);
    for (int x = 0; x < dfu_nblks; x++) {
        secs[x] = mp_obj_new_int(dfu_blks[x]);
        offs[x] = mp_obj_new_int(dfu_offs[x]);
    }
    mp_obj_t *ret2 = m_new(mp_obj_t, 2);
    ret2[0] = mp_obj_new_list(dfu_nblks, secs);
    ret2[1] = mp_obj_new_list(dfu_nblks, offs);
    return mp_obj_new_list(2, ret2);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_get_blks_obj, esp_get_blks);
#endif  // MICROPY_DFU

#ifdef MICROPY_DFM

// Dynamic Frozen Modules (DFM)
extern mp_import_stat_t (*mp_find_dynamic_frozen)(const char *str, int *frozen_type, void **data);
uint32_t dynamic_frozen_start = 0;  // Flash absolute byte offset of the starting sector for dynamic frozen modules
mp_import_stat_t mp_find_dynamic_frozen_(const char *str, int *frozen_type, void **data);

/* This function serves many purposes:
- if out_frozen_list!=NULL, it will output the list of strings of frozen items' names
- if name!=NULL, it will stop at the frozen item with <name>
- returns a pointer to the stopping sector
Sector format for dynamic frozen modules: {
    uint8_t n_sectors_;     // 0xff: end of dynamic_frozen list; 0x00: deleted; else: must be the same as n_sectors
    uint8_t n_sectors;      // number of sectors spanned by the frozen item
    uint8_t module_name_length;
    uint8_t pad;
    uint32_t frozen_content_length;
    char module_name[?];
    char frozen_content[?];
}*/
uint8_t *scan_dynamic_frozen(mp_obj_t out_frozen_list, const char *name) {
    if (!dynamic_frozen_start) {
        dynamic_frozen_start = (*(uint32_t *)(0x40208ffc) + FLASH_SEC_SIZE - 1) / FLASH_SEC_SIZE * FLASH_SEC_SIZE;
    }
    int n_modules = 0;
    uint8_t *ptr = FLASH_START + dynamic_frozen_start;
    while (ptr < FLASH_END) {
        if (*ptr == 0xff) {
            break;              // end-of-dynamic_frozen: erased sector are filled by with 0xff in NOR flash
        }
        if (*ptr == 0) {           // deleted frozen sector has 1st byte == 0 => skip
            ptr += FLASH_SEC_SIZE;
            continue;
        }
        if (ptr + FLASH_SEC_SIZE * ptr[1] > FLASH_END) {
            break;                                       // corrupted frozen sector
        }
        if (*ptr != ptr[1]) {
            break;                   // valid frozen sector must have byte1 == byte2 == #-of-sectors, or corrupted
        }
        if (ptr[8 + ptr[2]]) {
            break;                   // not end-of-string NULL for module name => corrupted
        }
        if (strlen(ptr + 8) != ptr[2]) {
            break;                        // incorrect module name string length
        }
        uint32_t mpy_size = *(uint32_t *)&ptr[4];
        if (ptr + 9 + ptr[2] + mpy_size >= FLASH_END) {
            break;                                       // module size overshoot => corrupted
        }
        n_modules++;

        // handle output
        if (out_frozen_list != NULL) {
            mp_obj_t dfm_info[3] = {mp_obj_new_str(ptr + 8, ptr[2]), mp_obj_new_int(mpy_size), mp_obj_new_int(ptr - FLASH_START)};
            mp_obj_list_append(out_frozen_list, mp_obj_new_tuple(3, &dfm_info[0]));
        }
        if (name && !strcmp(ptr + 8, name)) {
            break;                              // found match
        }
        ptr += FLASH_SEC_SIZE * ptr[0];
    }
    mp_find_dynamic_frozen = n_modules?mp_find_dynamic_frozen_:NULL;
    return ptr;
}

mp_import_stat_t mp_find_dynamic_frozen_(const char *str, int *frozen_type, void **data) {
    byte *ptr = scan_dynamic_frozen(NULL, str);
    if (ptr >= FLASH_END || ptr[0] != ptr[1] || ptr[0] == 0xff || strcmp(str, ptr + 8)) {
        return MP_IMPORT_STAT_NO_EXIST;
    }
    if (frozen_type) {
        *frozen_type = MP_FROZEN_MPY;
    }
    if (data) {
        mp_module_context_t *ctx = m_new_obj(mp_module_context_t);
        ctx->module.globals = mp_globals_get();
        mp_compiled_module_t cm;
        cm.context = ctx;
        mp_raw_code_load_mem(&ptr[strlen(str) + 9], *(uint32_t *)&ptr[4], &cm);
        mp_frozen_module_t *fc = malloc(sizeof(mp_frozen_module_t));
        *(mp_module_constants_t *)&fc->constants = cm.context->constants;
        fc->rc = cm.rc;
        *data = fc;
    }
    return MP_IMPORT_STAT_FILE;
}

STATIC mp_obj_t esp_ls_frozen() {
    mp_obj_t frozen_list = mp_obj_new_list(0, NULL);
    scan_dynamic_frozen(frozen_list, NULL);
    return frozen_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_ls_frozen_obj, esp_ls_frozen);

bool has_enough_free_sect(uint8_t *ptr, int n_needed) {
    int x = 0;
    for (; ptr < FLASH_END && x <= n_needed && (*ptr == 0 || *ptr == 0xff); x++) {
        ptr += FLASH_SEC_SIZE;
    }
    return x >= n_needed;
}
mp_obj_t flash_mpy_to_rom(const char *filename, const char *modname, uint8_t *ptr, uint32_t filesize, int tot_secs) {
    for(int x=0, sn=(int)(ptr-FLASH_START)/FLASH_SEC_SIZE; x<tot_secs; x++, sn++){
        erase_sector(sn);
    }
    mp_obj_t res = mp_const_true;
    byte buf[256] = {tot_secs, tot_secs, strlen(modname), 0};
    mp_reader_t reader;
    mp_reader_new_file(&reader, qstr_from_str(filename));
    *(uint32_t *)&buf[4] = filesize;
    strcpy(&buf[8], modname);
    mp_uint_t ch = 0;
    int off = strlen(modname) + 9;
    while (ch != MP_READER_EOF) {
        while (off < 256 && (ch = reader.readbyte(reader.data)) != MP_READER_EOF) {
            buf[off] = ch;
            off++;
        }
        while (off & 0x3) {
            buf[off++] = 0xff;  // reached EOF, fill with 0xff until a 4-byte boundary
        }
        if (spi_flash_write(ptr - FLASH_START, &buf[0], off) != SPI_FLASH_RESULT_OK) {
            res = mp_const_false;
            break;
        }
        ptr += off;
        off = 0;
    }
    reader.close(reader.data);
    mp_find_dynamic_frozen = res?mp_find_dynamic_frozen_:NULL;
    return res;
}
bool delete_mpy_from_rom(byte *ptr) {
    uint32_t new_dword = (*(uint32_t *)ptr) & 0xffffff00;  // set 1st byte to 0
    for (int x = ptr[1]; x > 0 && ptr < FLASH_END; x--, ptr += FLASH_SEC_SIZE) { // free every sector occupied by the module
        if (spi_flash_write(ptr - FLASH_START, (uint32_t *)&new_dword, 4) != SPI_FLASH_RESULT_OK) { // NOR flash can write 0-bit without erasing
            return mp_const_false;
        }
    }
    return mp_const_true;
}
bool compare_flash_to_file(const char *filename, byte *ptr) {
    bool is_diff = false;
    mp_reader_t reader;
    mp_reader_new_file(&reader, qstr_from_str(filename));
    byte ch;
    while ((ch = reader.readbyte(reader.data)) != MP_READER_EOF) {
        if (*ptr != ch) {
            is_diff = true;
            break;
        }
    }
    reader.close(reader.data);
    return is_diff;
}
STATIC mp_obj_t esp_add_frozen(size_t n_args, const mp_obj_t *args) {
    uint8_t *ptr;
    const char *filename = mp_obj_str_get_str(args[0]);
    char *modname = n_args > 1?mp_obj_str_get_str(args[1]):filename;

    // get .mpy filesize
    int mpy_size = 0;
    mp_reader_t reader;
    mp_reader_new_file(&reader, qstr_from_str(filename));
    while (reader.readbyte(reader.data) != MP_READER_EOF) {
        mpy_size++;
    }
    reader.close(reader.data);

    // compute spaces needed
    int tot_size = mpy_size + strlen(modname) + 9;
    int tot_secs = (tot_size + FLASH_SEC_SIZE - 1) / FLASH_SEC_SIZE;

    // find existing mpy
    ptr = scan_dynamic_frozen(NULL, modname);
    if (ptr < FLASH_END && *ptr != 0xff && ptr[0] == ptr[1] && !strcmp(modname, ptr + 8)) {  // found module with the same name
        // if same module name and content, return true
        if (*(uint32_t *)&ptr[4] == mpy_size && !compare_flash_to_file(filename, ptr + strlen(modname) + 9)) {
            return mp_const_true;
        }
        // otherwise, delete the existing module
        if (!delete_mpy_from_rom(ptr)) {
            return mp_const_false;
        }
    }

    ptr = FLASH_START + dynamic_frozen_start;
    while (ptr < FLASH_END) {
        switch (*ptr) {
            case 0x0:
                if (!has_enough_free_sect(ptr, tot_secs)) {
                    break;
                }
                return flash_mpy_to_rom(filename, modname, ptr, mpy_size, tot_secs);
            case 0xff:
                if (!has_enough_free_sect(ptr, tot_secs)) {
                    return mp_const_false;
                }
                return flash_mpy_to_rom(filename, modname, ptr, mpy_size, tot_secs);
            default:
                if (ptr[1] != ptr[0]) {
                    return mp_const_false;
                }
                ptr += FLASH_SEC_SIZE * (*ptr);
                continue;
        }
        ptr += FLASH_SEC_SIZE;
    }
    return mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_add_frozen_obj, 1, 2, esp_add_frozen);

STATIC mp_obj_t esp_del_frozen(mp_obj_t module_name) {
    const char *_module_name = mp_obj_str_get_str(module_name);
    uint8_t *ptr = scan_dynamic_frozen(NULL, _module_name);
    if (ptr >= FLASH_END || strcmp(ptr + 8, _module_name)) { // not found
        return mp_const_false;
    }
    if (!delete_mpy_from_rom(ptr)) {
        return mp_const_false;
    }
    scan_dynamic_frozen(NULL, NULL);
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_del_frozen_obj, esp_del_frozen);

STATIC mp_obj_t esp_reset_frozen() {
    for (int p = dynamic_frozen_start / FLASH_SEC_SIZE, P = (FLASH_END - FLASH_START) / FLASH_SEC_SIZE; p < P; p++) {
        erase_sector(p);
    }
    mp_find_dynamic_frozen = NULL;
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_reset_frozen_obj, esp_reset_frozen);

#endif  // MICROPY_DFM

STATIC const mp_rom_map_elem_t esp_module_globals_table[] = {
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

    #ifdef MICROPY_DFU
    { MP_ROM_QSTR(MP_QSTR_DFU), MP_ROM_PTR(&esp_DFU_obj) },
    { MP_ROM_QSTR(MP_QSTR_showMemory), MP_ROM_PTR(&esp_showMemory_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_dfu), MP_ROM_PTR(&esp_set_dfu_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_blks), MP_ROM_PTR(&esp_get_blks_obj) },
    #endif

    #ifdef MICROPY_DFM
    { MP_ROM_QSTR(MP_QSTR_add_frozen), MP_ROM_PTR(&esp_add_frozen_obj) },
    { MP_ROM_QSTR(MP_QSTR_del_frozen), MP_ROM_PTR(&esp_del_frozen_obj) },
    { MP_ROM_QSTR(MP_QSTR_ls_frozen), MP_ROM_PTR(&esp_ls_frozen_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset_frozen), MP_ROM_PTR(&esp_reset_frozen_obj) },
    #endif

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

STATIC MP_DEFINE_CONST_DICT(esp_module_globals, esp_module_globals_table);

const mp_obj_module_t esp_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&esp_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_esp, esp_module);
