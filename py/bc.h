mp_obj_t mp_execute_byte_code(const byte *code, const mp_obj_t *args, uint n_args, const mp_obj_t *args2, uint n_args2, uint n_state);
bool mp_execute_byte_code_2(const byte *code_info, const byte **ip_in_out, mp_obj_t *fastn, mp_obj_t **sp_in_out);
void mp_byte_code_print(const byte *code, int len);
