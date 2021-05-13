/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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
#include <stdbool.h>

#include "py/mperrno.h"
#include "py/runtime.h"

#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/sdioio/SDCard.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "supervisor/shared/translate.h"

#include "genhdr/sdiodata.h"

#include "sd_mmc/sd_mmc.h"
#include "sd_mmc/conf_sd_mmc.h"
#include "peripheral_clk_config.h"

#ifndef DEBUG_SDIO
#define DEBUG_SDIO (0)
#endif

#if DEBUG_SDIO
#define DEBUG_PRINT(...) ((void)mp_printf(&mp_plat_print, __VA_ARGS__))
#define DEBUG_PRINT_OBJ(o) ((void)mp_obj_print_helper(&mp_plat_print, (mp_obj_t)o, PRINT_REPR))
#else
#define DEBUG_PRINT(...) ((void)0)
#define DEBUG_PRINT_OBJ(...) ((void)0)
#endif
#define DEBUG_PRINT_OBJ_NL(o) (DEBUG_PRINT_OBJ(o), DEBUG_PRINT("\n"))

#define GPIO_PIN_FUNCTION_SDIO (GPIO_PIN_FUNCTION_I)

static Sdhc *sdhc_insts[] = SDHC_INSTS;

void common_hal_sdioio_sdcard_construct(sdioio_sdcard_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *command,
    uint8_t num_data, mcu_pin_obj_t **data, uint32_t frequency) {
    /*
SD breakout as assembled  ("*" = minimum viable set)

PURPLE  9   DAT2        SDA
BLUE    1   DAT3        SCL
GREEN   2   CMD  *      D32
YELLOW  3   VSS1
RED     4   VDD  *      3.3V
BROWN   5   CLK  *      BROWN
BLACK   6   VSS2  *     GND
WHITE   7   DAT0  *     D8
GREY    8   DAT1        D29

DAT0..3 PB18..21 (D8 D29 D20 D21) WHITE GREY PURPLE BLUE
CMD PA20 PCC_D? (D33)  GREEN
CLK PA21 PCC_D? (D32)  BROWN

*/

    mcu_pin_function_t *functions[6] = {};
    functions[0] = mcu_find_pin_function(sdio_cmd, command, -1, MP_QSTR_command);
    int instance = functions[0]->instance;
    functions[1] = mcu_find_pin_function(sdio_ck, clock, instance, MP_QSTR_clock);
    functions[2] = mcu_find_pin_function(sdio_dat0, data[0], instance, MP_QSTR_data0);
    if (num_data == 4) {
        functions[3] = mcu_find_pin_function(sdio_dat1, data[1], instance, MP_QSTR_data1);
        functions[4] = mcu_find_pin_function(sdio_dat2, data[2], instance, MP_QSTR_data2);
        functions[5] = mcu_find_pin_function(sdio_dat3, data[3], instance, MP_QSTR_data3);
    }

    // We've verified all pins, now set their special functions
    self->command_pin = common_hal_mcu_pin_number(functions[0]->obj);
    self->clock_pin = common_hal_mcu_pin_number(functions[1]->obj);

    for (int i = 0; i < num_data; i++) {
        mcu_pin_function_t *function = functions[2 + i];
        if (function) {
            self->data_pins[i] = common_hal_mcu_pin_number(function->obj);
        } else {
            self->data_pins[i] = COMMON_HAL_MCU_NO_PIN;
        }
    }

    for (size_t i = 0; i < MP_ARRAY_SIZE(functions); i++) {
        if (!functions[i]->obj) {
            break;
        }

        gpio_set_pin_direction(functions[i]->pin, GPIO_DIRECTION_OUT);
        gpio_set_pin_level(functions[i]->pin, false);
        // Enable pullups on all pins except CLK and DAT3
        gpio_set_pin_pull_mode(functions[i]->pin,
            (i == 1 || i == 5) ? GPIO_PULL_OFF : GPIO_PULL_UP);
        gpio_set_pin_function(functions[i]->pin, GPIO_PIN_FUNCTION_SDIO);

        common_hal_never_reset_pin(functions[i]->obj);
    }

    self->num_data = num_data;
    self->frequency = frequency;

    if (instance == 0) {
        hri_mclk_set_AHBMASK_SDHC0_bit(MCLK);
        hri_gclk_write_PCHCTRL_reg(GCLK, SDHC0_GCLK_ID, CONF_GCLK_SDHC0_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
        hri_gclk_write_PCHCTRL_reg(GCLK, SDHC0_GCLK_ID_SLOW, CONF_GCLK_SDHC0_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    #ifdef SDHC1_GCLK_ID
    } else {
        hri_mclk_set_AHBMASK_SDHC1_bit(MCLK);
        hri_gclk_write_PCHCTRL_reg(GCLK, SDHC1_GCLK_ID, CONF_GCLK_SDHC1_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
        hri_gclk_write_PCHCTRL_reg(GCLK, SDHC1_GCLK_ID_SLOW, CONF_GCLK_SDHC1_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    #endif
    }

    DEBUG_PRINT("instance %d @%p\n", instance, sdhc_insts[instance]);
    mci_sync_init(&self->IO_BUS, sdhc_insts[instance]);
    sd_mmc_init(&self->IO_BUS, NULL, NULL);

    sd_mmc_err_t result = SD_MMC_INIT_ONGOING;

    for (int i = 0; result == SD_MMC_INIT_ONGOING && i < 100; i++) {
        result = sd_mmc_check(0);
        DEBUG_PRINT("sd_mmc_check(0) -> %d\n", result);
    }

    if (result != SD_MMC_OK) {
        mp_raise_OSError_msg_varg(translate("%q failure: %d"), MP_QSTR_sd_mmc_check, (int)result);
    }
    // sd_mmc_get_capacity() is in KiB, but our "capacity" is in 512-byte blocks
    self->capacity = sd_mmc_get_capacity(0) * 2;

    DEBUG_PRINT("capacity=%u\n", self->capacity);
}

uint32_t common_hal_sdioio_sdcard_get_count(sdioio_sdcard_obj_t *self) {
    return self->capacity;
}

uint32_t common_hal_sdioio_sdcard_get_frequency(sdioio_sdcard_obj_t *self) {
    return self->frequency; // self->frequency;
}

uint8_t common_hal_sdioio_sdcard_get_width(sdioio_sdcard_obj_t *self) {
    return self->num_data; // self->width;
}

STATIC void check_for_deinit(sdioio_sdcard_obj_t *self) {
}

STATIC void check_whole_block(mp_buffer_info_t *bufinfo) {
    if (bufinfo->len % 512) {
        mp_raise_ValueError(translate("Buffer length must be a multiple of 512"));
    }
}

STATIC void wait_write_complete(sdioio_sdcard_obj_t *self) {
    if (self->state_programming) {
        sd_mmc_wait_end_of_write_blocks(0);
        self->state_programming = 0;
    }
}

STATIC void debug_print_state(sdioio_sdcard_obj_t *self, const char *what, sd_mmc_err_t r) {
    #if DEBUG_SDIO
    DEBUG_PRINT("%s: %d\n", what, r);
    #endif
}

int common_hal_sdioio_sdcard_writeblocks(sdioio_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *bufinfo) {
    check_for_deinit(self);
    check_whole_block(bufinfo);
    wait_write_complete(self);
    self->state_programming = true;
    sd_mmc_err_t r = sd_mmc_init_write_blocks(0, start_block, bufinfo->len / 512);
    if (r != SD_MMC_OK) {
        debug_print_state(self, "sd_mmc_init_write_blocks", r);
        return -EIO;
    }
    r = sd_mmc_start_write_blocks(bufinfo->buf, bufinfo->len / 512);
    if (r != SD_MMC_OK) {
        debug_print_state(self, "sd_mmc_start_write_blocks", r);
        return -EIO;
    }
    // debug_print_state(self, "after writeblocks OK");
    return 0;
}

int common_hal_sdioio_sdcard_readblocks(sdioio_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *bufinfo) {
    check_for_deinit(self);
    check_whole_block(bufinfo);
    wait_write_complete(self);
    sd_mmc_err_t r = sd_mmc_init_read_blocks(0, start_block, bufinfo->len / 512);
    if (r != SD_MMC_OK) {
        debug_print_state(self, "sd_mmc_init_read_blocks", r);
        return -EIO;
    }
    r = sd_mmc_start_read_blocks(bufinfo->buf, bufinfo->len / 512);
    if (r != SD_MMC_OK) {
        debug_print_state(self, "sd_mmc_start_read_blocks", r);
        return -EIO;
    }
    sd_mmc_wait_end_of_write_blocks(0);
    return 0;
}

bool common_hal_sdioio_sdcard_configure(sdioio_sdcard_obj_t *self, uint32_t frequency, uint8_t bits) {
    check_for_deinit(self);
    return true;
}

bool common_hal_sdioio_sdcard_deinited(sdioio_sdcard_obj_t *self) {
    return self->command_pin == COMMON_HAL_MCU_NO_PIN;
}

void common_hal_sdioio_sdcard_deinit(sdioio_sdcard_obj_t *self) {
    reset_pin_number(self->command_pin);
    reset_pin_number(self->clock_pin);
    reset_pin_number(self->data_pins[0]);
    reset_pin_number(self->data_pins[1]);
    reset_pin_number(self->data_pins[2]);
    reset_pin_number(self->data_pins[3]);

    self->command_pin = COMMON_HAL_MCU_NO_PIN;
    self->clock_pin = COMMON_HAL_MCU_NO_PIN;
    self->data_pins[0] = COMMON_HAL_MCU_NO_PIN;
    self->data_pins[1] = COMMON_HAL_MCU_NO_PIN;
    self->data_pins[2] = COMMON_HAL_MCU_NO_PIN;
    self->data_pins[3] = COMMON_HAL_MCU_NO_PIN;
}

void common_hal_sdioio_sdcard_never_reset(sdioio_sdcard_obj_t *self) {
}
