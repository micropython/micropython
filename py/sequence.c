#include <stdbool.h>
#include <string.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime0.h"
#include "runtime.h"

// Helpers for sequence types

#define SWAP(type, var1, var2) { type t = var2; var2 = var1; var1 = t; }

// Implements backend of sequence * integer operation. Assumes elements are
// memory-adjacent in sequence.
void mp_seq_multiply(const void *items, uint item_sz, uint len, uint times, void *dest) {
    for (int i = 0; i < times; i++) {
        uint copy_sz = item_sz * len;
        memcpy(dest, items, copy_sz);
        dest = (char*)dest + copy_sz;
    }
}

bool m_seq_get_fast_slice_indexes(machine_uint_t len, mp_obj_t slice, machine_uint_t *begin, machine_uint_t *end) {
    machine_int_t start, stop, step;
    mp_obj_slice_get(slice, &start, &stop, &step);
    if (step != 1) {
        return false;
    }

    // Unlike subscription, out-of-bounds slice indexes are never error
    if (start < 0) {
        start = len + start;
        if (start < 0) {
            start = 0;
        }
    } else if (start > len) {
        start = len;
    }
    if (stop <= 0) {
        stop = len + stop;
        // CPython returns empty sequence in such case
        if (stop < 0) {
            stop = start;
        }
    } else if (stop > len) {
        stop = len;
    }
    *begin = start;
    *end = stop;
    return true;
}

// Special-case comparison function for sequences of bytes
// Don't pass MP_BINARY_OP_NOT_EQUAL here
bool mp_seq_cmp_bytes(int op, const byte *data1, uint len1, const byte *data2, uint len2) {
    // Let's deal only with > & >=
    if (op == MP_BINARY_OP_LESS || op == MP_BINARY_OP_LESS_EQUAL) {
        SWAP(const byte*, data1, data2);
        SWAP(uint, len1, len2);
        if (op == MP_BINARY_OP_LESS) {
            op = MP_BINARY_OP_MORE;
        } else {
            op = MP_BINARY_OP_MORE_EQUAL;
        }
    }
    uint min_len = len1 < len2 ? len1 : len2;
    int res = memcmp(data1, data2, min_len);
    if (res < 0) {
        return false;
    }
    if (res > 0) {
        return true;
    }

    // If we had tie in the last element...
    // ... and we have lists of different lengths...
    if (len1 != len2) {
        if (len1 < len2) {
            // ... then longer list length wins (we deal only with >)
            return false;
        }
    } else if (op == MP_BINARY_OP_MORE) {
        // Otherwise, if we have strict relation, equality means failure
        return false;
    }
    return true;
}

// Special-case comparison function for sequences of mp_obj_t
// Don't pass MP_BINARY_OP_NOT_EQUAL here
bool mp_seq_cmp_objs(int op, const mp_obj_t *items1, uint len1, const mp_obj_t *items2, uint len2) {
    if (op == MP_BINARY_OP_EQUAL && len1 != len2) {
        return false;
    }

    // Let's deal only with > & >=
    if (op == MP_BINARY_OP_LESS || op == MP_BINARY_OP_LESS_EQUAL) {
        SWAP(const mp_obj_t *, items1, items2);
        SWAP(uint, len1, len2);
        if (op == MP_BINARY_OP_LESS) {
            op = MP_BINARY_OP_MORE;
        } else {
            op = MP_BINARY_OP_MORE_EQUAL;
        }
    }

    int len = len1 < len2 ? len1 : len2;
    bool eq_status = true; // empty lists are equal
    bool rel_status;
    for (int i = 0; i < len; i++) {
        eq_status = mp_obj_equal(items1[i], items2[i]);
        if (op == MP_BINARY_OP_EQUAL && !eq_status) {
            return false;
        }
        rel_status = (mp_binary_op(op, items1[i], items2[i]) == mp_const_true);
        if (!eq_status && !rel_status) {
            return false;
        }
    }

    // If we had tie in the last element...
    if (eq_status) {
        // ... and we have lists of different lengths...
        if (len1 != len2) {
            if (len1 < len2) {
                // ... then longer list length wins (we deal only with >)
                return false;
            }
        } else if (op == MP_BINARY_OP_MORE) {
            // Otherwise, if we have strict relation, equality means failure
            return false;
        }
    }

    return true;
}

// Special-case of index() which searches for mp_obj_t
mp_obj_t mp_seq_index_obj(const mp_obj_t *items, uint len, uint n_args, const mp_obj_t *args) {
    mp_obj_type_t *type = mp_obj_get_type(args[0]);
    mp_obj_t *value = args[1];
    uint start = 0;
    uint stop = len;

    if (n_args >= 3) {
        start = mp_get_index(type, len, args[2], true);
        if (n_args >= 4) {
            stop = mp_get_index(type, len, args[3], true);
        }
    }

    for (machine_uint_t i = start; i < stop; i++) {
        if (mp_obj_equal(items[i], value)) {
            // Common sense says this cannot overflow small int
            return MP_OBJ_NEW_SMALL_INT(i);
        }
    }

    nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "object not in sequence"));
}

mp_obj_t mp_seq_count_obj(const mp_obj_t *items, uint len, mp_obj_t value) {
    machine_uint_t count = 0;
    for (uint i = 0; i < len; i++) {
         if (mp_obj_equal(items[i], value)) {
              count++;
         }
    }

    // Common sense says this cannot overflow small int
    return MP_OBJ_NEW_SMALL_INT(count);
}
