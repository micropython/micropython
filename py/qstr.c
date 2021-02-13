/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#include <string.h>
#include <stdio.h>

#include "py/mpstate.h"
#include "py/qstr.h"
#include "py/gc.h"
#include "py/runtime.h"

// NOTE: we are using linear arrays to store and search for qstr's (unique strings, interned strings)
// ultimately we will replace this with a static hash table of some kind
// also probably need to include the length in the string data, to allow null bytes in the string

#if MICROPY_DEBUG_VERBOSE // print debugging info
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#endif

// A qstr is an index into the qstr pool.
// The data for a qstr contains (hash, length, data):
//  - hash (configurable number of bytes)
//  - length (configurable number of bytes)
//  - data ("length" number of bytes)
//  - \0 terminated (so they can be printed using printf)

#if MICROPY_QSTR_BYTES_IN_HASH == 1
    #define Q_HASH_MASK (0xff)
    #define Q_GET_HASH(q) ((mp_uint_t)(q)[0])
    #define Q_SET_HASH(q, hash) do { (q)[0] = (hash); } while (0)
#elif MICROPY_QSTR_BYTES_IN_HASH == 2
    #define Q_HASH_MASK (0xffff)
    #define Q_GET_HASH(q) ((mp_uint_t)(q)[0] | ((mp_uint_t)(q)[1] << 8))
    #define Q_SET_HASH(q, hash) do { (q)[0] = (hash); (q)[1] = (hash) >> 8; } while (0)
#else
    #error unimplemented qstr hash decoding
#endif
#define Q_GET_ALLOC(q)  (MICROPY_QSTR_BYTES_IN_HASH + MICROPY_QSTR_BYTES_IN_LEN + Q_GET_LENGTH(q) + 1)
#define Q_GET_DATA(q)   ((q) + MICROPY_QSTR_BYTES_IN_HASH + MICROPY_QSTR_BYTES_IN_LEN)
#if MICROPY_QSTR_BYTES_IN_LEN == 1
    #define Q_GET_LENGTH(q) ((q)[MICROPY_QSTR_BYTES_IN_HASH])
    #define Q_SET_LENGTH(q, len) do { (q)[MICROPY_QSTR_BYTES_IN_HASH] = (len); } while (0)
#elif MICROPY_QSTR_BYTES_IN_LEN == 2
    #define Q_GET_LENGTH(q) ((q)[MICROPY_QSTR_BYTES_IN_HASH] | ((q)[MICROPY_QSTR_BYTES_IN_HASH + 1] << 8))
    #define Q_SET_LENGTH(q, len) do { (q)[MICROPY_QSTR_BYTES_IN_HASH] = (len); (q)[MICROPY_QSTR_BYTES_IN_HASH + 1] = (len) >> 8; } while (0)
#else
    #error unimplemented qstr length decoding
#endif

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

const qstr_pool_t mp_qstr_special_const_pool = {
    NULL,                        // no previous pool
    0,                           // no previous pool
    MICROPY_ALLOC_QSTR_ENTRIES_INIT,
    MP_QSTRspecial_const_number_of + 1, // corresponds to number of strings in array just below
    false,                       // special constant qstrs are not sorted
    {
        #ifndef NO_QSTR
#define QDEF0(id, str) str,
#define QDEF1(id, str)
        #include "genhdr/qstrdefs.generated.h"
#undef QDEF0
#undef QDEF1
        #endif
        (const byte *)"",       // spacer for MP_QSTRspecial_const_number_of
    },
};

const qstr_pool_t mp_qstr_const_pool = {
    (qstr_pool_t *)&mp_qstr_special_const_pool,
    MP_QSTRspecial_const_number_of + 1,
    MICROPY_ALLOC_QSTR_ENTRIES_INIT,
    MP_QSTRnumber_of -
    (MP_QSTRspecial_const_number_of + 1),      // corresponds to number of strings in array just below
    true,                               // constant qstrs are sorted
    {
        #ifndef NO_QSTR
#define QDEF0(id, str)
#define QDEF1(id, str) str,
        #include "genhdr/qstrdefs.generated.h"
#undef QDEF0
#undef QDEF1
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

STATIC const byte *find_qstr(qstr q) {
    // search pool for this qstr
    // total_prev_len==0 in the final pool, so the loop will always terminate
    qstr_pool_t *pool = MP_STATE_VM(last_pool);
    while (q < pool->total_prev_len) {
        pool = pool->prev;
    }
    return pool->qstrs[q - pool->total_prev_len];
}

// qstr_mutex must be taken while in this function
STATIC qstr qstr_add(const byte *q_ptr) {
    DEBUG_printf("QSTR: add hash=%d len=%d data=%.*s\n", Q_GET_HASH(q_ptr), Q_GET_LENGTH(q_ptr), Q_GET_LENGTH(q_ptr), Q_GET_DATA(q_ptr));

    // make sure we have room in the pool for a new qstr
    if (MP_STATE_VM(last_pool)->len >= MP_STATE_VM(last_pool)->alloc) {
        size_t new_alloc = MP_STATE_VM(last_pool)->alloc * 2;
        #ifdef MICROPY_QSTR_EXTRA_POOL
        // Put a lower bound on the allocation size in case the extra qstr pool has few entries
        new_alloc = MAX(MICROPY_ALLOC_QSTR_ENTRIES_INIT, new_alloc);
        #endif
        qstr_pool_t *pool = m_new_obj_var_maybe(qstr_pool_t, const char *, new_alloc);
        if (pool == NULL) {
            QSTR_EXIT();
            m_malloc_fail(new_alloc);
        }
        pool->prev = MP_STATE_VM(last_pool);
        pool->total_prev_len = MP_STATE_VM(last_pool)->total_prev_len + MP_STATE_VM(last_pool)->len;
        pool->alloc = new_alloc;
        pool->len = 0;
        pool->sorted = false;
        MP_STATE_VM(last_pool) = pool;
        DEBUG_printf("QSTR: allocate new pool of size %d\n", MP_STATE_VM(last_pool)->alloc);
    }

    // add the new qstr
    MP_STATE_VM(last_pool)->qstrs[MP_STATE_VM(last_pool)->len++] = q_ptr;

    // return id for the newly-added qstr
    return MP_STATE_VM(last_pool)->total_prev_len + MP_STATE_VM(last_pool)->len - 1;
}

#define MP_QSTR_SEARCH_THRESHOLD 10

qstr qstr_find_strn(const char *str, size_t str_len) {
    mp_uint_t str_hash = qstr_compute_hash((const byte *)str, str_len);

    // search pools for the data
    for (qstr_pool_t *pool = MP_STATE_VM(last_pool); pool != NULL; pool = pool->prev) {
        size_t low = 0;
        size_t high = pool->len - 1;

        // binary search inside the pool
        if (pool->sorted) {
            while (high - low > MP_QSTR_SEARCH_THRESHOLD) {
                size_t mid = (low + high + 1) / 2;
                const byte **q = pool->qstrs + mid;
                size_t len = Q_GET_LENGTH(*q);
                if (len > str_len) {
                    len = str_len;
                }
                int cmp = memcmp(Q_GET_DATA(*q), str, str_len);
                if (cmp < 0) {
                    low = mid;
                } else if (cmp > 0) {
                    high = mid;
                } else {
                    if (Q_GET_LENGTH(*q) < str_len) {
                        low = mid;
                    } else if (Q_GET_LENGTH(*q) > str_len) {
                        high = mid;
                    } else {
                        return pool->total_prev_len + (q - pool->qstrs);
                    }
                }
            }
        }

        // sequential search for the remaining strings
        for (const byte **q = pool->qstrs + low; q != pool->qstrs + high + 1; q++) {
            if (*q &&
                Q_GET_HASH(*q) == str_hash &&
                Q_GET_LENGTH(*q) == str_len &&
                memcmp(Q_GET_DATA(*q), str, str_len) == 0) {
                return pool->total_prev_len + (q - pool->qstrs);
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
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("name too long"));
        }

        // compute number of bytes needed to intern this string
        size_t n_bytes = MICROPY_QSTR_BYTES_IN_HASH + MICROPY_QSTR_BYTES_IN_LEN + len + 1;

        if (MP_STATE_VM(qstr_last_chunk) != NULL && MP_STATE_VM(qstr_last_used) + n_bytes > MP_STATE_VM(qstr_last_alloc)) {
            // not enough room at end of previously interned string so try to grow
            byte *new_p = m_renew_maybe(byte, MP_STATE_VM(qstr_last_chunk), MP_STATE_VM(qstr_last_alloc), MP_STATE_VM(qstr_last_alloc) + n_bytes, false);
            if (new_p == NULL) {
                // could not grow existing memory; shrink it to fit previous
                (void)m_renew_maybe(byte, MP_STATE_VM(qstr_last_chunk), MP_STATE_VM(qstr_last_alloc), MP_STATE_VM(qstr_last_used), false);
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
            MP_STATE_VM(qstr_last_chunk) = m_new_maybe(byte, al);
            if (MP_STATE_VM(qstr_last_chunk) == NULL) {
                // failed to allocate a large chunk so try with exact size
                MP_STATE_VM(qstr_last_chunk) = m_new_maybe(byte, n_bytes);
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
        byte *q_ptr = MP_STATE_VM(qstr_last_chunk) + MP_STATE_VM(qstr_last_used);
        MP_STATE_VM(qstr_last_used) += n_bytes;

        // store the interned strings' data
        mp_uint_t hash = qstr_compute_hash((const byte *)str, len);
        Q_SET_HASH(q_ptr, hash);
        Q_SET_LENGTH(q_ptr, len);
        memcpy(q_ptr + MICROPY_QSTR_BYTES_IN_HASH + MICROPY_QSTR_BYTES_IN_LEN, str, len);
        q_ptr[MICROPY_QSTR_BYTES_IN_HASH + MICROPY_QSTR_BYTES_IN_LEN + len] = '\0';
        q = qstr_add(q_ptr);
    }
    QSTR_EXIT();
    return q;
}

mp_uint_t qstr_hash(qstr q) {
    const byte *qd = find_qstr(q);
    return Q_GET_HASH(qd);
}

size_t qstr_len(qstr q) {
    const byte *qd = find_qstr(q);
    return Q_GET_LENGTH(qd);
}

const char *qstr_str(qstr q) {
    const byte *qd = find_qstr(q);
    return (const char *)Q_GET_DATA(qd);
}

const byte *qstr_data(qstr q, size_t *len) {
    const byte *qd = find_qstr(q);
    *len = Q_GET_LENGTH(qd);
    return Q_GET_DATA(qd);
}

void qstr_pool_info(size_t *n_pool, size_t *n_qstr, size_t *n_str_data_bytes, size_t *n_total_bytes) {
    QSTR_ENTER();
    *n_pool = 0;
    *n_qstr = 0;
    *n_str_data_bytes = 0;
    *n_total_bytes = 0;
    for (qstr_pool_t *pool = MP_STATE_VM(last_pool); pool != NULL && pool != &CONST_POOL; pool = pool->prev) {
        *n_pool += 1;
        *n_qstr += pool->len;
        for (const byte **q = pool->qstrs, **q_top = pool->qstrs + pool->len; q < q_top; q++) {
            *n_str_data_bytes += Q_GET_ALLOC(*q);
        }
        #if MICROPY_ENABLE_GC
        *n_total_bytes += gc_nbytes(pool); // this counts actual bytes used in heap
        #else
        *n_total_bytes += sizeof(qstr_pool_t) + sizeof(qstr) * pool->alloc;
        #endif
    }
    *n_total_bytes += *n_str_data_bytes;
    QSTR_EXIT();
}

#if MICROPY_PY_MICROPYTHON_MEM_INFO
void qstr_dump_data(void) {
    QSTR_ENTER();
    for (qstr_pool_t *pool = MP_STATE_VM(last_pool); pool != NULL && pool != &CONST_POOL; pool = pool->prev) {
        for (const byte **q = pool->qstrs, **q_top = pool->qstrs + pool->len; q < q_top; q++) {
            mp_printf(&mp_plat_print, "Q(%s)\n", Q_GET_DATA(*q));
        }
    }
    QSTR_EXIT();
}
#endif

#if MICROPY_ROM_TEXT_COMPRESSION

#ifdef NO_QSTR

// If NO_QSTR is set, it means we're doing QSTR extraction.
// So we won't yet have "genhdr/compressed.data.h"

#else

// Emit the compressed_string_data string.
#define MP_COMPRESSED_DATA(x) STATIC const char *compressed_string_data = x;
#define MP_MATCH_COMPRESSED(a, b)
#include "genhdr/compressed.data.h"
#undef MP_COMPRESSED_DATA
#undef MP_MATCH_COMPRESSED

#endif // NO_QSTR

// This implements the "common word" compression scheme (see makecompresseddata.py) where the most
// common 128 words in error messages are replaced by their index into the list of common words.

// The compressed string data is delimited by setting high bit in the final char of each word.
// e.g. aaaa<0x80|a>bbbbbb<0x80|b>....
// This method finds the n'th string.
STATIC const byte *find_uncompressed_string(uint8_t n) {
    const byte *c = (byte *)compressed_string_data;
    while (n > 0) {
        while ((*c & 0x80) == 0) {
            ++c;
        }
        ++c;
        --n;
    }
    return c;
}

// Given a compressed string in src, decompresses it into dst.
// dst must be large enough (use MP_MAX_UNCOMPRESSED_TEXT_LEN+1).
void mp_decompress_rom_string(byte *dst, const mp_rom_error_text_t src_chr) {
    // Skip past the 0xff marker.
    const byte *src = (byte *)src_chr + 1;
    // Need to add spaces around compressed words, except for the first (i.e. transition from 1<->2).
    // 0 = start, 1 = compressed, 2 = regular.
    int state = 0;
    while (*src) {
        if ((byte) * src >= 128) {
            if (state != 0) {
                *dst++ = ' ';
            }
            state = 1;

            // High bit set, replace with common word.
            const byte *word = find_uncompressed_string(*src & 0x7f);
            // The word is terminated by the final char having its high bit set.
            while ((*word & 0x80) == 0) {
                *dst++ = *word++;
            }
            *dst++ = (*word & 0x7f);
        } else {
            // Otherwise just copy one char.
            if (state == 1) {
                *dst++ = ' ';
            }
            state = 2;

            *dst++ = *src;
        }
        ++src;
    }
    // Add null-terminator.
    *dst = 0;
}

#endif // MICROPY_ROM_TEXT_COMPRESSION
