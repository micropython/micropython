// Simplified dual-mode module support
// Uses conditional compilation to handle the differences

#ifndef MP_DUALMODE_V2_H
#define MP_DUALMODE_V2_H

#ifdef MICROPY_ENABLE_DYNRUNTIME
// Native module build
#include "py/dynruntime.h"
#else
// User C module build  
#include "py/runtime.h"
#include "py/obj.h"
#endif

// Helper to ensure QSTRs are referenced in native builds
#ifdef MICROPY_ENABLE_DYNRUNTIME
#define DUALMODE_QSTR_REF(name) static __attribute__((used)) void _ref_##name(void) { (void)MP_QSTR_##name; }
#else
#define DUALMODE_QSTR_REF(name) /* no-op for user modules */
#endif

#endif // MP_DUALMODE_V2_H