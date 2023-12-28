// Xtensa-Windowed specific stuff

#include "py/mpconfig.h"

#if MICROPY_EMIT_XTENSAWIN

// this is defined so that the assembler exports generic assembler API macros
#define GENERIC_ASM_API (1)
#define GENERIC_ASM_API_WIN (1)
#include "py/asmxtensa.h"

// Word indices of REG_LOCAL_x in nlr_buf_t
#define NLR_BUF_IDX_LOCAL_1 (2 + 4) // a4

#define N_NLR_SETJMP (1)
#define N_XTENSAWIN (1)
#define EXPORT_FUN(name) emit_native_xtensawin_##name
#include "py/emitnative.c"

#endif
