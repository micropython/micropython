// x64 specific stuff

#include "py/mpconfig.h"

#if MICROPY_EMIT_X64

// This is defined so that the assembler exports generic assembler API macros
#define GENERIC_ASM_API (1)
#include "py/asmx64.h"

#if defined(__x86_64__)
#if MICROPY_NLR_SETJMP
#define N_NLR_SETJMP (1)
#else
#define N_NLR_SETJMP (0)
#endif
#endif

// Word indices of REG_LOCAL_x in nlr_buf_t
#if !defined(NLR_BUF_IDX_LOCAL_1)
#if N_NLR_SETJMP
#error Must define NLR_BUF_IDX_LOCAL_1 (depends on layout of jmp_buf)
#endif
#define NLR_BUF_IDX_LOCAL_1 (5) // rbx
#endif

#define N_X64 (1)
#define EXPORT_FUN(name) emit_native_x64_##name
#include "py/emitnative.c"

#endif
