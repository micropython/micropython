typedef struct _mp_obj_int_t {
    mp_obj_base_t base;
#if MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_LONGLONG
    mp_longint_impl_t val;
#elif MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_MPZ
    mpz_t mpz;
#endif
} mp_obj_int_t;

void mp_obj_int_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind);
char *mp_obj_int_formatted(char **buf, int *buf_size, int *fmt_size, mp_obj_t self_in,
                           int base, const char *prefix, char base_char, char comma);
char *mp_obj_int_formatted_impl(char **buf, int *buf_size, int *fmt_size, mp_obj_t self_in,
                                int base, const char *prefix, char base_char, char comma);
bool mp_obj_int_is_positive(mp_obj_t self_in);
mp_obj_t mp_obj_int_unary_op(int op, mp_obj_t o_in);
mp_obj_t mp_obj_int_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in);
mp_obj_t mp_obj_int_binary_op_extra_cases(int op, mp_obj_t lhs_in, mp_obj_t rhs_in);
