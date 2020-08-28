#pragma once

#include <stdbool.h>
#include "py/gc.h"
#include "py/misc.h"
#include "supervisor/memory.h"

#define _PM_ALLOCATOR common_hal_rgbmatrix_allocator_impl
#define _PM_FREE(x) (common_hal_rgbmatrix_free_impl((x)), (x)=NULL, (void)0)
extern void *common_hal_rgbmatrix_allocator_impl(size_t sz);
extern void common_hal_rgbmatrix_free_impl(void *);
