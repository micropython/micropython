typedef struct _mp_map_elem_t {
    mp_obj_t key;
    mp_obj_t value;
} mp_map_elem_t;

typedef struct _mp_map_t {
    struct {
        machine_uint_t all_keys_are_qstrs : 1;
        machine_uint_t used : (8 * sizeof(machine_uint_t) - 1);
    };
    machine_uint_t alloc;
    mp_map_elem_t *table;
} mp_map_t;

typedef struct _mp_set_t {
    machine_uint_t alloc;
    machine_uint_t used;
    mp_obj_t *table;
} mp_set_t;

typedef enum _mp_map_lookup_kind_t {
    MP_MAP_LOOKUP,
    MP_MAP_LOOKUP_ADD_IF_NOT_FOUND,
    MP_MAP_LOOKUP_REMOVE_IF_FOUND,
} mp_map_lookup_kind_t;

int get_doubling_prime_greater_or_equal_to(int x);
void mp_map_init(mp_map_t *map, int n);
mp_map_t *mp_map_new(int n);
mp_map_elem_t* mp_map_lookup(mp_map_t *map, mp_obj_t index, mp_map_lookup_kind_t lookup_kind);
void mp_map_clear(mp_map_t *map);

void mp_set_init(mp_set_t *set, int n);
mp_obj_t mp_set_lookup(mp_set_t *set, mp_obj_t index, mp_map_lookup_kind_t lookup_kind);
void mp_set_clear(mp_set_t *set);
