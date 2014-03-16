#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "misc.h"
#include "mpconfig.h"
#include "asmthumb.h"

// wrapper around everything in this file
#if MICROPY_EMIT_THUMB || MICROPY_EMIT_INLINE_THUMB

#define UNSIGNED_FIT8(x) (((x) & 0xffffff00) == 0)
#define UNSIGNED_FIT16(x) (((x) & 0xffff0000) == 0)
#define SIGNED_FIT8(x) (((x) & 0xffffff80) == 0) || (((x) & 0xffffff80) == 0xffffff80)
#define SIGNED_FIT9(x) (((x) & 0xffffff00) == 0) || (((x) & 0xffffff00) == 0xffffff00)
#define SIGNED_FIT12(x) (((x) & 0xfffff800) == 0) || (((x) & 0xfffff800) == 0xfffff800)

struct _asm_thumb_t {
    int pass;
    uint code_offset;
    uint code_size;
    byte *code_base;
    byte dummy_data[8];

    int max_num_labels;
    int *label_offsets;
    int num_locals;
    uint push_reglist;
    uint stack_adjust;
};

asm_thumb_t *asm_thumb_new(uint max_num_labels) {
    asm_thumb_t *as;

    as = m_new(asm_thumb_t, 1);
    as->pass = 0;
    as->code_offset = 0;
    as->code_size = 0;
    as->code_base = NULL;
    as->max_num_labels = max_num_labels;
    as->label_offsets = m_new(int, max_num_labels);
    as->num_locals = 0;

    return as;
}

void asm_thumb_free(asm_thumb_t *as, bool free_code) {
    if (free_code) {
        m_del(byte, as->code_base, as->code_size);
    }
    /*
    if (as->label != NULL) {
        int i;
        for (i = 0; i < as->label->len; ++i)
        {
            Label *lab = &g_array_index(as->label, Label, i);
            if (lab->unresolved != NULL)
                g_array_free(lab->unresolved, true);
        }
        g_array_free(as->label, true);
    }
    */
    m_del_obj(asm_thumb_t, as);
}

void asm_thumb_start_pass(asm_thumb_t *as, int pass) {
    as->pass = pass;
    as->code_offset = 0;
    if (pass == ASM_THUMB_PASS_2) {
        memset(as->label_offsets, -1, as->max_num_labels * sizeof(int));
    }
}

void asm_thumb_end_pass(asm_thumb_t *as) {
    if (as->pass == ASM_THUMB_PASS_2) {
        // calculate size of code in bytes
        as->code_size = as->code_offset;
        as->code_base = m_new(byte, as->code_size);
        //printf("code_size: %u\n", as->code_size);
    }

    /*
    // check labels are resolved
    if (as->label != NULL)
    {
        int i;
        for (i = 0; i < as->label->len; ++i)
            if (g_array_index(as->label, Label, i).unresolved != NULL)
                return false;
    }
    */
}

// all functions must go through this one to emit bytes
STATIC byte *asm_thumb_get_cur_to_write_bytes(asm_thumb_t *as, int num_bytes_to_write) {
    //printf("emit %d\n", num_bytes_to_write);
    if (as->pass < ASM_THUMB_PASS_3) {
        as->code_offset += num_bytes_to_write;
        return as->dummy_data;
    } else {
        assert(as->code_offset + num_bytes_to_write <= as->code_size);
        byte *c = as->code_base + as->code_offset;
        as->code_offset += num_bytes_to_write;
        return c;
    }
}

uint asm_thumb_get_code_size(asm_thumb_t *as) {
    return as->code_size;
}

void *asm_thumb_get_code(asm_thumb_t *as) {
    // need to set low bit to indicate that it's thumb code
    return (void *)(((machine_uint_t)as->code_base) | 1);
}

/*
STATIC void asm_thumb_write_byte_1(asm_thumb_t *as, byte b1) {
    byte *c = asm_thumb_get_cur_to_write_bytes(as, 1);
    c[0] = b1;
}
*/

STATIC void asm_thumb_write_op16(asm_thumb_t *as, uint op) {
    byte *c = asm_thumb_get_cur_to_write_bytes(as, 2);
    // little endian
    c[0] = op;
    c[1] = op >> 8;
}

STATIC void asm_thumb_write_op32(asm_thumb_t *as, uint op1, uint op2) {
    byte *c = asm_thumb_get_cur_to_write_bytes(as, 4);
    // little endian, op1 then op2
    c[0] = op1;
    c[1] = op1 >> 8;
    c[2] = op2;
    c[3] = op2 >> 8;
}

/*
#define IMM32_L0(x) ((x) & 0xff)
#define IMM32_L1(x) (((x) >> 8) & 0xff)
#define IMM32_L2(x) (((x) >> 16) & 0xff)
#define IMM32_L3(x) (((x) >> 24) & 0xff)

STATIC void asm_thumb_write_word32(asm_thumb_t *as, int w32) {
    byte *c = asm_thumb_get_cur_to_write_bytes(as, 4);
    c[0] = IMM32_L0(w32);
    c[1] = IMM32_L1(w32);
    c[2] = IMM32_L2(w32);
    c[3] = IMM32_L3(w32);
}
*/

// rlolist is a bit map indicating desired lo-registers
#define OP_PUSH_RLIST(rlolist)      (0xb400 | (rlolist))
#define OP_PUSH_RLIST_LR(rlolist)   (0xb400 | 0x0100 | (rlolist))
#define OP_POP_RLIST(rlolist)       (0xbc00 | (rlolist))
#define OP_POP_RLIST_PC(rlolist)    (0xbc00 | 0x0100 | (rlolist))

#define OP_ADD_SP(num_words) (0xb000 | (num_words))
#define OP_SUB_SP(num_words) (0xb080 | (num_words))

void asm_thumb_entry(asm_thumb_t *as, int num_locals) {
    // work out what to push and how many extra space to reserve on stack
    // so that we have enough for all locals and it's aligned an 8-byte boundary
    uint reglist;
    uint stack_adjust;
    if (num_locals < 0) {
        num_locals = 0;
    }
    // don't ppop r0 because it's used for return value
    switch (num_locals) {
        case 0:
            reglist = 0xf2;
            stack_adjust = 0;
            break;

        case 1:
            reglist = 0xf2;
            stack_adjust = 0;
            break;

        case 2:
            reglist = 0xfe;
            stack_adjust = 0;
            break;

        case 3:
            reglist = 0xfe;
            stack_adjust = 0;
            break;

        default:
            reglist = 0xfe;
            stack_adjust = ((num_locals - 3) + 1) & (~1);
            break;
    }
    asm_thumb_write_op16(as, OP_PUSH_RLIST_LR(reglist));
    if (stack_adjust > 0) {
        asm_thumb_write_op16(as, OP_SUB_SP(stack_adjust));
    }
    as->push_reglist = reglist;
    as->stack_adjust = stack_adjust;
    as->num_locals = num_locals;
}

void asm_thumb_exit(asm_thumb_t *as) {
    if (as->stack_adjust > 0) {
        asm_thumb_write_op16(as, OP_ADD_SP(as->stack_adjust));
    }
    asm_thumb_write_op16(as, OP_POP_RLIST_PC(as->push_reglist));
}

void asm_thumb_label_assign(asm_thumb_t *as, int label) {
    assert(label < as->max_num_labels);
    if (as->pass == ASM_THUMB_PASS_2) {
        // assign label offset
        assert(as->label_offsets[label] == -1);
        as->label_offsets[label] = as->code_offset;
    } else if (as->pass == ASM_THUMB_PASS_3) {
        // ensure label offset has not changed from PASS_2 to PASS_3
        //printf("l%d: (at %d=%ld)\n", label, as->label_offsets[label], as->code_offset);
        assert(as->label_offsets[label] == as->code_offset);
    }
}

STATIC int get_label_dest(asm_thumb_t *as, int label) {
    assert(label < as->max_num_labels);
    return as->label_offsets[label];
}

#define OP_MOVS_RLO_I8(rlo_dest, i8_src) (0x2000 | ((rlo_dest) << 8) | (i8_src))

// the i8_src value will be zero extended into the r32 register!
void asm_thumb_movs_rlo_i8(asm_thumb_t *as, uint rlo_dest, int i8_src) {
    assert(rlo_dest < REG_R8);
    // movs rlo_dest, #i8_src
    asm_thumb_write_op16(as, OP_MOVS_RLO_I8(rlo_dest, i8_src));
}

#define OP_MOVW (0xf240)
#define OP_MOVT (0xf2c0)

// if loading lo half with movw, the i16 value will be zero extended into the r32 register!
STATIC void asm_thumb_mov_reg_i16(asm_thumb_t *as, uint mov_op, uint reg_dest, int i16_src) {
    assert(reg_dest < REG_R15);
    // mov[wt] reg_dest, #i16_src
    asm_thumb_write_op32(as, mov_op | ((i16_src >> 1) & 0x0400) | ((i16_src >> 12) & 0xf), ((i16_src << 4) & 0x7000) | (reg_dest << 8) | (i16_src & 0xff));
}

// the i16_src value will be zero extended into the r32 register!
void asm_thumb_movw_reg_i16(asm_thumb_t *as, uint reg_dest, int i16_src) {
    asm_thumb_mov_reg_i16(as, OP_MOVW, reg_dest, i16_src);
}

// the i16_src value will be zero extended into the r32 register!
void asm_thumb_movt_reg_i16(asm_thumb_t *as, uint reg_dest, int i16_src) {
    asm_thumb_mov_reg_i16(as, OP_MOVT, reg_dest, i16_src);
}

void asm_thumb_mov_reg_reg(asm_thumb_t *as, uint reg_dest, uint reg_src) {
    uint op_lo;
    if (reg_src < 8) {
        op_lo = reg_src << 3;
    } else {
        op_lo = 0x40 | ((reg_src - 8) << 3);
    }
    if (reg_dest < 8) {
        op_lo |= reg_dest;
    } else {
        op_lo |= 0x80 | (reg_dest - 8);
    }
    // mov reg_dest, reg_src
    asm_thumb_write_op16(as, 0x4600 | op_lo);
}

#define OP_SUBS_RLO_RLO_I3(rlo_dest, rlo_src, i3_src) (0x1e00 | ((i3_src) << 6) | ((rlo_src) << 3) | (rlo_dest))

void asm_thumb_subs_rlo_rlo_i3(asm_thumb_t *as, uint rlo_dest, uint rlo_src, int i3_src) {
    assert(rlo_dest < REG_R8);
    assert(rlo_src < REG_R8);
    asm_thumb_write_op16(as, OP_SUBS_RLO_RLO_I3(rlo_dest, rlo_src, i3_src));
}

#define OP_CMP_RLO_I8(rlo, i8) (0x2800 | ((rlo) << 8) | (i8))

void asm_thumb_cmp_rlo_i8(asm_thumb_t *as, uint rlo, int i8) {
    assert(rlo < REG_R8);
    asm_thumb_write_op16(as, OP_CMP_RLO_I8(rlo, i8));
}

#define OP_B_N(byte_offset) (0xe000 | (((byte_offset) >> 1) & 0x07ff))

void asm_thumb_b_n(asm_thumb_t *as, int label) {
    int dest = get_label_dest(as, label);
    int rel = dest - as->code_offset;
    rel -= 4; // account for instruction prefetch, PC is 4 bytes ahead of this instruction
    if (SIGNED_FIT12(rel)) {
        asm_thumb_write_op16(as, OP_B_N(rel));
    } else {
        printf("asm_thumb_b_n: branch does not fit in 12 bits\n");
    }
}

#define OP_BCC_N(cond, byte_offset) (0xd000 | ((cond) << 8) | (((byte_offset) >> 1) & 0x00ff))

void asm_thumb_bcc_n(asm_thumb_t *as, int cond, int label) {
    int dest = get_label_dest(as, label);
    int rel = dest - as->code_offset;
    rel -= 4; // account for instruction prefetch, PC is 4 bytes ahead of this instruction
    if (SIGNED_FIT9(rel)) {
        asm_thumb_write_op16(as, OP_BCC_N(cond, rel));
    } else {
        printf("asm_thumb_bcc_n: branch does not fit in 9 bits\n");
    }
}

void asm_thumb_mov_reg_i32(asm_thumb_t *as, uint reg_dest, machine_uint_t i32) {
    // movw, movt does it in 8 bytes
    // ldr [pc, #], dw does it in 6 bytes, but we might not reach to end of code for dw

    asm_thumb_mov_reg_i16(as, OP_MOVW, reg_dest, i32);
    asm_thumb_mov_reg_i16(as, OP_MOVT, reg_dest, i32 >> 16);
}

void asm_thumb_mov_reg_i32_optimised(asm_thumb_t *as, uint reg_dest, int i32) {
    if (reg_dest < 8 && UNSIGNED_FIT8(i32)) {
        asm_thumb_movs_rlo_i8(as, reg_dest, i32);
    } else if (UNSIGNED_FIT16(i32)) {
        asm_thumb_mov_reg_i16(as, OP_MOVW, reg_dest, i32);
    } else {
        asm_thumb_mov_reg_i32(as, reg_dest, i32);
    }
}

#define OP_STR_TO_SP_OFFSET(rlo_dest, word_offset) (0x9000 | ((rlo_dest) << 8) | ((word_offset) & 0x00ff))
#define OP_LDR_FROM_SP_OFFSET(rlo_dest, word_offset) (0x9800 | ((rlo_dest) << 8) | ((word_offset) & 0x00ff))

void asm_thumb_mov_local_reg(asm_thumb_t *as, int local_num, uint rlo_src) {
    assert(rlo_src < REG_R8);
    int word_offset = as->num_locals - local_num - 1;
    assert(as->pass < ASM_THUMB_PASS_3 || word_offset >= 0);
    asm_thumb_write_op16(as, OP_STR_TO_SP_OFFSET(rlo_src, word_offset));
}

void asm_thumb_mov_reg_local(asm_thumb_t *as, uint rlo_dest, int local_num) {
    assert(rlo_dest < REG_R8);
    int word_offset = as->num_locals - local_num - 1;
    assert(as->pass < ASM_THUMB_PASS_3 || word_offset >= 0);
    asm_thumb_write_op16(as, OP_LDR_FROM_SP_OFFSET(rlo_dest, word_offset));
}

#define OP_ADD_REG_SP_OFFSET(rlo_dest, word_offset) (0xa800 | ((rlo_dest) << 8) | ((word_offset) & 0x00ff))

void asm_thumb_mov_reg_local_addr(asm_thumb_t *as, uint rlo_dest, int local_num) {
    assert(rlo_dest < REG_R8);
    int word_offset = as->num_locals - local_num - 1;
    assert(as->pass < ASM_THUMB_PASS_3 || word_offset >= 0);
    asm_thumb_write_op16(as, OP_ADD_REG_SP_OFFSET(rlo_dest, word_offset));
}

#define OP_ADD_REG_REG_REG(rlo_dest, rlo_src_a, rlo_src_b) (0x1800 | ((rlo_src_b) << 6) | ((rlo_src_a) << 3) | (rlo_dest))

void asm_thumb_add_reg_reg_reg(asm_thumb_t *as, uint rlo_dest, uint rlo_src_a, uint rlo_src_b) {
    asm_thumb_write_op16(as, OP_ADD_REG_REG_REG(rlo_dest, rlo_src_a, rlo_src_b));
}

#define OP_CMP_REG_REG(rlo_a, rlo_b) (0x4280 | ((rlo_b) << 3) | (rlo_a))

void asm_thumb_cmp_reg_reg(asm_thumb_t *as, uint rlo_a, uint rlo_b) {
    asm_thumb_write_op16(as, OP_CMP_REG_REG(rlo_a, rlo_b));
}

void asm_thumb_ite_ge(asm_thumb_t *as) {
    asm_thumb_write_op16(as, 0xbfac);
}

// this could be wrong, because it should have a range of +/- 16MiB...
#define OP_BW_HI(byte_offset) (0xf000 | (((byte_offset) >> 12) & 0x07ff))
#define OP_BW_LO(byte_offset) (0xb800 | (((byte_offset) >> 1) & 0x07ff))

void asm_thumb_b_label(asm_thumb_t *as, int label) {
    int dest = get_label_dest(as, label);
    int rel = dest - as->code_offset;
    rel -= 4; // account for instruction prefetch, PC is 4 bytes ahead of this instruction
    if (dest >= 0 && rel <= -4) {
        // is a backwards jump, so we know the size of the jump on the first pass
        // calculate rel assuming 12 bit relative jump
        if (SIGNED_FIT12(rel)) {
            asm_thumb_write_op16(as, OP_B_N(rel));
        } else {
            goto large_jump;
        }
    } else {
        // is a forwards jump, so need to assume it's large
        large_jump:
        asm_thumb_write_op32(as, OP_BW_HI(rel), OP_BW_LO(rel));
    }
}

// all these bit arithmetics need coverage testing!
#define OP_BCC_W_HI(cond, byte_offset) (0xf000 | ((cond) << 6) | (((byte_offset) >> 10) & 0x0400) | (((byte_offset) >> 14) & 0x003f))
#define OP_BCC_W_LO(byte_offset) (0x8000 | ((byte_offset) & 0x2000) | (((byte_offset) >> 1) & 0x0fff))

void asm_thumb_bcc_label(asm_thumb_t *as, int cond, int label) {
    int dest = get_label_dest(as, label);
    int rel = dest - as->code_offset;
    rel -= 4; // account for instruction prefetch, PC is 4 bytes ahead of this instruction
    if (dest >= 0 && rel <= -4) {
        // is a backwards jump, so we know the size of the jump on the first pass
        // calculate rel assuming 9 bit relative jump
        if (SIGNED_FIT9(rel)) {
            asm_thumb_write_op16(as, OP_BCC_N(cond, rel));
        } else {
            goto large_jump;
        }
    } else {
        // is a forwards jump, so need to assume it's large
        large_jump:
        asm_thumb_write_op32(as, OP_BCC_W_HI(cond, rel), OP_BCC_W_LO(rel));
    }
}

#define OP_BLX(reg) (0x4780 | ((reg) << 3))
#define OP_SVC(arg) (0xdf00 | (arg))
#define OP_LDR_FROM_BASE_OFFSET(rlo_dest, rlo_base, word_offset) (0x6800 | (((word_offset) << 6) & 0x07c0) | ((rlo_base) << 3) | (rlo_dest))

void asm_thumb_bl_ind(asm_thumb_t *as, void *fun_ptr, uint fun_id, uint reg_temp) {
    /* TODO make this use less bytes
    uint rlo_base = REG_R3;
    uint rlo_dest = REG_R7;
    uint word_offset = 4;
    asm_thumb_write_op16(as, 0x0000);
    asm_thumb_write_op16(as, 0x6800 | (word_offset << 6) | (rlo_base << 3) | rlo_dest); // ldr rlo_dest, [rlo_base, #offset]
    asm_thumb_write_op16(as, 0x4780 | (REG_R9 << 3)); // blx reg
    */

    if (0) {
        // load ptr to function into register using immediate, then branch
        // not relocatable
        asm_thumb_mov_reg_i32(as, reg_temp, (machine_uint_t)fun_ptr);
        asm_thumb_write_op16(as, OP_BLX(reg_temp));
    } else if (1) {
        asm_thumb_write_op16(as, OP_LDR_FROM_BASE_OFFSET(reg_temp, REG_R7, fun_id));
        asm_thumb_write_op16(as, OP_BLX(reg_temp));
    } else {
        // use SVC
        asm_thumb_write_op16(as, OP_SVC(fun_id));
    }
}

#endif // MICROPY_EMIT_THUMB || MICROPY_EMIT_INLINE_THUMB
