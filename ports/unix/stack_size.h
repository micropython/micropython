#ifndef MICROPY_INCLUDED_UNIX_STACK_SIZE_H
#define MICROPY_INCLUDED_UNIX_STACK_SIZE_H

#include "py/misc.h"

// Define scaling factors for the stack size (also applies to main thread)
#ifndef UNIX_STACK_MULTIPLIER

#if defined(__arm__) && !defined(__thumb2__)
// ARM (non-Thumb) architectures require more stack.
#define UNIX_STACK_MUL_ARM 2
#else
#define UNIX_STACK_MUL_ARM 1
#endif

#if MP_SANITIZER_BUILD
// Sanitizer features consume significant stack in some cases
#define UNIX_STACK_MUL_SANITIZERS (1.5)
#else
#define UNIX_STACK_MUL_SANITIZERS 1
#endif

// Double the stack size for 64-bit builds, plus additional scaling
#define UNIX_STACK_MULTIPLIER ((sizeof(void *) / 4) * UNIX_STACK_MUL_ARM * UNIX_STACK_MUL_SANITIZERS)

#endif // UNIX_STACK_MULTIPLIER

#endif // MICROPY_INCLUDED_UNIX_STACK_SIZE_H
