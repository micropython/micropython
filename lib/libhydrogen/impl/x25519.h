/*
 * Based on Michael Hamburg's STROBE reference implementation.
 * Copyright (c) 2015-2016 Cryptography Research, Inc.
 * MIT License (MIT)
 */

#if defined(__GNUC__) && defined(__SIZEOF_INT128__)
#define hydro_x25519_WBITS 64
#else
#define hydro_x25519_WBITS 32
#endif

#if hydro_x25519_WBITS == 64
typedef uint64_t    hydro_x25519_limb_t;
typedef __uint128_t hydro_x25519_dlimb_t;
typedef __int128_t  hydro_x25519_sdlimb_t;
#define hydro_x25519_eswap_limb(X) LOAD64_LE((const uint8_t *) &(X))
#define hydro_x25519_LIMB(x) x##ull
#elif hydro_x25519_WBITS == 32
typedef uint32_t hydro_x25519_limb_t;
typedef uint64_t hydro_x25519_dlimb_t;
typedef int64_t  hydro_x25519_sdlimb_t;
#define hydro_x25519_eswap_limb(X) LOAD32_LE((const uint8_t *) &(X))
#define hydro_x25519_LIMB(x) (uint32_t)(x##ull), (uint32_t)((x##ull) >> 32)
#else
#error "Need to know hydro_x25519_WBITS"
#endif

#define hydro_x25519_NLIMBS (256 / hydro_x25519_WBITS)
typedef hydro_x25519_limb_t hydro_x25519_fe[hydro_x25519_NLIMBS];

typedef hydro_x25519_limb_t hydro_x25519_scalar_t[hydro_x25519_NLIMBS];

static const hydro_x25519_limb_t hydro_x25519_MONTGOMERY_FACTOR =
    (hydro_x25519_limb_t) 0xd2b51da312547e1bull;

static const hydro_x25519_scalar_t hydro_x25519_sc_p = { hydro_x25519_LIMB(0x5812631a5cf5d3ed),
                                                         hydro_x25519_LIMB(0x14def9dea2f79cd6),
                                                         hydro_x25519_LIMB(0x0000000000000000),
                                                         hydro_x25519_LIMB(0x1000000000000000) };

static const hydro_x25519_scalar_t hydro_x25519_sc_r2 = { hydro_x25519_LIMB(0xa40611e3449c0f01),
                                                          hydro_x25519_LIMB(0xd00e1ba768859347),
                                                          hydro_x25519_LIMB(0xceec73d217f5be65),
                                                          hydro_x25519_LIMB(0x0399411b7c309a3d) };

static const uint8_t hydro_x25519_BASE_POINT[hydro_x25519_BYTES] = { 9 };

static const hydro_x25519_limb_t hydro_x25519_a24[1] = { 121665 };

static inline hydro_x25519_limb_t
hydro_x25519_umaal(hydro_x25519_limb_t *carry, hydro_x25519_limb_t acc, hydro_x25519_limb_t mand,
                   hydro_x25519_limb_t mier)
{
    hydro_x25519_dlimb_t tmp = (hydro_x25519_dlimb_t) mand * mier + acc + *carry;

    *carry = tmp >> hydro_x25519_WBITS;
    return (hydro_x25519_limb_t) tmp;
}

static inline hydro_x25519_limb_t
hydro_x25519_adc(hydro_x25519_limb_t *carry, hydro_x25519_limb_t acc, hydro_x25519_limb_t mand)
{
    hydro_x25519_dlimb_t total = (hydro_x25519_dlimb_t) *carry + acc + mand;

    *carry = total >> hydro_x25519_WBITS;
    return (hydro_x25519_limb_t) total;
}

static inline hydro_x25519_limb_t
hydro_x25519_adc0(hydro_x25519_limb_t *carry, hydro_x25519_limb_t acc)
{
    hydro_x25519_dlimb_t total = (hydro_x25519_dlimb_t) *carry + acc;

    *carry = total >> hydro_x25519_WBITS;
    return (hydro_x25519_limb_t) total;
}

static void
hydro_x25519_propagate(hydro_x25519_fe x, hydro_x25519_limb_t over)
{
    hydro_x25519_limb_t carry;
    int                 i;

    over = x[hydro_x25519_NLIMBS - 1] >> (hydro_x25519_WBITS - 1) | over << 1;
    x[hydro_x25519_NLIMBS - 1] &= ~((hydro_x25519_limb_t) 1 << (hydro_x25519_WBITS - 1));
    carry = over * 19;
    for (i = 0; i < hydro_x25519_NLIMBS; i++) {
        x[i] = hydro_x25519_adc0(&carry, x[i]);
    }
}

static void
hydro_x25519_add(hydro_x25519_fe out, const hydro_x25519_fe a, const hydro_x25519_fe b)
{
    hydro_x25519_limb_t carry = 0;
    int                 i;

    for (i = 0; i < hydro_x25519_NLIMBS; i++) {
        out[i] = hydro_x25519_adc(&carry, a[i], b[i]);
    }
    hydro_x25519_propagate(out, carry);
}

static void
hydro_x25519_sub(hydro_x25519_fe out, const hydro_x25519_fe a, const hydro_x25519_fe b)
{
    hydro_x25519_sdlimb_t carry = -38;
    int                   i;

    for (i = 0; i < hydro_x25519_NLIMBS; i++) {
        out[i] = (hydro_x25519_limb_t)(carry = carry + a[i] - b[i]);
        carry >>= hydro_x25519_WBITS;
    }
    hydro_x25519_propagate(out, (hydro_x25519_limb_t)(1 + carry));
}

static void
hydro_x25519_swapin(hydro_x25519_limb_t *x, const uint8_t *in)
{
    int i;

    memcpy(x, in, sizeof(hydro_x25519_fe));
    for (i = 0; i < hydro_x25519_NLIMBS; i++) {
        x[i] = hydro_x25519_eswap_limb(x[i]);
    }
}

static void
hydro_x25519_swapout(uint8_t *out, hydro_x25519_limb_t *x)
{
    int i;

    for (i = 0; i < hydro_x25519_NLIMBS; i++) {
        x[i] = hydro_x25519_eswap_limb(x[i]);
    }
    memcpy(out, x, sizeof(hydro_x25519_fe));
}

static void
hydro_x25519_mul(hydro_x25519_fe out, const hydro_x25519_fe a, const hydro_x25519_fe b, int nb)
{
    hydro_x25519_limb_t accum[2 * hydro_x25519_NLIMBS] = { 0 };
    hydro_x25519_limb_t carry2;
    int                 i, j;

    for (i = 0; i < nb; i++) {
        carry2                   = 0;
        hydro_x25519_limb_t mand = b[i];
        for (j = 0; j < hydro_x25519_NLIMBS; j++) {
            accum[i + j] = hydro_x25519_umaal(&carry2, accum[i + j], mand, a[j]);
        }
        accum[i + j] = carry2;
    }
    carry2 = 0;
    for (j = 0; j < hydro_x25519_NLIMBS; j++) {
        const hydro_x25519_limb_t mand = 38;

        out[j] = hydro_x25519_umaal(&carry2, accum[j], mand, accum[j + hydro_x25519_NLIMBS]);
    }
    hydro_x25519_propagate(out, carry2);
}

static void
hydro_x25519_sqr(hydro_x25519_fe out, const hydro_x25519_fe a)
{
    hydro_x25519_mul(out, a, a, hydro_x25519_NLIMBS);
}

static void
hydro_x25519_mul1(hydro_x25519_fe out, const hydro_x25519_fe a)
{
    hydro_x25519_mul(out, a, out, hydro_x25519_NLIMBS);
}

static void
hydro_x25519_sqr1(hydro_x25519_fe a)
{
    hydro_x25519_mul1(a, a);
}

static void
hydro_x25519_condswap(hydro_x25519_limb_t a[2 * hydro_x25519_NLIMBS],
                      hydro_x25519_limb_t b[2 * hydro_x25519_NLIMBS], hydro_x25519_limb_t doswap)
{
    int i;

    for (i = 0; i < 2 * hydro_x25519_NLIMBS; i++) {
        hydro_x25519_limb_t xorv = (a[i] ^ b[i]) & doswap;
        a[i] ^= xorv;
        b[i] ^= xorv;
    }
}

static int
hydro_x25519_canon(hydro_x25519_fe x)
{
    hydro_x25519_sdlimb_t carry;
    hydro_x25519_limb_t   carry0 = 19;
    hydro_x25519_limb_t   res;
    int                   i;

    for (i = 0; i < hydro_x25519_NLIMBS; i++) {
        x[i] = hydro_x25519_adc0(&carry0, x[i]);
    }
    hydro_x25519_propagate(x, carry0);
    carry = -19;
    res   = 0;
    for (i = 0; i < hydro_x25519_NLIMBS; i++) {
        res |= x[i] = (hydro_x25519_limb_t)(carry += x[i]);
        carry >>= hydro_x25519_WBITS;
    }
    return ((hydro_x25519_dlimb_t) res - 1) >> hydro_x25519_WBITS;
}

static void
hydro_x25519_ladder_part1(hydro_x25519_fe xs[5])
{
    hydro_x25519_limb_t *x2 = xs[0], *z2 = xs[1], *x3 = xs[2], *z3 = xs[3], *t1 = xs[4];

    hydro_x25519_add(t1, x2, z2);              // t1 = A
    hydro_x25519_sub(z2, x2, z2);              // z2 = B
    hydro_x25519_add(x2, x3, z3);              // x2 = C
    hydro_x25519_sub(z3, x3, z3);              // z3 = D
    hydro_x25519_mul1(z3, t1);                 // z3 = DA
    hydro_x25519_mul1(x2, z2);                 // x3 = BC
    hydro_x25519_add(x3, z3, x2);              // x3 = DA+CB
    hydro_x25519_sub(z3, z3, x2);              // z3 = DA-CB
    hydro_x25519_sqr1(t1);                     // t1 = AA
    hydro_x25519_sqr1(z2);                     // z2 = BB
    hydro_x25519_sub(x2, t1, z2);              // x2 = E = AA-BB
    hydro_x25519_mul(z2, x2, hydro_x25519_a24, // z2 = E*a24
                     sizeof(hydro_x25519_a24) / sizeof(hydro_x25519_a24[0]));
    hydro_x25519_add(z2, z2, t1); // z2 = E*a24 + AA
}

static void
hydro_x25519_ladder_part2(hydro_x25519_fe xs[5], const hydro_x25519_fe x1)
{
    hydro_x25519_limb_t *x2 = xs[0], *z2 = xs[1], *x3 = xs[2], *z3 = xs[3], *t1 = xs[4];

    hydro_x25519_sqr1(z3);        // z3 = (DA-CB)^2
    hydro_x25519_mul1(z3, x1);    // z3 = x1 * (DA-CB)^2
    hydro_x25519_sqr1(x3);        // x3 = (DA+CB)^2
    hydro_x25519_mul1(z2, x2);    // z2 = AA*(E*a24+AA)
    hydro_x25519_sub(x2, t1, x2); // x2 = BB again
    hydro_x25519_mul1(x2, t1);    // x2 = AA*BB
}

static void
hydro_x25519_core(hydro_x25519_fe xs[5], const uint8_t scalar[hydro_x25519_BYTES],
                  const uint8_t *x1, bool clamp)
{
    hydro_x25519_limb_t  swap;
    hydro_x25519_limb_t *x2 = xs[0], *x3 = xs[2], *z3 = xs[3];
    hydro_x25519_fe      x1i;
    int                  i;

    hydro_x25519_swapin(x1i, x1);
    x1   = (const uint8_t *) x1i;
    swap = 0;
    mem_zero(xs, 4 * sizeof(hydro_x25519_fe));
    x2[0] = z3[0] = 1;
    memcpy(x3, x1, sizeof(hydro_x25519_fe));
    for (i = 255; i >= 0; i--) {
        uint8_t             bytei = scalar[i / 8];
        hydro_x25519_limb_t doswap;
        hydro_x25519_fe     x1_dup;

        if (clamp) {
            if (i / 8 == 0) {
                bytei &= ~7;
            } else if (i / 8 == hydro_x25519_BYTES - 1) {
                bytei &= 0x7F;
                bytei |= 0x40;
            }
        }
        doswap = 1U + ~(hydro_x25519_limb_t)((bytei >> (i % 8)) & 1);
        hydro_x25519_condswap(x2, x3, swap ^ doswap);
        swap = doswap;
        hydro_x25519_ladder_part1(xs);
        memcpy(x1_dup, x1, sizeof x1_dup);
        hydro_x25519_ladder_part2(xs, x1_dup);
    }
    hydro_x25519_condswap(x2, x3, swap);
}

static int
hydro_x25519_scalarmult(uint8_t       out[hydro_x25519_BYTES],
                        const uint8_t scalar[hydro_x25519_SECRETKEYBYTES],
                        const uint8_t x1[hydro_x25519_PUBLICKEYBYTES], bool clamp)
{
    hydro_x25519_fe      xs[5];
    hydro_x25519_limb_t *x2, *z2, *z3;
    hydro_x25519_limb_t *prev;
    int                  i;
    int                  ret;

    hydro_x25519_core(xs, scalar, x1, clamp);

    /* Precomputed inversion chain */
    x2   = xs[0];
    z2   = xs[1];
    z3   = xs[3];
    prev = z2;

    /* Raise to the p-2 = 0x7f..ffeb */
    for (i = 253; i >= 0; i--) {
        hydro_x25519_sqr(z3, prev);
        prev = z3;
        if (i >= 8 || (0xeb >> i & 1)) {
            hydro_x25519_mul1(z3, z2);
        }
    }

    /* Here prev = z3 */
    /* x2 /= z2 */
    hydro_x25519_mul1(x2, z3);
    ret = hydro_x25519_canon(x2);
    hydro_x25519_swapout(out, x2);

    if (clamp == 0) {
        return 0;
    }
    return ret;
}

static inline int
hydro_x25519_scalarmult_base(uint8_t       pk[hydro_x25519_PUBLICKEYBYTES],
                             const uint8_t sk[hydro_x25519_SECRETKEYBYTES])
{
    return hydro_x25519_scalarmult(pk, sk, hydro_x25519_BASE_POINT, 1);
}

static inline void
hydro_x25519_scalarmult_base_uniform(uint8_t       pk[hydro_x25519_PUBLICKEYBYTES],
                                     const uint8_t sk[hydro_x25519_SECRETKEYBYTES])
{
    if (hydro_x25519_scalarmult(pk, sk, hydro_x25519_BASE_POINT, 0) != 0) {
        abort();
    }
}

static void
hydro_x25519_sc_montmul(hydro_x25519_scalar_t out, const hydro_x25519_scalar_t a,
                        const hydro_x25519_scalar_t b)
{
    hydro_x25519_limb_t hic = 0;
    int                 i, j;

    for (i = 0; i < hydro_x25519_NLIMBS; i++) {
        hydro_x25519_limb_t carry = 0, carry2 = 0, mand = a[i],
                            mand2 = hydro_x25519_MONTGOMERY_FACTOR;

        for (j = 0; j < hydro_x25519_NLIMBS; j++) {
            hydro_x25519_limb_t acc = out[j];

            acc = hydro_x25519_umaal(&carry, acc, mand, b[j]);
            if (j == 0) {
                mand2 *= acc;
            }
            acc = hydro_x25519_umaal(&carry2, acc, mand2, hydro_x25519_sc_p[j]);
            if (j > 0) {
                out[j - 1] = acc;
            }
        }

        /* Add two carry registers and high carry */
        out[hydro_x25519_NLIMBS - 1] = hydro_x25519_adc(&hic, carry, carry2);
    }

    /* Reduce */
    hydro_x25519_sdlimb_t scarry = 0;
    for (i = 0; i < hydro_x25519_NLIMBS; i++) {
        out[i] = (hydro_x25519_limb_t)(scarry = scarry + out[i] - hydro_x25519_sc_p[i]);
        scarry >>= hydro_x25519_WBITS;
    }
    hydro_x25519_limb_t need_add = (hydro_x25519_limb_t) - (scarry + hic);

    hydro_x25519_limb_t carry = 0;
    for (i = 0; i < hydro_x25519_NLIMBS; i++) {
        out[i] = hydro_x25519_umaal(&carry, out[i], need_add, hydro_x25519_sc_p[i]);
    }
}
