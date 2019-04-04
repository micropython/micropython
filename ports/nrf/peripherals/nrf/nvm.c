/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2019 Nick Moore for Adafruit Industries
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

#include <stdio.h>
#include <string.h>

#include "nrf_nvmc.h"

#define FLASH_PAGE_SIZE (4096)

#ifdef BLUETOOTH_SD
#include "ble_drv.h"
#include "nrf_sdm.h"

STATIC void sd_flash_operation_start(void) {
    sd_flash_operation_status = SD_FLASH_OPERATION_IN_PROGRESS;
}

STATIC sd_flash_operation_status_t sd_flash_operation_wait_until_done(void) {
    while (sd_flash_operation_status == SD_FLASH_OPERATION_IN_PROGRESS) {
        sd_app_evt_wait();
    }
    return sd_flash_operation_status;
}
#endif

void nrf_nvm_safe_flash_page_write(uint32_t page_addr, uint8_t *data) {
    #ifdef BLUETOOTH_SD
        uint8_t sd_en = 0;
        (void) sd_softdevice_is_enabled(&sd_en);
        if (sd_en) {
            uint32_t err_code;
            sd_flash_operation_status_t status;

            sd_flash_operation_start();
            err_code = sd_flash_page_erase(page_addr / FLASH_PAGE_SIZE);
            if (err_code != NRF_SUCCESS) {
                mp_raise_OSError_msg_varg(translate("Flash erase failed to start, err 0x%04x"), err_code);
            }
            status = sd_flash_operation_wait_until_done();
            if (status == SD_FLASH_OPERATION_ERROR) {
                mp_raise_OSError_msg(translate("Flash erase failed"));
            }

            // Divide a full page into parts, because writing a full page causes an assertion failure.
            // See https://devzone.nordicsemi.com/f/nordic-q-a/40088/sd_flash_write-cause-nrf_fault_id_sd_assert/
            const size_t BLOCK_PARTS = 2;
            size_t words_to_write = FLASH_PAGE_SIZE / sizeof(uint32_t) / BLOCK_PARTS;
            for (size_t i = 0; i < BLOCK_PARTS; i++) {
                sd_flash_operation_start();
                err_code = sd_flash_write(((uint32_t *)page_addr) + i * words_to_write,
                                          (uint32_t *)data + i * words_to_write,
                                          words_to_write);
                if (err_code != NRF_SUCCESS) {
                    mp_raise_OSError_msg_varg(translate("Flash write failed to start, err 0x%04x"), err_code);
                }
                status = sd_flash_operation_wait_until_done();
                if (status == SD_FLASH_OPERATION_ERROR) {
                    mp_raise_OSError_msg(translate("Flash write failed"));
                }
            }

            return;
        }
    #endif

    nrf_nvmc_page_erase(page_addr);
    nrf_nvmc_write_bytes(page_addr, data, FLASH_PAGE_SIZE);
}    
