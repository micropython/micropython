#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <string.h>

#include "misc.h"
#include "asmx64.h"

/* all offsets are measured in multiples of 8 bytes */
#define WORD_SIZE                (8)

#define OPCODE_NOP               (0x90)
#define OPCODE_PUSH_R64          (0x50)
#define OPCODE_PUSH_I64          (0x68)
#define OPCODE_PUSH_M64          (0xff) /* /6 */
#define OPCODE_POP_R64           (0x58)
#define OPCODE_RET               (0xc3)
#define OPCODE_MOV_I8_TO_R8      (0xb0) /* +rb */
#define OPCODE_MOV_I64_TO_R64    (0xb8)
#define OPCODE_MOV_I32_TO_RM32   (0xc7)
#define OPCODE_MOV_R64_TO_RM64   (0x89)
#define OPCODE_MOV_RM64_TO_R64   (0x8b)
#define OPCODE_LEA_MEM_TO_R64    (0x8d) /* /r */
#define OPCODE_XOR_R64_TO_RM64   (0x31) /* /r */
#define OPCODE_ADD_R64_TO_RM64   (0x01)
#define OPCODE_ADD_I32_TO_RM32   (0x81) /* /0 */
#define OPCODE_ADD_I8_TO_RM32    (0x83) /* /0 */
#define OPCODE_SUB_R64_FROM_RM64 (0x29)
#define OPCODE_SUB_I32_FROM_RM64 (0x81) /* /5 */
#define OPCODE_SUB_I8_FROM_RM64  (0x83) /* /5 */
#define OPCODE_SHL_RM32_BY_I8    (0xc1) /* /4 */
#define OPCODE_SHR_RM32_BY_I8    (0xc1) /* /5 */
#define OPCODE_SAR_RM32_BY_I8    (0xc1) /* /7 */
#define OPCODE_CMP_I32_WITH_RM32 (0x81) /* /7 */
#define OPCODE_CMP_I8_WITH_RM32  (0x83) /* /7 */
#define OPCODE_CMP_R64_WITH_RM64 (0x39)
#define OPCODE_CMP_RM32_WITH_R32 (0x3b)
#define OPCODE_TEST_R8_WITH_RM8  (0x84) /* /r */
#define OPCODE_JMP_REL8          (0xeb)
#define OPCODE_JMP_REL32         (0xe9)
#define OPCODE_JCC_REL8          (0x70) /* | jcc type */
#define OPCODE_JCC_REL32_A       (0x0f)
#define OPCODE_JCC_REL32_B       (0x80) /* | jcc type */
#define OPCODE_SETCC_RM8_A       (0x0f)
#define OPCODE_SETCC_RM8_B       (0x90) /* | jcc type, /0 */
#define OPCODE_CALL_REL32        (0xe8)
#define OPCODE_CALL_RM32         (0xff) /* /2 */
#define OPCODE_LEAVE             (0xc9)

#define MODRM_R64(x)    ((x) << 3)
#define MODRM_RM_DISP0  (0x00)
#define MODRM_RM_DISP8  (0x40)
#define MODRM_RM_DISP32 (0x80)
#define MODRM_RM_REG    (0xc0)
#define MODRM_RM_R64(x) (x)

#define REX_PREFIX  (0x40)
#define REX_W       (0x08)  // width
#define REX_R       (0x04)  // register
#define REX_X       (0x02)  // index
#define REX_B       (0x01)  // base

#define IMM32_L0(x) ((x) & 0xff)
#define IMM32_L1(x) (((x) >> 8) & 0xff)
#define IMM32_L2(x) (((x) >> 16) & 0xff)
#define IMM32_L3(x) (((x) >> 24) & 0xff)
#define IMM64_L4(x) (((x) >> 32) & 0xff)
#define IMM64_L5(x) (((x) >> 40) & 0xff)
#define IMM64_L6(x) (((x) >> 48) & 0xff)
#define IMM64_L7(x) (((x) >> 56) & 0xff)

#define UNSIGNED_FIT8(x) (((x) & 0xffffffffffffff00) == 0)
#define UNSIGNED_FIT32(x) (((x) & 0xffffffff00000000) == 0)
#define SIGNED_FIT8(x) (((x) & 0xffffff80) == 0) || (((x) & 0xffffff80) == 0xffffff80)

struct _asm_x64_t {
    int pass;
    uint code_offset;
    uint code_size;
    byte *code_base;
    byte dummy_data[8];

    int next_label;
    int max_num_labels;
    int *label_offsets;
};

// for allocating memory, see src/v8/src/platform-linux.cc
void *alloc_mem(uint req_size, uint *alloc_size, bool is_exec) {
    req_size = (req_size + 0xfff) & (~0xfff);
    int prot = PROT_READ | PROT_WRITE | (is_exec ? PROT_EXEC : 0);
    void *ptr = mmap(NULL, req_size, prot, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        assert(0);
    }
    *alloc_size = req_size;
    return ptr;
}

asm_x64_t* asm_x64_new() {
    asm_x64_t* as;

    as = m_new(asm_x64_t, 1);
    as->pass = 0;
    as->code_offset = 0;
    as->code_size = 0;
    as->code_base = NULL;
    as->label_offsets = NULL;

    return as;
}

void asm_x64_free(asm_x64_t* as, bool free_code) {
    if (free_code) {
        m_free(as->code_base);
    }
    /*
    if (as->label != NULL) {
        int i;
        for (i = 0; i < as->label->len; ++i)
        {
            Label* lab = &g_array_index(as->label, Label, i);
            if (lab->unresolved != NULL)
                g_array_free(lab->unresolved, true);
        }
        g_array_free(as->label, true);
    }
    */
    m_free(as);
}

void asm_x64_start_pass(asm_x64_t *as, int pass) {
    as->pass = pass;
    as->code_offset = 0;
    as->next_label = 1;
    if (pass == ASM_X64_PASS_1) {
        as->max_num_labels = 0;
    } else {
        if (pass == ASM_X64_PASS_2) {
            memset(as->label_offsets, -1, as->max_num_labels * sizeof(int));
        }
    }
}

void asm_x64_end_pass(asm_x64_t *as) {
    if (as->pass == ASM_X64_PASS_1) {
        // calculate number of labels need
        if (as->next_label > as->max_num_labels) {
            as->max_num_labels = as->next_label;
        }
        as->label_offsets = m_new(int, as->max_num_labels);
    } else if (as->pass == ASM_X64_PASS_2) {
        // calculate size of code in bytes
        as->code_size = as->code_offset;
        as->code_base = m_new(byte, as->code_size);
        printf("code_size: %u\n", as->code_size);
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
static byte* asm_x64_get_cur_to_write_bytes(asm_x64_t* as, int num_bytes_to_write) {
    //printf("emit %d\n", num_bytes_to_write);
    if (as->pass < ASM_X64_PASS_3) {
        as->code_offset += num_bytes_to_write;
        return as->dummy_data;
    } else {
        assert(as->code_offset + num_bytes_to_write <= as->code_size);
        byte *c = as->code_base + as->code_offset;
        as->code_offset += num_bytes_to_write;
        return c;
    }
}

uint asm_x64_get_code_size(asm_x64_t* as) {
    return as->code_size;
}

void* asm_x64_get_code(asm_x64_t* as) {
    return as->code_base;
}

static void asm_x64_write_byte_1(asm_x64_t* as, byte b1) {
    byte* c = asm_x64_get_cur_to_write_bytes(as, 1);
    c[0] = b1;
}

static void asm_x64_write_byte_2(asm_x64_t* as, byte b1, byte b2) {
    byte* c = asm_x64_get_cur_to_write_bytes(as, 2);
    c[0] = b1;
    c[1] = b2;
}

static void asm_x64_write_byte_3(asm_x64_t* as, byte b1, byte b2, byte b3) {
    byte* c = asm_x64_get_cur_to_write_bytes(as, 3);
    c[0] = b1;
    c[1] = b2;
    c[2] = b3;
}

static void asm_x64_write_word32(asm_x64_t* as, int w32) {
    byte* c = asm_x64_get_cur_to_write_bytes(as, 4);
    c[0] = IMM32_L0(w32);
    c[1] = IMM32_L1(w32);
    c[2] = IMM32_L2(w32);
    c[3] = IMM32_L3(w32);
}

static void asm_x64_write_word64(asm_x64_t* as, int64_t w64) {
    byte* c = asm_x64_get_cur_to_write_bytes(as, 8);
    c[0] = IMM32_L0(w64);
    c[1] = IMM32_L1(w64);
    c[2] = IMM32_L2(w64);
    c[3] = IMM32_L3(w64);
    c[4] = IMM64_L4(w64);
    c[5] = IMM64_L5(w64);
    c[6] = IMM64_L6(w64);
    c[7] = IMM64_L7(w64);
}

/* unused
static void asm_x64_write_word32_to(asm_x64_t* as, int offset, int w32) {
    byte* c;
    assert(offset + 4 <= as->code_size);
    c = as->code_base + offset;
    c[0] = IMM32_L0(w32);
    c[1] = IMM32_L1(w32);
    c[2] = IMM32_L2(w32);
    c[3] = IMM32_L3(w32);
}
*/

static void asm_x64_write_r64_disp(asm_x64_t* as, int r64, int disp_r64, int disp_offset) {
    assert(disp_r64 != REG_RSP);

    if (disp_offset == 0 && disp_r64 != REG_RBP) {
        asm_x64_write_byte_1(as, MODRM_R64(r64) | MODRM_RM_DISP0 | MODRM_RM_R64(disp_r64));
    } else if (SIGNED_FIT8(disp_offset)) {
        asm_x64_write_byte_2(as, MODRM_R64(r64) | MODRM_RM_DISP8 | MODRM_RM_R64(disp_r64), IMM32_L0(disp_offset));
    } else {
        asm_x64_write_byte_1(as, MODRM_R64(r64) | MODRM_RM_DISP32 | MODRM_RM_R64(disp_r64));
        asm_x64_write_word32(as, disp_offset);
    }
}

void asm_x64_nop(asm_x64_t* as)
{
    asm_x64_write_byte_1(as, OPCODE_NOP);
}

void asm_x64_push_r64(asm_x64_t* as, int src_r64)
{
    asm_x64_write_byte_1(as, OPCODE_PUSH_R64 | src_r64);
}

void asm_x64_push_i32(asm_x64_t* as, int src_i32)
{
    asm_x64_write_byte_1(as, OPCODE_PUSH_I64);
    asm_x64_write_word32(as, src_i32); // will be sign extended to 64 bits
}

void asm_x64_push_disp(asm_x64_t* as, int src_r64, int src_offset) {
    asm_x64_write_byte_1(as, OPCODE_PUSH_M64);
    asm_x64_write_r64_disp(as, 6, src_r64, src_offset);
}

void asm_x64_pop_r64(asm_x64_t* as, int dest_r64)
{
    asm_x64_write_byte_1(as, OPCODE_POP_R64 | dest_r64);
}

static void asm_x64_ret(asm_x64_t* as)
{
    asm_x64_write_byte_1(as, OPCODE_RET);
}

void asm_x64_mov_r32_to_r32(asm_x64_t* as, int src_r32, int dest_r32) {
    // defaults to 32 bit operation
    asm_x64_write_byte_2(as, OPCODE_MOV_R64_TO_RM64, MODRM_R64(src_r32) | MODRM_RM_REG | MODRM_RM_R64(dest_r32));
}

void asm_x64_mov_r64_to_r64(asm_x64_t* as, int src_r64, int dest_r64) {
    // use REX prefix for 64 bit operation
    asm_x64_write_byte_3(as, REX_PREFIX | REX_W, OPCODE_MOV_R64_TO_RM64, MODRM_R64(src_r64) | MODRM_RM_REG | MODRM_RM_R64(dest_r64));
}

void asm_x64_mov_r64_to_disp(asm_x64_t* as, int src_r64, int dest_r64, int dest_disp) {
    // use REX prefix for 64 bit operation
    asm_x64_write_byte_2(as, REX_PREFIX | REX_W, OPCODE_MOV_R64_TO_RM64);
    asm_x64_write_r64_disp(as, src_r64, dest_r64, dest_disp);
}

void asm_x64_mov_disp_to_r64(asm_x64_t* as, int src_r64, int src_disp, int dest_r64) {
    // use REX prefix for 64 bit operation
    asm_x64_write_byte_2(as, REX_PREFIX | REX_W, OPCODE_MOV_RM64_TO_R64);
    asm_x64_write_r64_disp(as, dest_r64, src_r64, src_disp);
}

void asm_x64_lea_disp_to_r64(asm_x64_t* as, int src_r64, int src_disp, int dest_r64) {
    // use REX prefix for 64 bit operation
    asm_x64_write_byte_2(as, REX_PREFIX | REX_W, OPCODE_LEA_MEM_TO_R64);
    asm_x64_write_r64_disp(as, dest_r64, src_r64, src_disp);
}

void asm_x64_mov_i8_to_r8(asm_x64_t *as, int src_i8, int dest_r64) {
    asm_x64_write_byte_2(as, OPCODE_MOV_I8_TO_R8 | dest_r64, src_i8);
}

void asm_x64_mov_i32_to_r64(asm_x64_t* as, int src_i32, int dest_r64) {
    // cpu defaults to i32 to r64, with zero extension
    asm_x64_write_byte_1(as, OPCODE_MOV_I64_TO_R64 | dest_r64);
    asm_x64_write_word32(as, src_i32);
}

void asm_x64_mov_i64_to_r64(asm_x64_t* as, int64_t src_i64, int dest_r64) {
    // cpu defaults to i32 to r64
    // to mov i64 to r64 need to use REX prefix
    asm_x64_write_byte_2(as, REX_PREFIX | REX_W, OPCODE_MOV_I64_TO_R64 | dest_r64);
    asm_x64_write_word64(as, src_i64);
}

void asm_x64_mov_i64_to_r64_optimised(asm_x64_t *as, int64_t src_i64, int dest_r64) {
    if (UNSIGNED_FIT32(src_i64)) {
        // 5 bytes
        asm_x64_mov_i32_to_r64(as, src_i64 & 0xffffffff, dest_r64);
    } else {
        // 10 bytes
        asm_x64_mov_i64_to_r64(as, src_i64, dest_r64);
    }
}

void asm_x64_mov_i32_to_disp(asm_x64_t* as, int src_i32, int dest_r32, int dest_disp)
{
    assert(0);
    asm_x64_write_byte_1(as, OPCODE_MOV_I32_TO_RM32);
    //asm_x64_write_r32_disp(as, 0, dest_r32, dest_disp);
    asm_x64_write_word32(as, src_i32);
}

void asm_x64_xor_r64_to_r64(asm_x64_t *as, int src_r64, int dest_r64) {
    asm_x64_write_byte_3(as, REX_PREFIX | REX_W, OPCODE_XOR_R64_TO_RM64, MODRM_R64(src_r64) | MODRM_RM_REG | MODRM_RM_R64(dest_r64));
}

void asm_x64_add_r64_to_r64(asm_x64_t* as, int src_r64, int dest_r64) {
    asm_x64_write_byte_3(as, REX_PREFIX | REX_W, OPCODE_ADD_R64_TO_RM64, MODRM_R64(src_r64) | MODRM_RM_REG | MODRM_RM_R64(dest_r64));
}

void asm_x64_add_i32_to_r32(asm_x64_t* as, int src_i32, int dest_r32)
{
    assert(dest_r32 != REG_RSP); // in this case i think src_i32 must be 64 bits
    if (SIGNED_FIT8(src_i32))
    {
        asm_x64_write_byte_2(as, OPCODE_ADD_I8_TO_RM32, MODRM_R64(0) | MODRM_RM_REG | MODRM_RM_R64(dest_r32));
        asm_x64_write_byte_1(as, src_i32 & 0xff);
    }
    else
    {
        asm_x64_write_byte_2(as, OPCODE_ADD_I32_TO_RM32, MODRM_R64(0) | MODRM_RM_REG | MODRM_RM_R64(dest_r32));
        asm_x64_write_word32(as, src_i32);
    }
}

void asm_x64_sub_r32_from_r32(asm_x64_t* as, int src_r32, int dest_r32) {
    // defaults to 32 bit operation
    asm_x64_write_byte_2(as, OPCODE_SUB_R64_FROM_RM64, MODRM_R64(src_r32) | MODRM_RM_REG | MODRM_RM_R64(dest_r32));
}

void asm_x64_sub_r64_from_r64(asm_x64_t* as, int src_r64, int dest_r64) {
    // use REX prefix for 64 bit operation
    asm_x64_write_byte_3(as, REX_PREFIX | REX_W, OPCODE_SUB_R64_FROM_RM64, MODRM_R64(src_r64) | MODRM_RM_REG | MODRM_RM_R64(dest_r64));
}

void asm_x64_sub_i32_from_r32(asm_x64_t* as, int src_i32, int dest_r32) {
    if (SIGNED_FIT8(src_i32)) {
        // defaults to 32 bit operation
        asm_x64_write_byte_2(as, OPCODE_SUB_I8_FROM_RM64, MODRM_R64(5) | MODRM_RM_REG | MODRM_RM_R64(dest_r32));
        asm_x64_write_byte_1(as, src_i32 & 0xff);
    } else {
        // defaults to 32 bit operation
        asm_x64_write_byte_2(as, OPCODE_SUB_I32_FROM_RM64, MODRM_R64(5) | MODRM_RM_REG | MODRM_RM_R64(dest_r32));
        asm_x64_write_word32(as, src_i32);
    }
}

void asm_x64_sub_i32_from_r64(asm_x64_t* as, int src_i32, int dest_r64) {
    if (SIGNED_FIT8(src_i32)) {
        // use REX prefix for 64 bit operation
        asm_x64_write_byte_3(as, REX_PREFIX | REX_W, OPCODE_SUB_I8_FROM_RM64, MODRM_R64(5) | MODRM_RM_REG | MODRM_RM_R64(dest_r64));
        asm_x64_write_byte_1(as, src_i32 & 0xff);
    } else {
        // use REX prefix for 64 bit operation
        asm_x64_write_byte_3(as, REX_PREFIX | REX_W, OPCODE_SUB_I32_FROM_RM64, MODRM_R64(5) | MODRM_RM_REG | MODRM_RM_R64(dest_r64));
        asm_x64_write_word32(as, src_i32);
    }
}

/* shifts not tested */
void asm_x64_shl_r32_by_imm(asm_x64_t* as, int r32, int imm) {
    asm_x64_write_byte_2(as, OPCODE_SHL_RM32_BY_I8, MODRM_R64(4) | MODRM_RM_REG | MODRM_RM_R64(r32));
    asm_x64_write_byte_1(as, imm);
}

void asm_x64_shr_r32_by_imm(asm_x64_t* as, int r32, int imm) {
    asm_x64_write_byte_2(as, OPCODE_SHR_RM32_BY_I8, MODRM_R64(5) | MODRM_RM_REG | MODRM_RM_R64(r32));
    asm_x64_write_byte_1(as, imm);
}

void asm_x64_sar_r32_by_imm(asm_x64_t* as, int r32, int imm) {
    asm_x64_write_byte_2(as, OPCODE_SAR_RM32_BY_I8, MODRM_R64(7) | MODRM_RM_REG | MODRM_RM_R64(r32));
    asm_x64_write_byte_1(as, imm);
}

void asm_x64_cmp_r64_with_r64(asm_x64_t* as, int src_r64_a, int src_r64_b) {
    asm_x64_write_byte_3(as, REX_PREFIX | REX_W, OPCODE_CMP_R64_WITH_RM64, MODRM_R64(src_r64_a) | MODRM_RM_REG | MODRM_RM_R64(src_r64_b));
}

void asm_x64_cmp_r32_with_disp(asm_x64_t* as, int src_r32_a, int src_r32_b, int src_disp_b) {
    assert(0);
    asm_x64_write_byte_1(as, OPCODE_CMP_R64_WITH_RM64);
    //asm_x64_write_r32_disp(as, src_r32_a, src_r32_b, src_disp_b);
}

void asm_x64_cmp_disp_with_r32(asm_x64_t* as, int src_r32_a, int src_disp_a, int src_r32_b) {
    assert(0);
    asm_x64_write_byte_1(as, OPCODE_CMP_RM32_WITH_R32);
    //asm_x64_write_r32_disp(as, src_r32_b, src_r32_a, src_disp_a);
}

void asm_x64_cmp_i32_with_r32(asm_x64_t* as, int src_i32, int src_r32) {
    if (SIGNED_FIT8(src_i32)) {
        asm_x64_write_byte_2(as, OPCODE_CMP_I8_WITH_RM32, MODRM_R64(7) | MODRM_RM_REG | MODRM_RM_R64(src_r32));
        asm_x64_write_byte_1(as, src_i32 & 0xff);
    } else {
        asm_x64_write_byte_2(as, OPCODE_CMP_I32_WITH_RM32, MODRM_R64(7) | MODRM_RM_REG | MODRM_RM_R64(src_r32));
        asm_x64_write_word32(as, src_i32);
    }
}

void asm_x64_test_r8_with_r8(asm_x64_t* as, int src_r64_a, int src_r64_b) {
    asm_x64_write_byte_2(as, OPCODE_TEST_R8_WITH_RM8, MODRM_R64(src_r64_a) | MODRM_RM_REG | MODRM_RM_R64(src_r64_b));
}

void asm_x64_setcc_r8(asm_x64_t* as, int jcc_type, int dest_r8) {
    asm_x64_write_byte_3(as, OPCODE_SETCC_RM8_A, OPCODE_SETCC_RM8_B | jcc_type, MODRM_R64(0) | MODRM_RM_REG | MODRM_RM_R64(dest_r8));
}

int asm_x64_label_new(asm_x64_t* as) {
    return as->next_label++;
}

void asm_x64_label_assign(asm_x64_t* as, int label) {
    if (as->pass > ASM_X64_PASS_1) {
        assert(label < as->max_num_labels);
        if (as->pass == ASM_X64_PASS_2) {
            // assign label offset
            assert(as->label_offsets[label] == -1);
            as->label_offsets[label] = as->code_offset;
        } else if (as->pass == ASM_X64_PASS_3) {
            // ensure label offset has not changed from PASS_2 to PASS_3
            //printf("l%d: (at %d=%ld)\n", label, as->label_offsets[label], as->code_offset);
            assert(as->label_offsets[label] == as->code_offset);
        }
    }
}

void asm_x64_jmp_label(asm_x64_t* as, int label) {
    if (as->pass > ASM_X64_PASS_1) {
        int dest = as->label_offsets[label];
        int rel = dest - as->code_offset;
        if (dest >= 0 && rel < 0) {
            // is a backwards jump, so we know the size of the jump on the first pass
            // calculate rel assuming 8 bit relative jump
            rel -= 2;
            if (SIGNED_FIT8(rel)) {
                asm_x64_write_byte_2(as, OPCODE_JMP_REL8, rel & 0xff);
            } else {
                rel += 2;
                goto large_jump;
            }
        } else {
            // is a forwards jump, so need to assume it's large
            large_jump:
            rel -= 5;
            asm_x64_write_byte_1(as, OPCODE_JMP_REL32);
            asm_x64_write_word32(as, rel);
        }
    }
}

void asm_x64_jcc_label(asm_x64_t* as, int jcc_type, int label) {
    if (as->pass > ASM_X64_PASS_1) {
        int dest = as->label_offsets[label];
        int rel = dest - as->code_offset;
        if (dest >= 0 && rel < 0) {
            // is a backwards jump, so we know the size of the jump on the first pass
            // calculate rel assuming 8 bit relative jump
            rel -= 2;
            if (SIGNED_FIT8(rel)) {
                asm_x64_write_byte_2(as, OPCODE_JCC_REL8 | jcc_type, rel & 0xff);
            } else {
                rel += 2;
                goto large_jump;
            }
        } else {
            // is a forwards jump, so need to assume it's large
            large_jump:
            rel -= 6;
            asm_x64_write_byte_2(as, OPCODE_JCC_REL32_A, OPCODE_JCC_REL32_B | jcc_type);
            asm_x64_write_word32(as, rel);
        }
    }
}

void asm_x64_entry(asm_x64_t* as, int num_locals) {
    asm_x64_push_r64(as, REG_RBP);
    asm_x64_mov_r64_to_r64(as, REG_RSP, REG_RBP);
    if (num_locals < 0) {
        num_locals = 0;
    }
    num_locals |= 1; // make it odd so stack is aligned on 16 byte boundary
    asm_x64_sub_i32_from_r64(as, num_locals * WORD_SIZE, REG_RSP);
    asm_x64_push_r64(as, REG_RBX);
}

void asm_x64_exit(asm_x64_t* as) {
    asm_x64_pop_r64(as, REG_RBX);
    asm_x64_write_byte_1(as, OPCODE_LEAVE);
    asm_x64_ret(as);
}

void asm_x64_push_arg(asm_x64_t* as, int src_arg_num) {
    assert(0);
    asm_x64_push_disp(as, REG_RBP, 8 + src_arg_num * WORD_SIZE);
}

void asm_x64_mov_arg_to_r32(asm_x64_t* as, int src_arg_num, int dest_r32) {
    assert(0);
    //asm_x64_mov_disp_to_r32(as, REG_RBP, 8 + src_arg_num * WORD_SIZE, dest_r32);
}

void asm_x64_mov_r32_to_arg(asm_x64_t* as, int src_r32, int dest_arg_num) {
    assert(0);
    //asm_x64_mov_r32_to_disp(as, src_r32, REG_RBP, 8 + dest_arg_num * WORD_SIZE);
}

static int asm_x64_local_offset_from_ebp(int local_num)
{
    return -(local_num + 1) * WORD_SIZE;
}

void asm_x64_mov_local_to_r64(asm_x64_t* as, int src_local_num, int dest_r64) {
    asm_x64_mov_disp_to_r64(as, REG_RBP, asm_x64_local_offset_from_ebp(src_local_num), dest_r64);
}

void asm_x64_mov_r64_to_local(asm_x64_t* as, int src_r64, int dest_local_num) {
    asm_x64_mov_r64_to_disp(as, src_r64, REG_RBP, asm_x64_local_offset_from_ebp(dest_local_num));
}

void asm_x64_mov_local_addr_to_r64(asm_x64_t* as, int local_num, int dest_r64) {
    int offset = asm_x64_local_offset_from_ebp(local_num);
    if (offset == 0) {
        asm_x64_mov_r64_to_r64(as, REG_RBP, dest_r64);
    } else {
        asm_x64_lea_disp_to_r64(as, REG_RBP, offset, dest_r64);
    }
}

void asm_x64_push_local(asm_x64_t* as, int local_num) {
    asm_x64_push_disp(as, REG_RBP, asm_x64_local_offset_from_ebp(local_num));
}

void asm_x64_push_local_addr(asm_x64_t* as, int local_num, int temp_r64)
{
    asm_x64_mov_r64_to_r64(as, REG_RBP, temp_r64);
    asm_x64_add_i32_to_r32(as, asm_x64_local_offset_from_ebp(local_num), temp_r64);
    asm_x64_push_r64(as, temp_r64);
}

/*
   can't use these because code might be relocated when resized

void asm_x64_call(asm_x64_t* as, void* func)
{
    asm_x64_sub_i32_from_r32(as, 8, REG_RSP);
    asm_x64_write_byte_1(as, OPCODE_CALL_REL32);
    asm_x64_write_word32(as, func - (void*)(as->code_cur + 4));
    asm_x64_mov_r64_to_r64(as, REG_RBP, REG_RSP);
}

void asm_x64_call_i1(asm_x64_t* as, void* func, int i1)
{
    asm_x64_sub_i32_from_r32(as, 8, REG_RSP);
    asm_x64_sub_i32_from_r32(as, 12, REG_RSP);
    asm_x64_push_i32(as, i1);
    asm_x64_write_byte_1(as, OPCODE_CALL_REL32);
    asm_x64_write_word32(as, func - (void*)(as->code_cur + 4));
    asm_x64_add_i32_to_r32(as, 16, REG_RSP);
    asm_x64_mov_r64_to_r64(as, REG_RBP, REG_RSP);
}
*/

void asm_x64_call_ind(asm_x64_t* as, void *ptr, int temp_r64) {
    /*
    asm_x64_mov_i64_to_r64_optimised(as, (int64_t)ptr, temp_r64);
    asm_x64_write_byte_2(as, OPCODE_CALL_RM32, MODRM_R64(2) | MODRM_RM_REG | MODRM_RM_R64(temp_r64));
    */
    // this reduces code size by 2 bytes per call, but doesn't seem to speed it up at all
    asm_x64_write_byte_1(as, OPCODE_CALL_REL32);
    asm_x64_write_word32(as, ptr - (void*)(as->code_base + as->code_offset + 4));
}
