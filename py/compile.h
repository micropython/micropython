// These must fit in 8 bits; see scope.h
enum {
    MP_EMIT_OPT_NONE,
    MP_EMIT_OPT_BYTE_CODE,
    MP_EMIT_OPT_NATIVE_PYTHON,
    MP_EMIT_OPT_VIPER,
    MP_EMIT_OPT_ASM_THUMB,
};

mp_obj_t mp_compile(mp_parse_node_t pn, qstr source_file, uint emit_opt, bool is_repl);
