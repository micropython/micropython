#define hydro_sign_CHALLENGEBYTES 32
#define hydro_sign_NONCEBYTES 32
#define hydro_sign_PREHASHBYTES 64

static void
hydro_sign_p2(uint8_t sig[hydro_x25519_BYTES], const uint8_t challenge[hydro_sign_CHALLENGEBYTES],
              const uint8_t eph_sk[hydro_x25519_BYTES], const uint8_t sk[hydro_x25519_BYTES])
{
    hydro_x25519_scalar_t scalar1, scalar2, scalar3;

    COMPILER_ASSERT(hydro_sign_CHALLENGEBYTES == hydro_x25519_BYTES);
    hydro_x25519_swapin(scalar1, eph_sk);
    hydro_x25519_swapin(scalar2, sk);
    hydro_x25519_swapin(scalar3, challenge);
    hydro_x25519_sc_montmul(scalar1, scalar2, scalar3);
    mem_zero(scalar2, sizeof scalar2);
    hydro_x25519_sc_montmul(scalar2, scalar1, hydro_x25519_sc_r2);
    hydro_x25519_swapout(sig, scalar2);
}

static void
hydro_sign_challenge(uint8_t       challenge[hydro_sign_CHALLENGEBYTES],
                     const uint8_t nonce[hydro_sign_NONCEBYTES],
                     const uint8_t pk[hydro_sign_PUBLICKEYBYTES],
                     const uint8_t prehash[hydro_sign_PREHASHBYTES])
{
    hydro_hash_state st;

    hydro_hash_init(&st, (const char *) zero, NULL);
    hydro_hash_update(&st, nonce, hydro_sign_NONCEBYTES);
    hydro_hash_update(&st, pk, hydro_sign_PUBLICKEYBYTES);
    hydro_hash_update(&st, prehash, hydro_sign_PREHASHBYTES);
    hydro_hash_final(&st, challenge, hydro_sign_CHALLENGEBYTES);
}

static int
hydro_sign_prehash(uint8_t csig[hydro_sign_BYTES], const uint8_t prehash[hydro_sign_PREHASHBYTES],
                   const uint8_t sk[hydro_sign_SECRETKEYBYTES])
{
    hydro_hash_state st;
    uint8_t          challenge[hydro_sign_CHALLENGEBYTES];
    const uint8_t *  pk     = &sk[hydro_x25519_SECRETKEYBYTES];
    uint8_t *        nonce  = &csig[0];
    uint8_t *        sig    = &csig[hydro_sign_NONCEBYTES];
    uint8_t *        eph_sk = sig;

    hydro_random_buf(eph_sk, hydro_x25519_SECRETKEYBYTES);
    COMPILER_ASSERT(hydro_x25519_SECRETKEYBYTES == hydro_hash_KEYBYTES);
    hydro_hash_init(&st, (const char *) zero, sk);
    hydro_hash_update(&st, eph_sk, hydro_x25519_SECRETKEYBYTES);
    hydro_hash_update(&st, prehash, hydro_sign_PREHASHBYTES);
    hydro_hash_final(&st, eph_sk, hydro_x25519_SECRETKEYBYTES);

    hydro_x25519_scalarmult_base_uniform(nonce, eph_sk);
    hydro_sign_challenge(challenge, nonce, pk, prehash);

    COMPILER_ASSERT(hydro_sign_BYTES == hydro_sign_NONCEBYTES + hydro_x25519_SECRETKEYBYTES);
    COMPILER_ASSERT(hydro_x25519_SECRETKEYBYTES <= hydro_sign_CHALLENGEBYTES);
    hydro_sign_p2(sig, challenge, eph_sk, sk);

    return 0;
}

static int
hydro_sign_verify_core(hydro_x25519_fe xs[5], const hydro_x25519_limb_t *other1,
                       const uint8_t other2[hydro_x25519_BYTES])
{
    hydro_x25519_limb_t *     z2 = xs[1], *x3 = xs[2], *z3 = xs[3];
    hydro_x25519_fe           xo2;
    const hydro_x25519_limb_t sixteen = 16;

    hydro_x25519_swapin(xo2, other2);
    memcpy(x3, other1, 2 * sizeof(hydro_x25519_fe));
    hydro_x25519_ladder_part1(xs);

    /* Here z2 = t2^2 */
    hydro_x25519_mul1(z2, other1);
    hydro_x25519_mul1(z2, other1 + hydro_x25519_NLIMBS);
    hydro_x25519_mul1(z2, xo2);

    hydro_x25519_mul(z2, z2, &sixteen, 1);

    hydro_x25519_mul1(z3, xo2);
    hydro_x25519_sub(z3, z3, x3);
    hydro_x25519_sqr1(z3);

    /* check equality */
    hydro_x25519_sub(z3, z3, z2);

    /* canon(z2): both sides are zero. canon(z3): the two sides are equal. */
    /* Reject sigs where both sides are zero. */
    return hydro_x25519_canon(z2) | ~hydro_x25519_canon(z3);
}

static int
hydro_sign_verify_p2(const uint8_t sig[hydro_x25519_BYTES],
                     const uint8_t challenge[hydro_sign_CHALLENGEBYTES],
                     const uint8_t nonce[hydro_sign_NONCEBYTES],
                     const uint8_t pk[hydro_x25519_BYTES])
{
    hydro_x25519_fe xs[7];

    hydro_x25519_core(&xs[0], challenge, pk, 0);
    hydro_x25519_core(&xs[2], sig, hydro_x25519_BASE_POINT, 0);

    return hydro_sign_verify_core(&xs[2], xs[0], nonce);
}

static int
hydro_sign_verify_challenge(const uint8_t csig[hydro_sign_BYTES],
                            const uint8_t challenge[hydro_sign_CHALLENGEBYTES],
                            const uint8_t pk[hydro_sign_PUBLICKEYBYTES])
{
    const uint8_t *nonce = &csig[0];
    const uint8_t *sig   = &csig[hydro_sign_NONCEBYTES];

    return hydro_sign_verify_p2(sig, challenge, nonce, pk);
}

void
hydro_sign_keygen(hydro_sign_keypair *kp)
{
    uint8_t *pk_copy = &kp->sk[hydro_x25519_SECRETKEYBYTES];

    COMPILER_ASSERT(hydro_sign_SECRETKEYBYTES ==
                    hydro_x25519_SECRETKEYBYTES + hydro_x25519_PUBLICKEYBYTES);
    COMPILER_ASSERT(hydro_sign_PUBLICKEYBYTES == hydro_x25519_PUBLICKEYBYTES);
    hydro_random_buf(kp->sk, hydro_x25519_SECRETKEYBYTES);
    hydro_x25519_scalarmult_base_uniform(kp->pk, kp->sk);
    memcpy(pk_copy, kp->pk, hydro_x25519_PUBLICKEYBYTES);
}

void
hydro_sign_keygen_deterministic(hydro_sign_keypair *kp, const uint8_t seed[hydro_sign_SEEDBYTES])
{
    uint8_t *pk_copy = &kp->sk[hydro_x25519_SECRETKEYBYTES];

    COMPILER_ASSERT(hydro_sign_SEEDBYTES >= hydro_random_SEEDBYTES);
    hydro_random_buf_deterministic(kp->sk, hydro_x25519_SECRETKEYBYTES, seed);
    hydro_x25519_scalarmult_base_uniform(kp->pk, kp->sk);
    memcpy(pk_copy, kp->pk, hydro_x25519_PUBLICKEYBYTES);
}

int
hydro_sign_init(hydro_sign_state *state, const char ctx[hydro_sign_CONTEXTBYTES])
{
    return hydro_hash_init(&state->hash_st, ctx, NULL);
}

int
hydro_sign_update(hydro_sign_state *state, const void *m_, size_t mlen)
{
    return hydro_hash_update(&state->hash_st, m_, mlen);
}

int
hydro_sign_final_create(hydro_sign_state *state, uint8_t csig[hydro_sign_BYTES],
                        const uint8_t sk[hydro_sign_SECRETKEYBYTES])
{
    uint8_t prehash[hydro_sign_PREHASHBYTES];

    hydro_hash_final(&state->hash_st, prehash, sizeof prehash);

    return hydro_sign_prehash(csig, prehash, sk);
}

int
hydro_sign_final_verify(hydro_sign_state *state, const uint8_t csig[hydro_sign_BYTES],
                        const uint8_t pk[hydro_sign_PUBLICKEYBYTES])
{
    uint8_t        challenge[hydro_sign_CHALLENGEBYTES];
    uint8_t        prehash[hydro_sign_PREHASHBYTES];
    const uint8_t *nonce = &csig[0];

    hydro_hash_final(&state->hash_st, prehash, sizeof prehash);
    hydro_sign_challenge(challenge, nonce, pk, prehash);

    return hydro_sign_verify_challenge(csig, challenge, pk);
}

int
hydro_sign_create(uint8_t csig[hydro_sign_BYTES], const void *m_, size_t mlen,
                  const char    ctx[hydro_sign_CONTEXTBYTES],
                  const uint8_t sk[hydro_sign_SECRETKEYBYTES])
{
    hydro_sign_state st;

    if (hydro_sign_init(&st, ctx) != 0 || hydro_sign_update(&st, m_, mlen) != 0 ||
        hydro_sign_final_create(&st, csig, sk) != 0) {
        return -1;
    }
    return 0;
}

int
hydro_sign_verify(const uint8_t csig[hydro_sign_BYTES], const void *m_, size_t mlen,
                  const char    ctx[hydro_sign_CONTEXTBYTES],
                  const uint8_t pk[hydro_sign_PUBLICKEYBYTES])
{
    hydro_sign_state st;

    if (hydro_sign_init(&st, ctx) != 0 || hydro_sign_update(&st, m_, mlen) != 0 ||
        hydro_sign_final_verify(&st, csig, pk) != 0) {
        return -1;
    }
    return 0;
}
