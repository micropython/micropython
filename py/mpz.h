typedef uint16_t mpz_dig_t;
typedef uint32_t mpz_dbl_dig_t;
typedef int32_t mpz_dbl_dig_signed_t;

typedef struct _mpz_t {
    struct {
        machine_uint_t neg : 1;
        machine_uint_t alloc : 31;
    };
    machine_uint_t len;
    mpz_dig_t *dig;
} mpz_t;

bool mpz_int_is_sml_int(int i);

void mpz_init_zero(mpz_t *z);
void mpz_init_from_int(mpz_t *z, machine_int_t val);
void mpz_deinit(mpz_t *z);

mpz_t *mpz_zero();
mpz_t *mpz_from_int(machine_int_t i);
mpz_t *mpz_from_str(const char *str, uint len, bool neg, uint base);
void mpz_free(mpz_t *z);

mpz_t *mpz_clone(const mpz_t *src);

void mpz_set(mpz_t *dest, const mpz_t *src);
void mpz_set_from_int(mpz_t *z, machine_int_t src);
uint mpz_set_from_str(mpz_t *z, const char *str, uint len, bool neg, uint base);

bool mpz_is_zero(const mpz_t *z);
bool mpz_is_pos(const mpz_t *z);
bool mpz_is_neg(const mpz_t *z);
bool mpz_is_odd(const mpz_t *z);
bool mpz_is_even(const mpz_t *z);

int mpz_cmp(const mpz_t *lhs, const mpz_t *rhs);
int mpz_cmp_sml_int(const mpz_t *lhs, int sml_int);

mpz_t *mpz_abs(const mpz_t *z);
mpz_t *mpz_neg(const mpz_t *z);
mpz_t *mpz_add(const mpz_t *lhs, const mpz_t *rhs);
mpz_t *mpz_sub(const mpz_t *lhs, const mpz_t *rhs);
mpz_t *mpz_mul(const mpz_t *lhs, const mpz_t *rhs);
mpz_t *mpz_pow(const mpz_t *lhs, const mpz_t *rhs);

void mpz_abs_inpl(mpz_t *dest, const mpz_t *z);
void mpz_neg_inpl(mpz_t *dest, const mpz_t *z);
void mpz_add_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_sub_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_mul_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_pow_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);

mpz_t *mpz_gcd(const mpz_t *z1, const mpz_t *z2);
mpz_t *mpz_lcm(const mpz_t *z1, const mpz_t *z2);
void mpz_divmod(const mpz_t *lhs, const mpz_t *rhs, mpz_t **quo, mpz_t **rem);
void mpz_divmod_inpl(mpz_t *dest_quo, mpz_t *dest_rem, const mpz_t *lhs, const mpz_t *rhs);
mpz_t *mpz_div(const mpz_t *lhs, const mpz_t *rhs);
mpz_t *mpz_mod(const mpz_t *lhs, const mpz_t *rhs);

int mpz_as_int(const mpz_t *z);
machine_float_t mpz_as_float(const mpz_t *z);
uint mpz_as_str_size(const mpz_t *z, uint base);
char *mpz_as_str(const mpz_t *z, uint base);
uint mpz_as_str_inpl(const mpz_t *z, uint base, char *str);
