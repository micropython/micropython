#include "../standard/mpconfigvariant.h"

#define MICROPY_NLR_SETJMP (1)

// This value is appropriate for x86_64 glibc on linux but is likely incorrect
// on other systems.
#ifdef __linux__
#include <features.h>
#if defined(__GLIBC_PREREQ) && __GLIBC_PREREQ(2, 0)
#define NLR_BUF_IDX_LOCAL_1 (2 + 0) // rbx in glibc 2.41 jmpbuf-offsets.h
#endif
#endif

#ifndef NLR_BUF_IDX_LOCAL_1
#error Need definition of NLR_BUF_IDX_LOCAL_1 for this environment
#endif
