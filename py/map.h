typedef enum {
    MAP_QSTR,
    MAP_PY_OBJ,
} py_map_kind_t;

typedef struct _py_map_elem_t {
    py_obj_t key;
    py_obj_t value;
} py_map_elem_t;

typedef struct _py_map_t {
    struct {
        py_map_kind_t kind : 1;
        machine_uint_t used : (8 * BYTES_PER_WORD - 1);
    };
    machine_uint_t alloc;
    py_map_elem_t *table;
} py_map_t;

// these are defined in runtime.c
py_map_t *rt_get_map_locals(void);
void rt_set_map_locals(py_map_t *m);

int get_doubling_prime_greater_or_equal_to(int x);
void py_map_init(py_map_t *map, py_map_kind_t kind, int n);
py_map_t *py_map_new(py_map_kind_t kind, int n);
py_map_elem_t* py_map_lookup_helper(py_map_t *map, py_obj_t index, bool add_if_not_found);
py_map_elem_t* py_qstr_map_lookup(py_map_t *map, qstr index, bool add_if_not_found);
py_map_elem_t* py_map_lookup(py_obj_t o, py_obj_t index, bool add_if_not_found);
py_obj_t py_set_lookup(py_obj_t o_in, py_obj_t index, bool add_if_not_found);
