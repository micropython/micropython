#include "py/nlr.h"
#include "py/runtime.h"
#include "py/mphal.h"

#include "lib/fatfs/ff.h"
#include "extmod/fsusermount.h"

#include "sdcard.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "bufhelper.h"

#if MICROPY_HW_HAS_SDCARD

#include <stdio.h>

#include "sdhc.h"

STATIC bool    card_detected = false;

void sdcard_init(void) {
    DSTATUS status = SDHC_InitCard();

    card_detected = (status == 0);

    printf("SDHC_Init returned %d\n", status);
}

bool sdcard_is_present(void) {
    // The card detect on the sdcard connector doesn't connect to anything,
    // so we need to use the card initialization status to detect that a card
    // is present.

    return card_detected;
}

bool sdcard_power_on(void) {
    if (!sdcard_is_present()) {
        return false;
    }
    /// TODO: Implement
    return true;
}

void sdcard_power_off(void) {
    /// TODO: Implement
}

uint64_t sdcard_get_capacity_in_bytes(void) {
    return SDHC_GetBlockCnt() * SDHC_BLOCK_SIZE;
}

mp_uint_t sdcard_read_blocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks) {
    SDHC_DMAWait();	// make sure uSD card is not busy
    DWORD res = SDHC_ReadBlocks(dest, block_num, num_blocks);
    if (res == RES_OK) {
        SDHC_DMAWait();
    }
    return res;
}

mp_uint_t sdcard_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks) {
    SDHC_DMAWait();	// make sure uSD card is not busy
    return SDHC_WriteBlocks((UCHAR *)src, block_num, num_blocks);
}

/******************************************************************************/
// Micro Python bindings
//
// Expose the SD card as an object with the block protocol.

// there is a singleton SDCard object
const mp_obj_base_t pyb_sdcard_obj = {&pyb_sdcard_type};

STATIC mp_obj_t pyb_sdcard_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return singleton object
    return (mp_obj_t)&pyb_sdcard_obj;
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
    if (sdCardDesc.status) {
        return mp_const_none;
    }

    int cardType = 0;   // STD_CAPACITY_SD_CARD_V1_1 from stmhal
    if (sdCardDesc.highCapacity) {
        cardType = 2;   // HIGH_CAPACITY_SD_CARD from stmhal
    } else if (sdCardDesc.version2) {
        cardType = 1;   // STD_CAPACITY_SD_CARD_V2_0 from stmhal
    }

    mp_obj_t tuple[3] = {
        mp_obj_new_int_from_ull(SDHC_GetBlockCnt() * SDHC_BLOCK_SIZE),
        mp_obj_new_int_from_uint(SDHC_BLOCK_SIZE),
        mp_obj_new_int(cardType),
    };
    return mp_obj_new_tuple(3, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sd_info_obj, sd_info);

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
            SDHC_DMAWait();
            return MP_OBJ_NEW_SMALL_INT(0); // success

        case BP_IOCTL_SEC_COUNT:
            return MP_OBJ_NEW_SMALL_INT(0); // TODO

        case BP_IOCTL_SEC_SIZE:
            return MP_OBJ_NEW_SMALL_INT(SDCARD_BLOCK_SIZE);

        default: // unknown command
            return MP_OBJ_NEW_SMALL_INT(-1); // error
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_sdcard_ioctl_obj, pyb_sdcard_ioctl);

STATIC const mp_map_elem_t pyb_sdcard_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_present), (mp_obj_t)&sd_present_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_power), (mp_obj_t)&sd_power_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_info), (mp_obj_t)&sd_info_obj },
    // block device protocol
    { MP_OBJ_NEW_QSTR(MP_QSTR_readblocks), (mp_obj_t)&pyb_sdcard_readblocks_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_writeblocks), (mp_obj_t)&pyb_sdcard_writeblocks_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ioctl), (mp_obj_t)&pyb_sdcard_ioctl_obj },
};

STATIC MP_DEFINE_CONST_DICT(pyb_sdcard_locals_dict, pyb_sdcard_locals_dict_table);

const mp_obj_type_t pyb_sdcard_type = {
    { &mp_type_type },
    .name = MP_QSTR_SD,
    .make_new = pyb_sdcard_make_new,
    .locals_dict = (mp_obj_t)&pyb_sdcard_locals_dict,
};

void sdcard_init_vfs(fs_user_mount_t *vfs) {
    vfs->flags |= FSUSER_NATIVE | FSUSER_HAVE_IOCTL;
    vfs->readblocks[0] = (mp_obj_t)&pyb_sdcard_readblocks_obj;
    vfs->readblocks[1] = (mp_obj_t)&pyb_sdcard_obj;
    vfs->readblocks[2] = (mp_obj_t)sdcard_read_blocks; // native version
    vfs->writeblocks[0] = (mp_obj_t)&pyb_sdcard_writeblocks_obj;
    vfs->writeblocks[1] = (mp_obj_t)&pyb_sdcard_obj;
    vfs->writeblocks[2] = (mp_obj_t)sdcard_write_blocks; // native version
    vfs->u.ioctl[0] = (mp_obj_t)&pyb_sdcard_ioctl_obj;
    vfs->u.ioctl[1] = (mp_obj_t)&pyb_sdcard_obj;
}

#endif // MICROPY_HW_HAS_SDCARD
