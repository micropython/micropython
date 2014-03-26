#include <stdio.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "map.h"
#include "file.h"
#include "ff.h"

typedef struct _pyb_file_obj_t {
    mp_obj_base_t base;
    FIL fp;
} pyb_file_obj_t;

void file_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    printf("<file %p>", self_in);
}

mp_obj_t file_obj_read(mp_obj_t self_in, mp_obj_t arg) {
    pyb_file_obj_t *self = self_in;
    int n = mp_obj_get_int(arg);
    byte *buf = m_new(byte, n);
    UINT n_out;
    f_read(&self->fp, buf, n, &n_out);
    return mp_obj_new_str(buf, n_out, false);
}

mp_obj_t file_obj_write(mp_obj_t self_in, mp_obj_t arg) {
    pyb_file_obj_t *self = self_in;
    uint l;
    const char *s = mp_obj_str_get_data(arg, &l);
    UINT n_out;
    FRESULT res = f_write(&self->fp, s, l, &n_out);
    if (res != FR_OK) {
        printf("File error: could not write to file; error code %d\n", res);
    } else if (n_out != l) {
        printf("File error: could not write all data to file; wrote %d / %d bytes\n", n_out, l);
    }
    return mp_const_none;
}

mp_obj_t file_obj_close(mp_obj_t self_in) {
    pyb_file_obj_t *self = self_in;
    f_close(&self->fp);
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_2(file_obj_read_obj, file_obj_read);
static MP_DEFINE_CONST_FUN_OBJ_2(file_obj_write_obj, file_obj_write);
static MP_DEFINE_CONST_FUN_OBJ_1(file_obj_close_obj, file_obj_close);

// TODO gc hook to close the file if not already closed

STATIC const mp_map_elem_t file_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&file_obj_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&file_obj_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_close), (mp_obj_t)&file_obj_close_obj },
};

STATIC MP_DEFINE_CONST_DICT(file_locals_dict, file_locals_dict_table);

static const mp_obj_type_t file_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_File,
    .print = file_obj_print,
    .locals_dict = (mp_obj_t)&file_locals_dict,
};

STATIC mp_obj_t pyb_io_open(mp_obj_t o_filename, mp_obj_t o_mode) {
    const char *filename = mp_obj_str_get_str(o_filename);
    const char *mode = mp_obj_str_get_str(o_mode);
    pyb_file_obj_t *self = m_new_obj(pyb_file_obj_t);
    self->base.type = &file_obj_type;
    if (mode[0] == 'r') {
        // open for reading
        FRESULT res = f_open(&self->fp, filename, FA_READ);
        if (res != FR_OK) {
            printf("FileNotFoundError: [Errno 2] No such file or directory: '%s'\n", filename);
            return mp_const_none;
        }
    } else if (mode[0] == 'w') {
        // open for writing, truncate the file first
        FRESULT res = f_open(&self->fp, filename, FA_WRITE | FA_CREATE_ALWAYS);
        if (res != FR_OK) {
            printf("?FileError: could not create file: '%s'\n", filename);
            return mp_const_none;
        }
    } else {
        printf("ValueError: invalid mode: '%s'\n", mode);
        return mp_const_none;
    }
    return self;
}

MP_DEFINE_CONST_FUN_OBJ_2(mp_builtin_open_obj, pyb_io_open);
