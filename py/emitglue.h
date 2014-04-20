// These variables and functions glue the code emitters to the runtime.

typedef enum {
    MP_CODE_UNUSED,
    MP_CODE_RESERVED,
    MP_CODE_BYTE,
    MP_CODE_NATIVE,
    MP_CODE_INLINE_ASM,
} mp_raw_code_kind_t;

typedef struct _mp_code_t {
    mp_raw_code_kind_t kind : 8;
    uint scope_flags : 8;
    uint n_args : 16;
    union {
        struct {
            byte *code;
            uint len;
        } u_byte;
        struct {
            mp_fun_t fun;
        } u_native;
        struct {
            void *fun;
        } u_inline_asm;
    };
    qstr *arg_names;
} mp_raw_code_t;

void mp_emit_glue_init(void);
void mp_emit_glue_deinit(void);

mp_raw_code_t *mp_emit_glue_new_raw_code(void);

void mp_emit_glue_assign_byte_code(mp_raw_code_t *rc, byte *code, uint len, int n_args, int n_locals, uint scope_flags, qstr *arg_names);
void mp_emit_glue_assign_native_code(mp_raw_code_t *rc, void *f, uint len, int n_args);
void mp_emit_glue_assign_inline_asm_code(mp_raw_code_t *rc, void *f, uint len, int n_args);

mp_obj_t mp_make_function_from_raw_code(mp_raw_code_t *rc, mp_obj_t def_args, mp_obj_t def_kw_args);
mp_obj_t mp_make_closure_from_raw_code(mp_raw_code_t *rc, uint n_closed_over, const mp_obj_t *args);
