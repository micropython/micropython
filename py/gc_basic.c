/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "py/mpstate.h"
#include "py/gc.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/heap_basic.h"

#if MICROPY_ENABLE_GC && MICROPY_GC == MICROPY_GC_BASIC

#if 0 // print debugging info
#define DEBUG_PRINT (1)
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_PRINT (0)
#define DEBUG_printf(...) (void)0
#endif

// TODO waste less memory; currently requires that all entries in alloc_table have a corresponding block in pool
void gc_init(void* start, void* end) {
    // initialize the underlying memory manager
    heap_init(start, end);

}

void gc_lock(void) {
    MP_STATE_MEM(gc_lock_depth)++;
}

void gc_unlock(void) {
    MP_STATE_MEM(gc_lock_depth)--;
}

bool gc_is_locked(void) {
    return MP_STATE_MEM(gc_lock_depth) != 0;
}


#define VERIFY_MARK_AND_PUSH(ptr) \
    if(heap_valid(block)){ \
        if(!heap_get_mark(block)){ \
            heap_set_mark(block); \
            if (MP_STATE_MEM(gc_sp) < &MP_STATE_MEM(gc_stack)[MICROPY_ALLOC_GC_STACK_SIZE]) { \
                *MP_STATE_MEM(gc_sp)++ = block; \
            } else { \
                MP_STATE_MEM(gc_stack_overflow) = 1; \
            } \
        } \
    }

STATIC void gc_drain_stack(void) {
    while (MP_STATE_MEM(gc_sp) > MP_STATE_MEM(gc_stack)) {
        // pop the next block off the stack
        mp_uint_t block = *--MP_STATE_MEM(gc_sp);
        mp_uint_t size_ints = heap_sizeof(block) / sizeof(mp_uint_t);

        /*    // check this block's children*/
        mp_uint_t* scan = (mp_uint_t*)heap_void_p(block);
        for (mp_uint_t i = size_ints; i > 0; i--, scan++) {
            block = heap_block((void*) *scan);
            VERIFY_MARK_AND_PUSH(block);
        }
    }
}

STATIC void gc_deal_with_stack_overflow(void) {
    while (MP_STATE_MEM(gc_stack_overflow)) {
        MP_STATE_MEM(gc_stack_overflow) = 0;
        MP_STATE_MEM(gc_sp) = MP_STATE_MEM(gc_stack);

        // scan entire memory looking for blocks which have been marked but not their children
        for (mp_uint_t block = heap_first(); block != MEM_BLOCK_ERROR; block = heap_next(block)) {
            // trace (again) if mark bit set
            *MP_STATE_MEM(gc_sp)++ = block;
            gc_drain_stack();
        }
    }
}

STATIC void gc_sweep(void) {
    #if MICROPY_PY_GC_COLLECT_RETVAL
    MP_STATE_MEM(gc_collected) = 0;
    #endif
    // free unmarked heads and their tails
    for (mp_uint_t block = heap_first(); block != MEM_BLOCK_ERROR; block = heap_next(block)) {
        if (heap_get_mark(block)) {
            heap_clear_mark(block);
        } else {
            #if MICROPY_ENABLE_FINALISER        // python __del__ method
            if (heap_finalizer_get(block)) {
                mp_obj_t obj = (mp_obj_t)heap_void_p(block);
                if (((mp_obj_base_t*)obj)->type != MP_OBJ_NULL) {
                    // if the object has a type then see if it has a __del__ method
                    mp_obj_t dest[2];
                    mp_load_method_maybe(obj, MP_QSTR___del__, dest);
                    if (dest[0] != MP_OBJ_NULL) {
                        // load_method returned a method
                        mp_call_method_n_kw(0, 0, dest);
                    }
                }
                // clear finaliser flag
                heap_finalizer_clear(block);
            }
            #endif
            #if MICROPY_PY_GC_COLLECT_RETVAL
            MP_STATE_MEM(gc_collected)++;
            #endif
            assert(!heap_get_mark(block));
            assert(!heap_get_mark(heap_block(heap_void_p(block))));
            heap_free(block);  // don't use gc free, as it clears the gc_collected
        }
    }
}

void gc_collect_start(void) {
    gc_lock();
    MP_STATE_MEM(gc_stack_overflow) = 0;
    MP_STATE_MEM(gc_sp) = MP_STATE_MEM(gc_stack);
    // Trace root pointers.  This relies on the root pointers being organised
    // correctly in the mp_state_ctx structure.  We scan nlr_top, dict_locals,
    // dict_globals, then the root pointer section of mp_state_vm.
    void** ptrs = (void**)(void*)&mp_state_ctx;
    gc_collect_root(ptrs, offsetof(mp_state_ctx_t, vm.stack_top) / sizeof(mp_uint_t));
}

void gc_collect_root(void** ptrs, mp_uint_t len) {
    for (mp_uint_t i = 0; i < len; i++) {
        mp_uint_t block = heap_block(ptrs[i]);
        VERIFY_MARK_AND_PUSH(block);
        gc_drain_stack();
    }
}

void gc_collect_end(void) {
    gc_deal_with_stack_overflow();
    gc_sweep();
    gc_unlock();
}

void gc_info(gc_info_t* info) {
    heap_info((heap_info_t*)info);
}

void* gc_alloc(mp_uint_t n_bytes, bool has_finaliser) {
    if (MP_STATE_MEM(gc_lock_depth) > 0) {
        return NULL;
    }
    if (!n_bytes) {
        return NULL;
    }
    mp_uint_t block = heap_alloc(n_bytes);
    if (block == MEM_BLOCK_ERROR) {
        if (!MP_STATE_MEM(gc_auto_collect_enabled)) {
            return NULL;
        }
        gc_collect();
        block = heap_alloc(n_bytes);
    }
    if (block == MEM_BLOCK_ERROR) {
        return NULL;   // failed after garbage collect
    }

    // zero out the additional bytes of the newly allocated blocks
    // This is needed because the blocks may have previously held pointers
    // to the heap and will not be set to something else if the caller
    // doesn't actually use the entire block.  As such they will continue
    // to point to the heap and may prevent other blocks from being reclaimed.
    memset((byte*)heap_void_p(block) + n_bytes, 0, heap_sizeof(block) - n_bytes);
    #if MICROPY_ENABLE_FINALISER
    if (has_finaliser) {
        // clear type pointer in case it is never set
        ((mp_obj_base_t*)heap_void_p(block))->type = MP_OBJ_NULL;
        // set mp_obj flag only if it has a finaliser
        heap_finalizer_set(block);
    }
    #endif

    #if EXTENSIVE_HEAP_PROFILING
    gc_dump_alloc_table();
    #endif
    return heap_void_p(block);
}


// force the freeing of a piece of memory
void gc_free(void* ptr_in) {
    if (MP_STATE_MEM(gc_lock_depth) > 0) {
        // TODO how to deal with this error?
        return;
    }

    /*mp_uint_t ptr = (mp_uint_t)ptr_in;*/
    /*DEBUG_printf("gc_free(%p)\n", ptr);*/

    mp_uint_t block = heap_block(ptr_in);
    if (heap_valid(block)) {
        #if EXTENSIVE_HEAP_PROFILING
        gc_dump_alloc_table();
        #endif
        heap_free(block);
    } else {
        /*assert(!"bad free");*/
    }
}

mp_uint_t gc_nbytes(const void* ptr_in) {
    return heap_sizeof(heap_block(ptr_in));
}

void* gc_realloc(void* ptr_in, mp_uint_t n_bytes, bool allow_move) {
    if (MP_STATE_MEM(gc_lock_depth) > 0) {
        return NULL;
    }

    // check for pure allocation
    if (ptr_in == NULL) {
        return gc_alloc(n_bytes, false);
    }

    // check for pure free
    if (n_bytes == 0) {
        gc_free(ptr_in);
        return NULL;
    }
    mp_uint_t block_in = heap_block(ptr_in);

    if (!heap_valid(block_in)) {
        return NULL;
    }
    #if MICROPY_ENABLE_FINALISER
    int8_t has_finaliser = heap_finalizer_get(block_in);
    #endif
    mp_uint_t original_bytes = heap_sizeof(block_in);
    mp_uint_t block = heap_realloc(block_in, n_bytes, allow_move);
    if (block == MEM_BLOCK_ERROR) { // failed to allocate, no change in ptr
        if (n_bytes <= original_bytes) {
            return NULL;   // garbage collection will not solve issue
        }
        if (!MP_STATE_MEM(gc_auto_collect_enabled)) {
            return NULL;
        }
        gc_collect();
        block = heap_realloc(block_in, n_bytes, allow_move);
    }
    if (block == MEM_BLOCK_ERROR) {
        return NULL;   // failed after garbage collect
    }

    if (heap_sizeof(block) == original_bytes) { // block stayed the same size
        assert(heap_void_p(block) == ptr_in);
        return ptr_in;
    }
    if (heap_sizeof(block) < original_bytes) { // block shrank
        assert(heap_void_p(block) == ptr_in);
        #if EXTENSIVE_HEAP_PROFILING
        gc_dump_alloc_table();
        #endif
        return ptr_in;
    }
    // block grew
    // zero out the additional bytes of the newly allocated blocks (see comment above in gc_alloc)
    memset((byte*)heap_void_p(block) + n_bytes, 0, heap_sizeof(block) - n_bytes);
    if (block_in == block) { // grew in place
        #if EXTENSIVE_HEAP_PROFILING
        gc_dump_alloc_table();
        #endif
    } else {  // data moved, deal with finalizer
        #if EXTENSIVE_HEAP_PROFILING
        gc_dump_alloc_table(); // dump because a pointer has been freed
        #endif
        #if MICROPY_ENABLE_FINALISER
        if (has_finaliser) { // deal with finalsier
            // clear type pointer in case it is never set
            ((mp_obj_base_t*)heap_void_p(block))->type = MP_OBJ_NULL;
            // set mp_obj flag only if it has a finaliser
            heap_finalizer_set(block);
        }
        #endif
    }
    return heap_void_p(block);
}

void gc_dump_info(void) {
    gc_info_t info;
    gc_info(&info);
    mp_printf(&mp_plat_print, "GC: total: " UINT_FMT ", used: " UINT_FMT ", free: " UINT_FMT "\n",
              info.total, info.used, info.free);
    mp_printf(&mp_plat_print, " No. of 1-blocks: " UINT_FMT ", 2-blocks: " UINT_FMT ", max blk sz: " UINT_FMT "\n",
              info.num_1block, info.num_2block, info.max_block);
}

void gc_dump_alloc_table(void) {
    heap_dump_alloc_table();
}

#if DEBUG_PRINT
void gc_test(void) {
    mp_uint_t len = 500;
    mp_uint_t* heap = malloc(len);
    gc_init(heap, heap + len / sizeof(mp_uint_t));
    void* ptrs[100];
    {
        mp_uint_t** p = gc_alloc(16, false);
        p[0] = gc_alloc(64, false);
        p[1] = gc_alloc(1, false);
        p[2] = gc_alloc(1, false);
        p[3] = gc_alloc(1, false);
        mp_uint_t** *p2 = gc_alloc(16, false);
        p2[0] = p;
        p2[1] = p;
        ptrs[0] = p2;
    }
    for (int i = 0; i < 25; i += 2) {
        mp_uint_t* p = gc_alloc(i, false);
        printf("p=%p\n", p);
        if (i & 3) {
            //ptrs[i] = p;
        }
    }

    printf("Before GC:\n");
    gc_dump_alloc_table();
    printf("Starting GC...\n");
    gc_collect_start();
    gc_collect_root(ptrs, sizeof(ptrs) / sizeof(void*));
    gc_collect_end();
    printf("After GC:\n");
    gc_dump_alloc_table();
}
#endif

#endif // MICROPY_ENABLE_GC
