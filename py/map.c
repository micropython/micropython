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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "py/mpconfig.h"
#include "py/misc.h"
#include "py/runtime.h"

#if MICROPY_DEBUG_VERBOSE // print debugging info
#define DEBUG_PRINT (1)
#else // don't print debugging info
#define DEBUG_PRINT (0)
#define DEBUG_printf(...) (void)0
#endif

#if MICROPY_OPT_MAP_LOOKUP_CACHE
// MP_STATE_VM(map_lookup_cache) provides a cache of index to the last known
// position of that index in any map. On a cache hit, this allows
// short-circuiting the full linear search in the case of an ordered map
// (i.e. all builtin modules and objects' locals dicts), and computation of
// the hash (and potentially some linear probing) in the case of a regular
// map. Note the same cache is shared across all maps.

// Gets the index into the cache for this index. Shift down by two to remove
// mp_obj_t tag bits.
#define MAP_CACHE_OFFSET(index) ((((uintptr_t)(index)) >> 2) % MICROPY_OPT_MAP_LOOKUP_CACHE_SIZE)
// Gets the map cache entry for the corresponding index.
#define MAP_CACHE_ENTRY(index) (MP_STATE_VM(map_lookup_cache)[MAP_CACHE_OFFSET(index)])
// Retrieve the mp_obj_t at the location suggested by the cache.
#define MAP_CACHE_GET(map, index) (&(map)->table[MAP_CACHE_ENTRY(index) % (map)->alloc])
// Update the cache for this index.
#define MAP_CACHE_SET(index, pos) MAP_CACHE_ENTRY(index) = (pos) & 0xff;
#else
#define MAP_CACHE_SET(index, pos)
#endif

// This table of sizes is used to control the growth of hash tables.
// The first set of sizes are chosen so the allocation fits exactly in a
// 4-word GC block, and it's not so important for these small values to be
// prime.  The latter sizes are prime and increase at an increasing rate.
STATIC const uint16_t hash_allocation_sizes[] = {
    0, 2, 4, 6, 8, 10, 12, // +2
    17, 23, 29, 37, 47, 59, 73, // *1.25
    97, 127, 167, 223, 293, 389, 521, 691, 919, 1223, 1627, 2161, // *1.33
    3229, 4831, 7243, 10861, 16273, 24407, 36607, 54907, // *1.5
};

STATIC size_t get_hash_alloc_greater_or_equal_to(size_t x) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(hash_allocation_sizes); i++) {
        if (hash_allocation_sizes[i] >= x) {
            return hash_allocation_sizes[i];
        }
    }
    // ran out of primes in the table!
    // return something sensible, at least make it odd
    return (x + x / 2) | 1;
}

/******************************************************************************/
/* map                                                                        */

void mp_map_init(mp_map_t *map, size_t n) {
    if (n == 0) {
        map->alloc = 0;
        map->table = NULL;
    } else {
        map->alloc = n;
        map->table = m_new0(mp_map_elem_t, map->alloc);
    }
    map->used = 0;
    map->all_keys_are_qstrs = 1;
    map->is_fixed = 0;
    map->is_ordered = 0;
}

void mp_map_init_fixed_table(mp_map_t *map, size_t n, const mp_obj_t *table) {
    map->alloc = n;
    map->used = n;
    map->all_keys_are_qstrs = 1;
    map->is_fixed = 1;
    map->is_ordered = 1;
    map->table = (mp_map_elem_t *)table;
}

// Differentiate from mp_map_clear() - semantics is different
void mp_map_deinit(mp_map_t *map) {
    if (!map->is_fixed) {
        m_del(mp_map_elem_t, map->table, map->alloc);
    }
    map->used = map->alloc = 0;
}

void mp_map_clear(mp_map_t *map) {
    if (!map->is_fixed) {
        m_del(mp_map_elem_t, map->table, map->alloc);
    }
    map->alloc = 0;
    map->used = 0;
    map->all_keys_are_qstrs = 1;
    map->is_fixed = 0;
    map->table = NULL;
}

STATIC void mp_map_rehash(mp_map_t *map) {
    size_t old_alloc = map->alloc;
    size_t new_alloc = get_hash_alloc_greater_or_equal_to(map->alloc + 1);
    DEBUG_printf("mp_map_rehash(%p): " UINT_FMT " -> " UINT_FMT "\n", map, old_alloc, new_alloc);
    mp_map_elem_t *old_table = map->table;
    mp_map_elem_t *new_table = m_new0(mp_map_elem_t, new_alloc);
    // If we reach this point, table resizing succeeded, now we can edit the old map.
    map->alloc = new_alloc;
    map->used = 0;
    map->all_keys_are_qstrs = 1;
    map->table = new_table;
    for (size_t i = 0; i < old_alloc; i++) {
        if (old_table[i].key != MP_OBJ_NULL && old_table[i].key != MP_OBJ_SENTINEL) {
            mp_map_lookup(map, old_table[i].key, MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = old_table[i].value;
        }
    }
    m_del(mp_map_elem_t, old_table, old_alloc);
}

// MP_MAP_LOOKUP behaviour:
//  - returns NULL if not found, else the slot it was found in with key,value non-null
// MP_MAP_LOOKUP_ADD_IF_NOT_FOUND behaviour:
//  - returns slot, with key non-null and value=MP_OBJ_NULL if it was added
// MP_MAP_LOOKUP_REMOVE_IF_FOUND behaviour:
//  - returns NULL if not found, else the slot if was found in with key null and value non-null
mp_map_elem_t *mp_map_lookup(mp_map_t *map, mp_obj_t index, mp_map_lookup_kind_t lookup_kind) {
    // If the map is a fixed array then we must only be called for a lookup
    assert(!map->is_fixed || lookup_kind == MP_MAP_LOOKUP);

    #if MICROPY_OPT_MAP_LOOKUP_CACHE
    // Try the cache for lookup or add-if-not-found.
    if (lookup_kind != MP_MAP_LOOKUP_REMOVE_IF_FOUND && map->alloc) {
        mp_map_elem_t *slot = MAP_CACHE_GET(map, index);
        // Note: Just comparing key for value equality will have false negatives, but
        // these will be handled by the regular path below.
        if (slot->key == index) {
            return slot;
        }
    }
    #endif

    // Work out if we can compare just pointers
    bool compare_only_ptrs = map->all_keys_are_qstrs;
    if (compare_only_ptrs) {
        if (mp_obj_is_qstr(index)) {
            // Index is a qstr, so can just do ptr comparison.
        } else if (mp_obj_is_type(index, &mp_type_str)) {
            // Index is a non-interned string.
            // We can either intern the string, or force a full equality comparison.
            // We chose the latter, since interning costs time and potentially RAM,
            // and it won't necessarily benefit subsequent calls because these calls
            // most likely won't pass the newly-interned string.
            compare_only_ptrs = false;
        } else if (lookup_kind != MP_MAP_LOOKUP_ADD_IF_NOT_FOUND) {
            // If we are not adding, then we can return straight away a failed
            // lookup because we know that the index will never be found.
            return NULL;
        }
    }

    // if the map is an ordered array then we must do a brute force linear search
    if (map->is_ordered) {
        for (mp_map_elem_t *elem = &map->table[0], *top = &map->table[map->used]; elem < top; elem++) {
            if (elem->key == index || (!compare_only_ptrs && mp_obj_equal(elem->key, index))) {
                #if MICROPY_PY_COLLECTIONS_ORDEREDDICT
                if (MP_UNLIKELY(lookup_kind == MP_MAP_LOOKUP_REMOVE_IF_FOUND)) {
                    // remove the found element by moving the rest of the array down
                    mp_obj_t value = elem->value;
                    --map->used;
                    memmove(elem, elem + 1, (top - elem - 1) * sizeof(*elem));
                    // put the found element after the end so the caller can access it if needed
                    // note: caller must NULL the value so the GC can clean up (e.g. see dict_get_helper).
                    elem = &map->table[map->used];
                    elem->key = MP_OBJ_NULL;
                    elem->value = value;
                }
                #endif
                MAP_CACHE_SET(index, elem - map->table);
                return elem;
            }
        }
        #if MICROPY_PY_COLLECTIONS_ORDEREDDICT
        if (MP_LIKELY(lookup_kind != MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)) {
            return NULL;
        }
        if (map->used == map->alloc) {
            // TODO: Alloc policy
            map->alloc += 4;
            map->table = m_renew(mp_map_elem_t, map->table, map->used, map->alloc);
            mp_seq_clear(map->table, map->used, map->alloc, sizeof(*map->table));
        }
        mp_map_elem_t *elem = map->table + map->used++;
        elem->key = index;
        if (!mp_obj_is_qstr(index)) {
            map->all_keys_are_qstrs = 0;
        }
        return elem;
        #else
        return NULL;
        #endif
    }

    // map is a hash table (not an ordered array), so do a hash lookup

    if (map->alloc == 0) {
        if (lookup_kind == MP_MAP_LOOKUP_ADD_IF_NOT_FOUND) {
            mp_map_rehash(map);
        } else {
            return NULL;
        }
    }

    // get hash of index, with fast path for common case of qstr
    mp_uint_t hash;
    if (mp_obj_is_qstr(index)) {
        hash = qstr_hash(MP_OBJ_QSTR_VALUE(index));
    } else {
        hash = MP_OBJ_SMALL_INT_VALUE(mp_unary_op(MP_UNARY_OP_HASH, index));
    }

    size_t pos = hash % map->alloc;
    size_t start_pos = pos;
    mp_map_elem_t *avail_slot = NULL;
    for (;;) {
        mp_map_elem_t *slot = &map->table[pos];
        if (slot->key == MP_OBJ_NULL) {
            // found NULL slot, so index is not in table
            if (lookup_kind == MP_MAP_LOOKUP_ADD_IF_NOT_FOUND) {
                map->used += 1;
                if (avail_slot == NULL) {
                    avail_slot = slot;
                }
                avail_slot->key = index;
                avail_slot->value = MP_OBJ_NULL;
                if (!mp_obj_is_qstr(index)) {
                    map->all_keys_are_qstrs = 0;
                }
                return avail_slot;
            } else {
                return NULL;
            }
        } else if (slot->key == MP_OBJ_SENTINEL) {
            // found deleted slot, remember for later
            if (avail_slot == NULL) {
                avail_slot = slot;
            }
        } else if (slot->key == index || (!compare_only_ptrs && mp_obj_equal(slot->key, index))) {
            // found index
            // Note: CPython does not replace the index; try x={True:'true'};x[1]='one';x
            if (lookup_kind == MP_MAP_LOOKUP_REMOVE_IF_FOUND) {
                // delete element in this slot
                map->used--;
                if (map->table[(pos + 1) % map->alloc].key == MP_OBJ_NULL) {
                    // optimisation if next slot is empty
                    slot->key = MP_OBJ_NULL;
                } else {
                    slot->key = MP_OBJ_SENTINEL;
                }
                // keep slot->value so that caller can access it if needed
            }
            MAP_CACHE_SET(index, pos);
            return slot;
        }

        // not yet found, keep searching in this table
        pos = (pos + 1) % map->alloc;

        if (pos == start_pos) {
            // search got back to starting position, so index is not in table
            if (lookup_kind == MP_MAP_LOOKUP_ADD_IF_NOT_FOUND) {
                if (avail_slot != NULL) {
                    // there was an available slot, so use that
                    map->used++;
                    avail_slot->key = index;
                    avail_slot->value = MP_OBJ_NULL;
                    if (!mp_obj_is_qstr(index)) {
                        map->all_keys_are_qstrs = 0;
                    }
                    return avail_slot;
                } else {
                    // not enough room in table, rehash it
                    mp_map_rehash(map);
                    // restart the search for the new element
                    start_pos = pos = hash % map->alloc;
                }
            } else {
                return NULL;
            }
        }
    }
}

/******************************************************************************/
/* set                                                                        */

#if MICROPY_PY_BUILTINS_SET

void mp_set_init(mp_set_t *set, size_t n) {
    set->alloc = n;
    set->used = 0;
    set->table = m_new0(mp_obj_t, set->alloc);
}

STATIC void mp_set_rehash(mp_set_t *set) {
    size_t old_alloc = set->alloc;
    mp_obj_t *old_table = set->table;
    set->alloc = get_hash_alloc_greater_or_equal_to(set->alloc + 1);
    set->used = 0;
    set->table = m_new0(mp_obj_t, set->alloc);
    for (size_t i = 0; i < old_alloc; i++) {
        if (old_table[i] != MP_OBJ_NULL && old_table[i] != MP_OBJ_SENTINEL) {
            mp_set_lookup(set, old_table[i], MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
        }
    }
    m_del(mp_obj_t, old_table, old_alloc);
}

mp_obj_t mp_set_lookup(mp_set_t *set, mp_obj_t index, mp_map_lookup_kind_t lookup_kind) {
    // Note: lookup_kind can be MP_MAP_LOOKUP_ADD_IF_NOT_FOUND_OR_REMOVE_IF_FOUND which
    // is handled by using bitwise operations.

    if (set->alloc == 0) {
        if (lookup_kind & MP_MAP_LOOKUP_ADD_IF_NOT_FOUND) {
            mp_set_rehash(set);
        } else {
            return MP_OBJ_NULL;
        }
    }
    mp_uint_t hash = MP_OBJ_SMALL_INT_VALUE(mp_unary_op(MP_UNARY_OP_HASH, index));
    size_t pos = hash % set->alloc;
    size_t start_pos = pos;
    mp_obj_t *avail_slot = NULL;
    for (;;) {
        mp_obj_t elem = set->table[pos];
        if (elem == MP_OBJ_NULL) {
            // found NULL slot, so index is not in table
            if (lookup_kind & MP_MAP_LOOKUP_ADD_IF_NOT_FOUND) {
                if (avail_slot == NULL) {
                    avail_slot = &set->table[pos];
                }
                set->used++;
                *avail_slot = index;
                return index;
            } else {
                return MP_OBJ_NULL;
            }
        } else if (elem == MP_OBJ_SENTINEL) {
            // found deleted slot, remember for later
            if (avail_slot == NULL) {
                avail_slot = &set->table[pos];
            }
        } else if (mp_obj_equal(elem, index)) {
            // found index
            if (lookup_kind & MP_MAP_LOOKUP_REMOVE_IF_FOUND) {
                // delete element
                set->used--;
                if (set->table[(pos + 1) % set->alloc] == MP_OBJ_NULL) {
                    // optimisation if next slot is empty
                    set->table[pos] = MP_OBJ_NULL;
                } else {
                    set->table[pos] = MP_OBJ_SENTINEL;
                }
            }
            return elem;
        }

        // not yet found, keep searching in this table
        pos = (pos + 1) % set->alloc;

        if (pos == start_pos) {
            // search got back to starting position, so index is not in table
            if (lookup_kind & MP_MAP_LOOKUP_ADD_IF_NOT_FOUND) {
                if (avail_slot != NULL) {
                    // there was an available slot, so use that
                    set->used++;
                    *avail_slot = index;
                    return index;
                } else {
                    // not enough room in table, rehash it
                    mp_set_rehash(set);
                    // restart the search for the new element
                    start_pos = pos = hash % set->alloc;
                }
            } else {
                return MP_OBJ_NULL;
            }
        }
    }
}

mp_obj_t mp_set_remove_first(mp_set_t *set) {
    for (size_t pos = 0; pos < set->alloc; pos++) {
        if (mp_set_slot_is_filled(set, pos)) {
            mp_obj_t elem = set->table[pos];
            // delete element
            set->used--;
            if (set->table[(pos + 1) % set->alloc] == MP_OBJ_NULL) {
                // optimisation if next slot is empty
                set->table[pos] = MP_OBJ_NULL;
            } else {
                set->table[pos] = MP_OBJ_SENTINEL;
            }
            return elem;
        }
    }
    return MP_OBJ_NULL;
}

void mp_set_clear(mp_set_t *set) {
    m_del(mp_obj_t, set->table, set->alloc);
    set->alloc = 0;
    set->used = 0;
    set->table = NULL;
}

#endif // MICROPY_PY_BUILTINS_SET

#if defined(DEBUG_PRINT) && DEBUG_PRINT
void mp_map_dump(mp_map_t *map) {
    for (size_t i = 0; i < map->alloc; i++) {
        if (map->table[i].key != MP_OBJ_NULL) {
            mp_obj_print(map->table[i].key, PRINT_REPR);
        } else {
            DEBUG_printf("(nil)");
        }
        DEBUG_printf(": %p\n", map->table[i].value);
    }
    DEBUG_printf("---\n");
}
#endif
