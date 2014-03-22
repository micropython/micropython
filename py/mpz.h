typedef uint16_t mpz_dig_t;
typedef uint32_t mpz_dbl_dig_t;
typedef int32_t mpz_dbl_dig_signed_t;

typedef struct _mpz_t {
    machine_uint_t neg : 1;
    machine_uint_t fixed_dig : 1;
    machine_uint_t alloc : 30;
    machine_uint_t len;
    mpz_dig_t *dig;
} mpz_t;

#define MPZ_DIG_SIZE (15) // see mpn_div for why this needs to be at most 15
#define MPZ_NUM_DIG_FOR_INT (sizeof(machine_int_t) * 8 / MPZ_DIG_SIZE + 1)
#define MPZ_NUM_DIG_FOR_LL (sizeof(long long) * 8 / MPZ_DIG_SIZE + 1)

// convenience macro to declare an mpz with a digit array from the stack, initialised by an integer
#define MPZ_CONST_INT(z, val) mpz_t z; mpz_dig_t z ## _digits[MPZ_NUM_DIG_FOR_INT]; mpz_init_fixed_from_int(&z, z_digits, MPZ_NUM_DIG_FOR_INT, val);

void mpz_init_zero(mpz_t *z);
void mpz_init_from_int(mpz_t *z, machine_int_t val);
void mpz_init_fixed_from_int(mpz_t *z, mpz_dig_t *dig, uint dig_alloc, machine_int_t val);
void mpz_deinit(mpz_t *z);

mpz_t *mpz_zero();
mpz_t *mpz_from_int(machine_int_t i);
mpz_t *mpz_from_ll(long long i);
mpz_t *mpz_from_str(const char *str, uint len, bool neg, uint base);
void mpz_free(mpz_t *z);

mpz_t *mpz_clone(const mpz_t *src);

void mpz_set(mpz_t *dest, const mpz_t *src);
void mpz_set_from_int(mpz_t *z, machine_int_t src);
void mpz_set_from_ll(mpz_t *z, long long i);
uint mpz_set_from_str(mpz_t *z, const char *str, uint len, bool neg, uint base);

bool mpz_is_zero(const mpz_t *z);
bool mpz_is_pos(const mpz_t *z);
bool mpz_is_neg(const mpz_t *z);
bool mpz_is_odd(const mpz_t *z);
bool mpz_is_even(const mpz_t *z);

int mpz_cmp(const mpz_t *lhs, const mpz_t *rhs);

mpz_t *mpz_abs(const mpz_t *z);
mpz_t *mpz_neg(const mpz_t *z);
mpz_t *mpz_add(const mpz_t *lhs, const mpz_t *rhs);
mpz_t *mpz_sub(const mpz_t *lhs, const mpz_t *rhs);
mpz_t *mpz_mul(const mpz_t *lhs, const mpz_t *rhs);
mpz_t *mpz_pow(const mpz_t *lhs, const mpz_t *rhs);

void mpz_abs_inpl(mpz_t *dest, const mpz_t *z);
void mpz_neg_inpl(mpz_t *dest, const mpz_t *z);
void mpz_not_inpl(mpz_t *dest, const mpz_t *z);
void mpz_shl_inpl(mpz_t *dest, const mpz_t *lhs, machine_int_t rhs);
void mpz_shr_inpl(mpz_t *dest, const mpz_t *lhs, machine_int_t rhs);
void mpz_add_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_sub_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_mul_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_pow_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_and_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_or_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_xor_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);

mpz_t *mpz_gcd(const mpz_t *z1, const mpz_t *z2);
mpz_t *mpz_lcm(const mpz_t *z1, const mpz_t *z2);
void mpz_divmod(const mpz_t *lhs, const mpz_t *rhs, mpz_t **quo, mpz_t **rem);
void mpz_divmod_inpl(mpz_t *dest_quo, mpz_t *dest_rem, const mpz_t *lhs, const mpz_t *rhs);
mpz_t *mpz_div(const mpz_t *lhs, const mpz_t *rhs);
mpz_t *mpz_mod(const mpz_t *lhs, const mpz_t *rhs);

machine_int_t mpz_as_int(const mpz_t *z);
#if MICROPY_ENABLE_FLOAT
mp_float_t mpz_as_float(const mpz_t *z);
#endif
uint mpz_as_str_size(const mpz_t *z, uint base);
char *mpz_as_str(const mpz_t *z, uint base);
uint mpz_as_str_inpl(const mpz_t *z, uint base, char *str);
