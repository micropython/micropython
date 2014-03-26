// TODO
// make it work with DMA

#include <stdio.h>
//#include "stm32f4xx_sdio.h"
#include "stm324x7i_eval_sdio_sd.h"
#include "misc.h"
#include "systick.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "map.h"
#include "runtime.h"
#include "sdcard.h"

#if 0
#define BLOCK_SIZE            512 /* Block Size in Bytes */

uint8_t aBuffer_Block_Rx[BLOCK_SIZE];

void sdio_init(void) {
    SD_Error error = SD_Init();
    printf("Init: %x\n", error);
    uint8_t det = SD_Detect();
    printf("Detc: %x\n", det);

    if (!det) {
        printf("no card detected\n");
        SD_PowerOFF();
        SD_DeInit();
        return;
    }

    // read a block!
    error = SD_ReadBlock(aBuffer_Block_Rx, 512, BLOCK_SIZE);
    printf("ReadBlock: %d\n", error);

    /*
    // Check if the Transfer is finished
    error = SD_WaitReadOperation();
    printf("WaitReadOp: %d\n", error);
    */

    uint32_t stc = sys_tick_counter;
    while (SD_GetStatus() != SD_TRANSFER_OK) {
        if (sys_tick_has_passed(stc, 2000)) {
            printf("timeout waiting for read to finish\n");
            break;
        }
    }
    printf("done!!\n");

    printf("%.16s", aBuffer_Block_Rx);

    /*
    snprintf((char*)aBuffer_Block_Rx, BLOCK_SIZE, "Here is some data back for you!\nBLOCK_SIZE=%d\n", BLOCK_SIZE);
    error = SD_WriteBlock(aBuffer_Block_Rx, 512, BLOCK_SIZE);
    printf("WriteBlock: %d\n", error);

    while (SD_GetStatus() != SD_TRANSFER_OK) {
    }
    printf("done writing!\n");
    */

    SD_PowerOFF();
    SD_DeInit();
}
#endif

void sdcard_init(void) {
    // init the SD card detect pin
    SD_LowLevel_Init_Detect();
}

bool sdcard_is_present(void) {
    return SD_Detect() != 0;
}

bool sdcard_power_on(void) {
    if (!SD_Detect()) {
        return false;
    }

    SD_Error status = SD_Init();
    if (status != SD_OK) {
        SD_PowerOFF();
        SD_DeInit();
        return false;
    }

    return true;
}

void sdcard_power_off(void) {
    SD_PowerOFF();
    SD_DeInit();
}

uint64_t sdcard_get_capacity_in_bytes(void) {
    SD_CardInfo SDCardInfo;
    SD_GetCardInfo(&SDCardInfo);
    return SDCardInfo.CardCapacity;
}

bool sdcard_read_block(uint8_t *dest, uint32_t block_num) {
    // TODO return error if not powered on

    SD_Error status;

    status = SD_ReadBlock(dest, block_num * SDCARD_BLOCK_SIZE, SDCARD_BLOCK_SIZE);
    if (status != SD_OK) {
        return false;
    }

#ifdef SD_DMA_MODE
    // wait for DMA transfer to finish
    status = SD_WaitReadOperation();
    if (status != SD_OK) {
        return false;
    }
#endif

    // wait for SD controller to finish
    uint32_t stc = sys_tick_counter;
    while (SD_GetStatus() != SD_TRANSFER_OK) {
        if (sys_tick_has_passed(stc, 5000)) {
            //printf("[ERROR] timeout waiting for SD card read to finish\n");
            return false;
        }
    }

    return true;
}

bool sdcard_write_block(const uint8_t *src, uint32_t block_num) {
    // TODO return error if not powered on

    SD_Error status;

    status = SD_WriteBlock((uint8_t*)src, block_num * SDCARD_BLOCK_SIZE, SDCARD_BLOCK_SIZE);
    if (status != SD_OK) {
        return false;
    }

#ifdef SD_DMA_MODE
    // wait for DMA transfer to finish
    status = SD_WaitReadOperation();
    if (status != SD_OK) {
        return false;
    }
#endif

    // wait for SD controller to finish
    uint32_t stc = sys_tick_counter;
    while (SD_GetStatus() != SD_TRANSFER_OK) {
        if (sys_tick_has_passed(stc, 5000)) {
            //printf("[ERROR] timeout waiting for SD card write to finish\n");
            return false;
        }
    }

    return true;
}

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
    if (!sdcard_read_block(dest, mp_obj_get_int(block_num))) {
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
