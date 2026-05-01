// Xtensa specific stuff

#include "py/mpconfig.h"

#if MICROPY_EMIT_XTENSA

// this is defined so that the assembler exports generic assembler API macros
#define GENERIC_ASM_API (1)
#include "py/asmxtensa.h"

#if defined(__xtensa__)
#if MICROPY_NLR_SETJMP
#define N_NLR_SETJMP (1)
#else
#define N_NLR_SETJMP (0)
#endif
#endif

#if !defined(NLR_BUF_IDX_LOCAL_1)
#if N_NLR_SETJMP
#error Must define NLR_BUF_IDX_LOCAL_1 (depends on layout of jmp_buf)
#endif
// Word indices of REG_LOCAL_x in nlr_buf_t
#define NLR_BUF_IDX_LOCAL_1 (8) // a12
#endif

#define N_XTENSA (1)
#define EXPORT_FUN(name) emit_native_xtensa_##name
#include "py/emitnative.c"

#endif
