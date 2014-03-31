#include <stdlib.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime0.h"

// approximatelly doubling primes; made with Mathematica command: Table[Prime[Floor[(1.7)^n]], {n, 3, 24}]
// prefixed with zero for the empty case.
STATIC int doubling_primes[] = {0, 7, 19, 43, 89, 179, 347, 647, 1229, 2297, 4243, 7829, 14347, 26017, 47149, 84947, 152443, 273253, 488399, 869927, 1547173, 2745121, 4861607};

STATIC int get_doubling_prime_greater_or_equal_to(int x) {
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

void mp_map_init(mp_map_t *map, int n) {
    if (n == 0) {
        map->alloc = 0;
        map->table = NULL;
    } else {
        map->alloc = get_doubling_prime_greater_or_equal_to(n + 1);
        map->table = m_new0(mp_map_elem_t, map->alloc);
    }
    map->used = 0;
    map->all_keys_are_qstrs = 1;
    map->table_is_fixed_array = 0;
}

void mp_map_init_fixed_table(mp_map_t *map, int n, const mp_obj_t *table) {
    map->alloc = n;
    map->used = n;
    map->all_keys_are_qstrs = 1;
    map->table_is_fixed_array = 1;
    map->table = (mp_map_elem_t*)table;
}

mp_map_t *mp_map_new(int n) {
    mp_map_t *map = m_new(mp_map_t, 1);
    mp_map_init(map, n);
    return map;
}

// Differentiate from mp_map_clear() - semantics is different
void mp_map_deinit(mp_map_t *map) {
    if (!map->table_is_fixed_array) {
        m_del(mp_map_elem_t, map->table, map->alloc);
    }
    map->used = map->alloc = 0;
}

void mp_map_free(mp_map_t *map) {
    mp_map_deinit(map);
    m_del_obj(mp_map_t, map);
}

void mp_map_clear(mp_map_t *map) {
    if (!map->table_is_fixed_array) {
        m_del(mp_map_elem_t, map->table, map->alloc);
    }
    map->alloc = 0;
    map->used = 0;
    map->all_keys_are_qstrs = 1;
    map->table_is_fixed_array = 0;
    map->table = NULL;
}

STATIC void mp_map_rehash(mp_map_t *map) {
    int old_alloc = map->alloc;
    mp_map_elem_t *old_table = map->table;
    map->alloc = get_doubling_prime_greater_or_equal_to(map->alloc + 1);
    map->used = 0;
    map->all_keys_are_qstrs = 1;
    map->table = m_new0(mp_map_elem_t, map->alloc);
    for (int i = 0; i < old_alloc; i++) {
        if (old_table[i].key != NULL) {
            mp_map_lookup(map, old_table[i].key, MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = old_table[i].value;
        }
    }
    m_del(mp_map_elem_t, old_table, old_alloc);
}

mp_map_elem_t* mp_map_lookup(mp_map_t *map, mp_obj_t index, mp_map_lookup_kind_t lookup_kind) {
    // if the map is a fixed array then we must do a brute force linear search
    if (map->table_is_fixed_array) {
        if (lookup_kind != MP_MAP_LOOKUP) {
            return NULL;
        }
        for (mp_map_elem_t *elem = &map->table[0], *top = &map->table[map->used]; elem < top; elem++) {
            if (elem->key == index || (!map->all_keys_are_qstrs && mp_obj_equal(elem->key, index))) {
                return elem;
            }
        }
        return NULL;
    }

    // map is a hash table (not a fixed array), so do a hash lookup

    machine_uint_t hash;
    hash = mp_obj_hash(index);
    if (map->alloc == 0) {
        if (lookup_kind & MP_MAP_LOOKUP_ADD_IF_NOT_FOUND) {
            mp_map_rehash(map);
        } else {
            return NULL;
        }
    }

    uint pos = hash % map->alloc;
    for (;;) {
        mp_map_elem_t *elem = &map->table[pos];
        if (elem->key == NULL) {
            // not in table
            if (lookup_kind & MP_MAP_LOOKUP_ADD_IF_NOT_FOUND) {
                if (map->used + 1 >= map->alloc) {
                    // not enough room in table, rehash it
                    mp_map_rehash(map);
                    // restart the search for the new element
                    pos = hash % map->alloc;
                } else {
                    map->used += 1;
                    elem->key = index;
                    if (!MP_OBJ_IS_QSTR(index)) {
                        map->all_keys_are_qstrs = 0;
                    }
                    return elem;
                }
            } else {
                return NULL;
            }
        } else if (elem->key == index || (!map->all_keys_are_qstrs && mp_obj_equal(elem->key, index))) {
            // found it
            /* it seems CPython does not replace the index; try x={True:'true'};x[1]='one';x
            if (add_if_not_found) {
                elem->key = index;
            }
            */
            if (lookup_kind & MP_MAP_LOOKUP_REMOVE_IF_FOUND) {
                map->used--;
                // this leaks this memory (but see dict_get_helper)
                mp_map_elem_t *retval = m_new(mp_map_elem_t, 1);
                retval->key = elem->key;
                retval->value = elem->value;
                elem->key = NULL;
                elem->value = NULL;
                return retval;
            }
            return elem;
        } else {
            // not yet found, keep searching in this table
            pos = (pos + 1) % map->alloc;
        }
    }
}

/******************************************************************************/
/* set                                                                        */

void mp_set_init(mp_set_t *set, int n) {
    set->alloc = get_doubling_prime_greater_or_equal_to(n + 1);
    set->used = 0;
    set->table = m_new0(mp_obj_t, set->alloc);
}

STATIC void mp_set_rehash(mp_set_t *set) {
    int old_alloc = set->alloc;
    mp_obj_t *old_table = set->table;
    set->alloc = get_doubling_prime_greater_or_equal_to(set->alloc + 1);
    set->used = 0;
    set->table = m_new0(mp_obj_t, set->alloc);
    for (int i = 0; i < old_alloc; i++) {
        if (old_table[i] != NULL) {
            mp_set_lookup(set, old_table[i], true);
        }
    }
    m_del(mp_obj_t, old_table, old_alloc);
}

mp_obj_t mp_set_lookup(mp_set_t *set, mp_obj_t index, mp_map_lookup_kind_t lookup_kind) {
    int hash;
    int pos;
    if (set->alloc == 0) {
        if (lookup_kind & MP_MAP_LOOKUP_ADD_IF_NOT_FOUND) {
            mp_set_rehash(set);
        } else {
            return NULL;
        }
    }
    if (lookup_kind & MP_MAP_LOOKUP_FIRST) {
        hash = 0;
        pos = 0;
    } else {
        hash = mp_obj_hash(index);;
        pos = hash % set->alloc;
    }
    for (;;) {
        mp_obj_t elem = set->table[pos];
        if (elem == MP_OBJ_NULL) {
            // not in table
            if (lookup_kind & MP_MAP_LOOKUP_ADD_IF_NOT_FOUND) {
                if (set->used + 1 >= set->alloc) {
                    // not enough room in table, rehash it
                    mp_set_rehash(set);
                    // restart the search for the new element
                    pos = hash % set->alloc;
                } else {
                    set->used += 1;
                    set->table[pos] = index;
                    return index;
                }
            } else if (lookup_kind & MP_MAP_LOOKUP_FIRST) {
                pos++;
            } else {
                return MP_OBJ_NULL;
            }
        } else if ((lookup_kind & MP_MAP_LOOKUP_FIRST) || mp_obj_equal(elem, index)) {
            // found it
            if (lookup_kind & MP_MAP_LOOKUP_REMOVE_IF_FOUND) {
                set->used--;
                set->table[pos] = NULL;
            }
            return elem;
        } else {
            // not yet found, keep searching in this table
            pos = (pos + 1) % set->alloc;
        }
    }
}

void mp_set_clear(mp_set_t *set) {
    m_del(mp_obj_t, set->table, set->alloc);
    set->alloc = 0;
    set->used = 0;
    set->table = NULL;
}
