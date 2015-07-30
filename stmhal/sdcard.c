/*
 * This file is part of the Micro Python project, http://micropython.org/
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

// TODO make it work with DMA

#include STM32_HAL_H

#include "py/nlr.h"
#include "py/runtime.h"
#include "sdcard.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "bufhelper.h"

#if MICROPY_HW_HAS_SDCARD

static SD_HandleTypeDef sd_handle;

void sdcard_init(void) {
    GPIO_InitTypeDef GPIO_Init_Structure;

    // invalidate the sd_handle
    sd_handle.Instance = NULL;

    // configure SD GPIO
    // we do this here an not in HAL_SD_MspInit because it apparently
    // makes it more robust to have the pins always pulled high
    GPIO_Init_Structure.Mode = GPIO_MODE_AF_PP;
    GPIO_Init_Structure.Pull = GPIO_PULLUP;
    GPIO_Init_Structure.Speed = GPIO_SPEED_HIGH;
    GPIO_Init_Structure.Alternate = GPIO_AF12_SDIO;
    GPIO_Init_Structure.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
    HAL_GPIO_Init(GPIOC, &GPIO_Init_Structure);
    GPIO_Init_Structure.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOD, &GPIO_Init_Structure);

    // configure the SD card detect pin
    // we do this here so we can detect if the SD card is inserted before powering it on
    GPIO_Init_Structure.Mode = GPIO_MODE_INPUT;
    GPIO_Init_Structure.Pull = MICROPY_HW_SDCARD_DETECT_PULL;
    GPIO_Init_Structure.Speed = GPIO_SPEED_HIGH;
    GPIO_Init_Structure.Pin = MICROPY_HW_SDCARD_DETECT_PIN.pin_mask;
    HAL_GPIO_Init(MICROPY_HW_SDCARD_DETECT_PIN.gpio, &GPIO_Init_Structure);
}

void HAL_SD_MspInit(SD_HandleTypeDef *hsd) {
    // enable SDIO clock
    __SDIO_CLK_ENABLE();

    // GPIO have already been initialised by sdcard_init

    // interrupts are not used at the moment
    // they are needed only for DMA transfer (I think...)
}

void HAL_SD_MspDeInit(SD_HandleTypeDef *hsd) {
    __SDIO_CLK_DISABLE();
}

bool sdcard_is_present(void) {
    return HAL_GPIO_ReadPin(MICROPY_HW_SDCARD_DETECT_PIN.gpio, MICROPY_HW_SDCARD_DETECT_PIN.pin_mask) == MICROPY_HW_SDCARD_DETECT_PRESENT;
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
    sd_handle.Init.ClockBypass         = SDIO_CLOCK_BYPASS_DISABLE;
    sd_handle.Init.ClockPowerSave      = SDIO_CLOCK_POWER_SAVE_DISABLE;
    sd_handle.Init.BusWide             = SDIO_BUS_WIDE_1B;
    sd_handle.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    sd_handle.Init.ClockDiv            = SDIO_TRANSFER_CLK_DIV;

    // init the SD interface, with retry if it's not ready yet
    HAL_SD_CardInfoTypedef cardinfo;
    for (int retry = 10; HAL_SD_Init(&sd_handle, &cardinfo) != SD_OK; retry--) {
        if (retry == 0) {
            goto error;
        }
        HAL_Delay(50);
    }

    // configure the SD bus width for wide operation
    if (HAL_SD_WideBusOperation_Config(&sd_handle, SDIO_BUS_WIDE_4B) != SD_OK) {
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
    HAL_SD_CardInfoTypedef cardinfo;
    HAL_SD_Get_CardInfo(&sd_handle, &cardinfo);
    return cardinfo.CardCapacity;
}

mp_uint_t sdcard_read_blocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks) {
    // check that dest pointer is aligned on a 4-byte boundary
    if (((uint32_t)dest & 3) != 0) {
        return SD_ERROR;
    }

    // check that SD card is initialised
    if (sd_handle.Instance == NULL) {
        return SD_ERROR;
    }

    // We must disable IRQs because the SDIO peripheral has a small FIFO
    // buffer and we can't let it fill up in the middle of a read.
    // This will not be needed when SD uses DMA for transfer.
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    HAL_SD_ErrorTypedef err = HAL_SD_ReadBlocks_BlockNumber(&sd_handle, (uint32_t*)dest, block_num, SDCARD_BLOCK_SIZE, num_blocks);
    MICROPY_END_ATOMIC_SECTION(atomic_state);

    return err;
}

mp_uint_t sdcard_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks) {
    // check that src pointer is aligned on a 4-byte boundary
    if (((uint32_t)src & 3) != 0) {
        return SD_ERROR;
    }

    // check that SD card is initialised
    if (sd_handle.Instance == NULL) {
        return SD_ERROR;
    }

    // We must disable IRQs because the SDIO peripheral has a small FIFO
    // buffer and we can't let it drain to empty in the middle of a write.
    // This will not be needed when SD uses DMA for transfer.
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    HAL_SD_ErrorTypedef err = HAL_SD_WriteBlocks_BlockNumber(&sd_handle, (uint32_t*)src, block_num, SDCARD_BLOCK_SIZE, num_blocks);
    MICROPY_END_ATOMIC_SECTION(atomic_state);

    return err;
}

#if 0
DMA not implemented
bool sdcard_read_blocks_dma(uint8_t *dest, uint32_t block_num, uint32_t num_blocks) {
    // check that dest pointer is aligned on a 4-byte boundary
    if (((uint32_t)dest & 3) != 0) {
        return false;
    }

    // check that SD card is initialised
    if (sd_handle.Instance == NULL) {
        return false;
    }

    // do the read
    if (HAL_SD_ReadBlocks_BlockNumber_DMA(&sd_handle, (uint32_t*)dest, block_num, SDCARD_BLOCK_SIZE) != SD_OK) {
        return false;
    }

    // wait for DMA transfer to finish, with a large timeout
    if (HAL_SD_CheckReadOperation(&sd_handle, 100000000) != SD_OK) {
        return false;
    }

    return true;
}

bool sdcard_write_blocks_dma(const uint8_t *src, uint32_t block_num, uint32_t num_blocks) {
    // check that src pointer is aligned on a 4-byte boundary
    if (((uint32_t)src & 3) != 0) {
        return false;
    }

    // check that SD card is initialised
    if (sd_handle.Instance == NULL) {
        return false;
    }

    SD_Error status;

    status = HAL_SD_WriteBlocks_BlockNumber_DMA(&sd_handle, (uint32_t*)src, block_num, SDCARD_BLOCK_SIZE, num_blocks);
    if (status != SD_OK) {
        return false;
    }

    // wait for DMA transfer to finish, with a large timeout
    status = HAL_SD_CheckWriteOperation(&sd_handle, 100000000);
    if (status != SD_OK) {
        return false;
    }

    return true;
}
#endif

/******************************************************************************/
// Micro Python bindings
//
// Note: these function are a bit ad-hoc at the moment and are mainly intended
// for testing purposes.  In the future SD should be a proper class with a
// consistent interface and methods to mount/unmount it.

STATIC mp_obj_t sd_present(mp_obj_t self) {
    return MP_BOOL(sdcard_is_present());
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
    return MP_BOOL(result);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(sd_power_obj, sd_power);

STATIC mp_obj_t sd_info(mp_obj_t self) {
    if (sd_handle.Instance == NULL) {
        return mp_const_none;
    }
    HAL_SD_CardInfoTypedef cardinfo;
    HAL_SD_Get_CardInfo(&sd_handle, &cardinfo);
    // cardinfo.SD_csd and cardinfo.SD_cid have lots of info but we don't use them
    mp_obj_t tuple[3] = {
        mp_obj_new_int_from_ull(cardinfo.CardCapacity),
        mp_obj_new_int_from_uint(cardinfo.CardBlockSize),
        mp_obj_new_int(cardinfo.CardType),
    };
    return mp_obj_new_tuple(3, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sd_info_obj, sd_info);

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

STATIC const mp_map_elem_t sdcard_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_present), (mp_obj_t)&sd_present_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_power), (mp_obj_t)&sd_power_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_info), (mp_obj_t)&sd_info_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&sd_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&sd_write_obj },
};

STATIC MP_DEFINE_CONST_DICT(sdcard_locals_dict, sdcard_locals_dict_table);

static const mp_obj_type_t sdcard_type = {
    { &mp_type_type },
    .name = MP_QSTR_SDcard,
    .locals_dict = (mp_obj_t)&sdcard_locals_dict,
};

const mp_obj_base_t pyb_sdcard_obj = {&sdcard_type};

#endif // MICROPY_HW_HAS_SDCARD
