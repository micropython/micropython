#include <assert.h>
#include <string.h>

#include "misc.h"
#include "mpqstr.h"

// NOTE: we are using linear arrays to store and search for qstr's (unique strings, interned strings)
// ultimately we will replace this with a static hash table of some kind
// also probably need to include the length in the string data, to allow null bytes in the string

#if 0 // print debugging info
#include <stdio.h>
#define DEBUG_printf(args...) printf(args)
#else // don't print debugging info
#define DEBUG_printf(args...) (void)0
#endif

typedef struct _qstr_pool_t {
    struct _qstr_pool_t *prev;
    uint total_prev_len;
    uint alloc;
    uint len;
    const char *qstrs[];
} qstr_pool_t;

const static qstr_pool_t const_pool = {
    NULL,               // no previous pool
    0,                  // no previous pool
    10,                 // set so that the first dynamically allocated pool is twice this size; must be <= the len (just below)
    MP_QSTR_number_of,  // corresponds to number of strings in array just below
    {
        "nil", // must be first, since 0 qstr is nil
#define Q(id) #id,
#include "mpqstrraw.h"
#undef Q
    },
};

static qstr_pool_t *last_pool;

void qstr_init(void) {
    last_pool = (qstr_pool_t*)&const_pool; // we won't modify the const_pool since it has no allocated room left
}

static qstr qstr_add(const char *str) {
    DEBUG_printf("QSTR: add %s\n", str);

    // make sure we have room in the pool for a new qstr
    if (last_pool->len >= last_pool->alloc) {
        qstr_pool_t *pool = m_new_obj_var(qstr_pool_t, const char*, last_pool->alloc * 2);
        pool->prev = last_pool;
        pool->total_prev_len = last_pool->total_prev_len + last_pool->len;
        pool->alloc = last_pool->alloc * 2;
        pool->len = 0;
        last_pool = pool;
        DEBUG_printf("QSTR: allocate new pool of size %d\n", last_pool->alloc);
    }

    // add the new qstr
    last_pool->qstrs[last_pool->len++] = str;

    // return id for the newly-added qstr
    return last_pool->total_prev_len + last_pool->len - 1;
}

qstr qstr_from_str_static(const char *str) {
    for (qstr_pool_t *pool = last_pool; pool != NULL; pool = pool->prev) {
        for (const char **qstr = pool->qstrs, **qstr_top = pool->qstrs + pool->len; qstr < qstr_top; qstr++) {
            if (strcmp(*qstr, str) == 0) {
                return pool->total_prev_len + (qstr - pool->qstrs);
            }
        }
    }
    return qstr_add(str);
}

qstr qstr_from_str_take(char *str, int alloc_len) {
    for (qstr_pool_t *pool = last_pool; pool != NULL; pool = pool->prev) {
        for (const char **qstr = pool->qstrs, **qstr_top = pool->qstrs + pool->len; qstr < qstr_top; qstr++) {
            if (strcmp(*qstr, str) == 0) {
                m_del(char, str, alloc_len);
                return pool->total_prev_len + (qstr - pool->qstrs);
            }
        }
    }
    return qstr_add(str);
}

qstr qstr_from_strn_copy(const char *str, int len) {
    for (qstr_pool_t *pool = last_pool; pool != NULL; pool = pool->prev) {
        for (const char **qstr = pool->qstrs, **qstr_top = pool->qstrs + pool->len; qstr < qstr_top; qstr++) {
            if (strncmp(*qstr, str, len) == 0 && (*qstr)[len] == '\0') {
                return pool->total_prev_len + (qstr - pool->qstrs);
            }
        }
    }
    return qstr_add(strndup(str, len));
}

// convert qstr id to pointer to its string
const char *qstr_str(qstr qstr) {
    // search
    for (qstr_pool_t *pool = last_pool; pool != NULL; pool = pool->prev) {
        if (qstr >= pool->total_prev_len) {
            return pool->qstrs[qstr - pool->total_prev_len];
        }
    }

    // not found, return nil
    return const_pool.qstrs[0];
}
