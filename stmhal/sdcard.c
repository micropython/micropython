// TODO make it work with DMA

#include <stm32f4xx_hal.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "map.h"
#include "runtime.h"
#include "sdcard.h"

static SD_HandleTypeDef sd_handle;

#define SDCARD_DETECT_GPIO_PORT (GPIOA)
#define SDCARD_DETECT_GPIO_PIN (GPIO_PIN_8)
#define SDCARD_DETECT_GPIO_PULL (GPIO_PULLUP)
#define SDCARD_DETECT_GPIO_PRESENT (GPIO_PIN_RESET)
#define __SDCARD_DETECT_GPIO_CLK_ENABLE() __GPIOA_CLK_ENABLE()

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
    GPIO_Init_Structure.Pull = SDCARD_DETECT_GPIO_PULL;
    GPIO_Init_Structure.Speed = GPIO_SPEED_HIGH;
    GPIO_Init_Structure.Pin = SDCARD_DETECT_GPIO_PIN;
    HAL_GPIO_Init(SDCARD_DETECT_GPIO_PORT, &GPIO_Init_Structure);
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
    return HAL_GPIO_ReadPin(SDCARD_DETECT_GPIO_PORT, SDCARD_DETECT_GPIO_PIN) == SDCARD_DETECT_GPIO_PRESENT;
}

bool sdcard_power_on(void) {
    if (!sdcard_is_present()) {
        return false;
    }

    // SD device interface configuration
    sd_handle.Instance = SDIO;
    sd_handle.Init.ClockEdge           = SDIO_CLOCK_EDGE_RISING;
    sd_handle.Init.ClockBypass         = SDIO_CLOCK_BYPASS_DISABLE;
    sd_handle.Init.ClockPowerSave      = SDIO_CLOCK_POWER_SAVE_DISABLE;
    sd_handle.Init.BusWide             = SDIO_BUS_WIDE_1B;
    sd_handle.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    sd_handle.Init.ClockDiv            = SDIO_TRANSFER_CLK_DIV;

    // init the SD interface
    HAL_SD_CardInfoTypedef cardinfo;
    if (HAL_SD_Init(&sd_handle, &cardinfo) != SD_OK) {
        goto error;
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

bool sdcard_read_blocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks) {
    // check that dest pointer is aligned on a 4-byte boundary
    if (((uint32_t)dest & 3) != 0) {
        return false;
    }

    // check that SD card is initialised
    if (sd_handle.Instance == NULL) {
        return false;
    }

    if (HAL_SD_ReadBlocks(&sd_handle, (uint32_t*)dest, block_num * SDCARD_BLOCK_SIZE, SDCARD_BLOCK_SIZE, num_blocks) != SD_OK) {
        return false;
    }

    return true;
}

bool sdcard_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks) {
    // check that src pointer is aligned on a 4-byte boundary
    if (((uint32_t)src & 3) != 0) {
        return false;
    }

    // check that SD card is initialised
    if (sd_handle.Instance == NULL) {
        return false;
    }

    if (HAL_SD_WriteBlocks(&sd_handle, (uint32_t*)src, block_num * SDCARD_BLOCK_SIZE, SDCARD_BLOCK_SIZE, num_blocks) != SD_OK) {
        return false;
    }

    return true;
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
    if (HAL_SD_ReadBlocks_DMA(&sd_handle, (uint32_t*)dest, block_num * SDCARD_BLOCK_SIZE, SDCARD_BLOCK_SIZE) != SD_OK) {
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

    status = HAL_SD_WriteBlock_DMA(&sd_handle, (uint32_t*)src, block_num * SDCARD_BLOCK_SIZE, SDCARD_BLOCK_SIZE, num_blocks);
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

static mp_obj_t sd_present(mp_obj_t self) {
    return MP_BOOL(sdcard_is_present());
}

static MP_DEFINE_CONST_FUN_OBJ_1(sd_present_obj, sd_present);

static mp_obj_t sd_power(mp_obj_t self, mp_obj_t state) {
    bool result;
    if (rt_is_true(state)) {
        result = sdcard_power_on();
    } else {
        sdcard_power_off();
        result = true;
    }
    return MP_BOOL(result);
}

static MP_DEFINE_CONST_FUN_OBJ_2(sd_power_obj, sd_power);

static mp_obj_t sd_read(mp_obj_t self, mp_obj_t block_num) {
    uint8_t *dest = m_new(uint8_t, SDCARD_BLOCK_SIZE);
    if (!sdcard_read_blocks(dest, mp_obj_get_int(block_num), 1)) {
        m_free(dest, SDCARD_BLOCK_SIZE);
        return mp_const_none;
    }
    return mp_obj_new_bytearray_by_ref(SDCARD_BLOCK_SIZE, dest);
}

static MP_DEFINE_CONST_FUN_OBJ_2(sd_read_obj, sd_read);

STATIC const mp_map_elem_t sdcard_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_present), (mp_obj_t)&sd_present_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_power), (mp_obj_t)&sd_power_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&sd_read_obj },
};

STATIC MP_DEFINE_CONST_DICT(sdcard_locals_dict, sdcard_locals_dict_table);

static const mp_obj_type_t sdcard_type = {
    { &mp_type_type },
    .name = MP_QSTR_SDcard,
    .locals_dict = (mp_obj_t)&sdcard_locals_dict,
};

const mp_obj_base_t pyb_sdcard_obj = {&sdcard_type};
