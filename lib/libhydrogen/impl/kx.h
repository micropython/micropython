#define hydro_kx_AEAD_KEYBYTES hydro_hash_KEYBYTES
#define hydro_kx_AEAD_MACBYTES 16

#define hydro_kx_CONTEXT "hydro_kx"

static void
hydro_kx_aead_init(uint8_t aead_state[gimli_BLOCKBYTES], uint8_t k[hydro_kx_AEAD_KEYBYTES],
                   hydro_kx_state *state)
{
    static const uint8_t prefix[] = { 6, 'k', 'x', 'x', '2', '5', '6', 0 };

    hydro_hash_final(&state->h_st, k, hydro_kx_AEAD_KEYBYTES);

    mem_zero(aead_state + sizeof prefix, gimli_BLOCKBYTES - sizeof prefix);
    memcpy(aead_state, prefix, sizeof prefix);
    gimli_core_u8(aead_state, gimli_TAG_HEADER);

    COMPILER_ASSERT(hydro_kx_AEAD_KEYBYTES == 2 * gimli_RATE);
    mem_xor(aead_state, k, gimli_RATE);
    gimli_core_u8(aead_state, gimli_TAG_KEY);
    mem_xor(aead_state, k + gimli_RATE, gimli_RATE);
    gimli_core_u8(aead_state, gimli_TAG_KEY);
}

static void
hydro_kx_aead_final(uint8_t *aead_state, const uint8_t key[hydro_kx_AEAD_KEYBYTES])
{
    COMPILER_ASSERT(hydro_kx_AEAD_KEYBYTES == gimli_CAPACITY);
    mem_xor(aead_state + gimli_RATE, key, hydro_kx_AEAD_KEYBYTES);
    gimli_core_u8(aead_state, gimli_TAG_FINAL);
    mem_xor(aead_state + gimli_RATE, key, hydro_kx_AEAD_KEYBYTES);
    gimli_core_u8(aead_state, gimli_TAG_FINAL);
}

static void
hydro_kx_aead_xor_enc(uint8_t aead_state[gimli_BLOCKBYTES], uint8_t *out, const uint8_t *in,
                      size_t inlen)
{
    size_t i;
    size_t leftover;

    for (i = 0; i < inlen / gimli_RATE; i++) {
        mem_xor2(&out[i * gimli_RATE], &in[i * gimli_RATE], aead_state, gimli_RATE);
        memcpy(aead_state, &out[i * gimli_RATE], gimli_RATE);
        gimli_core_u8(aead_state, gimli_TAG_PAYLOAD);
    }
    leftover = inlen % gimli_RATE;
    if (leftover != 0) {
        mem_xor2(&out[i * gimli_RATE], &in[i * gimli_RATE], aead_state, leftover);
        mem_cpy(aead_state, &out[i * gimli_RATE], leftover);
    }
    gimli_pad_u8(aead_state, leftover, gimli_DOMAIN_AEAD);
    gimli_core_u8(aead_state, gimli_TAG_PAYLOAD);
}

static void
hydro_kx_aead_xor_dec(uint8_t aead_state[gimli_BLOCKBYTES], uint8_t *out, const uint8_t *in,
                      size_t inlen)
{
    size_t i;
    size_t leftover;

    for (i = 0; i < inlen / gimli_RATE; i++) {
        mem_xor2(&out[i * gimli_RATE], &in[i * gimli_RATE], aead_state, gimli_RATE);
        memcpy(aead_state, &in[i * gimli_RATE], gimli_RATE);
        gimli_core_u8(aead_state, gimli_TAG_PAYLOAD);
    }
    leftover = inlen % gimli_RATE;
    if (leftover != 0) {
        mem_xor2(&out[i * gimli_RATE], &in[i * gimli_RATE], aead_state, leftover);
        mem_cpy(aead_state, &in[i * gimli_RATE], leftover);
    }
    gimli_pad_u8(aead_state, leftover, gimli_DOMAIN_AEAD);
    gimli_core_u8(aead_state, gimli_TAG_PAYLOAD);
}

static void
hydro_kx_aead_encrypt(hydro_kx_state *state, uint8_t *c, const uint8_t *m, size_t mlen)
{
    _hydro_attr_aligned_(16) uint8_t aead_state[gimli_BLOCKBYTES];
    uint8_t                          k[hydro_kx_AEAD_KEYBYTES];
    uint8_t *                        mac = &c[0];
    uint8_t *                        ct  = &c[hydro_kx_AEAD_MACBYTES];

    hydro_kx_aead_init(aead_state, k, state);
    hydro_kx_aead_xor_enc(aead_state, ct, m, mlen);
    hydro_kx_aead_final(aead_state, k);
    COMPILER_ASSERT(hydro_kx_AEAD_MACBYTES <= gimli_CAPACITY);
    memcpy(mac, aead_state + gimli_RATE, hydro_kx_AEAD_MACBYTES);
    hydro_hash_update(&state->h_st, c, mlen + hydro_kx_AEAD_MACBYTES);
}

static int hydro_kx_aead_decrypt(hydro_kx_state *state, uint8_t *m, const uint8_t *c,
                                 size_t clen) _hydro_attr_warn_unused_result_;

static int
hydro_kx_aead_decrypt(hydro_kx_state *state, uint8_t *m, const uint8_t *c, size_t clen)
{
    _hydro_attr_aligned_(16) uint32_t int_state[gimli_BLOCKBYTES / 4];
    uint32_t                          pub_mac[hydro_kx_AEAD_MACBYTES / 4];
    uint8_t                           k[hydro_kx_AEAD_KEYBYTES];
    uint8_t *                         aead_state = (uint8_t *) (void *) int_state;
    const uint8_t *                   mac;
    const uint8_t *                   ct;
    size_t                            mlen;
    uint32_t                          cv;

    if (clen < hydro_kx_AEAD_MACBYTES) {
        return -1;
    }
    mac  = &c[0];
    ct   = &c[hydro_kx_AEAD_MACBYTES];
    mlen = clen - hydro_kx_AEAD_MACBYTES;
    memcpy(pub_mac, mac, sizeof pub_mac);
    hydro_kx_aead_init(aead_state, k, state);
    hydro_hash_update(&state->h_st, c, clen);
    hydro_kx_aead_xor_dec(aead_state, m, ct, mlen);
    hydro_kx_aead_final(aead_state, k);
    COMPILER_ASSERT(hydro_kx_AEAD_MACBYTES <= gimli_CAPACITY);
    COMPILER_ASSERT(gimli_RATE % 4 == 0);
    cv = hydro_mem_ct_cmp_u32(int_state + gimli_RATE / 4, pub_mac, hydro_kx_AEAD_MACBYTES / 4);
    hydro_mem_ct_zero_u32(int_state, gimli_BLOCKBYTES / 4);
    if (cv != 0) {
        mem_zero(m, mlen);
        return -1;
    }
    return 0;
}

/* -- */

void
hydro_kx_keygen(hydro_kx_keypair *static_kp)
{
    hydro_random_buf(static_kp->sk, hydro_kx_SECRETKEYBYTES);
    if (hydro_x25519_scalarmult_base(static_kp->pk, static_kp->sk) != 0) {
        abort();
    }
}

void
hydro_kx_keygen_deterministic(hydro_kx_keypair *static_kp, const uint8_t seed[hydro_kx_SEEDBYTES])
{
    COMPILER_ASSERT(hydro_kx_SEEDBYTES >= hydro_random_SEEDBYTES);
    hydro_random_buf_deterministic(static_kp->sk, hydro_kx_SECRETKEYBYTES, seed);
    if (hydro_x25519_scalarmult_base(static_kp->pk, static_kp->sk) != 0) {
        abort();
    }
}

static void
hydro_kx_init_state(hydro_kx_state *state, const char *name)
{
    mem_zero(state, sizeof *state);
    hydro_hash_init(&state->h_st, hydro_kx_CONTEXT, NULL);
    hydro_hash_update(&state->h_st, name, strlen(name));
    hydro_hash_final(&state->h_st, NULL, 0);
}

static void
hydro_kx_final(hydro_kx_state *state, uint8_t session_k1[hydro_kx_SESSIONKEYBYTES],
               uint8_t session_k2[hydro_kx_SESSIONKEYBYTES])
{
    uint8_t kdf_key[hydro_kdf_KEYBYTES];

    hydro_hash_final(&state->h_st, kdf_key, sizeof kdf_key);
    hydro_kdf_derive_from_key(session_k1, hydro_kx_SESSIONKEYBYTES, 0, hydro_kx_CONTEXT, kdf_key);
    hydro_kdf_derive_from_key(session_k2, hydro_kx_SESSIONKEYBYTES, 1, hydro_kx_CONTEXT, kdf_key);
}

static int
hydro_kx_dh(hydro_kx_state *state, const uint8_t sk[hydro_x25519_SECRETKEYBYTES],
            const uint8_t pk[hydro_x25519_PUBLICKEYBYTES])
{
    uint8_t dh_result[hydro_x25519_BYTES];

    if (hydro_x25519_scalarmult(dh_result, sk, pk, 1) != 0) {
        return -1;
    }
    hydro_hash_update(&state->h_st, dh_result, hydro_x25519_BYTES);

    return 0;
}

static void
hydro_kx_eph_keygen(hydro_kx_state *state, hydro_kx_keypair *kp)
{
    hydro_kx_keygen(kp);
    hydro_hash_update(&state->h_st, kp->pk, sizeof kp->pk);
}

/* NOISE_N */

int
hydro_kx_n_1(hydro_kx_session_keypair *kp, uint8_t packet1[hydro_kx_N_PACKET1BYTES],
             const uint8_t psk[hydro_kx_PSKBYTES],
             const uint8_t peer_static_pk[hydro_kx_PUBLICKEYBYTES])
{
    hydro_kx_state state;
    uint8_t *      packet1_eph_pk = &packet1[0];
    uint8_t *      packet1_mac    = &packet1[hydro_kx_PUBLICKEYBYTES];

    if (psk == NULL) {
        psk = zero;
    }
    hydro_kx_init_state(&state, "Noise_Npsk0_hydro1");
    hydro_hash_update(&state.h_st, peer_static_pk, hydro_x25519_PUBLICKEYBYTES);

    hydro_hash_update(&state.h_st, psk, hydro_kx_PSKBYTES);
    hydro_kx_eph_keygen(&state, &state.eph_kp);
    if (hydro_kx_dh(&state, state.eph_kp.sk, peer_static_pk) != 0) {
        return -1;
    }
    hydro_kx_aead_encrypt(&state, packet1_mac, NULL, 0);
    memcpy(packet1_eph_pk, state.eph_kp.pk, sizeof state.eph_kp.pk);

    hydro_kx_final(&state, kp->rx, kp->tx);

    return 0;
}

int
hydro_kx_n_2(hydro_kx_session_keypair *kp, const uint8_t packet1[hydro_kx_N_PACKET1BYTES],
             const uint8_t psk[hydro_kx_PSKBYTES], const hydro_kx_keypair *static_kp)
{
    hydro_kx_state state;
    const uint8_t *peer_eph_pk = &packet1[0];
    const uint8_t *packet1_mac = &packet1[hydro_kx_PUBLICKEYBYTES];

    if (psk == NULL) {
        psk = zero;
    }
    hydro_kx_init_state(&state, "Noise_Npsk0_hydro1");
    hydro_hash_update(&state.h_st, static_kp->pk, hydro_kx_PUBLICKEYBYTES);

    hydro_hash_update(&state.h_st, psk, hydro_kx_PSKBYTES);
    hydro_hash_update(&state.h_st, peer_eph_pk, hydro_x25519_PUBLICKEYBYTES);
    if (hydro_kx_dh(&state, static_kp->sk, peer_eph_pk) != 0 ||
        hydro_kx_aead_decrypt(&state, NULL, packet1_mac, hydro_kx_AEAD_MACBYTES) != 0) {
        return -1;
    }
    hydro_kx_final(&state, kp->tx, kp->rx);

    return 0;
}

/* NOISE_KK */

int
hydro_kx_kk_1(hydro_kx_state *state, uint8_t packet1[hydro_kx_KK_PACKET1BYTES],
              const uint8_t           peer_static_pk[hydro_kx_PUBLICKEYBYTES],
              const hydro_kx_keypair *static_kp)
{
    uint8_t *packet1_eph_pk = &packet1[0];
    uint8_t *packet1_mac    = &packet1[hydro_kx_PUBLICKEYBYTES];

    hydro_kx_init_state(state, "Noise_KK_hydro1");
    hydro_hash_update(&state->h_st, static_kp->pk, hydro_kx_PUBLICKEYBYTES);
    hydro_hash_update(&state->h_st, peer_static_pk, hydro_kx_PUBLICKEYBYTES);

    hydro_kx_eph_keygen(state, &state->eph_kp);
    if (hydro_kx_dh(state, state->eph_kp.sk, peer_static_pk) != 0 ||
        hydro_kx_dh(state, static_kp->sk, peer_static_pk) != 0) {
        return -1;
    }
    hydro_kx_aead_encrypt(state, packet1_mac, NULL, 0);
    memcpy(packet1_eph_pk, state->eph_kp.pk, sizeof state->eph_kp.pk);

    return 0;
}

int
hydro_kx_kk_2(hydro_kx_session_keypair *kp, uint8_t packet2[hydro_kx_KK_PACKET2BYTES],
              const uint8_t           packet1[hydro_kx_KK_PACKET1BYTES],
              const uint8_t           peer_static_pk[hydro_kx_PUBLICKEYBYTES],
              const hydro_kx_keypair *static_kp)
{
    hydro_kx_state state;
    const uint8_t *peer_eph_pk    = &packet1[0];
    const uint8_t *packet1_mac    = &packet1[hydro_kx_PUBLICKEYBYTES];
    uint8_t *      packet2_eph_pk = &packet2[0];
    uint8_t *      packet2_mac    = &packet2[hydro_kx_PUBLICKEYBYTES];

    hydro_kx_init_state(&state, "Noise_KK_hydro1");
    hydro_hash_update(&state.h_st, peer_static_pk, hydro_kx_PUBLICKEYBYTES);
    hydro_hash_update(&state.h_st, static_kp->pk, hydro_kx_PUBLICKEYBYTES);

    hydro_hash_update(&state.h_st, peer_eph_pk, hydro_kx_PUBLICKEYBYTES);
    if (hydro_kx_dh(&state, static_kp->sk, peer_eph_pk) != 0 ||
        hydro_kx_dh(&state, static_kp->sk, peer_static_pk) != 0 ||
        hydro_kx_aead_decrypt(&state, NULL, packet1_mac, hydro_kx_AEAD_MACBYTES) != 0) {
        return -1;
    }

    hydro_kx_eph_keygen(&state, &state.eph_kp);
    if (hydro_kx_dh(&state, state.eph_kp.sk, peer_eph_pk) != 0 ||
        hydro_kx_dh(&state, state.eph_kp.sk, peer_static_pk) != 0) {
        return -1;
    }
    hydro_kx_aead_encrypt(&state, packet2_mac, NULL, 0);
    hydro_kx_final(&state, kp->tx, kp->rx);
    memcpy(packet2_eph_pk, state.eph_kp.pk, sizeof state.eph_kp.pk);

    return 0;
}

int
hydro_kx_kk_3(hydro_kx_state *state, hydro_kx_session_keypair *kp,
              const uint8_t packet2[hydro_kx_KK_PACKET2BYTES], const hydro_kx_keypair *static_kp)
{
    const uint8_t *peer_eph_pk = packet2;
    const uint8_t *packet2_mac = &packet2[hydro_kx_PUBLICKEYBYTES];

    hydro_hash_update(&state->h_st, peer_eph_pk, hydro_kx_PUBLICKEYBYTES);
    if (hydro_kx_dh(state, state->eph_kp.sk, peer_eph_pk) != 0 ||
        hydro_kx_dh(state, static_kp->sk, peer_eph_pk) != 0) {
        return -1;
    }

    if (hydro_kx_aead_decrypt(state, NULL, packet2_mac, hydro_kx_AEAD_MACBYTES) != 0) {
        return -1;
    }
    hydro_kx_final(state, kp->rx, kp->tx);

    return 0;
}

/* NOISE_XX */

int
hydro_kx_xx_1(hydro_kx_state *state, uint8_t packet1[hydro_kx_XX_PACKET1BYTES],
              const uint8_t psk[hydro_kx_PSKBYTES])
{
    uint8_t *packet1_eph_pk = &packet1[0];
    uint8_t *packet1_mac    = &packet1[hydro_kx_PUBLICKEYBYTES];

    if (psk == NULL) {
        psk = zero;
    }
    hydro_kx_init_state(state, "Noise_XXpsk0+psk3_hydro1");

    hydro_kx_eph_keygen(state, &state->eph_kp);
    hydro_hash_update(&state->h_st, psk, hydro_kx_PSKBYTES);
    memcpy(packet1_eph_pk, state->eph_kp.pk, sizeof state->eph_kp.pk);
    hydro_kx_aead_encrypt(state, packet1_mac, NULL, 0);

    return 0;
}

int
hydro_kx_xx_2(hydro_kx_state *state, uint8_t packet2[hydro_kx_XX_PACKET2BYTES],
              const uint8_t packet1[hydro_kx_XX_PACKET1BYTES], const uint8_t psk[hydro_kx_PSKBYTES],
              const hydro_kx_keypair *static_kp)
{
    const uint8_t *peer_eph_pk           = &packet1[0];
    const uint8_t *packet1_mac           = &packet1[hydro_kx_PUBLICKEYBYTES];
    uint8_t *      packet2_eph_pk        = &packet2[0];
    uint8_t *      packet2_enc_static_pk = &packet2[hydro_kx_PUBLICKEYBYTES];
    uint8_t *      packet2_mac =
        &packet2[hydro_kx_PUBLICKEYBYTES + hydro_kx_PUBLICKEYBYTES + hydro_kx_AEAD_MACBYTES];

    if (psk == NULL) {
        psk = zero;
    }
    hydro_kx_init_state(state, "Noise_XXpsk0+psk3_hydro1");

    hydro_hash_update(&state->h_st, peer_eph_pk, hydro_kx_PUBLICKEYBYTES);
    hydro_hash_update(&state->h_st, psk, hydro_kx_PSKBYTES);
    if (hydro_kx_aead_decrypt(state, NULL, packet1_mac, hydro_kx_AEAD_MACBYTES) != 0) {
        return -1;
    }

    hydro_kx_eph_keygen(state, &state->eph_kp);
    if (hydro_kx_dh(state, state->eph_kp.sk, peer_eph_pk) != 0) {
        return -1;
    }
    hydro_kx_aead_encrypt(state, packet2_enc_static_pk, static_kp->pk, sizeof static_kp->pk);
    if (hydro_kx_dh(state, static_kp->sk, peer_eph_pk) != 0) {
        return -1;
    }
    hydro_kx_aead_encrypt(state, packet2_mac, NULL, 0);

    memcpy(packet2_eph_pk, state->eph_kp.pk, sizeof state->eph_kp.pk);

    return 0;
}

int
hydro_kx_xx_3(hydro_kx_state *state, hydro_kx_session_keypair *kp,
              uint8_t       packet3[hydro_kx_XX_PACKET3BYTES],
              uint8_t       peer_static_pk[hydro_kx_PUBLICKEYBYTES],
              const uint8_t packet2[hydro_kx_XX_PACKET2BYTES], const uint8_t psk[hydro_kx_PSKBYTES],
              const hydro_kx_keypair *static_kp)
{
    uint8_t        peer_static_pk_[hydro_kx_PUBLICKEYBYTES];
    const uint8_t *peer_eph_pk        = &packet2[0];
    const uint8_t *peer_enc_static_pk = &packet2[hydro_kx_PUBLICKEYBYTES];
    const uint8_t *packet2_mac =
        &packet2[hydro_kx_PUBLICKEYBYTES + hydro_kx_PUBLICKEYBYTES + hydro_kx_AEAD_MACBYTES];
    uint8_t *packet3_enc_static_pk = &packet3[0];
    uint8_t *packet3_mac           = &packet3[hydro_kx_PUBLICKEYBYTES + hydro_kx_AEAD_MACBYTES];

    if (psk == NULL) {
        psk = zero;
    }
    if (peer_static_pk == NULL) {
        peer_static_pk = peer_static_pk_;
    }
    hydro_hash_update(&state->h_st, peer_eph_pk, hydro_kx_PUBLICKEYBYTES);
    if (hydro_kx_dh(state, state->eph_kp.sk, peer_eph_pk) != 0 ||
        hydro_kx_aead_decrypt(state, peer_static_pk, peer_enc_static_pk,
                              hydro_kx_PUBLICKEYBYTES + hydro_kx_AEAD_MACBYTES) != 0 ||
        hydro_kx_dh(state, state->eph_kp.sk, peer_static_pk) != 0 ||
        hydro_kx_aead_decrypt(state, NULL, packet2_mac, hydro_kx_AEAD_MACBYTES) != 0) {
        return -1;
    }

    hydro_kx_aead_encrypt(state, packet3_enc_static_pk, static_kp->pk, sizeof static_kp->pk);
    if (hydro_kx_dh(state, static_kp->sk, peer_eph_pk) != 0) {
        return -1;
    }
    hydro_hash_update(&state->h_st, psk, hydro_kx_PSKBYTES);
    hydro_kx_aead_encrypt(state, packet3_mac, NULL, 0);
    hydro_kx_final(state, kp->rx, kp->tx);

    return 0;
}

int
hydro_kx_xx_4(hydro_kx_state *state, hydro_kx_session_keypair *kp,
              uint8_t       peer_static_pk[hydro_kx_PUBLICKEYBYTES],
              const uint8_t packet3[hydro_kx_XX_PACKET3BYTES], const uint8_t psk[hydro_kx_PSKBYTES])
{
    uint8_t        peer_static_pk_[hydro_kx_PUBLICKEYBYTES];
    const uint8_t *peer_enc_static_pk = &packet3[0];
    const uint8_t *packet3_mac        = &packet3[hydro_kx_PUBLICKEYBYTES + hydro_kx_AEAD_MACBYTES];

    if (psk == NULL) {
        psk = zero;
    }
    if (peer_static_pk == NULL) {
        peer_static_pk = peer_static_pk_;
    }
    if (hydro_kx_aead_decrypt(state, peer_static_pk, peer_enc_static_pk,
                              hydro_kx_PUBLICKEYBYTES + hydro_kx_AEAD_MACBYTES) != 0 ||
        hydro_kx_dh(state, state->eph_kp.sk, peer_static_pk) != 0) {
        return -1;
    }
    hydro_hash_update(&state->h_st, psk, hydro_kx_PSKBYTES);
    if (hydro_kx_aead_decrypt(state, NULL, packet3_mac, hydro_kx_AEAD_MACBYTES) != 0) {
        return -1;
    }
    hydro_kx_final(state, kp->tx, kp->rx);

    return 0;
}
