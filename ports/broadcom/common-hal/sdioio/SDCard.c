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

#include "mphalport.h"

#include "py/mperrno.h"
#include "py/runtime.h"

#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/sdioio/SDCard.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "supervisor/port.h"

#include "peripherals/broadcom/cpu.h"
#include "peripherals/broadcom/defines.h"
#include "peripherals/broadcom/gpio.h"

#include "lib/sdmmc/include/sdmmc_cmd.h"

void osal_task_delay(uint32_t msec) {
    mp_hal_delay_ms(msec);
}

/*!< Host function to initialize the driver */
STATIC sdmmc_err_t _init(void) {
    return SDMMC_OK;
}

/*!< host function to set bus width */
STATIC sdmmc_err_t _set_bus_width(int slot, size_t width) {
    if (width == 4) {
        EMMC->CONTROL0_b.HCTL_DWIDTH = true;
    } else if (width == 8) {
        EMMC->CONTROL0_b.HCTL_8BIT = true;
    }
    return SDMMC_OK;
}

/*!< host function to get the maximum bus width of a particular slot */
STATIC size_t _get_bus_width(int slot) {
    return 4;
}

/*!< host function to set card clock frequency */
STATIC sdmmc_err_t _set_card_clk(int slot, uint32_t freq_khz) {
    uint32_t base_clock = 125000000;
    uint32_t frequency = freq_khz * 1000;
    uint64_t start_ticks = port_get_raw_ticks(NULL);
    while ((EMMC->STATUS_b.CMD_INHIBIT || EMMC->STATUS_b.DAT_INHIBIT) && (port_get_raw_ticks(NULL) - start_ticks) < 1024) {
    }
    if (EMMC->STATUS_b.CMD_INHIBIT || EMMC->STATUS_b.DAT_INHIBIT) {
        return SDMMC_ERR_TIMEOUT;
    }

    EMMC->CONTROL1_b.CLK_EN = false;

    uint32_t divisor = base_clock / frequency;
    // + 1 to ensure we round the frequency down.
    if (base_clock % frequency > 0) {
        divisor += 1;
    }
    // Only even divisors are supported
    uint32_t register_value = divisor / 2;
    if (divisor % 2 == 1) {
        register_value += 1;
    }

    // The divisor is stored with bits swapped.
    EMMC->CONTROL1_b.CLK_FREQ8 = divisor & 0xff;
    EMMC->CONTROL1_b.CLK_FREQ_MS2 = (divisor >> 8) & 0x3;

    EMMC->CONTROL1_b.CLK_EN = true;
    start_ticks = port_get_raw_ticks(NULL);
    while (!EMMC->CONTROL1_b.CLK_STABLE && (port_get_raw_ticks(NULL) - start_ticks) < 1024) {
    }
    if (!EMMC->CONTROL1_b.CLK_STABLE) {
        return SDMMC_ERR_TIMEOUT;
    }
    return SDMMC_OK;
}

/*!< host function to do a transaction */
STATIC sdmmc_err_t _do_transaction(int slot, sdmmc_command_t *cmdinfo) {
    if (EMMC->STATUS_b.CMD_INHIBIT) {
        return SDMMC_ERR_BUSY;
    }

    // Clear interrupts and then send the command
    EMMC->INTERRUPT = 0xffffffff;
    EMMC->ARG1 = cmdinfo->arg;

    uint32_t cmd_flags = 0;
    bool read = (cmdinfo->flags & SCF_CMD_READ) != 0;
    if (SCF_CMD(cmdinfo->flags) == SCF_CMD_ADTC ||
        SCF_CMD(cmdinfo->flags) == (SCF_CMD_ADTC | SCF_CMD_READ)) {
        if (EMMC->STATUS_b.DAT_INHIBIT) {
            return SDMMC_ERR_BUSY;
        }
        cmd_flags = Arasan_EMMC_Distributor_CMDTM_TM_BLKCNT_EN_Msk | Arasan_EMMC_Distributor_CMDTM_CMD_ISDATA_Msk;
        if (cmdinfo->datalen > cmdinfo->blklen) {
            cmd_flags |= Arasan_EMMC_Distributor_CMDTM_TM_MULTI_BLOCK_Msk;
            if ((cmdinfo->flags & SCF_AUTO_STOP) != 0) {
                cmd_flags |= 1 << Arasan_EMMC_Distributor_CMDTM_TM_AUTO_CMD_EN_Pos;
            }
        }
        if (read) {
            cmd_flags |= Arasan_EMMC_Distributor_CMDTM_TM_DAT_DIR_Msk;
        }
        EMMC->BLKSIZECNT = (cmdinfo->datalen / cmdinfo->blklen) << Arasan_EMMC_Distributor_BLKSIZECNT_BLKCNT_Pos |
            cmdinfo->blklen << Arasan_EMMC_Distributor_BLKSIZECNT_BLKSIZE_Pos;
    }

    uint32_t response_type = EMMC_CMDTM_CMD_RSPNS_TYPE_RESPONSE_48BITS;
    uint32_t crc = 0;
    if ((cmdinfo->flags & SCF_RSP_CRC) != 0) {
        crc |= Arasan_EMMC_Distributor_CMDTM_CMD_CRCCHK_EN_Msk;
    }
    if ((cmdinfo->flags & SCF_RSP_IDX) != 0) {
        crc |= Arasan_EMMC_Distributor_CMDTM_CMD_IXCHK_EN_Msk;
    }
    if ((cmdinfo->flags & SCF_RSP_136) != 0) {
        response_type = EMMC_CMDTM_CMD_RSPNS_TYPE_RESPONSE_136BITS;
    } else if ((cmdinfo->flags & SCF_RSP_BSY) != 0) {
        response_type = EMMC_CMDTM_CMD_RSPNS_TYPE_RESPONSE_48BITS_USING_BUSY;
    } else if ((cmdinfo->flags & SCF_RSP_PRESENT) == 0) {
        response_type = EMMC_CMDTM_CMD_RSPNS_TYPE_RESPONSE_NONE;
    }
    uint32_t full_cmd = cmd_flags | crc |
        cmdinfo->opcode << Arasan_EMMC_Distributor_CMDTM_CMD_INDEX_Pos |
        response_type << Arasan_EMMC_Distributor_CMDTM_CMD_RSPNS_TYPE_Pos;
    EMMC->CMDTM = full_cmd;

    // Wait for an interrupt to indicate completion of the command.
    uint64_t start_ticks = port_get_raw_ticks(NULL);
    while (EMMC->INTERRUPT == 0 && (port_get_raw_ticks(NULL) - start_ticks) < (size_t)cmdinfo->timeout_ms) {
    }
    if (!EMMC->INTERRUPT_b.CMD_DONE) {
        cmdinfo->response[0] = 0;
        // Reset the command circuit
        EMMC->CONTROL1_b.SRST_CMD = true;
        // Wait for the bit to clear
        start_ticks = port_get_raw_ticks(NULL);
        while (EMMC->CONTROL1_b.SRST_CMD && (port_get_raw_ticks(NULL) - start_ticks) < (size_t)cmdinfo->timeout_ms) {
        }
        return SDMMC_ERR_TIMEOUT;
    } else {
        EMMC->INTERRUPT = Arasan_EMMC_Distributor_INTERRUPT_CMD_DONE_Msk;
    }

    // Transfer the data.
    // TODO: Use DMA
    if (cmd_flags != 0) {
        if (read) {
            for (size_t i = 0; i < cmdinfo->datalen / sizeof(uint32_t); i++) {
                start_ticks = port_get_raw_ticks(NULL);
                while (!EMMC->INTERRUPT_b.READ_RDY && (port_get_raw_ticks(NULL) - start_ticks) < (size_t)cmdinfo->timeout_ms) {
                }
                if (!EMMC->INTERRUPT_b.READ_RDY) {
                    return SDMMC_ERR_TIMEOUT;
                }
                ((uint32_t *)cmdinfo->data)[i] = EMMC->DATA;
            }
        } else {
            for (size_t i = 0; i < cmdinfo->datalen / sizeof(uint32_t); i++) {
                start_ticks = port_get_raw_ticks(NULL);
                while (!EMMC->INTERRUPT_b.WRITE_RDY && (port_get_raw_ticks(NULL) - start_ticks) < (size_t)cmdinfo->timeout_ms) {
                }
                if (!EMMC->INTERRUPT_b.WRITE_RDY) {
                    return SDMMC_ERR_TIMEOUT;
                }
                EMMC->DATA = ((uint32_t *)cmdinfo->data)[i];
            }
        }
        uint32_t data_done_mask = Arasan_EMMC_Distributor_INTERRUPT_ERR_Msk | Arasan_EMMC_Distributor_INTERRUPT_DATA_DONE_Msk;
        start_ticks = port_get_raw_ticks(NULL);
        while ((EMMC->INTERRUPT & data_done_mask) == 0 && (port_get_raw_ticks(NULL) - start_ticks) < (size_t)cmdinfo->timeout_ms) {
        }
        if (!EMMC->INTERRUPT_b.DATA_DONE) {
            cmdinfo->response[0] = 0;
            // Reset the command circuit
            EMMC->CONTROL1_b.SRST_DATA = true;
            // Wait for the bit to clear
            start_ticks = port_get_raw_ticks(NULL);
            while (EMMC->CONTROL1_b.SRST_DATA && (port_get_raw_ticks(NULL) - start_ticks) < (size_t)cmdinfo->timeout_ms) {
            }
            return SDMMC_ERR_TIMEOUT;
        }
    }

    cmdinfo->response[0] = EMMC->RESP0;
    if (response_type == EMMC_CMDTM_CMD_RSPNS_TYPE_RESPONSE_136BITS) {
        // Rotate one more byte for some reason.
        cmdinfo->response[3] = EMMC->RESP3 << 8 | ((EMMC->RESP2 >> 24) & 0xff);
        cmdinfo->response[2] = EMMC->RESP2 << 8 | ((EMMC->RESP1 >> 24) & 0xff);
        cmdinfo->response[1] = EMMC->RESP1 << 8 | ((EMMC->RESP0 >> 24) & 0xff);
        cmdinfo->response[0] <<= 8;
    }
    return SDMMC_OK;
}

/*!< host function to deinitialize the driver, called with the `slot` */
STATIC sdmmc_err_t _deinit(int slot) {
    return SDMMC_OK;
}

void common_hal_sdioio_sdcard_construct(sdioio_sdcard_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *command,
    uint8_t num_data, const mcu_pin_obj_t **data, uint32_t frequency) {

    if (clock != NULL) {
        gpio_set_function(clock->number, GPIO_GPFSEL4_FSEL48_SD1_CLK);
        gpio_set_pull(clock->number, BP_PULL_NONE);
        self->clock_pin = clock->number;
        gpio_set_function(command->number, GPIO_GPFSEL4_FSEL49_SD1_CMD);
        gpio_set_pull(command->number, BP_PULL_UP);
        self->command_pin = command->number;
        gpio_set_function(data[0]->number, GPIO_GPFSEL5_FSEL50_SD1_DAT0);
        gpio_set_pull(data[0]->number, BP_PULL_UP);
        self->data_pins[0] = data[0]->number;
        gpio_set_function(data[1]->number, GPIO_GPFSEL5_FSEL51_SD1_DAT1);
        gpio_set_pull(data[1]->number, BP_PULL_UP);
        self->data_pins[1] = data[1]->number;
        gpio_set_function(data[2]->number, GPIO_GPFSEL5_FSEL52_SD1_DAT2);
        gpio_set_pull(data[2]->number, BP_PULL_UP);
        self->data_pins[2] = data[2]->number;
        gpio_set_function(data[3]->number, GPIO_GPFSEL5_FSEL53_SD1_DAT3);
        gpio_set_pull(data[3]->number, BP_PULL_UP);
        self->data_pins[3] = data[3]->number;
    } else {
        // Switch the sdcard to use the old arasan interface.
        GPIO->EXTRA_MUX_b.SDIO = GPIO_EXTRA_MUX_SDIO_ARASAN;
    }

    COMPLETE_MEMORY_READS;

    self->host_info = (sdmmc_host_t) {
        .flags = SDMMC_HOST_FLAG_1BIT | SDMMC_HOST_FLAG_4BIT | SDMMC_HOST_FLAG_DEINIT_ARG,
        .slot = 0,
        .max_freq_khz = SDMMC_FREQ_HIGHSPEED,
        .io_voltage = 3.3f,
        .command_timeout_ms = 0,
        .init = _init,
        .set_bus_width = _set_bus_width,
        .get_bus_width = _get_bus_width,
        .set_bus_ddr_mode = NULL,
        .set_card_clk = _set_card_clk,
        .do_transaction = _do_transaction,
        .deinit_p = _deinit,
    };

    // Reset the peripheral.
    EMMC->CONTROL0 = 0;
    EMMC->CONTROL1_b.SRST_HC = true;
    // Wait for the bit to clear
    while (EMMC->CONTROL1_b.SRST_HC) {

    }
    // Set max timeout
    EMMC->CONTROL1 |= Arasan_EMMC_Distributor_CONTROL1_CLK_INTLEN_Msk | (0xe << Arasan_EMMC_Distributor_CONTROL1_DATA_TOUNIT_Pos);

    EMMC->IRPT_MASK = 0xffffffff;

    // Start clocking the card.
    _set_card_clk(0, 400);

    sdmmc_err_t err = SDMMC_ERR_INVALID_RESPONSE;
    size_t tries = 3;
    while (err == SDMMC_ERR_INVALID_RESPONSE && tries > 0) {
        err = sdmmc_card_init(&self->host_info, &self->card_info);
        if (err != SDMMC_OK) {
            mp_printf(&mp_plat_print, "SD card init failed %d\n", err);
        } else if (tries < 3) {
            mp_printf(&mp_plat_print, "SD card init success\n");
        }
        tries--;
    }

    self->init = err == SDMMC_OK;

    self->capacity = self->card_info.csd.capacity;
    COMPLETE_MEMORY_READS;
}

uint32_t common_hal_sdioio_sdcard_get_count(sdioio_sdcard_obj_t *self) {
    return self->capacity;
}

uint32_t common_hal_sdioio_sdcard_get_frequency(sdioio_sdcard_obj_t *self) {
    return self->frequency;
}

uint8_t common_hal_sdioio_sdcard_get_width(sdioio_sdcard_obj_t *self) {
    return self->num_data;
}

STATIC void check_whole_block(mp_buffer_info_t *bufinfo) {
    if (bufinfo->len % 512) {
        mp_raise_ValueError(MP_ERROR_TEXT("Buffer length must be a multiple of 512"));
    }
}

int common_hal_sdioio_sdcard_writeblocks(sdioio_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *bufinfo) {
    if (!self->init) {
        return -EIO;
    }
    check_whole_block(bufinfo);
    self->state_programming = true;

    COMPLETE_MEMORY_READS;
    sdmmc_err_t error = sdmmc_write_sectors(&self->card_info, bufinfo->buf,
        start_block, bufinfo->len / 512);
    COMPLETE_MEMORY_READS;

    if (error != SDMMC_OK) {
        mp_printf(&mp_plat_print, "write sectors result %d\n", error);
        return -EIO;
    }

    return 0;
}

int common_hal_sdioio_sdcard_readblocks(sdioio_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *bufinfo) {
    if (!self->init) {
        return -EIO;
    }
    check_whole_block(bufinfo);
    COMPLETE_MEMORY_READS;
    sdmmc_err_t error = sdmmc_read_sectors(&self->card_info, bufinfo->buf,
        start_block, bufinfo->len / 512);
    COMPLETE_MEMORY_READS;

    if (error != SDMMC_OK) {
        mp_printf(&mp_plat_print, "read sectors result %d when reading block %d for %d\n", error, start_block, bufinfo->len / 512);
        return -EIO;
    }

    return 0;
}

bool common_hal_sdioio_sdcard_configure(sdioio_sdcard_obj_t *self, uint32_t frequency, uint8_t bits) {
    if (!self->init) {
        return false;
    }
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
    self->init = false;
}

void common_hal_sdioio_sdcard_never_reset(sdioio_sdcard_obj_t *self) {
    never_reset_pin_number(self->command_pin);
    never_reset_pin_number(self->clock_pin);
    never_reset_pin_number(self->data_pins[0]);
    never_reset_pin_number(self->data_pins[1]);
    never_reset_pin_number(self->data_pins[2]);
    never_reset_pin_number(self->data_pins[3]);
}
