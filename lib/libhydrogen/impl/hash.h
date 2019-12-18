int
hydro_hash_update(hydro_hash_state *state, const void *in_, size_t in_len)
{
    const uint8_t *in  = (const uint8_t *) in_;
    uint8_t *      buf = (uint8_t *) (void *) state->state;
    size_t         left;
    size_t         ps;
    size_t         i;

    while (in_len > 0) {
        left = gimli_RATE - state->buf_off;
        if ((ps = in_len) > left) {
            ps = left;
        }
        for (i = 0; i < ps; i++) {
            buf[state->buf_off + i] ^= in[i];
        }
        in += ps;
        in_len -= ps;
        state->buf_off += (uint8_t) ps;
        if (state->buf_off == gimli_RATE) {
            gimli_core_u8(buf, 0);
            state->buf_off = 0;
        }
    }
    return 0;
}

/* pad(str_enc("kmac") || str_enc(context)) || pad(str_enc(k)) ||
   msg || right_enc(msg_len) || 0x00 */

int
hydro_hash_init(hydro_hash_state *state, const char ctx[hydro_hash_CONTEXTBYTES],
                const uint8_t key[hydro_hash_KEYBYTES])
{
    uint8_t block[64] = { 4, 'k', 'm', 'a', 'c', 8 };
    size_t  p;

    COMPILER_ASSERT(hydro_hash_KEYBYTES <= sizeof block - gimli_RATE - 1);
    COMPILER_ASSERT(hydro_hash_CONTEXTBYTES == 8);
    mem_zero(block + 14, sizeof block - 14);
    memcpy(block + 6, ctx, 8);
    if (key != NULL) {
        block[gimli_RATE] = (uint8_t) hydro_hash_KEYBYTES;
        memcpy(block + gimli_RATE + 1, key, hydro_hash_KEYBYTES);
        p = (gimli_RATE + 1 + hydro_hash_KEYBYTES + (gimli_RATE - 1)) & ~(size_t)(gimli_RATE - 1);
    } else {
        block[gimli_RATE] = (uint8_t) 0;
        p                 = (gimli_RATE + 1 + 0 + (gimli_RATE - 1)) & ~(size_t)(gimli_RATE - 1);
    }
    mem_zero(state, sizeof *state);
    hydro_hash_update(state, block, p);

    return 0;
}

/* pad(str_enc("tmac") || str_enc(context)) || pad(str_enc(k)) ||
   pad(right_enc(tweak)) || msg || right_enc(msg_len) || 0x00 */

static int
hydro_hash_init_with_tweak(hydro_hash_state *state, const char ctx[hydro_hash_CONTEXTBYTES],
                           uint64_t tweak, const uint8_t key[hydro_hash_KEYBYTES])
{
    uint8_t block[80] = { 4, 't', 'm', 'a', 'c', 8 };
    size_t  p;

    COMPILER_ASSERT(hydro_hash_KEYBYTES <= sizeof block - 2 * gimli_RATE - 1);
    COMPILER_ASSERT(hydro_hash_CONTEXTBYTES == 8);
    mem_zero(block + 14, sizeof block - 14);
    memcpy(block + 6, ctx, 8);
    if (key != NULL) {
        block[gimli_RATE] = (uint8_t) hydro_hash_KEYBYTES;
        memcpy(block + gimli_RATE + 1, key, hydro_hash_KEYBYTES);
        p = (gimli_RATE + 1 + hydro_hash_KEYBYTES + (gimli_RATE - 1)) & ~(size_t)(gimli_RATE - 1);
    } else {
        block[gimli_RATE] = (uint8_t) 0;
        p                 = (gimli_RATE + 1 + 0 + (gimli_RATE - 1)) & ~(size_t)(gimli_RATE - 1);
    }
    block[p] = (uint8_t) sizeof tweak;
    STORE64_LE(&block[p + 1], tweak);
    p += gimli_RATE;
    mem_zero(state, sizeof *state);
    hydro_hash_update(state, block, p);

    return 0;
}

int
hydro_hash_final(hydro_hash_state *state, uint8_t *out, size_t out_len)
{
    uint8_t  lc[4];
    uint8_t *buf = (uint8_t *) (void *) state->state;
    size_t   i;
    size_t   lc_len;
    size_t   leftover;

    if (out_len < hydro_hash_BYTES_MIN || out_len > hydro_hash_BYTES_MAX) {
        return -1;
    }
    COMPILER_ASSERT(hydro_hash_BYTES_MAX <= 0xffff);
    lc[1]  = (uint8_t) out_len;
    lc[2]  = (uint8_t)(out_len >> 8);
    lc[3]  = 0;
    lc_len = (size_t)(1 + (lc[2] != 0));
    lc[0]  = (uint8_t) lc_len;
    hydro_hash_update(state, lc, 1 + lc_len + 1);
    gimli_pad_u8(buf, state->buf_off, gimli_DOMAIN_XOF);
    for (i = 0; i < out_len / gimli_RATE; i++) {
        gimli_core_u8(buf, 0);
        memcpy(out + i * gimli_RATE, buf, gimli_RATE);
    }
    leftover = out_len % gimli_RATE;
    if (leftover != 0) {
        gimli_core_u8(buf, 0);
        mem_cpy(out + i * gimli_RATE, buf, leftover);
    }
    state->buf_off = gimli_RATE;

    return 0;
}

int
hydro_hash_hash(uint8_t *out, size_t out_len, const void *in_, size_t in_len,
                const char ctx[hydro_hash_CONTEXTBYTES], const uint8_t key[hydro_hash_KEYBYTES])
{
    hydro_hash_state st;
    const uint8_t *  in = (const uint8_t *) in_;

    if (hydro_hash_init(&st, ctx, key) != 0 || hydro_hash_update(&st, in, in_len) != 0 ||
        hydro_hash_final(&st, out, out_len) != 0) {
        return -1;
    }
    return 0;
}

void
hydro_hash_keygen(uint8_t key[hydro_hash_KEYBYTES])
{
    hydro_random_buf(key, hydro_hash_KEYBYTES);
}
