#pragma once

#include "py/gc.h"

#if !MICROPY_GC_CONSERVATIVE_CLEAR
// so that we can implement calloc as m_malloc
#error Requires MICROPY_GC_CONSERVATIVE_CLEAR
#endif

#define QUIRC_MALLOC(x) gc_alloc((x), 0, false)
#define QUIRC_CALLOC(x,y) gc_alloc((x) * (y), 0, false)
#define QUIRC_FREE(x) gc_free((x))

#define QUIRC_SMALL_STACK (1)
