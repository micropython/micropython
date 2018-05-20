// ARM specific stuff

#include "py/mpconfig.h"

#if MICROPY_EMIT_ARM

// This is defined so that the assembler exports generic assembler API macros
#define GENERIC_ASM_API (1)
#include "py/asmarm.h"

#define N_ARM (1)
#define EXPORT_FUN(name) emit_native_arm_##name
#include "py/emitnative.c"

#endif
