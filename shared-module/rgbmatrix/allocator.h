#ifndef MICROPY_INCLUDED_SHARED_MODULE_RGBMATRIX_ALLOCATOR_H
#define MICROPY_INCLUDED_SHARED_MODULE_RGBMATRIX_ALLOCATOR_H

#include <stdbool.h>
#include "py/gc.h"
#include "py/misc.h"
#include "supervisor/memory.h"

#define _PM_ALLOCATOR _PM_allocator_impl
#define _PM_FREE(x) (_PM_free_impl((x)), (x)=NULL, (void)0)

static inline void *_PM_allocator_impl(size_t sz) {
    if (gc_alloc_possible()) {
        return m_malloc(sz + sizeof(void*), true);
    } else {
        supervisor_allocation *allocation = allocate_memory(align32_size(sz), false);
        return allocation ? allocation->ptr : NULL;
    }
}

static inline void _PM_free_impl(void *ptr_in) {
    supervisor_allocation *allocation = allocation_from_ptr(ptr_in);

    if (allocation) {
        free_memory(allocation);
    }
}

#endif
