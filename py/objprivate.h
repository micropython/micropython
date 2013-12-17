// internal py_obj_t representation
// not for general use, instead access using py_obj_xxx functions

#define IS_O(o, k) (((((py_small_int_t)(o)) & 1) == 0) && (((py_obj_base_t*)(o))->kind == (k)))
#define IS_SMALL_INT(o) (((py_small_int_t)(o)) & 1)
#define FROM_SMALL_INT(o) (((py_small_int_t)(o)) >> 1)
#define TO_SMALL_INT(o) ((py_obj_t)(((o) << 1) | 1))

typedef enum {
    O_CONST,
    O_STR,
#if MICROPY_ENABLE_FLOAT
    O_FLOAT,
    O_COMPLEX,
#endif
    O_CELL,
    O_EXCEPTION_0,
    O_EXCEPTION_N,
    O_RANGE,
    O_RANGE_IT,
    O_FUN_0,
    O_FUN_1,
    O_FUN_2,
    O_FUN_N,
    O_FUN_VAR,
    O_FUN_BC,
    O_FUN_ASM,
    O_GEN_WRAP,
    O_GEN_INSTANCE,
    O_BOUND_METH,
    O_CLOSURE,
    O_TUPLE,
    O_LIST,
    O_TUPLE_IT,
    O_LIST_IT,
    O_SET,
    O_MAP,
    O_CLASS,
    O_OBJ,
    O_USER,
} py_obj_kind_t;

typedef struct _py_obj_base_t py_obj_base_t;

struct _py_obj_base_t {
    py_obj_kind_t kind;
    union {
        const char *id;
        qstr u_str;
#if MICROPY_ENABLE_FLOAT
        py_float_t u_float; // for O_FLOAT
        struct { // for O_COMPLEX
            py_float_t real;
            py_float_t imag;
        } u_complex;
#endif
        py_obj_t u_cell; // for O_CELL
        struct { // for O_EXCEPTION_0
            qstr id;
        } u_exc0;
        struct { // for O_EXCEPTION_N
            // TODO make generic object or something
            qstr id;
            int n_args;
            const void **args;
        } u_exc_n;
        struct { // for O_RANGE
            // TODO make generic object or something
            machine_int_t start;
            machine_int_t stop;
            machine_int_t step;
        } u_range;
        struct { // for O_RANGE_IT
            // TODO make generic object or something
            machine_int_t cur;
            machine_int_t stop;
            machine_int_t step;
        } u_range_it;
        struct { // for O_FUN_[012N], O_FUN_VAR
            int n_args;
            void *fun;
        } u_fun;
        struct { // for O_FUN_BC
            int n_args;
            uint n_state;
            byte *code;
        } u_fun_bc;
        struct { // for O_FUN_ASM
            int n_args;
            void *fun;
        } u_fun_asm;
        struct { // for O_GEN_WRAP
            int n_state;
            py_obj_base_t *fun;
        } u_gen_wrap;
        struct { // for O_GEN_INSTANCE
            py_obj_t *state;
            const byte *ip;
            py_obj_t *sp;
        } u_gen_instance;
        struct { // for O_BOUND_METH
            py_obj_t meth;
            py_obj_t self;
        } u_bound_meth;
        struct { // for O_CLOSURE
            py_obj_t fun;
            py_obj_t vars;
        } u_closure;
        struct { // for O_TUPLE, O_LIST
            machine_uint_t alloc;
            machine_uint_t len;
            py_obj_t *items;
        } u_tuple_list;
        struct { // for O_TUPLE_IT, O_LIST_IT
            py_obj_base_t *obj;
            machine_uint_t cur;
        } u_tuple_list_it;
        struct { // for O_SET
            machine_uint_t alloc;
            machine_uint_t used;
            py_obj_t *table;
        } u_set;
        py_map_t u_map; // for O_MAP
        struct { // for O_CLASS
            py_map_t *locals;
        } u_class;
        struct { // for O_OBJ
            py_obj_base_t *class; // points to a O_CLASS object
            py_map_t *members;
        } u_obj;
        struct { // for O_USER
            const py_user_info_t *info;
            machine_uint_t data1;
            machine_uint_t data2;
        } u_user;
    };
};

py_obj_t py_obj_new_tuple_iterator(py_obj_base_t *tuple, int cur);
py_obj_t py_obj_new_list_iterator(py_obj_base_t *list, int cur);
