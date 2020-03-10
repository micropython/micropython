#ifndef MICROPY_INCLUDED_SHARED_MODULE_PROTOMATTER_ALLOCATOR_H
#define MICROPY_INCLUDED_SHARED_MODULE_PROTOMATTER_ALLOCATOR_H

#include <stdbool.h>
#include "py/misc.h"
#include "supervisor/memory.h"

#define _PM_ALLOCATOR _PM_allocator_impl
#define _PM_FREE _PM_free_impl

static inline void *_PM_allocator_impl(size_t sz) {
    supervisor_allocation *allocation = allocate_memory(align32_size(sz), true);    
    if (allocation) {
        return allocation->ptr;
    } else {
        return m_malloc(sz + sizeof(void*), true);
    }
}

static inline void _PM_free_impl(void *ptr_in) {
    supervisor_allocation *allocation = allocation_from_ptr(ptr_in);
    
    if (allocation) {
        free_memory(allocation);
    } else {
        m_free(ptr_in);
    }
}

#endif
