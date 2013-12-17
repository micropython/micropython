typedef machine_int_t py_small_int_t; // do we need to expose this in the public API?

#if MICROPY_ENABLE_FLOAT
typedef machine_float_t py_float_t;
#endif

// user defined objects

typedef struct _py_user_method_t {
    const char *name;
    machine_uint_t kind;
    void *fun;
} py_user_method_t;

typedef struct _py_user_info_t {
    const char *type_name;
    void (*print)(py_obj_t);
    const py_user_method_t methods[];
} py_user_info_t;

py_obj_t py_obj_new_int(machine_int_t value);
py_obj_t py_obj_new_const(const char *id);
py_obj_t py_obj_new_str(qstr qstr);
#if MICROPY_ENABLE_FLOAT
py_obj_t py_obj_new_float(py_float_t val);
py_obj_t py_obj_new_complex(py_float_t real, py_float_t imag);
#endif
py_obj_t py_obj_new_exception_0(qstr id);
py_obj_t py_obj_new_exception_2(qstr id, const char *fmt, const char *s1, const char *s2);
py_obj_t py_obj_new_range(int start, int stop, int step);
py_obj_t py_obj_new_range_iterator(int cur, int stop, int step);
py_obj_t py_obj_new_user(const py_user_info_t *info, machine_uint_t data1, machine_uint_t data2);

const char *py_obj_get_type_str(py_obj_t o_in);

void py_obj_print_helper(void (*print)(void *env, const char *fmt, ...), void *env, py_obj_t o_in);
void py_obj_print(py_obj_t o);

bool py_obj_is_callable(py_obj_t o_in);
machine_int_t py_obj_hash(py_obj_t o_in);
bool py_obj_equal(py_obj_t o1, py_obj_t o2);
bool py_obj_less(py_obj_t o1, py_obj_t o2);

machine_int_t py_obj_get_int(py_obj_t arg);
#if MICROPY_ENABLE_FLOAT
machine_float_t py_obj_get_float(py_obj_t arg);
void py_obj_get_complex(py_obj_t arg, py_float_t *real, py_float_t *imag);
#endif
qstr py_obj_get_qstr(py_obj_t arg);
py_obj_t *py_obj_get_array_fixed_n(py_obj_t o, machine_int_t n);

void py_user_get_data(py_obj_t o, machine_uint_t *data1, machine_uint_t *data2);
void py_user_set_data(py_obj_t o, machine_uint_t data1, machine_uint_t data2);

// temporary way of making C modules
py_obj_t py_module_new(void);
