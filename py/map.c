#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include "misc.h"
#include "mpyconfig.h"
#include "runtime.h"

#include "map.h"
#include "obj.h"
#include "objprivate.h"

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

void py_map_init(py_map_t *map, py_map_kind_t kind, int n) {
    map->kind = kind;
    map->used = 0;
    map->alloc = get_doubling_prime_greater_or_equal_to(n + 1);
    map->table = m_new0(py_map_elem_t, map->alloc);
}

py_map_t *py_map_new(py_map_kind_t kind, int n) {
    py_map_t *map = m_new(py_map_t, 1);
    py_map_init(map, kind, n);
    return map;
}

py_map_elem_t* py_map_lookup_helper(py_map_t *map, py_obj_t index, bool add_if_not_found) {
    bool is_map_py_obj = (map->kind == MAP_PY_OBJ);
    machine_uint_t hash;
    if (is_map_py_obj) {
        hash = py_obj_hash(index);
    } else {
        hash = (machine_uint_t)index;
    }
    uint pos = hash % map->alloc;
    for (;;) {
        py_map_elem_t *elem = &map->table[pos];
        if (elem->key == NULL) {
            // not in table
            if (add_if_not_found) {
                if (map->used + 1 >= map->alloc) {
                    // not enough room in table, rehash it
                    int old_alloc = map->alloc;
                    py_map_elem_t *old_table = map->table;
                    map->alloc = get_doubling_prime_greater_or_equal_to(map->alloc + 1);
                    map->used = 0;
                    map->table = m_new0(py_map_elem_t, map->alloc);
                    for (int i = 0; i < old_alloc; i++) {
                        if (old_table[i].key != NULL) {
                            py_map_lookup_helper(map, old_table[i].key, true)->value = old_table[i].value;
                        }
                    }
                    m_free(old_table);
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
        } else if (elem->key == index || (is_map_py_obj && py_obj_equal(elem->key, index))) {
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

py_map_elem_t* py_qstr_map_lookup(py_map_t *map, qstr index, bool add_if_not_found) {
    py_obj_t o = (py_obj_t)(machine_uint_t)index;
    return py_map_lookup_helper(map, o, add_if_not_found);
}

py_map_elem_t* py_map_lookup(py_obj_t o, py_obj_t index, bool add_if_not_found) {
    assert(IS_O(o, O_MAP));
    return py_map_lookup_helper(&((py_obj_base_t *)o)->u_map, index, add_if_not_found);
}

py_obj_t py_set_lookup(py_obj_t o_in, py_obj_t index, bool add_if_not_found) {
    assert(IS_O(o_in, O_SET));
    py_obj_base_t *o = o_in;
    int hash = py_obj_hash(index);
    int pos = hash % o->u_set.alloc;
    for (;;) {
        py_obj_t elem = o->u_set.table[pos];
        if (elem == NULL) {
            // not in table
            if (add_if_not_found) {
                if (o->u_set.used + 1 >= o->u_set.alloc) {
                    // not enough room in table, rehash it
                    int old_alloc = o->u_set.alloc;
                    py_obj_t *old_table = o->u_set.table;
                    o->u_set.alloc = get_doubling_prime_greater_or_equal_to(o->u_set.alloc + 1);
                    o->u_set.used = 0;
                    o->u_set.table = m_new(py_obj_t, o->u_set.alloc);
                    for (int i = 0; i < old_alloc; i++) {
                        if (old_table[i] != NULL) {
                            py_set_lookup(o, old_table[i], true);
                        }
                    }
                    m_free(old_table);
                    // restart the search for the new element
                    pos = hash % o->u_set.alloc;
                } else {
                    o->u_set.used += 1;
                    o->u_set.table[pos] = index;
                    return index;
                }
            } else {
                return NULL;
            }
        } else if (py_obj_equal(elem, index)) {
            // found it
            return elem;
        } else {
            // not yet found, keep searching in this table
            pos = (pos + 1) % o->u_set.alloc;
        }
    }
}
