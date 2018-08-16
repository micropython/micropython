// Xtensa specific stuff

#include "py/mpconfig.h"

#if MICROPY_EMIT_XTENSA

// this is defined so that the assembler exports generic assembler API macros
#define GENERIC_ASM_API (1)
#include "py/asmxtensa.h"

// Word index of REG_LOCAL_1(=a12) in nlr_buf_t
#define NLR_BUF_IDX_LOCAL_1 (8)

#define N_XTENSA (1)
#define EXPORT_FUN(name) emit_native_xtensa_##name
#include "py/emitnative.c"

#endif
