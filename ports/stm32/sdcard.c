/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#if MICROPY_HW_HAS_SDCARD

#if defined(STM32F7) || defined(STM32H7) || defined(STM32L4)

// The F7 has 2 SDMMC units but at the moment we only support using one of them in
// a given build.  If a boards config file defines MICROPY_HW_SDMMC2_CK then SDMMC2
// is used, otherwise SDMMC1 is used.

#if defined(MICROPY_HW_SDMMC2_CK)
#define SDIO SDMMC2
#define SDMMC_CLK_ENABLE() __HAL_RCC_SDMMC2_CLK_ENABLE()
#define SDMMC_CLK_DISABLE() __HAL_RCC_SDMMC2_CLK_DISABLE()
#define SDMMC_IRQn SDMMC2_IRQn
#define SDMMC_TX_DMA dma_SDMMC_2_TX
#define SDMMC_RX_DMA dma_SDMMC_2_RX
#else
#define SDIO SDMMC1
#define SDMMC_CLK_ENABLE() __HAL_RCC_SDMMC1_CLK_ENABLE()
#define SDMMC_CLK_DISABLE() __HAL_RCC_SDMMC1_CLK_DISABLE()
#define SDMMC_IRQn SDMMC1_IRQn
#define SDMMC_TX_DMA dma_SDIO_0_TX
#define SDMMC_RX_DMA dma_SDIO_0_RX
#define STATIC_AF_SDMMC_CK STATIC_AF_SDMMC1_CK
#define STATIC_AF_SDMMC_CMD STATIC_AF_SDMMC1_CMD
#define STATIC_AF_SDMMC_D0 STATIC_AF_SDMMC1_D0
#define STATIC_AF_SDMMC_D1 STATIC_AF_SDMMC1_D1
#define STATIC_AF_SDMMC_D2 STATIC_AF_SDMMC1_D2
#define STATIC_AF_SDMMC_D3 STATIC_AF_SDMMC1_D3
#endif

// The F7 & L4 series calls the peripheral SDMMC rather than SDIO, so provide some
// #defines for backwards compatability.

#define SDIO_CLOCK_EDGE_RISING              SDMMC_CLOCK_EDGE_RISING
#define SDIO_CLOCK_EDGE_FALLING             SDMMC_CLOCK_EDGE_FALLING

#define SDIO_CLOCK_BYPASS_DISABLE           SDMMC_CLOCK_BYPASS_DISABLE
#define SDIO_CLOCK_BYPASS_ENABLE            SDMMC_CLOCK_BYPASS_ENABLE

#define SDIO_CLOCK_POWER_SAVE_DISABLE       SDMMC_CLOCK_POWER_SAVE_DISABLE
#define SDIO_CLOCK_POWER_SAVE_ENABLE        SDMMC_CLOCK_POWER_SAVE_ENABLE

#define SDIO_BUS_WIDE_1B                    SDMMC_BUS_WIDE_1B
#define SDIO_BUS_WIDE_4B                    SDMMC_BUS_WIDE_4B
#define SDIO_BUS_WIDE_8B                    SDMMC_BUS_WIDE_8B

#define SDIO_HARDWARE_FLOW_CONTROL_DISABLE  SDMMC_HARDWARE_FLOW_CONTROL_DISABLE
#define SDIO_HARDWARE_FLOW_CONTROL_ENABLE   SDMMC_HARDWARE_FLOW_CONTROL_ENABLE

#if defined(STM32H7)
#define GPIO_AF12_SDIO                      GPIO_AF12_SDIO1
#define SDIO_IRQHandler                     SDMMC1_IRQHandler
#define SDIO_TRANSFER_CLK_DIV               SDMMC_NSpeed_CLK_DIV
#define SDIO_USE_GPDMA                      0
#else
#define SDIO_TRANSFER_CLK_DIV               SDMMC_TRANSFER_CLK_DIV
#define SDIO_USE_GPDMA                      1
#endif

#else

// These are definitions for F4 MCUs so there is a common macro across all MCUs.

#define SDMMC_CLK_ENABLE() __SDIO_CLK_ENABLE()
#define SDMMC_CLK_DISABLE() __SDIO_CLK_DISABLE()
#define SDMMC_IRQn SDIO_IRQn
#define SDMMC_TX_DMA dma_SDIO_0_TX
#define SDMMC_RX_DMA dma_SDIO_0_RX
#define SDIO_USE_GPDMA 1
#define STATIC_AF_SDMMC_CK STATIC_AF_SDIO_CK
#define STATIC_AF_SDMMC_CMD STATIC_AF_SDIO_CMD
#define STATIC_AF_SDMMC_D0 STATIC_AF_SDIO_D0
#define STATIC_AF_SDMMC_D1 STATIC_AF_SDIO_D1
#define STATIC_AF_SDMMC_D2 STATIC_AF_SDIO_D2
#define STATIC_AF_SDMMC_D3 STATIC_AF_SDIO_D3

#endif

// If no custom SDIO pins defined, use the default ones
#ifndef MICROPY_HW_SDMMC_CK

#define MICROPY_HW_SDMMC_D0 (pin_C8)
#define MICROPY_HW_SDMMC_D1 (pin_C9)
#define MICROPY_HW_SDMMC_D2 (pin_C10)
#define MICROPY_HW_SDMMC_D3 (pin_C11)
#define MICROPY_HW_SDMMC_CK (pin_C12)
#define MICROPY_HW_SDMMC_CMD (pin_D2)

#endif

// TODO: Since SDIO is fundamentally half-duplex, we really only need to
//       tie up one DMA channel. However, the HAL DMA API doesn't
// seem to provide a convenient way to change the direction. I believe that
// its as simple as changing the CR register and the Init.Direction field
// and make DMA_SetConfig public.

// TODO: I think that as an optimization, we can allocate these dynamically
//       if an sd card is detected. This will save approx 260 bytes of RAM
//       when no sdcard was being used.
static SD_HandleTypeDef sd_handle;
#if SDIO_USE_GPDMA
static DMA_HandleTypeDef sd_rx_dma, sd_tx_dma;
#endif

void sdcard_init(void) {
    // invalidate the sd_handle
    sd_handle.Instance = NULL;

    // configure SD GPIO
    // we do this here an not in HAL_SD_MspInit because it apparently
    // makes it more robust to have the pins always pulled high
    // Note: the mp_hal_pin_config function will configure the GPIO in
    // fast mode which can do up to 50MHz.  This should be plenty for SDIO
    // which clocks up to 25MHz maximum.
    #if defined(MICROPY_HW_SDMMC2_CK)
    // Use SDMMC2 peripheral with pins provided by the board's config
    mp_hal_pin_config_alt_static(MICROPY_HW_SDMMC2_CK, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDMMC2_CK);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDMMC2_CMD, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDMMC2_CMD);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDMMC2_D0, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDMMC2_D0);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDMMC2_D1, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDMMC2_D1);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDMMC2_D2, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDMMC2_D2);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDMMC2_D3, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDMMC2_D3);
    #else
    // Default SDIO/SDMMC1 config
    mp_hal_pin_config_alt_static(MICROPY_HW_SDMMC_D0, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDMMC_D0);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDMMC_D1, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDMMC_D1);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDMMC_D2, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDMMC_D2);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDMMC_D3, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDMMC_D3);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDMMC_CK, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDMMC_CK);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDMMC_CMD, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDMMC_CMD);
    #endif

    // configure the SD card detect pin
    // we do this here so we can detect if the SD card is inserted before powering it on
    mp_hal_pin_config(MICROPY_HW_SDCARD_DETECT_PIN, MP_HAL_PIN_MODE_INPUT, MICROPY_HW_SDCARD_DETECT_PULL, 0);
}

void HAL_SD_MspInit(SD_HandleTypeDef *hsd) {
    // enable SDIO clock
    SDMMC_CLK_ENABLE();

    #if defined(STM32H7)
    // Reset SDMMC
    __HAL_RCC_SDMMC1_FORCE_RESET();
    __HAL_RCC_SDMMC1_RELEASE_RESET();
    #endif

    // NVIC configuration for SDIO interrupts
    NVIC_SetPriority(SDMMC_IRQn, IRQ_PRI_SDIO);
    HAL_NVIC_EnableIRQ(SDMMC_IRQn);

    // GPIO have already been initialised by sdcard_init
}

void HAL_SD_MspDeInit(SD_HandleTypeDef *hsd) {
    HAL_NVIC_DisableIRQ(SDMMC_IRQn);
    SDMMC_CLK_DISABLE();
}

bool sdcard_is_present(void) {
    return HAL_GPIO_ReadPin(MICROPY_HW_SDCARD_DETECT_PIN->gpio, MICROPY_HW_SDCARD_DETECT_PIN->pin_mask) == MICROPY_HW_SDCARD_DETECT_PRESENT;
}

bool sdcard_power_on(void) {
    if (!sdcard_is_present()) {
        return false;
    }
    if (sd_handle.Instance) {
        return true;
    }

    // SD device interface configuration
    sd_handle.Instance = SDIO;
    sd_handle.Init.ClockEdge           = SDIO_CLOCK_EDGE_RISING;
    #ifndef STM32H7
    sd_handle.Init.ClockBypass         = SDIO_CLOCK_BYPASS_DISABLE;
    #endif
    sd_handle.Init.ClockPowerSave      = SDIO_CLOCK_POWER_SAVE_ENABLE;
    sd_handle.Init.BusWide             = SDIO_BUS_WIDE_1B;
    sd_handle.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    sd_handle.Init.ClockDiv            = SDIO_TRANSFER_CLK_DIV;

    // init the SD interface, with retry if it's not ready yet
    for (int retry = 10; HAL_SD_Init(&sd_handle) != HAL_OK; retry--) {
        if (retry == 0) {
            goto error;
        }
        mp_hal_delay_ms(50);
    }

    // configure the SD bus width for wide operation
    #if defined(STM32F7)
    // use maximum SDMMC clock speed on F7 MCUs
    sd_handle.Init.ClockBypass = SDMMC_CLOCK_BYPASS_ENABLE;
    #endif
    if (HAL_SD_ConfigWideBusOperation(&sd_handle, SDIO_BUS_WIDE_4B) != HAL_OK) {
        HAL_SD_DeInit(&sd_handle);
        goto error;
    }

    return true;

error:
    sd_handle.Instance = NULL;
    return false;
}

void sdcard_power_off(void) {
    if (!sd_handle.Instance) {
        return;
    }
    HAL_SD_DeInit(&sd_handle);
    sd_handle.Instance = NULL;
}

uint64_t sdcard_get_capacity_in_bytes(void) {
    if (sd_handle.Instance == NULL) {
        return 0;
    }
    HAL_SD_CardInfoTypeDef cardinfo;
    HAL_SD_GetCardInfo(&sd_handle, &cardinfo);
    return (uint64_t)cardinfo.LogBlockNbr * (uint64_t)cardinfo.LogBlockSize;
}

#if !defined(MICROPY_HW_SDMMC2_CK)
void SDIO_IRQHandler(void) {
    IRQ_ENTER(SDIO_IRQn);
    HAL_SD_IRQHandler(&sd_handle);
    IRQ_EXIT(SDIO_IRQn);
}
#endif

#if defined(STM32F7)
void SDMMC2_IRQHandler(void) {
    IRQ_ENTER(SDMMC2_IRQn);
    HAL_SD_IRQHandler(&sd_handle);
    IRQ_EXIT(SDMMC2_IRQn);
}
#endif

STATIC HAL_StatusTypeDef sdcard_wait_finished(SD_HandleTypeDef *sd, uint32_t timeout) {
    // Wait for HAL driver to be ready (eg for DMA to finish)
    uint32_t start = HAL_GetTick();
    for (;;) {
        // Do an atomic check of the state; WFI will exit even if IRQs are disabled
        uint32_t irq_state = disable_irq();
        if (sd->State != HAL_SD_STATE_BUSY) {
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
        HAL_SD_CardStateTypedef state = HAL_SD_GetCardState(sd);
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
    if (sd_handle.Instance == NULL) {
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
        uint32_t basepri = raise_irq_pri(IRQ_PRI_OTG_FS);

        #if SDIO_USE_GPDMA
        dma_init(&sd_rx_dma, &SDMMC_RX_DMA, &sd_handle);
        sd_handle.hdmarx = &sd_rx_dma;
        #endif

        // make sure cache is flushed and invalidated so when DMA updates the RAM
        // from reading the peripheral the CPU then reads the new data
        MP_HAL_CLEANINVALIDATE_DCACHE(dest, num_blocks * SDCARD_BLOCK_SIZE);

        err = HAL_SD_ReadBlocks_DMA(&sd_handle, dest, block_num, num_blocks);
        if (err == HAL_OK) {
            err = sdcard_wait_finished(&sd_handle, 60000);
        }

        #if SDIO_USE_GPDMA
        dma_deinit(&SDMMC_RX_DMA);
        sd_handle.hdmarx = NULL;
        #endif

        restore_irq_pri(basepri);
    } else {
        err = HAL_SD_ReadBlocks(&sd_handle, dest, block_num, num_blocks, 60000);
        if (err == HAL_OK) {
            err = sdcard_wait_finished(&sd_handle, 60000);
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
    if (sd_handle.Instance == NULL) {
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
        uint32_t basepri = raise_irq_pri(IRQ_PRI_OTG_FS);

        #if SDIO_USE_GPDMA
        dma_init(&sd_tx_dma, &SDMMC_TX_DMA, &sd_handle);
        sd_handle.hdmatx = &sd_tx_dma;
        #endif

        // make sure cache is flushed to RAM so the DMA can read the correct data
        MP_HAL_CLEAN_DCACHE(src, num_blocks * SDCARD_BLOCK_SIZE);

        err = HAL_SD_WriteBlocks_DMA(&sd_handle, (uint8_t*)src, block_num, num_blocks);
        if (err == HAL_OK) {
            err = sdcard_wait_finished(&sd_handle, 60000);
        }

        #if SDIO_USE_GPDMA
        dma_deinit(&SDMMC_TX_DMA);
        sd_handle.hdmatx = NULL;
        #endif

        restore_irq_pri(basepri);
    } else {
        err = HAL_SD_WriteBlocks(&sd_handle, (uint8_t*)src, block_num, num_blocks, 60000);
        if (err == HAL_OK) {
            err = sdcard_wait_finished(&sd_handle, 60000);
        }
    }

    return err;
}

/******************************************************************************/
// MicroPython bindings
//
// Expose the SD card as an object with the block protocol.

// there is a singleton SDCard object
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
    if (sd_handle.Instance == NULL) {
        return mp_const_none;
    }
    HAL_SD_CardInfoTypeDef cardinfo;
    HAL_SD_GetCardInfo(&sd_handle, &cardinfo);
    // cardinfo.SD_csd and cardinfo.SD_cid have lots of info but we don't use them
    mp_obj_t tuple[3] = {
        mp_obj_new_int_from_ull((uint64_t)cardinfo.LogBlockNbr * (uint64_t)cardinfo.LogBlockSize),
        mp_obj_new_int_from_uint(cardinfo.LogBlockSize),
        mp_obj_new_int(cardinfo.CardType),
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

#endif // MICROPY_HW_HAS_SDCARD
