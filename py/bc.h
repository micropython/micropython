// Exception stack entry
typedef struct _mp_exc_stack {
    const byte *handler;
    // bit 0 is saved currently_in_except_block value
    mp_obj_t *val_sp;
    // Saved exception, valid if currently_in_except_block bit is 1
    mp_obj_t prev_exc;
    // We might only have 2 interesting cases here: SETUP_EXCEPT & SETUP_FINALLY,
    // consider storing it in bit 1 of val_sp. TODO: SETUP_WITH?
    byte opcode;
} mp_exc_stack_t;

mp_vm_return_kind_t mp_execute_byte_code(const byte *code, const mp_obj_t *args, uint n_args, const mp_obj_t *args2, uint n_args2, mp_obj_t *ret);
mp_vm_return_kind_t mp_execute_byte_code_2(const byte *code_info, const byte **ip_in_out, mp_obj_t *fastn, mp_obj_t **sp_in_out, mp_exc_stack_t *exc_stack, mp_exc_stack_t **exc_sp_in_out, volatile mp_obj_t inject_exc);
void mp_byte_code_print(const byte *code, int len);

// Helper macros to access pointer with least significant bit holding a flag
#define MP_TAGPTR_PTR(x) ((void*)((machine_uint_t)(x) & ~((machine_uint_t)1)))
#define MP_TAGPTR_TAG(x) ((machine_uint_t)(x) & 1)
#define MP_TAGPTR_MAKE(ptr, tag) ((void*)((machine_uint_t)(ptr) | tag))
