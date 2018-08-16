// thumb specific stuff

#include "py/mpconfig.h"

#if MICROPY_EMIT_THUMB

// this is defined so that the assembler exports generic assembler API macros
#define GENERIC_ASM_API (1)
#include "py/asmthumb.h"

// Word index of REG_LOCAL_1(=r4) in nlr_buf_t
#define NLR_BUF_IDX_LOCAL_1 (3)

#define N_THUMB (1)
#define EXPORT_FUN(name) emit_native_thumb_##name
#include "py/emitnative.c"

#endif
