#define hydro_pwhash_ENC_ALGBYTES 1
#define hydro_pwhash_HASH_ALGBYTES 1
#define hydro_pwhash_THREADSBYTES 1
#define hydro_pwhash_OPSLIMITBYTES 8
#define hydro_pwhash_MEMLIMITBYTES 8
#define hydro_pwhash_HASHBYTES 32
#define hydro_pwhash_SALTBYTES 16
#define hydro_pwhash_PARAMSBYTES                                                           \
    (hydro_pwhash_HASH_ALGBYTES + hydro_pwhash_THREADSBYTES + hydro_pwhash_OPSLIMITBYTES + \
     hydro_pwhash_MEMLIMITBYTES + hydro_pwhash_SALTBYTES + hydro_pwhash_HASHBYTES)
#define hydro_pwhash_ENC_ALG 0x01
#define hydro_pwhash_HASH_ALG 0x01
#define hydro_pwhash_CONTEXT "hydro_pw"

static int
_hydro_pwhash_hash(uint8_t out[hydro_random_SEEDBYTES], size_t h_len,
                   const uint8_t salt[hydro_pwhash_SALTBYTES], const char *passwd,
                   size_t passwd_len, const char ctx[hydro_pwhash_CONTEXTBYTES],
                   const uint8_t master_key[hydro_pwhash_MASTERKEYBYTES], uint64_t opslimit,
                   size_t memlimit, uint8_t threads)
{
    _hydro_attr_aligned_(16) uint8_t state[gimli_BLOCKBYTES];
    hydro_hash_state                 h_st;
    uint8_t                          tmp64_u8[8];
    uint64_t                         i;
    uint8_t                          tmp8;

    COMPILER_ASSERT(hydro_pwhash_MASTERKEYBYTES >= hydro_hash_KEYBYTES);
    hydro_hash_init(&h_st, ctx, master_key);

    STORE64_LE(tmp64_u8, (uint64_t) passwd_len);
    hydro_hash_update(&h_st, tmp64_u8, sizeof tmp64_u8);
    hydro_hash_update(&h_st, passwd, passwd_len);

    hydro_hash_update(&h_st, salt, hydro_pwhash_SALTBYTES);

    tmp8 = hydro_pwhash_HASH_ALG;
    hydro_hash_update(&h_st, &tmp8, 1);

    hydro_hash_update(&h_st, &threads, 1);

    STORE64_LE(tmp64_u8, (uint64_t) memlimit);
    hydro_hash_update(&h_st, tmp64_u8, sizeof tmp64_u8);

    STORE64_LE(tmp64_u8, (uint64_t) h_len);
    hydro_hash_update(&h_st, tmp64_u8, sizeof tmp64_u8);

    hydro_hash_final(&h_st, (uint8_t *) (void *) &state, sizeof state);

    gimli_core_u8(state, 1);
    COMPILER_ASSERT(gimli_RATE >= 8);
    for (i = 0; i < opslimit; i++) {
        mem_zero(state, gimli_RATE);
        STORE64_LE(state, i);
        gimli_core_u8(state, 0);
    }
    mem_zero(state, gimli_RATE);

    COMPILER_ASSERT(hydro_random_SEEDBYTES == gimli_CAPACITY);
    memcpy(out, state + gimli_RATE, hydro_random_SEEDBYTES);
    hydro_memzero(state, sizeof state);

    return 0;
}

void
hydro_pwhash_keygen(uint8_t master_key[hydro_pwhash_MASTERKEYBYTES])
{
    hydro_random_buf(master_key, hydro_pwhash_MASTERKEYBYTES);
}

int
hydro_pwhash_deterministic(uint8_t *h, size_t h_len, const char *passwd, size_t passwd_len,
                           const char    ctx[hydro_pwhash_CONTEXTBYTES],
                           const uint8_t master_key[hydro_pwhash_MASTERKEYBYTES], uint64_t opslimit,
                           size_t memlimit, uint8_t threads)
{
    uint8_t seed[hydro_random_SEEDBYTES];

    COMPILER_ASSERT(sizeof zero >= hydro_pwhash_SALTBYTES);
    COMPILER_ASSERT(sizeof zero >= hydro_pwhash_MASTERKEYBYTES);

    (void) memlimit;
    if (_hydro_pwhash_hash(seed, h_len, zero, passwd, passwd_len, ctx, master_key, opslimit,
                           memlimit, threads) != 0) {
        return -1;
    }
    hydro_random_buf_deterministic(h, h_len, seed);
    hydro_memzero(seed, sizeof seed);

    return 0;
}

int
hydro_pwhash_create(uint8_t stored[hydro_pwhash_STOREDBYTES], const char *passwd, size_t passwd_len,
                    const uint8_t master_key[hydro_pwhash_MASTERKEYBYTES], uint64_t opslimit,
                    size_t memlimit, uint8_t threads)
{
    uint8_t *const enc_alg     = &stored[0];
    uint8_t *const secretbox   = &enc_alg[hydro_pwhash_ENC_ALGBYTES];
    uint8_t *const hash_alg    = &secretbox[hydro_secretbox_HEADERBYTES];
    uint8_t *const threads_u8  = &hash_alg[hydro_pwhash_HASH_ALGBYTES];
    uint8_t *const opslimit_u8 = &threads_u8[hydro_pwhash_THREADSBYTES];
    uint8_t *const memlimit_u8 = &opslimit_u8[hydro_pwhash_OPSLIMITBYTES];
    uint8_t *const salt        = &memlimit_u8[hydro_pwhash_MEMLIMITBYTES];
    uint8_t *const h           = &salt[hydro_pwhash_SALTBYTES];

    COMPILER_ASSERT(hydro_pwhash_STOREDBYTES >= hydro_pwhash_ENC_ALGBYTES +
                                                    hydro_secretbox_HEADERBYTES +
                                                    hydro_pwhash_PARAMSBYTES);
    (void) memlimit;
    mem_zero(stored, hydro_pwhash_STOREDBYTES);
    *enc_alg    = hydro_pwhash_ENC_ALG;
    *hash_alg   = hydro_pwhash_HASH_ALG;
    *threads_u8 = threads;
    STORE64_LE(opslimit_u8, opslimit);
    STORE64_LE(memlimit_u8, (uint64_t) memlimit);
    hydro_random_buf(salt, hydro_pwhash_SALTBYTES);

    COMPILER_ASSERT(sizeof zero >= hydro_pwhash_MASTERKEYBYTES);
    if (_hydro_pwhash_hash(h, hydro_pwhash_HASHBYTES, salt, passwd, passwd_len,
                           hydro_pwhash_CONTEXT, zero, opslimit, memlimit, threads) != 0) {
        return -1;
    }
    COMPILER_ASSERT(hydro_pwhash_MASTERKEYBYTES == hydro_secretbox_KEYBYTES);

    return hydro_secretbox_encrypt(secretbox, hash_alg, hydro_pwhash_PARAMSBYTES,
                                   (uint64_t) *enc_alg, hydro_pwhash_CONTEXT, master_key);
}

static int
_hydro_pwhash_verify(uint8_t       computed_h[hydro_pwhash_HASHBYTES],
                     const uint8_t stored[hydro_pwhash_STOREDBYTES], const char *passwd,
                     size_t passwd_len, const uint8_t master_key[hydro_pwhash_MASTERKEYBYTES],
                     uint64_t opslimit_max, size_t memlimit_max, uint8_t threads_max)
{
    const uint8_t *const enc_alg   = &stored[0];
    const uint8_t *const secretbox = &enc_alg[hydro_pwhash_ENC_ALGBYTES];

    uint8_t        params[hydro_pwhash_PARAMSBYTES];
    uint8_t *const hash_alg    = &params[0];
    uint8_t *const threads_u8  = &hash_alg[hydro_pwhash_HASH_ALGBYTES];
    uint8_t *const opslimit_u8 = &threads_u8[hydro_pwhash_THREADSBYTES];
    uint8_t *const memlimit_u8 = &opslimit_u8[hydro_pwhash_OPSLIMITBYTES];
    uint8_t *const salt        = &memlimit_u8[hydro_pwhash_MEMLIMITBYTES];
    uint8_t *const h           = &salt[hydro_pwhash_SALTBYTES];

    uint64_t opslimit;
    size_t   memlimit;
    uint8_t  threads;

    (void) memlimit;
    if (*enc_alg != hydro_pwhash_ENC_ALG) {
        return -1;
    }
    if (hydro_secretbox_decrypt(params, secretbox,
                                hydro_secretbox_HEADERBYTES + hydro_pwhash_PARAMSBYTES,
                                (uint64_t) *enc_alg, hydro_pwhash_CONTEXT, master_key) != 0) {
        return -1;
    }
    if (*hash_alg != hydro_pwhash_HASH_ALG || (opslimit = LOAD64_LE(opslimit_u8)) > opslimit_max ||
        (memlimit = (size_t) LOAD64_LE(memlimit_u8)) > memlimit_max ||
        (threads = *threads_u8) > threads_max) {
        return -1;
    }
    if (_hydro_pwhash_hash(computed_h, hydro_pwhash_HASHBYTES, salt, passwd, passwd_len,
                           hydro_pwhash_CONTEXT, zero, opslimit, memlimit, threads) == 0 &&
        hydro_equal(computed_h, h, hydro_pwhash_HASHBYTES) == 1) {
        return 0;
    }
    return -1;
}

int
hydro_pwhash_verify(const uint8_t stored[hydro_pwhash_STOREDBYTES], const char *passwd,
                    size_t passwd_len, const uint8_t master_key[hydro_pwhash_MASTERKEYBYTES],
                    uint64_t opslimit_max, size_t memlimit_max, uint8_t threads_max)
{
    uint8_t computed_h[hydro_pwhash_HASHBYTES];
    int     ret;

    ret = _hydro_pwhash_verify(computed_h, stored, passwd, passwd_len, master_key, opslimit_max,
                               memlimit_max, threads_max);
    hydro_memzero(computed_h, sizeof computed_h);

    return ret;
}

int
hydro_pwhash_derive_static_key(uint8_t *static_key, size_t static_key_len,
                               const uint8_t stored[hydro_pwhash_STOREDBYTES], const char *passwd,
                               size_t passwd_len, const char ctx[hydro_pwhash_CONTEXTBYTES],
                               const uint8_t master_key[hydro_pwhash_MASTERKEYBYTES],
                               uint64_t opslimit_max, size_t memlimit_max, uint8_t threads_max)
{
    uint8_t computed_h[hydro_pwhash_HASHBYTES];

    if (_hydro_pwhash_verify(computed_h, stored, passwd, passwd_len, master_key, opslimit_max,
                             memlimit_max, threads_max) != 0) {
        hydro_memzero(computed_h, sizeof computed_h);
        return -1;
    }
    COMPILER_ASSERT(hydro_kdf_CONTEXTBYTES <= hydro_pwhash_CONTEXTBYTES);
    COMPILER_ASSERT(hydro_kdf_KEYBYTES <= hydro_pwhash_HASHBYTES);
    hydro_kdf_derive_from_key(static_key, static_key_len, 0, ctx, computed_h);
    hydro_memzero(computed_h, sizeof computed_h);

    return 0;
}

int
hydro_pwhash_reencrypt(uint8_t       stored[hydro_pwhash_STOREDBYTES],
                       const uint8_t master_key[hydro_pwhash_MASTERKEYBYTES],
                       const uint8_t new_master_key[hydro_pwhash_MASTERKEYBYTES])
{
    uint8_t *const enc_alg   = &stored[0];
    uint8_t *const secretbox = &enc_alg[hydro_pwhash_ENC_ALGBYTES];
    uint8_t *const params    = &secretbox[hydro_secretbox_HEADERBYTES];

    if (*enc_alg != hydro_pwhash_ENC_ALG) {
        return -1;
    }
    if (hydro_secretbox_decrypt(secretbox, secretbox,
                                hydro_secretbox_HEADERBYTES + hydro_pwhash_PARAMSBYTES,
                                (uint64_t) *enc_alg, hydro_pwhash_CONTEXT, master_key) != 0) {
        return -1;
    }
    memmove(params, secretbox, hydro_pwhash_PARAMSBYTES);
    return hydro_secretbox_encrypt(secretbox, params, hydro_pwhash_PARAMSBYTES, (uint64_t) *enc_alg,
                                   hydro_pwhash_CONTEXT, new_master_key);
}

int
hydro_pwhash_upgrade(uint8_t       stored[hydro_pwhash_STOREDBYTES],
                     const uint8_t master_key[hydro_pwhash_MASTERKEYBYTES], uint64_t opslimit,
                     size_t memlimit, uint8_t threads)
{
    uint8_t *const enc_alg     = &stored[0];
    uint8_t *const secretbox   = &enc_alg[hydro_pwhash_ENC_ALGBYTES];
    uint8_t *const params      = &secretbox[hydro_secretbox_HEADERBYTES];
    uint8_t *const hash_alg    = &params[0];
    uint8_t *const threads_u8  = &hash_alg[hydro_pwhash_HASH_ALGBYTES];
    uint8_t *const opslimit_u8 = &threads_u8[hydro_pwhash_THREADSBYTES];
    uint8_t *const memlimit_u8 = &opslimit_u8[hydro_pwhash_OPSLIMITBYTES];
    uint8_t *const salt        = &memlimit_u8[hydro_pwhash_MEMLIMITBYTES];
    uint8_t *const h           = &salt[hydro_pwhash_SALTBYTES];

    _hydro_attr_aligned_(16) uint8_t state[gimli_BLOCKBYTES];
    uint64_t                         i;
    uint64_t                         opslimit_prev;

    if (*enc_alg != hydro_pwhash_ENC_ALG) {
        return -1;
    }
    if (hydro_secretbox_decrypt(secretbox, secretbox,
                                hydro_secretbox_HEADERBYTES + hydro_pwhash_PARAMSBYTES,
                                (uint64_t) *enc_alg, hydro_pwhash_CONTEXT, master_key) != 0) {
        return -1;
    }
    memmove(params, secretbox, hydro_pwhash_PARAMSBYTES);
    opslimit_prev = LOAD64_LE(opslimit_u8);
    if (*hash_alg != hydro_pwhash_HASH_ALG) {
        mem_zero(stored, hydro_pwhash_STOREDBYTES);
        return -1;
    }
    COMPILER_ASSERT(hydro_random_SEEDBYTES == gimli_CAPACITY);
    memcpy(state + gimli_RATE, h, hydro_random_SEEDBYTES);
    for (i = opslimit_prev; i < opslimit; i++) {
        mem_zero(state, gimli_RATE);
        STORE64_LE(state, i);
        gimli_core_u8(state, 0);
    }
    mem_zero(state, gimli_RATE);
    memcpy(h, state + gimli_RATE, hydro_random_SEEDBYTES);
    *threads_u8 = threads;
    STORE64_LE(opslimit_u8, opslimit);
    STORE64_LE(memlimit_u8, (uint64_t) memlimit);

    return hydro_secretbox_encrypt(secretbox, params, hydro_pwhash_PARAMSBYTES, (uint64_t) *enc_alg,
                                   hydro_pwhash_CONTEXT, master_key);
}
