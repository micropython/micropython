// These variables and functions glue the code emitters to the runtime.

void mp_emit_glue_init(void);
void mp_emit_glue_deinit(void);
uint mp_emit_glue_get_unique_code_id(void);
void mp_emit_glue_assign_byte_code(uint unique_code_id, byte *code, uint len, int n_args, int n_locals, uint scope_flags, qstr *arg_names);
void mp_emit_glue_assign_native_code(uint unique_code_id, void *f, uint len, int n_args);
void mp_emit_glue_assign_inline_asm_code(uint unique_code_id, void *f, uint len, int n_args);
