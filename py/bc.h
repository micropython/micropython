mp_obj_t mp_execute_byte_code(const byte *code, const mp_obj_t *args, uint n_args, uint n_state);
MP_BOOL mp_execute_byte_code_2(const byte **ip_in_out, mp_obj_t *fastn, mp_obj_t **sp_in_out);
