// AArch64 specific stuff

#include "py/mpconfig.h"

#if MICROPY_EMIT_AARCH64

// This is defined so that the assembler exports generic assembler API macros
#define GENERIC_ASM_API (1)
#include "py/asmaarch64.h"

// Word indices of REG_LOCAL_x in nlr_buf_t
#define NLR_BUF_IDX_LOCAL_1 (4) // x19, at nlr_buf_t.regs[2] (after lr, sp)

#define N_AARCH64 (1)
#define EXPORT_FUN(name) emit_native_aarch64_##name
#include "py/emitnative.c"

#endif
