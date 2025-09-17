// native-debug specific stuff

#include "py/mpconfig.h"

#if MICROPY_EMIT_NATIVE_DEBUG

#include "py/asmbase.h"
#include "py/nativeglue.h"

#define asm_debug_printf(as, fmt, ...) \
    do { \
        if (as->base.pass == MP_ASM_PASS_EMIT) { \
            if (fmt[0] != 'E') { \
                mp_printf(MICROPY_EMIT_NATIVE_DEBUG_PRINTER, "    "); \
            } \
            if (as->base.suppress) { \
                mp_printf(MICROPY_EMIT_NATIVE_DEBUG_PRINTER, "dead_code "); \
            } \
            mp_printf(MICROPY_EMIT_NATIVE_DEBUG_PRINTER, fmt, __VA_ARGS__); \
        } \
    } while (0)

enum {
    ASM_DEBUG_REG_R00,
    ASM_DEBUG_REG_R01,
    ASM_DEBUG_REG_R02,
    ASM_DEBUG_REG_R03,
    ASM_DEBUG_REG_R04,
    ASM_DEBUG_REG_R05,
    ASM_DEBUG_REG_R06,
    ASM_DEBUG_REG_R07,
    ASM_DEBUG_REG_R08,
    ASM_DEBUG_REG_R09,
    ASM_DEBUG_REG_R10,
    ASM_DEBUG_REG_R11,
};

typedef struct _asm_debug_t {
    mp_asm_base_t base;
} asm_debug_t;

static const char *const reg_name_table[] = {
    "r_ret",
    "r_arg1",
    "r_arg2",
    "r_arg3",
    "r_arg4",
    "r_temp0",
    "r_temp1",
    "r_temp2",
    "r_local1",
    "r_local2",
    "r_local3",
    "r_fun_table",
};

static const char *const fun_name_table[MP_F_NUMBER_OF] = {
    [MP_F_CONVERT_OBJ_TO_NATIVE] = "convert_obj_to_native",
    [MP_F_CONVERT_NATIVE_TO_OBJ] = "convert_native_to_obj",
    [MP_F_NATIVE_SWAP_GLOBALS] = "native_swap_globals",
    [MP_F_LOAD_NAME] = "load_name",
    [MP_F_LOAD_GLOBAL] = "load_global",
    [MP_F_LOAD_BUILD_CLASS] = "load_build_class",
    [MP_F_LOAD_ATTR] = "load_attr",
    [MP_F_LOAD_METHOD] = "load_method",
    [MP_F_LOAD_SUPER_METHOD] = "load_super_method",
    [MP_F_STORE_NAME] = "store_name",
    [MP_F_STORE_GLOBAL] = "store_global",
    [MP_F_STORE_ATTR] = "store_attr",
    [MP_F_OBJ_SUBSCR] = "obj_subscr",
    [MP_F_OBJ_IS_TRUE] = "obj_is_true",
    [MP_F_UNARY_OP] = "unary_op",
    [MP_F_BINARY_OP] = "binary_op",
    [MP_F_BUILD_TUPLE] = "build_tuple",
    [MP_F_BUILD_LIST] = "build_list",
    [MP_F_BUILD_MAP] = "build_map",
    [MP_F_BUILD_SET] = "build_set",
    [MP_F_STORE_SET] = "store_set",
    [MP_F_LIST_APPEND] = "list_append",
    [MP_F_STORE_MAP] = "store_map",
    [MP_F_MAKE_FUNCTION_FROM_PROTO_FUN] = "make_function_from_proto_fun",
    [MP_F_NATIVE_CALL_FUNCTION_N_KW] = "native_call_function_n_kw",
    [MP_F_CALL_METHOD_N_KW] = "call_method_n_kw",
    [MP_F_CALL_METHOD_N_KW_VAR] = "call_method_n_kw_var",
    [MP_F_NATIVE_GETITER] = "native_getiter",
    [MP_F_NATIVE_ITERNEXT] = "native_iternext",
    [MP_F_NLR_PUSH] = "nlr_push",
    [MP_F_NLR_POP] = "nlr_pop",
    [MP_F_NATIVE_RAISE] = "native_raise",
    [MP_F_IMPORT_NAME] = "import_name",
    [MP_F_IMPORT_FROM] = "import_from",
    [MP_F_IMPORT_ALL] = "import_all",
    [MP_F_NEW_SLICE] = "new_slice",
    [MP_F_UNPACK_SEQUENCE] = "unpack_sequence",
    [MP_F_UNPACK_EX] = "unpack_ex",
    [MP_F_DELETE_NAME] = "delete_name",
    [MP_F_DELETE_GLOBAL] = "delete_global",
    [MP_F_NEW_CLOSURE] = "new_closure",
    [MP_F_ARG_CHECK_NUM_SIG] = "arg_check_num_sig",
    [MP_F_SETUP_CODE_STATE] = "setup_code_state",
    [MP_F_SMALL_INT_FLOOR_DIVIDE] = "small_int_floor_divide",
    [MP_F_SMALL_INT_MODULO] = "small_int_modulo",
    [MP_F_NATIVE_YIELD_FROM] = "native_yield_from",
    [MP_F_SETJMP] = "setjmp",
};

static void asm_debug_end_pass(asm_debug_t *as) {
    (void)as;
}

static void asm_debug_entry(asm_debug_t *as, int num_locals, char *name) {
    asm_debug_printf(as, "ENTRY(%s, num_locals=%d)\n", name != NULL ? name : "?", num_locals);
}

static void asm_debug_exit(asm_debug_t *as) {
    asm_debug_printf(as, "EXIT(%u)\n", 0);
}

static void asm_debug_fun(asm_debug_t *as, const char *op, int fun_idx) {
    asm_debug_printf(as, "%s(%s)\n", op, fun_name_table[fun_idx]);
}

static void asm_debug_reg(asm_debug_t *as, const char *op, int reg) {
    asm_debug_printf(as, "%s(%s)\n", op, reg_name_table[reg]);
}

static void asm_debug_label(asm_debug_t *as, const char *op, unsigned int label) {
    asm_debug_printf(as, "%s(label_%u)\n", op, label);
}

static void asm_debug_reg_imm(asm_debug_t *as, const char *op, int reg, int imm) {
    asm_debug_printf(as, "%s(%s, %d=0x%x)\n", op, reg_name_table[reg], imm, imm);
}

#if !MICROPY_PERSISTENT_CODE_SAVE
static void asm_debug_reg_qstr(asm_debug_t *as, const char *op, int reg, int qst) {
    asm_debug_printf(as, "%s(%s, %s)\n", op, reg_name_table[reg], qstr_str(qst));
}
#endif

static void asm_debug_reg_reg(asm_debug_t *as, const char *op, int reg1, int reg2) {
    asm_debug_printf(as, "%s(%s, %s)\n", op, reg_name_table[reg1], reg_name_table[reg2]);
}

static void asm_debug_reg_local(asm_debug_t *as, const char *op, int reg, unsigned int local) {
    asm_debug_printf(as, "%s(%s, local_%u)\n", op, reg_name_table[reg], local);
}

static void asm_debug_reg_label(asm_debug_t *as, const char *op, int reg, unsigned int label) {
    asm_debug_printf(as, "%s(%s, label_%u)\n", op, reg_name_table[reg], label);
}

static void asm_debug_local_reg(asm_debug_t *as, const char *op, int local, int reg) {
    asm_debug_printf(as, "%s(local_%d, %s)\n", op, local, reg_name_table[reg]);
}

static void asm_debug_reg_label_bool(asm_debug_t *as, const char *op, int reg, unsigned int label, bool b) {
    asm_debug_printf(as, "%s(%s, label_%u, %s)\n", op, reg_name_table[reg], label, b ? "true" : "false");
}

static void asm_debug_reg_reg_offset(asm_debug_t *as, const char *op, int reg1, int reg2, int offset) {
    asm_debug_printf(as, "%s(%s, %s, %d)\n", op, reg_name_table[reg1], reg_name_table[reg2], offset);
}

static void asm_debug_reg_reg_label(asm_debug_t *as, const char *op, int reg1, int reg2, unsigned int label) {
    asm_debug_printf(as, "%s(%s, %s, label_%u)\n", op, reg_name_table[reg1], reg_name_table[reg2], label);
}

static void asm_debug_setcc_reg_reg_reg(asm_debug_t *as, int op, int reg1, int reg2, int reg3) {
    asm_debug_printf(as, "setcc(%d, %s, %s, %s)\n", op, reg_name_table[reg1], reg_name_table[reg2], reg_name_table[reg3]);
}

// The following macros provide a (mostly) arch-independent API to
// generate native code, and are used by the native emitter.

#define ASM_WORD_SIZE (8)

#define REG_RET ASM_DEBUG_REG_R00
#define REG_ARG_1 ASM_DEBUG_REG_R01
#define REG_ARG_2 ASM_DEBUG_REG_R02
#define REG_ARG_3 ASM_DEBUG_REG_R03
#define REG_ARG_4 ASM_DEBUG_REG_R04

#define REG_TEMP0 ASM_DEBUG_REG_R05
#define REG_TEMP1 ASM_DEBUG_REG_R06
#define REG_TEMP2 ASM_DEBUG_REG_R07

#define REG_LOCAL_1 ASM_DEBUG_REG_R08
#define REG_LOCAL_2 ASM_DEBUG_REG_R09
#define REG_LOCAL_3 ASM_DEBUG_REG_R10
#define REG_LOCAL_NUM (3)

// Holds a pointer to mp_fun_table
#define REG_FUN_TABLE ASM_DEBUG_REG_R11

#define ASM_T               asm_debug_t
#define ASM_END_PASS        asm_debug_end_pass
#define ASM_ENTRY(as, num_locals, name) \
    asm_debug_entry(as, num_locals, name)
#define ASM_EXIT(as) \
    asm_debug_exit(as)

#define ASM_JUMP(as, label) \
    asm_debug_label(as, "jump", label)
#define ASM_JUMP_IF_REG_ZERO(as, reg, label, bool_test) \
    asm_debug_reg_label_bool(as, "jump_if_reg_zero", reg, label, bool_test)
#define ASM_JUMP_IF_REG_NONZERO(as, reg, label, bool_test) \
    asm_debug_reg_label_bool(as, "jump_if_reg_nonzero", reg, label, bool_test)
#define ASM_JUMP_IF_REG_EQ(as, reg1, reg2, label) \
    asm_debug_reg_reg_label(as, "jump_if_reg_eq", reg1, reg2, label)
#define ASM_JUMP_REG(as, reg) \
    asm_debug_reg(as, "jump_reg", reg)
#define ASM_CALL_IND(as, idx) \
    asm_debug_fun(as, "call_ind", idx)

#define ASM_MOV_LOCAL_REG(as, local_num, reg_src) \
    asm_debug_local_reg(as, "mov_local_reg", local_num, reg_src)
#define ASM_MOV_REG_IMM(as, reg_dest, imm) \
    asm_debug_reg_imm(as, "mov_reg_imm", reg_dest, imm)
#define ASM_MOV_REG_QSTR(as, reg_dest, qst) \
    asm_debug_reg_qstr(as, "mov_reg_qstr", reg_dest, qst)
#define ASM_MOV_REG_LOCAL(as, reg_dest, local_num) \
    asm_debug_reg_local(as, "mov_reg_local", reg_dest, local_num)
#define ASM_MOV_REG_REG(as, reg_dest, reg_src) \
    asm_debug_reg_reg(as, "mov_reg_reg", reg_dest, reg_src)
#define ASM_MOV_REG_LOCAL_ADDR(as, reg_dest, local_num) \
    asm_debug_reg_local(as, "mov_reg_local_addr", reg_dest, local_num)
#define ASM_MOV_REG_PCREL(as, reg_dest, label) \
    asm_debug_reg_label(as, "mov_reg_pcrel", reg_dest, label)

#define ASM_NOT_REG(as, reg_dest) \
    asm_debug_reg(as, "not", reg_dest)
#define ASM_NEG_REG(as, reg_dest) \
    asm_debug_reg(as, "neg", reg_dest)
#define ASM_LSL_REG_REG(as, reg_dest, reg_src) \
    asm_debug_reg_reg(as, "lsl", reg_dest, reg_src)
#define ASM_LSR_REG_REG(as, reg_dest, reg_src) \
    asm_debug_reg_reg(as, "lsr", reg_dest, reg_src)
#define ASM_ASR_REG_REG(as, reg_dest, reg_src) \
    asm_debug_reg_reg(as, "asr", reg_dest, reg_src)
#define ASM_OR_REG_REG(as, reg_dest, reg_src) \
    asm_debug_reg_reg(as, "or", reg_dest, reg_src)
#define ASM_XOR_REG_REG(as, reg_dest, reg_src) \
    asm_debug_reg_reg(as, "xor", reg_dest, reg_src)
#define ASM_AND_REG_REG(as, reg_dest, reg_src) \
    asm_debug_reg_reg(as, "and", reg_dest, reg_src)
#define ASM_ADD_REG_REG(as, reg_dest, reg_src) \
    asm_debug_reg_reg(as, "add", reg_dest, reg_src)
#define ASM_SUB_REG_REG(as, reg_dest, reg_src) \
    asm_debug_reg_reg(as, "sub", reg_dest, reg_src)
#define ASM_MUL_REG_REG(as, reg_dest, reg_src) \
    asm_debug_reg_reg(as, "mul", reg_dest, reg_src)

#define ASM_LOAD_REG_REG_OFFSET(as, reg_dest, reg_base, word_offset) \
    asm_debug_reg_reg_offset(as, "load", reg_dest, reg_base, word_offset)
#define ASM_LOAD8_REG_REG(as, reg_dest, reg_base) \
    asm_debug_reg_reg(as, "load8", reg_dest, reg_base)
#define ASM_LOAD16_REG_REG(as, reg_dest, reg_base) \
    asm_debug_reg_reg(as, "load16", reg_dest, reg_base)
#define ASM_LOAD16_REG_REG_OFFSET(as, reg_dest, reg_base, uint16_offset) \
    asm_debug_reg_reg_offset(as, "load16", reg_dest, reg_base, uint16_offset)
#define ASM_LOAD32_REG_REG(as, reg_dest, reg_base) \
    asm_debug_reg_reg(as, "load32", reg_dest, reg_base)

#define ASM_STORE_REG_REG_OFFSET(as, reg_src, reg_base, word_offset) \
    asm_debug_reg_reg_offset(as, "store", reg_src, reg_base, word_offset)
#define ASM_STORE8_REG_REG(as, reg_src, reg_base) \
    asm_debug_reg_reg(as, "store8", reg_src, reg_base)
#define ASM_STORE16_REG_REG(as, reg_src, reg_base) \
    asm_debug_reg_reg(as, "store16", reg_src, reg_base)
#define ASM_STORE32_REG_REG(as, reg_src, reg_base) \
    asm_debug_reg_reg(as, "store32", reg_src, reg_base)

// Word indices of REG_LOCAL_x in nlr_buf_t
#define NLR_BUF_IDX_LOCAL_1 (5) // rbx

#define N_DEBUG (1)
#define EXPORT_FUN(name) emit_native_debug_##name
#include "py/emitnative.c"

#endif
