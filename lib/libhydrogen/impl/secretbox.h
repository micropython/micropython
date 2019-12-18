#define hydro_secretbox_IVBYTES 20
#define hydro_secretbox_SIVBYTES 20
#define hydro_secretbox_MACBYTES 16

void
hydro_secretbox_keygen(uint8_t key[hydro_secretbox_KEYBYTES])
{
    hydro_random_buf(key, hydro_secretbox_KEYBYTES);
}

static void
hydro_secretbox_xor_enc(uint8_t buf[gimli_BLOCKBYTES], uint8_t *out, const uint8_t *in,
                        size_t inlen)
{
    size_t i;
    size_t leftover;

    for (i = 0; i < inlen / gimli_RATE; i++) {
        mem_xor2(&out[i * gimli_RATE], &in[i * gimli_RATE], buf, gimli_RATE);
        memcpy(buf, &out[i * gimli_RATE], gimli_RATE);
        gimli_core_u8(buf, gimli_TAG_PAYLOAD);
    }
    leftover = inlen % gimli_RATE;
    if (leftover != 0) {
        mem_xor2(&out[i * gimli_RATE], &in[i * gimli_RATE], buf, leftover);
        mem_cpy(buf, &out[i * gimli_RATE], leftover);
    }
    gimli_pad_u8(buf, leftover, gimli_DOMAIN_AEAD);
    gimli_core_u8(buf, gimli_TAG_PAYLOAD);
}

static void
hydro_secretbox_xor_dec(uint8_t buf[gimli_BLOCKBYTES], uint8_t *out, const uint8_t *in,
                        size_t inlen)
{
    size_t i;
    size_t leftover;

    for (i = 0; i < inlen / gimli_RATE; i++) {
        mem_xor2(&out[i * gimli_RATE], &in[i * gimli_RATE], buf, gimli_RATE);
        memcpy(buf, &in[i * gimli_RATE], gimli_RATE);
        gimli_core_u8(buf, gimli_TAG_PAYLOAD);
    }
    leftover = inlen % gimli_RATE;
    if (leftover != 0) {
        mem_xor2(&out[i * gimli_RATE], &in[i * gimli_RATE], buf, leftover);
        mem_cpy(buf, &in[i * gimli_RATE], leftover);
    }
    gimli_pad_u8(buf, leftover, gimli_DOMAIN_AEAD);
    gimli_core_u8(buf, gimli_TAG_PAYLOAD);
}

static void
hydro_secretbox_setup(uint8_t buf[gimli_BLOCKBYTES], uint64_t msg_id,
                      const char    ctx[hydro_secretbox_CONTEXTBYTES],
                      const uint8_t key[hydro_secretbox_KEYBYTES],
                      const uint8_t iv[hydro_secretbox_IVBYTES], uint8_t key_tag)
{
    static const uint8_t prefix[] = { 6, 's', 'b', 'x', '2', '5', '6', 8 };
    uint8_t              msg_id_le[8];

    mem_zero(buf, gimli_BLOCKBYTES);
    COMPILER_ASSERT(hydro_secretbox_CONTEXTBYTES == 8);
    COMPILER_ASSERT(sizeof prefix + hydro_secretbox_CONTEXTBYTES <= gimli_RATE);
    memcpy(buf, prefix, sizeof prefix);
    memcpy(buf + sizeof prefix, ctx, hydro_secretbox_CONTEXTBYTES);
    COMPILER_ASSERT(sizeof prefix + hydro_secretbox_CONTEXTBYTES == gimli_RATE);
    gimli_core_u8(buf, gimli_TAG_HEADER);

    COMPILER_ASSERT(hydro_secretbox_KEYBYTES == 2 * gimli_RATE);
    mem_xor(buf, key, gimli_RATE);
    gimli_core_u8(buf, key_tag);
    mem_xor(buf, key + gimli_RATE, gimli_RATE);
    gimli_core_u8(buf, key_tag);

    COMPILER_ASSERT(hydro_secretbox_IVBYTES < gimli_RATE * 2);
    buf[0] ^= hydro_secretbox_IVBYTES;
    mem_xor(&buf[1], iv, gimli_RATE - 1);
    gimli_core_u8(buf, gimli_TAG_HEADER);
    mem_xor(buf, iv + gimli_RATE - 1, hydro_secretbox_IVBYTES - (gimli_RATE - 1));
    STORE64_LE(msg_id_le, msg_id);
    COMPILER_ASSERT(hydro_secretbox_IVBYTES - gimli_RATE + 8 <= gimli_RATE);
    mem_xor(buf + hydro_secretbox_IVBYTES - gimli_RATE, msg_id_le, 8);
    gimli_core_u8(buf, gimli_TAG_HEADER);
}

static void
hydro_secretbox_final(uint8_t *buf, const uint8_t key[hydro_secretbox_KEYBYTES], uint8_t tag)
{
    COMPILER_ASSERT(hydro_secretbox_KEYBYTES == gimli_CAPACITY);
    mem_xor(buf + gimli_RATE, key, hydro_secretbox_KEYBYTES);
    gimli_core_u8(buf, tag);
    mem_xor(buf + gimli_RATE, key, hydro_secretbox_KEYBYTES);
    gimli_core_u8(buf, tag);
}

static int
hydro_secretbox_encrypt_iv(uint8_t *c, const void *m_, size_t mlen, uint64_t msg_id,
                           const char    ctx[hydro_secretbox_CONTEXTBYTES],
                           const uint8_t key[hydro_secretbox_KEYBYTES],
                           const uint8_t iv[hydro_secretbox_IVBYTES])
{
    _hydro_attr_aligned_(16) uint32_t state[gimli_BLOCKBYTES / 4];
    uint8_t *                         buf = (uint8_t *) (void *) state;
    const uint8_t *                   m   = (const uint8_t *) m_;
    uint8_t *                         siv = &c[0];
    uint8_t *                         mac = &c[hydro_secretbox_SIVBYTES];
    uint8_t *                         ct  = &c[hydro_secretbox_SIVBYTES + hydro_secretbox_MACBYTES];
    size_t                            i;
    size_t                            leftover;

    if (c == m) {
        memmove(c + hydro_secretbox_HEADERBYTES, m, mlen);
        m = c + hydro_secretbox_HEADERBYTES;
    }

    /* first pass: compute the SIV */

    hydro_secretbox_setup(buf, msg_id, ctx, key, iv, gimli_TAG_KEY0);
    for (i = 0; i < mlen / gimli_RATE; i++) {
        mem_xor(buf, &m[i * gimli_RATE], gimli_RATE);
        gimli_core_u8(buf, gimli_TAG_PAYLOAD);
    }
    leftover = mlen % gimli_RATE;
    if (leftover != 0) {
        mem_xor(buf, &m[i * gimli_RATE], leftover);
    }
    gimli_pad_u8(buf, leftover, gimli_DOMAIN_XOF);
    gimli_core_u8(buf, gimli_TAG_PAYLOAD);

    hydro_secretbox_final(buf, key, gimli_TAG_FINAL0);
    COMPILER_ASSERT(hydro_secretbox_SIVBYTES <= gimli_CAPACITY);
    memcpy(siv, buf + gimli_RATE, hydro_secretbox_SIVBYTES);

    /* second pass: encrypt the message, mix the key, squeeze an extra block for
     * the MAC */

    COMPILER_ASSERT(hydro_secretbox_SIVBYTES == hydro_secretbox_IVBYTES);
    hydro_secretbox_setup(buf, msg_id, ctx, key, siv, gimli_TAG_KEY);
    hydro_secretbox_xor_enc(buf, ct, m, mlen);

    hydro_secretbox_final(buf, key, gimli_TAG_FINAL);
    COMPILER_ASSERT(hydro_secretbox_MACBYTES <= gimli_CAPACITY);
    memcpy(mac, buf + gimli_RATE, hydro_secretbox_MACBYTES);

    return 0;
}

void
hydro_secretbox_probe_create(uint8_t probe[hydro_secretbox_PROBEBYTES], const uint8_t *c,
                             size_t c_len, const char ctx[hydro_secretbox_CONTEXTBYTES],
                             const uint8_t key[hydro_secretbox_KEYBYTES])
{
    const uint8_t *mac;

    if (c_len < hydro_secretbox_HEADERBYTES) {
        abort();
    }
    mac = &c[hydro_secretbox_SIVBYTES];
    COMPILER_ASSERT(hydro_secretbox_CONTEXTBYTES >= hydro_hash_CONTEXTBYTES);
    COMPILER_ASSERT(hydro_secretbox_KEYBYTES >= hydro_hash_KEYBYTES);
    hydro_hash_hash(probe, hydro_secretbox_PROBEBYTES, mac, hydro_secretbox_MACBYTES, ctx, key);
}

int
hydro_secretbox_probe_verify(const uint8_t probe[hydro_secretbox_PROBEBYTES], const uint8_t *c,
                             size_t c_len, const char ctx[hydro_secretbox_CONTEXTBYTES],
                             const uint8_t key[hydro_secretbox_KEYBYTES])
{
    uint8_t        computed_probe[hydro_secretbox_PROBEBYTES];
    const uint8_t *mac;

    if (c_len < hydro_secretbox_HEADERBYTES) {
        return -1;
    }
    mac = &c[hydro_secretbox_SIVBYTES];
    hydro_hash_hash(computed_probe, hydro_secretbox_PROBEBYTES, mac, hydro_secretbox_MACBYTES, ctx,
                    key);
    if (hydro_equal(computed_probe, probe, hydro_secretbox_PROBEBYTES) == 1) {
        return 0;
    }
    hydro_memzero(computed_probe, hydro_secretbox_PROBEBYTES);
    return -1;
}

int
hydro_secretbox_encrypt(uint8_t *c, const void *m_, size_t mlen, uint64_t msg_id,
                        const char    ctx[hydro_secretbox_CONTEXTBYTES],
                        const uint8_t key[hydro_secretbox_KEYBYTES])
{
    uint8_t iv[hydro_secretbox_IVBYTES];

    hydro_random_buf(iv, sizeof iv);

    return hydro_secretbox_encrypt_iv(c, m_, mlen, msg_id, ctx, key, iv);
}

int
hydro_secretbox_decrypt(void *m_, const uint8_t *c, size_t clen, uint64_t msg_id,
                        const char    ctx[hydro_secretbox_CONTEXTBYTES],
                        const uint8_t key[hydro_secretbox_KEYBYTES])
{
    _hydro_attr_aligned_(16) uint32_t state[gimli_BLOCKBYTES / 4];
    uint32_t                          pub_mac[hydro_secretbox_MACBYTES / 4];
    uint8_t *                         buf = (uint8_t *) (void *) state;
    const uint8_t *                   siv;
    const uint8_t *                   mac;
    const uint8_t *                   ct;
    uint8_t *                         m = (uint8_t *) m_;
    size_t                            mlen;
    uint32_t                          cv;

    if (clen < hydro_secretbox_HEADERBYTES) {
        return -1;
    }
    siv = &c[0];
    mac = &c[hydro_secretbox_SIVBYTES];
    ct  = &c[hydro_secretbox_SIVBYTES + hydro_secretbox_MACBYTES];

    mlen = clen - hydro_secretbox_HEADERBYTES;
    memcpy(pub_mac, mac, sizeof pub_mac);
    COMPILER_ASSERT(hydro_secretbox_SIVBYTES == hydro_secretbox_IVBYTES);
    hydro_secretbox_setup(buf, msg_id, ctx, key, siv, gimli_TAG_KEY);
    hydro_secretbox_xor_dec(buf, m, ct, mlen);

    hydro_secretbox_final(buf, key, gimli_TAG_FINAL);
    COMPILER_ASSERT(hydro_secretbox_MACBYTES <= gimli_CAPACITY);
    COMPILER_ASSERT(gimli_RATE % 4 == 0);
    cv = hydro_mem_ct_cmp_u32(state + gimli_RATE / 4, pub_mac, hydro_secretbox_MACBYTES / 4);
    hydro_mem_ct_zero_u32(state, gimli_BLOCKBYTES / 4);
    if (cv != 0) {
        mem_zero(m, mlen);
        return -1;
    }
    return 0;
}
