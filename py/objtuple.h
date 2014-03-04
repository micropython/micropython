typedef struct _mp_obj_tuple_t {
    mp_obj_base_t base;
    machine_uint_t len;
    mp_obj_t items[];
} mp_obj_tuple_t;

void tuple_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in, mp_print_kind_t kind);
mp_obj_t tuple_unary_op(int op, mp_obj_t self_in);
mp_obj_t tuple_binary_op(int op, mp_obj_t lhs, mp_obj_t rhs);
