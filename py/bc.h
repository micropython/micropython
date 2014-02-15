typedef enum {
    MP_VM_RETURN_NORMAL,
    MP_VM_RETURN_YIELD,
    MP_VM_RETURN_EXCEPTION,
} mp_vm_return_kind_t;

mp_vm_return_kind_t mp_execute_byte_code(const byte *code, const mp_obj_t *args, uint n_args, const mp_obj_t *args2, uint n_args2, uint n_state, mp_obj_t *ret);
mp_vm_return_kind_t mp_execute_byte_code_2(const byte *code_info, const byte **ip_in_out, mp_obj_t *fastn, mp_obj_t **sp_in_out);
void mp_byte_code_print(const byte *code, int len);
