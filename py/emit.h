/* Notes on passes:
 * We don't know exactly the opcodes in pass 1 because they depend on the
 * closing over of variables (LOAD_CLOSURE, BUILD_TUPLE, MAKE_CLOSURE), which
 * depends on determining the scope of variables in each function, and this
 * is not known until the end of pass 1.
 * As a consequence, we don't know the maximum stack size until the end of pass 2.
 * This is problematic for some emitters (x64) since they need to know the maximum
 * stack size to compile the entry to the function, and this affects code size.
 */

typedef enum {
    PASS_1 = 1, // work out id's and their kind, and number of labels
    PASS_2 = 2, // work out stack size and code size and label offsets
    PASS_3 = 3, // emit code
} pass_kind_t;

typedef struct _emit_t emit_t;

typedef struct _emit_method_table_t {
    void (*set_native_types)(emit_t *emit, bool do_native_types);
    void (*start_pass)(emit_t *emit, pass_kind_t pass, scope_t *scope);
    void (*end_pass)(emit_t *emit);
    bool (*last_emit_was_return_value)(emit_t *emit);
    int (*get_stack_size)(emit_t *emit);
    void (*set_stack_size)(emit_t *emit, int size);
    void (*set_line_number)(emit_t *emit, int line);

    void (*load_id)(emit_t *emit, qstr qstr);
    void (*store_id)(emit_t *emit, qstr qstr);
    void (*delete_id)(emit_t *emit, qstr qstr);

    void (*label_assign)(emit_t *emit, int l);
    void (*import_name)(emit_t *emit, qstr qstr);
    void (*import_from)(emit_t *emit, qstr qstr);
    void (*import_star)(emit_t *emit);
    void (*load_const_tok)(emit_t *emit, mp_token_kind_t tok);
    void (*load_const_small_int)(emit_t *emit, machine_int_t arg);
    void (*load_const_int)(emit_t *emit, qstr qstr);
    void (*load_const_dec)(emit_t *emit, qstr qstr);
    void (*load_const_id)(emit_t *emit, qstr qstr);
    void (*load_const_str)(emit_t *emit, qstr qstr, bool bytes);
    void (*load_const_verbatim_str)(emit_t *emit, const char *str); // only needed for emitcpy
    void (*load_fast)(emit_t *emit, qstr qstr, int local_num);
    void (*load_deref)(emit_t *emit, qstr qstr, int local_num);
    void (*load_closure)(emit_t *emit, qstr qstr, int local_num);
    void (*load_name)(emit_t *emit, qstr qstr);
    void (*load_global)(emit_t *emit, qstr qstr);
    void (*load_attr)(emit_t *emit, qstr qstr);
    void (*load_method)(emit_t *emit, qstr qstr);
    void (*load_build_class)(emit_t *emit);
    void (*store_fast)(emit_t *emit, qstr qstr, int local_num);
    void (*store_deref)(emit_t *emit, qstr qstr, int local_num);
    void (*store_name)(emit_t *emit, qstr qstr);
    void (*store_global)(emit_t *emit, qstr qstr);
    void (*store_attr)(emit_t *emit, qstr qstr);
    void (*store_subscr)(emit_t *emit);
    void (*delete_fast)(emit_t *emit, qstr qstr, int local_num);
    void (*delete_deref)(emit_t *emit, qstr qstr, int local_num);
    void (*delete_name)(emit_t *emit, qstr qstr);
    void (*delete_global)(emit_t *emit, qstr qstr);
    void (*delete_attr)(emit_t *emit, qstr qstr);
    void (*delete_subscr)(emit_t *emit);
    void (*dup_top)(emit_t *emit);
    void (*dup_top_two)(emit_t *emit);
    void (*pop_top)(emit_t *emit);
    void (*rot_two)(emit_t *emit);
    void (*rot_three)(emit_t *emit);
    void (*jump)(emit_t *emit, int label);
    void (*pop_jump_if_true)(emit_t *emit, int label);
    void (*pop_jump_if_false)(emit_t *emit, int label);
    void (*jump_if_true_or_pop)(emit_t *emit, int label);
    void (*jump_if_false_or_pop)(emit_t *emit, int label);
    void (*setup_loop)(emit_t *emit, int label);
    void (*break_loop)(emit_t *emit, int label, int except_depth);
    void (*continue_loop)(emit_t *emit, int label, int except_depth);
    void (*setup_with)(emit_t *emit, int label);
    void (*with_cleanup)(emit_t *emit);
    void (*setup_except)(emit_t *emit, int label);
    void (*setup_finally)(emit_t *emit, int label);
    void (*end_finally)(emit_t *emit);
    void (*get_iter)(emit_t *emit);
    void (*for_iter)(emit_t *emit, int label);
    void (*for_iter_end)(emit_t *emit);
    void (*pop_block)(emit_t *emit);
    void (*pop_except)(emit_t *emit);
    void (*unary_op)(emit_t *emit, mp_unary_op_t op);
    void (*binary_op)(emit_t *emit, mp_binary_op_t op);
    void (*build_tuple)(emit_t *emit, int n_args);
    void (*build_list)(emit_t *emit, int n_args);
    void (*list_append)(emit_t *emit, int list_stack_index);
    void (*build_map)(emit_t *emit, int n_args);
    void (*store_map)(emit_t *emit);
    void (*map_add)(emit_t *emit, int map_stack_index);
    void (*build_set)(emit_t *emit, int n_args);
    void (*set_add)(emit_t *emit, int set_stack_index);
    void (*build_slice)(emit_t *emit, int n_args);
    void (*unpack_sequence)(emit_t *emit, int n_args);
    void (*unpack_ex)(emit_t *emit, int n_left, int n_right);
    void (*make_function)(emit_t *emit, scope_t *scope, uint n_pos_defaults, uint n_kw_defaults);
    void (*make_closure)(emit_t *emit, scope_t *scope, uint n_pos_defaults, uint n_kw_defaults);
    void (*call_function)(emit_t *emit, int n_positional, int n_keyword, bool have_star_arg, bool have_dbl_star_arg);
    void (*call_method)(emit_t *emit, int n_positional, int n_keyword, bool have_star_arg, bool have_dbl_star_arg);
    void (*return_value)(emit_t *emit);
    void (*raise_varargs)(emit_t *emit, int n_args);
    void (*yield_value)(emit_t *emit);
    void (*yield_from)(emit_t *emit);
} emit_method_table_t;

void emit_common_load_id(emit_t *emit, const emit_method_table_t *emit_method_table, scope_t *scope, qstr qstr);
void emit_common_store_id(emit_t *emit, const emit_method_table_t *emit_method_table, scope_t *scope, qstr qstr);
void emit_common_delete_id(emit_t *emit, const emit_method_table_t *emit_method_table, scope_t *scope, qstr qstr);

extern const emit_method_table_t emit_pass1_method_table;
extern const emit_method_table_t emit_cpython_method_table;
extern const emit_method_table_t emit_bc_method_table;
extern const emit_method_table_t emit_native_x64_method_table;
extern const emit_method_table_t emit_native_thumb_method_table;

emit_t *emit_pass1_new(void);
emit_t *emit_cpython_new(uint max_num_labels);
emit_t *emit_bc_new(uint max_num_labels);
emit_t *emit_native_x64_new(uint max_num_labels);
emit_t *emit_native_thumb_new(uint max_num_labels);

void emit_pass1_free(emit_t *emit);
void emit_bc_free(emit_t *emit);
void emit_native_x64_free(emit_t *emit);
void emit_native_thumb_free(emit_t *emit);

typedef struct _emit_inline_asm_t emit_inline_asm_t;

typedef struct _emit_inline_asm_method_table_t {
    void (*start_pass)(emit_inline_asm_t *emit, pass_kind_t pass, scope_t *scope);
    void (*end_pass)(emit_inline_asm_t *emit);
    int (*count_params)(emit_inline_asm_t *emit, int n_params, mp_parse_node_t *pn_params);
    void (*label)(emit_inline_asm_t *emit, int label_num, qstr label_id);
    void (*op)(emit_inline_asm_t *emit, qstr op, int n_args, mp_parse_node_t *pn_args);
} emit_inline_asm_method_table_t;

extern const emit_inline_asm_method_table_t emit_inline_thumb_method_table;

emit_inline_asm_t *emit_inline_thumb_new(uint max_num_labels);
void emit_inline_thumb_free(emit_inline_asm_t *emit);

