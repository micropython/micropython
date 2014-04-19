typedef struct _mp_obj_fun_bc_t {
    mp_obj_base_t base;
    mp_obj_dict_t *globals; // the context within which this function was defined
    machine_uint_t n_args : 15;         // number of arguments this function takes
    machine_uint_t n_def_args : 15;     // number of default arguments
    machine_uint_t takes_var_args : 1;  // set if this function takes variable args
    machine_uint_t takes_kw_args : 1;   // set if this function takes keyword args
    const byte *bytecode;   // bytecode for the function
    qstr *args;             // argument names (needed to resolve positional args passed as keywords)
    // values of default args (if any), plus a slot at the end for var args and/or kw args (if it takes them)
    mp_obj_t extra_args[];
} mp_obj_fun_bc_t;
