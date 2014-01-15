typedef struct _mp_obj_tuple_t {
    mp_obj_base_t base;
    machine_uint_t len;
    mp_obj_t items[];
} mp_obj_tuple_t;

void tuple_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in, mp_print_kind_t kind);
