// x64 specific stuff

#include "py/mpconfig.h"

#if MICROPY_EMIT_X64

// This is defined so that the assembler exports generic assembler API macros
#define GENERIC_ASM_API (1)
#include "py/asmx64.h"

// Word indices of REG_LOCAL_x in nlr_buf_t
#define NLR_BUF_IDX_LOCAL_1 (5) // rbx

#define N_X64 (1)
#define EXPORT_FUN(name) emit_native_x64_##name
#include "py/emitnative.c"

#endif
