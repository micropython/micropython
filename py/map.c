#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include "misc.h"
#include "mpconfig.h"
#include "obj.h"
#include "runtime0.h"
#include "map.h"

// approximatelly doubling primes; made with Mathematica command: Table[Prime[Floor[(1.7)^n]], {n, 3, 24}]
static int doubling_primes[] = {7, 19, 43, 89, 179, 347, 647, 1229, 2297, 4243, 7829, 14347, 26017, 47149, 84947, 152443, 273253, 488399, 869927, 1547173, 2745121, 4861607};

int get_doubling_prime_greater_or_equal_to(int x) {
    for (int i = 0; i < sizeof(doubling_primes) / sizeof(int); i++) {
        if (doubling_primes[i] >= x) {
            return doubling_primes[i];
        }
    }
    // ran out of primes in the table!
    // return something sensible, at least make it odd
    return x | 1;
}

/******************************************************************************/
/* map                                                                        */

void mp_map_init(mp_map_t *map, mp_map_kind_t kind, int n) {
    map->kind = kind;
    map->used = 0;
    map->alloc = get_doubling_prime_greater_or_equal_to(n + 1);
    map->table = m_new0(mp_map_elem_t, map->alloc);
}

mp_map_t *mp_map_new(mp_map_kind_t kind, int n) {
    mp_map_t *map = m_new(mp_map_t, 1);
    mp_map_init(map, kind, n);
    return map;
}

mp_map_elem_t* mp_map_lookup_helper(mp_map_t *map, mp_obj_t index, bool add_if_not_found) {
    bool is_map_mp_obj = (map->kind == MP_MAP_OBJ);
    machine_uint_t hash;
    if (is_map_mp_obj) {
        hash = mp_obj_hash(index);
    } else {
        hash = (machine_uint_t)index;
    }
    uint pos = hash % map->alloc;
    for (;;) {
        mp_map_elem_t *elem = &map->table[pos];
        if (elem->key == NULL) {
            // not in table
            if (add_if_not_found) {
                if (map->used + 1 >= map->alloc) {
                    // not enough room in table, rehash it
                    int old_alloc = map->alloc;
                    mp_map_elem_t *old_table = map->table;
                    map->alloc = get_doubling_prime_greater_or_equal_to(map->alloc + 1);
                    map->used = 0;
                    map->table = m_new0(mp_map_elem_t, map->alloc);
                    for (int i = 0; i < old_alloc; i++) {
                        if (old_table[i].key != NULL) {
                            mp_map_lookup_helper(map, old_table[i].key, true)->value = old_table[i].value;
                        }
                    }
                    m_del(mp_map_elem_t, old_table, old_alloc);
                    // restart the search for the new element
                    pos = hash % map->alloc;
                } else {
                    map->used += 1;
                    elem->key = index;
                    return elem;
                }
            } else {
                return NULL;
            }
        } else if (elem->key == index || (is_map_mp_obj && mp_obj_equal(elem->key, index))) {
            // found it
            /* it seems CPython does not replace the index; try x={True:'true'};x[1]='one';x
            if (add_if_not_found) {
                elem->key = index;
            }
            */
            return elem;
        } else {
            // not yet found, keep searching in this table
            pos = (pos + 1) % map->alloc;
        }
    }
}

mp_map_elem_t* mp_qstr_map_lookup(mp_map_t *map, qstr index, bool add_if_not_found) {
    mp_obj_t o = (mp_obj_t)(machine_uint_t)index;
    return mp_map_lookup_helper(map, o, add_if_not_found);
}

/******************************************************************************/
/* set                                                                        */

void mp_set_init(mp_set_t *set, int n) {
    set->alloc = get_doubling_prime_greater_or_equal_to(n + 1);
    set->used = 0;
    set->table = m_new0(mp_obj_t, set->alloc);
}

mp_obj_t mp_set_lookup(mp_set_t *set, mp_obj_t index, bool add_if_not_found) {
    int hash = mp_obj_hash(index);
    int pos = hash % set->alloc;
    for (;;) {
        mp_obj_t elem = set->table[pos];
        if (elem == MP_OBJ_NULL) {
            // not in table
            if (add_if_not_found) {
                if (set->used + 1 >= set->alloc) {
                    // not enough room in table, rehash it
                    int old_alloc = set->alloc;
                    mp_obj_t *old_table = set->table;
                    set->alloc = get_doubling_prime_greater_or_equal_to(set->alloc + 1);
                    set->used = 0;
                    set->table = m_new(mp_obj_t, set->alloc);
                    for (int i = 0; i < old_alloc; i++) {
                        if (old_table[i] != NULL) {
                            mp_set_lookup(set, old_table[i], true);
                        }
                    }
                    m_del(mp_obj_t, old_table, old_alloc);
                    // restart the search for the new element
                    pos = hash % set->alloc;
                } else {
                    set->used += 1;
                    set->table[pos] = index;
                    return index;
                }
            } else {
                return MP_OBJ_NULL;
            }
        } else if (mp_obj_equal(elem, index)) {
            // found it
            return elem;
        } else {
            // not yet found, keep searching in this table
            pos = (pos + 1) % set->alloc;
        }
    }
}
