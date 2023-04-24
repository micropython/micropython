// micropython includes
#include "py/gc.h"
#include "shared/runtime/gchelper.h"


#if MICROPY_ENABLE_GC

void gc_collect(void) {
    gc_collect_start();
    gc_helper_collect_regs_and_stack();
    #if MICROPY_PY_THREAD
    mp_thread_gc_others();
    #endif
    gc_collect_end();


    #if MICROPY_LOGGER_DEBUG

    gc_dump_info();

    #endif
}

#endif
