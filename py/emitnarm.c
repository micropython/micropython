// ARM specific stuff

#include "py/mpconfig.h"

#if MICROPY_EMIT_ARM

// This is defined so that the assembler exports generic assembler API macros
#define GENERIC_ASM_API (1)
#include "py/asmarm.h"

// Word index of REG_LOCAL_1(=r4) in nlr_buf_t
#define NLR_BUF_IDX_LOCAL_1 (3)

#define N_ARM (1)
#define EXPORT_FUN(name) emit_native_arm_##name
#include "py/emitnative.c"

#endif
