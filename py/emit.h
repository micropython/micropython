//#define EMIT_DO_CPY
#define EMIT_DO_BC
//#define EMIT_DO_X64
//#define EMIT_DO_THUMB

/* Notes on passes:
 * We don't know exactly the opcodes in pass 1 because they depend on the
 * closing over of variables (LOAD_CLOSURE, BUILD_TUPLE, MAKE_CLOSURE), which
 * depends on determining the scope of variables in each function, and this
 * is not known until the end of pass 1.
 * As a consequence, we don't know the maximum stack size until the end of pass 2.
 * This is problematic for some emitters (x64) since they need to know the maximum
 * stack size to compile the entry to the function, and this effects code size.
 */

typedef enum {
    PASS_1 = 1, // work out id's and their kind, and number of labels
    PASS_2 = 2, // work out stack size and code size and label offsets
    PASS_3 = 3, // emit code
} pass_kind_t;

typedef struct _emitter_t emitter_t;

void emit_common_declare_global(pass_kind_t pass, scope_t *scope, qstr qstr);
void emit_common_declare_nonlocal(pass_kind_t pass, scope_t *scope, qstr qstr);
void emit_common_load_id(pass_kind_t pass, scope_t *scope, qstr qstr___class__, emitter_t *emit, qstr qstr);
void emit_common_store_id(pass_kind_t pass, scope_t *scope, emitter_t *emit, qstr qstr);
void emit_common_delete_id(pass_kind_t pass, scope_t *scope, emitter_t *emit, qstr qstr);

emitter_t *emit_new();
void emit_set_native_types(emitter_t *emit, bool do_native_types);
void emit_start_pass(emitter_t *emit, pass_kind_t pass, scope_t *scope);
void emit_end_pass(emitter_t *emit);
bool emit_last_emit_was_return_value(emitter_t *emit);
int emit_get_stack_size(emitter_t *emit);
void emit_set_stack_size(emitter_t *emit, int size);

int emit_label_new(emitter_t *emit);
void emit_label_assign(emitter_t *emit, int l);
void emit_import_name(emitter_t *emit, qstr qstr);
void emit_import_from(emitter_t *emit, qstr qstr);
void emit_import_star(emitter_t *emit);
void emit_load_const_tok(emitter_t *emit, py_token_kind_t tok);
void emit_load_const_small_int(emitter_t *emit, int arg);
void emit_load_const_int(emitter_t *emit, qstr qstr);
void emit_load_const_dec(emitter_t *emit, qstr qstr);
void emit_load_const_id(emitter_t *emit, qstr qstr);
void emit_load_const_str(emitter_t *emit, qstr qstr, bool bytes);
void emit_load_const_verbatim_start(emitter_t *emit);
void emit_load_const_verbatim_int(emitter_t *emit, int val);
void emit_load_const_verbatim_str(emitter_t *emit, const char *str);
void emit_load_const_verbatim_strn(emitter_t *emit, const char *str, int len);
void emit_load_const_verbatim_quoted_str(emitter_t *emit, qstr qstr, bool bytes);
void emit_load_const_verbatim_end(emitter_t *emit);
void emit_load_fast(emitter_t *emit, qstr qstr, int local_num);
void emit_load_name(emitter_t *emit, qstr qstr);
void emit_load_global(emitter_t *emit, qstr qstr);
void emit_load_deref(emitter_t *emit, qstr qstr);
void emit_load_closure(emitter_t *emit, qstr qstr);
void emit_load_attr(emitter_t *emit, qstr qstr);
void emit_load_method(emitter_t *emit, qstr qstr);
void emit_load_build_class(emitter_t *emit);
void emit_store_fast(emitter_t *emit, qstr qstr, int local_num);
void emit_store_name(emitter_t *emit, qstr qstr);
void emit_store_global(emitter_t *emit, qstr qstr);
void emit_store_deref(emitter_t *emit, qstr qstr);
void emit_store_attr(emitter_t *emit, qstr qstr);
void emit_store_locals(emitter_t *emit);
void emit_store_subscr(emitter_t *emit);
void emit_delete_fast(emitter_t *emit, qstr qstr, int local_num);
void emit_delete_name(emitter_t *emit, qstr qstr);
void emit_delete_global(emitter_t *emit, qstr qstr);
void emit_delete_deref(emitter_t *emit, qstr qstr);
void emit_delete_attr(emitter_t *emit, qstr qstr);
void emit_delete_subscr(emitter_t *emit);
void emit_dup_top(emitter_t *emit);
void emit_dup_top_two(emitter_t *emit);
void emit_pop_top(emitter_t *emit);
void emit_rot_two(emitter_t *emit);
void emit_rot_three(emitter_t *emit);
void emit_jump(emitter_t *emit, int label);
void emit_pop_jump_if_true(emitter_t *emit, int label);
void emit_pop_jump_if_false(emitter_t *emit, int label);
void emit_jump_if_true_or_pop(emitter_t *emit, int label);
void emit_jump_if_false_or_pop(emitter_t *emit, int label);
void emit_setup_loop(emitter_t *emit, int label);
void emit_break_loop(emitter_t *emit, int label);
void emit_continue_loop(emitter_t *emit, int label);
void emit_setup_with(emitter_t *emit, int label);
void emit_with_cleanup(emitter_t *emit);
void emit_setup_except(emitter_t *emit, int label);
void emit_setup_finally(emitter_t *emit, int label);
void emit_end_finally(emitter_t *emit);
void emit_get_iter(emitter_t *emit); // tos = getiter(tos)
void emit_for_iter(emitter_t *emit, int label);
void emit_for_iter_end(emitter_t *emit);
void emit_pop_block(emitter_t *emit);
void emit_pop_except(emitter_t *emit);
void emit_unary_op(emitter_t *emit, rt_unary_op_t op);
void emit_binary_op(emitter_t *emit, rt_binary_op_t op);
void emit_compare_op(emitter_t *emit, rt_compare_op_t op);
void emit_build_tuple(emitter_t *emit, int n_args);
void emit_build_list(emitter_t *emit, int n_args);
void emit_list_append(emitter_t *emit, int list_stack_index);
void emit_build_map(emitter_t *emit, int n_args);
void emit_store_map(emitter_t *emit);
void emit_map_add(emitter_t *emit, int map_stack_index);
void emit_build_set(emitter_t *emit, int n_args);
void emit_set_add(emitter_t *emit, int set_stack_index);
void emit_build_slice(emitter_t *emit, int n_args);
void emit_unpack_sequence(emitter_t *emit, int n_args);
void emit_unpack_ex(emitter_t *emit, int n_left, int n_right);
void emit_make_function(emitter_t *emit, scope_t *scope, int n_dict_params, int n_default_params);
void emit_make_closure(emitter_t *emit, scope_t *scope, int n_dict_params, int n_default_params);
void emit_call_function(emitter_t *emit, int n_positional, int n_keyword, bool have_star_arg, bool have_dbl_star_arg);
void emit_call_method(emitter_t *emit, int n_positional, int n_keyword, bool have_star_arg, bool have_dbl_star_arg);
void emit_return_value(emitter_t *emit);
void emit_raise_varargs(emitter_t *emit, int n_args);
void emit_yield_value(emitter_t *emit);
void emit_yield_from(emitter_t *emit);
