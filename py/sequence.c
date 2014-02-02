#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "map.h"
#include "runtime0.h"
#include "runtime.h"

// Helpers for sequence types

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
