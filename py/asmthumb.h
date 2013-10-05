#define ASM_THUMB_PASS_1 (1)
#define ASM_THUMB_PASS_2 (2)
#define ASM_THUMB_PASS_3 (3)

#define REG_R0  (0)
#define REG_R1  (1)
#define REG_R2  (2)
#define REG_R3  (3)
#define REG_R4  (4)
#define REG_R5  (5)
#define REG_R6  (6)
#define REG_R7  (7)
#define REG_R8  (8)
#define REG_R9  (9)
#define REG_R10 (10)
#define REG_R11 (11)
#define REG_R12 (12)
#define REG_R13 (13)
#define REG_R14 (14)
#define REG_R15 (15)
#define REG_LR  (REG_R14)

#define REG_RET REG_R0
#define REG_ARG_1 REG_R0
#define REG_ARG_2 REG_R1
#define REG_ARG_3 REG_R2
#define REG_ARG_4 REG_R3

typedef struct _asm_thumb_t asm_thumb_t;

asm_thumb_t *asm_thumb_new(uint max_num_labels);
void asm_thumb_free(asm_thumb_t *as, bool free_code);
void asm_thumb_start_pass(asm_thumb_t *as, int pass);
void asm_thumb_end_pass(asm_thumb_t *as);
uint asm_thumb_get_code_size(asm_thumb_t *as);
void *asm_thumb_get_code(asm_thumb_t *as);

void asm_thumb_entry(asm_thumb_t *as, int num_locals);
void asm_thumb_exit(asm_thumb_t *as);

void asm_thumb_label_assign(asm_thumb_t *as, int label);

// argument order follows ARM, in general dest is first
// note there is a difference between movw and mov.w, and many others!

void asm_thumb_movs_rlo_i8(asm_thumb_t *as, uint rlo_dest, int i8_src);
void asm_thumb_movw_reg_i16(asm_thumb_t *as, uint reg_dest, int i16_src);
void asm_thumb_movt_reg_i16(asm_thumb_t *as, uint reg_dest, int i16_src);
void asm_thumb_mov_reg_reg(asm_thumb_t *as, uint reg_dest, uint reg_src);
void asm_thumb_subs_rlo_rlo_i3(asm_thumb_t *as, uint rlo_dest, uint rlo_src, int i3_src);
void asm_thumb_cmp_rlo_i8(asm_thumb_t *as, uint rlo, int i8);
void asm_thumb_b_n(asm_thumb_t *as, int label);
void asm_thumb_bgt_n(asm_thumb_t *as, int label);

void asm_thumb_mov_reg_i32(asm_thumb_t *as, uint reg_dest, machine_uint_t i32_src); // convenience
void asm_thumb_mov_reg_i32_optimised(asm_thumb_t *as, uint reg_dest, int i32_src); // convenience
void asm_thumb_mov_local_reg(asm_thumb_t *as, int local_num_dest, uint rlo_src); // convenience
void asm_thumb_mov_reg_local(asm_thumb_t *as, uint rlo_dest, int local_num); // convenience
void asm_thumb_mov_reg_local_addr(asm_thumb_t *as, uint reg_dest, int local_num); // convenience

void asm_thumb_add_reg_reg_reg(asm_thumb_t *as, uint rlo_dest, uint rlo_src_a, uint rlo_src_b); // convenience ?
void asm_thumb_cmp_reg_reg(asm_thumb_t *as, uint rlo_a, uint rlo_b); // convenience ?
void asm_thumb_ite_ge(asm_thumb_t *as); // convenience ?

void asm_thumb_b_label(asm_thumb_t *as, int label); // convenience ?
void asm_thumb_cmp_reg_bz_label(asm_thumb_t *as, uint rlo, int label); // convenience ?
void asm_thumb_bl_ind(asm_thumb_t *as, void *fun_ptr, uint fun_id, uint reg_temp); // convenience ?

