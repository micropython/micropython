// thumb specific stuff

#include "py/mpconfig.h"

#if MICROPY_EMIT_THUMB

// this is defined so that the assembler exports generic assembler API macros
#define GENERIC_ASM_API (1)
#include "py/asmthumb.h"

#define N_THUMB (1)
#define EXPORT_FUN(name) emit_native_thumb_##name
#include "py/emitnative.c"

#endif
