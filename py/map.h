typedef enum {
    MP_MAP_QSTR,
    MP_MAP_OBJ,
} mp_map_kind_t;

typedef struct _mp_map_elem_t {
    mp_obj_t key;
    mp_obj_t value;
} mp_map_elem_t;

typedef struct _mp_map_t {
    struct {
        mp_map_kind_t kind : 1;
        machine_uint_t used : (8 * BYTES_PER_WORD - 1);
    };
    machine_uint_t alloc;
    mp_map_elem_t *table;
} mp_map_t;

typedef struct _mp_set_t {
    machine_uint_t alloc;
    machine_uint_t used;
    mp_obj_t *table;
} mp_set_t;

int get_doubling_prime_greater_or_equal_to(int x);
void mp_map_init(mp_map_t *map, mp_map_kind_t kind, int n);
mp_map_t *mp_map_new(mp_map_kind_t kind, int n);
mp_map_elem_t* mp_map_lookup_helper(mp_map_t *map, mp_obj_t index, MP_BOOL add_if_not_found);
mp_map_elem_t* mp_qstr_map_lookup(mp_map_t *map, qstr index, MP_BOOL add_if_not_found);

void mp_set_init(mp_set_t *set, int n);
mp_obj_t mp_set_lookup(mp_set_t *set, mp_obj_t index, MP_BOOL add_if_not_found);
