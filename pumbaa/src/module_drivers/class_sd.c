/**
 * @file module_drivers/class_sd.c
 *
 * @section License
 * Copyright (C) 2016, Erik Moqvist
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This file is part of the Pumbaa project.
 */

#include "pumbaa.h"

/**
 * CID named tuple fields.
 */
static const qstr cid_fields[] = {
    MP_QSTR_mid,
    MP_QSTR_oid,
    MP_QSTR_pnm,
    MP_QSTR_prv,
    MP_QSTR_psn,
    MP_QSTR_mdt,
    MP_QSTR_crc
};

static const qstr csd_v1_fields[] = {
    MP_QSTR_csd_structure,
    MP_QSTR_taac,
    MP_QSTR_nsac,
    MP_QSTR_tran_speed,
    MP_QSTR_ccc,
    MP_QSTR_read_bl_len,
    MP_QSTR_read_bl_partial,
    MP_QSTR_write_blk_misalign,
    MP_QSTR_read_blk_misalign,
    MP_QSTR_dsr_imp,
    MP_QSTR_c_size,
    MP_QSTR_vdd_r_curr_min,
    MP_QSTR_vdd_r_curr_max,
    MP_QSTR_vdd_w_curr_min,
    MP_QSTR_vdd_w_curr_max,
    MP_QSTR_c_size_mult,
    MP_QSTR_erase_blk_en,
    MP_QSTR_sector_size,
    MP_QSTR_wp_grp_size,
    MP_QSTR_wp_grp_enable,
    MP_QSTR_r2w_factor,
    MP_QSTR_write_bl_len,
    MP_QSTR_write_bl_partial,
    MP_QSTR_file_format_grp,
    MP_QSTR_copy,
    MP_QSTR_perm_write_protect,
    MP_QSTR_tmp_write_protect,
    MP_QSTR_file_format,
    MP_QSTR_crc
};

static const qstr csd_v2_fields[] = {
    MP_QSTR_csd_structure,
    MP_QSTR_taac,
    MP_QSTR_nsac,
    MP_QSTR_tran_speed,
    MP_QSTR_ccc,
    MP_QSTR_read_bl_len,
    MP_QSTR_dsr_imp,
    MP_QSTR_read_blk_misalign,
    MP_QSTR_write_blk_misalign,
    MP_QSTR_read_bl_partial,
    MP_QSTR_c_size,
    MP_QSTR_sector_size,
    MP_QSTR_erase_blk_en,
    MP_QSTR_wp_grp_size,
    MP_QSTR_write_bl_len,
    MP_QSTR_r2w_factor,
    MP_QSTR_wp_grp_enable,
    MP_QSTR_write_partial,
    MP_QSTR_file_format,
    MP_QSTR_tmp_write_protect,
    MP_QSTR_perm_write_protect,
    MP_QSTR_copy,
    MP_QSTR_file_format_grp,
    MP_QSTR_crc
};

/**
 * Print the sd object.
 */
static void class_sd_print(const mp_print_t *print_p,
                            mp_obj_t self_in,
                            mp_print_kind_t kind)
{
    struct class_sd_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    mp_printf(print_p, "<0x%p>", self_p);
}

/**
 * Create a new Sd object.
 */
static mp_obj_t class_sd_make_new(const mp_obj_type_t *type_p,
                                   mp_uint_t n_args,
                                   mp_uint_t n_kw,
                                   const mp_obj_t *args_p)
{
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_spi, MP_ARG_REQUIRED | MP_ARG_OBJ }
    };
    struct class_sd_t *self_p;
    mp_map_t kwargs;
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    struct class_spi_t *spi_p;

    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    /* Parse the arguments. */
    mp_map_init(&kwargs, 0);
    mp_arg_parse_all(n_args,
                     args_p,
                     &kwargs,
                     MP_ARRAY_SIZE(allowed_args),
                     allowed_args,
                     args);

    /* Validate SPI driver argument. */
    spi_p = args[0].u_obj;

    if (spi_p->base.type != &module_drivers_class_spi) {
        nlr_raise(mp_obj_new_exception(&mp_type_TypeError));
    }

    /* Create a new SD object. */
    self_p = m_new_obj(struct class_sd_t);
    self_p->base.type = &module_drivers_class_sd;
    self_p->spi_p = spi_p;

    if (sd_init(&self_p->drv, &spi_p->drv) != 0) {
        nlr_raise(mp_obj_new_exception(&mp_type_OSError));
    }

    return (self_p);
}

/**
 * def start(self)
 */
static mp_obj_t class_sd_start(mp_obj_t self_in)
{
    struct class_sd_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);

    if (sd_start(&self_p->drv) != 0) {
        nlr_raise(mp_obj_new_exception(&mp_type_OSError));
    }

    return (mp_const_none);
}

/**
 * def stop(self, samples)
 */
static mp_obj_t class_sd_stop(mp_obj_t self_in)
{
    struct class_sd_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);

    if (sd_stop(&self_p->drv) != 0) {
        nlr_raise(mp_obj_new_exception(&mp_type_OSError));
    }

    return (mp_const_none);
}

/**
 * def read_cid(self)
 */
static mp_obj_t class_sd_read_cid(mp_obj_t self_in)
{
    struct class_sd_t *self_p;
    struct sd_cid_t cid;
    mp_obj_t tuple[7];

    self_p = MP_OBJ_TO_PTR(self_in);

    if (sd_read_cid(&self_p->drv, &cid) != sizeof(cid)) {
        nlr_raise(mp_obj_new_exception(&mp_type_OSError));
    }

    tuple[0] = MP_OBJ_NEW_SMALL_INT(cid.mid);
    tuple[1] = mp_obj_new_bytes((const byte *)cid.oid, sizeof(cid.oid));
    tuple[2] = mp_obj_new_bytes((const byte *)cid.pnm, sizeof(cid.pnm));
    tuple[3] = MP_OBJ_NEW_SMALL_INT(cid.prv);
    tuple[4] = mp_obj_new_int(cid.psn);
    tuple[5] = mp_obj_new_int(cid.mdt);
    tuple[6] = MP_OBJ_NEW_SMALL_INT(cid.crc);

    return (mp_obj_new_attrtuple(&cid_fields[0], 7, tuple));
}

/**
 * def read_cid(self)
 */
static mp_obj_t class_sd_read_csd(mp_obj_t self_in)
{
    struct class_sd_t *self_p;
    union sd_csd_t csd;
    mp_obj_t tuple[29];

    self_p = MP_OBJ_TO_PTR(self_in);

    if (sd_read_csd(&self_p->drv, &csd) != sizeof(csd)) {
        nlr_raise(mp_obj_new_exception(&mp_type_OSError));
    }

    switch (csd.v1.csd_structure) {

    case SD_CSD_STRUCTURE_V1:
        tuple[0] = MP_OBJ_NEW_SMALL_INT(csd.v1.csd_structure);
        tuple[1] = MP_OBJ_NEW_SMALL_INT(csd.v1.taac);
        tuple[2] = MP_OBJ_NEW_SMALL_INT(csd.v1.nsac);
        tuple[3] = MP_OBJ_NEW_SMALL_INT(csd.v1.tran_speed);
        tuple[4] = MP_OBJ_NEW_SMALL_INT(SD_CCC(&csd.v1));
        tuple[5] = MP_OBJ_NEW_SMALL_INT(csd.v1.read_bl_len);
        tuple[6] = MP_OBJ_NEW_SMALL_INT(csd.v1.read_bl_partial);
        tuple[7] = MP_OBJ_NEW_SMALL_INT(csd.v1.write_blk_misalign);
        tuple[8] = MP_OBJ_NEW_SMALL_INT(csd.v1.read_blk_misalign);
        tuple[9] = MP_OBJ_NEW_SMALL_INT(csd.v1.dsr_imp);
        tuple[10] = MP_OBJ_NEW_SMALL_INT(SD_C_SIZE(&csd.v1));
        tuple[11] = MP_OBJ_NEW_SMALL_INT(csd.v1.vdd_r_curr_min);
        tuple[12] = MP_OBJ_NEW_SMALL_INT(csd.v1.vdd_r_curr_max);
        tuple[13] = MP_OBJ_NEW_SMALL_INT(csd.v1.vdd_w_curr_min);
        tuple[14] = MP_OBJ_NEW_SMALL_INT(csd.v1.vdd_w_curr_max);
        tuple[15] = MP_OBJ_NEW_SMALL_INT(SD_C_SIZE_MULT(&csd.v1));
        tuple[16] = MP_OBJ_NEW_SMALL_INT(csd.v1.erase_blk_en);
        tuple[17] = MP_OBJ_NEW_SMALL_INT(SD_SECTOR_SIZE(&csd.v1));
        tuple[18] = MP_OBJ_NEW_SMALL_INT(csd.v1.wp_grp_size);
        tuple[19] = MP_OBJ_NEW_SMALL_INT(csd.v1.wp_grp_enable);
        tuple[20] = MP_OBJ_NEW_SMALL_INT(csd.v1.r2w_factor);
        tuple[21] = MP_OBJ_NEW_SMALL_INT(SD_WRITE_BL_LEN(&csd.v1));
        tuple[22] = MP_OBJ_NEW_SMALL_INT(csd.v1.write_bl_partial);
        tuple[23] = MP_OBJ_NEW_SMALL_INT(csd.v1.file_format_grp);
        tuple[24] = MP_OBJ_NEW_SMALL_INT(csd.v1.copy);
        tuple[25] = MP_OBJ_NEW_SMALL_INT(csd.v1.perm_write_protect);
        tuple[26] = MP_OBJ_NEW_SMALL_INT(csd.v1.tmp_write_protect);
        tuple[27] = MP_OBJ_NEW_SMALL_INT(csd.v1.file_format);
        tuple[28] = MP_OBJ_NEW_SMALL_INT(csd.v1.crc);

        return (mp_obj_new_attrtuple(&csd_v1_fields[0], 29, tuple));

    case SD_CSD_STRUCTURE_V2:
        tuple[0] = MP_OBJ_NEW_SMALL_INT(csd.v2.csd_structure);
        tuple[1] = MP_OBJ_NEW_SMALL_INT(csd.v2.taac);
        tuple[2] = MP_OBJ_NEW_SMALL_INT(csd.v2.nsac);
        tuple[3] = MP_OBJ_NEW_SMALL_INT(csd.v2.tran_speed);
        tuple[4] = MP_OBJ_NEW_SMALL_INT(SD_CCC(&csd.v2));
        tuple[5] = MP_OBJ_NEW_SMALL_INT(csd.v2.read_bl_len);
        tuple[6] = MP_OBJ_NEW_SMALL_INT(csd.v2.dsr_imp);
        tuple[7] = MP_OBJ_NEW_SMALL_INT(csd.v2.read_blk_misalign);
        tuple[8] = MP_OBJ_NEW_SMALL_INT(csd.v2.write_blk_misalign);
        tuple[9] = MP_OBJ_NEW_SMALL_INT(csd.v2.read_bl_partial);
        tuple[10] = MP_OBJ_NEW_SMALL_INT(SD_C_SIZE(&csd.v2));
        tuple[11] = MP_OBJ_NEW_SMALL_INT(SD_SECTOR_SIZE(&csd.v2));
        tuple[12] = MP_OBJ_NEW_SMALL_INT(csd.v2.erase_blk_en);
        tuple[13] = MP_OBJ_NEW_SMALL_INT(csd.v2.wp_grp_size);
        tuple[14] = MP_OBJ_NEW_SMALL_INT(SD_WRITE_BL_LEN(&csd.v2));
        tuple[15] = MP_OBJ_NEW_SMALL_INT(csd.v2.r2w_factor);
        tuple[16] = MP_OBJ_NEW_SMALL_INT(csd.v2.wp_grp_enable);
        tuple[17] = MP_OBJ_NEW_SMALL_INT(csd.v2.write_bl_partial);
        tuple[18] = MP_OBJ_NEW_SMALL_INT(csd.v2.file_format);
        tuple[19] = MP_OBJ_NEW_SMALL_INT(csd.v2.tmp_write_protect);
        tuple[20] = MP_OBJ_NEW_SMALL_INT(csd.v2.perm_write_protect);
        tuple[21] = MP_OBJ_NEW_SMALL_INT(csd.v2.copy);
        tuple[22] = MP_OBJ_NEW_SMALL_INT(csd.v2.file_format_grp);
        tuple[23] = MP_OBJ_NEW_SMALL_INT(csd.v2.crc);

        return (mp_obj_new_attrtuple(&csd_v2_fields[0], 24, tuple));

    default:
        return (mp_const_none);
    }
}

/**
 * def read_block(self, block)
 */
static mp_obj_t class_sd_read_block(mp_obj_t self_in,
                                    mp_obj_t block_in)
{
    struct class_sd_t *self_p;
    vstr_t vstr;
    uint32_t block;

    self_p = MP_OBJ_TO_PTR(self_in);
    block = mp_obj_get_int(block_in);

    vstr_init_len(&vstr, SD_BLOCK_SIZE);

    if (sd_read_block(&self_p->drv, vstr.buf, block) != SD_BLOCK_SIZE) {
        nlr_raise(mp_obj_new_exception(&mp_type_OSError));
    }

    return (mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr));
}

/**
 * def read_into(self, block, buffer)
 */
static mp_obj_t class_sd_read_block_into(mp_obj_t self_in,
                                         mp_obj_t block_in,
                                         mp_obj_t buffer_in)
{
    struct class_sd_t *self_p;
    uint32_t block;
    mp_buffer_info_t buffer_info;

    self_p = MP_OBJ_TO_PTR(self_in);
    block = mp_obj_get_int(block_in);
    mp_get_buffer_raise(MP_OBJ_TO_PTR(buffer_in),
                        &buffer_info,
                        MP_BUFFER_WRITE);

    if (buffer_info.len != SD_BLOCK_SIZE) {
        nlr_raise(mp_obj_new_exception(&mp_type_OSError));
    }

    if (sd_read_block(&self_p->drv, buffer_info.buf, block) != SD_BLOCK_SIZE) {
        nlr_raise(mp_obj_new_exception(&mp_type_OSError));
    }

    return (mp_const_none);
}

/**
 * def write(self, block, buffer)
 */
static mp_obj_t class_sd_write_block(mp_obj_t self_in,
                                     mp_obj_t block_in,
                                     mp_obj_t buffer_in)
{
    struct class_sd_t *self_p;
    uint32_t block;
    mp_buffer_info_t buffer_info;

    self_p = MP_OBJ_TO_PTR(self_in);
    block = mp_obj_get_int(block_in);
    mp_get_buffer_raise(MP_OBJ_TO_PTR(buffer_in),
                        &buffer_info,
                        MP_BUFFER_READ);

    if (buffer_info.len != SD_BLOCK_SIZE) {
        nlr_raise(mp_obj_new_exception(&mp_type_OSError));
    }

    if (sd_write_block(&self_p->drv, block, buffer_info.buf) != SD_BLOCK_SIZE) {
        nlr_raise(mp_obj_new_exception(&mp_type_OSError));
    }

    return (mp_const_none);
}

static MP_DEFINE_CONST_FUN_OBJ_1(class_sd_start_obj, class_sd_start);
static MP_DEFINE_CONST_FUN_OBJ_1(class_sd_stop_obj, class_sd_stop);
static MP_DEFINE_CONST_FUN_OBJ_1(class_sd_read_cid_obj, class_sd_read_cid);
static MP_DEFINE_CONST_FUN_OBJ_1(class_sd_read_csd_obj, class_sd_read_csd);
static MP_DEFINE_CONST_FUN_OBJ_2(class_sd_read_block_obj, class_sd_read_block);
static MP_DEFINE_CONST_FUN_OBJ_3(class_sd_read_block_into_obj, class_sd_read_block_into);
static MP_DEFINE_CONST_FUN_OBJ_3(class_sd_write_block_obj, class_sd_write_block);

static const mp_rom_map_elem_t class_sd_locals_dict_table[] = {
    /* Instance methods. */
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&class_sd_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&class_sd_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_cid), MP_ROM_PTR(&class_sd_read_cid_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_csd), MP_ROM_PTR(&class_sd_read_csd_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_block), MP_ROM_PTR(&class_sd_read_block_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_block_into), MP_ROM_PTR(&class_sd_read_block_into_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_block), MP_ROM_PTR(&class_sd_write_block_obj) }
};

static MP_DEFINE_CONST_DICT(class_sd_locals_dict, class_sd_locals_dict_table);

/**
 * Sd class type.
 */
const mp_obj_type_t module_drivers_class_sd = {
    { &mp_type_type },
    .name = MP_QSTR_Sd,
    .print = class_sd_print,
    .make_new = class_sd_make_new,
    .locals_dict = (mp_obj_t)&class_sd_locals_dict,
};
