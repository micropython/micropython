/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
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
#include <string.h>
#include <stdio.h>

#include "py/gc.h"
#include "py/mpstate.h"
#include "py/qstr.h"
#include "py/gc.h"
#include "py/runtime.h"

#include "supervisor/linker.h"

// NOTE: we are using linear arrays to store and search for qstr's (unique strings, interned strings)
// ultimately we will replace this with a static hash table of some kind

#if MICROPY_DEBUG_VERBOSE // print debugging info
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#endif

// A qstr is an index into the qstr pool.
// The data for a qstr is \0 terminated (so they can be printed using printf)

#define Q_HASH_MASK ((1 << (8 * MICROPY_QSTR_BYTES_IN_HASH)) - 1)

#if MICROPY_PY_THREAD && !MICROPY_PY_THREAD_GIL
#define QSTR_ENTER() mp_thread_mutex_lock(&MP_STATE_VM(qstr_mutex), 1)
#define QSTR_EXIT() mp_thread_mutex_unlock(&MP_STATE_VM(qstr_mutex))
#else
#define QSTR_ENTER()
#define QSTR_EXIT()
#endif

// Initial number of entries for qstr pool, set so that the first dynamically
// allocated pool is twice this size.  The value here must be <= MP_QSTRnumber_of.
#define MICROPY_ALLOC_QSTR_ENTRIES_INIT (10)

// this must match the equivalent function in makeqstrdata.py
mp_uint_t qstr_compute_hash(const byte *data, size_t len) {
    // djb2 algorithm; see http://www.cse.yorku.ca/~oz/hash.html
    mp_uint_t hash = 5381;
    for (const byte *top = data + len; data < top; data++) {
        hash = ((hash << 5) + hash) ^ (*data); // hash * 33 ^ data
    }
    hash &= Q_HASH_MASK;
    // Make sure that valid hash is never zero, zero means "hash not computed"
    if (hash == 0) {
        hash++;
    }
    return hash;
}

const qstr_attr_t mp_qstr_const_attr[] = {
    #ifndef NO_QSTR
#define QDEF(id, hash, len, str) { hash, len },
#define TRANSLATION(id, length, compressed ...)
    #include "genhdr/qstrdefs.generated.h"
#undef TRANSLATION
#undef QDEF
    #endif
};

const qstr_pool_t mp_qstr_const_pool = {
    NULL,               // no previous pool
    0,                  // no previous pool
    MICROPY_ALLOC_QSTR_ENTRIES_INIT,
    MP_QSTRnumber_of,   // corresponds to number of strings in array just below
    (qstr_attr_t *)mp_qstr_const_attr,
    {
        #ifndef NO_QSTR
#define QDEF(id, hash, len, str) str,
#define TRANSLATION(id, length, compressed ...)
        #include "genhdr/qstrdefs.generated.h"
#undef TRANSLATION
#undef QDEF
        #endif
    },
};

#ifdef MICROPY_QSTR_EXTRA_POOL
extern const qstr_pool_t MICROPY_QSTR_EXTRA_POOL;
#define CONST_POOL MICROPY_QSTR_EXTRA_POOL
#else
#define CONST_POOL mp_qstr_const_pool
#endif

void qstr_init(void) {
    MP_STATE_VM(last_pool) = (qstr_pool_t *)&CONST_POOL; // we won't modify the const_pool since it has no allocated room left
    MP_STATE_VM(qstr_last_chunk) = NULL;

    #if MICROPY_PY_THREAD && !MICROPY_PY_THREAD_GIL
    mp_thread_mutex_init(&MP_STATE_VM(qstr_mutex));
    #endif
}

STATIC const char *find_qstr(qstr q, qstr_attr_t *attr) {
    // search pool for this qstr
    // total_prev_len==0 in the final pool, so the loop will always terminate
    const qstr_pool_t *pool = MP_STATE_VM(last_pool);
    while (q < pool->total_prev_len) {
        pool = pool->prev;
    }
    q -= pool->total_prev_len;
    assert(q < pool->len);
    *attr = pool->attrs[q];
    return pool->qstrs[q];
}

// qstr_mutex must be taken while in this function
STATIC qstr qstr_add(mp_uint_t hash, mp_uint_t len, const char *q_ptr) {
    DEBUG_printf("QSTR: add hash=%d len=%d data=%.*s\n", hash, len, len, q_ptr);

    // make sure we have room in the pool for a new qstr
    if (MP_STATE_VM(last_pool)->len >= MP_STATE_VM(last_pool)->alloc) {
        uint32_t new_pool_length = MP_STATE_VM(last_pool)->alloc * 2;
        if (new_pool_length > MICROPY_QSTR_POOL_MAX_ENTRIES) {
            new_pool_length = MICROPY_QSTR_POOL_MAX_ENTRIES;
        }
        #ifdef MICROPY_QSTR_EXTRA_POOL
        // Put a lower bound on the allocation size in case the extra qstr pool has few entries
        if (new_pool_length < MICROPY_ALLOC_QSTR_ENTRIES_INIT) {
            new_pool_length = MICROPY_ALLOC_QSTR_ENTRIES_INIT;
        }
        #endif
        mp_uint_t pool_size = sizeof(qstr_pool_t)
            + (sizeof(const char *) + sizeof(qstr_attr_t)) * new_pool_length;
        qstr_pool_t *pool = (qstr_pool_t *)m_malloc_maybe(pool_size, true);
        if (pool == NULL) {
            QSTR_EXIT();
            m_malloc_fail(new_pool_length);
        }
        pool->attrs = (qstr_attr_t *)(pool->qstrs + new_pool_length);
        pool->prev = MP_STATE_VM(last_pool);
        pool->total_prev_len = MP_STATE_VM(last_pool)->total_prev_len + MP_STATE_VM(last_pool)->len;
        pool->alloc = new_pool_length;
        pool->len = 0;
        MP_STATE_VM(last_pool) = pool;
        DEBUG_printf("QSTR: allocate new pool of size %d\n", MP_STATE_VM(last_pool)->alloc);
    }

    // add the new qstr
    mp_uint_t at = MP_STATE_VM(last_pool)->len;
    MP_STATE_VM(last_pool)->attrs[at].hash = hash;
    MP_STATE_VM(last_pool)->attrs[at].len = len;
    MP_STATE_VM(last_pool)->qstrs[at] = q_ptr;
    MP_STATE_VM(last_pool)->len++;

    // return id for the newly-added qstr
    return MP_STATE_VM(last_pool)->total_prev_len + at;
}

qstr qstr_find_strn(const char *str, size_t str_len) {
    // work out hash of str
    mp_uint_t str_hash = qstr_compute_hash((const byte *)str, str_len);

    // search pools for the data
    for (const qstr_pool_t *pool = MP_STATE_VM(last_pool); pool != NULL; pool = pool->prev) {
        qstr_attr_t *attrs = pool->attrs;
        for (mp_uint_t at = 0, top = pool->len; at < top; at++) {
            if (attrs[at].hash == str_hash && attrs[at].len == str_len && memcmp(pool->qstrs[at], str, str_len) == 0) {
                return pool->total_prev_len + at;
            }
        }
    }

    // not found; return null qstr
    return 0;
}

qstr qstr_from_str(const char *str) {
    return qstr_from_strn(str, strlen(str));
}

qstr qstr_from_strn(const char *str, size_t len) {
    QSTR_ENTER();
    qstr q = qstr_find_strn(str, len);
    if (q == 0) {
        // qstr does not exist in interned pool so need to add it

        // check that len is not too big
        if (len >= (1 << (8 * MICROPY_QSTR_BYTES_IN_LEN))) {
            QSTR_EXIT();
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Name too long"));
        }

        // compute number of bytes needed to intern this string
        size_t n_bytes = len + 1;

        if (MP_STATE_VM(qstr_last_chunk) != NULL && MP_STATE_VM(qstr_last_used) + n_bytes > MP_STATE_VM(qstr_last_alloc)) {
            // not enough room at end of previously interned string so try to grow
            char *new_p = m_renew_maybe(char, MP_STATE_VM(qstr_last_chunk), MP_STATE_VM(qstr_last_alloc), MP_STATE_VM(qstr_last_alloc) + n_bytes, false);
            if (new_p == NULL) {
                // could not grow existing memory; shrink it to fit previous
                (void)m_renew_maybe(char, MP_STATE_VM(qstr_last_chunk), MP_STATE_VM(qstr_last_alloc), MP_STATE_VM(qstr_last_used), false);
                MP_STATE_VM(qstr_last_chunk) = NULL;
            } else {
                // could grow existing memory
                MP_STATE_VM(qstr_last_alloc) += n_bytes;
            }
        }

        if (MP_STATE_VM(qstr_last_chunk) == NULL) {
            // no existing memory for the interned string so allocate a new chunk
            size_t al = n_bytes;
            if (al < MICROPY_ALLOC_QSTR_CHUNK_INIT) {
                al = MICROPY_ALLOC_QSTR_CHUNK_INIT;
            }
            MP_STATE_VM(qstr_last_chunk) = m_new_ll_maybe(char, al);
            if (MP_STATE_VM(qstr_last_chunk) == NULL) {
                // failed to allocate a large chunk so try with exact size
                MP_STATE_VM(qstr_last_chunk) = m_new_ll_maybe(char, n_bytes);
                if (MP_STATE_VM(qstr_last_chunk) == NULL) {
                    QSTR_EXIT();
                    m_malloc_fail(n_bytes);
                }
                al = n_bytes;
            }
            MP_STATE_VM(qstr_last_alloc) = al;
            MP_STATE_VM(qstr_last_used) = 0;
        }

        // allocate memory from the chunk for this new interned string's data
        char *q_ptr = MP_STATE_VM(qstr_last_chunk) + MP_STATE_VM(qstr_last_used);
        MP_STATE_VM(qstr_last_used) += n_bytes;

        // store the interned strings' data
        mp_uint_t hash = qstr_compute_hash((const byte *)str, len);
        memcpy(q_ptr, str, len);
        q_ptr[len] = '\0';
        q = qstr_add(hash, len, q_ptr);
    }
    QSTR_EXIT();
    return q;
}

mp_uint_t PLACE_IN_ITCM(qstr_hash)(qstr q) {
    qstr_attr_t attr;
    find_qstr(q, &attr);
    return attr.hash;
}

size_t qstr_len(qstr q) {
    qstr_attr_t attr;
    find_qstr(q, &attr);
    return attr.len;
}

const char *qstr_str(qstr q) {
    qstr_attr_t attr;
    return find_qstr(q, &attr);
}

const byte *qstr_data(qstr q, size_t *len) {
    qstr_attr_t attr;
    const char *qd = find_qstr(q, &attr);
    *len = attr.len;
    return (byte *)qd;
}

void qstr_pool_info(size_t *n_pool, size_t *n_qstr, size_t *n_str_data_bytes, size_t *n_total_bytes) {
    QSTR_ENTER();
    *n_pool = 0;
    *n_qstr = 0;
    *n_str_data_bytes = 0;
    *n_total_bytes = 0;
    for (const qstr_pool_t *pool = MP_STATE_VM(last_pool); pool != NULL && pool != &CONST_POOL; pool = pool->prev) {
        *n_pool += 1;
        *n_qstr += pool->len;
        for (const qstr_attr_t *q = pool->attrs, *q_top = pool->attrs + pool->len; q < q_top; q++) {
            *n_str_data_bytes += sizeof(*q) + q->len + 1;
        }
        #if MICROPY_ENABLE_GC
        // this counts actual bytes used in heap
        *n_total_bytes += gc_nbytes(pool) - sizeof(qstr_attr_t) * pool->alloc;
        #else
        *n_total_bytes += sizeof(qstr_pool_t) + sizeof(const char *) * pool->alloc;
        #endif
    }
    *n_total_bytes += *n_str_data_bytes;
    QSTR_EXIT();
}

#if MICROPY_PY_MICROPYTHON_MEM_INFO
void qstr_dump_data(void) {
    QSTR_ENTER();
    for (const qstr_pool_t *pool = MP_STATE_VM(last_pool); pool != NULL && pool != &CONST_POOL; pool = pool->prev) {
        for (const char *const *q = pool->qstrs, *const *q_top = pool->qstrs + pool->len; q < q_top; q++) {
            mp_printf(&mp_plat_print, "Q(%s)\n", *q);
        }
    }
    QSTR_EXIT();
}
#endif
