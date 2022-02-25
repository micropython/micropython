#include "../../../py/mpconfig.h"
#include "../../../py/objint.h"
#include "../../../py/objstr.h"
#include "../../../py/emitglue.h"
#include "../../../py/nativeglue.h"

#if MICROPY_LONGINT_IMPL != 0
#error "incompatible MICROPY_LONGINT_IMPL"
#endif

#if MICROPY_PY_BUILTINS_FLOAT
typedef struct _mp_obj_float_t {
    mp_obj_base_t base;
    mp_float_t value;
} mp_obj_float_t;
#endif

#if MICROPY_PY_BUILTINS_COMPLEX
typedef struct _mp_obj_complex_t {
    mp_obj_base_t base;
    mp_float_t real;
    mp_float_t imag;
} mp_obj_complex_t;
#endif

enum {
    MP_QSTR_frozentest_dot_py = MP_QSTRnumber_of,
    MP_QSTR_uPy,
    MP_QSTR_i,
};

extern const qstr_pool_t mp_qstr_const_pool;
const qstr_pool_t mp_qstr_frozen_const_pool = {
    (qstr_pool_t*)&mp_qstr_const_pool, // previous pool
    MP_QSTRnumber_of, // previous pool size
    3, // allocated entries
    3, // used entries
    {
        (const byte*)"\xfe\x0d" "frozentest.py",
        (const byte*)"\xf9\x03" "uPy",
        (const byte*)"\xcc\x01" "i",
    },
};

// frozen bytecode for file frozentest.py, scope frozentest_<module>
STATIC const byte fun_data_frozentest__lt_module_gt_[85] = {
    0x10, 0x16,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_frozentest_dot_py & 0xff, MP_QSTR_frozentest_dot_py >> 8,
    0x29, 0x28, 0x28, 0x28, 0x2b, 0x28, 0x00,
    0x11, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x10, MP_QSTR_uPy & 0xff, MP_QSTR_uPy >> 8, 
    0x34, 0x01, 
    0x59, 
    0x11, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x00, 
    0x34, 0x01, 
    0x59, 
    0x11, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x01, 
    0x34, 0x01, 
    0x59, 
    0x11, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x02, 
    0x34, 0x01, 
    0x59, 
    0x11, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x22, 0xba, 0xef, 0x9a, 0x15, 
    0x34, 0x01, 
    0x59, 
    0x80, 
    0x42, 0x0f, 0x80, 
    0x57, 
    0x16, MP_QSTR_i & 0xff, MP_QSTR_i >> 8, 
    0x11, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x11, MP_QSTR_i & 0xff, MP_QSTR_i >> 8, 
    0x34, 0x01, 
    0x59, 
    0x81, 
    0xe5, 
    0x57, 
    0x84, 
    0xd7, 
    0x43, 0xeb, 0x7f, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_frozentest__lt_module_gt__0 = {{&mp_type_str}, 246, 34, (const byte*)"\x61\x20\x6c\x6f\x6e\x67\x20\x73\x74\x72\x69\x6e\x67\x20\x74\x68\x61\x74\x20\x69\x73\x20\x6e\x6f\x74\x20\x69\x6e\x74\x65\x72\x6e\x65\x64"};
STATIC const mp_obj_str_t const_obj_frozentest__lt_module_gt__1 = {{&mp_type_str}, 200, 38, (const byte*)"\x61\x20\x73\x74\x72\x69\x6e\x67\x20\x74\x68\x61\x74\x20\x68\x61\x73\x20\x75\x6e\x69\x63\x6f\x64\x65\x20\xce\xb1\xce\xb2\xce\xb3\x20\x63\x68\x61\x72\x73"};
STATIC const mp_obj_str_t const_obj_frozentest__lt_module_gt__2 = {{&mp_type_bytes}, 57, 11, (const byte*)"\x62\x79\x74\x65\x73\x20\x31\x32\x33\x34\x01"};
STATIC const mp_rom_obj_t const_table_data_frozentest__lt_module_gt_[3] = {
    MP_ROM_PTR(&const_obj_frozentest__lt_module_gt__0),
    MP_ROM_PTR(&const_obj_frozentest__lt_module_gt__1),
    MP_ROM_PTR(&const_obj_frozentest__lt_module_gt__2),
};
const mp_raw_code_t raw_code_frozentest__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_frozentest__lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_frozentest__lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 85,
    .n_obj = 3,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_frozentest_dot_py,
        .line_info = fun_data_frozentest__lt_module_gt_ + 6,
        .opcodes = fun_data_frozentest__lt_module_gt_ + 13,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

const char mp_frozen_names[] = {
#ifdef MP_FROZEN_STR_NAMES
MP_FROZEN_STR_NAMES
#endif
"frozentest.py\0"
"\0"};
const mp_raw_code_t *const mp_frozen_mpy_content[] = {
    &raw_code_frozentest__lt_module_gt_,
};
#ifdef MICROPY_FROZEN_LIST_ITEM
MICROPY_FROZEN_LIST_ITEM("frozentest", "frozentest.py")
#endif
