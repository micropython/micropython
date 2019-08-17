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

#include <string.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "lib/oofatfs/ff.h"
#include "extmod/vfs_fat.h"

#include "sdcard.h"
#include "pin.h"
#include "pin_static_af.h"
#include "bufhelper.h"
#include "dma.h"
#include "irq.h"

#if MICROPY_HW_ENABLE_SDCARD

// These are definitions for F1 MCUs so there is a common macro across all f103xx MCUs.
#define SDIO_USE_GPDMA 1

// If no custom SDIO pins defined, use the default ones
#ifndef MICROPY_HW_SDIO_CK
#define MICROPY_HW_SDIO_D0  (pin_C8)
#define MICROPY_HW_SDIO_D1  (pin_C9)
#define MICROPY_HW_SDIO_D2  (pin_C10)
#define MICROPY_HW_SDIO_D3  (pin_C11)
#define MICROPY_HW_SDIO_CK  (pin_C12)
#define MICROPY_HW_SDIO_CMD (pin_D2)
#endif

#define PYB_SD_FLAG_ACTIVE   (0x04)

static uint8_t pyb_sdcard_flags;
static SD_HandleTypeDef sd_handle;

void sdcard_init(void) {
    // Set SD to no mode and inactive
    pyb_sdcard_flags = 0;

    // configure SD GPIO
    // we do this here an not in HAL_SD_MspInit because it apparently
    // makes it more robust to have the pins always pulled high
    // Note: the mp_hal_pin_config function will configure the GPIO in
    // fast mode which can do up to 50MHz.  This should be plenty for SDIO
    // which clocks up to 25MHz maximum.
    // Default SDIO config
    // TODO: the alt pin config error when input
    mp_hal_pin_config_alt_static(MICROPY_HW_SDIO_D0, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDIO_D0);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDIO_D1, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDIO_D1);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDIO_D2, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDIO_D2);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDIO_D3, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDIO_D3);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDIO_CK, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDIO_CK);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDIO_CMD, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDIO_CMD);

    // configure the SD card detect pin
    // we do this here so we can detect if the SD card is inserted before powering it on
    mp_hal_pin_config(MICROPY_HW_SDCARD_DETECT_PIN, MP_HAL_PIN_MODE_INPUT, MICROPY_HW_SDCARD_DETECT_PULL, 0);
}

void HAL_SD_MspInit(SD_HandleTypeDef *hsd) {
    // enable SDIO clock
    __HAL_RCC_SDIO_CLK_ENABLE();

    // NVIC configuration for SDIO interrupts
    NVIC_SetPriority(SDIO_IRQn, IRQ_PRI_SDIO);
    HAL_NVIC_EnableIRQ(SDIO_IRQn);

    // GPIO have already been initialised by sdcard_init
}

void HAL_SD_MspDeInit(SD_HandleTypeDef *hsd) {
    HAL_NVIC_DisableIRQ(SDIO_IRQn);
    __HAL_RCC_SDIO_CLK_DISABLE();
}

bool sdcard_is_present(void) {
    return HAL_GPIO_ReadPin(MICROPY_HW_SDCARD_DETECT_PIN->gpio, MICROPY_HW_SDCARD_DETECT_PIN->pin_mask) == MICROPY_HW_SDCARD_DETECT_PRESENT;
}

STATIC HAL_StatusTypeDef sdcard_hal_init(void) {
    // SD device interface configuration
    sd_handle.Instance = SDIO;
    sd_handle.Init.ClockEdge           = SDIO_CLOCK_EDGE_RISING;
    sd_handle.Init.ClockPowerSave      = SDIO_CLOCK_POWER_SAVE_ENABLE;
    sd_handle.Init.BusWide             = SDIO_BUS_WIDE_1B; // Default
    sd_handle.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    sd_handle.Init.ClockDiv            = SDIO_TRANSFER_CLK_DIV;

    // init the SD interface, with retry if it's not ready yet
    HAL_StatusTypeDef status;
    for (int retry = 10; (status = HAL_SD_Init(&sd_handle)) != HAL_OK; retry--) {
        if (retry == 0) {
            return status;
        }
        mp_hal_delay_ms(50);
    }

    // configure the SD bus width for wide operation
    status = HAL_SD_ConfigWideBusOperation(&sd_handle, SDIO_BUS_WIDE_4B);
    if (status != HAL_OK) {
        HAL_SD_DeInit(&sd_handle);
        return status;
    }

    return HAL_OK;
}

bool sdcard_power_on(void) {
    if (pyb_sdcard_flags == PYB_SD_FLAG_ACTIVE) {
        return true;
    }

    HAL_StatusTypeDef status = HAL_ERROR;
    if (sdcard_is_present()) {
        status = sdcard_hal_init();
    }

    if (status == HAL_OK) {
        pyb_sdcard_flags = PYB_SD_FLAG_ACTIVE;
        return true;
    } else {
        return false;
    }
}

void sdcard_power_off(void) {
    if (pyb_sdcard_flags == PYB_SD_FLAG_ACTIVE) {
        HAL_SD_DeInit(&sd_handle);
    }
    pyb_sdcard_flags = 0;
}

uint64_t sdcard_get_capacity_in_bytes(void) {
    if (pyb_sdcard_flags == PYB_SD_FLAG_ACTIVE) {
            HAL_SD_CardInfoTypeDef cardinfo;
            HAL_SD_GetCardInfo(&sd_handle, &cardinfo);
            return (uint64_t)cardinfo.LogBlockNbr * (uint64_t)cardinfo.LogBlockSize;
    }
    return 0;
}

void SDIO_IRQHandler(void) {
    IRQ_ENTER(SDIO_IRQn);
    if (pyb_sdcard_flags == PYB_SD_FLAG_ACTIVE) {
        HAL_SD_IRQHandler(&sd_handle);
    }
    IRQ_EXIT(SDIO_IRQn);
}

STATIC void sdcard_reset_periph(void) {
    // Fully reset the SDIO peripheral before calling HAL SD DMA functions.
    // (There could be an outstanding DTIMEOUT event from a previous call and the
    // HAL function enables IRQs before fully configuring the SDIO peripheral.)
    SDIO->DTIMER = 0;
    SDIO->DLEN = 0;
    SDIO->DCTRL = 0;
    SDIO->ICR = SDIO_STATIC_FLAGS;
}

STATIC HAL_StatusTypeDef sdcard_wait_finished(uint32_t timeout) {
    // Wait for HAL driver to be ready (eg for DMA to finish)
    uint32_t start = HAL_GetTick();
    for (;;) {
        // Do an atomic check of the state; WFI will exit even if IRQs are disabled
        uint32_t irq_state = disable_irq();
        if (sd_handle.State != HAL_SD_STATE_BUSY) {
            enable_irq(irq_state);
            break;
        }
        __WFI();
        enable_irq(irq_state);
        if (HAL_GetTick() - start >= timeout) {
            return HAL_TIMEOUT;
        }
    }

    // Wait for SD card to complete the operation
    for (;;) {
        uint32_t state = HAL_SD_GetCardState(&sd_handle);

        if (state == HAL_SD_CARD_TRANSFER) {
            return HAL_OK;
        }
        if (!(state == HAL_SD_CARD_SENDING || state == HAL_SD_CARD_RECEIVING || state == HAL_SD_CARD_PROGRAMMING)) {
            return HAL_ERROR;
        }
        if (HAL_GetTick() - start >= timeout) {
            return HAL_TIMEOUT;
        }
        __WFI();
    }
    return HAL_OK;
}

mp_uint_t sdcard_read_blocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks) {
    // check that SD card is initialised
    if (pyb_sdcard_flags != PYB_SD_FLAG_ACTIVE) {
        return HAL_ERROR;
    }

    HAL_StatusTypeDef err = HAL_OK;

    // check that dest pointer is aligned on a 4-byte boundary
    uint8_t *orig_dest = NULL;
    uint32_t saved_word;
    if (((uint32_t)dest & 3) != 0) {
        // Pointer is not aligned so it needs fixing.
        // We could allocate a temporary block of RAM (as sdcard_write_blocks
        // does) but instead we are going to use the dest buffer inplace.  We
        // are going to align the pointer, save the initial word at the aligned
        // location, read into the aligned memory, move the memory back to the
        // unaligned location, then restore the initial bytes at the aligned
        // location.  We should have no trouble doing this as those initial
        // bytes at the aligned location should be able to be changed for the
        // duration of this function call.
        orig_dest = dest;
        dest = (uint8_t*)((uint32_t)dest & ~3);
        saved_word = *(uint32_t*)dest;
    }

    if (query_irq() == IRQ_STATE_ENABLED) {
        // we must disable USB irqs to prevent MSC contention with SD card
        uint32_t basepri = raise_irq_pri(IRQ_PRI_USB);

        #if SDIO_USE_GPDMA
        DMA_HandleTypeDef sd_dma;
        dma_init(&sd_dma, &dma_SDIO_0, DMA_PERIPH_TO_MEMORY, &sd_handle);
        sd_handle.hdmarx = &sd_dma;
        #endif

        // make sure cache is flushed and invalidated so when DMA updates the RAM
        // from reading the peripheral the CPU then reads the new data
        MP_HAL_CLEANINVALIDATE_DCACHE(dest, num_blocks * SDCARD_BLOCK_SIZE);

        sdcard_reset_periph();
        err = HAL_SD_ReadBlocks_DMA(&sd_handle, dest, block_num, num_blocks);
        if (err == HAL_OK) {
            err = sdcard_wait_finished(60000);
        }

        #if SDIO_USE_GPDMA
        dma_deinit(&dma_SDIO_0);
        sd_handle.hdmarx = NULL;
        #endif

        restore_irq_pri(basepri);
    } else {
        err = HAL_SD_ReadBlocks(&sd_handle, dest, block_num, num_blocks, 60000);
        if (err == HAL_OK) {
            err = sdcard_wait_finished(60000);
        }
    }

    if (orig_dest != NULL) {
        // move the read data to the non-aligned position, and restore the initial bytes
        memmove(orig_dest, dest, num_blocks * SDCARD_BLOCK_SIZE);
        memcpy(dest, &saved_word, orig_dest - dest);
    }

    return err;
}

mp_uint_t sdcard_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks) {
    // check that SD card is initialised
    if (pyb_sdcard_flags != PYB_SD_FLAG_ACTIVE) {
        return HAL_ERROR;
    }

    HAL_StatusTypeDef err = HAL_OK;

    // check that src pointer is aligned on a 4-byte boundary
    if (((uint32_t)src & 3) != 0) {
        // pointer is not aligned, so allocate a temporary block to do the write
        uint8_t *src_aligned = m_new_maybe(uint8_t, SDCARD_BLOCK_SIZE);
        if (src_aligned == NULL) {
            return HAL_ERROR;
        }
        for (size_t i = 0; i < num_blocks; ++i) {
            memcpy(src_aligned, src + i * SDCARD_BLOCK_SIZE, SDCARD_BLOCK_SIZE);
            err = sdcard_write_blocks(src_aligned, block_num + i, 1);
            if (err != HAL_OK) {
                break;
            }
        }
        m_del(uint8_t, src_aligned, SDCARD_BLOCK_SIZE);
        return err;
    }

    if (query_irq() == IRQ_STATE_ENABLED) {
        // we must disable USB irqs to prevent MSC contention with SD card
        uint32_t basepri = raise_irq_pri(IRQ_PRI_USB);

        #if SDIO_USE_GPDMA
        DMA_HandleTypeDef sd_dma;
        dma_init(&sd_dma, &dma_SDIO_0, DMA_MEMORY_TO_PERIPH, &sd_handle);
        sd_handle.hdmatx = &sd_dma;
        #endif

        // make sure cache is flushed to RAM so the DMA can read the correct data
        MP_HAL_CLEAN_DCACHE(src, num_blocks * SDCARD_BLOCK_SIZE);

        sdcard_reset_periph();
        err = HAL_SD_WriteBlocks_DMA(&sd_handle, (uint8_t*)src, block_num, num_blocks);
        if (err == HAL_OK) {
            err = sdcard_wait_finished(60000);
        }

        #if SDIO_USE_GPDMA
        dma_deinit(&dma_SDIO_0);
        sd_handle.hdmatx = NULL;
        #endif

        restore_irq_pri(basepri);
    } else {
        err = HAL_SD_WriteBlocks(&sd_handle, (uint8_t*)src, block_num, num_blocks, 60000);
        if (err == HAL_OK) {
            err = sdcard_wait_finished(60000);
        }
    }

    return err;
}

/******************************************************************************/
// MicroPython bindings
//
// Expose the SD card as an object with the block protocol.

// There are singleton SDCard objects
const mp_obj_base_t pyb_sdcard_obj = {&pyb_sdcard_type};

STATIC mp_obj_t pyb_sdcard_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return singleton object
    return MP_OBJ_FROM_PTR(&pyb_sdcard_obj);
}

STATIC mp_obj_t sd_present(mp_obj_t self) {
    return mp_obj_new_bool(sdcard_is_present());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sd_present_obj, sd_present);

STATIC mp_obj_t sd_power(mp_obj_t self, mp_obj_t state) {
    bool result;
    if (mp_obj_is_true(state)) {
        result = sdcard_power_on();
    } else {
        sdcard_power_off();
        result = true;
    }
    return mp_obj_new_bool(result);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(sd_power_obj, sd_power);

STATIC mp_obj_t sd_info(mp_obj_t self) {
    if (pyb_sdcard_flags != PYB_SD_FLAG_ACTIVE) {
        return mp_const_none;
    }

    uint32_t card_type = sd_handle.SdCard.CardType;
    uint32_t log_block_nbr = sd_handle.SdCard.LogBlockNbr;
    uint32_t log_block_size = sd_handle.SdCard.LogBlockSize;
    // cardinfo.SD_csd and cardinfo.SD_cid have lots of info but we don't use them
    mp_obj_t tuple[3] = {
        mp_obj_new_int_from_ull((uint64_t)log_block_nbr * (uint64_t)log_block_size),
        mp_obj_new_int_from_uint(log_block_size),
        mp_obj_new_int(card_type),
    };
    return mp_obj_new_tuple(3, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sd_info_obj, sd_info);

// now obsolete, kept for backwards compatibility
STATIC mp_obj_t sd_read(mp_obj_t self, mp_obj_t block_num) {
    uint8_t *dest = m_new(uint8_t, SDCARD_BLOCK_SIZE);
    mp_uint_t ret = sdcard_read_blocks(dest, mp_obj_get_int(block_num), 1);

    if (ret != 0) {
        m_del(uint8_t, dest, SDCARD_BLOCK_SIZE);
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "sdcard_read_blocks failed [%u]", ret));
    }

    return mp_obj_new_bytearray_by_ref(SDCARD_BLOCK_SIZE, dest);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(sd_read_obj, sd_read);

// now obsolete, kept for backwards compatibility
STATIC mp_obj_t sd_write(mp_obj_t self, mp_obj_t block_num, mp_obj_t data) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);
    if (bufinfo.len % SDCARD_BLOCK_SIZE != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "writes must be a multiple of %d bytes", SDCARD_BLOCK_SIZE));
    }

    mp_uint_t ret = sdcard_write_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / SDCARD_BLOCK_SIZE);

    if (ret != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "sdcard_write_blocks failed [%u]", ret));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(sd_write_obj, sd_write);

STATIC mp_obj_t pyb_sdcard_readblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);
    mp_uint_t ret = sdcard_read_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / SDCARD_BLOCK_SIZE);
    return mp_obj_new_bool(ret == 0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_sdcard_readblocks_obj, pyb_sdcard_readblocks);

STATIC mp_obj_t pyb_sdcard_writeblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    mp_uint_t ret = sdcard_write_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / SDCARD_BLOCK_SIZE);
    return mp_obj_new_bool(ret == 0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_sdcard_writeblocks_obj, pyb_sdcard_writeblocks);

STATIC mp_obj_t pyb_sdcard_ioctl(mp_obj_t self, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case BP_IOCTL_INIT:
            if (!sdcard_power_on()) {
                return MP_OBJ_NEW_SMALL_INT(-1); // error
            }
            return MP_OBJ_NEW_SMALL_INT(0); // success

        case BP_IOCTL_DEINIT:
            sdcard_power_off();
            return MP_OBJ_NEW_SMALL_INT(0); // success

        case BP_IOCTL_SYNC:
            // nothing to do
            return MP_OBJ_NEW_SMALL_INT(0); // success

        case BP_IOCTL_SEC_COUNT:
            return MP_OBJ_NEW_SMALL_INT(sdcard_get_capacity_in_bytes() / SDCARD_BLOCK_SIZE);

        case BP_IOCTL_SEC_SIZE:
            return MP_OBJ_NEW_SMALL_INT(SDCARD_BLOCK_SIZE);

        default: // unknown command
            return MP_OBJ_NEW_SMALL_INT(-1); // error
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_sdcard_ioctl_obj, pyb_sdcard_ioctl);

STATIC const mp_rom_map_elem_t pyb_sdcard_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_present), MP_ROM_PTR(&sd_present_obj) },
    { MP_ROM_QSTR(MP_QSTR_power), MP_ROM_PTR(&sd_power_obj) },
    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&sd_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&sd_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&sd_write_obj) },
    // block device protocol
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&pyb_sdcard_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&pyb_sdcard_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&pyb_sdcard_ioctl_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_sdcard_locals_dict, pyb_sdcard_locals_dict_table);

const mp_obj_type_t pyb_sdcard_type = {
    { &mp_type_type },
    .name = MP_QSTR_SDCard,
    .make_new = pyb_sdcard_make_new,
    .locals_dict = (mp_obj_dict_t*)&pyb_sdcard_locals_dict,
};

void sdcard_init_vfs(fs_user_mount_t *vfs, int part) {
    vfs->base.type = &mp_fat_vfs_type;
    vfs->flags |= FSUSER_NATIVE | FSUSER_HAVE_IOCTL;
    vfs->fatfs.drv = vfs;
    vfs->fatfs.part = part;
    vfs->readblocks[0] = MP_OBJ_FROM_PTR(&pyb_sdcard_readblocks_obj);
    vfs->readblocks[1] = MP_OBJ_FROM_PTR(&pyb_sdcard_obj);
    vfs->readblocks[2] = MP_OBJ_FROM_PTR(sdcard_read_blocks); // native version
    vfs->writeblocks[0] = MP_OBJ_FROM_PTR(&pyb_sdcard_writeblocks_obj);
    vfs->writeblocks[1] = MP_OBJ_FROM_PTR(&pyb_sdcard_obj);
    vfs->writeblocks[2] = MP_OBJ_FROM_PTR(sdcard_write_blocks); // native version
    vfs->u.ioctl[0] = MP_OBJ_FROM_PTR(&pyb_sdcard_ioctl_obj);
    vfs->u.ioctl[1] = MP_OBJ_FROM_PTR(&pyb_sdcard_obj);
}

#endif // MICROPY_HW_ENABLE_SDCARD
