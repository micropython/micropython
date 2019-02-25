#include <stdint.h>
#include <stddef.h>

#define HEADER_SIZE (48)
#define CODE_SIZE (256 - HEADER_SIZE)

#define UINT_HI(x) (0x80 | ((x) >> 7))
#define UINT_LO(x) ((x) & 0x7f)

#define MP_BC_LOAD_CONST_NONE    (0x11)
#define MP_BC_STORE_GLOBAL       (0x25) // qstr
#define MP_BC_RETURN_VALUE       (0x5b)
#define MP_BC_MAKE_FUNCTION      (0x60) // uint

#define MP_OBJ_NEW_SMALL_INT(x) ((x) << 1 | 1)

typedef enum {
    MP_CODE_BYTECODE = 2,
    MP_CODE_NATIVE_PY,
    MP_CODE_NATIVE_VIPER,
    MP_CODE_NATIVE_ASM,
} mp_raw_code_kind_t;

struct _mp_f_table_t {
    uintptr_t none_obj;
    uintptr_t false_obj;
    uintptr_t true_obj;
    uintptr_t pad0[2];
    uintptr_t (*swap_globals)(uintptr_t new_globals);
    uintptr_t (*load_name)(uint16_t qst);
    uintptr_t pad1[13];
    uintptr_t (*new_list)(size_t n, uintptr_t *items);
    uintptr_t pad2[6];
    uintptr_t (*call_function_n_kw)(uintptr_t fun_in, size_t n_args_kw, const uintptr_t *args);
};

typedef struct _mp_obj_fun_bc_t {
    uintptr_t base;
    uintptr_t globals;
    const void *fun_data;
    const void **const_table;
} mp_obj_fun_bc_t;

const uint8_t header[HEADER_SIZE] __attribute__((section(".header"))) = {
    'M', 4, 3 | 2 << 2, 31, 32, // x64
    //'M', 4, 2 | 5 << 2, 31, 32, // thumb
    //'M', 4, 2 | 9 << 2, 31, 32, // xtensa

    ((12 + 10) << 2) | (MP_CODE_BYTECODE - MP_CODE_BYTECODE), // n bytes

    // prelude
    1, // n_state
    0, // n_exc_stack
    0, // scope_flags
    0, // n_pos_args
    0, // n_kwonly_args
    0, // n_def_pos_args
    5, // code_info_size (incl this member)
    0, 0, // simple_name
    0, 0, // source_file
    255, // end of list sentinel

    // bytecode
    MP_BC_MAKE_FUNCTION, 0,
    MP_BC_STORE_GLOBAL, 3 << 1, 'f', 'o', 'o',
    MP_BC_LOAD_CONST_NONE,
    MP_BC_RETURN_VALUE,
    0, 0, 0, // padding

    6 << 1, 'n', 'a', 't', 'i', 'v', 'e', // simple_name
    6 << 1, 'n', 'a', 't', 'i', 'v', 'e', // source_file
    0, // n_obj
    1, // n_raw_code

    // raw_code
    UINT_HI(((CODE_SIZE + 2) << 2) | (MP_CODE_NATIVE_VIPER - MP_CODE_BYTECODE)),
    UINT_LO(((CODE_SIZE + 2) << 2) | (MP_CODE_NATIVE_VIPER - MP_CODE_BYTECODE)),
};

const uint8_t trailer[] __attribute__((section(".trailer"))) = {
    0, 0, // qstr0
    1, // n_qstr
    UINT_HI(CODE_SIZE << 2), UINT_LO(CODE_SIZE << 2), 5 << 1, 'p', 'r', 'i', 'n', 't', // qstr0
    0, // scope_flags
    0, // n_obj
    0, // n_raw_code
};

uintptr_t foo(mp_obj_fun_bc_t *self, uintptr_t n_args, uintptr_t n_kw, uintptr_t *args) {
    uint16_t *qstr_table = (uint16_t*)(self->fun_data + CODE_SIZE);
    const struct _mp_f_table_t *f = (const void*)self->const_table[0];

    uintptr_t old_globals = f->swap_globals(self->globals);

    uintptr_t print = f->load_name(qstr_table[0]);

    for (int i = 0; i < 10; ++i) {
        uintptr_t args[2] = {MP_OBJ_NEW_SMALL_INT(i), (i & 1) ? f->true_obj : f->false_obj};
        f->call_function_n_kw(print, 2, &args[0]);
    }

    f->swap_globals(old_globals);

    uintptr_t list[2] = {MP_OBJ_NEW_SMALL_INT(42), MP_OBJ_NEW_SMALL_INT(n_args)};
    return f->new_list(2, list);
}
